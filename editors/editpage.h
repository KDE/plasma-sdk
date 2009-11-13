
#ifndef EDITPAGE_H
#define EDITPAGE_H

#include <QWidget>
#include <QModelIndex>
#include <kparts/part.h>
#include <kservice.h>
#include <KMenu>
#include <QTreeView>

class QWidget;

class PackageModel;

class EditPage : public QTreeView
{
    Q_OBJECT

public:
    explicit EditPage(QWidget *parent = 0);

private:
    KMenu *m_contextMenu;

private Q_SLOTS:
    void findEditor(const QModelIndex &index);
    void showTreeContextMenu(const QPoint&);
    void doDelete(bool);

signals:
    void loadEditor(const KService::List offers, KUrl target);
    void loadMetaDataEditor(KUrl target);
};

#endif

