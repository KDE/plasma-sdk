/*
    SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once

#include <QObject>
#include <QPointer>
#include <QQuickItem>
#include <QQuickWindow>
#include <QtQuick/private/qquickaccessibleattached_p.h>
#include <QtQuick/private/qquickitem_p.h>
#include <qqmlregistration.h>

class AccessibleDebugger;

struct WindowData {
    explicit WindowData(AccessibleDebugger *parent)
        : parent(parent)
    {
    }

    AccessibleDebugger *parent;
    QPointer<QQuickWindow> window;
    QPointer<QQuickItem> activeFocusItem;
    QPointer<QQuickKeyNavigationAttached> keyNavigationAttached;
    QPointer<QQuickAccessibleAttached> accessibleAttached;

    void operator=(QQuickWindow *newWindow);
    void operator=(QQuickItem *newActiveFocusItem);
    void operator=(QQuickKeyNavigationAttached *newAttachedObject);
    void operator=(QQuickAccessibleAttached *newAttachedObject);

    explicit operator bool() const
    {
        return !window.isNull();
    }

    operator QQuickWindow *() const
    {
        return window;
    }

    QQuickWindow *operator->()
    {
        return window;
    }
};

class AccessibleDebugger : public QObject
{
    Q_OBJECT

    Q_PROPERTY(DebugOptions options READ options WRITE setOptions NOTIFY optionsChanged)
    Q_PROPERTY(QQuickWindow *targetWindow READ targetWindow WRITE setTargetWindow NOTIFY targetWindowChanged)
    Q_PROPERTY(QQuickItem *activeFocusItem READ activeFocusItem NOTIFY activeFocusItemChanged)
    QML_ELEMENT

public:
    enum DebugOption : std::uint32_t {
        Disabled = 0,
        ActiveFocusItem = 1 << 0,
        PreviousItemInFocusChain = 1 << 1,
        NextItemInFocusChain = 1 << 2,
        FocusChain = ActiveFocusItem | PreviousItemInFocusChain | NextItemInFocusChain,
        KeyNavigationUp = 1 << 3,
        KeyNavigationDown = 1 << 4,
        KeyNavigationLeft = 1 << 5,
        KeyNavigationRight = 1 << 6,
        KeyNavigationTab = 1 << 7,
        KeyNavigationBackTab = 1 << 8,
        KeyNavigation = KeyNavigationUp | KeyNavigationDown | KeyNavigationLeft | KeyNavigationRight | KeyNavigationTab | KeyNavigationBackTab,
        AccessibleName = 1 << 9,
        AccessibleDescription = 1 << 10,
        AccessibleRole = 1 << 11,
        Accessible = AccessibleName | AccessibleDescription | AccessibleRole,
        Enabled = FocusChain | KeyNavigation | Accessible,
    };
    Q_ENUM(DebugOption)
    Q_DECLARE_FLAGS(DebugOptions, DebugOption)

    explicit AccessibleDebugger(QObject *parent = nullptr);
    ~AccessibleDebugger() override;

    DebugOptions options() const;
    void setOptions(DebugOptions newOptions);

    QQuickWindow *targetWindow() const;
    void setTargetWindow(QQuickWindow *window);

    QQuickItem *activeFocusItem() const;

Q_SIGNALS:
    void optionsChanged();
    void targetWindowChanged();
    void activeFocusItemChanged();

private Q_SLOTS:
    void onActiveFocusItemChanged();
    void onKeyNavigationUpChanged();
    void onKeyNavigationDownChanged();
    void onKeyNavigationLeftChanged();
    void onKeyNavigationRightChanged();
    void onKeyNavigationTabChanged();
    void onKeyNavigationBackTabChanged();

    void onAccessibleNameChanged();
    void onAccessibleDescriptionChanged();
    void onAccessibleRoleChanged();

private:
    [[nodiscard]] QDebug itemDebug(const char *prefix) const;
    void sendFullInformation();
    void drawHighlight();
    void drawFocusChain();

    DebugOptions m_options = Disabled;
    WindowData m_windowData{this};
    QPointer<QQuickItem> m_highlightOverlayItem;
    bool m_highlightCreated = false;
    QPointer<QQuickItem> m_focusChainIndicatorItem;
    bool m_focusChainCreated = false;

    friend WindowData;
};
