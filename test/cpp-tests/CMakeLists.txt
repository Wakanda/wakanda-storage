cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=C++11")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)

macro(use_cxx14)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=C++14")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 14)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
  endif ()
endmacro(use_cxx14)

#use_cxx11()
use_cxx14()

project (cpp-tests)

add_executable(cpp-tests
  "${CMAKE_CURRENT_SOURCE_DIR}/../../src/shared_item.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../src/shared_storage.cpp"
  basis.cpp
  main.cpp
)

target_include_directories(cpp-tests PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../../src"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../deps"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../deps/catch2"
)

target_compile_definitions(cpp-tests PUBLIC BOOST_DATE_TIME_NO_LIB)

if (UNIX AND NOT APPLE)
  find_library(LIB_RT rt)
  if (LIB_RT)
    target_link_libraries(cpp-tests ${LIB_RT})
  else()
    message(SEND_ERROR "lib rt is missing")
  endif()

  find_library(LIB_PT pthread)
  if (LIB_PT)
    target_link_libraries(cpp-tests ${LIB_PT})
  else()
    message(SEND_ERROR "lib pthread is missing")
  endif()
endif()
