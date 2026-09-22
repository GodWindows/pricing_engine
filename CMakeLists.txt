cmake_minimum_required(VERSION 3.15)   
project(pricing_engine LANGUAGES CXX)  

set(CMAKE_CXX_STANDARD 17)             
set(CMAKE_CXX_STANDARD_REQUIRED ON)  

#my library : the pricing code
add_library(pricing src/black_scholes.cpp)

target_include_directories(pricing PUBLIC include)

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

add_executable(unit_tests tests/test_black_scholes.cpp)
target_link_libraries(unit_tests PRIVATE pricing Catch2::Catch2WithMain)

enable_testing()
include(Catch)
catch_discover_tests(unit_tests)