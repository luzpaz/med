//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// Partitioning/decimation module for the SALOME v3.2 platform
//
/**
 * \file    multipr.cxx
 *
 * \brief   Standalone command line application of the MULTIPR component.
 *          The application aims to reduce large data set to allow interactive visualization.
 *          Its two main function are the following:
 *          1. Splits any TETRA10 mesh contained in a MED file.
 *             a. Builds a distributed MED file by extracting all the groups from a sequential MED file.
 *             b. Splits a part of distributed MED file.
 *          2. Decimates fields to produce multi-resolution data set.
 *
 *          Return 0 if application succeed, 1 otherwise (failure).
 *
 *          See http://www.salome-platform.org to learn more about Salome or MED.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#include "MULTIPR_API.hxx"
#include "MULTIPR_Obj.hxx"
#include "MULTIPR_Mesh.hxx"
#include "MULTIPR_MeshDis.hxx"
#include "MULTIPR_Exceptions.hxx"
#include "MULTIPR_Utils.hxx"

#include <iostream>
#include <vector>
#include <string>

using namespace std;


// This command line application can use 2 differents API to do the same work.
// If MULTIPR_USE_OBJ_API is defined then this command line application used the MULTIPR_Obj API;
// otherwise, it uses the MULTIPR_API
//#define MULTIPR_USE_OBJ_API


const int MULTIPR_APP_OK     = 0;
const int MULTIPR_APP_FAILED = 1;


/**
 * Enumerates all the usages of this application.
 */
enum Usage
{
    MULTIPR_USAGE_UNKNOWN,
    MULTIPR_USAGE_DISPLAY_HELP,
    MULTIPR_USAGE_AUTOTEST,
    MULTIPR_USAGE_PARTITION1,
    MULTIPR_USAGE_PARTITION2,
    MULTIPR_USAGE_DECIMATION,
    MULTIPR_USAGE_MERGE,
    MULTIPR_USAGE_INFO
};


/**
 * Enumerates all the possible errors.
 */
enum Error
{
    MULTIPR_APP_NO_ERROR,
    MULTIPR_APP_UNKNOWN_USAGE,
    MULTIPR_APP_WRONG_NUMBER_OF_ARGUMENTS,
    MULTIPR_APP_ILLEGAL_ARGUMENT,
    MULTIPR_APP_FILE_NOT_FOUND,
    MULTIPR_APP_IO_ERROR,
    MULTIPR_APP_UNDEFINED_ERROR
};


// global variables used to configure the application
int   g_usage               = 0; 
int   g_errorCode           = MULTIPR_APP_NO_ERROR;
char* g_medFilename         = NULL;
char* g_meshName            = NULL;
char* g_partName            = NULL;
int   g_nbParts             = 0;
int   g_splitter            = multipr::MULTIPR_METIS;
char* g_filterName          = NULL;
char* g_fieldName           = NULL;
int   g_fieldTimeStepIt     = 0;
float g_decimThresholdMed   = 0.0f;
float g_decimThresholdLow   = 0.0f;
float g_decimRadius         = 0.0f;
int   g_boxing              = 100;
char  g_params[1024]            = "";

// merge
std::vector<std::string> g_medFilenameSrc;
char* g_medFilenameDst      = NULL;


/**
 * \fn     const char* getUsage(int usage)
 * \brief  returns a string describing the usage of this application.
 */
const char* getUsage(int usage)
{
    switch(usage)
    {
        case MULTIPR_USAGE_DISPLAY_HELP:
            return "--help: display help";
            
        case MULTIPR_USAGE_AUTOTEST:
            return "--auto: perform all unit tests";

        case MULTIPR_USAGE_PARTITION1:
            return "--part1: extract all groups from a sequential MED file";

        case MULTIPR_USAGE_PARTITION2:
            return "--part2: split a part of a distributed MED file";

        case MULTIPR_USAGE_DECIMATION:
            return "--decim: generated level of details from a part of a distributed MED file";
        
        case MULTIPR_USAGE_MERGE:
            return "--merge: merge two sequential MED file if possible";    
            
        case MULTIPR_USAGE_INFO:
            return "--info: prints all infos about a mesh in a sequential MED file";

        default:
            return "unknown";
    }
}


