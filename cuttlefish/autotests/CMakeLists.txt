
include(ECMAddTests)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Test)

include_directories(../src)

ecm_add_test(iconmodeltest.cpp ../src/iconmodel.cpp ../src/sortfiltermodel.cpp
             TEST_NAME iconmodeltest
             LINK_LIBRARIES
                Qt5::Gui
                Qt5::Test
                KF5::ConfigCore
                KF5::IconThemes
                KF5::Package
                KF5::Service
                KF5::Plasma
                KF5::ConfigWidgets
                )
