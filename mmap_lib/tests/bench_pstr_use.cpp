#include "fmt/format.h"
#include "iassert.hpp"
#include <vector>
#include "lrand.hpp"
#include "lbench.hpp"
#include "flat_hash_map.hpp"
#include <type_traits>
#include "mmap_str.hpp"
#include "mmap_map.hpp"

#if 0
//implicitly changes ts to string_view
void test_sview(const char * ts) {
  std::cout << "> Test Case: str(\"" << ts << "\")" << std::endl;
  mmap_lib::str test11(ts);
  std::cout << "  "; test11.print_PoS(); 
  std::cout << "  "; test11.print_e();
  std::cout << "  "; test11.print_StrMap();
  std::cout << "  "; test11.print_StrVec();
  std::cout << std::endl;
}
#endif

void test_ctor(mmap_lib::str ts, const char* rs) {
  std::cout << "> Test Case: str(\"" << rs << "\")" << std::endl;
  std::cout << "  "; ts.print_PoS(); 
  std::cout << "  "; ts.print_e();
  std::cout << "  "; ts.print_StrMap();
  std::cout << "  "; ts.print_StrVec();
  std::cout << std::endl;
}

void mmap_pstr_ctor_tests() { 
  std::cout << "================================ " << std::endl;
  std::cout << "Constructor 1 (size < 14) Tests: " << std::endl;
  std::cout << "================================ " << std::endl;
  
  test_ctor(mmap_lib::str("hello"), "hello"); 
  test_ctor(mmap_lib::str("cat"), "cat");
  test_ctor(mmap_lib::str("abcd"), "abcd");
  test_ctor(mmap_lib::str("feedback"), "feedback"); 
  test_ctor(mmap_lib::str("neutralizatio"), "neutralizatio");
  
  std::cout << "================================ " << std::endl;
  std::cout << "Constructor 2 (size >=14) Tests: " << std::endl;
  std::cout << "================================ " << std::endl;

  test_ctor(mmap_lib::str("01words23456789"), "01words23456789");
  test_ctor(mmap_lib::str("98words76543210"), "98words76543210");
  test_ctor(mmap_lib::str("01sloan23456789"), "01sloan23456789");
  test_ctor(mmap_lib::str("01andy23456789"), "01andy23456789");
  test_ctor(mmap_lib::str("hisloanbuzzball"), "hisloanbuzzball");
  test_ctor(mmap_lib::str("--this_var_will_bee_very_longbuzzball")
                         , "--this_var_will_bee_very_longbuzzball");
  test_ctor(mmap_lib::str("hidifferentbuzzball"), "hidifferentbuzzball");

  std::cout << "================================ " << std::endl;
  std::cout << "Constructor 3 (string_view) Tests: " << std::endl;
  std::cout << "================================ " << std::endl;

  test_ctor(mmap_lib::str(std::string_view("hello")), "hello"); 
  test_ctor(mmap_lib::str(std::string_view("cat")), "cat"); 
  test_ctor(mmap_lib::str(std::string_view("abcd")), "abcd"); 
  test_ctor(mmap_lib::str(std::string_view("feedback")), "feedback"); 
  test_ctor(mmap_lib::str(std::string_view("neutralizatio")), "neutralizatio"); 
  test_ctor(mmap_lib::str(std::string_view("neutralization")), "neutralization"); 
  test_ctor(mmap_lib::str(std::string_view("01andy23456789")), "01andy23456789"); 
  test_ctor(mmap_lib::str(std::string_view("--this_var_will_bee_very_longbuzzball"))
                         , "--this_var_will_bee_very_longbuzzball");

  /*
  std::cout << "> Test 17: str(\"lonng_andalsjdfkajsdkfljkalsjdfkljaskldjfklajdkslfjalsdjfllaskdfjklajskdlfjklasjdfljasdklfjklasjdflasjdflkajsdflkjakljdfkljaldjfkjakldsjfjaklsjdfjklajsdfjaklsfasjdklfjklajskdljfkljlaksjdklfjlkajsdklfjkla01words23456789\") " << std::endl;
  mmap_lib::str test17("lonng_andalsjdfkajsdkfljkalsjdfkljaskldjfklajdkslfjalsdjfllaskdfjklajskdlfjklasjdfljasdklfjklasjdflasjdflkajsdflkjakljdfkljaldjfkjakldsjfjaklsjdfjklajsdfjaklsfasjdklfjklajskdljfkljlaksjdklfjlkajsdklfjkla01words23456789");
  */
}


template<std::size_t N>
void test_eq(mmap_lib::str ts, const char (&rs)[N], bool ans) {
  if ((ts == rs) == ans) { std::cout << "passed" << std::endl; }
  else { std::cout << "failed" << std::endl; }
}

int main(int argc, char **argv) {
  //mmap_pstr_ctor_tests();
  test_eq(mmap_lib::str("hello"),              "hello",              true);
  test_eq(mmap_lib::str("hello"),              "hi",                 false);
  test_eq(mmap_lib::str("hi"),                 "hi",                 true);
  test_eq(mmap_lib::str("hi"),                 "hello",              false);
  test_eq(mmap_lib::str("hello_!_world"),      "hello_!_world",      true);
  test_eq(mmap_lib::str("hello_!_world"),      "hi",                 false);
  test_eq(mmap_lib::str("hello"),              "hello_!_word",       false);
  test_eq(mmap_lib::str("micro-architecture"), "micro-architecture", true);
  test_eq(mmap_lib::str("$!%^!@%$!%@$@^!$%@"), "micro-architecture", false); // <--
  test_eq(mmap_lib::str("$!%^!@%$!%@$@^!$%@"), "$!%^!@%$!%@$@^!$%@", true); // <--
  test_eq(mmap_lib::str("micro-architecture"), "micro-architecture", true); // <--
  test_eq(mmap_lib::str("micro-architecture"), "hi",                 false);
  test_eq(mmap_lib::str("hi"),                 "micro-architecture", false);
  
  /*
  test_eq(mmap_lib::str("hello"), "hi", false);
  test_eq(mmap_lib::str("hello"), "hi", false);
  test_eq(mmap_lib::str("hello"), "hi", false);
  test_eq(mmap_lib::str("hello"), "hi", false);
  test_eq(mmap_lib::str("hello"), "hi", false);
  test_eq(mmap_lib::str("hello"), "hi", false);
  test_eq(mmap_lib::str("hello"), "hi", false);
  test_eq(mmap_lib::str("hello"), "hi", false);
  */
  //mmap_lib::str tt1("hello");
  //mmap_lib::str tt2("yo");
  //mmap_lib::str tt3("hello_world");
  //mmap_lib::str tt4("swag");
  
  //test_ctor(tt1, "hello");
 
  //std::cout << "i should = [24, 16, 8, 0]" << std::endl; 
  //if (tt1 == "hello") { std::cout << "same" << std::endl; }  
  //else { std::cout << "not\n"; }
 
#if 0  
  //std::cout << "i should = [8, 0]" << std::endl; 
  if (tt2 == "hello") { std::cout << "same" << std::endl; }  
  else { std::cout << "not\n"; }
  
  //std::cout << "i should = [24, 16, 8, 0]" << std::endl; 
  if (tt3 == "hello") { std::cout << "same" << std::endl; }  
  else { std::cout << "not\n"; }
  
  //std::cout << "i should = [24, 16, 8, 0]" << std::endl; 
  if (tt4 == "hello") { std::cout << "same" << std::endl; }  
  else { std::cout << "not\n"; }
  //tt1.print_StrMap();
  //tt1.print_StrVec();
#endif
  return 0;
}
