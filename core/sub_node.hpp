//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#pragma once

// NOTE:
//
// instace_pid is the PID used inside and outside the lgraph to connect the input/output.
// Both the subgraph and the calling graph have the same PID
//
// graph_pos is the "optional" IO position needed when a module is called
// without names, just by position. Each language should specify the way to
// populate graph_pos for verilog interoperativity. E.g: alphabetical order, or
// declaration order or ??

#include "absl/container/flat_hash_map.h"
#include "absl/types/span.h"
#include "lgraph_base_core.hpp"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "tech_library.hpp"

class Sub_node {
protected:
  void add_phys_pin_int(const Port_ID instance_pid, const Tech_pin &ppin) {
    I(io_pins.size() > instance_pid);
#ifndef NDEBUG
    // Make sure not to double insert
    for (const auto &p : io_pins[instance_pid].phys) {
      I(!p.overlap(ppin));
    }
#endif
    io_pins[instance_pid].phys.emplace_back(ppin);
  }

public:
  enum class Direction { Invalid, Output, Input };

  struct Physical_cell {
    Physical_cell() : height(0), width(0) {}
    float height;
    float width;
  };

  struct IO_pin {
    IO_pin() : dir(Direction::Invalid), graph_io_pos(Port_invalid) {}
    IO_pin(std::string_view _name, Direction _dir, Port_ID _graph_io_pos) : name(_name), dir(_dir), graph_io_pos(_graph_io_pos) {}
    std::string           name;
    Direction             dir;
    Port_ID               graph_io_pos;
    std::vector<Tech_pin> phys;  // There could be more than one location per pin

    bool    is_mapped() const { return graph_io_pos != Port_invalid; }
    bool    is_input() const { return dir == Direction::Input; }
    bool    is_output() const { return dir == Direction::Output; }
    bool    is_invalid() const { return dir == Direction::Invalid; }
    bool    has_io_pos() const { return graph_io_pos != Port_invalid; }
    Port_ID get_io_pos() const { return graph_io_pos; }

    void clear() {
      dir          = Direction::Invalid;
      name         = "INVALID_PID";
      graph_io_pos = Port_invalid;
      phys.clear();
    }
  };

private:
  std::string   name;
  Lg_type_id    lgid;
  Physical_cell phys;

  std::vector<IO_pin> io_pins;

  absl::flat_hash_map<std::string, Port_ID> name2id;
  std::vector<Port_ID>                      graph_pos2instance_pid;
  std::vector<Port_ID>                      deleted;

  void map_pin_int(Port_ID instance_pid, Port_ID graph_pos) {
    I(graph_pos != Port_invalid);
    I(instance_pid);  // Must be non zero for input/output pid
    I(io_pins[instance_pid].graph_io_pos == graph_pos);

    if (graph_pos2instance_pid.size() <= graph_pos) {
      graph_pos2instance_pid.resize(graph_pos + 1, Port_invalid);
      I(graph_pos2instance_pid[graph_pos] == Port_invalid);
    } else {
      I(graph_pos2instance_pid[graph_pos] == Port_invalid || graph_pos2instance_pid[graph_pos] == instance_pid);
    }
    graph_pos2instance_pid[graph_pos] = instance_pid;
  }

public:
  Sub_node() {
    name.clear();
    expunge();
  }
  // Sub_node(const Sub_node &s) = delete;
  Sub_node &operator=(const Sub_node &) = delete;
  void      copy_from(std::string_view new_name, Lg_type_id new_lgid, const Sub_node &sub);

