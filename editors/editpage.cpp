
#include "editpage.h"

#include <QHBoxLayout>
#include <QTreeView>

#include "packagemodel.h"

EditPage::EditPage(QWidget *parent)
    : QWidget(parent)
{
    m_tree = new QTreeView(this);
    m_editor = new QWidget(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_tree);
    layout->addWidget(m_editor);
}

void EditPage::setModel(PackageModel *model)
{
    m_tree->setModel(model);
}

#include "editpage.moc"

