#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

############################################################################
# This file provides utilities to dump content of MEDMEM objects: mesh,
# field, group, family, nodal connectivity, node coordinates.
############################################################################
#
from libMEDMEM_Swig import *

import os

theEntityName = { MED_CELL        :"CELL",
                  MED_FACE        :"FACE",
                  MED_EDGE        :"EDGE",
                  MED_NODE        :"NODE",
                  MED_ALL_ENTITIES:"ALL_ENTITIES" }

theTypeName = {MED_NONE        :"NONE",
               MED_POINT1      :"POINT1",
               MED_SEG2        :"SEG2",
               MED_SEG3        :"SEG3",
               MED_TRIA3       :"TRIA3",
               MED_QUAD4       :"QUAD4",
               MED_TRIA6       :"TRIA6",
               MED_QUAD8       :"QUAD8",
               MED_TETRA4      :"TETRA4",
               MED_PYRA5       :"PYRA5",
               MED_PENTA6      :"PENTA6",
               MED_HEXA8       :"HEXA8",
               MED_TETRA10     :"TETRA10",
               MED_PYRA13      :"PYRA13",
               MED_PENTA15     :"PENTA15",
               MED_HEXA20      :"HEXA20",
               MED_POLYGON     :"POLYGON",
               MED_POLYHEDRA   :"POLYHEDRA",
               MED_ALL_ELEMENTS:"ALL_ELEMENTS"}

medModeSwitch = { 0:"FULL_INTERLACE",
                  1:"NO_INTERLACE",
                  3:"UNDEFINED_INTERLACE" };

med_type_champ = { 6 : "REEL64",
                   24: "INT32",
                   26: "INT64",
                   0 : "UNDEFINED_TYPE" } ;

tab="   "

debugShowConn=True
debugShowConn=False

SHOW_ALL = -1

# private
def _showNodalConnectivity(mesh,entity,type,elems,tablevel,showOnly=SHOW_ALL):
    if debugShowConn: print "ELEMENTS:",elems
    tab1 = tab*tablevel
    tab2 = tab*(tablevel+1)
    typeName = theTypeName[type]
    nbElem = len( elems )
    if showOnly > 0:
        elems = elems[:showOnly]
    nbShow = len( elems )
    connectivity = mesh.getConnectivity(MED_NODAL,entity,MED_ALL_ELEMENTS)
    index = mesh.getConnectivityIndex(MED_NODAL,entity)
    if debugShowConn: print "CONN:",connectivity,"\nIND:",index
    elemShift = 0
    types = mesh.getTypes( entity )
    for t in types:
        if t != type:
            elemShift += mesh.getNumberOfElements(entity,t)
        else:
            break
        pass
    for i in elems:
        elem = i + elemShift
        print tab1,typeName,i,":",connectivity[index[elem-1]-1 : index[elem]-1]
        pass
    nbSkip = nbElem - nbShow
    if nbSkip > 0:
        print tab1,"...",nbSkip,"elements not shown"
        pass
    pass

#private
def _showSupport(support, tablevel,showElems=0):
    tab1 = tab*(tablevel+0)
    tab2 = tab*(tablevel+1)
    tab3 = tab*(tablevel+3)
    entity    = support.getEntity()
    types     = support.getTypes()
    nbOfTypes = support.getNumberOfTypes()
    onAll     = support.isOnAllElements()
    print tab1,"-Entity:",theEntityName[entity]
    print tab1,"-Types :",types
    print tab1,"-Elements"
    if onAll:
        print tab2,"<< Is on all elements >>"
    else:
        for type in types:
            nbOfElmtsOfType = support.getNumberOfElements(type)
            number = support.getNumber(type)
            number.sort()
            print tab2,"* Type:",theTypeName[type]
            print     tab3,". Nb elements:",nbOfElmtsOfType
            print     tab3,". Numbers:",number[:min(100,nbOfElmtsOfType)],
            if nbOfElmtsOfType > 100:
                print "...skip", nbOfElmtsOfType-100
            else:
                print
                pass
            if entity != MED_NODE and showElems:
                print tab3,". Nodal connectivity"
                _showNodalConnectivity(support.getMesh(),entity,type,number,tablevel+4,showElems)
                pass
            pass
        pass
    print
    return

## Dump i-th family of given entity in mesh.
## Optionally dump nodal connectivity of <showElems> first elements.
## Use showElems=SHOW_ALL to dump connectivity of all elements.

