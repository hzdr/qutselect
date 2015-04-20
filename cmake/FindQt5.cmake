# Our own FindQt5.cmake find package which itself tries to use the
# original Qt5 find package from an installed Qt5 version. In addition to
# that it then tries to parse the corresponding *.prl files to find
# out the static lib dependencies that the globally available Qt5
# cmake modules don't extract, unfortunately.

string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "" CMAKE_MODULE_PATH  "${CMAKE_MODULE_PATH}")
find_package(Qt5 REQUIRED ${Qt5_FIND_COMPONENTS})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

macro(Qt_process_prl_file prl_file_location Module_Name)
    if (EXISTS "${prl_file_location}/../${Module_Name}.prl")
        file(STRINGS "${prl_file_location}/../${Module_Name}.prl" prl_strings REGEX "QMAKE_PRL_LIBS")
        if(prl_strings)
          string(REGEX REPLACE "QMAKE_PRL_LIBS *= *([^\\n]*)" "\\1" static_depends ${prl_strings})
          string(STRIP ${static_depends} static_depends)
          set(${Module_Name}_STATIC_LIB_DEPENDENCIES "${_list_sep}${static_depends}")
        endif()
    endif()
endmacro()

foreach(module ${Qt5_FIND_COMPONENTS})
  Qt_process_prl_file(${_qt5_install_prefix} Qt5${module})
  set(Qt5_STATIC_LIB_DEPENDENCIES ${Qt5_STATIC_LIB_DEPENDENCIES} ${Qt5${module}_STATIC_LIB_DEPENDENCIES})
endforeach()

string(STRIP "${Qt5_STATIC_LIB_DEPENDENCIES}" Qt5_STATIC_LIB_DEPENDENCIES)
