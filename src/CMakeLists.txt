cmake_minimum_required(VERSION 3.10.0)
project(MY_PROJECT CXX)
#SET(CMAKE_C_FLAGS_DEBUG "/MTd /Z7 /Od")
add_executable(proj_test test.cpp lib.hpp)
add_executable(proj main.cpp lib.hpp)

#set(GCC_CXX_FLAGS ${GCC_CXX_FLAGS} "-static-libgcc -static-libstdc++")
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
#target_link_options(proj_test PUBLIC -static)
#target_link_options(proj PUBLIC -static)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

target_link_libraries(proj_test PUBLIC Catch2::Catch2 default PUBLIC nana::nana sfml-graphics sfml-audio)

target_link_libraries(proj PUBLIC Catch2::Catch2 default PUBLIC nana::nana sfml-graphics sfml-audio)
catch_discover_tests(proj_test)
