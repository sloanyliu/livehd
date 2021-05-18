//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <array>
#include <cstdint>
#include <string_view>

#include "mmap_map.hpp"
#include "mmap_vector.hpp"

#define append_debug 0



namespace mmap_lib {

template<int map_id>
#if 0
class str {
#else
class __attribute__((packed)) str {
#endif
protected:
  // Keeping the code constexpr for small strings (not long) requires templates (A challenge but reasonable).
  // Some references:
  // https://github.com/tcsullivan/constexpr-to-string
  // https://github.com/vesim987/constexpr_string/blob/master/constexpr_string.hpp
  // https://github.com/vivkin/constexprhash/blob/master/constexprhash.h
  // https://github.com/unterumarmung/fixed_string
  // https://github.com/proxict/constexpr-string
  // https://github.com/tonypilz/ConstexprString
  //
  // 16 bytes data structure:
  // ptr_or_start: 4 bytes
  // e:            10 bytes
  // _size:        2 bytes
  //
  // This avoid having a "size" in the costly str in-place data. The overflow
  // can have a string size like a string_view does
  //
  // The only drawback is that to compute size, it needs to iterate over the e
  // field, but asking size is not a common operation in LiveHD


  uint32_t             ptr_or_start;  // 4 chars if _size < 14, ptr to string_vector otherwise
  std::array<char, 10> e;             // last 10 chars of string, or first 2 + last 8 of string
  uint16_t             _size;         // string length
  constexpr bool       is_digit(char c) const { return c >= '0' && c <= '9'; }
  constexpr uint8_t    posShifter(uint8_t s) const { return s < 4 ? (s - 1) : 3; }
  constexpr uint8_t    posStopper(uint8_t s) const { return s < 4 ? s : 4; }
  constexpr char       isol8(uint32_t ps, uint8_t s) const { return (ps >> (s * 8)) & 0xff; }
  constexpr uint32_t   l8(uint32_t size, uint8_t i) const { return i - (size - 10); }
  constexpr uint32_t   mid(uint32_t ps, uint8_t i) const { return ps + (i - 2); }

  static mmap_lib::vector<int> vec0, vec1, vec2, vec3;
  static mmap_lib::map<std::string_view, uint32_t> map0, map1, map2, map3;
  /* 
  static mmap_lib::vector<int> vec0;
  static mmap_lib::vector<int> vec1;
  static mmap_lib::vector<int> vec2;
  static mmap_lib::vector<int> vec3;

  static mmap_lib::map<std::string_view, uint32_t> map0;
  static mmap_lib::map<std::string_view, uint32_t> map1;
  static mmap_lib::map<std::string_view, uint32_t> map2;
  static mmap_lib::map<std::string_view, uint32_t> map3;
  */


public:
  str() : ptr_or_start(0), e{0}, _size(0) {}        // constructor 0 (empty obj)
  str(char c) : ptr_or_start(0), e{0}, _size(1) {   // constructor 0.5 (single char)
    ptr_or_start <<= 8;
    ptr_or_start |= static_cast<uint8_t>(c);
  }

  // constructor 1 (_size <= 13)
  template <std::size_t N, typename = std::enable_if_t<(N - 1) < 14>>
  constexpr str(const char (&s)[N]) : ptr_or_start(0), e{0}, _size(N - 1) {
    auto stop = posStopper(_size);// if _size < 4, whole string is in ptr_or_start
    // ptr_or_start will hold first 4 chars
    // | first | second | third | forth |
    // 31      23       15      7       0
    for (auto i = 0; i < stop; ++i) {
      ptr_or_start <<= 8;
      ptr_or_start |= s[i];
    }
    auto e_pos = 0u;
    for (auto i = stop; i < N - 1; ++i) {
      e[e_pos] = s[i];
      ++e_pos;
    }
  }

  mmap_lib::map<std::string_view, uint32_t> &map_ref() {
    return map_id==1 ? map1 : map_id==2 ? map2 : map_id==3 ? map3 : map0;
  }
  
  mmap_lib::vector<int> &vec_ref() {
    return map_id==1 ? vec1 : map_id==2 ? vec2 : map_id==3 ? vec3 : vec0;
  }

