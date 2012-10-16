// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//  MED MEDGUI : MED component GUI implemetation
//  File   : MEDGUI.cxx
//  Module : MED
//
#include "MEDGUI.h"

#include "MEDGUIFileContentDial.h"
#include "MEDGUIDataBaseDockWidget.hxx"
#include "MEDGUISelection.hxx"

#include <MED_version.h>

#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"
#include "utilities.h"

#include <SALOME_LifeCycleCORBA.hxx>
#include <SALOME_InteractiveObject.hxx>
#include <SALOME_ListIO.hxx>
#include <SalomeApp_Tools.h>

#include <SUIT_MessageBox.h>
#include <SUIT_Tools.h>
#include <SUIT_FileDlg.h>
#include <SUIT_ResourceMgr.h>

#include <CAM_Application.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_DataModel.h>
#include <SalomeApp_Study.h>
#include <SALOMEDSClient_Study.hxx>
#include <SALOMEDSClient_SObject.hxx>

#include <LightApp_SelectionMgr.h>

#include <QAction>
#include <QIcon>
#include <QInputDialog>
#include <QLineEdit>
#include <QKeyEvent>
#include <QDockWidget>

static CORBA::ORB_var   _orb;

//=============================================================================
/*!
 *
 */
//=============================================================================
MedGUI::MedGUI() : LightApp_Module( "MED" ),SalomeApp_Module( "MED" )
{
}

void MedGUI::createMedAction( const int id, const QString& po_id, const QString& icon_id )
{
  QWidget* parent = application()->desktop();
  SUIT_ResourceMgr* mgr = application()->resourceMgr();

  QPixmap pix;
  QIcon icon;
  if( !icon_id.isEmpty() )
    pix = mgr->loadPixmap( "MED", tr( (const char*)icon_id.toLatin1() ) );
//   else
//     pix = mgr->loadPixmap( "MED", tr( QString( "ICO_" )+po_id ) );

  if ( !pix.isNull() )
    icon = QIcon( pix );

  createAction( id,
                tr( (const char*)("TOP_" + po_id).toLatin1() ),
                icon,
                tr( (const char*)("MEN_" + po_id).toLatin1() ),
                tr( (const char*)("STB_" + po_id).toLatin1() ),
                0,
                parent,
                false,
                this,
                SLOT( onGUIEvent() ) );

  if ( action( id ) )
    action( id )->setObjectName( QString( "Action %1" ).arg( id ) );
}

//=============================================================================
/*!
 *
 */
//=============================================================================
void MedGUI::initialize( CAM_Application* app )
{
  SalomeApp_Module::initialize( app );

  //QWidget* parent = application()->desktop();

  createMedAction( 931, "MESHSEL", "ICO_TB_MESHSEL" );
  createMedAction( 932, "FIELDSEL", "ICO_TB_FIELDSEL" );
  createMedAction( 934, "DUMPMESH" );
  createMedAction( 935, "DUMPSUBMESH" );
  createMedAction( 936, "EXPLORE", "ICO_TB_EXPLORE");

  createMedAction( 4031, "MESHSEL", "ICO_TB_MESHSEL" );
  createMedAction( 4032, "FIELDSEL", "ICO_TB_FIELDSEL" );
  createMedAction( 4034, "EXPLORE", "ICO_TB_EXPLORE");

  int MedId = createMenu( tr( "MED" ), -1, 50, 10 );
  createMenu( separator(), MedId, 10 );
  createMenu( 931, MedId, 11 );
  createMenu( 932, MedId, 11 );
  createMenu( 934, MedId, 11 );
  createMenu( 935, MedId, 11 );
  createMenu( 936, MedId, 11 );

  int medTb = createTool( tr( "TB_MED" ) );
  createTool( 4031, medTb );
  createTool( 4032, medTb );
  createTool( 4034, medTb );

  _data_base = new MEDGUIDataBaseDockWidget(application(),application()->desktop());
  application()->desktop()->addDockWidget(Qt::LeftDockWidgetArea,_data_base);
}

