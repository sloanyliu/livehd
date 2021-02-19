//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include <stdio.h>
#include <stdlib.h>
#include "mmap_map.hpp"


namespace mmap_lib {

class str {
protected:
  // Keepking the code constexpr for small strings (not long) requires templates (A challenge but reasonable).
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

  uint32_t ptr_or_start;  // 4 chars if _size < 14, ptr to mmap otherwise
  std::array<char, 10> e; // last 10 "special <128" characters ending the string
  uint16_t _size;          // 2 bytes
  int map_key 
  using Map = typename mmap_lib::map<uint32_t, uint32_t>;
  static Map string_map();
  staticc vector<int> string_vector ; 


  constexpr bool is_digit(char c) const {
    return c>='0' && c <='9';
  }

public:

  //first two are compile time -> compiler will do everything for you, not a single instr. at a time

  // Must be constexpr to allow fast (constexpr) cmp for things like IDs.
  /*template<std::size_t N, typename = std::enable_if_t<(N-1)<14>>

    constexpr str(const char(&s)[N]): ptr_or_start(0), e{0}, _size(N-1) { // N-1 because str includes the zero
      auto stop    = _size<4?_size:4;
      //isptr =  _size<14?false:true;
      for(auto i=0;i<stop;++i) {
        ptr_or_start <<= 8;
        ptr_or_start |= s[i];
      }
      auto e_pos = 0;
      for(auto i=stop;i<_size;++i) {
        assert(s[i]<128); // FIXME: use ptr if so
        if (is_digit(s[i]) && i<_size && is_digit(s[i+1])) {
          uint8_t v = (s[i]-'0')*10+s[i+1]-'0';
          assert(v<100); // 2 digits only
          e[e_pos] = 0x80 | v;
          ++i; // skip one more
        }else{
          e[e_pos] = s[i];
        }
        ++e_pos;
      }
    }
  */

<<<<<<< HEAD
  // copy of first one, add some stuff
  template<std::size_t N, typename = std::enable_if_t<(N-1)>=14>, typename=void>
=======
    constexpr str(const char(&s)[N]): ptr_or_start(0), e{0}, _size(N-1) { // N-1 because str includes the zero
      auto stop    = _size<4?_size:4;
      for(auto i=0;i<stop;++i) {
        ptr_or_start <<= 8;
        ptr_or_start |= s[i];
      }
      auto e_pos = 0;
      for(auto i=stop;i<_size;++i) {
        assert(s[i]<128); // FIXME: use ptr if so
        /*if (is_digit(s[i]) && i<_size && is_digit(s[i+1])) {
          uint8_t v = (s[i]-'0')*10+s[i+1]-'0';
          assert(v<100); // 2 digits only
          e[e_pos] = 0x80 | v;1000 0000
          ++i; // skip one more
        }else{*/
        e[e_pos] = s[i];
        /*}*/
        ++e_pos;
      }
    }
  /*template<std::size_t N, typename = std::enable_if_t<(N-1)>=14>, typename=void>
>>>>>>> upstream/master
    constexpr str(const char(&s)[N]): ptr_or_start(0), e{0}, _size(N-1) { // N-1 because str includes the zero
      ptr_or_start = 0;
      auto e_pos   = 0u;
      for(auto i=(N-1-8);i<N-1;++i) { // 8 (not 10 to allow to grow a bit) last positions
        assert(s[i]<128); // FIXME: use ptr if so
        if (is_digit(s[i]) && i<_size && is_digit(s[i+1])) {
          uint8_t v = (s[i]-'0')*10+s[i+1]-'0';
          assert(v<100); // 2 digits only
          e[e_pos] = 0x80 | v;
          ++i; // skip one more
          --_size;
        }else{
          e[e_pos] = s[i];
        }
        ++e_pos;
      }
    }*/
    constexpr str(const char(&s)[N]): ptr_or_start(0), e{0}, _size(N-1) { // N-1 because str includes the zero
      //the first two charactors 
      e[0] = s[0];
      e[1] = s[1];
      //the last eight  charactors
      for (int i=0;i<8;i++){
        e[_size-i] = s[_size-i];
      } 
      //checking if it exists
      char*  long_str;
      for (int i =0 ; i<_size-8 ; i++){
          long_str[i] = s[i+2]; 
      }
      pair<int, int> pair = str_exists(long_str, _size-10);
      if (pair.second){
       ptr_or_start = pair.first;
      } else{

<<<<<<< HEAD
  constexpr str(std::string_view sv) : ptr_or_start(0), e{0}, _size(sv.size()) {  //starting with a string_view instead of string literal ("hello")
                                                                                  //compiler doesnt know size of something, we call this one
=======
        for (int i =0;i<_size-10;i++){
          string_vector.push_back(s[i]);
          //add the starting position of the vector as a key to the map
          //now add the size -10 as a value to the map
          //ptr_ot_start will be the key
        }

      }

    }
    std::pair<int, int> str_exists(const char* string_to_check,uint32_t size){
      bool vector_flag = true;
        for (auto i = string_map.begin(),end = string_map.end();i !=end ;++i){
          uint32_t key = string_map.get_key(i); 
          uint16_t value = string_map.gets(i);
          if (value != size) continue;
          for(int i =0; i<size;i++){
            if(string_vector.at(key+i) != string_to_check[i])  {
              vector_flag = false;
              break;
            } 
            if (vector_flag) return std::make_pair(key, value);
            vector_flag = true; 
          }   
        }
      return std::make_pair(0, 0);
    }
  constexpr str(std::string_view sv) : ptr_or_start(0), e{0}, _size(sv.size()) {
>>>>>>> upstream/master
    // FIXME: maybe short maybe long
    if (sv.size()<14) { // FIXME: create method to share this code with str short char constructor
      auto stop    = _size<4?_size:4;
      for(auto i=0;i<stop;++i) {
        ptr_or_start <<= 8;
        ptr_or_start |= sv[i];
      }
      auto e_pos = 0;
      for(auto i=stop;i<_size;++i) {
        assert(sv[i]<128); // FIXME: use ptr if so
        if (is_digit(sv[i]) && i<_size && is_digit(sv[i+1])) {
          uint8_t v = (sv[i]-'0')*10+sv[i+1]-'0';
          assert(v<100); // 2 digits only
          e[e_pos] = 0x80 | v;
          ++i; // skip one more
        }else{
          e[e_pos] = sv[i];
        }
        ++e_pos;
      }
      _size = sv.size();
    }
  }

#if 0
  fixme_const_iterator begin()  const {
    for(const auto &ch:data.b) {
      if (ch!=0)
        return &ch;
    }
    if (size<16)
      return &e[0];

    return ptr;
  }
  fixme_const_iterator end()    const {
    if (size<16)
      return &e[size-4];

    for(const auto &ch:e) {
      if (ch==0)
        return &ch;
    }
    return e.end();
  }
#endif

  [[nodiscard]] constexpr std::size_t size() const { return _size; }
  [[nodiscard]] constexpr std::size_t length()   const { return _size; }
  [[nodiscard]] constexpr std::size_t max_size() const { return 65535; }

  [[nodiscard]] constexpr bool empty() const { return 0 == _size; }

  [[nodiscard]] void print_PoS() {std::cout << ptr_or_start << std::endl;}
  [[nodiscard]] void print_e() {  // e is weird, printing question marks
    std::cout << "e is: ";
    for (int i = 0; i < 10 ; ++i) {
      std::cout << int(e.at(i));
    }
    std::cout << std::endl;
  }
  [[nodiscard]] void print_size() {std::cout << "String size is:" << _size << std::endl;}


  template<std::size_t N>
  constexpr bool operator==(const char(&s)[N]) const {
    return str(s) == *this;
  }

  constexpr bool operator==(const str& rhs) const {
    for(auto i=0u;i<e.size();++i) {
      if (e[i] != rhs.e[i])
        return false;
    }
    return ptr_or_start == ptr_or_start && _size == rhs._size;
  }
  constexpr bool operator!=(const str& rhs) const {
    return !(*this == rhs);
  }

  constexpr char operator[](std::size_t pos) const {
#ifndef NDEBUG
    if (pos >= _size)
      throw std::out_of_range("");
#endif
    if (_size<14) {
      if (pos<4)
        return (ptr_or_start>>(8*(3-pos))) & 0xFF;
      return e[pos-4]; // FIXME: this fails if string has digits like "f33a"
    }

    assert(false); // FIXME for long strings
    return 0;
  }

  bool starts_with(const str &v) const;
  bool starts_with(std::string_view sv) const;
  bool starts_with(std::string s) const;

  bool ends_with(const str &v) const;
  bool ends_with(std::string_view sv) const;
  bool ends_with(std::string s) const;

  std::size_t find(const str &v, std::size_t pos = 0 ) const;
  std::size_t find(char c, std::size_t pos = 0 ) const;
  template<std::size_t N>
  constexpr std::size_t find(const char(&s)[N], std::size_t pos=0) {
    return find(str(s), pos );
  }

  std::size_t rfind(const str &v, std::size_t pos = 0 ) const;
  std::size_t rfind(char c, std::size_t pos = 0 ) const;
  std::size_t rfind(const char *s, std::size_t pos, std::size_t n ) const;
  std::size_t rfind(const char *s, std::size_t pos = 0 ) const;

  //returns a pstr from two objects (pstr)
  static str concat(const str &a, const str &b);
  static str concat(std::string_view a, const str &b);
  static str concat(const str &a, std::string_view b);
  static str concat(const str &a, int v); // just puts two things together concat(x, b); -> x.append(b)
                                          //                               concat(b, x); -> b.append(x)


  // New Stuff:
  str append(const str       &b) const; // adds to the end, x.append(b); x<=b
  str append(std::string_view b) const;
  str append(int              b) const;


  std::vector<str> split(const char chr); // used as a tokenizing func, return vector of pstr's

  bool is_i() const{ // starts with digit -> is integer
    //this fun works when str size is <14   
    //if(!isptr){
      char chars[5];
      std::cout << "chars[] inside is_i(): ";
      for (int i =3, j=0;i>=0;i--,j++){
         chars[j] = (ptr_or_start >> (i*sizeof(char)*8)) & 0x000000ff;
         std::cout << chars[j];
      } 
      std::cout << std::endl;
      if (chars[0]!='-' and( chars[0]<'0' or chars[0]> '9')) {
        std::cout << "Non-number char detected in ptr_or_start[0]\n";
        return false; 
      }
      for (int i= 1; i<(_size>4?4:_size);i++){
        switch (chars[i]){
          case '0'...'9':
            break;
          default:
            std::cout << "Non-number char detected in ptr_or_start[1:3]\n";
            return false;
            break;
        }
      }
      for (int i=0; i<(_size>4?_size-4:0);i++){
        switch (e[i]){
          case '0'...'9':
            break;
          default:
            std::cout << "Non-number char detected in e\n";
            return false;
            break;
        }
      }
    //}
    return true;  
  } 
  

  // How to handle if it's not an int?
  // what to return/exceptions?
  int64_t to_i() const { // only works if _size < 14
    /*
    if (this.is_i()) {  
      int64_t hold = 0;
      // convert ptr_or_start first
      // convert e next
    } else {
      return;
    } 
  */  
  } // convert to integer

  //first or last refers to occurence of the char chr
  str get_str_after_last_occurence_of_chr (const char chr) const; // split str from chr 
  str get_str_after_first_occurence_of_chr (const char chr) const; // split str from chr

  str get_str_before_last_occurence_of_chr (const char chr) const; // split str from chr
  str get_str_before_first_occurence_of_chr (const char chr) const; // split str from chr

  str substr(size_t start) const;
  str substr(size_t start, size_t end) const;
};

}  // namespace mmap_lib
