if(ess_test)
    return()
endif(ess_test)
set(ess_test true)

include (ess_deploy)
include (ess_add_gtest)

find_package(Threads REQUIRED)

############################################################################
#
# ESS C++ testing module
#
# This test facilitates the creation of UnitTests


# enable ctest support for testing
enable_testing()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(ESS_TEST_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include CACHE PATH "unittest headers")


option(ESS_COVERAGE "Enable coverage measurements" FALSE)
option(ESS_NO_CATCH "Dissable the use of catch" FALSE)
option(ESS_NO_GTEST "Dissable the use of gtest" FALSE)

function(ess_add_test)

    set(OPTIONS "CATCH;GTEST")
    set(SINGLES "NAME;TARGET")
    set(MULTI   "FILES;LIBS;INCLUDE;OPTIONS;DEFINITIONS")
    cmake_parse_arguments(TEST "${OPTIONS}" "${SINGLES}" "${MULTI}" ${ARGN} )
        
    # provide flags to deactivate tests based on frameworks
    if (TEST_CATCH AND ESS_NO_CATCH)
        return()
    elseif (TEST_GTEST AND ESS_NO_GTEST)
        return()
    endif()

    # set a macro definition pointing to our current source directory
    set(SOURCE_DIR ${CMAKE_SOURCE_DIR})
    if(WIN32)
        string(REPLACE "/" "\\\\" SOURCE_DIR ${SOURCE_DIR})
    endif()

    add_executable(${TEST_NAME} ${TEST_FILES})
    target_compile_definitions(${TEST_NAME} 
        PRIVATE 
            ${TEST_DEFINITIONS} 
            ESS_TEST 
            ESS_SOURCE_DIR="${SOURCE_DIR}"
            ESS_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}"
    )
    target_compile_options(${TEST_NAME} 
        PRIVATE 
            ${TEST_OPTIONS}
    )
    target_include_directories(${TEST_NAME} 
    PRIVATE
            ${TEST_INCLUDE} 
    )
    target_link_libraries(${TEST_NAME}
    PRIVATE
        ${TEST_TARGET}
        ${TEST_LIBS}
    )

    if(TEST_CATCH)

        set_property(TARGET ${TEST_NAME} PROPERTY CXX_STANDARD 11)
        set_property(TARGET ${TEST_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

        target_compile_definitions(${TEST_NAME} 
            PRIVATE 
                CATCH_CONFIG_MAIN
        )
        target_include_directories(${TEST_NAME} 
        PRIVATE
                ${ESS_TEST_HEADERS}
        )
    endif()

    if(TEST_GTEST)

        fetch_gtest()
        
        target_link_libraries(${TEST_NAME}
            PRIVATE
                gtest gtest_main
                Threads::Threads
        )

    endif()

    ess_deploy(TARGETS ${TEST_NAME} JUST_COPY_RT)
    get_target_property(UNIT_TEST_BIN_OUTPUT_DIR  ${TEST_NAME} RUNTIME_OUTPUT_DIRECTORY)
    if (NOT UNIT_TEST_BIN_OUTPUT_DIR)
        set(UNIT_TEST_BIN_OUTPUT_DIR ${CMAKE_BINARY_DIR})
    endif()
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} WORKING_DIRECTORY ${UNIT_TEST_BIN_OUTPUT_DIR})
    #ess_debug_target(${TEST_NAME})

    if (ESS_COVERAGE)
        # coverage (gcov, linux only):
        if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)

            target_compile_options(${TEST_NAME} PRIVATE --coverage)
            target_link_libraries(${TEST_NAME} PRIVATE gcov)
            target_compile_options(${TEST_TARGET} PRIVATE --coverage)
            target_link_libraries(${TEST_TARGET} PRIVATE gcov)

            message(STATUS " ----------------------------------")
            message(STATUS "This build includes COVERAGE meassurements, and performace my be compromished")
            message(STATUS "   To visualize coverage execute your binaries and/or tests, then execute: ")
            message(STATUS "   > lcov --directory . --capture --output-file coverage.info")
            message(STATUS "   > genhtml -o coverage_report/ coverage.info")
            message(STATUS "   > xdg-open coverage_report/index.html")
            message(STATUS "WARNING: Do not ship a coverage build")
            message(STATUS " ----------------------------------")
        else()

            message(FATAL_ERROR "coverage not yet implemented in this platform")

        endif()

    endif()

    set_target_properties(${TEST_NAME} PROPERTIES  FOLDER  tests)

endfunction()


if (ESS_COVERAGE_ALL)
    # coverage (gcov, linux only):
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)

        add_definitions(--coverage)
        link_libraries(gcov)

        message(STATUS " ----------------------------------")
        message(STATUS "This build includes COVERAGE meassurements, and performace my be compromished")
        message(STATUS "   To visualize coverage execute your binaries and/or tests, then execute: ")
        message(STATUS "   > lcov --directory . --capture --output-file coverage.info")
        message(STATUS "   > genhtml -o coverage_report/ coverage.info")
        message(STATUS "   > xdg-open coverage_report/index.html")
        message(STATUS "WARNING: Do not ship a coverage build")
        message(STATUS " ----------------------------------")
    else()

            message(FATAL_ERROR "coverage not yet implemented in this platform")

    endif()

endif()


