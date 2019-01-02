#if !defined(AFX_DLGSUPPLEMENT_H__3F88A415_CEA7_4577_95AF_4636624FBE19__INCLUDED_)
#define AFX_DLGSUPPLEMENT_H__3F88A415_CEA7_4577_95AF_4636624FBE19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSupplement.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CDlgSupplement dialog

class CDlgSupplement : public CDialog
{
// Construction
public:
	CDlgSupplement(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSupplement)
	enum { IDD = IDD_DLG_SUPPLEMENT };
  CStatic	m_staticDptDate;
	CStatic	m_staticTranche;
	CString	m_szSeatAfter;
	CString	m_szSeatBefore;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSupplement)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  YmIcTrancheLisDom* m_pTrancheSelected;

  CString FormatDptDate(int iDptDate);
  CTrancheView* GetTrancheView();
  void InsertRecord(YmIcResaCmptDom* pRecord);

	// Generated message map functions
	//{{AFX_MSG(CDlgSupplement)
	virtual BOOL OnInitDialog();
	virtual void OnSupplementAdd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSUPPLEMENT_H__3F88A415_CEA7_4577_95AF_4636624FBE19__INCLUDED_)
