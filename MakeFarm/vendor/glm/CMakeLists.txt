message(STATUS "Fetching GLM...")

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG cc98465e3508535ba8c7f6208df934c156a018dc
)
FetchContent_MakeAvailable(glm)

message(STATUS "GLM Fetched!")