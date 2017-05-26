cmake_minimum_required(VERSION 2.8)
set(CMAKE_SUPPRESS_REGENERATION true)
project(myItkImageLoader)

set(ITK_DIR "C:/lib/InsightToolkit-4.8.1-64")
# Find ITK
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

file(GLOB my_source_files "*.h" "*.cpp" "*.cxx")
add_library(myItkImageLoader ${my_source_files} )  
#bota vtk e ITK
target_link_libraries(myItkImageLoader ${ITK_LIBRARIES})

