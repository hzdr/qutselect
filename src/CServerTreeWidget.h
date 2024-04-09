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

#ifndef CSERVERTREEWIDGET_H
#define CSERVERTREEWIDGET_H

#include <QTreeWidget>

// forward declarations

class CServerTreeWidget : public QTreeWidget
{
  Q_OBJECT

  public:
    CServerTreeWidget(QWidget* parent = 0);

    QSize sizeHint() const;
};

#endif /* CSERVERTREEWIDGET_H */
