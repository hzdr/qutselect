#/* vim:set ts=2 nowrap: ****************************************************
#
# qutselect - A simple Qt-based GUI frontend for remote terminals
# Copyright (C) 2009-2015 Jens Maus <mail@jens-maus.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# $Id$
#
#**************************************************************************/

cmake_minimum_required(VERSION 3.22.1)

# Default configuration values. These must be before the project command or
# they won't work in Windows.
# If no build type is specified, default to "Release"
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "None Debug Release RelWithDebInfo MinSizeRel"
      FORCE)
endif()

# define the project and the version number
project(qutselect)
set(PROJECT_LONGNAME "qutselect")
set(PROJECT_VERSION_MAJOR 3)
set(PROJECT_VERSION_MINOR 13)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

# change output directory to top-level 'bin' dir.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Global CMake options
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# set the cmake modules path
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# In case we are cross compiling we have to setup
# everything for different search paths
if(CMAKE_CROSS_COMPILING)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
  set(PETLIB_PATH "/usr/local/petlib/${CROSS_OS}")
else()
  set(PETLIB_PATH "/usr/local/petlib")
endif()

# if /usr/local/petlib is found use that one for install and search prefix
if(EXISTS ${PETLIB_PATH})
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PETLIB_PATH})
  if(CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX ${PETLIB_PATH})
  endif()
endif()

# Configure third party packages
find_package(Qt6 REQUIRED Core Gui Widgets Network)
find_package(RTDEBUG REQUIRED)

# add some special libraries in case we are build for windows
# WARNING: This seems to be a workaround because Qt5 itself should
# include them?!?!
if(WIN32 AND CMAKE_CROSS_COMPILING)
  if(${CMAKE_C_COMPILER} MATCHES "static-gcc")
    set(DEPEND_STATIC_LIB ${Qt5_STATIC_LIB_DEPENDENCIES})
    set(RTDEBUG_SHARED_LIB ${RTDEBUG_STATIC_LIB})
  endif()
endif()

# make sure to require Qt6
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)

# Set some better optimization options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE, "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO, "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -g -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG, "${CMAKE_CXX_FLAGS_RELEASE} -O0")

add_subdirectory(src)
