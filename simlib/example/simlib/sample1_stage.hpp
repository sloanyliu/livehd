#pragma once
#include "vcd_writer.hpp"

struct Sample1_stage {
  uint64_t hidx;  // lgraph hierarchy index

  UInt<1>  to2_aValid;
  UInt<32> to2_a;
  UInt<32> to2_b;

  UInt<1>  to3_cValid;
  UInt<32> to3_c;

  UInt<32> tmp;

#ifdef SIMLIB_VCD
  std::string     scope_name;
  vcd::VCDWriter* vcd_writer;
  //  vcd::VCDWriter* vcd_writer = vcd::initialize_vcd_writer();
  vcd::VarPtr vcd_clk        = vcd_writer->register_passed_var(scope_name, "clk", vcd::VariableType::wire, 1);
  vcd::VarPtr vcd_reset      = vcd_writer->register_passed_var(scope_name, "reset", vcd::VariableType::wire, 1);
  vcd::VarPtr vcd_to1_aValid = vcd_writer->register_passed_var(scope_name, "to1_aValid", vcd::VariableType::wire, 1);
  vcd::VarPtr vcd_to1_a      = vcd_writer->register_passed_var(scope_name, "to1_a[31:0]", vcd::VariableType::wire, 32);
  vcd::VarPtr vcd_tmp        = vcd_writer->register_var(scope_name, "tmp[31:0]", vcd::VariableType::wire, 32);
  vcd::VarPtr vcd_to1_b      = vcd_writer->register_passed_var(scope_name, "to1_b[31:0]", vcd::VariableType::wire, 32);
  vcd::VarPtr vcd_to2_aValid = vcd_writer->register_passed_var(scope_name, "to2_aValid", vcd::VariableType::wire, 1);
  vcd::VarPtr vcd_to2_a      = vcd_writer->register_passed_var(scope_name, "to2_a[31:0]", vcd::VariableType::wire, 32);
  vcd::VarPtr vcd_to2_b      = vcd_writer->register_passed_var(scope_name, "to2_b[31:0]", vcd::VariableType::wire, 32);
  vcd::VarPtr vcd_to3_cValid = vcd_writer->register_passed_var(scope_name, "to3_cValid", vcd::VariableType::wire, 1);
  vcd::VarPtr vcd_to3_c      = vcd_writer->register_passed_var(scope_name, "to3_c[31:0]", vcd::VariableType::wire, 32);
  Sample1_stage(uint64_t _hidx, const std::string& parent_name, vcd::VCDWriter* writer);
  void vcd_reset_cycle();
  void vcd_posedge();
  void vcd_negedge();
  void vcd_comb(UInt<32> s3_to1_b, UInt<1> s2_to1_aValid, UInt<32> s2_to1_a);
#else
  Sample1_stage(uint64_t _hidx);
  void reset_cycle();
  void cycle(UInt<32> s3_to1_b, UInt<1> s2_to1_aValid, UInt<32> s2_to1_a);
#endif
#ifdef SIMLIB_TRACE
  void add_signature(Simlib_signature& sign);
#endif
};
