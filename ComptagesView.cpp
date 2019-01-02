// ComptagesView.cpp : implementation file
//
#include "StdAfx.h"

#include "CTA_Resource.h"
#include "CTAChartHelpDlg.h"
#include "RRDHistView.h"
#include "ComptagesView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Constants
BOOL* CComptagesView::m_pbSortTimeContext;

/////////////////////////////////////////////////////////////////////////////
// CComptagesAltView

IMPLEMENT_DYNCREATE(CComptagesAltView, CComptagesView)

CComptagesAltView::CComptagesAltView()
:CComptagesView()
{
}

void CComptagesAltView::OnInitialUpdate() 
{
  ((CChildFrame*)GetParentFrame())->SetAltFlag(TRUE);

  CComptagesView::OnInitialUpdate();  // base class method
}

/////////////////////////////////////////////////////////////////////////////
// CComptagesView

IMPLEMENT_DYNCREATE(CComptagesView, CListView)

CComptagesView::CComptagesView()
{
  m_apSortFunctions[FIRST_COLUMN] = SortDummy;
  m_apSortFunctions[FAMILY_NO] = SortFamilyNo;
  m_apSortFunctions[FAMILY_FLAG] = SortFamilyFlag; 
  m_apSortFunctions[DPT_DATE] = SortDptDate;
  m_apSortFunctions[LEG_ORIG] = SortLegOrig;
  m_apSortFunctions[LEG_DEST] = SortLegDest;
  m_apSortFunctions[TRAIN_NO] = SortTrainNo;
  m_apSortFunctions[CMPT] = SortCmpt;
  m_apSortFunctions[TYPE_JOUR] = SortTypeJour;	
  m_apSortFunctions[TYPE_CMPT] = SortTypeCmpt;	
  m_apSortFunctions[NB_OCCUPEES] = SortNbOccupees;
  m_apSortFunctions[OCCUP_PCT] = SortOccupPct;
  m_apSortFunctions[NB_OFFERT] = SortNbOffert;
  m_apSortFunctions[NB_OBS] = SortNbObs; 
  m_apSortFunctions[MOY_OCC_MOIS] = SortMoyOccMois;	 
  m_apSortFunctions[ECT_OCC_MOIS] = SortEctOccMois;
  m_apSortFunctions[MOY_OFF_MOIS] = SortMoyOffMois;
  m_apSortFunctions[ECT_OFF_MOIS] = SortEctOffMois;

  m_iFamilyNo = 0;
  m_iDptDateRef = 0;
  m_bFandnotB = FALSE;
}

CComptagesView::~CComptagesView()
{
  m_apColAttr.RemoveAll();
}

BEGIN_MESSAGE_MAP(CComptagesView, CView)
	//{{AFX_MSG_MAP(CComptagesView)
  ON_WM_CONTEXTMENU()
  ON_WM_SIZE()
  ON_WM_CLOSE()
  ON_WM_PAINT()
  ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
  ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
  ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
  ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComptagesView diagnostics

#ifdef _DEBUG
void CComptagesView::AssertValid() const
{
	CView::AssertValid();
}

void CComptagesView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CComptagesView message handlers

void CComptagesView::OnPaint()
{
  COLORREF clrBk = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();
  GetListCtrl().SetBkColor(clrBk);
  YM_ListView::OnPaint();
}

//////////////////////////////////////////////////////////////
// Methodes virtuelles de YM_ListView

CFont* CComptagesView::GetStdFont(CDC* pDC)
{
  return ((CCTAApp*)APP)->GetDocument()->GetStdFont(pDC);
}

CString CComptagesView::GetViewTitle()
{
  return ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMPTAGES_TITLE);
}

CString CComptagesView::GetColumnTitle(int iResId)
{
  if (iResId > 0)
    return ((CCTAApp*)APP)->GetResource()->LoadResString(iResId);
  else
    return ("");
}

CString CComptagesView::GetDefaultSortOrder()
{
  CString szDefaultOrder;
  szDefaultOrder.Format("%d>,%d>", FAMILY_NO,DPT_DATE);
  return szDefaultOrder;
}

