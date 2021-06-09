//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

#include "mmap_map.hpp"

#include <unistd.h>

#include "absl/container/flat_hash_map.h"
#include "fmt/format.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "lrand.hpp"
#include "mmap_bimap.hpp"

class Setup_mmap_map_test : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(Setup_mmap_map_test, string_data) {
  Lrand<int> rng;

  bool zero_found = false;
  while (!zero_found) {
    mmap_lib::map<uint32_t, std::string_view> map;
    map.clear();
    absl::flat_hash_map<uint32_t, std::string> map2;

    int conta = 0;
    for (int i = 0; i < 10000; i++) {
      int         key     = rng.max(0xFFFF);
      std::string key_str = std::to_string(key) + "foo";

      if (map.has(key)) {
        EXPECT_EQ(map2.count(key), 1);
        continue;
      }

      conta++;

      EXPECT_TRUE(!map.has(key));
      map.set(key, key_str);
      EXPECT_TRUE(map.has(key));

      EXPECT_EQ(map2.count(key), 0);
      map2[key] = key_str;
      EXPECT_EQ(map2.count(key), 1);
    }

    for (const auto &it : map) {
      (void)it;
      if (it.getFirst() == 0)
        zero_found = true;
      const auto &key = map.get_key(it);
      EXPECT_TRUE(map.has(key));

      std::string_view val = map.get(it);
      EXPECT_EQ(val, std::to_string(it.first) + "foo");
      conta--;
    }
    for (const auto &it : map2) {
      if (!map.has(it.first))
        std::cout << "HI\n";
      EXPECT_TRUE(map.has(it.first));
    }

    EXPECT_EQ(conta, 0);

    fmt::print("load_factor:{} conflict_factor:{} txt_size:{}\n", map.load_factor(), map.conflict_factor(), map.txt_size());
  }
}

TEST_F(Setup_mmap_map_test, string_data_persistance) {
  Lrand<int> rng;

  absl::flat_hash_map<uint32_t, std::string> map2;

  unlink("lgdb_bench/mmap_map_test_sview_data");
  EXPECT_EQ(access("lgdb_bench/mmap_map_test_sview_data", F_OK), -1);

  int conta;
  for (int n = 0; n < 3; n++) {
    mmap_lib::map<uint32_t, std::string_view> map("lgdb_bench", "mmap_map_test_sview_data");
    auto                                      it = map.set(3, "test");
    EXPECT_EQ(it->first, 3);
    EXPECT_NE(it->second, 0);
    EXPECT_EQ(map.get(it->first), "test");
    map.clear();
    map2.clear();

    conta = 0;
    for (int i = 0; i < 10000; i++) {
      int         key     = rng.max(0xFFFF);
      std::string key_str = std::to_string(key) + "foo";

      if (map.has(key)) {
        EXPECT_EQ(map2.count(key), 1);
        continue;
      }

      conta++;

      EXPECT_TRUE(!map.has(key));
      map.set(key, key_str);
      EXPECT_TRUE(map.has(key));

      EXPECT_EQ(map2.count(key), 0);
      map2[key] = key_str;
      EXPECT_EQ(map2.count(key), 1);
    }
  }

  EXPECT_EQ(access("lgdb_bench/mmap_map_test_sview_data", F_OK), 0);
  EXPECT_EQ(access("lgdb_bench/mmap_map_test_sview_datatxt", F_OK), 0);

  {
    mmap_lib::map<uint32_t, std::string_view> map("lgdb_bench", "mmap_map_test_sview_data");
    for (const auto &it : map) {
      auto txt1 = map.get(it);
      auto txt2 = map.get(it.first);
      auto txt3 = map.get(it);
      auto it2  = map.find(it.first);
      EXPECT_NE(it2, map.end());
      auto txt4 = map.get(it2);
      EXPECT_EQ(txt1, txt2);
      EXPECT_EQ(txt1, txt3);
      EXPECT_EQ(txt1, txt4);
      std::string_view val = map.get(it);
      EXPECT_EQ(val, std::to_string(it.first) + "foo");
      conta--;
    }
    for (const auto &it : map2) {
      EXPECT_TRUE(map.has(it.first));
    }

    EXPECT_EQ(conta, 0);

    fmt::print("load_factor:{} conflict_factor:{} txt_size:{}\n", map.load_factor(), map.conflict_factor(), map.txt_size());
  }
}

