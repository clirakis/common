/**
 ******************************************************************
 *
 * Module Name : H5Logger.hh
 *
 * Author/Date : C.B. Lirakis / 01-May-20
 *
 * Description : A method to use with anything that produces data
 *               to log binary data. This class will enable read and
 *               write of
 *               1) A header containing Filename and path, 
 *               2) A user name for the data
 *
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : https://support.hdfgroup.org/HDF5/doc/cpplus_RM/classes.html
 * test location: code/Hacks/HDF5/Logger/v0.7
 *
 *******************************************************************
 */
#ifndef __H5LOGGER_hh_
#define __H5LOGGER_hh_
#  include <inttypes.h>
#  include <stdlib.h>
#  include "CObject.hh"
#  include "H5Cpp.h"
#  include "Split.hh"

using namespace H5;

/// H5Logger documentation here. 
class H5Logger : public CObject
{
public:
    enum HeaderIndex {kFILENAME=0, kDATE, kDATADESCRIPTOR};

    /*! 
     * H5Logger constructor
     *    Filename - full file path and name to open. 
     *    UserDataName - 
     *    NVariables - Number of variables that the user wants to store. 
     *    ReadOnly   - User specified read (true) or write (false)
     *    Comment    - Optional user comment to be stored in file 
     */
    H5Logger(const char *Filename, const char *UserDataName, 
	     size_t NVariables, bool ReadOnly, const char*Comment=NULL);

    /*!
     * H5Logger destructor. 
     * Clean up all the dynmically allocated memory. 
     */
    ~H5Logger();

    /*!
     * Write out data tag strings. This is an array of 
     * the size of the number of variables that you intend to store
     * in the actual data set.
     * TagNames - character array of names. 
     * NTags    - Number of tags in array. 
     */
    bool WriteDataTags(const char *TagNames);

    /*! 
     * How many data tags/variables do we have in the file?
     */
    size_t NVariables(void);

    /*!
     * Fill the desired data. 
     * The user supplies the vector. 
     * if no user data is supplied, then the internal data vector 
     * fVariables is used to log the data. 
     */
    bool Fill(const double *var = NULL);

    /*!
     * Allow the user to input his data into the internal variable
     * data vector. 
     */
    void FillInternalVector(const double &Val, size_t Location);

    /*!
     * Read back the data a vector slice at a time.
     * This will read back an entire row of data.
     *
     * Row is the row index. Think of a single row per fill.  
     */
    bool DatasetReadRow(size_t Row);

    /*!
     * Once we have a row of data, allow the user to get at it. 
     */
    inline const double* RowData(void) const {return fVariables;};
    /*!
     * Once we have a row of data, allow user to get at a single 
     * value. 
     */
    double RowData(size_t Column);

    /*!
     * Read back the data in columnar format. 
     * This may take multiple reads, but will be more efficient 
     * if I want to get all of the data for a specific variable (column).
     * 
     * Column  - the column that we want to access.
     * Row     - The row we want to access, this will be updated with each
     *           call. This is user supplied, but can be random access. 
     * Vector  - a user allocated vector to store the data we got back. 
     * NVector - the size of the user allocated vector. 
     * The number of times that this operation will have to be performed 
     * is NEntries/NVector. It would be ideal if the ratio of this 
     * was an integer.  
     */
    bool DatasetReadColumn(size_t Column, size_t &Row, double *Vector, size_t NVector);

    /*!
     * Manipulate when the data set is increased by. 
     * NOTE: Later we can get more tricky by coding in memory and 
     * limiting the number of write to the block size. 
     */
    inline size_t BlockSize(void) const {return fNBlocking;};

    /*!
     * How many times has the vector been stored?
     * Or how many entries total are there on a read. 
     */
    inline size_t NEntries(void) const {return fNReadWrite;};

    /*!
     * Index from Name, given a name return the index into the 
     * row where that data lives for that name. 
     */
    uint32_t IndexFromName(const char*);

    /*!
     * return the tag name assoicated with the index. 
     */
    const char *NameFromIndex(uint32_t i);

    /*! 
     * Access header information
     * index into header must be less than 3.
     * 0 - Filename
     * 1 - Creation Date
     * 2 - Dataset Name 
     */
    const char *HeaderInfo(HeaderIndex);

    /*!
     * Utility to convert Creation date from string to struct tm. 
     * This is currently not re-entrant. 
     */
    struct tm *H5ParseTime(const char *val);


    /*!
     * Access revsion information 
     */
    inline unsigned int MajorRevision(void) const {return fRevision[0];};
    inline unsigned int MinorRevision(void) const {return fRevision[1];};

    /*! Enable a more friendly way of printing the contents of the class. */
    friend std::ostream& operator<<(std::ostream& output, const H5Logger &n);

private:
    H5File* fpFile;
    bool    fReadOnly; 

    /*!
     * These pertain to the storage of the filename, creation date, 
     * and user comment. 4 strings each of 80 characters max. 
     */
    static const size_t kCOLS = 4;
    static const size_t kSSIZ = 80;
    /*! 
     * Actual data stored for the user header. 
     */
    char    fHeader[kCOLS][kSSIZ];
    /*!
     * Use the split class to store the names for the variables. 
     * This class has a way of managing the data labels. 
     * The number of labels should but does not have to equal 
     * the number of variables decleared. 
     */
    Split*  fVariableNames;
    /*!
     * This is the number of variables we will be storing per Fill call. 
     * This is specified by the constructor during write OR
     * read from the file in read mode. 
     */
    size_t  fNVariables;
    /*!
     * A persistant array to read or write the data. 
     */
    double* fVariables;
    /*!
     * Variable specifying the number of "blocks" of data to chunk by. 
     */
    size_t  fNBlocking; 
    /*!
     * This counts the number of times we expanded the dataspace
     */
    unsigned long fNExpand;

    /*!
     * Count the number of writes we make too.
     */
    unsigned long fNReadWrite;

    /*!
     * What is the maximum extent size of the dataset?
     */
    unsigned long fMaxSize;

    /*!
     * Handle to the user data set
     */
    DataSet fUserDataset;

    /*!
     * Create the header for the HDF5 file. 
     */
    void    WriteHeader(const char *Filename, const char *UserDataName, 
			const char *Comment);

    /// H5Logger ReadHeader
    bool    ReadHeader(void);
    /*!
     * Read back the names and fill the array below for parsing. 
     */
    bool    ReadDataTags(void);

    /*!
     * Store interesting stuff about the system. 
     */
    bool   WriteFinalState(void);

    /*!
     * And read back the final state of the system. 
     */
    bool   ReadFinalState(void);

    /*!
     * Create the variable data space. 
     */
    bool CreateUserDataSet(void);

    /*!
     * Open dataset for read. 
     */
    bool OpenUserDatasetRead(void);
    /*!
     * Version information, so we can determine how to change 
     * internal code for future revisions if things change. 
     */
    unsigned int fRevision[2];
    bool WriteVersionInformation(void);
    bool ReadVersionInformation(void);

};
#endif
