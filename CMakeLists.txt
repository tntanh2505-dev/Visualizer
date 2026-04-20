cmake_minimum_required(VERSION 3.16)
project(DSA-Visualization)

set(CMAKE_CXX_STANDARD 17)

# Point to SFML
set(SFML_DIR "${CMAKE_SOURCE_DIR}/lib/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

# Collect sources
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/include
)

target_link_libraries(${PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
)

# Copy assets to build folder
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY data DESTINATION ${CMAKE_BINARY_DIR})	

# Copy SFML DLLs to build folder (add this before or after the executable is created)
file(GLOB SFML_DLLS "${CMAKE_SOURCE_DIR}/lib/bin/*.dll")
file(COPY ${SFML_DLLS} DESTINATION ${CMAKE_BINARY_DIR})