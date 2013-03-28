#!/usr/bin/env python
#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2011-2013  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-i", "--imagefile", dest="imagefile",
                  help="image file to convert", metavar="FILE")
parser.add_option("-m", "--medfile", dest="medfile", default=None,
                  help="output med file", metavar="FILE")
(options, args) = parser.parse_args()

import sys, os
if options.imagefile is None:
    print "The image file must be specified"
    sys.exit()
imagefile = options.imagefile
if not os.path.exists(imagefile):
    print "The image file %s does not exists"%imagefile
    sys.exit()

if options.medfile is None:
    basename = os.path.basename(imagefile)
    medfile = basename[0:len(basename)-3] + "med"
else:
    medfile = options.medfile

print "Convert image file %s to a med field saved in %s"%(imagefile,medfile)
from xmedimages import FieldBuilder
builder = FieldBuilder()    
builder.image2med(imagefile,medfile)
