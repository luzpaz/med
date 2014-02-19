# -*- coding: iso-8859-1 -*-
# Copyright (C) 2011-2014  CEA/DEN, EDF R&D
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
# medop.py is a tools to initialize a python console outside of the
# GUI for field operations. A salome session must be running
# first. For example:
# $ ./runAppli -t
# $ ./runSession
# $ python -i path/to/medop.py
# ===================================================================
#

import xmed
# Set the globals dictionnary so that the fields tools work properly.
xmed.setConsoleGlobals(globals())
# Import the tools required for field operations
from xmed import load, get, put, dup, ls, la, save, view, doc, wipe
