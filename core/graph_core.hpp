//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#pragma once

#include <cassert>
#include <string_view>
#include <vector>

#include "lgraph_base_core.hpp"

class Graph_core;

class Index_iter {
protected:
  Graph_core *gc;

public:
  class Fast_iter {
  private:
    Graph_core *gc;
    uint32_t    id;
    // May need to add extra data here

  public:
    constexpr Fast_iter(Graph_core *_gc, uint32_t _id) : gc(_gc), id(_id) {}
    constexpr Fast_iter(const Fast_iter &it) : gc(it.gc), id(it.id) {}

    constexpr Fast_iter &operator=(const Fast_iter &it) {
      gc = it.gc;
      id = it.id;
      return *this;
    }

    Fast_iter &operator++();  // call Graph_core::xx if needed

    constexpr bool operator!=(const Fast_iter &other) const {
      assert(gc == other.gc);
      return id != other.id;
    }
    constexpr bool operator==(const Fast_iter &other) const {
      assert(gc == other.gc);
      return id == other.id;
    }

    constexpr uint32_t operator*() const { return id; }
  };

  Index_iter() = delete;
  explicit Index_iter(Graph_core *_gc) : gc(_gc) {}

  Fast_iter begin() const;                            // Find first elemnt in Graph_core
  Fast_iter end() const { return Fast_iter(gc, 0); }  // More likely 0 ID for end
};

class Graph_core {
protected:
  class __attribute__((packed)) Overflow_entry {
  protected:
    void extract_all(uint32_t self_id, std::vector<uint32_t> &expanded);
  public:
    Overflow_entry() { clear(); }
    void clear() {
      bzero(this, sizeof(Overflow_entry));  // set zero everything
      overflow_node = 1;
    }

    void readjust_edges(uint32_t overflow_id, std::vector<uint32_t> &pending_inp, std::vector<uint32_t> &pending_out);

    static inline constexpr size_t sedge0_size=11;
    static inline constexpr size_t sedge1_size=12;
    static inline constexpr size_t max_edges=sedge0_size+sedge1_size+3;

    // BEGIN cache line
    uint8_t overflow_node : 1;     // Overflow or not overflow node
    uint8_t master_root_node : 1;  // master_root or just master
    uint8_t inputs : 1;
    uint8_t sedge0_full:1; // not used now
    uint8_t sedge1_full:1; // not used now
    uint8_t padding:3;

    uint8_t n_edges;   // 0 to 18 (50 in compress)

    int16_t  sedge0[sedge0_size]; // between ledge_min..ledge1

    // 4 byte aligned
    uint32_t overflow_next_id;
    uint32_t ledge_min;

    // 2nd half cache line (bytes 32:63)
    uint32_t ledge1;
    uint32_t ledge_max;

    int16_t  sedge1[sedge1_size]; // between ledge1..ledge_max

    uint32_t get_overflow_id() const { return overflow_next_id; }
  };

  class __attribute__((packed)) Master_entry {  // AKA master or master_root entry
  public:
    static inline constexpr size_t Num_sedges=6;

    // CTRL: Byte 0:1
    uint8_t overflow_node : 1;     // Overflow or not overflow node
    uint8_t master_root_node : 1;  // master_root or just master
    uint16_t inp_mask : 9;          // are the edges input or output edges
    uint8_t n_outputs : 4;
    uint8_t overflow_link : 1;  // When set, ledge_min points to overflow
    // CTRL: Byte 2
    uint8_t lpid_or_type;  // type in master_root, pid bits in master
    // CTRL: Byte 3:5
    uint32_t bits : 24;
    // SEDGE: Byte 6:7 (special case)
    int16_t sedge2_or_pid;  // edge_store in master_root, 16 pid bits in master
    // LEDGE: Byte  8:11
    uint32_t ledge0_or_prev;  // prev pointer (only for master)
    // LEDGE: Byte 12:15
    uint32_t ledge1_or_overflow;
    // LEDGE: Byte 16:19
    uint32_t next_ptr;  // next pointer (master or master_root)
    // SEDGE: Byte 20:31
    int16_t sedge[Num_sedges];

    Master_entry() { clear(); }

    void clear() {
      bzero(this, sizeof(Master_entry));  // set zero everything
    }

    void readjust_edges(uint32_t self_id, std::vector<uint32_t> &pending_inp, std::vector<uint32_t> &pending_out);
    bool insert_sedge(int16_t rel_id, bool out);
    bool insert_ledge(uint32_t id, bool out);
    bool delete_edge(uint32_t self_id, uint32_t id);

    bool is_master() const { return !master_root_node; }
    bool is_master_root() const { return master_root_node; }

