#ifndef TIMELINEPRIVATESTORAGE_H
#define TIMELINEPRIVATESTORAGE_H

class   TimeLineListwidget;
class   TimeLineItem;
class   TimeLineDelegate;
class   TimeLineListWidget;
class   TimeLinePrivateStorage;

class QSplitter;
class QStackedWidget;
class QStackedWidget;

class TimeLinePrivateStorage
{
public:
    TimeLinePrivateStorage();

    void adjustListSize(bool recalc, bool expand = true);

    TimeLineListWidget *list;
    QSplitter *splitter;
    QStackedWidget *stack;
    QWidget *sideContainer;
    QLabel *sideTitle;
    QHBoxLayout *vlay;
    QWidget *sideWidget;
    QWidget *bottomWidget;
    QList< TimeLineItem* > pages;
    bool splitterSizesSet;
    int itemsHeight;
    TimeLineDelegate *sideDelegate;
};

#endif // TIMELINEPRIVATESTORAGE_H
