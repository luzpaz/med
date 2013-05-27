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
from CaseIO import CaseIO
import sys,re

class CaseReader(CaseIO):
    """ Converting a file in the Case format (Ensight) to the MED format.
    A new file with the same base name and the .med extension is created.
    """

    @classmethod
    def New(cls,fileName):
        """ Static constructor. """
        return CaseReader(fileName)
        pass

    def __init__(self,fileName):
        """ Constructor """
        self._fileName=fileName
        pass

    def __traduceMesh(self,name,typ,coords,cells):
        """ Convert a CASE mesh into a MEDCouplingUMesh. """
        nbCoords=len(coords)
        coo=np.array(coords,dtype="float64") ; coo=coo.reshape(nbCoords,3)
        coo=DataArrayDouble(coo) ; coo=coo.fromNoInterlace()
        ct=self.dictMCTyp2[typ]
        m=MEDCouplingUMesh(name,MEDCouplingUMesh.GetDimensionOfGeometricType(ct))
        m.setCoords(coo)
        nbNodesPerCell=MEDCouplingMesh.GetNumberOfNodesOfGeometricType(ct)
        cI=DataArrayInt(len(cells)+1) ; cI.iota() ; cI*=nbNodesPerCell+1
        #
        cells2=cells.reshape(len(cells),nbNodesPerCell)
        c2=DataArrayInt(cells2)
        c=DataArrayInt(len(cells),nbNodesPerCell+1) ; c[:,0]=ct ; c[:,1:]=c2-1 ; c.rearrange(1)
        m.setConnectivity(c,cI,True)
        m.checkCoherency2()
        return m

    def __convertGeo2MED(self,geoFileName):
        """ Convert all the geometry (all the meshes) contained in teh CASE file into MEDCouplingUMesh'es. """
        fd=open(geoFileName,"r+b") ; fd.seek(0,2) ; end=fd.tell() ; fd.seek(0) ; fd.readline() ; fd.readline()
        name=fd.readline().strip() ; fd.readline() ; fd.readline()
        pos=fd.tell()
        mcmeshes=[]
        while pos!=end:
            if fd.read(4)!="part":
                raise Exception("Error on reading mesh #1 !")
            fd.seek(fd.tell()+80)
            meshName=fd.read(80).strip()
            print meshName
            if fd.read(len("coordinates"))!="coordinates":
                raise Exception("Error on reading mesh #2 !")
            pos=fd.tell()
            typeOfCoo=np.memmap(fd,dtype='byte',mode='r',offset=pos,shape=(1)).tolist()[0]
            pos+=1+17*4
            nbNodes=np.memmap(fd,dtype='int32',mode='r',offset=pos,shape=(1,)).tolist()[0]
            pos+=4
            coo=np.memmap(fd,dtype='float32',mode='r',offset=pos,shape=(nbNodes,3))
            pos+=nbNodes*3*4 ; fd.seek(pos)#np.array(0,dtype='float%i'%(typeOfCoo)).nbytes
            typ=fd.read(80).strip() ; pos=fd.tell()
            nbNodesPerCell=MEDCouplingMesh.GetNumberOfNodesOfGeometricType(self.dictMCTyp2[typ])
            nbCellsOfType=np.memmap(fd,dtype='int32',mode='r',offset=pos,shape=(1,)).tolist()[0]
            pos+=4
            cells=np.memmap(fd,dtype='int32',mode='r',offset=pos,shape=(nbCellsOfType,nbNodesPerCell))
            pos+=nbCellsOfType*nbNodesPerCell*4
            fd.seek(pos)
            mcmeshes.append(self.__traduceMesh(meshName,typ,coo,cells))
            pass
        #
        ms=MEDFileMeshes()
        ms.resize(len(mcmeshes))
        for i,m in enumerate(mcmeshes):
            mlm=MEDFileUMesh()
            mlm.setMeshAtLevel(0,m)
            ms.setMeshAtPos(i,mlm)
            pass
        return mcmeshes,ms
    
    def __convertField(self,mlfields, mcmeshes, fileName, fieldName, discr, nbCompo, locId, it):
        """ Convert the fields. """
        stars=re.search("[\*]+",fileName).group()
        st="%0"+str(len(stars))+"i"
        trueFileName=fileName.replace(stars,st%(it))
        fd=open(trueFileName,"r+b") ; fd.seek(0,2) ; end=fd.tell() ; fd.seek(0)
        name=fd.readline().strip().split(" ")[0]
        print name
        if name!=fieldName:
            raise Exception("ConvertField : mismatch")
        pos=fd.tell()
        while pos!=end:
            st=fd.read(4)
            if st!="part":
                raise Exception("ConvertField : mismatch #2")
            pos=fd.tell()+76 ; fd.seek(pos)
            meshId=np.memmap(fd,dtype='int32',mode='r',offset=pos,shape=(1)).tolist()[0]-1
            fd.seek(pos+4)
            nbOfValues=mcmeshes[meshId].getNumberOfCells()
            typ=fd.read(80).strip() ; pos=fd.tell()
            vals=np.memmap(fd,dtype='float32',mode='r',offset=pos,shape=(nbOfValues,nbCompo))#np.memmap(fd,dtype='int32',mode='r',offset=159,shape=(1))
            vals2=DataArrayDouble(np.array(vals,dtype='float64'))
            pos+=nbOfValues*nbCompo*4 ; fd.seek(pos)
            f=MEDCouplingFieldDouble(self.discSpatial2[discr],ONE_TIME) ; f.setName("%s_%s"%(fieldName,mcmeshes[meshId].getName()))
            f.setMesh(mcmeshes[meshId]) ; f.setArray(vals2) ; f.setTime(float(it),it,-1)
            f.checkCoherency()
            mlfields[locId+meshId].appendFieldNoProfileSBT(f)
            pass
        pass
    
    def loadInMEDFileDS(self):
        """ Load a CASE file into a MEDFileData object. """
        f=file(self._fileName)
        lines=f.readlines()
        ind=lines.index("GEOMETRY\n")
        if ind==-1:
            raise Exception("Error with file %s"%(fname))
        geoName=re.match("model:([\W]*)([\w\.]+)",lines[ind+1]).group(2)
        m1,m2=self.__convertGeo2MED(geoName)
        ind=lines.index("VARIABLE\n")
        fieldsInfo=[]
        for i in xrange(ind+1,lines.index("TIME\n")):
            m=re.match("^([\w]+)[\s]+\per[\s]+([\w]+)[\s]*\:[\s]*([\w]+)[\s]+([\S]+)$",lines[i])
            if m:
                spatialDisc=m.groups()[1] ; fieldName=m.groups()[2] ; nbOfCompo=self.dictCompo2[m.groups()[0]] ; fieldFileName=m.groups()[3]
                fieldsInfo.append((fieldName,spatialDisc,nbOfCompo,fieldFileName))
                pass
            pass
        
        expr=re.compile("number[\s]+of[\s]+steps[\s]*\:[\s]*([\d]+)")
        nbOfTimeSteps=int(expr.search(filter(expr.search,lines)[0]).group(1))
        
        expr=re.compile("filename[\s]+start[\s]+number[\s]*\:[\s]*([\d]+)")
        startIt=int(expr.search(filter(expr.search,lines)[0]).group(1))
        
        expr=re.compile("filename[\s]+increment[\s]*\:[\s]*([\d]+)")
        incrIt=int(expr.search(filter(expr.search,lines)[0]).group(1))
        
        curIt=startIt
        mlfields=MEDFileFields()
        mlfields.resize(len(fieldsInfo)*len(m1))
        i=0
        for field in fieldsInfo:
            for m in m1:
                mlfields.setFieldAtPos(i,MEDFileFieldMultiTS())
                i+=1
                pass
            pass
        for ts in xrange(nbOfTimeSteps):
            i=0
            for field in fieldsInfo:
                self.__convertField(mlfields,m1,field[3],field[0],field[1],field[2],i,curIt)
                i+=len(m1)
                pass
            curIt+=incrIt
            pass
        ret=MEDFileData()
        ret.setMeshes(m2)
        ret.setFields(mlfields)
        return ret

    pass
