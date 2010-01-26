/*
  Copyright (c) 2009 Lim Yuen Hoe <yuenhoe@hotmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef DOCBROWSER_H
#define DOCBROWSER_H

#include <QWidget>

class QWebView;
class KUrl;
class QLabel;

namespace Ui
{
class DocBrowser;
}

class DocBrowser : public QWidget
{
    Q_OBJECT;
public:
    DocBrowser(QWidget* parent);
    KUrl currentPage() const;
    void load(KUrl page);

public slots:
    void showTutorial();
    void showApi();
    void findText(const QString& toFind);
    void findNext();
    void focusSearchField();

private:
    QWebView *m_view;
    KLineEdit *searchField;
    QLabel *searchLabel;
};

#endif // DOCBROWSER_H