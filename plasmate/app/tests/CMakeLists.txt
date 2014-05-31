include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# projecthandlertest
set(projecthandlertest_SRCS ../startpage/projecthandler.cpp
                            ../../editors/metadata/metadatahandler.cpp
                            ../../packagehandler/themehandler.cpp
                            ../../packagehandler/packagehandler.cpp
)

set(projecthandlertest_LIBS Qt5::Widgets KF5::Plasma)

PLASMATE_UNIT_TEST(projecthandlertest "${projecthandlertest_SRCS}"
                                      "${projecthandlertest_LIBS}"
)

