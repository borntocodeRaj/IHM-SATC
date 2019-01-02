//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_ENTITYLISTDLG_H__D785F5F1_3FC1_11D1_836B_400020302A31__INCLUDED_)
#define AFX_ENTITYLISTDLG_H__D785F5F1_3FC1_11D1_836B_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EntityDlg.h : header file
//

class CCTADoc;
class YM_RecordSet;



/////////////////////////////////////////////////////////////////////////////
// CEntityDlg dialog


#define MAX_COLUMNS_ENTITY		10
#define REVERSE_ORDER_FLAG	0x80000000
#define WM_LOAD_TRANCHES	WM_USER + 21

class CEntityDlg : public YM_ETSLayoutDialog
{
// Construction
public:
	enum EntityAttr {
		ENTITY
	};
	CEntityDlg(CCTADoc* pCTADoc, CWnd* pParent = NULL);   // standard constructor
	DWORD    getCurrentItem () { return m_wCurrentItem;}

// Dialog Data
	//{{AFX_DATA(CEntityDlg)
	enum { IDD = IDD_ENTITY_LIST_DLG };
	CButton	m_CheckDowMon;
	CButton	m_CheckDowTue;
	CButton	m_CheckDowWed;
	CButton	m_CheckDowThu;
	CButton	m_CheckDowFri;
	CButton	m_CheckDowSat;
	CButton	m_CheckDowSun;
	CButton	m_CheckDowAll;
	YM_ShadeButton	m_btnOK;
	YM_ShadeButton	m_btnCancel;
//	YM_DateTimeEditCtrl	m_ctlStartDptDate;
//	YM_DateTimeEditCtrl	m_ctlEndDptDate;
	CDateTimeCtrl	m_ctlStartDptDate;
	CDateTimeCtrl	m_ctlEndDptDate;
	COleDateTime	m_tmStartDptDate;
	COleDateTime	m_tmEndDptDate;
	CListCtrl	m_EntityView;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEntityDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  static const char szEntityConst[];
  static const char szDOWConst[];
  static const char szBegDptDateConst[];
  static const char szEndDptDateConst[];
  static const char szUseEndDptDateConst[];
  static const char szLastFcstDateOffsetConst[];
  static const char szConstrainLastFcDateConst[];
  static const char szNoConstrainDptDateConst[];
  static const char szTrainAutoOnly[];
  static const char szAllJMX[];

  static int CALLBACK Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
  void SetColumnAttribute(int iIndex, LPARAM lParam);
  void BuildProfileKey(CString& szProfileKey);

  CCTADoc* m_pCTADoc;
  DWORD    m_wCurrentItem;
  unsigned long  m_lBegDptDate;         // Begin departure date
  unsigned long  m_lEndDptDate;         // End departure date
  unsigned long  m_lMinBegDptDate;   // Minimum acceptable departure date
  unsigned long  m_lMaxBegDptDate;  // Maximum acceptable departure date
  unsigned long  m_lMinEndDptDate;   // Minimum acceptable departure date
  unsigned long  m_lMaxEndDptDate;  // Maximum acceptable departure date
  bool           m_bAutoTrain;

	// Generated message map functions
	//{{AFX_MSG(CEntityDlg)
	afx_msg void OnDptDate();
	afx_msg void OnFcstDay1();
	afx_msg void OnFcstDay2();
	afx_msg void OnFcstDay3();
	afx_msg void OnFcstDay4();
	afx_msg void OnFcstDay5();
	afx_msg void OnFcstDay6();
	afx_msg void OnFcstDay7();
	afx_msg void OnCheckDow();
	afx_msg void OnCheckDowAll();
  afx_msg void OnUseEndDptDate();
  //afx_msg void OnConstrainLastAuthFcDate();
  //afx_msg void OnConstrainDptDate();
  afx_msg void OnStartDptDateChange(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnEndDptDateChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickEntityListview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCloseupDatetimepickerStart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCloseupDatetimepickerEnd(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  enum EditSel
  {
    eNoSel = 0,
    eBegDay,
    eBegMonth,
    eBegYear,
    eEndDay,
    eEndMonth,
    eEndYear
  } ;

  void InitializeEntityList();
  BOOL SetLastAuthFcstDate();
  void InitializeBatchDayRadioButtons();
  void InitializeDptDateRange();
  void ConstrainStartDptDate();
  void ConstrainEndDptDate();
  int DptdateVerification();
  void EditFieldsFromRWDate();
  void SetFcstDaySelectionButtons( BOOL bState );
  void SetDOWSelectionButtons( BOOL bState );
  void EnableDptDateEdit( BOOL bEnable );
  BOOL FindEntity(CString szListP, const char* szEntity);

  CSpinButtonCtrl * m_SpinButtonCtrl;
	long m_ViewAttrArray[ MAX_COLUMNS_ENTITY ];
/*
	typedef struct {
		YM_RecordSet* pRecordSet;
		LPARAM lParam;
	} ENTITY_SORTPARAM;
*/
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTITYLISTDLG_H__D785F5F1_3FC1_11D1_836B_400020302A31__INCLUDED_)