TEST_F(Setup_mmap_map_test, string_key) {
  Lrand<int> rng;

#if 0
  {
    mmap_lib::map<std::string_view, uint32_t> map;

    map.set("foo", 3);
    map.has("foo");
    map.get("foo") -> 3
    it = map.find("foo");
    map.get_key(it)

    //--------------

    // Create new string
    auto it = map.find("bar");
    if (it == map.end()) { // hello is not there
      auto it = map.set("bar", 0);
    }else{
      auto pos = it->first; // position
      auto sv = map.get_key(pstr.pos); // size of the original insertion (bar == 3) bars
      std::string_view sv2(sv.data(), pstr.size);
    }

    // pos -> string_view
    auto sv = map.get_sview(pos);

  }
#endif

  for (int n = 0; n < 4; ++n) {
    mmap_lib::map<std::string_view, uint32_t> map;
    map.clear();
    absl::flat_hash_map<std::string, uint32_t> map2;

    int conta = 0;
    for (int i = 0; i < 10000; i++) {
      int              sz     = rng.max(0xFFFFF);
      std::string      sz_str = "base" + std::to_string(sz) + "foo";
      std::string_view key{sz_str};

      if (map.has(key)) {
        EXPECT_EQ(map2.count(key), 1);
        continue;
      }

      conta++;

      EXPECT_TRUE(!map.has(key));
      map.set(key, sz);
      EXPECT_TRUE(map.has(key));

      EXPECT_EQ(map2.count(key), 0);
      map2[key] = sz;
      EXPECT_EQ(map2.count(key), 1);
    }

    for (const auto &it : map) {
      (void)it;
      EXPECT_EQ(map.get_key(it), "base" + std::to_string(it.second) + "foo");
      EXPECT_EQ(map2.count(map.get_key(it)), 1);
      conta--;
    }
    for (const auto &it : map2) {
      EXPECT_TRUE(map.has(it.first));
    }

    EXPECT_EQ(conta, 0);

    fmt::print("load_factor:{} conflict_factor:{} txt_size:{}\n", map.load_factor(), map.conflict_factor(), map.txt_size());
  }
}

TEST_F(Setup_mmap_map_test, string_key_persistance) {
  Lrand<int> rng;

  mkdir("lgdb_bench", 0755);
  int fd = open("lgdb_bench/mmap_map_test_str", O_WRONLY | O_CREAT, 0600);  // Try to create a bogus mmap
  if (fd >= 0) {
    write(fd, "bunch of garbage", strlen("bunch of garbage"));
    close(fd);
  }

  absl::flat_hash_map<std::string, uint32_t> map2;

  int conta;
  {
    mmap_lib::map<std::string_view, uint32_t> map("lgdb_bench", "mmap_map_test_str");
    map.clear();  // Clear the garbage from before

    conta = 0;
    for (int i = 0; i < 10000; i++) {
      int              sz     = rng.max(0xFFFF);
      std::string      sz_str = std::to_string(sz) + "foo";
      std::string_view key{sz_str};

      if (map.has(key)) {
        EXPECT_EQ(map2.count(key), 1);
        continue;
      }

      conta++;

      EXPECT_TRUE(!map.has(key));
      map.set(key, sz);
      EXPECT_TRUE(map.has(key));

      EXPECT_EQ(map2.count(key), 0);
      map2[key] = sz;
      EXPECT_EQ(map2.count(key), 1);
    }
  }

  {
    mmap_lib::map<std::string_view, uint32_t> map("lgdb_bench", "mmap_map_test_str");

    for (const auto &it : map) {
      (void)it;
      EXPECT_EQ(map.get_key(it), std::to_string(it.second) + "foo");
      EXPECT_EQ(map2.count(map.get_key(it)), 1);
      conta--;
    }
    for (const auto &it : map2) {
      EXPECT_TRUE(map.has(it.first));
    }

    EXPECT_EQ(conta, 0);

    fmt::print("load_factor:{} conflict_factor:{} txt_size:{}\n", map.load_factor(), map.conflict_factor(), map.txt_size());
  }
}

