/*
  Copyright 2009 Lim Yuen Hoe <yuenhoe@hotmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <QDialog>
#include <QUrl>
#include <KProcess>

#include "ui_publisher.h"

// TODO
//class SigningWidget;

class Publisher : public QDialog
{
    Q_OBJECT;
public:
    Publisher(QWidget* parent, const QUrl &path, const QString& type);
    void setProjectName(const QString &name);

public slots:
    void doPlasmaPkg();

private slots:
    void doExport();
    void doPublish();
    void doInstall();
    //TODO
    //void doRemoteInstall();
    void checkInstallButtonState(int comboBoxCurrentIndex);

private:
    bool exportToFile(const QUrl& url);
    bool exportPackage(const QUrl &toExport, const QUrl &targetFile);

    const QString tempPackagePath();

    //avoid duplication
    QString currentPackagePath() const;

    //TODO
    //SigningWidget* m_signingWidget;
    QUrl m_projectPath;
    QString m_projectType;
    QString m_extension;
    QString m_projectName;
    int m_comboBoxIndex;

    Ui::Publisher m_ui;
};

#endif // PUBLISHER_H
