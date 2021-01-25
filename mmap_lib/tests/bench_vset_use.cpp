//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

#include "fmt/format.h"
#include "iassert.hpp"
#include <vector>
#include "lrand.hpp"
#include "lbench.hpp"
#include "flat_hash_map.hpp"
#include <type_traits>
#include "mmap_vset.hpp"
#include "mmap_map.hpp"

#define BIG_BENCH 400
#define SMALL_BENCH 400


/*
 * Creates a vset from mmap_lib namespace
 * And tests it a little
 */
void mmap_vset(int max, std::string_view name) {
  Lrand<int> rng;

  std::string type_test("mmap_vset ");
	if (name.empty()) { type_test += "(effemeral)"; }
	else { type_test += "(persistent)"; }

	Lbench b(type_test);
	mmap_lib::vset<uint32_t, uint32_t> set("lgdb_bench", name);
	//mmap_lib::map<uint32_t, uint64_t> map("lgdb_bench", name);

	set.clear();
  set.wht();

  int one, four, ten, twothree, ninenine;
  one = 1;
  four = 4;
  ten = 10;
  twothree = 23;
  ninenine = 99;
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 0)
  
  set.insert(one);
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 1)
  set.erase(one);
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 0)

  set.insert(four);
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 4)
  set.erase(four);
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 0)
  
  set.insert(ten);
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 10)
  set.erase(ten);
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 00)
  
  set.insert(twothree);
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 23)
  set.erase(twothree);
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 00)
  
  set.insert(ninenine);
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 99)
  set.erase(ninenine);
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 00)
  //===================================

  /*
  //Testing vIter declaration:
  // mmap_lib::vset<uint32_t, uint32_t>::vIter itt;
  // itt.iter_test() 

  //Messing around with vIter functions and using them in vset functions
  //set.test_begin(); // vset print w/ vIter func
  //auto foo = set.test_ret(); // returns a vIter
  //foo.iter_test(); // vIter print w/ vIter func
  auto bar = set.begin(); // vset using vIter funcs that call vset funcs
  std::cout << "value of vIter is: "<< bar.iter_val() << std::endl; // should be 14

  set.insert(0);                                                    // insert 0
  bar = set.begin();                                                // getting vset begin() (expect 0)
  std::cout << "value of vIter is: "<< bar.iter_val() << std::endl; // print vIter val (expect 0)
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 0)

  set.erase(0);                                                     // erase val (0)
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 0)
  set.insert(9);                                                    // insert (9) 
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 9)
  bar = set.begin();                                                // getting vset begin() (expect 9)
  std::cout << "value of vIter is: "<< bar.iter_val() << std::endl; // print vIter val (expect 9)
 
  ++bar;                                                            // ++vIter (9 -> 10)
  std::cout << "value of vIter is: "<< bar.iter_val() << std::endl; // print vIter val (expect 10)
  
  bar++;                                                            // vIter++ (10 -> 11)
  std::cout << "value of vIter is: "<< bar.iter_val() << std::endl; // print vIter val (expect 11)
  std::cout << "max is: " << set.get_max() << std::endl;            // print vset max (expect 11)
  */
  
  //=========================

  /*

	int conta = 0;

	// counting
	std::cout << "Counting...  ";
	for (auto it = set.bucket_begin(), end = set.bucket_end(); it != end; ++it) {
		auto hold = set.bucket_get_val(it);
		for (auto j = 0; j < set.bucket_size(); ++j) {
			if ((hold & 1) == 1) { conta++; }
			hold = hold >> 1;
		}
	}

	std::cout << "Initial conta: " << conta << "\n\n";

	std::cout << "Inserting 0 - " << (SMALL_BENCH-1) << " into vset...\n";
	for(int i = 0; i < SMALL_BENCH; ++i) { set.insert(i); }

	std::cout << "Counting...  ";
	for (auto it = set.bucket_begin(), end = set.bucket_end(); it != end; ++it) {
		auto hold = set.bucket_get_val(it);
		for (auto j = 0; j < set.bucket_size(); ++j) {
			if ((hold & 1) == 1) { conta++; }
			hold = hold >> 1;
		}
	}
	std::cout << "After insert conta: " << conta << "\n\n";

	conta = 0; // reset

	std::cout << "Erasing 0 - " << (SMALL_BENCH-1) << " from vset...\n";
	for(int i = 0; i < SMALL_BENCH; ++i) { set.erase(i); }

	std::cout << "Counting...  ";
	for (auto it = set.bucket_begin(), end = set.bucket_end(); it != end; ++it) {
		auto hold = set.bucket_get_val(it);
		for (auto j = 0; j < set.bucket_size(); ++j) {
			if ((hold & 1) == 1) { conta++; }
			hold = hold >> 1;
		}
	}
	std::cout << "After erase conta: " << conta << std::endl;

	conta = 0; // reset

	std::cout << "sanity check (printing bucket_len): " << set.bucket_size() << std::endl;

	std::cout << "===== vset BENCH =====" << std::endl;

	std::cout << "===== Insert/Erase 0-299\n";
	for(int i = 0; i < SMALL_BENCH; ++i) { set.insert(i); }
	for(int i = 0; i < SMALL_BENCH; ++i) { set.erase(i); }

	std::cout << "===== Insert/Erase Dense\n";
	// INSERT/ERASE DENSE TEST
	// runs about (BIG * SMALL) times
	// each run:
	//   --> generate rand num, insert that num into the map
	//   --> generate another rand num, erase that num from the map
	//   --> generate another rand num, if this num is not the end of the map, erase
  for (int n = 1; n < BIG_BENCH; ++n) {
    for (int i = 0; i < SMALL_BENCH; ++i) {
      auto pos = rng.max(max);
      set.insert(pos);

      pos = rng.max(max);
			set.erase(pos);

      pos = rng.max(max);

      set.erase(pos);
    }
  }

  b.sample("insert/erase dense");
  conta = 0;


	std::cout << "===== Traversal Sparse\n";
	// TRAVERSAL SPARSE TEST
	// runs (200) times
	// each run:
	//   --> go through the whole map and increment a variable per map index
	//   --> insert a random number into map
	//   --> generate a random num, if num is not end of map, erase it
  for (int i = 0; i < SMALL_BENCH; ++i) {
		for (auto it = set.bucket_begin(), end = set.bucket_end(); it != end; ++it) {
			auto hold = set.bucket_get_val(it);
			for (auto j = 0; j < 32; ++j) {
				if ((hold & 1) == 1) { conta++; }
				hold = hold >> 1;
			}
		}
    set.insert(rng.max(max));

    auto pos = rng.max(max);
    set.erase(pos);
  }
  b.sample("traversal sparse");

  printf("inserts random %d\n",conta);
  conta = 0;

	std::cout << "===== Traversal Dense\n";
	// TRAVERSAL DENSE TEST
	// --> First, for every num in domain of max, generate 4 random nums and erase them from map
	// --> runs (200) times -> each time go through whole map and incrememt number per map index
  for (int i = 0; i < max; ++i) {
    set.erase(rng.max(max));
    set.erase(rng.max(max));
    set.erase(rng.max(max));
    set.erase(rng.max(max));
  }

  for (int i = 0; i < SMALL_BENCH; ++i) {
		for (auto it = set.bucket_begin(), end = set.bucket_end(); it != end; ++it) {
			auto hold = set.bucket_get_val(it);
			for (auto j = 0; j < 32; ++j) {
				if ((hold & 1) == 1) { conta++; }
				hold = hold >> 1;
			}
		}
  }
  b.sample("traversal dense");
  printf("inserts random %d\n",conta);
*/

}




int main(int argc, char **argv) {

  bool run_mmap_vset = false;

	if (argc>1) {
    if (strcasecmp(argv[1],"vset") == 0) {
      run_mmap_vset = true;
		}
  } else {
    run_mmap_vset = true;
  }

  const std::vector<int> nums = {10000, 100000};

  for(auto i:nums) {
    if (run_mmap_vset) {
      mmap_vset(i, "bench_map_use_mmap.data");
		}
  }

  return 0;
}

