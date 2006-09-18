// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
/*************************************************************************
* COPYRIGHT (C) 1999 - 2003  EDF R&D
* THIS LIBRARY IS FREE SOFTWARE; YOU CAN REDISTRIBUTE IT AND/OR MODIFY
* IT UNDER THE TERMS OF THE GNU LESSER GENERAL PUBLIC LICENSE 
* AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION; 
* EITHER VERSION 2.1 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
*  
* THIS LIBRARY IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL, BUT
* WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF
* MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. SEE THE GNU
* LESSER GENERAL PUBLIC LICENSE FOR MORE DETAILS.
*
* YOU SHOULD HAVE RECEIVED A COPY OF THE GNU LESSER GENERAL PUBLIC LICENSE
* ALONG WITH THIS LIBRARY; IF NOT, WRITE TO THE FREE SOFTWARE FOUNDATION,
* INC., 59 TEMPLE PLACE, SUITE 330, BOSTON, MA 02111-1307 USA
*
*************************************************************************/
#include "MEDMEM_medimport_src.hxx"
#include "hdf5_version2api.hxx"

using namespace med_2_2;
using namespace MEDMEM;

namespace med_2_2 {
extern "C" {
  extern med_err MEDgaussInfo(med_idt fid, int indice, char * locname,
			      med_geometrie_element * type_geo,
			      med_int * ngauss );

  extern void _MEDmodeErreurVerrouiller(void);

  extern med_err _MEDnomGeometrie(char *nom_geo,
				  med_geometrie_element type_geo);
}
}

med_err med_2_2::OLD_MEDattrNumLire(med_idt pere, med_type_champ type,
				    char *nom, unsigned char *val,
				    hid_t hdf_file)
{
  med_idt attid;
  med_err ret;
  int type_hdf;

  if ((attid = H5Aopen_name(pere,nom)) < 0)
    return -1;

  switch(type) 
    {
    case MED_FLOAT64 :
#if defined(PCLINUX) || defined(OSF1)
/*       type_hdf = H5T_IEEE_F64BE; */
      if (H5Tequal(hdf_file,H5T_IEEE_F64BE))
	type_hdf = H5T_IEEE_F64LE;
      if (H5Tequal(hdf_file,H5T_IEEE_F64LE))
	type_hdf = H5T_IEEE_F64BE;
#else 
/*       type_hdf = H5T_IEEE_F64LE; */
      if (H5Tequal(hdf_file,H5T_IEEE_F64BE))
	type_hdf = H5T_IEEE_F64LE;
      if (H5Tequal(hdf_file,H5T_IEEE_F64LE))
	type_hdf = H5T_IEEE_F64BE;
#endif
      break;
      
    case MED_INT :
#if defined(IRIX64) || defined(OSF1)
      type_hdf = H5T_NATIVE_LONG; 
#else
      type_hdf = H5T_NATIVE_INT;
#endif
      break;
      
    default :
      return -1;
    }

  if ((ret = H5Aread(attid,type_hdf,val)) < 0)
    return -1;

  if ((ret = H5Aclose(attid)) < 0)
    return -1;

  return 0;
}


