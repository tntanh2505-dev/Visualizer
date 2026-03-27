cmake_minimum_required(VERSION 3.16)
project(DSA-Visualization)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Point to SFML 
set(SFML_DIR "${CMAKE_SOURCE_DIR}/lib/lib/cmake/SFML")
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

# 2. Collect all source files
# Included "main.cpp" from root and all .cpp files in "src" subfolders
file(GLOB_RECURSE SOURCES "src/*.cpp" "main.cpp")

# 3. Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# 4. Set Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/lib/include"
)

# 5. Link SFML Libraries
target_link_libraries(${PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
)

# 6. Robustly copy Assets and DLLs
# Pre-identify the DLL files so CMake doesn't get confused by the '*' wildcard
file(GLOB SFML_DLL_FILES "${CMAKE_SOURCE_DIR}/lib/bin/*.dll")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # Copy the entire assets directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    
    # Copy all identified DLLs to the folder where the .exe lives
    COMMAND ${CMAKE_COMMAND} -E copy
        ${SFML_DLL_FILES}
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    
    COMMENT "Deploying assets and SFML DLLs to build directory..."
)