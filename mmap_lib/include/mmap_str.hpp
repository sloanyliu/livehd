//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <array>
#include <cstdint>
#include <string_view>

#include "mmap_map.hpp"

#define posShifter(s) s<4 ? (s-1):3
#define posStopper(s) s<4 ? s:4
#define isol8(pos, s) (pos >> (s*8)) & 0xff
#define l8(size, i) i - (size - 10) 

namespace mmap_lib {

class str {
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
  //
  // ptr_or_start:
  // 10 "special" character (allow to encode 2 digits as 1 character when (c&0x80) is true)
  // _size is the str _size (original not compressed)
  //
  // NOTE: Maybe it is faster/easier to have this instead:
  //
  // ptr_or_start
  // 12 special characters
  // if e[11]&0x80 || e[12]==0 -> overflow (ptr not start)
  // end of string is first zero (or last e[11]&0x80)
  //
  // This avoid having a "size" in the costly str in-place data. The overflow
  // can have a string size like a string_view does
  //
  // The only drawback is that to compute size, it needs to iterate over the e
  // field, but asking size is not a common operation in LiveHD

  uint32_t ptr_or_start;  // 4 chars if _size < 14, is a ptr to string_map2 otherwise
  std::array<char, 10> e; // last 10 "special <128" characters ending the string
  uint16_t _size;          // 2 bytes
  constexpr bool is_digit(char c) const { return c >= '0' && c <= '9'; }

public:
  //string_map2 <sv of long_str (key), and position in str_vec (val)> 
  static mmap_lib::map<std::string_view, uint32_t> string_map2;
  
  // this holds all the raw data, (int kinda weird?)
  inline static std::vector<int> string_vector; // ptr_or_start points here! 
  
  //===========constructor 1============
  template<std::size_t N, typename = std::enable_if_t<(N - 1) < 14>>
  constexpr str(const char(&s)[N]): ptr_or_start(0), e{0}, _size(N-1) {
    // if _size is less than 4, then whole thing will be in ptr_or_start
    auto stop = posStopper(_size);
    // ptr_or_start will hold first 4 chars
    // | first | second | third | forth |
    // 31      23       15      7       0 
    for(auto i = 0; i < stop; ++i) {
      ptr_or_start <<= 8;
      ptr_or_start |= s[i];
    }
    auto e_pos = 0u; // e indx starts at 0
    // stores rest in e, starting from stop to end of string
    for(auto i = stop; i < N - 1; ++i) {
      e[e_pos] = s[i];
      ++e_pos;
    }
  }

  //=====helper function to check if a string exists in string_vector=====
  std::pair<int, int> insertfind(const char *string_to_check, uint32_t size) { 
    std::string_view sv(string_to_check); // string to sv
    // using substr here to take out all the weird things that come with sview
    auto it = string_map2.find(sv.substr(0, size)); // find the sv in the string_map2
    if (it == string_map2.end()) {          // if we can't find the sv
      //<std::string_view, uint32_t(position in vec)> string_map2
      string_map2.set(sv.substr(0, size), string_vector.size());  // insert it
      return std::make_pair(0,0);
    } else {
      return std::make_pair(string_map2.get(it), size); //found it, return
      // pair is (ptr_or_start, size of string)
    }

#if 0
    bool vector_flag = true;
    
    // the line below should be constant time
    
    for (auto i = string_map.begin(), end = string_map.end(); i != end; ++i) {
      uint32_t key   = string_map.get_key(i);
      uint16_t value = string_map.get(i);
      if (value != size)
        continue;i
      for (int i = 0; i < size; i++) {
        if (string_vector.at(key + i) != string_to_check[i]) {
          vector_flag = false;
          break;
        }
        if (vector_flag)
          return std::make_pair(key, value);
        vector_flag = true;
      }
    }
    return std::make_pair(0/*pos in vector*/, 0/*size of str*/);
#endif
  }

