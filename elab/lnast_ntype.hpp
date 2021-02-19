//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#pragma once

#include <string_view>
#include <array>

class Lnast_ntype {
public:
  enum Lnast_ntype_int : uint8_t {
    Lnast_ntype_invalid = 0,  // zero is not a valid Lnast_ntype
    //group: tree structure
    Lnast_ntype_top,
    Lnast_ntype_stmts,   // stmts
    Lnast_ntype_cstmts,  // statement for condition determination
    Lnast_ntype_if,
    Lnast_ntype_cond,
    Lnast_ntype_uif,
    Lnast_ntype_for,
    Lnast_ntype_while,
    Lnast_ntype_phi,
    Lnast_ntype_func_call,    // .()
    Lnast_ntype_func_def,     // ::{   func_def = sub-graph in lgraph

    //group: primitive operator
    Lnast_ntype_assign,       // =, pure assignment
    Lnast_ntype_dp_assign,    // :=, dp = deprecate
    Lnast_ntype_as,           // as
    Lnast_ntype_label,        // :
    Lnast_ntype_dot,          // .
    Lnast_ntype_logical_and,  // and
    Lnast_ntype_logical_or,   // or
    Lnast_ntype_logical_not,  // !
    Lnast_ntype_and,          // &
    Lnast_ntype_or,           // |
    Lnast_ntype_not,          // ~
    Lnast_ntype_xor,          // ^
    Lnast_ntype_parity,
    Lnast_ntype_plus,
    Lnast_ntype_minus,
    Lnast_ntype_mult,
    Lnast_ntype_div,
    Lnast_ntype_mod,
    Lnast_ntype_eq,
    Lnast_ntype_same,
    Lnast_ntype_lt,
    Lnast_ntype_le,
    Lnast_ntype_gt,
    Lnast_ntype_ge,
    Lnast_ntype_tuple,        // ()
    Lnast_ntype_tuple_concat, // ++
    Lnast_ntype_tuple_delete, // --
    Lnast_ntype_select,       // []
    Lnast_ntype_bit_select,   // [[]]
    Lnast_ntype_range,        // ..
    Lnast_ntype_shift_right,  // >>
    Lnast_ntype_shift_left,   // <<
    Lnast_ntype_logic_shift_right, // >>
    Lnast_ntype_arith_shift_right, // FIXME->sh: TBD
    Lnast_ntype_arith_shift_left,  // FIXME->sh: TBD
    Lnast_ntype_rotate_shift_right, // >>>
    Lnast_ntype_rotate_shift_left,  // <<<
    Lnast_ntype_dynamic_shift_right, //FIXME->sh: maybe
    Lnast_ntype_dynamic_shift_left,  //FIXME->sh: maybe

    //group: language variable
    Lnast_ntype_ref,
    Lnast_ntype_const,


    //group: others
    Lnast_ntype_assert,      // I
    Lnast_ntype_err_flag,    // compile error flag
    Lnast_ntype_reg_fwd,     // point to the corresponding reg_fwd in LGraph

    //group: compiler internal type
    Lnast_ntype_tuple_add,
    Lnast_ntype_tuple_get,
    Lnast_ntype_attr_set,
    Lnast_ntype_attr_get,

