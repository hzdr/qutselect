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
