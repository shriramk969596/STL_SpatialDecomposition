if(ess_walker_included)
    return()
endif(ess_walker_included)
set(ess_walker_included true)

include(GetPrerequisites)
include(ess_cache)
include(ess_debug)

############################################################################
#
# ESS dependeny walker module
#

function(is_in_system_paths PATH RES)

    if (NOT IS_ABSOLUTE ${PATH})
        get_filename_component(PATH ${PATH} ABSOLUTE)
    endif()

    if (NOT ESS_THIRDPARTY_DIR)
        # this is a bogus scenario, fill the variable to fail comparisons and avoid crash... here" 
        set(ESS_THIRDPARTY_DIR "this project does not use fetch_external")
    else()
        get_filename_component(ESS_THIRDPARTY_DIR ${ESS_THIRDPARTY_DIR} ABSOLUTE)
    endif()    

    string(FIND ${PATH} ${ESS_THIRDPARTY_DIR} FOUND)
    if(NOT ${FOUND} EQUAL -1)
        set(${RES} 0 PARENT_SCOPE)
        return()
    endif()

    string(FIND ${PATH} ${CMAKE_SOURCE_DIR} FOUND)
    if(NOT ${FOUND} EQUAL -1)
        set(${RES} 0 PARENT_SCOPE)
        return()
    endif()

    set(${RES} 1 PARENT_SCOPE)
endfunction()


function(ess_walk_binary FILE RESFILE PASS TAB)

    ess_assert(FILE_EXISTS ${FILE})

    ess_debug_message( "${TAB} binary: ${FILE}")
    ess_debug_message( "${TAB} current pass: ${PASS}")
    set(TAB "${TAB}  ")

    if("${PASS}" STREQUAL "")
        set (PASS_RESFILE ${RESFILE})
    else()
        set (PASS_RESFILE ${RESFILE}_${PASS})
    endif()

    ess_assert(FILE_EXISTS ${PASS_RESFILE})

    # we check if we already visited it, but construct a pair (file - pass)
    # this allows to visit the same file for two different configs (cygwin compiled libs)
    # Edited by Kevin & Johannes: The PASS variable is needed by WIN32, but not for Linux
    if (WIN32)    
        ess_is_cached(${FILE}_${PASS} CACHED)
    else()
        ess_is_cached(${FILE}_${RESFILE} CACHED)
    endif()

    if (${CACHED})
        ess_debug_message("${TAB}Cached, skip")
        return()
    endif()

    if (WIN32)
        get_filename_component(EXT ${FILE} EXT)
        if(${EXT} STREQUAL ".lib")
            get_filename_component(FILENAME ${FILE} NAME_WE)
            get_filename_component(DIRNAME ${FILE} DIRECTORY)
            if(EXISTS ${DIRNAME}/${FILENAME}.dll)
                # follow up dll
                set(FILE ${DIRNAME}/${FILENAME}.dll)
            else()
                # do not copy static libraries
                return()  
            endif()
        endif()
    endif()

    # ignore unix static libraries.
    get_filename_component(EXT ${FILE} EXT)
    if(${EXT} STREQUAL ".a")
        return()
    endif()

    is_in_system_paths(${FILE} SYSTEM_LOC)
    if (SYSTEM_LOC)
        ess_debug_message("ignore file, system install: ${DEP_FILE}")
        return()
    endif()

    ess_debug_message( "${TAB} ----> ${FILE}")
    file(APPEND ${PASS_RESFILE} "${FILE}\n")

    get_filename_component(JUST_NAME ${FILE} NAME_WE)
    get_filename_component(FILE_DIR ${FILE} DIRECTORY)
    if(EXISTS ${FILE_DIR}/${JUST_NAME}.so)
        file(APPEND ${PASS_RESFILE} "${FILE_DIR}/${JUST_NAME}.so\n")
    endif()

    if (IS_SYMLINK ${FILE})
        get_filename_component(FILE ${FILE} REALPATH)
        ess_debug_message( "${TAB} ----> ${FILE}")
        ess_assert(FILE_EXISTS ${FILE})
        file(APPEND ${PASS_RESFILE} "${FILE}\n")
    endif()

	# last minute add the current library path to the search path
    get_filename_component(CURRENT_DIR ${FILE} DIRECTORY)
    list(APPEND ESS_LIB_PATH ${CURRENT_DIR})

    ess_debug_message( "analyze ${FILE} ")
    set(FILE_DEPS "")
    if(WIN32)
        get_prerequisites(${FILE} FILE_DEPS 1 0 "${ESS_LIB_PATH}" "${ESS_LIB_PATH}")
    else()
        get_prerequisites(${FILE} FILE_DEPS 0 0 "${ESS_LIB_PATH}" "${ESS_LIB_PATH}")
    endif()
    foreach(DEP ${FILE_DEPS})

        get_filename_component(FILENAME_ORG ${FILE} NAME)
        get_filename_component(FILENAME_NEW ${DEP} NAME)
        if (${FILENAME_ORG} STREQUAL ${FILENAME_NEW})
            continue()
        endif()

        if(IS_ABSOLUTE ${DEP})
            set(DEP_FILE ${DEP})
        else()
            get_filename_component(FILENAME_WE ${DEP} NAME_WE)
            find_file(${FILENAME_WE}_LOC_${PASS} ${DEP} PATHS ${ESS_LIB_PATH})
            set(DEP_FILE ${${FILENAME_WE}_LOC_${PASS}})
        endif()

        is_in_system_paths(${DEP_FILE} SYSTEM_LOC)
        if (SYSTEM_LOC)
            ess_debug_message("ignore file, system install: ${DEP_FILE}")
            continue()
        endif()

        ess_assert(EXISTS ${DEP_FILE})

        ess_debug_message( "${TAB}   --> ${DEP_FILE}")
        if (NOT EXISTS ${DEP_FILE})
            ess_debug_message("${TAB}does not exist, skip")
            continue()
        endif()

        file(APPEND ${PASS_RESFILE} "${DEP_FILE}\n")

        ess_walk_binary(${DEP_FILE} ${RESFILE} "${PASS}" "  ${TAB}")

        get_filename_component(JUST_NAME ${DEP_FILE} NAME_WE)
        if(EXISTS ${FILE_DIR}/${JUST_NAME}.so)
            file(APPEND ${PASS_RESFILE} "${FILE_DIR}/${JUST_NAME}.so\n")
        endif()

        if (IS_SYMLINK ${DEP_FILE})
            get_filename_component(FILE ${DEP_FILE} REALPATH)
            ess_debug_message( "${TAB}   --> ${FILE}")
            ess_assert(EXISTS ${FILE})
            file(APPEND ${PASS_RESFILE} "${DEP_FILE}\n")
        endif()

    endforeach()

