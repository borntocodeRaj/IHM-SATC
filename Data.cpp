#include "StdAfx.h"


#include "CTAChartHelpDlg.h"

#include "RrdHistView.h"
#include "BktHistView.h"
#include "RelTrancheView.h"
#include "ComptagesView.h"
#include "VnlEngineDlg.h"
#include "BktSciSphere.h"
#include "SpheresPrix.h"

//
// CRRDHistBktEvolSet	5100
//
YmDtDom* CRRDHistBktEvolSet::OnSelectQuery()
{
  // Initialize query where clause values

  // From the Tranche List
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  // Set the Family Number from the Tranche record
  // Set the Family Flag from the Tranche record
  YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  DptDate( pTranche->DptDate() );
  FamilyNo( pTranche->FamilyNo() );
  FamilyFlag( pTranche->FamilyFlag() );
  RrdIndex( pTranche->LastJX() );
  Season (pTranche->Season());

  Cmpt(((CCTAApp*)APP)->GetDocument()->GetCurRrdList());
    
  // From the Leg List
  // Set the Leg Origin from the Leg record
  YmIcLegsDom* pLeg  = 
    YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegId( pLeg->LegId() );
  LegOrig( pLeg->LegOrig() );
  LegDest( pLeg->LegDest() );

  return this;
}

//
// CRRDHistPrevisionEvolSet	XXXX
//
/*YmDtDom* CRRDHistPrevisionEvolSet::OnSelectQuery()
{
  // Initialize query where clause values

  // From the Tranche List
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  // Set the Family Number from the Tranche record
  // Set the Family Flag from the Tranche record
  YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  DptDate( pTranche->DptDate() );
 
  RrdIndex( pTranche->LastJX() );
 
  Cmpt(((CCTAApp*)APP)->GetDocument()->GetCurRrdList());
     
  return this;
}*/

//
// CRRDHistoEvolSet
//
CRRDHistoEvolSet::CRRDHistoEvolSet(YM_Database* pDatabase, char HoiTra)
		: YM_ArraySet<YmIcHistoEvolDom>(pDatabase,IDS_SQL_SELECT_RRD_DATES_HISTO)
{
  m_hoitra = HoiTra;
}

YmDtDom* CRRDHistoEvolSet::OnSelectQuery()
{
	if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
		return NULL;
	
	// Initialize query where clause values
	char ht[2];
	ht[0] = m_hoitra;  //DM 7093
	ht[1] = 0;
	Cmpt(ht);

	// From the Tranche List
	// Set the Tranche No from the Tranche record
	// Set the Dpt Date from the Tranche record
	// Set the Family Number from the Tranche record
	// Set the Family Flag from the Tranche record
	YmIcTrancheLisDom* pTranche = 
	YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	DptDate( pTranche->DptDate() );
	FamilyNo( pTranche->FamilyNo() );
	FamilyFlag( pTranche->FamilyFlag() );
	RWDate d = pTranche->DptDate();
	int iDOWOffset = d.weekDay();
	iDOWOffset = ((CCTAApp*)APP)->ConvertRWDayOfWeek(iDOWOffset);
	DayOfWeek( iDOWOffset );

	// From the Leg List
	// Set the Leg Origin from the Leg record
	YmIcLegsDom* pLeg  = 
	YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
	LegOrig( pLeg->LegOrig() );

	// Set the Entity Description from the Entity record
	Entity( ((CCTAApp*)APP)->GetDocument()->GetEntities() );
	Commentaire(((CCTAApp*)APP)->GetDocument()->GetCurRrdList());
	// Ano 116770 et modif de la requete montee en charge historique
	RWDate dummy (1,1,1900);
	HistoDate1(dummy.julian());
	HistoDate2(dummy.julian());
	HistoDate3(dummy.julian());
	if (((CCTAApp*)APP)->m_CurHistoDates.DptDate() == pTranche->DptDate())
	{ 
		// NPI - DM7870 : Dates de référence
		/*HistoDate1( ((CCTAApp*)APP)->m_CurHistoDates.DateHisto1());
		HistoDate2( ((CCTAApp*)APP)->m_CurHistoDates.DateHisto2());
		HistoDate3( ((CCTAApp*)APP)->m_CurHistoDates.DateHisto3());*/
		if (((CCTAApp*)APP)->m_CurHistoDates.DateHisto1() != 0)
		{
			HistoDate1( ((CCTAApp*)APP)->m_CurHistoDates.DateHisto1());
		}

		if (((CCTAApp*)APP)->m_CurHistoDates.DateHisto2() != 0)
		{
			HistoDate2( ((CCTAApp*)APP)->m_CurHistoDates.DateHisto2());
		}

		if (((CCTAApp*)APP)->m_CurHistoDates.DateHisto3() != 0)
		{
			HistoDate3( ((CCTAApp*)APP)->m_CurHistoDates.DateHisto3());
		}
		///
	}
	return this;
}

//
// CRRDHistCmptEvolSet
//
YmDtDom* CRRDHistCmptEvolSet::OnSelectQuery()
{
  // Initialize query where clause values

  // From the Tranche List
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  // Set the Family Number from the Tranche record
  // Set the Family Number from the Tranche record
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  DptDate( pTranche->DptDate() );
  FamilyNo( pTranche->FamilyNo() );
  FamilyFlag( pTranche->FamilyFlag() );
  RrdIndex( pTranche->LastJX() );
  Season (pTranche->Season());

  Cmpt(((CCTAApp*)APP)->GetDocument()->GetCurRrdList());

  // From the Leg List
  // Set the Leg Origin from the Leg record
  YmIcLegsDom* pLeg  = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegId( pLeg->LegId() );
  LegOrig( pLeg->LegOrig() );
  LegDest( pLeg->LegDest() );
  
  return this;
}

