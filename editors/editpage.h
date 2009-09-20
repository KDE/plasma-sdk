
#ifndef EDITPAGE_H
#define EDITPAGE_H

#include <QWidget>
#include <QModelIndex>
#include <kparts/part.h>
#include <kservice.h>

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

private Q_SLOTS:
    void findEditor(const QModelIndex &index);

signals:
    void loadEditor(const KService::List offers, KUrl target);
};

#endif

