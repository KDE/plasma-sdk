/***************************************************************************
 *   Copyright (C) 2007 by Pino Toscano <pino@kde.org>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/




#include <qabstractitemdelegate.h>
#include <qaction.h>
#include <qapplication.h>
#include <qevent.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qpainter.h>
#include <qscrollbar.h>
#include <qsplitter.h>
#include <qstackedwidget.h>

#include <kiconloader.h>
#include <klocale.h>
#include <KUrl>
#include <KIcon>
#include	<QListWidget>
#include	<QRegExp>
#include	<QMessageBox>


#include	"timelineitem.h"
#include	"timelinedelegate.h"
#include	"timelinelistwidget.h"
#include	"timelineprivatestorage.h"
#include	"timeline.h"



//using namespace KDevelop;

TimeLine::TimeLine( QWidget *parent, const KUrl &dir )
	: QWidget( parent ), d( new TimeLinePrivateStorage )
{
	m_gitRunner = new GitRunner();
	setWorkingDir( dir );
	initUI( parent );

}

TimeLine::~TimeLine()
{
	if( m_workingDir )
		delete m_workingDir;
	delete d;
}

int TimeLine::uiAddItem( const QIcon &icon,
						 QStringList &data,
						 const TimeLineItem::ItemIdentifier id,
						 const Qt::ItemFlag flag )
{
	TimeLineItem *newitem = new TimeLineItem( icon, data, id, flag );
	d->list->addItem( newitem );
	d->pages.append( newitem );
	// updating the minimum height of the icon view, so all are visible with no scrolling
	d->adjustListSize( false, true );
	return d->pages.count() - 1;
}

void TimeLine::loadTimeLine( KUrl &dir )
{
	QStringList list = QStringList();

	// Ensure we are on a valid Git directory
	bool res = m_gitRunner->isValidDirectory( dir );
	if( !res )
	{
		// If the dir hasn't a git tree we have to create only one item,
		// used to notify the user that a timeline should be created
		// ( that is, call GitRunner::init() ).
		list.append( i18n( "Not a SavePoint" ) );
		list.append( i18n( "Click here to save your first SavePoint!\nHint: try to always add a small comment ;)" ) );
		list.append( QString() );				// We don't have a sha1sum now, so set an empty string

		this->uiAddItem( KIcon( "document-save" ),
						 list,
						 TimeLineItem::NotACommit,
						 Qt::ItemIsEnabled );

		return;
	}

	// Now we can mark dir as current directory
	m_gitRunner->setDirectory( dir );

	// The first element of the timeline is used to prompt the user to
	// add a SavePoint
	list.append( i18n( "Not a SavePoint" ) );
	list.append( i18n( "Click here to save your first SavePoint!\nHint: try to always add a small comment ;)" ) );
	list.append( QString() );				// We don't have a sha1sum now, so set an empty string
	this->uiAddItem( KIcon( "document-save" ),
					list,
					TimeLineItem::NotACommit,
					Qt::ItemIsEnabled );


	if( m_gitRunner->currentBranch() != DvcsJob::JobSucceeded )
	{
		// handle error
		return;
	}
	m_currentBranch = new QString( m_gitRunner->getResult() );

	if( m_gitRunner->branches() != DvcsJob::JobSucceeded )
	{
		// handle error
		return;
	}
	QString branches = m_gitRunner->getResult();

	if( m_gitRunner->log( dir ) != DvcsJob::JobSucceeded )
	{
		// handle error
		return;
	}

	QString rawCommits = m_gitRunner->getResult();
	QRegExp rx( "(commit )" );
	// For now, we assume to find only commits
	QStringList commits = rawCommits.split( rx, QString::SkipEmptyParts );
	int index = commits.size();

	// Iterate every commit and create an element in the sidebar.
	for( int i = 0; i < index; i++ )
	{
		// Save commit(index) and split it
		QStringList tmp = commits.takeFirst().split( "\n" );
		QString sha1sum = tmp.takeFirst();
		QString author = tmp.takeFirst().remove( 0, 8 );
		QString date = tmp.takeFirst().remove( 0, 6 );

		QString commitInfo = ( i18n( "SavePoint created on " ) + date );
		commitInfo.append( i18n( "\nBy " ) + author + "\n" );
		commitInfo.append( i18n( "Comment:\n" ) );
		int tmpSize = tmp.size();
		for( int c = 0; c < tmpSize-1; c++ )
		{
			commitInfo.append( tmp.takeFirst().append( "\n" ) );
			//commitInfo.append( "\n" );
		}
		QString text = QString( "SavePoint #" );
		QString savePointNumber = QString();
		savePointNumber.setNum( i );
		text.append( savePointNumber );

		QStringList list = QStringList( text );
		list.append( commitInfo );
		list.append( sha1sum );
		this->uiAddItem( KIcon( "dialog-ok" ),
						 list,
						 TimeLineItem::Commit,
						 Qt::ItemIsEnabled );

	}

	// As last element, show the current branch
	QString info = QString();
	list.clear();
	info.append( i18n( "On branch: " ) );
	info.append( m_currentBranch );
	list.append( info );

	info.clear();
	info.append( i18n( "You are currently working on branch:\n" ) );
	info.append( m_currentBranch );
	info.append( i18n( "\n" ) );
	info.append( i18n( "\nOther available branches are:\n" ) );
	info.append( branches );
	info.append( i18n( "\nClick here to switch to those branches." ) );

	list.append( info );
	list.append( i18n( "" ) );

	this->uiAddItem( KIcon( "system-switch-user" ),
						 list,
						 TimeLineItem::Branch,
						 Qt::ItemIsEnabled );

}

void TimeLine::setItemEnabled( int index, bool enabled )
{
	if ( index < 0 || index >= d->pages.count() )
		return;

	Qt::ItemFlags f = d->pages.at( index )->flags();
	if ( enabled )
	{
		f |= Qt::ItemIsEnabled;
		f |= Qt::ItemIsSelectable;
	}
	else
	{
		f &= ~Qt::ItemIsEnabled;
		f &= ~Qt::ItemIsSelectable;
	}
	d->pages.at( index )->setFlags( f );

	if ( !enabled && index == currentIndex() )
		// find an enabled item, and select that one
		for ( int i = 0; i < d->pages.count(); ++i )
			if ( d->pages.at(i)->flags() & Qt::ItemIsEnabled )
			{
				setCurrentIndex( i );
				break;
			}
}

bool TimeLine::isItemEnabled( int index ) const
{
	if ( index < 0 || index >= d->pages.count() )
		return false;

	Qt::ItemFlags f = d->pages.at( index )->flags();
	return ( f & Qt::ItemIsEnabled ) == Qt::ItemIsEnabled;
}

void TimeLine::setCurrentIndex( int index )
{
	if ( index < 0 || index >= d->pages.count() || !isItemEnabled( index ) )
		return;

	QModelIndex modelindex = d->list->model()->index( index, 0 );
	d->list->setCurrentIndex( modelindex );
	d->list->selectionModel()->select( modelindex, QItemSelectionModel::ClearAndSelect );
}

int TimeLine::currentIndex() const
{
	return d->list->currentRow();
}

void TimeLine::setTimeLineVisibility( bool visible )
{
	if ( visible != d->list->isHidden() )
		return;

	static bool sideWasVisible = !d->sideContainer->isHidden();

	d->list->setHidden( !visible );
	if ( visible )
	{
		d->sideContainer->setHidden( !sideWasVisible );
		sideWasVisible = true;
	}
	else
	{
		sideWasVisible = !d->sideContainer->isHidden();
		d->sideContainer->setHidden( true );
	}
}

bool TimeLine::isTimeLineVisible() const
{
	return !d->sideContainer->isHidden();
}

void TimeLine::setWorkingDir( const KUrl &dir )
{
	m_workingDir = new KUrl( dir );
}

void TimeLine::initUI( QWidget *parent )
{
	QVBoxLayout *mainlay = new QVBoxLayout( this );
	mainlay->setMargin( 0 );
	mainlay->setSpacing( 0 );

	d->list = new TimeLineListWidget( this );
	mainlay->addWidget( d->list );
	d->list->setMouseTracking( true );
	d->list->viewport()->setAttribute( Qt::WA_Hover );
	d->sideDelegate = new TimeLineDelegate( d->list );
	d->list->setItemDelegate( d->sideDelegate );
	d->list->setUniformItemSizes( true );
	d->list->setSelectionMode( QAbstractItemView::SingleSelection );
	int iconsize = 32;// Okular::Settings::timelineIconSize();
	d->list->setIconSize( QSize( iconsize, iconsize ) );
	d->list->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	d->list->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	d->list->setContextMenuPolicy( Qt::CustomContextMenu );
	d->list->viewport()->setAutoFillBackground( false );

	d->splitter = new QSplitter( this );
	mainlay->addWidget( d->splitter );
	d->splitter->setOpaqueResize( true );
	d->splitter->setChildrenCollapsible( false );

	d->sideContainer = new QWidget( d->splitter );
	d->sideContainer->setMinimumWidth( 90 );
	d->sideContainer->setMaximumWidth( 600 );
	d->vlay = new QHBoxLayout( d->sideContainer );
	d->vlay->setMargin( 0 );

	d->sideTitle = new QLabel( d->sideContainer );
	d->vlay->addWidget( d->sideTitle );
	QFont tf = d->sideTitle->font();
	tf.setBold( true );
	d->sideTitle->setFont( tf );
	d->sideTitle->setMargin( 3 );
	d->sideTitle->setIndent( 3 );

	d->stack = new QStackedWidget( d->sideContainer );
	d->vlay->addWidget( d->stack );
	d->sideContainer->hide();

	connect( d->list, SIGNAL( currentRowChanged( int ) ),
			 this, SIGNAL( currentIndexChanged( int ) ) );
}

#include "timeline.moc"
