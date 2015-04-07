/*
  Copyright 2009 Lim Yuen Hoe <yuenhoe@hotmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DOCBROWSER_H
#define DOCBROWSER_H

#include <QDockWidget>

class QWebView;
class KLineEdit;
class QLabel;

class DocBrowser : public QDockWidget
{
    Q_OBJECT;
public:
    DocBrowser(QWidget* parent = 0);

public Q_SLOTS:
    void showTutorial();
    void showApi();
    void showHelp();
    void findText(const QString& toFind);
    void findNext();
    void focusSearchField();

private:
    QWebView *m_view;
    KLineEdit *m_searchField;
    QLabel *m_searchLabel;
};

#endif // DOCBROWSER_H
