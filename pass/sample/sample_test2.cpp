//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

#include <unistd.h>

#include <string>

#include "eprp_utils.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "lgraph.hpp"
#include "pass_sample.hpp"

class SampleMainTest : public ::testing::Test {
protected:
  void SetUp() override {}
};

TEST_F(SampleMainTest, EmptyLgraph) {
  Eprp_utils::clean_dir("pass_test_lgdb");

  Lgraph *g = Lgraph::create("pass_test_lgdb", "empty", "nosource");

  Eprp_var var;
  var.add(g);
  var.add("data", "hello");

  EXPECT_FALSE(g->get_library().has_name("pass_sample"));

  Pass_sample::work(var);

  EXPECT_TRUE(g->get_library().has_name("pass_sample"));

  Graph_library::shutdown();  // needed to recycle pointers (AddressSanitizer will flag leak otherwise)
}