    //for unsigned
    Lnast_ntype_tposs
  };

protected:
  constexpr static std::array namemap{
    "invalid",
    //group: tree structure
    "top",
    "stmts",
    "cstmts",
    "if",
    "cond",
    "uif",
    "for",
    "while",
    "phi",
    "func_call",
    "func_def",

    //group: primitive operator
    "assign",
    "dp_assign",
    "as",
    "label",
    "dot",
    "logical_and",
    "logical_or",
    "logical_not",
    "and",
    "or",
    "not",
    "xor",
    "parity",
    "plus",
    "minus",
    "mult",
    "div",
    "mod",
    "eq",
    "same",
    "lt",
    "le",
    "gt",
    "ge",
    "tuple",
    "tuple_concat", // ++
    "tuple_delete", // --
    "selc",     // []
    "bit_select", // [[]]
    "range",      // ..
    "shift_right",
    "shift_left",
    "logic_shift_right",
    "arith_shift_right",
    "arith_shift_left",
    "rotate_shift_right",
    "rotate_shift_left",
    "dynamic_shift_right",
    "dynamic_shift_left",

    //group: language variable
    "ref",
    "const",

    //group: others
    "assert",
    "error_flag",
    "reg_fwd",
    //group: compiler internal type
    "tuple_add",
    "tuple_get",
    "attr_set",  
    "attr_get",
    //unsigned(tposs)
    "tposs"
  };
  constexpr static std::array namemap_cfg{
    "invalid",
    "top",
    "sts",
    "csts",
    "if",
    "cond",
    "uif",
    "for",
    "while",
    "phi",
    "func_call",
    "func_def",
    "=",
    ":=",
    "as",
    "=",
    ".",
    "add",
    "or",
    "not",
    "&",
    "|",
    "~",
    "^",
    "parity",
    "+",
    "-",
    "*",
    "/",
    "mod",
    "ed",
    "==",
    "<",
    "<=",
    ">",
    ">=",
    "()",
    "tuple_concat", // ++
    "tuple_delete", // --
    "selc",  // []
    "bit_select", // [[]]
    "range", // ..
    "shift_right",
    "shift_left",
    "logic_shift_right",
    "arith_shift_right",
    "arith_shift_left",
    "rotate_shift_right",
    "rotate_shift_left",
    "dynamic_shift_right",
    "dynamic_shift_left",

    "ref",
    "const",
    "I",
    "error_flag",
    "reg_fwd",
    "tuple_add",
    "tuple_get",
    "attr_set",
    "attr_get",
    "unsigned"//tposs
  };
  constexpr static std::array namemap_pyrope{
    "invalid",
    "top",
    "sts",
    "csts",
    "if",
    "cond",
    "uif",
    "for",
    "while",
    "phi",
    "func_call",
    "func_def",
    "=",
    ":=",
    "as",
    ":",
    ".",
    "and",
    "or",
    "!",
    "&",
    "|",
    "~",
    "^",
    "^", // parity
    "+",
    "-",
    "*",
    "/",
    "mod",
    "eq",
    "==",
    "<",
    "<=",
    ">",
    ">=",
    "()",
    "++", //"tuple_concat", // ++
    "--", //"tuple_delete", // --
    "selc",  // []
    "bit_select", // [[]]
    "range",      // ..
    ">>",
    "<<",
    "logic_shift_right",
    ">>", //arith_shift_right
    "arith_shift_left",
    "rotate_shift_right",
    "rotate_shift_left",
    "dynamic_shift_right",
    "dynamic_shift_left",

    "ref",
    "const",
    "assert",
    "error_flag",
    "reg_fwd",
    "tuple_add",
    "tuple_get",
    "attr_set",
    "attr_get",
    "unsigned"//tposs
  };
  constexpr static std::array namemap_verilog{
    "invalid",
    "top",
    "sts",
    "csts",
    "if",
    "cond",
    "uif",
    "for",
    "while",
    "phi",
    "func_call",
    "func_def",
    "=",
    "=",//dp_assign
    "as",
    "=",
    "_",
    "and",
    "or",
    "not",
    "&",
    "|",
    "~",
    "^",
    "^", // parity
    "+",
    "-",
    "*",
    "/",
    "%",
    "eq",
    "==",
    "<",
    "<=",
    ">",
    ">=",
    "()",
    "tuple_concat",
    "tuple_delete",
    "selc", // []
    "bit_select", // [[]]
    "range",      // ..
    "shift_right",
    "shift_left",
    "logic_shift_right",
    ">>>",//arithmetic right shift (>>>) - shift right specified number of bits, fill with value of sign bit if expression is signed, otherwise fill with zero
    "<<<",//"arith_shift_left",
    "rotate_shift_right",
    "rotate_shift_left",
    "dynamic_shift_right",
    "dynamic_shift_left",

    "ref",
    "const",
    "assert",
    "error_flag",
    "reg_fwd",
    "tuple_add",
    "tuple_get",
    "attr_set",
    "attr_get",
    "unsigned"//tposs
  };
  constexpr static std::array namemap_cpp{
    "invalid",
    "top",
    "sts",
    "csts",
    "if",
    "cond",
    "uif",
    "for",
    "while",
    "phi",
    "func_call",
    "func_def",
    "=", // assign
    "=", //dp_assign
    "as",
    "=", // label
    ".", // dot
    "&&", // logical_and
    "||", // logical_or
    "!",  // logical_not
    "&", // and
    "|", // or
    "~", // not
    "^",
    "^", // parity
    "+",
    "-",
    "*",
    "/",
    "%",
    "eq",
    "==",
    "<",
    "<=",
    ">",
    ">=",
    "()",
    "tuple_concat", // ++
    "tuple_delete", // --
    "selc", // []
    "bit_select", // [[]]
    "range",         // ..
    ">>",//shift_right",
    "<<",//shift_left",
    ">>",//logic_shift_right",
    ">>",//arith_shift_right",
    "<<",//arith_shift_left",
    "rotate_shift_right",
    "rotate_shift_left",
    "dynamic_shift_right",
    "dynamic_shift_left",

    "ref",
    "const",
    "assert",
    "error_flag",
    "reg_fwd",
    "tuple_add",
    "tuple_get",
    "attr_set",
    "attr_get",
    "unsigned"
  };

