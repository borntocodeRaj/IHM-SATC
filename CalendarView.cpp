// HlpView.cpp : implementation file
//

#include "StdAfx.h"









#include "CalendarView.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalendarView

IMPLEMENT_DYNCREATE(CCalendarView, CFormView)


BEGIN_MESSAGE_MAP(CCalendarView, CFormView)
	//{{AFX_MSG_MAP(CCalendarView)
	ON_BN_CLICKED(IDC_CHECK_TOP1, OnCheckTop1)
	ON_BN_CLICKED(IDC_CHECK_TOP2, OnCheckTop2)
	ON_BN_CLICKED(IDC_CHECK_COMMENT1, OnCheckComment1) 
	ON_BN_CLICKED(IDC_CHECK_COMMENT2, OnCheckComment2)
	ON_BN_CLICKED(IDC_CHECK_VNAU1, OnCheckVnau1)
	ON_BN_CLICKED(IDC_CHECK_VNAU2, OnCheckVnau2)
	ON_BN_CLICKED(IDC_CHECK_CRITIC1, OnCheckCritic1)
	ON_BN_CLICKED(IDC_CHECK_CRITIC2, OnCheckCritic2)
	ON_BN_CLICKED(IDC_CHECK_TGVJUM1, OnCheckTgvJum1)
	ON_BN_CLICKED(IDC_CHECK_TGVJUM2, OnCheckTgvJum2)
	ON_BN_CLICKED(IDC_CHECK_AFFAIRE1, OnCheckAffaire1)
	ON_BN_CLICKED(IDC_CHECK_AFFAIRE2, OnCheckAffaire2)
	ON_BN_CLICKED(IDC_CHECK_DESSERTE1, OnCheckDesserte1)
	ON_BN_CLICKED(IDC_CHECK_DESSERTE2, OnCheckDesserte2)
	ON_BN_CLICKED(IDC_CHECK_GESTIONOD1, OnCheckGestionOD1)
	ON_BN_CLICKED(IDC_CHECK_GESTIONOD2, OnCheckGestionOD2)
	ON_BN_CLICKED(IDC_CHECK_REVISION1, OnCheckRevision1)
	ON_BN_CLICKED(IDC_CHECK_REVISION2, OnCheckRevision2)
	ON_BN_CLICKED(IDC_CHECK_NIVEAU1, OnCheckNiveau1)
	ON_BN_CLICKED(IDC_CHECK_NIVEAU2, OnCheckNiveau2)
	ON_BN_CLICKED(IDC_CHECK_EQUIP1, OnCheckEquip1)
	ON_BN_CLICKED(IDC_CHECK_EQUIP2, OnCheckEquip2)
	ON_BN_CLICKED(IDC_CHECK_FORCE1, OnCheckForce1)
	ON_BN_CLICKED(IDC_CHECK_FORCE2, OnCheckForce2)
	ON_BN_CLICKED(IDC_CHECK_SUPP1, OnCheckSupp1)
	ON_BN_CLICKED(IDC_CHECK_SUPP2, OnCheckSupp2)
	ON_BN_CLICKED(IDC_CHECK_SERVICE1, OnCheckService1)
	ON_BN_CLICKED(IDC_CHECK_SERVICE2, OnCheckService2)
	ON_BN_CLICKED(IDC_CHECK_NOMOS1, OnCheckNomos1)
	ON_BN_CLICKED(IDC_CHECK_NOMOS2, OnCheckNomos2)
	ON_BN_CLICKED(IDC_CHECK_TAUX_1ERE, OnCheckTaux1ere)
	ON_BN_CLICKED(IDC_CHECK_TAUX_2EME, OnCheckTaux2eme)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

	
CCalendarView::CCalendarView()
	: CFormView(CCalendarView::IDD)
{
}

CCalendarView::~CCalendarView()
{
  if (m_pCalendar)
  {
	delete m_pCalendar;
	m_pCalendar = NULL;
  }
}

void CCalendarView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalendarView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


