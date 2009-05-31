
#include "editpage.h"

#include <QHBoxLayout>
#include <QTreeView>
#include <kservice.h>
#include <kmimetypetrader.h>

#include "packagemodel.h"

EditPage::EditPage(QWidget *parent)
    : QWidget(parent)
{
    m_tree = new QTreeView(this);
    m_editor = new QWidget(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_tree);
    layout->addWidget(m_editor);
    
    connect(m_tree, SIGNAL(activated(const QModelIndex &)), this, SLOT(findEditor(const QModelIndex &)));
}

void EditPage::findEditor(const QModelIndex &index)
{
    QStringList mimetypes = index.data(PackageModel::MimeTypeRole).toStringList();
    foreach (const QString &mimetype, mimetypes) {
        KService::List offers = KMimeTypeTrader::self()->query(mimetype, "KParts/ReadWritePart");
	kDebug() << mimetype;
        if (offers.isEmpty()) {
            offers = KMimeTypeTrader::self()->query(mimetype, "KParts/ReadOnlyPart");
        }
        if (!offers.isEmpty()) {
            //create the part using offers.at(0)
	    //kDebug() << offers.at(0);
	    //offers.at(0)->createInstance(parentWidget);
	    emit loadEditor(offers);
        return;
        }
    }
}

void EditPage::setModel(PackageModel *model)
{
    m_tree->setModel(model);
}

#include "editpage.moc"

