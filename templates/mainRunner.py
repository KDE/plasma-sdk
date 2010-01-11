# -*- coding: iso-8859-1 -*-
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
from PyQt4 import uic
from PyKDE4.kdeui import *
from PyKDE4.kdecore import *
from PyKDE4.plasma import *
from PyKDE4.plasmascript


class $RUNNER_NAME(plasmascript.Runner):

    #   Constructor, forward initialization to its superclass
    #   Note: try to NOT modify this constructor; all the setup code
    #   should be placed in the init method.
    def __init__(self,parent,args=None):
        plasmascript.Runner.__init__(self,parent)
        self.actions = []

    #   init method
    #   Put here all the code needed to initialize your runner
    def init(self):
        # Simple runner example
        #print '### init'
        #self.reloadConfiguration()
        #ign = Plasma.RunnerContext.Types(Plasma.RunnerContext.Directory |
        #Plasma.RunnerContext.File | \
        #Plasma.RunnerContext.NetworkLocation)
        #self.setIgnoredTypes(ign)
        #description = i18n("Python Test Runner. Responds Hello World! to hello (= :q:)");
        #self.addSyntax(Plasma.RunnerSyntax(":q:", description))
        #self.setSpeed(Plasma.AbstractRunner.NormalSpeed)
        #self.setPriority(Plasma.AbstractRunner.LowestPriority)
        #self.setHasRunOptions(True)

    #   match method
    #   Assign an action for a defined match
    def match(self, search):
        #print '### match:',
        #term = search.query()
        #print term
        #if term.toLower() == 'hello':
        #  match = Plasma.QueryMatch(self.runner)
        #  match.setType(Plasma.QueryMatch.ExactMatch)
        #  match.setIcon(KIcon('text-x-python'))
        #  match.setText(i18n('Hello World!'))
        #  match.setData(self.url)
        #  search.addMatch(term, match)

    #   run method
    #   Runs the runner
    def run(self, search, action):
        #print '### run:',
        #data = action.data().toString()
        #print data
        #KToolInvocation.invokeBrowser(data)

    #   prepare method
    #   Sets up the runner
    def prepare(self):
        #print '### prepare',
        #lock = QMutexLocker(self.bigLock())
        #print 'in BIG lock'
        
    #   teardown method
    #   
    def teardown(self):
        print '### teardown'
        
    #   createRunOption method
    #   Loads an UI to configure the runner options
    def createRunOptions(self, parent):
        #print '### createRunOptions', parent
        #uic.loadUi(self.package().filePath('ui', 'config.ui'), parent)   
        
    #   reloadConfiguration method
    #   
    def reloadConfiguration(self):
        #print '### reloadConfiguration: ',
        #self.url = unicode(self.config().readEntry('url', 'http://www.kde.org').toString())
        #print self.url

    #   CreateRunner method
    #   Note: do NOT modify it, needed by Plasma
def CreateRunner(parent):
    return $RUNNER_NAME(parent)

