#
# cloriSearch cmake 
# require cmake >= 2.8.10 && gcc >= 4.8
# Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
#

if(WITH_DEBUG_SYMBOLS)
    SET(DEBUG_SYMBOL "-g")
endif()

configure_file(${PROJECT_SOURCE_DIR}/cmake/def.h.in ${PROJECT_BINARY_DIR}/output/include/internal/def.h @ONLY)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include_directories(${CMAKE_CURRENT_SRC_DIR}
                    ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_SOURCE_DIR}/src/third_party
                    ${PROJECT_BINARY_DIR}/output/include)

set(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} ${DEBUG_SYMBOL}")
set(CMAKE_CXX_FLAGS "${CMAKE_CPP_FLAGS} -O2 -pipe -Wall -W -fPIC -fstrict-aliasing -Wno-invalid-offsetof -Wno-unused-parameter -fno-omit-frame-pointer")

macro(use_cxx11)
if(CMAKE_VERSION VERSION_LESS "3.1.3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
endmacro(use_cxx11)

set(DYNAMIC_LIB pthread)
use_cxx11()

# for *.so output
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output/lib)
# for *.a output
set(CMAKE_ARCHIVE_OUTPUT_DIRECOTRY ${PROJECT_BINARY_DIR}/output/lib)

set(PROJECT_CC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PROJECT_INDEXER_DIR ${PROJECT_SOURCE_DIR}/src/indexer)
set(PROJECT_INTERNAL_DIR ${PROJECT_SOURCE_DIR}/src/internal)
set(PROJECT_TPARTY_DIR ${PROJECT_SOURCE_DIR}/src/third_party)

# add .proto file
include(FindProtobuf)
protobuf_generate_cpp(PROTO_SRC PROTO_HEADER 
        ${PROJECT_CC_DIR}/proto/index_schema.proto
        ${PROJECT_CC_DIR}/proto/inverted_index.proto)
#
# butil is copyed from brpc, and thanks to Baidu Inc.
#
file(GLOB BASIC_SOURCES ${PROJECT_CC_DIR}/*.cc 
                        ${PROJECT_INDEXER_DIR}/*.cc
                        ${PROJECT_INTERNAL_DIR}/*.cc
                        ${PROJECT_TPARTY_DIR}/json2pb/*.cc 
                        ${PROJECT_TPARTY_DIR}/butil/*.cc
                        ${PROJECT_TPARTY_DIR}/butil/*.cpp
                        ${PROJECT_TPARTY_DIR}/butil/third_party/modp_b64/*.cc
                        )
set(PACKAGE_SOURCES ${BASIC_SOURCES} ${PROTO_SRC}) 

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(OBJ_LIB OBJECT ${PACKAGE_SOURCES})
set_property(TARGET ${OBJ_LIB} PROPERTY POSITION_INDEPENTENT_CODE 1)
add_library(clorisearch-shared SHARED $<TARGET_OBJECTS:OBJ_LIB>)
add_library(clorisearch-static STATIC $<TARGET_OBJECTS:OBJ_LIB>)

target_link_libraries(clorisearch-shared ${DYNAMIC_LIB})
set_target_properties(clorisearch-shared PROPERTIES OUTPUT_NAME clorisearch CLEAN_DIRECT_OUTPUT 1)
set_target_properties(clorisearch-static PROPERTIES OUTPUT_NAME clorisearch CLEAN_DIRECT_OUTPUT 1)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output/bin)
set(TUTORIAL_SOURCES ${PROJECT_SOURCE_DIR}/src/example/tutorial.cc)
add_executable(tutorial ${TUTORIAL_SOURCES})
target_link_libraries(tutorial clorisearch-shared protobuf)

file(COPY ${PROJECT_SOURCE_DIR}/bin/
    DESTINATION ${EXECUTABLE_OUTPUT_PATH})

file(COPY ${PROJECT_SOURCE_DIR}/conf/
    DESTINATION ${PROJECT_BINARY_DIR}/output/conf)

# file(COPY ${PROJECT_SOURCE_DIR}/src/config.h DESTINATION ${PROJECT_BINARY_DIR}/output/include/) 
file(COPY ${PROJECT_SOURCE_DIR}/src/internal/
    DESTINATION ${PROJECT_BINARY_DIR}/output/include/internal/
    FILES_MATCHING
    PATTERN "*.h"
    ) 
configure_file(${PROJECT_SOURCE_DIR}/cmake/clorisearch.pc.in ${PROJECT_BINARY_DIR}/output/clorisearch.pc)

# install
install(TARGETS clorisearch-shared 
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(TARGETS clorisearch-static
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY ${PROJECT_BINARY_DIR}/output/include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/clorisearch/
    FILES_MATCHING
    PATTERN "*.h"
    )

install(FILES ${PROJECT_BINARY_DIR}/output/clorisearch.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