/**
 * \fn     void printDescription()
 * \brief  prints a short description of this application.
 */
void printDescription()
{
    cout << "Keywords:" << endl;          
    cout << "    Post-processing numerical simulation, Salome platform, " << endl;
    cout << "    Large data set visualization, 3D meshes and fields" << endl;
    cout << "Description:" << endl;
    cout << "    multipr is a partitionning/decimation tool of MED files." << endl;
    cout << "    See http://www.salome-platform.org for information about MED or Salome." << endl;
    cout << "    Note: current version only accept TETRA10 meshes." << endl;
}


/**
 * \fn     void printUsage()
 * \brief  prints "how to use" manual of this tools.2NbPart
 */
void printUsage()
{
    cout << "Usages:" << endl;
    cout << "    --auto   Autotest: performs some unit tests on the MULTIPR API" << endl;
    cout << "                 * Usage: --auto path (path where to find test file \"agregat100grains_12pas.med\")" << endl;
    cout << "    --part1  Extracts all groups from a sequential MED file (V2.2 or higher)" << endl;
    cout << "                 * Usage: --part1 file.med meshName" << endl; 
    cout << "    --part2  Split a group of a distributed MED file (V2.3) produced with --part1" << endl;
    cout << "                 * Usage: --part2 file.med partName nbParts [splitter (0=METIS 1=SCOTCH)]" << endl;
    cout << "    --decim  Generates 3 level of details (full, medium and low) from a part" << endl;
    cout << "             of a distributed MED file (V2.3)" << endl;     
    cout << "                 * Usage: --decim file.med partName fieldName fieldIt filterName [...]" << endl;
    cout << "                 * Only one filter is currently available: Filtre_GradientMoyen" << endl;
    cout << "                 * Usage: --decim file.med partName fieldName fieldIt Filtre_GradientMoyen m l radius" << endl;
    cout << "                   where m=threshold for medium res. and l=threshold for low res.; assume m < l" << endl;
    cout << "    --info   Dumps all infos related to a mesh in a sequential MED file" << endl;
    cout << "                 * Usage: --info file.med [meshName]" << endl;
    cout << "    --help   Displays this help page" << endl;
    cout << endl;
}


/**
 * \fn     void printGlobals()
 * \brief  print current state of all global variables.
 */
void printGlobals()
{
    cout << endl;
    cout << "********************************************************************************" << endl;
    cout << "CONFIGURATION" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    cout << "Mode                        : " << getUsage(g_usage) << endl;
    cout << "Med filename                : " << ((g_medFilename != NULL) ? g_medFilename : "UNDEFINED") << endl;
    cout << "Mesh name                   : " << ((g_meshName != NULL) ? g_meshName : "UNDEFINED") << endl;
    cout << "Part name                   : " << ((g_partName != NULL) ? g_partName : "UNDEFINED") << endl;
    cout << "Nb parts                    : "  << g_nbParts << endl;
    cout << "Decimation:" << endl;
    cout << "    Field name              : " << ((g_fieldName != NULL) ? g_fieldName : "UNDEFINED") << endl;
    cout << "    Time step iteration     : " << g_fieldTimeStepIt << endl;
    cout << "    Filter name             : " << ((g_filterName != NULL) ? g_filterName : "UNDEFINED") << endl;
    cout << "    Threshold for med. res. : " << g_decimThresholdMed << endl;
    cout << "    Threshold for low res.  : " << g_decimThresholdLow << endl;
    cout << "    Radius                  : " << g_decimRadius << endl;
    cout << "    Boxing                  : " << g_boxing << endl;
    cout << "********************************************************************************" << endl;
    cout << endl;
}


/**
 * \fn     const char* getErrorMsg()
 * \brief  returns the error message corresponding to current error code.
 * \return the current error message.
 */
const char* getErrorMsg()
{
    switch (g_errorCode)
    {
        case MULTIPR_APP_NO_ERROR:
            return "no error";
        
        case MULTIPR_APP_UNKNOWN_USAGE:
            return "unknown usage";
            
        case MULTIPR_APP_WRONG_NUMBER_OF_ARGUMENTS:
            return "wrong number of arguments";
        
        case MULTIPR_APP_ILLEGAL_ARGUMENT:
            return "illegal argument";
        
        case MULTIPR_APP_FILE_NOT_FOUND:
            return "file not found";
            
        case MULTIPR_APP_IO_ERROR:
            return "i/o error";
            
        default:
            return "error (undefined)";
    }
}


