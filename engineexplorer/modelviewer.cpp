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

#include <Plasma/DataEngine>
#include <Plasma/Service>
#include <Plasma/ServiceJob>

#include "engineexplorer.h"

ModelViewer::ModelViewer(Plasma::DataEngine *engine, const QString &source, QWidget *parent)
    : QDialog(parent),
      m_engine(engine),
      m_source(source)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_view = new QTreeView(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_view);
    setLayout(layout);

    QString engineName = i18nc("Plasma engine with unknown name", "Unknown");

    if (m_engine) {
        engineName = KStringHandler::capwords(m_engine->pluginInfo().name());
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


