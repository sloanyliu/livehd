//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#pragma once

#include "absl/container/flat_hash_set.h"
//#include "absl/container/inlined_vector.h"

#include "cell.hpp"
#include "hierarchy.hpp"
#include "lconst.hpp"
#include "lgraph_base_core.hpp"
#include "mmap_map.hpp"
#include "node_pin.hpp"
#include "sub_node.hpp"

class Ann_place;
class Ann_dim;
using Node_iterator = std::vector<Node>;

class Node {
protected:
  LGraph *        top_g;
  mutable LGraph *current_g;
  Hierarchy_index hidx;
  Index_ID        nid;

  friend class LGraph;
  friend class LGraph_Node_Type;
  friend class Node_pin;
  friend class XEdge;
  friend class Fast_edge_iterator;
  friend class Flow_base_iterator;
  friend class Fwd_edge_iterator;
  friend class Bwd_edge_iterator;
  friend class Hierarchy_tree;

  constexpr Node(LGraph *_g, LGraph *_c_g, const Hierarchy_index &_hidx, Index_ID _nid)
      : top_g(_g), current_g(_c_g), hidx(_hidx), nid(_nid) {
    assert(nid);
    assert(top_g);
    assert(current_g);
  }

  void invalidate(LGraph *_g);
  void invalidate();
  void update(Index_ID _nid) { nid = _nid; }
  void update(const Hierarchy_index &_hidx, Index_ID _nid);

public:
  class __attribute__((packed)) Compact {
  protected:
    Hierarchy_index hidx;
    uint64_t        nid : Index_bits;

    friend class LGraph;
    friend class LGraph_Node_Type;
    friend class Node;
    friend class Node_pin;
    friend class XEdge;
    friend class Fast_edge_iterator;
    friend class Flow_base_iterator;
    friend class Fwd_edge_iterator;
    friend class Bwd_edge_iterator;
    friend class mmap_lib::hash<Compact>;

  public:
    constexpr Compact(const Hierarchy_index &_hidx, Index_ID _nid) : hidx(_hidx), nid(_nid) { assert(nid); };
    constexpr Compact() : nid(0){};

    constexpr Index_ID get_nid() const { return nid; }  // Mostly for debugging or to know order

    constexpr Hierarchy_index get_hidx() const {
      I(!hidx.is_invalid());
      return hidx;
    }

    // Can not be constexpr find current_g
    Node get_node(LGraph *lg) const { return Node(lg, *this); }

    constexpr bool is_invalid() const { return nid == 0; }

    constexpr bool operator==(const Compact &other) const {
      return nid == other.nid && (hidx == other.hidx || hidx.is_invalid() || other.hidx.is_invalid());
    }
    constexpr bool operator!=(const Compact &other) const { return !(*this == other); }

    template <typename H>
    friend H AbslHashValue(H h, const Compact &s) {
      return H::combine(std::move(h), s.hidx.get_hash(), s.nid);
    };
  };

  class __attribute__((packed)) Compact_class {
  protected:
    uint64_t nid : Index_bits;

    friend class LGraph;
    friend class LGraph_Node_Type;
    friend class Node;
    friend class Node_pin;
    friend class XEdge;
    friend class Fast_edge_iterator;
    friend class Flow_base_iterator;
    friend class Fwd_edge_iterator;
    friend class Bwd_edge_iterator;
    friend class Hierarchy_tree;
    friend class mmap_lib::hash<Compact_class>;

  public:
    // constexpr operator size_t() const { return nid; }
    constexpr Compact_class() : nid(0){};  // needed for mmap_tree which allocates empty data

    constexpr Compact_class(const Index_ID &_nid) : nid(_nid){};

    constexpr Node get_node(LGraph *lg) const { return Node(lg, *this); }

    constexpr Index_ID get_nid() const { return nid; }
    constexpr bool     is_invalid() const { return nid == 0; }

    constexpr bool operator==(const Compact_class &other) const { return nid == other.nid; }
    constexpr bool operator!=(const Compact_class &other) const { return nid != other.nid; }

    template <typename H>
    friend H AbslHashValue(H h, const Compact_class &s) {
      return H::combine(std::move(h), s.nid);
    };
  };

  void update(const Hierarchy_index &_hidx);

  template <typename H>
  friend H AbslHashValue(H h, const Node &s) {
    return H::combine(std::move(h), (int)s.hidx.get_hash(), (int)s.nid);  // Ignore lgraph pointer in hash
  };

  // NOTE: No operator<() needed for std::set std::map to avoid their use. Use flat_map_set for speed
  void update(LGraph *_g, const Node::Compact &comp);
  void update(const Node::Compact &comp);
  void update(const Node &node);

