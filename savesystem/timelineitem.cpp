/**
  *	Class for Item that will be showed in the Timeline.
  */


#include	<QListWidgetItem>
#include	<QDateTime>

static const int TimeLineItemType = QListWidgetItem::UserType + 1;

class TimeLineItem : public QListWidgetItem
{
	public:
		/* List item representing a timeline entry. */
		TimeLineItem( const QIcon &icon, const QString &text, const QString &commit )
					: QListWidgetItem( 0, QListWidgetItem::UserType )
		{
					setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
					setIcon( icon );
					setText( text );
					setToolTip( commit );
					m_commitInfo = new QString( commit );
		}

	private:
		QString *m_commitInfo;
		QDateTime *m_commitDate;
};
