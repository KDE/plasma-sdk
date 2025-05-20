# SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(lookandfeelexplorer)

ecm_add_qml_module(lookandfeelexplorer
    GENERATE_PLUGIN_SOURCE
    URI org.kde.plasma.lookandfeelexplorer
    SOURCES
        main.cpp
        lnflogic.cpp
        lnflistmodel.cpp
    QML_FILES
        qml/FormField.qml
        qml/MetadataEditor.qml
        qml/Main.qml
)


target_link_libraries(lookandfeelexplorer PRIVATE
    Qt::Gui
    Qt::Quick
    Qt::Widgets
    Qt::DBus
    KF6::Archive
    KF6::I18n
    KF6::I18nQml
    KF6::Package
    KF6::KIOCore
    KF6::KIOWidgets
    Plasma::Plasma
    Plasma::PlasmaQuick
)

install(TARGETS lookandfeelexplorer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
