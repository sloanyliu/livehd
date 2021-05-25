
#include "chunkify_verilog.hpp"

#include <iostream>

#include "eprp_utils.hpp"
#include "gtest/gtest.h"

class VTest1 : public ::testing::Test {
public:
protected:
  void SetUp() override {}
};

TEST_F(VTest1, interface) {
  Eprp_utils::clean_dir("tbase");
  Eprp_utils::clean_dir("tdelta");

  Chunkify_verilog chunker("tbase");

  std::string test1_verilog
      = "  "
        "`ifdef NOTHING  \n"
        "  a = ; \n"
        "  `endif  \n"
        "      /* comment */                                \n"
        "    module test1_moda(input [7:0] a, input [7:0] b,\n"
        "  output signed [7:0] h\n"
        ");\n"
        "                                                   \n"
        "  signed\twire\t[7:0] as = a;signed wire [7:0] bs = b;   \n"
        " /* endmodule module in comment */                 \n"
        "  wire [7:0] f = as + bs;assign h = as+bs-as; // endmodule inside comment\n"
        "  /* module a() /* nested module /* aa */ */*/     \n"
        "endmodule\tmodule test1_modb(a,b,\n"
        "  h) ;\n"
        "  input a;\n"
        "  output h;\n"
        "  input b;\n"
        "\n"
        "  reg [4:0] a;\n"
        "  reg [0:4] b;\n"
        "\n"
        "  assign h = a ^ b;\n"
        " typo in this block of code \"module\" \n"
        "endmodule";  // No return/space after endmodule

  chunker.parse_inline(test1_verilog);

  EXPECT_EQ(access("tbase/parse/file_inline", R_OK), F_OK);
  EXPECT_EQ(access("tbase/parse/chunk_inline/test1_moda.v", R_OK), F_OK);
  EXPECT_EQ(access("tbase/parse/chunk_inline/test1_modb.v", R_OK), F_OK);

  // No code change delta
  Chunkify_verilog chunker2("tdelta");
  EXPECT_NE(access("tdelta/parse/chunk_inline/test1_moda.v", R_OK), F_OK);
  EXPECT_NE(access("tdelta/parse/chunk_inline/test1_modb.v", R_OK), F_OK);
  chunker2.parse_inline(test1_verilog);
  EXPECT_EQ(access("tdelta/parse/chunk_inline/test1_moda.v", R_OK), F_OK);
  EXPECT_EQ(access("tdelta/parse/chunk_inline/test1_modb.v", R_OK), F_OK);

  std::string test2_verilog
      = "  "
        "    module test1_moda(input [1:0] a, input [7:0] b,\n"
        "  output signed [1:0] h\n"
        ");\n endmodule\n";
  // test1_moda different
  chunker2.parse_inline(test2_verilog);
  EXPECT_EQ(access("tdelta/parse/chunk_inline/test1_moda.v", R_OK), F_OK);
  EXPECT_NE(access("tdelta/parse/chunk_inline/test1_modb.v", R_OK), F_OK);
}

void test_throw() {
  std::string test2_verilog = "";

  mkdir("lgdb", 0755);
  mkdir("lgdb/noaccess_dir", 0000);

  Chunkify_verilog chunker("lgdb/noaccess_dir");
  chunker.parse_inline(test2_verilog.c_str());
}

TEST_F(VTest1, noaccess) { ASSERT_THROW(test_throw(), std::runtime_error); }
