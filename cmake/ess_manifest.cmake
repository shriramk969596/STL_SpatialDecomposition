if(ess_manifest_included)
    return()
endif(ess_manifest_included)
set(ess_manifest_included true)

include(ess_version)

set(MANIFEST_FILE ${CMAKE_BINARY_DIR}/manifest.txt CACHE INTERNAL "manifest location")


# creates a manifest file for the current build
function(ess_create_manifest_file)

    # print basic info
    file(WRITE ${MANIFEST_FILE} "project:\n")
    file(APPEND ${MANIFEST_FILE} "  name: ${PROJECT_NAME}\n")
    file(APPEND ${MANIFEST_FILE} "  version: ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}\n")
    file(APPEND ${MANIFEST_FILE} "  platform: ${ESS_PLATFORM}\n")

    # get git hash
    execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    file(APPEND ${MANIFEST_FILE} "  commit: ${GIT_COMMIT_HASH}\n")

    # get build type
    if (MSVC AND NOT CMAKE_BUILD_TYPE)
        set (BT "visual studio build")
    else()
        if (NOT CMAKE_BUILD_TYPE)
            set (BT Debug)
        else()
            set (BT ${CMAKE_BUILD_TYPE})
        endif()
    endif()
    file(APPEND ${MANIFEST_FILE} "  build type: ${BT}\n")

    # this code detects whenever is a development build or a build done by the CI
    # this is important becase local builds may contain untracked changes, pdb files may be lost as well
    if (ESS_BUILD_ID)
        set(BUILD_ID ${ESS_BUILD_ID})
    else()
        set(BUILD_ID "development build")
    endif()
    file(APPEND ${MANIFEST_FILE} "  build id: ${BUILD_ID}\n")

    # print dependencies info
    file(APPEND ${MANIFEST_FILE} "\n")
    file(APPEND ${MANIFEST_FILE} "  depends:\n")

    install(FILES ${MANIFEST_FILE} DESTINATION .)
endfunction()

# to be called by ess_fetch_internal
# looks for the manifest file of such library and nests it in the current manifest
function(ess_add_dependecy_to_manifest NAME VERSION DEP_PATH)


    if (EXISTS ${DEP_PATH}/manifest.txt)

        file(STRINGS ${DEP_PATH}/manifest.txt MANIFEST)
        foreach(LINE ${MANIFEST})
            file(APPEND ${MANIFEST_FILE} "    ${LINE}\n")
        endforeach()


    else()
        file(APPEND ${MANIFEST_FILE} "    - ${NAME}-${VERSION}\n")

    endif()




endfunction()


# to be called by ess_fetch_external
# adds the library name plus requested version to the manifest
function(ess_add_library_to_manifest NAME VERSION)
    file(APPEND ${MANIFEST_FILE} "    - ${NAME}-${VERSION}\n")
endfunction()


function (ess_print_summary)

    message(STATUS "======= BUILD SUMMARY =============")

    file(STRINGS ${MANIFEST_FILE} TEXT)
    foreach(IT ${TEXT})
        message(STATUS ${IT})
    endforeach()

    message(STATUS "===================================")

endfunction()
