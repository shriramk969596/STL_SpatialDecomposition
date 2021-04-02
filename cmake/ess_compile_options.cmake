if(ess_cops_included)
    return()
endif(ess_cops_included)
set(ess_cops_included true)



add_library(ESS::debug_symbols INTERFACE IMPORTED)
target_compile_options(ESS::debug_symbols 
    INTERFACE 
        -g
        # notice that -G turns off optiomizations for device code. use 
        #  -lineinfo for profiling
        $<$<COMPILE_LANGUAGE:CUDA>:-lineinfo>
)
