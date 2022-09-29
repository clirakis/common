/**
 ******************************************************************
 *
 * module Name : Split.hh
 *
 * Author/Date : C.B. Lirakis / 02-May-20
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
#ifndef __SPLIT_h_
#define __SPLIT_h_
/// Split documentation here. 
class Split {
public:
    /*!
     * Default Constructor
     * Names - character string that is desired to be tokenized.
     * delim - the string delimiter that we want to use to tokenize 
     *         the input string.  
     */
    Split(const char *Names, const char delim);
    /// Default destructor
    ~Split();
    /*! 
     * Split function, index through the available tokens
     * Index - the location of the desired token. 
     * ERROR: if Index => fNTokens, error condition is to return a NULL. 
     */
    const char* Token(size_t Index) const;

    /*!
     * Find a specific charter token in the list. 
     * If it does not exist return a -1 indicating an error. 
     */
    int FindToken(const char *needle) const;

    /*! Helper function to return the number of tokens available. */
    inline size_t NTokens(void) const {return fNTokens;};

    /*! Enable a more friendly way of printing the contents of the class. */
    friend std::ostream& operator<<(std::ostream& output, const Split &n);

private:

    /*!
     * Internal array of strings that the input string was split into. 
     */
    char **fTokens;
    /*!
     * Number of tokens that the input string was split into. 
     */
    size_t fNTokens;
};
#endif
