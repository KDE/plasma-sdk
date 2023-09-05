# SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

ecm_add_qml_module(AccessibleTools URI "org.kde.plasma.sdk.a11y" GENERATE_PLUGIN_SOURCE VERSION 1.0)
target_sources(AccessibleTools PRIVATE
    accessibledebugger.cpp
)
ecm_target_qml_sources(AccessibleTools SOURCES
    FocusChainIndicator.qml
    HighlightOverlay.qml
)
ecm_qt_declare_logging_category(AccessibleTools
    HEADER a11y_debug.h
    IDENTIFIER PLASMA_SDK_A11Y
    CATEGORY_NAME org.kde.plasma.sdk.a11y
    DESCRIPTION "plasma sdk a11y"
    EXPORT PLASMASDK
)
target_link_libraries(AccessibleTools PRIVATE
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickPrivate
)
ecm_finalize_qml_module(AccessibleTools)
