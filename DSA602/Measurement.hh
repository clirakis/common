/**
 ******************************************************************
 *
 * Module Name : Measurement.hh
 *
 * Author/Date : C.B. Lirakis / 11-Jan-01
 *
 * Description : 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : DSA602 Programmers Reference manual, page: 191
 *
 *******************************************************************
 */
#ifndef __MEASUREMENT_hh_
#define __MEASUREMENT_hh_
#  include "CObject.hh"
#  include "MValue.hh"

/// Measurement documentation here. 
class Measurement : public CObject
{
public:
    /*!
     * Description: 
     *   Default Constructor for measurements. 
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    Measurement(void);

    /*!
     * Description: 
     *   Default destructor for measurements. 
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    ~Measurement();


    /* ======================================================== */
    /*                      FUNCTIONS                           */ 
    /* ======================================================== */

    /*!
     * Description: 
     *   Query all items in the MSLIST 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline bool Update(void) {return Query("MEAS");};


    /*!
     * Description: 
     *   Query The time from the trigger point to a specified reference
     *   level crossing
     *
     * Arguments:
     *   q - perform query (default false)
     *
     * returns:
     *    MValue - Value and value qualifier. 
     */
    inline const MValue& Cross(bool q=false)  
	{if(q) Query("CROSS"); return fCross;};

    /*!
     * Description: 
     *   Query the time between the first and last mesial crossing
     *   within the measurement zone
     *
     * Arguments:
     *   q - perform query (default false)
     *
     * returns:
     *    MValue - Value and value qualifier. 
     */
    inline const MValue& Delay(bool q=false) 
	{if(q) Query("DELAY"); return fDelay;};


