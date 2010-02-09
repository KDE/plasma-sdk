
#include "editpage.h"

#include <QHBoxLayout>
#include <QFile>
#include <KIcon>
#include <KMessageBox>
#include <KInputDialog>
#include <kmimetypetrader.h>
#include "packagemodel.h"

EditPage::EditPage(QWidget *parent)
        : QTreeView(parent)
{
    m_contextMenu = new KMenu();
    QAction *del = m_contextMenu->addAction(KIcon("window-close"), i18n("Delete"));
    connect(del, SIGNAL(triggered(bool)), this, SLOT(doDelete(bool)));

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(activated(const QModelIndex &)), this, SLOT(findEditor(const QModelIndex &)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showTreeContextMenu(const QPoint&)));
}

void EditPage::doDelete(bool)
{
    QModelIndexList items = selectedIndexes();
    if (items.empty())
        return; // this shouldn't happen
    QModelIndex selected = items.at(0); // only one can be selected
    QString path = selected.data(PackageModel::UrlRole).toString();
    QString name = selected.data(Qt::DisplayRole).toString();
    QString dialogText = i18n("Are you sure you want to delete this file: ");
    int code = KMessageBox::warningContinueCancel(this, 
                  dialogText + "'" + name + "'?");
    if (code == KMessageBox::Continue)
        QFile::remove(path);
}

void EditPage::showTreeContextMenu(const QPoint& her)
{
    QModelIndexList items = selectedIndexes();
    if (items.empty())
        return;

    const char *key = static_cast<const char *>
                              (items.at(0).internalPointer());
    if (!key || // top level item
          items.at(0).row() == 0) // 'New' entry
        return;

    m_contextMenu->popup(QCursor::pos());
}

void EditPage::findEditor(const QModelIndex &index)
{
    QStringList mimetypes = index.data(PackageModel::MimeTypeRole).toStringList();
    foreach(const QString &mimetype, mimetypes) {
        QString target = index.data(PackageModel::UrlRole).toString();
        if (mimetype == "[plasmate]/metadata") {
            emit loadMetaDataEditor(target);
            return;
        }
        if (mimetype == "[plasmate]/new") {
            QString dialogText = i18n( "Enter a name for the new file:" );
            QString file = KInputDialog::getText(QString(), dialogText);
            if (!file.isEmpty()) {
                file = target + file;
                QFile fl(file);
                fl.open(QIODevice::ReadWrite); // create the file
                fl.close();
            }
        }
        KService::List offers = KMimeTypeTrader::self()->query(mimetype, "KParts/ReadWritePart");
        kDebug() << mimetype;
        if (offers.isEmpty()) {
            offers = KMimeTypeTrader::self()->query(mimetype, "KParts/ReadOnlyPart");
        }
        if (!offers.isEmpty()) {
            //create the part using offers.at(0)
            //kDebug() << offers.at(0);
            //offers.at(0)->createInstance(parentWidget);
            emit loadEditor(offers, KUrl(target));
            return;
        }
    }
}

