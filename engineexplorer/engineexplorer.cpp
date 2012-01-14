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

#include "engineexplorer.h"

#include <QApplication>
#include <QStandardItemModel>
#include <QBitmap>
#include <QBitArray>

#include <KIconLoader>
#include <KIconTheme>
#include <KMenu>
#include <KStandardAction>
#include <KStringHandler>
#include <KAction>

#ifdef FOUND_SOPRANO
#include <Soprano/Node>
Q_DECLARE_METATYPE(Soprano::Node)
#endif // FOUND_SOPRANO
Q_DECLARE_METATYPE(Plasma::DataEngine::Data)

#include <Plasma/DataEngineManager>

#include "serviceviewer.h"
#include "titlecombobox.h"

EngineExplorer::EngineExplorer(QWidget* parent)
    : KDialog(parent),
      m_engine(0),
      m_sourceCount(0),
      m_requestingSource(false)
{
#ifdef FOUND_SOPRANO
    (void) qRegisterMetaType<Soprano::Node>();
#endif
    setWindowTitle(i18n("Plasma Engine Explorer"));
    QWidget* mainWidget = new QWidget(this);
    setMainWidget(mainWidget);
    setupUi(mainWidget);

    m_engineManager = Plasma::DataEngineManager::self();
    m_dataModel = new QStandardItemModel(this);
    KIcon pix("plasma");
    int size = IconSize(KIconLoader::Dialog);
    m_title->setPixmap(pix.pixmap(size, size));
    connect(m_engines, SIGNAL(activated(QString)), this, SLOT(showEngine(QString)));
    connect(m_sourceRequesterButton, SIGNAL(clicked(bool)), this, SLOT(requestSource()));
    connect(m_serviceRequesterButton, SIGNAL(clicked(bool)), this, SLOT(requestServiceForSource()));
    m_data->setModel(m_dataModel);
    m_data->setWordWrap(true);

    m_searchLine->setTreeView(m_data);
    m_searchLine->setClickMessage(i18n("Search"));

    listEngines();
    m_engines->setFocus();

    setButtons(KDialog::Close | KDialog::User1 | KDialog::User2);
    setButtonText(KDialog::User1, i18n("Collapse All"));
    setButtonText(KDialog::User2, i18n("Expand All"));
    connect(this, SIGNAL(user1Clicked()), m_data, SLOT(collapseAll()));
    connect(this, SIGNAL(user2Clicked()), m_data, SLOT(expandAll()));
    enableButton(KDialog::User1, false);
    enableButton(KDialog::User2, false);

    addAction(KStandardAction::quit(qApp, SLOT(quit()), this));

    connect(m_data, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showDataContextMenu(QPoint)));
    m_data->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(cleanUp()));
}

EngineExplorer::~EngineExplorer()
{
}

void EngineExplorer::cleanUp()
{
    if (!m_engineName.isEmpty()) {
        m_engineManager->unloadEngine(m_engineName);
    }
}

void EngineExplorer::setApp(const QString &app)
{
    m_app = app;

    if (m_engines->count() > 0) {
        listEngines();
    }
}

void EngineExplorer::setEngine(const QString &engine)
{
    //find the engine in the combo box
    int index = m_engines->findText(engine);
    if (index != -1) {
        kDebug() << QString("Engine %1 found!").arg(engine);
        m_engines->setCurrentIndex(index);
        showEngine(engine);
    }
}

void EngineExplorer::setInterval(const int interval)
{
    m_updateInterval->setValue(interval);
}

void EngineExplorer::dataUpdated(const QString& source, const Plasma::DataEngine::Data& data)
{
    QList<QStandardItem*> items = m_dataModel->findItems(source, 0);

    if (items.isEmpty()) {
        return;
    }

    QStandardItem* parent = items.first();

    int rows = showData(parent, data);

    while (parent->rowCount() > rows) {
        parent->removeRow(rows);
    }
}

