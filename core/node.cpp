//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

#include "node.hpp"

#include <charconv>

#include "annotate.hpp"
#include "lgedgeiter.hpp"
#include "lgraph.hpp"

void Node::invalidate(LGraph *_g) {
  top_g     = _g;
  current_g = _g;
  hidx.invalidate();
  nid = 0;
}

void Node::invalidate() {
  current_g = top_g;
  hidx.invalidate();
  nid = 0;
}

void Node::update(const Hierarchy_index &_hidx, Index_ID _nid) {
  if (_hidx != hidx) {
    current_g = top_g->ref_htree()->ref_lgraph(_hidx);
    hidx      = _hidx;
  }
  nid = _nid;
  I(current_g->is_valid_node(nid));
}

void Node::update(const Hierarchy_index &_hidx) {
  I(_hidx != hidx);
  current_g = top_g->ref_htree()->ref_lgraph(_hidx);
  hidx      = _hidx;

  nid = current_g->fast_first();
  I(!nid.is_invalid());  // No update call if it is an empty graph

  I(current_g->is_valid_node(nid));
}

void Node::update(const Node &node) {
  top_g     = node.top_g;
  current_g = node.current_g;
  hidx      = node.hidx;
  nid       = node.nid;
}

void Node::update(LGraph *_g, const Node::Compact &comp) {
  I(comp.nid);
  I(_g);

  nid = comp.nid;
  if (top_g == nullptr) {
    top_g = _g;
    hidx  = comp.hidx;
  } else if (hidx == comp.hidx && _g == top_g) {
    return;
  }

  top_g = _g;
  hidx  = comp.hidx;
  if (hidx.is_root() || hidx.is_invalid()) {  // invalid->no hierarchy
    current_g = top_g;
    return;
  }
  current_g = top_g->ref_htree()->ref_lgraph(hidx);

  I(current_g->is_valid_node(nid));
}

void Node::update(const Node::Compact &comp) {
  I(!comp.hidx.is_invalid());
  I(comp.nid);
  I(top_g);

  nid = comp.nid;
  if (hidx == comp.hidx)
    return;
  hidx      = comp.hidx;
  current_g = top_g->ref_htree()->ref_lgraph(hidx);

  I(current_g->is_valid_node(nid));
}

Node::Node(LGraph *_g, const Hierarchy_index &_hidx, const Compact_class &comp)
    : top_g(_g), current_g(0), hidx(_hidx), nid(comp.nid) {
  I(nid);
  I(top_g);
  if (hidx.is_root() || hidx.is_invalid())
    current_g = top_g;
  else
    current_g = top_g->ref_htree()->ref_lgraph(hidx);

  I(current_g->is_valid_node(nid));
  // I(top_g->get_hierarchy_class_lgid(hidx) == current_g->get_lgid());
}

Node_pin Node::get_driver_pin_raw(Port_ID pid) const {
  I(!is_type_sub());  // Do not setup subs by PID, use name. IF your really need it, use setup_driver_pin_raw
  I(Ntype::has_driver(get_type_op(), pid));
  Index_ID idx = current_g->find_idx_from_pid(nid, pid);
  // It can be zero, then invalid node_pin
  return Node_pin(top_g, current_g, hidx, idx, pid, false);
}

Node_pin Node::get_sink_pin_raw(Port_ID pid) const {
  I(!is_type_sub());  // Do not setup subs by PID, use name. IF your really need it, use setup_driver_pin_raw
  I(Ntype::has_sink(get_type_op(), pid));
  Index_ID idx = current_g->find_idx_from_pid(nid, pid);
  // It can be zero, then invalid node_pin
  return Node_pin(top_g, current_g, hidx, idx, pid, true);
}

Node_pin Node::get_driver_pin_slow(std::string_view pname) const {
  I(is_type_sub());

  Lg_type_id sub_lgid = current_g->get_type_sub(nid);
  I(current_g->get_library().exists(sub_lgid));  // Must be a valid lgid

  const auto &sub = current_g->get_library().get_sub(sub_lgid);
  I(sub.has_pin(pname));
  I(sub.is_output(pname));

  auto pid = sub.get_instance_pid(pname);
  I(pid != Port_invalid);  // graph_pos must be valid if connected

  Index_ID idx = current_g->setup_idx_from_pid(nid, pid);  // WARNING: setup because Sub can delay the connection
  return Node_pin(top_g, current_g, hidx, idx, pid, false);
}

