//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#pragma once

#include <iostream>
#include <map>
#include <type_traits>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "iassert.hpp"
#include "lgedge.hpp"
#include "lgraph_base_core.hpp"
#include "mmap_vector.hpp"

class Fwd_edge_iterator;
class Bwd_edge_iterator;
class Fast_edge_iterator;
class Graph_library;

class LGraph_Base : public Lgraph_base_core {
private:
protected:
  mmap_lib::vector<Node_internal> node_internal;

  static inline constexpr std::string_view unknown_io = "unknown";
  Graph_library *                          library;

  absl::flat_hash_map<uint32_t, uint32_t> idx_insert_cache;

  Index_ID create_node_space(const Index_ID idx, const Port_ID dst_pid, const Index_ID master_nid, const Index_ID root_nid);
  Index_ID get_space_output_pin(const Index_ID idx, const Port_ID dst_pid, Index_ID &root_nid);
  Index_ID get_space_output_pin(const Index_ID master_nid, const Index_ID idx, const Port_ID dst_pid, const Index_ID root_nid);
  // Index_ID         get_space_input_pin(const Index_ID master_nid, const Index_ID idx, bool large = false);
  Index_ID create_node_int();

  void add_edge_int(Index_ID dst_nid, Port_ID dst_pid, Index_ID src_nid, Port_ID inp_pid);

  Port_ID recompute_io_ports(const Index_ID track_nid);

  Index_ID find_idx_from_pid_int(const Index_ID nid, const Port_ID pid) const;
  Index_ID find_idx_from_pid(const Index_ID nid, const Port_ID pid) const {
    if (likely(node_internal[nid].get_dst_pid() == pid)) {  // Common case
      return nid;
    }
    return find_idx_from_pid_int(nid, pid);
  }

  Index_ID setup_idx_from_pid(const Index_ID nid, const Port_ID pid);

  Index_ID get_master_nid(Index_ID idx) const { return node_internal[idx].get_master_root_nid(); }

  uint32_t get_bits(Index_ID idx) const {
    I(idx < node_internal.size());
    I(node_internal[idx].is_root());
    return node_internal[idx].get_bits();
  }

  void set_bits(Index_ID idx, uint32_t bits) {
    I(idx < node_internal.size());
    I(node_internal[idx].is_root());
    node_internal.ref(idx)->set_bits(bits);
  }

public:
  LGraph_Base() = delete;

  LGraph_Base(const LGraph_Base &) = delete;

  explicit LGraph_Base(std::string_view _path, std::string_view _name, Lg_type_id lgid) noexcept;
  virtual ~LGraph_Base();

  virtual void clear();
  virtual void sync();

  void emplace_back();

  void add_edge(const Index_ID dst_idx, const Index_ID src_idx) {
    I(src_idx < node_internal.size());
    I(node_internal[src_idx].is_root());
    I(dst_idx < node_internal.size());
    I(node_internal[dst_idx].is_root());
    I(src_idx != dst_idx);

    add_edge_int(dst_idx, node_internal[dst_idx].get_dst_pid(), src_idx, node_internal[src_idx].get_dst_pid());
  }

  void print_stats() const;

  const Node_internal &get_node_int(Index_ID idx) const {
    I(static_cast<Index_ID>(node_internal.size()) > idx);
    return node_internal[idx];
  }

  /*
  Node_internal &get_node_int(Index_ID idx) {
    I(static_cast<Index_ID>(node_internal.size()) > idx);
    return node_internal[idx];
  }
  */

  bool is_valid_node(Index_ID nid) const {
    if (nid >= node_internal.size())
      return false;
    return node_internal[nid].is_valid() && node_internal[nid].is_master_root();
  }

  bool is_valid_node_pin(Index_ID idx) const {
    if (idx >= node_internal.size())
      return false;
    return node_internal[idx].is_valid() && node_internal[idx].is_root();
  }

  Port_ID get_dst_pid(Index_ID idx) const {
    I(static_cast<Index_ID>(node_internal.size()) > idx);
    I(node_internal[idx].is_root());
    return node_internal[idx].get_dst_pid();
  }

  bool is_root(Index_ID idx) const {
    I(static_cast<Index_ID>(node_internal.size()) > idx);
    return node_internal[idx].is_root();
  }

  static size_t max_size() { return (((size_t)1) << Index_bits) - 1; }
  size_t        size() const { return node_internal.size(); }

  class _init {
  public:
    _init();
  } _static_initializer;

  const Graph_library &get_library() const { return *library; }
  Graph_library *      ref_library() const { return library; }

  static void error_int(std::string_view text);
  static void warn_int(std::string_view text);
  static void info_int(std::string_view text);

  template <typename S, typename... Args>
  static void error(const S& format, Args&&... args) {
    error_int(fmt::format(format, args...));
  }

  template <typename S, typename... Args>
  static void warn(const S& format, Args&&... args) {
    warn_int(fmt::format(format, args...));
  }

  template <typename S, typename... Args>
  static void info(const S& format, Args&&... args) {
    info_int(fmt::format(format, args...));
  }
};
