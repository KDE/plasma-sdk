# -*- coding: iso-8859-1 -*-
=begin
/*
 *   Author: $AUTHOR <$EMAIL>
 *   Date: $DATE
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
=end

# Load QT and KDE Ruby bindings
require 'plasma_applet'

module $PLASMOID_NAME

  # Declare our main class as PlasmaScripting::Applet subclass
  class Main$PLASMOID_NAME < PlasmaScripting::Applet

    #   Constructor, forward initialization to its superclass
    #   Note: try to NOT modify this constructor; all the setup code
    #   should be placed in the init method.
    def initialize parent
      super parent
    end

    #   Init method
    #   Put here all the code needed to initialize our plasmoid
    def init
      self.has_configuration_interface = false
      resize 125, 125
      self.aspect_ratio_mode = Plasma::Square
    end

    #   Simple painting function
    def paintInterface(painter, option, rect)
      painter.save
      # Simple paint code example
      #painter.pen = Qt::Color.new Qt::white
      #painter.draw_text rect, Qt::AlignVCenter | Qt::AlignHCenter, "Hello Ruby!"
      painter.restore
    end
  end
end