  mmap_lib::map<std::string_view, uint32_t> &map_cref() const {
    return map_id==1 ? map1 : map_id==2 ? map2 : map_id==3 ? map3 : map0;
  }
  
  mmap_lib::vector<int> &vec_cref() const {
    return map_id==1 ? vec1 : map_id==2 ? vec2 : map_id==3 ? vec3 : vec0;
  }
  
  int get_map_id() const { return map_id; }
  
  static void clear_map() { 
    map_id==1 ? map1.clear() : map_id==2 ? map2.clear() : map_id==3 ? map3.clear() : map0.clear(); 
  }
  
  static void clear_vec() {
    map_id==1 ? vec1.clear() : map_id==2 ? vec2.clear() : map_id==3 ? vec3.clear() : vec0.clear(); 
  }

  //=====helper function to check if a string exists in string_vector=====
  // If the string being searched exists inside the map, then return info about string
  // If the string being searched does not exist, then the function inserts string into map
  std::pair<int, int> insertfind(const char *string_to_check, uint32_t size) {
    std::string_view sv(string_to_check);
    auto it = map_ref().find(sv.substr(0, size));
    if (it == map_ref().end()) {
      //<std::string_view, uint32_t(position in vec)> string_map2
      map_ref().set(sv.substr(0, size), vec_ref().size());
      return std::make_pair(0, -1);
    } else {

      std::pair<int, int> foo;
      foo = std::make_pair(map_ref().get(it), size);
      return foo;
      // pair is (ptr_or_start, size of string)
    }
    return std::make_pair(map_ref().get(it), size);
  }

  //==========constructor 2 (_size >= 14) ==========
  template <std::size_t N, typename = std::enable_if_t<(N - 1) >= 14>, typename = void>
  str(const char (&s)[N]) : ptr_or_start(0), e{0}, _size(N - 1) {
    e[0] = s[0];
    e[1] = s[1];
    for (int i = 0; i < 8; i++) {
      e[9 - i] = s[_size - 1 - i];
    }

    char long_str[_size - 10];
    for (int i = 0; i < (_size - 10); ++i) {
      long_str[i] = s[i + 2];
    }
    // pair is (ptr_or_start, size of string)
    std::pair<int, int> pair = insertfind(long_str, _size - 10);

    if (pair.second != -1) {
      ptr_or_start = pair.first;
    } else {
      for (int i = 0; i < _size - 10; i++) {
        vec_ref().emplace_back(long_str[i]);
      }
      ptr_or_start = vec_ref().size() - (_size - 10);
    }
  }

  //============constructor 3=============
  // const char* and std::string will go through this one
  // implicit conversion from const char* --> string_view
  str(std::string_view sv) : ptr_or_start(0), e{0}, _size(sv.size()) {
    if (_size < 14) {  // constructor 1 logic
      auto stop = posStopper(_size);
      for (auto i = 0; i < stop; ++i) {
        ptr_or_start <<= 8;
        ptr_or_start |= sv.at(i);
      }
      auto e_pos = 0u;
      for (auto i = stop; i < _size; ++i) {
        e[e_pos] = sv.at(i);
        ++e_pos;
      }
    } else {  // constructor 2 logic
      e[0] = sv.at(0);
      e[1] = sv.at(1);
      for (int i = 0; i < 8; i++) {
        e[9 - i] = sv.at(_size - 1 - i);
      }
      char long_str[_size - 10];
      for (int i = 0; i < _size - 10; i++) {
        long_str[i] = sv.at(i + 2);
      }
      std::pair<int, int> pair = insertfind(long_str, _size - 10);
      if (pair.second != -1) {
        ptr_or_start = pair.first;
      } else {
        for (int i = 0; i < _size - 10; i++) {
          vec_ref().emplace_back(long_str[i]);
        }
        ptr_or_start = vec_ref().size() - (_size - 10);
      }
    }
  }

  //=========Printing==============
  void print_PoS() const {
    std::cout << "ptr_or_start is";
    if (_size >= 14) {
      std::cout << "(ptr): " << ptr_or_start << std::endl;
    } else if (_size < 14) {
      std::cout << "(start): ";
      // [first] [sec] [thr] [fourth]
      for (int i = 3; i >= 0; --i) {
        std::cout << char(ptr_or_start >> (i * 8));
      }
      std::cout << std::endl;
    }
  }

