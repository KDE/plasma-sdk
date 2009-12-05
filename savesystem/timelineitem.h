#ifndef TIMELINEITEM_H
#define TIMELINEITEM_H

#include <QTableWidgetItem>

class TimeLineItem : public QTableWidgetItem
{
public:
    enum ItemIdentifier {
        OutsideWorkingDir = 0, /**< The TimeLineItem represents an object that it isn't inside the working directory. */
        NotACommit = 1, /**< The TimeLineItem represents an object that is not yet a commit. */
        Commit = 2, /**< The TimeLineItem representd Commit type. */
        Branch = 3, /**< The TimeLineItem representd Branch type. */
        Merge = 4 /**< The TimeLineItem representd Merge type. */
        };

    TimeLineItem(const QIcon &icon,
              QStringList &dataList,
              const TimeLineItem::ItemIdentifier id,
              const Qt::ItemFlag flag);

    void setHash(const QString &hash);
    void setIdentifier(const TimeLineItem::ItemIdentifier id);

    QString getHash();
    TimeLineItem::ItemIdentifier getIdentifier();

private:
    QString m_sha1hash;
    ItemIdentifier m_type;
};

#endif // TIMELINEITEM_H