med_err med_2_2::OLD_MEDdatasetNumLire(med_idt pere, char *nom,
				       med_type_champ type,
				       med_mode_switch interlace,
				       med_size nbdim, med_size fixdim, 
				       med_size psize, med_ssize * pfltab,
				       med_int ngauss,
				       unsigned char *val,hid_t hdf_file)
{
  med_idt    dataset, dataspace = 0, memspace = 0;

#ifdef HDF_NEW_API
  med_size  start_mem[1],start_data[1],*pflmem=0,*pfldsk=0;
#else 
  med_ssize  start_mem[1],start_data[1],*pflmem=0,*pfldsk=0;
#endif

  med_size   stride[1],count[1],pcount[1],size[1],pflsize[1];
  med_err    ret;
  int        i,j,index,type_hdf;
  hid_t      datatype;
  size_t     typesize;
  int        dim, firstdim, dimutil, lastdim;
  med_mode_profil pflmod;

  /* Verify fixdim is between [0, nbdim] ( 0 is MED_ALL ) */
  if ( ( fixdim < 0 ) || ( fixdim > nbdim ) ) 
    return -1;
 
  /* block pflmod to MED_COMPACT (until med2.2) */
  pflmod = MED_COMPACT;

  switch(type)
    {
    case MED_FLOAT64 :
      /* 1) IA32 is LE but due to an (?HDF convertion BUG?) when using H5T_NATIVE_DOUBLE/MED_REEL64? under PCLINUX
	 the file read under SGI is incorrect
	 2) Compaq OSF/1 is LE, since we force SGI64,SUN4SOL2,HP to write double in LE even if they are BE, mips OSF/1 must be BE
	 REM  : Be careful of compatibility between MED files when changing this (med2.2)                    */
#if defined(PCLINUX) || defined(OSF1)
/*       type_hdf = H5T_IEEE_F64BE; */
      if (H5Tequal(hdf_file,H5T_IEEE_F64BE))
	type_hdf = H5T_IEEE_F64LE;
      if (H5Tequal(hdf_file,H5T_IEEE_F64LE))
	type_hdf = H5T_IEEE_F64BE;
#else 
/*       type_hdf = H5T_IEEE_F64LE; */
      if (H5Tequal(hdf_file,H5T_IEEE_F64BE))
	type_hdf = H5T_IEEE_F64LE;
      if (H5Tequal(hdf_file,H5T_IEEE_F64LE))
	type_hdf = H5T_IEEE_F64BE;
#endif
      break;

    case MED_INT32 :
      type_hdf = H5T_NATIVE_INT;
      break;

    case MED_INT64 :
      type_hdf = H5T_NATIVE_LONG;
      break;

    default :
      return -1;
    }

  /* Ouverture du Dataset à lire */
  if ((dataset = H5Dopen(pere,nom)) < 0)
    return -1;

  /* Interrogation de la taille du dataset */
  if ( (datatype  = H5Dget_type(dataset )) < 0) return -1;
  if ( (typesize  = H5Tget_size(datatype)) < 0) return -1;
  size[0] = H5Dget_storage_size(dataset) / typesize; 
  if ( H5Tclose(datatype) < 0) return -1;

  /* Create dataspace */
  if ((dataspace = H5Screate_simple(1,size,NULL)) < 0)
    return -1;
  
  switch(interlace)
    {
    case MED_FULL_INTERLACE :

      /*Initialisation des indices de boucle du traitement de l'entrelacement en fonction de la dimension fixee*/
      if ( fixdim != MED_ALL) 
	{ 
	  firstdim = (int)fixdim-1;
	  lastdim  = (int)fixdim;
	  dimutil  = 1;
	} else	{
	  firstdim = 0;
	  lastdim = (int)nbdim;
	  dimutil  = (int)nbdim; 
	}

      count [0] = (*size)/(nbdim);
      

      /*rem: Pas de vérification de l'assertion (*size)=n*nbdim */
      if ( psize == MED_NOPF ) {  

      /* Creation d'un data space mémoire de dimension 1, de longeur size, et de longeur maxi size */
      if ( (memspace = H5Screate_simple (1, size, NULL)) <0)
	return -1;

	stride[0] = nbdim;  

	for (dim=firstdim; dim < lastdim; dim++) {
	  	  
	  start_mem[0] = dim;
	  if ( (ret = H5Sselect_hyperslab (memspace, H5S_SELECT_SET, start_mem, stride, 
					   count, NULL)) <0)
	    return -1; 
	  
	  start_data[0] = dim*count[0];
	  if ( (ret = H5Sselect_hyperslab (dataspace, H5S_SELECT_SET, start_data, NULL, 
					   count, NULL)) <0)
	    return -1; 
	  
	  if ((ret = H5Dread(dataset,type_hdf,memspace,dataspace,
			     H5P_DEFAULT, val)) < 0)
	    return -1;
	}
	
      } else {

	pflsize [0] = psize*ngauss*nbdim;
	pcount  [0] = psize*ngauss*dimutil;

#ifdef HDF_NEW_API
        pflmem     = (med_size *) malloc (sizeof(med_size)*pcount[0]);
	pfldsk     = (med_size *) malloc (sizeof(med_size)*pcount[0]);
#else
	pflmem     = (med_ssize *) malloc (sizeof(med_ssize)*pcount[0]);
	pfldsk     = (med_ssize *) malloc (sizeof(med_ssize)*pcount[0]);
#endif

	switch(pflmod)
	  { /* switch pflmod pour FULL_INTERLACE*/
	  case MED_GLOBAL :

	    /* Creation d'un data space mémoire de dimension 1, de longeur size, et de longeur maxi size */
	    if ( (memspace = H5Screate_simple (1, size, NULL)) <0)
	      return -1;

	    for (dim=firstdim; dim < lastdim; dim++) {
	      
	      for (i=0; i < psize; i++)              /* i balaye les élements du profil */
		for (j=0; j < ngauss; j++) {         
		  index = i*ngauss+j + (dim-firstdim)*((int)psize*ngauss);
		  pflmem[index] = (pfltab[i]-1)*ngauss*nbdim + j*nbdim+dim;
		  pfldsk[index] = dim*count[0] + (pfltab[i]-1)*ngauss+j;	     
		}
	    }
	    
#ifdef HDF_NEW_API
	    if ( (ret = H5Sselect_elements(memspace ,H5S_SELECT_SET, pcount[0], (const hsize_t **) pflmem ) ) <0) 
	      return -1; 
	    
	    if ( (ret = H5Sselect_elements(dataspace,H5S_SELECT_SET, pcount[0], (const hsize_t **) pfldsk ) ) <0) 
	      return -1; 
#else
	    if ( (ret = H5Sselect_elements(memspace ,H5S_SELECT_SET, pcount[0], (const hssize_t **) pflmem ) ) <0) 
	      return -1; 
	    
	    if ( (ret = H5Sselect_elements(dataspace,H5S_SELECT_SET, pcount[0], (const hssize_t **) pfldsk ) ) <0) 
	      return -1;
#endif
	    break;
	
	  case MED_COMPACT :
	
	    /* Creation d'un data space mémoire de dimension 1, de la longeur du profil          */
	    /* La dimension utilisée est ici nbdim, même pour un profil compact on suppose       */
	    /*  que l'utilisateur a toutes les coordonées stockées, même si il en demande qu'une */ 
	    
	    if ( (memspace = H5Screate_simple (1, pflsize, NULL)) <0)
	      return -1;
	    
	    for (dim=firstdim; dim < lastdim; dim++) {
	      
	      for (i=0; i < psize; i++)              /* i balaye les élements du profil */
		for (j=0; j < ngauss; j++) {         
		  index = i*ngauss+j + (dim-firstdim)*((int)psize*ngauss);
		  pflmem[index] = i*ngauss*nbdim + j*nbdim+dim;
		  pfldsk[index] = dim*count[0] + (pfltab[i]-1)*ngauss+j;	     
		}	      
	    }

#ifdef HDF_NEW_API	    
	    if ( (ret = H5Sselect_elements(memspace ,H5S_SELECT_SET, pcount[0], (const hsize_t **) pflmem ) ) <0) 
	      return -1; 
	    
	    if ( (ret = H5Sselect_elements(dataspace,H5S_SELECT_SET, pcount[0], (const hsize_t **) pfldsk ) ) <0) 
	      return -1; 
#else
	    if ( (ret = H5Sselect_elements(memspace ,H5S_SELECT_SET, pcount[0], (const hssize_t **) pflmem ) ) <0) 
	      return -1; 
	    
	    if ( (ret = H5Sselect_elements(dataspace,H5S_SELECT_SET, pcount[0], (const hssize_t **) pfldsk ) ) <0) 
	      return -1;
#endif
	    
	    break;

	  default :
	    return -1; 
	  }
	
	if ((ret = H5Dread(dataset,type_hdf,memspace,dataspace,H5P_DEFAULT, val)) < 0)
	  return -1;
	
	free(pflmem);
	free(pfldsk);
      }
      
      break;
      
    case MED_NO_INTERLACE :

      /*Initialisation des indices de boucle du traitement de l'entrelacement en fonction de la dimension fixee*/

      count[0] = (*size)/nbdim;
      
      if ( psize == MED_NOPF ) {  
	
	if ( fixdim != MED_ALL) 
	  start_data[0] = (fixdim-1)*count[0];
	else {
	  count[0] = *size;
	  start_data[0] =  0;
	};
	
	if ( (ret = H5Sselect_hyperslab (dataspace, H5S_SELECT_SET, start_data, NULL, 
					 count, NULL)) <0)
	  return -1; 
	
	if ((ret = H5Dread(dataset,type_hdf,dataspace,dataspace,
			   H5P_DEFAULT, val)) < 0)
	  return -1;
	
      } else {

	if ( fixdim != MED_ALL) 
	  { 
	    firstdim = (int)fixdim-1;
	    lastdim  = (int)fixdim;
	    dimutil  = 1;
	  } else	{
	    firstdim = 0;
	    lastdim  = (int)nbdim;
	    dimutil  = (int)nbdim; 
	  }

	pflsize [0] = psize*ngauss*nbdim;	
  	pcount  [0] = psize*ngauss*dimutil; /* nom pas très coherent avec count !!! A revoir */	
#ifdef HDF_NEW_API
	pfldsk      = (med_size *) malloc(sizeof(med_size)*pcount[0]);
#else
	pfldsk      = (med_ssize *) malloc(sizeof(med_ssize)*pcount[0]);
#endif
	
	switch(pflmod)
	  { /*switch plfmod pour NO_INTERLACE */
	  case MED_GLOBAL :
	    
	    for (dim=firstdim; dim < lastdim; dim++) {
	      
	      for (i=0; i < psize; i++)              /* i balaye le nbre d'élements du profil                */
		for (j=0; j < ngauss; j++) { 
		  index = i*ngauss+j + (dim-firstdim)*((int)psize*ngauss);
		  pfldsk[index] = dim*count[0]+(pfltab[i]-1)*ngauss+j;	    
		}
	    }
	    
#ifdef HDF_NEW_API
	    if ( (ret = H5Sselect_elements(dataspace,H5S_SELECT_SET,pcount[0], (const hsize_t **) pfldsk ) ) <0) 
	      return -1;
#else
	    if ( (ret = H5Sselect_elements(dataspace,H5S_SELECT_SET,pcount[0], (const hssize_t **) pfldsk ) ) <0) 
	      return -1;
#endif
	    
	    if ((ret = H5Dread(dataset,type_hdf,dataspace,dataspace,H5P_DEFAULT, val)) < 0)
	      return -1;
	      
	    break;
	    
	  case MED_COMPACT :
	    
	    /* Creation d'un data space mémoire de dimension 1, de la longeur du profil          */
	    /* La dimension utilisée est ici nbdim, même pour un profil compact on suppose       */
	    /*  que l'utilisateur a toutes les coordonées stockées, même si il en demande qu'une */ 

	    if ( (memspace = H5Screate_simple (1, pflsize, NULL)) <0)
	      return -1;

#ifdef HDF_NEW_API
	    pflmem     = (med_size *) malloc (sizeof(med_size)*pcount[0]);
#else
	    pflmem     = (med_ssize *) malloc (sizeof(med_ssize)*pcount[0]);
#endif
	    
	    /* Le profil COMPACT est contigüe, mais il est possible que l'on selectionne uniquemenent une dimension*/

	    for (dim=firstdim; dim < lastdim; dim++) {
	      
	      for (i=0; i < psize; i++)              /* i balaye le nbre d'élements du profil                */
		for (j=0; j < ngauss; j++) {
		  index = i*ngauss+j + (dim-firstdim)*((int)psize*ngauss);
	          pflmem[index] = dim*(psize*ngauss) + (pfltab[i]-1)*ngauss+j;
		  pfldsk[index] = dim*count[0]  + (pfltab[i]-1)*ngauss+j;	    
		}
	    }
	    
#ifdef HDF_NEW_API
	    if ( (ret = H5Sselect_elements(memspace ,H5S_SELECT_SET, pcount[0], (const hsize_t **) pflmem ) ) <0) 
	      return -1; 
	    
	    if ( (ret = H5Sselect_elements(dataspace,H5S_SELECT_SET,pcount[0], (const hsize_t **) pfldsk ) ) <0) 
	      return -1;	  
#else
	    if ( (ret = H5Sselect_elements(memspace ,H5S_SELECT_SET, pcount[0], (const hssize_t **) pflmem ) ) <0) 
	      return -1; 
	    
	    if ( (ret = H5Sselect_elements(dataspace,H5S_SELECT_SET,pcount[0], (const hssize_t **) pfldsk ) ) <0) 
	      return -1;
#endif
	    
	    if ((ret = H5Dread(dataset,type_hdf,memspace,dataspace,H5P_DEFAULT, val)) < 0)
	      return -1;
	    
	    break;
	    
	  default :
	    return -1;	    
	    
	  }
	
	free(pfldsk);
	
      };
      
      break;
      
    default :
      return -1;
    }
  
  

  if (memspace) 
    if ((ret = H5Sclose(memspace)) < 0)
      return -1;

  if ((ret = H5Sclose(dataspace)) < 0)
    return -1;
  
  if ((ret = H5Dclose(dataset)) < 0)
    return -1;      

  return 0;
}


void med_2_2::conversion_chaine(char *ancienne_chaine,
			       char *nouvelle_chaine,
			       med_int n)
{
  int i;
  int j;
  char tmp[MED_TAILLE_PNOM+1];

  for (i=0;i<n;i++) {
    if (i == n-1) {
      strcpy(tmp,ancienne_chaine+i*ANCIEN_MED_TAILLE_PNOM);
      if (strlen(tmp) < ANCIEN_MED_TAILLE_PNOM)
	for(j=strlen(tmp);j<ANCIEN_MED_TAILLE_PNOM;j++) 
	  tmp[j] = ' ';
    }
    else
      strncpy(tmp,ancienne_chaine+i*ANCIEN_MED_TAILLE_PNOM,ANCIEN_MED_TAILLE_PNOM);
    tmp[ANCIEN_MED_TAILLE_PNOM] = '\0';
    /*          12345678 */
    strcat(tmp,"        ");
    /*     printf("[%s]\n",tmp); */
    if (i == 0)
      strcpy(nouvelle_chaine,tmp);
    else
      strcat(nouvelle_chaine,tmp);
  }
  *(nouvelle_chaine+MED_TAILLE_PNOM*n) = '\0';
} 

