//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

#include "cell.hpp"

#include "iassert.hpp"

Ntype::_init Ntype::_static_initializer;

Ntype::_init::_init() {
  for (uint8_t op = 1; op < static_cast<uint8_t>(Ntype_op::Last_invalid); ++op) {
    for (auto i = 0; i < 256; ++i) {
      sink_name2pid[i][op] = -1;
    }
    for (auto i = 0u; i < sink_pid2name.size(); ++i) {
      sink_pid2name[i][op] = "invalid";
    }

    int n_sinks = 0;
    for (int pid = 0; pid < 12; ++pid) {
      auto pin_name = Ntype::get_sink_name_slow(static_cast<Ntype_op>(op), pid);
      if (pin_name.empty() || pin_name == "invalid")
        continue;

      ++n_sinks;

      assert(is_unlimited_sink(static_cast<Ntype_op>(op)) || pid > 10 || sink_name2pid[pin_name[0]][op] == -1
             || sink_name2pid[pin_name[0]][op] == pid);  // No double assign

      sink_pid2name[pid][op] = pin_name;

      auto [it, inserted] = name2pid.insert({std::string(pin_name), pid});
      if (!inserted) {
        I(it->second == pid);  // same name should always have same PID
      }

      if (static_cast<Ntype_op>(op)!= Ntype_op::Memory && is_unlimited_sink(static_cast<Ntype_op>(op)) && pid >= 10)
        continue;

      sink_name2pid[pin_name[0]][op] = pid;
      assert(pid == Ntype::get_sink_pid(static_cast<Ntype_op>(op), pin_name));
      assert(pin_name == Ntype::get_sink_name(static_cast<Ntype_op>(op), pid));
    }

    if (n_sinks == 1) {
      ntype2single_input[op] = true;
      I(sink_pid2name[0][op] != "invalid");
    } else {
      ntype2single_input[op] = false;
    }

    int pid;
    (void)pid;
    // Check that common case is fine

    pid = sink_name2pid['a'][op];
    assert(pid == -1 || pid == 0);

    pid = sink_name2pid['b'][op];
    assert(pid == -1 || pid == 1);

    pid = sink_name2pid['c'][op];
    assert(pid == -1 || pid == 2);

    pid = sink_name2pid['d'][op];
    assert(pid == -1 || pid == 3);

    pid = sink_name2pid['e'][op];
    assert(pid == -1 || pid == 4);

    pid = sink_name2pid['f'][op];
    assert(pid == -1 || pid == 5);

    pid = sink_name2pid['A'][op];
    assert(pid == -1 || pid == 0);

    pid = sink_name2pid['B'][op];
    assert(pid == -1 || pid == 1);
  }

  int pos = 0;
  for (auto e : cell_name) {
    cell_name_map[e] = static_cast<Ntype_op>(pos);
    ++pos;
  }
}

