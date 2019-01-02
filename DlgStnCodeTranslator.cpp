// DlgStnCodeTranslator.cpp : implementation file
//
#include "stdafx.h"


#include "DlgStnCodeTranslator.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDlgStnCodeTranslatorSet
YmDtDom* CDlgStnCodeTranslatorSet::OnSelectQuery()
{
  this->StationCode(m_pParent->m_szStnCode);
  return this;
}


/////////////////////////////////////////////////////////////////////////////
// CDlgStnCodeTranslator dialog


CDlgStnCodeTranslator::CDlgStnCodeTranslator(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStnCodeTranslator::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgStnCodeTranslator)
	m_szStnCode = _T("");
	m_szStnDesc = _T("");
	//}}AFX_DATA_INIT
}


void CDlgStnCodeTranslator::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgStnCodeTranslator)
	DDX_Control(pDX, IDC_STATIC_STN_DESC, m_Static_StnDesc);
	DDX_Control(pDX, IDC_EDIT_STN_CODE, m_Edit_StnCode);
	DDX_Control(pDX, IDC_CHECK_TGV_STATION, m_CheckBox_TgvStation);
	DDX_Text(pDX, IDC_EDIT_STN_CODE, m_szStnCode);
	DDX_Text(pDX, IDC_STATIC_STN_DESC, m_szStnDesc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgStnCodeTranslator, CDialog)
	//{{AFX_MSG_MAP(CDlgStnCodeTranslator)
	ON_BN_CLICKED(IDC_BUTTON_TRANSLATE, OnButtonTranslate)
	ON_EN_CHANGE(IDC_EDIT_STN_CODE, OnChangeEditStnCode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgStnCodeTranslator message handlers

BOOL CDlgStnCodeTranslator::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_CheckBox_TgvStation.SetCheck(0);
  m_CheckBox_TgvStation.EnableWindow (FALSE);
	
  m_pStations = new CDlgStnCodeTranslatorSet(App().m_pDatabase);
  m_pStations->m_pParent = this;
	m_pStations->AddObserver(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgStnCodeTranslator::OnButtonTranslate() 
{
  UpdateData(TRUE);  // Transfer values from controls to member variables

  if (m_szStnCode.IsEmpty())  // user must put in something
  {
    MessageBeep(MB_ICONEXCLAMATION);
    m_Edit_StnCode.SetFocus();
    return;
  }
  // for sncf, automatically supply "FR" as prefix if user only enters 3 characters
  CString szClient = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient();
  if ((szClient == SNCF) ||  (szClient == SNCF_TRN))
  {
    if (m_szStnCode.GetLength() == 3)
      m_szStnCode = "FR" + m_szStnCode;
  }
  m_szStnCode.MakeUpper();
	m_pStations->Close();
	m_pStations->Open();  // automatically launches the SQL
}

void CDlgStnCodeTranslator::OnChangeEditStnCode() 
{
  // clear the output
  CString szBuffer;
  m_Static_StnDesc.SetWindowText(szBuffer);
  m_CheckBox_TgvStation.SetCheck(FALSE);
	
}

void CDlgStnCodeTranslator::OnOK() 
{
  delete m_pStations;
  m_pStations = NULL;

	CDialog::OnOK();
}

void CDlgStnCodeTranslator::OnNotify( YM_Observable* pObject )
{
  if( pObject->IsKindOf( RUNTIME_CLASS(YM_RecordSet) ) )
  {
    if( m_pStations->IsValid() ) 
    {
      YmBaTgvStationDom* pRecord;
      YM_Iterator<YmBaTgvStationDom>* pStationsIterator =
        m_pStations->CreateIterator();
      if ( pStationsIterator->GetCount() )
      {
        pStationsIterator->First();  // should be only one line returned
        pRecord = (YmBaTgvStationDom*)pStationsIterator->Current();
        m_szStnDesc = pRecord->Description();
        m_CheckBox_TgvStation.SetCheck(pRecord->TgvStationFlag());
      }
      else
      {
        CString szFormat = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BAD_STATIONCODE);
        m_szStnDesc.Format(szFormat, m_szStnCode);
        m_CheckBox_TgvStation.SetCheck(FALSE);
      }
	  delete pStationsIterator; // NPI - Correction Memory Leak

      UpdateData(FALSE);  // Transfer values from member variables to controls
      m_Edit_StnCode.SetFocus();
      m_Edit_StnCode.SetSel( 0, -1);
   }
  }
}
