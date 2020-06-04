cmake_minimum_required(VERSION 3.10.0)

# SET (CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
# add_definitions(-DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_BUILD_TYPE=Release -DWITH_XC_AUTOTYPE=ON -DWITH_XC_HTTP=ON)
set(NANA_CMAKE_NANA_FILESYSTEM_FORCE ON)

project(Homework CXX)
#set(NANA_CMAKE_ENABLE_AUDIO ON)
include(cmake/StandardProjectSettings.cmake)
include(cmake/Catch.cmake)


#SET(NANA_CMAKE_NANA_FILESYSTEM_FORCE ON)

#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREAD ON)

#SET(Boost_INCLUDE_DIR "C:\\Users\\a\\source\\lib\\boost_1_73_0")
#SET(BOOST_ROOT "C:\\Users\\a\\source\\lib\\boost_1_73_0")
#SET(BOOST_LIBRARYDIR "C:\\Users\\a\\source\\lib\\boost_1_73_0\\boost")
#SET(_boost_LIBRARIES_SEARCH_DIRS
#C:\\Users\\a\\source\\lib\\boost_1_73_0
#C:\\Users\\a\\source\\lib
#C:\\Users\\a\\source\\lib\\boost_1_73_0\\boost)
include_directories(external/boost_1_73_0)

#find_package(Boost REQUIRED)



#include(cmake/SFMLConfig.cmake)
# Link this 'library' to set the c++ standard / compile-time options requested
add_library(default INTERFACE)

target_compile_features(default INTERFACE cxx_std_20)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAG} -stdlib=libc++ -std=gnu++11")
set(CMAKE_CXX_STANDARD 20)


include(cmake/CompilerWarnings.cmake)
set_project_warnings(default)

enable_testing()
add_subdirectory(external/catch2)
add_subdirectory(external/nana)
add_subdirectory(external/SFML)
add_subdirectory(src)