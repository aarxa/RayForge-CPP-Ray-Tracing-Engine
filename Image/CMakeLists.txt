# Image/CMakeLists.txt
add_library(Image
    bmp.cpp
    image.cpp
    image.todo.cpp
    jpeg.cpp
    lineSegments.cpp
    lineSegments.todo.cpp
)

target_include_directories(Image PUBLIC ${SOURCE_DIR})
target_include_directories(Image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
