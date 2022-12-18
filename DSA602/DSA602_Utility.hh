/**
 ******************************************************************
 *
 * Module Name : DSA602_Utility.hh
 *
 * Author/Date : C.B. Lirakis / 29-Nov-14
 *
 * Description : 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : DSA602A Programming Reference Manual
 *
 * Commands are in the format of 
 * 
 *******************************************************************
 */
#ifndef __DSA602_UTILITY_h_
#define __DSA602_UTILITY_h_
#  include "DSA602_Types.hh"

    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    void RemoveQuotes(char *s);
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    void RemoveWhiteSpace(char *s);
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    const char* PT_String(PT_TYPES);
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    char SlotChar(SLOT);
    /*!
     * Description: 
     *   Parse out a double from the return value from a command
     *
     * Arguments:
     *   command - the command issued, performs a check
     *   instr   - input string to parse
     *
     * returns:
     *    double value from the command. 
     */
    double      Parse(const char *command, const char *instr);
    /*!
     * Description: 
     *   Parse out a string from the return value from a command
     *
     * Arguments:
     *   command - the command issued, performs a check
     *   instr   - input string to parse
     *
     * returns:
     *    string value from the parse.
     */
    const char* SParse(const char *command, const char *instr);

#endif