constexpr std::string_view Ntype::get_sink_name_slow(Ntype_op op, int pid) {
  switch (op) {
    case Ntype_op::Invalid: return "invalid"; break;
    case Ntype_op::Sum:
    case Ntype_op::LT:
    case Ntype_op::GT:
      if (pid == 0)
        return "A";
      else if (pid == 1)
        return "B";
      return "invalid";
      break;
    case Ntype_op::Mult:
    case Ntype_op::And:
    case Ntype_op::Or:
    case Ntype_op::Xor:
    case Ntype_op::Ror:
    case Ntype_op::EQ:
      if (pid == 0)
        return "A";
      return "invalid";
      break;
    case Ntype_op::Not:
      if (pid == 0)
        return "a";
      return "invalid";
      break;
    case Ntype_op::Sext:
    case Ntype_op::Div:
    case Ntype_op::SHL:
    case Ntype_op::SRA:
      if (pid == 0)
        return "a";
      else if (pid == 1)
        return "b";
      return "invalid";
      break;
    case Ntype_op::Const:  // No drivers to Constants
      return "invalid";
      break;
    case Ntype_op::IO:
    case Ntype_op::Mux:  // unlimited case: 1,2,3,4,5.... // Y = (pid0 == true) ? pid2 : pid1
    case Ntype_op::LUT:  // unlimited case: 1,2,3,4,5....
    case Ntype_op::Sub:  // unlimited case: 1,2,3,4,5....
    case Ntype_op::CompileErr:
      assert(is_unlimited_sink(op));
      switch (pid) {
        case 0: return "0";
        case 1: return "1";
        case 2: return "2";
        case 3: return "3";
        case 4: return "4";
        case 5: return "5";
        case 6: return "6";
        case 7: return "7";
        case 8: return "8";
        case 9: return "9";
        case 10: return "10";  // >10 handled with loop at get_sink_pid
        default: return "invalid";
      }
      return "invalid";
      break;
    case Ntype_op::Memory:
      switch (pid) {
        case 0: return "addr";      // runtime  x n_ports
        case 1: return "bits";      // comptime x 1
        case 2: return "clock";     // runtime  x 1 or n_ports
        case 3: return "din";       // runtime  x n_ports
        case 4: return "enable";    // runtime  x n_ports
        case 5: return "fwd";       // comptime x 1
        case 6: return "posclk";    // comptime x 1
        case 7: return "latency";   // comptime x n_ports
        case 8: return "wensize";   // comptime x 1  -- number of Write Enable bits
        case 9: return "size";      // comptime x 1
        case 10: return "rdport";   // comptime x n_ports (1 rd, 0 wr)
        default: return "invalid";
      }
      break;
    case Ntype_op::Flop:
      switch (pid) {
        case 0: return "async";
        case 1: return "initial";  // reset value
        case 2: return "clock";
        case 3: return "din";
        case 4: return "enable";
        case 5: return "negreset";
        case 6: return "posclk";
        case 7: return "reset";
        default: return "invalid";
      }
      break;
    case Ntype_op::Latch:
      switch (pid) {
          // No 1 to keep din at pos 3 (a,b,c)
        case 3: return "din";
        case 4: return "enable";
        case 6: return "posclk";
        default: return "invalid";
      }
      break;
    case Ntype_op::Fflop:  // Fluid-flop
      switch (pid) {
        case 0: return "valid";
        case 1: return "initial";  // reset value
        case 2: return "clock";
        case 3: return "din";
        case 5: return "stop";  // stop from next cycle
        case 7: return "reset";
        default: return "invalid";
      }
      break;
    case Ntype_op::Get_mask:
      switch (pid) {
        case 0: return "a";     // pass through wire
        case 2: return "mask";  // bit position
        default: return "invalid";
      }
      break;
    case Ntype_op::Set_mask:
      switch (pid) {
        case 0: return "a";     // pass through wire
        case 2: return "mask";  // bit position
        case 4: return "value";
        default: return "invalid";
      }
      break;
    case Ntype_op::TupAdd:
      switch (pid) {
        case 0: return "parent";  // tuple name
        case 4: return "value";
        case 5: return "field";  // position of tuple field
        default: return "invalid";
      }
      break;
    case Ntype_op::TupGet:
      switch (pid) {
        case 0: return "parent";
        case 5: return "field";  // SAME as AttrGet field to avoid rewire
        default: return "invalid";
      }
      break;
    case Ntype_op::AttrSet:
      switch (pid) {
        case 0: return "parent";
        case 4: return "value";
        case 5: return "field";
        default: return "invalid";
      }
      break;
    case Ntype_op::AttrGet:
      switch (pid) {
        case 0: return "parent";
        case 5: return "field";
        default: return "invalid";
      }
      break;
    default: assert(false); return "invalid";
  }

  return "invalid";
}

bool Ntype::has_sink(Ntype_op op, std::string_view str) {
  auto it = name2pid.find(str);
  if (it == name2pid.end()) {
    if (std::isdigit(str[0]) && is_unlimited_sink(op))
      return true;

    return false;
  }

  return sink_pid2name[it->second][static_cast<int>(op)] == str;
}

#ifdef NDEBUG
// asserts break the constexpr check
//
// This should be compile time constants (check assembly)
int cell_code_check_code1() {
  constexpr auto pid = Ntype::get_sink_pid(Ntype_op::Sum, "B");
  static_assert(pid == 1);
  return pid;
}

int cell_code_check_code2() {
  constexpr auto pid = Ntype::get_sink_pid(Ntype_op::Mux, "321");
  static_assert(pid == 321);
  return pid;
}
#endif