Node_pin Node::get_sink_pin_slow(std::string_view pname) const {
  I(is_type_sub());

  Lg_type_id sub_lgid = current_g->get_type_sub(nid);
  I(current_g->get_library().exists(sub_lgid));  // Must be a valid lgid

  const auto &sub = current_g->get_library().get_sub(sub_lgid);
  I(sub.has_pin(pname));
  I(sub.is_input(pname));

  auto pid = sub.get_instance_pid(pname);
  I(pid != Port_invalid);  // graph_pos must be valid if connected

  Index_ID idx = current_g->setup_idx_from_pid(nid, pid);  // WARNING: setup because Sub can delay the connection
  return Node_pin(top_g, current_g, hidx, idx, pid, true);
}

Node_pin Node::setup_driver_pin_slow(std::string_view name) const {
  I(is_type_sub());

  Lg_type_id sub_lgid = current_g->get_type_sub(nid);
  I(current_g->get_library().exists(sub_lgid));  // Must be a valid lgid

  const auto &sub = current_g->get_library().get_sub(sub_lgid);
  I(sub.has_pin(name));  // maybe you forgot an add_graph_input/output in the sub?
  I(sub.is_output(name));

  auto pid = sub.get_instance_pid(name);
  I(pid != Port_invalid);  // graph_pos must be valid if connected

  Index_ID idx = current_g->setup_idx_from_pid(nid, pid);
  return Node_pin(top_g, current_g, hidx, idx, pid, false);
}

bool Node::is_sink_connected(std::string_view pname) const {
  if (!is_type_sub()) {
    auto pid = Ntype::get_sink_pid(get_type_op(), pname);
    I(pid >= 0);  // if quering a cell, the name should be right, no?
    Index_ID idx = get_lg()->find_idx_from_pid(nid, pid);
    if (idx == 0)
      return false;
    return get_lg()->has_inputs(Node_pin(top_g, current_g, hidx, idx, pid, true));
  }

  Lg_type_id sub_lgid = current_g->get_type_sub(nid);

  const auto &sub = current_g->get_library().get_sub(sub_lgid);
  if (!sub.has_pin(pname) || !sub.is_input(pname))
    return false;

  auto pid = sub.get_instance_pid(pname);
  if (pid == Port_invalid)
    return false;

  Index_ID idx = get_lg()->find_idx_from_pid(nid, pid);
  if (idx == 0)
    return false;

  return get_lg()->has_inputs(Node_pin(top_g, current_g, hidx, idx, pid, true));
}

bool Node::is_driver_connected(std::string_view pname) const {
  if (!is_type_sub()) {
    auto pid = Ntype::get_driver_pid(get_type_op(), pname);
    I(pid >= 0);  // if quering a cell, the name should be right, no?
    Index_ID idx = get_lg()->find_idx_from_pid(nid, pid);
    if (idx == 0)
      return false;
    return get_lg()->has_outputs(Node_pin(top_g, current_g, hidx, idx, pid, false));
  }

  Lg_type_id sub_lgid = current_g->get_type_sub(nid);

  const auto &sub = current_g->get_library().get_sub(sub_lgid);
  if (!sub.has_pin(pname) || sub.is_input(pname))
    return false;

  auto pid = sub.get_instance_pid(pname);
  if (pid == Port_invalid)
    return false;

  Index_ID idx = get_lg()->find_idx_from_pid(nid, pid);
  if (idx == 0)
    return false;

  return get_lg()->has_inputs(Node_pin(top_g, current_g, hidx, idx, pid, false));
}

Node_pin Node::setup_sink_pin_slow(std::string_view name) {
  I(is_type_sub());

  Lg_type_id sub_lgid = current_g->get_type_sub(nid);
  I(current_g->get_library().exists(sub_lgid));  // Must be a valid lgid

  const auto &sub = current_g->get_library().get_sub(sub_lgid);
  I(sub.has_pin(name));  // maybe you forgot an add_graph_input/output in the sub?
  if (sub.is_output(name))
    return Node_pin();

  Port_ID pid;

  if (std::isdigit(name[0])) {
    int pos = 0;
    std::from_chars(name.data(), name.data() + name.size(), pos);

    if (!sub.has_instance_pin(pos))
      return Node_pin();  // invalid pin

    auto io_pin = sub.get_io_pin_from_graph_pos(pos);
    if (io_pin.dir == Sub_node::Direction::Output) {
      return Node_pin();  // invalid pin
    }

    pid = sub.get_instance_pid(io_pin.name);
  } else {
    pid = sub.get_instance_pid(name);
    if (pid == Port_invalid)
      return Node_pin();
  }

  I(pid != Port_invalid);  // graph_pos must be valid if connected

  Index_ID idx = current_g->setup_idx_from_pid(nid, pid);
  return Node_pin(top_g, current_g, hidx, idx, pid, true);
}

