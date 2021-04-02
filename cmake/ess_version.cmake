if(ess_version)
    return()    
endif(ess_version)
set(ess_version true)

set(ESS_CMAKE_TEMPLATES ${CMAKE_CURRENT_LIST_DIR}/templates CACHE PATH "ess cmake templates")

# create a txt file with the project version, this can be used by CI scripts
# to identify the build version
CONFIGURE_FILE(
    ${ESS_CMAKE_TEMPLATES}/version.txt.in
    ${CMAKE_BINARY_DIR}/version.txt
)
install(FILES ${CMAKE_BINARY_DIR}/version.txt DESTINATION .)

# the following function creates and uses a windows resource file to stamp libraries with version
function(ess_version)

    set(OPTIONS "")
    set(SINGLES TARGET)
    set(MULTI   "")
    cmake_parse_arguments(VERSION "${OPTIONS}" "${SINGLES}" "${MULTI}" ${ARGN} )

    if (WIN32)
        ess_assert(VAR_DEFINED VERSION_TARGET MESSAGE "Error: TARGET argument required")

        message(STATUS "generate resource files for ${VERSION_TARGET}")
        message(STATUS "     version tag: ${PROJECT_VERSION_MAJOR},${PROJECT_VERSION_MINOR},${PROJECT_VERSION_PATCH},${ESS_BUILD}")

        set (RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${VERSION_TARGET}.rc)
        if (NOT ESS_BUILD)
            set (ESS_BUILD 0)
        endif()

        get_target_property(OUT_FILE ${VERSION_TARGET} RUNTIME_OUTPUT_NAME)
        set(VERSION_TARGET_FILE ${OUT_FILE})

        configure_file(${ESS_CMAKE_TEMPLATES}/version.rc.in ${RC_FILE})
        target_sources(${VERSION_TARGET} PRIVATE ${RC_FILE})

    endif()

endfunction()