def ShowFamily(mesh, entity, i, showElems=0):
    family = mesh.getFamily(entity,i)
    familyName = family.getName()
    familyDescription = family.getDescription()
    entity = family.getEntity()
    familyBool = family.isOnAllElements()
    print "\nFAMILY", i, "on", theEntityName[entity]
    print tab*1,"-Name       :",familyName
    print tab*1,"-Description:",familyDescription
    familyIdentifier = family.getIdentifier()
    nbOfAtt = family.getNumberOfAttributes()
    print tab*1,"-Identifier :",familyIdentifier
    print tab*1,"-Attributes"
    attributesids = family.getAttributesIdentifiers()
    attributesvals = family.getAttributesValues()
    for k in range(nbOfAtt):
        print tab*2,"* Id         :",attributesids[k]
        print tab*2,"* Value      :",attributesvals[k]
        print tab*2,"* Description:",family.getAttributeDescription(k+1)
        pass
    nbOfGrp = family.getNumberOfGroups()
    print tab*1,"-Nb Of Groups:",nbOfGrp
    print tab*1,"-Groups:"
    for k in range(nbOfGrp):
        print tab*2,k+1,":",family.getGroupName(k+1)
        pass
    _showSupport(family,1,showElems)
    return

## Dump all families in mesh.
## Optionally dump nodal connectivity of <showElems> first elements of each family.
## Use showElems=SHOW_ALL to dump connectivity of all elements.

def ShowFamilies(mesh, showElems=0):
    line = "families in mesh <" + mesh.getName() + ">"
    print "\n",line,"\n","-"*len(line)
    for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
        nbFam = mesh.getNumberOfFamilies(entity)
        for i in range(nbFam):
            ShowFamily(mesh,entity,i+1,showElems)
        pass
    print

## Dump a GROUP.
## Optionally dump nodal connectivity of <showElems> first elements of the group.
## Use showElems=SHOW_ALL to dump connectivity of all elements.

def ShowGroup(group, showElems):
    groupName = group.getName()
    groupDescription = group.getDescription()
    nbOfFam = group.getNumberOfFamilies()
    print "\nGROUP on",theEntityName[group.getEntity()]
    print tab*1,"-Name          :",groupName
    print tab*1,"-Description   :",groupDescription
    print tab*1,"-Nb Of Families:",nbOfFam
    print tab*1,"-Families"
    for k in range(nbOfFam):
        print tab*2,k+1,":",group.getFamily(k+1).getName()
        pass
    _showSupport(group,1,showElems)
    return

## Dump all GROUP's in mesh.
## Optionally dump nodal connectivity of <showElems> first elements of each group.
## Use showElems=SHOW_ALL to dump connectivity of all elements.

def ShowGroups(mesh, showElems=0):
    line = "groups in mesh <" + mesh.getName() + ">"
    print "\n",line,"\n","-"*len(line)
    for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
        nbGrp = mesh.getNumberOfGroups(entity)
        if nbGrp > 0:
            for j in range(nbGrp):
                group = mesh.getGroup(entity,j+1)
                ShowGroup(group,showElems)
            pass
        pass
    pass

## Dump mesh general information.
## Optionally dump node coordinates of first <nodes2Show> nodes.
## <entity2Show> gives number of elements to dump nodal connectivity by
## entities: [<nb CELLs to show>, <nb FACEs>, <nb EDGEs> ].
## Use SHOW_ALL to dump all elements or node coordinates.

