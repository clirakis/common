/********************************************************************
 *
 * Module Name : H5Logger.cpp
 *
 * Author/Date : C.B. Lirakis / 01-May-20
 *
 * Description : Logger based on HDF5 file format
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * printError is replaced by printErrorStack for versions > 1.10.2
 * All the catch really needed to be (const whatever &error)
 *
 * Classification : Unclassified
 *
 * References : https://support.hdfgroup.org/HDF5/doc/cpplus_RM/classes.html
 * test location: code/Hacks/HDF5/Logger/v0.7
 *
 ********************************************************************/
// System includes.

#include <iostream>
using namespace std;
#include <string>
#include <cstring>
#include <cmath>

/// Local Includes.
#include "debug.h"
#include "H5Logger.hh"
#include "Split.hh"

const H5std_string sLoggerHeader("H5Logger_Header");
const H5std_string sVariableHeader("H5Variable_Descriptions");
const H5std_string sDatasetHeader("H5_UserData");
const H5std_string sVersionHeader("H5_VersionInformation");
const H5std_string sFinalStateHeader("H5_FinalStateInformation");

const size_t kDataRank = 2;

const unsigned int kMAJOR_VERSION = 1;
const unsigned int kMINOR_VERSION = 0;

/**
 ******************************************************************
 *
 * Function Name : H5Logger constructor
 *
 * Description :
 *
 * Inputs :
 *    Filename - full file path and name to open. 
 *    NVariables - Number of variables that the user wants to store. 
 *    ReadOnly   - User specified read (true) or write (false)
 *    Comment    - Optional user comment to be stored in file 
 *
 * Returns : constructed class
 *
 * Error Conditions : H5File open fail.
 *                    Read
 *                        Read Header failed
 *                        Read Data tags failed
 *                        Failed to open user data set
 *                        Read final data state. 
 *                    Write
 *                        Write header
 *                        Create user dataset
 * 
 * Unit Tested on: 03-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
H5Logger::H5Logger (const char *Filename, const char *UserDataName,
		    size_t NVariables, bool ReadOnly, 
		    const char*Comment) : CObject()
{
    SET_DEBUG_STACK;
    
    fReadOnly = ReadOnly;
    ClearError(__LINE__);
    SetVersion(kMAJOR_VERSION,kMINOR_VERSION);
    
    memset(fHeader, 0, kCOLS*kSSIZ);
    fVariableNames  = NULL;
    fVariables      = NULL;
    fNVariables     = NVariables;
    fNReadWrite     = 0;
    fNExpand        = 0;
    fNBlocking      = 5; /* Default chunking */
    fMaxSize        = 0;
    fRevision[0]  = kMAJOR_VERSION;
    fRevision[1]  = kMINOR_VERSION;

    try
    {
	/*
	 * Turn off the auto-printing when failure occurs so that we can
	 * handle the errors appropriately
	 */
	Exception::dontPrint();

        /*
	 * Create a new file. If file exists its contents will be overwritten.
	 */
	if (ReadOnly)
	{
	    fpFile = new H5File( Filename, H5F_ACC_RDONLY );
	    if(!ReadHeader())
	    {
		SetError(-1, __LINE__);
		return;
	    }
	    if(!ReadDataTags())
	    {
		SetError(-2, __LINE__);
		return;
	    }
	    if (!OpenUserDatasetRead())
	    {
		SetError(-3, __LINE__);
		return;
	    }
	    if (!ReadVersionInformation())
	    {
		SetError(-4, __LINE__);
		return;
	    }
	    if (!ReadFinalState())
	    {
		SetError(-5, __LINE__);
		return;
	    }
	}
	else
	{
	    fpFile = new H5File( Filename, H5F_ACC_TRUNC );
	    WriteHeader(Filename, UserDataName,Comment);
	    WriteVersionInformation();
	    CreateUserDataSet();
	}
    }
    // catch failure caused by the H5File operations
    catch( const FileIException &error )
    {
	error.printErrorStack();
	delete fpFile;
	fpFile = NULL;
	SetError(-1,__LINE__);
	SET_DEBUG_STACK;
	return;
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : H5Logger destructor
 *
 * Description : Clean up all the dynmically allocated memory. 
 *               Write out the final state
 *               close and delete the file handle. 
 *               delete the Variable names Split class 
 *               delete the variables vector. 
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 03-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
H5Logger::~H5Logger ()
{
    SET_DEBUG_STACK;

    /*
     * Finalize any writing
     */
    if(!fReadOnly)
    {
	WriteFinalState();
    }

    fpFile->close();
    delete fpFile;
    fpFile = 0;
    
    delete fVariableNames;
    delete fVariables;
}

/**
 ******************************************************************
 *
 * Function Name : H5Logger::WriteHeader
 *
 * Description : Create and write the data header associated with the
 *               log file. This contains the filename, time of creation, 
 *               and an arbitray comment from the user. 
 *
 * Inputs : Name - Filename
 *          Comment - User comment
 *
 * Returns : NONE
 *
 * Error Conditions : Fail to create DataSet
 *                    Fail to create DataSpace
 *                    Fail on Datatype creation
 * 
 * Unit Tested on: 02-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void H5Logger::WriteHeader(const char *Name, const char *UserDataName,
			   const char *Comment)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
        
    time_t now;

    memset(fHeader, 0, kSSIZ*kCOLS);

    // Store the filename 
    strncpy(fHeader[0],Name,kSSIZ);
    time(&now);
    strftime (fHeader[1], kSSIZ, "%F %T", gmtime(&now));
    strncpy(fHeader[2], UserDataName,kSSIZ);
    
    if (Comment) strncpy(fHeader[3],Comment, kSSIZ);

    try
    {
	hsize_t   dims[2];
	const int RANK = 1;

	/* 
	 * ---------------------------------------------------
	 * Setup the dataspace, need dimensions and type
	 */
	dims[0] = kCOLS;  // Number of strings
	dims[1] = kSSIZ;  // Max size of string

	// Create a simple dataspace
	DataSpace    DS_Strings( RANK, dims);

	// const PredType &pred_type, const size_t &size
	StrType strdatatype(PredType::C_S1, (const size_t )kSSIZ); 

	/*
	 * Create the dataset
	 *  Name of dataset
	 *  Data type of the dataset
	 *  Dataspace of the dataset
	 *  Creation property. 
	 */
	DataSet sDataset = fpFile->createDataSet(sLoggerHeader,// Dataset name
						 strdatatype,
						 DS_Strings); // DS definition

	sDataset.write( fHeader, strdatatype);
    }
    // catch failure caused by the DataSet operations
    catch( const DataSetIException &error )
    {
	error.printErrorStack();
	SetError(-1,__LINE__);
	return;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataSpaceIException &error )
    {
	error.printErrorStack();
	SetError(-2,__LINE__);
	return;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataTypeIException &error )
    {
	error.printErrorStack();
	SetError(-3,__LINE__);
	return;
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : H5Logger::ReadHeader
 *
 * Description : open the dataset associated with the Logger Header
 *               information and read in the strings. 
 *
 * Inputs : NONE
 *
 * Returns : True on success, false on failure.
 *
 * Error Conditions : dataset fails to open
 *                    fail to declare the string data type. 
 * 
 * Unit Tested on: 03-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool H5Logger::ReadHeader(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    try
    {
	DataSet dataset = fpFile->openDataSet( sLoggerHeader );

	/*
	 * Get filespace for rank and dimension
	 */
	DataSpace filespace = dataset.getSpace();

	StrType strdatatype(PredType::C_S1, (const size_t )kSSIZ); 
	memset (fHeader, 0, kSSIZ*kCOLS);
	dataset.read( fHeader, strdatatype);

    }  // end of try block
    // catch failure caused by the DataSet operations
    catch( const DataSetIException &error )
    {
	error.printErrorStack();
	SetError(-1,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataSpaceIException &error )
    {
	error.printErrorStack();
	SetError(-2,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataTypeIException &error )
    {
	error.printErrorStack();
	SetError(-3,__LINE__);
	return false;
    }
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : H5Logger::WriteDataTags
 *
 * Description : create a data space for the data tags and store them. 
 *
 * Inputs : a string of data tag/names seperated by full colons. (:)
 *
 * Returns : true on success
 *
 * Error Conditions : Dataset open fails. 
 *                    Dataspace open fails.
 *                    Datatype declaration fails.  
 * 
 * Unit Tested on: NONE
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool H5Logger::WriteDataTags(const char *TagNames)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    char Names[kSSIZ];

    /* Make a local copy of the names array for storing in the file. */
    memset(Names, 0, kSSIZ);
    strncpy(Names, TagNames, kSSIZ);

    fVariableNames = new Split(TagNames,':');

    if (fVariableNames->NTokens()>0)
    {
	try
	{
	    hsize_t   dims[2];
	    const int RANK = 1;

	    /* 
	     * ---------------------------------------------------
	     * Setup the dataspace, need dimensions and type
	     */
	    dims[0] = 1;                // Number of strings
	    dims[1] = kSSIZ;            // Max size of string

	    // Create a simple dataspace
	    DataSpace    DS_Strings( RANK, dims);

	    StrType strdatatype(PredType::C_S1, (const size_t )kSSIZ); 

	    /*
	     * Create the dataset
	     *  Name of dataset
	     *  Data type of the dataset
	     *  Dataspace of the dataset
	     *  Creation property. 
	     */
	    DataSet sDataset = fpFile->createDataSet(sVariableHeader,
						     strdatatype,
						     DS_Strings);

	    sDataset.write( Names, strdatatype);
	}
	// catch failure caused by the DataSet operations
	catch( const DataSetIException &error )
	{
	    error.printErrorStack();
	    SetError(-2,__LINE__);
	    return false;
	}
	// catch failure caused by the DataSpace operations
	catch( const DataSpaceIException &error )
	{
	    error.printErrorStack();
	    SetError(-3,__LINE__);
	    return false;
	}
	// catch failure caused by the DataSpace operations
	catch( const DataTypeIException &error )
	{
	    error.printErrorStack();
	    SetError(-4,__LINE__);
	    return false;
	}
    }
    else
    {
	SetError(-5, __LINE__);
	return false;
    }
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : H5Logger::ReadDataTags
 *
 * Description : Read back and parse the data tags.
 *
 * Inputs : a string of data tag/names seperated by full colons. (:)
 *
 * Returns : true on success
 *
 * Error Conditions : Dataset open fails. 
 *                    Dataspace open fails.
 *                    Datatype declaration fails.  
 * 
 * Unit Tested on: 03-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool H5Logger::ReadDataTags(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char Names[kSSIZ];

    try
    {
	DataSet dataset = fpFile->openDataSet( sVariableHeader );

	/*
	 * Get filespace for rank and dimension
	 */
	DataSpace filespace = dataset.getSpace();

	StrType strdatatype(PredType::C_S1, (const size_t )kSSIZ); 
	memset (Names, 0, kSSIZ);
	dataset.read( Names, strdatatype);

	fVariableNames = new Split(Names,':');
    }
    // catch failure caused by the DataSet operations
    catch( const DataSetIException &error )
    {
	error.printErrorStack();
	SetError(-1,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataSpaceIException &error )
    {
	error.printErrorStack();
	SetError(-2,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataTypeIException &error )
    {
	error.printErrorStack();
	SetError(-3,__LINE__);
	return false;
    }
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : H5Logger::CreateUserDataSet
 *
 * Description : Create the dataspace, for the moment just native
 *               doubles. This dataspace is expandable. Originally
 *               it is creted to the size of fNVariables x fNBlocking
 *
 * Inputs : none
 *
 * Returns : true on success, false on failure.
 *
 * Error Conditions : Dataset open fails. 
 *                    Dataspace open fails.
 *                    Datatype declaration fails.  
 * 
 * Unit Tested on: 03-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool H5Logger::CreateUserDataSet(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    /* 
     * Make this double to start with then figure out more complex 
     * stuff. 
     */
    /*
     * Allocate internal buffer for data, may not need this, check back. 
     */
    fVariables = new double[fNVariables];

    if (fVariables == NULL)
    {
	SetError(-1, __LINE__);
	return false;
    }

    try
    {
	/*
	 * Define the size of the array and create the data space for fixed
	 * size dataset.
	 * 
	 * The block size governs how often we increase the size of the 
	 * data set, so we don't thrash. 
	 */
	hsize_t     dimsf[kDataRank];              // dataset dimensions
	dimsf[0] = fNVariables;   // Number of variables (X Dimension)
	dimsf[1] = fNBlocking;    // Block size.

	fMaxSize = fNBlocking;

	/*
	 * Define datatype for the data in the file.
	 */
	IntType datatype( PredType::NATIVE_DOUBLE );

	/*
	 * Modify dataset creation properties, i.e. enable chunking.
	 */
	DSetCreatPropList cparms;

	/*
	 * Set fill value for the dataset
	 * The datatype may differ from that of the dataset, but it must be 
	 * one that the HDF5 library is able to convert value to the 
	 * dataset datatype when the dataset is created. The default 
	 * fill value is 0 (zero,) which is interpreted according to 
	 * the actual dataset datatype.
	 *
	 */
	int fill_val = 0;
	cparms.setFillValue( PredType::NATIVE_DOUBLE, &fill_val);

	/* 
	 * Unlimited is tied to set chunk dimensions. 
	 */
	hsize_t     maxdims[kDataRank] = {fNVariables, H5S_UNLIMITED};
	/*
	 * The ndims parameter currently must have the same value as the
	 * rank of the dataset. The values of the dim array define the 
	 * size of the chunks to store the dataset's raw data. As a 
	 * side-effect, the layout of the dataset will be changed to 
	 * H5D_CHUNKED, if it is not so already.
	 *
	 * Just because it is chunked doesn't require that we use 
	 * the chunks yet. 
	 */
	hsize_t           chunk_dims[2] = {fNVariables, 1};
	cparms.setChunk( kDataRank, chunk_dims );

	DataSpace   dataspace( kDataRank, dimsf, maxdims);

	/*
	 * Create a new dataset within the file using defined dataspace and
	 * datatype and default dataset creation properties.
	 *
	 */
	fUserDataset = fpFile->createDataSet( sDatasetHeader, 
					      datatype, 
					      dataspace,
					      cparms);
    }  // end of try block
    // catch failure caused by the DataSet operations
    catch( const DataSetIException &error )
    {
	SetError(-1,__LINE__);
	error.printErrorStack();
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataSpaceIException &error )
    {
	SetError(-2,__LINE__);
	error.printErrorStack();
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataTypeIException &error )
    {
	SetError(-3,__LINE__);
	error.printErrorStack();
	return false;
    }

    return true;
}
/**
 ******************************************************************
 *
 * Function Name : H5Logger::Fill
 *
 * Description : Log the data in var. 
 *
 * Inputs : var - vector to store in dataset.
 *          if this is NULL use internal vector. 
 *
 * Returns : true on success
 *
 * Error Conditions : fail in access of dataset
 *                    fail in access of dataspace
 * 
 * Unit Tested on: 03-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool H5Logger::Fill(const double *var)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    try
    {
	/*
	 * Select a hyperslab. 
	 */
	hsize_t     offset[2] = {0,0};
	/* This is how big a push we want to do for the data we are writing. */
	hsize_t     dims[kDataRank] = {fNVariables, 1};
	/*
	 * This kind of explains how the memory data layout is done. 
	 */
	DataSpace   MVSpace(kDataRank, dims);


	if ((fNReadWrite%fNBlocking == 0) && (fNReadWrite>0))
	{
	    /* This is the size we want for a data set increase in size. */
	    hsize_t     dime[kDataRank]  = {fNVariables,1};

	    /* Count the number of times we expand the data space. */
	    fNExpand++;
	    // The orignial value allocates way too much space.
	    //dime[1] = (1+fNExpand)*fNVariables;
	    dime[1] = (1+fNExpand)*fNBlocking;
	    fMaxSize = dime[1];
	    fUserDataset.extend(dime);
	}

	/* Get current file space */
	DataSpace file_space = fUserDataset.getSpace();
 
	/* Where do we want to write in the hyperslab? */
	offset[1] = fNReadWrite; 
	file_space.selectHyperslab( H5S_SELECT_SET, dims, offset);
	/*
	 * Write the data to the dataset using default memory space, file
	 * space, and transfer properties.
	 *
	 * buf	- IN: Buffer containing data to be written
	 * mem_type	- IN: Memory datatype
	 * mem_space	- IN: Memory dataspace
	 * file_space	- IN: Dataset's dataspace in the file
	 * xfer_plist	- IN: Transfer property list for this I/O operation
	 */
	if (var != NULL)
	{
	    fUserDataset.write( var, PredType::NATIVE_DOUBLE, MVSpace, file_space);
	}
	else
	{
	    fUserDataset.write( fVariables, PredType::NATIVE_DOUBLE, MVSpace, file_space);
	}

	/* Increment the number of writes counter. */
	fNReadWrite++;
    }  // end of try block
    // catch failure caused by the DataSet operations
    catch( const DataSetIException &error )
    {
	SetError(-1,__LINE__);
	error.printErrorStack();
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataSpaceIException &error )
    {
	SetError(-2,__LINE__);
	error.printErrorStack();
	return false;
    }

    return true;
}
/**
 ******************************************************************
 *
 * Function Name : H5Logger::WriteFinalState
 *
 * Description : Write out any final state variables at close time. 
 *               For the moment this is the number of vectors 
 *               written. 
 *
 * Inputs : NONE
 *
 * Returns : true on succee, false on failure. 
 *
 * Error Conditions : Dataset open fails. 
 *                    Dataspace open fails.
 *                    Datatype declaration fails.  
 * 
 * Unit Tested on: 03-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool H5Logger::WriteFinalState(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    try
    {
	hsize_t   dims[2];
	const int RANK = 1;

	/* 
	 * ---------------------------------------------------
	 * Setup the dataspace, need dimensions and type
	 */
	dims[0] = 1;  // Number of variables
	dims[1] = 1;  // Max size of string

	// Create a simple dataspace
	DataSpace    DS_FinalState( RANK, dims);
	IntType datatype( PredType::NATIVE_LONG );

	/*
	 * Create the dataset
	 *  Name of dataset
	 *  Data type of the dataset
	 *  Dataspace of the dataset
	 *  Creation property. 
	 */
	DataSet FSDataset = fpFile->createDataSet(sFinalStateHeader,
						 datatype,
						 DS_FinalState);

	FSDataset.write( &fNReadWrite, PredType::NATIVE_LONG);
    }
    // catch failure caused by the DataSet operations
    catch( const DataSetIException &error )
    {
	error.printErrorStack();
	SetError(-2,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataSpaceIException &error )
    {
	error.printErrorStack();
	SetError(-3,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataTypeIException &error )
    {
	SetError(-3,__LINE__);
	error.printErrorStack();
	return false;
    }

    return true;
}
/**
 ******************************************************************
 *
 * Function Name : H5Logger::ReadFinalState
 *
 * Description : Read the final state data in. 
 *               Right now it just initializes the variable
 *               fNReadWrite - the number of records written to the file. 
 *
 * Inputs : NONE
 *
 * Returns : true on success, false on failure. 
 *
 * Error Conditions : Dataset open fail
 *                    Dataspace declaration fail
 * 
 * Unit Tested on: 03-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool H5Logger::ReadFinalState(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    try
    {
	DataSet dataset = fpFile->openDataSet( sFinalStateHeader);

	/*
	 * Get filespace for rank and dimension
	 */
	DataSpace filespace = dataset.getSpace();

	dataset.read( &fNReadWrite, PredType::NATIVE_LONG);

    }
    // catch failure caused by the DataSet operations
    catch( const DataSetIException &error )
    {
	error.printErrorStack();
	SetError(-2,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataSpaceIException &error )
    {
	error.printErrorStack();
	SetError(-3,__LINE__);
	return false;
    }

    return true;
}
/**
 ******************************************************************
 *
 * Function Name : H5Logger::OpenUserDatasetRead
 *
 * Description : Open the Dataset associated with the user data. 
 *               Allocate the fNVariables vector. 
 *
 * Inputs : NONE
 *
 * Returns : true on success, false on failure. 
 *
 * Error Conditions : Dataset open fail
 *                    Dataspace declaration fail
 * 
 * Unit Tested on: 03-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool H5Logger::OpenUserDatasetRead(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    try
    {
	fUserDataset = fpFile->openDataSet( sDatasetHeader);

	/*
	 * Get filespace for rank and dimension
	 */
	DataSpace filespace = fUserDataset.getSpace();

	/*
	 * Get and print the dimension sizes of the file dataspace
	 */
	hsize_t dims[2];    // dataset dimensions
	//int rank = 
	filespace.getSimpleExtentDims( dims );
	// Save the dimensions, we will use them over and over 
	fNVariables = dims[0];
	fMaxSize    = dims[1];  // This dimension may be bigger than data. 

	/*
	 * Allocate internal buffer for data, may not need this, check back. 
	 */
	fVariables = new double[fNVariables];

	if (fVariables == NULL)
	{
	    SetError(-1, __LINE__);
	    return false;
	}
 
#ifdef DEBUG
	cout << "dataset rank = " << rank << ", dimensions ";
	for (int i=0;i<rank;i++)
	{
	    cout << (unsigned long)(dims[i]) << " ";
	}
	cout << endl;
#endif
    }
    // catch failure caused by the DataSet operations
    catch( const DataSetIException &error )
    {
	error.printErrorStack();
	SetError(-2,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataSpaceIException &error )
    {
	error.printErrorStack();
	SetError(-3,__LINE__);
	return false;
    }
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : H5Logger::DatasetReadRow
 *
 * Description : read the vector specified by index. 
 * 
 * Inputs : index - numerical index into the data slice
 *
 * Returns : true on success , false on falure
 *
 * Error Conditions : index > fNReadWrite
 *                    Dataset access fails
 *                    DataSpace fails
 * 
 * Unit Tested on: 03-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool H5Logger::DatasetReadRow(size_t index)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    if (index>=fNReadWrite)
    {
	SetError(-1, __LINE__);
	return false;
    }

    try
    {
	/*
	 * Select a hyperslab. 
	 */
	hsize_t     offset[2] = {0,0};
	/* This is how big is the read vector and of what dimension? */
	hsize_t     dims[kDataRank] = {fNVariables, 1};
	/*
	 * This kind of explains how the memory data layout is done. 
	 */
	DataSpace   MVSpace(kDataRank, dims);

	/* Get current file space */
	DataSpace file_space = fUserDataset.getSpace();
	/* Where do we want to write in the hyperslab? */
	offset[1] = index; 
	file_space.selectHyperslab( H5S_SELECT_SET, dims, offset);


	memset (fVariables, 0, fNVariables*sizeof(double));
	fUserDataset.read( fVariables, PredType::NATIVE_DOUBLE, 
			   MVSpace, file_space);

    }
    // catch failure caused by the DataSet operations
    catch( const DataSetIException &error )
    {
	error.printErrorStack();
	SetError(-2,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataSpaceIException &error )
    {
	error.printErrorStack();
	SetError(-3,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataTypeIException &error )
    {
	error.printErrorStack();
	SetError(-4,__LINE__);
	return false;
    }
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : H5Logger::RowData
 *
 * Description : Access a column of the row. 
 * 
 * Inputs : index - numerical index into the data slice
 *
 * Returns : true on success , false on falure
 *
 * Error Conditions : index > fNReadWrite
 *                    Dataset access fails
 *                    DataSpace fails
 * 
 * Unit Tested on: 03-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double H5Logger::RowData(size_t Column) 
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    if (Column < fNVariables)
    {
	return fVariables[Column];
    }
    SetError(-1, __LINE__);
    return 0;
}

/**
 ******************************************************************
 *
 * Function Name : H5Logger::DatasetReadColumn
 *
 * Description : read a column of data in NVector elements at a time. 
 *
 * 
 * Inputs : Column - The column of the named data you want to access. 
 *          Vector - A user supplied vector for reading the data into
 *          NVector- The size of the user supplied vector for reading data. 
 *                   ideally NEntries/NVector has no remainder. 
 *
 * Returns : true on success , false on falure
 *
 * Error Conditions : index > fNReadWrite
 *                    Dataset access fails
 *                    DataSpace fails
 * 
 * Unit Tested on: 03-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool H5Logger::DatasetReadColumn(size_t Column, size_t &Row, 
				 double *Vector, size_t NVector)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    if (Column>=fNVariables)
    {
	SetError(-1, __LINE__);
	return false;
    }
    if (Row>=fNReadWrite)
    {
	SetError(-2, __LINE__);
	return false;
    }
    if (Vector == NULL)
    {
	SetError(-3, __LINE__);
	return false;
    }
    if (NVector == 0)
    {
	SetError(-4, __LINE__);
	return false;
    }

    try
    {
	/*
	 * Select a hyperslab. 
	 */
	hsize_t     offset[2] = {Column,Row};
	/* This is how big a push we want to do for the data we are writing. */
	hsize_t     dims[kDataRank] = {1, NVector};
	/*
	 * This kind of explains how the memory data layout is done. 
	 */
	DataSpace   MVSpace(kDataRank, dims);

	/* Get current file space */
	DataSpace file_space = fUserDataset.getSpace();
	/* Where do we want to write in the hyperslab? */
	file_space.selectHyperslab( H5S_SELECT_SET, dims, offset);


	memset (Vector, 0, NVector*sizeof(double));
	fUserDataset.read( Vector, PredType::NATIVE_DOUBLE, 
			   MVSpace, file_space);
	Row = NVector;

    }
    // catch failure caused by the DataSet operations
    catch( const DataSetIException &error )
    {
	error.printErrorStack();
	SetError(-2,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataSpaceIException &error )
    {
	error.printErrorStack();
	SetError(-3,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataTypeIException &error )
    {
	error.printErrorStack();
	SetError(-4,__LINE__);
	return false;
    }

    return true;
}
/**
 ******************************************************************
 *
 * Function Name : H5Logger::WriteVersionInformation
 *
 * Description : 
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool H5Logger::WriteVersionInformation(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    try
    {
	hsize_t   dims[2];
	const int RANK = 1;

	/* 
	 * ---------------------------------------------------
	 * Setup the dataspace, need dimensions and type
	 */
	dims[0] = 2;  // Number of variables
	dims[1] = 1;  // Max size of string

	// Create a simple dataspace
	DataSpace    DS_Revision( RANK, dims);
	IntType datatype( PredType::NATIVE_UINT );

	/*
	 * Create the dataset
	 *  Name of dataset
	 *  Data type of the dataset
	 *  Dataspace of the dataset
	 *  Creation property. 
	 */
	DataSet FSDataset = fpFile->createDataSet(sVersionHeader,
						 datatype,
						 DS_Revision);

	FSDataset.write( &fRevision, PredType::NATIVE_UINT);
    }
    // catch failure caused by the DataSet operations
    catch( const DataSetIException &error )
    {
	error.printErrorStack();
	SetError(-2,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataSpaceIException &error )
    {
	error.printErrorStack();
	SetError(-3,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataTypeIException &error )
    {
	SetError(-3,__LINE__);
	error.printErrorStack();
	return false;
    }

    return true;
}
/**
 ******************************************************************
 *
 * Function Name : ReadVersionInformation
 *
 * Description : Once the file is open, get the revision information. 
 *               This can be used to modulate the read response
 *               as the version changes, if necessry. 
 *
 * Inputs : NONE
 *
 * Returns : true on success.
 *
 * Error Conditions : Can't find the dataspace. 
 * 
 * Unit Tested on: 06-Mar-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool H5Logger::ReadVersionInformation(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    try
    {
	DataSet dataset = fpFile->openDataSet( sVersionHeader);

	/*
	 * Get filespace for rank and dimension
	 */
	DataSpace filespace = dataset.getSpace();

	dataset.read( &fRevision, PredType::NATIVE_UINT);

    }
    // catch failure caused by the DataSet operations
    catch( const DataSetIException &error )
    {
	error.printErrorStack();
	SetError(-2,__LINE__);
	return false;
    }
    // catch failure caused by the DataSpace operations
    catch( const DataSpaceIException &error )
    {
	error.printErrorStack();
	SetError(-3,__LINE__);
	return false;
    }

    return true;
}
/**
 ******************************************************************
 *
 * Function Name : H5Logger::NVariables
 *
 * Description : return the number of variables in the row of 
 *               the ntuple.
 *
 * Inputs : NONE 
 *
 * Returns : size_t number of variables. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 06-Apr-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
size_t H5Logger::NVariables(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
#if 0
    if (fVariableNames > 0)
    {
	return fVariableNames->NTokens();
    }
    return 0;
#else
    return fNVariables;
#endif
}
/**
 ******************************************************************
 *
 * Function Name : FillInternalVector
 *
 * Description : Add a value to the internal vector at the given location
 *
 * Inputs : 
 *       Val - Value to store
 *       Location - where in the vector, 0:NVariables
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void H5Logger::FillInternalVector(const double &Val, size_t Location)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if (Location<fNVariables)
    {
	fVariables[Location] = Val;
    }
    else
    {
	SetError(-1,__LINE__);
    }
}
/**
 ******************************************************************
 *
 * Function Name : H5Logger::IndexFromName
 *
 * Description : return the index associated with the given name. 
 *
 * Inputs : name to find
 *
 * Returns : size_t index into row for the data. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint32_t H5Logger::IndexFromName (const char *Name)
{
    size_t rc = 999;
    SET_DEBUG_STACK;
    uint32_t i = 0;
    do
    {
	if(strncmp(fVariableNames->Token(i), Name, strlen(Name)) == 0)
	{
	    rc = i;
	    break;
	}
	i++;
    } while(i<fVariableNames->NTokens());

    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : H5Logger::IndexFromName
 *
 * Description : return the index associated with the given name. 
 *
 * Inputs : name to find
 *
 * Returns : size_t index into row for the data. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char* H5Logger::NameFromIndex(uint32_t i)
{
    const char *rc = NULL;
    SET_DEBUG_STACK;

    if (i<fVariableNames->NTokens())
    {
	rc = fVariableNames->Token(i);
    }
    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : HeaderInfo
 *
 * Description : Retrieve the header information like:
 * 0 - Filename
 * 1 - Creation Date
 * 2 - Dataset Name 
 *
 * Inputs : Index to header info that you want to retrieve
 *
 * Returns : String info from header
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 02-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char *H5Logger::HeaderInfo(HeaderIndex idx)
{
    const char *rc = NULL;
    SET_DEBUG_STACK;
    if (idx<kCOLS)
    {
	rc = fHeader[idx];
    }
    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : H5ParseTime
 *
 * Description : convert the string file creation time to struct tm
 *
 * Inputs : character value containing a time string like: 
 *         "2024-02-12 02:43:44"
 *
 * Returns : formed struct tm
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 13-Feb-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
struct tm *H5ParseTime(const char *val)
{
    static struct tm rv;
    string Time(val);
    string tmp;
    size_t pos;

    memset (&rv, 0, sizeof(struct tm));
    /*
     * Looks something like this. 
     * "2024-02-12 02:43:44"
     */ 
    if ((val != NULL) && (strlen(val)>18))
    {
	pos = Time.find('-');
	tmp = Time.substr(0,pos);
	rv.tm_year = stoi(tmp) - 1900;
	Time.erase(0, pos+1);

	pos = Time.find('-');
	tmp = Time.substr(0,pos);
	rv.tm_mon = stoi(tmp) - 1;
	Time.erase(0, pos+1);

	pos = Time.find(' ');
	tmp = Time.substr(0,pos);
	rv.tm_mday = stoi(tmp);
	Time.erase(0, pos+1);

	pos = Time.find(':');
	tmp = Time.substr(0,pos);
	rv.tm_hour = stoi(tmp);
	Time.erase(0, pos+1);

	pos = Time.find(':');
	tmp = Time.substr(0,pos);
	rv.tm_min = stoi(tmp);
	Time.erase(0, pos+1);

	rv.tm_sec = stoi(Time);
    }
    return &rv;
}

/**
 ******************************************************************
 *
 * Function Name : ostream operator << 
 *
 * Description : friend operator overloaded to dump the contents
 *               of the Logger class for debugging. 
 *
 * Inputs : output - ostream 
 *          n - H5Logger to format into output stream. 
 *
 * Returns : constructed output stream. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 02-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const H5Logger &n)
{
    SET_DEBUG_STACK;
    size_t i;
    output << "-----------------------------------" << endl
	   << "H5Logger Revision: " << n.fRevision[0] << "."  << n.fRevision[1]
	   << endl
	   << "    File Name: " << n.fHeader[0] << endl
	   << "    Creation Date: " << n.fHeader[1] << endl
	   << "    User Data Name:" << n.fHeader[2] << endl;

    

    if (strlen(n.fHeader[3])>0) 
	output << "    Comment: " << n.fHeader[3];
    output << endl;

    output << "  Number Variables: " << n.fVariableNames->NTokens() << endl
	 << "  Variable Names: " << endl;
    for (i=0; i<n.fVariableNames->NTokens();i++)
    {
	output << "    " << i << " " << n.fVariableNames->Token(i) << endl;
    }

    cout << "Variable data, Number of entries: " << n.fNReadWrite
	 << " max size: " << n.fMaxSize << endl;
    for (i=0; i<n.fNVariables;i++)
    {
	output << "    " << i << " " << n.fVariables[i] << endl;
    }

    return output;
}
