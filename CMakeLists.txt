cmake_minimum_required(VERSION 3.5.2 FATAL_ERROR)
set(BUILD_SHARED_LIBS ON)

# Set cmake policy by version: https://cmake.org/cmake/help/latest/manual/cmake-policies.7.html
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.12)
endif()

project(Common
  VERSION 0.0.0
  DESCRIPTION "O2 Common library"
  LANGUAGES C CXX
)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

# Set the default build type to "RelWithDebInfo"
if(NOT CMAKE_BUILD_TYPE)
  set(
    CMAKE_BUILD_TYPE "RelWithDebInfo"
    CACHE
      STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel Coverage."
    FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set CMAKE_INSTALL_LIBDIR explicitly to lib (to avoid lib64 on CC7)
set(CMAKE_INSTALL_LIBDIR lib)

####################################
# Dependencies
####################################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Boost 1.56
  COMPONENTS unit_test_framework
             program_options
             filesystem
             REQUIRED)


set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

####################################
# Library
####################################

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(INCLUDE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/include")

add_library(Common
            SHARED
            src/Daemon.cxx
            src/Exception.cxx
            src/Iommu.cxx
            src/LineBuffer.cxx
            src/Program.cxx
            src/SimpleLog.cxx
            src/SuffixNumber.cxx
            src/System.cxx
            src/Thread.cxx
            src/Timer.cxx
            src/Configuration.cxx
            src/DataBlock.cxx
            src/DataBlockContainer.cxx
            src/MemPool.cxx)

# Produce the final Version.h using template Version.h.in and substituting
# variables. We don't want to pollute our source tree with it, thus we put it in
# the build tree.
configure_file("include/Common/Version.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/include/Common/Version.h" @ONLY)

target_link_libraries(Common
                      PUBLIC Boost::boost
                             Boost::filesystem
                             Boost::program_options
                             Threads::Threads)
target_include_directories(
  Common
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>)


add_subdirectory(doc)


####################################
# Tests
####################################
enable_testing()

add_executable(testDataFormat test/testDataFormat.c)
target_link_libraries(testDataFormat Common)
set_source_files_properties(test/testDataFormat.c PROPERTIES LANGUAGE CXX)
add_test(NAME testDataFormat COMMAND testDataFormat)
set_tests_properties(testDataFormat PROPERTIES TIMEOUT 60)

add_executable(testSimpleLog test/testSimpleLog.cxx)
target_link_libraries(testSimpleLog Common)

set(TEST_SRCS
  test/TestBasicThread.cxx
  test/testFifo.cxx
  test/TestIommu.cxx
  test/TestSuffixNumber.cxx
  test/TestSuffixOption.cxx
  test/TestSystem.cxx
  test/testMemPool.cxx
  test/testTimer.cxx
)

foreach (test ${TEST_SRCS})
  get_filename_component(test_name ${test} NAME)
  string(REGEX REPLACE ".cxx" "" test_name ${test_name})

  add_executable(${test_name} ${test})
  target_link_libraries(${test_name}
    PRIVATE
      Common Boost::unit_test_framework
  )
  add_test(NAME ${test_name} COMMAND ${test_name})
  set_tests_properties(${test_name} PROPERTIES TIMEOUT 60)
endforeach()


####################################
# Install
####################################

include(GNUInstallDirs)

# Build targets with install rpath on Mac to dramatically speed up installation
# https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/RPATH-handling
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}")
  endif()
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()
unset(isSystemDir)

# Install library
install(TARGETS Common
  EXPORT CommonTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/Common DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(EXPORT CommonTargets
        FILE CommonTargets.cmake
        NAMESPACE AliceO2::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Common")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(CommonConfigVersion.cmake
                                 VERSION ${Common_VERSION}
                                 COMPATIBILITY SameMajorVersion)
configure_package_config_file(cmake/CommonConfig.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/CommonConfig.cmake"
                              INSTALL_DESTINATION
                              "${CMAKE_INSTALL_LIBDIR}/cmake/Common")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CommonConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/CommonConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Common)
