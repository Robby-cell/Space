#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "space/string.hpp"

using Space::String;

TEST_CASE("Basic String Test", "[string]") {
  String str{"Hello, world!"};
  str = String("Hello, world but better!");
  String str2(str);
  str2 = "Hello, wolrd!";
  REQUIRE(str == "Hello, world but better!");
  REQUIRE(str2 == "Hello, wolrd!");
}