/**
 * \fn     void parseCommandLine(int argc, char** argv)
 * \brief  parses the command line and configure this application.
 * \param  argc number of arguments.
 * \param  argv array of arguments.
 */
void parseCommandLine(int argc, char** argv)
{
    if (argc == 1)
    {
        g_usage = MULTIPR_USAGE_UNKNOWN; 
        return;
    }
    
    if (strcmp(argv[1],"--help") == 0)
    {
        g_usage = MULTIPR_USAGE_DISPLAY_HELP;
    }
    else if (strcmp(argv[1],"--auto") == 0)
    {
        if (argc != 3)
        {
            g_usage = MULTIPR_USAGE_UNKNOWN; 
            g_errorCode = MULTIPR_APP_WRONG_NUMBER_OF_ARGUMENTS;
        }
        else
        {
            g_usage = MULTIPR_USAGE_AUTOTEST;
            g_medFilename = argv[2];
        }
    }
    else if (strcmp(argv[1],"--part1") == 0)
    {
        if (argc != 4)
        {
            g_usage = MULTIPR_USAGE_UNKNOWN; 
            g_errorCode = MULTIPR_APP_WRONG_NUMBER_OF_ARGUMENTS;
        }
        else
        {
            g_usage       = MULTIPR_USAGE_PARTITION1;
            g_medFilename = argv[2];
            g_meshName    = argv[3];
        }
    }
    else if (strcmp(argv[1],"--part2") == 0)
    {
        if ((argc != 5) && (argc != 6))
        {
            g_usage = MULTIPR_USAGE_UNKNOWN; 
            g_errorCode = MULTIPR_APP_WRONG_NUMBER_OF_ARGUMENTS;
        }
        else
        {
            g_usage       = MULTIPR_USAGE_PARTITION2; 
            g_medFilename = argv[2];
            g_partName    = argv[3];
            g_nbParts     = atoi(argv[4]);
            
            if (argc == 6)
            {
                g_splitter = atoi(argv[5]);
            }
        }
    }
    else if (strcmp(argv[1],"--decim") == 0)
    {
        if (argc < 9)
        {
            g_usage = MULTIPR_USAGE_UNKNOWN; 
            g_errorCode = MULTIPR_APP_WRONG_NUMBER_OF_ARGUMENTS;
                        return ;
        }
        else
        {
            g_usage             = MULTIPR_USAGE_DECIMATION; 
            g_medFilename       = argv[2];
            g_partName          = argv[3];
            g_fieldName         = argv[4];
            g_fieldTimeStepIt   = atoi(argv[5]);
            g_filterName        = argv[6];
            g_decimThresholdMed = atof(argv[7]);
            g_decimThresholdLow = atof(argv[8]);
        }
                if (strcmp(g_filterName, "Filtre_GradientMoyen") == 0 && argc == 11)
                {
            g_decimRadius = atof(argv[9]);
            g_boxing = atoi(argv[10]);
                        sprintf(g_params, "%lf %lf %lf %d", g_decimThresholdMed, g_decimThresholdLow, g_decimRadius, g_boxing);
                }
                else if (strcmp(g_filterName, "Filtre_Direct") == 0)
                {
                        sprintf(g_params, "%lf %lf", g_decimThresholdMed, g_decimThresholdLow);
                }
                else
                {
                        g_usage = MULTIPR_USAGE_UNKNOWN; 
                        g_errorCode = MULTIPR_APP_ILLEGAL_ARGUMENT;
                }
                
    }
    else if (strcmp(argv[1],"--merge") == 0)
    {
        if (argc < 7)
        {
            g_usage = MULTIPR_USAGE_UNKNOWN; 
            g_errorCode = MULTIPR_APP_WRONG_NUMBER_OF_ARGUMENTS;
        }
        else
        {
            g_usage             = MULTIPR_USAGE_MERGE; 
            for (int i = 2 ; i < argc-3 ; i++)
            {
                g_medFilenameSrc.push_back(argv[i]);
            }
            g_meshName          = argv[argc-3];
            g_fieldName         = argv[argc-2];
            if (strcmp(g_fieldName, "NULL") == 0) g_fieldName = NULL;
            g_medFilenameDst    = argv[argc-1];
        }
    }
    else if (strcmp(argv[1],"--info") == 0)
    {
        if ((argc != 3) && (argc != 4))
        {
            g_usage = MULTIPR_USAGE_UNKNOWN; 
            g_errorCode = MULTIPR_APP_WRONG_NUMBER_OF_ARGUMENTS;
        }
        else
        {
            g_usage        = MULTIPR_USAGE_INFO; 
            g_medFilename  = argv[2];
            
            if (argc == 4)
            {
                g_meshName = argv[3];
            }
        }
    }
    else
    {
        g_usage = MULTIPR_USAGE_UNKNOWN;
        g_errorCode = MULTIPR_APP_UNKNOWN_USAGE;
    }
}


