/*
    SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "accessibledebugger.h"

#include <QQmlEngine>

#include "a11y_debug.h"

namespace
{
inline bool isDebugEnabled()
{
    return PLASMA_SDK_A11Y().isDebugEnabled();
}

constexpr const char *PLUGIN_URI = "org.kde.plasma.sdk.a11y";
}

void WindowData::operator=(QQuickWindow *newWindow)
{
    *this = (QQuickItem *)nullptr;
    if (window) {
        QObject::disconnect(window, nullptr, parent, nullptr);
    }

    window = newWindow;
    if (window && parent->options() != AccessibleDebugger::Disabled) {
        QObject::connect(window, &QQuickWindow::activeFocusItemChanged, parent, &AccessibleDebugger::onActiveFocusItemChanged);
        *this = window->activeFocusItem();
    }
}

void WindowData::operator=(QQuickItem *newActiveFocusItem)
{
    *this = (QQuickKeyNavigationAttached *)nullptr;
    *this = (QQuickAccessibleAttached *)nullptr;
    if (activeFocusItem) {
        QObject::disconnect(activeFocusItem, nullptr, parent, nullptr);
    }

    activeFocusItem = newActiveFocusItem;
    if (activeFocusItem) {
        if (parent->options() & AccessibleDebugger::KeyNavigation) {
            *this = qobject_cast<QQuickKeyNavigationAttached *>(qmlAttachedPropertiesObject<QQuickKeyNavigationAttached>(activeFocusItem, false));
        }
        if (parent->options() & AccessibleDebugger::Accessible) {
            *this = qobject_cast<QQuickAccessibleAttached *>(qmlAttachedPropertiesObject<QQuickAccessibleAttached>(activeFocusItem, false));
        }
    }
}

void WindowData::operator=(QQuickKeyNavigationAttached *newAttachedObject)
{
    if (keyNavigationAttached) {
        QObject::disconnect(keyNavigationAttached, nullptr, parent, nullptr);
    }

    keyNavigationAttached = newAttachedObject;
    if (keyNavigationAttached) {
        if (parent->options() & AccessibleDebugger::KeyNavigationUp) {
            QObject::connect(keyNavigationAttached, &QQuickKeyNavigationAttached::upChanged, parent, &AccessibleDebugger::onKeyNavigationUpChanged);
        }
        if (parent->options() & AccessibleDebugger::KeyNavigationDown) {
            QObject::connect(keyNavigationAttached, &QQuickKeyNavigationAttached::downChanged, parent, &AccessibleDebugger::onKeyNavigationDownChanged);
        }
        if (parent->options() & AccessibleDebugger::KeyNavigationLeft) {
            QObject::connect(keyNavigationAttached, &QQuickKeyNavigationAttached::leftChanged, parent, &AccessibleDebugger::onKeyNavigationLeftChanged);
        }
        if (parent->options() & AccessibleDebugger::KeyNavigationRight) {
            QObject::connect(keyNavigationAttached, &QQuickKeyNavigationAttached::rightChanged, parent, &AccessibleDebugger::onKeyNavigationRightChanged);
        }
        if (parent->options() & AccessibleDebugger::KeyNavigationTab) {
            QObject::connect(keyNavigationAttached, &QQuickKeyNavigationAttached::tabChanged, parent, &AccessibleDebugger::onKeyNavigationTabChanged);
        }
        if (parent->options() & AccessibleDebugger::KeyNavigationBackTab) {
            QObject::connect(keyNavigationAttached, &QQuickKeyNavigationAttached::backtabChanged, parent, &AccessibleDebugger::onKeyNavigationBackTabChanged);
        }
    }
}

void WindowData::operator=(QQuickAccessibleAttached *newAttachedObject)
{
    if (accessibleAttached) {
        QObject::disconnect(accessibleAttached, nullptr, parent, nullptr);
    }

    accessibleAttached = newAttachedObject;
    if (accessibleAttached) {
        if (parent->options() & AccessibleDebugger::AccessibleName) {
            QObject::connect(accessibleAttached, &QQuickAccessibleAttached::nameChanged, parent, &AccessibleDebugger::onAccessibleNameChanged);
        }
        if (parent->options() & AccessibleDebugger::AccessibleDescription) {
            QObject::connect(accessibleAttached, &QQuickAccessibleAttached::descriptionChanged, parent, &AccessibleDebugger::onAccessibleDescriptionChanged);
        }
        if (parent->options() & AccessibleDebugger::AccessibleRole) {
            QObject::connect(accessibleAttached, &QQuickAccessibleAttached::roleChanged, parent, &AccessibleDebugger::onAccessibleRoleChanged);
        }
    }
}

AccessibleDebugger::AccessibleDebugger(QObject *parent)
    : QObject(parent)
{
}

AccessibleDebugger::~AccessibleDebugger()
{
}

AccessibleDebugger::DebugOptions AccessibleDebugger::options() const
{
    return m_options;
}

void AccessibleDebugger::setOptions(AccessibleDebugger::DebugOptions newOptions)
{
    if (!isDebugEnabled() || m_options == newOptions) {
        return;
    }

    m_options = newOptions;
    if (m_highlightOverlayItem) {
        m_highlightOverlayItem->deleteLater();
    }
    if (m_focusChainIndicatorItem) {
        m_focusChainIndicatorItem->deleteLater();
    }
    m_windowData = m_windowData.window; // Reload all connections
    Q_EMIT optionsChanged();
}

QQuickWindow *AccessibleDebugger::targetWindow() const
{
    return m_windowData;
}

void AccessibleDebugger::setTargetWindow(QQuickWindow *window)
{
    if (!isDebugEnabled() || m_windowData == window) {
        return;
    }

    m_windowData = window;
    Q_EMIT activeFocusItemChanged();
    Q_EMIT targetWindowChanged();

    sendFullInformation();
}

QQuickItem *AccessibleDebugger::activeFocusItem() const
{
    return m_windowData.activeFocusItem;
}

void AccessibleDebugger::onActiveFocusItemChanged()
{
    Q_ASSERT(isDebugEnabled() && m_options != Disabled);

    m_windowData = m_windowData->activeFocusItem();
    Q_EMIT activeFocusItemChanged();

    sendFullInformation();
    drawHighlight();
    drawFocusChain();
}

void AccessibleDebugger::onKeyNavigationUpChanged()
{
    Q_ASSERT(isDebugEnabled() && m_options & KeyNavigationUp);
    if (m_focusChainIndicatorItem) {
        m_focusChainIndicatorItem->setProperty("_keyNavigationUp", QVariant::fromValue(m_windowData.keyNavigationAttached->up()));
    }
    qCDebug(PLASMA_SDK_A11Y).nospace() << "KeyNavigation.up of \"" << m_windowData.activeFocusItem << "\": " << m_windowData.keyNavigationAttached->up();
}

void AccessibleDebugger::onKeyNavigationDownChanged()
{
    Q_ASSERT(isDebugEnabled() && m_options & KeyNavigationDown);
    if (m_focusChainIndicatorItem) {
        m_focusChainIndicatorItem->setProperty("_keyNavigationDown", QVariant::fromValue(m_windowData.keyNavigationAttached->down()));
    }
    qCDebug(PLASMA_SDK_A11Y).nospace() << "KeyNavigation.down of \"" << m_windowData.activeFocusItem << "\": " << m_windowData.keyNavigationAttached->down();
}

void AccessibleDebugger::onKeyNavigationLeftChanged()
{
    Q_ASSERT(isDebugEnabled() && m_options & KeyNavigationLeft);
    if (m_focusChainIndicatorItem) {
        m_focusChainIndicatorItem->setProperty("_keyNavigationLeft", QVariant::fromValue(m_windowData.keyNavigationAttached->left()));
    }
    qCDebug(PLASMA_SDK_A11Y).nospace() << "KeyNavigation.left of \"" << m_windowData.activeFocusItem << "\": " << m_windowData.keyNavigationAttached->left();
}

void AccessibleDebugger::onKeyNavigationRightChanged()
{
    Q_ASSERT(isDebugEnabled() && m_options & KeyNavigationRight);
    if (m_focusChainIndicatorItem) {
        m_focusChainIndicatorItem->setProperty("_keyNavigationRight", QVariant::fromValue(m_windowData.keyNavigationAttached->right()));
    }
    qCDebug(PLASMA_SDK_A11Y).nospace() << "KeyNavigation.right of \"" << m_windowData.activeFocusItem << "\": " << m_windowData.keyNavigationAttached->right();
}

void AccessibleDebugger::onKeyNavigationTabChanged()
{
    Q_ASSERT(isDebugEnabled() && m_options & KeyNavigationTab);
    if (m_focusChainIndicatorItem) {
        m_focusChainIndicatorItem->setProperty("_keyNavigationTab", QVariant::fromValue(m_windowData.keyNavigationAttached->tab()));
    }
    qCDebug(PLASMA_SDK_A11Y).nospace() << "KeyNavigation.tab of \"" << m_windowData.activeFocusItem << "\": " << m_windowData.keyNavigationAttached->tab();
}

void AccessibleDebugger::onKeyNavigationBackTabChanged()
{
    Q_ASSERT(isDebugEnabled() && m_options & KeyNavigationBackTab);
    if (m_focusChainIndicatorItem) {
        m_focusChainIndicatorItem->setProperty("_keyNavigationBacktab", QVariant::fromValue(m_windowData.keyNavigationAttached->backtab()));
    }
    qCDebug(PLASMA_SDK_A11Y).nospace() << "KeyNavigation.backtab of \"" << m_windowData.activeFocusItem
                                       << "\": " << m_windowData.keyNavigationAttached->backtab();
}

void AccessibleDebugger::onAccessibleNameChanged()
{
    Q_ASSERT(isDebugEnabled() && m_options & AccessibleName);
    qCDebug(PLASMA_SDK_A11Y).nospace() << "Accessible.name of \"" << m_windowData.activeFocusItem << "\": " << m_windowData.accessibleAttached->name();
}

void AccessibleDebugger::onAccessibleDescriptionChanged()
{
    Q_ASSERT(isDebugEnabled() && m_options & AccessibleDescription);
    qCDebug(PLASMA_SDK_A11Y).nospace() << "KeyNavigation.backtab of \"" << m_windowData.activeFocusItem
                                       << "\": " << m_windowData.accessibleAttached->description();
}

void AccessibleDebugger::onAccessibleRoleChanged()
{
    Q_ASSERT(isDebugEnabled() && m_options & AccessibleRole);
    qCDebug(PLASMA_SDK_A11Y).nospace() << "KeyNavigation.backtab of \"" << m_windowData.activeFocusItem << "\": " << m_windowData.accessibleAttached->role();
}

QDebug AccessibleDebugger::itemDebug(const char *prefix) const
{
    QMessageLogger logger{nullptr, 0, nullptr, PLASMA_SDK_A11Y().categoryName()};
    QDebug debugger = logger.debug().nospace();
    debugger << "    " << prefix;
    if (!(m_options & ActiveFocusItem)) {
        debugger << " of \"" << m_windowData.activeFocusItem << "\"";
    }
    debugger << ": ";
    return debugger;
}

void AccessibleDebugger::sendFullInformation()
{
    if (!m_windowData) {
        return;
    }
    qCDebug(PLASMA_SDK_A11Y) << "======BEGIN Information======";
    if (m_options & ActiveFocusItem) {
        qCDebug(PLASMA_SDK_A11Y).nospace() << "ActiveFocusItem in " << m_windowData->title() << ": " << m_windowData.activeFocusItem;
    }

    if (!m_windowData.activeFocusItem) {
        qCDebug(PLASMA_SDK_A11Y) << "======END Information======";
        return;
    }
    if (m_options & PreviousItemInFocusChain) {
        itemDebug("PreviousItemInFocusChain") << m_windowData.activeFocusItem->nextItemInFocusChain(false);
    }
    if (m_options & NextItemInFocusChain) {
        itemDebug("NextItemInFocusChain") << m_windowData.activeFocusItem->nextItemInFocusChain(true);
    }

    if (m_windowData.keyNavigationAttached) {
        if (m_options & AccessibleDebugger::KeyNavigationUp) {
            itemDebug("KeyNavigation.up") << m_windowData.keyNavigationAttached->up();
        }
        if (m_options & AccessibleDebugger::KeyNavigationDown) {
            itemDebug("KeyNavigation.down") << m_windowData.keyNavigationAttached->down();
        }
        if (m_options & AccessibleDebugger::KeyNavigationLeft) {
            itemDebug("KeyNavigation.left") << m_windowData.keyNavigationAttached->left();
        }
        if (m_options & AccessibleDebugger::KeyNavigationRight) {
            itemDebug("KeyNavigation.right") << m_windowData.keyNavigationAttached->right();
        }
        if (m_options & AccessibleDebugger::KeyNavigationTab) {
            itemDebug("KeyNavigation.tab") << m_windowData.keyNavigationAttached->tab();
        }
        if (m_options & AccessibleDebugger::KeyNavigationBackTab) {
            itemDebug("KeyNavigation.backtab") << m_windowData.keyNavigationAttached->backtab();
        }
    }

    if (m_windowData.accessibleAttached) {
        if (m_options & AccessibleDebugger::AccessibleName) {
            itemDebug("Accessible.name") << m_windowData.accessibleAttached->name();
        }
        if (m_options & AccessibleDebugger::AccessibleDescription) {
            itemDebug("Accessible.description") << m_windowData.accessibleAttached->description();
        }
        if (m_options & AccessibleDebugger::AccessibleRole) {
            itemDebug("Accessible.role") << m_windowData.accessibleAttached->role();
        }
    }

    qCDebug(PLASMA_SDK_A11Y) << "======END Information======";
}

void AccessibleDebugger::drawHighlight()
{
    if (!m_windowData.activeFocusItem || !(m_options & FocusChain)) {
        if (m_highlightOverlayItem) {
            m_highlightOverlayItem->setProperty("overlayParent", QVariant::fromValue((QQuickItem *)nullptr));
            return;
        }
        return;
    }

    QQmlContext *context = QQmlEngine::contextForObject(m_windowData.activeFocusItem);
    if (!context) {
        return;
    }

    if (m_highlightOverlayItem) {
        m_highlightOverlayItem->setProperty("overlayParent", QVariant::fromValue(m_windowData.activeFocusItem.data()));
        return;
    }

    auto highlightComponent = new QQmlComponent(context->engine(), PLUGIN_URI, "HighlightOverlay", QQmlComponent::Asynchronous, m_windowData.activeFocusItem);
    auto create = [this, context, highlightComponent] {
        if (m_highlightCreated) {
            return;
        }
        m_highlightCreated = true;
        QVariantMap initialProperties;
        initialProperties.insert(QStringLiteral("parent"), QVariant::fromValue(m_windowData->contentItem()));
        initialProperties.insert(QStringLiteral("overlayParent"), QVariant::fromValue(m_windowData.activeFocusItem.data()));
        m_highlightOverlayItem = static_cast<QQuickItem *>(highlightComponent->createWithInitialProperties(initialProperties, context));
        highlightComponent->deleteLater();
    };
    if (highlightComponent->isLoading()) {
        connect(highlightComponent, &QQmlComponent::statusChanged, this, create);
    } else if (highlightComponent->isReady()) {
        create();
    } else {
        qCCritical(PLASMA_SDK_A11Y) << "Error creating HighlightOverlay:" << highlightComponent->errorString();
        delete highlightComponent;
    }
}

void AccessibleDebugger::drawFocusChain()
{
    if (!m_windowData.activeFocusItem || !(m_options & (FocusChain | KeyNavigation))) {
        if (m_focusChainIndicatorItem) {
            m_focusChainIndicatorItem->setProperty("_activeFocusItem", QVariant::fromValue((QQuickItem *)nullptr));
        }
        return;
    }

    QQmlContext *context = QQmlEngine::contextForObject(m_windowData.activeFocusItem);
    if (!context) {
        return;
    }

    if (m_focusChainIndicatorItem) {
        m_focusChainIndicatorItem->setProperty("_activeFocusItem", QVariant::fromValue(m_windowData.activeFocusItem.data()));
        m_focusChainIndicatorItem->setProperty("_previousItemInFocusChain", QVariant::fromValue(m_windowData.activeFocusItem->nextItemInFocusChain(false)));
        m_focusChainIndicatorItem->setProperty("_nextItemInFocusChain", QVariant::fromValue(m_windowData.activeFocusItem->nextItemInFocusChain(true)));
        if (m_windowData.keyNavigationAttached) {
            m_focusChainIndicatorItem->setProperty("_keyNavigationUp", QVariant::fromValue(m_windowData.keyNavigationAttached->up()));
            m_focusChainIndicatorItem->setProperty("_keyNavigationDown", QVariant::fromValue(m_windowData.keyNavigationAttached->down()));
            m_focusChainIndicatorItem->setProperty("_keyNavigationLeft", QVariant::fromValue(m_windowData.keyNavigationAttached->left()));
            m_focusChainIndicatorItem->setProperty("_keyNavigationRight", QVariant::fromValue(m_windowData.keyNavigationAttached->right()));
            m_focusChainIndicatorItem->setProperty("_keyNavigationTab", QVariant::fromValue(m_windowData.keyNavigationAttached->tab()));
            m_focusChainIndicatorItem->setProperty("_keyNavigationBacktab", QVariant::fromValue(m_windowData.keyNavigationAttached->backtab()));
        } else {
            m_focusChainIndicatorItem->setProperty("_keyNavigationUp", QVariant::fromValue((QQuickItem *)nullptr));
            m_focusChainIndicatorItem->setProperty("_keyNavigationDown", QVariant::fromValue((QQuickItem *)nullptr));
            m_focusChainIndicatorItem->setProperty("_keyNavigationLeft", QVariant::fromValue((QQuickItem *)nullptr));
            m_focusChainIndicatorItem->setProperty("_keyNavigationRight", QVariant::fromValue((QQuickItem *)nullptr));
            m_focusChainIndicatorItem->setProperty("_keyNavigationTab", QVariant::fromValue((QQuickItem *)nullptr));
            m_focusChainIndicatorItem->setProperty("_keyNavigationBacktab", QVariant::fromValue((QQuickItem *)nullptr));
        }
        return;
    }

    auto component = new QQmlComponent(context->engine(), PLUGIN_URI, "FocusChainIndicator", QQmlComponent::Asynchronous, m_windowData.activeFocusItem);
    auto create = [this, context, component] {
        if (m_focusChainCreated) {
            return;
        }
        m_focusChainCreated = true;
        QVariantMap initialProperties;
        initialProperties.insert(QStringLiteral("parent"), QVariant::fromValue(m_windowData->contentItem()));
        initialProperties.insert(QStringLiteral("drawPreviousLine"), bool(m_options & PreviousItemInFocusChain));
        initialProperties.insert(QStringLiteral("drawNextLine"), bool(m_options & NextItemInFocusChain));
        initialProperties.insert(QStringLiteral("drawKeyNavigation"), bool(m_options & KeyNavigation));
        if (m_windowData.activeFocusItem) {
            initialProperties.insert(QStringLiteral("_activeFocusItem"), QVariant::fromValue(m_windowData.activeFocusItem.data()));
            initialProperties.insert(QStringLiteral("_previousItemInFocusChain"),
                                     QVariant::fromValue(m_windowData.activeFocusItem->nextItemInFocusChain(false)));
            initialProperties.insert(QStringLiteral("_nextItemInFocusChain"), QVariant::fromValue(m_windowData.activeFocusItem->nextItemInFocusChain(true)));
        }
        if (m_windowData.keyNavigationAttached) {
            initialProperties.insert(QStringLiteral("_keyNavigationUp"), QVariant::fromValue(m_windowData.keyNavigationAttached->up()));
            initialProperties.insert(QStringLiteral("_keyNavigationDown"), QVariant::fromValue(m_windowData.keyNavigationAttached->down()));
            initialProperties.insert(QStringLiteral("_keyNavigationLeft"), QVariant::fromValue(m_windowData.keyNavigationAttached->left()));
            initialProperties.insert(QStringLiteral("_keyNavigationRight"), QVariant::fromValue(m_windowData.keyNavigationAttached->right()));
            initialProperties.insert(QStringLiteral("_keyNavigationTab"), QVariant::fromValue(m_windowData.keyNavigationAttached->tab()));
            initialProperties.insert(QStringLiteral("_keyNavigationBacktab"), QVariant::fromValue(m_windowData.keyNavigationAttached->backtab()));
        }
        m_focusChainIndicatorItem = static_cast<QQuickItem *>(component->createWithInitialProperties(initialProperties, context));
        if (!m_focusChainIndicatorItem) {
            qmlWarning(m_windowData, component->errors());
            Q_UNREACHABLE();
        }
        component->deleteLater();
    };
    if (component->isLoading()) {
        connect(component, &QQmlComponent::statusChanged, this, create);
    } else if (component->isReady()) {
        create();
    } else {
        qCCritical(PLASMA_SDK_A11Y) << "Error creating FocusChainIndicator:" << component->errorString();
        delete component;
    }
}
