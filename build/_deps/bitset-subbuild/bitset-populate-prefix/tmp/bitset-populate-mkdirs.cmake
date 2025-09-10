# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/workspace/build/_deps/bitset-src")
  file(MAKE_DIRECTORY "/workspace/build/_deps/bitset-src")
endif()
file(MAKE_DIRECTORY
  "/workspace/build/_deps/bitset-build"
  "/workspace/build/_deps/bitset-subbuild/bitset-populate-prefix"
  "/workspace/build/_deps/bitset-subbuild/bitset-populate-prefix/tmp"
  "/workspace/build/_deps/bitset-subbuild/bitset-populate-prefix/src/bitset-populate-stamp"
  "/workspace/build/_deps/bitset-subbuild/bitset-populate-prefix/src"
  "/workspace/build/_deps/bitset-subbuild/bitset-populate-prefix/src/bitset-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/workspace/build/_deps/bitset-subbuild/bitset-populate-prefix/src/bitset-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/workspace/build/_deps/bitset-subbuild/bitset-populate-prefix/src/bitset-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
