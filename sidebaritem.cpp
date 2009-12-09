
#include <QTableWidgetItem>

//#include "sidebaritem.h"
#ifndef SIDEBARITEM_H
#define SIDEBARITEM_H

static const int SidebarItemType = QTableWidgetItem::UserType + 1;

/* List item representing a sidebar entry. */
class SidebarItem : public QTableWidgetItem
{
public:
    SidebarItem(const QIcon &icon, const QString &text)
            : QTableWidgetItem(0, SidebarItemType) {
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        setIcon(icon);
        setText(text);
        setToolTip(text);
    }
};


#endif  // SIDEBARITEM_H