def ShowMesh(mesh, nodes2Show=0, entity2Show=[0,0,0]):
    print "---------------------- MESH -------------------------"
    meshName   = mesh.getName()
    spaceDim   = mesh.getSpaceDimension()
    meshDim    = mesh.getMeshDimension()
    print "The mesh <%s> is a %dD mesh on a %dD geometry" % (meshName,meshDim,spaceDim)
    nbNodes    = mesh.getNumberOfNodes()
    print "There are",nbNodes,"MED_NODE's"
    coordSyst  = mesh.getCoordinatesSystem()
    print "The Coordinates :"
    coordNames = []
    coordUnits = []
    for isd in range(spaceDim):
        coordNames.append(mesh.getCoordinateName(isd))
        coordUnits.append(mesh.getCoordinateUnit(isd))
        pass
    print tab,"system:",coordSyst
    print tab,"names:", coordNames
    print tab,"units:", coordUnits
    ## coordinates
    if nodes2Show:
        print tab,"values:"
        coordinates = mesh.getCoordinates(MED_FULL_INTERLACE)
        nbCoord = nodes2Show
        maxNbCoord = len( coordinates ) / spaceDim
        if nbCoord < 0: nbCoord = maxNbCoord
        else:           nbCoord = min( nbCoord, maxNbCoord )
        for k in range( nbCoord ):
            n = k*spaceDim
            print tab*2,k+1,coordinates[n:n+spaceDim]
            pass
        if nbCoord < maxNbCoord: print tab*2,"... %d nodes skipped" % (maxNbCoord-nbCoord)
        pass
    # elem types
    print "The Elements :"
    i = -1
    for entity in [MED_CELL,MED_FACE,MED_EDGE]:
        i += 1
        entityName = theEntityName[ entity ]
        if mesh.getNumberOfElements(entity,MED_ALL_ELEMENTS) < 1: continue
        nbTypes = mesh.getNumberOfTypes( entity )
        try:
            types = mesh.getTypes( entity )
        except:
            continue
        print tab,"%s types:" % entityName
        for type in types:
            nbElemType = mesh.getNumberOfElements(entity,type)
            print tab*2,"%s: \t %d elements" % ( theTypeName[ type ], nbElemType )
            pass
        # nodal connectivity
        if i >= len( entity2Show ): break
        if not entity2Show[ i ]: continue
        print tab,"%s nodal connectivity:" % entityName
        for type in types:
            typeName = theTypeName[ type ]
            nbElemType = mesh.getNumberOfElements(entity,type)
            if nbElemType == 0:
                continue
            d = 1
            number = range (d, nbElemType+d)
            _showNodalConnectivity(mesh,entity,type,number,2,entity2Show[ i ])
            pass
        pass

    print "----------------------Groups, Families-------------------------"
    nbF = 0
    nbG = 0
    for entity in [MED_NODE,MED_CELL,MED_FACE,MED_EDGE]:
        nbFam = mesh.getNumberOfFamilies(entity)
        nbGrp = mesh.getNumberOfGroups(entity)
        nbElem= mesh.getNumberOfElements(entity, MED_ALL_ELEMENTS);
        nbF += nbFam
        nbG += nbGrp
        if (entity == MED_NODE) :
            if (nbFam > 0) : print "This mesh has",nbFam,"Node Family(ies)"
            if (nbGrp > 0) : print "This mesh has",nbGrp,"Node Group(s)"
            if (nbElem> 0) : print "This mesh has",nbElem,"Node Element(s)"
            pass
        elif (entity == MED_CELL) :
            if (nbFam > 0) : print "This mesh has",nbFam,"Cell Family(ies)"
            if (nbGrp > 0) : print "This mesh has",nbGrp,"Cell Group(s)"
            if (nbElem> 0) : print "This mesh has",nbElem,"Cell Element(s)"
            pass
        elif (entity == MED_FACE) :
            if (nbFam > 0) : print "This mesh has",nbFam,"Face Family(ies)"
            if (nbGrp > 0) : print "This mesh has",nbGrp,"Face Group(s)"
            if (nbElem> 0) : print "This mesh has",nbElem,"Face Element(s)"
            pass
        elif (entity == MED_EDGE) :
            if (nbFam > 0) : print "This mesh has",nbFam,"Edge Family(ies)"
            if (nbGrp > 0) : print "This mesh has",nbGrp,"Edge Group(s)"
            if (nbElem> 0) : print "This mesh has",nbElem,"Edge Element(s)"
            pass
        pass
    print "Total nbF", nbF,"nbG",nbG
    return

## Dump all FIELD's in MED.
## Optionally dump <showValues> first values.
## Use showValues=SHOW_ALL to dump all values.

