cmake_minimum_required (VERSION 3.29)
project (HPSU_tests)

set(TARGET_NAME hpsu_tests)
set(INCLUDE_DIR ../include/)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  ${TARGET_NAME}
  dummys.cpp
  test_utils.cpp
  test_request.cpp
)

target_link_libraries(
  ${TARGET_NAME}
  GTest::gtest_main
  GTest::gmock_main
)

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${TARGET_NAME} PRIVATE "${GMOCK_INCLUDE_DIRS}" "${GTEST_INCLUDE_DIRS}")

include(GoogleTest)
gtest_discover_tests(${TARGET_NAME})
