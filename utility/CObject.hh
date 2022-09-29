/**
 ******************************************************************
 *
 * Module Name : CObject
 *
 * Author/Date : C.B. Lirakis / 26-Dec-10
 *
 * Description : Should be the base class of all other classes I develop.
 * 
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
#ifndef __COBJECT_hh_
#define __COBJECT_hh_
#include <fstream>

class CObject 
{
public:
    enum ERROR_CODES {ENONE};
    enum DEBUG_LEVEL {DNONE, LOW, MEDIUM, HIGH};
    /*! Base constructor for CObject. */
    CObject(void);
    /*! This will create a CObject from a CObject */
    CObject(CObject &a);

    /*! Since most things will inherit this class, 
     * here is a virtual destructor 
     */
    virtual ~CObject();
    /*!
     * Print out all the base objects in this class.
     * No arguments. 
     */
    virtual void Print(void);
    /*!
     * The class can store an error string for the inheriting class. 
     * This method allows the user to convert the integer error 
     * to its corresponding string. This is intended to be overridden
     * by the parent. 
     */
    virtual const char* GetErrorString(int error);
    /*!
     * The class can store an error string for the inheriting class. 
     * This method allows the user to access the string associated 
     * with the last error
     * This is intended to be overridden by the parent. 
     */
    virtual const char* GetErrorString();

    /*! Operator to equate two CObject classes. */
    CObject& operator= (const CObject rhs);

    /*! Log a message, kind of overkill relative to the CLogger class. */
    void  LogData(const char *msg);
    /*! Set the output for the logger message. */
    void  SetOutput(std::ostream *v);
    /*! Get the unique numeric integer ID for this class */
    inline unsigned long GetUniqueID(void) const {return fUniqueID;};
    /*! Allow the user to override the integer id for the class. */
    void SetUniqueID(unsigned long i=0);
    /*! get the last error generated. */
    inline int Error(void) const       {return fError;};
    /*! Check error returns true if an error exists. */
    bool CheckError(void);
    /*! Clear all error codes. Set the line it was cleared at. */
    inline void  ClearError(int l)     {fError=ENONE; fELine=l;};
    /*! Get the integer high version number. */
    inline short VersionHi(void) const     {return fVersionHigh;};
    /*! Get the low integer version number. */
    inline short VersionLo(void) const     {return fVersionLow;};
    /*! return a double that represents the version number */
    double GetVersion(void);

    /*! Set the debug level for the class. */
    inline void  SetDebug(unsigned int level)   {fDebug=level;};
    /*! Return the line that the last error code was set at. */
    inline int   ErrorLine(void) {return fELine;};
    /*! Get the ouptut stream */
    inline std::ostream* GetOutput()        {return fOutput;};
    /*! Enable a more friendly way of printing the contents of the class. */
    friend std::ostream& operator<<(std::ostream& output, const CObject &n);

protected:
    /*! Inherited - permits upstream classes to set the error and the 
     * line number within the function that the error occured at. 
     */
    int SetError( int code=CObject::ENONE, int Line=0); 
    /*!
     * Set the name of the class 
     */
    void SetName(const char *name);

    /*!
     * Set the version values. 
     */
    inline void SetVersion(short High, short low) 
	{fVersionHigh=High;fVersionLow=low;};

    /*! Set the debug information, print the data if the level specified
     * is achieved. 
     */
    void DebugInfo(const char*, unsigned int level);
    /*!
     * Check if the level provided exceeds the current debugging level. 
     * return true if so. 
     */
    inline bool Debug(unsigned int Level) const {return (fDebug>=Level);};

    /*! Permit acces to any provided output stream. */
    std::ostream *fOutput;
    unsigned int  fDebug;       // debug level

private:
    unsigned int  fBits;        // control bits for class
    int           fError;       // Last operation error code.
    int           fELine;       // Line of last error. 
    char          *fName;
    unsigned long fUniqueID;
    short         fVersionHigh;
    short         fVersionLow;
};
#endif
