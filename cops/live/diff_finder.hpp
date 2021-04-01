//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#pragma once

#include "invariant.hpp"
#include "lgraph.hpp"
#include "live_common.hpp"
#include "live_options.hpp"

using namespace Live;

class Diff_finder {
private:
  const Lgraph *        original;
  const Lgraph *        synth;
  Invariant_boundaries *boundaries;

  std::string hier_sep;

  std::map<Graph_Node, std::set<Graph_Node>> cones;
  std::map<Graph_Node, bool>                 different;
  std::map<Graph_Node, std::set<Graph_Node>> endpoints;
  std::set<Graph_Node>                       stack;
  std::map<Graph_Node, Net_ID>               synth_map;
  std::map<Graph_Node, std::string>          bound2net;

  using Name2graph_type = absl::flat_hash_map<std::string, Lgraph *>;

  std::set<Graph_Node> fwd_visited;

  bool is_user_def(Lgraph *current, Index_id idx, Port_ID pid) const;
  bool set_invariant(Graph_Node node);

  void find_fwd_boundaries(Graph_Node &start_boundary, std::set<Graph_Node> &discovered, bool went_up = false);
  bool compare_cone(const Graph_Node &start_boundary, const Graph_Node &original_boundary, bool went_up = false);

  auto go_up(const Graph_Node &boundary);
  auto go_down(const Graph_Node &boundary, bool output);

  void generate_modules(std::set<Graph_Node> &different_nodes, const std::string &out_lgdb);

  void add_ios_up(Lgraph *module, const Node_pin &pin, Name2graph_type &name2graph);

public:
  // FIXME: can we remove the dependency on the synthesized graph?
  Diff_finder(Lgraph *original, Lgraph *synth, Invariant_boundaries *boundaries, const std::string &hier_sep = ".")
      : original(original), synth(synth), boundaries(boundaries), hier_sep(hier_sep) {}

  Diff_finder(Live_pass_options pack);

  void generate_delta(const std::string &mod_lgdb, const std::string &out_lgdb, std::set<Net_ID> &diffs);
};
