// This file is distributed under the BSD 3-Clause License. See LICENSE for details.

#pragma once

#include "lgedgeiter.hpp"
#include "lgraph.hpp"
#include "lgraphbase.hpp"
#include "lnast.hpp"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"

class Label_synth {
private:
  const bool verbose;
  const bool hier;
	bool synth;

  int last_free_id;
  int collapse_set_min;

  absl::flat_hash_set<int>                     collapse_set;
  absl::flat_hash_map<Node::Compact_flat, int> flat_node2id; //<node, color>
  absl::flat_hash_map<int, int>                flat_merges;  //<color, node_ID>or<node_ID, color>

  int  get_free_id();
  void set_id(const Node &node, int id);
  void collapse_merge(int dst);

  void mark_ids(Lgraph *g);
  void merge_ids();

public:

  /* takes in an Lgraph, the one we color
   * first run mark_ids(g) on input Lgraph 
   *   -> fill flat_node2id<node, int>
   *   -> also run set_id() -> fills flat_merges<> 
   * then run merge_ids() 
   *   -> collapses flat_merges -> fills collapse_set<>
   *   -> reassign the colors in flat_node2id<> based on flat_merges<>
   * clear the color (if there is any color)
   * then color the nodes based on the color 
   *   flat_node2id<node id, color>
   */
  void label(Lgraph *g);

  Label_synth(bool _verbose, bool _hier, std::string_view alg);

  void dump() const;
};
