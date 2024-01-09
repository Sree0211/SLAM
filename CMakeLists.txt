cmake_minimum_required(VERSION 3.10)
project(vslam)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

# Set the include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Set the library directories
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Locate OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OPENCV_INCLUDE_DIRS})

# Locate OpenGL 
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# Find GLFW
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})

# Find GLEW
find_library(GLEW_LIBRARY GLEW)
include_directories(${GLEW_INCLUDE_DIRS})

# Compile the package
add_executable(vslam src/main.cpp)

# Link against the libraries
target_link_libraries(vslam glfw ${GLFW_LIBRARIES} ${OPENGL_gl_LIBRARY} ${OpenCV_LIBS} ${GLEW_LIBRARY})