  void print_e() const {
    std::cout << "e is: [ ";
    for (auto i = 0u; i < e.size(); ++i) {
      std::cout << e[i] << " ";
    }
    std::cout << "]" << std::endl;
  }

  void print_StrVec() const {
    std::cout << "StrVec{ ";
    for (auto i = vec_cref().begin(); i != vec_cref().end(); ++i) {
      std::cout << static_cast<char>(*i) << " ";
    }
    std::cout << "}" << std::endl;
  }

  void print_StrMap() const {
    std::cout << "StrMap{ ";
    for (auto it = map_cref().begin(), end = map_cref().end(); it != end; ++it) {
      std::string key   = std::string(map_cref().get_key(it));
      uint32_t    value = map_cref().get(it);
      std::cout << "<" << key << ", " << value << "> ";
    }
    std::cout << "}" << std::endl;
  }

  void print_string() const {
    if (_size <= 13) {
      uint8_t mx = posShifter(_size);
      for (uint8_t i = mx; i <= 3u; --i) {
        std::cout << isol8(ptr_or_start, i);
      }
      if (_size > 4) {
        for (uint8_t j = 0; j < e.size(); ++j) {
          std::cout << static_cast<char>(e[j]);
        }
      }
    } else {
      std::cout << char(e[0]) << char(e[1]);
      for (auto i = 0; i < _size - 10; ++i) {
        std::cout << static_cast<char>(vec_cref()[i + ptr_or_start]);
      }
      for (uint8_t k = 2; k < 10; ++k) {
        std::cout << static_cast<char>(e[k]);
      }
    }
  }

  //=================================

  [[nodiscard]] constexpr std::size_t size() const { return _size; }
  [[nodiscard]] constexpr std::size_t length() const { return _size; }
  [[nodiscard]] constexpr std::size_t max_size() const { return 65535; }
  [[nodiscard]] constexpr bool        empty() const { return 0 == _size; }

  template <std::size_t N>
  bool operator==(const char (&rhs)[N]) const {
    return (*this == str<map_id>(rhs));
  }

  // const char* and std::string will go through this one
  // implicit conversion from const char* --> string_view
  bool operator==(std::string_view rhs) const {
    return (*this == str<map_id>(rhs));
  }

  constexpr bool operator==(const str<map_id> &rhs) const {
    if (get_map_id() == rhs.get_map_id() || (_size < 14 && rhs._size < 14)) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
      const uint64_t *a = (const uint64_t *)(this);
      const uint64_t *b = (const uint64_t *)(&rhs);
#pragma GCC diagnostic pop
      return a[0] == b[0] && a[1] == b[1]; // 16byte compare
    } else {
      //TODO: compare sviews
      std::string_view yala = map_cref().get_sview(ptr_or_start);
      std::string_view yalb = rhs.map_cref().get_sview(ptr_or_start);
      return false;
    }    
  }

  constexpr bool operator!=(const str &rhs) const { return !(*this == rhs); }

  template <std::size_t N>
  constexpr bool operator!=(const char (&rhs)[N]) const {
    return !(*this == rhs);
  }

  bool operator!=(std::string_view rhs) const { return !(*this == rhs); }

  constexpr char operator[](std::size_t pos) const {
    if (pos >= _size) {
      throw std::out_of_range("[] operator out of range.");
    }
    if (_size < 14) {
      if (pos < 4) {
        uint8_t mx = posShifter(_size);
        mx         = mx - pos;
        return isol8(ptr_or_start, mx);
      } else {
        return e[pos - 4];
      }
    } else {
      if (pos < 2) {
        return e[pos];
      } else if (pos >= static_cast<size_t>(_size - 8)) {
        return e[l8(_size, pos)];
      } else {
        return vec_cref()[mid(ptr_or_start, pos)];
      }
    }
  }

