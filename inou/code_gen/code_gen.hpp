#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "code_gen_all_lang.hpp"
#include "inou_code_gen.hpp"
#include "lnast.hpp"
#include "lnast_generic_parser.hpp"

class Code_gen {
protected:
  // Lnast *top;
  std::shared_ptr<Lnast>                  lnast;
  std::string_view                        path;
  std::string_view                        odir;
  std::string                             buffer_to_print = "";
  std::map<std::string, std::string> ref_map;
  // enum class Code_gen_type { Type_verilog, Type_prp, Type_cfg, Type_cpp };
private:
  std::unique_ptr<Code_gen_all_lang> lnast_to;
  int                                indendation = 0;
  std::string                        indent();
  std::vector<std::string_view>      const_vect;

public:
  Code_gen(Inou_code_gen::Code_gen_type code_gen_type, std::shared_ptr<Lnast> _lnast, std::string_view _path,
           std::string_view _odir);
  // virtual void generate() = 0;
  void        generate();
  void        do_stmts(const mmap_lib::Tree_index& stmt_node_index);
  void        do_assign(const mmap_lib::Tree_index& assign_node_index, std::vector<std::string>& hier_tup_vec, bool hier_tup_assign = false);
  void        do_for(const mmap_lib::Tree_index& assign_node_index);
  void        do_while(const mmap_lib::Tree_index& assign_node_index);
  void        do_op(const mmap_lib::Tree_index& op_node_index, const std::string& op_type);
  void        do_dot(const mmap_lib::Tree_index& dot_node_index, const std::string& select_type);
  void        do_if(const mmap_lib::Tree_index& dot_node_index);
  void        do_cond(const mmap_lib::Tree_index& cond_node_index);
  void        do_tuple(const mmap_lib::Tree_index& tuple_node_index);
  void        do_select(const mmap_lib::Tree_index& select_node_index, const std::string& select_type);
  void        do_func_def(const mmap_lib::Tree_index& func_def_node_index);
  void        do_func_call(const mmap_lib::Tree_index& func_def_node_index);
  void        do_get_mask(const mmap_lib::Tree_index& tposs_node_index);
  void        do_set_mask(const mmap_lib::Tree_index& tposs_node_index);
  void        do_tposs(const mmap_lib::Tree_index& tposs_node_index);
  std::string resolve_tuple_assign(const mmap_lib::Tree_index& tuple_assign_index);
  std::string resolve_func_cond(const mmap_lib::Tree_index& func_cond_index);
  bool        is_temp_var(std::string_view test_string);      // can go to private/protected section!?
  bool        is_temp_var(std::string test_string);      // can go to private/protected section!?
  bool        has_DblUndrScor(std::string_view test_string);  // can go to private/protected section!?
  bool        has_DblUndrScor(std::string test_string);  // can go to private/protected section!?
  // std::string_view get_node_name(Lnast_node node);//can go to private/protected section!?
  constexpr bool is_digit(char c) const { return c >= '0' && c <= '9'; }
  bool           is_number(std::string_view test_string);
  bool           is_pos_int(std::string_view test_string);
  bool           is_pos_int(std::string test_string);
  //  void invalid_node();
  std::string_view process_number(std::string_view num_string);
  std::string process_number(std::string num_string);
  // virtual std::string_view stmt_sep() = 0;
};
