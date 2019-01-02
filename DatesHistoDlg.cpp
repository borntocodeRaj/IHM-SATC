// DlgJumeaux.cpp : implementation file
//
#include "StdAfx.h"


#include "DatesHistoDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDatesHistoDlg::CDatesHistoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDatesHistoDlg::IDD, pParent)
{
	RWDate dummy (1,1,1900);
    m_rwDateHisto1 = dummy;
	m_rwDateHisto2 = dummy;
	m_rwDateHisto3 = dummy;
	m_rwDptDate = dummy;
	m_szNature = _T(""); // NPI - DT12135

	CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
	m_pQueryDateHisto = new YM_Query (*(WinApp->m_pDatabase));  // AutoDelete = FALSE;
	m_pQueryDateHisto->AddObserver(this);  
}

CDatesHistoDlg::~CDatesHistoDlg()
{
	delete m_pQueryDateHisto;
}

void CDatesHistoDlg::OnNotify( YM_Observable* pObject )
{
	if (pObject->IsKindOf( RUNTIME_CLASS(YM_RecordSet) ) )
	{
		CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
		if( (YM_RecordSet*)pObject == WinApp->GetDocument()->m_pDateHistoSet &&
			WinApp->GetDocument()->m_pDateHistoSet->IsValid())
		{
			YM_Iterator<YmIcDateHistoDom>* pVariablesIterator =
				YM_Set<YmIcDateHistoDom>::FromKey (DATE_HISTO_KEY)->CreateIterator();
			YmIcDateHistoDom* pDateHisto = NULL;
			for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
			{
				pDateHisto = pVariablesIterator->Current();
			}
			delete pVariablesIterator;

			CWnd* pwnd;
			pwnd = GetDlgItem (IDC_DPTDATE);
			pwnd->EnableWindow (TRUE);
			pwnd = GetDlgItem (IDOK);
			pwnd->EnableWindow (TRUE);
			if (pDateHisto)
			{
				if (pDateHisto->DateHisto1() != 0)
				{
                    m_rwDateHisto1.julian(pDateHisto->DateHisto1());
				}
				if (pDateHisto->DateHisto2() != 0)
				{
                    m_rwDateHisto2.julian(pDateHisto->DateHisto2());
				}
				if (pDateHisto->DateHisto3() != 0)
				{
                    m_rwDateHisto3.julian(pDateHisto->DateHisto3());
				}

				RWDate dummy (1,1,1900);
				if (m_rwDateHisto1 != dummy)
				{
					m_szDateHisto1 = m_rwDateHisto1.asString("%d/%m/%Y");
				}
				if (m_rwDateHisto2 != dummy)
				{
					m_szDateHisto2 = m_rwDateHisto2.asString("%d/%m/%Y");
				}
				if (m_rwDateHisto3 != dummy)
				{
					m_szDateHisto3 = m_rwDateHisto3.asString("%d/%m/%Y");
				}
			}
		}
	}

	UpdateData(FALSE);
}

void CDatesHistoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTgvJumDlg)
	DDX_Control(pDX, IDC_COLOR1, m_Color1);
	DDX_Control(pDX, IDC_COLOR2, m_Color2);
	DDX_Control(pDX, IDC_COLOR3, m_Color3);
	DDX_Control(pDX, IDC_DPTDATE, m_CtlDptDate);
	DDX_Text(pDX, IDC_DATE_HISTO1, m_szDateHisto1);
	DDX_Text(pDX, IDC_DATE_HISTO2, m_szDateHisto2);
	DDX_Text(pDX, IDC_DATE_HISTO3, m_szDateHisto3);
	DDX_DateTimeCtrl(pDX, IDC_DPTDATE, m_tmDptDate);
	DDX_Control(pDX, IDC_COMBO_NATURE, m_ctlComboNature); // NPI - DT12135
	DDX_Text(pDX, IDC_COMBO_NATURE, m_szNature); // NPI - DT12135
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDatesHistoDlg, CDialog)
	//{{AFX_MSG_MAP(CDatesHistoDlg)
	ON_NOTIFY(DTN_CLOSEUP, IDC_DPTDATE, OnDateChange)
	ON_NOTIFY(NM_KILLFOCUS, IDC_DPTDATE, OnDateChange)
	ON_CBN_SELCHANGE(IDC_COMBO_NATURE, OnComboNatureSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTgvJumDlg message handlers


BOOL CDatesHistoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	CWnd* pwnd;
	pwnd = GetDlgItem(IDOK);
	pwnd->EnableWindow(FALSE);
	pwnd = GetDlgItem(IDC_DPTDATE);
	pwnd->EnableWindow(FALSE);
	pwnd = GetDlgItem(IDC_DATE_HISTO1);
	pwnd->EnableWindow(FALSE);
	pwnd = GetDlgItem(IDC_DATE_HISTO2);
	pwnd->EnableWindow(FALSE);
	pwnd = GetDlgItem(IDC_DATE_HISTO3);
	pwnd->EnableWindow(FALSE);

	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	m_Color1.SetBkColor(pDoc->m_pGlobalSysParms->GetGraphRGBValue(CCTAChart::eHisto1ResHoldTotStyleIdx));
	m_Color2.SetBkColor(pDoc->m_pGlobalSysParms->GetGraphRGBValue(CCTAChart::eHisto2ResHoldTotStyleIdx));
	m_Color3.SetBkColor(pDoc->m_pGlobalSysParms->GetGraphRGBValue(CCTAChart::eHisto3ResHoldTotStyleIdx));
	m_Color1.ShowWindow(SW_SHOW);
	m_Color2.ShowWindow(SW_SHOW);
	m_Color3.ShowWindow(SW_SHOW);

	  
	YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	RWDate dptdate;
	RWDate today;
	if (pTranche && pTranche->TrancheNo())
	{
		dptdate.julian( pTranche->DptDate() );
	}
	else
	{
		dptdate = today; 
	}
	COleDateTime tmDate(dptdate.year(), dptdate.month(), dptdate.dayOfMonth(), 0, 0, 0);
	m_tmDptDate = tmDate;
	m_CtlDptDate.SetTime(m_tmDptDate);

	// NPI - DT12135
	if (pTranche)
	{
		m_szNature.Format("%ld", pTranche->Nature());
		m_lNatureByDef = pTranche->Nature();
		if (pTranche->Nature() == 0)
		{
			m_ctlComboNature.AddString("0");
			m_ctlComboNature.EnableWindow(FALSE);
		}
		else if ((pTranche->Nature() == 1) || (pTranche->Nature() == 2) || (pTranche->Nature() == 3))
		{
			m_ctlComboNature.AddString("1");
			m_ctlComboNature.AddString("2");
			m_ctlComboNature.AddString("3");
		}
		else if ((pTranche->Nature() == 4) || (pTranche->Nature() == 5))
		{
			m_ctlComboNature.AddString("4");
			m_ctlComboNature.AddString("5");
		}
	}
	///

	CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
	((CWnd*)GetDlgItem(IDC_ENTITY))->SetWindowText(WinApp->GetDocument()->GetEntities());
	WinApp->GetDocument()->m_pDateHistoSet->AddObserver(this);	// DATE_HISTO_KEY
	WinApp->m_queryDate = dptdate;
	m_rwDptDate = dptdate;
	if (WinApp->GetDocument()->m_pDateHistoSet->IsOpen())
	{
		WinApp->GetDocument()->m_pDateHistoSet->Close();
	}
	WinApp->GetDocument()->m_pDateHistoSet->Open();
	  
	return TRUE;  // return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}

void CDatesHistoDlg::OnDateChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	RWDate dummy (1,1,1900);
	m_rwDateHisto1 = dummy;
	m_szDateHisto1.Empty();
	m_rwDateHisto2 = dummy;
	m_szDateHisto2.Empty();
	m_rwDateHisto3 = dummy;
	m_szDateHisto3.Empty();

	m_szNature.Format("%ld", m_lNatureByDef); // NPI - DT12135

	m_CtlDptDate.GetTime(m_tmDptDate);
	RWDate dptdate(m_tmDptDate.GetDay(), m_tmDptDate.GetMonth(), m_tmDptDate.GetYear());
	CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
	WinApp->m_queryDate = dptdate;
	m_rwDptDate = dptdate;
	WinApp->GetDocument()->m_pDateHistoSet->Close();
	WinApp->GetDocument()->m_pDateHistoSet->Open();

	*pResult = 0;
}

void CDatesHistoDlg::OnOK() 
{
	CDialog::OnOK();
}

// NPI - DT12135
void CDatesHistoDlg::OnComboNatureSelchange()
{
	RWDate dummy (1,1,1900);
	m_rwDateHisto1 = dummy;
	m_szDateHisto1.Empty();
	m_rwDateHisto2 = dummy;
	m_szDateHisto2.Empty();
	m_rwDateHisto3 = dummy;
	m_szDateHisto3.Empty();

	CString sNature;
	m_ctlComboNature.GetLBText(m_ctlComboNature.GetCurSel(), sNature);
	m_szNature.Format("%s", sNature);

	CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
	WinApp->m_queryDateRefDlg = TRUE;
	WinApp->SetQueryEntityNatureID(((CCTAApp*)APP)->GetDocument()->GetEntities(), atol(sNature));
	WinApp->GetDocument()->m_pDateHistoSet->Close();
	WinApp->GetDocument()->m_pDateHistoSet->Open();
}
///
