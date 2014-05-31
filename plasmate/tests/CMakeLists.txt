# plasmoidhandertest
set(plasmoidhandler_SRCS ../packagehandler/packagehandler.cpp
                         ../packagehandler/plasmoidhandler.cpp
                         ../editors/metadata/metadatahandler.cpp
)

set(plasmoidhandler_LIBS KF5::Plasma)

PLASMATE_UNIT_TEST(plasmoidhandlertest "${plasmoidhandler_SRCS}"
                                       "${plasmoidhandler_LIBS}"
)

# themehandlertest
set(themehandler_SRCS  ../packagehandler/packagehandler.cpp
                       ../packagehandler/themehandler.cpp
                       ../editors/metadata/metadatahandler.cpp
)

set(themehandler_LIBS KF5::Plasma)

PLASMATE_UNIT_TEST(themehandlertest "${themehandler_SRCS}"
                                    "${themehandler_LIBS}"
)

# metadatahandlertest
set(metadatahandlertest_SRCS ../editors/metadata/metadatahandler.cpp)
set(metadatahandlertest_LIBS KF5::ConfigCore)

PLASMATE_UNIT_TEST(metadatahandlertest "${metadatahandlertest_SRCS}"
                                       "${metadatahandlertest_LIBS}"
)


