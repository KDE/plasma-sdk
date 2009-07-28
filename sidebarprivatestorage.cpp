#include    <QSplitter>
#include    <QStackedWidget>
#include    <QWidget>
#include    <QLabel>
#include    <QVBoxLayout>
#include    <QList>


#include    "sidebaritem.cpp"
#include    "sidebardelegate.h"
#include    "sidebarlistwidget.h"
#include    "sidebarprivatestorage.h"


SidebarPrivateStorage::SidebarPrivateStorage()
        : sideWidget(0), bottomWidget(0), splitterSizesSet(false),
        itemsHeight(0)
{
}

void SidebarPrivateStorage::adjustListSize(bool recalc, bool expand)
{
    QRect bottomElemRect(
        QPoint(0, 0),
        list->sizeHintForIndex(list->model()->index(list->count() - 1, 0))
    );
    if (recalc) {
        int w = 0;
        for (int i = 0; i < list->count(); ++i) {
            QSize s = list->sizeHintForIndex(list->model()->index(i, 0));
            if (s.width() > w)
                w = s.width();
        }
        bottomElemRect.setWidth(w);
    }
    bottomElemRect.translate(0, bottomElemRect.height() *(list->count() - 1));
    itemsHeight = bottomElemRect.height() * list->count();
    list->setMinimumHeight(itemsHeight + list->frameWidth() * 2);
    int curWidth = list->minimumWidth();
    int newWidth = expand
                   ? qMax(bottomElemRect.width() + list->frameWidth() * 2, curWidth)
                   : qMin(bottomElemRect.width() + list->frameWidth() * 2, curWidth);
    list->setFixedWidth(newWidth);
}
