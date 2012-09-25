include_directories(
    src/aurorae/lib
)
include(KDE4Defaults)
Message("helloooooo" ${KDEBASE_WORKSPACE_SOURCE_DIR})
# set(AURORAE_SOURCE_DIR src/aurorae/lib)
Message("Aurorae sources " ${AURORAE_SOURCE_DIR})
set(kcm_kwindecoration_PART_SRCS
    kwindecoration.cpp
    preview.cpp
    decorationmodel.cpp
    ${AURORAE_SOURCE_DIR}/auroraetheme.cpp
    ${AURORAE_SOURCE_DIR}/themeconfig.cpp
    src/libkdecorations/kdecoration_plugins_p.h
    main.cpp
)

kde4_add_ui_files(kcm_kwindecoration_PART_SRCS
    auroraeconfig.ui
    decoration.ui
)

kde4_add_executable(kwinkoker ${kcm_kwindecoration_PART_SRCS})
target_link_libraries(kwinkoker ${KDE4_KNEWSTUFF3_LIBS} kdecorations  ${X11_LIBRARIES} ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS} ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTUITOOLS_LIBRARY} ${X11_LIBRARIES})
install(TARGETS kwinkoker ${INSTALL_TARGETS_DEFAULT_ARGS})



########### install files ###############

install( FILES kwinkoker.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES
    qml/main.qml
    qml/AuroraeDecoration.qml
    qml/AuroraePreview.qml
    qml/DecorationPreview.qml
    DESTINATION ${DATA_INSTALL_DIR}/kwin/kwinkoker)