  Lnast_ntype_int val;
  constexpr explicit Lnast_ntype(Lnast_ntype_int _val) : val(_val) {}
public:
  constexpr Lnast_ntype() : val(Lnast_ntype_invalid) {
  }

  std::string_view to_s() const { return namemap[val]; }

  Lnast_ntype_int get_raw_ntype() const { return val; }

  static constexpr Lnast_ntype create_invalid()  { return Lnast_ntype(Lnast_ntype_invalid); }
  static constexpr Lnast_ntype create_top()          { return Lnast_ntype(Lnast_ntype_top); }

  static constexpr Lnast_ntype create_stmts()        { return Lnast_ntype(Lnast_ntype_stmts); }
  static constexpr Lnast_ntype create_cstmts()       { return Lnast_ntype(Lnast_ntype_cstmts); }
  static constexpr Lnast_ntype create_if()           { return Lnast_ntype(Lnast_ntype_if); }
  static constexpr Lnast_ntype create_cond()         { return Lnast_ntype(Lnast_ntype_cond); }
  static constexpr Lnast_ntype create_uif()          { return Lnast_ntype(Lnast_ntype_uif); }
  static constexpr Lnast_ntype create_for()          { return Lnast_ntype(Lnast_ntype_for); }
  static constexpr Lnast_ntype create_while()        { return Lnast_ntype(Lnast_ntype_while); }
  static constexpr Lnast_ntype create_phi()          { return Lnast_ntype(Lnast_ntype_phi); }
  static constexpr Lnast_ntype create_func_call()    { return Lnast_ntype(Lnast_ntype_func_call); }
  static constexpr Lnast_ntype create_func_def()     { return Lnast_ntype(Lnast_ntype_func_def); }

