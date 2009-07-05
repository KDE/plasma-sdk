#ifndef TIMELINEDELEGATE_H
#define TIMELINEDELEGATE_H

class TimeLineDelegate : public QAbstractItemDelegate
{
	public:
		TimeLineDelegate( QObject *parent = 0 );
		~TimeLineDelegate();

		void setShowText( bool show );
		bool isTextShown() const;

		// from QAbstractItemDelegate
		void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
		QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	private:
		bool m_showText;
};

#endif // TIMELINEDELEGATE_H
