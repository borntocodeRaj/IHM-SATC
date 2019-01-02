#if !defined(COPTIMIZATION_H)
#define COPTIMIZATION_H

// COptimization.h : header file
//

#define MAX_CMPT 20  // maximum number of compartments should not exceed 20
#define MAX_NSTLVL 20  // maximum number of nest levels (bkts) should not exceed 20

#define OPT_NET_NESTING 0
#define OPT_THRESHOLD_NESTING 1

#define STDV_FACTOR_RATE  0.59
#define ZERO_EPSILON      0.000001

#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#define __min(a,b)  (((a) < (b)) ? (a) : (b))

#ifndef max
template <class T>
inline T max(T a,T b)
{
  return  (((a) > (b)) ? (a) : (b));
}

template <class T>
inline T min(T a,T b)
{
  return  (((a) < (b)) ? (a) : (b));
}
#endif

/////////////////////////////////////////////////////////////////////////////
// COptSysParms object

class COptSysParms
{
  friend class COptimization;

public:
  // Constructor
  COptSysParms::COptSysParms();
  COptSysParms::COptSysParms(
  int iInventoryMethod,
  unsigned short bUseUnresCap,
  unsigned short bOvbModel,
  int iTotNumNestLevels,
  double dRecaptureRate,
  double dUpgrade
  );
  COptSysParms::~COptSysParms();

  void SetNestMethod(int iInventoryMethod) { m_iInventoryMethod = iInventoryMethod; }
  void SetUseUnresCap(unsigned short bUseUnresCap) { m_bUseUnresCap = bUseUnresCap; }
  void SetOvbModel(unsigned short bOvbModel) { m_bOvbModel = bOvbModel; }
  void SetTotNumNestLevels(int iTotNumNestLevels) { m_iTotNumNestLevels = iTotNumNestLevels; }
  void SetRecaptureRate(double dRecaptureRate) { m_dRecaptureRate = dRecaptureRate; }
  void SetUpgrade(double dUpgrade) { m_dUpgrade = dUpgrade; }

private:
  int m_iInventoryMethod;
  unsigned short m_bUseUnresCap;
  unsigned short m_bOvbModel;
  int m_iTotNumNestLevels;
  double m_dRecaptureRate;
  double m_dUpgrade;
};

/////////////////////////////////////////////////////////////////////////////
// COptNstLvl object

class COptNstLvl
{
  friend class COptimization;

public:
  COptNstLvl::COptNstLvl();
  COptNstLvl::COptNstLvl(
      int iNestLevel,
      int iResHoldGrp,
      int iResHoldIdv,
      int iResHoldNotYield,
      double dAddDmdFcst,
      double dAddDmdStdvFcst,
      int iDmdFcstAvgRev,
      int iAuth = 0,  /* backward compatibility to Revenue Mix Model */
      int iCerIdv = 0  /* backward compatibility to Revenue Mix Model */
    );
  COptNstLvl::~COptNstLvl();

  int GetNestLevel() { return m_iNestLevel;  }

  int GetResHoldGrp() { return m_iResHoldGrp; }
  void SetResHoldGrp(int iResHoldGrp) { m_iResHoldGrp = iResHoldGrp; }
  int GetResHoldIdv() { return m_iResHoldIdv; }
  void SetResHoldIdv(int iResHoldIdv) { m_iResHoldIdv = iResHoldIdv; }
  int GetResHoldNotYield() { return m_iResHoldNotYield; }
  void SetResHoldNotYield(int iResHoldNotYield) { m_iResHoldNotYield = iResHoldNotYield; }
  int GetDmdFcstAvgRev() { return m_iDmdFcstAvgRev; }
  void SetDmdFcstAvgRev(int iDmdFcstAvgRev) { m_iDmdFcstAvgRev = iDmdFcstAvgRev; }
  double GetAddDmdFcst();
  void SetAddDmdFcst(double dAddDmdFcst) { m_dAddDmdFcst = dAddDmdFcst; }
  double GetAddDmdStdvFcst();
  void SetAddDmdStdvFcst(double dAddDmdStdvFcst) { m_dAddDmdStdvFcst = dAddDmdStdvFcst; }
  int GetAuth() { return m_iAuth; }
  void SetAuth(int iAuth) { m_iAuth = iAuth; }
  int GetAvail() { return m_iAvail; }
  void SetAvail(int iAvail) { m_iAvail = iAvail; }

