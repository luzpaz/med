// Programme de test des operations sur les champs

#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"

#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_define.hxx"

double myfunction1(double x)
{
    return 0.25*(x-1.0);
}


using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

void affiche_field_(FIELD_ * myField, const SUPPORT * mySupport)
{
  cout << "Field "<< myField->getName() << " : " <<myField->getDescription() <<  endl ;
  int NumberOfComponents = myField->getNumberOfComponents() ;
  cout << "- Nombre de composantes : "<< NumberOfComponents << endl ;
  cout << "- Nombre de valeurs     : "<< myField->getNumberOfValues() << endl ;
  for (int i=1; i<NumberOfComponents+1; i++) {
    cout << "  - composante "<<i<<" :"<<endl ;
    cout << "      - nom         : "<<myField->getComponentName(i)<< endl;
    cout << "      - description : "<<myField->getComponentDescription(i) << endl;
    cout << "      - units       : "<<myField->getMEDComponentUnit(i) << endl;
  }
  cout << "- iteration :" << endl ;
  cout << "    - numero : " << myField->getIterationNumber()<< endl  ;
  cout << "    - ordre  : " << myField->getOrderNumber()<< endl  ;
  cout << "    - temps  : " << myField->getTime()<< endl  ;

  cout << "- Type : " << myField->getValueType()<< endl;

  cout << "- Adresse support : " << mySupport << endl;
}

void affiche_fieldT(FIELD<double> * myField, const SUPPORT * mySupport)
{
  affiche_field_((FIELD_ *) myField, mySupport);

  cout << "- Valeurs :"<<endl;
  int NumberOf = mySupport->getNumberOfElements(MED_ALL_ELEMENTS);
  int NumberOfComponents = myField->getNumberOfComponents() ;

  for (int i=1; i<NumberOf+1; i++) {
    const double * value = myField->getValueI(MED_FULL_INTERLACE,i) ;
    for (int j=0; j<NumberOfComponents; j++)
      cout << value[j]<< " ";
    cout<<endl;
  }
  cout << endl;
  cout << "Norme euclidienne : " << myField->norm2() << endl;
  cout << "Norme max         : " << myField->normMax() << endl;
  try
  {
      for (int i=1; i<=myField->getNumberOfComponents(); ++i)
	    cout << "Norme L2 - comp=" << i << " : " << myField->normL2(i) << endl;
      cout << "Norme L2          : " << myField->normL2() << endl;

      for (int i=1; i<=myField->getNumberOfComponents(); ++i)
	    cout << "Norme L1 - comp=" << i << " : " << myField->normL1(i) << endl;
      cout << "Norme L1          : " << myField->normL1() << endl;
  }
  catch (MEDEXCEPTION &ex)
  {
      cout << ex.what() << endl;
  }
}

void affiche_valeur_field(const FIELD<double>& f)
{
    const int tailleMax=12;
    const int taille=f.getNumberOfValues()*f.getNumberOfComponents();
    const double * value=f.getValue(f.getvalue()->getMode());
    if(taille<=tailleMax)
	for(int i=0;i<taille;i++)
	    cout << setw(3) << value[i] << " ";
    else
    {
	for(int i=0; i<tailleMax/2; ++i)
	    cout << setw(3) << value[i] << " ";
	cout << "    ...    ";
	for(int i=taille-tailleMax/2 ; i<taille; ++i)
	    cout << setw(3) << value[i] << " ";
    }
}

void checkOperation(const FIELD<double>& resOp, const FIELD<double>& f1, const FIELD<double>& f2, 
	char Op, const char* intitule, int verbose)
{
    int res=0;

    // get pointers to inside arrays of values
    medModeSwitch mode=resOp.getvalue()->getMode();
    const double * value=resOp.getValue(mode);
    const double * value1=f1.getValue(mode);
    const double * value2=f2.getValue(mode);
    const int size=f1.getNumberOfValues()*f1.getNumberOfComponents(); // size of field1
 
    // check size compatibility
    if(f1.getNumberOfValues()*f1.getNumberOfComponents()!=size ||
	    resOp.getNumberOfValues()*resOp.getNumberOfComponents()!=size)
	res=1;

    if(!res)
    {
	switch(Op)
	{
	    case '+':
		for(int i=0; i!=size; ++i)
		    if(value[i]!=value1[i]+value2[i])	
			res+=1;
		break;
	    case '-':
		for(int i=0; i!=size; ++i)
		    if(value[i]!=value1[i]-value2[i])	
			res+=1;
		break;
	    case 'n':
		for(int i=0; i!=size; ++i)
		    if(value[i]!=-value1[i])
			res+=1;
		break;
	    case '*':
		for(int i=0; i!=size; ++i)
		    if(value[i]!=value1[i]*value2[i])	
			res+=1;
		break;
	    case '/':
		for(int i=0; i!=size; ++i)
		    if(value2[i]!=0.0)
			if(value[i]!=value1[i]/value2[i])	
			    res+=1;
		break;
	    case '=':
		for(int i=0; i!=size; ++i)
		    if(value[i]!=value2[i])
			res+=1;
		break;
	    case 'a':
		for(int i=0; i!=size; ++i)
		    if(value[i]!=value1[i]+value2[i]*value2[i])	
			res+=1;
		break;
	}
	    
    }

    if (verbose)
	cout << endl << intitule << "[";
    cout << res;
    if (verbose)
    {
	cout << "] : ";
	affiche_valeur_field(resOp);
    }
    else 
	cout << endl;
}

