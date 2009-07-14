#ifndef TIMELINELISTWIDGET_H
#define TIMELINELISTWIDGET_H

class TimeLineListWidget : public QListWidget
{
	public:
		TimeLineListWidget( QWidget *parent = 0 );
		~TimeLineListWidget();

	protected:
		// from QWidget
		void mouseDoubleClickEvent( QMouseEvent *event );
		void mouseMoveEvent( QMouseEvent *event );
		void mousePressEvent( QMouseEvent *event );
		void mouseReleaseEvent( QMouseEvent *event );

		QModelIndex moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers );
};

#endif // TIMELINELISTWIDGET_H
