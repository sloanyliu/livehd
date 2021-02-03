// Visitor set to LiveHD
/*
 * This is a more optimized set data structure, built using mmap_lib's map.
 * 
 * The idea is that there is a hash map under the hood, and each <key,val>
 * pair's key is a unique index (0 to n), and the val is a bit vector (32 bits) 
 * that represents the existence (0 or 1) of 32 distinct numbers.
 *
 * For example: 
 * set.insert(100) 
 * will flip the 4th bit of the val of the 'key = 3' <key,val> pair high.
 *
 */

#pragma once

#include <string_view>

#include "mmap_map.hpp"

namespace mmap_lib {

// vset will have a Key to identify each BitMap
// data will be the BitMap (various numbers of different bits)

template <typename Key, typename T>
class vset {

private:
  T max = 0;
  T min = 0;

public:
  using VisitorSet = typename mmap_lib::map<Key, T>;
  VisitorSet   visitor_set;
  const size_t bucket_len   = sizeof(T) * 8;

  using bucket_iterator       = typename VisitorSet::iterator;
  using const_bucket_iterator = typename VisitorSet::const_iterator;

 

  // What does explicit do?
  explicit vset(std::string_view _set_name) : visitor_set(std::string(_set_name) + "_vs") {}
  explicit vset(std::string_view _path, std::string_view _set_name) : visitor_set(_path, std::string(_set_name) + "_vs") {}

  // Clears the whole data structures
  void clear() { 
    visitor_set.clear();
    max = 0;
    min = 0; 
  }

  //====
  void   wht() { std::cout << sizeof(T) << std::endl; }
  size_t bucket_size() { return bucket_len; }
  size_t num_buckets(size_t ln) { return (ln / (sizeof(T) * 8)); }
  //====

  /* All the bucet_set() functions all returns const_iterator
   * These functions set the ENTIRE bitmap at the key inputted
   * If I want to use these, I need to change bits outside of the set()
   */
  const_bucket_iterator bucket_set(Key &&key, T &&bitmap) { return visitor_set.set(key, bitmap); }
  const_bucket_iterator bucket_set(const Key &key, T &&bitmap) { return visitor_set.set(key, bitmap); }
  const_bucket_iterator bucket_set(Key &&key, const T &bitmap) { return visitor_set.set(key, bitmap); }
  const_bucket_iterator bucket_set(const Key &key, const T &bitmap) { return visitor_set.set(key, bitmap); }

  // returns true/false depending on if the key exists
  [[nodiscard]] bool has_key(const Key &key) const { return visitor_set.has(key); }

  [[nodiscard]] Key get_key(const bucket_iterator &it) const { return visitor_set.get_key(it); }
  [[nodiscard]] Key get_key(const const_bucket_iterator &it) const { return visitor_set.get_key(it); }

  /* All the bucket_get() functions, returns whatever the whole 64 bit number
   * Why the need for a template here?
   * Need to access single bits of the bitmap outside of the functions
   */

  template <typename T_ = T, typename = std::enable_if_t<!is_array_serializable<T_>::value>>
  [[nodiscard]] const T &bucket_get_val(const Key &key) const {
    return visitor_set.get(key);
  }

  template <typename T_ = T, typename = std::enable_if_t<is_array_serializable<T_>::value>>
  [[nodiscard]] T bucket_get_val(const Key &key) const {
    return visitor_set.get(key);
  }

  template <typename T_ = T, typename = std::enable_if_t<!is_array_serializable<T_>::value>>
  [[nodiscard]] const T &bucket_get_val(const bucket_iterator &it) const {
    return visitor_set.get(it);
  }

  template <typename T_ = T, typename = std::enable_if_t<is_array_serializable<T_>::value>>
  [[nodiscard]] T bucket_get_val(const bucket_iterator &it) const {
    return visitor_set.get(it);
  }

  template <typename T_ = T, typename = std::enable_if_t<!is_array_serializable<T_>::value>>
  [[nodiscard]] const T &bucket_get_val(const const_bucket_iterator &it) const {
    return visitor_set.get(it);
  }

  template <typename T_ = T, typename = std::enable_if_t<is_array_serializable<T_>::value>>
  [[nodiscard]] T bucket_get_val(const const_bucket_iterator &it) const {
    return visitor_set.get(it);
  }

  [[nodiscard]] bucket_iterator bucket_find(const Key &key) { return visitor_set.find(key); }
  [[nodiscard]] const_bucket_iterator bucket_find(const Key &key) const { return visitor_set.find(key); }

  // Functions used for iterating, begin() and end()
  [[nodiscard]] bucket_iterator bucket_begin() { return visitor_set.begin(); }
  [[nodiscard]] const_bucket_iterator bucket_begin() const { return visitor_set.cbegin(); }
  [[nodiscard]] const_bucket_iterator bucket_cbegin() const { return visitor_set.cbegin(); }

