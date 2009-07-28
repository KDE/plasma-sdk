#ifndef SIDEBARPRIVATESTORAGE_H
#define SIDEBARPRIVATESTORAGE_H

class   SidebarListwidget;
class   SidebarItem;
class   SidebarDelegate;
class   SidebarListWidget;
class   SidebarPrivateStorage;


class QSplitter;
class QStackedWidget;
class QStackedWidget;

class SidebarPrivateStorage
{
public:
    SidebarPrivateStorage();

    void adjustListSize(bool recalc, bool expand = true);

    SidebarListWidget *list;
    QSplitter *splitter;
    QStackedWidget *stack;
    QWidget *sideContainer;
    QLabel *sideTitle;
    QVBoxLayout *vlay;
    QWidget *sideWidget;
    QWidget *bottomWidget;
    QList< SidebarItem* > pages;
    bool splitterSizesSet;
    int itemsHeight;
    SidebarDelegate *sideDelegate;

};

#endif // SIDEBARPRIVATESTORAGE_H
