# Automatically fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Create the test executable (add your other test_*.cpp files here)
add_executable(figure_test_suite
    test_triangle.cpp
    test_rectangle.cpp
    test_circle.cpp
)

add_executable(factory_tests
    test_stream_factory.cpp
    test_string_to_figure.cpp
    test_random_factory.cpp
    test_abstract_factory.cpp
    test_figure_registry.cpp
)

# Link your library and the Google Test main library
target_link_libraries(figure_test_suite PRIVATE 
    figure_lib 
    GTest::gtest_main
)

target_link_libraries(factory_tests PRIVATE 
    figure_lib
    GTest::gtest_main
)

# Register the tests with CTest so you can run 'ctest' in the build folder
include(GoogleTest)
gtest_discover_tests(figure_test_suite)