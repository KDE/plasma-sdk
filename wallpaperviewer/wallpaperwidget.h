/*
 *   SPDX-FileCopyrightText: 2009 Aaron Seigo <aseigo@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef WALLPAPERWIDGET_H
#define WALLPAPERWIDGET_H

#include <QWidget>

#include <KConfigGroup>

class KDialog;

namespace Plasma
{
    class Wallpaper;
} // namespace Plasma

class WallpaperWidget : public QWidget
{
    Q_OBJECT

public:
    WallpaperWidget(const QString &paper, const QString &mode, QWidget *parent = 0);
    ~WallpaperWidget();

public Q_SLOTS:
    void configure();

protected:
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

protected Q_SLOTS:
    void quit();
    void updatePaper(const QRectF &exposedRect);
    void saveConfig();
    void configDone();
    void syncConfig();

private:
    KConfigGroup configGroup();

    Plasma::Wallpaper *m_wallpaper;
    KDialog *m_configDialog;
    QPoint m_mousePressPoint;
    QPoint m_mousePressScreenPoint;
    QPoint m_mouseMovePoint;
    QPoint m_mouseMoveScreenPoint;
};

// WallpaperConfigWidget is passed the wallpaper
// in createConfigurationInterface so it can notify
// of changes (used to enable the apply button)
class WallpaperConfigWidget :public QWidget
{
    Q_OBJECT
public:
      WallpaperConfigWidget(QWidget *parent)
          : QWidget(parent)
      {
      }

signals:
    void modified(bool isModified);

public slots:
    void settingsChanged(bool isModified)
    {
        emit modified(isModified);
    }
};

#endif

