cmake_minimum_required(VERSION 3.0)

project(plasma-sdk)
set(PROJECT_VERSION "5.14.2")
set(PROJECT_VERSION_MAJOR 5)

set(QT_MIN_VERSION "5.11.0")
set(KF5_MIN_VERSION "5.50.0")

################# Disallow in-source build #################

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(FATAL_ERROR "plasmate requires an out of source build. Please create a separate build directory and run 'cmake path_to_plasmate [options]' there.")
endif()

################# set KDE specific information #################

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core DBus Gui Qml Quick Svg Widgets Xml)

find_package(Qt5Test ${QT_MIN_VERSION} CONFIG QUIET)
set_package_properties(Qt5Test PROPERTIES
         PURPOSE "Required for tests"
         TYPE OPTIONAL
         )
add_feature_info("Qt5Test" Qt5Test_FOUND "Required for building tests")
if (NOT Qt5Test_FOUND)
    set(BUILD_TESTING OFF CACHE BOOL "Build the testing tree.")
endif()

find_package(KF5Kirigami2 ${KF5_MIN_VERSION})
set_package_properties(KF5Kirigami2 PROPERTIES
    DESCRIPTION "KDE's lightweight user interface framework for mobile and convergent applications"
    URL "https://techbase.kde.org/Kirigami"
    PURPOSE "Required by lookandfeelexplorer"
    TYPE RUNTIME
)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Archive Completion Config ConfigWidgets CoreAddons DBusAddons Declarative I18n IconThemes
    KIO Plasma PlasmaQuick Service TextEditor WidgetsAddons DocTools)

#########################################################################

add_definitions(
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050900
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_CAST_TO_ASCII
#     -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_STRICT_ITERATORS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
)

add_subdirectory(cuttlefish)
add_subdirectory(engineexplorer)
add_subdirectory(plasmoidviewer)
add_subdirectory(themeexplorer)
add_subdirectory(lookandfeelexplorer)
#add_subdirectory(wallpaperviewer)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
