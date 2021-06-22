// Visitor set to LiveHD

#pragma once
#include <string_view>
#include <typeinfo>
#include "mmap_map.hpp"


namespace mmap_lib {

// This set is optimized for spatial locality.
// Meaning: numbers that are close to each other in VALUE,
// will be made 'close' to each other from quick and easy access.
// 
// We accomplish by: having a mmap that holds <number, neighborhood>
// -> each number will be an actual value that is stored with all its bits.
// -> each neighborhood will represent bits around the number, as a bitmap.
//   -> the number will sit in the neighborhood.
//   -> could be in the front, back, middle (probably harder)
// -> each access to values near number, will access the neighborhood.


template<typename prsn, typename neighbors>
class set {
  protected:
    mmap_lib::map<prsn, neighbors> stown;
  public:
    set(std::string_view _name): stown(_name) {}
    set(std::string_view _path, std::string_view _name): stown(_path, _name) {}
    ~set() { set_data.clear(); }

    [[nodiscard]] void clear() { stown.clear(); }
    [[nodiscard]] size_t size() { stown.size(); }    
    [[nodiscard]] bool empty() { stown.empty(); }
    [[nodiscard]] size_t capacity() { stown.capacity(); }
    
    //insert()
    //Duplicate inserts are not allowed for now
    //TODO: Maybe enable an option for duplicate inserts
    //TODO: 
    uint32_t insert(prsn var) {
      if () {

      } else {

      }
      prsn adj_var = (prsn % );
      auto it = stown.find(var);
      if (it == stown.end()) {
        auto insert_pos = stown.set(var, 0);
        return static_cast<uint32_t>(insert_pos->first);
      } else {
        
      }
    } 

    //erase()
    void erase(prsn var) {
    
    }

    //find()
    //contains()
    //begin()
    //end()
    //rbegin()
    //rend()
    //cbegin()
    //cend()
    //crbegin()
    //crend()
    //max_size()
    //insert(it, Obj)
    //iter find(Obj)
    //iter contains(Obj)
    //emplace(Obj) --> only insert if unique

    class sIter {
      private:
        setIter &owner;
      public:
        sIter(setIter &tmp) : owner(tmp) {}
        ~sIter() { ; }

        /*
        sIter &operator++() { 
          ++owner;
          return *this; 
        }

        sIter operator++(int other) { 
          sIter temp = *this;
          ++*this;
          return temp; 
        } //postfix ++

        sIter &operator--() { 
          --owner; 
          return *this;
        }

        sIter operator--(int other) { 
          sIter temp = *this;
          --*this; 
          return temp;
        } //postfix --

        bool operator==(sIter other) const {
          return owner == other.owner;
        }

        bool operator!=(sIter other) const {
          return owner != other.owner;
        }
        */

        //operator++()
        //operator--()
        //operator==()
        //operator!=()
        //Obj operator*() const { return set_data.get_key(owner); }
    };


};




// vset will have a Key to identify each BitMap
// data will be the BitMap (various numbers of different bits)
// end() for now returns the Actual last element, need it to return AFTER last

// one implementation is the easy mmap set
// the other is the bitmap one, which can be improved and will be

#if 0
template<typename num, typename neighbors>
class set {
  protected:
    using setMap = typename mmap_lib::map<num, neighbors>;
    using setIter = typename setMap::iterator;
    using setConstIter = typename setMap::const_iterator;
    
    setMap set_data; // mmap_lib::map<num, neighbors> set_data
  public:
    set(std::string_view _name): set_data(_name) {}
    set(std::string_view _path, std::string_view _name): set_data(_path, _name) {}
    ~set() { set_data.clear(); }

    void clear() { set_data.clear(); }
    size_t size() { set_data.size(); }    
    bool empty() { set_data.empty(); }
    size_t capacity() { set_data.capacity(); }
    
    setIter insert(Obj var) { 
      //TODO: add case where the var already exists
      return set_data.set(var, 0); 
    }   

    void erase(Obj var) {
      //TODO: add case where the var already exists as well
      if (set_data.find(var) != set_data.end()) {
        set_data.erase(var);      
      }
    }

    void erase(setIter it) { set_data.erase(it); }
    void erase(setCIter it) { set_data.erase(it); }

    setIter find(Obj var) { return set_data.find(var); }
    setIter contains(Obj var) { return find(var); }

    class sIter {
      private:
        setIter &owner;
      public:
        sIter(setIter &tmp) : owner(tmp) {}
        ~sIter() { ; }
        sIter &operator++() { 
          ++owner;
          return *this; 
        }

        sIter operator++(int other) { 
          sIter temp = *this;
          ++*this;
          return temp; 
        } //postfix ++

        sIter &operator--() { 
          --owner; 
          return *this;
        }

