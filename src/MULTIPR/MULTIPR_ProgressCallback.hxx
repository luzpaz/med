// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_ProgressCallback.hxx
 *
 * \brief   Class MULTIPR_ProgressCallback
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_PROGRESS_CALLBACK_HXX
#define MULTIPR_PROGRESS_CALLBACK_HXX

//*****************************************************************************
// Includes section
//*****************************************************************************


//*****************************************************************************
// Class MULTIPR_ProgressCallback
// Used to provide feedback on the progress of a slow operation.
//*****************************************************************************

class MULTIPR_ProgressCallback
{
public:
    
    /**
     * Builds a new MULTIPR_ProgressCallback (default constructor).
     */
    MULTIPR_ProgressCallback() { init(100); }
    
    /**
     * Destructor. Removes everything.
     */
    virtual ~MULTIPR_ProgressCallback() { /* do nothing */ }
    
    /**
     * Starts to provide feedback on the progress of a slow operation.
     * \param  pTaskTitle name of the task to be monitored.
     * \param  pNumSteps  number of steps in the task.
     */
    virtual void start(const char* pTaskTitle, int pNumSteps) = 0;

    /**
     * Moves on the current amount of progress made.
     */
    void moveOn() { mCurrentStep++; float percent = float(mCurrentStep)/float(mTotalSteps)*100.0f; progress(percent); }
    
    /**
     * Termines to provide feedback.
     */
    virtual void done() = 0;
    
protected:
    
    /**
     * Resets this progress callback.
     * \param  pNumSteps number of steps in the task to be monitored.
     */
    void init(int pNumSteps) { mCurrentStep = 0; mTotalSteps = pNumSteps; }
    
    /**
     * Callback. Called on each progress.
     * \param  pPercent percent accomplished.
     */
    virtual void progress(float pPercent) = 0;

private:

    int mCurrentStep;
    int mTotalSteps;
    
}; // class MULTIPR_ProgressCallback


//*****************************************************************************
// Class MULTIPR_EmptyMeshCallback
// Used to provide feedback when an empty mesh is produced.
//*****************************************************************************

class MULTIPR_EmptyMeshCallback
{
public:
    
    /**
     * Builds a new MULTIPR_EmptyMeshCallback (default constructor).
     */
    MULTIPR_EmptyMeshCallback() { /* do nothing */ }
    
    /**
     * Destructor. Removes everything.
     */
    virtual ~MULTIPR_EmptyMeshCallback() { /* do nothing */ }

    /**
     * Callback. Called each time an empty mesh is produced.
     * \param  pInfo information related to the empty mesh.
     */
    virtual void reportEmptyMesh(std::string pInfo) = 0;
    
}; // class MULTIPR_EmptyMeshCallback


#endif // MULTIPR_PROGRESS_CALLBACK_HXX

// EOF