endfunction()


function(ess_walk_target TARGET RESFILE TAB)

    # ignore generator expressions for 
    string(FIND ${TARGET} "$<LINK_ONLY:" FOUND)
    if(NOT ${FOUND} EQUAL -1)
        return()
    endif()

    string(REGEX MATCH "\\$<\\$<BOOL:[^>]+>:([a-zA-Z_0-9]+)>" MATCH ${TARGET})
    if(MATCH)
		string(REGEX REPLACE "\\$<\\$<BOOL:[^>]+>:" "" TARGET  ${TARGET})
		string(REGEX REPLACE ">$" "" TARGET  ${TARGET})
    endif()

    # prevent crashes if malformed target name
    # this should be responsability of the developer
    if(NOT TARGET ${TARGET})
        return()
    endif()

    # do not need header only libs
    get_target_property(TYPE ${TARGET} TYPE)
    if(${TYPE} STREQUAL INTERFACE_LIBRARY)
        return()
    endif()

    get_target_property(IMPORTED ${TARGET} IMPORTED)
    if (${IMPORTED})

        ess_debug_message( "${TAB} imported: ${TARGET}")
        set(TAB " |${TAB}")

        set (READY false)

        # so far this in only needed for visual studio builds, but the 
        # condition is not trivial
        if (WIN32)

            foreach(BUILDTYPE ${CMAKE_CONFIGURATION_TYPES})
                string(TOUPPER ${BUILDTYPE} BUILDTYPE)
                get_target_property(LOC ${TARGET} IMPORTED_LOCATION_${BUILDTYPE})

                # in case we have the default build configs.
                # dependencies may not provide MinSizeRel or  RelWithDebInfo
                # using release is good enough in this cases.
                if (${BUILDTYPE} STREQUAL "MINSIZEREL" OR ${BUILDTYPE} STREQUAL "RELWITHDEBINFO")
                    if(LOC AND NOT EXISTS ${LOC})
                        get_target_property(LOC ${TARGET} IMPORTED_LOCATION_RELEASE)
                    endif()
                endif()

                if(LOC)
                    ess_walk_binary(${LOC} ${RESFILE} ${BUILDTYPE} ${TAB})
                    set(READY true)
                endif()

            endforeach()

        endif()

        # if not Visual stuido or not found
        if (NOT READY)

            get_target_property(IMPLOC ${TARGET} IMPORTED_LOCATION)
            get_target_property(LOC ${TARGET} LOCATION)

            if(IMPLOC)

                ess_walk_binary(${IMPLOC} ${RESFILE} "" ${TAB})

            elseif(LOC)

                ess_walk_binary(${LOC} ${RESFILE} "" ${TAB})

            elseif (CMAKE_BUILD_TYPE)

                string(TOUPPER ${CMAKE_BUILD_TYPE} BUILDTYPE)
                get_target_property(VAR ${TARGET} IMPORTED_LOCATION_${BUILDTYPE})
                if (VAR)
                    ess_walk_binary(${VAR} ${RESFILE} "" ${TAB})
                endif()

            endif()
        endif()

        get_target_property(DEPS ${TARGET} INTERFACE_LINK_LIBRARIES)
        if(DEPS)
            foreach(DEP ${DEPS})
                ess_walk(${DEP} ${RESFILE} ${TAB})
            endforeach()
        endif()

    else()

        ess_debug_message( "${TAB} target: ${TARGET}")
        set(TAB "${TAB}  ")

        get_target_property(LIBS ${TARGET} LINK_LIBRARIES)
        if (LIBS)
            foreach(LIB ${LIBS})
                ess_walk(${LIB} ${RESFILE} ${TAB})
            endforeach()
        endif()

    endif()

