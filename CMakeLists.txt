#
# cloriSearch cmake 
# require cmake >= 2.8.10 && gcc >= 4.8
# Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
#
cmake_minimum_required(VERSION 2.8.10)
project(clorisearch CXX)

set(CLORICONF_VERSION 1.0.0)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # require at least gcc 4.8
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC is too old, please install a newer version supporting C++11")
    endif()
else()
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with GCC only")
endif()

# compile options
OPTION(DEBUG "Print debug logs" OFF)
OPTION(WITH_DEBUG_SYMBOLS "With debug symbols" ON)

# install prefix
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr/local/clorisearch" CACHE PATH "clorisearch install prefix" FORCE)
endif()

add_subdirectory(src)

