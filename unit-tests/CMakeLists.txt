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

set(BOOST_GIT_TAG "boost-1.66.0")
set(CATCH2_VERSION "v2.2.3")

#find git binary
find_package(Git)
if (NOT Git_FOUND)
  message(FATAL_ERROR "Git not found")
endif()


#clone lib boost system repository
set(BOOST_SYSTEM_DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../deps/libs/boost/system")
if (NOT EXISTS "${BOOST_SYSTEM_DESTINATION}/src/error_code.cpp")
  set(BOOST_SYSTEM_GIT_URL "https://github.com/boostorg/system.git")

  message(STATUS "Cloning git repository \"${BOOST_SYSTEM_GIT_URL}\""
          " with tag \"${BOOST_GIT_TAG}\""
          " into directory \"${BOOST_SYSTEM_DESTINATION}\"")

  file(MAKE_DIRECTORY "${BOOST_SYSTEM_DESTINATION}")

  execute_process(
    COMMAND "${GIT_EXECUTABLE}" clone --branch "${BOOST_GIT_TAG}" "${BOOST_SYSTEM_GIT_URL}" .
    WORKING_DIRECTORY "${BOOST_SYSTEM_DESTINATION}"
    RESULT_VARIABLE BOOST_SYSTEM_CLONE_RESULT)

  if (NOT "${BOOST_SYSTEM_CLONE_RESULT}" EQUAL 0)
    file(REMOVE_RECURSE "${BOOST_SYSTEM_DESTINATION}")
    message(FATAL_ERROR "Failed to clone the git repository \"boost system\"")
  endif()
endif()


#clone lib boost filesystem repository
set(BOOST_FILESYSTEM_DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../deps/libs/boost/filesystem")
if (NOT EXISTS "${BOOST_FILESYSTEM_DESTINATION}/src/path.cpp")
  set(BOOST_FILESYSTEM_GIT_URL "https://github.com/boostorg/filesystem.git")

  message(STATUS "Cloning git repository \"${BOOST_FILESYSTEM_GIT_URL}\""
          " with tag \"${BOOST_GIT_TAG}\""
          " into directory \"${BOOST_FILESYSTEM_DESTINATION}\"")

  file(MAKE_DIRECTORY "${BOOST_FILESYSTEM_DESTINATION}")

  execute_process(
    COMMAND "${GIT_EXECUTABLE}" clone --branch "${BOOST_GIT_TAG}" "${BOOST_FILESYSTEM_GIT_URL}" .
    WORKING_DIRECTORY "${BOOST_FILESYSTEM_DESTINATION}"
    RESULT_VARIABLE BOOST_FILESYSTEM_CLONE_RESULT)

  if (NOT "${BOOST_FILESYSTEM_CLONE_RESULT}" EQUAL 0)
    file(REMOVE_RECURSE "${BOOST_FILESYSTEM_DESTINATION}")
    message(FATAL_ERROR "Failed to clone the git repository \"boost filesystem\"")
  endif()
endif()


#download catch2 header
set(CATCH2_DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../deps/catch2")
if (NOT EXISTS "${CATCH2_DESTINATION}/catch.hpp")
  set(CATCH2_HEADER_URL "https://github.com/catchorg/Catch2/releases/download/${CATCH2_VERSION}/catch.hpp")

  message(STATUS "Download catch2 header from github \"${CATCH2_HEADER_URL}\""
          " to directory ${CATCH2_DESTINATION}")

  FILE(DOWNLOAD "${CATCH2_HEADER_URL}" "${CATCH2_DESTINATION}/catch.hpp"
      STATUS CATCH2_DOWNLOAD_STATUS)

  list(GET CATCH2_DOWNLOAD_STATUS 0 CATCH2_DOWNLOAD_RESULT)

  if (NOT "${CATCH2_DOWNLOAD_RESULT}" EQUAL 0)
    file(REMOVE_RECURSE "${CATCH2_DESTINATION}")
    message(FATAL_ERROR "Failed to download catch header")
  endif()
endif()


#create target for lib boost system
file(GLOB_RECURSE boost_system_src
  "${CMAKE_CURRENT_SOURCE_DIR}/../deps/libs/boost/system/src/*.*")

add_library(boost-system STATIC ${boost_system_src})

target_include_directories(boost-system PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../deps")

#create target for lib boost filesystem
file(GLOB_RECURSE boost_filesystem_src
  "${CMAKE_CURRENT_SOURCE_DIR}/../deps/libs/boost/filesystem/src/*.*")

add_library(boost-filesystem STATIC ${boost_filesystem_src})

target_include_directories(boost-filesystem PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../deps")

target_compile_definitions(boost-filesystem PUBLIC BOOST_FILESYSTEM_NO_LIB)
target_compile_definitions(boost-filesystem PUBLIC BOOST_SYSTEM_NO_LIB)

add_executable(cpp-tests
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/shared_item.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/shared_storage.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/shared_storage.cpp"
  common_process.h
  basis.cpp
  main.cpp
)

target_include_directories(cpp-tests PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../src"
  "${CMAKE_CURRENT_SOURCE_DIR}/../deps"
  "${CMAKE_CURRENT_SOURCE_DIR}/../deps/catch2"
)

target_compile_definitions(cpp-tests PUBLIC BOOST_DATE_TIME_NO_LIB)
target_link_libraries(cpp-tests boost-filesystem boost-system)

# create target for the child process (multi-process tests)
add_executable(child-process
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/shared_item.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/shared_storage.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/shared_storage.cpp"
  common_process.h
  child_process.cpp
)

target_include_directories(child-process PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../src"
  "${CMAKE_CURRENT_SOURCE_DIR}/../deps"
)

target_compile_definitions(child-process PUBLIC BOOST_DATE_TIME_NO_LIB)

add_dependencies(cpp-tests child-process)

if (UNIX AND NOT APPLE)
  find_library(LIB_RT rt)
  if (LIB_RT)
    target_link_libraries(cpp-tests ${LIB_RT})
    target_link_libraries(child-process ${LIB_RT})
  else()
    message(SEND_ERROR "lib rt is missing")
  endif()

  find_library(LIB_PT pthread)
  if (LIB_PT)
    target_link_libraries(cpp-tests ${LIB_PT})
    target_link_libraries(child-process ${LIB_PT})
  else()
    message(SEND_ERROR "lib pthread is missing")
  endif()
endif()