Node_pin Node::setup_sink_pin_raw(Port_ID pid) {
#ifndef NDEBUG
  if (is_type_sub()) {
    Lg_type_id  sub_lgid = current_g->get_type_sub(nid);
    const auto &sub      = current_g->get_library().get_sub(sub_lgid);
    I(sub.has_instance_pin(pid));
  } else {
    I(Ntype::has_sink(get_type_op(), pid));
  }
#endif

  Index_ID idx = current_g->setup_idx_from_pid(nid, pid);
  return Node_pin(top_g, current_g, hidx, idx, pid, true);
}

Node_pin Node::setup_sink_pin() const {
  I(!Ntype::is_multi_sink(get_type_op()));
  return Node_pin(top_g, current_g, hidx, nid, 0, true);
}

bool Node::has_inputs() const { return current_g->has_inputs(*this); }
bool Node::has_outputs() const { return current_g->has_outputs(*this); }

int Node::get_num_inp_edges() const { return current_g->get_num_inp_edges(*this); }
int Node::get_num_out_edges() const { return current_g->get_num_out_edges(*this); }
int Node::get_num_edges() const { return current_g->get_num_edges(*this); }

Node Node::get_non_hierarchical() const { return Node(current_g, current_g, Hierarchy_tree::invalid_index(), nid); }

Node_pin Node::setup_driver_pin_raw(Port_ID pid) const {
#ifndef NDEBUG
  if (is_type_sub()) {
    Lg_type_id  sub_lgid = current_g->get_type_sub(nid);
    const auto &sub      = current_g->get_library().get_sub(sub_lgid);
    I(sub.has_instance_pin(pid));
    I(sub.is_output_from_instance_pid(pid), "ERROR: An input can not be a driver pin");
  } else {
    I(Ntype::has_driver(get_type_op(), pid));
  }
#endif

  Index_ID idx = current_g->setup_idx_from_pid(nid, pid);
  return Node_pin(top_g, current_g, hidx, idx, pid, false);
}

Node_pin Node::setup_driver_pin() const {
  I(!Ntype::is_multi_driver(get_type_op()));
  return Node_pin(top_g, current_g, hidx, nid, 0, false);
}

Ntype_op         Node::get_type_op() const { return current_g->get_type_op(nid); }
std::string_view Node::get_type_name() const { return Ntype::get_name(current_g->get_type_op(nid)); }

void Node::set_type(const Ntype_op op) {
  I(op != Ntype_op::Sub && op != Ntype_op::Const && op != Ntype_op::LUT);  // do not set type directly, call set_type_const ....
  current_g->set_type(nid, op);
}

void Node::set_type(const Ntype_op op, Bits_t bits) {
  current_g->set_type(nid, op);

  I(!Ntype::is_multi_driver(op));  // bits only possible when the cell has a single output

  setup_driver_pin().set_bits(bits);
}

bool Node::is_type(const Ntype_op op) const { return get_type_op() == op; }

bool Node::is_type_const() const { return current_g->is_type_const(nid); }

bool Node::is_type_attr() const {
  auto op = get_type_op();

  return op == Ntype_op::AttrGet || op == Ntype_op::AttrSet;
}

bool Node::is_type_flop() const {
  auto op = get_type_op();

  return op == Ntype_op::Flop || op == Ntype_op::Fflop;
}

bool Node::is_type_tup() const {
  auto op = get_type_op();

  return op == Ntype_op::TupAdd || op == Ntype_op::TupGet;
}

bool Node::is_type_loop_breaker() const {
  auto op = get_type_op();
  if (op == Ntype_op::Sub) {
    /* //FIXME->sh: delet __fir stuff after the two lg->fast() lgraph firmap clone is working */
    /* const auto sub_name = get_type_sub_node().get_name(); */
    /* if (sub_name.substr(0, 5) == "__fir") */
    /*   return false; */
    return true;
  }

  return Ntype::is_loop_breaker(op);
}

Hierarchy_index Node::hierarchy_go_down() const {
  I(current_g->is_sub(nid));
  return top_g->ref_htree()->go_down(*this);
}

