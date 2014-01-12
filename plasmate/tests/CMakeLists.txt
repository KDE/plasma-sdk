set(packageHandler_SRCS packagehandlertest.cpp
                        ../packagehandler.cpp
)

add_executable(packagehandlertest ${packageHandler_SRCS})

target_link_libraries(packagehandlertest Qt5::Core Qt5::Test KF5::Plasma)

#FIXME
install(DIRECTORY ./templates DESTINATION ${DATA_INSTALL_DIR}/packagehandlertest)

set(packagemodelTest_SRCS modeltest.cpp
                          packagemodeltest.cpp
                          ../packagemodel.cpp
                          ../packagehandler.cpp
)

add_executable(packagemodeltest ${packagemodelTest_SRCS})

target_link_libraries(packagemodeltest Qt5::Core Qt5::Test KF5::Plasma)

set(editpageTest_SRCS ui/editpagetest.cpp
                      ../packagemodel.cpp
                      ../packagehandler.cpp
                      ../editors/editpage.cpp
                      ../editors/imageviewer/imageloader.cpp
                      ../editors/imageviewer/imageviewer.cpp
                      ../editors/kconfigxt/kconfigxteditor.cpp
                      ../editors/kconfigxt/kconfigxtreader.cpp
                      ../editors/kconfigxt/kconfigxtwriter.cpp
)

kde4_add_ui_files(editpageTest_SRCS ../editors/kconfigxt/kconfigxteditor.ui
)

add_executable(editpagetest ${editpageTest_SRCS})

target_link_libraries(editpagetest Qt5::Core Qt5::Widgets Qt5::Test
                      Qt5::Svg KF5::Plasma KF5::I18n KF5::Service
                      KF5::KIOCore KF5::KDE4Support)

set(metadatahandler_SRCS metadatahandlertest.cpp
                         ../editors/metadata/metadatahandler.cpp
)

add_executable(metadatahandlertest ${metadatahandler_SRCS})

target_link_libraries(metadatahandlertest Qt5::Core Qt5::Test KF5::ConfigCore)