void CCalendarView::OnInitialUpdate() 
{
  CFormView::OnInitialUpdate();

  CSize sizeTotal;
  // TODO: calculate the total size of this view
  sizeTotal.cx = 580;
  sizeTotal.cy = 480;
  SetScrollSizes(MM_TEXT, sizeTotal);
	
  // TODO: Add your specialized code here and/or call the base class
  CString szWndKey;
  GetParentFrame()->GetWindowText(szWndKey);
  ((YM_PersistentChildWnd*)GetParentFrame())->SetChildKey (szWndKey);
  ((YM_PersistentChildWnd*)GetParentFrame())->SetChildView (this);
 
  RWDate today;
  int m1, m2, y1, y2;
  m1 = m2 = today.month();
  y1 = today.year()-1;
  y2 = y1 +2;
  if (m1 == 1)
  {
    m1 = 12;
	y1 -= 1;
  }
  else
    m1 -=1;

  m_pCalendar = new CCalendarWnd(m1, y1, m2, y2);
  m_pCalendar->CreateAndReplace (this, IDC_CALENDAR);
  
  YM_RecordSet* pCalendarTrainSet = 
    YM_Set<YmIcTrancheLisDom>::FromKey( CALENDAR_TRAIN_KEY);
  pCalendarTrainSet->AddObserver( this );
  YM_RecordSet* pCalendarPastTrainSet = 
    YM_Set<YmIcTrancheLisDom>::FromKey( CALENDAR_PAST_TRAIN_KEY);
  pCalendarPastTrainSet->AddObserver( this );

  YM_RecordSet* pCalendarTDLJKSet = 
    YM_Set<YmIcTDLJKDom>::FromKey( CALENDAR_TDLJK_KEY);
  pCalendarTDLJKSet->AddObserver( this );

  RestoreViewConfig();
  m_pCalendar->PositionOnToday();

  CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
  CString szClient = WinApp->m_pDatabase->GetClientParameter();
  if (szClient == THALYS)
  {
	((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->EnableWindow(false);
    ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->EnableWindow(false);
	((CButton*)GetDlgItem (IDC_CHECK_AFFAIRE1))->EnableWindow(false);
    ((CButton*)GetDlgItem (IDC_CHECK_AFFAIRE2))->EnableWindow(false);
	((CButton*)GetDlgItem (IDC_CHECK_TOP1))->EnableWindow(false);
    ((CButton*)GetDlgItem (IDC_CHECK_TOP2))->EnableWindow(false);
	((CButton*)GetDlgItem (IDC_CHECK_SERVICE1))->EnableWindow(false);
    ((CButton*)GetDlgItem (IDC_CHECK_SERVICE2))->EnableWindow(false);
	((CButton*)GetDlgItem (IDC_CHECK_DESSERTE1))->EnableWindow(false);
    ((CButton*)GetDlgItem (IDC_CHECK_DESSERTE2))->EnableWindow(false);
	((CButton*)GetDlgItem (IDC_CHECK_GESTIONOD1))->EnableWindow(false);
    ((CButton*)GetDlgItem (IDC_CHECK_GESTIONOD2))->EnableWindow(false);

	((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->ShowWindow(SW_HIDE);
    ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->ShowWindow(SW_HIDE);
	((CButton*)GetDlgItem (IDC_CHECK_AFFAIRE1))->ShowWindow(SW_HIDE);
    ((CButton*)GetDlgItem (IDC_CHECK_AFFAIRE2))->ShowWindow(SW_HIDE);
	((CButton*)GetDlgItem (IDC_CHECK_TOP1))->ShowWindow(SW_HIDE);
    ((CButton*)GetDlgItem (IDC_CHECK_TOP2))->ShowWindow(SW_HIDE);
	((CButton*)GetDlgItem (IDC_CHECK_SERVICE1))->ShowWindow(SW_HIDE);
    ((CButton*)GetDlgItem (IDC_CHECK_SERVICE2))->ShowWindow(SW_HIDE);
	((CButton*)GetDlgItem (IDC_CHECK_DESSERTE1))->ShowWindow(SW_HIDE);
    ((CButton*)GetDlgItem (IDC_CHECK_DESSERTE2))->ShowWindow(SW_HIDE);
	((CButton*)GetDlgItem (IDC_CHECK_GESTIONOD1))->ShowWindow(SW_HIDE);
    ((CButton*)GetDlgItem (IDC_CHECK_GESTIONOD2))->ShowWindow(SW_HIDE);
  }
  else
  {
    ((CButton*)GetDlgItem (IDC_CHECK_NOMOS1))->EnableWindow(false);
    ((CButton*)GetDlgItem (IDC_CHECK_NOMOS2))->EnableWindow(false);
	((CButton*)GetDlgItem (IDC_CHECK_NOMOS1))->ShowWindow(SW_HIDE);
    ((CButton*)GetDlgItem (IDC_CHECK_NOMOS2))->ShowWindow(SW_HIDE);
  }
}

void CCalendarView::FrameSizeChange(UINT nType, int cx, int cy)
{
  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  if (!pTranche || !pTranche->TrancheNo())
    return;
  // Si appel lors de l'ouverture de la fenêtre le trancheNo est à 0.

  YM_RecordSet* pCalendarTrainSet = 
    YM_Set<YmIcTrancheLisDom>::FromKey( CALENDAR_TRAIN_KEY);
  ASSERT(pCalendarTrainSet != NULL);

  if ((nType == SIZE_MINIMIZED) && pCalendarTrainSet->IsOpen())
    pCalendarTrainSet->Close();
  else if ((nType != SIZE_MINIMIZED) && !pCalendarTrainSet->IsOpen())
    pCalendarTrainSet->Open();


  YM_RecordSet* pCalendarPastTrainSet = 
    YM_Set<YmIcTrancheLisDom>::FromKey( CALENDAR_PAST_TRAIN_KEY);
  ASSERT(pCalendarPastTrainSet != NULL);

  if ((nType == SIZE_MINIMIZED) && pCalendarPastTrainSet->IsOpen())
    pCalendarPastTrainSet->Close();
  else if ((nType != SIZE_MINIMIZED) && !pCalendarPastTrainSet->IsOpen())
    pCalendarPastTrainSet->Open();
}

void CCalendarView::OnDestroy() 
{
  WriteViewConfig();
  CFormView::OnDestroy();
  
  // TODO: Add your message handler code here
  YM_RecordSet* pCalendarTrainSet = 
    YM_Set<YmIcTrancheLisDom>::FromKey( CALENDAR_TRAIN_KEY);
  YM_RecordSet* pCalendarPastTrainSet = 
    YM_Set<YmIcTrancheLisDom>::FromKey( CALENDAR_PAST_TRAIN_KEY);
  YM_RecordSet* pCalendarTDLJKSet = 
    YM_Set<YmIcTDLJKDom>::FromKey( CALENDAR_TDLJK_KEY);

  ASSERT( pCalendarTrainSet != NULL );
  ASSERT( pCalendarPastTrainSet != NULL );
  ASSERT( pCalendarTDLJKSet != NULL );
 
  pCalendarTrainSet->RemoveObserver(this);
  pCalendarTrainSet->Close();
  pCalendarPastTrainSet->RemoveObserver(this);
  pCalendarPastTrainSet->Close();
  pCalendarTDLJKSet->RemoveObserver(this);
  pCalendarTDLJKSet->Close();
}

void CCalendarView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
  YM_RecordSet* pCalendarTrainSet = YM_Set<YmIcTrancheLisDom>::FromKey( CALENDAR_TRAIN_KEY);
  YM_RecordSet* pCalendarPastTrainSet = YM_Set<YmIcTrancheLisDom>::FromKey( CALENDAR_PAST_TRAIN_KEY);

  if (pHint == pCalendarTrainSet)
  {
    if (pCalendarTrainSet->IsValid())
    {
      SetStatusBarText(IDS_DONE);
	  m_pCalendar->EraseSel();
	  m_pCalendar->Invalidate();
	  YmIcTrancheLisDom* pTranche = 
      YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	  CString titi;
	  if (pTranche->Nature())
		  titi.Format ("TRAIN: %s, Tranche: %d, Nature: %d  Peak: %s", pTranche->TrainNo(), pTranche->TrancheNo(),
		               pTranche->Nature(), pTranche->PeakLeg());
	  else
		  titi.Format ("TRAIN: %s, Tranche: %d Peak: %s", pTranche->TrainNo(), pTranche->TrancheNo(),
		             pTranche->PeakLeg());
	  GetParentFrame()->SetWindowText (titi);

	  // on vient de changer de tranche, les autres records sont forcement invalides
	  YM_RecordSet* pCalendarTDLJKSet = 
        YM_Set<YmIcTDLJKDom>::FromKey( CALENDAR_TDLJK_KEY);
	  pCalendarTDLJKSet->Close();

	  if ((GetCheck1() == CCalendarWnd::VNAU) || (GetCheck2() == CCalendarWnd::VNAU) ||
		  (GetCheck1() == CCalendarWnd::CRITIC) || (GetCheck2() == CCalendarWnd::CRITIC))
	  {
	    ((CButton*)GetDlgItem (IDC_CHECK_VNAU1))->EnableWindow(false);
        ((CButton*)GetDlgItem (IDC_CHECK_VNAU2))->EnableWindow(false);
	    pCalendarTDLJKSet->Open();
	  }

	  if ((GetCheck1() == CCalendarWnd::TAUX_PREMIERE) || (GetCheck1() == CCalendarWnd::TAUX_SECONDE))
	    LaunchReadTrancheResa();
    }
    else
	{
      SetStatusBarText(IDS_READ_DATA);
	  GetParentFrame()->SetWindowText (((CCTAApp*)APP)->GetResource()->LoadResString(IDS_READ_DATA));
	  m_pCalendar->Invalidate();
	  m_pCalendar->SetMinDateTranche(0);
      m_pCalendar->SetMaxDateTranche(0);
	  m_pCalendar->SetMinDateOp(0);
	}
  }

  if (pCalendarTrainSet->IsValid() && pCalendarPastTrainSet->IsValid())
  {
      unsigned long mindate = 0;
	  unsigned long mindateop = 0;
	  unsigned long maxdate = 0;
	  YM_Iterator<YmIcTrancheLisDom>* pIteratorPast = YM_Set<YmIcTrancheLisDom>::FromKey( CALENDAR_PAST_TRAIN_KEY)->CreateIterator();
      YmIcTrancheLisDom* pTrancheRecord = NULL;
      for( pIteratorPast->First(); !pIteratorPast->Finished(); pIteratorPast->Next() ) 
      { // le recordset est ordonné par date croissante.
        pTrancheRecord = (YmIcTrancheLisDom*)pIteratorPast->Current();
	    if (mindate == 0)
		  mindate = pTrancheRecord->DptDate();
	    maxdate = pTrancheRecord->DptDate();
      }
      delete pIteratorPast;

      YM_Iterator<YmIcTrancheLisDom>* pIterator = YM_Set<YmIcTrancheLisDom>::FromKey( CALENDAR_TRAIN_KEY)->CreateIterator();
      for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
      { // le recordset est ordonné par date croissante.
        pTrancheRecord = (YmIcTrancheLisDom*)pIterator->Current();
	    if (mindate == 0)
		  mindate = pTrancheRecord->DptDate();
		if (mindateop == 0)
		  mindateop = pTrancheRecord->DptDate();
	    maxdate = pTrancheRecord->DptDate();
      }
      delete pIterator;
	  m_pCalendar->SetMinDateTranche(mindate);
      m_pCalendar->SetMaxDateTranche(maxdate);
	  m_pCalendar->SetMinDateOp(mindateop);
  }
  

  CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
  CString szClient = WinApp->m_pDatabase->GetClientParameter();
  if (szClient == THALYS)
    return;  // Les 2 recordset suivants n'ont pas d'intérêt pour Thalys

  YM_RecordSet* pCalendarTDLJKSet = 
    YM_Set<YmIcTDLJKDom>::FromKey( CALENDAR_TDLJK_KEY);
  if (pHint == pCalendarTDLJKSet)
  {
    if (pCalendarTDLJKSet->IsValid())
	{
	  ((CButton*)GetDlgItem (IDC_CHECK_VNAU1))->EnableWindow(true);
      ((CButton*)GetDlgItem (IDC_CHECK_VNAU2))->EnableWindow(true);
	  if (((CButton*)GetDlgItem (IDC_CHECK_VNAU1))->GetCheck() ||
		  ((CButton*)GetDlgItem (IDC_CHECK_VNAU2))->GetCheck() ||
		  ((CButton*)GetDlgItem (IDC_CHECK_CRITIC1))->GetCheck() ||
		  ((CButton*)GetDlgItem (IDC_CHECK_CRITIC2))->GetCheck())
		m_pCalendar->Invalidate();
	}
  }
}



void CCalendarView::WriteViewConfig()
{
  CString szProfileKey;
  ((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

  APP->WriteProfileString(szProfileKey, "CHOIX1", GetStrChoix (GetCheck1()));
  APP->WriteProfileString(szProfileKey, "CHOIX2", GetStrChoix (GetCheck2()));

}

void CCalendarView::RestoreViewConfig()
{
  CString szProfileKey;
  ((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

  CString sChoix1 = APP->GetProfileString(szProfileKey, "CHOIX1", "UNDEFINED");
  CString sChoix2 = APP->GetProfileString(szProfileKey, "CHOIX2", "UNDEFINED");

  CCalendarWnd::choixAffichage ch1, ch2;
  ch1 = GetChoix (sChoix1);
  ch2 = GetChoix (sChoix2);
  SetCheck1 (ch1);
  SetCheck2 (ch2);
  m_pCalendar->ChangeAffichage(ch1, ch2, false);
}

void CCalendarView::SetStatusBarText( UINT nId )
{
  ((CChildFrame*)GetParentFrame())->
    GetStatusBar()->SetPaneOneAndLock(
      ((CCTAApp*)APP)->GetResource()->LoadResString(nId) );
}

void CCalendarView::SetStatusBarText( CString mess )
{
  ((CChildFrame*)GetParentFrame())->
    GetStatusBar()->SetPaneOneAndLock(mess);
}


/////////////////////////////////////////////////////////////////////////////
// CCalendarView diagnostics

#ifdef _DEBUG
void CCalendarView::AssertValid() const
{
	CFormView::AssertValid();
}

void CCalendarView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCalendarView message handlers

void CCalendarView::OnCheckTop1() 
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_TOP2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TOP1))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckTop2() 
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_TOP1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TOP2))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckComment1() 
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_COMMENT2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_COMMENT1))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckComment2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_COMMENT1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_COMMENT2))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckVnau1()
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_VNAU2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_VNAU1))->SetCheck(1);
  WriteViewConfig();
  YM_RecordSet* pCalendarTDLJKSet = 
    YM_Set<YmIcTDLJKDom>::FromKey( CALENDAR_TDLJK_KEY);
  if (!pCalendarTDLJKSet->IsOpen())
  {
    pCalendarTDLJKSet->Close();
    pCalendarTDLJKSet->Open();
  }
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckVnau2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_VNAU1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_VNAU2))->SetCheck(1);
  WriteViewConfig();
  YM_RecordSet* pCalendarTDLJKSet = 
    YM_Set<YmIcTDLJKDom>::FromKey( CALENDAR_TDLJK_KEY);
  if (!pCalendarTDLJKSet->IsOpen())
  {
    pCalendarTDLJKSet->Close();
    pCalendarTDLJKSet->Open();
  }
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckCritic1()
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_CRITIC2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_CRITIC1))->SetCheck(1);
  YM_RecordSet* pCalendarTDLJKSet = 
    YM_Set<YmIcTDLJKDom>::FromKey( CALENDAR_TDLJK_KEY);
  if (!pCalendarTDLJKSet->IsOpen())
  {
    pCalendarTDLJKSet->Close();
    pCalendarTDLJKSet->Open();
  }
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckCritic2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_CRITIC1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_CRITIC2))->SetCheck(1);
  YM_RecordSet* pCalendarTDLJKSet = 
    YM_Set<YmIcTDLJKDom>::FromKey( CALENDAR_TDLJK_KEY);
  if (!pCalendarTDLJKSet->IsOpen())
  {
    pCalendarTDLJKSet->Close();
    pCalendarTDLJKSet->Open();
  }
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckTgvJum1()
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_TGVJUM2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TGVJUM1))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckTgvJum2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_TGVJUM1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TGVJUM2))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckAffaire1()
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_AFFAIRE2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_AFFAIRE1))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckAffaire2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_AFFAIRE1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_AFFAIRE2))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckDesserte1()
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_DESSERTE2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_DESSERTE1))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckDesserte2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_DESSERTE1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_DESSERTE2))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckGestionOD1()
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_GESTIONOD2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_GESTIONOD1))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckGestionOD2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_GESTIONOD1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_GESTIONOD2))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckRevision1()
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_REVISION2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_REVISION1))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckRevision2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_REVISION1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_REVISION2))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckNiveau1()
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_NIVEAU2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_NIVEAU1))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckNiveau2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_NIVEAU1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_NIVEAU2))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckEquip1()
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_EQUIP2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_EQUIP1))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckEquip2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_EQUIP1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_EQUIP2))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckForce1()
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_FORCE2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_FORCE1))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckForce2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_FORCE1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_FORCE2))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckSupp1()
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_SUPP2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_SUPP1))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckSupp2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_SUPP1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_SUPP2))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckService1()
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_SERVICE2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_SERVICE1))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckService2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_SERVICE1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_SERVICE2))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckNomos1()
{
  UnCheckColumn1();
  ((CButton*)GetDlgItem (IDC_CHECK_NOMOS2))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_NOMOS1))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckNomos2()
{
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);

  ((CButton*)GetDlgItem (IDC_CHECK_NOMOS1))->SetCheck(0);
  ((CButton*)GetDlgItem (IDC_CHECK_NOMOS2))->SetCheck(1);
  WriteViewConfig();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckTaux1ere()
{
  UnCheckColumn1();
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(1);
  WriteViewConfig();
  LaunchReadTrancheResa();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::OnCheckTaux2eme()
{
  UnCheckColumn1();
  UnCheckColumn2();
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(1);
  WriteViewConfig();
  LaunchReadTrancheResa();
  m_pCalendar->ChangeAffichage(GetCheck1(), GetCheck2());
}

void CCalendarView::UnCheckColumn1()
{
	((CButton*)GetDlgItem (IDC_CHECK_TOP1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_COMMENT1))->SetCheck(0); 
	((CButton*)GetDlgItem (IDC_CHECK_VNAU1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_CRITIC1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_TGVJUM1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_AFFAIRE1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_DESSERTE1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_GESTIONOD1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_REVISION1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_NIVEAU1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_EQUIP1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_FORCE1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_SUPP1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_SERVICE1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_NOMOS1))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->SetCheck(0);
}

void CCalendarView::UnCheckColumn2()
{
	((CButton*)GetDlgItem (IDC_CHECK_TOP2))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_COMMENT2))->SetCheck(0); 
	((CButton*)GetDlgItem (IDC_CHECK_VNAU2))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_CRITIC2))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_TGVJUM2))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_AFFAIRE2))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_DESSERTE2))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_GESTIONOD2))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_REVISION2))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_NIVEAU2))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_EQUIP2))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_FORCE2))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_SUPP2))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_SERVICE2))->SetCheck(0);
	((CButton*)GetDlgItem (IDC_CHECK_NOMOS2))->SetCheck(0);
}

