/* vim:set ts=2 nowrap: ****************************************************

 qutselect - A simple Qt-based GUI frontend for remote terminals
 Copyright (C) 2008-2024 by Jens Maus <mail@jens-maus.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

**************************************************************************/

#ifndef CLOGINDIALOG_H

#include <QDialog>
 
class QLabel;
class QLineEdit;
class QDialogButtonBox;

class CLoginDialog : public QDialog
{
  Q_OBJECT
 
  public:
    explicit CLoginDialog(QWidget *parent, const QString& username, const QString& serverName);

    QString username(void);
    QString password(void);

  private:
    QLabel* labelUsername;
    QLabel* labelPassword;
    QLineEdit* editUsername;
    QLineEdit* editPassword;
    QDialogButtonBox* buttons;
 
    void setUpGUI();
};
 
#endif // CLOGINDIALOG_H