class Big_entry {
public:
  int  f0;
  int  f1;
  int  f2;
  int  f3;
  bool operator==(const Big_entry &o) const { return f0 == o.f0 && f1 == o.f1 && f2 == o.f2 && f3 == o.f3; }
  bool operator!=(const Big_entry &o) const { return f0 != o.f0 || f1 != o.f1 || f2 != o.f2 || f3 != o.f3; };
  Big_entry() {
    f0 = 0;
    f1 = 0;
    f2 = 0;
    f3 = 0;
  }
  Big_entry(int x) : f0(x), f1(x + 1), f2(x + 2), f3(x + 3) {}
  Big_entry(int x, int y) : f0(x), f1(y), f2(y + 1), f3(y + 2) {}
  Big_entry(const Big_entry &o) {
    f0 = o.f0;
    f1 = o.f1;
    f2 = o.f2;
    f3 = o.f3;
  }
#if 1
  Big_entry &operator=(const Big_entry &o) = delete;
  // Big_entry(const Big_entry &o) = delete;
#else
  Big_entry &operator=(const Big_entry &o) {
    f0 = o.f0;
    f1 = o.f1;
    f2 = o.f2;
    f3 = o.f3;
    return *this;
  }
#endif

  template <typename H>
  friend H AbslHashValue(H h, const Big_entry &s) {
    return H::combine(std::move(h), s.f0, s.f1, s.f2, s.f3);
  };
};

TEST_F(Setup_mmap_map_test, big_entry) {
  Lrand<int> rng;

  mmap_lib::map<uint32_t, Big_entry>       map("lgdb_bench", "mmap_map_test_se");
  absl::flat_hash_map<uint32_t, Big_entry> map2;
  auto                                     cap = map.capacity();

  for (auto n = 0; n < 1000; n++) {
    map.clear();
    map.clear();  // 2 calls to clear triggers a delete to map file

    EXPECT_EQ(map.has(33), 0);
    EXPECT_EQ(map.has(0), 0);
    map.set(0, 33);
    EXPECT_EQ(map.has(0), 1);
    EXPECT_EQ(map.has(33), 0);
    EXPECT_EQ(map.get(0).f0, 33);
    EXPECT_EQ(map.get(0).f1, 34);
    map.erase(0);
    EXPECT_EQ(map.has(0), 0);
    map.set(0, {13, 24});
    EXPECT_EQ(map.has(0), 1);
    EXPECT_EQ(map.get(0).f0, 13);
    EXPECT_EQ(map.get(0).f1, 24);
    EXPECT_EQ(map.get(0).f2, 25);

    EXPECT_EQ(map.capacity(), cap);  // No capacity degeneration
    map.erase(0);

    int conta = 0;
    for (int i = 1; i < rng.max(16); ++i) {
      int sz = rng.max(0xFFFFFF);
      if (map.find(sz) != map.end()) {
        map.erase(sz);
      } else {
        map.set(sz, {11, sz});
        EXPECT_EQ(map.has(sz), 1);
        conta++;
      }
    }

    for (const auto &it : map) {
      EXPECT_EQ(it.first, it.second.f1);
      conta--;
    }
    EXPECT_EQ(conta, 0);
  }
  map.clear();

  int conta = 0;
  for (int i = 0; i < 10000; i++) {
    int sz = rng.max(0xFFFFFF);

    if (map2.find(sz) == map2.end()) {
      EXPECT_EQ(map.has(sz), 0);
    } else {
      EXPECT_EQ(map.has(sz), 1);
    }

    map.set(sz, sz);

    if (map2.find(sz) == map2.end()) {
      conta++;
      const Big_entry &data = map.get(sz);
      map2.try_emplace(sz, data);
    }
  }

  for (const auto &it : map) {
    EXPECT_EQ(it.first + 0, it.second.f0);
    EXPECT_EQ(it.first + 1, it.second.f1);
    EXPECT_EQ(it.first + 2, it.second.f2);
    EXPECT_EQ(it.first + 3, it.second.f3);
    EXPECT_TRUE(map2[it.first] == it.second);
    conta--;
  }
  EXPECT_EQ(conta, 0);

  fmt::print("load_factor:{} conflict_factor:{}\n", map.load_factor(), map.conflict_factor());

  map.clear();
  for (const auto &it : map) {
    (void)it;
    EXPECT_TRUE(false);
  }
}

