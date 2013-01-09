/*
*   Copyright (c) 2012 $AUTHOR <$EMAIL>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License as published by
*   the Free Software Foundation; either version 2.1 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with this program; if not, write to the
*   Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .
*/

/*
 * For more information about the KWin Scripting please take a look at the below lines
 * Tutorial: http://techbase.kde.org/Development/Tutorials/KWin/Scripting
 * Api: http://techbase.kde.org/Development/Tutorials/KWin/Scripting/API_4.9
*/

var keepAboveMaximized = new Array();

function manageKeepAbove(client, h, v) {
  if (h && v) {
    // maximized
    if (client.keepAbove) {
      keepAboveMaximized[keepAboveMaximized.length] = client;
      client.keepAbove = false;
    }
  } else {
    // no longer maximized
    var found = keepAboveMaximized.indexOf(client);
    if (found != -1) {
      client.keepAbove = true;
      keepAboveMaximized.splice(found, 1);
    }
  }
}

workspace.clientMaximizeSet.connect(manageKeepAbove);
