cmake_minimum_required(VERSION 3.20.0)

if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project (cppserv VERSION 0.1.0 DESCRIPTION "Simple server library for C++" LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)
set (LIBNAME ${PROJECT_NAME}_lib)

include(FetchContent)

FetchContent_Declare(llhttp
  URL "https://github.com/nodejs/llhttp/archive/refs/tags/release/v8.1.0.tar.gz")

FetchContent_MakeAvailable(llhttp)

add_library(${LIBNAME} SHARED
  core/poll_server.cpp
  core/server.cpp
  core/request_parser.cpp
)

target_include_directories(${LIBNAME} PRIVATE .)
target_include_directories(${LIBNAME} PRIVATE ./build/_deps/llhttp-src/include)

set_target_properties(${LIBNAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_link_libraries(${LIBNAME} PRIVATE llhttp_shared)