void CComptagesView::InsertColumns() 
{
  int iColIndex = 0;

  InsertColumn( iColIndex++, FAMILY_NO, IDS_EMPTY_STRING, LVCFMT_RIGHT, 0);
//  InsertColumn( iColIndex++, FAMILY_FLAG, IDS_FAMILY_FLAG, LVCFMT_CENTER, 50);
  InsertColumn( iColIndex++, DPT_DATE, IDS_DPT_DATE, LVCFMT_CENTER, 50);
  InsertColumn( iColIndex++, LEG_ORIG, IDS_LEG_ORIG, LVCFMT_CENTER, 50);
  InsertColumn( iColIndex++, LEG_DEST, IDS_LEG_DEST, LVCFMT_CENTER, 50);
  InsertColumn( iColIndex++, TRAIN_NO, IDS_TRAIN_NO, LVCFMT_RIGHT, 50);
  InsertColumn( iColIndex++, CMPT, IDS_CMPT, LVCFMT_CENTER, 50);
//  InsertColumn( iColIndex++, TYPE_JOUR, IDS_TYPE_JOUR, LVCFMT_CENTER, 50);
//  InsertColumn( iColIndex++, TYPE_CMPT, IDS_TYPE_CMPT, LVCFMT_CENTER, 50);
  InsertColumn( iColIndex++, NB_OCCUPEES, IDS_NB_OCCUPEES, LVCFMT_RIGHT, 50);
  InsertColumn( iColIndex++, OCCUP_PCT, IDS_OCCUP_PCT, LVCFMT_RIGHT, 50);
  InsertColumn( iColIndex++, NB_OFFERT, IDS_NB_OFFERT, LVCFMT_RIGHT, 50);
  InsertColumn( iColIndex++, NB_OBS, IDS_NB_OBS, LVCFMT_RIGHT, 50);
  InsertColumn( iColIndex++, MOY_OCC_MOIS, IDS_MOY_OCC_MOIS, LVCFMT_RIGHT, 50);
  InsertColumn( iColIndex++, ECT_OCC_MOIS, IDS_ECT_OCC_MOIS, LVCFMT_RIGHT, 50);
  InsertColumn( iColIndex++, MOY_OFF_MOIS, IDS_MOY_OFF_MOIS, LVCFMT_RIGHT, 50);
  InsertColumn( iColIndex++, ECT_OFF_MOIS, IDS_ECT_OFF_MOIS, LVCFMT_RIGHT, 50);

  m_iColsInserted = iColIndex;
}

int CComptagesView::GetPropertyLevel(LPARAM lParam)
{
  YmIcComptagesDom* pRecord = (YmIcComptagesDom*)lParam;
  if (strcmp (pRecord->TypeCmpt(), "S") == 0)
	return 4;

  // note that critical flag is dependent upon the system parameter 'Critical Threshold'
  return pRecord->HistoRef(); 
}

CString CComptagesView::GetDispInfo(int iSubItem, LPARAM lParam, int iItem)
{
  YmIcComptagesDom* pRecord = (YmIcComptagesDom*)lParam;

  CString szBuffer;
  int colnum = (GetColId( iSubItem ) & ~REVERSE_ORDER_FLAG);
  switch( colnum ) 
	{
    case FAMILY_NO :
  	  szBuffer.Format ("%d",pRecord->FamilyNo());
  	  break;
    case FAMILY_FLAG :
  	  szBuffer.Format ("%s",pRecord->FamilyFlag());
  	  break;
    case DPT_DATE :
      {
	    if (pRecord->HistoRef() && strlen(pRecord->Commentaire()) )
		  szBuffer.Format ("%s",pRecord->Commentaire());
	    else
		{
          RWDate d(pRecord->DptDate());
          szBuffer.Format( "%2.2u/%2.2u/%2.2u",  d.dayOfMonth(), d.month(), d.year() % 100 );
		}
      }
      break;
    case LEG_ORIG :
  	  szBuffer.Format ("%s",pRecord->LegOrig());
  	  break;
    case LEG_DEST :
  	  szBuffer.Format ("%s",pRecord->LegDest());
  	  break;
    case TRAIN_NO :
  	  szBuffer.Format ("%d",pRecord->TrainNo());
  	  break;
    case CMPT :
  	  szBuffer.Format ("%s",pRecord->Cmpt());
	  if ((szBuffer == "B") && m_bFandnotB)
		szBuffer = "F";
  	  break;
    case TYPE_JOUR :
	  if (pRecord->HistoRef())
		szBuffer = "";
  	  else
		szBuffer.Format ("%d",pRecord->TypeJour());
  	  break;
    case TYPE_CMPT :
  	  szBuffer.Format ("%s",pRecord->TypeCmpt());
  	  break;
    case NB_OCCUPEES :
  	  szBuffer.Format ("%d",pRecord->NbOccupees());
  	  break;
	case OCCUP_PCT :
	  if (pRecord->NbOffert() == 0)
		szBuffer.Format ("NA");
	  else
  	    szBuffer.Format ("%d %%",(pRecord->NbOccupees() * 100) / pRecord->NbOffert());
  	  break;
    case NB_OFFERT :
  	  szBuffer.Format ("%d",pRecord->NbOffert());
  	  break;
    case NB_OBS :
	  if (pRecord->HistoRef())
		szBuffer = "";
  	  else
  	    szBuffer.Format ("%d",pRecord->NbObs());
  	  break;
    case  MOY_OCC_MOIS :
  	  if (pRecord->HistoRef())
		szBuffer = "";
  	  else
		szBuffer.Format ("%d",pRecord->MoyOccMois());
  	  break;
    case ECT_OCC_MOIS :
  	  if (pRecord->HistoRef())
		szBuffer = "";
  	  else
		szBuffer.Format ("%d",pRecord->EctOccMois());
  	  break;
    case MOY_OFF_MOIS :
  	  if (pRecord->HistoRef())
		szBuffer = "";
  	  else
		szBuffer.Format ("%d",pRecord->MoyOffMois());
  	  break;
    case ECT_OFF_MOIS :
  	  if (pRecord->HistoRef())
		szBuffer = "";
  	  else
		szBuffer.Format ("%d",pRecord->EctOffMois());
  	  break;
    default:
      szBuffer = "<???>";
      break;
	}
	return szBuffer;
}

