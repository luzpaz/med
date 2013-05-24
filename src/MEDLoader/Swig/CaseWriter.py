#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2013  CEA/DEN, EDF R&D
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
# Author Anthony GEAY (CEA/DEN/DM2S/STMF/LGLS)

import numpy as np
from MEDLoader import *
import sys,re,os,mmap

dictMCTyp={NORM_HEXA8:"hexa8",NORM_POLYHED:"nfaced",NORM_QUAD4:"quad4",NORM_POLYGON:"nsided",NORM_POINT1:"point",NORM_SEG2:"bar2",NORM_SEG3:"bar3",NORM_TRI3:"tria3",NORM_TRI6:"tria6",NORM_QUAD8:"quad8",NORM_TETRA4:"tetra4",NORM_TETRA10:"tetra10",NORM_PYRA5:"pyramid5",NORM_PYRA13:"pyramid13",NORM_PENTA6:"penta6",NORM_PENTA15:"penta15",NORM_HEXA20:"hexa20"}
discSpatial={ON_CELLS:"element",ON_NODES:"node"}
dictCompo={1:"scalar",3:"vector",6:"tensor",9:"tensor9"}

### www-vis.lbl.gov/NERSC/Software/ensight/doc/OnlineHelp/UM-C11.pdf

class CaseWriter:
    """ Converting MED file format in memory to a the Case file format (Ensight).
    A new file with the same base name and the .case extension is created with its depencies (.geo ...).
    """
    
    header="""FORMAT
type: ensight gold
GEOMETRY
model: %(geofilewithoutpath)s
"""
    header_varpart="""VARIABLE"""
    header_timepart="""TIME
time set:               1
number of steps:        %(NbTimeSteps)i
filename start number:  0
filename increment:     1
time values:
%(TimeValues)s
"""
    
    @classmethod
    def New(cls):
        """ Static constructor. """
        return CaseWriter()
        pass

    def __init__(self):
        """ Constructor """
        pass

    def setMEDFileDS(self,medData):
        """ Input should be MEDFileData instance  """
        self._med_data=medData
        pass

    def write(self,fileName):
        """ Write into the specified fileName series the result """
        self._file_name=fileName
        self._base_name_without_dir=os.path.splitext(os.path.basename(self._file_name))[0]
        self._l=self._file_name.split(os.path.sep) ; self._l[-1]=os.path.splitext(self._l[-1])[0] ; self._base_name_with_dir=os.path.sep.join(self._l)
        self._dico={"geofilewithoutpath":"%s.geo"%self._base_name_without_dir}
        h0=self.header%self._dico
        self.__writeMeshesPart(self._med_data.getMeshes())
        h2=self.__writeFieldsPart(self._med_data.getFields())
        fheader=open(self._file_name,"w") ; fheader.write((h0+h2)%self._dico)
        pass
    
    def __writeMeshesPart(self,mdms):
        meshfn="%s.geo"%(self._base_name_with_dir,)
        try:
            os.remove(meshfn)
        except:
            pass
        f=open(meshfn,"w+b")
        sz=5*80
        ms1=[]
        for mdm in mdms:
            assert(isinstance(mdm,MEDFileUMesh))
            ms2=[[mdm.getMeshAtLevel(lev) for lev in mdm.getNonEmptyLevels()[:1]]]
            for grpnm in mdm.getGroupsNames():
                ms3=[]
                for lev in mdm.getGrpNonEmptyLevels(grpnm)[:1]:
                    ms3.append(mdm.getGroup(lev,grpnm))
                    pass
                ms2.append(ms3)
                pass
            for ms in ms2:
                nn=ms[0].getNumberOfNodes()
                sz+=self.__computeSizeOfGeoFile(ms,nn)
                pass
            ms1.append(ms2)
            pass
        a=np.memmap(f,dtype='byte',mode='w+',offset=0,shape=(sz,)) ; a.flush() # truncate to set the size of the file
        mm=mmap.mmap(f.fileno(),offset=0,length=0)
        mm.write(self.__str80("C Binary"))
        mm.write(self.__str80("Exported from MEDCoupling/MEDLoader SALOME version %s"%(MEDCouplingVersionStr())))
        mm.write(self.__str80("Conversion using CaseWriter class"))
        mm.write(self.__str80("node id off"))
        mm.write(self.__str80("element id off"))
        for ms2 in ms1:
            for iii,ms in enumerate(ms2):
                nn=ms[0].getNumberOfNodes()
                mm.write(self.__str80("part"))
                a=np.memmap(f,dtype='int32',mode='w+',offset=mm.tell(),shape=(1,))
                a[0]=iii+1 ; a.flush() ; mm.seek(mm.tell()+4) # part number maybe to change ?
                name=ms[0].getName()
                if iii>0:
                    name="%s_%s"%(ms2[0][0].getName(),name)
                    pass
                mm.write(self.__str80(name))
                mm.write(self.__str80("coordinates"))
                a=np.memmap(f,dtype='int32',mode='w+',offset=mm.tell(),shape=(1,))
                a[0]=nn ; a.flush() # number of nodes
                mm.seek(mm.tell()+4)
                coo=ms[0].getCoords()
                spaceDim=coo.getNumberOfComponents()
                if spaceDim!=3:
                    coo=coo.changeNbOfComponents(3,0.)
                    pass
                a=np.memmap(f,dtype='float32',mode='w+',offset=mm.tell(),shape=(3,nn))
                c=coo.toNoInterlace() ; c.rearrange(1) ; cnp=c.toNumPyArray() ; cnp=cnp.reshape(3,nn)
                a[:]=cnp ; a.flush() ; mm.seek(mm.tell()+3*nn*4)
                for m in ms:
                    i=0
                    for typ2,nbelem,dummy in m.getDistributionOfTypes():
                        typ=typ2
                        if typ not in dictMCTyp:
                            typ=MEDCouplingMesh.GetCorrespondingPolyType(typ)
                            pass
                        mp=m[i:i+nbelem]
                        mm.write(self.__str80(dictMCTyp[typ]))
                        a=np.memmap(f,dtype='int32',mode='w+',offset=mm.tell(),shape=(1,))
                        a[0]=nbelem ; a.flush() ; mm.seek(mm.tell()+4)
                        if typ!=NORM_POLYHED and typ!=NORM_POLYGON:
                            nbNodesPerElem=MEDCouplingMesh.GetNumberOfNodesOfGeometricType(typ)
                            c=mp.getNodalConnectivity() ; c.rearrange(nbNodesPerElem+1) ; c=c[:,1:] ; c.rearrange(1) ; c+=1
                            a=np.memmap(f,dtype='int32',mode='w+',offset=mm.tell(),shape=(nbNodesPerElem*nbelem,))
                            a[:]=c.toNumPyArray() ; a.flush() ; mm.seek(mm.tell()+nbNodesPerElem*nbelem*4)
                            pass
                        elif typ==NORM_POLYHED:
                            mp.orientCorrectlyPolyhedrons()
                            c=mp.computeNbOfFacesPerCell()
                            a=np.memmap(f,dtype='int32',mode='w+',offset=mm.tell(),shape=(nbelem,))
                            a[:]=c.toNumPyArray(); a.flush() ; mm.seek(mm.tell()+nbelem*4)
                            c=mp.getNodalConnectivity()[:] ; c.pushBackSilent(-1) ; c[mp.getNodalConnectivityIndex()[:-1]]=-1 ; ids=c.getIdsEqual(-1) ; nbOfNodesPerFace=ids.deltaShiftIndex()-1
                            a=np.memmap(f,dtype='int32',mode='w+',offset=mm.tell(),shape=(len(nbOfNodesPerFace),))
                            a[:]=nbOfNodesPerFace.toNumPyArray() ; a.flush() ; mm.seek(mm.tell()+len(nbOfNodesPerFace)*4)
                            ids2=ids.buildComplement(ids.back()+1)
                            c2=mp.getNodalConnectivity()[ids2]+1
                            a=np.memmap(f,dtype='int32',mode='w+',offset=mm.tell(),shape=(len(c2),))
                            a[:]=c2.toNumPyArray() ; a.flush() ; mm.seek(mm.tell()+len(c2)*4)
                            pass
                        else:
                            nbOfNodesPerCell=mp.getNodalConnectivityIndex().deltaShiftIndex()-1
                            a=np.memmap(f,dtype='int32',mode='w+',offset=mm.tell(),shape=(len(nbOfNodesPerCell),))
                            a[:]=nbOfNodesPerCell.toNumPyArray() ; a.flush() ; mm.seek(mm.tell()+len(nbOfNodesPerCell)*4)
                            ids2=mp.getNodalConnectivityIndex().buildComplement(mp.getNodalConnectivityIndex().back()+1)
                            c2=mp.getNodalConnectivity()[ids2]+1
                            a=np.memmap(f,dtype='int32',mode='w+',offset=mm.tell(),shape=(len(c2),))
                            a[:]=c2.toNumPyArray() ; a.flush() ; mm.seek(mm.tell()+len(c2)*4)
                            pass
                        i+=nbelem
                        pass
                    pass
                pass
            pass
        pass
    
    def __writeFieldsPart(self,mdfs):
        self._ze_top_dict={}
        its,areForgottenTS=mdfs.getCommonIterations()
        if areForgottenTS:
            print "WARNING : some iterations are NOT present in all fields ! Kept iterations are : %s !"%(str(its))
            pass
        TimeValues=""
        for it in its:
            TimeValues+="%s\n"%(str(mdfs[0][it].getTime()[-1]))
            pass
        dictVars={}
        for mdf in mdfs:
            nbCompo=mdf.getNumberOfComponents()
            if nbCompo not in dictCompo:
                print "Field \"%s\" will be ignored because number of components (%i) is not in %s supported by case files !"%(mdf.getName(),nbCompo,str(dictCompo.keys()))
                pass
            if nbCompo in dictVars:
                dictVars[nbCompo].append(mdf)
                pass
            else:
                dictVars[nbCompo]=[mdf]
                pass
            pass
        for mdf in mdfs:
            nbCompo=mdf.getNumberOfComponents()
            if nbCompo not in dictCompo:
                continue;
            for iii,it in enumerate(its):
                ff=mdf[it]
                for typ in ff.getTypesOfFieldAvailable():
                    l=self._l[:] ; l[-1]="%s%s.%s_%s"%(self._base_name_without_dir,str(iii).rjust(4,"0"),ff.getName(),MEDCouplingFieldDiscretization.New(typ).getStringRepr())
                    fffn=l[-1]
                    try:
                        os.remove(os.path.sep.join(l))
                    except:
                        pass
                    f=open(os.path.sep.join(l),"w+b")
                    summ=0
                    for geo,[(curTyp,(bg,end),pfl,loc)] in ff.getFieldSplitedByType():
                        if typ==curTyp:
                            summ+=4*nbCompo*(end-bg)+80
                            pass
                        pass
                    a=np.memmap(f,dtype='byte',mode='w+',offset=0,shape=(2*80+4+summ,)) ; a.flush() # truncate to set the size of the file
                    mm=mmap.mmap(f.fileno(),offset=0,length=0)
                    k1="%s_%s"%(ff.getName(),MEDCouplingFieldDiscretization.New(typ).getStringRepr())
                    mm.write(self.__str80(k1))
                    mm.write(self.__str80("part"))
                    a=np.memmap(f,dtype='int32',mode='w+',offset=mm.tell(),shape=(1,))
                    a[0]=1 ; a.flush() ; mm.seek(mm.tell()+4) # part number maybe to change ?
                    for geo,[(curTyp,(bg,end),pfl,loc)] in ff.getFieldSplitedByType():
                        if typ==curTyp:
                            arr=ff.getUndergroundDataArray()[bg:end].toNoInterlace()
                            if typ==ON_CELLS:
                                mm.write(self.__str80(dictMCTyp[geo]))
                                pass
                            elif typ==ON_NODES:
                                mm.write(self.__str80("coordinates"))
                                pass
                            else:
                                print "UnManaged type of field for field \"%s\" !"%(mdf.getName())
                                pass
                            a=np.memmap(f,dtype='float32',mode='w+',offset=mm.tell(),shape=(nbCompo,end-bg))
                            a[:]=arr.toNumPyArray() ; a.flush() ; mm.seek(mm.tell()+nbCompo*(end-bg)*4)
                            pass
                        pass
                    k="%s per %s"%(dictCompo[nbCompo],discSpatial[typ])
                    if k in self._ze_top_dict:
                        if k1 in self._ze_top_dict[k]:
                            self._ze_top_dict[k][k1].append(fffn)
                            pass
                        else:
                            self._ze_top_dict[k][k1]=[fffn]
                            pass
                    else:
                        self._ze_top_dict[k]={k1:[fffn]}
                        pass
                    pass
                pass
            pass
        headerPart=""
        if len(self._ze_top_dict)!=0:
            hvp=self.header_varpart[:]
            for k in self._ze_top_dict:
                for k1 in self._ze_top_dict[k]:
                    hvp+="\n%s: %s %s"%(k,k1,re.sub("([\d]{4})",4*"*",self._ze_top_dict[k][k1][0]))
                    pass
                pass
            hvp+="\n"
            headerPart+=hvp
            #
            ddd={"NbTimeSteps":len(its),"TimeValues":TimeValues}
            htp=self.header_timepart%ddd
            headerPart+=htp
            pass
        return headerPart
    
    @classmethod
    def __str80(cls,st):
        if len(st)>79:
            raise Exception("String \"%s\" is too long (>79) !"%(st))
        return st.ljust(79)+"\n"
    
    def __computeSizeOfGeoFile(self,listOfMeshes,nn):
        sz=0
        for m in listOfMeshes:
            distribTypes=m.getDistributionOfTypes()
            sz+=80+4+2*80+4+nn*3*4
            i=0
            for typ2,nbelem,dummy in distribTypes:
                typ=typ2
                if typ not in dictMCTyp:
                    typ=MEDCouplingMesh.GetCorrespondingPolyType()
                    pass
                if typ!=NORM_POLYHED and typ!=NORM_POLYGON:
                    sz+=80+4+MEDCouplingMesh.GetNumberOfNodesOfGeometricType(typ)*nbelem*4
                    pass
                elif typ==NORM_POLYHED:
                    mplh=m[i:i+nbelem] ; delta=len(mplh.getNodalConnectivity())+nbelem
                    sz+=80+4+delta*4
                    pass
                else:
                    mplh=m[i:i+nbelem] ; delta=len(mplh.getNodalConnectivity())
                    sz+=80+4+delta*4
                    pass
                i+=nbelem
                pass
            pass
        return sz
