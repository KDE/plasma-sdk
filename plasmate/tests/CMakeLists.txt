include(ECMMarkAsTest)

macro(PLASMATE_UNIT_TEST testname srcs libraries)
    set(testname_SRCS ${testname}.cpp
                      ${srcs}
    )

    add_executable(${testname} ${testname_SRCS})

    # plasmate_unit_test is also being called
    # by plasmate_ui_test, so we will check if we
    # have extra arguments. If we have then we don't
    # want to add them as tests because `make test` will
    # freeze.

    set(extra_macro_args ${ARGN})
    list(LENGTH extra_macro_args num_extra_args)

    if (${num_extra_args} EQUAL 0)
        add_test(plasmate-${testname} ${testname})
    endif()

    ecm_mark_as_test(${testname})

    target_link_libraries(${testname} ${libraries} Qt5::Test Qt5::Core)
endmacro()

# packageHandertest
set(packagehandler_SRCS ../packagehandler.cpp)
set(packagehandler_LIBS KF5::Plasma)

PLASMATE_UNIT_TEST(packagehandlertest "${packagehandler_SRCS}"
                                      "${packagehandler_LIBS}"
)

# packagemodeltest
set(packagemodeltest_SRCS modeltest.cpp
                          ../packagemodel.cpp
                          ${packagehandler_SRCS}
)

set(packagemodeltest_LIBS ${packagehandler_LIBS})

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
                            ../packagehandler.cpp
)

set(projecthandlertest_LIBS KF5::Plasma)

plasmate_unit_test(projecthandlertest "${projecthandlertest_SRCS}"
                                      "${projecthandlertest_LIBS}"
)

add_subdirectory(ui)

