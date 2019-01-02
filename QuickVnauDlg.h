// QuickVnauDlg.h : header file
//
#if !defined(AFX_QUICKVNAUDLG_H__DD767281_5F12_11D5_A8B9_0002A50F053B__INCLUDED_)
#define AFX_QUICKVNAUDLG_H__DD767281_5F12_11D5_A8B9_0002A50F053B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class YM_Observer;
class YM_Observable;
class CVnasByLegAuthMap;

/////////////////////////////////////////////////////////////////////////////
// CQuickVnauDlg dialog
class CQuickVnauDlg : public CDialog, public YM_Observer
{
// Construction
public:
	CQuickVnauDlg();
	~CQuickVnauDlg();

	void Show();
	void Hide();

	void OnNotify( YM_Observable* pObject );
	void Changefont();

// Dialog Data
	//{{AFX_DATA(CQuickVnauDlg)
	enum { IDD = IDD_QUICK_VNAU };
	CString	m_input;
	CString	m_result;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuickVnauDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void Create();
	int AnalyseVnau(); // return 0 if OK, else return a message ID
	void BuildAndSendVnau();
	void BuildAndSendVnas();
	void EnvoiVnl();
    void EnvoiDispo();
	void Envoi4GY();
	void EnableInput (BOOL enabled);

	CString m_basemsg;	// VNAUxxxx/jjMMM
	CString m_basedemain; // idem mais pour le lendemain.

	CVnauByLegAuthMap* m_LegAuthList; // To store the result of the VNL.
	CVnasByLegAuthMap* m_LegSciList; // To store the SCI result of the VNL.
	CVnauAutho m_NewAuth; // autorisations saisies par l'utilisateur
	CVnasAutho m_NewSci;  // quotas SCI saisis par l'utilisateur
	CString m_jumdata;		// string containing % tgv jumeaux
	BOOL    m_bwaitingVnl;

	// DM 5480 train auto
	BOOL m_bTrainAuto;
	int m_capaTrainAuto;

	// Generated message map functions
	//{{AFX_MSG(CQuickVnauDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPurge();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUICKVNAUDLG_H__DD767281_5F12_11D5_A8B9_0002A50F053B__INCLUDED_)
