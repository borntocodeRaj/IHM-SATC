#if !defined(AFX_COMPTAGES_H__109491F9_C404_11D6_A92E_0002A50F053B__INCLUDED_)
#define AFX_COMPTAGES_H__109491F9_C404_11D6_A92E_0002A50F053B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComptagesView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComptagesView view

class CRRDHistView;

class CComptagesView : public YM_ListView
{
protected:
	CComptagesView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CComptagesView)

  enum ColumnId
  {
    FIRST_COLUMN,
    FAMILY_NO,
    FAMILY_FLAG,
    DPT_DATE,
    LEG_ORIG,
    LEG_DEST,
    TRAIN_NO,
    CMPT,
    TYPE_JOUR,
    TYPE_CMPT,
    NB_OCCUPEES,
	OCCUP_PCT,
    NB_OFFERT,
    NB_OBS,
    MOY_OCC_MOIS,
    ECT_OCC_MOIS,
    MOY_OFF_MOIS,
    ECT_OFF_MOIS,
	ENTITY_NATURE_ID // NPI - DM7870 : Dates de référence
  };

  CRRDHistView* GetRRDHistView();
	void FrameSizeChange (UINT nType, int cx, int cy);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComptagesView)
	protected:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Attributes
public:

// Operations
public:

  CString GetColumnTitle(int iResId);	// virtual de YM_ListView
  CUIntArray* GetSortOrderArray() { return &m_aiSortOrder; }	// virtual de YM_ListView
  CColAttrArray* GetColAttributeArray() { return &m_apColAttr; }	// virtual de YM_ListView
  FUNC_SORT* GetSortFuncArray() { return m_apSortFunctions; }	// virtual de YM_ListView

  void WriteSettings();
  void RestoreSettings();

	void LoadSelectedItem() {}	//virtual de YM_ListView
	BOOL** GetSortContextPointer() {return &m_pbSortTimeContext; }
	BOOL DrawHeaderImage() {return TRUE;}  // do we draw header images or simply text ?
	static BOOL* m_pbSortTimeContext;

  void SetFamilyNo(int iFamilyNo) { m_iFamilyNo = iFamilyNo; }
  void DptDateRef(int iDptDateRef) { m_iDptDateRef = iDptDateRef; }

// Implementation
protected:
	virtual ~CComptagesView();
	CFont* GetStdFont(CDC* pDC);		// virtual de YM_ListView
	void CreateItemImageList();			// virtual de YM_ListView
	CString GetDefaultSortOrder();		// virtual de YM_ListView
	YM_RecordSet* GetRecordSet() const {return NULL;}       // virtual de YM_ListView
	void DrawImageList(LPDRAWITEMSTRUCT lpDrawItemStruct, 
                                 UINT uiFlags, 
                                 COLORREF clrTextSave, 
                                 COLORREF clrBkSave, 
                                 COLORREF clrImage) {return ; } 
  int GetPropertyLevel(LPARAM lParam); // virtual de YM_ListView
  CString GetViewTitle();					// virtual de YM_ListView
  CString GetDispInfo(int iSubItem, LPARAM lParam, int iItem);	// virtual de YM_ListView
  void InsertColumns();					// virtual de YM_ListView
  BOOL IsLoadedItem(LPARAM lParam) { return FALSE; }	// virtual de YM_ListView
  void EnableToolTips() {GetListCtrl().EnableToolTips( TRUE );}	// virtual de YM_ListView
  CString GetToolTipText(LVITEM* pItem) const;		// virtual de YM_ListView
  BOOL SetPropertyLevelColor(int nLevel, COLORREF clrProperty );

  void BuildData();
  
  static int SortDummy(LPARAM lParam1, LPARAM lParam2);
  static int SortFamilyNo(LPARAM lParam1, LPARAM lParam2);
  static int SortFamilyFlag(LPARAM lParam1, LPARAM lParam2); 
  static int SortDptDate(LPARAM lParam1, LPARAM lParam2);
  static int SortLegOrig(LPARAM lParam1, LPARAM lParam2);
  static int SortLegDest(LPARAM lParam1, LPARAM lParam2);
  static int SortTrainNo(LPARAM lParam1, LPARAM lParam2);
  static int SortCmpt(LPARAM lParam1, LPARAM lParam2);
  static int SortTypeJour(LPARAM lParam1, LPARAM lParam2);	
  static int SortTypeCmpt(LPARAM lParam1, LPARAM lParam2);	
  static int SortNbOccupees(LPARAM lParam1, LPARAM lParam2);
  static int SortOccupPct(LPARAM lParam1, LPARAM lParam2);
  static int SortNbOffert(LPARAM lParam1, LPARAM lParam2);
  static int SortNbObs(LPARAM lParam1, LPARAM lParam2); 
  static int SortMoyOccMois(LPARAM lParam1, LPARAM lParam2);	 
  static int SortEctOccMois(LPARAM lParam1, LPARAM lParam2);
  static int SortMoyOffMois(LPARAM lParam1, LPARAM lParam2);
  static int SortEctOffMois(LPARAM lParam1, LPARAM lParam2);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CComptagesView)
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  CColAttrArray m_apColAttr;
  CUIntArray m_aiSortOrder;							// required by YM_ListView
  FUNC_SORT m_apSortFunctions[MAX_COLUMNS];			// required by YM_ListView

  int m_iFamilyNo;
  int m_iDptDateRef;

  BOOL m_bFandnotB;	// cmpt F présent et pas de B.
};
  
/////////////////////////////////////////////////////////////////////////////
// CComptagesAltView view

class CComptagesAltView : public CComptagesView
{
friend class CComptagesView;
	CComptagesAltView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CComptagesAltView)

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComptagesAltView)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPTAGES_H__109491F9_C404_11D6_A92E_0002A50F053B__INCLUDED_)
