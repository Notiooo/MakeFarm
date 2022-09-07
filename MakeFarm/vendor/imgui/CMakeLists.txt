message(STATUS "Fetching ImGui...")

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG e99c4fc6688e218a0e5da50f56638aebab45da9b
)
FetchContent_MakeAvailable(imgui)

message(STATUS "ImGui Fetched!")