message(STATUS "Fetching FastNoiseLite...")

FetchContent_Declare(
        fastnoiselite
        GIT_REPOSITORY https://github.com/Auburn/FastNoiseLite
        GIT_TAG 6be3d6bf7fb408de341285f9ee8a29b67fd953f1
)
FetchContent_MakeAvailable(fastnoiselite)

message(STATUS "FastNoiseLite Fetched!")