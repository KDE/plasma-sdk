set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(containment_SRCS
    previewcontainment.cpp
)

add_subdirectory(test)

add_library(plasma_containment_studiopreviewer MODULE ${containment_SRCS})
target_link_libraries(plasma_containment_studiopreviewer ${KDE4_KDEUI_LIBS} KF5::Plasma)

install(TARGETS plasma_containment_studiopreviewer DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-containment-studiopreviewer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
