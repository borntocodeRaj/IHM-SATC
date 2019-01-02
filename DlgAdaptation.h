#if !defined(AFX_DLGADAPTATION_H__3F88A415_CEA7_4577_95AF_4636624FBE19__INCLUDED_)
#define AFX_DLGADAPTATION_H__3F88A415_CEA7_4577_95AF_4636624FBE19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAdaptation.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CDlgAdaptation dialog

class CDlgAdaptation : public CDialog
{
// Construction
public:
	CDlgAdaptation(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAdaptation)
	enum { IDD = IDD_DLG_ADAPTATION };
  CComboBox	m_comboDptDate;
	CComboBox	m_comboCmpt;
	CComboBox	m_comboTranche;
	CString	m_szSeatAfter;
	CString	m_szSeatBefore;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAdaptation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CString FormatDptDate(int iDptDate);
  void InsertTrancheNo(YmIcTrancheLisDom* pTrancheIn);
  YmIcTrancheLisDom* BuildTrancheNoList();
  void InsertDptDate(YmIcTrancheLisDom* pTrancheIn);
  void BuildDptDateList(YmIcTrancheLisDom* pTrancheIn);
  void InsertCmpt(CString szCmptIn, int iCapacityIn);
  void BuildCmptList(YmIcTrancheLisDom* pTrancheIn);
  CTrancheView* GetTrancheView();

	// Generated message map functions
	//{{AFX_MSG(CDlgAdaptation)
	virtual BOOL OnInitDialog();
	virtual void OnAdaptationAdd();
	afx_msg void OnSelchangeComboTranche();
  afx_msg void OnSelchangeComboDptDate();
  afx_msg void OnSelchangeComboCmpt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  CStringArray m_aszCmpts;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADAPTATION_H__3F88A415_CEA7_4577_95AF_4636624FBE19__INCLUDED_)