  [[nodiscard]] bucket_iterator       bucket_end() { return visitor_set.end(); }
  [[nodiscard]] const_bucket_iterator bucket_end() const { return visitor_set.cend(); }
  [[nodiscard]] const_bucket_iterator bucket_cend() const { return visitor_set.cend(); }

  /* These functions erase the WHOLE bitmap,
   * Erases element at pos, returns iterator to next element
   */
  bucket_iterator bucket_erase(const_bucket_iterator pos) { return visitor_set.erase(pos); }
  bucket_iterator bucket_erase(bucket_iterator pos) { return visitor_set.erase(pos); }

  // erases the key if it's there, if not, returns 0
  size_t bucket_erase_key(const Key &key) {
    auto it = visitor_set.find(key);
    if (it == visitor_set.end()) {
      return 0;
    }
    erase(it);
    return 1;
  }

  // Makes space inside the map to accommodate for whatever size s is
  void reserve(size_t s) { visitor_set.reserve(s); }

  [[nodiscard]] size_t size() const { return visitor_set.size(); }
  [[nodiscard]] bool   empty() const { return visitor_set.empty(); }
  [[nodiscard]] size_t capacity() const { return visitor_set.capacity(); }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  /* Can add functions here to set a bit specifically in the bitmap at key
   * call them bit_set() x 4, all return const_iterator too
   *
   * Can also add functions here to un-set bits instead of deleting entire bitmap
   * Kind of like another variation of set but set it to 0 instead of 1
   *
   * ex) func -->
   * const_iterator bit_set(Key &&key, T &&bitmap, someTypeDependsOnBitMap bit_pos) {
   *   const auto t = visitor_set.get(key)
   *   [change bitmap at index bit_pos]
   *   return visitor_set.set(key, bitmap)
   * }
   */

  /* All the insert() functions are void
   * The funcs intake a number you wish to insert into the set
   *
   * All the erase() functions are void too
   * The funcs intake a number you wish to remove from the set
   */
  
  
  [[nodiscard]] void insert(T &&ele) {
    // find correct index the pos is at
    const auto p    = ele / (sizeof(T) * 8);  // p will be the key that points to the correct bitmap
    const auto i    = ele % (sizeof(T) * 8);  // i will be the bit we are interested in in the bitmap
    T          hold = 0;                      // will hold the bitmap at index p if there is one
    if (visitor_set.has((Key)p)) {
      hold = visitor_set.get((Key)p);
    }                                  // is there a bitmap at key p
    hold = hold | (1 << i);            // modify the bit at pos
    visitor_set.set((Key)p, (T)hold);  // put it back in the bitmap

    if (ele > max) { max = ele; }
  }

  [[nodiscard]] void insert(const T &&ele) {
    // find correct index the pos is at
    const auto p    = ele / (sizeof(T) * 8);  // p will be the key for the bitmap
    const auto i    = ele % (sizeof(T) * 8);  // i will be the bit we are interested in
    T          hold = 0;                      // will hold the bitmap at index p if there is one
    if (visitor_set.has((Key)p)) {
      hold = visitor_set.get((Key)p);
    }                                  // is there a bitmap at key p
    hold = hold | (1 << i);            // modify the bit at pos
    visitor_set.set((Key)p, (T)hold);  // put it back in the bitmap
    
    if (ele > max) { max = ele; }
  }
  
  //================================

  [[nodiscard]] void erase(T &&ele) {
    // find correct index the pos is at
    Key     p    = ele / (sizeof(T) * 8);  // p will be the key that points to the correct bitmap
    uint8_t i    = ele % (sizeof(T) * 8);  // i will be the bit we are interested in in the bitmap
    T       hold = 0;                      // will hold the bitmap at index p if there is one

    if (visitor_set.has((Key)p)) {
      hold = visitor_set.get((Key)p);
      hold = hold & ~(1 << i);           // modify the bit at i
      if (hold == 0) { 
        visitor_set.erase((Key)p); 
      } else { 
        visitor_set.set((Key)p, (T)hold); 
      } // put it back in the bitmap
    }

    // Need to  add logic to update max    
    if (ele == max) {
      while (hold == 0) {
        if (p == 0) {
          max = 0;
          return;
        } else {
          p = p - 1;
          if (visitor_set.has((Key)p)) {
            hold = visitor_set.get((Key)p);
          }
        }
      }

      i = (sizeof(T) * 8) - 1;
      ele = (p + 1) * (sizeof(T) * 8) - 1;

      while (i != 0) {
        if ((hold >> i) & 1) {
          max = ele;
          return;
        } else {
          i = i - 1;
          ele = ele - 1; 
        }
      }
    }
  }

