/*

Copyright 2009-2010 Diego '[Po]lentino' Casella <polentino911@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <QApplication>
#include <QModelIndex>
#include <QPainter>
#include <QStyleOptionViewItem>

#include "tabledelegate.h"

static const int ITEM_MARGIN_LEFT = 5;
static const int ITEM_MARGIN_TOP = 5;
static const int ITEM_MARGIN_RIGHT = 5;
static const int ITEM_MARGIN_BOTTOM = 5;
static const int ITEM_PADDING = 5;


TableDelegate::TableDelegate()
{
}

void TableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QBrush backBrush;
    QColor foreColor;
    bool disabled = false;
    bool hover = false;
    if (!(option.state & QStyle::State_Enabled)) {
        backBrush = option.palette.brush(QPalette::Disabled, QPalette::Base);
        foreColor = option.palette.color(QPalette::Disabled, QPalette::Text);
        disabled = true;

        // Since the TimeLine doesn't allow selectable items, because you don't actually "are" on a given savepoint,
        // we don't actually need this check.
    /*} else if (option.state & (QStyle::State_HasFocus | QStyle::State_Selected)) {
        backBrush = option.palette.brush(QPalette::Highlight);
        foreColor = option.palette.color(QPalette::HighlightedText);*/
    } else if (option.state & QStyle::State_MouseOver) {
        backBrush = option.palette.color(QPalette::Highlight).darker();
        foreColor = option.palette.color(QPalette::Text);
        hover = true;
    } else { /*if ( option.state & QStyle::State_Enabled )*/
        backBrush = option.palette.brush(QPalette::Base);
        foreColor = option.palette.color(QPalette::Text);
    }
    QStyle *style = QApplication::style();
    QStyleOptionViewItemV4 opt(option);
    // KStyle provides an "hover highlight" effect for free;
    // but we want that for non-KStyle-based styles too
    if (!style->inherits("KStyle") && hover) {
        Qt::BrushStyle bs = opt.backgroundBrush.style();
        if (bs > Qt::NoBrush && bs < Qt::TexturePattern)
            opt.backgroundBrush = opt.backgroundBrush.color().light(115);
        else
            opt.backgroundBrush = backBrush;
    }
    painter->save();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, 0);
    painter->restore();
    QIcon icon = index.data(Qt::DecorationRole).value< QIcon >();
    if (!icon.isNull()) {
        QPoint iconpos(
            (option.rect.width() - option.decorationSize.width()) / 2,
            ITEM_MARGIN_TOP
        );
        iconpos += option.rect.topLeft();
        QIcon::Mode iconmode = disabled ? QIcon::Disabled : QIcon::Normal;
        painter->drawPixmap(iconpos, icon.pixmap(option.decorationSize, iconmode));
    }

    QString text = index.data(Qt::DisplayRole).toString();
    QRect fontBoundaries = QFontMetrics(option.font).boundingRect(text);
    fontBoundaries.setWidth(fontBoundaries.width() + ITEM_PADDING);
    QPoint textPos(
        ITEM_MARGIN_LEFT + (option.rect.width() - ITEM_MARGIN_LEFT - ITEM_MARGIN_RIGHT - fontBoundaries.width()) / 2,
        ITEM_MARGIN_TOP + option.decorationSize.height() + ITEM_PADDING
    );
    fontBoundaries.translate(-fontBoundaries.topLeft());
    fontBoundaries.translate(textPos);
    fontBoundaries.translate(option.rect.topLeft());
    painter->setPen(foreColor);
    painter->drawText(fontBoundaries, Qt::AlignCenter, text);
}

QSize TableDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize baseSize(option.decorationSize.width(), option.decorationSize.height());
    QRect fontBoundaries = QFontMetrics(option.font).boundingRect(index.data(Qt::DisplayRole).toString());
    baseSize.setWidth(qMax(fontBoundaries.width(), baseSize.width()));
    baseSize.setHeight(baseSize.height() + fontBoundaries.height() + 2*ITEM_PADDING);

    return baseSize += QSize(ITEM_MARGIN_LEFT + ITEM_MARGIN_RIGHT,
                            ITEM_MARGIN_TOP + ITEM_MARGIN_BOTTOM);

}