void CComptagesView::CreateItemImageList()
{
  // Inutile, car il n'y a pas d'icones dans cette listview
  // mais, il faut quand meme initialiser m_itemImageList
  m_itemImageList.Create( IDB_STATUS_BITMAP, iBitMapSizeConst, 0, RGB( 0, 0, 255 ) );
}

int CComptagesView::SortDummy(LPARAM lParam1, LPARAM lParam2)
{
  return 0;
}
int CComptagesView::SortFamilyNo(LPARAM lParam1, LPARAM lParam2)
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  return ( 0 - NumCmp( pRecord1->FamilyNo(), pRecord2->FamilyNo() ) );
}
int CComptagesView::SortFamilyFlag(LPARAM lParam1, LPARAM lParam2) 
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  return ( strcmp(pRecord1->FamilyFlag(), pRecord2->FamilyFlag()) );
}
int CComptagesView::SortDptDate(LPARAM lParam1, LPARAM lParam2)
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  int result = pRecord1->DptDate() - pRecord2->DptDate();
  if (result == 0)
  {
    if (result == 0)
      result = (pRecord1->FamilyNo() - pRecord2->FamilyNo());
  }
  return result;
}
int CComptagesView::SortLegOrig(LPARAM lParam1, LPARAM lParam2)
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  CString szBuffer = pRecord1->LegOrig();
  return szBuffer.Compare( pRecord2->LegOrig() );
}
int CComptagesView::SortLegDest(LPARAM lParam1, LPARAM lParam2)
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  CString szBuffer = pRecord1->LegDest();
  return szBuffer.Compare( pRecord2->LegDest() );
}
int CComptagesView::SortTrainNo(LPARAM lParam1, LPARAM lParam2)
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  return ( 0 - NumCmp( pRecord1->TrainNo(), pRecord2->TrainNo() ) );
}
int CComptagesView::SortCmpt(LPARAM lParam1, LPARAM lParam2)
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  CString szBuffer = pRecord1->Cmpt();
  return szBuffer.Compare( pRecord2->Cmpt() );
}
int CComptagesView::SortTypeJour(LPARAM lParam1, LPARAM lParam2)	
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  return ( 0 - NumCmp( pRecord1->TypeJour(), pRecord2->TypeJour() ) );
}
int CComptagesView::SortTypeCmpt(LPARAM lParam1, LPARAM lParam2)	
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  CString szBuffer = pRecord1->TypeCmpt();
  return szBuffer.Compare( pRecord2->TypeCmpt() );
}
int CComptagesView::SortNbOccupees(LPARAM lParam1, LPARAM lParam2)
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  return ( 0 - NumCmp( pRecord1->NbOccupees(), pRecord2->NbOccupees() ) );
}
int CComptagesView::SortOccupPct(LPARAM lParam1, LPARAM lParam2)
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  int pct1 = (pRecord1->NbOccupees() * 100) / pRecord1->NbOffert();
  int pct2 = (pRecord2->NbOccupees() * 100) / pRecord2->NbOffert();
  return ( 0 - NumCmp( pct1, pct2 ) );
}
int CComptagesView::SortNbOffert(LPARAM lParam1, LPARAM lParam2)
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  return ( 0 - NumCmp( pRecord1->NbOffert(), pRecord2->NbOffert() ) );
}
int CComptagesView::SortNbObs(LPARAM lParam1, LPARAM lParam2) 
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  return ( 0 - NumCmp( pRecord1->NbObs(), pRecord2->NbObs() ) );
}
int CComptagesView::SortMoyOccMois(LPARAM lParam1, LPARAM lParam2)	 
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  return ( 0 - NumCmp( pRecord1->MoyOccMois(), pRecord2->MoyOccMois() ) );
}
int CComptagesView::SortEctOccMois(LPARAM lParam1, LPARAM lParam2)
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  return ( 0 - NumCmp( pRecord1->EctOccMois(), pRecord2->EctOccMois() ) );
}
int CComptagesView::SortMoyOffMois(LPARAM lParam1, LPARAM lParam2)
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  return ( 0 - NumCmp( pRecord1->MoyOffMois(), pRecord2->MoyOffMois() ) );
}
int CComptagesView::SortEctOffMois(LPARAM lParam1, LPARAM lParam2)
{
  YmIcComptagesDom* pRecord1 = (YmIcComptagesDom*)lParam1;
  YmIcComptagesDom* pRecord2 = (YmIcComptagesDom*)lParam2;
  return ( 0 - NumCmp( pRecord1->EctOffMois(), pRecord2->EctOffMois() ) );
}

