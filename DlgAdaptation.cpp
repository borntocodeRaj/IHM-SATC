// DlgAdaptation.cpp : implementation file
//
#include "StdAfx.h"


#include "TrancheView.h"
#include "ResaRailView.h"
#include "DlgAdaptation.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAdaptation dialog


CDlgAdaptation::CDlgAdaptation(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAdaptation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAdaptation)
	m_szSeatAfter = _T("");
	m_szSeatBefore = _T("");
	//}}AFX_DATA_INIT
}


void CDlgAdaptation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAdaptation)
	DDX_Control(pDX, IDC_COMBO_DPTDATE, m_comboDptDate);
	DDX_Control(pDX, IDC_COMBO_CMPT, m_comboCmpt);
	DDX_Control(pDX, IDC_COMBO_TRANCHE, m_comboTranche);
	DDX_Text(pDX, IDC_EDIT_SEAT_AFTER, m_szSeatAfter);
	DDX_Text(pDX, IDC_EDIT_SEAT_BEFORE, m_szSeatBefore);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAdaptation, CDialog)
	//{{AFX_MSG_MAP(CDlgAdaptation)
	ON_CBN_SELCHANGE(IDC_COMBO_TRANCHE, OnSelchangeComboTranche)
	ON_CBN_SELCHANGE(IDC_COMBO_DPTDATE, OnSelchangeComboDptDate)
	ON_CBN_SELCHANGE(IDC_COMBO_CMPT, OnSelchangeComboCmpt)
	ON_BN_CLICKED(ID_ADAPTATION_ADD, OnAdaptationAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTrancheView* CDlgAdaptation::GetTrancheView()
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
// CDlgAdaptation message handlers

BOOL CDlgAdaptation::OnInitDialog() 
{
	CDialog::OnInitDialog();

  // Load Tranche Numbers existing in Tranche List
  YmIcTrancheLisDom* pTrancheSelected = BuildTrancheNoList();

  // Load Dpt Dates for this Tranche
  BuildDptDateList(pTrancheSelected);

  // Load compartments for this Tranche/Dpt Date
  BuildCmptList(pTrancheSelected);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAdaptation::InsertTrancheNo(YmIcTrancheLisDom* pTrancheIn)
{
  CString szTrancheNo;
  szTrancheNo.Format("%d", pTrancheIn->TrancheNo());
  if ( ( m_comboTranche.FindString( -1, szTrancheNo ) != CB_ERR) )  // already in list?
    return;
  int iIndex = 0;
  for (iIndex = 0; iIndex < m_comboTranche.GetCount(); iIndex++)
  {
    YmIcTrancheLisDom* pThisTranche = 
      ((YmIcTrancheLisDom*)m_comboTranche.GetItemData( iIndex ));
    if (! pThisTranche)
      return;

    if (pThisTranche->TrancheNo() > pTrancheIn->TrancheNo())
      break;
  }
  m_comboTranche.InsertString(iIndex, szTrancheNo);
  m_comboTranche.SetItemData(iIndex, (DWORD)pTrancheIn);
}

YmIcTrancheLisDom* CDlgAdaptation::BuildTrancheNoList()
{
  BOOL bData = FALSE;
  ((CButton *)GetDlgItem(ID_ADAPTATION_ADD))->EnableWindow( bData );

  // Get selected tranche from Tranche List
  CTrancheView* pTrancheView = GetTrancheView();
  int iIndex = pTrancheView->GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
  LPARAM lParam = pTrancheView->GetListCtrl().GetItemData(iIndex);
  YmIcTrancheLisDom* pTranche = NULL;
  YmIcTrancheLisDom* pTrancheSelected = 
    (YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY))->GetRecord(lParam);

  CString szTrancheHold;
  szTrancheHold.Format("%d", pTrancheSelected->TrancheNo());
  YM_Iterator<YmIcTrancheLisDom>* pTranListIterator =
    YM_Set<YmIcTrancheLisDom>::FromKey (TRANCHE_KEY)->CreateIterator();

  // iterate through the tranche list, and insert all unique train no in the combo box
  if ( pTranListIterator->GetCount() > 0 )
  {
    bData = TRUE;
    for (pTranListIterator->First(); !pTranListIterator->Finished(); pTranListIterator->Next())
    { 
      pTranche = pTranListIterator->Current();
      if (pTranche)
        InsertTrancheNo(pTranche);
    }
    // Select the tranche which is selected in the Tranche List
    iIndex = m_comboTranche.FindString( -1, szTrancheHold );
    m_comboTranche.SetCurSel( iIndex );  
  }
  delete pTranListIterator;
  ((CButton *)GetDlgItem(ID_ADAPTATION_ADD))->EnableWindow( bData );

  return pTrancheSelected;
}

CString CDlgAdaptation::FormatDptDate(int iDptDate)
{
  RWDate rwDate;
  rwDate.julian(iDptDate);
  CString szDptDate;
  szDptDate.Format( "%2.2u/%2.2u/%2.2u",  rwDate.dayOfMonth(), rwDate.month(), rwDate.year() % 100 );
  return szDptDate;
}

void CDlgAdaptation::InsertDptDate(YmIcTrancheLisDom* pTrancheIn)
{
  // format the dpt date from the Tranche record parameter passed in ...
  CString szDptDate = FormatDptDate(pTrancheIn->DptDate());
  if ( ( m_comboDptDate.FindString( -1, szDptDate ) != CB_ERR) )
    return;    // already in list ... get out

  YmIcTrancheLisDom* pThisTranche = NULL;
  int iIndex;
  for (iIndex = 0; iIndex < m_comboDptDate.GetCount(); iIndex++)
  {
    // get the tranche record attached to this combo box item, and compare the date to the tranche record passed in
    pThisTranche = (YmIcTrancheLisDom*)m_comboDptDate.GetItemData( iIndex );
    if (pThisTranche->DptDate() > pTrancheIn->DptDate())
      break;
  }
  m_comboDptDate.InsertString(iIndex, szDptDate);  // insert the formatted date into the combo box
  m_comboDptDate.SetItemData(iIndex, (DWORD)pTrancheIn);  // attach the tranche record passed in to this item in the combo box
}

void CDlgAdaptation::BuildDptDateList(YmIcTrancheLisDom* pTrancheIn)
{
  m_comboDptDate.ResetContent( );  // empty the ComboBox

  BOOL bData = FALSE;
  ((CButton *)GetDlgItem(ID_ADAPTATION_ADD))->EnableWindow( bData );

  if (! pTrancheIn)
    return;

  // iterate through the tranche list, and insert all dpt dates for this tranche in the combo box
  YmIcTrancheLisDom* pTranche = NULL;
  YM_Iterator<YmIcTrancheLisDom>* pTranListIterator =
    YM_Set<YmIcTrancheLisDom>::FromKey (TRANCHE_KEY)->CreateIterator();
  if ( pTranListIterator->GetCount() > 0 )
  {
    bData = TRUE;
    for (pTranListIterator->First(); !pTranListIterator->Finished(); pTranListIterator->Next())
    { 
      pTranche = pTranListIterator->Current();
      if (pTranche->TrancheNo() == pTrancheIn->TrancheNo())
        InsertDptDate(pTranche);
    } 
    CString szDptDate = FormatDptDate(pTrancheIn->DptDate());
    int iIndex = m_comboDptDate.FindString( -1, szDptDate );
    m_comboDptDate.SetCurSel( iIndex );
  }
  delete pTranListIterator;
  ((CButton *)GetDlgItem(ID_ADAPTATION_ADD))->EnableWindow( bData );
}

void CDlgAdaptation::InsertCmpt(CString szCmptIn, int iCapacityIn)
{
  CString szThisCmpt;
  int iIndex;
  for (iIndex = 0; iIndex < m_comboCmpt.GetCount(); iIndex++)
  {
    m_comboCmpt.GetLBText( iIndex, szThisCmpt );
    if (szThisCmpt > szCmptIn)
      break;
  }
  m_comboCmpt.InsertString(iIndex, szCmptIn);
  m_comboCmpt.SetItemData(iIndex, (DWORD)iCapacityIn);
}

void CDlgAdaptation::BuildCmptList(YmIcTrancheLisDom* pTrancheIn)
{
  m_comboCmpt.ResetContent( );  // empty the ComboBox

  BOOL bData = FALSE;
  ((CButton *)GetDlgItem(ID_ADAPTATION_ADD))->EnableWindow( bData );

  if (! pTrancheIn)
    return;

  YmIcResaCmptDom ResaCmpt;
  ResaCmpt.TrancheNo(pTrancheIn->TrancheNo());
  ResaCmpt.DptDate(pTrancheIn->DptDate());
  ResaCmpt.LegOrig(pTrancheIn->TrancheOrig());
  ResaCmpt.RrdIndex(pTrancheIn->LastJX());
  CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
  YM_Query* pSql = new YM_Query(* (WinApp->m_pDatabase), FALSE);
  pSql->SetDomain(&ResaCmpt);
  RWDBReader Reader( ((CCTAApp*)APP)->m_pDatabase->Transact(pSql, IDS_SQL_SELECT_RESA_CMPT) );
  int iCapacity = 0;
  while(Reader())
    if (Reader.isValid())
    {
      Reader >> ResaCmpt;
      InsertCmpt(ResaCmpt.Cmpt(), ResaCmpt.Capacity());
      if (bData == FALSE) // first time through
        iCapacity = ResaCmpt.Capacity();  // hold this one
      bData = TRUE;
    }
  delete pSql;
  ((CButton *)GetDlgItem(ID_ADAPTATION_ADD))->EnableWindow( bData );
  m_comboCmpt.SetCurSel( 0 );

  CString szCapacity;
  szCapacity.Format("%d", iCapacity);
  ((CEdit*)GetDlgItem(IDC_EDIT_SEAT_AFTER))->SetWindowText(szCapacity);

  return;
}

void CDlgAdaptation::OnSelchangeComboTranche() 
{
  // enable the button ...
  ((CButton *)GetDlgItem(ID_ADAPTATION_ADD))->EnableWindow( TRUE );
  ((CEdit*)GetDlgItem(IDC_EDIT_SEAT_BEFORE))->SetWindowText("");

  YmIcTrancheLisDom* pTranche = 
    (YmIcTrancheLisDom*)m_comboTranche.GetItemData( m_comboTranche.GetCurSel() );

  // Load Dpt Dates for this Tranche
  BuildDptDateList(pTranche);

  // Load compartments for this Tranche/Dpt Date
  BuildCmptList(pTranche);
}

void CDlgAdaptation::OnSelchangeComboDptDate() 
{
  // enable the button and clear the "Seat Before" edit box ...
  ((CButton *)GetDlgItem(ID_ADAPTATION_ADD))->EnableWindow( TRUE );
  ((CEdit*)GetDlgItem(IDC_EDIT_SEAT_BEFORE))->SetWindowText("");

  YmIcTrancheLisDom* pTranche = 
    (YmIcTrancheLisDom*)m_comboDptDate.GetItemData( m_comboDptDate.GetCurSel() );

  // Load compartments for this Tranche/Dpt Date
  BuildCmptList(pTranche);
}

void CDlgAdaptation::OnSelchangeComboCmpt() 
{
  int iCapacity = (int)m_comboCmpt.GetItemData( m_comboCmpt.GetCurSel() );
  CString szCapacity;
  szCapacity.Format("%d", iCapacity);
  ((CEdit*)GetDlgItem(IDC_EDIT_SEAT_AFTER))->SetWindowText(szCapacity);
  ((CEdit*)GetDlgItem(IDC_EDIT_SEAT_BEFORE))->SetWindowText("");

  // enable the button ...
  ((CButton *)GetDlgItem(ID_ADAPTATION_ADD))->EnableWindow( TRUE );
}

void CDlgAdaptation::OnAdaptationAdd() 
{
  CEdit* pEditSeatsBefore = ((CEdit*)GetDlgItem(IDC_EDIT_SEAT_BEFORE));
  CEdit* pEditSeatsAfter = ((CEdit*)GetDlgItem(IDC_EDIT_SEAT_AFTER));

  CString szSeatsBefore;
  pEditSeatsBefore->GetWindowText(szSeatsBefore);
  CString szSeatsAfter;
  pEditSeatsAfter->GetWindowText(szSeatsAfter);

  CString szErrMsg = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VALUE_REQUIRED);
  BOOL bError = FALSE;
  // bounding itemRect for an item where the balloon help will originate
  CRect rect;
  if (szSeatsBefore.GetLength() == 0)
  {
    pEditSeatsBefore->GetWindowRect(&rect);
    bError = TRUE;
  }
  else if (szSeatsAfter.GetLength() == 0)
  {
    pEditSeatsAfter->GetWindowRect(&rect);
    bError = TRUE;
  }

  if (bError)
  {
    YM_BalloonHelp::LaunchBalloon(szErrMsg, "", rect.CenterPoint(), IDI_ERROR, 
      unCLOSE_ON_MOUSE_MOVE|unDISABLE_FADEOUT|unCLOSE_ON_KEYPRESS, NULL, "", 3000);   
    return;
  }

  // disable the button ... don't let the user immediately add this one again
  ((CButton *)GetDlgItem(ID_ADAPTATION_ADD))->EnableWindow( FALSE );

  // launch SQL to insert into SC_ADAPTATIONS
  YmIcTrancheLisDom* pTranche = 
    (YmIcTrancheLisDom*)m_comboTranche.GetItemData( m_comboTranche.GetCurSel() );
  YmIcTrancheLisDom* pTrancheDptDate = 
    (YmIcTrancheLisDom*)m_comboDptDate.GetItemData( m_comboDptDate.GetCurSel() );
//  int iCapacity = (int)m_comboCmpt.GetItemData( m_comboCmpt.GetCurSel() );
  CString szCmpt;
  m_comboCmpt.GetLBText( m_comboCmpt.GetCurSel(), szCmpt );
  int iSeatsBefore = atoi(szSeatsBefore);
  int iSeatsAfter = atoi(szSeatsAfter);

  YmIcTrainSupplementDom pRecord;
  pRecord.TrancheNo(pTranche->TrancheNo());
  pRecord.DptDate(pTrancheDptDate->DptDate());
  pRecord.Cmpt(szCmpt);
  pRecord.setSeatBeforeIsNull(FALSE);
  pRecord.SeatBefore(iSeatsBefore);
  pRecord.setSeatAfterIsNull(FALSE);
  pRecord.SeatAfter(iSeatsAfter);
  pRecord.setAdaptTypeIsNull(FALSE);
  pRecord.AdaptType(TRAIN_ADAPTATION);

  CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
  YM_Query* pSql = new YM_Query(* (WinApp->m_pDatabase), FALSE);
  pSql->SetDomain(&pRecord);
  WinApp->m_pDatabase->Transact(pSql, IDS_SQL_INSERT_TRAIN_SUPPLEMENT);
  delete pSql;

  // close, re-open the Adaptation RecordSet forcing an SQL to be sent
  YM_RecordSet* pTrainSupplementSet = 
    YM_Set<YmIcTrainSupplementDom>::FromKey(TRAINADAPTATION_KEY);
  ASSERT( pTrainSupplementSet != NULL );
	pTrainSupplementSet->Close();
	pTrainSupplementSet->Open();
}
