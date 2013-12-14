project( plasmate-metadata-editor )

#find_package(KDE4 REQUIRED)
#include(KDE4Defaults)

set(metadata_editor_SRCS
    metadataeditor.cpp)

kde4_add_ui_files (metadata_editor_SRCS
   metadata.ui
)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${KDE4_INCLUDES} ../..)

add_library(plasmate_metadata_editor MODULE ${metadata_editor_SRCS})
target_link_libraries(plasmate_metadata_editor KF5::Plasma KF5::KIOCore KF5::KIOWidgets KF5::KDE4Support)

set( testmetadata_SRCS tests/test_metadata.cpp
     		       metadataeditor.cpp
      )

kde4_add_ui_files (testmetadata_SRCS
   metadata.ui
)

add_executable( testmetadata ${testmetadata_SRCS})
target_link_libraries( testmetadata KF5::KIOCore KF5::KIOWidgets KF5::Plasma KF5::KDE4Support )
