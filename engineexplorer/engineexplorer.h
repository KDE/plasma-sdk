/*
 *   Copyright 2007 Aaron Seigo <aseigo@kde.org>
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

#ifndef ENGINEEXPLORER_H
#define ENGINEEXPLORER_H

class QStandardItemModel;
class QStandardItem;

#include <Plasma/DataEngine>

#include "ui_engineexplorer.h"

namespace Plasma
{
    class DataEngineManager;
    class DataEngine;
} // namespace Plasma

class EngineExplorer : public KDialog, public Ui::EngineExplorer
{
    Q_OBJECT

    public:
        explicit EngineExplorer(QWidget *parent = 0);
        ~EngineExplorer();

        void setApp(const QString &app);
        void setEngine(const QString &engine);
        void setInterval(const int interval);
        void requestSource(const QString &source);

        static QString convertToString(const QVariant &value);

    public slots:
        void dataUpdated(const QString& source, const Plasma::DataEngine::Data& data);

    private slots:
        void showEngine(const QString& engine);
        void addSource(const QString& source);
        void removeSource(const QString& source);
        void requestSource();
        void requestServiceForSource();
        void showDataContextMenu(const QPoint &point);
        void cleanUp();

    private:
        void listEngines();
        int showData(QStandardItem* parent, Plasma::DataEngine::Data data);
        void updateTitle();

        Plasma::DataEngineManager* m_engineManager;
        QStandardItemModel* m_dataModel;
        QString m_app;
        QString m_engineName;
        Plasma::DataEngine* m_engine;
        int m_sourceCount;
        bool m_requestingSource;
};

#endif // multiple inclusion guard

