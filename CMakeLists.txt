cmake_minimum_required(VERSION 3.15)   
project(pricing_engine LANGUAGES CXX)  

set(CMAKE_CXX_STANDARD 17)             
set(CMAKE_CXX_STANDARD_REQUIRED ON)  

#my libraries : the pricing code
add_library(pricing src/black_scholes.cpp src/monte_carlo.cpp)

target_include_directories(pricing PUBLIC include)

# strict warnings on the library code
target_compile_options(pricing PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>)

# needed so the same library can be linked into the Python module
set_target_properties(pricing PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_executable(demo apps/main.cpp)

# my demo right here
target_link_libraries(demo PRIVATE pricing)


#tests (i used Catch2)         
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(unit_tests tests/test_black_scholes.cpp tests/test_monte_carlo.cpp)
target_link_libraries(unit_tests PRIVATE pricing Catch2::Catch2WithMain)

enable_testing()
include(Catch)
catch_discover_tests(unit_tests)

# to add Python bindings (pybind11) 
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG        v2.13.6
)
FetchContent_MakeAvailable(pybind11)

pybind11_add_module(pricing_engine python/bindings.cpp)
target_link_libraries(pricing_engine PRIVATE pricing)
