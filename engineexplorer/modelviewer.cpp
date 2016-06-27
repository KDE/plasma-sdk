/*
 *   Copyright 2008 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2013 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2,
 *   or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "modelviewer.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <KMessageBox>
#include <KStringHandler>
#include <KLocalizedString>

#include <Plasma/DataEngine>
#include <Plasma/Service>
#include <Plasma/ServiceJob>

#include "engineexplorer.h"

Delegate::Delegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{

}

Delegate::~Delegate()
{
}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const
{
    if (!index.model()) {
        return;
    }

    QFontMetrics fm(option.font);
    int maxWidth = 0;
    foreach (int role, index.model()->roleNames().keys()) {
        maxWidth = qMax(maxWidth, fm.width(index.model()->roleNames().value(role) + ": "));
    }

    int i = 2;
    foreach (int role, index.model()->roleNames().keys()) {
        painter->drawText(option.rect.x() + maxWidth - fm.width(index.model()->roleNames().value(role) + ": "), option.rect.y() + i*fm.height(), index.model()->roleNames().value(role) + ": ");

        if (index.data(role).canConvert<QIcon>()) {
            index.data(role).value<QIcon>().paint(painter, option.rect.x() + maxWidth, option.rect.y() + (i-1)*fm.height(), 16, 16);
        } else if (!index.data(role).isValid()) {
            painter->drawText(option.rect.x() + maxWidth, option.rect.y() + i*fm.height(), "null");
        } else {
            painter->drawText(option.rect.x() + maxWidth, option.rect.y() + i*fm.height(), index.data(role).toString());
        }
        ++i;
    }
}


QSize Delegate::sizeHint(const QStyleOptionViewItem &option,
               const QModelIndex &index) const
{
    if (!index.model()) {
        return QSize();
    }

    QFontMetrics fm(option.font);
    return QSize(fm.width("M") * 50, fm.height() * (index.model()->roleNames().count()+2));
}


ModelViewer::ModelViewer(Plasma::DataEngine *engine, const QString &source, QWidget *parent)
    : QDialog(parent),
      m_engine(engine),
      m_source(source)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_view = new QTreeView(this);
    m_view->setItemDelegate(new Delegate(m_view));
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_view);
    setLayout(layout);

    QString engineName = i18nc("Plasma engine with unknown name", "Unknown");

    if (m_engine) {
        if (m_engine->pluginInfo().isValid()) {
            engineName = KStringHandler::capwords(m_engine->pluginInfo().name());
        }
        qDebug() << "########### CALLING SERVICE FOR SOURCE: " << m_source;
        m_model = m_engine->modelForSource(m_source);

        if (m_model != 0) {
            connect(m_engine, SIGNAL(destroyed(QObject*)), this, SLOT(engineDestroyed()));
            m_view->setModel(m_model);
        } else {
            KMessageBox::sorry(this, i18n("No valid model was returned. Verify that a model is available for this source."));
            close();
        }
    }

    setWindowTitle(i18nc("%1 is a Plasma dataengine name", "%1 Model Explorer", engineName));
}

ModelViewer::~ModelViewer()
{
    m_engine = 0;
}


void ModelViewer::engineDestroyed()
{
    m_model = 0;
    m_engine = 0;
    hide();
    deleteLater();
}


