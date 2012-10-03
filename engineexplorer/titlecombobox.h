/*
 *   Copyright 2008 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2,
 *   or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef TITLECOMBOBOX_H
#define TITLECOMBOBOX_H

#include <QComboBox>
#include <QPainter>

#include <KDebug>
#include <KLocale>

class TitleComboBox : public QComboBox
{
public:
    TitleComboBox(QWidget *parent = 0)
        : QComboBox(parent)
    {
    }

protected:
    void paintEvent(QPaintEvent *event)
    {
        QComboBox::paintEvent(event);

        if (currentIndex() != -1) {
            return;
        }

        QPainter p(this);
        /*QFont bold = p.font();
        bold.setBold(true);
        p.setFont(bold);*/
        p.setPen(palette().color(QPalette::Disabled, QPalette::WindowText));
        int frameWidth = style()->pixelMetric(QStyle::PM_ComboBoxFrameWidth);
        QRect r = rect().adjusted(frameWidth, frameWidth, frameWidth, frameWidth);
        p.drawText(QStyle::visualRect(layoutDirection(), rect(), r), i18n("Data Engines"));
    }
};

#endif

