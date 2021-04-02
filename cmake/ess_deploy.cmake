if(ess_deploy_included)
    return()
endif(ess_deploy_included)
set(ess_deploy_included true)
include(ess_platform)

############################################################################
#
# ESS deploy module
#
# This module is a wrapped arround the CMake install infrastructure, by 
# using this we can generate packages of all the needed tools right from
# the build directory.
#
# How to use: 
#   - Call deploy on every target you want in the output package
#    ess_deploy(TARGETS main library GROUP <group name> [HEADERS headers] [JUST_COPY_RT])
#   - assing the same group name
#   - call install(EXPORT <group name> DESTINATION cmake) at the end of the main cmake script

# some other ess tooling
include (ess_dep_walker)
include (ess_platform)
include (ess_debug)
include (ess_manifest)

# configure packaging 
# ==================

# force debug postfix
set(CMAKE_DEBUG_POSTFIX d)

# forward project version to cpack
if (PROJECT_VERSION)
    set(CPACK_PACKAGE_VENDOR "ESS")
    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
else()
    message(FATAL_ERROR "no version defined")
endif()
set(CPACK_SYSTEM_NAME ${ESS_PLATFORM} CACHE STRING "the plaform suffix for cpack")
include (CPack)

set(PROJECT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE STRING "project version")
set(PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE STRING "project version")
set(PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE STRING "project version")

ess_create_manifest_file()

# copy wrapper that avoids overwrites of newer stuff

option(ESS_FORCE_LIBRARIES_COPY "force copying the dlls during cmake run" off)
function(ess_copy SRC_FILE DST_DIR)

    ess_assert(FILE_EXISTS ${SRC_FILE})
    get_filename_component(FILENAME ${SRC_FILE} NAME)

    if(ESS_FORCE_LIBRARIES_COPY)
        ess_debug_message("${SRC_FILE}    ->       ${DST_DIR}")
        file (COPY ${SRC_FILE} DESTINATION ${DST_DIR})
        return()
    endif()

    if(EXISTS ${DST_DIR}/${FILENAME})
        if(NOT ${SRC_FILE} IS_NEWER_THAN ${DST_DIR}/${FILENAME})
            return()
        endif()
    endif()

    ess_debug_message("${SRC_FILE}    ->       ${DST_DIR}")
    file (COPY ${SRC_FILE} DESTINATION ${DST_DIR})

endfunction()

# create a script in linux to do the LD_LIBRARY_PATH trick
set(ESS_CMAKE_TEMPLATES ${CMAKE_CURRENT_LIST_DIR}/templates CACHE PATH "ess cmake templates")
function(ess_create_run_script TARGET_NAME)

    configure_file (${ESS_CMAKE_TEMPLATES}/run.sh.in
                    ${CMAKE_CURRENT_BINARY_DIR}/ess/${TARGET_NAME}
                    @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ess/${TARGET_NAME}
            PERMISSIONS
                OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE
            DESTINATION bin/..
    )

endfunction()

# deploy function:
# this function makes the target to be available for installation and therefore to be used by cpack.
# as a side effect, it makes dlls available in the runtime folder of the target (windows builds)
function(ess_deploy)

    set(OPTIONS JUST_COPY_RT;WINDOWS_BUILD)
    set(SINGLES GROUP)
    set(MULTI   TARGETS;HEADERS)
    cmake_parse_arguments(DEPLOY "${OPTIONS}" "${SINGLES}" "${MULTI}" ${ARGN} )

    if(WIN32 AND NOT DEPLOY_JUST_COPY_RT)
		if (DEPLOY_WINDOWS_BUILD)
        install(TARGETS ${DEPLOY_TARGETS}
            EXPORT ${DEPLOY_GROUP}
            RUNTIME DESTINATION . COMPONENT runtime
            ARCHIVE DESTINATION . COMPONENT devel
        )
		else()
        install(TARGETS ${DEPLOY_TARGETS}
            EXPORT ${DEPLOY_GROUP}
            RUNTIME DESTINATION bin COMPONENT runtime
            ARCHIVE DESTINATION lib COMPONENT devel
        )		
		endif()
    elseif(NOT DEPLOY_JUST_COPY_RT)
        install(TARGETS ${DEPLOY_TARGETS}
            EXPORT ${DEPLOY_GROUP}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib 
            ARCHIVE DESTINATION lib
        )
    endif()

    install(FILES ${DEPLOY_HEADERS} DESTINATION include)

    export(TARGETS ${DEPLOY_TARGETS} 
       APPEND FILE ${CMAKE_BINARY_DIR}/${DEPLOY_GROUP}.cmake 
       EXPORT_LINK_INTERFACE_LIBRARIES)

    foreach(TRG ${DEPLOY_TARGETS})

        message(STATUS "computing dependencies for ${TRG}...")
        ess_get_dependencies(${TRG} DEPS)

        # define where we want the binaries, only for the windows version
        if(WIN32)
            set_property(TARGET ${TRG} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
            set_property(TARGET ${TRG} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
            set_property(TARGET ${TRG} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
        endif()

        # windows platforms are embarasing, and they need to copy the DLLs to the build folder.
        if(WIN32 AND NOT CMAKE_BUILD_TYPE)

            message(STATUS "copy runtime libraries for ${TRG}...")
            foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})

                if(DEFINED DEPS_${TYPE})
                    ess_debug_message("deps for ${TYPE}")

                    set(RUNTIME_DIR ${CMAKE_BINARY_DIR}/${TYPE})
                    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${TYPE})

                    foreach(DEP ${DEPS_${TYPE}})
                        ess_copy(${DEP} ${RUNTIME_DIR})
						if (DEPLOY_WINDOWS_BUILD)
                            install(FILES ${DEP} DESTINATION . CONFIGURATIONS ${TYPE})
						else()
                            install(FILES ${DEP} DESTINATION bin CONFIGURATIONS ${TYPE})			
						endif()
                    endforeach()

                endif()

            endforeach()

        # other windows builds, like ninja
        elseif(WIN32)
            message(STATUS "copy runtime libraries for ${TRG}...")
            
            foreach(DEP ${DEPS})
                ess_copy(${DEP} ${CMAKE_BINARY_DIR})
            endforeach()
            if (NOT DEPLOY_JUST_COPY_RT)
				if (DEPLOY_WINDOWS_BUILD)
                    install(FILES ${DEPS} DESTINATION .)
				else()
                    install(FILES ${DEPS} DESTINATION bin)
				endif()
			endif()

        else()
            if (NOT DEPLOY_JUST_COPY_RT)
                if (DEPLOY_WINDOWS_BUILD)
                     install(FILES ${DEPS} DESTINATION .)
                else()
                     install(FILES ${DEPS} DESTINATION lib)
                endif()
            endif()
        endif()

        # install dependencies when packaging the code
        if (NOT DEPLOY_JUST_COPY_RT)
            message(STATUS "add dependencies of ${TRG} to install/package step...")
			if (DEPLOY_WINDOWS_BUILD)
                install(FILES ${DEPS} DESTINATION .)	
			else()
                install(FILES ${DEPS} DESTINATION lib)
			endif()
        endif()

        #create run script for unix builds
        if (UNIX AND NOT DEPLOY_JUST_COPY_RT)
            get_target_property(target_type ${TRG} TYPE)
            if (target_type STREQUAL "EXECUTABLE")
                ess_create_run_script(${TRG})
            endif()
        endif()

    endforeach()

endfunction()
