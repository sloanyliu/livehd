//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

#include "attribute.hpp"

#include <boost/multiprecision/cpp_int.hpp>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "lbench.hpp"
#include "lgedgeiter.hpp"
#include "lgraph.hpp"
#include "node.hpp"

using testing::HasSubstr;

unsigned int rseed = 123;

class Setup_attr_test : public ::testing::Test {
protected:
  Lgraph *top;
  Lgraph *subs[1024];

  void SetUp() override {
    srand(rseed++);
    mkdir("lgdb_attr", 0755);
    top = Lgraph::create("lgdb_attr", "top", "-");
    for (int i = 0; i < 1024; i++) {
      subs[i] = Lgraph::create("lgdb_attr", "sub_" + std::to_string(i), "-");
    }

    for (int i = 0; i < 1000000; i++) {
      if ((i & 0xFFF) == 0) {
        top->create_node_sub(subs[rand() & 1023]->get_lgid());
      }
      top->create_node_const(Lconst(i));
    }
  };

  void TearDown() override {
    top->sync();
    delete top;
    for (int i = 0; i < 1024; i++) {
      subs[i]->sync();
      delete subs[i];
    }
  };
};

TEST_F(Setup_attr_test, data_test1) {
  unlink("lgdb_attr/lgraph_dtest1_sparse_attr");
  unlink("lgdb_attr/lgraph_dtest1_dense_attr");
  unlink("lgdb_attr/lgraph_dtest1_dense_attr_max");
  unlink("lgdb_attr/lgraph_dtest1_dense_attr_size");

  unlink("lgdb_attr/lgraph_dtest2_sparse_attr");
  unlink("lgdb_attr/lgraph_dtest2_dense_attr");
  unlink("lgdb_attr/lgraph_dtest2_dense_attr_max");
  unlink("lgdb_attr/lgraph_dtest2_dense_attr_size");

  Lbench b("core.ATTR_data_test1");

  struct Data {
    int  a;
    char b;
  };
  static constexpr char name[] = "dest1";
  using dtest1                 = Attribute<name, Node, mmap_lib::map<Node::Compact_class, Data> >;

  b.sample("setup");

  int i = 0;
  for (auto node : top->fast()) {
    Data d;
    d.a = i;
    d.b = i & 0xFF;
    dtest1::ref(node)->set(node.get_compact_class(), d);
    i++;
  }

  b.sample("check");

  i = 0;
  for (auto node : top->fast()) {
    Data d;
    d.a     = i;
    d.b     = i & 0xFF;
    auto d1 = dtest1::ref(node)->get(node.get_compact_class());
    EXPECT_EQ(d.a, d1.a);
    EXPECT_EQ(d.b, d1.b);
    i++;
  }
}
