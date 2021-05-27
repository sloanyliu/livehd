//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

#include "mmap_str.hpp"

/*
template <int map_id>
mmap_lib::vector<int> mmap_lib::str<map_id>::vec0;

template <int map_id>
mmap_lib::vector<int> mmap_lib::str<map_id>::vec1("lgdb/str_use", "strV1");

template <int map_id>
mmap_lib::vector<int> mmap_lib::str<map_id>::vec2("lgdb/str_use", "strV2");

template <int map_id>
mmap_lib::vector<int> mmap_lib::str<map_id>::vec3("lgdb/str_use", "strV3");
*/


template <int map_id>
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str<map_id>::m0;

template <int map_id>
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str<map_id>::m1("lgdb/str_use", "strMap1");

template <int map_id>
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str<map_id>::m2("lgdb/str_use", "strMap2");

template <int map_id>
mmap_lib::map<std::string_view, uint32_t> mmap_lib::str<map_id>::m3("lgdb/str_use", "strMap3");

template class mmap_lib::str<0>;
template class mmap_lib::str<1>;
template class mmap_lib::str<2>;
template class mmap_lib::str<3>;