void med_2_2::MAJ_version(med_idt fid) throw (MEDEXCEPTION)
{
  med_err ret;
  med_idt gid;
  med_int majeur, mineur, release;

  /* On ecrit le bon numero de version */
  majeur = 2;
  mineur = 2;
  release = 1;
  gid = _MEDdatagroupOuvrir(fid,MED_NOM_INFOS);
  if (gid < 0)
    gid = _MEDdatagroupCreer(fid,MED_NOM_INFOS);

//   EXIT_IF(gid < 0,"Ouverture du groupe HDF MED_NOM_INFOS",NULL);

  if (gid < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = _MEDattrEntierEcrire(gid,MED_NOM_MAJEUR,&majeur);

//   EXIT_IF(ret < 0,"Ecriture du numéro majeur",NULL);

  if (ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = _MEDattrEntierEcrire(gid,MED_NOM_MINEUR,&mineur);

//   EXIT_IF(ret < 0,"Ecriture du numéro mineur",NULL);

  if (ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = _MEDattrEntierEcrire(gid,MED_NOM_RELEASE,&release);

//   EXIT_IF(ret < 0,"Ecrriture du numéro de release",NULL);

  if (ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = _MEDdatagroupFermer(gid);

//   EXIT_IF(ret < 0,"Fermeture du groupe HDF MED_NOM_INFOS",NULL);

  if (ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
  
}

void med_2_2::MAJ_noeuds_maillage(med_idt mid, med_int dimension)
  throw (MEDEXCEPTION)
{
  med_idt nid, gid, tid;
  med_float *coo;
  char *nom, *unit;
  char *nouvelle_chaine;
  med_int n;
  med_size dimd[1];
  med_err ret;
  hid_t hdf_type;
  med_repere repere;

  /* Accès aux noeuds du maillage */
  nid = _MEDdatagroupOuvrir(mid,MED_NOM_NOE);

//   EXIT_IF(nid < 0,"Ouverture du groupe HDF MED_NOM_NOE",NULL);

  if(nid < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  /* Lecture du nombre de noeuds */
  gid = _MEDdatasetOuvrir(nid,MED_NOM_COO);
  if (gid > 0) 
     ret = _MEDattrEntierLire(gid,MED_NOM_NBR,&n);

//   EXIT_IF(gid < 0,"Lecture du nombre de noeuds",NULL);

  if(gid < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
  
  /* Mise a jour des coordonnees */
  /* On cree un groupe HDF tampon */
  /* Lecture des coordonnées */
  hdf_type = H5Dget_type(gid);

//   EXIT_IF(hdf_type < 0,"Lecture du type HDF de stockage des coordonnées",NULL);

  if(hdf_type < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  coo = (med_float *) malloc(sizeof(med_float)*n*dimension);

//   EXIT_IF(coo == NULL,NULL,NULL);

  if(coo == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = OLD_MEDdatasetNumLire(nid,MED_NOM_COO,MED_FLOAT64, 
 			      MED_FULL_INTERLACE,dimension,MED_ALL, 
 			      0,NULL,MED_NOPG, 
 			      (unsigned char*) coo,hdf_type); 

//   EXIT_IF(ret < 0,"Lecture des coordonnées des noeuds",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  dimd[0] = n*dimension;
  /*   printf("EGALITE des types : %d \n",H5Tequal(hdf_type,H5T_NATIVE_DOUBLE)); */
  /*   printf("Avant conversion : [%f] [%f] [%f] \n",*(coo),*(coo+1),*(coo+2)); */
  if (! H5Tequal(hdf_type,H5T_NATIVE_DOUBLE)) {
    ret = H5Tconvert(hdf_type,H5T_NATIVE_DOUBLE,(size_t)*dimd,(void *)coo,NULL,0);
    /*   printf("Après conversion éventuelle : [%f] [%f] [%f] \n",*(coo),*(coo+1),*(coo+2)); */

//     EXIT_IF(ret < 0,"Conversion des coordonnées des noeuds",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
  }
  ret = H5Tclose(hdf_type);

//   EXIT_IF(ret < 0,"Fermeture du data type HDF",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  /* On reporte les coordonnées dans le groupe HDF TMP */
  ret = _MEDdatasetNumEcrire(nid,"TMP",MED_FLOAT64,MED_FULL_INTERLACE,
			     dimension,MED_ALL,MED_NOPF,MED_NO_PFLMOD,0,MED_NOPG,dimd,
			     (unsigned char*) coo);

//   EXIT_IF(ret < 0,"Ecriture des coordonnées des noeuds après conversion",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  free(coo);

  /* Ecriture du nombre de noeuds et du type du repertoire dans le dataset TMP */
  tid = _MEDdatasetOuvrir(nid,"TMP");

//   EXIT_IF(tid < 0,"Ouverture du dataset HDF TMP",NULL);

  if(tid < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = _MEDattrEntierEcrire(tid,MED_NOM_NBR,&n);

//   EXIT_IF(ret < 0,"Ecriture du nombre de noeuds dans le dataset HDF TMP",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = _MEDattrEntierLire(gid,MED_NOM_REP,&repere);

//   EXIT_IF(ret < 0,"Lecture du type de repere des coordonnées", NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = _MEDattrEntierEcrire(tid,MED_NOM_REP,&repere);

//   EXIT_IF(ret < 0,"Ecriture du type de répertoire dans le dataset HDF TMP",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  /* Mise a jour des noms et types des unités des coordonnees */
  nom = (char *) malloc(dimension*ANCIEN_MED_TAILLE_PNOM+1);

//   EXIT_IF(nom == NULL,NULL,NULL);

  if(nom == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  unit = (char *) malloc(dimension*ANCIEN_MED_TAILLE_PNOM+1);

//   EXIT_IF(unit == NULL,NULL,NULL);

  if(unit == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  nouvelle_chaine = (char *) malloc(dimension*MED_TAILLE_PNOM+1);

//   EXIT_IF(nouvelle_chaine == NULL,NULL,NULL);

  if(nouvelle_chaine == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = _MEDattrStringLire(gid,MED_NOM_NOM,dimension*ANCIEN_MED_TAILLE_PNOM,nom);

//   EXIT_IF(ret < 0,"Lecture des noms des coordonnées des noeuds",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = _MEDattrStringLire(gid,MED_NOM_UNI,dimension*ANCIEN_MED_TAILLE_PNOM,unit);

//   EXIT_IF(ret < 0,"Lecture des unités des coordonnées des noeuds",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  conversion_chaine(nom,nouvelle_chaine,dimension);
  ret = _MEDattrStringEcrire(tid,MED_NOM_NOM,dimension*MED_TAILLE_PNOM,nouvelle_chaine);

//   EXIT_IF(ret < 0,"Ecriture des nouveaux noms des coordonnées des noeuds",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  conversion_chaine(unit,nouvelle_chaine,dimension);
  ret = _MEDattrStringEcrire(tid,MED_NOM_UNI,dimension*MED_TAILLE_PNOM,nouvelle_chaine);

//   EXIT_IF(ret < 0,"Ecriture des nouvelles unités des coordonnées des noeuds",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  free(nom);
  free(unit);
  free(nouvelle_chaine);

  /* Fermeture de l'accès aux coordonnées */
  ret = _MEDdatasetFermer(gid);

//   EXIT_IF(ret < 0,"Fermeture du dataset HDF MED_NOM_COO",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = H5Gunlink(nid,MED_NOM_COO);

//   EXIT_IF(ret < 0,"Suppression des anciennes coordonnées",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = _MEDdatasetFermer(tid);

//   EXIT_IF(ret < 0,"Fermeture du dataset HDF TMP",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = H5Gmove(nid,"TMP",MED_NOM_COO);

//   EXIT_IF(ret < 0,"Mise en place des nouvelles coordonnées",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  /* Mise a jour des noms des noeuds */
  nom = (char *) malloc(n*ANCIEN_MED_TAILLE_PNOM+1);

//   EXIT_IF(nom == NULL,NULL,NULL);

  if(nom == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  nouvelle_chaine = (char *) malloc(n*MED_TAILLE_PNOM+1);

//   EXIT_IF(nouvelle_chaine == NULL,NULL,NULL);

  if(nouvelle_chaine == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = _MEDdatasetStringLire(nid,MED_NOM_NOM,nom);
  if (ret == 0) {
    conversion_chaine(nom,nouvelle_chaine,n);
    H5Gunlink(nid,MED_NOM_NOM);
    dimd[0] = n*MED_TAILLE_PNOM+1;
    ret = _MEDdatasetStringEcrire(nid,MED_NOM_NOM,dimd,nouvelle_chaine);  

//     EXIT_IF(ret < 0,"Ecriture des nouveaux noms des noeuds",NULL);  

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    gid = _MEDdatasetOuvrir(nid,MED_NOM_NOM);
    ret = _MEDattrEntierEcrire(gid,MED_NOM_NBR,&n);
    ret = _MEDdatasetFermer(gid);
  }
  free(nom);
  free(nouvelle_chaine);

  /* on ferme tout */
  ret = _MEDdatagroupFermer(nid);

//   EXIT_IF(ret < 0,"Fermeture de l'accès aux noeuds",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
}

void med_2_2::MAJ_elements_maillage(med_idt mid, med_int dimension)
  throw (MEDEXCEPTION)
{
  med_idt eid,gid,did,tid;
  med_err ret;
  int i,j;
  med_geometrie_element typmai[MED_NBR_GEOMETRIE_MAILLE] = {MED_POINT1,MED_SEG2, 
							    MED_SEG3,MED_TRIA3,
							    MED_TRIA6,MED_QUAD4,
							    MED_QUAD8,MED_TETRA4,
							    MED_TETRA10,MED_HEXA8,
							    MED_HEXA20,MED_PENTA6,
							    MED_PENTA15,MED_PYRA5,
							    MED_PYRA13};
  int taille, edim;
  char *nom, *nouvelle_chaine;
  char nomgroup[MED_TAILLE_NOM_ENTITE+1];
  med_int n;
  med_size dimd[1];
  med_int *old_conn,*conn;
  
  /* On ne regarde que les mailles et la connectivité nodale */
  eid = _MEDdatagroupOuvrir(mid,MED_NOM_MAI);

//   EXIT_IF(eid < 0,"Ouverture du groupe HDF MED_NOM_MAI",NULL);

  if(eid < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  /* On normalise selon tous les types geometriques */
  for (i=0;i<MED_NBR_GEOMETRIE_MAILLE;i++) {
    
    /* On recupere le nom du groupe HDF */
    _MEDnomGeometrie(nomgroup,typmai[i]);

    /* On accède au type s'il existe dans le fichier */
    gid = _MEDdatagroupOuvrir(eid,nomgroup);
    if (gid < 0)
      continue;

    /* Nombre d'element ? */
    did = _MEDdatasetOuvrir(gid,MED_NOM_NOD);

//     EXIT_IF(did < 0,"Ouverture du dataset HDF MED_NOM_NOD",NULL);

    if(did < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    ret = _MEDattrEntierLire(did,MED_NOM_NBR,&n);

//     EXIT_IF(ret < 0,"Lecture du nombre d'elements",NULL);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    ret = _MEDdatasetFermer(did);

//     EXIT_IF(ret < 0,"Fermeture du dataset HDF MED_NOM_NOD",NULL);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    /* on normalise la connectivité si edim < dimension */
    edim = typmai[i] / 100;
    if (edim < dimension) {
      taille = typmai[i]%100 + 1;
      old_conn = (med_int *) malloc(sizeof(med_int)*taille*n);

//       EXIT_IF(old_conn == NULL,NULL,NULL);

      if(old_conn == NULL)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

#if defined(IRIX64)||defined(OSF1)
      ret = OLD_MEDdatasetNumLire(gid,MED_NOM_NOD,MED_INT64,
				  MED_NO_INTERLACE,(med_size)taille,MED_ALL,
				  0,NULL,MED_NOPG,
				  (unsigned char*) old_conn,H5T_NATIVE_INT);
#else
      ret = OLD_MEDdatasetNumLire(gid,MED_NOM_NOD,MED_INT32,
				  MED_NO_INTERLACE,(med_size) taille,MED_ALL,
				  0,NULL,MED_NOPG,
				  (unsigned char*) old_conn,H5T_NATIVE_INT);
#endif 
      /* On recopie dans le bon tableau */
      taille --;
      conn = (med_int *) malloc(sizeof(med_int)*taille*n);

//       EXIT_IF(conn == NULL,NULL,NULL);

      if(conn == NULL)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

      for (j=0;j<n*taille;j++)
	*(conn+j) = *(old_conn+j);
      dimd[0] = n*taille;
#if defined(IRIX64)||defined(OSF1)
      ret = _MEDdatasetNumEcrire(gid,"TMP",MED_INT64,MED_NO_INTERLACE,
				 taille,MED_ALL,MED_NOPF,MED_NO_PFLMOD,0,MED_NOPG,dimd,
				 (unsigned char*) conn);
#else
      ret = _MEDdatasetNumEcrire(gid,"TMP",MED_INT32,MED_NO_INTERLACE,
				 taille,MED_ALL,MED_NOPF,MED_NO_PFLMOD,0,MED_NOPG,dimd,
				 (unsigned char*) conn);
#endif

//       EXIT_IF(ret < 0,"Ecriture de la nouvelle connectivité des mailles",NULL);

      if(ret < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
      
      /* Ecriture du nombre de mailles dans le dataset HDF TMP */
      tid = _MEDdatasetOuvrir(gid,"TMP");

//       EXIT_IF(tid < 0,"Ouverture du dataset HDF TMP",NULL);

      if(tid < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

      ret = _MEDattrEntierEcrire(tid,MED_NOM_NBR,&n);

//       EXIT_IF(ret < 0,"Ecriture du nombre de noeuds dans le dataset HDF TMP",NULL);

      if(ret < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

      ret = _MEDdatasetFermer(tid);

//       EXIT_IF(ret < 0,"Fermeture du dataset HDF TMP",NULL);

      if(ret < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

      /* Fermeture de l'accès aux connectivites */
      ret = H5Gunlink(gid,MED_NOM_NOD);

//       EXIT_IF(ret < 0,"Suppression des anciennes connectivités",NULL);

      if(ret < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

      ret = H5Gmove(gid,"TMP",MED_NOM_NOD);

//       EXIT_IF(ret < 0,"Mise en place des nouvelles connectivités",NULL);

      if(ret < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

      /* on libere la memoire */
      free(old_conn);
      free(conn);
    }
     
    /* Mise a niveau des noms */
    nom = (char *) malloc(n*ANCIEN_MED_TAILLE_PNOM+1);

//     EXIT_IF(nom == NULL,NULL,NULL);

    if(nom == NULL)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    nouvelle_chaine = (char *) malloc(n*MED_TAILLE_PNOM+1);

//     EXIT_IF(nouvelle_chaine == NULL,NULL,NULL);

    if(nouvelle_chaine == NULL)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    ret = _MEDdatasetStringLire(gid,MED_NOM_NOM,nom);
    if (ret == 0) {
      conversion_chaine(nom,nouvelle_chaine,n);
      H5Gunlink(gid,MED_NOM_NOM);
      dimd[0] = n*MED_TAILLE_PNOM+1;
      ret = _MEDdatasetStringEcrire(gid,MED_NOM_NOM,dimd,nouvelle_chaine);  

//       EXIT_IF(ret < 0,"Ecriture des nouveaux noms des éléments",NULL);  

      if(ret < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

      did = _MEDdatasetOuvrir(gid,MED_NOM_NOM);
      ret = _MEDattrEntierEcrire(did,MED_NOM_NBR,&n);
      ret = _MEDdatasetFermer(did);
    }
    free(nom);
    free(nouvelle_chaine);

    /* on ferme avant de passer au type geometrique suivant */
    ret = _MEDdatagroupFermer(gid);

//     EXIT_IF(ret < 0,"Fermeture de l'accès aux mailles",NULL);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
  }

  /* On ferme tout */
  ret = _MEDdatagroupFermer(eid);

//   EXIT_IF(ret < 0,"Fermeture de l'accès aux mailles",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
}

void med_2_2::MAJ_familles_maillage(med_idt mid) throw (MEDEXCEPTION)
{
  med_idt fid, gid;
  int n;
  med_err ret;
  int i;
  char nom[MED_TAILLE_NOM+1];
  char *noeuds, *elements;
  int nnoeuds = 0;
  int nelements = 0;
  med_int numero;
  char *groupe_noeuds = "NOEUD";
  char *groupe_elements = "ELEME";
  char *nom_famille_zero = "FAMILLE_ZERO";
  char famille0[MED_TAILLE_NOM+1];
  int *index_noeuds, *index_elements;
  char stockage[MED_TAILLE_DESC];

  /* Combien de famille ? */
  fid = _MEDdatagroupOuvrir(mid,"FAS");

//   EXIT_IF(fid < 0,"Ouverture du groupe HDF 'FAS'",NULL);

  if(fid < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  n = 0;
  _MEDnObjets(fid,".",&n);

//   EXIT_IF(n < 0,"Lecture du nombre de famille",NULL);

  if(n < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  noeuds = (char *) malloc(n*MED_TAILLE_NOM+1);

//   EXIT_IF(noeuds == NULL,NULL,NULL);

  if(noeuds == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  index_noeuds = (int *) malloc(sizeof(int)*(n+1));

//   EXIT_IF(index_noeuds == NULL,NULL,NULL);

  if(index_noeuds < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
  
  elements = (char *) malloc(n*MED_TAILLE_NOM+1);

//   EXIT_IF(elements == NULL,NULL,NULL);

  if(elements == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  index_elements = (int *) malloc(sizeof(int)*(n+1));

//   EXIT_IF(index_elements == NULL,NULL,NULL);
  
  if(index_elements == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  /* On recupere les familles une par une et on les
     normalise */
  for (i=0;i<n;i++) {
    ret = _MEDobjetIdentifier(fid,".",i,nom);

//     EXIT_IF(ret < 0,"Identification d'une famille",NULL);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    /* On accede a la famille */
    gid = _MEDdatagroupOuvrir(fid,nom);

//     EXIT_IF(gid < 0,"Ouverture de l'accès à la famille",nom);

    if(gid < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    /* On lit le numero de la famille */
    ret = _MEDattrEntierLire(gid,MED_NOM_NUM,&numero);

//     EXIT_IF(ret < 0,"Lecture du numéro de la famille",nom);
     
    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    /* On ferme l'acces a la famille */
    ret = _MEDdatagroupFermer(gid);

//     EXIT_IF(ret < 0,"Fermeture de l'accès a la famille",nom);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    if (numero == 0) 
      strcpy(famille0,nom);

    if (numero < 0) {
      if (nelements == 0) { 
	*(index_elements) = 0;
	strcpy(elements,nom);
      }
      else
	strcat(elements,nom);
      nelements ++;
      *(index_elements+nelements) = strlen(nom) + *(index_elements+nelements-1);
    }

    if (numero > 0) {
      if (nnoeuds == 0) {
	strcpy(noeuds,nom);
	*(index_noeuds) = 0;
      }
      else
	strcat(noeuds,nom);
      nnoeuds++;
      *(index_noeuds+nnoeuds) = strlen(nom) + *(index_noeuds+nnoeuds-1);
    }
  }

  /* Normalisation de la famille 0 */
  ret = H5Gmove(fid,famille0,nom_famille_zero);

//   EXIT_IF(ret < 0,"Normalisation de la famille ZERO",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  /* On cree les groupes HDF pour les familles de noeuds et d'elements */
  gid = _MEDdatagroupCreer(fid,groupe_noeuds); 

//   EXIT_IF(gid < 0,"Creation du grupe HDF pour les familles de noeuds",NULL); 

  if(gid < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = _MEDdatagroupFermer(gid); 

//   EXIT_IF(ret < 0,"Fermeture du groupe HDF pour les familles de noeuds",NULL); 

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  gid = _MEDdatagroupCreer(fid,groupe_elements); 

//   EXIT_IF(gid < 0,"Creation du groupe HDF pour les familles d'éléments",NULL); 

  if(gid < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  ret = _MEDdatagroupFermer(gid); 

//   EXIT_IF(ret < 0,"Fermeture du groupe HDF pour les familles d'éléments",NULL); 

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  /* On deplace les groupes HDF des familles en fonction de la nature de 
     chaque famille */
  for (i=0;i<nnoeuds;i++) {
    strncpy(nom,noeuds+*(index_noeuds+i),*(index_noeuds+i+1)-*(index_noeuds+i));
    nom[*(index_noeuds+i+1)-*(index_noeuds+i)] = '\0';
    strcpy(stockage,groupe_noeuds);
    strcat(stockage,"/");
    strcat(stockage,nom);
    ret = H5Gmove(fid,nom,stockage); 

//     EXIT_IF(ret < 0,"Normalisation de la famille",nom); 

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
  }

  for (i=0;i<nelements;i++) {
    strncpy(nom,elements+*(index_elements+i),*(index_elements+i+1)-*(index_elements+i));
    nom[*(index_elements+i+1)-*(index_elements+i)] = '\0';
    strcpy(stockage,groupe_elements);
    strcat(stockage,"/"); 
    strcat(stockage,nom);
    ret = H5Gmove(fid,nom,stockage);  

//     EXIT_IF(ret < 0,"Normalisation de la famille",nom);  

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
  }

  /* On libere la memoire */
  free(noeuds);
  free(index_noeuds);
  free(elements);
  free(index_elements);

  /* On ferme tout */
  ret = _MEDdatagroupFermer(fid);

//   EXIT_IF(ret < 0,"Fermeture du groupe HDF 'FAS'",NULL);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
}

void med_2_2::MAJ_maillages(med_idt fid) throw (MEDEXCEPTION)
{
  med_idt gid;
  med_err ret;
  int n,i;
  char nom[MED_TAILLE_NOM+1];
  char chemin[MED_TAILLE_MAA+MED_TAILLE_NOM+1];
  char description[MED_TAILLE_DESC+1] = "Maillage converti au format MED V2.2";
  med_int type = (med_int) MED_NON_STRUCTURE;
  med_int dimension;
  
  /* Lecture du nombre de maillages */
  n = 0;
  _MEDnObjets(fid,MED_MAA,&n);

//   EXIT_IF(n < 0,"Erreur a la lecture du nombre de maillage",NULL);

  if(n < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  /* 
   * Mise a jour des maillages :
   *  - type : MED_NON_STRUCTURE
   *  - description : "Maillage converti au format V2.2"
   */
  for (i=0;i<n;i++) {
    /* on recupere le nom du maillage */
    ret = _MEDobjetIdentifier(fid,MED_MAA,i,nom);

//     EXIT_IF(ret < 0,"Identification d'un maillage",NULL);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

//     fprintf(stdout,"  >>> Normalisation du maillage [%s] \n",nom);

    MESSAGE("med File convertor 2.1 to 2.2 :: >>> Normalisation du maillage [" << nom << "]");

    /* on accede au maillage */
    strcpy(chemin,MED_MAA);
    strcat(chemin,nom);
    gid = _MEDdatagroupOuvrir(fid,chemin); 

//     EXIT_IF(gid < 0,"Accès au maillage",nom);

    if(gid < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    /* lecture de la dimension du maillage */
    ret = _MEDattrEntierLire(gid,MED_NOM_DIM,&dimension);

//     EXIT_IF(ret < 0,"Lecture de la dimension du maillage",nom);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    /* Ecriture du type et de la description */
    ret = _MEDattrStringEcrire(gid,MED_NOM_DES,MED_TAILLE_DESC,description);

//     EXIT_IF(ret < 0,"Ecriture de la description du maillage ",nom);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    ret = _MEDattrEntierEcrire(gid,MED_NOM_TYP,&type);

//     EXIT_IF(ret < 0,"Ecriture de la dimension du maillage ",nom);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
    
    /* Mise a jour des noeuds du maillage */ 
    MAJ_noeuds_maillage(gid,dimension);  

//     fprintf(stdout,"  ... Normalisation des noeuds effectuée ... \n");

    MESSAGE("med File convertor 2.1 to 2.2 ::  ... Normalisation des noeuds effectuée ...");

    /* Mise a jour des éléments du maillage */ 
    MAJ_elements_maillage(gid,dimension);  

//     fprintf(stdout,"  ... Normalisation des éléments effectuée ... \n");

    MESSAGE("med File convertor 2.1 to 2.2 :: ... Normalisation des éléments effectuée ...");

    /* Mise a jour des familles du maillage */
    MAJ_familles_maillage(gid);

//     fprintf(stdout,"  ... Normalisation des familles effectuée ... \n");

    MESSAGE("med File convertor 2.1 to 2.2 :: ... Normalisation des familles effectuée ...");
    
    /* On ferme tout */
    ret = _MEDdatagroupFermer(gid);

//     EXIT_IF(ret < 0,"Fermeture de l'accès au maillage",NULL);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

//     fprintf(stdout,"  >>> Normalisation du maillage [%s] ... OK ... \n",nom);

    MESSAGE("med File convertor 2.1 to 2.2 :: Normalisation du maillage [" << nom << "] ... OK ... ");
  }
}

void med_2_2::MAJ_localisation_Gauss(med_idt fid,char *nom_modele,
				     med_int ngauss)
  throw (MEDEXCEPTION)
{
  med_err ret;
  med_geometrie_element type;
  med_float *refcoo, *gaucoo, *poids;
  char nom_type[4];
  med_int edim, nsom,n,nlu;
  int i;
  char nom[MED_TAILLE_NOM+1];

  /* On regarde si la localisation bidon existe deja 
     Si oui on s'en va */
  n = MEDnGauss(fid);
  if (n > 0)
    for (i=1;i<=n;i++) {
      ret = MEDgaussInfo(fid,i,nom,&type,&nlu);
      if (! strcmp(nom,nom_modele)) return;
    }
  
  /* On deduit le type en fonction de nom_modele */
  strncpy(nom_type,nom_modele,3);
  nom_type[3] = '\0';
  if (! strcmp(nom_type,MED_NOM_SE2))
    type = MED_SEG2;
  if (! strcmp(nom_type,MED_NOM_SE3))
    type = MED_SEG3; 
  if (! strcmp(nom_type,MED_NOM_TR3))
    type = MED_TRIA3; 
  if (! strcmp(nom_type,MED_NOM_TR6))
    type = MED_TRIA6;
  if (! strcmp(nom_type,MED_NOM_QU4))
    type = MED_QUAD4; 
  if (! strcmp(nom_type,MED_NOM_QU8))
    type = MED_QUAD8;
  if (! strcmp(nom_type,MED_NOM_TE4))
    type = MED_TETRA4; 
  if (! strcmp(nom_type,MED_NOM_T10))
    type = MED_TETRA10; 
    if (! strcmp(nom_type,MED_NOM_HE8))
    type = MED_HEXA8; 
  if (! strcmp(nom_type,MED_NOM_H20))
    type = MED_HEXA20;
  if (! strcmp(nom_type,MED_NOM_PE6))
    type = MED_PENTA6; 
  if (! strcmp(nom_type,MED_NOM_P15))
    type = MED_PENTA15;
  if (! strcmp(nom_type,MED_NOM_PY5))
    type = MED_PYRA5; 
  if (! strcmp(nom_type,MED_NOM_P13))
    type = MED_PYRA13;

  /* Dimension de la maille */
  edim = type / 100;
  nsom = type % 100;

  /* On definit des coordonnées et des poids bidon */
  refcoo = (med_float*) malloc(sizeof(med_float)*edim*nsom);

//   EXIT_IF(refcoo == NULL,NULL,NULL);

  if(refcoo == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  for (i=0;i<edim*nsom;i++) *(refcoo+i) = 0.;
  gaucoo = (med_float*) malloc(sizeof(med_float)*edim*ngauss);

//   EXIT_IF(gaucoo == NULL,NULL,NULL);

  if(gaucoo == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  for (i=0;i<edim*ngauss;i++) *(gaucoo+i) = 0.;
  poids  = (med_float*) malloc(sizeof(med_float)*ngauss);

//   EXIT_IF(poids == NULL,NULL,NULL);

  if(poids == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  for (i=0;i<ngauss;i++) *(poids+i) = 0.;

  /* On ecrit la localisation bidon */
  ret = MEDgaussEcr(fid,type,refcoo,MED_FULL_INTERLACE,ngauss,gaucoo,poids,nom_modele);

//   EXIT_IF(ret < 0,"Ecriture de la localisation des points de Gauss",nom_modele);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  /* On libere la memoire */
  free(refcoo);
  free(gaucoo);
  free(poids);
}

void med_2_2::MAJ_champs(med_idt fid) throw (MEDEXCEPTION)
{
  med_idt gid,eid,pid,mid,did,aid;
  med_err ret;
  int n,i,j,k;
  char nom[MED_TAILLE_NOM+1];
  char chemin[MED_TAILLE_CHA+MED_TAILLE_NOM+1];
  char maillage[MED_TAILLE_NOM+1];
  med_int type;
  char *comp, *unit;
  char *nouvelle_chaine;
  med_int ncomp;
  int nent, npdt;
  char noment[MED_TAILLE_NOM+1];
  char nompdt[2*MED_MAX_PARA+1];
  char oldunidt[ANCIEN_MED_TAILLE_PNOM+1];
  char unitdt[MED_TAILLE_PNOM+1];
  char tmp[MED_TAILLE_DESC+1];
  med_int att;
  char profil[MED_TAILLE_NOM+1];
  char gauss[MED_TAILLE_NOM+1];
  med_float *valr;
  med_int nval;
  hid_t hdf_type;
  med_size dimd[1];
  med_float pdt;
  
  /* Lecture du nombre de champs */
  n = 0;
  _MEDnObjets(fid,MED_CHA,&n);

//   EXIT_IF(n < 0,"Erreur a la lecture du nombre de champ",NULL);

  if(n < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  for (i=0;i<n;i++) {
    /* on recupere le nom du champ */
    ret = _MEDobjetIdentifier(fid,MED_CHA,i,nom);

//     EXIT_IF(ret < 0,"Identification d'un champ",NULL);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

//     fprintf(stdout,"  >>> Normalisation du champ [%s] \n",nom);

    MESSAGE("med File convertor 2.1 to 2.2 :: Normalisation du champ [" << nom << "]");

    /* on accede au champ */
    strcpy(chemin,MED_CHA);
    strcat(chemin,nom);
    gid = _MEDdatagroupOuvrir(fid,chemin); 

//     EXIT_IF(gid < 0,"Accès au champ",nom);

    if(gid < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    /* Lecture du nombre de composantes */ 
    ret = _MEDattrEntierLire(gid,MED_NOM_NCO,&ncomp);

//     EXIT_IF(ret < 0,"Lecture du nombre de composante",NULL);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    /* Lecture du type du champ */
    ret = _MEDattrEntierLire(gid,MED_NOM_TYP,&type);

//     EXIT_IF(ret < 0,"Lecture du type du champ",NULL);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    /* Normalisation des nom et unites des composantes */
    comp = (char *) malloc(ncomp*ANCIEN_MED_TAILLE_PNOM+1);

//     EXIT_IF(comp == NULL,NULL,NULL);

    if(comp == NULL)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    unit = (char *) malloc(ncomp*ANCIEN_MED_TAILLE_PNOM+1);

//     EXIT_IF(unit == NULL,NULL,NULL);

    if(unit == NULL)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    nouvelle_chaine = (char *) malloc(ncomp*MED_TAILLE_PNOM+1);

//     EXIT_IF(nouvelle_chaine == NULL,NULL,NULL);

    if(nouvelle_chaine == NULL)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    ret = _MEDattrStringLire(gid,MED_NOM_NOM,ncomp*ANCIEN_MED_TAILLE_PNOM,comp);

//     EXIT_IF(ret < 0,"Lecture des noms des composantes du champ",NULL);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    conversion_chaine(comp,nouvelle_chaine,ncomp);
    H5Adelete(gid,MED_NOM_NOM);
    ret = _MEDattrStringEcrire(gid,MED_NOM_NOM,MED_TAILLE_PNOM*ncomp,nouvelle_chaine);

//     EXIT_IF(ret < 0,"Ecriture des nouveaux noms des composantes du champ",NULL);  

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    free(comp);

    ret = _MEDattrStringLire(gid,MED_NOM_UNI,ncomp*ANCIEN_MED_TAILLE_PNOM,unit);

//     EXIT_IF(ret < 0,"Lecture des unités des composantes du champ",NULL);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    conversion_chaine(unit,nouvelle_chaine,ncomp);
    H5Adelete(gid,MED_NOM_UNI);
    ret = _MEDattrStringEcrire(gid,MED_NOM_UNI,MED_TAILLE_PNOM*ncomp,nouvelle_chaine);

//     EXIT_IF(ret < 0,"Ecriture des nouvelles unités des composantes du champ",NULL);  

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    free(unit);

    free(nouvelle_chaine);

//     fprintf(stdout,"  ... Normalisation des noms et unités des composantes effectuée...\n");

    MESSAGE("med File convertor 2.1 to 2.2 :: ... Normalisation des noms et unités des composantes effectuée...");

    /* On ajoute le groupe HDF correspondant au maillage */
    /* On recupere le nom du maillage par defaut */
    nent = 0;
    _MEDnObjets(gid,"./",&nent);

//     EXIT_IF(nent < 0,"Erreur a la lecture du nombre d'entité dans un champ",NULL);

    if(nent < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

    for (j=0;j<nent;j++) {
      /* on recupere le nom du groupe HDF correspondant a l'entite */ 
      ret = _MEDobjetIdentifier(gid,"./",j,noment);

//       EXIT_IF(ret < 0,"Identification des objets de niveau 1 dans le champ",NULL);

      if(ret < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

      /* On ouvre le groupe HDF */
      eid = _MEDdatagroupOuvrir(gid,noment);

//       EXIT_IF(eid < 0,"Ouverture du groupe HDF de niveau 1",NULL);

      if(eid < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

      /* On recupere le nombre de pas de temps */
      npdt = 0;
      _MEDnObjets(eid,"./",&npdt);

//       EXIT_IF(nent < 0,"Erreur a la lecture du nombre de pas de temps",NULL);
      
      if(nent < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

      /* On traite chaque pas de temps */
      for (k=0;k<npdt;k++) {
	/* On recupere le nom du groupe HDF correspondant au pas de temps */ 
	ret = _MEDobjetIdentifier(eid,"./",k,nompdt);

// 	EXIT_IF(ret < 0,"Identification des objets de niveau 2 dans le champ",NULL);
	
	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	/* On ouvre le groupe HDF correspondant au pas de temps */
	pid = _MEDdatagroupOuvrir(eid,nompdt);

// 	EXIT_IF(pid < 0,"Ouverture du groupe HDF de niveau 2",NULL);

	if(pid < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	/* On lit le pas de temps */
	aid = H5Aopen_name(pid,MED_NOM_PDT);

// 	EXIT_IF(aid < 0,"Ouverture de l'attribut HDF MED_NOM_PDT",NULL);

	if(aid < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	hdf_type = H5Aget_type(aid);

// 	EXIT_IF(hdf_type < 0,"Lecture du type de l'attribut HDF MED_NOM_PDT",NULL);

	if(hdf_type < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	ret = H5Aclose(aid);

// 	EXIT_IF(ret < 0,"Fermeture de l'attribut MED_NOM_PDT",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	ret = OLD_MEDattrNumLire(pid,MED_FLOAT64,MED_NOM_PDT,(unsigned char*)(&pdt),hdf_type);

// 	EXIT_IF(ret < 0,"Lecture du pas de temps dans MED_NOM_PDT",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	dimd[0] = 1;
	/* 	printf("EGALITE des types : %d \n",H5Tequal(hdf_type,H5T_NATIVE_DOUBLE)); */
	/* 	printf("Avant conversion : [%f]\n",pdt); */
	if (! H5Tequal(hdf_type,H5T_NATIVE_DOUBLE)) {
	  ret = H5Tconvert(hdf_type,H5T_NATIVE_DOUBLE,(size_t)*dimd,(void *)(&pdt),NULL,0);
	  /* 	printf("Après conversion éventuelle : [%f] \n",pdt); */

// 	  EXIT_IF(ret < 0,"Conversion du pas de temps",NULL);

	  if(ret < 0)
	    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
	}
	H5Tclose(hdf_type);
	/* 	printf("pdt = %f\n",pdt); */
	H5Adelete(pid,MED_NOM_PDT);
	ret = _MEDattrFloatEcrire(pid,MED_NOM_PDT,&pdt);

// 	EXIT_IF(ret < 0,"Ecriture du nouveau pas de temps",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	/* On met a jour l'unité du pas de temps : 8 -> 16 caractères 
           Si on rencontre "        " => on ecrit "" 
        */
	ret = _MEDattrStringLire(pid,MED_NOM_UNI,ANCIEN_MED_TAILLE_PNOM,oldunidt);

// 	EXIT_IF(ret < 0,"Lecture de l'unite du pas de temps",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	if (! strcmp(oldunidt,ANCIEN_MED_BLANC_PNOM))
	  strcpy(unitdt,"");
	else
	  conversion_chaine(oldunidt,unitdt,1);
	H5Adelete(pid,MED_NOM_UNI);
	ret = _MEDattrStringEcrire(pid,MED_NOM_UNI,MED_TAILLE_PNOM,unitdt);

// 	EXIT_IF(ret < 0,"Ecriture de la nouvelle unité du pas de temps",NULL);  
	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	/* On recupere le nom du maillage */
	ret = _MEDattrStringLire(pid,MED_NOM_MAI,MED_TAILLE_NOM,maillage);

// 	EXIT_IF(ret < 0,"Lecture du nom du maillage du pas de temps",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	/* On cree le groupe HDF de niveau 3 qui porte le nom du maillage */
	mid = _MEDdatagroupCreer(pid,maillage);

// 	EXIT_IF(mid < 0,"Creation du groupe HDF de niveau 3",NULL);

	if(mid < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	/* Déplacement de l'attribut MED_NOM_NBR */
	ret = _MEDattrEntierLire(pid,MED_NOM_NBR,&nval);

// 	EXIT_IF(ret < 0,"Lecture de l'attribut MED_NOM_NBR",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	ret = H5Adelete(pid,MED_NOM_NBR);

// 	EXIT_IF(ret < 0,"Destruction de l'attribut MED_NOM_NBR",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	ret = _MEDattrEntierEcrire(mid,MED_NOM_NBR,&nval);

// 	EXIT_IF(ret < 0,"Ecriture de l'attribut MED_NOM_NBR au niveau 3",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	/* Deplacement de l'attribut MED_NOM_NGA */
	ret = _MEDattrEntierLire(pid,MED_NOM_NGA,&att);

// 	EXIT_IF(ret < 0,"Lecture de l'attribut MED_NOM_NGA",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	ret = H5Adelete(pid,MED_NOM_NGA);

// 	EXIT_IF(ret < 0,"Destruction de l'attribut MED_NOM_NGA",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	ret = _MEDattrEntierEcrire(mid,MED_NOM_NGA,&att);

// 	EXIT_IF(ret < 0,"Ecriture de l'attribut MED_NOM_NGA au niveau 3",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	/* Ecriture de l'attribut MED_NOM_GAU : localisation des points de Gauss */
	if (att < 2)
	  strcpy(gauss,MED_NOM_BLANC);
	else {
	  /* on prend en compte que les 3 dernières lettres de noment : 
	     Exemple : "MAI.QU4" => gauss="QU4_LocalisationDefaut_MEDIMPORT */
	  strcpy(gauss,noment+4);
	  strcat(gauss,"_LocalisationDefaut_MEDIMPORT");
	  /* On ecrit en plus la localisation bidon des points de Gauss */
	  MAJ_localisation_Gauss(fid,gauss,att);
	}
	ret = _MEDattrStringEcrire(mid,MED_NOM_GAU,MED_TAILLE_NOM,gauss);

// 	EXIT_IF(ret < 0,"Ecriture de la localisation des points de Gauss",NULL);
	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	/* Deplacement de l'attribut MED_NOM_PFL */
	ret = _MEDattrStringLire(pid,MED_NOM_PFL,MED_TAILLE_NOM,profil);

// 	EXIT_IF(ret < 0,"Lecture de l'attribut MED_NOM_PFL",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	ret = H5Adelete(pid,MED_NOM_PFL);

// 	EXIT_IF(ret < 0,"Desctruction de l'attribut MED_NOM_PFL",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	ret = _MEDattrStringEcrire(mid,MED_NOM_PFL,MED_TAILLE_NOM,profil);

// 	EXIT_IF(ret < 0,"Ecriture de l'attribut MED_NOM_PFL au niveau 3",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	/* On ferme le groupe HDF de niveau 3 */
	ret = _MEDdatagroupFermer(mid);

// 	EXIT_IF(ret < 0,"fermeture du groupe HDF de niveau 3",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	/* On deplace du niveau 2 -> 3, le dataset MED_NOM_CO 
           avec si type==MED_FLOAT_64 une conversion au passage */
	strcpy(tmp,maillage);
	strcat(tmp,"/");
	strcat(tmp,MED_NOM_CO);
	if (type != MED_FLOAT64) {
	  ret = H5Gmove(pid,MED_NOM_CO,tmp);

// 	  EXIT_IF(ret < 0,"Transfert du dataset MED_NOM_CO",NULL);

	  if(ret < 0)
	    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
	} else {
	  did = _MEDdatasetOuvrir(pid,MED_NOM_CO);

// 	  EXIT_IF(did < 0,"Ouverture du dataset HDF des valeurs",NULL);

	  if(did < 0)
	    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	  hdf_type = H5Dget_type(did);

// 	  EXIT_IF(hdf_type < 0,"Lecture du type HDF de stockage des valeurs",NULL);

	  if(hdf_type < 0)
	    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	  ret = _MEDdatasetFermer(did);

// 	  EXIT_IF(ret < 0,"Fermeture du dataset HDF des valeurs",NULL);

	  if(ret < 0)
	    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	  valr = (med_float *) malloc(sizeof(med_float)*ncomp*nval);

// 	  EXIT_IF(valr == NULL,NULL,NULL);

	  if(valr == NULL)
	    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	  ret = OLD_MEDdatasetNumLire(pid,MED_NOM_CO,MED_FLOAT64,
				      MED_FULL_INTERLACE,ncomp,MED_ALL,
				      0,NULL,MED_NOPG,
				      (unsigned char*) valr,hdf_type);

// 	  EXIT_IF(ret < 0,"Lecture des valeurs du champ",NULL);

	  if(ret < 0)
	    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	  H5Gunlink(pid,MED_NOM_CO);
	  dimd[0] = ncomp*nval;
	  ret = H5Tconvert(hdf_type,H5T_NATIVE_DOUBLE,(size_t)*dimd,(void *)valr,NULL,0);

// 	  EXIT_IF(ret < 0,"Conversion des valeurs",NULL);

	  if(ret < 0)
	    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	  H5Tclose(hdf_type);
	  /* On reporte les valeurs dans le groupe HDF TMP */
	  ret = _MEDdatasetNumEcrire(pid,tmp,MED_FLOAT64,MED_FULL_INTERLACE,
				     ncomp,MED_ALL,MED_NOPF,MED_NO_PFLMOD,0,MED_NOPG,dimd,
				     (unsigned char*) valr);

// 	  EXIT_IF(ret < 0,"Ecriture des valeurs après conversion",NULL);

	  if(ret < 0)
	    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

	  free(valr);
	}
	  
	/* On ferme le groupe HDF de niveau 2*/
	ret = _MEDdatagroupFermer(pid);

// 	EXIT_IF(ret < 0,"Fermeture du groupe HDF de niveau 2",NULL);

	if(ret < 0)
	  throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
      }

      /* On ferme le groupe HDF */
      ret = _MEDdatagroupFermer(eid);

 //      EXIT_IF(ret < 0,"Fermeture du groupe HDF de niveau 1",NULL);

      if(ret < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");
    }

//     fprintf(stdout,"  ... Normalisation de tous les pas de temps effectuée...\n");

    MESSAGE("med File convertor 2.1 to 2.2 :: ... Normalisation de tous les pas de temps effectuée...\n");

    /* On ferme tout */
    ret = _MEDdatagroupFermer(gid);

//     EXIT_IF(ret < 0,"Fermeture de l'accès au champ",NULL);
    
    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

//     fprintf(stdout,"  >>> Normalisation du champ [%s] : ... OK ... \n",nom);

    MESSAGE("med File convertor 2.1 to 2.2 :: Normalisation du champ [" << nom << "] : ... OK ... ");
  }
}

void med_2_2::MAJ_profils(med_idt fid,med_int nprofil) throw (MEDEXCEPTION)
{
  med_err ret;
  med_int i;
  char nom[MED_TAILLE_NOM+1];
  char chemin[MED_TAILLE_DESC+1];
  med_idt gid;
  med_int att;

  for (i=0;i<nprofil;i++)
    {    
      /* on recupere le nom du profil */
      ret = _MEDobjetIdentifier(fid,MED_PROFILS,i,nom);

 //      EXIT_IF(ret < 0,"Identification d'un profil",NULL);

      if(ret < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

//       fprintf(stdout,"  >>> Normalisation du profil [%s] \n",nom);

      MESSAGE("med File convertor 2.1 to 2.2 ::  Normalisation du profil [" << nom << "] ");

      /* on accede au profil */
      strcpy(chemin,MED_PROFILS);
      strcat(chemin,nom);
      gid = _MEDdatagroupOuvrir(fid,chemin); 

//       EXIT_IF(gid < 0,"Accès au profil",nom);

      if(gid < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");


      /* On change l'attribut MED_NOM_N => MED_NOM_NBR */
      ret = _MEDattrEntierLire(gid,"N",&att);

//       EXIT_IF(ret < 0,"Lecture de l'attribut MED_NOM_N",NULL);

      if(ret < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

      ret = H5Adelete(gid,"N");

//       EXIT_IF(ret < 0,"Destruction de l'attribut MED_NOM_N",NULL);

      if(ret < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

      ret = _MEDattrEntierEcrire(gid,MED_NOM_NBR,&att);

//       EXIT_IF(ret < 0,"Ecriture de l'attribut MED_NOM_NBR dans le profil ",nom);

      if(ret < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

      /* on ferme le profil */
      ret = _MEDdatagroupFermer(gid);

//       EXIT_IF(ret < 0,"Fermeture de l'acces au profil",nom);

      if(ret < 0)
	throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

//       fprintf(stdout,"  >>> Normalisation du profil [%s] effectuée \n",nom);

      MESSAGE("med File convertor 2.1 to 2.2 :: Normalisation du profil [" << nom << "]");
    }
}

char * MEDMEM::med2_1_To_med2_2(char * fileNameIn) throw (MEDEXCEPTION)
{
  med_idt fid, gid;
  med_err ret;
  med_int majeur, mineur, release;
/*   char *nom; */
  char *commande;
  med_int nprofil;
  char chemin_profils[MED_TAILLE_PROFILS+1];
  char chemin_liens[MED_TAILLE_LIENS+1];

  char * InternalFileNameOut;

  char * envHome = (char *) malloc(sizeof(char)*(strlen("HOME")+1));

  strcpy(envHome,"HOME");

  char * envHomeChar = getenv(envHome);

  /* Test du nombre d'argument */
//   EXIT_IF(argc != 2,"Nombre de parametre incorrect",NULL);

  /* Test du format du fichier */
/*   EXIT_IF(MEDformatConforme(argv[1]) < 0, */
/* 	  "Le fichier n'est pas au format HDF 5 : ", argv[1]); */

//   EXIT_IF(MEDformatConforme(fileNameIn) < 0,
// 	  "Le fichier n'est pas au format HDF 5 : ", fileNameIn);

  if(MEDformatConforme(fileNameIn) < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check the med File you wanted to mount in memory");

  /* opening the old file to check the versions (Med File and HDF) */

  fid = MEDouvrir(fileNameIn,MED_LECTURE);

/*   EXIT_IF(fid < 0,"Ouverture du fichier : ", */
/* 	  nom); */

//   EXIT_IF(fid < 0,"Ouverture du fichier : ",
// 	  InternalFileNameOut);

  MESSAGE("med File convertor 2.1 to 2.2 ::  opening of the old file id " << fid);

  if(fid < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check you have enough memory and/or space disc to write the new med 2.2 file : openning of the file");

  /* Verification du numero de version */
  ret = MEDversionLire(fid,&majeur,&mineur,&release); 

//   EXIT_IF(ret < 0,"Lecture du numero de version de MED-fichier",NULL);  

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check you have enough memory and/or space disc to write the new med 2.2 file");

  if (majeur == 2 && mineur == 2) {  
/*     fprintf(stdout,"Le fichier %s est déjà au format V2.2 !!! \n",nom);   */

//     fprintf(stdout,"Le fichier %s est déjà au format V2.2 !!! \n",InternalFileNameOut);  

    MESSAGE("med File convertor 2.1 to 2.2 :: the file " << fileNameIn << " is already a med 2.2 file");

    ret = MEDfermer(fid);  
/*     EXIT_IF(ret < 0,"Fermeture du fichier",argv[1]);  */

//     EXIT_IF(ret < 0,"Fermeture du fichier",fileNameIn); 

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check you have enough memory and/or space disc to write the new med 2.2 file");

    return fileNameIn ;
  }

  ret = MEDfermer(fid); 

  /*
    we are sure now that the file is an old version one
    creation of the directory if it does not existe
  */

  commande = (char *) malloc(sizeof(char)*(strlen("mkdir ")+
					   strlen(envHomeChar)+
					   strlen("/TMP_Med2_2Files/")+1));

//   EXIT_IF(commande == NULL,NULL,NULL);

  if(commande == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check you have enough memory and/or space disc to write the new med 2.2 file");

  strcpy(commande,"ls ");
  strcat(commande,envHomeChar);
  strcat(commande,"/TMP_Med2_2Files/");

  int systemReturn = system(commande);

  if (systemReturn != 0)
    {
      strcpy(commande,"mkdir ");
      strcat(commande,envHomeChar);
      strcat(commande,"/TMP_Med2_2Files/");

      systemReturn = system(commande);
    }

  free(commande);

  free(envHome);

  string::size_type pos1 = string(fileNameIn).rfind('/');
  string::size_type pos = strlen(fileNameIn);
  InternalFileNameOut = (char *) malloc(sizeof(char)*(strlen(envHomeChar)+
						      strlen("/TMP_Med2_2Files/")
						      +strlen(string(fileNameIn,pos1+1,pos-pos1-1).c_str())+1));

  strcpy(InternalFileNameOut,envHomeChar);
  strcat(InternalFileNameOut,"/TMP_Med2_2Files/");
  strcat(InternalFileNameOut,string(fileNameIn,pos1+1,pos-pos1-1).c_str());

  MESSAGE("med File convertor 2.1 to 2.2 :: " << InternalFileNameOut);

  /* Creation et ouverture du fichier que l'on va convertire au format MED V2.2 */
/*   commande = (char *) malloc(sizeof(char)*(strlen("cp ")+2*strlen(argv[1])+strlen(" ")+strlen("2.2")+1)); */

#ifndef WNT
  commande = (char *) malloc(sizeof(char)*(strlen("cp -f ") +
#else
  commande = (char *) malloc(sizeof(char)*(strlen("copy /Y ") +
#endif
					   strlen(fileNameIn) + strlen(" ") +
					   strlen(InternalFileNameOut) + 
					   strlen(" ; chmod ug+rw ") +
					   strlen(InternalFileNameOut) + 1));

//   EXIT_IF(commande == NULL,NULL,NULL);

  if(commande == NULL)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check you have enough memory and/or space disc to write the new med 2.2 file");

/*   nom = (char *) malloc(sizeof(char)*(strlen(argv[1])+strlen("2.2")+1)); */

/*   nom = (char *) malloc(sizeof(char)*(strlen(fileNameIn)+strlen("2.2")+1)); */

/*   EXIT_IF(nom == NULL,NULL,NULL); */
/*   strcpy(nom,argv[1]); */

/*   strcpy(nom,fileNameIn); */

/*   strcat(nom,"2.2"); */

//   strcpy(fileNameOut,fileNameIn);

//   strcat(fileNameOut,"2.2");

#ifndef WNT
  strcpy(commande,"cp -f ");
#else
  strcpy(commande,"copy /Y ");
#endif
/*   strcat(commande,argv[1]); */

  strcat(commande,fileNameIn);

  strcat(commande," ");
/*   strcat(commande,nom); */

  strcat(commande,InternalFileNameOut);

  strcat(commande," ; chmod ug+rw ");

  strcat(commande,InternalFileNameOut);

  system(commande);
/*   fprintf(stdout,">>> Creation du fichier %s : %s \n",nom,commande); */

//   fprintf(stdout,">>> Creation du fichier %s : %s \n",InternalFileNameOut,commande);

  MESSAGE("med File convertor 2.1 to 2.2 ::  Creation du fichier " << InternalFileNameOut << " with the commande " << commande);

  free(commande);

/*   fid = MEDouvrir(nom,MED_LECTURE_ECRITURE); */

  fid = MEDouvrir(InternalFileNameOut,MED_LECTURE_ECRITURE);

/*   EXIT_IF(fid < 0,"Ouverture du fichier : ", */
/* 	  nom); */

//   EXIT_IF(fid < 0,"Ouverture du fichier : ",
// 	  InternalFileNameOut);

  MESSAGE("med File convertor 2.1 to 2.2 ::  opening of the new file id " << fid);

  if(fid < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check you have enough memory and/or space disc to write the new med 2.2 file : openning of the file");

  /* On avertit qu'on commence la conversion */

//   fprintf(stdout,">>> Lancement de la normalisation du fichier selon le format MED V2.2 ...\n");

  MESSAGE("med File convertor 2.1 to 2.2 :: Running the convertor fromV 2.1 to V2.2");

  /* On inhibe le gestionnaire d'erreur HDF5 */
  ::_MEDmodeErreurVerrouiller();  

  /* Mise a jour du numero de version */

//   fprintf(stdout,"- Lancement de la mise à jour du numéro de version ... \n");

  MESSAGE("med File convertor 2.1 to 2.2 :: Running the update of the version number");

  MAJ_version(fid);

//   fprintf(stdout,"  Numéro de version : ... OK ... \n");

  MESSAGE("med File convertor 2.1 to 2.2 :: Version number OK ...");

  /* Mise a jour des maillages : type = MED_NON_STRUCTURE, description, ... */

//   fprintf(stdout,"- Lancement de la mise à jour des maillages ... \n"); 

  MESSAGE("med File convertor 2.1 to 2.2 :: Running the update of all meshes");

  MAJ_maillages(fid); 

//   fprintf(stdout,"  Maillage(s) : ... OK ...\n"); 

  MESSAGE("med File convertor 2.1 to 2.2 :: meshes are OK....");

  /* Mise a jour des champs */

//   fprintf(stdout,"- Lancement de la mise à jour des champs de résultats ... \n");

  MESSAGE("med File convertor 2.1 to 2.2 :: Running of the update of all fields ..");

  MAJ_champs(fid);

//   fprintf(stdout,"  Champs(s) : ... OK ...\n");

  MESSAGE("med File convertor 2.1 to 2.2 :: Fields are OK ..");

  /* Mise a jour des profils eventuels */
  nprofil = MEDnProfil(fid);
  if (nprofil > 0) {
//     fprintf(stdout,"- Lancement de la mise à jour des profils ... \n");

    MESSAGE("med File convertor 2.1 to 2.2 :: Running the update of all profils");

    MAJ_profils(fid,nprofil);

//     fprintf(stdout,"  Profils(s) : ... OK ...\n");

    MESSAGE("med File convertor 2.1 to 2.2 :: Profils are OK ... ");
  } else {
    strncpy(chemin_profils,MED_PROFILS,MED_TAILLE_PROFILS-1);
    chemin_profils[MED_TAILLE_PROFILS-1] = '\0';
    gid = _MEDdatagroupCreer(fid,chemin_profils);

//     EXIT_IF(gid < 0,"Creation du groupe HDF sur les profils",chemin_profils);

    if(gid < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check you have enough memory and/or space disc to write the new med 2.2 file");

    ret = _MEDdatagroupFermer(gid);

//     EXIT_IF(ret < 0,"Fermeture du groupe HDF sur les profils",chemin_profils);

    if(ret < 0)
      throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check you have enough memory and/or space disc to write the new med 2.2 file");

  }

  /* On cree le groupe HDF pour les liens */
  strncpy(chemin_liens,MED_LIENS,MED_TAILLE_LIENS-1);
  chemin_liens[MED_TAILLE_LIENS-1] = '\0';
  gid = _MEDdatagroupCreer(fid,chemin_liens);

//   EXIT_IF(gid < 0,"Creation du groupe HDF sur les liens",chemin_liens);

  if(gid < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check you have enough memory and/or space disc to write the new med 2.2 file");

  ret = _MEDdatagroupFermer(gid);

//   EXIT_IF(ret < 0,"Fermeture du groupe HDF sur les liens",chemin_liens);
 
  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check you have enough memory and/or space disc to write the new med 2.2 file");

  /* Fermeture du fichier */
  ret = MEDfermer(fid);
/*   EXIT_IF(ret < 0,"Fermeture du fichier",nom); */

//   EXIT_IF(ret < 0,"Fermeture du fichier",InternalFileNameOut);

  if(ret < 0)
    throw MEDEXCEPTION("Problem in the med File convertor 2.1 to 2.2, you should check you have enough memory and/or space disc to write the new med 2.2 file");

  /* On avertit que c'est fini */
/*   fprintf(stdout,">>> Conversion du fichier %s au format MED V2.2. terminée\n", */
/* 	  nom); */

//   fprintf(stdout,">>> Conversion du fichier %s au format MED V2.2. terminée\n",
// 	  InternalFileNameOut);

  /* On libere la memoire */
/*   free(nom); */

  char * fileNameOut = new char[strlen(InternalFileNameOut)+1];

  fileNameOut = strcpy(fileNameOut,InternalFileNameOut);

  free(InternalFileNameOut);

  MESSAGE("med File convertor 2.1 to 2.2 :: Convertion of the med file " << fileNameOut << " has run OK");

  return fileNameOut;
}
