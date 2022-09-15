cmake_minimum_required(VERSION 3.16)

project(plasma-sdk)
set(PROJECT_VERSION "5.25.80")
set(PROJECT_VERSION_MAJOR 5)

set(QT_MIN_VERSION "5.15.2")
set(KF5_MIN_VERSION "5.98.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################# Disallow in-source build #################

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(FATAL_ERROR "plasmate requires an out of source build. Please create a separate build directory and run 'cmake path_to_plasmate [options]' there.")
endif()

################# set KDE specific information #################

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)
include(ECMDeprecationSettings)
include(FeatureSummary)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core DBus Gui Qml Quick Svg Widgets Xml)

find_package(Qt${QT_MAJOR_VERSION}Test ${QT_MIN_VERSION} CONFIG QUIET)
set_package_properties(Qt${QT_MAJOR_VERSION}Test PROPERTIES
         PURPOSE "Required for tests"
         TYPE OPTIONAL
         )
add_feature_info("Qt${QT_MAJOR_VERSION}Test" Qt5Test_FOUND "Required for building tests")
if (NOT Qt${QT_MAJOR_VERSION}Test_FOUND)
    set(BUILD_TESTING OFF CACHE BOOL "Build the testing tree.")
endif()

if (QT_MAJOR_VERSION STREQUAL "6")
    find_package(Qt6Core5Compat)
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
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_CAST_TO_ASCII
#     -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_STRICT_ITERATORS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
)

ecm_set_disabled_deprecation_versions(
    KF 5.94.0
)

add_subdirectory(cuttlefish)
add_subdirectory(engineexplorer)
add_subdirectory(plasmoidviewer)
add_subdirectory(themeexplorer)
add_subdirectory(lookandfeelexplorer)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
