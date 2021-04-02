if(ess_external_included)
    return()
endif(ess_external_included)
set(ess_external_included true)
include(ess_platform)

cmake_minimum_required(VERSION 3.10)

include(ess_platform)
include(ess_download)
include(ess_manifest)
include(ess_licensing)

############################################################################
#
# ESS C++ dependecies module
#
# The following module was create to facilitate the installation of third party
# modules.
#
# Usage:
#
#     fetch_external(NAME package_name VERSION version_number)
#
# optionaly we can load the package with the built in CMake find_package module.
#
#     fetch_external(NAME package_name VERSION version_numberi FIND_PACKAGE)
#
#
# The packages are precompiled binaries commited a ESS repository, we need to manually 
# maintain such projects.

if(DEFINED $ENV{ESS_THIRDPARTY_DIR})
    set(ESS_PREFIX $ENV{ESS_THIRDPARTY_DIR})
elseif(UNIX)
    set(ESS_PREFIX $ENV{HOME}/.ess-third-party/)
elseif(WIN32)
    string(REGEX MATCH "([A-Z]):.*" RES ${CMAKE_CURRENT_LIST_DIR})
    set(ESS_PREFIX ${CMAKE_MATCH_1}:/.ess-third-party/)
endif()

# download uses git
#find_package(Git REQUIRED)

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
# function to download and configure dependencies

function(ess_fetch_external)

    set(OPTIONS FIND_PACKAGE;UNIX_ONLY;FETCH_LICENSE)
    set(SINGLES NAME;VERSION)
    set(MULTI COMPONENTS;EXTRA_PACKAGES)

    cmake_parse_arguments(FETCH "${OPTIONS}" "${SINGLES}" "${MULTI}" ${ARGN} )

    if (FETCH_UNIX_ONLY AND NOT UNIX)
        return()
    endif()

    string(TOUPPER ${FETCH_NAME} UP_NAME)
    string(TOLOWER ${FETCH_NAME} DW_NAME)
    set (FILENAME ${FETCH_NAME}-${FETCH_VERSION})
    set (SW_PATH ${ESS_THIRDPARTY_DIR}/${FILENAME})
    set (${UP_NAME}_ROOT ${SW_PATH} CACHE PATH "where to find ${FETCH_NAME}")
    set (${UP_NAME}_ROOT_DIR ${SW_PATH} CACHE PATH "where to find ${FETCH_NAME}")
    list(APPEND CMAKE_MODULE_PATH ${UP_NAME}_PATH)
    #set (DEBUG_INFO OUTPUT_QUIET;ERROR_QUIET)
    
    message(STATUS "         ")
    message(STATUS "fetch external: ${FETCH_NAME} ${FETCH_VERSION} ${FETCH_LICENSE}")

    if (${FETCH_VERSION} STREQUAL "SYSTEM")
        ess_fetch_system(${FETCH_NAME} ${FETCH_FIND_PACKAGE})
        return()
    endif()

    # ============= DOWNLOAD PACKAGE ======================================= 
    if (NOT EXISTS ${SW_PATH})

        file(MAKE_DIRECTORY ${SW_PATH})

        ess_dowload (REPO     ${ESS_PACKAGE_REPO}
                     NAME     ${FETCH_NAME}
                     VERSION  ${FETCH_VERSION}
                     FILENAME ${FILENAME}
                     WD       ${SW_PATH}
                     RESULT   RES)

    endif()
    
    # ============= CONFIGURE PACKAGE ======================================= 
    #  some libraries may include more that one package, that is the reason behind
    #  extra_packages. we can configure more than one package downloaded under the
    #  same tarball
   
    set(PACKAGES_LIST "${FETCH_NAME};${FETCH_EXTRA_PACKAGES}")
    foreach(PACKAGE ${PACKAGES_LIST})

        string(TOUPPER ${PACKAGE} UP_NAME)
        string(TOLOWER ${PACKAGE} DW_NAME)

        message (STATUS "looking in ${SW_PATH} for ${DW_NAME}-config.cmake")
        file (GLOB_RECURSE CONFIGFILE ${SW_PATH}/${DW_NAME}-config.cmake )
        message (STATUS "looking in ${SW_PATH} for ${UP_NAME}-config.cmake")
        file (GLOB_RECURSE CONFIGFILE_UP ${SW_PATH}/${UP_NAME}-config.cmake )

        if (EXISTS ${CONFIGFILE})

            message(STATUS "using config file: ${CONFIGFILE}")
            include (${CONFIGFILE})

        elseif (EXISTS ${CONFIGFILE_UP})

            message(STATUS "using config file: ${CONFIGFILE_UP}")
            include (${CONFIGFILE_UP})

        elseif (FETCH_FIND_PACKAGE)

            file (GLOB_RECURSE CONFIGFILE    ${SW_PATH}/${FETCH_NAME}Config.cmake )

            if (NOT ${CONFIGFILE} STREQUAL "")
                get_filename_component(DIR ${CONFIGFILE} DIRECTORY)
                set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${DIR} )
            endif()
            
            set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${SW_PATH} PARENT_SCOPE)
            message(STATUS "  CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")
            message(STATUS "  SW_PATH=${SW_PATH}")
            message(STATUS "  ${UP_NAME}_ROOT=${${UP_NAME}_ROOT}")

            # for the supported ones, use cmake infrastructure
            find_package(${PACKAGE} 
                         ${FETCH_VERSION} EXACT 
                         REQUIRED 
                         COMPONENTS ${FETCH_COMPONENTS}
            )

        else()

            # custom script writen by us stored in the package
            include (${SW_PATH}/${DW_NAME}.cmake)

        endif()

    endforeach()

    # Last but not least, append it to the list of libraries search path,
    # this will make our life much easier for deploy

    if(WIN32)
        set(ESS_LIB_PATH ${ESS_LIB_PATH};${SW_PATH}/lib;${SW_PATH}/bin PARENT_SCOPE)
    else()
        if(${ESS_64_BITS})
            set(ESS_LIB_PATH ${ESS_LIB_PATH};${SW_PATH}/lib;${SW_PATH}/lib64 PARENT_SCOPE)
        else()
            set(ESS_LIB_PATH ${ESS_LIB_PATH};${SW_PATH}/lib PARENT_SCOPE)
        endif()
    endif()

    ess_add_library_to_manifest(${FETCH_NAME} ${FETCH_VERSION})

    if(FETCH_FETCH_LICENSE)
   	   ess_fetch_licenses(${FETCH_NAME} ${FETCH_VERSION})
    endif()
	
