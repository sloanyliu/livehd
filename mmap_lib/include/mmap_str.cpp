//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

#include "mmap_str.hpp"

template<int map_id> 
mmap_lib::vector<int> mmap_lib::str<map_id>::string_vector2("lgdb/str_use", "strVector");

template<int map_id>
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str<map_id>::string_map2("lgdb/str_use", "strMap");

template<int map_id>
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str<map_id>::string_map3("lgdb/str_use", "strMap2");


//static_assert(sizeof(mmap_lib::str)==16);


#if 0
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str::string_map2("lgdb/str_use", "strMap");

mmap_lib::vector<int> mmap_lib::str::string_vector2("lgdb/str_use", "strVector");
#elif 0
template<int map_id>
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str<map_id>::string_map2("lgdb/str_use", "strMap");

template<int map_id>
mmap_lib::vector<int> mmap_lib::str<map_id>::string_vector2("lgdb/str_use", "strVector");
#endif

#if 0
template<size_t map_id>
std::array<mmap_lib::map<std::string_view, uint32_t>,4> mmap_lib::str<map_id>::string_deck = {
  mmap_lib::map<std::string_view, uint32_t>(),
  mmap_lib::map<std::string_view, uint32_t>("lgdb/str_use", "strMap2"),
  mmap_lib::map<std::string_view, uint32_t>("lgdb/str_use", "strMap3"),
  mmap_lib::map<std::string_view, uint32_t>("lgdb/str_use", "strMap4")
};
#elif 0
template<int map_id>
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str<map_id>::map0;

template<int map_id>
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str<map_id>::map1("lgdb/str_use", "strMap1");

template<int map_id>
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str<map_id>::map2("lgdb/str_use", "strMap2"); 

template<int map_id>
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str<map_id>::map3("lgdb/str_use", "strMap3"); 
#endif

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
