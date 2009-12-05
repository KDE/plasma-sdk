project(plasmate-savesystem)

set (savesystem_SRCS
    dvcsjob.cpp
    gitrunner.cpp
    commitdialog.cpp
    branchdialog.cpp
    timelineitem.cpp
    tabledelegate.cpp
    tablewidget.cpp
    timeline.cpp
    )

kde4_add_plugin(plasmate_savesystem ${savesystem_SRCS})

target_link_libraries(plasmate_savesystem ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS})

