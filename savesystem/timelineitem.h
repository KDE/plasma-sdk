#ifndef TIMELINEITEM_H
#define TIMELINEITEM_H

#include	<QListWidgetItem>

class TimeLineItem : public QListWidgetItem
{
	public:

		enum ItemIdentifier
		{
			OutsideWorkingDir = 0,
			NotACommit = 1,
			Commit = 2,
			Branch = 3,
			Merge = 4
		};

		/* List item representing a timeline entry. */
		TimeLineItem( const QIcon &icon,
					  QStringList &dataList,
					  const TimeLineItem::ItemIdentifier id,
					  const Qt::ItemFlag flag);


		void setHash( const QString &hash );

		void setIdentifier( const TimeLineItem::ItemIdentifier id );

		QString& getHash();

		TimeLineItem::ItemIdentifier getIdentifier();

	private:
		QString *m_sha1sum;
		ItemIdentifier m_type;
};


#endif // TIMELINEITEM_H