void EngineExplorer::listEngines()
{
    m_engines->clear();
    KPluginInfo::List engines = m_engineManager->listEngineInfo(m_app);
    qSort(engines);

    foreach (const KPluginInfo engine, engines) {
        m_engines->addItem(KIcon(engine.icon()), engine.pluginName());
    }

    m_engines->setCurrentIndex(-1);
}

void EngineExplorer::showEngine(const QString& name)
{
    m_sourceRequester->setEnabled(false);
    m_sourceRequesterButton->setEnabled(false);
    m_serviceRequester->setEnabled(false);
    m_serviceRequesterButton->setEnabled(false);
    enableButton(KDialog::User1, false);
    enableButton(KDialog::User2, false);
    m_dataModel->clear();
    m_dataModel->setColumnCount(4);
    QStringList headers;
    headers << i18n("DataSource") << i18n("Key") << i18n("Value") << i18n("Type");
    m_dataModel->setHorizontalHeaderLabels(headers);
    m_engine = 0;
    m_sourceCount = 0;

    if (!m_engineName.isEmpty()) {
        m_engineManager->unloadEngine(m_engineName);
    }

    m_engineName = name;
    if (m_engineName.isEmpty()) {
        updateTitle();
        return;
    }

    m_engine = m_engineManager->loadEngine(m_engineName);
    if (!m_engine) {
        m_engineName.clear();
        updateTitle();
        return;
    }

    //kDebug() << "showing engine " << m_engine->objectName();
    //kDebug() << "we have " << sources.count() << " data sources";
    connect(m_engine, SIGNAL(sourceAdded(QString)), this, SLOT(addSource(QString)));
    connect(m_engine, SIGNAL(sourceRemoved(QString)), this, SLOT(removeSource(QString)));
    foreach (const QString& source, m_engine->sources()) {
        //kDebug() << "adding " << source;
        addSource(source);
    }

    m_sourceRequesterButton->setEnabled(true);
    m_updateInterval->setEnabled(true);
    m_sourceRequester->setEnabled(true);
    m_sourceRequester->setFocus();
    m_serviceRequester->setEnabled(true);
    m_serviceRequesterButton->setEnabled(true);
    updateTitle();
}

void EngineExplorer::addSource(const QString& source)
{
    kDebug() << "adding" << source;
    QList<QStandardItem*> items = m_dataModel->findItems(source, 0);
    if (!items.isEmpty()) {
        kDebug() << "er... already there?";
        return;
    }

    QStandardItem* parent = new QStandardItem(source);
    m_dataModel->appendRow(parent);

    kDebug() << "getting data for source " << source;

    if (!m_requestingSource || m_sourceRequester->text() != source) {
        kDebug() << "connecting up now";
        m_engine->connectSource(source, this);
    }

    ++m_sourceCount;
    updateTitle();

    enableButton(KDialog::User1, true);
    enableButton(KDialog::User2, true);
}

void EngineExplorer::removeSource(const QString& source)
{
    QList<QStandardItem*> items = m_dataModel->findItems(source, 0);

    if (items.count() < 1) {
        return;
    }

    foreach (QStandardItem* item, items) {
        m_dataModel->removeRow(item->row());
    }

    --m_sourceCount;
    m_engine->disconnectSource(source, this);
    updateTitle();
}

void EngineExplorer::requestSource()
{
    requestSource(m_sourceRequester->text());
}

void EngineExplorer::requestServiceForSource()
{
    ServiceViewer *viewer = new ServiceViewer(m_engine, m_serviceRequester->text());
    viewer->show();
}

void EngineExplorer::requestSource(const QString &source)
{
    if (!m_engine || source.isEmpty()) {
        return;
    }

    kDebug() << "request source" << source;
    m_requestingSource = true;
    m_engine->connectSource(source, this, (uint)m_updateInterval->value());
    m_requestingSource = false;
}

