message(STATUS "Fetching GLEW...")

if (WIN32)
    SET(BUILD_UTILS OFF)
    FetchContent_Declare(
            glew
            URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip
    )
    FetchContent_GetProperties(glew)
    if(NOT glew_POPULATED)
        FetchContent_Populate(glew)
        add_subdirectory(${glew_SOURCE_DIR}/build/cmake ${CMAKE_CURRENT_BINARY_DIR}/glew)
    endif()
endif()

message(STATUS "GLEW Fetched!")