  //==========constructor 2==========
  template<std::size_t N, typename = std::enable_if_t<(N-1)>=14>, typename=void>
  str(const char (&s)[N]) : ptr_or_start(0), e{0}, _size(N - 1) {
    // the first two characters saved in e
    e[0] = s[0]; e[1] = s[1];
    // the last eight characters saved in e
    for (int i = 0; i < 8; i++) { e[9 - i] = s[_size - 1 - i]; }
    
    char long_str[_size-10]; // holds the long part     
    // filling long_str with long part of string
    for (int i = 0; i < (_size - 10); ++i) { long_str[i] = s[i + 2]; } 
    // checking if long part of string already exists in vector
    std::pair<int, int> pair = insertfind(long_str, _size - 10);

    // if string exists in vector, get the ptr to it in string_map2
    // pair is (ptr_or_start, size of string)
    if (pair.second) {
      ptr_or_start = pair.first;
    } else { // if string is not invector, put it in vector
      for (int i = 0; i < _size - 10; i++) { 
        string_vector.push_back(long_str[i]); 
      }
      ptr_or_start = string_vector.size() - (_size - 10);
    }
  }
  
  //============constructor 3=============
  // const char * will go through this one
  // implicit conversion from const char* --> string_view
  //
  // std::string will also go through this one
  str(std::string_view sv) : ptr_or_start(0), e{0}, _size(sv.size()) {
    if (_size < 14 ){ // constructor 1 logic
		  auto stop = posStopper(_size);
	    for(auto i=0;i<stop;++i) {
	      ptr_or_start <<= 8;
	      ptr_or_start |= sv.at(i);
	    }
	    auto e_pos   = 0u;
	    for(auto i=stop;i<_size;++i) {
	       e[e_pos] = sv.at(i);
	       ++e_pos;
	  	}
  	} else { // constructor 2 logic
  		e[0] = sv.at(0);
	    e[1] = sv.at(1);
	    // the last eight  characters
	    for (int i = 0; i < 8; i++) {
	      e[9-i] = sv.at(_size -1- i); //there is no null terminator in sv
	    }
	    // checking if it exists
	    char long_str[_size-10];
	    for (int i = 0; i < _size - 10; i++) {
	      long_str[i] = sv.at(i + 2);
	    }
	    std::pair<int, int> pair = insertfind(long_str, _size - 10);
	    if (pair.second) {
	      ptr_or_start = pair.first;
	    } else {
	      for (int i = 0; i < _size - 10; i++) {
	        string_vector.push_back(long_str[i]);
	      }
        ptr_or_start = string_vector.size() - (_size-10);
	    }
  	}
  }


