#if !defined(AFX_ENTITYSTATSDLG_H__11C06DC3_21B3_11D4_90E3_00104BE1C142__INCLUDED_)
#define AFX_ENTITYSTATSDLG_H__11C06DC3_21B3_11D4_90E3_00104BE1C142__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EntityStatsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEntityStatsDlg dialog

class CEntityStatsDlg : public YM_ETSLayoutDialog
{
// Construction
public:
	CEntityStatsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEntityStatsDlg)
	enum { IDD = IDD_ENTITY_STATS };
	CString	m_szCriticalTranches;
	CString	m_szSentTranches;
	CString	m_szSentBatchTranches;
	CString	m_szSentHRRDTranches;
	CString	m_szTotalTranches;
	CString	m_szUnsentTranches;
	CString	m_szViewedTranches;
	CStatic	m_StaticViewedValue;
	CStatic	m_StaticUnsentValue;
	CStatic	m_StaticTotalValue;
	CStatic	m_StaticSentValue;
	CStatic	m_StaticSentBatchValue;
	CStatic	m_StaticSentHRRDValue;
	CStatic	m_StaticCriticalValue;
	YM_ShadeButton	m_btnOK;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEntityStatsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEntityStatsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTITYSTATSDLG_H__11C06DC3_21B3_11D4_90E3_00104BE1C142__INCLUDED_)
