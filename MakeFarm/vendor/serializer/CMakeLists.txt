message(STATUS "Fetching serializer...")

FetchContent_Declare(
        serializer
        GIT_REPOSITORY "https://github.com/eyalz800/serializer"
        GIT_TAG ad0b50a22259af249c6573510548966866e8334a
)

FetchContent_MakeAvailable(serializer)

message(STATUS "serializer Fetched!")