message(STATUS "Fetching ImGui-SFML...")

FetchContent_Declare(
        imgui-sfml
        GIT_REPOSITORY https://github.com/eliasdaler/imgui-sfml
        GIT_TAG a80d9547832c57a149cda452ed9456def5bb6328
)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)

FetchContent_MakeAvailable(imgui-sfml)

message(STATUS "ImGui-SFML Fetched!")