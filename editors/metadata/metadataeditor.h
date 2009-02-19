#ifndef METADATAEDITOR_H
#define METADATAEDITOR_H

#include <QWidget>
#include <QStringList>

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
    void serviceTypeChanged();

private:
    Ui::MetaDataEditor *view;
    QString filename;
    Plasma::PackageMetadata *metadata;
    QStringList apis;
};

#endif // METADATAEDITOR__H