CRRDHistView* CComptagesView::GetRRDHistView()
{
  return ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetRRDHistView(FALSE);
}

void CComptagesView::FrameSizeChange (UINT nType, int cx, int cy)
{
  YM_RecordSet* pComptagesSet = 
    YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_KEY);
  ASSERT( pComptagesSet != NULL );

  if ( (nType == SIZE_MINIMIZED) && pComptagesSet->IsOpen() && !GetRRDHistView() )
	pComptagesSet->Close();
  else if ((nType != SIZE_MINIMIZED) && !pComptagesSet->IsOpen())
	pComptagesSet->Open();

  YM_RecordSet* pQuelCmptSet = 
    YM_Set<YmIcComptagesDom>::FromKey(QUELCMPT_KEY);
  ASSERT( pQuelCmptSet != NULL );

  if ( (nType == SIZE_MINIMIZED) && pQuelCmptSet->IsOpen() && !GetRRDHistView() )
	pQuelCmptSet->Close();
  else if ((nType != SIZE_MINIMIZED) && !pQuelCmptSet->IsOpen())
	pQuelCmptSet->Open();

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() )
  {
    YM_RecordSet* pComptagesHistSet = 
      YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_HIST_KEY);
    ASSERT( pComptagesHistSet != NULL );

	if ( (nType == SIZE_MINIMIZED) && pComptagesHistSet->IsOpen() && !GetRRDHistView() )
	  pComptagesHistSet->Close();
	else if ((nType != SIZE_MINIMIZED) && !pComptagesHistSet->IsOpen())
	  pComptagesHistSet->Open();
  }
}

void CComptagesView::OnDestroy() 
{
	CView::OnDestroy();
	
  if ( !GetRRDHistView() )
  {
    YM_RecordSet* pComptagesSet = 
      YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_KEY);
	ASSERT( pComptagesSet != NULL );

	pComptagesSet->RemoveObserver( this );
	pComptagesSet->Close();

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() )
	{
      YM_RecordSet* pComptagesHistSet = 
        YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_HIST_KEY);
      ASSERT( pComptagesHistSet != NULL );

	  pComptagesHistSet->RemoveObserver( this );
	  pComptagesHistSet->Close();
	}
  }

  YM_RecordSet* pQuelCmptSet = 
      YM_Set<YmIcComptagesDom>::FromKey(QUELCMPT_KEY);
	ASSERT( pQuelCmptSet != NULL );

	pQuelCmptSet->RemoveObserver( this );
	pQuelCmptSet->Close();
}

