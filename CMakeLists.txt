cmake_minimum_required(VERSION 2.6)
project(skeletonvisualizer)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

SET(CMAKE_CXX_FLAGS "-std=c++14")
set(SkeletonVisualizer_SRCS SkeletonVisualizer.cpp main.cpp)

qt4_automoc(${SkeletonVisualizer_SRCS})
add_executable(skeletonvisualizer ${SkeletonVisualizer_SRCS})
target_link_libraries(skeletonvisualizer ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install(TARGETS skeletonvisualizer RUNTIME DESTINATION bin)
