#include <QTableWidgetItem>
#include <QString>

#include "timelineitem.h"

static const int TimeLineItemType = QTableWidgetItem::UserType + 1;

TimeLineItem::TimeLineItem(const QIcon &icon,
        QStringList &dataList,
        const TimeLineItem::ItemIdentifier id,
        const Qt::ItemFlag flag)
        : QTableWidgetItem(0,TimeLineItemType)
{
    setFlags(flag);
    setIcon(icon);
    setText(dataList.takeFirst());
    setToolTip(dataList.takeFirst());
    /*setData(Qt::DecorationRole,QVariant(icon));
    setData(Qt::DisplayRole,QVariant(dataList.takeFirst()));
    setData(Qt::ToolTipRole,QVariant(dataList.takeFirst()));*/
    m_type = id;
    m_sha1hash = dataList.takeFirst();
}

void TimeLineItem::setHash(const QString &hash)
{
    m_sha1hash = hash;
}

void TimeLineItem::setIdentifier(const TimeLineItem::ItemIdentifier id)
{
    m_type = id;
}

QString TimeLineItem::getHash()
{
    return m_sha1hash;
}

TimeLineItem::ItemIdentifier TimeLineItem::getIdentifier()
{
    return m_type;
}

/*QVariant TimeLineItem::data(int role)
{
    switch(role) {
        case Qt::DisplayRole:
            return QVariant(text());
        break;

        case Qt::DecorationRole:
            return QVariant(icon());
        break;

        case Qt::ToolTipRole:
            return QVariant(toolTip());
        break;

        default:
        return QVariant();
    }
}*/
