if(ess_add_gtest)
    return()
endif(ess_add_gtest)
set(ess_add_gtest true)





function(fetch_gtest)

    if (NOT TARGET gtest)

        ess_fetch_external(NAME gtest VERSION 1.8.0)

    endif()


endfunction()
