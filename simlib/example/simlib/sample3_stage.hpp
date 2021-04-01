#pragma once
#include "vcd_writer.hpp"
struct Sample3_stage {
  uint64_t                  hidx;
  UInt<32>                  to1_b;
  std::array<UInt<32>, 256> memory;

  uint8_t  reset_iterator;
  UInt<32> tmp;
  UInt<32> tmp2;

#ifdef SIMLIB_VCD
  // vcd::VarPtr vcd_to1_b;
  std::string     scope_name;
  vcd::VCDWriter* vcd_writer;
  //  vcd::VCDWriter* vcd_writer = vcd::initialize_vcd_writer();
  // mem part//  vcd::VarPtr vcd_memory_127 = vcd_writer->register_var(scope_name, "memory[127]", vcd::VariableType::reg, 32);
  vcd::VarPtr vcd_to1_b          = vcd_writer->register_passed_var(scope_name, "to1_b[31:0]", vcd::VariableType::wire, 32);
  vcd::VarPtr vcd_tmp            = vcd_writer->register_var(scope_name, "tmp[31:0]", vcd::VariableType::wire, 32);
  vcd::VarPtr vcd_tmp2           = vcd_writer->register_var(scope_name, "tmp2[31:0]", vcd::VariableType::wire, 32);
  vcd::VarPtr vcd_to3_d          = vcd_writer->register_passed_var(scope_name, "to3_d[31:0]", vcd::VariableType::wire, 32);
  vcd::VarPtr vcd_to3_c          = vcd_writer->register_passed_var(scope_name, "to3_c[31:0]", vcd::VariableType::wire, 32);
  vcd::VarPtr vcd_to3_cValid     = vcd_writer->register_passed_var(scope_name, "to3_cValid", vcd::VariableType::wire, 1);
  vcd::VarPtr vcd_to3_dValid     = vcd_writer->register_passed_var(scope_name, "to3_dValid", vcd::VariableType::wire, 1);
  vcd::VarPtr vcd_reset_iterator = vcd_writer->register_var(scope_name, "reset_iterator[7:0]", vcd::VariableType::integer, 8);
  vcd::VarPtr vcd_clk            = vcd_writer->register_passed_var(scope_name, "clk", vcd::VariableType::wire, 1);
  vcd::VarPtr vcd_reset          = vcd_writer->register_passed_var(scope_name, "reset", vcd::VariableType::wire, 1);

  Sample3_stage(uint64_t _hidx, const std::string& parent_name, vcd::VCDWriter* writer);
  void vcd_reset_cycle();
  void vcd_posedge();
  void vcd_negedge();
  void vcd_comb(UInt<1> s1_to3_cValid, UInt<32> s1_to3_c, UInt<1> s2_to3_dValid, UInt<32> s2_to3_d);
#else
  Sample3_stage(uint64_t _hidx);
  void reset_cycle();
  void cycle(UInt<1> s1_to3_cValid, UInt<32> s1_to3_c, UInt<1> s2_to3_dValid, UInt<32> s2_to3_d);
#endif

#ifdef SIMLIB_TRACE
  void add_signature(Simlib_signature& sign);
#endif
};
