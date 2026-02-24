# GL/CMakeLists.txt

# Create a static library from the glew source file
add_library(GLEW STATIC glew.c)

# Add the parent directory so that #include <GL/glew.h> works
# Platform-specific settings (Darwin = macOS)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_include_directories(GLEW PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
else()
    target_include_directories(GLEW PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# Set compiler options for GLEW (for C files, not C++)
target_compile_options(GLEW PRIVATE -Wunused-result)

# Set the C standard to C99 or C11
set_target_properties(GLEW PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)