def ShowFields( fields, showValues=0 ):
    nbFields = len(fields)
    print "---------------------- Fields-------------------------"
    print "Nb fields", nbFields
    for (iField, f ) in enumerate( fields ):
        sup       = f.getSupport()
        name      = f.getName()
        desc      = f.getDescription()
        itnb      = f.getIterationNumber()
        time      = f.getTime()
        order     = f.getOrderNumber()
        ftype     = f.getValueType()
        mode      = f.getInterlacingType()
        nbcomp    = f.getNumberOfComponents()
        nbval     = f.getNumberOfValues()
        nbelem    = sup.getNumberOfElements(MED_ALL_ELEMENTS)
        nbOfTypes = sup.getNumberOfTypes()
        types     = sup.getTypes()
        isOnAll   = sup.isOnAllElements()
        print '\nFIELD',iField
        print tab*1,'-Name             : "%s"' % name
        print tab*1,'-Description      : "%s"' % desc
        print tab*1,'-IterationNumber  :  %s' % itnb
        print tab*1,'-Time             :  %s' % time
        print tab*1,'-OrderNumber      :  %s' % order
        print tab*1,'-Nb Values        :  %s' % nbval
        print tab*1,'-Nb Supp. Elements:  %s' % nbelem
        print tab*1,'-Nb Componenets   :  %s' % nbcomp
        print tab*1,'-ValueType        :  %s' % med_type_champ[ftype]
        print tab*1,'-Interlace        :  %s' % medModeSwitch[mode]
        print tab*1,'-Conponents'
        for k in range(nbcomp):
            kp1 = k+1
            compName = f.getComponentName(kp1)
            compDesc = f.getComponentDescription(kp1)
            compUnit = f.getMEDComponentUnit(kp1)
            print     tab*2,kp1,'*Name       : "%s"' % compName
            try:
                print tab*2,'  *Description: "%s"' % compDesc
            except:
                print 'INVALID'
                pass
            try:
                print tab*2,'  *Unit       : "%s"' % compUnit
            except:
                print 'INVALID'
                pass
            pass
        print tab*1,'-MESH             : "%s"' % sup.getMeshName()
        print tab*1,'-SUPPORT          : "%s"' % sup.getName()
        print tab*1,'-On all elements  :  %s' % bool(isOnAll)
        print tab*1,'-Types            :  %s'  % types

        if ftype == MED_REEL64:
            if mode == MED_FULL_INTERLACE:
                f = createFieldDoubleFromField(f)
            else:
                f = createFieldDoubleNoInterlaceFromField( f )
                pass
            pass
        elif ftype == MED_INT32:
            if mode == MED_FULL_INTERLACE:
                f = createFieldIntFromField(f)
            else:
                f = createFieldIntNoInterlaceFromField( f )
                pass
            pass
        else:
            print tab*1,'<< Unknown field type >>:',ftype
            continue
        nbGauss = 1
        hasGauss = False
        if nbcomp == 0:
            nbGauss = 0
        else:
            hasGauss = f.getGaussPresence()
            pass
        if hasGauss:
            nbGaussByType = f.getNumberOfGaussPoints()
            pass
        for k in range(nbOfTypes):
            type = types[k]
            nbOfElmtsOfType = sup.getNumberOfElements(type)
            if hasGauss: nbGauss = nbGaussByType[ k ]
            if type == 0: type = MED_POINT1
            print tab*2,k+1,theTypeName[type],':',nbOfElmtsOfType, 'elements,',\
                  nbGauss,'gauss point(s)'
            pass
        nbOf = sup.getNumberOfElements(MED_ALL_ELEMENTS)
        elements = []
        if not isOnAll:
            elements = sup.getNumber(MED_ALL_ELEMENTS)
            pass
        if nbcomp == 0:
            nbOf = 0
            print tab*1,'-Nb Values        :',nbOf
            #value = f.getValue(MED_FULL_INTERLACE)
            #print value[0: min( 100, len(value)-1 )]

        toShow = min( nbOf, showValues )
        if toShow < 0: toShow = nbOf
        for I in range( toShow ):
            if elements:
                i = elements[ I ]
            else:
                i = I+1
            if mode == MED_FULL_INTERLACE:
                valueI = f.getRow(i)
            else:
                valueI = []
                for j in range( nbcomp ):
                    for k in range( f.getNbGaussI( i ) ):
                        valueI.append( f.getValueIJK(i,j+1,k+1) )
            print '         ',i,' - ',valueI #[:nbcomp]
            pass
        if nbOf > toShow:
            print '            ...skip',nbOf - toShow,'values'
            pass
        pass
    pass

## Read all fields in MED

def ReadFields(med):
    nbFields = med.getNumberOfFields()
    if (nbFields>0):
        print 'READ FIELDs ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++'
        med.updateSupport()
        for i in range(nbFields):
            field_name = med.getFieldName(i)
            nbOfIt = med.getFieldNumberOfIteration(field_name)
            print '  The field is',field_name,'with',nbOfIt,'iteration(s)'
            for j in range(nbOfIt):
                dtitfield = med.getFieldIteration(field_name,j)
                dt = dtitfield.getdt()
                it = dtitfield.getit()
                field = med.getField(field_name,dt,it)
                type = field.getValueType()
                print '     * Iteration:',dt,'Order number:',it,'Type:',type
                mode = field.getInterlacingType()
                if type == MED_INT32:
                    if mode == MED_FULL_INTERLACE:
                        fieldint = createFieldIntFromField(field)
                    else:
                        fieldint = createFieldIntNoInterlaceFromField( field )
                    print '     Reading',fieldint.getName(),'...'
                    fieldint.read()
                elif type == MED_REEL64:
                    if mode == MED_FULL_INTERLACE:
                        f = createFieldDoubleFromField(field)
                    else:
                        f = createFieldDoubleNoInterlaceFromField( field )
                    print '     Reading',f.getName(),'...'
                    f.read()
                else:
                    print '  !!!! Bad type of Field !!!!'

# Remove a file if it exists

def supprimer(f):
    if os.access(f, os.F_OK):
        os.remove(f)

# Remove a file if it exists

def deleteFile( f ):
    supprimer( f )