int main (int argc, char ** argv)
{
    /* process the arguments */
    int verbose=0;  //  verbose=1 if the verbose mode is selected
    int res=0; // unit test result
    int ntest=0;  // numéro du test

    if (argc>=2 && !strcmp(argv[1],"-v"))
	verbose=1;

    if (argc != 4+verbose) 
    {
	cerr << "Usage : " << argv[0] 
	<< "[-v] filename meshname fieldname" << endl << endl
	<< "-> tests field's operations on the FIELD<double> fieldname" << endl
	<< "Use optional option -v to select verbose mode" << endl;
	exit(-1);
    }

    string filename  = argv[verbose+1];
    string meshname  = argv[verbose+2];
    string fieldname = argv[verbose+3];

    /* read MESH, SUPPORT and FIELDS */
    MESH * myMesh = new MESH(MED_DRIVER,filename,meshname);
    SUPPORT * mySupport;
    FIELD<double> * myField1;
    try
    {
	mySupport = new SUPPORT(myMesh,"Support on all Cells",MED_CELL);
	myField1 = new FIELD<double>(mySupport,MED_DRIVER,filename,fieldname) ;
	myField1->setValueType(MED_REEL64);
    }
    catch (MEDEXCEPTION &ex)
    {
	// field wasn't found on cells, try on nodes
	delete mySupport ;
	mySupport = new SUPPORT(myMesh,"On_all_node",MED_NODE);
	try 
	{
	    myField1 = new FIELD<double>(mySupport,MED_DRIVER,filename,fieldname) ;
	}
	catch (...) 
	{
	    cout << "Field double " << fieldname << " not found !!!" << endl ;
	    exit (-1) ;
	}
    }
    FIELD<double> * myField2 = new FIELD<double>(* myField1);
    FIELD<double> myFieldPlus = *myField1 + *myField2;
    if(verbose)
    {
	// affichage des nprmes,des champs f1, f2, scalarProduct(f1,f2) et f1+f2
	FIELD<double>* myField1_vol=myField1->getSupport()->getMesh()->getVolume(myField1->getSupport());
	cout << "Norme L2 calculée en fournissant le volume : " << myField1->normL2(myField1_vol) << endl;
        for (int i=1; i<=myField1->getNumberOfComponents(); ++i)
	    cout << "Norme L2 - comp=" << i << " : " << myField1->normL2(i,myField1_vol) << endl;
	cout << "Norme L1 calculée en fournissant le volume : " << myField1->normL1(myField1_vol) << endl;
        for (int i=1; i<=myField1->getNumberOfComponents(); ++i)
	    cout << "Norme L1 - comp=" << i << " : " << myField1->normL1(i,myField1_vol) << endl;
	delete myField1_vol;

	affiche_fieldT(myField1, myField1->getSupport());
	cout <<  endl << string(60,'-') << endl;
	affiche_fieldT(myField2, myField2->getSupport());
	cout << endl << string(60,'-') << endl;
	
	FIELD<double>* myFieldDot = FIELD<double>::scalarProduct(*myField1, *myField2);
	affiche_fieldT(myFieldDot, myFieldDot->getSupport());
	delete myFieldDot;
	cout <<  endl << string(60,'-') << endl ;
	affiche_fieldT(&myFieldPlus, myFieldPlus.getSupport());
	cout <<  endl << string(60,'-') << endl << endl ;
    }


    // Verifie plusieurs cas de non compatibilité 

    // test 1 : Unites non compatibles
    const string unite=myField1->getMEDComponentUnit(1);
    myField1->setMEDComponentUnit(1,string("UniteBidon"));
    ntest++; res=1;
    try
    {
	FIELD<double> myFieldPlus = *myField1 + *myField2;
	if(verbose) 
	{
	    cout << endl << string(60,'-') << endl;
	    cout<< "Test " << ntest << " : incompatibilité d'unité : " << endl << endl;
	}
    }
    catch (MEDEXCEPTION & ex)
    {
	res=0;
	if(verbose) 
	    cout << ex.what() << endl;
	myField1->setMEDComponentUnit(1,unite);
    }
    cout << res << endl;

    // test 2 : numberOfComponents non compatibles
    const int numberOfComponents =myField1->getNumberOfComponents();
    myField1->setNumberOfComponents(13);
    ntest++; res=1;
    try
    {
	if(verbose) 
	{
	    cout << endl << string(60,'-') << endl;
	    cout<< "Test " << ntest << " : incompatibilité nombre de composantes : " << endl << endl;
	}
	FIELD<double> myFieldPlus = *myField1 + *myField2;
    }
    catch (MEDEXCEPTION & ex)
    {
	res=0;
	if(verbose)
	    cout << endl << ex.what() << endl << endl;
	myField1->setNumberOfComponents(numberOfComponents);
    }
    cout << res << endl;

    // test 3 : supports non compatibles
    const SUPPORT mySupport2(myMesh,"On_all_node",MED_NODE);
    myField1->setSupport(&mySupport2);
    ntest++; res=1;
    try
    {
	if(verbose)
	    cout << endl << string(60,'-') << endl << "Test " << ntest << " : incompatibilité des supports"  << endl << endl;
	FIELD<double> myFieldPlus = *myField1 + *myField2;
    }
    catch (MEDEXCEPTION & ex)
    {
	res=0;
	if(verbose)
	    cout << ex.what() << endl << endl << endl;
	myField1->setSupport(mySupport);
    }
    cout << res << endl;

    // test 4 : champs de taille nulle
    myField1->setNumberOfComponents(0);
    myField2->setNumberOfComponents(0);
    ntest++; res=2;
    try
    {
	if(verbose)
	    cout<< endl << string(60,'-') << endl << "Test " << ntest << " : incompatibilité taille nulle" << endl << endl;
	FIELD<double> myFieldPlus = *myField1 + *myField2;
    }
    catch (MEDEXCEPTION & ex)
    {
	--res;
	if(verbose) 
	    cout << ex.what() << endl << endl ;
    }
    try
    {
	double mynorm2=myField1->norm2();
    }
    catch (MEDEXCEPTION & ex)
    {
	--res;
	if(verbose) 
	    cout << ex.what() << endl << endl ;
	myField1->setNumberOfComponents(numberOfComponents);
	myField2->setNumberOfComponents(numberOfComponents);
    }
    cout << res << endl;

    // Apres toutes ces exceptions, des opérations qui marchent!

    if(verbose)
    {
	cout<< endl << string(60,'-') << endl << "Test " << ++ntest << " : Operations arithmétiques" << endl;
	cout << endl << " f1           : "; affiche_valeur_field(*myField1);
	cout << endl << " f2           : "; affiche_valeur_field(*myField2);
	cout  << endl << string(140,'-');
    }

    // Test du résultats de certaines opérations et affichage si verbose
    checkOperation(myFieldPlus, *myField1, *myField2, '+', " f1+f2    ", verbose);
    FIELD<double>* myFieldadd = FIELD<double>::add(*myField1, *myField2);
    checkOperation( *myFieldadd, *myField1, *myField2, '+', "add(f1,f2)", verbose);
    delete myFieldadd;

    FIELD<double> myFieldMoins = *myField1 - *myField2;
    checkOperation(myFieldMoins, *myField1, *myField2, '-', " f1-f2    ", verbose);
    FIELD<double>* myFieldsub = FIELD<double>::sub(*myField1, *myField2);
    checkOperation( *myFieldsub, *myField1, *myField2, '-', "sub(f1,f2)", verbose);
    delete myFieldsub;
    FIELD<double> myFieldNeg = -(*myField1);
    checkOperation(myFieldNeg, *myField1, *myField1, 'n', " -f1      ", verbose);
    
    FIELD<double> myFieldFois = *myField1 * *myField2;
    checkOperation(myFieldFois, *myField1, *myField2, '*', " f1*f2    ", verbose);
    FIELD<double>* myFieldmul = FIELD<double>::mul(*myField1, *myField2);
    checkOperation( *myFieldmul, *myField1, *myField2, '*', "mul(f1,f2)", verbose);
    
    FIELD<double> myFieldDiv = *myField1 / *myField2;
    checkOperation(myFieldDiv, *myField1, *myField2, '/', " f1/f2    ", verbose);
    FIELD<double>* myFielddiv = FIELD<double>::div(*myField1, *myField2);
    checkOperation( *myFielddiv, *myField1, *myField2, '/', "div(f1,f2)", verbose);
    delete myFielddiv;

    FIELD<double> myFieldAsso = (*myField1)+(*myField2)*(*myField2);
    checkOperation(myFieldAsso, *myField1, *myField2, 'a', " f1+f2*f2 ", verbose);

    myField1->applyLin(4.0,1.0);
    checkOperation(*myField1, *myField2, *myField2, 'l', " 4.f1 + 1 ", verbose);
    myField1->applyFunc<myfunction1>();
    checkOperation( *myField1, *myField2, *myField1, '=', "CB : ->f1)", verbose);

    *myField1 += *myField2;
    checkOperation(*myField1, *myField2, *myField2, '+', " f1+=f2   ", verbose);

    *myField1 -= *myField2;
    checkOperation(*myField1, *myField2, *myField2, '=', " f1-=f2   ", verbose);

    *myField1 *= *myField2;
    checkOperation(*myField1, *myField2, *myField2, '*', " f1*=f2   ", verbose);
    *myField1 /= *myField2;
    checkOperation(*myField1, *myFieldmul, *myField2, '/', " f1/=f2   ", verbose);
    delete myFieldmul;


    delete myField1;
    delete myField2;
    delete mySupport ;
    delete myMesh ;
    return 0;
}
