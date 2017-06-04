cmake_minimum_required(VERSION 2.8)
set(CMAKE_SUPPRESS_REGENERATION true)
project(myItkImageLoader)
set(ITK_DIR "C:/libs/InsightToolkit-4.8.1-64bits")#path da itk
# Acha a package
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
#cria a lib
file(GLOB my_source_files "*.h" "*.cpp" "*.cxx")
add_library(myItkImageLoader ${my_source_files} )  
#bota ITK
target_link_libraries(myItkImageLoader ${ITK_LIBRARIES})
#expõe
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

#O speed log (https://github.com/gabime/spdlog)
include_directories(C:/libs/spdlog/include/spdlog)

add_definitions(-D_SCL_SECURE_NO_WARNINGS)