namespace mmap_lib {
template <>
struct hash<Big_entry> {
  size_t operator()(Big_entry const &o) const {
    uint32_t h = o.f0;
    h          = (h << 2) ^ o.f1;
    h          = (h << 2) ^ o.f2;
    h          = (h << 2) ^ o.f3;
    return hash<uint32_t>{}(h);
  }
};
}  // namespace mmap_lib

TEST_F(Setup_mmap_map_test, big_key) {
  Lrand<int>  rng;
  Lrand<bool> rbool;

  mmap_lib::map<Big_entry, uint32_t> map("lgdb_bench", "mmap_map_test_be");
  map.clear();  // Remove data from previous runs
  absl::flat_hash_map<Big_entry, uint32_t> map2;

  int conta = 0;
  for (int i = 0; i < 10000; i++) {
    int       sz = rng.max(0xFFFFFF);
    Big_entry key(sz);

    if (rbool.any()) {
      map.set(key, sz);
    } else {
      map.set({sz}, sz);
    }

    if (map2.find(key) == map2.end()) {
      conta++;
      map2[key] = sz;
    }
  }

  for (const auto &it : map) {
    EXPECT_EQ(it.second + 0, it.first.f0);
    EXPECT_EQ(it.second + 1, it.first.f1);
    EXPECT_EQ(it.second + 2, it.first.f2);
    EXPECT_EQ(it.second + 3, it.first.f3);
    EXPECT_TRUE(map2[it.first] == it.second);
    conta--;
  }

  EXPECT_EQ(conta, 0);

  fmt::print("load_factor:{} conflict_factor:{}\n", map.load_factor(), map.conflict_factor());
}

TEST_F(Setup_mmap_map_test, lots_of_strings) {
  const std::vector<std::string> roots = {"potato", "__t", "very_long_string", "a"};

  Lrand<int> rseed;
  auto       seed = rseed.any();
  {
    Lrand<int>                                  rng(seed);
    mmap_lib::bimap<uint32_t, std::string_view> bimap("lgdb_bench", "mmap_map_large_sview");
    bimap.clear();  // Remove data from previous runs

    for (uint32_t i = 0; i < 60'000; ++i) {
      std::string str = roots[rng.max(roots.size())];
      str             = str + ":" + std::to_string(i);

      EXPECT_FALSE(bimap.has_key(i));
      EXPECT_FALSE(bimap.has_val(str));

      bimap.set(i, str);

      EXPECT_TRUE(bimap.has_key(i));
      EXPECT_TRUE(bimap.has_val(str));

      auto str2 = bimap.get_val(i);
      auto i2   = bimap.get_key(str);

      EXPECT_EQ(str, str2);
      EXPECT_EQ(i, i2);
    }
  }

  {
    Lrand<int>                                  rng(seed);  // same seed
    mmap_lib::bimap<uint32_t, std::string_view> bimap("lgdb_bench", "mmap_map_large_sview");

    for (uint32_t i = 0; i < 60'000; ++i) {
      std::string str = roots[rng.max(roots.size())];
      str             = str + ":" + std::to_string(i);

      EXPECT_TRUE(bimap.has_key(i));
      EXPECT_TRUE(bimap.has_val(str));

      auto        str2 = bimap.get_val(i);
      const auto &i2   = bimap.get_key(str);

      EXPECT_EQ(str, str2);
      EXPECT_EQ(i, i2);
    }
  }
}

static_assert(mmap_lib::is_array_serializable<std::string_view>::value);
static_assert(mmap_lib::is_array_serializable<std::vector<int>>::value);
static_assert(!mmap_lib::is_array_serializable<uint32_t>::value);
static_assert(!mmap_lib::is_array_serializable<std::map<int, int>>::value);
static_assert(!mmap_lib::is_array_serializable<std::string>::value);

TEST_F(Setup_mmap_map_test, serializable) {}