  constexpr Node() : top_g(nullptr), current_g(nullptr), nid(0) {}

  Node(LGraph *_g, const Compact &comp) { update(_g, comp); }
  Node(LGraph *_g, const Hierarchy_index &_hidx, const Compact_class &comp);
  constexpr Node(LGraph *_g, const Compact_class &comp)
      : top_g(_g), current_g(nullptr), hidx(Hierarchy_tree::invalid_index()), nid(comp.nid) {
    I(nid);
    I(top_g);

    current_g = top_g;
  }
#if 0
  Node &operator=(const Node &obj) {
    I(this != &obj); // Do not assign object to itself. works but wastefull
    top_g     = obj.top_g;
    current_g = obj.current_g;
    const_cast<Index_ID&>(nid)     = obj.nid;
    const_cast<Hierarchy_index&>(hidx) = obj.hidx;

    return *this;
  };
#endif

  inline Compact get_compact() const { return Compact(hidx, nid); }

  inline Compact_class get_compact_class() const {
    // OK to pick a hierarchical to avoid replication of info like names
    return Compact_class(nid);
  }

  LGraph *get_top_lgraph() const { return top_g; }
  LGraph *get_class_lgraph() const { return current_g; }
  LGraph *get_lg() const { return current_g; }  // To handle hierarchical API

  Index_ID        get_nid() const { return nid; }
  Hierarchy_index get_hidx() const { return hidx; }

  Node_pin get_driver_pin() const {
    I(!Ntype::is_multi_driver(get_type_op()));
    Node_pin pin(top_g, current_g, hidx, nid, 0, false);
    return pin;
  }
  Node_pin get_sink_pin() const {
    I(!Ntype::is_multi_sink(get_type_op()));
    return Node_pin(top_g, current_g, hidx, nid, 0, true);
  }

  Node_pin get_driver_pin_raw(Port_ID pid) const;
  Node_pin get_sink_pin_raw(Port_ID pid) const;

  Node_pin get_driver_pin_slow(std::string_view pname) const;
  Node_pin get_driver_pin(std::string_view pname) const {
    assert(pname.size());
    if (unlikely(is_type_sub())) {
      return get_driver_pin_slow(pname);
    }
    auto pid = Ntype::get_driver_pid(get_type_op(), pname);
    if (pid)
      return get_driver_pin_raw(pid);
    return Node_pin(top_g, current_g, hidx, nid, 0, false);  // could be invalid if not setup
  }
  Node_pin get_sink_pin_slow(std::string_view pname) const;
  Node_pin get_sink_pin(std::string_view pname) const {
    assert(pname.size());
    if (unlikely(is_type_sub())) {
      return get_sink_pin_slow(pname);
    }
    auto pid = Ntype::get_sink_pid(get_type_op(), pname);
    if (pid)
      return get_sink_pin_raw(pid);
    return Node_pin(top_g, current_g, hidx, nid, 0, true);  // could be invalid if not setup
  }
  Node_pin setup_driver_pin_slow(std::string_view name) const;
  Node_pin setup_driver_pin(std::string_view pname) const {
    assert(pname.size());
    if (unlikely(is_type_sub())) {
      return setup_driver_pin_slow(pname);
    }
    auto pid = Ntype::get_driver_pid(get_type_op(), pname);
    if (pid)
      return setup_driver_pin_raw(pid);
    return Node_pin(top_g, current_g, hidx, nid, 0, false);
  }
  Node_pin setup_driver_pin_raw(Port_ID pid) const;
  Node_pin setup_driver_pin() const;

  Node_pin setup_sink_pin_slow(std::string_view name);
  Node_pin setup_sink_pin(std::string_view pname) {
    assert(pname.size());
    if (unlikely(is_type_sub())) {
      return setup_sink_pin_slow(pname);
    }
    auto pid = Ntype::get_sink_pid(get_type_op(), pname);
    if (pid)
      return setup_sink_pin_raw(pid);
    return Node_pin(top_g, current_g, hidx, nid, 0, true);  // could be invalid if not setup
  }

  Node_pin setup_sink_pin_raw(Port_ID pid);
  Node_pin setup_sink_pin() const;

  bool has_inputs() const;
  bool has_outputs() const;
  int  get_num_inp_edges() const;
  int  get_num_out_edges() const;
  int  get_num_edges() const;

  constexpr bool is_invalid() const { return nid == 0; }
  constexpr bool is_down_node() const { return top_g != current_g; }
  constexpr bool is_hierarchical() const { return !hidx.is_invalid(); }
  Node           get_non_hierarchical() const;

