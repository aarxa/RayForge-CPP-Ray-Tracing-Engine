# Ray/CMakeLists.txt
add_library(Ray
    box.cpp
    box.todo.cpp 
    camera.cpp
    camera.todo.cpp 
    cone.cpp
    cone.todo.cpp
    cylinder.cpp
    cylinder.todo.cpp 
    directionalLight.cpp
    directionalLight.todo.cpp
    fileInstance.cpp
    GLSLProgram.cpp
    mouse.cpp
    pointLight.cpp
    pointLight.todo.cpp 
    scene.cpp
    scene.todo.cpp 
    shape.cpp
    shapeList.cpp
    shapeList.todo.cpp
    sphere.cpp
    sphere.todo.cpp 
    spotLight.cpp
    spotLight.todo.cpp
    torus.cpp
    torus.todo.cpp 
    triangle.cpp
    triangle.todo.cpp 
    window.cpp
)



target_link_libraries(Ray PRIVATE GLEW)
target_include_directories(Ray PUBLIC ${SOURCE_DIR})
target_include_directories(Ray PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