  bool starts_with(str &st) const {
    if (st.size() > _size) {
      return false;
    }
    else if (st.size() == _size) {
      return *this == st;
    } else if (st.size() == 0) {
      return true;
    } else {  // if (st._size < *this._size), compare
      for (auto i = 0u; i < st.size(); ++i) {
        if ((*this)[i] != st[i]) {
          return false;
        }
      }
      return true;
    }
  }

  // const char * and std::string will come through here
  bool starts_with(std::string_view st) const {
    if (st.size() > _size) {
      return false;
    } else if (st.size() == _size) {
      return *this == st;
    } else if (st.size() == 0) {
      return true;
    }
    for (auto i = 0u; i < st.size(); ++i) {
      if ((*this)[i] != st[i]) {
        return false;
      }
    }
    return true;
  }

  // checks if *this pstr ends with en
  bool ends_with(const str &en) const {
    if (en.size() > _size) {
      return false;
    } else if (en.size() == _size) {
      return *this == en;
    } else if (en.size() == 0) {
      return true;
    }
    for (uint32_t j = _size - en.size(), i = 0; j < _size; ++j, ++i) {
      if ((*this)[j] != en[i]) {
        return false;
      }
    }
    return true;
  }

  bool ends_with(std::string_view en) const {
    if (en.size() > _size) {
      return false;
    } else if (en.size() == _size) {
      return *this == en;
    } else if (en.size() == 0) {
      return true;
    }
    // Actual compare logic
    for (uint32_t j = _size - en.size(), i = 0; j < _size; ++j, ++i) {
      if ((*this)[j] != en[i]) {
        return false;
      }
    }
    return true;
  }

  // will use the string_view function
  bool ends_with(std::string en) const { return ends_with(std::string_view(en.c_str())); }

  std::size_t find(const str &v, std::size_t pos = 0) const {
    char first = v[0];
    for (size_t i = ((pos == 0) ? 0 : pos); i < _size; i++) {
      if ((first == (*this)[i]) and ((i + v._size) <= _size)) {
        for (size_t j = i, k = 0; j < i + v._size; j++, k++) {
          if ((*this)[j] != v[k])
            break;
          if (j == (i + v._size - 1))
            return i;
        }
      }
    }
    return -1;
  }

  std::size_t find(char c, std::size_t pos = 0) const {
    if (pos >= _size)
      return -1;
    for (size_t i = pos; i < _size; i++) {
      if ((*this)[i] == c)
        return i;
    }
    return -1;
  }

  template <std::size_t N>
  constexpr std::size_t find(const char (&s)[N], std::size_t pos = 0) {
    return find(str<map_id>(s), pos);
  }

  std::size_t rfind(const str &v, std::size_t pos = 0) const {
    int position = _size - 1;
    if (pos != 0)
      position = pos;
    char   first    = v[0];
    size_t retvalue = -1;
    for (int i = (_size - v._size); i >= 0; i--) {
      if ((first == (*this)[i]) and ((i + v._size) <= _size)) {
        if (v._size == 1)
          return i;
        size_t k = 0;
        for (int j = i; k < v._size && j < i + v._size; j++, k++) {  // FIXME: is hist k<v && j<i ??
          if ((*this)[j] != v[k])
            break;
          if ((j == (i + v._size - 1)) and (i <= position))
            return i;
        }
      }
    }
    return retvalue;
  }

  std::size_t rfind(char c, std::size_t pos = 0) const {
    int position = _size - 1;
    if (pos != 0)
      position = pos;
    for (int i = position; i >= 0; i--) {
      if ((*this)[i] == c)
        return i;
    }
    return -1;
  }

  template <std::size_t N>
  constexpr std::size_t rfind(const char (&s)[N], std::size_t pos = 0) {
    return rfind(str<map_id>(s), pos);
  }

  static str concat(str &a, const str &b) { return a.append(b); }

  static str concat(std::string_view a, const str &b) {
    mmap_lib::str<map_id> temp(a);
    return temp.append(b);
  }

  static str concat(str &a, std::string_view b) { return a.append(b); }

  static str concat(str &a, int v) { return a.append(v); }

