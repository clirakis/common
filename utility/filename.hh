/**
 ******************************************************************
 *
 * Module Name : filename.h
 *
 * Author/Date : C.B. Lirakis / 13-Jun-02
 *
 * Description :
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 *******************************************************************
 */
#ifndef __FILENAME_h_
#define __FILENAME_h_
#  include <fstream>
#  include "precisetime.hh" // To let us know when midnight occurs. 
#  include "CObject.hh"

const size_t FILELEN=256;

class FileName : public CObject
{
public:
    /**
     * Filename constructor, user supplies sensor name and suffix. 
     * By default the time to change filenames is not set and thus
     * the file never closes and renames but remains as one long file. 
     *
     * update is the timespec as to when to change, some useful 
     * definitions are in precisetime.h
     * This Relies on an ENVIRONMENTAL variable DATAPATH to be set
     * to work correctly.
     */ 
    FileName(const char *SensorName=NULL, 
	     const char *Suffix=NULL,
	     const struct timespec &update = Zero, ostream *output=NULL);
    /// Destructor for filename class to make sure cleanup exists. 
    ~FileName();

    /**
     * Return the currently applied filespec. 
     */
    inline const char *GetCurrentFilespec(void) {return Filespec;};
    /**
     *  A method to get the fully constructed and qualified name. 
     * Kind of raw with no real checks of any kind. 
     */
    const char *GetName();
    /**
     * Get a name that is unique. IE if the filename assembled is 
     * the same as an exsisting file, bump the revision number until a 
     * unique name is found. 
     */ 
    const char *GetUniqueName();
    /// Indicate that a new file should be created. 
    bool ChangeNames();

    /// ASCII logging features. 
    bool EnableLogging(const bool val=true);
    /// Give me a string to log. 
    bool LogData(const char *data);
    /// Get a new update time. 
    void NewUpdateTime();

 private:
    char *sensorName;             // Name of sensor as setup in constructor
    char *Filespec;               // Fully specified path for data file.
    char *Path;                   // Path for datafile as provided.
    char *suffix;                 // Constructor provided suffix. 
    int  revision;                // Revision number if used. 
    PreciseTime *TimeToChange;    // Time for new filename to be generated.
    PreciseTime *Update;          // Delta for update
    bool LoggingEnabled;          // True if primitive logging is enabled.
    ofstream *fOutput;
};
#endif
