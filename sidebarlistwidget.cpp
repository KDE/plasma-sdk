
#include    <QWidget>
#include    <QMouseEvent>
#include    <QModelIndex>
#include    <QAbstractItemView>
#include    <QListWidget>

#include "sidebarlistwidget.h"

SidebarListWidget::SidebarListWidget(QWidget *parent)
        : QListWidget(parent)
{
}

SidebarListWidget::~SidebarListWidget()
{
}

void SidebarListWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && !(index.flags() & Qt::ItemIsSelectable))
        return;

    QListWidget::mouseDoubleClickEvent(event);
}

void SidebarListWidget::mouseMoveEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && !(index.flags() & Qt::ItemIsSelectable))
        return;

    QListWidget::mouseMoveEvent(event);
}

void SidebarListWidget::mousePressEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && !(index.flags() & Qt::ItemIsSelectable))
        return;

    QListWidget::mousePressEvent(event);
}

void SidebarListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && !(index.flags() & Qt::ItemIsSelectable))
        return;

    QListWidget::mouseReleaseEvent(event);
}

QModelIndex SidebarListWidget::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)
    QModelIndex oldindex = currentIndex();
    QModelIndex newindex = oldindex;
    switch (cursorAction) {
    case MoveUp:
    case MovePrevious: {
        int row = oldindex.row() - 1;
        while (row > -1 && !(model()->index(row, 0).flags() & Qt::ItemIsSelectable)) --row;
        if (row > -1)
            newindex = model()->index(row, 0);
        break;
    }
    case MoveDown:
    case MoveNext: {
        int row = oldindex.row() + 1;
        int max = model()->rowCount();
        while (row < max && !(model()->index(row, 0).flags() & Qt::ItemIsSelectable)) ++row;
        if (row < max)
            newindex = model()->index(row, 0);
        break;
    }
    case MoveHome:
    case MovePageUp: {
        int row = 0;
        while (row < oldindex.row() && !(model()->index(row, 0).flags() & Qt::ItemIsSelectable)) ++row;
        if (row < oldindex.row())
            newindex = model()->index(row, 0);
        break;
    }
    case MoveEnd:
    case MovePageDown: {
        int row = model()->rowCount() - 1;
        while (row > oldindex.row() && !(model()->index(row, 0).flags() & Qt::ItemIsSelectable)) --row;
        if (row > oldindex.row())
            newindex = model()->index(row, 0);
        break;
    }
    // no navigation possible for these
    case MoveLeft:
    case MoveRight:
        break;
    }

    // dirty hack to change item when the key cursor changes item
    if (oldindex != newindex) {
        emit itemClicked(itemFromIndex(newindex));
    }
    return newindex;
}
