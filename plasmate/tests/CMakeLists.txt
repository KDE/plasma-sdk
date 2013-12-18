set(packageHandler_SRCS packagehandlertest.cpp
                        ../packagehandler.cpp
)

add_executable(packagehandlertest ${packageHandler_SRCS})

target_link_libraries(packagehandlertest Qt5::Core Qt5::Test KF5::Plasma)

#FIXME
install(DIRECTORY ./templates DESTINATION ${DATA_INSTALL_DIR}/packagehandlertest)
