cmake_minimum_required(VERSION 3.5.2 FATAL_ERROR)

project(Common VERSION 0.0.0 LANGUAGES C CXX)

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

# Add compiler flags for warnings and (more importantly) fPIC and debug symbols
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -fPIC")

find_package(Boost 1.56
             COMPONENTS unit_test_framework
                        program_options
                        filesystem
                        REQUIRED)
find_package(TCL)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

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

# Some extra targets to build when cmake -DBUILD_STANDALONE=1 ...
if(BUILD_STANDALONE)
  foreach(modul Configuration Daemon LineBuffer SimpleLog Thread Timer)
    # add_library(objCommon${modul} OBJECT src/${modul}.cxx)
    # target_link_libraries(objCommon${modul} PRIVATE Common)
    add_library(Common${modul} src/${modul}.cxx)
    target_link_libraries(Common${modul} PRIVATE Common)
  endforeach(modul)
endif(BUILD_STANDALONE)

add_subdirectory(doc)

enable_testing()

add_executable(testDataFormat test/testDataFormat.c)
target_link_libraries(testDataFormat Common)
set_source_files_properties(test/testDataFormat.c PROPERTIES LANGUAGE CXX)
add_test(testDataFormat testDataFormat)

add_executable(testTimer test/testTimer.cxx)
target_link_libraries(testTimer Common Boost::unit_test_framework)
add_test(testTimer testTimer)

foreach(BTEST
        TestBasicThread
        testFifo
        TestIommu
        TestSuffixNumber
        TestSuffixOption
        TestSystem
        testMemPool)
  add_executable(${BTEST} test/${BTEST}.cxx)
  target_link_libraries(${BTEST} Common Boost::unit_test_framework)
  add_test(${BTEST} ${BTEST})
endforeach()

# Library for Tcl. Usually we don't allow multiple libraries in O2 modules. This
# is an exception.
if(TCL_FOUND)
  add_library(libtclConfiguration
              SHARED
              src/Configuration.cxx
              src/Configuration_tcl_wrap.cxx)
  target_link_libraries(libtclConfiguration PUBLIC Boost::boost ${TCL_LIBRARY})
  target_include_directories(
    libtclConfiguration
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>)
endif()

# Installation of targets and relevant files for other cmake packages that will
# depend on us
install(TARGETS Common EXPORT CommonTargets LIBRARY DESTINATION lib)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT CommonTargets
        FILE CommonTargets.cmake
        NAMESPACE Common::
        DESTINATION lib/cmake/Common)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(CommonConfigVersion.cmake
                                 VERSION ${Common_VERSION}
                                 COMPATIBILITY SameMajorVersion)
configure_package_config_file(cmake/CommonConfig.cmake.in
                              ${CMAKE_CURRENT_BINARY_DIR}/CommonConfig.cmake
                              INSTALL_DESTINATION
                              lib/cmake/Common)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CommonConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/CommonConfigVersion.cmake
        DESTINATION lib/cmake/Common)