  static constexpr Lnast_ntype create_assign()       { return Lnast_ntype(Lnast_ntype_assign); }
  static constexpr Lnast_ntype create_dp_assign()    { return Lnast_ntype(Lnast_ntype_dp_assign); }
  static constexpr Lnast_ntype create_as()           { return Lnast_ntype(Lnast_ntype_as); }
  static constexpr Lnast_ntype create_label()        { return Lnast_ntype(Lnast_ntype_label); }
  static constexpr Lnast_ntype create_dot()          { return Lnast_ntype(Lnast_ntype_dot); }
  static constexpr Lnast_ntype create_logical_and()  { return Lnast_ntype(Lnast_ntype_logical_and); }
  static constexpr Lnast_ntype create_logical_or()   { return Lnast_ntype(Lnast_ntype_logical_or); }
  static constexpr Lnast_ntype create_logical_not()  { return Lnast_ntype(Lnast_ntype_logical_not); }
  static constexpr Lnast_ntype create_and()          { return Lnast_ntype(Lnast_ntype_and); }
  static constexpr Lnast_ntype create_or()           { return Lnast_ntype(Lnast_ntype_or); }
  static constexpr Lnast_ntype create_not()          { return Lnast_ntype(Lnast_ntype_not); }
  static constexpr Lnast_ntype create_xor()          { return Lnast_ntype(Lnast_ntype_xor); }
  static constexpr Lnast_ntype create_parity()          { return Lnast_ntype(Lnast_ntype_parity); }
  static constexpr Lnast_ntype create_plus()         { return Lnast_ntype(Lnast_ntype_plus); }
  static constexpr Lnast_ntype create_minus()        { return Lnast_ntype(Lnast_ntype_minus); }
  static constexpr Lnast_ntype create_mult()         { return Lnast_ntype(Lnast_ntype_mult); }
  static constexpr Lnast_ntype create_div()          { return Lnast_ntype(Lnast_ntype_div); }
  static constexpr Lnast_ntype create_mod()          { return Lnast_ntype(Lnast_ntype_mod); }
  static constexpr Lnast_ntype create_eq()           { return Lnast_ntype(Lnast_ntype_eq); }
  static constexpr Lnast_ntype create_same()         { return Lnast_ntype(Lnast_ntype_same); }
  static constexpr Lnast_ntype create_lt()           { return Lnast_ntype(Lnast_ntype_lt); }
  static constexpr Lnast_ntype create_le()           { return Lnast_ntype(Lnast_ntype_le); }
  static constexpr Lnast_ntype create_gt()           { return Lnast_ntype(Lnast_ntype_gt); }
  static constexpr Lnast_ntype create_ge()           { return Lnast_ntype(Lnast_ntype_ge); }
  static constexpr Lnast_ntype create_tuple()        { return Lnast_ntype(Lnast_ntype_tuple); }
  static constexpr Lnast_ntype create_tuple_concat() { return Lnast_ntype(Lnast_ntype_tuple_concat); }
  static constexpr Lnast_ntype create_tuple_delete() { return Lnast_ntype(Lnast_ntype_tuple_delete); }
  static constexpr Lnast_ntype create_select()       { return Lnast_ntype(Lnast_ntype_select);}
  static constexpr Lnast_ntype create_bit_select()   { return Lnast_ntype(Lnast_ntype_bit_select);}
  static constexpr Lnast_ntype create_range()        { return Lnast_ntype(Lnast_ntype_range);}

  static constexpr Lnast_ntype create_shift_right()         {return Lnast_ntype(Lnast_ntype_shift_right);}
  static constexpr Lnast_ntype create_shift_left()          {return Lnast_ntype(Lnast_ntype_shift_left);}
  static constexpr Lnast_ntype create_logic_shift_right()   {return Lnast_ntype(Lnast_ntype_logic_shift_right);}
  static constexpr Lnast_ntype create_arith_shift_right()   {return Lnast_ntype(Lnast_ntype_arith_shift_right);}
  static constexpr Lnast_ntype create_arith_shift_left()    {return Lnast_ntype(Lnast_ntype_arith_shift_left);}
  static constexpr Lnast_ntype create_rotate_shift_right()  {return Lnast_ntype(Lnast_ntype_rotate_shift_right);}
  static constexpr Lnast_ntype create_rotate_shift_left()   {return Lnast_ntype(Lnast_ntype_rotate_shift_left);}
  static constexpr Lnast_ntype create_dynamic_shift_right() {return Lnast_ntype(Lnast_ntype_dynamic_shift_right);}
  static constexpr Lnast_ntype create_dynamic_shift_left()  {return Lnast_ntype(Lnast_ntype_dynamic_shift_left);}

  static constexpr Lnast_ntype create_ref()           { return Lnast_ntype(Lnast_ntype_ref); }
  static constexpr Lnast_ntype create_const()         { return Lnast_ntype(Lnast_ntype_const); }

