/*
 *  SPDX-FileCopyrightText: 2013 Giorgos Tsiapaliokas <terietor@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef VIEW_H
#define VIEW_H

#include "privateheaders/containmentview.h"

class ViewerCorona;

class View : public PlasmaQuick::ContainmentView
{
    Q_OBJECT
    Q_PROPERTY(bool konsoleVisible READ konsoleVisible CONSTANT)

public:
    View(ViewerCorona *corona, bool konsoleVisible, QWindow *parent = nullptr);
    ~View() override;

    void addApplet(const QString &applet);
    void addContainment(const QString &containment);
    void addFormFactor(const QString &formFactor = QStringLiteral("planar"));
    void addLocation(const QString &location = QStringLiteral("floating"));
    void emitExternalData(const QString &data);
    bool konsoleVisible();

    Q_INVOKABLE void changeFormFactor(int formFactor);
    Q_INVOKABLE void changeLocation(int location);
    Q_INVOKABLE void takeScreenShot();

    static ViewerCorona *createCorona();

protected:
    void resizeEvent(QResizeEvent *event) override;
    QString pluginFromPath(const QString &path) const;

private:
    QString m_lastAppletName;
    bool m_konsoleVisible;
};

#endif // VIEW_H
