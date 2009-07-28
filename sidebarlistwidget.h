#ifndef SIDEBARLISTWIDGET_H
#define SIDEBARLISTWIDGET_H

class SidebarListWidget : public QListWidget
{
public:
    SidebarListWidget(QWidget *parent = 0);
    ~SidebarListWidget();

protected:
    // from QListWidget
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
};

#endif // SIDEBARLISTWIDGET_H
