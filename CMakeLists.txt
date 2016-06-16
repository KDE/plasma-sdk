project(plasma-sdk)
set(PROJECT_VERSION "5.6.95")
set(PROJECT_VERSION_MAJOR 5)
cmake_minimum_required(VERSION 2.8.12)

set(QT_MIN_VERSION "5.4.0")
set(KF5_MIN_VERSION "5.8.0")

################# Disallow in-source build #################

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(FATAL_ERROR "plasmate requires an out of source build. Please create a separate build directory and run 'cmake path_to_plasmate [options]' there.")
endif()

# Make CPack available to easy generate binary packages
include(CPack)

include(FeatureSummary)

################# set KDE specific information #################

find_package(ECM 1.8.0 REQUIRED NO_MODULE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 REQUIRED NO_MODULE COMPONENTS Core DBus Gui Qml Quick Svg Widgets Xml)

find_package(Qt5WebKit ${QT_MIN_VERSION} CONFIG)
set_package_properties(Qt5WebKit PROPERTIES
         PURPOSE "Required for plasmate"
         TYPE OPTIONAL
         )
find_package(Qt5WebKitWidgets ${QT_MIN_VERSION} CONFIG)
set_package_properties(Qt5WebKitWidgets PROPERTIES
         PURPOSE "Required for plasmate"
         TYPE OPTIONAL
         )

find_package(Qt5Test ${QT_MIN_VERSION} CONFIG QUIET)
set_package_properties(Qt5Test PROPERTIES
         PURPOSE "Required for tests"
         TYPE OPTIONAL
         )
add_feature_info("Qt5Test" Qt5Test_FOUND "Required for building tests")
if (NOT Qt5Test_FOUND)
    set(BUILD_TESTING OFF CACHE BOOL "Build the testing tree.")
endif()

find_package(KDevPlatform 4.90.90)
set_package_properties(KDevPlatform PROPERTIES
    PURPOSE "Required for plasmate"
    TYPE OPTIONAL
)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

if(KDE_PLATFORM_FEATURE_DISABLE_DEPRECATED)
   set(KDE_NO_DEPRECATED TRUE)
   set(CMAKE_AUTOMOC_MOC_OPTIONS "-DKDE_NO_DEPRECATED")
endif()

################# Enable C++11 features for clang and gcc #################

if(UNIX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++0x")
endif()


# Load CMake, Compiler and InstallDirs settings from KF5 and the following are already somewhat "done" tier1/tier2 libs from kdelibs:
find_package(KF5 REQUIRED COMPONENTS
    Archive Completion Config ConfigWidgets CoreAddons DBusAddons Declarative I18n IconThemes ItemModels
    KIO Plasma PlasmaQuick Service TextEditor WidgetsAddons XmlGui WindowSystem)

#########################################################################

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_KEYWORDS)

add_subdirectory(cuttlefish)
add_subdirectory(engineexplorer)
add_subdirectory(plasmoidviewer)
add_subdirectory(themeexplorer)
#add_subdirectory(remote-widgets-browser)
#add_subdirectory(wallpaperviewer)
if (KDevPlatform_FOUND AND Qt5WebKit_FOUND AND Qt5WebKitWidgets_FOUND)
    add_subdirectory(plasmate)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
