project(plasma-sdk)

find_package(KDE4 REQUIRED)
find_package(KDeclarative REQUIRED)

include(MacroLibrary)
include(KDE4Defaults)

include_directories(${KDE4_INCLUDES}
                    ${KDECLARATIVE_INCLUDE_DIR}
)

add_subdirectory(engineexplorer)
add_subdirectory(plasmoidviewer)
add_subdirectory(remote-widgets-browser)
add_subdirectory(wallpaperviewer)
add_subdirectory(plasmate)