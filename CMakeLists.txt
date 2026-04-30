cmake_minimum_required(VERSION 3.16)
project(redis-starter-cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
enable_testing()

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
add_executable(redis ${SOURCE_FILES})
target_link_libraries(redis PRIVATE asio asio::asio Threads::Threads)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(redis PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wshadow
  )
endif()

# App for performing test
file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS tests/*.cpp)
add_executable(redis_tests ${TEST_FILES})
target_link_libraries(redis_tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
)

add_test(NAME UnitTests COMMAND redis_tests)
