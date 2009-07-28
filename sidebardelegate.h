#ifndef SIDEBARDELEGATE_H
#define SIDEBARDELEGATE_H

class SidebarDelegate : public QAbstractItemDelegate
{
public:
    SidebarDelegate(QObject *parent = 0);
    ~SidebarDelegate();

    void setShowText(bool show);
    bool isTextShown() const;

    // from QAbstractItemDelegate
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    bool m_showText;
};

#endif // SIDEBARDELEGATE_H
