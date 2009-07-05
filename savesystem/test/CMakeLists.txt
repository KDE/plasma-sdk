  project(gitrunner_test)

find_package(KDE4)


include(KDE4Defaults)

set (gitrunner_test_SRC
    ../dvcsjob.cpp
    ../gitrunner.cpp
    main.cpp
    )

include_directories (${KDE4_INCLUDES})


kde4_add_executable (gitrunner_test ${gitrunner_test_SRC})

# Check for unused/redundant link libraries =P
target_link_libraries (gitrunner_test ${KDE4_KDEUI_LIBS}
    ${KDE4_KTEXTEDITOR_LIBS}
    ${KDE4_KPARTS_LIBS}
    ${KDE4_KIO_LIBRARY}
    ${KDE4_PLASMA_LIBS}
    )

install (TARGETS gitrunner_test ${INSTALL_TARGETS_DEFAULT_ARGS})
