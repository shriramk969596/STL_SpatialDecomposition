if(ess_platform)
    return()
endif(ess_platform)
set(ess_platform true)

############################################################################
#
# ESS C++ platform module
#
# The following module was designed to detect the platform we are currently targeting
# cross compiling is not currently supported
#

# ============================================================
# ========  Detect Compiler (for the ABI, and libc version) 
# ============================================================
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	# using Clang


elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# using GCC

	if (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.4)
		message(FATAL_ERROR "we require GCC 4.4, your version is ${CMAKE_CXX_COMPILER_VERSION}")
	endif()


elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	# using Intel C++


elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# using Visual Studio C++

	if (MSVC_VERSION LESS 1600)
		message(FATAL_ERROR "This software requires vs 2010 or newer")
	elseif (MSVC_VERSION LESS 1700)
		set(ESS_VS_VERSION "vc100" CACHE STRING "vs codename")
	elseif (MSVC_VERSION LESS 1800)
		set(ESS_VS_VERSION "vc110" CACHE STRING "vs codename")
	elseif (MSVC_VERSION LESS 1900)
		set(ESS_VS_VERSION "vc120" CACHE STRING "vs codename")
	elseif (MSVC_VERSION LESS 1910)
		set(ESS_VS_VERSION "vc140" CACHE STRING "vs codename")
	elseif (MSVC_VERSION LESS 1920)
		set(ESS_VS_VERSION "vc141" CACHE STRING "vs codename")
	elseif (MSVC_VERSION LESS 1930)
		set(ESS_VS_VERSION "vc142" CACHE STRING "vs codename")
	elseif (MSVC_VERSION GREATER_EQUAL 1930)
		# https://cmake.org/cmake/help/latest/variable/MSVC_VERSION.html
		message(FATAL_ERROR "Please update this list")
	endif()

endif()

# ============================================================
# ===========  Detect word size  
# ============================================================

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	# 64 bits
	set(ESS_64_BITS on CACHE BOOL "")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
	set(ESS_64_BITS off CACHE BOOL "")
endif()

# ============================================================
# ===========  Detect Operating system  
# ============================================================

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

if (LINUX)

	find_program(LSB_RELEASE lsb_release)
	if(NOT ${LSB_RELEASE} STREQUAL LSB_RELEASE-NOTFOUND ) 

		# if lsb_release is present we may be on some ubuntu
		execute_process(COMMAND ${LSB_RELEASE} -is
			OUTPUT_VARIABLE DISTRO_NAME
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		execute_process(COMMAND ${LSB_RELEASE} -rs
			OUTPUT_VARIABLE DISTRO_VERSION
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)

 	elseif(EXISTS /etc/centos-release)

 	    file(READ /etc/centos-release RELEASE_STR)
        string(REGEX MATCH "CentOS (Linux )?release ([0-9\.]+) .*" OUT ${RELEASE_STR})
        
        set(DISTRO_NAME CentOS)
        set(DISTRO_VERSION ${CMAKE_MATCH_2})

	else()

		message(FATAL_ERROR "
Current Linux distribution could not be detectected
please install lsb_release to facilitate detection")

	endif()

    set(ESS_DISTRO_NAME ${DISTRO_NAME} CACHE STRING "OS name")
    set(ESS_DISTRO_VERSION ${DISTRO_VERSION} CACHE STRING "OS version")
    string(REGEX MATCH "([0-9\]+)\..*" OUT ${DISTRO_VERSION})
    set(ESS_DISTRO_SHORT_VERSION ${CMAKE_MATCH_1} CACHE STRING "OS short version")
	set(ESS_PLATFORM_OS ${DISTRO_NAME}${DISTRO_VERSION} CACHE STRING  "OS version name")

elseif(WIN32)

	set(ESS_PLATFORM_OS win_${ESS_VS_VERSION} CACHE STRING "OS version name")

else()

    message(FATAL_ERROR "other plarforms not yet supported")

endif()

# ============================================================
# ============ Detect Word architecture
# ============================================================

set(ESS_PLATFORM_ARCH ${CMAKE_SYSTEM_PROCESSOR} CACHE STRING  "Platform architecture")
set(ESS_PLATFORM ${ESS_PLATFORM_OS}_${ESS_PLATFORM_ARCH} CACHE STRING  "Platform string")

# ============================================================
# ============ print debug
# ============================================================

if (ESS_CMAKE_DEBUG)
    message(STATUS "ESS_PLATFORM_OS:   ${ESS_PLATFORM_OS}")
    message(STATUS "ESS_PLATFORM_ARCH: ${ESS_PLATFORM_ARCH}")
    message(STATUS "ESS_PLATFORM:      ${ESS_PLATFORM}")
endif()




