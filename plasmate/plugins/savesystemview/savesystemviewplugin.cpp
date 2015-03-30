/*
   Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "savesystemviewplugin.h"
#include "savesystemview.h"

#include <interfaces/icore.h>
#include <interfaces/iuicontroller.h>
#include <interfaces/iplugincontroller.h>

#include <QAction>

#include <KActionCollection>
#include <KLocalizedString>
#include <KPluginFactory>

K_PLUGIN_FACTORY(KDevSaveSystemFactory, registerPlugin<SaveSystemViewPlugin>(); )

class KDevSaveSystemViewFactory: public KDevelop::IToolViewFactory
{
    public:
        KDevSaveSystemViewFactory(SaveSystemViewPlugin *plugin)
            : m_plugin(plugin)
        {
        }
        virtual QWidget* create(QWidget *parent = 0)
        {
            SaveSystemView *view = new SaveSystemView(m_plugin);
            return QWidget::createWindowContainer(view, parent);
        }
        virtual Qt::DockWidgetArea defaultPosition()
        {
            return Qt::LeftDockWidgetArea;
        }
        virtual QString id() const
        {
            return "org.kdevelop.SaveSystemView";
        }
    private:
        SaveSystemViewPlugin *m_plugin;
};

SaveSystemViewPlugin::SaveSystemViewPlugin(QObject *parent, const QVariantList&)
    : IPlugin("kdevsavesystemview", parent),
      m_factory(new KDevSaveSystemViewFactory(this))
{
    setXMLFile("kdevsavesystemview.rc");
    KDevelop::ICore::self()->uiController()->addToolView(i18n("Save System"), m_factory);
}

SaveSystemViewPlugin::~SaveSystemViewPlugin()
{
}

void SaveSystemViewPlugin::unload()
{
    KDevelop::ICore::self()->uiController()->removeToolView(m_factory);
}


#include "savesystemviewplugin.moc"

