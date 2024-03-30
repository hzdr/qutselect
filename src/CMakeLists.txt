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

# configure files where variables should be replaced.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)

# specify include directories
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${RTDEBUG_INCLUDES}
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/rtdebug
)

# consider all *.cpp files found to be actual code files
file(GLOB_RECURSE SOURCE_FILES *.cpp)

# add the qt resource files
qt6_add_resources(UI_RESOURCES qutselect.qrc)

# specify the executable we are going to build
add_executable(qutselect ${SOURCE_FILES}
                         ${UI_RESOURCES}
)

# define link libraries dependencies
target_link_libraries(qutselect Qt6::Core 
                                Qt6::Gui
                                Qt6::Widgets
                                Qt6::Network
                                ${RTDEBUG_STATIC_LIB} 
                                ${DEPEND_STATIC_LIB}
)

# set target properties so that install only installs with
# version postfix
set_target_properties(qutselect PROPERTIES OUTPUT_NAME qutselect-${PROJECT_VERSION})

# definition of install targets
install(TARGETS qutselect
        RUNTIME DESTINATION bin
)
