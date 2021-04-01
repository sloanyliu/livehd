//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#pragma once

#include "absl/container/flat_hash_map.h"
#include "bm.h"
#include "invariant.hpp"
#include "invariant_options.hpp"
#include "lgraph.hpp"

class Invariant_finder {
private:
  Lgraph *             elab_graph;
  Lgraph *             synth_graph;
  Invariant_boundaries boundaries;

  bool          processed;
  bm::bvector<> stack;

  typedef std::pair<Index_id, uint32_t>   Node_bit;
  absl::flat_hash_map<Node_bit, Gate_set> partial_cone_cells;  // partial_gate_count
  absl::flat_hash_map<Node_bit, Net_set>  partial_endpoints;   // sips

  // there is a delay between allocation of the cache and populating it
  absl::flat_hash_set<Node_bit> cached;

#ifndef NDEBUG
  absl::flat_hash_set<Node_bit> deleted;
#endif

  void get_topology();

  void find_invariant_boundaries();

  void propagate_until_boundary(Index_id nid, uint32_t bit_selection);
  void clear_cache(const Node_bit &entry);

public:
  Invariant_finder(Lgraph *elab, Lgraph *synth, const std::string &hier_sep = ".") : boundaries(hier_sep) {
    processed   = false;
    elab_graph  = elab;
    synth_graph = synth;
  }

  Invariant_finder(const Invariant_find_options &pack) : boundaries(pack.hierarchical_separator) {
    processed   = false;
    elab_graph  = Lgraph::open(pack.elab_lgdb, pack.top);
    synth_graph = Lgraph::open(pack.synth_lgdb, pack.top);
  }

  const Invariant_boundaries &get_boundaries() {
    if (!processed) {
      find_invariant_boundaries();
    }
    return boundaries;
  }
};
