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
#include	"gitrunner.h"


class QIcon;
class QListWidgetItem;


class TimeLine : public QWidget
{
	Q_OBJECT
	public:
		TimeLine( QWidget *parent, KUrl *dir );
		~TimeLine();



		// void setMainWidget( QWidget *widget );
		// void setBottomWidget( QWidget *widget );

		void setItemEnabled( int index, bool enabled );
		bool isItemEnabled( int index ) const;

		void setCurrentIndex( int index );
		int currentIndex() const;

		void setTimeLineVisibility( bool visible );
		bool isTimeLineVisible() const;

		void setWorkingDir( KUrl &dir );
		void parseTimeLine();

		// adds an item in the list
		int uiAddItem( const QIcon &icon, const QString &text );

		// Init Repo tree
		bool initDirectoryTree( const KUrl *dir );


	signals:
		void currentIndexChanged(int);

	// private slots:
	// void itemClicked( QListWidgetItem *item );
	// void splitterMoved( int pos, int index );
	// void showTextToggled( bool );
	// void iconSizeChanged( QAction *action );

	private:
		// Maybe it can be removed
		KUrl		*m_workingDir;

		GitRunner	*m_gitRunner;

		void saveSplitterSize() const;

		TimeLinePrivateStorage *d;

		void initUI( QWidget *parent );
};

#endif
