#if !defined(AFX_DLGSTNCODETRANSLATOR_H__5A20A2A9_599A_4CBE_B52F_4330E0206441__INCLUDED_)
#define AFX_DLGSTNCODETRANSLATOR_H__5A20A2A9_599A_4CBE_B52F_4330E0206441__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgStnCodeTranslator.h : header file
//



class CDlgStnCodeTranslator;

class CDlgStnCodeTranslatorSet : public YM_ArraySet<YmBaTgvStationDom>
{
public:
	CDlgStnCodeTranslatorSet(YM_Database* pDatabase)
		: YM_ArraySet<YmBaTgvStationDom>(pDatabase,IDS_SQL_SELECT_STATION_DESCRIPTION){}

  CDlgStnCodeTranslator* m_pParent;
//  SetParent(CDlgStnCodeTranslator* pParent) { m_pParent = pParent; }

protected:
	YmDtDom* OnSelectQuery();
};
/*
class CDlgStnCodeTranslatorDom : public YmBaTgvStationDom
{
  YmDtDom* OnSelectQuery();
};
*/
/////////////////////////////////////////////////////////////////////////////
// CDlgStnCodeTranslator dialog

class CDlgStnCodeTranslator : public CDialog, public YM_Observer
{
// Construction
public:
	CDlgStnCodeTranslator(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgStnCodeTranslator)
	enum { IDD = IDD_DLG_STN_CODE_TRANSLATOR };
	CStatic	m_Static_StnDesc;
	CEdit	m_Edit_StnCode;
	CButton	m_CheckBox_TgvStation;
	CString	m_szStnCode;
	CString	m_szStnDesc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgStnCodeTranslator)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgStnCodeTranslator)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonTranslate();
	afx_msg void OnChangeEditStnCode();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void OnNotify( YM_Observable* pObject );

	CDlgStnCodeTranslatorSet* m_pStations;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSTNCODETRANSLATOR_H__5A20A2A9_599A_4CBE_B52F_4330E0206441__INCLUDED_)
