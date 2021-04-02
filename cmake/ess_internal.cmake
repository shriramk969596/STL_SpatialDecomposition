if(ess_internal_included)
    return()
endif(ess_internal_included)
set(ess_internal_included true)
include(ess_platform)
include(ess_manifest)

cmake_minimum_required(VERSION 3.10)

############################################################################
#
# ESS C++ dependecies module
#
# The following module was create to facilitate the installation of third party
# modules.
#
# Usage:
#
#     fetch_internal(NAME package_name VERSION version_number)
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


set(ESS_LIB_PATH "" CACHE INTERNAL "to contain a list of locations where to find libraries")
option(ESS_FORCE_INTERNAL_DOWNLOAD "prevent from using the catched libraries and fetch them allways fresh" on)


# function to download and configure dependencies
function(ess_fetch_internal)

    set(OPTIONS UNIX_ONLY)
    set(SINGLES NAME;VERSION;LOCATION)
    set(MULTI COMPONENTS)
    cmake_parse_arguments(FETCH "${OPTIONS}" "${SINGLES}" "${MULTI}" ${ARGN} )

    if (FETCH_UNIX_ONLY AND NOT UNIX)
        return()
    endif()

    string(TOUPPER ${FETCH_NAME} UP_NAME)
    string(TOLOWER ${FETCH_NAME} DW_NAME)
    set (FILENAME ${FETCH_NAME}-${FETCH_VERSION})
    set (SW_PATH ${ESS_THIRDPARTY_DIR}/${FILENAME})
    list(APPEND CMAKE_MODULE_PATH ${UP_NAME}_PATH)
    #set (DEBUG_INFO OUTPUT_QUIET;ERROR_QUIET)

    if(DEFINED FETCH_LOCATION)
        set (REPO_LOCATION "ssh://git@gitlab.essteyr.com:2222/${FETCH_LOCATION}")
    else()
        set (REPO_LOCATION "ssh://git@gitlab.essteyr.com:2222/Dependencies")
    endif()

    message(STATUS "         ")
    if (DEFINED ${UP_NAME}_BUILD_DIR)
        message(STATUS "NOTE: using a development build for ${FETCH_NAME}:")
        message(STATUS "      ${UP_NAME}_BUILD_DIR := ${${UP_NAME}_BUILD_DIR}")

    else ()
        message(STATUS "fetch internal: ${FETCH_NAME} ${FETCH_VERSION}")
        message(STATUS "NOTE: ${UP_NAME}_BUILD_DIR was not set:")
        message(STATUS "      Downloading latest precompiled version for ${FETCH_NAME}") 
        message(STATUS "      in ${SW_PATH} ")
        message(STATUS "      you can change to a development version by calling cmake with:")
        message(STATUS "         -D${UP_NAME}_BUILD_DIR=<${FETCH_NAME} build folder>")
        

        if(ESS_FORCE_INTERNAL_DOWNLOAD AND EXISTS ${SW_PATH} AND ${FETCH_VERSION} STREQUAL "latest")
            if(EXISTS ${SW_PATH}_backup)
                file(REMOVE_RECURSE ${SW_PATH}_backup)
            endif()
            file(RENAME ${SW_PATH} ${SW_PATH}_backup)
            message(STATUS "NOTE: force donwload of internal libraries is on, deactivate with: -DESS_FORCE_INTERNAL_DOWNLOAD=off")
            message(STATUS "      latest checkout if ${FETCH_NAME} is backed up in:")
            message(STATUS "      ${SW_PATH}_backup")
        endif()

        # ============= DOWNLOAD PACKAGE ======================================= 
        if (NOT EXISTS ${SW_PATH})

            file(MAKE_DIRECTORY ${SW_PATH})
            ess_dowload (REPO     ${REPO_LOCATION}
                         NAME     ${FETCH_NAME}
                         VERSION  ${FETCH_VERSION}
                         FILENAME ${FILENAME}
                         WD       ${SW_PATH}
                         RESULT   RES)


        endif()
    endif()
    
    # ============= CONFIGURE PACKAGE ======================================= 
    
    if (DEFINED ${UP_NAME}_BUILD_DIR)
        set(CONFIGFILE ${${UP_NAME}_BUILD_DIR}/${FETCH_NAME}.cmake)
    else()
        message (STATUS "looking in ${SW_PATH} for ${FETCH_NAME}.cmake")
        file (GLOB_RECURSE CONFIGFILE ${SW_PATH}/${FETCH_NAME}.cmake)
    endif()

    if(NOT WIN32)
        if (NOT EXISTS ${CONFIGFILE})
            message(FATAL_ERROR "no config file found for ${FETCH_NAME}")
        endif()
    endif()

    message(STATUS "using config file: ${CONFIGFILE}")
    include (${CONFIGFILE})


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

    ess_add_dependecy_to_manifest(${FETCH_NAME} ${FETCH_VERSION} ${SW_PATH})


endfunction()
