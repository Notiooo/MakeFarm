message(STATUS "Fetching TGUI...")

FetchContent_Declare(
        tgui
        GIT_REPOSITORY "https://github.com/texus/TGUI"
        GIT_TAG 2b7610f251937cf854a362127a47a13f027457a1 # 0.9.5
)

set(TGUI_BACKEND SFML)
set(TGUI_CXX_STANDARD 20)
set(TGUI_BUILD_GUI_BUILDER false)

FetchContent_MakeAvailable(tgui)

message(STATUS "TGUI Fetched!")