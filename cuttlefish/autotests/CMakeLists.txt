
include(ECMAddTests)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED Test)

include_directories(../src)

ecm_add_test(iconmodeltest.cpp ../src/iconmodel.cpp ../src/sortfiltermodel.cpp
             TEST_NAME iconmodeltest
             LINK_LIBRARIES
                Qt::Gui
                Qt::Test
                KF5::KIOWidgets
                KF5::ConfigCore
                KF5::IconThemes
                KF5::Service
                KF5::Plasma
                KF5::ConfigWidgets
                )