/**
 * \fn     int runAutotest()
 * \brief  performs some unit tests on the MULTIPR API. 
 * \return MULTIPR_APP_OK if successful, MULTIPR_APP_FAILED if failure.
 */
int runAutotest()
{
    cout << "Start autotest..." << endl;
    
    int ret = MULTIPR_APP_OK;
    int nbPartsSupp = 100;
    try
    {
        string strMEDfilename = g_medFilename;
        strMEDfilename += "/agregat100grains_12pas.med";
        
        cout << "Test file: " << strMEDfilename << endl << endl;
        
        //---------------------------------------------------------------------
        // Test partionneDomaine() = extract groups from a sequential MED file
        //---------------------------------------------------------------------
        multipr::partitionneDomaine(strMEDfilename.c_str(), "MAIL"); 
        
        //---------------------------------------------------------------------
        // Test partitionneGroupe() = split a group from a distributed MED file
        // using MEDSPLITTER (METIS)
        //---------------------------------------------------------------------
        string strDistributedMEDfilename = g_medFilename;
        strDistributedMEDfilename += "/agregat100grains_12pas_groupes_maitre.med";
        
        multipr::partitionneGroupe(
            strDistributedMEDfilename.c_str(), 
            "MAIL_1", 
            4, 
            multipr::MULTIPR_SCOTCH);
        nbPartsSupp += 4;
        
        multipr::partitionneGroupe(
            strDistributedMEDfilename.c_str(), 
            "MAIL_97", 
            3, 
            multipr::MULTIPR_METIS);
        nbPartsSupp += 3;
        
        //---------------------------------------------------------------------
        // Test decimePartition() = generate 2 lower resolution of a mesh
        // using decimation based on gradient
        //---------------------------------------------------------------------
        multipr::decimePartition(
            strDistributedMEDfilename.c_str(), 
            "MAIL_98",
            "SIG_____SIEF_ELGA_______________",
            12,
            "Filtre_GradientMoyen",
            "10.0 25.0 0.3 100");
        nbPartsSupp += 2;
        
        multipr::decimePartition(
            strDistributedMEDfilename.c_str(), 
            "MAIL_92",
            "SIG_____SIEF_ELGA_______________",
            11,
            "Filtre_GradientMoyen",
            "10.0 25.0 0.5 10");
        nbPartsSupp += 2;
        
        multipr::decimePartition(
            strDistributedMEDfilename.c_str(), 
            "MAIL_97_2",
            "SIG_____SIEF_ELGA_______________",
            10,
            "Filtre_GradientMoyen",
            "10.0 25.0 0.4 20");
        nbPartsSupp += 2;
        
        //---------------------------------------------------------------------
        // Test passed: OK!
        //---------------------------------------------------------------------
        cout << endl;
        
        // Check quantity of parts at the end
        multipr::MeshDis meshDis;
        meshDis.readDistributedMED(strDistributedMEDfilename.c_str());
        int nbParts = meshDis.getNumParts();
        cout << "Auto Test: final quantity of parts is " << nbParts
             << " (must be " << nbPartsSupp << ")" << endl;
        if (nbParts != nbPartsSupp)
        {
          cout << "Test failed: wrong quantity of parts obtained!" << endl;
          cout << "Failure" << endl << endl;
          ret = MULTIPR_APP_FAILED;
        }
        else
        {
          cout << "Test passed: everything seems to be OK" << endl;
          cout << "OK" << endl << endl;
        }
    }
    catch (multipr::RuntimeException& e)
    {
        e.dump(cout);
        cout << endl;
        cout << "Test failed" << endl;
        cout << "Failure" << endl << endl;
        ret = MULTIPR_APP_FAILED;
    }
    
    return ret;
}


