
#ifndef EDITPAGE_H
#define EDITPAGE_H

#include <QWidget>

class QTreeView;
class QWidget;

class PackageModel;

class EditPage : public QWidget
{
    Q_OBJECT

public:
    explicit EditPage(QWidget *parent = 0);

    void setModel(PackageModel *model);

private:
    QTreeView *m_tree;
    QWidget *m_editor;
};

#endif

