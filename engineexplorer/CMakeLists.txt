set(plasmaengineexplorer_SRCS
    engineexplorer.cpp
    ktreeviewsearchline.cpp
    main.cpp
    serviceviewer.cpp
    modelviewer.cpp
)

ki18n_wrap_ui(plasmaengineexplorer_SRCS engineexplorer.ui serviceviewer.ui)

add_executable(plasmaengineexplorer ${plasmaengineexplorer_SRCS})
target_compile_definitions(plasmaengineexplorer PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(plasmaengineexplorer
    KF6::I18n
    KF6::Completion
    KF6::Plasma5Support
    KF6::I18n
    KF6::ConfigCore
    KF6::ConfigWidgets
)
target_link_libraries(plasmaengineexplorer Qt6::Core5Compat)


install(TARGETS plasmaengineexplorer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS org.kde.plasmaengineexplorer.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.plasmaengineexplorer.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

kdoctools_create_manpage (man-plasmaengineexplorer.1.docbook 1 INSTALL_DESTINATION ${KDE_INSTALL_MANDIR})
