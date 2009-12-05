#ifndef TABLEDELEGATE_H
#define TABLEDELEGATE_H

#include <QAbstractItemDelegate>

class TableDelegate : public QAbstractItemDelegate
{
public:
    TableDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // TABLEDELEGATE_H