    void set_overflow(uint32_t oid) {
      I(!overflow_node);
      overflow_node = 1;
      I(ledge1_or_overflow == 0);
      ledge1_or_overflow = oid;
    }

    Bits_t get_bits() const { return bits; }
    void   set_bits(Bits_t b) { bits = b; }

    uint8_t get_type() const { return lpid_or_type; }
    void    set_type(uint8_t type) { lpid_or_type = type; }

    constexpr uint32_t get_overflow() const;  // returns the next Entry64 if overflow, zero otherwise

    uint32_t remove_free_next() {
      auto tmp       = ledge0_or_prev;
      ledge0_or_prev = 0;
      return tmp;
    }
    void insert_free_next(uint32_t ptr) {
      clear();
      ledge0_or_prev = ptr;
    }

    uint32_t get_prev_ptr() const {
      I(is_master());
      return ledge0_or_prev;
    }

    bool has_edges() const { return overflow_link || n_outputs || inp_mask; }

    bool     has_overflow() const { return overflow_link; }
    uint32_t get_overflow_id() const {
      if (overflow_link)
        return ledge1_or_overflow;
      return 0;
    }

    void set_pid(const Port_ID pid) {
      I(!master_root_node);
      lpid_or_type  = static_cast<uint8_t>(pid);  // 8bits
      sedge2_or_pid = pid >> 8;                   // upper 16bits
    }

    uint32_t get_pid() const {
      if (is_master_root())
        return 0;

      uint32_t pid = sedge2_or_pid;
      pid <<= 8;
      pid |= lpid_or_type;

      return pid;
    }
  };

  std::vector<Master_entry> table;  // to be replaced by mmap_lib::vector once it works

  uint32_t free_master_id;
  uint32_t free_overflow_id;

  uint32_t allocate_overflow();

  void add_edge_int(uint32_t self_id, uint32_t other_id, bool out);
  void del_edge_int(uint32_t self_id, uint32_t other_id, bool out);

public:
  Graph_core(std::string_view path, std::string_view name);

  uint8_t get_type(uint32_t id) const {
    I(id && id < table.size());
    I(table[id].is_master_root());
    return table[id].get_type();
  }

  void set_type(uint32_t id, uint8_t type) {
    I(id && id < table.size());
    I(table[id].is_master_root());
    table[id].set_type(type);
  }

  Port_ID get_pid(uint32_t id) const {
    I(!is_invalid(id));
    return table[id].get_pid();
  }

  void set_bits(uint32_t id, Bits_t bits) {
    I(id && id < table.size());
    table[id].set_bits(bits);
  }
  Bits_t get_bits(uint32_t id) {
    I(id && id < table.size());
    return table[id].get_bits();
  }

  bool is_invalid(uint32_t id) const {
    if (id == 0 || table.size()<=id)
      return true;

    return table[id].overflow_node; // overflow set in deleted nodes
  }

  bool is_master_root(uint32_t id) const {
    I(id && id < table.size());
    return table[id].is_master_root();
  }
  bool is_master(uint32_t id) const {
    I(id && id < table.size());
    return table[id].is_master();
  }

  uint32_t create_master_root();
  uint32_t create_master(uint32_t master_root_id, const Port_ID pid);

  uint32_t get_master_root(uint32_t id) {
    I(!is_invalid(id));

    if (table[id].is_master_root())
      return id;

    return table[id].get_prev_ptr();
  }

  void add_edge(uint32_t driver_id, uint32_t sink_id) {
    add_edge_int(driver_id, sink_id, true);
    add_edge_int(sink_id, driver_id, false);
  }

  void del_edge(uint32_t driver_id, uint32_t sink_id) {
    del_edge_int(driver_id, sink_id, true);
    del_edge_int(sink_id, driver_id, false);
  }

  // Make sure that this methods have "c++ copy elision" (strict rules in return)
  const std::vector<uint32_t> get_setup_drivers(uint32_t master_root_id) const;  // the drivers set for master_root_id
  const std::vector<uint32_t> get_setup_sinks(uint32_t master_root_id) const;    // the sinks set for master_root_id

  // unlike the const iterator, it should allow to delete edges/nodes while
  uint32_t fast_next(uint32_t start);

  Index_iter out_ids(uint32_t id);  // Iterate over the out edges of s (*it is uint32_t)
  Index_iter inp_ids(uint32_t id);  // Iterate over the inp edges of s

  // Delete edges and node itself (master or master_root. If master_root, delete masters too)
  void del(uint32_t id);
  // Delete edges (master or master_root)
  void del_edges(uint32_t id);
};
