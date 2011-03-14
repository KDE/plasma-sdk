/*
 *  Copyright 2010 by Diego '[Po]lentino' Casella <polentino911@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef SIGNINGDIALOG_H
#define SIGNINGDIALOG_H

#include <QDialog>


class QCheckBox;
class QLabel;
class QPushButton;
class KComboBox;
class KIntNumInput;
class KLineEdit;


class SigningDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SigningDialog(QWidget *parent = 0);

Q_SIGNALS:
    void emitCreateKey(const QString &param);

public Q_SLOTS:
    void validateParams();
    void toggleShowPassword(const bool hide);
    //void toggleExpirationLine();
    void fetchParameters();

private:
    QLabel *m_nameLabel;
    QLabel *m_emailLabel;
    QLabel *m_commentLabel;
    //QLabel *m_expirationLabel;
    QLabel *m_pwdLabel;
    QLabel *m_repeatPwdLabel;
    QCheckBox *m_showPassword;

    KLineEdit *m_nameLine;
    KLineEdit *m_emailLine;
    KLineEdit *m_commentLine;
    //KIntNumInput *m_expirationLineInput;
    //KComboBox *m_expirationComboBox;
    KLineEdit *m_pwdLine;
    KLineEdit *m_repeatPwdLine;

    QPushButton *m_createButton;
    QPushButton *m_cancelButton;

};

#endif // SIGNINGDIALOG_H
