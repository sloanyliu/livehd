#include "mmap_str.hpp"

#include <functional>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include "fmt/format.h"
#include "gtest/gtest.h"

#define RNDN 10 // number of rand strings
#define MXLN 32 // max len + 1 for rand strings
#define MNLN 0  // min len for rand strings

class Mmap_str_test : public ::testing::Test {
  //std::vector<std::vector<std::string>> ast_sorted_verification;
  std::vector<std::string> str_bank;

public:
  void SetUp() override {
    srand(time(0));
    uint8_t t_len = 0u;

    // random string generation
    for (uint8_t i = 0; i < RNDN; ++i) { // # of strings in vectors
      std::string ele;
      t_len = MNLN + (rand() % MXLN); // deciding string length (0-31)
      // construct string with ASCII (32-126)
      for(uint8_t j = 0; j < t_len; ++j) { ele += (' ' + (rand() % 95)); }
      str_bank.push_back(ele); // add string to vector
    }   
    // for (auto i = str_bank.begin(); i != str_bank.end(); ++i) {std::cout << *i << "\n";}
  }

  // wrapper for vector.at() since str_bank is private
  std::string get(int i) { return str_bank.at(i); }
};

// TEST_F are different types of tests
// >> TEST_F(class_name, test_name) {
//      tests in here
//    }

TEST_F(Mmap_str_test, basic_ctor) {
  mmap_lib::str a1("hello"), a2("hello");
  
  EXPECT_EQ(a1, a2);

  std::string_view b_sv("schizophrenia");
  std::string b_st("schizophrenia");
  mmap_lib::str b1(b_sv), b2(b_st), b3("schizophrenia");

  EXPECT_EQ(b3, b1);
  EXPECT_EQ(b3, b2);
  
  std::string_view c_sv("neutralization");
  std::string c_st("neutralization");
  mmap_lib::str c1(c_sv), c2(c_st), c3("neutralization");

  EXPECT_EQ(c3, c1);
  EXPECT_EQ(c3, c2);
  
  std::string_view d_sv("--this_var_will_be_very_longbuzzball");
  std::string d_st("--this_var_will_be_very_longbuzzball");
  mmap_lib::str d1(d_sv), d2(d_st), d3("--this_var_will_be_very_longbuzzball");

  EXPECT_EQ(d3, d1);
  EXPECT_EQ(d3, d2);
}

TEST_F(Mmap_str_test, random_ctor) {
  for (auto i = 0; i < RNDN; ++i) {
    std::string r_st = get(i);
    //std::cout << r_st << std::endl;
    std::string_view r_sv = r_st;
    mmap_lib::str r1(r_sv), r2(r_st), r3(r_st.c_str());
    EXPECT_EQ(r3, r1);
    EXPECT_EQ(r3, r2);
  }
}


TEST_F(Mmap_str_test, random_comparisons) {
  for (auto i = 0; i < RNDN; ++i) {
    std::string c_st = get(i % RNDN), n_st = get((i+1) % RNDN);
    std::string_view c_sv = c_st, n_sv = n_st;
    mmap_lib::str c_s1(c_st), n_s1(n_st), c_s2(c_sv), n_s2(c_sv);
    
    /*
    std::string n_st = get((i+1) % RNDN);
    std::string_view n_sv = n_st;
    mmap_lib::str n_s1(n_st);
    mmap_lib::str n_s2(n_sv);
    */

    EXPECT_TRUE(c_s1 == c_s2);
    EXPECT_TRUE(c_s1 == c_st);
    EXPECT_TRUE(c_s1 == c_sv);
    EXPECT_TRUE(c_s1 == c_st.c_str());

    EXPECT_FALSE(c_s1 != c_s2);
    EXPECT_FALSE(c_s1 != c_st);
    EXPECT_FALSE(c_s1 != c_sv);
    EXPECT_FALSE(c_s1 != c_st.c_str());
  
    //tests for next and curr
    EXPECT_TRUE(c_s1 != n_s1);
    EXPECT_TRUE(c_s1 != n_st);
    EXPECT_TRUE(c_s1 != n_sv);
    EXPECT_TRUE(c_s1 != n_st.c_str());
    
    EXPECT_FALSE(n_s1 == c_s1);
    EXPECT_FALSE(n_s1 == c_st);
    EXPECT_FALSE(n_s1 == c_sv);
    EXPECT_FALSE(n_s1 == c_st.c_str());
  }
}


#if 0



TEST_F(Mmap_str_test, const_expr_trival_cmp) {

  mmap_lib::str long_a("hello_hello_hello_hello_hello_hello"); // not right, but it should compile
  constexpr mmap_lib::str a("hello");

  auto b = do_check("hello");
  EXPECT_TRUE(b);

  std::string_view a_sv{"hello"};

  EXPECT_TRUE(do_check(a_sv));

  EXPECT_TRUE(do_check_string("hello"));
  EXPECT_TRUE(do_check_sv("hello"));

  /*
  fmt::print("a[0]:{} size:{}\n",a[0], a.size());
  fmt::print("a[1]:{} size:{}\n",a[1], a.size());
  fmt::print("a[2]:{} size:{}\n",a[2], a.size());
  fmt::print("a[3]:{} size:{}\n",a[3], a.size());
  fmt::print("a[4]:{} size:{}\n",a[4], a.size());
  */

  assert(a[0] == 'h');   // compile time check
  assert(a[1] == 'e');   // compile time check
  assert(a[2] == 'l');   // compile time check
  assert(a[3] == 'l');   // compile time check
  assert(a[4] == 'o');   // compile time check
  assert(a.size() == 5); // compile time check
  // TODO: static_assert(a==a_sv);       // compile time check
  assert(a=="hello");    // compile time check
  //static_assert(a!="helxo");    // compile time check
  //static_assert(a!="hell");     // compile time check
  //static_assert(a!="hellox");   // compile time check
}
#endif


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
};