Hierarchy_index Node::hierarchy_go_up() const {
  I(current_g != top_g);
  return top_g->ref_htree()->go_up(*this);
}

bool Node::is_root() const {
  bool ans = top_g == current_g;
  return ans;
}

Node Node::get_up_node() const {
  I(!is_root());
  I(!hidx.is_invalid());
  auto up_node = top_g->ref_htree()->get_instance_up_node(hidx);

  return up_node;
}

void Node::set_type_sub(Lg_type_id subid) { current_g->set_type_sub(nid, subid); }

Lg_type_id Node::get_type_sub() const { return current_g->get_type_sub(nid); }

LGraph *Node::ref_type_sub_lgraph() const {
  auto lgid = current_g->get_type_sub(nid);
  return LGraph::open(top_g->get_path(), lgid);
}

bool Node::is_type_sub_present() const {
  if (!is_type_sub())
    return false;

  auto *sub_lg = ref_type_sub_lgraph();
  if (sub_lg)
    return !sub_lg->is_empty();

  return false;
}

void Node::set_type_lut(const Lconst &lutid) { current_g->set_type_lut(nid, lutid); }

Lconst Node::get_type_lut() const { return current_g->get_type_lut(nid); }

const Sub_node &Node::get_type_sub_node() const { return current_g->get_type_sub_node(nid); }

Sub_node *Node::ref_type_sub_node() const { return current_g->ref_type_sub_node(nid); }

Lconst Node::get_type_const() const { return current_g->get_type_const(nid); }

void Node::nuke() {
  I(false);  // TODO:
}

XEdge_iterator Node::inp_edges() const { return current_g->inp_edges(*this); }

XEdge_iterator Node::out_edges() const { return current_g->out_edges(*this); }

XEdge_iterator Node::inp_edges_ordered() const { return current_g->inp_edges_ordered(*this); }

XEdge_iterator Node::out_edges_ordered() const { return current_g->out_edges_ordered(*this); }

XEdge_iterator Node::inp_edges_ordered_reverse() const { return current_g->inp_edges_ordered_reverse(*this); }

XEdge_iterator Node::out_edges_ordered_reverse() const { return current_g->out_edges_ordered_reverse(*this); }

Node_pin_iterator Node::inp_connected_pins() const { return current_g->inp_connected_pins(*this); }
Node_pin_iterator Node::out_connected_pins() const { return current_g->out_connected_pins(*this); }

Node_pin_iterator Node::inp_drivers() const { return current_g->inp_drivers(*this); }

void Node::del_node() {
  current_g->del_node(*this);
  nid = 0;  // invalidate node after delete
}

void Node::set_name(std::string_view iname) { Ann_node_name::ref(current_g)->set(get_compact_class(), iname); }

void Node::set_instance_name(std::string_view iname) { Ann_inst_name::ref(current_g)->set(get_compact(), iname); }

std::string_view Node::get_instance_name() const { return Ann_inst_name::ref(current_g)->get(get_compact()); }

bool Node::has_instance_name() const { return Ann_inst_name::ref(current_g)->has(get_compact()); }

std::string Node::default_instance_name() const {
  std::string name{"i"};

  if (is_hierarchical()) {
    absl::StrAppend(&name, "_lg", current_g->get_name(), "_hidx", std::to_string(hidx.level), "_", std::to_string(hidx.pos));
  }

  if (has_name()) {
    absl::StrAppend(&name, get_name());
    return name;
  }

  absl::StrAppend(&name, "_nid", std::to_string(nid));

  return name;
}

std::string_view Node::create_name() const {
  auto *     ref = Ann_node_name::ref(current_g);
  const auto it  = ref->find(get_compact_class());
  if (it != ref->end())
    return ref->get_val(it);

  auto        cell_name = Ntype::get_name(get_type_op());
  std::string sig       = absl::StrCat("lg_", cell_name, std::to_string(nid));
  const auto  it2       = ref->set(get_compact_class(), sig);
  return ref->get_val(it2);
#if 0
  // FIXME: HERE. Does not scale for large designs (too much recursion)

  if (get_type_op() == GraphIO_Op) {
    absl::StrAppend(&signature, "_io");
	}else if (get_type_op() == SubGraph_Op) {
    absl::StrAppend(&signature, "_", get_type_sub_node().get_name());
  }

  for(const auto &e:inp_edges()) {
    absl::StrAppend(&signature, "_", e.driver.create_name());
  }

  auto nod = Ann_node_name::find(current_g, signature);
  if (nod.is_invalid()) {
    return Ann_node_name::set(*this, signature);
  }

  absl::StrAppend(&signature,"_", std::to_string(nid)); // OK, add to stop trying

  I(Ann_node_name::find(current_g, signature).is_invalid());

  return Ann_node_name::set(*this, signature);
#endif
}