//
// CQuelCmptSet
//
YmDtDom* CQuelCmptSet::OnSelectQuery()
{
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  FamilyNo(pTranche->FamilyNo());
  FamilyFlag(pTranche->FamilyFlag());
  LegOrig(pTranche->TrancheOrig());
  DptDate(pTranche->DptDate());
  return this;
}

//
// CComptagesSet
//
YmDtDom* CComptagesSet::OnSelectQuery()
{
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
 	FamilyNo(pTranche->FamilyNo());
	FamilyFlag(pTranche->FamilyFlag());
  long lJulian = (! pTranche->DptDateRefIsNull()) ?
      pTranche->DptDateRef() :  // dpt date reference present in the record...use it
      ((CCTAApp*)APP)->GetDocument()->GetReferenceDate(pTranche->DptDate(), -1);  // else, build it
  DptDate(lJulian);

  // find the ComptagesView
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument(); 
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
    CView* pView = pDoc->GetNextView( pos ); 
    if( ((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd())->IsComptagesView(pView) )
    {
      ((CComptagesView*)pView)->SetFamilyNo(this->FamilyNo());
      ((CComptagesView*)pView)->DptDateRef(this->DptDate());
      break;
    }
  }
  return this;
}

//
// CComptagesHistSet
//

YmDtDom* CComptagesHistSet::OnSelectQuery()
{
  if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	return NULL;
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
 	FamilyNo(pTranche->FamilyNo());
	FamilyFlag(pTranche->FamilyFlag());
	DptDate( pTranche->DptDate() );
	Entity( ((CCTAApp*)APP)->GetDocument()->GetEntities() );
	EntityNatureID(((CCTAApp*)APP)->GetDocument()->GetCurrentEntityNatureID()); // NPI - DM7870 : Dates de référence
  return this;
}

//
// CHistEvolSet
//
YmDtDom* CHistDptSet::OnSelectQuery()
{
  // Initialize query where clause values

  // From the Tranche List
  // Set the Family Number from the Tranche record
  // Set the Family Number from the Tranche record
  // Set Day of Week from the Tranche Dpt Date
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  FamilyNo( pTranche->FamilyNo() );
  FamilyFlag( pTranche->FamilyFlag() );
  RWDate d = pTranche->DptDate();
  int iDOWOffset = d.weekDay();
  iDOWOffset = ((CCTAApp*)APP)->ConvertRWDayOfWeek(iDOWOffset);
  DayOfWeek( iDOWOffset );
//  DayOfWeek( d.weekDay() );

  // From the Leg List
  // Set the Leg Origin from the Leg record
  YmIcLegsDom* pLeg  = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegOrig( pLeg->LegOrig() );
  LegId( pLeg->LegId() );

  // Last Auth Fcst Date from the Document
  YmIcDcDateDom* pDCDate = YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY);
  LastAuthFcDate( pDCDate->DcDate() );
//  LastAuthFcDate( ((CCTAApp*)APP)->GetDocument()->GetLastAuthFcDate() );

  return this;
}

//
// CBktHistSet
//
YmDtDom* CBktHistSet::OnSelectQuery()
{
  return OnSelectQueryX(FALSE);
}
YmDtDom* CBktHistAltSet::OnSelectQuery()
{
  return OnSelectQueryX(TRUE);
}
YmDtDom* CBktHistSet::OnSelectQueryX(BOOL bAlt /* = FALSE */)
{
  // Initialize query where clause values

  // We load a completly new Bucket history, so it is time to empty
  // the possible TGV jumeaux bucket history, which are also displayed in
  // the CBktHistView
  ((CCTAApp*)APP)->GetTempBktJumHist(bAlt)->Empty();

  // From the Tranche record set
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  DptDate( pTranche->DptDate() );
  FamilyNo (pTranche->FamilyNo());
  FamilyFlag (pTranche->FamilyFlag());

  // From the Leg record set
  // Set the Leg Origin from the Leg record
  YmIcLegsDom* pLeg  = 
    YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegOrig( pLeg->LegOrig() );
  LegId( pLeg->LegId() );
  
  // Set Day of Week from the Tranche Dpt Date
  RWDate d = pTranche->DptDate();
  int iDOWOffset = d.weekDay();
  iDOWOffset = ((CCTAApp*)APP)->ConvertRWDayOfWeek(iDOWOffset);
  DayOfWeek( iDOWOffset );
//  DayOfWeek( d.weekDay() );

  // Set DC Date from the DCDate record set
  YmIcDcDateDom* pDCDate = YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY);
  Dcdate( pDCDate->DcDate() );

  // Last Auth Fcst Date from the Document (same as DC Date?)
  LastAuthFcDate( pDCDate->DcDate() );
  return this;
}

//
// CBktHistoSet
//
YmDtDom* CBktHistoSet::OnSelectQuery()
{
  // Initialize query where clause values

  if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	return NULL;

  Entity( ((CCTAApp*)APP)->GetDocument()->GetEntities() );

  // From the Tranche record set
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  DptDate( pTranche->DptDate() );
  FamilyNo( pTranche->FamilyNo() );
  FamilyFlag( pTranche->FamilyFlag() );

  // From the Leg record set
  // Set the Leg Origin from the Leg record
  YmIcLegsDom* pLeg  = 
    YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegOrig( pLeg->LegOrig() );
  
  EntityNatureID(((CCTAApp*)APP)->GetDocument()->GetCurrentEntityNatureID()); // NPI - DM7870 : Dates de référence

  return this;
}

