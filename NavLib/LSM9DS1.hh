/**
 ******************************************************************
 *
 * Module Name : LSM9DS1.h
 *
 * Author/Date : C.B. Lirakis / 27-Feb-16
 *
 * Description : 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 02-Apr-22  CBL changed name to reflect the real sensor used. 
 *
 * Classification : Unclassified
 *
 * References :
 *
 *******************************************************************
 */
#ifndef __LSM9DS1_hh_
#define __LSM9DS1_hh_
# include "Point.hh"

/// LSM9DS1 documentation here. 
class LSM9DS1 
{
public:
    friend ostream& operator<<(ostream& output, const LSM9DS1 &in);

    /// Default Constructor
    LSM9DS1(void);
    /// Default destructor
    ~LSM9DS1(void);
    /// LSM9DS1 function
    void Decode(const char* line);

    /* In Degrees. */
    inline double Heading(void) const {return fHeading;};
    inline double Roll(void)    const {return fRoll;};
    inline double Pitch(void)   const {return fPitch;};

    /* in mg */
    inline const Point& Acceleration(void) const {return fA;};
    /* in milli-deg per sec */
    inline const Point& Gyro(void) const {return fG;};

    /* in milligauss */
    inline const Point& Magnetic(void) const {return fM;};

    inline void X0(double x) {fX0=x;};
    inline void Y0(double x) {fY0=x;};

    inline void SetLP_A(double a) {fLP_A = a;};
    inline void SetLP_G(double a) {fLP_G = a;};
    inline void SetLP_M(double a) {fLP_M = a;};

    inline double LP_A(void) const {return fLP_A;};
    inline double LP_G(void) const {return fLP_G;};
    inline double LP_M(void) const {return fLP_M;};

private:
    void   ApplyScale(void);
    void   Filter(void);
    void   CalculateAttitude(void);
    double CalculateHeading(void);

    bool   fScale;              /* Turn scaling on/off */
    Point  fG,fA,fM;            /* Raw data */
    Point  fLG, fLA, fLM;       /* Filtered data */
    double fLP_G, fLP_A, fLP_M; /* Low pass filter coef. */

    double fRoll, fPitch, fHeading;
    double fMagScale, fAccScale, fGyroScale;
    double fX0, fY0;            /* Center for heading info. */
};
#endif
