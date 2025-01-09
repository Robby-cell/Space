#include "gtest/gtest.h"

#include "space/string.hpp"

using Space::String;

TEST(BasicFunctionality, SimpleEqualityTest) {
  String str{"Hello, world!"};
  str = String("Hello, world but better!");
  String str2(str);
  str2 = "Hello, wolrd!";
  ASSERT_EQ(str, "Hello, world but better!");
  ASSERT_EQ(str2, "Hello, wolrd!");
}