//
// CBktPrevisionVenteSet
//
YmDtDom* CBktPrevisionVenteSet::OnSelectQuery()
{
  // Initialize query where clause values

  if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	return NULL;

  // From the Tranche record set
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  setDptDate(pTranche->DptDate());
  setTrancheNo(pTranche->TrancheNo());
  setRRDs(((CCTAApp*)APP)->GetDocument()->GetCurRrdList().GetBuffer());
  
  //EntityNatureID(((CCTAApp*)APP)->GetDocument()->GetCurrentEntityNatureID()); // NPI - DM7870 : Dates de référence

  return this;
}

//
// CBktPrevisionRevenuSet
//
YmDtDom* CBktPrevisionRevenuSet::OnSelectQuery()
{
  // Initialize query where clause values

  if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	return NULL;

  // From the Tranche record set
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  setDptDate(pTranche->DptDate());
  setTrancheNo(pTranche->TrancheNo());
  setRRDs(((CCTAApp*)APP)->GetDocument()->GetCurRrdList().GetBuffer());
  
  //EntityNatureID(((CCTAApp*)APP)->GetDocument()->GetCurrentEntityNatureID()); // NPI - DM7870 : Dates de référence

  return this;
}

//
// CBktHistJumSet
//

CBktHistJumSet::CBktHistJumSet(YM_Database* pDatabase, long jumno,
           unsigned long dptdate, CString legorig)
  : YM_ArraySet<YmIcBktHistDom>(pDatabase,IDS_SQL_SELECT_BKT_JUM,NULL,YM_RecordSet::AUTO_SELECT)
{
  m_jumno = jumno;
  m_legorig = legorig;
  m_dptdate = dptdate;
}


YmDtDom* CBktHistJumSet::OnSelectQuery()
{
  // Initialize query where clause values
  TrancheNo( m_jumno );
  LegOrig( m_legorig );
  DptDate( m_dptdate );

  return this;
}

//
// CPrevAuthSet
//
YmDtDom* CPrevAuthSet::OnSelectQuery()
{
  // Initialize query where clause values

  // Set the Leg Origin from the Leg record
  YmIcLegsDom* pLeg  = 
    YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegOrig( pLeg->LegOrig() );
  LegId( pLeg->LegId() );
  
  // find the BktHistView
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument(); 
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
    CView* pView = pDoc->GetNextView( pos ); 
    if( ((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd())->IsBktHistView(pView, FALSE) )
    { // initialisation de la requete IDS_SQL_PREVAUTH
      CommentText ( ((CBktHistView*)pView)->GetTDforRAD() );
      break;
    }
  }  
  return this;
}

//
// CCUserInfo
//
YmDtDom* CCUserInfo::OnSelectQuery()
{
  // Get the information on the user
  UserId ((const char *)((CCTAApp*)APP)->m_pDatabase->UserName());
  return this;
}

//
// CLegSet
//

YmDtDom* CLegSet::OnSelectQuery()
{
  // Initialize query where clause values

  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  DptDate( pTranche->DptDate() );
  RrdIndex( pTranche->LastJX() );

  return this;
}


YmDtDom* CLegSet::OnUpdateQuery()
{
  YmIcLegsDom* pLeg  = 
    YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);

  // set the update keys T,D,L
  TrancheNo( pLeg->TrancheNo() );
  DptDate( pLeg->DptDate() );
  LegOrig( pLeg->LegOrig() );
  J_X ( pLeg->J_X() );

  EditStatus( pLeg->EditStatus() );
  SentFlag( pLeg->SentFlag() );

  return this;
}

//
// COthLegsSet
//
YmDtDom* COthLegsSet::OnSelectQuery()
{
  // Initialize query where clause values

  // From the Tranche List
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  // Set the RRD Index from the Tranche record
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  DptDate( pTranche->DptDate() );
  RrdIndex( pTranche->LastJX() );

  return this;
}

//
// CRelatedSet
//
YmDtDom* CRelatedSet::OnSelectQuery()
{
  return OnSelectQueryX(FALSE);
}
YmDtDom* CRelatedAltSet::OnSelectQuery()
{
  return OnSelectQueryX(TRUE);
}
YmDtDom* CRelatedSet::OnSelectQueryX(BOOL bAlt /* = FALSE */)
{
  // Initialize query where clause values

  // for TRN clients, we might join with the TGV production database ...
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTRNFlag() )
  {
    // find the Related Tranche View
    CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument(); 
    POSITION pos = pDoc->GetFirstViewPosition();
    while( pos != NULL ) 
    {
      CView* pView = pDoc->GetNextView( pos ); 
//      if( pView->IsKindOf( RUNTIME_CLASS(CRelTrancheView) ) ) 
      if( ((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd())->IsRelTrancheView(pView, bAlt) )
      {
        Join(((CRelTrancheView*)pView)->GetTRNJoinFlag());  // sets the flag on or off to join with TGV prod database
        break;
      }
    }
  }

  // From the Tranche List
  // Set the Dpt Date from the Tranche record
  // Set the RRD Index from the Tranche record
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  DptDate( pTranche->DptDate() );
  RrdIndex( pTranche->LastJX() );

  Rrd(pTranche->LastJX());

  // From the Leg List
  // Set the Leg Origin from the Leg record
  // Set the Leg Origin from the Leg record
  // Set the Departure Time Begin from the Leg record
  // Set the Departure Time End from the Leg record
  YmIcLegsDom* pLeg  = 
    YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  if ( m_pMkt == NULL || m_szMktOrig.IsEmpty() || m_szMktOrig.IsEmpty() )
  {
    LegOrig( pLeg->LegOrig() );
    LegDest( pLeg->LegDest() );
    DptTimeStart( pLeg->DptTime() - 200 /* minus an offset */ );
    DptTimeEnd( pLeg->DptTime() + 200   /* plus an offset */ );
    m_bMktSelect = FALSE;  // query was executed as a result of leg selected (used in RelTrancheView)
  }
  else
  {
    LegOrig( m_szMktOrig );
    LegDest( m_szMktDest );
    DptTimeStart( pLeg->DptTime() - 200 /* minus an offset */ );
    DptTimeEnd( pLeg->DptTime() + 200   /* plus an offset */ );
    m_bMktSelect = TRUE;  // query was executed as a result of market selected (used in RelTrancheView)
  }
  
  return this;
}