        sIter operator--(int other) { 
          sIter temp = *this;
          --*this; 
          return temp;
        } //postfix --

        bool operator==(sIter other) const {
          return owner == other.owner;
        }

        bool operator!=(sIter other) const {
          return owner != other.owner;
        }

        // Fix this to make the auto for loop work
        //Obj operator*() const { return set_data.get_key(owner); }
    };

    sIter begin() { return sIter(set_data.begin()); }
    sIter end() { return sIter(set_data.end()); }

    //begin()
    //end()
    //rbegin()
    //rend()
    //cbegin()
    //cend()
    //crbegin()
    //crend()
    //max_size()
    //insert(it, Obj)
    //iter find(Obj)
    //iter contains(Obj)
    //emplace(Obj) --> only insert if unique

};

///////////////////////////////////////////////
#endif



#if 0
template <typename Key, typename T>
class vset {
  // FIXME: info sentinel?
private:
  T max = 0;
  T min = 0;

public:
  using VisitorSet = typename mmap_lib::map<Key, T>;
  VisitorSet   visitor_set;
  const size_t bucket_len = sizeof(T) * 8;

  using bucket_iterator       = typename VisitorSet::iterator;
  using const_bucket_iterator = typename VisitorSet::const_iterator;

  // What does explicit do?
  explicit vset(std::string_view _set_name) : visitor_set(std::string(_set_name) + "_vs") {}
  explicit vset(std::string_view _path, std::string_view _set_name) : visitor_set(_path, std::string(_set_name) + "_vs") {}

  ~vset() { visitor_set.clear(); }

  // Clears the whole data structure
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

  /* All the bucket_set() functions all returns const_iterator
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

  [[nodiscard]] bucket_iterator       bucket_find(const Key &key) { return visitor_set.find(key); }
  [[nodiscard]] const_bucket_iterator bucket_find(const Key &key) const { return visitor_set.find(key); }

  // Functions used for iterating, begin() and end()
  [[nodiscard]] bucket_iterator       bucket_begin() { return visitor_set.begin(); }
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
    const auto p    = ele / (sizeof(T) * 8);  // p points to the correct bitmap
    const auto i    = ele % (sizeof(T) * 8);  // i is the bit we want in the bitmap
    T          hold = 0;                      // will hold the bitmap at index p
    if (visitor_set.has((Key)p)) {
      hold = visitor_set.get((Key)p);
    }                                  // get the bitmap at p
    hold = hold | (1 << i);            // modify the bit at pos
    visitor_set.set((Key)p, (T)hold);  // put it back in the bitmap
    if (ele > max) {
      max = ele;
    }
  }

  [[nodiscard]] void insert(const T &&ele) {
    // find correct index the pos is at
    const auto p    = ele / (sizeof(T) * 8);  // p points to the correct bitmap
    const auto i    = ele % (sizeof(T) * 8);  // i is the bit we want in the bitmap
    T          hold = 0;                      // will hold the bitmap at index p
    if (visitor_set.has((Key)p)) {
      hold = visitor_set.get((Key)p);
    }                                  // get the bitmap at p
    hold = hold | (1 << i);            // modify the bit at pos
    visitor_set.set((Key)p, (T)hold);  // put it back in the bitmap
    if (ele > max) {
      max = ele;
    }
  }

  //================================

  [[nodiscard]] void erase(T &&ele) {
    // find correct index the pos is at
    Key     p    = ele / (sizeof(T) * 8);  // p will be the key that points to the correct bitmap
    uint8_t i    = ele % (sizeof(T) * 8);  // i will be the bit we are interested in in the bitmap
    T       hold = 0;                      // will hold the bitmap at index p if there is one

    if (visitor_set.has((Key)p)) {
      hold = visitor_set.get((Key)p);
      hold = hold & ~(1 << i);  // modify the bit at i
      if (hold == 0) {
        visitor_set.erase((Key)p);
      } else {
        visitor_set.set((Key)p, (T)hold);
      }  // put it back in the bitmap
    }

    // Need to  add logic to update max
    //=======================================================
    // Maybe add some logic here to get rid of the idxs of map that are zero
    // if hold == 0 --> delete the spot at key p
    // this way, we avoid needing to traverse it again
    //=======================================================
    if (ele == max) {
      //==============================
      //===Trying new logic for Max===
      //==============================
      // if hold == 0
      //   delete hold; check the p
      //   if p == 0: max is 0; return
      //   else:
      //     decrement p
      //     while p >= 0: check the p
      //       if p exists: get and check the hold
      //         if hold == 0: delete hold; decrement p
      //         else:
      //           check hold for next high bit and set max
      //           return
      //     max is 0; return
      // else
      //   check hold for next high bit and set max
      //   return
      //

      while (hold == 0) {
        if (p == 0) {
          max = 0;
          return;
        } else {
          --p;
          if (visitor_set.has((Key)p)) {
            hold = visitor_set.get((Key)p);
          }
        }
      }

      i   = (sizeof(T) * 8) - 1;
      ele = (p + 1) * (sizeof(T) * 8) - 1;

      while (i != 0) {
        if ((hold >> i) & 1) {
          max = ele;
          return;
        } else {
          --i;
          --ele;
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
      hold = hold & ~(1 << i);  // modify the bit at i
      if (hold == 0) {
        visitor_set.erase((Key)p);
      } else {
        visitor_set.set((Key)p, (T)hold);
      }  // put it back in the bitmap
    }
    if (ele == max) {
      while (hold == 0) {
        if (p == 0) {
          max = 0;
          return;
        } else {
          --p;
          if (visitor_set.has((Key)p)) {
            hold = visitor_set.get((Key)p);
          }
        }
      }

      i   = (sizeof(T) * 8) - 1;
      ele = (p + 1) * (sizeof(T) * 8) - 1;

      while (i != 0) {
        if ((hold >> i) & 1) {
          max = ele;
          return;
        } else {
          --i;
          --ele;
        }
      }
    }
  }

  // Not the 'real' find function
  // Need a wrapper to call this func and put it in a vIter
  [[nodiscard]] bool efind(T &&ele) {
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

  [[nodiscard]] bool efind(const T &&ele) {
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

  [[nodiscard]] T get_max() const { return max; }

  /*
   * Iterator class for vset
   */
  class vIter {
  private:
    T     test = 0;
    vset &owner;  // a reference to the vset this vIter is a part of
                  // this reference included in order to access vset members
    T iData = 0;