QString MedGUI::engineIOR() const
{
  QString anIOR( "" );
  SALOME_MED::MED_Gen_ptr aMedGen = InitMedGen();
  if ( !CORBA::is_nil( aMedGen) )
  {
    CORBA::String_var objStr = getApp()->orb()->object_to_string( aMedGen );
    anIOR = QString( objStr.in() );
  }
  return anIOR;
}

void MedGUI::windows( QMap<int, int>& mappa ) const
{
  mappa.clear();
  mappa.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
  mappa.insert( SalomeApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea );
}

//=============================================================================
/*!
 *
 */
//=============================================================================
#include <QMetaObject>
#include <QtxAction.h>
void MedGUI::onGUIEvent()
{
  const QtxAction* obj = qobject_cast<const QtxAction*>(sender());
  if ( !obj )
    return;
  int id = actionId( obj );
  if ( id != -1 )
    OnGUIEvent( id );
}

//=============================================================================
/*!
 *
 */
//=============================================================================
void MedGUI::EmitSignalCloseAllDialogs()
{
  emit SignalCloseAllDialogs();
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::deactivateModule( SUIT_Study* study )
{
  setMenuShown( false );
  setToolShown( false );

  _data_base->setVisible( false );
  _data_base->toggleViewAction()->setVisible( false );

  disconnect( application()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
             this, SLOT( onWindowActivated( SUIT_ViewWindow* ) ) );

  EmitSignalCloseAllDialogs();

  return SalomeApp_Module::deactivateModule( study );
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::activateModule( SUIT_Study* study )
{
  bool res = SalomeApp_Module::activateModule( study );

  setMenuShown( true );
  setToolShown( true );

  _data_base->setVisible( true );
  _data_base->toggleViewAction()->setVisible( true );

  connect( application()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
          this, SLOT( onWindowActivated( SUIT_ViewWindow* ) ) );
  return res;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
void MedGUI::onWindowActivated( SUIT_ViewWindow* )
{
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::OnGUIEvent (int theCommandID)
{
  setOrb();

  SalomeApp_Study* myActiveStudy = dynamic_cast< SalomeApp_Study* >( application()->activeStudy() );
  if( !myActiveStudy )
    return false;

  _PTR(Study) aStudy = myActiveStudy->studyDS();
  //SALOME_NamingService* myNameService = parent->getNameService();

  QString file;
  QStringList filtersList ;

  filtersList.append( tr("MED_MEN_IMPORT_MED") );
  filtersList.append( tr("MED_MEN_ALL_FILES") ) ;

  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( application() );
  if( !app )
    return false;

  switch (theCommandID)
    {
    case 4031:
    case 9031:
    case 931:
      {
        MESSAGE("command " << theCommandID << " activated");

        QString myStudyName = myActiveStudy->studyName();
        bool ok=FALSE;
//      int myStudyId = myActiveStudy->id();

        // load MED engine
        SALOME_MED::MED_Gen_ptr medgen = InitMedGen();

        // Selection du Fichier
        file = SUIT_FileDlg::getFileName(application()->desktop(),
                                        "",
                                        filtersList,
                                        tr("MED_MEN_IMPORT"),
                                        true);

        // Selection du Maillage
        if (!file.isEmpty() )
          {
            SCRUTE((const char*)file.toLatin1());
            QString meshName;
            meshName = QInputDialog::getText( application()->desktop(),
                                              tr("MED_INF_MESHNAME"),
                                              QString::null,
                                              QLineEdit::Normal,
                                              QString::null,
                                              &ok );
            if ( ok && !meshName.isEmpty() )
              {
                  try
                    {
                      medgen->readMeshInFile( (const char*)file.toLatin1(),
                                              (const char*)myStudyName.toLatin1(),
                                              (const char*)meshName.toLatin1() );
                      if (myActiveStudy->studyDS()->GetProperties()->IsLocked()) {
                        SUIT_MessageBox::warning ( application()->desktop(),
                                                   tr("WRN_WARNING"),
                                                   tr("WRN_STUDY_LOCKED") );
                        //QObject::tr("BUT_OK")); by default
                      }
                    }
                  catch (const SALOME::SALOME_Exception & S_ex)
                    {
                      SalomeApp_Tools::QtCatchCorbaException(S_ex);
                    }
                }
                updateObjBrowser();
          }
        break;
      }
    case 4032:
    case 9032:
    case 932:
      {
        MESSAGE("command " << theCommandID << " activated");

        QString myStudyName = myActiveStudy->studyName();
        bool ok=FALSE;
//      int myStudyId = myActiveStudy->id();

        // load MED engine
        SALOME_MED::MED_Gen_ptr medgen = InitMedGen();

        // Selection du Fichier
        QString anInitialPath = "";
        if ( SUIT_FileDlg::getLastVisitedPath().isEmpty() )
          anInitialPath = QDir::currentPath();

        file = SUIT_FileDlg::getFileName(application()->desktop(),
                                        anInitialPath,
                                        filtersList,
                                        tr("MED_MEN_IMPORT"),
                                        true);

        // Selection du Maillage
        if (!file.isEmpty() )
          {
            SCRUTE((const char*)file.toLatin1());
            QString fieldName;
            fieldName = QInputDialog::getText( application()->desktop(),
                                               tr("MED_INF_FIELDNAME"),
                                               QString::null,
                                               QLineEdit::Normal,
                                               QString::null,
                                               &ok );
            if ( ok && !fieldName.isEmpty())
              {
                try
                  {
                    medgen->readFieldInFile( (const char*)file.toLatin1(),
                                             (const char*)myStudyName.toLatin1(),
                                             (const char*)fieldName.toLatin1(),
                                             -1,
                                             -1 );
                    if (myActiveStudy->studyDS()->GetProperties()->IsLocked()) {
                      SUIT_MessageBox::warning ( application()->desktop(),
                                                 tr("WRN_WARNING"),
                                                 tr("WRN_STUDY_LOCKED") );
                      //tr("BUT_OK")); by default
                    }
                  }
                catch (const SALOME::SALOME_Exception & S_ex)
                  {
                    SalomeApp_Tools::QtCatchCorbaException(S_ex);
                  }
                updateObjBrowser ();
              }
          }
        break;
      }
    case 934:
      {
        //Handle(SMESH_TypeFilter) aMeshFilter = new SMESH_TypeFilter( MESH );

        SALOME_ListIO list;
        SalomeApp_Application* app = getApp();
        LightApp_SelectionMgr* mgr = app ? app->selectionMgr() : NULL;
        if( mgr )
          mgr->selectedObjects( list );
        //Sel->AddFilter(aMeshFilter) ;

        int nbSel = list.Extent();
        if ( nbSel == 1 )
          {
            //      SMESH::SMESH_Mesh_var aM;
            SALOME_MED::MESH_var aMesh;
            Handle(SALOME_InteractiveObject) IObject = list.First();
            if ( IObject->hasEntry() )
              {
                _PTR(SObject) aMorSM = aStudy->FindObjectID( IObject->getEntry() );
                if ( aMorSM )
                  {
                    _PTR(GenericAttribute) anAttr;
                    _PTR(AttributeIOR)     anIOR;
                    if (aMorSM->FindAttribute(anAttr, "AttributeIOR"))
                      {
                        anIOR = anAttr;
                        aMesh = SALOME_MED::MESH::_narrow( _orb->string_to_object(anIOR->Value().c_str()) );
                        if ( aMesh->_is_nil() )
                        {
                          SUIT_MessageBox::warning
                            ( application()->desktop(),
                              tr ("MED_WRN_WARNING"),
                              tr ("MED_INF_NOTIMPL") );
                          break;
                        }
                        DumpMesh( aMesh );
                        //Sel->ClearFilters() ;
                      }
                    else
                      {
                        SUIT_MessageBox::warning
                          ( application()->desktop(),
                            tr ("MED_WRN_WARNING"),
                            tr ("MED_INF_NOIOR") );
                        //tr ("MED_BUT_OK") ); by default
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

        SALOME_ListIO list;
        SalomeApp_Application* app = getApp();
        LightApp_SelectionMgr* mgr = app ? app->selectionMgr() : NULL;
        if( mgr )
          mgr->selectedObjects( list );

        //Sel->AddFilter(aSubMeshFilter) ;

        int nbSel = list.Extent();
        if ( nbSel == 1 )
          {
            //      SMESH::SMESH_subMesh_var aSubM;
            SALOME_MED::FAMILY_var aFam;
            Handle(SALOME_InteractiveObject) IObject = list.First();
            if ( IObject->hasEntry() )
              {
                _PTR(SObject) aMorSM = aStudy->FindObjectID( IObject->getEntry() );
                if ( aMorSM )
                  {
                    _PTR(GenericAttribute) anAttr;
                    _PTR(AttributeIOR)     anIOR;
                    if (aMorSM->FindAttribute(anAttr, "AttributeIOR"))
                      {
                        anIOR = anAttr;
                      }
                    else
                      {
                        SUIT_MessageBox::warning
                          ( application()->desktop(),
                            tr ("MED_WRN_WARNING"),
                            tr ("MED_INFNOIOR") );
                        //  tr ("MED_BUT_OK") ); by default
                        break;
                      }
                  }
              }
          }
        break;
      }
      case 936 :
      case 4034 :
        {
          
          MEDGUIFileContentDial* mfcd = new MEDGUIFileContentDial(_data_base, application()->desktop());
          mfcd->show();
          break;
        }
    }

  app->updateActions(); //SRN: To update a Save button in the toolbar

  return true;
}


//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::OnMousePress (QMouseEvent* pe ,
                           SUIT_ViewWindow* wnd )
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
                          SUIT_ViewWindow* wnd )
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
                         SUIT_ViewWindow* wnd)
{
  MESSAGE("MedGUI::OnKeyPress");

  return true;
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

  std::string name = MEDMesh->getName();
  SCRUTE(name);

  int dim2 = MEDMesh->getSpaceDimension();
  SCRUTE(dim2);

  int k = MEDMesh->getNumberOfNodes() ;
  SCRUTE(k);
  SALOME_TYPES::ListOfDouble_var coords = MEDMesh->getCoordinates( SALOME_MED::MED_FULL_INTERLACE );
  int i = 0;
  int lu = 0;
  while (lu < k ) {
    if (dim2==3)
      {
        MESSAGE ( " Coordinates  X = " << coords[i] << " Y = " << coords[i+1] << " Z = " << coords[i+2] );
        i = i + 3;
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
    std::string nomFam=Families[k]->getName();
    SCRUTE(nomFam);
    SALOME_TYPES::ListOfLong_var tabnoeuds=Families[k]->getNumber(SALOME_MED::MED_NONE);
    for (int l=0;l<(int)tabnoeuds->length();l++)
      SCRUTE(tabnoeuds[l]);
  }

  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::DumpSubMesh( SALOME_MED::FAMILY_var Fam )
{

  if ( Fam->_is_nil() )
    return false;

  SALOME_TYPES::ListOfLong_var tabnoeuds=Fam->getNumber(SALOME_MED::MED_NONE);
  for (int l=0;l<(int)tabnoeuds->length();l++)
    SCRUTE(tabnoeuds[l]);

  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
SALOME_MED::MED_Gen_ptr MedGUI::InitMedGen() const
{
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( application() );
  Engines::EngineComponent_var comp =
    SALOME_LifeCycleCORBA(app->namingService()).FindOrLoad_Component( "FactoryServer", "MED" );

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

extern "C" {
  Standard_EXPORT CAM_Module* createModule() {
    return new MedGUI();
  }
  
  Standard_EXPORT char* getModuleVersion() {
      return (char*)SALOMEMED_VERSION_STR;
  }
}