//
// CResaBktSet
//

YmDtDom* CResaBktSet::OnSelectQuery()
{
  // Initialize query where clause values

  // From the Tranche List
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  // Set the Reading Day Index from the Tranche record
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  DptDate( pTranche->DptDate() );
  RrdIndex( pTranche->LastJX() );
  
  // From the Leg List
  // Set the Leg Origin from the Leg record
  YmIcLegsDom* pLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegId( pLeg->LegId() );
  LegOrig( pLeg->LegOrig() );
  Cmpt( pLeg->Cmpt() );

  return this;
}

YmDtDom* CResaBktSet::OnUpdateQuery ()
{
  YmIcResaBktDom* pResa  = YM_Set<YmIcResaBktDom>::FromKey(RESABKT_KEY);

  // set the update keys T,D,L,P,R,N
  TrancheNo( pResa->TrancheNo() );
  DptDate( pResa->DptDate() );
  LegOrig( pResa->LegOrig() );
  Cmpt( pResa->Cmpt() );
  RrdIndex( pResa->RrdIndex() );
  NestLevel( pResa->NestLevel() );

  AuthUser( pResa->AuthUser() );

  // From the Leg List
  // Set the Leg Origin from the Leg record
  YmIcLegsDom* pLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegId( pLeg->LegId() );

  return this;
}

//
// CResaCmptSet
//

YmDtDom* CResaCmptSet::OnSelectQuery()
{
  // Initialize query where clause values

  // From the Tranche List
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  // Set the Reading Day Index from the Tranche record
  YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  DptDate( pTranche->DptDate() );
  RrdIndex( pTranche->LastJX() );

  // From the Leg List
  // Set the Tranche No from the Tranche record
  YmIcLegsDom* pLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegId( pLeg->LegId() );
  LegOrig( pLeg->LegOrig() );
  Cmpt( pLeg->Cmpt() );

  // Initialize query values
  return this;
}

YmDtDom* CResaCmptSet::OnUpdateQuery()
{
  YmIcResaCmptDom* pResa  = YM_Set<YmIcResaCmptDom>::FromKey(RESACMPT_KEY);

  // set the update keys T,D,L,P,R,N
  TrancheNo( pResa->TrancheNo() );
  DptDate( pResa->DptDate() );
  Cmpt( pResa->Cmpt() );
  RrdIndex( pResa->RrdIndex() );

  TgvJumeauxPct( pResa->TgvJumeauxPct() );

  // From the Leg List
  // Set the Tranche No from the Tranche record
  YmIcLegsDom* pLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegId( pLeg->LegId() );
  LegOrig( pLeg->LegOrig() );

  return this;
}

//
// CTrancheSet
//

YmDtDom* CTrancheSet::OnSelectQuery()
{
  // Initialize query where clause values

  // Set the Entity Description from the Entity record
  Entity( ((CCTAApp*)APP)->GetDocument()->GetEntities() );

  // Get the Last Authorization Forecast Date from the Document
  LastDate( ((CCTAApp*)APP)->GetDocument()->GetLastAuthFcDate() );
  long lDcDate = YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY)->DcDate();
  int offset = ((CCTAApp*)APP)->GetDocument()->GetDecalBatch(); //lDcDate - LastDate();
  if ((offset > 7) || (offset < 0))
    offset = 0;
  EditStatus (offset); // le champ edit_status sert à passer le nombre de jour de décalage du batch voulue

  DptDate( ((CCTAApp*)APP)->GetDocument()->GetBegDptDate() );
  DptDateBeg( ((CCTAApp*)APP)->GetDocument()->GetBegDptDate() );
  DptDateEnd( ((CCTAApp*)APP)->GetDocument()->GetEndDptDate() );
  DayOfWeek( ((CCTAApp*)APP)->GetDocument()->GetDOWs() );
  // ATTENTION, en l'absence de clefs spécifiques, les clefs utilisées ne doivent pas être des updates utilisateur

  // utilisation du MessageNo pour passer un boolean dans la requête
  if (((CCTAApp*)APP)->GetDocument()->GetAllJMX())
    MessageNo (1);
  else
    MessageNo (0);

  //DM 5480
  if (((CCTAApp*)APP)->GetDocument()->IsTrainAutoOnly())
    TrancheTypology ("O");
  else
    TrancheTypology ("%");

  int nbj = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNbJoursVisuModifTop();
  NewTop (nbj);
  return this;
}