void EngineExplorer::showDataContextMenu(const QPoint &point)
{
    QModelIndex index = m_data->indexAt(point);
    if (index.isValid()) {
        if (index.parent().isValid()) {
            index = index.parent();
        }

        if (index.column() != 0) {
            index = m_dataModel->index(index.row(), 0);
        }

        const QString source = index.data().toString();
        KMenu menu;
        menu.addTitle(source);
        QAction *service = menu.addAction(i18n("Get associated service"));
        QAction *update = menu.addAction(i18n("Update source now"));
        QAction *remove = menu.addAction(i18n("Remove source"));

        QAction *activated = menu.exec(m_data->viewport()->mapToGlobal(point));
        if (activated == service) {
            ServiceViewer *viewer = new ServiceViewer(m_engine, source);
            viewer->show();
        } else if (activated == update) {
            m_engine->connectSource(source, this);
            Plasma::DataEngine::Data data = m_engine->query(source);
        } else if (activated == remove) {
            removeSource(source);
        }
    }
}

QString EngineExplorer::convertToString(const QVariant &value)
{
    switch (value.type())
    {
        case QVariant::BitArray: {
            return i18np("&lt;1 bit&gt;", "&lt;%1 bits&gt;", value.toBitArray().size());
        }
        case QVariant::Bitmap: {
            QBitmap bitmap = value.value<QBitmap>();
            return QString("<%1x%2px - %3bpp>").arg(bitmap.width()).arg(bitmap.height()).arg(bitmap.depth());
        }
        case QVariant::ByteArray: {
            // Return the array size if it is not displayable
            if (value.toString().isEmpty()) {
                return i18np("&lt;1 byte&gt;", "&lt;%1 bytes&gt;", value.toByteArray().size());
            }
            else {
                return value.toString();
            }
        }
        case QVariant::Image: {
            QImage image = value.value<QImage>();
            return QString("<%1x%2px - %3bpp>").arg(image.width()).arg(image.height()).arg(image.depth());
        }
        case QVariant::Line: {
           QLine line = value.toLine();
           return QString("<x1:%1, y1:%2, x2:%3, y2:%4>").arg(line.x1()).arg(line.y1()).arg(line.x2()).arg(line.y2());
        }
        case QVariant::LineF: {
           QLineF lineF = value.toLineF();
           return QString("<x1:%1, y1:%2, x2:%3, y2:%4>").arg(lineF.x1()).arg(lineF.y1()).arg(lineF.x2()).arg(lineF.y2());
        }
        case QVariant::Locale: {
            return QString("%1").arg(value.toLocale().name());
        }
        case QVariant::Map: {
            QVariantMap map = value.toMap();
            QString str = i18np("&lt;1 item&gt;", "&lt;%1 items&gt;", map.size());

            QMapIterator<QString, QVariant> it(map);
            while (it.hasNext()) {
                it.next();
                str += "\n" + it.key() + ": " + convertToString(it.value());
            }

            return str;
        }
        case QVariant::Pixmap: {
            QPixmap pixmap = value.value<QPixmap>();
            return QString("<%1x%2px - %3bpp>").arg(pixmap.width()).arg(pixmap.height()).arg(pixmap.depth());
        }
        case QVariant::Point: {
           QPoint point = value.toPoint();
           return QString("<x:%1, y:%2>").arg(point.x()).arg(point.y());
        }
        case QVariant::PointF: {
           QPointF pointF = value.toPointF();
           return QString("<x:%1, y:%2>").arg(pointF.x()).arg(pointF.y());
        }
        case QVariant::Rect: {
            QRect rect = value.toRect();
            return QString("<x:%1, y:%2, w:%3, h:%4>").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
        }
        case QVariant::RectF: {
            QRectF rectF = value.toRectF();
            return QString("<x:%1, y:%2, w:%3, h:%4>").arg(rectF.x()).arg(rectF.y()).arg(rectF.width()).arg(rectF.height());
        }
        case QVariant::RegExp: {
            return QString("%1").arg(value.toRegExp().pattern());
        }
        case QVariant::Region: {
            QRect region = value.value<QRegion>().boundingRect();
            return QString("<x:%1, y:%2, w:%3, h:%4>").arg(region.x()).arg(region.y()).arg(region.width()).arg(region.height());
        }
        case QVariant::Size: {
            QSize size = value.toSize();
            return QString("<w:%1, h:%2>").arg(size.width()).arg(size.height());
        }
        case QVariant::SizeF: {
            QSizeF sizeF = value.toSizeF();
            return QString("<w:%1, h:%2>").arg(sizeF.width()).arg(sizeF.height());
        }
        case QVariant::Url: {
            return QString("%1").arg(value.toUrl().toString());
        }
        default: {
#ifdef FOUND_SOPRANO
            if (QLatin1String(value.typeName()) == "Soprano::Node") {
                Soprano::Node node = value.value<Soprano::Node>();
                if (node.isLiteral()) {
                    return convertToString(node.literal().variant());
                } else if (node.isResource()) {
                    return node.uri().toString();
                } else if (node.isBlank()) {
                    return QString("_:%1").arg(node.identifier());
                }
            }
#endif
            Plasma::DataEngine::Data data = value.value<Plasma::DataEngine::Data>();
            if (!data.isEmpty()) {
                QStringList result;
                QHashIterator<QString, QVariant> it(data);

                while (it.hasNext()) {
                    it.next();
                    result << (it.key() + ": " + it.value().toString());
                }

                return result.join("\n");
            } else if (value.canConvert(QVariant::String)) {
                if (value.toString().isEmpty()) {
                    return i18nc("The user did a query to a dataengine and it returned empty data", "<empty>");
                }
                else {
                    return value.toString();
                }
            }

            return i18nc("A the dataengine returned something that the humble view on the engineexplorer can't display, like a picture", "<not displayable>");
        }
    }
}

