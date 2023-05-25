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

add_library(${LIBNAME} SHARED)

target_sources(${LIBNAME} PRIVATE
  core/server_runners/poll_server.cpp
  core/server_runners/select_server.cpp
  core/server.cpp
  core/request_parser.cpp
  core/router.cpp
  core/thread_pool.cpp
)

target_include_directories(${LIBNAME} PRIVATE ./core)
target_include_directories(${LIBNAME} PRIVATE ./build/_deps/llhttp-src/include)
target_include_directories(${LIBNAME} PUBLIC ./public/exports.hpp)

set_target_properties(${LIBNAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_link_libraries(${LIBNAME} PRIVATE llhttp_shared)

target_compile_options(${LIBNAME} PRIVATE -g -O0)

install(TARGETS ${LIBNAME} LIBRARY DESTINATION /usr/local/lib)

file(GLOB HEADERS 
  ${CMAKE_SOURCE_DIR}/core/*.hpp 
  ${CMAKE_SOURCE_DIR}/build/_deps/llhttp-src/include/*.h 
  ${CMAKE_SOURCE_DIR}/core/server_runners/*.hpp
  ${CMAKE_SOURCE_DIR}/public/*.hpp
)

file(COPY ${HEADERS} DESTINATION ${CMAKE_SOURCE_DIR}/include/)