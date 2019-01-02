// DlgSupplement.cpp : implementation file
//
#include "StdAfx.h"


#include "TrancheView.h"
#include "ResaRailView.h"
#include "DlgSupplement.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDlgSupplement dialog
CDlgSupplement::CDlgSupplement(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSupplement::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSupplement)
	m_szSeatAfter = _T("");
	m_szSeatBefore = _T("");
	//}}AFX_DATA_INIT
}


void CDlgSupplement::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSupplement)
	DDX_Control(pDX, IDC_STATIC_DPT_DATE, m_staticDptDate);
	DDX_Control(pDX, IDC_STATIC_TRANCHE, m_staticTranche);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgSupplement, CDialog)
	//{{AFX_MSG_MAP(CDlgSupplement)
	ON_BN_CLICKED(ID_SUPPLEMENT_ADD, OnSupplementAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTrancheView* CDlgSupplement::GetTrancheView()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CView* pView = NULL;
  CView* pTrView = NULL;
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	pView = pDoc->GetNextView( pos ); 
	if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) )
	{
	  pTrView = pView;
      break;
	}
  }
  ASSERT(pTrView != NULL);
  CTrancheView* pTrancheView = (CTrancheView*)pTrView;
  return pTrancheView;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgSupplement message handlers

BOOL CDlgSupplement::OnInitDialog() 
{
	CDialog::OnInitDialog();

  // Get selected tranche from Tranche List
  CTrancheView* pTrancheView = GetTrancheView();
  int iIndex = pTrancheView->GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
  LPARAM lParam = pTrancheView->GetListCtrl().GetItemData(iIndex);
  m_pTrancheSelected = (YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY))->GetRecord(lParam);
  ASSERT(m_pTrancheSelected != NULL);

  CString szTrancheNo;
  szTrancheNo.Format("%d", m_pTrancheSelected->TrancheNo());
  m_staticTranche.SetWindowText(szTrancheNo);

  CString szDptDate = FormatDptDate(m_pTrancheSelected->DptDate());
  m_staticDptDate.SetWindowText(szDptDate);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString CDlgSupplement::FormatDptDate(int iDptDate)
{
  RWDate rwDate;
  rwDate.julian(iDptDate);
  CString szDptDate;
  szDptDate.Format( "%2.2u/%2.2u/%2.2u",  rwDate.dayOfMonth(), rwDate.month(), rwDate.year() % 100 );
  return szDptDate;
}

void CDlgSupplement::OnSupplementAdd() 
{
  // disable the button ... don't let the user immediately add this one again
  ((CButton *)GetDlgItem(ID_SUPPLEMENT_ADD))->EnableWindow( FALSE );

  // get all cmpts for this tranche / date
  YmIcResaCmptDom Record;
  Record.TrancheNo(m_pTrancheSelected->TrancheNo());
  Record.DptDate(m_pTrancheSelected->DptDate());
  Record.RrdIndex(m_pTrancheSelected->LastJX());
  YM_Query* pSQL = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
  pSQL->SetDomain(&Record);
  RWDBReader Reader( ((CCTAApp*)APP)->m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_TRAIN_SUPPLEMENT_TDLPR) );
  while (Reader())
  {
    if (Reader.isValid())
    {
      Reader >> Record;
      InsertRecord(&Record);
    }
  }
  delete pSQL;
/*
  // close, re-open the Supplement RecordSet forcing an SQL to be sent
  YM_RecordSet* pTrainSupplementSet = 
    YM_Set<YmIcTrainSupplementDom>::FromKey(TRAINSUPPLEMENT_KEY);
  ASSERT( pTrainSupplementSet != NULL );
	pTrainSupplementSet->Close();
	pTrainSupplementSet->Open();
*/
}

void CDlgSupplement::InsertRecord(YmIcResaCmptDom* pResaCmptRecord) 
{
  // launch SQL to insert into SC_ADAPTATIONS
  YmIcTrainSupplementDom pSupplementRecord;
  pSupplementRecord.TrancheNo(pResaCmptRecord->TrancheNo());
  pSupplementRecord.DptDate(pResaCmptRecord->DptDate());
  pSupplementRecord.Cmpt(pResaCmptRecord->Cmpt());
  pSupplementRecord.setSeatBeforeIsNull(FALSE);
  pSupplementRecord.SeatBefore(0);
  pSupplementRecord.setSeatAfterIsNull(FALSE);
  pSupplementRecord.SeatAfter(pResaCmptRecord->Capacity());
  pSupplementRecord.setAdaptTypeIsNull(FALSE);
  pSupplementRecord.AdaptType(TRAIN_SUPPLEMENT);

  CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
  YM_Query* pSql = new YM_Query(* (WinApp->m_pDatabase), FALSE);
  pSql->SetDomain(&pSupplementRecord);
  WinApp->m_pDatabase->Transact(pSql, IDS_SQL_INSERT_TRAIN_SUPPLEMENT);
  delete pSql;
}
