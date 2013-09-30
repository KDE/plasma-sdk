set(plasmoidviewer_SRCS
    #    fullview.cpp
    main.cpp
    view.cpp
)

#FIXME Remove the macro
macro(plasma_install_package dir component)
   set(root ${ARGV2})
   set(type ${ARGV3})
   if(NOT root)
      set(root plasma/plasmoids)
   endif()
   if(NOT type)
      set(type applet)
   endif()
   install(DIRECTORY ${dir}/ DESTINATION ${DATA_INSTALL_DIR}/${root}/${component} PATTERN .svn EXCLUDE PATTERN CMakeLists.txt EXCLUDE PATTERN Messages.sh EXCLUDE)
   install(FILES ${dir}/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR} RENAME plasma-${type}-${component}.desktop)
endmacro()

plasma_install_package(qmlpackages/shell org.kde.plasmoidviewershell plasma/shells shell)
plasma_install_package(qmlpackages/containment org.kde.plasmoidviewercontainment plasma/plasmoids containment)

add_executable(plasmoidviewer ${plasmoidviewer_SRCS})

target_link_libraries(plasmoidviewer
    ${Plasma_LIBRARIES}
    ${PlasmaQuick_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
    KF5::KI18n
    ${KDE4Attic_LIBRARIES}
)

install(TARGETS plasmoidviewer ${INSTALL_TARGETS_DEFAULT_ARGS})

#FIXME
#kde4_create_manpage(man-plasmoidviewer.1.docbook 1 INSTALL_DESTINATION ${MAN_INSTALL_DIR})
