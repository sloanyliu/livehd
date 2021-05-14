//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

#include "mmap_str.hpp"

//static_assert(sizeof(mmap_lib::str)==16);

#define TEMP_PERSIST 1

#ifdef TEMP_PERSIST
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str::string_map2("lgdb", "strMap");
mmap_lib::vector<int> mmap_lib::str::string_vector2("lgdb/mmap_vector", "global_str_vector");

#else
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str::string_map2;
mmap_lib::vector<int> mmap_lib::str::string_vector2;

#endif

mmap_lib::map<std::string_view, uint32_t> mmap_lib::str::map_one();
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str::map_two("lgdb/mmap_strMap1", "lnast_strMap");
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str::map_three("lgdb/mmap_strMap2", "lgraph_strMap"); 
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str::map_four("lgdb/mmap_strMap3", "other_strMap"); 

#if 0
std::array<mmap_lib::map<std::string_view, uint32_t>,4> mmap_lib::str::string_map2;

string_map2[0] is no disk saved (same as now)
string_map2[1] "lgdb/mmap_str1" // LNAST
string_map2[2] "lgdb/mmap_str2" // LGRaph
string_map2[3] "lgdb/mmap_str3" // Other

operator==() {
  string_map[0] != string_map[0]
  string_map[0] != string_map[1] {
    get_sv() != get_sv();
  }

#endif