//
// CTDLJKSet
//
YmDtDom* CTDLJKSet::OnSelectQuery()
{
  // Initialize query where clause values

  // Set the Entity Description from the Entity record
  Entity( ((CCTAApp*)APP)->GetDocument()->GetEntities() );

  // Get the Last Authorization Forecast Date from the Document
  LastDate( ((CCTAApp*)APP)->GetDocument()->GetLastAuthFcDate() );
  long lDcDate = YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY)->DcDate();
  int offset = ((CCTAApp*)APP)->GetDocument()->GetDecalBatch(); //lDcDate - LastDate();
  if ((offset > 7) || (offset < 0))
    offset = 0;
  LegSeq (offset); // le champ leg_seq sert à passer le nombre de jour de décalage du batch voulue

  DptDateBeg( ((CCTAApp*)APP)->GetDocument()->GetBegDptDate() );
  DptDateEnd( ((CCTAApp*)APP)->GetDocument()->GetEndDptDate() );
  DayOfWeek( ((CCTAApp*)APP)->GetDocument()->GetDOWs() );
  // ATTENTION, en l'absence de clefs spécifiques, les clefs utilisées ne doivent pas être des updates utilisateur
  // utilisation du tranche_no pour passer un boolean dans la requête
  if (((CCTAApp*)APP)->GetDocument()->GetAllJMX())
    TrancheNo (1);
  else
    TrancheNo (0);
  //DM 5480
  if (((CCTAApp*)APP)->GetDocument()->IsTrainAutoOnly())
    TrancheTypology ("O");
  else
    TrancheTypology ("%");
  YmDtDom::bulkMode = true;

  return this;
}


//
// CTDPJESet
//
CTDPJESet::CTDPJESet(YM_Database* pDatabase, char HoiTra)
		: YM_ArraySet<YmIcTDPJEDom>(pDatabase,IDS_SQL_SELECT_TDPJE,NULL,YM_RecordSet::AUTO_SELECT)
{
  m_hoitra = HoiTra;
}

YmDtDom* CTDPJESet::OnSelectQuery()
{
  // Initialize query where clause values

  // Set the Entity Description from the Entity record
  Entity( ((CCTAApp*)APP)->GetDocument()->GetEntities() );

  char ht[2];
  ht[0] = m_hoitra;  //DM 7093
  ht[1] = 0;
  Cmpt(ht);

  // Get the Last Authorization Forecast Date from the Document
  LastDate( ((CCTAApp*)APP)->GetDocument()->GetLastAuthFcDate() );
  long lDcDate = YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY)->DcDate();
  int offset = ((CCTAApp*)APP)->GetDocument()->GetDecalBatch(); //lDcDate - LastDate();
  if ((offset > 7) || (offset < 0))
    offset = 0;
  Resa (offset); // le champ resa sert à passer le nombre de jour de décalage du batch voulue

  DptDateBeg( ((CCTAApp*)APP)->GetDocument()->GetBegDptDate() );
  DptDateEnd( ((CCTAApp*)APP)->GetDocument()->GetEndDptDate() );
  DayOfWeek( ((CCTAApp*)APP)->GetDocument()->GetDOWs() );
  // ATTENTION, en l'absence de clefs spécifiques, les clefs utilisées ne doivent pas être des updates utilisateur
  // utilisation du tranche_no pour passer un boolean dans la requête
  if (((CCTAApp*)APP)->GetDocument()->GetAllJMX())
    TrancheNo (1);
  else
    TrancheNo (0);
  //DM 5480
  if (((CCTAApp*)APP)->GetDocument()->IsTrainAutoOnly())
    TrancheTypology ("O");
  else
    TrancheTypology ("%");

  return this;
}

YmDtDom* CTrancheSet::OnUpdateQuery()
{
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);

  // set the update keys T,D
  TrancheNo( pTranche->TrancheNo() );
  DptDate( pTranche->DptDate() );
  if (pTranche->TrancheNo() == 0)
  {
    CommentText ("");
    return this;
  }

  EditStatus( pTranche->EditStatus() );
  ReviewFlag( pTranche->ReviewFlag() );
  ForceStatus( pTranche->ForceStatus() );
  AlwaysCritFlag( pTranche->AlwaysCritFlag() );
  TgvJumeauxNo( pTranche->TgvJumeauxNo() );
  // NPI - DM7978 : souplesse tarifaire lot 2
  //SupplementLevel( pTranche->SupplementLevel() );
  if (((CCTAApp*)APP)->GetSouplesseTarifaire())
  {
	NiveauSocial(pTranche->NiveauSocial());
  }
  ///
  SentFlag (pTranche->SentFlag());
  ScxSentFlag (pTranche->ScxSentFlag());
  NotOpenFlag (pTranche->NotOpenFlag() );
  TgvJumPleinFlag (pTranche->TgvJumPleinFlag() );
  CommentText (pTranche->CommentText());
  ODManaged (pTranche->ODManaged() );
  DesserteFlag (pTranche->DesserteFlag());
  LastJX (pTranche->LastJX());
  TopTH (pTranche->TopTH());
  TopRR (pTranche->TopRR());

  YM_Query* pUpdateQuery = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
  pUpdateQuery->SetDomain (pTranche);
  ((CCTAApp*)APP)->m_pDatabase->Transact (pUpdateQuery, IDS_SQL_UPDATE_TD_LIST);
  delete pUpdateQuery;

  // NPI - DM7838 : souplesse tarifaire
  pUpdateQuery = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
  pUpdateQuery->SetDomain(pTranche);
  ((CCTAApp*)APP)->m_pDatabase->Transact (pUpdateQuery, IDS_SQL_UPDATE_TRANCHE_LIST);
  delete pUpdateQuery;
  ///

  return this;
}


