//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#pragma once

// External package includes
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/substitute.h"
#include "kernel/rtlil.h"
#include "kernel/yosys.h"

#pragma GCC diagnostic pop
// LiveHD includes

#include "annotate.hpp"
#include "inou.hpp"
#include "lgraph.hpp"
#include "pass.hpp"

USING_YOSYS_NAMESPACE
// PRIVATE_NAMESPACE_BEGIN

class Lgyosys_dump : public Inou {
private:
  RTLIL::Design *design;
  RTLIL::Wire *  get_wire(const Node_pin &pin);
  RTLIL::Wire *  add_wire(RTLIL::Module *module, const Node_pin &pin);

  void to_yosys(Lgraph *g);

  absl::flat_hash_set<std::string_view>                            created_sub;
  absl::flat_hash_map<Node_pin::Compact, RTLIL::Wire *>            input_map;
  absl::flat_hash_map<Node_pin::Compact, RTLIL::Wire *>            output_map;
  absl::flat_hash_map<Node_pin::Compact, RTLIL::Wire *>            cell_output_map;
  absl::flat_hash_map<Node::Compact, std::vector<RTLIL::SigChunk>> mem_output_map;
  absl::flat_hash_set<RTLIL::Wire *>                               unsigned_wire;

  uint64_t ids = 0;

  bool hierarchy;

  std::string unique_name(Lgraph *g, const std::string &test) {
    std::string tmp;
    assert(test.size() >= 1);
    if (test[0] == '\\')
      tmp = &test[1];
    else
      tmp = test;

    while (true) {
      tmp = absl::StrCat(test, "_", std::to_string(ids++));

      if (Ann_node_pin_name::ref(g)->has_val(tmp))
        continue;
      if (Ann_node_name::ref(g)->has_val(tmp))
        continue;

      return tmp;
    }
  }

  RTLIL::IdString next_id(Lgraph *lg) { return RTLIL::IdString(absl::StrCat("\\", unique_name(lg, "lg"))); }

  // FIXME: any way of merging these two?
  typedef RTLIL::Cell *(RTLIL::Module::*add_cell_fnc_sign)(RTLIL::IdString, const RTLIL::SigSpec &, const RTLIL::SigSpec &,
                                                           const RTLIL::SigSpec &, bool, const std::string &);
  typedef RTLIL::Cell *(RTLIL::Module::*add_cell_fnc)(RTLIL::IdString, const RTLIL::SigSpec &, const RTLIL::SigSpec &,
                                                      const RTLIL::SigSpec &, const std::string &);

  RTLIL::Wire *create_tree(Lgraph *g, const std::vector<RTLIL::Wire *> &wires, RTLIL::Module *mod, add_cell_fnc_sign add_cell,
                           bool sign, RTLIL::Wire *result_wire, int width = 0);

  RTLIL::Wire *create_io_wire(const Node_pin &pin, RTLIL::Module *module, Port_ID pos);
  void         create_wires(Lgraph *g, RTLIL::Module *module);

  void create_blackbox(const Sub_node &sub, RTLIL::Design *design);
  void create_subgraph_outputs(Lgraph *g, RTLIL::Module *module, Node &node);
  void create_subgraph(Lgraph *g, RTLIL::Module *module, Node &node);
  void create_memory(Lgraph *g, RTLIL::Module *module, Node &node);

  RTLIL::Wire *zero_extend_one_bit(Lgraph *g, RTLIL::Module *module, RTLIL::Wire *w);

protected:
public:
  Lgyosys_dump(RTLIL::Design *d, bool hier = false) : design(d) { hierarchy = hier; };

  void fromlg(std::vector<Lgraph *> &out) final {
    for (const auto &g : out) {
      if (!g) {
        ::Pass::warn("null lgraph (ignoring)");
        continue;
      }
      std::cout << g->get_name() << std::endl;
      to_yosys(g);
    }
  }

  std::vector<Lgraph *> tolg() final {
    assert(false);
    std::vector<Lgraph *> empty;
    return empty;  // to avoid warning
  };

  void set(const std::string &key, const std::string &value) final {
    (void)key;
    (void)value;

    assert(false);  // No main_api interface for this module as it uses yosys plug-ins
  };
};