  int GetCerIdv() { return m_iCerIdv; }
  void SetCerIdv(int iCerIdv) { m_iCerIdv = iCerIdv; }

private:
  int m_iNestLevel;  // the nest level

  int m_iResHoldGrp;
  int m_iResHoldIdv;
  int m_iResHoldNotYield;
  int m_iDmdFcstAvgRev;
  int m_iAuth;  // output 
  int m_iAvail; // output;
  double m_dAddDmdFcst;
  double m_dAddDmdStdvFcst;

  // backward compatibility with Revenue Mix model
  int m_iCerIdv;
};

/////////////////////////////////////////////////////////////////////////////
// COptimization object

class COptimization
{
public:
  // Constructor
  COptimization(
    char* pszCmpt,
    COptSysParms* pOptSysParms,
    long lCollectionDate,
    int iCapRes = 0,
    int iCapWU = 0
    );
	~COptimization();

	void Clean();

  // mutators and accessors of compartment level data
  char* GetCmpt() { return m_szCmpt; }
  void SetCmpt(char* pszCmpt) { 
    memset(m_szCmpt, '\0', sizeof(m_szCmpt));
    (strlen(pszCmpt) >= sizeof(m_szCmpt)) ?
      memcpy(m_szCmpt, pszCmpt, sizeof(m_szCmpt)-1) :
      memcpy(m_szCmpt, pszCmpt, strlen(pszCmpt));
  }
  int GetNstLvlCnt();  // return number of loaded buckets for a given compartment
  COptNstLvl* GetOptNstLvl(int iNstLvlIndex) { return m_apOptNstLvl[iNstLvlIndex]; }
  void Add(COptNstLvl* pOptNstLvl);

  int GetCapRes() {return m_iCapRes; }
  void SetCapRes(int iCapRes) {m_iCapRes = iCapRes; }
  int GetCapWU() {return m_iCapWU; }
  void SetCapWU(int iCapWU) {m_iCapWU = iCapWU; }
  int GetAuthFc() { return m_iAuthFc; }
  void SetAuthFc(int iAuthFc) { m_iAuthFc = iAuthFc; }

  // these mutator methods simply set the appropriate value in the Sys Parms object
  void SetNestMethod(int iInventoryMethod) { m_pOptSysParms->m_iInventoryMethod = iInventoryMethod; }
  void SetUseUnresCap(unsigned short bUseUnresCap) { m_pOptSysParms->m_bUseUnresCap = bUseUnresCap; }
  void SetOvbModel(unsigned short bOvbModel) { m_pOptSysParms->m_bOvbModel = bOvbModel; }
  void SetTotNumNestLevels(int iTotNumNestLevels) { m_pOptSysParms->m_iTotNumNestLevels = iTotNumNestLevels; }
  void SetRecaptureRate(double dRecaptureRate) { m_pOptSysParms->m_dRecaptureRate = dRecaptureRate; }
  void SetUpgrade(double dUpgrade) { m_pOptSysParms->m_dUpgrade = dUpgrade; }

  // these mutator methods simply set the appropriate value in the Sys Parms object
  int GetNestMethod() { return m_pOptSysParms->m_iInventoryMethod; }
  unsigned short GetUseUnresCap() { return m_pOptSysParms->m_bUseUnresCap; }
  unsigned short GetOvbModel() { return m_pOptSysParms->m_bOvbModel; }
  int GetTotNumNestLevels() { return m_pOptSysParms->m_iTotNumNestLevels; }
  double GetRecaptureRate() { return m_pOptSysParms->m_dRecaptureRate; }
  double GetUpgrade() { return m_pOptSysParms->m_dUpgrade; }

  // this mutator method sets the collection date
  void SetCollectionDate(long lCollectionDate) { m_lCollectionDate = lCollectionDate; }

  void Optimize();  // method to perform optimization

  // accessor methods for output values
  int GetAuth(int iIndex);
  int GetAvail(int iIndex);

protected:
  COptSysParms* m_pOptSysParms;
  long m_lCollectionDate;

  char m_szCmpt[10];  // the compartment  (10 should be plenty large)
  COptNstLvl* m_apOptNstLvl[MAX_NSTLVL];  // array to pointers of nest level data for this cmpt

  int m_iCapRes;
  int m_iCapWU;
  int m_iAuthFc;
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(COPTIMIZATION_H)
