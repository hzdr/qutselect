/* vim:set ts=2 nowrap: ****************************************************

 qutselect - A simple Qt4 based GUI frontend for SRSS (utselect)
 Copyright (C) 2009-2013 by Jens Langner <Jens.Langner@light-speed.de>

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

 $Id: CMainWindow.cpp 133 2014-02-28 12:42:18Z maus $

**************************************************************************/

#include "CServerTreeWidget.h"

#include <QApplication>
#include <QSize>

#include <iostream>
#include <rtdebug.h>

#include "config.h"

CServerTreeWidget::CServerTreeWidget(QWidget* parent)
  : QTreeWidget(parent)
{
  ENTER();
  LEAVE();
}

QSize CServerTreeWidget::sizeHint() const
{
  ENTER();

  int width=0;
  int height=0;
  QSize sh;

  // iterate through all columns to calculate the width
  for(int i=0; i < columnCount(); ++i)
    width += 2 + columnWidth(i);

  // iterate through all TreeWidgetItems to calculate the height
  for(int i=0; i < topLevelItemCount(); ++i)
  {
    QTreeWidgetItem *item = topLevelItem(i);
    height += 2 + visualItemRect(item).height();
  }

  // set the QSize
  sh.setWidth(width + qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
  sh.setHeight(height + qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent));

  LEAVE();
  return sh;
}
