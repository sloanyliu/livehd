`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/09/2017 06:15:07 PM
// Design Name: 
// Module Name: full_adder
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module full_adder(
    input a,       // first bit
    input b,       // second bit
    input cin,     // carry
    output cout,   // TRUE output
    output s       // what should be displayed
    );
    
    // Under what circumstances should we display a 1?
    // Under what circumstances should we display a 0?
    assign s = (cin & ((~a & ~b) | (a & b))) | (~cin & ((a & ~b) | (~a & b)));  // what is displayed
    
    // Under what cirsumstances is the result a 1?
    // Under what circumstances is the result a 0?
    assign cout = (cin & a) | (cin & b) | (a & b & ~cin);  // OUTPUT
endmodule
