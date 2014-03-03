/* vim:set ts=2 nowrap: ****************************************************

 qutselect - A simple Qt4 based GUI frontend for SRSS (utselect)
 Copyright (C) 2009 by Jens Langner <Jens.Langner@light-speed.de>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 $Id: CMainWindow.h 133 2014-02-28 12:42:18Z maus $

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
