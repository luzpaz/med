//  MED MEDGUI : MED component GUI implemetation 
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : MedGUI.cxx
//  Module : MED

using namespace std;
#include "MedGUI.h"

// SALOME Includes
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"
#include "utilities.h"

#include "SALOME_Selection.h"
#include "SALOME_InteractiveObject.hxx"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include "QAD_MessageBox.h"
#include "QAD_Tools.h"
#include "QAD_FileDlg.h"

//#include "SMESH_TypeFilter.hxx"

// QT Includes
#include <qinputdialog.h>

//VRV: porting on Qt 3.0.5
#if QT_VERSION >= 0x030005
#include <qlineedit.h>
#endif
//VRV: porting on Qt 3.0.5

static CORBA::ORB_var   _orb;

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::OnGUIEvent (int theCommandID, QAD_Desktop* parent)
{
  setOrb();
  
  QAD_Study* myActiveStudy   = parent->getActiveStudy();
  SALOMEDS::Study_var aStudy = myActiveStudy->getStudyDocument();
  SALOME_NamingService* myNameService = parent->getNameService();

  QString file;
  QStringList filtersList ;	

  filtersList.append( tr("MED_MEN_IMPORT_MED") );
  filtersList.append( tr("MED_MEN_ALL_FILES") ) ;

  switch (theCommandID)
    {
    case 4031:
    case 9031:
    case 931:
      {
	MESSAGE("command " << theCommandID << " activated");

	QString myStudyName = myActiveStudy->getTitle();
	bool ok=FALSE;
	int myStudyId = myActiveStudy->getStudyId();

	// load MED engine
	SALOME_MED::MED_Gen_ptr medgen = InitMedGen(parent);

	// Selection du Fichier
	file = QAD_FileDlg::getFileName(parent,
					"",
					filtersList,
					tr("MED_MEN_IMPORT"),
					true);

        // Selection du Maillage
	if (!file.isEmpty() )
	  {
	    SCRUTE(file);
	    QString meshName;
	    meshName = QInputDialog::getText( QString( tr("MED_INF_MESHNAME") ), 
					      QString::null,
					      //VRV: porting on Qt 3.0.5
#if QT_VERSION >= 0x030005
					      QLineEdit::Normal,
#endif
					      //VRV: porting on Qt 3.0.5
					      QString::null, &ok);
	    if ( ! meshName.isEmpty())
	      {
		  try
		    {
		      medgen->readMeshInFile(file.latin1(),myStudyName,meshName);
		      if (myActiveStudy->getStudyDocument()->GetProperties()->IsLocked()) {
			QAD_MessageBox::warn1 ((QWidget*)QAD_Application::getDesktop(),
					       QObject::tr("WRN_WARNING"), 
					       QObject::tr("WRN_STUDY_LOCKED"),
					       QObject::tr("BUT_OK"));
		      }
		    }
		  catch (const SALOME::SALOME_Exception & S_ex)
		    {
		      QtCatchCorbaException(S_ex);
		    }
		}
		myActiveStudy->updateObjBrowser();
	  }
	break;
      }
    case 4032:
    case 9032:
    case 932:
      {
	MESSAGE("command " << theCommandID << " activated");

	QString myStudyName = myActiveStudy->getTitle();
	bool ok=FALSE;
	int myStudyId = myActiveStudy->getStudyId();

	// load MED engine
	SALOME_MED::MED_Gen_ptr medgen = InitMedGen(parent);

	// Selection du Fichier
	file = QAD_FileDlg::getFileName(parent,
					"",
					filtersList,
					tr("MED_MEN_IMPORT"),
					true);

        // Selection du Maillage
	if (!file.isEmpty() )
	  {
	    SCRUTE(file);
	    QString fieldName;
	    fieldName = QInputDialog::getText(
					      QString( tr("MED_INF_FIELDNAME") ), QString::null,
					      //VRV: porting on Qt 3.0.5
#if QT_VERSION >= 0x030005
					      QLineEdit::Normal,
#endif
					      //VRV: porting on Qt 3.0.5
					      QString::null, &ok);
	    if ( ! fieldName.isEmpty())
	      {
		try
		  {
		    medgen->readFieldInFile(file.latin1(),myStudyName,fieldName,-1,-1);
		    if (myActiveStudy->getStudyDocument()->GetProperties()->IsLocked()) {
		      QAD_MessageBox::warn1 ((QWidget*)QAD_Application::getDesktop(),
					     QObject::tr("WRN_WARNING"), 
					     QObject::tr("WRN_STUDY_LOCKED"),
					     QObject::tr("BUT_OK"));
		    }
		  }
		catch (const SALOME::SALOME_Exception & S_ex)
		  {
		    QtCatchCorbaException(S_ex);
		  }

		myActiveStudy->updateObjBrowser();
	      }
	  }
	break;
      }
    case 4033:
    case 933:
      {
	MESSAGE("command " << theCommandID << " activated");

	QString myStudyName = myActiveStudy->getTitle();
	int myStudyId = myActiveStudy->getStudyId();

	// load MED engine
	SALOME_MED::MED_Gen_ptr medgen = InitMedGen(parent);

	// Selection du Fichier
	file = QAD_FileDlg::getFileName(parent,
					"",
					filtersList,
					tr("MED_MEN_IMPORT"),
					true);
	if (!file.isEmpty() )
	  {
	    SCRUTE(file);
	    try
	      {
//		medgen->readStructFile(file.latin1(),myStudyName);
		medgen->readStructFileWithFieldType(file.latin1(),myStudyName);
		if (myActiveStudy->getStudyDocument()->GetProperties()->IsLocked()) {
		  QAD_MessageBox::warn1 ((QWidget*)QAD_Application::getDesktop(),
					 QObject::tr("WRN_WARNING"), 
					 QObject::tr("WRN_STUDY_LOCKED"),
					 QObject::tr("BUT_OK"));
		}
	      }
	    catch (const SALOME::SALOME_Exception & S_ex)
	      {
		QtCatchCorbaException(S_ex);
	      }
	    myActiveStudy->updateObjBrowser();
	  }
	break;
      }
      
    case 934:
      {
	//Handle(SMESH_TypeFilter) aMeshFilter = new SMESH_TypeFilter( MESH );
	SALOME_Selection* Sel = SALOME_Selection::Selection( myActiveStudy->getSelection() );
	//Sel->AddFilter(aMeshFilter) ;
	
	int nbSel = Sel->IObjectCount();
	if ( nbSel == 1 ) 
	  {
	    //	    SMESH::SMESH_Mesh_var aM;
	    SALOME_MED::MESH_var aMesh;
	    Handle(SALOME_InteractiveObject) IObject = Sel->firstIObject();
	    if ( IObject->hasEntry() ) 
	      {
		SALOMEDS::SObject_var aMorSM = aStudy->FindObjectID( IObject->getEntry() );
		if ( !aMorSM->_is_nil() ) 
		  {
		    SALOMEDS::GenericAttribute_var anAttr;
		    SALOMEDS::AttributeIOR_var     anIOR;
		    if (aMorSM->FindAttribute(anAttr, "AttributeIOR")) 
		      {
			anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
			aMesh = SALOME_MED::MESH::_narrow( _orb->string_to_object(anIOR->Value()) );
			if ( aMesh->_is_nil() )
			  {
			    //  			    aM = SMESH::SMESH_Mesh::_narrow(_orb->string_to_object(anIOR->Value()));
			    //  			    if ( aM->_is_nil() )
			    //  			      {
			    //  				QAD_MessageBox::warn1
			    //  				  ( QAD_Application::getDesktop(),
			    //  				    tr ("MED_WRN_WARNING"),
			    //  				    tr ("MED_INF_NOTIMPL"),
			    //  				    tr ("MED_BUT_OK") );
			    //  				break;
			    //  			      }
			    //  			    aMesh = aM->GetMEDMesh();
			    if ( aMesh->_is_nil() )
			      {
				QAD_MessageBox::warn1
				  ( QAD_Application::getDesktop(),
				    tr ("MED_WRN_WARNING"),
				    tr ("MED_INF_NOTIMPL"),
				    tr ("MED_BUT_OK") );
				break;
			      }
  			  }
  			DumpMesh( aMesh );
			//Sel->ClearFilters() ;
		      }
		    else
		      {
			QAD_MessageBox::warn1
			  ( QAD_Application::getDesktop(),
			    tr ("MED_WRN_WARNING"),
			    tr ("MED_INF_NOIOR"),
			    tr ("MED_BUT_OK") );
			break;
		      }
		  }
	      }
	  } 
	break;
      }

    case 935:
      {
	//Handle(SMESH_TypeFilter) aSubMeshFilter = new SMESH_TypeFilter( SUBMESH );
	SALOME_Selection* Sel = SALOME_Selection::Selection( myActiveStudy->getSelection() );
	//Sel->AddFilter(aSubMeshFilter) ;
	
	int nbSel = Sel->IObjectCount();
	if ( nbSel == 1 ) 
	  {
	    //	    SMESH::SMESH_subMesh_var aSubM;
	    SALOME_MED::FAMILY_var aFam;
	    Handle(SALOME_InteractiveObject) IObject = Sel->firstIObject();
	    if ( IObject->hasEntry() ) 
	      {
		SALOMEDS::SObject_var aMorSM = aStudy->FindObjectID( IObject->getEntry() );
		if ( !aMorSM->_is_nil() ) 
		  {
		    SALOMEDS::GenericAttribute_var anAttr;
		    SALOMEDS::AttributeIOR_var     anIOR;
		    if (aMorSM->FindAttribute(anAttr, "AttributeIOR")) 
		      {
			anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
			//			aSubM = SMESH::SMESH_subMesh::_narrow( _orb->string_to_object(anIOR->Value()) );
			//  			if ( aSubM->_is_nil() )
			//  			  {
			//  			    aFam=SALOME_MED::FAMILY::_narrow( _orb->string_to_object(anIOR->Value()));
			//  			    if ( aFam->_is_nil() )
			//  			      {
			//  				QAD_MessageBox::warn1
			//  				  ( QAD_Application::getDesktop(),
			//  				    tr ("MED_WRN_WARNING"),
			//  				    tr ("MED_INF_NOTIMPL"),
			//  				    tr ("MED_BUT_OK") );
			//  				break;
			//  			      }
			//  			    DumpSubMesh( aFam );
			//    }
			//  			else
			//  			  {
			//  			    DumpSubMesh( aSubM );
			//Sel->ClearFilters() ;
			//       }
		      }
		    else
		      {
			QAD_MessageBox::warn1
			  ( QAD_Application::getDesktop(),
			    tr ("MED_WRN_WARNING"),
			    tr ("MED_INFNOIOR"),
			    tr ("MED_BUT_OK") );
			break;
		      }
		  }
	      }
	  } 
	break;
      }
    }
  return true;
}