endfunction()


function(ess_walk TARGET RESFILE TAB)

    if (TARGET ${WALKER_TARGET})
        ess_walk_target(${TARGET} ${RESFILE} ${TAB})
    elseif(EXISTS ${TARGET})
        ess_walk_binary(${TARGET} ${RESFILE} "" ${TAB})
    else()
        message(FATAL_ERROR "ess_walker: ${TARGET} is no target")
    endif()

endfunction()


function (ess_get_dependencies TARGET DEPS)

    ess_debug_message( "==================================")
    ess_debug_message( "getting dependencies for target: ${TARGET}")
    ess_debug_message( "using paths:")
    foreach(LIB_DIR ${ESS_LIB_PATH})
        ess_debug_message( "   ${LIB_DIR}")
    endforeach()
    ess_debug_message()

    # clean result files
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dep_file "")
    if(WIN32 AND NOT CMAKE_BUILD_TYPE)
        foreach(BUILDTYPE ${CMAKE_CONFIGURATION_TYPES})
	        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dep_file_${BUILDTYPE} "")
        endforeach()
    endif()

	ess_walk(${TARGET} ${CMAKE_CURRENT_BINARY_DIR}/dep_file "  ")

    # default list:
	file(STRINGS ${CMAKE_CURRENT_BINARY_DIR}/dep_file TMP_LIST)
	list(REMOVE_DUPLICATES TMP_LIST)

    set (RES_LIST "")
    foreach(FILE ${TMP_LIST})
        list(APPEND RES_LIST ${FILE})
        
        # some installations contain symlinks with diferent version name:
        # lets just copy all of them for the shake of completeness
        get_filename_component(FILENAME ${FILE} NAME)
        get_filename_component(DIRECTORY ${FILE} DIRECTORY)
        file(GLOB FILES ${DIRECTORY}/${FILENAME}*)
        list(APPEND RES_LIST ${FILES})

    endforeach()
	list(REMOVE_DUPLICATES RES_LIST)

    ess_debug_message("")
    ess_debug_message("dependencies to copy:")
    foreach(DEP ${RES_LIST})
        ess_debug_message( " ${DEP}")
    endforeach()
	
    set(${DEPS} ${RES_LIST} PARENT_SCOPE)
    
    # complement list for windows targets
    # the windows vs (multi configuration) build have debug release
    # and whatnot all together
    if(WIN32 AND NOT CMAKE_BUILD_TYPE)
        foreach(BUILDTYPE ${CMAKE_CONFIGURATION_TYPES})

            if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/dep_file_${BUILDTYPE})
                continue()
            endif()

            # add files for the current configuration
            file(STRINGS ${CMAKE_CURRENT_BINARY_DIR}/dep_file_${BUILDTYPE} TMP_LIST)
            list(REMOVE_DUPLICATES TMP_LIST)

            set (RES_LIST "")
            foreach(FILE ${TMP_LIST})

                ess_assert(FILE_EXISTS ${FILE})
                list(APPEND RES_LIST ${FILE})
                
                # some installations contain symlinks with diferent version name:
                # lets just copy all of them for the shake of completeness
                get_filename_component(FILENAME ${FILE} NAME)
                get_filename_component(DIRECTORY ${FILE} DIRECTORY)
                file(GLOB FILES ${DIRECTORY}/${FILENAME}*)
                list(APPEND RES_LIST ${FILES})

            endforeach()

            # add files with no specific configuration
            file(STRINGS ${CMAKE_CURRENT_BINARY_DIR}/dep_file TMP_LIST)
            list(REMOVE_DUPLICATES TMP_LIST)

            foreach(FILE ${TMP_LIST})

                ess_assert(FILE_EXISTS ${FILE})
                list(APPEND RES_LIST ${FILE})
                
            endforeach()

            # remove duplicates and we are ready to go
            list(REMOVE_DUPLICATES RES_LIST)

            ess_debug_message("")
            ess_debug_message("dependencies to copy (${BUILDTYPE}):")
            foreach(DEP ${RES_LIST})
                ess_debug_message( " ${DEP}")
            endforeach()
            
            set(${DEPS}_${BUILDTYPE} ${RES_LIST} PARENT_SCOPE)
        endforeach()
    endif()

    ess_debug_message( "==================================")

endfunction()
