#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>

class QMouseEvent;
class TimeLineItem;

class TableWidget : public QTableWidget
{
    Q_OBJECT

public:
    TableWidget(Qt::DockWidgetArea location, QWidget *parent = 0);
    void addItem(TimeLineItem *item);
    void clear();
    QSize scrollbarSize(bool vertical);
    Qt::DockWidgetArea location();
    int totalLenght();

public Q_SLOTS:
    void updateLayout(Qt::DockWidgetArea location);

private:
    void updateSize(TimeLineItem *item);
    void mouseReleaseEvent(QMouseEvent *event);

    QScrollBar *m_horizontalSB;
    QScrollBar *m_verticalSB;
    Qt::DockWidgetArea m_location;

    // Max Cell Dimension: according with its position, could be a width or height
    int m_maxCellDimension;

    // Total Length: stores the width/height of the entire row/column
    int m_totalLenght;

};

#endif // TABLEWIDGET_H
