module counter (
  input reset,
  input clock,
  input en,
  output reg [3:0] cnt_o
);


reg [4:0] cnt_o_next;
always @ (*) begin
  if (en == 1)
    cnt_o_next = cnt_o + 1;
  else
    cnt_o_next = {1'b0, cnt_o};
end

always @ (posedge clock) begin
  if (reset) begin
    cnt_o <= 'h0;
  end else begin
    cnt_o <= cnt_o_next[3:0];
  end
end


endmodule
