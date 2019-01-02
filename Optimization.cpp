// COptimization.cpp : implementation file
//

#include <afxwin.h>         // MFC core and standard components

#include "string.h"
#include "math.h"
#include "Optimization.h"

/////////////////////////////////////////////////////////////////////////////
// COptSysParms

COptSysParms::COptSysParms(){}; // JSZ

COptSysParms::COptSysParms(
  int iInventoryMethod,
  unsigned short bUseUnresCap,
  unsigned short bOvbModel,
  int iTotNumNestLevels,
  double dRecaptureRate,
  double dUpgrade
  )
{
  m_iInventoryMethod = iInventoryMethod;
  m_bUseUnresCap = bUseUnresCap;
  m_bOvbModel = bOvbModel;
  m_iTotNumNestLevels = iTotNumNestLevels;
  m_dRecaptureRate = dRecaptureRate;
  m_dUpgrade = dUpgrade;
}

COptSysParms::~COptSysParms()
{
}

/////////////////////////////////////////////////////////////////////////////
// COptNstLvl object

COptNstLvl::COptNstLvl(
    int iNestLevel,
    int iResHoldGrp,
    int iResHoldIdv,
    int iResHoldNotYield,
    double dAddDmdFcst,
    double dAddDmdStdvFcst,
    int iDmdFcstAvgRev,
    int iAuth, /* = 0 */  /* backward compatibility to Revenue Mix Model */
    int iCerIdv /* = 0 */  /* backward compatibility to Revenue Mix Model */
  )
{
  m_iNestLevel = iNestLevel;
  m_iResHoldGrp = iResHoldGrp;
  m_iResHoldIdv = iResHoldIdv;
  m_iResHoldNotYield = iResHoldNotYield;
  m_iDmdFcstAvgRev = iDmdFcstAvgRev;
  m_dAddDmdFcst = dAddDmdFcst;
  m_dAddDmdStdvFcst = dAddDmdStdvFcst;

  m_iAuth = iAuth;   /* backward compatibility to Revenue Mix */
  m_iCerIdv = iCerIdv;   /* backward compatibility to Revenue Mix */

  m_iAvail = 0;
}

COptNstLvl::~COptNstLvl()
{
}

double COptNstLvl::GetAddDmdFcst() 
{ 
	return m_dAddDmdFcst;
}
double COptNstLvl::GetAddDmdStdvFcst() 
{ 
    return m_dAddDmdStdvFcst;
}

/////////////////////////////////////////////////////////////////////////////
// COptimization

COptimization::COptimization(
    char* pszCmpt,
    COptSysParms* pOptSysParms,
    long lCollectionDate,
    int iCapRes /* = 0 */, 
    int iCapWU /* = 0 */
    )
{
  // set the compartment to be optimized...careful of the string length
  SetCmpt(pszCmpt);

  // set the systems parameter object
  m_pOptSysParms = pOptSysParms;

  // set the collection date
  m_lCollectionDate = lCollectionDate;

  // set remaining member variables
  m_iCapRes = iCapRes;
  m_iCapWU = iCapWU;

  m_iAuthFc = 0;  // output

  for( int iIndex = 0; iIndex < MAX_NSTLVL; iIndex++ )
    m_apOptNstLvl[iIndex] = 0;
}

COptimization::~COptimization()
{
  if (m_pOptSysParms)
    delete m_pOptSysParms;
  m_pOptSysParms = 0;
  for( int iIndex = 0; iIndex < MAX_NSTLVL; iIndex++ )
  {
    if (m_apOptNstLvl[iIndex])
      delete m_apOptNstLvl[iIndex];
    m_apOptNstLvl[iIndex] = 0;
  }
}

// clean the optimization object of the last compartment; get ready for the next
void COptimization::Clean()
{
  memset(m_szCmpt, '\0', sizeof(m_szCmpt));

  m_iCapRes = 0;
  m_iCapWU = 0;
  m_iAuthFc = 0;

  for( int iIndex = 0; iIndex < MAX_NSTLVL; iIndex++ )
  {
    if (m_apOptNstLvl[iIndex])
      delete m_apOptNstLvl[iIndex];
    m_apOptNstLvl[iIndex] = 0;
  }
}

void COptimization::Add(COptNstLvl* pOptNstLvl)
{
  for( int iIndex = 0; iIndex < MAX_NSTLVL; iIndex++ )
    if (m_apOptNstLvl[iIndex] == 0)
    {
      m_apOptNstLvl[iIndex] = pOptNstLvl;
      break;
    }
}

int COptimization::GetNstLvlCnt()  // return number of loaded buckets for a given compartment
{
  for( int iIndex = 0; iIndex < MAX_NSTLVL; iIndex++ )
    if (m_apOptNstLvl[iIndex] == 0)
      return iIndex;  // there are this many
  return 0;  // there are none.
}

