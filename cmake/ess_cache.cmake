if(ess_cache_included)
    return()
endif(ess_cache_included)
set(ess_cache_included true)



set (ESS_CACHE_FILE ${CMAKE_BINARY_DIR}/ess/cache.txt CACHE INTERNALE "")

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ess)
file(WRITE ${ESS_CACHE_FILE} "")

function (ess_is_cached ITEM RES)

    file(STRINGS ${ESS_CACHE_FILE} CACHED)

    if(NOT CACHED )
        file(APPEND  ${ESS_CACHE_FILE} "${ITEM}\n")
        set(${RES} false PARENT_SCOPE)
    endif()

    list(FIND CACHED ${ITEM} FIT)
    if (${FIT} STREQUAL -1)
        file(APPEND  ${ESS_CACHE_FILE} "${ITEM}\n")
        set(${RES} false PARENT_SCOPE)
    else()
        set(${RES} true PARENT_SCOPE)
    endif()

endfunction()
