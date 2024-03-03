/**
 ******************************************************************
 *
 * Module Name : SimpleCommand.hh
 *
 * Author/Date : C.B. Lirakis / 26-Feb-24
 *
 * Description : Global signal to indicate a file change
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
#ifndef __SIMPLECOMMAND_hh_
#define __SIMPLECOMMAND_hh_
#    include <cstdint>
#    include "SharedMem2.hh"

constexpr uint32_t kNAMELENGTH=32;
struct CommandStructure
{
    // 32 bit wide string for naming a specific process to target.
    char ProcessName[kNAMELENGTH]; 
    uint32_t Command;
};

/// SimpleCommand documentation here. 
class SimpleCommand : public SharedMem2
{
public:
    /*!
     * Divide up the 32 bit word into pieces. 
     * Upper 8 bits for addressing a specific process type. 
     * Zero means the message was meant for all. 
     */
    enum COMMANDS{kNONE=0, kCHANGE_FILE_NAME=0x0001, kSHUTDOWN=0x0002, 
		  kINVALID = 0xFFFF};

    /*!
     * Name - Name used for consumer, ignored on Source declaration. 
     * 
     * Source or consumer of information?
     * Source = true  - source
     * Source = false - consumer.
     */
    SimpleCommand(bool Source=false, const char *Name="NULL");

    /// Default destructor
    ~SimpleCommand(void);

    /*!
     * Description: 
     *   If source, send a bit encoded command message 
     *   If consumer return the request information. 
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    struct CommandStructure *Command(const char *Name, uint32_t Command);

    /*!
     * Description: 
     *   Either can reset the request. 
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    void Reset(void);

private:
    char fMyName[kNAMELENGTH]; 
    struct CommandStructure fMyCommand;
    bool fSource; 
};
#endif
