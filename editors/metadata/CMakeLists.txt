project( plasmate-metadata-editor )

find_package(KDE4)
include(KDE4Defaults)

set(metadata_editor_SRCS
    metadataeditor.cpp)

kde4_add_ui_files (metadata_editor_SRCS
   metadata.ui
)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${KDE4_INCLUDES} ../..)

kde4_add_plugin(plasmate_metadata_editor ${metadata_editor_SRCS})
target_link_libraries(plasmate_metadata_editor ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS})

set( testmetadata_SRCS tests/test_metadata.cpp
     		       metadataeditor.cpp
      )

kde4_add_ui_files (testmetadata_SRCS
   metadata.ui
)

kde4_add_executable( testmetadata TEST ${testmetadata_SRCS})
target_link_libraries( testmetadata ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS} )
