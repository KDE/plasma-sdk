/**
  *	Class for Item that will be showed in the Timeline.
  */


#include	<QListWidgetItem>
#include	<klocalizedstring.h>

#include	"timelineitem.h"

static const int TimeLineItemType = QListWidgetItem::UserType + 1;

/* List item representing a timeline entry. */
TimeLineItem::TimeLineItem( const QIcon &icon,
					  QStringList &dataList,
					  const TimeLineItem::ItemIdentifier id,
					  const Qt::ItemFlag flag)
					  : QListWidgetItem( 0, TimeLineItemType )

{
	//	Note: we expect dataList is as follows:
	//	dataList(0) = text string that will be showed
	//	dataList(1) = commit string used for tooltip
	//	dataList(2) = sha1 hash

	setFlags( flag );
	setIcon( icon );
	setText( dataList.takeFirst() );
	QListWidgetItem::setToolTip( dataList.takeFirst() );
	m_type = id;
	m_sha1sum = dataList.takeFirst();
}

void TimeLineItem::setHash( const QString &hash )
{
	m_sha1sum = hash;
}

void TimeLineItem::setIdentifier( const TimeLineItem::ItemIdentifier id  )
{
	m_type = id;
}

QString TimeLineItem::getHash( )
{
	return m_sha1sum;
}

TimeLineItem::	ItemIdentifier TimeLineItem::getIdentifier()
{
	return m_type;
}

