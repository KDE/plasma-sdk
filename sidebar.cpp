/***************************************************************************
 *   Copyright (C) 2007 by Pino Toscano <pino@kde.org>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/




#include <qabstractitemdelegate.h>
#include <qaction.h>
#include <qapplication.h>
#include <qevent.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qpainter.h>
#include <qscrollbar.h>
#include <qsplitter.h>
#include <qstackedwidget.h>

#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>

// #include "settings.h"

#include "sidebaritem.cpp"
#include "sidebardelegate.h"
#include "sidebarlistwidget.h"
#include "sidebarprivatestorage.h"
#include "sidebar.h"


Sidebar::Sidebar(QWidget *parent)
        : QWidget(parent), d(new SidebarPrivateStorage)
{
    QHBoxLayout *mainlay = new QHBoxLayout(this);
    mainlay->setMargin(0);
    mainlay->setSpacing(0);

    d->list = new SidebarListWidget(this);
    mainlay->addWidget(d->list);
    d->list->setMouseTracking(true);
    d->list->viewport()->setAttribute(Qt::WA_Hover);
    d->sideDelegate = new SidebarDelegate(d->list);
//     d->sideDelegate->setShowText( Okular::Settings::sidebarShowText() );
    d->list->setItemDelegate(d->sideDelegate);
    d->list->setUniformItemSizes(true);
    d->list->setSelectionMode(QAbstractItemView::SingleSelection);
    int iconsize = 32;// Okular::Settings::sidebarIconSize();
    d->list->setIconSize(QSize(iconsize, iconsize));
    d->list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->list->setContextMenuPolicy(Qt::CustomContextMenu);
    d->list->viewport()->setAutoFillBackground(false);

    d->splitter = new QSplitter(this);
    mainlay->addWidget(d->splitter);
    d->splitter->setOpaqueResize(true);
    d->splitter->setChildrenCollapsible(false);

    d->sideContainer = new QWidget(d->splitter);
    d->sideContainer->setMinimumWidth(90);
    d->sideContainer->setMaximumWidth(600);
    d->vlay = new QVBoxLayout(d->sideContainer);
    d->vlay->setMargin(0);

    d->sideTitle = new QLabel(d->sideContainer);
    d->vlay->addWidget(d->sideTitle);
    QFont tf = d->sideTitle->font();
    tf.setBold(true);
    d->sideTitle->setFont(tf);
    d->sideTitle->setMargin(3);
    d->sideTitle->setIndent(3);

    d->stack = new QStackedWidget(d->sideContainer);
    d->vlay->addWidget(d->stack);
    d->sideContainer->hide();

    connect(d->list, SIGNAL(clicked(const QModelIndex &)),
            this, SIGNAL(currentIndexClicked(const QModelIndex &)));

}

Sidebar::~Sidebar()
{
    delete d;
}

int Sidebar::addItem(const QIcon &icon, const QString &text)
{
    SidebarItem *newitem = new SidebarItem(icon, text);
    d->list->addItem(newitem);
    d->pages.append(newitem);
    // updating the minimum height of the icon view, so all are visible with no scrolling
    d->adjustListSize(false, true);
    return d->pages.count() - 1;
}

void Sidebar::setItemEnabled(int index, bool enabled)
{
    if (index < 0 || index >= d->pages.count())
        return;

    Qt::ItemFlags f = d->pages.at(index)->flags();
    if (enabled) {
        f |= Qt::ItemIsEnabled;
        f |= Qt::ItemIsSelectable;
    } else {
        f &= ~Qt::ItemIsEnabled;
        f &= ~Qt::ItemIsSelectable;
    }
    d->pages.at(index)->setFlags(f);

    if (!enabled && index == currentIndex())
        // find an enabled item, and select that one
        for (int i = 0; i < d->pages.count(); ++i)
            if (d->pages.at(i)->flags() & Qt::ItemIsEnabled) {
                setCurrentIndex(i);
                break;
            }
}

bool Sidebar::isItemEnabled(int index) const
{
    if (index < 0 || index >= d->pages.count())
        return false;

    Qt::ItemFlags f = d->pages.at(index)->flags();
    return (f & Qt::ItemIsEnabled) == Qt::ItemIsEnabled;
}

void Sidebar::setCurrentIndex(int index)
{
    if (index < 0 || index >= d->pages.count() || !isItemEnabled(index))
        return;

    QModelIndex modelindex = d->list->model()->index(index, 0);
    d->list->setCurrentIndex(modelindex);
    d->list->selectionModel()->select(modelindex, QItemSelectionModel::ClearAndSelect);
}

int Sidebar::currentIndex() const
{
    return d->list->currentRow();
}

void Sidebar::setSidebarVisibility(bool visible)
{
    if (visible != d->list->isHidden())
        return;

    static bool sideWasVisible = !d->sideContainer->isHidden();

    d->list->setHidden(!visible);
    if (visible) {
        d->sideContainer->setHidden(!sideWasVisible);
        sideWasVisible = true;
    } else {
        sideWasVisible = !d->sideContainer->isHidden();
        d->sideContainer->setHidden(true);
    }
}

bool Sidebar::isSidebarVisible() const
{
    return !d->sideContainer->isHidden();
}

#include "sidebar.moc"
