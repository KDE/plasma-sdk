/*
 *  SPDX-FileCopyrightText: 2025 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QObject>
#include <qqmlregistration.h>

#include <KSharedConfig>

class WindowState : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QByteArray mainSplitView READ mainSplitView WRITE setMainSplitView NOTIFY mainSplitViewChanged)

public:
    WindowState(QObject *parent = nullptr);

    QByteArray mainSplitView() const;
    void setMainSplitView(const QByteArray &state);
    Q_SIGNAL void mainSplitViewChanged();

private:
    KSharedConfigPtr m_stateConfig;
};
