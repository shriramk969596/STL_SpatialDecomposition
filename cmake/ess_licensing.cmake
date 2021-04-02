if(ess_licensing_included)
    return()
endif(ess_licensing_included)
set(ess_licensing_included true)

# Parameters that can be extended / changed. See description below.
set(license_dir_name "licenses")
set(license_prefix "LICENSE")
set(license_file_name "LICENSE;COPYING;COPYING.GPLv2;COPYING.GPLv3;COPYING.LGPLv2.1;gpl-2.0.txt;lgpl-3.0.txt")

cmake_minimum_required(VERSION 3.10)


if(DEFINED $ENV{ESS_THIRDPARTY_DIR})
    set(ESS_PREFIX $ENV{ESS_THIRDPARTY_DIR})
elseif(UNIX)
    set(ESS_PREFIX $ENV{HOME}/.ess-third-party/)
elseif(WIN32)
    string(REGEX MATCH "([A-Z]):.*" RES ${CMAKE_CURRENT_LIST_DIR})
    set(ESS_PREFIX ${CMAKE_MATCH_1}:/.ess-third-party/)
endif()

# Default third party locations
if (NOT ESS_THIRDPARTY_DIR)
    set(ESS_THIRDPARTY_DIR ${ESS_PREFIX}/${ESS_PLATFORM}/ CACHE PATH "where the third party libraries are to be found")
    message("WARING: third party software path defaulted to: ${ESS_THIRDPARTY_DIR}")
    message("        use -DESS_THIRDPARTY_DIR=path to change it on a fresh build")
endif()
file(MAKE_DIRECTORY ${ESS_THIRDPARTY_DIR})

set (ESS_PACKAGE_REPO "ssh://git@gitlab.essteyr.com:2222/Dependencies"
    CACHE STRING "location where to fecth the precompiled packages from"
)

set(ESS_LIB_PATH "" CACHE INTERNAL "to contain a list of locations where to find libraries")


############################################################################
# function to fetch licenses
#
# This function is called only, if the option FETCH_LICENSE is passed to the 
# ess_fetch_external command.
# 
# This function creates the directory ${license_dir_name} if it does not exist.
#
# This function copies any license file with name contained in the list ${license_file_name}
# to the directory ${license_dir_name} and renames it to ${license_prefix}-${NAME}-${VERSION}
#
# If more than one license file exists in ${ESS_THIRDPARTY_DIR}/${NAME}-${VERSION}/ than all
# licenses are copied to ${license_dir_name} and named ${license_prefix}_<running number>-${NAME}-${VERSION}
#
# If no license file is found, then a warning message is produced
#

function(ess_fetch_licenses NAME VERSION)

	# create directory for License file if it does not exist
	if(NOT EXISTS ${CMAKE_BINARY_DIR}/${license_dir_name})
  	   file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${license_dir_name})
	endif()

    # License might be available in different names. Iterate through all names defined in list ${license_file_name}
	# If a file of this name is found, copy it to ${CMAKE_BINARY_DIR}/${license_dir_name}/${license_prefix}-${NAME}-${VERSION}
	# If two matching files are found, they are numbered by ${CMAKE_BINARY_DIR}/${license_dir_name}/${license_prefix}_<running number>-${NAME}-${VERSION}
	set(file_found 0)
	
	foreach(license_name IN LISTS license_file_name)
	    if(EXISTS "${ESS_THIRDPARTY_DIR}/${NAME}-${VERSION}/${license_name}")
		    if(file_found EQUAL 0)
				file(COPY "${ESS_THIRDPARTY_DIR}/${NAME}-${VERSION}/${license_name}"
						DESTINATION "${CMAKE_BINARY_DIR}/${license_dir_name}/"
						)
				file(RENAME ${CMAKE_BINARY_DIR}/${license_dir_name}/${license_name} ${CMAKE_BINARY_DIR}/${license_dir_name}/${license_prefix}-${NAME}-${VERSION}.txt)
				set(file_found 1)
			else()
				MATH(EXPR file_found "${file_found}+1")
				file(COPY "${ESS_THIRDPARTY_DIR}/${NAME}-${VERSION}/${license_name}"
						DESTINATION "${CMAKE_BINARY_DIR}/${license_dir_name}/"
						)
				file(RENAME ${CMAKE_BINARY_DIR}/${license_dir_name}/${license_name} ${CMAKE_BINARY_DIR}/${license_dir_name}/${license_prefix}_${file_found}-${NAME}-${VERSION}.txt)
			endif()
		endif()
	endforeach()
	
	# produce a warning message, if no license is found
	if(file_found EQUAL 0)
		message('No License file found for ${ESS_THIRDPARTY_DIR}/${NAME}-${VERSION}')
	endif()

endfunction()
