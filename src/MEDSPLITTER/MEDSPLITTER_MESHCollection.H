// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef __MEDSPLITTER_MESHCOLLECTION_H__
#define __MEDSPLITTER_MESHCOLLECTION_H__

/*! Projects an integer or double field onto a new mesh
 * 
 * \param old_collection initial mesh collection supporting the field
 * \param fieldname name of the field to be cast
 * \param itnumber time iteration number of the field to be cast
 * \param ordernumber inner iteration number of the field to be cast
 * \param type_of_template dummy argument passed for separate template instanciation
 */

template <class T>
void MESHCollection::castFields (const MESHCollection& old_collection,
                                 const string& fieldname, int itnumber, int ordernumber)
{
  char fieldchar[80];
  strcpy(fieldchar,fieldname.c_str());

  int nb_old_domains=old_collection._topology->nbDomain();
  int nb_new_domains=_topology->nbDomain();

  vector <list<int> > element_array  (_topology->nbDomain());

  vector <MEDMEM::FIELD<T> *> old_fields ;

  //cout << "MEDSPLITTER -  reading fields from old collection"<<endl;
  old_collection.getDriver()->readFields(old_fields,fieldchar, itnumber, ordernumber);

  //if (dynamic_cast<MEDMEM::FIELD<int>*> (old_fields[0])==0)
  //  old_collection.getDriver()->readFieldsDouble(old_fields,fieldchar, itnumber, ordernumber);
  //else
  //  old_collection.getDriver()->readFieldsInt(old_fields,fieldchar, itnumber, ordernumber);
  //cout <<"MEDSPLITTER - end of read"<<endl;

  vector <const MEDMEM::SUPPORT*> old_supports(nb_old_domains);
  vector <MEDMEM::SUPPORT*> new_supports(nb_new_domains);
  vector <MEDMEM::FIELD<T> *> new_fields(nb_new_domains);

  for (int iold = 0; iold < nb_old_domains; iold++)
  {
    old_supports[iold]=old_fields[iold]->getSupport();
  }
  for (int inew = 0; inew < nb_new_domains; inew++)
  {
    new_supports[inew]=new MEDMEM::SUPPORT();
  }

  //cout << "MEDSPLITTER - casting supports"<<endl;
  castSupport(old_collection,old_supports,new_supports);
  //cout << "MEDSPLITTER - end of cast"<<endl;

  int nb_components = old_fields[0]->getNumberOfComponents();
  const string* components_names = old_fields[0]->getComponentsNames();
  const string* components_description = old_fields[0]->getComponentsDescriptions();
  const string* components_units = old_fields[0]->getMEDComponentsUnits();
  if (itnumber != old_fields[0]->getIterationNumber()) {cout << "PB with iteration number"<<endl;exit (1);}
  int iteration_number=old_fields[0]->getIterationNumber();
  int order_number=old_fields[0]->getOrderNumber();
  double time=old_fields[0]->getTime();
  bool has_gauss_pts = old_fields[0]->getGaussPresence();
  //bool has_gauss_pts=true;

  // Creating the fields objects 
  // Two different procedures are used whether the field contains Gauss points
  // or not

  vector <MEDMEM::MEDMEM_Array<T, FullInterlaceGaussPolicy>*> medarray (nb_new_domains);

  for (int inew=0; inew < nb_new_domains; inew++)
  {
    if (!has_gauss_pts) {
      new_fields[inew] = new MEDMEM::FIELD<T>(new_supports[inew],nb_components);
      new_fields[inew]->setName(fieldname);
      new_fields[inew]->setComponentsNames(components_names);
      new_fields[inew]->setComponentsDescriptions(components_description);
      new_fields[inew]->setMEDComponentsUnits(components_units);
      new_fields[inew]->setIterationNumber(iteration_number);
      new_fields[inew]->setOrderNumber(order_number);
      new_fields[inew]->setTime(time);
    }
    if (has_gauss_pts)
    {
      new_fields[inew]=new MEDMEM::FIELD<T>();
      //copying the structures describing the field
      new_fields[inew]->setNumberOfComponents(nb_components);
      new_fields[inew]->setSupport(new_supports[inew]);
      new_fields[inew]->setName(fieldname);
      new_fields[inew]->setComponentsNames(components_names);
      new_fields[inew]->setComponentsDescriptions(components_description);
      new_fields[inew]->setMEDComponentsUnits(components_units);
      new_fields[inew]->setIterationNumber(iteration_number);
      new_fields[inew]->setOrderNumber(order_number);
      new_fields[inew]->setTime(time);

      //counters for the gauss points
      //nbtypegeo is the number of geometric types on the field
      //nbelgeoc is the count of element for each type
      //nbgaussgeo is the number of gauss points for each type
      int nbtypegeo=0;
      int* nbelgeoc=0;
      int* nbgaussgeo=0;

      vector<int> gauss_pts_number;
      vector<int> nb_elem;

      // the GaussLocalization structures are browsed in the old field
      // and copied to the new one
      // the nbtypegeo counter is incremented so that 
      // it contains the number of types for which a gauss localization is defined
      MED_EN::MESH_ENTITIES::const_iterator currentEntity;
      std::list<MED_EN::medGeometryElement>::const_iterator iter;
      currentEntity  = MED_EN::meshEntities.find(MED_EN::MED_CELL);
      for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)        
      {
        // getGaussLocalization throws an exception 
        // if the GaussLocalization is not available
        try
        {
          MEDMEM::GAUSS_LOCALIZATION<FullInterlace> 
            gaussloc(old_fields[0]->getGaussLocalization(*iter));
          new_fields[inew]->setGaussLocalization(*iter,gaussloc);

          nbtypegeo++;
          gauss_pts_number.push_back(gaussloc.getNbGauss());

          nb_elem.push_back(new_supports[inew]->getNumberOfElements(*iter));
        }
        catch(...)
        {
          continue;
        }
      }

      // les tableaux nbelgeoc commencent a 1
      nbelgeoc = new int [nbtypegeo+1];
      nbgaussgeo= new int [nbtypegeo+1];
      int size=0;
      nbelgeoc[0]=0;
      nbgaussgeo[0]=-1;
      for (int i=1; i<=nbtypegeo;i++)
      {
        size+=nb_elem[i-1];
        nbelgeoc [i]=nb_elem[i-1]+nbelgeoc[i-1];
        nbgaussgeo[i]=gauss_pts_number[i-1];    
      }

      //a MEDMEM_Array structure is created to contain 
      //the data on the Gauss points

      medarray[inew]=new MEDMEM::MEDMEM_Array<T,FullInterlaceGaussPolicy>
        (new_fields[inew]->getNumberOfComponents(), size, nbtypegeo,
         static_cast<const int* const>(nbelgeoc),
         static_cast<const int* const> (nbgaussgeo));
      new_fields[inew]->setArray(medarray[inew]);
      //                        delete[] nbelgeoc;
      //                        delete[] nbgaussgeo;
    }
  }

  for (int idomain=0; idomain < old_collection._topology->nbDomain(); idomain++)
  {
    //retrieves the group igroup on domain idomain

    const MEDMEM::SUPPORT* support = old_supports[idomain];
    int nbelem = support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
    if (nbelem==0) continue;
    int* list_of_elems=0;

    if (support->isOnAllElements())
    {
      list_of_elems = new int[nbelem];
      for (int i=0; i<nbelem;i++)
        list_of_elems[i]=i+1;
    }
    else
      list_of_elems = const_cast<int*> (support->getNumber(MED_EN::MED_ALL_ELEMENTS));

    int* array=new int[nbelem];
    int* ip=0;
    int* local=0;
    int* initial_local=0;
    int* full_array=0;

    int size=0;
    MED_EN::medEntityMesh entity = support->getEntity();
    switch (entity)
    {
    case MED_EN::MED_CELL :
      ip=new int[nbelem];
      local=new int[nbelem];
      initial_local=list_of_elems;
      old_collection.getTopology()->convertCellToGlobal(idomain,list_of_elems,nbelem,array);
      _topology->convertGlobalCellList(array,nbelem,local,ip);
      size=nbelem;
      break;
    case MED_EN::MED_FACE :
      old_collection.getTopology()->convertFaceToGlobal(idomain,list_of_elems,nbelem,array);
      _topology->convertGlobalFaceListWithTwins(array,nbelem,local,ip,full_array,size);
      initial_local=new int[size];
      old_collection.getTopology()->convertGlobalFaceList(full_array,size,initial_local,idomain);
      delete[] full_array;
      break;
    case MED_EN::MED_NODE :
      old_collection.getTopology()->convertNodeToGlobal(idomain,list_of_elems,nbelem,array);
      _topology->convertGlobalNodeListWithTwins(array,nbelem,local,ip,full_array,size);
      initial_local=new int[size];
      old_collection.getTopology()->convertGlobalNodeList(full_array,size,initial_local,idomain);
      delete[] full_array;
      break;
    }

    if (!has_gauss_pts) {
      for (int i=0; i<size; i++)
      {
        for (int j=0; j<nb_components;j++)
        {
          T value = old_fields[idomain]->getValueIJ(initial_local[i],j+1);

          new_fields[ip[i]]->setValueIJ(local[i],j+1, value);
        }
      }
    }
    else
    {
      for (int i=0; i<size; i++)
      {
        MED_EN::medGeometryElement type =
          old_collection._mesh[idomain]->getElementType(entity,initial_local[i]);
        int nb_gauss_points=old_fields[idomain]->getNumberOfGaussPoints(type);
        for (int j=0; j<nb_components;j++)
          for (int k=0;k<nb_gauss_points; k++)
          {
            T value = old_fields[idomain]->getValueIJK(initial_local[i],j+1,k+1);
            medarray[ip[i]]->setIJK(local[i],j+1,k+1, value);
          }     
      }
    }   
    delete[]array;
    delete[]ip;
    delete[]local;
    if (support->isOnAllElements()) 
    {
      delete[] list_of_elems;
      list_of_elems=0;
    }
    if (entity==MED_EN::MED_FACE || entity==MED_EN::MED_NODE) delete[] initial_local;
  }

  retrieveDriver()->writeFields(new_fields,fieldchar);
  //if (dynamic_cast<MEDMEM::FIELD<int>*>(new_fields[0])==0)
  //  retrieveDriver()->writeFieldsDouble(new_fields,fieldchar);
  //else
  //  retrieveDriver()->writeFieldsInt(new_fields,fieldchar);
  for (int i=0; i<nb_new_domains; i++)
  {
    new_fields[i]->removeReference();
    new_supports[i]->removeReference();
    //delete medarray[i];
  }
  for (unsigned i=0; i<old_fields.size(); i++)
  {
    cout << "old field deletion" <<endl;
    old_fields[i]->removeReference();
    //old_supports[i]->removeReference();//as we did not addReference() neither created it
  }
}

#endif /*MEDSPLITTER_MESHCOLLECTION_H_*/
