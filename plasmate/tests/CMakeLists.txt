include(ECMMarkAsTest)

macro(PLASMATE_UNIT_TEST testname srcs libraries)
    set(testname_SRCS ${testname}.cpp
                      ${srcs}
    )

    add_executable(${testname} ${testname_SRCS})
    add_test(plasmate-${testname} ${testname})

    ecm_mark_as_test(${testname})

    target_link_libraries(${testname} ${libraries} Qt5::Test Qt5::Core)
endmacro()

# packageHandertest
set(plasmoidhandler_SRCS ../packagehandler/packagehandler.cpp
                        ../packagehandler/plasmoidhandler.cpp
                        ../editors/metadata/metadatahandler.cpp
)

set(plasmoidhandler_LIBS KF5::Plasma)

PLASMATE_UNIT_TEST(plasmoidhandlertest "${plasmoidhandler_SRCS}"
                                       "${plasmoidhandler_LIBS}"
)

# packagemodeltest
set(packagemodeltest_SRCS modeltest.cpp
                          ../packagemodel.cpp
                          ../editors/metadata/metadatahandler.cpp
                          ${plasmoidhandler_SRCS}
)

set(packagemodeltest_LIBS ${plasmoidhandler_LIBS})

plasmate_unit_test(packagemodeltest "${packagemodeltest_SRCS}"
                                    "${packagemodeltest_LIBS}"
)

# metadatahandlertest
set(metadatahandlertest_SRCS ../editors/metadata/metadatahandler.cpp)
set(metadatahandlertest_LIBS KF5::ConfigCore)

plasmate_unit_test(metadatahandlertest "${metadatahandlertest_SRCS}"
                                       "${metadatahandlertest_LIBS}"
)

# projecthandlertest
set(projecthandlertest_SRCS ../projecthandler.cpp
                            ${plasmoidhandler_SRCS}
                            ../editors/metadata/metadatahandler.cpp
)

set(projecthandlertest_LIBS KF5::Plasma)

plasmate_unit_test(projecthandlertest "${projecthandlertest_SRCS}"
                                      "${projecthandlertest_LIBS}"
)