    /*!
     * Description: 
     *   Query The percentage of a period that a waveform spends
     *   above the mesial level
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Duty(bool q=false) 
	{if(q) Query("DUTY"); return fDuty;};

    /*!
     * Description: 
     *   Query The transiiton time of a falling pulse edge from 
     *   the distal to proximal levels.
     *   Requires that the waveform be enveloped. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Falltime(bool q=false) 
	{if(q) Query("FALLTIME"); return fFalltime;};


    /*!
     * Description: 
     *   Query The Frequencey of the waveform or 1/T
     *   Requires that the waveform be enveloped. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Frequency(bool q=false) 
	{if(q) Query("FREQ"); return fFreq;};

    /*!
     * Description: 
     *   Query The Ratio of the peak-to-peak amplitudes of the 
     *   reference waveform verses the selected waveform. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Gain(bool q=false) 
	{if(q) Query("GAIN"); return fGain;};

    /*!
     * Description: 
     *   Query The maximum amplitude of the waveform
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Max(bool q=false) 
	{if(q) Query("MAX"); return fMax;};

    /*!
     * Description: 
     *   Query the average (arithmatic mean voltage) of the waveform
     *   Requires that DAINT is set to single. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Mean(bool q=false) 
	{if(q) Query("MEAN"); return fMean;};

    /*!
     * Description: 
     *   Query Th amplitude midpoint voltage.
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Midpoint(bool q=false) 
	{if(q) Query("MID"); return fMidpoint;};

    /*!
     * Description: 
     *   Query the minimum amplitude
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Min(bool q=false) 
	{if(q) Query("MIN"); return fMin;};


    /*!
     * Description: 
     *   Query the difference between the maximum amplitude and the
     *   topline value, given as a percentage of the difference between
     *   the topline and baseline values. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Overshoot(bool q=false) 
	{if(q) Query("OVE"); return fOvershoot;};

    /*!
     * Description: 
     *   Query The propagation delay between mesial crossings on 
     *   two waveforms (see DLYTRACE command)
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& PDelay(bool q=false) 
	{if(q) Query("PDE"); return fPDelay;};

    /*!
     * Description: 
     *   Query the time between the first and next mesial crossings of the 
     *   same slope. Also 1/F
     *   Requires that the waveform be enveloped. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Period(bool q=false) 
	{if(q) Query("PER"); return fPeriod;};

    /*!
     * Description: 
     *   Query the phase relationship between a reference waveform and 
     *   selected waveform. (+/- 360 degrees)
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Phase(bool q=false) 
	{if(q) Query("PHA"); return fPhase;};

    /*!
     * Description: 
     *   Query the peak-to-peak value, or the voltage difference between
     *   the maximum and minimum amplitude. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& PeakToPeak(bool q=false) 
	{if(q) Query("PP"); return fPeakToPeak;};

    /*!
     * Description: 
     *   Query the transition time of a rising pulse edge from the proximal
     *   to distal levels. 
     *   Requires that the waveform be enveloped. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Risetime(bool q=false) 
	{if(q) Query("RIS"); return fRisetime;};

    /*!
     * Description: 
     *   Query the true Root-mean-square voltage.
     *   Requires that DAINT is set to single. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& RMS(bool q=false) 
	{if(q) Query("RMS"); return fRMS;};

    /*!
     * Description: 
     *   Query the spectrial frequency (harmonic or peak.)
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& SpectralFrequency(bool q=false) 
	{if(q) Query("SFR"); return fSpectralFrequency;};

    /*!
     * Description: 
     *   Query The propagation delay or time delay between mesial crossings
     *   on two different waveforms. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Skew(bool q=false) 
	{if(q) Query("SKEW"); return fSkew;};

    /*!
     * Description: 
     *   Query the spectrial magnitude (harmonic or peak)
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline const MValue& SpectralMagnitude(bool q=false) 
	{if(q) Query("SMA"); return fSpectralMagnitude;};

    /*!
     * Description: 
     *   Query The total harmonic distortion
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& TotalHarmonicDistortion(bool q=false) 
	{if(q) Query("THD"); return fTotalHarmonicDistortion;};

    /*!
     * Description: 
     *   Query the time between the main trigger point and the window 
     *   trigger point
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& TimeToTrigger(bool q=false) 
	{if(q) Query("TTR"); return fTimeToTrigger;};

    /*!
     * Description: 
     *   Query The difference between the baseline value and the 
     *   minimum amplitude, given as a percentage of the difference 
     *   between the topline and baseline values.
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Undershoot(bool q=false) 
	{if(q) Query("UND"); return fUndershoot;};

    /*!
     * Description: 
     *   Query The time between the first and next mesial crossing
     *   of the opposite slope. 
     *   Requires that the waveform be enveloped. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& Width(bool q=false) 
	{if(q) Query("WID"); return fWidth;};

    /*!
     * Description: 
     *   Query The energy represented uner the curve of a Yt waveform.
     *   (Can be divided by the resistance of the circuit to yield 
     *    power measurements.)
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& YTEnergy(bool q=false) 
	{if(q) Query("YTE"); return fYTEnergy;};

    /*!
     * Description: 
     *   Query The difference between the area under a Yt curve above
     *   a specified reference level, and the area under the curve
     *   below that level
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& YTMNS_Area(bool q=false) 
	{if(q) Query("YTM"); return fYTMNS_Area;};

    /*!
     * Description: 
     *   Query The absolute value of all areas between a Yt waveform
     *   and a user-specified reference level.
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline const MValue& YTPLS_Area(bool q=false) 
	{if(q) Query("YTP"); return fYTPLS_Area;};

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
    friend ostream& operator<<(ostream& output, const Measurement &n); 


private:

    bool Query(const char *Command);

    void Decode(const char *Command, const char* Response);

    char fResponse[64];   // Response to the Query. Parse above. 

    MValue fCross;
    MValue fDelay;
    MValue fDuty;
    MValue fFalltime;
    MValue fFreq;        // 
    MValue fGain;
    MValue fMax;
    MValue fMean;
    MValue fMidpoint;
    MValue fMin;
    MValue fOvershoot;
    MValue fPDelay;
    MValue fPeriod;
    MValue fPhase;
    MValue fPeakToPeak;
    MValue fRisetime;
    MValue fRMS;
    MValue fSpectralFrequency;
    MValue fSkew;
    MValue fSpectralMagnitude;
    MValue fTotalHarmonicDistortion;
    MValue fTimeToTrigger;
    MValue fUndershoot;
    MValue fWidth;
    MValue fYTEnergy;
    MValue fYTMNS_Area;
    MValue fYTPLS_Area;
};
#endif
