
# Enable cuda support
set(CMAKE_CUDA_COMPILER "/usr/local/cuda/bin/nvcc")
enable_language(CUDA)
#find_package(CUDA REQUIRED)

# Set C++ standards to 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CUDA_STANDARD 11)

# Set necessary CUDA Flags
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -gencode=arch=compute_52,code=sm_52 -gencode=arch=compute_52,code=compute_52 -gencode=arch=compute_61,code=compute_61 -maxrregcount=58")
if(UNIX)
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -maxrregcount=58 ")
endif()
# Include CUDA Building Blocks
add_library(cub INTERFACE)
target_include_directories(cub INTERFACE ${CMAKE_CURRENT_LIST_DIR})
# Include all necessary directories
include_directories(include)
include_directories(cub)

add_library(AABB_Triangle
            utils.h
            stl_utils.h
            grid_utils.h
            stl_reader.h
            dem_core.h
            utils.cpp
            stl_utils.cpp
            grid_utils.cpp
            dem_core.h
            dem_core.cpp
            ParticleGenerator.cpp
            ParticleGenerator.h
            z_hash.h
            z_hash.cpp triangle4_utils.cuh triangle4_utils.cu
            cuda_math_utils.h params.h
            cuda_error_check.h)

set(KERNELS
    

          )


set(Boost_NO_SYSTEM_PATHS TRUE)
if (Boost_NO_SYSTEM_PATHS)
  set(BOOST_ROOT "/home/shriram.krishna/.ess-third-party/Ubuntu18.04_x86_64/Boost-1.66.0")
  #set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../3p/boost")
  set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/include")
  set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib")
endif (Boost_NO_SYSTEM_PATHS)
find_package(Boost REQUIRED regex date_time system filesystem thread graph program_options)
include_directories(${BOOST_INCLUDE_DIRS})
find_package(glfw3 3.2.1 REQUIRED)


add_executable(AABBtriangle main.cu)

target_link_libraries(AABBtriangle AABB_Triangle)

target_link_libraries(AABBtriangle Boost::filesystem)
