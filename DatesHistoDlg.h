#if !defined(AFX_DLGHISTODATES_H__802DED31_EFC0_11D1_91DC_400020302A10__INCLUDED_)
#define AFX_DLGHISTODATES_H__802DED31_EFC0_11D1_91DC_400020302A10__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DatesHistoDlh.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTgvJumDlg dialog
class YM_RecordSet;

class CDatesHistoDlg : public CDialog, public YM_Observer
{
// Construction
public:
	CDatesHistoDlg(CWnd* pParent = NULL);   // standard constructor
	~CDatesHistoDlg();						 // Destructor

// Dialog Data
	//{{AFX_DATA(CTgvJumDlg)
	enum { IDD = IDD_DLG_DATES_HISTO };
	CDateTimeCtrl	m_CtlDptDate;
	COleDateTime	m_tmDptDate;

	CString	m_szDateHisto1;
	CString	m_szDateHisto2;
	CString	m_szDateHisto3;

	YM_Static	m_Color1;
	YM_Static	m_Color2;
	YM_Static	m_Color3;

	CComboBox	m_ctlComboNature; // NPI - DT12135
	CString	m_szNature; // NPI - DT12135
	//}}AFX_DATA

	long m_lNatureByDef;// NPI - DT12135

    RWDate m_rwDptDate;
	RWDate m_rwDateHisto1;
	RWDate m_rwDateHisto2;
	RWDate m_rwDateHisto3;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTgvJumDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnNotify( YM_Observable* pObject );
	//}}AFX_VIRTUAL
	
    YM_Query* m_pQueryDateHisto;

protected:

	// Generated message map functions
	//{{AFX_MSG(CTgvJumDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDateChange(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnComboNatureSelchange(); // NPI - DT12135
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGHISTODATES_H__802DED31_EFC0_11D1_91DC_400020302A10__INCLUDED_)