void COptimization::Optimize()
{
#ifdef _DEBUG_REVMIX
  TRACE( "Executing Optimization model for Compartment %s\n", GetCmpt() );
#endif

  // System Parameters
  //
  double dRecapture = GetRecaptureRate();
  double dUpgrade   = GetUpgrade();
  double dRecaptureRate = dRecapture / 100;
  double dUpgradeRate   = dUpgrade / 100;

  double dUpRecapFctr1 = dUpgradeRate + dRecaptureRate - dUpgradeRate * dRecaptureRate;
  double dUpRecapFctr2 = (1.0 - dRecaptureRate) * (1.0 - dUpgradeRate);
  double dProt_I = 0;

#ifdef _DEBUG_REVMIX
  TRACE( "Recapture Rate (Sys Parms) = %f\n",  dRecaptureRate );
  TRACE( "Upgrade Rate (Sys Parms) = %f\n",  dUpgradeRate );
  TRACE( "Upgrade Recapture Factor 1 = %f\n",  dUpRecapFctr1 );
  TRACE( "Upgrade Recapture Factor 2 = %f\n",  dUpRecapFctr2 );
#endif

#ifdef _DEBUG_REVMIX
    TRACE( "  Auth Fc Cmpt Level = %d\n", GetAuthFc() );
    TRACE( "  Reservable Capacity = %d\n", GetCapRes() );
#endif

  double dAddDmdFc = 0;
  double dResHoldTot = 0;
  double dResHoldIdv = 0;
  double dStdvFctr = 0;
  int iCER_i = 0;
  double dDenom = 0;
  double dTail = 0;

  // iterate through buckets for this compartment
#ifdef _DEBUG_REVMIX
  TRACE( "Beginning loop for j = %d to %d\n", 1, GetNstLvlCnt() );
#endif
  for( int j = 1; j < GetNstLvlCnt(); j++ )
  {
    COptNstLvl* pOptNstLvl_J = GetOptNstLvl( j );
    COptNstLvl* pOptNstLvl_J_prev = GetOptNstLvl( j-1 );

    int iCER_j = pOptNstLvl_J->GetCerIdv();
#ifdef _DEBUG_REVMIX
    TRACE( "  CER (j=%d) = %d\n", j, iCER_j );
#endif

#ifdef _DEBUG_REVMIX
  TRACE( "  Beginning loop for i = %d to %d\n", 0, j-1 );
#endif
    int iAuth = 0;
    dProt_I = 0;  // reset 
    for( int i = 0; i < j; i++ )
    {
      COptNstLvl* pOptNstLvl_I = GetOptNstLvl( i );

      dAddDmdFc = pOptNstLvl_I->GetAddDmdFcst();
      dResHoldTot = (pOptNstLvl_I->GetResHoldIdv()+pOptNstLvl_I->GetResHoldGrp());
      dResHoldIdv = pOptNstLvl_I->GetResHoldIdv();
      dStdvFctr = pOptNstLvl_I->GetAddDmdStdvFcst() * STDV_FACTOR_RATE;
      iCER_i = pOptNstLvl_I->GetCerIdv();
#ifdef _DEBUG_REVMIX
      TRACE( "    Add Dmd Fc (i=%d) = %f\n", i, dAddDmdFc );
      TRACE( "    ResHold Tot (i=%d) = %f\n", i, dResHoldTot );
      TRACE( "    ResHold Idv (i=%d) = %f\n", i, dResHoldIdv );
      TRACE( "    Stdv Fctr (i=%d) = %f\n", i, dStdvFctr );
      TRACE( "    CER (i=%d) = %d\n", i, iCER_i );
#endif

      if ( (dDenom = iCER_i * dUpRecapFctr2) < ZERO_EPSILON )
      {
        dProt_I += ( dResHoldTot );
//        dProt_I += ( 1 + dResHoldTot );
      }
      else
      {
        dTail = ( iCER_j - ( iCER_i * dUpRecapFctr1 ) ) / dDenom;
        if ( dTail < ZERO_EPSILON )
          dProt_I += ( (max( 0.0, dAddDmdFc )) + dResHoldTot );
//          dProt_I += ( (max( 1.0, dAddDmdFc )) + dResHoldTot );
        else
          if ( dTail >= 1.0 || dAddDmdFc == 0 )
            dProt_I += ( dResHoldTot );
//            dProt_I += ( 1 + dResHoldTot );
          else
            dProt_I += 
              ( ( max( 0.0, dAddDmdFc + dStdvFctr * log( (1.0/dTail) - 1.0 ) ) ) + dResHoldTot );
      }

#ifdef _DEBUG_REVMIX
        TRACE( "    Denominator = %f\n", dDenom );
        TRACE( "    Tail = %f\n", dTail );
        TRACE( "    Prot = %f\n", dProt_I );
#endif
    }  // for i

    if ( (! GetOvbModel()) ) // first index (highest valued non-reservable bucket)
    {
      iAuth = GetCapRes();
    }
    else
    {
      iAuth = max( 0, pOptNstLvl_J_prev->GetAuth() );
      iAuth = (int) min( (double)iAuth, max( 0.0, (double)GetAuthFc() - (dProt_I) ) );
//      iAuth = (int) min( (double)iAuth, max( 0.0, (double)GetAuthFc() - (dProt_I+0.5) ) );
    }

    // update object with new authorization value
    pOptNstLvl_J->SetAuth(iAuth);

#ifdef _DEBUG_REVMIX
    TRACE( "  Auth Fc Bkt Level  (bkt=%d) = %d\n", j, pOptNstLvl_J->GetAuth() );
    TRACE( "  Auth Fc Cmpt Level = %d\n", GetAuthFc() );
    TRACE( "  Newly calculated Auth Fcst (bkt=%d) = %d\n", j, iAuth );
    TRACE( "  ***\n");
#endif

  }  // for bucket loop (j)
}

int COptimization::GetAuth(int iIndex) 
{
  if (iIndex < MAX_NSTLVL && m_apOptNstLvl[iIndex]) 
    return m_apOptNstLvl[iIndex]->m_iAuth;
  else
    return -1;
}
int COptimization::GetAvail(int iIndex) 
{
  if (iIndex < MAX_NSTLVL && m_apOptNstLvl[iIndex]) 
    return m_apOptNstLvl[iIndex]->m_iAvail;
  else
    return -1;
}