YmDtDom* CVirtNestSet::OnSelectQuery()
{
  // Initialize query where clause values

  // From the Tranche List
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  // Set the Reading Day Index from the Tranche record
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  DptDate( pTranche->DptDate() );
  RrdIndex( pTranche->LastJX() );

  // From the Leg List
  // Set the Tranche No from the Tranche record
  YmIcLegsDom* pLeg  = 
    YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegOrig( pLeg->LegOrig() );

  return this;
}

//
// CTrainDateSet
//
YmDtDom* CTrainDateSet::OnSelectQuery()
{
  // From the Tranche List
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  DptDate( pTranche->DptDate() );
  TrancheTypology (pTranche->TrancheTypology());

  return this;
}

//
// CTrainSupplementSet
//
YmDtDom* CTrainSupplementSet::OnSelectQuery()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument(); 

  // Set the Entity Description from the Entity record
  Entity( pDoc->GetEntities() );

  // Get the Last Authorization Forecast Date from the Document
  LastAuthFcDate( pDoc->GetLastAuthFcDate() );
  long lDcDate = YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY)->DcDate();
  int offset = ((CCTAApp*)APP)->GetDocument()->GetDecalBatch(); //lDcDate - LastDate();
  if ((offset > 7) || (offset < 0))
    offset = 0;
  SupFlag (offset); // le champ supflag sert à passer le nombre de jour de décalage du batch voulue

  DptDateBeg( ((CCTAApp*)APP)->GetDocument()->GetBegDptDate() );
  DptDateEnd( ((CCTAApp*)APP)->GetDocument()->GetEndDptDate() );
  // ATTENTION, en l'absence de clefs spécifiques, les clefs utilisées ne doivent pas être des updates utilisateur
    // utilisation du tranche_no pour passer un boolean dans la requête
  if (((CCTAApp*)APP)->GetDocument()->GetAllJMX())
    TrancheNo (1);
  else
    TrancheNo (0);
  // Get the Maximum RRD from the Document
  MaxRrd( pDoc->GetMaxRrdIndex() );
  SupFlag( 1 );  // Supplement ( not Adaptation)

  return this;
}

//
// CTrainAdaptationSet
//
YmDtDom* CTrainAdaptationSet::OnSelectQuery()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument(); 

  // Set the Entity Description from the Entity record
  Entity( pDoc->GetEntities() );

  // Get the Last Authorization Forecast Date from the Document
  LastAuthFcDate( pDoc->GetLastAuthFcDate() );
  long lDcDate = YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY)->DcDate();
  int offset = ((CCTAApp*)APP)->GetDocument()->GetDecalBatch(); //lDcDate - LastDate();
  if ((offset > 7) || (offset < 0))
    offset = 0;
  SupFlag (offset); // le champ supflag sert à passer le nombre de jour de décalage du batch voulue

  DptDateBeg( ((CCTAApp*)APP)->GetDocument()->GetBegDptDate() );
  DptDateEnd( ((CCTAApp*)APP)->GetDocument()->GetEndDptDate() );
  // ATTENTION, en l'absence de clefs spécifiques, les clefs utilisées ne doivent pas être des updates utilisateur

  // utilisation du tranche_no pour passer un boolean dans la requête
  if (((CCTAApp*)APP)->GetDocument()->GetAllJMX())
    TrancheNo (1);
  else
    TrancheNo (0);
  // Get the Maximum RRD from the Document
  MaxRrd( pDoc->GetMaxRrdIndex() );
  SupFlag( 0 );  // Adaptation, not Supplement

  AdaptType(TRAIN_ADAPTATION);

  return this;
}

//
// CTrainDeletedSet
//
YmDtDom* CTrainDeletedSet::OnSelectQuery()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument(); 

  Entity( ((CCTAApp*)APP)->GetDocument()->GetEntities() );

  CString szAdaptTypes;
  szAdaptTypes.Format("'%d', '%d', '%d', '%d'", 
    TRAIN_DELETED_ADAPTED, 
    TRAIN_DELETED, 
    TRAIN_DELETED_UNDETERMINED, 
    TRAIN_DELETED_BASEPLAN);

  AdaptTypeS(szAdaptTypes);

  // Get the Last Authorization Forecast Date from the Document
  LastAuthFcDate( ((CCTAApp*)APP)->GetDocument()->GetLastAuthFcDate() );
  long lDcDate = YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY)->DcDate();
  int offset = ((CCTAApp*)APP)->GetDocument()->GetDecalBatch(); //lDcDate - LastDate();
  if ((offset > 7) || (offset < 0))
    offset = 0;
  SupFlag (offset); // le champ supflag sert à passer le nombre de jour de décalage du batch voulue

  DptDate( ((CCTAApp*)APP)->GetDocument()->GetBegDptDate() );
  DptDateBeg( ((CCTAApp*)APP)->GetDocument()->GetBegDptDate() );
  DptDateEnd( ((CCTAApp*)APP)->GetDocument()->GetEndDptDate() );
  DayOfWeek( ((CCTAApp*)APP)->GetDocument()->GetDOWs() );
  // ATTENTION, en l'absence de clefs spécifiques, les clefs utilisées ne doivent pas être des updates utilisateur
 
  // utilisation du tranche_no pour passer un boolean dans la requête
  if (((CCTAApp*)APP)->GetDocument()->GetAllJMX())
    TrancheNo (1);
  else
    TrancheNo (0);
  LastRrdIndex (((CCTAApp*)APP)->GetDocument()->GetMinRrdIndex());

  return this;
}

//
// CTrainEntitySet
//
YmDtDom* CTrainEntitySet::OnSelectQuery()
{
  // Get user name from the database object
  CommentText ( ((CCTAApp*)APP)->m_EntityList );

  return this;
}