  constexpr bool operator==(const Node &other) const {
    GI(nid == 0, hidx.is_invalid());
    GI(other.nid == 0, other.hidx.is_invalid());
    GI(nid && other.nid, top_g == other.top_g);

    return nid == other.nid && (hidx == other.hidx || hidx.is_invalid() || other.hidx.is_invalid());
  }
  constexpr bool operator!=(const Node &other) const { return !(*this == other); }

  void   set_type_lut(const Lconst &lutid);
  Lconst get_type_lut() const;

  std::string_view get_type_name() const;
  Ntype_op         get_type_op() const;
  void             set_type(const Ntype_op op);
  void             set_type(const Ntype_op op, Bits_t bits);
  bool             is_type(const Ntype_op op) const;
  bool             is_type_sub() const { return get_type_op() == Ntype_op::Sub; }
  bool             is_type_synth() const { return Ntype::is_synthesizable(get_type_op()); }
  bool             is_type_const() const;
  bool             is_type_attr() const;
  bool             is_type_flop() const;
  bool             is_type_tup() const;
  bool             is_type_io() const { return nid == Hardcoded_input_nid || nid == Hardcoded_output_nid; }
  bool             is_type_loop_breaker() const;

  Hierarchy_index hierarchy_go_down() const;
  Hierarchy_index hierarchy_go_up() const;
  Node            get_up_node() const;
  bool            is_root() const;

  void            set_type_sub(Lg_type_id subid);
  Lg_type_id      get_type_sub() const;
  const Sub_node &get_type_sub_node() const;
  Sub_node *      ref_type_sub_node() const;
  LGraph *        ref_type_sub_lgraph() const;  // Slower than other get_type_sub
  bool            is_type_sub_present() const;

  Lconst get_type_const() const;

  void connect_sink(const Node &n2) const { setup_sink_pin().connect_driver(n2.setup_driver_pin()); }
  void connect_driver(const Node &n2) const { setup_driver_pin().connect_sink(n2.setup_sink_pin()); }

  void connect_sink(const Node_pin &dpin) const { setup_sink_pin().connect_driver(dpin); }
  void connect_driver(const Node_pin &spin) const { setup_driver_pin().connect_sink(spin); }

  void nuke();  // Delete all the pins, edges, and attributes of this node

  bool is_sink_connected(std::string_view v) const;
  bool is_driver_connected(std::string_view v) const;

  Node_pin_iterator out_connected_pins() const;
  Node_pin_iterator inp_connected_pins() const;

  XEdge_iterator out_edges() const;
  XEdge_iterator inp_edges() const;

  XEdge_iterator out_edges_ordered() const;  // Slower than inp_edges, but edges ordered by driver.pid
  XEdge_iterator inp_edges_ordered() const;  // Slower than inp_edges, but edges ordered by sink.pid

  XEdge_iterator out_edges_ordered_reverse() const;  // Slower than inp_edges, but edges ordered by driver.pid
  XEdge_iterator inp_edges_ordered_reverse() const;  // Slower than inp_edges, but edges ordered by sink.pid

  Node_pin_iterator inp_drivers() const;

  bool is_graph_io() const { return nid == Hardcoded_input_nid || nid == Hardcoded_output_nid; }
  bool is_graph_input() const { return nid == Hardcoded_input_nid; }
  bool is_graph_output() const { return nid == Hardcoded_output_nid; }

  void del_node();

  // BEGIN ATTRIBUTE ACCESSORS
  std::string debug_name() const;
  std::string default_instance_name() const;

  // user-defined node instance name (1 per node instance)
  void             set_instance_name(std::string_view iname);
  std::string_view get_instance_name() const;
  bool             has_instance_name() const;

  // non-hierarchical node name (1 for all nodes)
  void             set_name(std::string_view iname);
  std::string_view get_name() const;
  std::string_view create_name() const;
  bool             has_name() const;

  void             set_place(const Ann_place &p);
  const Ann_place &get_place() const;
  Ann_place *      ref_place();
  bool             has_place() const;

  Bits_t get_bits() const;

  void set_color(int color);
  int  get_color() const;
  bool has_color() const;
  // END ATTRIBUTE ACCESSORS

  void dump();
};

namespace mmap_lib {
template <>
struct hash<Node::Compact> {
  constexpr size_t operator()(Node::Compact const &o) const {
    uint64_t h = o.nid;
    h          = (h << 12) ^ o.hidx.get_hash() ^ o.nid;
    return hash<uint64_t>{}(h);
  }
};

template <>
struct hash<Node::Compact_class> {
  constexpr size_t operator()(Node::Compact_class const &o) const { return hash<uint32_t>{}(o.nid); }
};
}  // namespace mmap_lib
