project(kparttut1)
 
FIND_PACKAGE(KDE4 REQUIRED)
INCLUDE_DIRECTORIES( ${KDE4_INCLUDES} . )
 
set(kparttut1_SRCS
   main.cpp
   mainwindow.cpp
 )
 
kde4_add_executable(kparttut1 ${kparttut1_SRCS})
 
target_link_libraries(kparttut1 ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS})
 
########### install files ###############
install(TARGETS kparttut1 DESTINATION ${BIN_INSTALL_DIR} )
install( FILES kparttut1ui.rc 
    DESTINATION  ${DATA_INSTALL_DIR}/kparttut1 )