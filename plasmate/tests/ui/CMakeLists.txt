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
set(startPage_SRCS startpagetest.cpp
                   ../../startpage.cpp
)

qt5_wrap_ui(startPage_SRCS ../../startpage.ui)
add_executable(startpagetest ${startPage_SRCS})

target_link_libraries(startpagetest Qt5::Widgets Qt5::Test)