//
// CTrainTrancheSet
//
YmDtDom* CTrainTrancheSet::OnSelectQuery()
{
  // Get user name from the database object
  CommentText ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_TrainList );
  DptDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_bEndDate)
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateFin.julian() );
  else
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );

  return this;
}

//
// CServedLegWEntSet
//
YmDtDom* CServedLegWEntSet::OnSelectQuery()
{
  // Get user name from the database object
  TrancheOrig ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_StationList );
  TrancheDest ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_StationList );
  CommentText ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szEntityList );
  // DM 7790 Ano 51566 : possibilité d'entité des axes et/ou routes
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_szEntityList.IsEmpty())
  {
	  CommentText ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szEntityListSiAxeOuRouteSel );
  }
  DptDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_bEndDate)
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateFin.julian() );
  else
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );

  return this;
}

//
// CServedLegSet
//
YmDtDom* CServedLegSet::OnSelectQuery()
{
  // Get user name from the database object
  TrancheOrig ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_StationList );
  TrancheDest ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_StationList );
  DptDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_bEndDate)
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateFin.julian() );
  else
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );

  return this;
}

//
// CODTrainWEntSet
//
YmDtDom* CODTrainWEntSet::OnSelectQuery()
{
  // Get user name from the database object
  TrancheOrig ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_MktOrig );
  TrancheDest ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_MktDest );
  CommentText ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szEntityList );
  // DM 7790 Ano 51566 : possibilité d'entité des axes et/ou routes
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_szEntityList.IsEmpty())
  {
	  CommentText ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szEntityListSiAxeOuRouteSel );
  }
  DptDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_bEndDate)
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateFin.julian() );
  else
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );

  return this;
}

//
// CODTrainSet
//
YmDtDom* CODTrainSet::OnSelectQuery()
{
  // Get user name from the database object
  TrancheOrig ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_MktOrig );
  TrancheDest ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_MktDest );
  DptDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_bEndDate)
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateFin.julian() );
  else
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );

  return this;
}

//
// CAllTrainWEntSet
//
YmDtDom* CAllTrainWEntSet::OnSelectQuery()
{
  CommentText ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szEntityList );
  DptDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_bEndDate)
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateFin.julian() );
  else
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );

  return this;
}

//
// CAllTrainSet
//
YmDtDom* CAllTrainSet::OnSelectQuery()
{
  DptDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_bEndDate)
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateFin.julian() );
  else
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );

  return this;
}

//
// CServedMarketWEntSet
//
YmDtDom* CServedMarketWEntSet::OnSelectQuery()
{
  // Get user name from the database object
  CommentText ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szEntityList );
  // DM 7790 Ano 51566 : possibilité d'entité des axes et/ou routes
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_szEntityList.IsEmpty())
  {
	  CommentText ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szEntityListSiAxeOuRouteSel );
  }
  TrancheOrig ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_MktOrig );
  TrancheDest ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_MktDest );
  DptDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_bEndDate)
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateFin.julian() );
  else
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );

  return this;
}

//
// CServedMarketSet
//
YmDtDom* CServedMarketSet::OnSelectQuery()
{
  // Get user name from the database object
  TrancheOrig ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_MktOrig );
  TrancheDest ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_MktDest );
  DptDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_bEndDate)
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateFin.julian() );
  else
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );

  return this;
}

//
// CLegTimesSet
//
YmDtDom* CLegTimesSet::OnSelectQuery()
{
  // Get user name from the database object
  LegOrig ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szReq_LegTimes );

  return this;
}

//
//CTgvGagSet
YmDtDom* CTgvGagSet::OnSelectQuery()
{ // J'utilise le m_pVnlEngineData pour transmettre l'info. L'utilisateurne lancera pas simultanement
  // Moteur VNL et VNAU automatique.
  Commentaire ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szEntityList );
  Datedeb (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );
  Datefin (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateFin.julian() );
  return this;
}
//

//
//CDateHistoSet
YmDtDom* CDateHistoSet::OnSelectQuery()
{ 
	Entity( ((CCTAApp*)APP)->GetDocument()->GetEntities() );
	DptDate (((CCTAApp*)APP)->m_queryDate.julian() );
	// NPI - DT12135
	//EntityNatureID(((CCTAApp*)APP)->GetDocument()->GetCurrentEntityNatureID()); // NPI - DM7870 : Dates de référence
	if (((CCTAApp*)APP)->m_queryDateRefDlg == TRUE)
	{
		EntityNatureID(((CCTAApp*)APP)->m_queryEntityNatureID);
		((CCTAApp*)APP)->m_queryDateRefDlg = FALSE;
	}
	else
	{
		EntityNatureID(((CCTAApp*)APP)->GetDocument()->GetCurrentEntityNatureID()); // NPI - DM7870 : Dates de référence
	}
	///
	return this;
}



//
// CVnauListSet
YmDtDom* CVnauListSet::OnSelectQuery()
{
  // Initialize query where clause values
  // From the Tranche List
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  DptDate( pTranche->DptDate() );

  // From the Leg List
  // Set the Tranche No from the Tranche record
  YmIcLegsDom* pLeg  = 
    YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegOrig( pLeg->LegOrig() );
  return this;
}

YmDtDom* CVnlListStubSet::OnSelectQuery()
{
  YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  trancheNumber( pTranche->TrancheNo() );
  dptDate( pTranche->DptDate() );
  return this;
}

YmDtDom* CVnlListStubSet::OnUpdateQuery()
{
  YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  trancheNumber( pTranche->TrancheNo() );
  dptDate( pTranche->DptDate() );
  return this;
}



