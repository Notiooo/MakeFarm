include(FetchContent)

include(sfml/CMakeLists.txt)
include(imgui/CMakeLists.txt)
include(imgui-sfml/CMakeLists.txt)

add_subdirectory(glm)
add_subdirectory(fastnoiselite)
add_subdirectory(glew)
add_subdirectory(tgui)
add_subdirectory(serializer)