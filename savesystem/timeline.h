/***************************************************************************
 *   Copyright (C) 2007 by Pino Toscano <pino@kde.org>                     *
 *   Modified by Diego [Po]lentino Casella <polentino911@gmail.coim>       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include	<QWidget>

#include	"timelineprivatestorage.h"
#include	"timelineitem.h"
#include	"dvcsjob.h"
#include	"gitrunner.h"


class QIcon;
class QListWidgetItem;


class TimeLine : public QWidget
{
	Q_OBJECT
	public:
		TimeLine( QWidget *parent, const KUrl &dir );
		~TimeLine();



		// void setMainWidget( QWidget *widget );
		// void setBottomWidget( QWidget *widget );

		void setItemEnabled( int index, bool enabled );
		bool isItemEnabled( int index ) const;

		void setCurrentIndex( int index );
		int currentIndex() const;

		void setTimeLineVisibility( bool visible );
		bool isTimeLineVisible() const;

		void setWorkingDir( const KUrl& dir );
		void loadTimeLine( const KUrl& dir );


	public slots:
		void customContextMenuPainter( QListWidgetItem* item );
	signals:
		void currentIndexChanged(int);

	private slots:
		void restoreCommit();
		void moveToCommit();

		void switchBranch();
		void createBranch();
		void renameBranch();
		void mergeBranch();
		void deleteBranch();
	// void itemClicked( QListWidgetItem *item );
	// void splitterMoved( int pos, int index );
	// void showTextToggled( bool );
	// void iconSizeChanged( QAction *action );

	private:
		// adds an item in the list
		int uiAddItem( const QIcon& icon,
					   QStringList& data,
					   const TimeLineItem::ItemIdentifier id,
					   const Qt::ItemFlag flag );

		void saveSplitterSize() const;
		void initUI( QWidget *parent );

		KUrl		m_workingDir;
		GitRunner	*m_gitRunner;
		QString		m_currentBranch;
		QStringList	m_branches;
		TimeLinePrivateStorage *d;


};

#endif
