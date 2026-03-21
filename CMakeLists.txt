cmake_minimum_required(VERSION 3.16)
project(DataVisualizer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(DataVisualizer main.cpp ${SOURCES})

add_custom_command(TARGET DataVisualizer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:DataVisualizer>/assets
)

target_include_directories(DataVisualizer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/include
)

target_link_directories(DataVisualizer PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/bin
)

target_link_libraries(DataVisualizer PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

file(GLOB SFML_DLLS "${CMAKE_SOURCE_DIR}/lib/bin/*.dll")
foreach(DLL ${SFML_DLLS})
    add_custom_command(TARGET DataVisualizer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${DLL}
            $<TARGET_FILE_DIR:DataVisualizer>
    )
endforeach()