std::string_view Node::get_name() const { return Ann_node_name::ref(current_g)->get_val(get_compact_class()); }

std::string Node::debug_name() const {
#ifndef NDEBUG
  static uint16_t conta = 8192;
  if (conta++ == 0) {
    fmt::print("WARNING: Node::debug_name should not be called during release (Slowww!)\n");
  }
#endif
  if (nid == 0) {  // legal for invalid node/pins
    return "invalid_node";
  }
  I(current_g);

  auto *      ref = Ann_node_name::ref(current_g);
  std::string name;
  const auto  it = ref->find(get_compact_class());
  if (it != ref->end()) {
    name = ref->get_val(it);
  }

  if (is_type_sub()) {
    if (name.find(get_type_sub_node().get_name()) == std::string::npos) {  // filter out unnecessary module name
      absl::StrAppend(&name, get_type_sub_node().get_name());
    }
  }

  auto cell_name = Ntype::get_name(get_type_op());
  if (name.empty())
    return absl::StrCat("n", std::to_string(nid), "_", cell_name, "_lg", current_g->get_name());
  return absl::StrCat("n", std::to_string(nid), "_", cell_name, "_", name, "_lg", current_g->get_name());
}

bool Node::has_name() const { return Ann_node_name::ref(current_g)->has_key(get_compact_class()); }

void Node::set_place(const Ann_place &p) { Ann_node_place::ref(top_g)->set(get_compact(), p); }

const Ann_place &Node::get_place() const {
  auto *data = Ann_node_place::ref(top_g)->ref(get_compact());
  I(data);
  return *data;
}

Ann_place *Node::ref_place() {
  auto *ref = Ann_node_place::ref(top_g);

  auto it = ref->find(get_compact());
  if (it != ref->end()) {
    return ref->ref(it);
  }

  auto it2 = ref->set(get_compact(), Ann_place());  // Empty
  return ref->ref(it2);
}

Bits_t Node::get_bits() const {
  I(!Ntype::is_multi_driver(get_type_op()));
  return current_g->get_bits(nid);
}

bool Node::has_place() const { return Ann_node_place::ref(top_g)->has(get_compact()); }

//----- Subject to changes in the future:
enum { WHITE = 0, GREY, BLACK };
void Node::set_color(int new_color) { Ann_node_color::ref(current_g)->set(get_compact_class(), std::to_string(new_color)); }

int Node::get_color() const {
  auto str = Ann_node_color::ref(current_g)->get_val(get_compact_class());
  int  color;
  auto ok = absl::SimpleAtoi(str, &color);
  I(ok);
  return color;
}

bool Node::has_color() const { return Ann_node_color::ref(current_g)->has_key(get_compact_class()); }

// LCOV_EXCL_START
void Node::dump() {
  fmt::print("nid: {} type: {} lgraph: {} ", nid, get_type_name(), current_g->get_name());
  if (get_type_op() == Ntype_op::LUT) {
    fmt::print(" lut = {}\n", get_type_lut().to_pyrope());
  } else if (get_type_op() == Ntype_op::Const) {
    fmt::print(" const = {}\n", get_type_const().to_pyrope());
  } else {
    fmt::print("\n");
  }
  for (const auto &edge : inp_edges()) {
    fmt::print("  inp bits: {:<3} pid: {:<2} name: {:<30} <- nid: {} idx: {} pid: {:<2} name: {}\n",
               edge.get_bits(),
               edge.sink.get_pid(),
               edge.sink.debug_name(),
               edge.driver.get_node().nid,
               edge.driver.get_idx(),
               edge.driver.get_pid(),
               edge.driver.debug_name());
  }
  for (const auto &edge : out_edges()) {
    fmt::print("  out bits: {:<3} pid: {:<2} name: {:<30} -> nid: {} idx: {} pid: {:<2} name: {}\n",
               edge.get_bits(),
               edge.driver.get_pid(),
               edge.driver.debug_name(),
               edge.sink.get_node().nid,
               edge.sink.get_idx(),
               edge.sink.get_pid(),
               edge.sink.debug_name());
  }
}
// LCOV_EXCL_STOP
//
