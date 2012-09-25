/*
    This is the new kwindecoration kcontrol module

    Copyright (c) 2001
        Karol Szwed <gallium@kde.org>
        http://gallium.n3.net/
    Copyright 2009, 2010 Martin Gräßlin <kde@martin-graesslin.com>

    Supports new kwin configuration plugins, and titlebar button position
    modification via dnd interface.

    Based on original "kwintheme" (Window Borders)
    Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

*/
// own
#include "kwindecoration.h"
#include "decorationmodel.h"
#include "src/aurorae/lib/auroraetheme.h"
// Qt
#include <QTimer>
#include <QtDBus/QtDBus>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeItem>
#include <QtDeclarative/QDeclarativeView>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QGraphicsObject>
#include <QtGui/QScrollBar>
#include <QUiLoader>
// KDE
#include <KAboutData>
#include <KDialog>
#include <KLocale>
#include <KMessageBox>
#include <KNS3/DownloadDialog>
#include <KDE/KStandardDirs>
#include <KDE/KConfigDialogManager>
#include <KPluginFactory>
#include <Plasma/ConfigLoader>
#include <qdeclarative.h>
#include <KDebug>
#include <QDebug>

namespace KWin
{

KWinDecorationForm::KWinDecorationForm(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
}


KWinDecorationModule::KWinDecorationModule(QWidget* parent) :
      kwinConfig(KSharedConfig::openConfig("kwinrc"))
    , m_showTooltips(false)
    , m_configLoaded(false)
    , m_decorationButtons(new DecorationButtons(this))
    , m_lastPreviewWidth(-1)
    , m_previewUpdateTimer(NULL)
{
    const QString mainQmlPath = KStandardDirs::locate("data", "kwin/kwinkoker/main.qml");
    if (mainQmlPath.isNull()) {
        // TODO 4.10 i18n this
        KMessageBox::error(this, "<h1>Installation error</h1>"
        "The resource<h2>kwin/kcm_kwindecoration/main.qml</h2>could not be located in any application data path."
        "<h2>Please contact your distribution</h2>"
        "The application will now abort", "Installation Error");
        abort();
    }
    qmlRegisterType<Aurorae::AuroraeTheme>("org.kde.kwin.aurorae", 0, 1, "AuroraeTheme");
    m_ui = new KWinDecorationForm(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_ui);

    KConfigGroup style(kwinConfig, "Style");

    // Set up the decoration lists and other UI settings
    m_model = new DecorationModel(kwinConfig, this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    kDebug() << "adssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss";
    m_ui->decorationList->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    foreach (const QString &importPath, KGlobal::dirs()->findDirs("module", "imports")) {
        m_ui->decorationList->engine()->addImportPath(importPath);
        kDebug() << "importttttttttttttttttttttttt" << importPath;
    }
    m_ui->decorationList->rootContext()->setContextProperty("decorationModel", m_proxyModel);
    m_ui->decorationList->rootContext()->setContextProperty("decorationBaseModel", m_model);
    m_ui->decorationList->rootContext()->setContextProperty("options", m_decorationButtons);
    m_ui->decorationList->rootContext()->setContextProperty("highlightColor", m_ui->decorationList->palette().color(QPalette::Highlight));
    m_ui->decorationList->rootContext()->setContextProperty("sliderWidth", m_ui->decorationList->verticalScrollBar()->width());
    m_ui->decorationList->rootContext()->setContextProperty("auroraeSource", KStandardDirs::locate("data", "kwin/aurorae/aurorae.qml"));
    m_ui->decorationList->rootContext()->setContextProperty("decorationActiveCaptionColor", KDecoration::options()->color(ColorFont, true));
    m_ui->decorationList->rootContext()->setContextProperty("decorationInactiveCaptionColor", KDecoration::options()->color(ColorFont, false));
    m_ui->decorationList->rootContext()->setContextProperty("decorationActiveTitleBarColor", KDecoration::options()->color(ColorTitleBar, true));
    m_ui->decorationList->rootContext()->setContextProperty("decorationInactiveTitleBarColor", KDecoration::options()->color(ColorTitleBar, false));
    m_ui->decorationList->setSource(mainQmlPath);

    connect(m_ui->decorationList->rootObject(), SIGNAL(widthChanged()), SLOT(updatePreviewWidth()));
    connect(m_ui->searchEdit, SIGNAL(textChanged(QString)), m_proxyModel, SLOT(setFilterFixedString(QString)));

    m_ui->decorationList->disconnect(m_ui->decorationList->verticalScrollBar());
    m_ui->decorationList->verticalScrollBar()->disconnect(m_ui->decorationList);
    connect(m_ui->decorationList->rootObject(), SIGNAL(contentYChanged()), SLOT(updateScrollbarValue()));
    connect(m_ui->decorationList->rootObject(), SIGNAL(contentHeightChanged()), SLOT(updateScrollbarRange()));
    connect(m_ui->decorationList->verticalScrollBar(), SIGNAL(rangeChanged(int, int )), SLOT(updateScrollbarRange()));
    connect(m_ui->decorationList->verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(updateViewPosition(int)));

    m_ui->decorationList->installEventFilter(this);
    m_ui->decorationList->viewport()->installEventFilter(this);
}


KWinDecorationModule::~KWinDecorationModule()
{
}

int KWinDecorationModule::itemWidth() const
{
    return m_ui->decorationList->rootObject()->property("width").toInt();
}

bool KWinDecorationModule::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_ui->decorationList) {
        if (e->type() == QEvent::Resize)
            updateScrollbarRange();
        else if (e->type() == QEvent::KeyPress) {
            int d = 0;
            const int currentRow = m_ui->decorationList->rootObject()->property("currentIndex").toInt();
            const int key = static_cast<QKeyEvent*>(e)->key();
            switch (key) {
            case Qt::Key_Home:
                d = -currentRow;
                break;
            case Qt::Key_End:
                d = m_proxyModel->rowCount() - (1 + currentRow);
                break;
            case Qt::Key_Up:
                d = -1;
                break;
            case Qt::Key_Down:
                d = 1;
                break;
            case Qt::Key_PageUp:
            case Qt::Key_PageDown:
                d = 150;
                if (QObject *decoItem = m_ui->decorationList->rootObject()->findChild<QObject*>("decorationItem")) {
                    QVariant v = decoItem->property("height");
                    if (v.isValid())
                        d = v.toInt();
                }
                if (d > 0)
                    d = qMax(m_ui->decorationList->height() / d, 1);
                if (key == Qt::Key_PageUp)
                    d = -d;
                break;
            default:
                break;
            }
            if (d) {
                d = qMin(qMax(0, currentRow + d), m_proxyModel->rowCount());
                m_ui->decorationList->rootObject()->setProperty("currentIndex", d);
                return true;
            }
        }
    } else if (m_ui->decorationList->viewport()) {
        if (e->type() == QEvent::Wheel) {
            return static_cast<QWheelEvent*>(e)->orientation() == Qt::Horizontal;
        }
    }
    return false;
}

