# -*- coding: utf-8 -*-
#
# Author: $AUTHOR <$EMAIL>
# Date: $DATE
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as
# published by the Free Software Foundation; either version 2, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details
#
# You should have received a copy of the GNU Library General Public
# License along with this program; if not, write to the
# Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

# Import essential modules
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyKDE4.plasma import Plasma
from PyKDE4 import plasmascript


class $PLASMOID_NAME(plasmascript.Applet):

    #	Constructor, forward initialization to its superclass
    #	Note: try to NOT modify this constructor; all the setup code
    #	should be placed in the init method.
    def __init__(self,parent,args=None):
        plasmascript.Applet.__init__(self,parent)
 
    #	init method
    #	Put here all the code needed to initialize our plasmoid
    def init(self):
        self.setHasConfigurationInterface(False)
        self.resize(125, 125)
        self.setAspectRatioMode(Plasma.Square)
 
    #	Simple painting function
    def paintInterface(self, painter, option, rect):
        painter.save()
	# Simple paint code example
        #painter.setPen(Qt.white)
        #painter.drawText(rect, Qt.AlignVCenter | Qt.AlignHCenter, "Hello Python!")
        painter.restore()

    #	CreateApplet method
    #	Note: do NOT modify it, needed by Plasma
def CreateApplet(parent):
    return $PLASMOID_NAME(parent) 