void CComptagesView::OnInitialUpdate() 
{
  m_clrItemBK = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();
  YmIcTrancheLisDom* pTrancheData = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  if (pTrancheData)
  {
	if (pTrancheData->DptDateRefIsNull())
	  m_iDptDateRef = ((CCTAApp*)APP)->GetDocument()->GetReferenceDate(pTrancheData->DptDate(), -1);
	else
      m_iDptDateRef = pTrancheData->DptDateRef();
	m_iFamilyNo = pTrancheData->FamilyNo();
  }


  YM_RecordSet* pComptagesSet = 
    YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_KEY);
	ASSERT( pComptagesSet != NULL );

	pComptagesSet->AddObserver( this );  // Ask for notification from this RecordSet 
  if ( ! pComptagesSet->IsOpen() )
    pComptagesSet->Open();
  OnUpdate( this, 0L, pComptagesSet );

  YM_RecordSet* pQuelCmptSet = 
    YM_Set<YmIcComptagesDom>::FromKey(QUELCMPT_KEY);
	ASSERT( pQuelCmptSet != NULL );

	pQuelCmptSet->AddObserver( this );  // Ask for notification from this RecordSet 
  if ( ! pQuelCmptSet->IsOpen() )
    pQuelCmptSet->Open();
  OnUpdate( this, 0L, pQuelCmptSet );

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() )
  {
    YM_RecordSet* pComptagesHistSet = 
      YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_HIST_KEY);
    ASSERT( pComptagesHistSet != NULL );
	pComptagesHistSet->AddObserver( this );  // Ask for notification from this RecordSet 
    if ( ! pComptagesHistSet->IsOpen() )
      pComptagesHistSet->Open();
    OnUpdate( this, 0L, pComptagesHistSet );
  }

  CString titi, toto;
  titi = 
    ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMPTAGES_TITLE);
  GetParentFrame()->GetWindowText(toto);
  GetParentFrame()->SetWindowText(titi);
  ((YM_PersistentChildWnd*)GetParentFrame())->SetChildKey (toto);
  ((YM_PersistentChildWnd*)GetParentFrame())->SetChildView (this);

	YM_ListView::OnInitialUpdate();  // base class method

  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  SetPropertyLevelColor (1, pDoc->m_pGlobalSysParms->GetGraphRGBValue(CCTAChart::eHisto1ResHoldTotStyleIdx));
  SetPropertyLevelColor (2, pDoc->m_pGlobalSysParms->GetGraphRGBValue(CCTAChart::eHisto2ResHoldTotStyleIdx));
  SetPropertyLevelColor (3, pDoc->m_pGlobalSysParms->GetGraphRGBValue(CCTAChart::eHisto3ResHoldTotStyleIdx));
  SetPropertyLevelColor( 4, RGB(0,0,255) );
}

BOOL CComptagesView::SetPropertyLevelColor(int nLevel, COLORREF clrProperty )
{
	if( (nLevel > 0) && (nLevel <= MAX_PROPERTYLEVEL) )
	{
		m_clrProperty[ nLevel - 1 ] = clrProperty;
		return TRUE;
	}
	return FALSE;
}

