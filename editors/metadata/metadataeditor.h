#ifndef METADATAEDITOR_H
#define METADATAEDITOR_H

#include <QWidget>

namespace Ui {
    class MetaDataEditor;
};

namespace Plasma {
    class PackageMetadata;
};

class MetaDataEditor : public QWidget
{
    Q_OBJECT

public:
    MetaDataEditor( QWidget *parent = 0 );
    ~MetaDataEditor();

    void setFilename( const QString &filename );

public slots:
    void readFile();
    void writeFile();

private slots:
    void updateKnownApis();

private:
    Ui::MetaDataEditor *view;
    QString filename;
    Plasma::PackageMetadata *metadata;
};

#endif // METADATAEDITOR__H

