project(timeline_test)

find_package(KDE4)


include(KDE4Defaults)

set (gitrunner_test_SRC
    ../dvcsjob.cpp
    ../gitrunner.cpp
    ../commitdialog.cpp
    ../branchdialog.cpp
    ../timelineitem.cpp
    ../tabledelegate.cpp
    ../tablewidget.cpp
    ../timeline.cpp
    main.cpp
    )

include_directories (${KDE4_INCLUDES})


add_executable (timeline_test ${gitrunner_test_SRC})

# Check for unused/redundant link libraries =P
target_link_libraries (timeline_test ${KDE4_KDEUI_LIBS}
    KF5::Plasma
    )

