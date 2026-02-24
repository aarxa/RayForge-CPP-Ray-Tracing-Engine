# Util/CMakeLists.txt
add_library(Util
    geometry.cpp
    interpolation.cpp
    poly34.cpp
)

target_link_libraries(Util PRIVATE GLEW)
target_include_directories(Util PUBLIC ${SOURCE_DIR})
target_include_directories(Util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
