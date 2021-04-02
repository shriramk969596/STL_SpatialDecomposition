if(ess_cuda_included)
    return()
endif(ess_cuda_included)
set(ess_cuda_included true)



function(ess_find_mpi_cuda)

    find_package(MPI REQUIRED)


    get_target_property(MPI_INCLUDES  MPI::MPI_CXX INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(MPI_OPTIONS   MPI::MPI_CXX INTERFACE_COMPILE_OPTIONS)
    get_target_property(MPI_LIBRARIES MPI::MPI_CXX INTERFACE_LINK_LIBRARIES)


    # guard mpi compiler options with backend compiler for cuda
    set (MPI_CUDA_OPTIONS "")
    foreach(OP ${MPI_OPTIONS})
        list(APPEND MPI_CUDA_OPTIONS -Xcompiler ${OP})
    endforeach()


    add_library(MPI::MPI_CUDA IMPORTED INTERFACE)
    set_property(TARGET MPI::MPI_CUDA
        PROPERTY INTERFACE_COMPILE_OPTIONS_CUDA     ${MPI_CUDA_OPTIONS}
    )
    set_property(TARGET MPI::MPI_CUDA
        PROPERTY INTERFACE_COMPILE_OPTIONS_CXX     ${MPI_OPTIONS}
    )
    set_property(TARGET MPI::MPI_CUDA
        PROPERTY INTERFACE_COMPILE_OPTIONS_C     ${MPI_OPTIONS}
    )
    set_property(TARGET MPI::MPI_CUDA
        PROPERTY INTERFACE_LINK_LIBRARIES      ${MPI_LIBRARIES}
    )
    set_property(TARGET MPI::MPI_CUDA
        PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${MPI_INCLUDES}
    )

endfunction()

