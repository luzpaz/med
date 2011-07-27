//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D
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
 * \file    MULTIPR_Exceptions.hxx
 *
 * \brief   All the exceptions used by MULTIPR.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_EXCEPTIONS_HXX
#define MULTIPR_EXCEPTIONS_HXX

//*****************************************************************************
// Includes section
//*****************************************************************************

#include <string>
#include <iostream>


namespace multipr
{

//*****************************************************************************
// Class RuntimeException
// Super class for all exceptions used by the module MULTIPR
//*****************************************************************************

class RuntimeException
{
public:
    
    /**
     * Constructor. Build a new RuntimeException.
     * \param  pMsg   message to be associated with this exception.
     * \param  pFile  name of the file where the probem occur (you can use the macro __FILE__); "unknown" by default.
     * \param  pLine  number of the line where the probem occur (you can use the macro __LINE__); 0 by default.
     */
    RuntimeException(
        const std::string& pMsg, 
        const std::string& pFile = "unknown", 
        int                pLine = 0) 
    { 
        mMsg  = pMsg; 
        mFile = pFile;
        mLine = pLine;
        mType = "RuntimeException";
    }
    
    /**
     * Dumps info about this exception to the given output stream.
     */
    void dump(std::ostream& pOs) const 
    { 
        pOs << "MULTIPR: " << mType << " (" << mFile << ", line " << mLine << "): " << mMsg << std::endl; 
    }
    
protected:

    std::string mMsg;    /**< Message associated with this exception. */
    std::string mFile;   /**< Name of the source file where the problem occurs (macro __FILE__ can be used to retrieve this field). */
    int         mLine;   /**< Number of the line where the problem occurs (macro __LINE__ can be used to retrieve this field). */
    std::string mType;   /**< Type of this exception. */
};


//*****************************************************************************
// Class NullArgumentException
// Should be used when an unexpected NULL pointer occurs
//*****************************************************************************

class NullArgumentException : public RuntimeException
{
public:
    NullArgumentException(
        const std::string& pMsg, 
        const std::string& pFile="unknown", 
        int pLine=0) : RuntimeException(pMsg, pFile, pLine) 
    { 
        mType = "NullArgumentException";
    }
};


//*****************************************************************************
// Class IllegalArgumentException
// Should be used when an invalid parameter is detected (check precondition)
//*****************************************************************************

class IllegalArgumentException : public RuntimeException
{
public:
    IllegalArgumentException(
        const std::string& pMsg, 
        const std::string& pFile="unknown", 
        int pLine=0) : RuntimeException(pMsg, pFile, pLine) 
    { 
        mType = "IllegalArgumentException";
    }
};


//*****************************************************************************
// Class IllegalStateException
// Should be used when the internal state of an object is invalid
//*****************************************************************************

class IllegalStateException : public RuntimeException
{
public:
    IllegalStateException(
        const std::string& pMsg, 
        const std::string& pFile="unknown", 
        int pLine=0) : RuntimeException(pMsg, pFile, pLine)
    { 
        mType = "IllegalStateException";
    }
};


//*****************************************************************************
// Class IndexOutOfBoundsException
// Should be used when an index is out of bounds
//*****************************************************************************

class IndexOutOfBoundsException : public RuntimeException
{
public:
    IndexOutOfBoundsException(
        const std::string& pMsg, 
        const std::string& pFile="unknown", 
        int pLine=0) : RuntimeException(pMsg, pFile, pLine)
    { 
        mType = "IndexOutOfBoundsException";
    }
};


//*****************************************************************************
// Class IOException
// Should be used when any i/o error occurs
//*****************************************************************************

class IOException : public RuntimeException
{
public:
    IOException(
        const std::string& pMsg, 
        const std::string& pFile="unknown", 
        int pLine=0) : RuntimeException(pMsg, pFile, pLine)
    { 
        mType = "IOException";
    }
};


//*****************************************************************************
// Class FileNotFoundException
// Should be used to indicate that a file has not been found
//*****************************************************************************

class FileNotFoundException : public IOException
{
public:
    FileNotFoundException(
        const std::string& pMsg, 
        const std::string& pFile="unknown", 
        int pLine=0) : IOException(pMsg, pFile, pLine)
    { 
        mType = "FileNotFoundException";
    }
};


//*****************************************************************************
// Class UnsupportedOperationException
// Should be used when a function/method is not yet implemented or 
// if an operation is not supported in a given context
//*****************************************************************************

class UnsupportedOperationException : public RuntimeException
{
public:
    UnsupportedOperationException(
        const std::string& pMsg, 
        const std::string& pFile="unknown", 
        int pLine=0) : RuntimeException(pMsg, pFile, pLine)
    { 
        mType = "UnsupportedOperationException";
    }
};


} // namespace MULTIPR


#endif // MULTIPR_EXCEPTIONS_HXX

// EOF
