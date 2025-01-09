enable_testing()

add_executable(test
  test.cpp)

find_package(GTest CONFIG REQUIRED)
target_link_libraries(test PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(test PRIVATE Space)

add_test(AllTestsInMain test)
