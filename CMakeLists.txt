cmake_minimum_required(VERSION 3.10)

project(demo_show)
list(APPEND CUDA_NVCC_FLAGS "-std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
add_definitions(-std=c++17)
add_definitions(-DAPI_EXPORTS)
option(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

find_package(CUDA REQUIRED)
find_package(PCL 1.2 REQUIRED)
#find_package(GTest)

# ===== cuda =====
include_directories(${CUDA_INCLUDE_DIRS})
link_directories(/usr/local/cuda-11.4/lib64)
# ===== tensorrt =====
include_directories(/usr/include/aarch64-linux-gnu/)
link_directories(/usr/lib/aarch64-linux-gnu/)
# ===== pcl =====
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})
# ===== main =====
aux_source_directory(${PROJECT_SOURCE_DIR}/src M_SRC_DIR)
include_directories(${PROJECT_SOURCE_DIR}/include)
# ===== build =====
add_executable(demo ${M_SRC_DIR} demo.cpp)
target_link_libraries(demo ${CUDA_LIBS} ${PCL_LIBRARIES} cudart nvinfer gtest_main gtest)