//
// CPrevisionSet
YmDtDom* CPrevisionSet::OnSelectQuery()
{
  // Initialize query where clause values
  // From the Tranche List
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  DptDate( pTranche->DptDate() );

  // From the Leg List
  // Set the Tranche No from the Tranche record
  YmIcLegsDom* pLeg  = 
    YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegOrig( pLeg->LegOrig() );
  return this;
}

//
// CCalendarTrainSet
//
YmDtDom* CCalendarTDLJKSet::OnSelectQuery()
{
  // Initialize query where clause values
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  return this;
}

//
// CalendarTDLJKSet
//
YmDtDom* CCalendarTrainSet::OnSelectQuery()
{
  // Initialize query where clause values
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  return this;
}

//
// CVnauListSet
YmDtDom* CTraqueTarifSet::OnSelectQuery()
{
  // Initialize query where clause values
  // From the Tranche List
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  YmIcLegsDom* pLeg  = 
    YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);

  // première partie de la clause where, on regarde au départ
  // avec leg_orig, tranche_dest et leg dpt_time
  DptTime( pLeg->DptTime());
  Origine (pLeg->LegOrig());
  Destination (pTranche->TrancheDest());
  if (pLeg->DptTime() < pTranche->DptTime()) // départ le lendemain
    DateCirculation (pTranche->DptDate() + 1);
  else
    DateCirculation( pTranche->DptDate());

  // deuxième partie de la clause where on regarde à l'arrivée
  // avec tranche_orig, leg_dest et leg arv time
  ArvTime (pTranche->DptTime());
  TypeTransporteur (pTranche->TrancheOrig()); // magouille utilisation de TypeTransporteur pour passer l'origine de la tranche
  TypeTrain (pLeg->LegDest()); // magouille utilisation de TypeTransporteur pour passer la destination du leg
  DateRequete (pTranche->DptDate());
  

  return this;
}

//
// CVnauExcluSet
YmDtDom* CVnauExcluSet::OnSelectQuery()
{
  // Initialize query where clause values
  // From the Tranche List
  // Set the Tranche No from the Tranche record
  // Set the Dpt Date from the Tranche record
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  TrancheNo( pTranche->TrancheNo() );
  DptDate( pTranche->DptDate() );

  // From the Leg List
  // Set the Tranche No from the Tranche record
  YmIcLegsDom* pLeg  = 
    YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  LegOrig( pLeg->LegOrig() );
  return this;
}

//
//DM7790
//
// CRouteAxeSet
//
YmDtDom* CRouteAxeSet::OnSelectQuery()
{
  // Get user name from the database object
  CommentText(((CCTAApp*)APP)->m_AxeList);

  return this;
}

//
//DM7790
//
// CEntityRouteSet
//
YmDtDom* CEntityRouteSet::OnSelectQuery()
{
  // Get user name from the database object
  CommentText(((CCTAApp*)APP)->m_RouteList);

  return this;
}

//DM7790
//
// CAllTrainWAxesSet
//
YmDtDom* CAllTrainWAxesSet::OnSelectQuery()
{
  CommentText ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szAxeList );
  DptDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_bEndDate)
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateFin.julian() );
  else
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );

  return this;
}

//DM7790
//
// CAllTrainWRoutesSet
//
YmDtDom* CAllTrainWRoutesSet::OnSelectQuery()
{
  CommentText ( ((CCTAApp*)APP)->m_pVnlEngineData->m_szRouteList );
  DptDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );
  if (((CCTAApp*)APP)->m_pVnlEngineData->m_bEndDate)
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateFin.julian() );
  else
  	LastDate (((CCTAApp*)APP)->m_pVnlEngineData->m_rwDateDeb.julian() );

  return this;
}

//DM7870
//
// CEntityNatureSet
//
YmDtDom* CEntityNatureSet::OnSelectQuery()
{
	YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);

	Entity(pTranche->Entity());
	Nature(pTranche->Nature());

    return this;
}

// DM-7978
// 
// CUserCCSet
//
YmDtDom* CUserCCSet::OnSelectQuery()
{
	YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	setTrancheNo(pTranche->TrancheNo());
	setDptDate(pTranche->DptDate());

	return this;
}

YmDtDom* CUserCCSet::OnUpdateQuery()
{
  YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  setTrancheNo(pTranche->TrancheNo());
  setDptDate(pTranche->DptDate());
	return this;
}



//DM7978 - LME - CCmax CCouv et j-x histo
YmDtDom* CHistoDataRRDSet::OnSelectQuery()
{

  return this;
}
//DM7978 - LME - FIN

//DM7978 - LME - YI-6046
YmDtDom* CAscTarifSet::OnSelectQuery()
{
	YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();//KGH
	long entiteNatureId = pDoc->m_sphereManager->findEntiteNatureId(pTranche->Nature(),pTranche->Entity());//KGH
	setEntiteNatureId(entiteNatureId);//KGH
	setDptDate(pTranche->DptDate());
  return this;
}
//DM7978 - LME - FIN


// DM-7978 - CLE
YmDtDom* CCcMaxCcOuvSet::OnSelectQuery()
{
	YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	setTrancheNo(pTranche->TrancheNo());
	setDptDate(pTranche->DptDate());
	setJx(pTranche->LastJX());

	return this;
}


YmDtDom* CCcMaxCcOuvLegSet::OnSelectQuery()
{
	YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	setTrancheNo(pTranche->TrancheNo());
	setDptDate(pTranche->DptDate());
	setLegOrig(pTranche->TrancheOrig());
	setJx(pTranche->LastJX());

	return this;
}

// DM-7978 - CLE
