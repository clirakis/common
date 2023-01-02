/**
 ******************************************************************
 *
 * Module Name : StatusAndEvent.hh
 *
 * Author/Date : C.B. Lirakis / 24-Jan-21
 *
 * Description : Determine the configuration of the mainframe and
 *               it's plug in modules. 
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
#ifndef __STATUSANDEVENT_hh_
#define __STATUSANDEVENT_hh_
#  include <string>
#  include "CObject.hh"
#  include "Module.hh"



/// StatusAndEvent documentation here. 
class StatusAndEvent : public CObject
{
public:
    /*!
     * Description: 
     *   StatusAndEvent command set encapsulation. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    StatusAndEvent(void);

    /*!
     * Description: 
     *   StatusAndEvent command set encapsulation. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    ~StatusAndEvent(void);


    /*!
     * Description: 
     *   Query mainframe configuration - which modules are installed?
     *   (CONFIG?)
     *   tells me all about all the modules and thier channels etc. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    bool Configuration(void);

    /*!
     * Description: 
     *   Query event code information (EVENT?)
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    bool Event(void);

    /*!
     * Description: 
     *   Query version numbers of systems (ID?)
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline bool VersionID(void) const {return true;};

    /*!
     * Description: 
     *   Query  the ID of the probe with the last button press. (IDPROBE?)
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline bool ID_Probe(void) const {return true;};


    /*!
     * Description: 
     *   Query the status  GPIB only (RQS?)
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    true if on. 
     */
    bool RQS(void);

    /*!
     * Description: 
     *   Set RQS
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    bool SetRQS(bool ON);

    unsigned char GetStatus(void);

    /*!
     * Description: 
     *   Query mask values for events
     *   page 337
     *   I think the status is in the iberr bit. 
     *   This is accessed from GPIB 
     * 
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline bool SRQ_Mask(void) const {return true;};

    /*!
     * Description: 
     *   Set
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool SQR_Mask(int val) {return true;};

 
    /*!
     * Description: 
     *   Get the serial numbers of the of the DSA and its plug in modules. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    true on success. 
     */
    bool UID(void);

    /* ======================================================== */
    /*         FUNCTIONS To Access the data                     */ 
    /* ======================================================== */

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
    inline unsigned char GetNModule(void) const {return fNModule;};

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
    inline Module*   GetModule(unsigned char n) {if (n<fNModule) 
	    return fModule[n]; else return NULL;};

    /*!
     * Description: 
     *   print out the entire data about this class. 
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    friend ostream& operator<<(ostream& output, const StatusAndEvent &n); 


#ifdef DEBUG_SAE
    /*!
     * Description: 
     *     Perform tests on the module class - used for debugging
     *
     * Arguments:
     *     NONE
     *
     * returns:
     *    NONE
     */
    void Test(void);
#endif


private:
    unsigned char  fNModule;        // number of plug in units.
    std::string    fUnitSerial;     // Mainframe serial number
    Module*        fModule[3];      // pointer to calls to modules
    unsigned char  fStatus;         // Status byte page 338
};
#endif