/**
 * \fn     void runPartition1()
 * \brief  builds a distributed MED file (v2.3) by extracting groups from a sequential MED file. 
 * \return MULTIPR_APP_OK if successful, MULTIPR_APP_FAILED if failure.
 */
int runPartition1()
{
    int ret = MULTIPR_APP_OK;
    try
    {
        multipr::partitionneDomaine(g_medFilename, g_meshName);    
    }
    catch (multipr::RuntimeException& e)
    {
        e.dump(cout);
        ret = MULTIPR_APP_FAILED;
    }
        
    return ret;
}


/**
 * \fn     void runPartition2()
 * \brief  builds a distributed MED file (v2.3) by splitting a part of a distributed MED file generated by runPartition1().  
 * \return MULTIPR_APP_OK if successful, MULTIPR_APP_FAILED if failure.
 */
int runPartition2()
{    
    int ret = MULTIPR_APP_OK;
    try
    {
        multipr::partitionneGroupe(
            g_medFilename, 
            g_partName, 
            g_nbParts, 
            g_splitter);
    }
    catch (multipr::RuntimeException& e)
    {
        e.dump(cout);
        ret = MULTIPR_APP_FAILED;
    }
        
    return ret;
}


/**
 * \fn     int runDecimation()
 * \brief  creates 3 resolutions of a part of a distributed MED file. 
 * \return MULTIPR_APP_OK if successful, MULTIPR_APP_FAILED if failure.
 */
int runDecimation()
{
    int         ret = MULTIPR_APP_OK;
    try
    {
        multipr::decimePartition(
            g_medFilename, 
            g_partName,
            g_fieldName,
            g_fieldTimeStepIt,
            g_filterName,
            g_params);
    }
    catch (multipr::RuntimeException& e)
    {
        e.dump(cout);
        ret = MULTIPR_APP_FAILED;
    }
        
    return ret;
}


/**
 * \fn     int runMerge()
 * \brief  merges two sequential MED file if possible.
 * \return MULTIPR_APP_OK if successful, MULTIPR_APP_FAILED if failure.
 */
int runMerge()
{
    int ret = MULTIPR_APP_OK;
    try
    {

                multipr::merge(
            g_medFilenameSrc, 
            g_meshName,
            g_fieldName,
            g_medFilenameDst);
    }
    catch (multipr::RuntimeException& e)
    {
        e.dump(cout);
        ret = MULTIPR_APP_FAILED;
    }
        
    return ret;
}


/**
 * \fn     int runDumpMED()
 * \brief  dumps info about a sequential MED file.
 * \return MULTIPR_APP_OK if successful, MULTIPR_APP_FAILED if failure.
 */
