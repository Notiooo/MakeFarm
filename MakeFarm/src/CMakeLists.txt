cmake_minimum_required(VERSION 3.16)

project(MakeFarmSrc VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(CMakeLists_Sources.txt)

add_library(MakeFarmSrc STATIC ${PROJECT_SOURCES})
target_precompile_headers(MakeFarmSrc PUBLIC pch.h)

set(CUSTOM_INCLUDES_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_includes)
file(MAKE_DIRECTORY ${CUSTOM_INCLUDES_DIR})

add_custom_command(TARGET MakeFarmSrc PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${FETCHCONTENT_BASE_DIR}/serializer-src/serializer.h
        ${CUSTOM_INCLUDES_DIR}
        )

target_include_directories(MakeFarmSrc PUBLIC
        ${FETCHCONTENT_BASE_DIR}/glew-src/include
        ${glm_SOURCE_DIR}
        ${FETCHCONTENT_BASE_DIR}/fastnoiselite-src/Cpp
        ${CUSTOM_INCLUDES_DIR}
        )

target_link_libraries(MakeFarmSrc
        PUBLIC
        sfml-graphics
        ImGui-SFML::ImGui-SFML
        glew_s
        glm
        tgui
        )

# Copy libraries

add_custom_command(TARGET MakeFarmSrc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:ImGui-SFML::ImGui-SFML>
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE:tgui>
        $<TARGET_FILE_DIR:MakeFarmSrc>
        )

# Remove previous game resources

add_custom_command(TARGET MakeFarmSrc PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        $<TARGET_FILE_DIR:MakeFarmSrc>/resources
        )

# Copy game resources

add_custom_command(TARGET MakeFarmSrc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:MakeFarmSrc>/resources
        )

# Copy game settings

add_custom_command(TARGET MakeFarmSrc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/settings.cfg $<TARGET_FILE_DIR:MakeFarmSrc>
        )

add_executable(MakeFarmApp main.cpp)

add_custom_command(TARGET MakeFarmApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:MakeFarmSrc>
        $<TARGET_FILE_DIR:MakeFarmApp>
        )

target_link_libraries(MakeFarmApp PRIVATE MakeFarmSrc)
