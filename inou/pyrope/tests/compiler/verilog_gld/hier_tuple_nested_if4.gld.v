/* Generated by Yosys livehd+0.9+ */

module hier_tuple_nested_if4(op, en, \foo.a );
  wire [4:0] \_._tg_11 ;
  wire [4:0] \_._tg_5 ;
  wire [4:0] \_._tg_7 ;
  wire [4:0] \_._tg_9 ;
  wire ___d_0;
  wire ___e_0;
  wire ___f_0;
  wire ___i_0;
  input [1:0] en;
  output [4:0] \foo.a ;
  wire lg_0;
  wire [2:0] lg_1;
  wire [3:0] lg_11;
  wire [3:0] lg_13;
  wire [2:0] lg_15;
  wire [4:0] lg_18;
  wire [1:0] lg_2;
  wire [4:0] lg_3;
  wire [4:0] lg_4;
  wire [4:0] lg_5;
  wire [4:0] lg_6;
  wire [2:0] lg_7;
  wire [1:0] lg_8;
  wire [3:0] lg_9;
  input op;
  assign ___d_0 = ! $signed({ 2'h0, en });
  assign ___e_0 = $signed(2'h1) == $signed(en);
  assign ___f_0 = $signed(2'h2) == $signed(en);
  assign \_._tg_5  = op ? 5'h0c : 5'h0d;
  assign \_._tg_7  = ___f_0 ? \_._tg_5  : 5'h00;
  assign \_._tg_9  = ___e_0 ? 5'h0b : \_._tg_7 ;
  assign \_._tg_11  = ___d_0 ? 5'h0a : \_._tg_9 ;
  assign ___i_0 = op;
  assign \foo.a  = \_._tg_11 ;
  assign lg_0 = 1'h0;
  assign lg_1 = 3'h2;
  assign lg_11 = { 2'h0, en };
  assign lg_13 = { 2'h0, en };
  assign lg_15 = { 2'h0, op };
  assign lg_18 = 5'h00;
  assign lg_2 = 2'h1;
  assign lg_3 = 5'h0a;
  assign lg_4 = 5'h0b;
  assign lg_5 = 5'h0c;
  assign lg_6 = 5'h0d;
  assign lg_7 = { 1'h0, en };
  assign lg_8 = { 1'h0, op };
  assign lg_9 = { 2'h0, en };
endmodule