  //=========Printing==============
  void print_PoS () const { 
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

  void print_e () const {
    std::cout << "e is: [ ";
    for (int i = 0; i < e.size(); ++i) { std::cout << e[i] << " "; }
    std::cout << "]" << std::endl;
  }

  void print_StrVec () const {
    std::cout << "StrVec{ ";
    for (std::vector<int>::const_iterator i = string_vector.begin(); i != string_vector.end(); ++i) 
    {
      std::cout << char(*i) << " ";
    }
    std::cout << "}" << std::endl;
  }

  void print_StrMap () const {
    std::cout << "StrMap{ ";
    for (auto it = string_map2.begin(), end = string_map2.end(); it != end; ++it) {
      std::string key = std::string(string_map2.get_key(it));
      uint32_t value = string_map2.get(it);
      std::cout << "<" << key << ", " << value << "> ";
    }
    std::cout << "}" << std::endl;
  }
  
  void print_string() {
    if (_size <= 13) {
      uint8_t mx = posShifter(_size);
      for (uint8_t i = mx; i >= 0, i <= 3; --i) {
        std::cout << static_cast<char>((ptr_or_start >> (i*8)) & 0xff);
      }
      if (_size > 4) {
        for (uint8_t j = 0; j < e.size(); ++j) {
          std::cout << static_cast<char>(e[j]);
        }
      }
    } else {
      std::cout << char(e[0]) << char(e[1]);
      for (auto i = 0; i < _size - 10; ++i) {
        std::cout << static_cast<char>(string_vector.at(i + ptr_or_start));
      }
      for (uint8_t k = 2; k < 10; ++k) {
        std::cout << static_cast<char>(e[k]);
      }
    }
  }

  //=================================

#if 0
  fixme_const_iterator begin()  const {
    for(const auto &ch:data.b) {
      if (ch!=0) { return &ch; }
    }
    if (size<16) { return &e[0]; }
    return ptr;
  }
  fixme_const_iterator end()    const {
    if (size<16)
      return &e[size-4];

    for(const auto &ch:e) {
      if (ch==0)
        return &ch;
    }
    return e.end();}
#endif

  [[nodiscard]] constexpr std::size_t size() const { return _size; }
  [[nodiscard]] constexpr std::size_t length() const { return _size; }
  [[nodiscard]] constexpr std::size_t max_size() const { return 65535; }
  [[nodiscard]] constexpr bool empty() const { return 0 == _size; }

  template <std::size_t N>
  constexpr bool operator==(const char (&rhs)[N]) const {       
    auto rhs_size = N - 1;
    if (_size != rhs_size) { return false; } // if size doesnt match, false
    if (_size < 14) { return str(rhs) == *this; }
    else if (_size >= 14) { // string_vector ptr in ptr_or_start, chars in e
      if (e[0] != rhs[0] || e[1] != rhs[1]) { return false; } // check first two
      uint8_t idx = 8;
      for (auto i = 2; i < 10; ++i) { 
        if (e[i] != rhs[rhs_size - idx--]) { return false; } // check last eight
      }
      
      // Getting data from string_vector and comparing with rest of rhs 
      auto j = 2; // rhs[2 .. _size - 8] --> the long part
      // for loop range: (ptr_or_start) .. (ptr_or_start + _size-10) 
      for (auto i = ptr_or_start; i < (ptr_or_start + _size - 10); ++i) {   
        if (string_vector.at(i) != rhs[j]) { return false; }
        j = j < _size-8 ? j+1 : j;
      }
      return true;
    }
    return false;
  }

  // const char * will go through this one
  // implicit conversion from const char* --> string_view
  //
  // std::string will also go through this one
  constexpr bool operator==(std::string_view rhs) const {       
    auto rhs_size = rhs.size();
    if (_size != rhs_size) { return false; } // if size doesnt match, false
    if (_size < 14) { return str(rhs) == *this; }
    else if (_size >= 14) { // string_vector ptr in ptr_or_start, chars in e
      if (e[0] != rhs.at(0) || e[1] != rhs.at(1)) { return false; } // chk first two
      uint8_t idx = 8;
      for (auto i = 2; i < 10; ++i) { 
        if (e[i] != rhs.at(rhs_size - idx--)) { return false; } // chk last eight
      }
      // return if rhs w/out first two and last eight is in string_map2  
      return !(string_map2.find(rhs.substr(2, rhs_size-10)) == string_map2.end());
    }
    return false;
  }

  constexpr bool operator==(const str &rhs) const {
    //1) compare size
    //1) compare e
    //2) compare ptr_or_start
    if (_size == 0 && rhs._size == 0) { return true; }
    if (_size != rhs._size) { return false; }
    for (auto i = 0; i < e.size(); ++i) {
      if (e[i] != rhs.e[i]) { return false; }
    }
    return (ptr_or_start == rhs.ptr_or_start);
  }

  constexpr bool operator!=(const str &rhs) const { return !(*this == rhs); }

  template <std::size_t N>
  constexpr bool operator!=(const char (&rhs)[N]) const { return !(*this == rhs); }
 
  constexpr bool operator!=(std::string_view rhs) const { return !(*this == rhs); }

  constexpr char operator[](std::size_t pos) const {
  #ifndef NDEBUG
    if (pos >= _size)
      throw std::out_of_range("");
  #endif
    if (_size < 14) {
      if (pos < 4){
        if(_size == 1) return (ptr_or_start >> (8 * (0 - pos))) & 0xFF;
        if(_size == 2) return (ptr_or_start >> (8 * (1 - pos))) & 0xFF;
        if(_size == 3) return (ptr_or_start >> (8 * (2 - pos))) & 0xFF;
        return (ptr_or_start >> (8 * (3 - pos))) & 0xFF;
      }else{
        return e[pos - 4];
      }
    } else {
      if(pos <2){
        return e[pos];
      } else if (pos >= (_size-8)) {
        return e[10 -( _size - pos)];
      } else{
        return string_vector.at(ptr_or_start+pos-2);
      }
    }
  }

  // checks if *this pstr starts with st
  // Thought...
  // Can use substr function of sview in cases where both st and *this are LONG,
  //    and just compare sviews
  bool starts_with(str &st) const { 
    if (st.size() > _size) { return false; }// st.size > *this.size, false
    else if (st.size() == _size) { return *this == st; }
    else if (st.size() == 0) { return true; }
    else { // if (st._size < *this._size), compare   
      uint8_t mx = posShifter(_size);
      uint8_t mx_st = posShifter(st.size());
      if (_size <= 13) { //==== case 1: if *this is SHORT, st is SHORT
        for (auto i = 0; i < st.size(); ++i) { // iterate based on st
          if (i < 4) { // for *this and st, first 4 will be in p_o_s
            if (((st.ptr_or_start >> (mx_st*8)) & 0xff) != ((ptr_or_start >> (mx*8)) & 0xff)) {
              return false;
            } else { --mx_st; --mx; }
          } else { // rest of string will be in e
            if (e[i-4] != st.e[i-4]) { return false; }
          }
        }
        return true; 
      } else if (_size > 13) { //==== case 2: if *this is LONG, st can be LONG or SHORT
        uint32_t v_ptr = ptr_or_start;
        uint8_t e_ptr = 0;
        if (st._size <= 13) { //==== case 2a: *this is LONG, st is SHORT
          for (auto i = 0; i < st.size(); ++i) { //i refers to st index
            // i = 0, 1
            // for *this, data will be in e -> index with i
            // for st   , data will be in p_o_s -> shift 
            if (i < 2) { // i = 0, 1 : *this.e is used, st.ptr_or_start is used
              if (e[e_ptr] != ((st.ptr_or_start >> (mx_st*8)) & 0xff)) { 
                return false; 
              } else { --mx_st; ++e_ptr; }
            // i = 2, 3
            // for *this, data will be in vec only -> index with v_ptr
            // for st   , data will be in p_o_s -> shift
            } else if ((i >= 2) && (i < 4)) {
              if (string_vector.at(v_ptr) != ((st.ptr_or_start >> (mx_st*8)) & 0xff)) {
                return false;
              } else { --mx_st; ++v_ptr; }
            // i = 4 ... 12 (max)
            // for *this, data will be in vec/e depend on length
            // for st   , data will be in e -> index with i-4
            } else {
              // if we're done using string_vector, we use last 8 of e of *this
              if ((v_ptr - ptr_or_start) >= (_size - 10)) {
                if (e[e_ptr] != st.e[i - 4]) { return false; } 
                else { ++e_ptr; }
              } else { // use e for st and use vector for *this
                if (string_vector.at(v_ptr) != (st.e[i - 4])) { return false; } 
                else { ++v_ptr; }
              }
            }
          }
          return true; // made it out of the for loop means no mismatch 
        } else if (st.size() > 13) { //===== case 2b: *this is LONG, st is LONG
          uint8_t e_ptr = 2, ste_ptr = 2; // used to iterate through last 8 of e
          uint32_t v_ptr = ptr_or_start, stv_ptr = st.ptr_or_start;
          // start with first two of e for both, then move to vector, then last 8 of e
          // NOTE: be careful when *this is still in vec, and st runs out of vec
          for (auto i = 0; i < st.size(); ++i) { // using st._size to iterate 
            // i = 0, 1
            // for both, in e
            if (i < 2) {
              if (e[i] != st.e[i]) { return false; }
            // i = 2 .. start of last 8
            // for both, in vec, BUT, st will ALWAYS reach e before *this
            } else if ((i >= 2) && (i < (st.size() - 8))) {
              if (string_vector.at(v_ptr) != string_vector.at(stv_ptr)) {
                return false;
              } else { ++v_ptr; ++stv_ptr; }
            // i = last 8 of st
            // *this can still be in vec, st always n e here
            } else {
              // if *this has reached last 8, we use e for *this
              if ((v_ptr - ptr_or_start) >= (_size - 10)) {
                if (e[e_ptr] != st.e[ste_ptr]) { return false; } 
                else { ++e_ptr; ++ste_ptr; }
              } else { // use vec for *this, e for st
                if (st.e[ste_ptr] != string_vector.at(v_ptr)) { return false; } 
                else { ++ste_ptr; ++v_ptr; }
              }
            }
          }
          return true;
        }
      }
      return false;
    }
  }

  // const char * and std::string will come thru here
  bool starts_with(std::string_view st) const { 
    if (st.size() > _size) { return false; }
    else if (st.size() == _size) { return *this == st; }
    else if (st.size() == 0) { return true; }
    else if (st.size() < _size) {
      // Actual compare logic
      auto fndsize = 0;
      if (_size <= 13) {
        uint8_t mx = posShifter(_size);
        for (auto i = mx; i >= 0, i <= 3; --i) {
          if (((ptr_or_start >> (i*8)) & 0xff) != st[fndsize++] ) {
            return false; 
          }
          if (fndsize == st.size()) { return true; }
        }
        for (uint8_t j = 0; j < e.size(); ++j) {
          if (e[j] != st[fndsize++]) { return false; }
          if (fndsize == st.size()) { return true; }
        }
      } else {
        // compare first two of e
        // then all the way up till _size-10
        for (auto i = 0; i < 2; ++i) {
          if (e[i] != st[i]) { return false; } 
          else { 
            ++fndsize;
            if (fndsize == st.size()) { return true; }
          }
        }
        for (auto i = 0; i < _size-10; ++i) {
          if (string_vector.at(ptr_or_start + i) != st[fndsize++]) {
            return false;
          }
          if (fndsize == st.size()) { return true; }
        }
        for (uint8_t i = 2; i < 10; ++i) {
          if (e[i] != st[fndsize++]) { return false; }
          if (fndsize == st.size()) { return true; }
        }
      }
      return false;
    }
  }

  // checks if *this pstr ends with en
  bool ends_with(const str &en) const {
    if (en.size() > _size) { printf("1\n"); return false; }
    else if (en.size() == _size) { return *this == en; }
    else if (en.size() == 0) { return true; }
    else if (en.size() < _size) {
      if (_size <= 13) {// if *this is SHORT

        uint8_t mx = posShifter(_size);
        mx = mx - (_size - en.size());
        uint8_t mx_st = posShifter(en.size());
        printf("posShift(%d) = %d for *this.\n", _size, posShifter(_size));        
        printf("posShift(%d) = %d for en.\n", en.size(), posShifter(en.size()));        
        printf("diff is: %d, mx is: %d, mx_st is: %d\n", _size-en.size(), mx, mx_st);
        for (long unsigned int i = 0, j = _size-en.size(); i < en.size(); ++i, ++j) {
          // -> *this and en are in ptr_or_start 
          // FIXME: things are weird here with the shifting,
          // print and check
          if ((i <= 3) && (mx_st <= 3) && (mx_st >= 0)) { // en needs to shift
            if (mx <= 3 && mx >= 0) { // *this needs to shift
              uint8_t one = isol8(ptr_or_start, mx);
              uint8_t two = isol8(en.ptr_or_start, mx_st);
              if (one != two) {
                printf("2\n"); 
                printf("*this: %d\n", isol8(ptr_or_start, mx)); 
                printf("en: %d\n", isol8(en.ptr_or_start, mx_st)); 
                return false;
              } else {
                --mx_st; --mx;
              }
            } else { // *this does not need to shift anymore
              if (e[j-4] != static_cast<char>(isol8(en.ptr_or_start, mx_st))) {
                printf("3\n"); return false;
              } else {
                --mx_st;
              }
            }
          } else { // en goes to e
            if (e[j-4] != en.e[i-4]) {
              printf("4\n"); return false;
            }
          } 
        }
        return true;
      } else if (_size > 13) { // if *this is LONG
        if (en.size() > 13) { // -> en is LONG
          for (long unsigned int i = 0, j = _size-en.size(); i < en.size(); ++i, ++j) {
            if (i < 2) { // en in e[0,1]
              if (j < 2) { // *this is in e[]
                if (e[j] != en.e[i]) {
                  printf("5\n"); return false;
                }
              } else if ((j >= 2) && (j < (_size - 8))) { // *this is in vector now
                if (string_vector.at(ptr_or_start + (j-2)) != en.e[i]) {
                  printf("6\n"); return false;
                }
              } 
            } else if ((i >= 2) && (i < (en.size() - 8))) { // en in vec
              if ((j >= 2) && (j < (_size - 8))) { // *this is in vector
                if (string_vector.at(ptr_or_start + (j-2)) != string_vector.at(en.ptr_or_start + (i-2))) {
                  printf("7\n"); return false;
                }
              }
            } else { // last 8 for both
              if (l8(_size,j) <= 9 && l8(en.size(),i) <= 9 && l8(_size,j) == l8(en.size(),i)) {
                if (e[l8(_size, j)] != en.e[l8(en.size(), i)]) {
                  printf("8\n"); return false;
                }
              }
            }
          }
          return true;
        } else if (en.size() <= 13) { // -> en is SHORT
          uint8_t mx_st = posShifter(en.size());
          for (long unsigned int i = 0, j = _size-en.size(); i < en.size(); ++i, ++j) {
            if (i < 4) { //en needs to shift
              if (j < 2) { // *this in e[0,1]
                if (e[j] != isol8(en.ptr_or_start, mx_st--)) {
                  printf("9\n"); return false;
                }
              } else if ((j >= 2) && (j < (_size - 8))) { // *this is in vec
                if (string_vector.at(ptr_or_start + (j-2)) != static_cast<char>(isol8(en.ptr_or_start, mx_st))) {
                  printf("10\n"); return false;
                } else {
                  --mx_st;
                }
              } else { // *this is in last 8
                if (e[l8(_size, j)] != static_cast<char>(isol8(en.ptr_or_start, mx_st))) {
                  printf("11\n"); 
                  std::cout << "e[" << l8(_size,j) << "]: " << e[l8(_size, j)] << std::endl;
                  printf("ptr_or_start shifter: %c\n", isol8(en.ptr_or_start, mx_st));
                  return false;
                } else {
                  --mx_st;
                }
              }
            } else { // en is in e[]
              if ((j >= 2) && (j < (_size - 8))) {
                if (string_vector.at(ptr_or_start + (j-2)) != en.e[i-4]) {
                  printf("13\n"); return false;
                }
              } else { // *this is in last 8
                if (e[l8(_size, j)] != en.e[i-4]) {
                  printf("14\n"); return false;
                }
              }
            }
          }
          return true;
        }
      }
      printf("12\n"); return false;
    }
  }

  bool ends_with(std::string_view en) const {
    if (en.size() > _size) { return false; }
    else if (en.size() == _size) { return *this == en; }
    else if (en.size() < _size) {
      // Actual compare logic
      return false;
    }
  }

  // will use the string_view function
  bool ends_with(std::string en) const {
    return ends_with(std::string_view(en.c_str())); 
  }

  //OLY
  std::size_t find(const str &v, std::size_t pos = 0) const{
    if (v._size >_size) return -1;
    //if size ==vsize and == is true return 0 else return -1
    if (_size<=14){
    	return -1;// find_small_size(this, v, pos);
    } else {
    	std::string my_string = this->to_s();
    	std::string their_string = v.to_s ();
    	return my_string.find(their_string);
    }
  }
 
  std::size_t find(char c, std::size_t pos = 0) const {
  	int count =0;
  	if (_size <=14){
  		for (int i = 0 ; i < ((_size>4) ? 4: _size);i++){
  			char first = ((ptr_or_start >> (8 * (_size -1))) & 0xFF);
  			if ((first == c) and (count >= pos )) return count;
  			count ++;
  		}
  		if (_size >4 ){
  			for (int i =0; i < (_size -4); i++){
  				if ((e[i]) and (count >= pos)) return count ;
  			}
  		}
  		return -1;
  	} else {
  		for (int i = 0 ; i <  2;i++){
  			if ((e[i] == c) and (count >= pos) ) return count ;
  			count ++;
  		}
  		for (int i = 0 ; i< (_size-10); i++){
  			if ((string_vector.at(i)) and (count >= pos )) return count ;
  			count ++;
  		}
  		for (int i = 2 ; i <  10;i++){
  			if ((e[i] == c) and (count >= pos) ) return count ;
  			count ++;
  		}
  		return -1;

  	}
  }

  template <std::size_t N>
  constexpr std::size_t find(const char (&s)[N], std::size_t pos = 0) {
    return find(str(s), pos);
  }

  std::size_t rfind(const str &v, std::size_t pos = 0) const;
  std::size_t rfind(char c, std::size_t pos = 0) const;
  std::size_t rfind(const char *s, std::size_t pos, std::size_t n) const;
  std::size_t rfind(const char *s, std::size_t pos = 0) const;

  // returns a pstr from two objects (pstr)
  static str concat(const str &a, const str &b);
  static str concat(std::string_view a, const str &b);
  static str concat(const str &a, std::string_view b);
  static str concat(const str &a, int v);  // just puts two things together concat(x, b); -> x.append(b)
                                           //                               concat(b, x); -> b.append(x)

  str append(const str &b) const;
  str append(std::string_view b) const;
  str append(int b) const;

  std::vector<str> split(const char chr);  // used as a tokenizing func, return vector of pstr's

  bool is_i() const{ 
    if (_size < 14) {
      int temp = (_size >= 4) ? 3 : (_size-1);
      char first = (ptr_or_start >> (8 * (temp))) & 0xFF;
      //char first = ((ptr_or_start >> (8 * (_size -1))) & 0xFF);
      if (first !='-' and( first <'0' or first > '9')) {
        return false;
      }
      for (int i= 1; i<(_size>4?4:_size);i++){
        switch ((ptr_or_start >> (8 * (temp - i))) & 0xFF){
          case '0'...'9':
            break;
          default:
            return false;
            break;
        }
      }
      for (int i=0; i<(_size>4?_size-4:0);i++){
        switch (e[i]){
          case '0'...'9':
            break;
          default:
            return false;
            break;
        }
      }
    } else {
      char first = e[0];
      if (first !='-' and( first <'0' or first > '9')) {
        return false;
      }
      for (int i = 1;i<10 ; i++){
        switch (e[i]){
          case '0'...'9':
            break;
          default:
            return false;
            break;
        } 
      }
      for (int i = ptr_or_start ; i< _size-10;i++){
        switch (string_vector.at(i)){
          case '0'...'9':
            break;
          default:
            return false;
            break;
        }
      }
    }
    return true;
  }


  int64_t to_i() const{  // convert to integer
    if(this->is_i()){
      //std::cout << "The input is an integer " << std::endl;
      std::string temp = this->to_s();
      //std::cout << "The string is  " << _size << std::endl;
      return stoi(temp);
    }
    std::cout << "The input is not an integer " << std::endl;
  } 
  
  std::string to_s() const{  // convert to string
    std::string out;
    if (_size <= 14 ){
      //adding charactors from ptr_or_start based on the size of the string
      for (int i =0; i<((_size>4) ? 4: _size); i++){
        int temp = (_size >= 4) ? 3 : (_size-1); 
        out += (ptr_or_start >> (8 * (temp-i))) & 0xFF;
        //std::cout << "The out is  " << out << std::endl;
      }
      //if there are any characotrs in e, we add them as well
      if(_size>4){
        for(int i =0 ; i< (_size-4); i++){
          out += e[i];
        }
      } 
    } else{
      //adding the first two charactors
      for (int i =0; i< 2; i++){
        out += e[i];
      }
      //adding the middle section of the string from string vector
      for (int i = ptr_or_start; i < (ptr_or_start + _size - 10); i++) {   
        out += string_vector.at(i);
      }
      //adding the last 8 charactors
      for (int i = 2; i<10; i++){
        out += e[i];
      }
    }
    return out; 
  }

  str get_str_after_last(const char chr) const;
  str get_str_after_first(const char chr) const;

  str get_str_before_last(const char chr) const;
  str get_str_before_first(const char chr) const;

  str substr(size_t start) const;
  str substr(size_t start, size_t end) const;

  void test_cpp() const;
};


// For static string_map
mmap_lib::map<std::string_view, uint32_t> str::string_map2;

}  // namespace mmap_lib