  [[nodiscard]] void erase(const T &&ele) {
    // find correct index the pos is at
    Key     p    = ele / (sizeof(T) * 8);  // p will be the key that points to the correct bitmap
    uint8_t i    = ele % (sizeof(T) * 8);  // i will be the bit we are interested in in the bitmap
    T       hold = 0;                      // will hold the bitmap at index p if there is one

    if (visitor_set.has((Key)p)) {
      hold = visitor_set.get((Key)p);
      hold = hold & ~(1 << i);           // modify the bit at i
      if (hold == 0) { 
        visitor_set.erase((Key)p); 
      } else { 
        visitor_set.set((Key)p, (T)hold); 
      } // put it back in the bitmap
    }
    
    // Need to  add logic to update max
    if (ele == max) {
      while (hold == 0) {
        if (p == 0) {
          max = 0;
          return;
        } else {
          p = p - 1;
          if (visitor_set.has((Key)p)) {
            hold = visitor_set.get((Key)p);
          }
        }
      }

      i = (sizeof(T) * 8) - 1;
      ele = (p + 1) * (sizeof(T) * 8) - 1;

      while (i != 0) {
        if ((hold >> i) & 1) {
          max = ele;
          return;
        } else {
          i = i - 1;
          ele = ele - 1; 
        }
      }
    }
  }

  /* Can add functions here to get a single bit of the bitmap
   */

  [[nodiscard]] bool find(T &&ele) {
    // finding the correct bitmap to get
    const auto p = ele / (sizeof(T) * 8);
    const auto i = ele % (sizeof(T) * 8);

    // if this map is there and the bit we want is set high, return true
    if (visitor_set.has((Key)p) && (visitor_set.get((Key)p) >> i & 1)) {
      return true;
    } else {
      return false;  // otherwise it's not there
    }
  }

  [[nodiscard]] bool find(const T &&ele) {
    const auto p = ele / (sizeof(T) * 8);
    const auto i = ele % (sizeof(T) * 8);

    if (visitor_set.has((Key)p) && (visitor_set.get((Key)p) >> i & 1)) {
      return true;
    } else {
      return false;
    }
  }

  // Functions used for iterating, begin() and end()
  [[nodiscard]] bool is_start(T &&ele) {
    if (ele == 0) {
      return true;
    }
    return false;
  }

  [[nodiscard]] bool is_start(const T &&ele) {
    if (ele == 0) {
      return true;
    }
    return false;
  }

  [[nodiscard]] T get_max() { return max; } 
  
  /*
   * Iterator class for vset
   */
  class vIter {
  private:
    T test = 0;
    vset &owner;

  public:
    //unsigned int iData;
    T iData;

    vIter(vset &tmp): iData(0) , owner(tmp){ }
    ~vIter() { ; }

    void cont_test() { std::cout << "made it" << std::endl; }
    void iter_change(T ele) { iData = ele; }
    T iter_val() { return iData; }
    
    /* Revert back to old ways for now 
     * Need to think about how to make sure vIter doesnt ++ or -- blindly*/
    vIter operator++() { iData = (iData < 0) ? 0 : iData - 1;}
    vIter operator++(int other) { iData = (iData == 0) ? 0 : iData - 1;} 
    vIter operator--() { iData = (iData == 0) ? 0 : iData - 1; }
    vIter operator--(int other) { iData = (iData == 0) ? 0 : iData - 1; }
    /*
    vIter operator++() { 
      while (true) {
        if (iData < vset::get_max()) {
          if (vset::find(iData + 1)) {
            iData = iData + 1;
            break;
          } else {
            iData = iData + 1;
          }
        } else if (iData == vset::get_max()) {
          break;
        }
      }
    } //prefix ++i

    vIter operator++(int other) { 
      while (true) {
        if (iData < vset::get_max()) {
          if (vset::find(iData + 1)) {
            iData = iData + 1;
            break;
          } else {
            iData = iData + 1;
          }
        } else if (iData == vset::get_max()) {
          break;
        }
      }
    } //postfix i++

    vIter operator--() { 
      while (true) {
        if (iData == 0) {
          break;
        } else {
          if (vset::find(iData - 1)) {
            iData = iData - 1;
            break;
          } else {
            iData = iData - 1;
          }
        }
      }
    } //prefix --i
    
    vIter operator--(int other) { iData = (iData == 0) ? 0 : iData - 1;} //postfix i--
    */

    T get_set_max() { return owner.max; }
  };
  
  void test_begin() { 
    vIter alpha;
    alpha.cont_test();
  }

  vIter test_ret() {
    vIter beta;
    return beta;
  }

  // trying to change vIter member vars using vset
  T set_and_ret (T ele) {
    vIter::test = ele;
    return vIter::test;
  }

  [[nodiscard]] vIter begin() {
    vIter tmp(*this);
    if (visitor_set.empty() == true) {
      return tmp;
    }
    
    for (auto i = 0; i <= max; i = i + 1) {
      if (vset::find(i) == true) {
        tmp.iter_change(i);
        return tmp;
      }
    }
    /*
    if (vset::find(0) == false) {
      std::cout << "find() works" << std::endl;
    } else {
      std::cout << "Detected!" << std::endl;
    }*/
    return tmp;
  }
};

}  // namespace mmap_lib
