# Root CMakeLists.txt for Assignment2
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(Assignment2)

# Set the C++ standard to C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Source Directory
set(SOURCE_DIR ${CMAKE_SOURCE_DIR})

# Collect all source files
file(GLOB SRC_FILES ${SOURCE_DIR}/*.cpp)

# Add subdirectories for dependencies (Image, Util, Ray, GL)
add_subdirectory(GL)
add_subdirectory(JPEG)
add_subdirectory(Image)
add_subdirectory(Util)
add_subdirectory(Ray)

# Platform-specific settings (Darwin = macOS)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(PLATFORM_LIBS "-framework GLUT -framework OpenGL")
else()
    set(PLATFORM_LIBS "-lglut -lGLU -lGL -lgomp")
endif()

# Find and link the system libraries
find_package(OpenGL REQUIRED)

# Set JPEG library paths
set(JPEG_INCLUDE_DIR "/opt/homebrew/opt/jpeg/include")
set(JPEG_LIBRARY_DIR "/opt/homebrew/opt/jpeg/lib")
find_library(JPEG_LIB jpeg PATHS ${JPEG_LIBRARY_DIR} REQUIRED)

# Add the directories explicitly
include_directories(${JPEG_INCLUDE_DIR})
link_directories(${JPEG_LIBRARY_DIR})

# Create the executable for Assignment2
add_executable(Assignment2 ${SRC_FILES})

# Link libraries (Image, Util, Ray, GLEW, OpenGL)
target_link_libraries(Assignment2 PRIVATE JPEG Image Util Ray GLEW ${OPENGL_LIBRARIES} ${JPEG_LIB} ${PLATFORM_LIBS})

# Set include directories for dependencies
target_include_directories(Assignment2 PRIVATE ${CMAKE_SOURCE_DIR}/GL ${CMAKE_SOURCE_DIR}/JPEG ${OPENGL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/Image ${CMAKE_SOURCE_DIR}/Util ${CMAKE_SOURCE_DIR}/Ray ${JPEG_INCLUDE_DIR})

# Set compiler options
target_compile_options(Assignment2 PRIVATE -std=c++14 -Wunused-result)

# Set Debug and Release build configurations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++11-narrowing")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g3 -DUSE_SOLUTION=5")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DRELEASE -funroll-loops -ffast-math -DNDEBUG")

# Global output directory settings (applies to all targets)
set_target_properties(Assignment2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin
)
