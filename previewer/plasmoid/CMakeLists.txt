set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(containment_SRCS
    previewcontainment.cpp
    overlaytoolbox.cpp
)

add_subdirectory(test)

kde4_add_plugin(plasma_containment_studiopreviewer ${containment_SRCS})
target_link_libraries(plasma_containment_studiopreviewer ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS})

install(TARGETS plasma_containment_studiopreviewer DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-containment-studiopreviewer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