  public:
    vIter(vset &tmp) : iData(0), owner(tmp) {}
    ~vIter() { ; }

    void cont_test() { std::cout << "made it" << std::endl; }
    void iter_change(T ele) { iData = ele; }
    T    iter_val() { return iData; }

    T get_set_max() { return owner.max; }

    vIter &operator++() {
      int flg = 0;
      if (iData == owner.max) {
        ++iData;
        return *this;
      } else if (iData > owner.max) {
        return *this;
      } else if (iData < owner.max) {
        // FIXME
        // Concerned with the while under here
        while (owner.efind(iData + 1) == false) {
          ++iData;
        }
        ++iData;
      }
      return *this;
    }  // prefix ++i

    vIter operator++(int other) {
      vIter temp = *this;
      ++*this;
      return temp;
    }  // postfix i++

    vIter &operator--() {
      T _iData = iData;

      while (true) {
        if (_iData == 0) {
          break;
        } else {
          // FIXME
          // There is a while here to
          if (owner.efind(_iData - 1)) {
            _iData = _iData - 1;
            iData  = _iData;
            break;
          } else {
            _iData = _iData - 1;
          }
        }
      }
      return *this;
    }  // prefix --i

    vIter operator--(int other) {
      vIter temp = *this;
      --*this;
      return temp;
    }  // postfix i--

    bool operator!=(vIter other) const {
      if (iData != other.iter_val()) {
        return true;
      }
      return false;
    }

    bool operator==(vIter other) const {
      if (iData == other.iter_val()) {
        return true;
      }
      return false;
    }

    // Fix this to make the auto for loop work
    T operator*() const { return iData; }
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
  T set_and_ret(T ele) {
    vIter::test = ele;
    return vIter::test;
  }

  [[nodiscard]] vIter begin() {
    vIter tmp(*this);
    tmp.iter_change(0);
    if (visitor_set.empty() == true) {
      return tmp;
    }

    for (auto i = 0; i <= max; ++i) {
      if (vset::efind(i) == true) {
        tmp.iter_change(i);
        return tmp;
      }
    }
    return tmp;
  }

  [[nodiscard]] vIter end() {
    vIter tmp(*this);
    if (visitor_set.empty() == true) {
      return tmp;
    }

    // includes last element of set
    // tmp.iter_change(vset::get_max()+1);

    // does not include last element of set
    tmp.iter_change(max + 1);
    return tmp;
  }

  [[nodiscard]] vIter find(T ele) {
    vIter tmp(*this);
    if (vset::efind(ele + 0)) {
      tmp.iter_change(ele);
      return tmp;
    } else {  // if it does not exist, it equals end
      tmp.iter_change(max + 1);
      return tmp;
    }
  }

  [[nodiscard]] vIter contains(T &&ele) { return vset::find(ele); }

  [[nodiscard]] vIter contains(const T &&ele) { return vset::find(ele); }
};

#endif

}  // namespace mmap_lib