  static constexpr Lnast_ntype create_assert()        { return Lnast_ntype(Lnast_ntype_assert); }
  static constexpr Lnast_ntype create_err_flag()      { return Lnast_ntype(Lnast_ntype_err_flag); }
  static constexpr Lnast_ntype create_reg_fwd()       { return Lnast_ntype(Lnast_ntype_reg_fwd); }

  static constexpr Lnast_ntype create_tuple_add()     { return Lnast_ntype(Lnast_ntype_tuple_add);}
  static constexpr Lnast_ntype create_tuple_get()     { return Lnast_ntype(Lnast_ntype_tuple_get);}
  static constexpr Lnast_ntype create_attr_set()      { return Lnast_ntype(Lnast_ntype_attr_set) ;}
  static constexpr Lnast_ntype create_attr_get()      { return Lnast_ntype(Lnast_ntype_attr_get) ;}
  static constexpr Lnast_ntype create_tposs()      { return Lnast_ntype(Lnast_ntype_tposs) ;}

  bool constexpr is_invalid()      const { return val == Lnast_ntype_invalid; }
  bool constexpr is_top()          const { return val == Lnast_ntype_top; }

  bool constexpr is_stmts()        const { return val == Lnast_ntype_stmts; }
  bool constexpr is_cstmts()       const { return val == Lnast_ntype_cstmts; }
  bool constexpr is_if()           const { return val == Lnast_ntype_if; }
  bool constexpr is_cond()         const { return val == Lnast_ntype_cond; }
  bool constexpr is_uif()          const { return val == Lnast_ntype_uif; }
  bool constexpr is_for()          const { return val == Lnast_ntype_for; }
  bool constexpr is_while()        const { return val == Lnast_ntype_while; }
  bool constexpr is_phi()          const { return val == Lnast_ntype_phi; }
  bool constexpr is_func_call()    const { return val == Lnast_ntype_func_call; }
  bool constexpr is_func_def()     const { return val == Lnast_ntype_func_def; }

  bool constexpr is_assign()       const { return val == Lnast_ntype_assign; }
  bool constexpr is_dp_assign()    const { return val == Lnast_ntype_dp_assign; }
  bool constexpr is_as()           const { return val == Lnast_ntype_as; }
  bool constexpr is_label()        const { return val == Lnast_ntype_label; }
  bool constexpr is_dot()          const { return val == Lnast_ntype_dot; }
  bool constexpr is_logical_and()  const { return val == Lnast_ntype_logical_and; }
  bool constexpr is_logical_or()   const { return val == Lnast_ntype_logical_or; }
  bool constexpr is_logical_not()  const { return val == Lnast_ntype_logical_not; }
  bool constexpr is_and()          const { return val == Lnast_ntype_and; }
  bool constexpr is_or()           const { return val == Lnast_ntype_or; }
  bool constexpr is_not()          const { return val == Lnast_ntype_not; }
  bool constexpr is_xor()          const { return val == Lnast_ntype_xor; }
  bool constexpr is_parity()       const { return val == Lnast_ntype_parity; }
  bool constexpr is_plus()         const { return val == Lnast_ntype_plus; }
  bool constexpr is_minus()        const { return val == Lnast_ntype_minus; }
  bool constexpr is_mult()         const { return val == Lnast_ntype_mult; }
  bool constexpr is_div()          const { return val == Lnast_ntype_div; }
  bool constexpr is_mod()          const { return val == Lnast_ntype_mod; }
  bool constexpr is_eq()           const { return val == Lnast_ntype_eq; }
  bool constexpr is_same()         const { return val == Lnast_ntype_same; }
  bool constexpr is_lt()           const { return val == Lnast_ntype_lt; }
  bool constexpr is_le()           const { return val == Lnast_ntype_le; }
  bool constexpr is_gt()           const { return val == Lnast_ntype_gt; }
  bool constexpr is_ge()           const { return val == Lnast_ntype_ge; }
  bool constexpr is_tuple()        const { return val == Lnast_ntype_tuple; }
  bool constexpr is_tuple_concat() const { return val == Lnast_ntype_tuple_concat; }
  bool constexpr is_tuple_delete() const { return val == Lnast_ntype_tuple_delete; }
  bool constexpr is_selc()         const { return val == Lnast_ntype_select; }
  bool constexpr is_bit_select()   const { return val == Lnast_ntype_bit_select; }
  bool constexpr is_range()        const { return val == Lnast_ntype_range; }