int runDumpMED()
{
#ifdef MULTIPR_USE_OBJ_API
    int ret = MULTIPR_APP_OK;
    try
    {
        // if mesh is unknown, then list all the meshes in the given MED file
        if (g_meshName == NULL)
        {
            multipr::Obj obj;
            obj.create(g_medFilename);
            
            {
                // display list of meshes contained in the MED file
                vector<string> res = obj.getMeshes();
                cout << "List of meshes in this MED file:" << endl;
                for (unsigned i = 0 ; i < res.size() ; i++)
                {
                    cout << "Mesh " << (i + 1) << ": \"" << res[i] << "\"" << endl;
                }
            }
            
            cout << endl;
            
            {
                // display list of fields contained in the MED file
                vector<string> names = obj.getFields();
                cout << "List of scalar fields in this MED file:" << endl;
                for (unsigned i = 0 ; i < names.size() ; i++)
                {
                    cout << "Field " << (i + 1) << ": \"" << names[i] << "\"  #Time stamps=" << obj.getTimeStamps(names[i].c_str()) << endl;
                }
            }
        }
        else
        {
            // display all infos about one mesh in a MED file
            multipr::Mesh mesh;
            mesh.readSequentialMED(g_medFilename, g_meshName);
            mesh.setPrintAll(true);
            cout << mesh << endl;
        }
        cout << "OK" << endl;
    }
    catch (multipr::RuntimeException& e)
    {
        e.dump(cout);
        ret = MULTIPR_APP_FAILED;
    }
    
    return ret;
#else
    int ret = MULTIPR_APP_OK;
    try
    {
        // if mesh is unknown, then list all the meshes in the given MED file
        if (g_meshName == NULL)
        {
            {
                // display list of meshes contained in the MED file
                vector<string> res = multipr::getListMeshes(g_medFilename);
                cout << "List of meshes in this MED file:" << endl;
                for (unsigned i = 0 ; i < res.size() ; i++)
                {
                    cout << "Mesh " << (i + 1) << ": \"" << res[i] << "\"" << endl;
                }
            }
            
            cout << endl;
            
            {
                // display list of fields contained in the MED file
                vector<pair<string,int> > res;
                                multipr::getListScalarFields(g_medFilename, res);
                cout << "List of scalar fields in this MED file:" << endl;
                for (unsigned i = 0 ; i < res.size() ; i++)
                {
                    cout << "Field " << (i + 1) << ": \"" << res[i].first << "\"  #Time stamps=" << res[i].second << endl;
                }
            }
        }
        else
        {
            // display all infos about one mesh in a MED file
            multipr::Mesh mesh;
            mesh.readSequentialMED(g_medFilename, g_meshName);
            mesh.setPrintAll(true);
            cout << mesh << endl;
        }
        cout << "OK" << endl;
    }
    catch (multipr::RuntimeException& e)
    {
        e.dump(cout);
        ret = MULTIPR_APP_FAILED;
    }
    
    return ret;
#endif
}


/**
 * \fn     int run()
 * \brief  applies partitioning/decimation according to global parameters.
 * \return MULTIPR_APP_OK if successful, MULTIPR_APP_FAILED if failure.
 */
int run()
{
    printGlobals();

    int ret = MULTIPR_APP_OK;
    switch (g_usage)
    {
        case MULTIPR_USAGE_AUTOTEST:   ret = runAutotest();   break;
        case MULTIPR_USAGE_PARTITION1: ret = runPartition1(); break;
        case MULTIPR_USAGE_PARTITION2: ret = runPartition2(); break;
        case MULTIPR_USAGE_DECIMATION: ret = runDecimation(); break;
        case MULTIPR_USAGE_MERGE:      ret = runMerge();      break;
        case MULTIPR_USAGE_INFO:       ret = runDumpMED();    break;
        default:
            cout << "ERROR: unknown usage" << endl;
            ret = MULTIPR_APP_FAILED;
            break;
    }
    
    return ret;
}


/**
 * \fn     int main(int argc, char** argv)
 * \brief  entry point of the application.
 * \param  argc number of arguments.
 * \param  argv list of arguments.
 * \return 0 if OK, 1 if failed.
 */
int main(int argc, char** argv)
{
    string strTitle = string("multipr v") + string(multipr::getVersion()) + string(" - by EDF/CS - 04/2007");    
    string strUnderline = "";

    for (int i = 0, len = strTitle.length() ; i < len ; i++) strUnderline += '=';
    
    cout << strTitle << endl;
    cout << strUnderline << endl;
    
    #ifdef MULTIPR_USE_OBJ_API
        cout << "Version MULTIPR_Obj" << endl;
    #else
        cout << "Version MULTIPR_API" << endl;
    #endif   

    parseCommandLine(argc, argv);

    int ret = MULTIPR_APP_OK; // assume no error at the beginning
    
    if (g_usage == MULTIPR_USAGE_UNKNOWN)
    {
        if (argc != 1)
        {
            // if usage is unknown and there are some arguments, print an error message 
            cout << "ERROR: " << getErrorMsg() << endl;
            cout << endl;
            ret = MULTIPR_APP_FAILED;
        }
        else
        {
            // if no argument, print a description of this application
            printDescription();
        }

        printUsage();
    }
    else if (g_usage == MULTIPR_USAGE_DISPLAY_HELP)
    {
        printDescription();
        printUsage();
    }
    else
    {
        // the application seems to be configured properly: it can be executed
        ret = run();
    }
    
    return ret;
}

// EOF
