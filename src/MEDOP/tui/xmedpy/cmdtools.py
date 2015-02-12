#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2011-2015  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author : Guillaume Boulant (EDF) 

#
# ===================================================================
# The functions of this module must be imported as global functions in
# the SALOME python console. It provides the console context with
# general purpose python commands.
# ===================================================================
#
# add something to clear the screen
class Cls(object):
    """
    This class can be used to create a command that clear the shell
    screen running this python interpreter.
    """
    def __repr__(self):
        import os
        os.system('cls' if os.name == 'nt' else 'clear')
        return ''

cls = Cls()

class Prompt:
    """
    This class can be used to create a special prompt
    """
    def __str__(self):
        import os
        return 'medop> '

import sys
sys.ps1 = Prompt()
del sys
del Prompt

class Wipe(object):
    def __repr__(self):
        return '>>>\n'*40

wipe = Wipe()

