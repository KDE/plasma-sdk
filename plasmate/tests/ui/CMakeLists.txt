macro(PLASMATE_UI_TEST testname uifiles srcs libraries)
    set(testname_SRCS ${srcs})
    qt5_wrap_ui(testname_SRCS ${uifiles})

    set(libraries ${libraries} Qt5::Widgets)
    plasmate_unit_test(${testname} "${testname_SRCS}" "${libraries}" false)
endmacro()

# editpagetest
set(editpagetest_SRCS ../../packagemodel.cpp
                      ../../packagehandler.cpp
                      ../../editors/editpage.cpp
                      ../../editors/imageviewer/imageloader.cpp
                      ../../editors/imageviewer/imageviewer.cpp
                      ../../editors/kconfigxt/kconfigxteditor.cpp
                      ../../editors/kconfigxt/kconfigxtreader.cpp
                      ../../editors/kconfigxt/kconfigxtwriter.cpp
)

set(editpagetest_UI ../../editors/kconfigxt/kconfigxteditor.ui)

set(editpagetest_LIBS Qt5::Svg
                      KF5::Plasma
                      KF5::I18n
                      KF5::Service
                      KF5::KIOCore
                      KF5::KIOWidgets
                      KF5::Parts
)

plasmate_ui_test(editpagetest "${editpagetest_UI}"
                              "${editpagetest_SRCS}"
                              "${editpagetest_LIBS}"
)
set(startpagetest_SRCS startpagetest.cpp
                   ../../startpage.cpp
                   ../../projectmanager/projectmanager.cpp
                   ../../projecthandler.cpp
                   ../../packagehandler.cpp
                   ../../editors/metadata/metadatahandler.cpp
)

set(startpagetest_UI ../../startpage.ui)

set(startpagetest_LIBS KF5::Archive KF5::Plasma KF5::I18n KF5::KIOCore KF5::KIOWidgets KF5::NewStuff)

plasmate_ui_test(startpagetest "${startpagetest_UI}"
                               "${startpagetest_SRCS}"
                               "${startpagetest_LIBS}"
)