void KWinDecorationModule::updatePreviews()
{
    m_ui->decorationList->rootContext()->setContextProperty("sliderWidth", m_ui->decorationList->verticalScrollBar()->width());
    const int newWidth = m_ui->decorationList->rootObject()->property("width").toInt();
    if (newWidth == m_lastPreviewWidth)
        return;
    m_lastPreviewWidth = newWidth;
    const int h = m_ui->decorationList->rootObject()->property("contentHeight").toInt();
    const int y = m_ui->decorationList->rootObject()->property("contentY").toInt();
    // start at first element in sight
    int row = 0;
    if (h > 0)
        row = qMin(qMax(0, y*m_model->rowCount()/h), m_model->rowCount());
}

void KWinDecorationModule::updatePreviewWidth()
{
    if (!m_previewUpdateTimer) {
        m_previewUpdateTimer = new QTimer(this);
        m_previewUpdateTimer->setSingleShot(true);
        connect(m_previewUpdateTimer, SIGNAL(timeout()), this, SLOT(updatePreviews()));
    }
    m_previewUpdateTimer->start(100);
}

void KWinDecorationModule::updateScrollbarRange()
{
    m_ui->decorationList->verticalScrollBar()->blockSignals(true);
    const int h = m_ui->decorationList->rootObject()->property("contentHeight").toInt();
    m_ui->decorationList->verticalScrollBar()->setRange(0, h - m_ui->decorationList->height());
    m_ui->decorationList->verticalScrollBar()->setPageStep(m_ui->decorationList->verticalScrollBar()->maximum()/m_model->rowCount());
    m_ui->decorationList->verticalScrollBar()->blockSignals(false);
}

void KWinDecorationModule::updateScrollbarValue()
{
    const int v = m_ui->decorationList->rootObject()->property("contentY").toInt();
    m_ui->decorationList->verticalScrollBar()->blockSignals(true); // skippig this will kill kinetic scrolling but the scrollwidth is too low
    m_ui->decorationList->verticalScrollBar()->setValue(v);
    m_ui->decorationList->verticalScrollBar()->blockSignals(false);
}

void KWinDecorationModule::updateViewPosition(int v)
{
    QGraphicsObject *list = m_ui->decorationList->rootObject();
    list->setProperty("contentY", v);
}

DecorationButtons::DecorationButtons(QObject *parent)
    : QObject(parent)
    , m_customPositions(false)
    , m_leftButtons(KDecorationOptions::defaultTitleButtonsLeft())
    , m_rightButtons(KDecorationOptions::defaultTitleButtonsRight())
{
}

DecorationButtons::~DecorationButtons()
{
}

bool DecorationButtons::customPositions() const
{
    return m_customPositions;
}

const QString &DecorationButtons::leftButtons() const
{
    return m_leftButtons;
}

const QString &DecorationButtons::rightButtons() const
{
    return m_rightButtons;
}

void DecorationButtons::setCustomPositions(bool set)
{
    if (m_customPositions == set) {
        return;
    }
    m_customPositions = set;
    emit customPositionsChanged();
}

void DecorationButtons::setLeftButtons(const QString &leftButtons)
{
    if (m_leftButtons == leftButtons) {
        return;
    }
    m_leftButtons = leftButtons;
    emit leftButtonsChanged();
}

void DecorationButtons::setRightButtons (const QString &rightButtons)
{
    if (m_rightButtons == rightButtons) {
        return;
    }
    m_rightButtons = rightButtons;
    emit rightButtonsChanged();
}

} // namespace KWin

#include "kwindecoration.moc"
