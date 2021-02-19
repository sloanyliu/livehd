// This file is distributed under the BSD 3-Clause License. See LICENSE for details.

#pragma once

#include <string>

#include "lgraph.hpp"

class Cgen_verilog {
private:
  const bool        verbose;
  const std::string odir;

  using pin2str_type = absl::flat_hash_map<Node_pin::Compact_class, std::string>;

  pin2str_type pin2expr;
  pin2str_type pin2var;
  absl::flat_hash_map<Node::Compact_class, std::string> mux2vector;

  inline static std::string get_scaped_name(const std::string &name) {
    for(auto ch:name) {
      if (!std::isalnum(ch) && ch != '_')
        return std::string("\\") + name + " ";
    }
    return name;
  }

  static std::string get_scaped_name(std::string_view wire_name) {
    std::string name{wire_name};
    return get_scaped_name(name);
  }

  std::string get_append_to_name(const std::string &name, std::string_view ext) const;
  std::string get_expression(const Node_pin &dpin) const;
  std::string get_expression(const Node_pin &&dpin) const {
    return get_expression(dpin);
  }
  void add_expression(std::string &txt_seq, std::string_view txt_op, Node_pin &dpin) const;

  void process_flop(std::string &buffer, Node &node);
  void process_mux(std::string &buffer, Node &node);
  void process_simple_node(std::string &buffer, Node &node);

  void create_module_io(std::string &buffer, LGraph *lg);
  void create_subs(std::string &buffer, LGraph *lg);
  void create_combinational(std::string &buffer, LGraph *lg);
  void create_outputs(std::string &buffer, LGraph *lg);
  void create_registers(std::string &buffer, LGraph *lg);
  void create_locals(std::string &buffer, LGraph *lg);

  std::tuple<std::string, int> setup_file(LGraph *lg) const;
  void append_to_file(const std::string &filename, int fd, const std::string &buffer) const;

public:
  void do_from_lgraph(LGraph *lg_parent);

  Cgen_verilog(bool _verbose, std::string_view _odir);
};