int EngineExplorer::showData(QStandardItem* parent, Plasma::DataEngine::Data data)
{
    int rowCount = 0;
    Plasma::DataEngine::DataIterator it(data);
//    parent->insertRows(0, data.count());
//    parent->setColumnCount(3);
    while (it.hasNext()) {
        it.next();
        parent->setChild(rowCount, 1, new QStandardItem(it.key()));
        if (it.value().canConvert(QVariant::List)) {
            foreach(const QVariant &var, it.value().toList()) {
                QStandardItem *item = new QStandardItem(convertToString(var));
                item->setToolTip(item->text());
                parent->setChild(rowCount, 2, item);
                parent->setChild(rowCount, 3, new QStandardItem(var.typeName()));
                ++rowCount;
            }
        } else {
            QStandardItem *item;
            if (it.value().canConvert<QIcon>()) {
                item = new QStandardItem(it.value().value<QIcon>(), "");
            } else {
                item = new QStandardItem(convertToString(it.value()));
            }

            item->setToolTip(item->text());
            parent->setChild(rowCount, 2, item);
            parent->setChild(rowCount, 3, new QStandardItem(it.value().typeName()));
            ++rowCount;
        }
    }

    return rowCount;
}

void EngineExplorer::updateTitle()
{
    if (!m_engine) {
        m_title->setPixmap(KIcon("plasma").pixmap(IconSize(KIconLoader::Dialog)));
        m_title->setText(i18n("Plasma DataEngine Explorer"));
        return;
    }

    m_title->setText(ki18ncp("The name of the engine followed by the number of data sources",
                             "%1 Engine - 1 data source", "%1 Engine - %2 data sources")
                              .subs(KStringHandler::capwords(m_engine->name()))
                              .subs(m_sourceCount).toString());

    if (m_engine->icon().isEmpty()) {
        m_title->setPixmap(KIcon("plasma").pixmap(IconSize(KIconLoader::Dialog)));
    } else {
        //m_title->setPixmap(KIcon("alarmclock").pixmap(IconSize(KIconLoader::Dialog)));
        m_title->setPixmap(KIcon(m_engine->icon()).pixmap(IconSize(KIconLoader::Dialog)));
    }
}

#include "engineexplorer.moc"

