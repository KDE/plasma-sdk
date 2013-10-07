project(plasma-sdk)

cmake_minimum_required(VERSION 2.8.10.1)

project(plasma)

################# Disallow in-source build #################

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(FATAL_ERROR "plasmate requires an out of source build. Please create a separate build directory and run 'cmake path_to_plasmate [options]' there.")
endif()

# Make CPack available to easy generate binary packages
include(CPack)

include(FeatureSummary)

################# set KDE specific information #################

find_package(ECM 0.0.8 REQUIRED NO_MODULE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

find_package(Qt5Core REQUIRED NO_MODULE)

find_package(Qt5Transitional MODULE)
find_package(Qt5Quick REQUIRED NO_MODULE)
find_package(Qt5X11Extras REQUIRED NO_MODULE)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)

if(KDE_PLATFORM_FEATURE_DISABLE_DEPRECATED)
   set(KDE_NO_DEPRECATED TRUE)
   set(CMAKE_AUTOMOC_MOC_OPTIONS "-DKDE_NO_DEPRECATED")
endif()

################# Enable C++11 features for clang and gcc #################

if(UNIX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++0x")
endif()

################# now find all used packages #################

set (QT_MIN_VERSION "5.2.0")


# Load CMake, Compiler and InstallDirs settings from KF5 and the following are already somewhat "done" tier1/tier2 libs from kdelibs:
find_package(KF5 MODULE REQUIRED COMPONENTS CMake Compiler InstallDirs
    KIdleTime ItemModels KWidgetsAddons KWindowSystem KCodecs KArchive KCoreAddons Solid ThreadWeaver
    KConfig KAuth KJS KWallet KDBusAddons Sonnet
    KI18n KGuiAddons KService KConfigWidgets ItemViews KNotifications KIconThemes KStyle KCompletion KJobWidgets KTextWidgets XmlGui KCrash
    KUnitConversion KDE4Attic KBookmarks Kross)

find_package(Plasma REQUIRED NO_MODULE)
find_package(KDeclarative REQUIRED NO_MODULE)
find_package(KIO REQUIRED NO_MODULE)

# needed for set_package_properties macro
find_package(KDELibs4 REQUIRED NO_MODULE)
#find_package(KDE4Support REQUIRED NO_MODULE)


find_package(LibAttica NO_MODULE)
set_package_properties(LibAttica PROPERTIES DESCRIPTION "Support for Get Hot New Stuff related stuff"
                       URL "https://projects.kde.org/attica"
                       TYPE REQUIRED
                      )

#########################################################################

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

remove_definitions(-DQT3_SUPPORT_WARNINGS -DQT3_SUPPORT)
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_KEYWORDS)


################# setup the include directories #################
include_directories( ${kdeqt5staging_INCLUDE_DIRS}
                     ${kdeqt5staging_INCLUDE_DIR}
                     ${KDE4_INCLUDES}
                     ${KF5_INCLUDE_DIRS}  # since e-c-m 0.0.7
                   )

# for including config.h and for includes like <kparts/foo.h>
include_directories( ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/src ${CMAKE_SOURCE_DIR}/interfaces ${CMAKE_SOURCE_DIR}/src/plasma/includes)


add_subdirectory(engineexplorer)
#add_subdirectory(plasmoidviewer)
#add_subdirectory(remote-widgets-browser)
#add_subdirectory(wallpaperviewer)
#add_subdirectory(plasmate)
