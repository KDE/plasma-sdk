#ifndef METADATAEDITOR_H
#define METADATAEDITOR_H

#include <QWidget>

namespace Ui {
    class MetaDataEditor;
};

class MetaDataEditor : public QWidget
{
    Q_OBJECT

public:
    MetaDataEditor( QWidget *parent = 0 );
    ~MetaDataEditor();

    void readConfig( const QString &filename );

private:
    Ui::MetaDataEditor *view;
};

#endif // METADATAEDITOR__H