  void to_json(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;
  void from_json(const rapidjson::Value &entry);

  void reset_pins() {
    clear_io_pins();
    io_pins.clear();    // WARNING: Do NOT remove mappings, just port id. (allows to reload designs)
    io_pins.resize(1);  // No id ZERO
    name2id.clear();
  }

  void reset(std::string_view _name, Lg_type_id _lgid) {
    name = _name;
    lgid = _lgid;

    reset_pins();
  }

  void rename(std::string_view _name) {
    I(!is_invalid());
    I(name != _name);
    name = _name;
  }

  void expunge() {
    name2id.clear();
    io_pins.clear();
    lgid = 0;
  }

  bool is_black_box() const {
    return graph_pos2instance_pid.empty();  // BBox if we still do not know how to map from instance_pid to pos
  }

  void clear_io_pins() {
    graph_pos2instance_pid.clear();
    // io_pins.clear();   // FIXME: version?? Do NOT remove mappings, just port id. (allows to reload designs)
    // io_pins.resize(1); // No id ZERO
  }

  bool is_invalid() const { return lgid == 0; }

  Lg_type_id get_lgid() const {
    I(lgid);
    return lgid;
  }

  std::string_view get_name() const {
    I(lgid);
    return name;
  }

  Port_ID add_input_pin(std::string_view io_name, Port_ID graph_pos = Port_invalid) {
    return add_pin(io_name, Direction::Input, graph_pos);
  }

  Port_ID add_output_pin(std::string_view io_name, Port_ID graph_pos = Port_invalid) {
    return add_pin(io_name, Direction::Output, graph_pos);
  }

  Port_ID add_pin(std::string_view io_name, Direction dir, Port_ID graph_pos = Port_invalid) {
    I(lgid);
    I(!has_pin(io_name));
    Port_ID instance_pid;
    if (deleted.empty()) {
      instance_pid = io_pins.size();
      io_pins.emplace_back(io_name, dir, graph_pos);
    } else {
      instance_pid = deleted.back();
      deleted.pop_back();
      if (io_pins.size() <= instance_pid) {
        io_pins.resize(instance_pid + 1);
      }
      io_pins[instance_pid].name         = io_name;
      io_pins[instance_pid].dir          = dir;
      io_pins[instance_pid].graph_io_pos = graph_pos;
    }
    name2id[io_name] = instance_pid;
    I(io_pins[instance_pid].name == io_name);
    if (graph_pos != Port_invalid)
      map_pin_int(instance_pid, graph_pos);

    return instance_pid;
  }
  void del_pin(Port_ID instance_pid);

  Port_ID map_graph_pos(std::string_view io_name, Direction dir, Port_ID graph_pos) {
    I(graph_pos);  // 0 possition is also not used (to catch bugs)
    I(has_pin(io_name));

    Port_ID instance_pid = name2id[io_name];
    I(io_pins[instance_pid].name == io_name);
    I(io_pins[instance_pid].graph_io_pos == graph_pos || !has_graph_pos_pin(graph_pos));
    io_pins[instance_pid].dir = dir;

    if (io_pins[instance_pid].graph_io_pos != Port_invalid) {
      I(graph_pos2instance_pid.size() > io_pins[instance_pid].graph_io_pos);
      graph_pos2instance_pid[io_pins[instance_pid].graph_io_pos] = Port_invalid;
    }

    if (graph_pos != Port_invalid) {
      io_pins[instance_pid].graph_io_pos = graph_pos;
      map_pin_int(instance_pid, graph_pos);
    }

    return instance_pid;
  }

  void populate_graph_pos();

  bool has_pin(std::string_view io_name) const {
    I(lgid);
    return name2id.find(io_name) != name2id.end();
  }
  bool has_graph_pos_pin(Port_ID graph_pos) const {
    I(lgid);
    return graph_pos2instance_pid.size() > graph_pos && graph_pos2instance_pid[graph_pos] != Port_invalid;
  }
  bool has_instance_pin(Port_ID instance_pid) const {
    I(lgid);
    return io_pins.size() > instance_pid && !io_pins[instance_pid].is_invalid();
  }

  Port_ID get_instance_pid(std::string_view io_name) const {
    has_pin(io_name);
    return name2id.at(io_name);
  }

  Port_ID get_io_pos(std::string_view io_name) const {
    auto instance_pid = get_instance_pid(io_name);
    return io_pins[instance_pid].graph_io_pos;
  }

  Port_ID get_instance_pid_from_graph_pos(Port_ID graph_pos) const {
    I(has_graph_pos_pin(graph_pos));
    return graph_pos2instance_pid[graph_pos];
  }

  const IO_pin &get_io_pin_from_graph_pos(Port_ID graph_pos) const {
    I(has_graph_pos_pin(graph_pos));
    return io_pins[graph_pos2instance_pid[graph_pos]];
  }

  const IO_pin &get_io_pin_from_instance_pid(Port_ID instance_pid) const {
    if (instance_pid >= io_pins.size()) {
      return io_pins[0];  // invalid PID
    }
    return io_pins[instance_pid];
  }

  Port_ID get_io_pos_from_instance_pid(Port_ID instance_pid) const {
    I(has_instance_pin(instance_pid));
    return io_pins[instance_pid].graph_io_pos;
  }

  const IO_pin &get_pin(std::string_view io_name) const {
    I(has_pin(io_name));
    auto instance_pid = name2id.at(io_name);
    I(io_pins[instance_pid].name == io_name);
    return io_pins[instance_pid];
  }

  const IO_pin &get_graph_output_io_pin(std::string_view io_name) const {
    I(has_pin(io_name));
    auto instance_pid = name2id.at(io_name);
    I(io_pins[instance_pid].name == io_name);
    I(io_pins[instance_pid].dir == Direction::Output);
    return io_pins[instance_pid];
  }

  const IO_pin &get_graph_input_io_pin(std::string_view io_name) const {
    I(has_pin(io_name));
    auto instance_pid = name2id.at(io_name);
    I(io_pins[instance_pid].name == io_name);
    I(io_pins[instance_pid].dir == Direction::Input);
    return io_pins[instance_pid];
  }

  Port_ID get_graph_io_pos(std::string_view io_name) const {
    I(has_pin(io_name));
    auto instance_pid = name2id.at(io_name);
    I(io_pins[instance_pid].name == io_name);
    return io_pins[instance_pid].graph_io_pos;
  }

  std::string_view get_name_from_instance_pid(Port_ID instance_pid) const {
    if (!has_instance_pin(instance_pid))
      return "INVALID_PID";
    return io_pins[instance_pid].name;
  }

  std::string_view get_name_from_graph_pos(Port_ID graph_pos) const {
    I(has_graph_pos_pin(graph_pos));  // The pos does not seem to exist
    return io_pins[graph_pos2instance_pid[graph_pos]].name;
  }

  bool is_input_from_instance_pid(Port_ID instance_pid) const {
    I(has_instance_pin(instance_pid));
    return io_pins[instance_pid].dir == Direction::Input;
  }

  bool is_input_from_graph_pos(Port_ID graph_pos) const {
    I(has_graph_pos_pin(graph_pos));
    return io_pins[graph_pos2instance_pid[graph_pos]].dir == Direction::Input;
  }

  bool is_input(std::string_view io_name) const {
    if (!has_pin(io_name))
      return false;

    auto instance_pid = name2id.at(io_name);
    I(io_pins[instance_pid].name == io_name);
    return (io_pins.at(instance_pid).dir == Direction::Input);
  }

  bool is_output_from_instance_pid(Port_ID instance_pid) const {
    I(has_instance_pin(instance_pid));
    return io_pins[instance_pid].dir == Direction::Output;
  }

  bool is_output_from_graph_pos(Port_ID graph_pos) const {
    I(has_graph_pos_pin(graph_pos));
    return io_pins[graph_pos2instance_pid[graph_pos]].dir == Direction::Output;
  }

  bool is_output(std::string_view io_name) const {
    if (!has_pin(io_name))
      return false;

    auto instance_pid = name2id.at(io_name);
    I(io_pins[instance_pid].name == io_name);
    return (io_pins[instance_pid].dir == Direction::Output);
  }

  void add_phys_pin(std::string_view io_name, const Tech_pin &ppin) {
    I(has_pin(io_name));
    add_phys_pin_int(name2id.at(io_name), ppin);
  }

  void add_phys_pin_from_instance_pid(Port_ID instance_pid, const Tech_pin &ppin) {
    I(has_instance_pin(instance_pid));
    add_phys_pin_int(instance_pid, ppin);
  }

  void add_phys_pin_from_graph_pos(Port_ID graph_pos, const Tech_pin &ppin) {
    I(has_graph_pos_pin(graph_pos));
    add_phys_pin_int(graph_pos2instance_pid[graph_pos], ppin);
  }

  size_t size() const { return io_pins.size() - 1; };

  // Returns a span/vector-like array of all the pins. If the pin was deleted, there may be a pin witout name and position.
  const std::vector<const IO_pin *> get_io_pins() const {
    I(io_pins.size() >= 1);
    std::vector<const IO_pin *> v;
    for (const auto &e : io_pins) {
      if (e.is_invalid())
        continue;
      v.emplace_back(&e);
    }
    return v;
  }

  const std::vector<std::pair<const IO_pin *, Port_ID>> get_output_pins() const {
    I(io_pins.size() >= 1);
    std::vector<std::pair<const IO_pin *, Port_ID>> v;
    Port_ID                                         i = 0;
    for (const auto &e : io_pins) {
      if (e.is_output())
        v.emplace_back(&e, i);
      ++i;
    }
    return v;
  }

  const std::vector<std::pair<const IO_pin *, Port_ID>> get_input_pins() const {
    I(io_pins.size() >= 1);
    std::vector<std::pair<const IO_pin *, Port_ID>> v;
    Port_ID                                         i = 0;
    for (const auto &e : io_pins) {
      if (e.is_input())
        v.emplace_back(&e, i);
      ++i;
    }
    return v;
  }

  std::vector<IO_pin> get_sorted_io_pins() const;

  void set_phys(const Physical_cell &&cphys) {
    I(lgid);
    phys = cphys;
  }

  const Physical_cell &get_phys() const {
    I(lgid);
    return phys;
  }

  Physical_cell *ref_phys() {
    I(lgid);
    return &phys;
  }

  void dump() const;
};
