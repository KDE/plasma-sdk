/*
 *  SPDX-FileCopyrightText: 2025 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "windowstate.h"

#include <KConfigGroup>

using namespace Qt::Literals;

WindowState::WindowState(QObject *parent)
    : QObject(parent)
{
    m_stateConfig = KSharedConfig::openStateConfig();
}

QByteArray WindowState::mainSplitView() const
{
    return m_stateConfig->group(u"MainWindow"_s).readEntry("mainSplitView", QByteArray());
}

void WindowState::setMainSplitView(const QByteArray &state)
{
    m_stateConfig->group(u"MainWindow"_s).writeEntry("mainSplitView", state);
    Q_EMIT mainSplitViewChanged();
}