  str append(const str &b) {
    if (_size <= 13) {
      if ((_size + b._size) <= 13) {  // size and b size < = 13
        if (_size <= 3) {
          long unsigned int i     = 0;
          uint8_t           e_ptr = _size <= 4 ? 0 : _size - 4;
          for (; i < b._size; ++i) {
            if (_size + i < 4) {
              ptr_or_start = (ptr_or_start << 8) | static_cast<uint8_t>(b[i]);
            } else {
              e[e_ptr++] = b[i];
            }
          }
        } else {
          for (auto i = _size - 4, j = 0; i < 10; ++i, ++j) {
            if (j >= b._size)
              break;
            e[i] = b[j];
          }
        }
      } else {  // size and b size > 13
        char full[_size + b._size - 10];
        uint8_t indx = 2, b_indx = 0, e_indx = 2;
        if (b._size > 8) {
          auto i = 0;
          for (; i < _size + b._size - 10; ++i) {
            if (indx <= _size - 1) {
              full[i] = (*this)[indx++];
            } else {
              full[i] = b[b_indx++];
            }
          }
          for (; i < _size + b._size - 2; ++i) {
            e[e_indx++] = b[b_indx++];
          }
          e[0] = (*this)[0];
          e[1] = (*this)[1];
        } else if (b._size < 8) {
          auto i = 0;
          for (; i < _size + b._size - 10; ++i) {
            full[i] = (*this)[indx++];
          }
          for (; i < _size + b._size - 2; ++i) {
            if (indx <= _size - 1) {
              e[e_indx++] = (*this)[indx++];
            } else {
              e[e_indx++] = b[b_indx++];
            }
          }
          e[0] = (*this)[0];
          e[1] = (*this)[1];
        } else if (b._size == 8) {
          auto i = 0;
          for (; i < _size + b._size - 10; ++i) {
            full[i] = (*this)[indx++];
          }
          for (; i < _size + b._size - 2; ++i) {
            e[e_indx++] = b[b_indx++];
          }
          e[0] = (*this)[0];
          e[1] = (*this)[1];
        }
        std::pair<int, int> ret = insertfind(full, _size + b._size - 10);
        if (ret.second != -1) {
          ptr_or_start = ret.first;
        } else {
          for (int i = 0; i < _size + b._size - 10; i++) {
            vec_ref().emplace_back(full[i]);
          }
          ptr_or_start = vec_ref().size() - (_size + b._size - 10);
        }
      }
    } else {
      if ((ptr_or_start + (_size-10)) == vec_ref().size()) {  // last one inserted
        for (auto k = 0; k < b._size; ++k) {
          if (b._size >= 8) {
            for (auto i = 2; i < 10; ++i) {
              vec_ref().emplace_back(e[i]);
            }
            for (auto i = 0; i < b._size - 8; ++i) {
              vec_ref().emplace_back(b[i]);
            }
            for (auto i = b._size - 8, j = 2; i < b._size; ++i, ++j) {
              if (j == 10)
                break;
              e[j] = b[i];
            }
          } else {  // b._size < 8
            for (auto i = 2; i < b._size + 2; ++i) {
              vec_ref().emplace_back(e[i]);
            }
            auto i = 2;
            for (; i < b._size + 2; ++i) {
              e[i] = e[i + b._size];
            }  // overwrite e
            for (auto j = 0; j < b._size; ++i, ++j) {
              if (i == 10)
                break;
              e[i] = b[j];
            }
          }
        }
        std::string full_str = this->to_s();  // n
        full_str += b.to_s();                 // m
        insertfind((full_str.substr(2,_size + b._size -10)).data(), _size + b._size - 10);
      } else {
        std::string start = this->to_s();  // n
        start += b.to_s();  // m
        str<map_id> temp_str = str<map_id>(start);  // n + m
        ptr_or_start = temp_str.ptr_or_start;
        for (uint8_t i = 0; i < static_cast<uint8_t>((temp_str.e).size()); i++) {
          e[i] = temp_str.e[i];
        }
      }
    }
    _size += b._size;
    return *this;
  }

  str append(std::string_view b) { return append(mmap_lib::str<map_id>(b)); }

  str append(char c) {
    mmap_lib::str<map_id> h(c);
    return append(h);
  }

