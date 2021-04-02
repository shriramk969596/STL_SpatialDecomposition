if(ess_dowload_included)
    return()
endif(ess_dowload_included)
set(ess_dowload_included true)



include(ess_platform)



function(ess_dowload_aux)
    
    set(OPTIONS "")
    set(SINGLES REPO;NAME;VERSION;FILENAME;WD;RESULT;PLATFORM)
    set(MULTI   "")
    cmake_parse_arguments(DW "${OPTIONS}" "${SINGLES}" "${MULTI}" ${ARGN} )
    

    SET(CMD git archive --format=tar.gz --remote ${DW_REPO}/${DW_NAME} HEAD:${DW_VERSION}/${DW_PLATFORM} -o ${DW_FILENAME}.tar.gz)
    
    if(ESS_CMAKE_DEBUG)
        message(STATUS "    fetch:  ${CMD}")
    endif()

    execute_process(
        COMMAND
            ${CMD}
        WORKING_DIRECTORY ${DW_WD}
        RESULT_VARIABLE RES
    )

    set(${DW_RESULT} ${RES} PARENT_SCOPE)

endfunction()




function(ess_dowload)

    set(OPTIONS "")
    set(SINGLES REPO;NAME;VERSION;FILENAME;WD;PLATFORM)
    set(MULTI   "")
    cmake_parse_arguments(DW "${OPTIONS}" "${SINGLES}" "${MULTI}" ${ARGN} )

    message(STATUS "    Attempt download ${DW_FILENAME} for ${ESS_PLATFORM}...")

    ess_dowload_aux (REPO     ${DW_REPO}
                     NAME     ${DW_NAME}
                     VERSION  ${DW_VERSION}
                     FILENAME ${DW_FILENAME}
                     FILENAME ${DW_FILENAME}
                     WD       ${DW_WD}
                     RESULT   RES
                     PLATFORM ${ESS_PLATFORM})

    # if not available try with the short version
    if (${RES} EQUAL 0)

        message(STATUS "    ...success")

    else ()
    
        set(SHORT_PLATFORM "${ESS_DISTRO_NAME}${ESS_DISTRO_SHORT_VERSION}_${ESS_PLATFORM_ARCH}")
        message(STATUS "    Attempt download ${DW_FILENAME} for ${SHORT_PLATFORM}...")
        ess_dowload_aux (REPO     ${DW_REPO}
                         NAME     ${DW_NAME}
                         VERSION  ${DW_VERSION}
                         FILENAME ${DW_FILENAME}
                         FILENAME ${DW_FILENAME}
                         WD       ${DW_WD}
                         RESULT   RES
                         PLATFORM ${SHORT_PLATFORM})
        if (${RES} EQUAL 0)
            message(STATUS "    ...success")
        else()

            set(SHORT_PLATFORM "${ESS_DISTRO_NAME}_${ESS_PLATFORM_ARCH}")
            message(STATUS "    Attempt download ${DW_FILENAME} for ${SHORT_PLATFORM}...")
            ess_dowload_aux (REPO     ${DW_REPO}
                    NAME     ${DW_NAME}
                    VERSION  ${DW_VERSION}
                    FILENAME ${DW_FILENAME}
                    FILENAME ${DW_FILENAME}
                    WD       ${DW_WD}
                    RESULT   RES
                    PLATFORM ${SHORT_PLATFORM})
            if (${RES} EQUAL 0)
                message(STATUS "    ...success")
            else()

                message(STATUS "    Attempt download ${DW_FILENAME} for Universal...")
                ess_dowload_aux (REPO     ${DW_REPO}
                                 NAME     ${DW_NAME}
                                 VERSION  ${DW_VERSION}
                                 FILENAME ${DW_FILENAME}
                                 FILENAME ${DW_FILENAME}
                                 WD       ${DW_WD}
                                 RESULT   RES
                                 PLATFORM Universal)
                if (${RES} EQUAL 0)
                    message(STATUS "    ...success")
                else()

                    file(REMOVE_RECURSE ${SW_PATH})
                    message(FATAL_ERROR "could not download ${DW_NAME} \n is the package available?\n
                            ${DW_REPO}/${DW_NAME} HEAD:${DW_VERSION}/${ESS_PLATFORM} ")
                endif()
            endif()

        endif ()

    endif()

    if (RES EQUAL 0)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf ${FILENAME}.tar.gz
            WORKING_DIRECTORY ${SW_PATH}
            RESULT_VARIABLE RES
        )
        file(REMOVE ${SW_PATH}/${FILENAME}.tar.gz)
    endif ()

    file(REMOVE ${SW_PATH}/${FILENAME}.tar.gz)
    if (NOT RES EQUAL 0)
        file(REMOVE_RECURSE ${SW_PATH})
        message(FATAL_ERROR "could not unpack ${FILENAME}.tar.gz")
    endif()

endfunction()