  bool constexpr is_shift_right()         const { return val == Lnast_ntype_shift_right; }
  bool constexpr is_shift_left()          const { return val == Lnast_ntype_shift_left; }
  bool constexpr is_logic_shift_right()   const { return val == Lnast_ntype_logic_shift_right; }
  bool constexpr is_arith_shift_right()   const { return val == Lnast_ntype_arith_shift_right; }
  bool constexpr is_arith_shift_left()    const { return val == Lnast_ntype_arith_shift_left; }
  bool constexpr is_rotate_shift_right()  const { return val == Lnast_ntype_rotate_shift_right; }
  bool constexpr is_rotate_shift_left()   const { return val == Lnast_ntype_rotate_shift_left; }
  bool constexpr is_dynamic_shift_right() const { return val == Lnast_ntype_dynamic_shift_right; }
  bool constexpr is_dynamic_shift_left()  const { return val == Lnast_ntype_dynamic_shift_left; }

  bool constexpr is_ref()           const { return val == Lnast_ntype_ref; }
  bool constexpr is_const()         const { return val == Lnast_ntype_const; }

  bool constexpr is_assert()        const { return val == Lnast_ntype_assert; }
  bool constexpr is_err_flag()      const { return val == Lnast_ntype_err_flag; }
  bool constexpr is_reg_fwd()       const { return val == Lnast_ntype_reg_fwd; }

  bool constexpr is_tuple_add()          const { return val == Lnast_ntype_tuple_add; }
  bool constexpr is_tuple_get()          const { return val == Lnast_ntype_tuple_get; }
  bool constexpr is_attr_set()           const { return val == Lnast_ntype_attr_set; }
  bool constexpr is_attr_get()           const { return val == Lnast_ntype_attr_get; }
  bool constexpr is_tposs()              const { return val == Lnast_ntype_tposs; }

  // Super types
  bool constexpr is_logical_op()   const { return (val == Lnast_ntype_logical_and) ||
                                                  (val == Lnast_ntype_logical_or)  ||
                                                  (val == Lnast_ntype_logical_not); }
  bool constexpr is_logical_not_op()   const { return (val == Lnast_ntype_logical_not); }
  bool constexpr is_unary_op()     const { return (val == Lnast_ntype_not); }
  bool constexpr is_nary_op()      const { return (val == Lnast_ntype_and) ||
                                                  (val == Lnast_ntype_or) ||
                                                  (val == Lnast_ntype_xor) ||
                                                  (val == Lnast_ntype_plus) ||
                                                  (val == Lnast_ntype_minus) ||
                                                  (val == Lnast_ntype_mult) ||
                                                  (val == Lnast_ntype_div) ||
                                                  (val == Lnast_ntype_same) ||
                                                  (val == Lnast_ntype_lt) ||
                                                  (val == Lnast_ntype_le) ||
                                                  (val == Lnast_ntype_gt) ||
                                                  (val == Lnast_ntype_ge) ||
                                                  (val == Lnast_ntype_shift_left) ||
                                                  (val == Lnast_ntype_shift_right); }

  std::string_view debug_name() const { return namemap[val]; }
  std::string_view debug_name_cfg() const { return namemap_cfg[val]; }
  std::string_view debug_name_pyrope() const { return namemap_pyrope[val]; }
  std::string_view debug_name_verilog() const { return namemap_verilog[val]; }
  std::string_view debug_name_cpp() const { return namemap_cpp[val]; }

  static_assert(namemap_cpp.size()==namemap.size());
  static_assert(namemap_cpp.size()==namemap_cfg.size());
  static_assert(namemap_cpp.size()==namemap_pyrope.size());
  static_assert(namemap_cpp.size()==namemap_verilog.size());
};

