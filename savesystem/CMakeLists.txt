project(plasmate-savesystem)

set (savesystem_SRCS
  author.cpp
  savesystem.cpp
  dvcsjob.cpp
  gitrunner.cpp
  timelineitem.cpp
  timelinedelegate.cpp
  timelinelistwidget.cpp
  timelineprivatestorage.cpp
  timeline.cpp
    )

kde4_add_plugin(plasmate_savesystem ${savesystem_SRCS})

target_link_libraries(plasmate_savesystem ${KDE4_KDEUI_LIBS})

