# - Try to find RTDEBUG
# Once done this will define
#
#  RTDEBUG_FOUND      - True if RTDEBUG was found.
#  RTDEBUG_INCLUDES   - Where to find CMedio.h, etc.
#  RTDEBUG_STATIC_LIB - List of static libraries when using RTDEBUG
#  RTDEBUG_SHARED_LIB - List of shared libraries when using RTDEBUG
#  RTDEBUG_DEFINITIONS - Compiler switches required for using RTDEBUG

# only search for RTDEBUG in debug mode
if(CMAKE_BUILD_TYPE MATCHES Debug)

  if(RTDEBUG_INCLUDES)
    set(RTDEBUG_FIND_QUIETLY TRUE)
  endif(RTDEBUG_INCLUDES)
  
  find_path(RTDEBUG_INCLUDES CRTDebug.h
            PATH_SUFFIXES include/rtdebug
  )
  
  find_library(RTDEBUG_STATIC_LIB NAMES librtdebug${CMAKE_STATIC_LIBRARY_SUFFIX}
               PATH_SUFFIXES lib
  )
  find_library(RTDEBUG_SHARED_LIB NAMES librtdebug${CMAKE_SHARED_LIBRARY_SUFFIX} rtdebug
               PATH_SUFFIXES lib
  )
 
  # set the global DEBUG define while compiling
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

  # handle the QUIETLY and REQUIRED arguments and set RTDEBUG_FOUND to TRUE if
  # all listed variables are TRUE
  include(FindPackageHandleStandardArgs)
  set(RTDEBUG_LIBS ${RTDEBUG_STATIC_LIB} ${RTDEBUG_SHARED_LIB})
  find_package_handle_standard_args(RTDEBUG DEFAULT_MSG RTDEBUG_LIBS RTDEBUG_INCLUDES)
  
  mark_as_advanced(RTDEBUG_STATIC_LIB RTDEBUG_SHARED_LIB RTDEBUG_INCLUDES)

endif()
