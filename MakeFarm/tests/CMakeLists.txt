cmake_minimum_required(VERSION 3.16)

project(MakeFarmTests LANGUAGES CXX)

include_directories(../src)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

configure_file("CMakeLists.txt.in" "${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt")
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
        )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
        )

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
        "${CMAKE_BINARY_DIR}/googletest-build"
        )

if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include"
            "${gmock_SOURCE_DIR}/include"
            )
endif ()

include(ut/CMakeLists.txt)
include(mt/CMakeLists.txt)
include(mocks/CMakeLists.txt)
include_directories(ut/src)
include_directories(mt/src)
include_directories(mocks/src)
list(TRANSFORM UT_Sources PREPEND ut/)
list(TRANSFORM MT_Sources PREPEND mt/)
list(TRANSFORM Mocks_Sources PREPEND mocks/)

add_executable(MakeFarmTests
        main.cpp
        ${UT_Sources}
        ${MT_Sources}
        ${Mocks_Sources}
        )


target_compile_definitions(MakeFarmTests PUBLIC GTEST_LINKED_AS_SHARED_LIBRARY)
target_link_libraries(MakeFarmTests PRIVATE MakeFarmSrc)
target_link_libraries(MakeFarmTests PRIVATE gtest gtest_main)
target_link_libraries(MakeFarmTests PRIVATE gmock gmock_main)

# Copy libraries

add_custom_command(TARGET MakeFarmTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:gtest>
        $<TARGET_FILE:gmock>
        $<TARGET_FILE:gtest_main>
        $<TARGET_FILE:gmock_main>
        $<TARGET_FILE:ImGui-SFML::ImGui-SFML>
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:MakeFarmTests>
        )