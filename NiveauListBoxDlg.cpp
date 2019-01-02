// NiveauListBoxDlg.cpp : fichier d'implémentation
//

#include "StdAfx.h"



#include "NiveauListBoxDlg.h"
#include ".\niveaulistboxdlg.h"


// Boîte de dialogue CNiveauListBoxDlg

IMPLEMENT_DYNAMIC(CNiveauListBoxDlg, CDialog)
CNiveauListBoxDlg::CNiveauListBoxDlg(long nNiveauGamme, CWnd* pParent /*=NULL*/)
	: CDialog(CNiveauListBoxDlg::IDD, pParent)
	, m_nIndex(0)
	, m_nCurSel(-1)
{
	m_sNiveauGamme.Format("%ld", nNiveauGamme);
}

CNiveauListBoxDlg::~CNiveauListBoxDlg()
{
}

void CNiveauListBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTopListboxDlg)
	DDX_Control(pDX, IDC_LISTBOX_CHOIX_NIVEAU, m_Listbox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNiveauListBoxDlg, CDialog)
END_MESSAGE_MAP()


// Gestionnaires de messages CNiveauListBoxDlg
BOOL CNiveauListBoxDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  AjouterChoixNiveau("0");
  AjouterChoixNiveau("4");

  if (m_nCurSel >= 0)
  {
      m_Listbox.SetCurSel(m_nCurSel);
  }

  //SetWindowText (((CCTAApp*)APP)->GetResource()->LoadResString(ID_TRANCHE_NIVEAU));

  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNiveauListBoxDlg::AjouterChoixNiveau(CString sNiveau)
{
	m_Listbox.AddString(sNiveau);
	if (sNiveau == m_sNiveauGamme)
	{
		m_nCurSel = m_nIndex;
	}
	m_nIndex++;
}

void CNiveauListBoxDlg::OnOK() 
{
	// ensure the user has selected at least one item in the listbox
	int isel = m_Listbox.GetCurSel();
	if (isel == LB_ERR)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}
	m_Listbox.GetText(isel, m_seltext);
	
	CDialog::OnOK();
}
