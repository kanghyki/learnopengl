cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME output)
set(CMAKE_CXX_STANDARD 17)

set(WINDOW_NAME "Hello, OpenGL!")
set(WINDOW_WIDTH 1920)
set(WINDOW_HEIGHT 1080)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(${PROJECT_NAME})
add_executable(${PROJECT_NAME}
src/main.cpp
src/common.cpp        src/common.hpp
src/shader.cpp        src/shader.hpp
src/program.cpp       src/program.hpp
src/context.cpp       src/context.hpp
src/buffer.cpp        src/buffer.hpp
src/vertex_array.cpp  src/vertex_array.hpp
src/image.cpp         src/image.hpp
src/texture.cpp       src/texture.hpp
                      src/camera.hpp
                      src/light.hpp
src/mesh.cpp          src/mesh.hpp
src/model.cpp         src/model.hpp
src/material.cpp      src/material.hpp
                      src/framebuffer.hpp
src/object.cpp        src/object.hpp
                      src/ray.hpp
                      src/transform.hpp
                      src/bounding_sphere.hpp
)

include(Dependency.cmake)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "C:\\Users\\tnehs\\Desktop\\learn-opengl")

if (APPLE)
    set(CMAKE_CXX_FLAGS "-framework Cocoa -framework IOKit -framework OpenGL")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
    )

add_dependencies(${PROJECT_NAME} ${DEP_LIST})