endfunction()


############################################################################

# this function is a work around for some corner cases, it will only work in 
# cases where the name of the library matches the package and there is only
# one library included per package
# this was introduced to hack the GMP bug: https://gitlab.essteyr.com/MergeDev/Merge/issues/64
function(ess_fetch_system PACKAGE FIND_PACKAGE)

    if (FIND_PACKAGE)

        find_package(${PACKAGE} REQUIRED)

    else()

        find_library(
            ${PACKAGE}_LIB ${PACKAGE}   
            PATH_SUFFIXES lib lib64   
            DOC "${PACKAGE} library"
        )

        if (NOT ${PACKAGE}_LIB)
           message(FATAL_ERROR "no development packages installed in the system  for ${PACKAGE}")
        endif()

        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dependencies)
        file(COPY ${${PACKAGE}_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/dependencies)
        get_filename_component(LIB_NAME ${${PACKAGE}_LIB} NAME) 

        if (IS_SYMLINK ${${PACKAGE}_LIB})
            get_filename_component(REALFILE ${${PACKAGE}_LIB} REALPATH)
            file(COPY ${REALFILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/dependencies)
        endif()

        # some installations contain symlinks with diferent version name:
        # lets just copy all of them for the shake of completeness
        get_filename_component(FILENAME ${${PACKAGE}_LIB} NAME)
        get_filename_component(DIRECTORY ${${PACKAGE}_LIB} DIRECTORY)
        file(GLOB FILES ${DIRECTORY}/${FILENAME}*)
        #message(FATAL_ERROR ${FILES})
        foreach(FILE ${FILES})
            file(COPY ${FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/dependencies)
        endforeach()

        add_library(${PACKAGE} SHARED IMPORTED)
        set_target_properties(${PACKAGE} PROPERTIES
            IMPORTED_LOCATION  ${CMAKE_CURRENT_BINARY_DIR}/dependencies/${LIB_NAME}
        )
        # note, headers are left to be found by the compiler default paths.. this is far from
        # ideal but we have no means to know the library name.


    endif()

endfunction()