CCalendarWnd::choixAffichage CCalendarView::GetCheck1()
{
  if (((CButton*)GetDlgItem (IDC_CHECK_TOP1))->GetCheck())
	return CCalendarWnd::TOP;
  if (((CButton*)GetDlgItem (IDC_CHECK_COMMENT1))->GetCheck())
	return CCalendarWnd::COMMENT;
  if (((CButton*)GetDlgItem (IDC_CHECK_VNAU1))->GetCheck())
	return CCalendarWnd::VNAU;
  if (((CButton*)GetDlgItem (IDC_CHECK_CRITIC1))->GetCheck())
	return CCalendarWnd::CRITIC;
  if (((CButton*)GetDlgItem (IDC_CHECK_TGVJUM1))->GetCheck())
	return CCalendarWnd::TGVJUM;
  if (((CButton*)GetDlgItem (IDC_CHECK_AFFAIRE1))->GetCheck())
	return CCalendarWnd::AFFAIRE;
  if (((CButton*)GetDlgItem (IDC_CHECK_DESSERTE1))->GetCheck())
	return CCalendarWnd::DESSERTE;
  if (((CButton*)GetDlgItem (IDC_CHECK_GESTIONOD1))->GetCheck())
	return CCalendarWnd::GESTIONOD;
  if (((CButton*)GetDlgItem (IDC_CHECK_REVISION1))->GetCheck())
	return CCalendarWnd::REVISION;
  if (((CButton*)GetDlgItem (IDC_CHECK_NIVEAU1))->GetCheck())
	return CCalendarWnd::NIVEAU;
  if (((CButton*)GetDlgItem (IDC_CHECK_EQUIP1))->GetCheck())
	return CCalendarWnd::EQUIP;
  if (((CButton*)GetDlgItem (IDC_CHECK_FORCE1))->GetCheck())
	return CCalendarWnd::FORCEMENT;
  if (((CButton*)GetDlgItem (IDC_CHECK_SUPP1))->GetCheck())
	return CCalendarWnd::SUPPLEMENTAIRE;
  if (((CButton*)GetDlgItem (IDC_CHECK_SERVICE1))->GetCheck())
	return CCalendarWnd::SERVICE;
  if (((CButton*)GetDlgItem (IDC_CHECK_NOMOS1))->GetCheck())
	return CCalendarWnd::NOMOS;
  if (((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->GetCheck())
	return CCalendarWnd::TAUX_PREMIERE;
  if (((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->GetCheck())
	return CCalendarWnd::TAUX_SECONDE;
  return CCalendarWnd::UNDEFINED;
}

CCalendarWnd::choixAffichage CCalendarView::GetCheck2()
{
  if (((CButton*)GetDlgItem (IDC_CHECK_TOP2))->GetCheck())
	return CCalendarWnd::TOP;
  if (((CButton*)GetDlgItem (IDC_CHECK_COMMENT2))->GetCheck())
	return CCalendarWnd::COMMENT;
  if (((CButton*)GetDlgItem (IDC_CHECK_VNAU2))->GetCheck())
	return CCalendarWnd::VNAU;
  if (((CButton*)GetDlgItem (IDC_CHECK_CRITIC2))->GetCheck())
	return CCalendarWnd::CRITIC;
  if (((CButton*)GetDlgItem (IDC_CHECK_TGVJUM2))->GetCheck())
	return CCalendarWnd::TGVJUM;
  if (((CButton*)GetDlgItem (IDC_CHECK_AFFAIRE2))->GetCheck())
	return CCalendarWnd::AFFAIRE;
  if (((CButton*)GetDlgItem (IDC_CHECK_DESSERTE2))->GetCheck())
	return CCalendarWnd::DESSERTE;
  if (((CButton*)GetDlgItem (IDC_CHECK_GESTIONOD2))->GetCheck())
	return CCalendarWnd::GESTIONOD;
  if (((CButton*)GetDlgItem (IDC_CHECK_REVISION2))->GetCheck())
	return CCalendarWnd::REVISION;
  if (((CButton*)GetDlgItem (IDC_CHECK_NIVEAU2))->GetCheck())
	return CCalendarWnd::NIVEAU;
  if (((CButton*)GetDlgItem (IDC_CHECK_EQUIP2))->GetCheck())
	return CCalendarWnd::EQUIP;
  if (((CButton*)GetDlgItem (IDC_CHECK_FORCE2))->GetCheck())
	return CCalendarWnd::FORCEMENT;
  if (((CButton*)GetDlgItem (IDC_CHECK_SUPP2))->GetCheck())
	return CCalendarWnd::SUPPLEMENTAIRE;
  if (((CButton*)GetDlgItem (IDC_CHECK_SERVICE2))->GetCheck())
	return CCalendarWnd::SERVICE;
  if (((CButton*)GetDlgItem (IDC_CHECK_NOMOS2))->GetCheck())
	return CCalendarWnd::NOMOS;
  return CCalendarWnd::UNDEFINED;
}

CCalendarWnd::choixAffichage CCalendarView::GetChoix (CString sChoix)
{
  if (sChoix == "TOP")
    return CCalendarWnd::TOP;
  if (sChoix == "COMMENT")
    return CCalendarWnd::COMMENT;
  if (sChoix == "VNAU")
    return CCalendarWnd::VNAU;
  if (sChoix == "CRITIC")
    return CCalendarWnd::CRITIC;
  if (sChoix == "TGVJUM")
    return CCalendarWnd::TGVJUM;
  if (sChoix == "AFFAIRE")
    return CCalendarWnd::AFFAIRE;
  if (sChoix == "DESSERTE")
    return CCalendarWnd::DESSERTE;
  if (sChoix == "GESTIONOD")
    return CCalendarWnd::GESTIONOD;
  if (sChoix == "REVISION")
    return CCalendarWnd::REVISION;
  if (sChoix == "NIVEAU")
    return CCalendarWnd::NIVEAU;
  if (sChoix == "EQUIP")
    return CCalendarWnd::EQUIP;
  if (sChoix == "FORCEMENT")
    return CCalendarWnd::FORCEMENT;
  if (sChoix == "SUPPLEMENTAIRE")
    return CCalendarWnd::SUPPLEMENTAIRE;
  if (sChoix == "SERVICE")
    return CCalendarWnd::SERVICE;
  if (sChoix == "NOMOS")
    return CCalendarWnd::NOMOS;
  if (sChoix == "TAUX_PREMIERE")
    return CCalendarWnd::TAUX_PREMIERE;
  if (sChoix == "TAUX_SECONDE")
    return CCalendarWnd::TAUX_SECONDE;
  return CCalendarWnd::UNDEFINED;
}

CString CCalendarView::GetStrChoix (CCalendarWnd::choixAffichage ch)
{
  CString ss;
  switch (ch)
  {
    case CCalendarWnd::TOP: ss = "TOP"; break;
	case CCalendarWnd::COMMENT: ss = "COMMENT"; break;
	case CCalendarWnd::VNAU: ss = "VNAU"; break;
	case CCalendarWnd::CRITIC: ss = "CRITIC"; break;
	case CCalendarWnd::TGVJUM: ss = "TGVJUM"; break;
	case CCalendarWnd::AFFAIRE: ss = "AFFAIRE"; break;
	case CCalendarWnd::DESSERTE: ss = "DESSERTE"; break;
	case CCalendarWnd::GESTIONOD: ss = "GESTIONOD"; break;
	case CCalendarWnd::REVISION: ss = "REVISION"; break;
	case CCalendarWnd::NIVEAU: ss = "NIVEAU"; break;
    case CCalendarWnd::EQUIP: ss = "EQUIP"; break;
	case CCalendarWnd::FORCEMENT: ss = "FORCEMENT"; break;
    case CCalendarWnd::SUPPLEMENTAIRE: ss = "SUPPLEMENTAIRE"; break;
	case CCalendarWnd::SERVICE: ss = "SERVICE"; break;
	case CCalendarWnd::NOMOS: ss = "NOMOS"; break;
	case CCalendarWnd::TAUX_PREMIERE: ss = "TAUX_PREMIERE"; break;
	case CCalendarWnd::TAUX_SECONDE: ss = "TAUX_SECONDE"; break;
	default : ss = "UNDEFINED";
  }
  return ss;
}

void CCalendarView::SetCheck1 (CCalendarWnd::choixAffichage ch)
{
  UnCheckColumn1();
  int nId = 0;
  switch (ch)
  {
    case CCalendarWnd::TOP: nId = IDC_CHECK_TOP1; break;
	case CCalendarWnd::COMMENT: nId = IDC_CHECK_COMMENT1; break;
	case CCalendarWnd::VNAU: nId = IDC_CHECK_VNAU1; break;
	case CCalendarWnd::CRITIC: nId = IDC_CHECK_CRITIC1; break;
	case CCalendarWnd::TGVJUM: nId = IDC_CHECK_TGVJUM1; break;
	case CCalendarWnd::AFFAIRE: nId = IDC_CHECK_AFFAIRE1; break;
	case CCalendarWnd::DESSERTE: nId = IDC_CHECK_DESSERTE1; break;
	case CCalendarWnd::GESTIONOD: nId = IDC_CHECK_GESTIONOD1; break;
	case CCalendarWnd::REVISION: nId = IDC_CHECK_REVISION1; break;
	case CCalendarWnd::NIVEAU: nId = IDC_CHECK_NIVEAU1; break;
    case CCalendarWnd::EQUIP: nId = IDC_CHECK_EQUIP1; break;
	case CCalendarWnd::FORCEMENT: nId = IDC_CHECK_FORCE1; break;
	case CCalendarWnd::SUPPLEMENTAIRE: nId = IDC_CHECK_SUPP1; break;
	case CCalendarWnd::SERVICE: nId = IDC_CHECK_SERVICE1; break;
	case CCalendarWnd::NOMOS: nId = IDC_CHECK_NOMOS1; break;
	case CCalendarWnd::TAUX_PREMIERE: nId = IDC_CHECK_TAUX_1ERE; break;
	case CCalendarWnd::TAUX_SECONDE: nId = IDC_CHECK_TAUX_2EME; break;
	default : nId = 0;
  }

  if (nId)
    ((CButton*)GetDlgItem (nId))->SetCheck(1);
}

void CCalendarView::SetCheck2 (CCalendarWnd::choixAffichage ch)
{
  UnCheckColumn2();
  int nId = 0;
  switch (ch)
  {
    case CCalendarWnd::TOP: nId = IDC_CHECK_TOP2; break;
	case CCalendarWnd::COMMENT: nId = IDC_CHECK_COMMENT2; break;
	case CCalendarWnd::VNAU: nId = IDC_CHECK_VNAU2; break;
	case CCalendarWnd::CRITIC: nId = IDC_CHECK_CRITIC2; break;
	case CCalendarWnd::TGVJUM: nId = IDC_CHECK_TGVJUM2; break;
	case CCalendarWnd::AFFAIRE: nId = IDC_CHECK_AFFAIRE2; break;
	case CCalendarWnd::DESSERTE: nId = IDC_CHECK_DESSERTE2; break;
	case CCalendarWnd::GESTIONOD: nId = IDC_CHECK_GESTIONOD2; break;
	case CCalendarWnd::REVISION: nId = IDC_CHECK_REVISION2; break;
	case CCalendarWnd::NIVEAU: nId = IDC_CHECK_NIVEAU2; break;
    case CCalendarWnd::EQUIP: nId = IDC_CHECK_EQUIP2; break;
	case CCalendarWnd::FORCEMENT: nId = IDC_CHECK_FORCE2; break;
	case CCalendarWnd::SUPPLEMENTAIRE: nId = IDC_CHECK_SUPP2; break;
	case CCalendarWnd::SERVICE: nId = IDC_CHECK_SERVICE2; break;
	case CCalendarWnd::NOMOS: nId = IDC_CHECK_NOMOS2; break;
	default : nId = 0;
  }

  if (nId)
    ((CButton*)GetDlgItem (nId))->SetCheck(1);
}

void CCalendarView::EraseResa()
{
  m_pCalendar->PurgeTrancheResa();
}

void CCalendarView::LaunchReadTrancheResa()
{
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->EnableWindow(false);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->EnableWindow(false);
   YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  if (pTranche)
	  m_pCalendar->ReadTrancheResa(pTranche->TrancheNo(), pTranche->FamilyNo(),
	                               pTranche->FamilyFlag());
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_1ERE))->EnableWindow(true);
  ((CButton*)GetDlgItem (IDC_CHECK_TAUX_2EME))->EnableWindow(true);
}

