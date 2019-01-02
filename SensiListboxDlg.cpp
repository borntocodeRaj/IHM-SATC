// SensiListboxDlg.cpp : implementation file
//

#include "StdAfx.h"





#include "SensiListboxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSensiListboxDlg dialog

CSensiListboxDlg::CSensiListboxDlg( CString sSensi, CWnd* pParent /*=NULL*/)
	: CDialog(CSensiListboxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSensiListboxDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  m_pParent = pParent;
  m_inival = sSensi;
}

CSensiListboxDlg::~CSensiListboxDlg()
{
}

void CSensiListboxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSensiListboxDlg)
	DDX_Control(pDX, IDC_LISTBOX_CHOIX, m_Listbox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSensiListboxDlg, CDialog)
	//{{AFX_MSG_MAP(CSensiListboxDlg)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSensiListboxDlg message handlers

BOOL CSensiListboxDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  // Iterate through the leg list adding all but the currently loaded leg
  CString szTopDesc;
	YM_Iterator<YmIcTypeTrainDom>* pIterator = 
		YM_Set<YmIcTypeTrainDom>::FromKey(SENSIBILITE_KEY)->CreateIterator();
  YmIcTypeTrainDom* pTop;
  int index = 0;
  int isel = -1;
  CString sst;
  for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
  {
    pTop = pIterator->Current();
	sst = pTop->SousTypeTrain();
	if (sst == m_inival)
	  isel = index;
	szTopDesc.Format( "%s\t%s", pTop->SousTypeTrain(), pTop->Description() );
	m_Listbox.AddString(szTopDesc);
	index ++;
  }
  delete pIterator;

  if (isel >= 0)
	m_Listbox.SetCurSel(isel);

  SetWindowText (((CCTAApp*)APP)->GetResource()->LoadResString(ID_TRANCHE_SENSIBILITE));
  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSensiListboxDlg::OnOK() 
{
  // ensure the user has selected at least one item in the listbox
  int isel = m_Listbox.GetCurSel();
  if ( isel == LB_ERR )
  {
    MessageBeep(MB_ICONEXCLAMATION);
    return;
  }
  CString toto;
  m_Listbox.GetText(isel, toto);
  int k = toto.Find('\t');
  m_seltext = toto.Left(k);

  CDialog::OnOK();
}


