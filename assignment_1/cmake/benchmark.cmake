include(ExternalProject)

#set(GB_ROOT         ${CMAKE_BINARY_DIR}/benchmark)
#set(GB_LIB_DIR      ${GB_ROOT}/lib)
#set(GB_INCLUDE_DIR  ${GB_ROOT}/include)

set(GB_URL          ${CMAKE_SOURCE_DIR}/3rd-party/benchmark.tar.gz)
set(GB_CONFIGURE    cd ${CMAKE_BINARY_DIR}/benchmark &&
                    mkdir build && 
                    cd ${CMAKE_BINARY_DIR}/benchmark/build &&
                    cmake -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/benchmark/build ../)
set(GB_MAKE         cd ${CMAKE_BINARY_DIR}/benchmark/build && make -j8)
set(GB_INSTALL      cd ${CMAKE_BINARY_DIR}/benchmark/build && make install)

#message(STATUS "CMAKE SOURCE DIR ${CMAKE_SOURCE_DIR}")
#message(STATUS "CMAKE BINARY DIR ${CMAKE_BINARY_DIR}")
#message(STATUS "GB ROOT ${GB_ROOT}")

ExternalProject_Add(benchmark
  URL                ${GB_URL}
  SOURCE_DIR         ${CMAKE_BINARY_DIR}/benchmark
#  PREFIX             ${CMAKE_BINARY_DIR}/benchmark
  CONFIGURE_COMMAND  ${GB_CONFIGURE}
  BUILD_COMMAND      ${GB_MAKE}
  INSTALL_COMMAND    ${GB_INSTALL}
)

add_library(gbenchmark STATIC IMPORTED)
set_property(TARGET gbenchmark PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/benchmark/build/lib/libbenchmark.a)
add_dependencies(gbenchmark benchmark)