void CComptagesView::BuildData()
{
  YM_RecordSet* pComptagesSet = 
    YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_KEY);

  YM_RecordSet* pQuelCmptSet = 
    YM_Set<YmIcComptagesDom>::FromKey(QUELCMPT_KEY);
  
  YM_RecordSet* pComptagesHistSet = NULL;
    if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() )
	pComptagesHistSet = YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_HIST_KEY);
    

    if ( pComptagesSet->IsValid() && pQuelCmptSet->IsValid() &&
	   ((pComptagesHistSet == NULL) || pComptagesHistSet->IsValid()) )
    {
	  YmIcComptagesDom* pRecord = NULL;
	  
	  m_bFandnotB = FALSE;
	  BOOL yaB = FALSE;
	  YM_Iterator<YmIcComptagesDom>* pIterator0 = 
        YM_Set<YmIcComptagesDom>::FromKey(QUELCMPT_KEY)->CreateIterator();
	  for( pIterator0->First(); !pIterator0->Finished(); pIterator0->Next() ) 
      { // La requete est trie par CMPT
        pRecord = pIterator0->Current();
		if (!strcmp (pRecord->Cmpt(), "B"))
		  yaB = TRUE;
		else if (!strcmp (pRecord->Cmpt(), "F") && !yaB)
		  m_bFandnotB = TRUE;
	  }
	  delete pIterator0;

      LV_ITEM Item;
      GetListCtrl().DeleteAllItems();

      YM_Iterator<YmIcComptagesDom>* pIterator = 
        YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_KEY)->CreateIterator();
      int iIndex = 0;
      for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
      {
        pRecord = pIterator->Current();
        Item.mask = LVIF_TEXT | LVIF_PARAM;
        Item.iItem = iIndex++;
        Item.iSubItem = 0;
        Item.pszText = LPSTR_TEXTCALLBACK;
        Item.lParam = (LONG)pRecord; 
        GetListCtrl().InsertItem( &Item );
      }
      delete pIterator;

	  if (pComptagesHistSet)
	  {
		YM_Iterator<YmIcComptagesDom>* pIterator2 = 
        YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_HIST_KEY)->CreateIterator();
		for( pIterator2->First(); !pIterator2->Finished(); pIterator2->Next() ) 
		{
          pRecord = pIterator2->Current();
          Item.mask = LVIF_TEXT | LVIF_PARAM;
          Item.iItem = iIndex++;
          Item.iSubItem = 0;
          Item.pszText = LPSTR_TEXTCALLBACK;
          Item.lParam = (LONG)pRecord; 
          GetListCtrl().InsertItem( &Item );
		}
        delete pIterator2;
	  }

      if( GetListCtrl().GetItemCount() > 0 ) 
      {
        // Set text in status bar control leg list window
        RWDate d(m_iDptDateRef);
        CString szDate;
        szDate.Format( "%2.2u/%2.2u/%2.2u",  d.dayOfMonth(), d.month(), d.year() % 100 );
        CString szBuffer;
        szBuffer.Format("%s: %d  %s: %s - %s",
          ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_FAMILY_NO), m_iFamilyNo,
          ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMPTAGES_DPT_DATE_REF), szDate,
          ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DONE) );
        ((CChildFrame*)GetParentFrame())->GetStatusBar()->SetPaneOneAndLock(szBuffer);
		ReSort();
      }
      else
      {
        RWDate d(m_iDptDateRef);
        CString szDate;
        szDate.Format( "%2.2u/%2.2u/%2.2u",  d.dayOfMonth(), d.month(), d.year() % 100 );
        CString szBuffer;
        szBuffer.Format("%s: %d  %s: %s - %s",
          ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_FAMILY_NO), m_iFamilyNo,
          ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMPTAGES_DPT_DATE_REF), szDate,
          ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA) );
        ((CChildFrame*)GetParentFrame())->GetStatusBar()->SetPaneOneAndLock(szBuffer);
      }
    }
    else    // record set is not valid 
    {
      GetListCtrl().DeleteAllItems();

      // Set text in status bar control leg list window
      ((CChildFrame*)GetParentFrame())->
      GetStatusBar()->SetPaneOneAndLock(
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_WAITING_TEXT) );
    }
}

void CComptagesView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
  // If notification comes from a RecordSet, pHint is a RecordSet*
  YM_RecordSet* pComptagesSet = 
    YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_KEY);
  YM_RecordSet* pQuelCmptSet = 
    YM_Set<YmIcComptagesDom>::FromKey(QUELCMPT_KEY);
  YM_RecordSet* pComptagesHistSet = NULL;
    if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() )
	pComptagesHistSet = YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_HIST_KEY);
  
  if( pHint == pComptagesSet )
  {
	EnableWindow( pComptagesSet->IsValid() );
    if (pComptagesSet->IsValid())
	  BuildData();
  }
  else if ( pHint == pQuelCmptSet )
	BuildData();
  else if ( pHint != NULL && pHint == pComptagesHistSet &&  pComptagesHistSet->IsValid())  // historical comptages data set
   BuildData();

  // BuildData controle que toutes les données requisent sont présentes avant de travailler.

}

CString CComptagesView::GetToolTipText(LVITEM* pItem) const 
{
  CString szText;
  YmIcComptagesDom* pRecord = (YmIcComptagesDom*)pItem->lParam;

  if (pRecord)
  { 
    if (pItem->iSubItem == LEG_ORIG)
      szText = pRecord->LegOrigDesc();
    else if (pItem->iSubItem == LEG_DEST)
      szText = pRecord->LegDestDesc();
  }
  szText.TrimRight();
  return szText;
}
