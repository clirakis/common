/**
 ******************************************************************
 *
 * Module Name : GParse.hh
 *
 * Author/Date : C.B. Lirakis / 26-Jan-14
 *
 * Description : header file for DS602 digitizing scope with GParse 
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
#ifndef __GParse_h_
#define __GParse_h_
#include <string>
#include <vector>

/// GParse documentation here. 
class GParse 
{
public:
    enum eTYPES {TYPE_NONE=0,TYPE_STRING, TYPE_INT, TYPE_FLOAT};

    /*!
     * Description: 
     *   Constructor, without a string input, this is basically a no-op. 
     *
     * Arguments:
     *   string to parse. 
     *
     * returns:
     *    ...
     */
    GParse(const char *pstr=NULL);

    /*!
     * Description: 
     *   Destructor - clean up this mess. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    NONE
     */
    ~GParse();
    /*!
     * Description: 
     *   returns the number of arguments parsed out of the string
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    Number args in the fTokenVals vector. 
     */
    inline unsigned char NArg(void) const {return fTokenVals.size();};

    /*!
     * Description: 
     *   Return the string command value associated with this response
     *   that is being parsed. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    const char* Command(void) const;

    /*!
     * Description: 
     *   Return the character value assocated with the input string name. 
     *   In the event this is not found in the vector, return NULL. 
     *
     * Arguments:
     *   name - Name to find in the vector storing all the tokens.
     *
     * returns:
     *   NULL on failure to find the string, otherwise the assocated string. 
     *    
     */
    const char*  Value(const char *name = NULL);

    /*!
     * Description: 
     *   For the found value, determine what type it is. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    one of the types found in eTYPES above. 
     */
    int ValueType(void) const;


    /*!
     * Description: 
     *   return the character string associated with the index in
     *   the value vector that has been parsed. 
     *
     * Arguments:
     *   index {0:fTokenVals.size()}
     *
     * returns:
     *    string stored in vector. 
     */
    const char* IndexedValue(size_t index) const;

    /*!
     * Description: 
     *   output the entire structure to the specified stream
     *
     * Arguments:
     *   
     *
     * returns:
     *    formatted stream. 
     */
    friend ostream& operator<<(ostream& output, const GParse &n); 
  
private:
    size_t  fNarg;     // Number of arguments found
    string  *fLine;    // Line to parse. 
    string  *fCommand; // command assocated with input string.
    string  *fTmp; 
    vector<string> fTokenVals;  // A vector of tokenized values. 
};
#endif
