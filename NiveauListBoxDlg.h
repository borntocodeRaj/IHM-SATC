#pragma once


// Boîte de dialogue CNiveauListBoxDlg

class CNiveauListBoxDlg : public CDialog
{
	DECLARE_DYNAMIC(CNiveauListBoxDlg)

public:
	CNiveauListBoxDlg(long sNiveauGamme, CWnd* pParent = NULL);   // constructeur standard
	virtual ~CNiveauListBoxDlg();
	CString getSelText() { return m_seltext; }

// Données de boîte de dialogue
	enum { IDD = IDD_LIST_SEL_NIVEAU };
	CListBox	m_Listbox;
	CString m_sNiveauGamme;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV
	CString m_seltext;

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual void OnOK();

public:
	void AjouterChoixNiveau(CString sNiveau);
	int m_nIndex;
	int m_nCurSel;
};
