#  -*- coding: iso-8859-1 -*-
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
# Th module must be imported as global in the SALOME python console to
# provide the user with documentation on fields operations.
# ===================================================================
#

class UserGuide:
    """
    In this python console, you manipulate variables that identifies
    field objects. These variables are proxy objects towards the real
    MED fields stored as MEDCoupling instances in the SALOME
    container.

    What you can do with this proxy object:

    - you can manipulate this object in algebric operations (+,-,*,/):
    >>> add = f1 + f2
    >>> dif = f1 - f2
    >>> mul = f1 * f2
    >>> div = f1 / f2 (an exception is raised in case of division by 0)
    >>> square = pow(f1, 2)

    - the operande could be a scalar numerical value:
    >>> offset  = f1 + 4.2
    >>> scaling = 3.8 * f1
    
    - any combination of this operations can be done on a single instruction:
    >>> x = f1 + pow(f2)
    >>> y = f1 + x - f2

    - you can request some helper functions on this object:
    >>> doc      : print the user documentation for field operations
    >>> print f  : print the main properties of the field f (name, values, ...)
    >>> f.dup()  : create a dublicate of the field f
    >>> dup(f)   : do the same thing
    >>> f.visu() : display the modulus of this field using VISU post-processor.    
    """

    def __repr__(self):
        return UserGuide.__doc__

    def __call__(self, command):
        print "help on command %s"%command

doc=UserGuide()
