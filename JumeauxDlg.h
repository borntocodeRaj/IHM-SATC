#if !defined(AFX_DLGJUMEAUX_H__802DED31_EFC0_11D1_91DC_400020302A10__INCLUDED_)
#define AFX_DLGJUMEAUX_H__802DED31_EFC0_11D1_91DC_400020302A10__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgJumeaux.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTgvJumDlg dialog
class YM_RecordSet;
class YM_Observer;

class CTgvJumDlg : public CDialog, public YM_Observer
{
// Construction
public:
	CTgvJumDlg(CWnd* pParent = NULL);   // standard constructor
	~CTgvJumDlg();						 // Destructor

	void OnNotify( YM_Observable* pObject );
	CString PctVal (TCHAR cmpt);
	void SetPctVal (TCHAR cmpt, int pct, BOOL eraseprev);

	YmIcTrancheLisDom* pSelectedTranche;
	int m_resacmptkey;

// Dialog Data
	//{{AFX_DATA(CTgvJumDlg)
	enum { IDD = IDD_DLG_TGVJUM };
	UINT	m_pct1;
	UINT	m_pct2;
	UINT	m_pct3;
	UINT	m_pct4;
	UINT	m_pct5;
	UINT	m_pct6;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTgvJumDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
// RecordSet to get the list of symbols
	long m_curTwinTgv;
	long m_LastRRDindex;
	unsigned long m_CurDptDate;
	long m_curTrancheNo;
	CString	m_vnlresult;
	CString m_vnlmsg;
	BOOL m_notified;

	void ActivatePctEdit();

protected:

	// Generated message map functions
	//{{AFX_MSG(CTgvJumDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTrainInfluentDlg dialog

class CTrainInfluentDlg : public CDialog, public YM_Observer
{
// Construction
public:
	CTrainInfluentDlg(CWnd* pParent = NULL);   // standard constructor
    ~CTrainInfluentDlg();

	void OnNotify( YM_Observable* pObject );

	YmIcTrancheLisDom* pSelectedTranche;

// Dialog Data
	//{{AFX_DATA(CTrainInfluentDlg)
	enum { IDD = IDD_DLG_TRANCHEJUM };
	CString m_NewTwinTrain;
//	UINT	m_NewTwinTrain;
	UINT	m_CurTrancheNo;
	CString	m_szDptDate;
	//}}AFX_DATA

    UINT  m_NewTwinTranche;
	UINT  m_CurTwinTranche;
	CString m_CurTrainNo;
//	UINT  m_CurTrainNo;
	unsigned long m_CurDptDate;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrainInfluentDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
// RecordSet to get the list of Tranche/train for this dpt_date.
	YM_RecordSet* m_pTrainList;
	BOOL m_notified;

	void UpdateTwinTranche(BOOL SetIt);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTrainInfluentDlg)
		// NOTE: the ClassWizard will add member functions here
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSupprTwin();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGJUMEAUX_H__802DED31_EFC0_11D1_91DC_400020302A10__INCLUDED_)
