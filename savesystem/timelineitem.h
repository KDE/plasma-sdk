#ifndef TIMELINEITEM_H
#define TIMELINEITEM_H


class TimeLineItem : public QListWidgetItem
{
	public:
		TimeLineItem( const QIcon &icon, const QString &text, const QString &commit );

	private:
		QString *m_commit;
};


#endif // TIMELINEITEM_H