  str append(int b) {
    std::string hold = std::to_string(b);
    return append(mmap_lib::str<map_id>(hold));
  }


  // used as a tokenizing function
  std::vector<str> split(const char chr) {
    std::vector<str> vec;
    std::string      hold;

    if (_size == 0) {
      return vec;
    }
    uint8_t track = 0;
    for (auto i = 0; i < _size; ++i) {
      if ((*this)[i] == chr) {
        vec.push_back(mmap_lib::str<map_id>(hold));
        hold.clear();
        track = 0;
      } else {
        hold += (*this)[i];
        ++track;
      }
    }
    if (hold.size() != 0) {
      vec.push_back(mmap_lib::str<map_id>(hold));
    }
    return vec;
  }

  bool is_i() const {
    if (_size < 14) {
      int  temp  = (_size >= 4) ? 3 : (_size - 1);
      char first = (ptr_or_start >> (8 * (temp))) & 0xFF;
      if (first != '-' and (first < '0' or first > '9')) {
        return false;
      }
      for (int i = 1; i < (_size > 4 ? 4 : _size); i++) {
        switch ((ptr_or_start >> (8 * (temp - i))) & 0xFF) {
          case '0' ... '9': break;
          default: return false; break;
        }
      }
      for (int i = 0; i < (_size > 4 ? _size - 4 : 0); i++) {
        switch (e[i]) {
          case '0' ... '9': break;
          default: return false; break;
        }
      }
    } else {
      char first = e[0];
      if (first != '-' and (first < '0' or first > '9')) {
        return false;
      }
      for (int i = 1; i < 10; i++) {
        switch (e[i]) {
          case '0' ... '9': break;
          default: return false; break;
        }
      }
      for (int i = ptr_or_start; i < _size - 10; i++) {
        switch (static_cast<int>(vec_cref()[i])) {
          case '0' ... '9': break;
          default: return false; break;
        }
      }
    }
    return true;
  }
  
  int64_t to_i() const {  // convert to integer
    if (is_i()) {
      std::string temp = to_s();
      return stoi(temp);
    } else {
      return 0;
    }
  }

  std::string to_s() const {  // convert to string
    std::string out;
    for (auto i = 0; i < _size; ++i) {
      out += (*this)[i];
    }
    return out;
  }


  str get_str_after_last(const char chr) const {
    size_t      val = rfind(chr);
    std::string out;
    if (_size == 0 || val >= static_cast<size_t>(_size - 1)) {
      return str<map_id>();
    }
    for (size_t i = val + 1; i < _size; i++) {
      out += (*this)[i];
    }
    return str<map_id>(out);
  }

  str get_str_after_first(const char chr) const {
    size_t      val = find(chr);
    std::string out;
    if (_size == 0 || val >= static_cast<size_t>(_size - 1)) {
      return str<map_id>();
    }
    for (size_t i = val + 1; i < _size; i++) {
      out += (*this)[i];
    }
    return str<map_id>(out);
  }

  str get_str_before_last(const char chr) const {
    size_t      val = rfind(chr);
    std::string out;
    if (_size == 0 || val >= static_cast<size_t>(_size)) {
      return str<map_id>();
    }
    for (size_t i = 0; i < val; i++) {
      out += (*this)[i];
    }
    return str<map_id>(out);
  }

  str get_str_before_first(const char chr) const {
    size_t      val = find(chr);
    std::string out;
    if (_size == 0 || val >= static_cast<size_t>(_size)) {
      return str<map_id>();
    }
    for (size_t i = 0; i < val; i++) {
      out += (*this)[i];
    }
    return str<map_id>(out);
  }

  str substr(size_t start) const { return substr(start, _size - start); }

  str substr(size_t start, size_t end) const {
    if (_size == 0 || start > static_cast<size_t>(_size - 1)) {
      return mmap_lib::str<map_id>();
    }
    std::string hold;
    size_t      adj_end = (end > (_size - start)) ? (_size - start) : end;  // adjusting end for overflow
    for (auto i = start; i < (start + adj_end); ++i) {
      hold += (*this)[i];
    }
    return mmap_lib::str<map_id>(hold);
  }

};

}  // namespace mmap_lib