//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::OnMousePress (QMouseEvent* pe ,
			   QAD_Desktop* parent, 
			   QAD_StudyFrame* studyFrame)
{
  MESSAGE("MedGUI::OnMousePress");
  return true;
}
 
//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::OnMouseMove (QMouseEvent* pe ,
			  QAD_Desktop* parent, 
			  QAD_StudyFrame* studyFrame)
{
  //   MESSAGE("MedGUI::OnMouseMouve");
  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::OnKeyPress (QKeyEvent* pe,
			 QAD_Desktop* parent,
			 QAD_StudyFrame* studyFrame)
{
  MESSAGE("MedGUI::OnKeyPress");
  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::SetSettings (QAD_Desktop* parent)
{
  MESSAGE("MedGUI::SetSettings");
  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::CustomPopup ( QAD_Desktop* parent,
			   QPopupMenu* popup,
			   const QString & theContext,
			   const QString & theParent,
			   const QString & theObject )
{
  MESSAGE("MedGUI::CustomPopup");
  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
void MedGUI::ActiveStudyChanged( QAD_Desktop* parent )
{
}

//=============================================================================
/*!
 *
 */
//=============================================================================
void MedGUI::DefinePopup( QString & theContext, QString & theParent, QString & theObject )
{
  theObject = "";
  theContext = "";
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::DumpMesh( SALOME_MED::MESH_var MEDMesh)
{
  
  if ( MEDMesh->_is_nil() )
    {
      return false;
    }

  //SALOME_MED::MESH_var MEDMesh = aMesh->GetMEDMesh();
  string name = MEDMesh->getName();
  SCRUTE(name);

  int dim = MEDMesh->getMeshDimension();
  SCRUTE(dim);
  int dim2 = MEDMesh->getSpaceDimension();
  SCRUTE(dim2);

  int k = MEDMesh->getNumberOfNodes() ;
  SCRUTE(k);
  SALOME_MED::double_array_var coords = MEDMesh->getCoordinates( SALOME_MED::MED_FULL_INTERLACE );
  int i = 0;
  int lu = 0;
  while (lu < k ) {
    if (dim2==3)
      {
    	MESSAGE ( " Coordinates  X = " << coords[i] << " Y = " << coords[i+1] << " Z = " << coords[i+2] ); 
    	i = i + 3; // Only for triangles
      }
    else
      {
    	MESSAGE ( " Coordinates  X = " << coords[i] << " Y = " << coords[i+1] ); 
    	i = i + 2; 
      }
    lu=lu+1;
  }

  int nbfam=MEDMesh->getNumberOfFamilies(SALOME_MED::MED_NODE);
  SCRUTE(nbfam);
  SALOME_MED::Family_array_var Families=MEDMesh->getFamilies(SALOME_MED::MED_NODE) ;
  
  for (k=0;  k < nbfam; k++) {
    SCRUTE(k);
    string nomFam=Families[k]->getName();
    SCRUTE(nomFam);
    int identfam=Families[k]->getIdentifier();
    SCRUTE(identfam);
    int nbelemnts=Families[k]->getNumberOfElements(SALOME_MED::MED_NONE);
    SCRUTE(nbelemnts);
    SALOME_MED::long_array_var tabnoeuds=Families[k]->getNumber(SALOME_MED::MED_NONE);
    for (int l=0;l<tabnoeuds->length();l++)
      SCRUTE(tabnoeuds[l]); 
  }
   
  //     int famIdent = 1;
  //     SALOME_MED::FAMILY_ptr Family=MEDMesh->getFamily(SALOME_MED::MED_NODE,1) ;
  //     MESSAGE("ici");
  //     string nomFam=Family->getName();
  //     SCRUTE(nomFam);
  //     int identfam=Family->getIdentifier();
  //     SCRUTE(identfam);
  //     SALOME_MED::long_array_var tabnoeuds=Family->getNumber(SALOME_MED::MED_NONE);
  //     for (int l=0;l<tabnoeuds->length();l++)
  //       SCRUTE(tabnoeuds[l]); 

  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
//  bool MedGUI::DumpSubMesh( SMESH::SMESH_subMesh_ptr aSubMesh )
//  {
//    if ( aSubMesh->_is_nil() )
//      return false;

//    SALOME_MED::FAMILY_var Fam = aSubMesh->GetFamily();
//    if ( Fam->_is_nil() )
//      return false;

//    SALOME_MED::long_array_var tabnoeuds=Fam->getNumber(SALOME_MED::MED_NONE);
//    for (int l=0;l<tabnoeuds->length();l++)
//      SCRUTE(tabnoeuds[l]); 

//    return true;
//  }
//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::DumpSubMesh( SALOME_MED::FAMILY_var Fam )
{

  if ( Fam->_is_nil() )
    return false;

  SALOME_MED::long_array_var tabnoeuds=Fam->getNumber(SALOME_MED::MED_NONE);
  for (int l=0;l<tabnoeuds->length();l++)
    SCRUTE(tabnoeuds[l]); 

  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
SALOME_MED::MED_Gen_ptr MedGUI::InitMedGen(QAD_Desktop* parent)
{
  Engines::Component_var comp = 
    parent->getEngine("FactoryServer", "MED");
  MESSAGE("_________________________________________");
  SALOME_MED::MED_Gen_var clr = SALOME_MED::MED_Gen::_narrow(comp);
  ASSERT(!CORBA::is_nil(clr));
  return clr._retn();
}

//=============================================================================
/*!
 *
 */
//=============================================================================
void MedGUI::setOrb()
{
  try {
    ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance();
    ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting());
    _orb = init( 0 , 0 );
  } catch (...) {
    INFOS("internal error : orb not found");
    _orb = 0;
  }
  ASSERT(! CORBA::is_nil(_orb));
}

//=============================================================================
/*!
 *
 */
//=============================================================================
extern "C"
{
  bool OnGUIEvent(int theCommandID, QAD_Desktop* parent)
  {
    return MedGUI::OnGUIEvent(theCommandID, parent);
  }

  bool OnKeyPress (QKeyEvent* pe,
		   QAD_Desktop* parent, 
		   QAD_StudyFrame* studyFrame)
  {
    return MedGUI::OnKeyPress (pe, parent, studyFrame);
  }

  bool OnMousePress (QMouseEvent* pe,
		     QAD_Desktop* parent,
		     QAD_StudyFrame* studyFrame)
  {
    return MedGUI::OnMousePress (pe, parent, studyFrame);
  }

  bool OnMouseMove (QMouseEvent* pe,
		    QAD_Desktop* parent, 
		    QAD_StudyFrame* studyFrame)
  {
    return MedGUI::OnMouseMove (pe, parent, studyFrame);
  }

  bool SetSettings ( QAD_Desktop* parent )
  {
    return MedGUI::SetSettings( parent );
  }

  bool customPopup ( QAD_Desktop* parent,
		     QPopupMenu* popup,
		     const QString & theContext,
		     const QString & theParent,
		     const QString & theObject )
  {
    return MedGUI::CustomPopup( parent, popup, theContext,
				theParent, theObject );
  }

  void definePopup ( QString & theContext, QString & theParent, QString & theObject )
  {
    MedGUI::DefinePopup( theContext, theParent, theObject );
  }
  
  bool activeStudyChanged ( QAD_Desktop* parent )
  {
    MedGUI::ActiveStudyChanged( parent );
  }
}
