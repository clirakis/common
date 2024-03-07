/**
 ******************************************************************
 *
 * Module Name : Average.hh
 *
 * Author/Date : C.B. Lirakis / 28-June-08
 *
 * Description :
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 03-Mar-24 Change to std:vector
 *
 * Classification : Unclassified
 *
 * References :
 *
 *******************************************************************
 */
#ifndef  __AVERAGE_hh_
# define __AVERAGE_hh_
# include <vector>

class Average
{
public:
    Average(size_t Nele);
    void   AddElement(double val);
    double Sigma(double *avg = NULL);
    double Get(void);
    void   Reset(void);
    inline int  CurrentPointer(void) {return fCurrentPointer;};
    friend ostream& operator<<(ostream& output, Average &n); 

protected:
    vector<double> fData;

private:
    void    Add(double val);

    size_t  fRejectCount, fCurrentPointer;
    bool    fFull;
};

#endif
