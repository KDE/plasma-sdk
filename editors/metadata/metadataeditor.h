/*
  Copyright (c) 2009 Richard Moore, <rich@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef METADATAEDITOR_H
#define METADATAEDITOR_H

#include <QWidget>
#include <QStringList>

namespace Ui
{
class MetaDataEditor;
}

namespace Plasma
{
class PackageMetadata;
}

class MetaDataEditor : public QWidget
{
    Q_OBJECT

public:
    MetaDataEditor(QWidget *parent = 0);
    ~MetaDataEditor();

    void setFilename(const QString &filename);

public slots:
    void readFile();
    void writeFile();

private slots:
    void serviceTypeChanged();

private:
    Ui::MetaDataEditor *view;
    QString filename;
    Plasma::PackageMetadata *metadata;
    QStringList apis;
    QStringList categories;
};

#endif // METADATAEDITOR__H

