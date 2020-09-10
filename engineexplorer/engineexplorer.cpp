/*
 *   SPDX-FileCopyrightText: 2007 Aaron Seigo <aseigo@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "engineexplorer.h"

#include <QApplication>
#include <QStandardItemModel>
#include <QBitmap>
#include <QBitArray>
#include <QDialogButtonBox>
#include <QMenu>
#include <QUrl>

#include <KStandardAction>
#include <KStringHandler>
#include <KLocalizedString>
#include <QAction>
#include <QDateTime>

#include <Plasma/PluginLoader>

#ifdef FOUND_SOPRANO
#include <Soprano/Node>
Q_DECLARE_METATYPE(Soprano::Node)
#endif // FOUND_SOPRANO
Q_DECLARE_METATYPE(Plasma::DataEngine::Data)

#include "modelviewer.h"
#include "serviceviewer.h"
#include "titlecombobox.h"

EngineExplorer::EngineExplorer(QWidget* parent)
    : QDialog(parent),
      m_engine(nullptr),
      m_sourceCount(0),
      m_requestingSource(false),
      m_expandButton(new QPushButton(i18n("Expand All"), this)),
      m_collapseButton(new QPushButton(i18n("Collapse All"), this))
{
#ifdef FOUND_SOPRANO
    (void) qRegisterMetaType<Soprano::Node>();
#endif
    setWindowTitle(i18n("Plasma Engine Explorer"));
    QWidget* mainWidget = new QWidget(this);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->addButton(m_expandButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_collapseButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(QDialogButtonBox::Close);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(mainWidget);
    layout->addWidget(buttonBox);
    setLayout(layout);

    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    setupUi(mainWidget);

    m_engineManager = Plasma::PluginLoader::self();
    m_dataModel = new QStandardItemModel(this);
    const int size = m_title->style()->pixelMetric(QStyle::PM_LargeIconSize);
    m_title->setIconSize(QSize(size, size));
    m_title->setIcon(QIcon::fromTheme("plasma"));
    connect(m_engines, SIGNAL(activated(QString)), this, SLOT(showEngine(QString)));
    connect(m_sourceRequesterButton, SIGNAL(clicked(bool)), this, SLOT(requestSource()));
    connect(m_serviceRequesterButton, &QAbstractButton::clicked, this, &EngineExplorer::requestServiceForSource);
    m_data->setModel(m_dataModel);
    m_data->setWordWrap(true);

    m_searchLine->setTreeView(m_data);
    m_searchLine->setPlaceholderText(i18n("Search"));

    listEngines();
    m_engines->setFocus();

    connect(m_collapseButton, &QAbstractButton::clicked, m_data, &QTreeView::collapseAll);
    connect(m_expandButton, &QAbstractButton::clicked, m_data, &QTreeView::expandAll);
    enableButtons(false);

    addAction(KStandardAction::quit(qApp, SLOT(quit()), this));

    connect(m_data, &QWidget::customContextMenuRequested,
            this, &EngineExplorer::showDataContextMenu);
    m_data->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(qApp, &QCoreApplication::aboutToQuit, this, &EngineExplorer::cleanUp);
}

EngineExplorer::~EngineExplorer()
{
}

void EngineExplorer::cleanUp()
{
    if (!m_engineName.isEmpty()) {
        //m_engineManager->unloadEngine(m_engineName);
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
    const int index = m_engines->findText(engine);
    if (index != -1) {
        qDebug() << QString("Engine %1 found!").arg(engine);
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
    QList<QStandardItem*> items = m_dataModel->findItems(source, Qt::MatchExactly);

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
    KPluginInfo::List engines = m_engineManager->listDataEngineInfo(m_app);
    std::sort(engines.begin(), engines.end());

    foreach (const KPluginInfo engine, engines) {
        m_engines->addItem(QIcon::fromTheme(engine.icon()), engine.pluginName());
    }

    m_engines->setCurrentIndex(-1);
}

void EngineExplorer::showEngine(const QString& name)
{
    m_sourceRequester->setEnabled(false);
    m_sourceRequesterButton->setEnabled(false);
    m_serviceRequester->setEnabled(false);
    m_serviceRequesterButton->setEnabled(false);
    enableButtons(false);
    m_dataModel->clear();
    m_dataModel->setColumnCount(4);
    QStringList headers;
    headers << i18n("DataSource") << i18n("Key") << i18n("Value") << i18n("Type");
    m_dataModel->setHorizontalHeaderLabels(headers);
    m_engine = nullptr;
    m_sourceCount = 0;

    if (!m_engineName.isEmpty()) {
        //m_engineManager->unloadEngine(m_engineName);
    }

    m_engineName = name;
    if (m_engineName.isEmpty()) {
        updateTitle();
        return;
    }

    m_engine = m_engineManager->loadDataEngine(m_engineName);
    if (!m_engine) {
        m_engineName.clear();
        updateTitle();
        return;
    }

    //qDebug() << "showing engine " << m_engine->objectName();
    //qDebug() << "we have " << sources.count() << " data sources";
    connect(m_engine, &Plasma::DataEngine::sourceAdded, this, &EngineExplorer::addSource);
    connect(m_engine, &Plasma::DataEngine::sourceRemoved, this, &EngineExplorer::removeSource);
    foreach (const QString& source, m_engine->sources()) {
        //qDebug() << "adding " << source;
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
    //qDebug() << "adding" << source;
    QList<QStandardItem*> items = m_dataModel->findItems(source, Qt::MatchExactly);
    if (!items.isEmpty()) {
        //qDebug() << "er... already there?";
        return;
    }

    QStandardItem* parent = new QStandardItem(source);
    m_dataModel->appendRow(parent);

    //qDebug() << "getting data for source " << source;
    if (!m_requestingSource || m_sourceRequester->text() != source) {
        //qDebug() << "connecting up now";
        m_engine->connectSource(source, this);
    }

    ++m_sourceCount;
    updateTitle();

    enableButtons(true);
}

void EngineExplorer::removeSource(const QString& source)
{
    QList<QStandardItem*> items = m_dataModel->findItems(source, Qt::MatchExactly);

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

    qDebug() << "request source" << source;
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
        QMenu menu;
        menu.addSection(source);
        QAction *service = menu.addAction(i18n("Get associated service"));
        QAction *model = menu.addAction(i18n("Get associated model"));
        QAction *update = menu.addAction(i18n("Update source now"));
        QAction *remove = menu.addAction(i18n("Remove source"));

        QAction *activated = menu.exec(m_data->viewport()->mapToGlobal(point));
        if (activated == service) {
            ServiceViewer *viewer = new ServiceViewer(m_engine, source);
            viewer->show();
        } else if (activated == model) {
            ModelViewer *viewer = new ModelViewer(m_engine, source);
            viewer->show();
        } else if (activated == update) {
            m_engine->connectSource(source, this);
            //Plasma::DataEngine::Data data = m_engine->query(source);
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
            return i18np("<1 bit>", "<%1 bits>", value.toBitArray().size());
        }
        case QVariant::Bitmap: {
            QBitmap bitmap = value.value<QBitmap>();
            return QString("<%1x%2px - %3bpp>").arg(bitmap.width()).arg(bitmap.height()).arg(bitmap.depth());
        }
        case QVariant::ByteArray: {
            // Return the array size if it is not displayable
            if (value.toString().isEmpty()) {
                return i18np("<1 byte>", "<%1 bytes>", value.toByteArray().size());
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
            QString str = i18np("<1 item>", "<%1 items>", map.size());

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
        case QVariant::StringList: {
            return QString("%1").arg(value.toStringList().join(", "));
        }
        case QVariant::Date: {
            return QString("%1").arg(value.toDate().toString());
        }
        case QVariant::DateTime: {
            return QString("%1").arg(value.toDateTime().toString());
        }
        case QVariant::Time: {
            return QString("%1").arg(value.toTime().toString());
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
            if (QLatin1String(value.typeName()) == "QDateTime") {
                return QString("%1").arg(value.value<QDateTime>().toString());
            }

            Plasma::DataEngine::Data data = value.value<Plasma::DataEngine::Data>();
            if (!data.isEmpty()) {
                QStringList result;
                QMapIterator<QString, QVariant> it(data);

                while (it.hasNext()) {
                    it.next();
                    result << (it.key() + ": " + convertToString(it.value()));
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
        if (it.value().canConvert(QVariant::List) /* && ! it.value().type() == QVariant::StringList
                                                     */) {
            bool first = true;
            foreach (const QVariant &var, it.value().toList()) {
                QStandardItem *item = new QStandardItem(convertToString(var));
                if (!first) {
                    parent->setChild(rowCount, 1, new QStandardItem(QString()));
                }
                item->setToolTip(item->text());
                parent->setChild(rowCount, 2, item);
                parent->setChild(rowCount, 3, new QStandardItem(var.typeName()));
                first = false;
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
    if (!m_engine || !m_engine->pluginInfo().isValid()) {
        m_title->setIcon(QIcon::fromTheme("plasma"));
        m_title->setText(i18n("Plasma DataEngine Explorer"));
        return;
    }

    m_title->setText(ki18ncp("The name of the engine followed by the number of data sources",
                             "%1 Engine - 1 data source", "%1 Engine - %2 data sources")
                              .subs(KStringHandler::capwords(m_engine->pluginInfo().name()))
                              .subs(m_sourceCount).toString());

    if (m_engine->pluginInfo().icon().isEmpty()) {
        m_title->setIcon(QIcon::fromTheme("plasma"));
    } else {
        m_title->setIcon(QIcon::fromTheme(m_engine->pluginInfo().icon()));
    }
}

void EngineExplorer::enableButtons(bool enable)
{
    if (m_expandButton) {
        m_expandButton->setEnabled(enable);
    }

    if (m_collapseButton) {
        m_collapseButton->setEnabled(enable);
    }
}

