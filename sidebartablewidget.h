#ifndef SIDEBARTABLEWIDGET_H
#define SIDEBARTABLEWIDGET_H

#include <QTableWidget>
class SidebarItem;

class SidebarTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    SidebarTableWidget(Qt::DockWidgetArea location, QWidget *parent = 0);
    ~SidebarTableWidget();

    void addItem(SidebarItem *item);
    void clear();
    QSize scrollbarSize(bool vertical);
    Qt::DockWidgetArea location();
    int totalLenght();

public Q_SLOTS:
    void updateLayout(Qt::DockWidgetArea location);

protected:
    private:
    void updateSize(SidebarItem *item);
    void mouseReleaseEvent(QMouseEvent *event);

    QScrollBar *m_horizontalSB;
    QScrollBar *m_verticalSB;
    Qt::DockWidgetArea m_location;

    // Max Cell Dimension: according with its position, could be a width or height
    int m_maxCellDimension;

    // Total Length: stores the width/height of the entire row/column
    int m_totalLenght;

    // from QTableWidget
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    //void mouseReleaseEvent(QMouseEvent *event);

    QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
};

#endif // SIDEBARTABLEWIDGET_H
