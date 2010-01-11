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

module $DATAENGINE_NAME

# Declare our main class as PlasmaScripting::DataEngine subclass
class Main$DATAENGINE_NAME < PlasmaScripting::DataEngine


    #   Constructor, forward initialization to its superclass
    #   Note: try to NOT modify this constructor; all the setup code
    #   should be placed in the init method.
    def initialize(parent, args = nil)
      super(parent)
    end

    #   Init method
    #   Put here all the code needed to initialize our plasmoid
    def init
      setMinimumPollingInterval(333)
    end

    #   sources method
    #   Used by applets to request what data source the DataEngine has
    def sources
      timezones = KDE::SystemTimeZones.zones.keys
      timezones << "Local"
      return timezones
    end

    #   sourceRequestEvent method
    #   Called when an applet access the DataEngine and request for
    #   a specific source (name)
    def sourceRequestEvent(name)
      return updateSourceEvent(name)
    end

    #   updateSourceEvent method
    #   The main function for a DataEngine
    def updateSourceEvent(tz)
      # Add custom code here
      return true
    end
  end

end
