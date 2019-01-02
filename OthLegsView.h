#if !defined(AFX_OTHLEGSVIEW_H__A923F4D2_59AA_11D1_8376_400020302A31__INCLUDED_)
#define AFX_OTHLEGSVIEW_H__A923F4D2_59AA_11D1_8376_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OthLegsView.h : header file
//
class COthLegsChart;
class COthLegsView;
class CCTAChartHelpDlg;
class YmIcOthLegsDom;

typedef struct 
{
  char  szLabel[40];
  double dValue;
} OthLegsCapStruct;
 
typedef CTypedPtrArray<CPtrArray,OthLegsCapStruct*> CCapValuesArray;

// The Array below contains the YmIcOthLegsDom
typedef CTypedPtrArray<CPtrArray,YmIcOthLegsDom*> CHelpDlgRecArray;
typedef CTypedPtrArray<CPtrArray, YmIcLegsDom*> CLegPtrArray;

/////////////////////////////////////////////////////////////////////////////
// COthLegsHelp dialog
class COthLegsHelp : public CCTAChartHelpDlg
{
// Construction
public:
  COthLegsHelp(COthLegsChart* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(COthLegsHelp)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COthLegsHelp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  COthLegsView* GetParentView() { return ((COthLegsView*)(m_pParent)->GetParentView()); }
  COthLegsChart* GetParentChart() { return ((COthLegsChart*)m_pParent); }

  void GetTitle1( CString& szTitle );
  void GetTitle2( CString& szTitle );

  BOOL IsRecetteDataSet ();
  CSize GetExtentLongestLabel( CPaintDC& dc );
  CString GetValue( int iDataSet, int iCurSelectSet, int iCurSelectPt  );
  CString GetValueRecette( );
  BOOL GetTextLabel( int iDataSet, CString& szTextLabel, int iCurSelectSet );
  BOOL GetTextLabelSphere( int iDataSet, CString& szTextLabel, int iCurSelectSet );//DM7978 KGH  remplace GetTextLabel
  int GetCTADataStyleIndex( int iDataSet, int iCurSelectSet );
  // DM 7879 KGH reimplementation de la methode  de la classe CCTAChartHelpDlg
  virtual BOOL DrawThisLabel( int iDataSet, CPaintDC& dc, int iCurSelectSet, CPoint& pointDraw, int iLeftMargin, int iRightMargin,BOOL bWithCheckBox);


	// Generated message map functions
	//{{AFX_MSG(COthLegsHelp)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// COthLegsChart window

class COthLegsView;

class COthLegsChart : public CCTAChart
{
// Construction
public:
  COthLegsChart();
  ~COthLegsChart();// DM7978 KGH

// Attributes
public:

  void SetGraphProperties(enum CCTAChartData::GraphType eGraphType);
  CCTAChartHelpDlg* CreateHelpDlg();
  void ContextualDisplay(CPoint point, BOOL bInRange);
  double GetFlashLabelValue(XrtDataHandle& hXrtData, int iCurFlashSet, int iCurFlashPt, BOOL bAccumulate);
  bool GetRotateLabelsFlag() { return m_bRotateLabels; }
  void SetRotateLabelsFlag( bool bRotateLabels ) { m_bRotateLabels = bRotateLabels; }

// Operations
public:
  void ScaleGraph(enum GraphScaleIndex eGraphScale );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COthLegsChart)
	//}}AFX_VIRTUAL

// Implementation
public:
  CHelpDlgRecArray* GetHelpDlgCmptArray() { return &m_apHelpDlgCmptRecs; }
  CStringArray* GetHelpDlgLblArray() { return &m_apHelpDlgLbls; }
  CCapValuesArray* GetCapStructArray() { return &m_apCapValues; }
  XrtDataStyle * GetSphereDataStylesAt(int dataStyleIdx){return m_paSphereDataStyles[dataStyleIdx];} //DM7978 KGH retourner le style de la sphere a l'index demande

	// Generated message map functions
protected:
  //{{AFX_MSG(COthLegsChart)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  COthLegsView* GetParentView() { return ((COthLegsView*)m_pParentView); }

  bool m_bRotateLabels;  // do we rotate the x-axis labels ?

  // Array of domain records used by the chart help dialog
  CHelpDlgRecArray m_apHelpDlgCmptRecs;
  CStringArray m_apHelpDlgLbls;
  CCapValuesArray m_apCapValues;
  
private:
	XrtDataStyle **m_paSphereDataStyles;// DM7978 KGH Style des spheres de prix
	int m_nSpherePrix; // NPI - Ano 70149
};

/////////////////////////////////////////////////////////////////////////////
// COthLegsView view

class COthLegsView : public CView, public YM_ObsView
{
friend class COthLegsChart;

#define IDC_OTHLEGS_TABCTRL 101

protected:
	COthLegsView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(COthLegsView)

// Attributes
public:
	CCTADoc* GetDocument();
  void SetGraphFont() { m_Chart.SetGraphFont(); }
  void UpdateAuthValue( const char* szLegOrig, const char* szCmpt, const long lNestLevel, long lValue );
  CTabCtrl* GetTabCtrl() { return &m_ChartTabCtrl; }
  CString GetTabText(enum CCTAChartData::GraphType eGraphType);
  CCmptBktLabelArray* GetCmptBktLabelArray() { return &m_paCmptBktLabels; }
  void ClearCmptBktLabelArray();
  
  int GetMaxSpheresPrixLegende() { return m_nbSpheresPrixLegende; } // DM 7978 KGH //retourne le nb max des spheres de prix pour la fenetre legende
  
  int CalcRevenu(YmIcOthLegsDom* pOthLegsP);

  BOOL GetAltFlag() const { return ((CChildFrame*)GetParentFrame())->GetAltFlag(); }

// Operations
public:

  // Enumeration type for indexing of the graph lines
  int index_recette;

  int eAuthDptFcst;
  int eDmdDptFcst;
  int eResHoldTot;
  int eNumberDataSets;

  int eEmptySpace1;
  int eCapacity;
  int eEmptySpace2;
  int eCapPlusUnres;
  int eEmptySpace3;
  int eCapPlusStdgPlusUnres;
  int eNumberDataSets2;

 	BOOL CanCloseFrame() { return TRUE; };
	void FrameSizeChange (UINT nType, int cx, int cy);
  void WriteViewConfig();
  void RestoreViewConfig();

  int GetMaxBucketCount() { return m_iNstLvlMaxCount; };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COthLegsView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:

	virtual ~COthLegsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

  // the tab control
  CTabCtrl m_ChartTabCtrl; // the tab control
  CString   _sLastTabUsed;

  int m_iCurTabSelection;  // index of the currently selected tab

  BOOL m_bTabResized;

  CLegPtrArray m_apLegs;  // array containing legs to display (selected by user)

  CCmptBktLabelArray m_paCmptBktLabels;

  // The graph
  COthLegsChart  m_Chart;

  // The data
  CChart2DData* m_pCmptChartData;
  CChart2DData* m_pCmptChartData2;

  int m_iNstLvlMaxCount;  // Max of the number of nest levels (max bucket count)
  int m_nbSpheresPrixMax;   //DM7978 - KGH -  nb max des spheres de prix CC/SC
  int m_nbSpheresPrixLegende;  	//DM7978 - KGH - nb max des spheres de prix CC/SC pour la fenetre legende

	// Generated message map functions
protected:
	//{{AFX_MSG(COthLegsView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow (BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChartSelchange(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSelectLegs();
  afx_msg void OnUpdateSelectLegs( CCmdUI* pCmdUI );
	//}}AFX_MSG
  afx_msg void OnScaleMax();
  afx_msg void OnUpdateScaleMax(CCmdUI* pCmdUI);
  afx_msg void OnScaleAuthorizations();
  afx_msg void OnUpdateScaleAuthorizations(CCmdUI* pCmdUI);
  afx_msg void OnGraphDisplayFlashLabels();
  afx_msg void OnUpdateGraphDisplayFlashLabels( CCmdUI* pCmdUI );
  afx_msg void OnGraphRotateLabels();
  afx_msg void OnUpdateGraphRotateLabels( CCmdUI* pCmdUI );
  afx_msg void OnPrint();
  afx_msg void OnUpdatePrint( CCmdUI* pCmdUI );
	DECLARE_MESSAGE_MAP()

  BOOL IsLegSelected( YmIcOthLegsDom* pOthLegs );

  void ResizeChart();
  void BuildGraph(enum CCTAChartData::GraphType eGraphType);
  // NPI - Ano 70148
  void BuildGraphGeneral(enum CCTAChartData::GraphType eGraphType);
  void BuildGraphAlleo(enum CCTAChartData::GraphType eGraphType);
  ///
  void ShowGraph();
  void ClearGraph();
  void BuildTabs();
  void ClearTabs();
  void GetCapValues(
                                 YmIcOthLegsDom* pOthLegs, 
                                 long& lReservableCap, 
                                 long& lStandingCap, 
                                 long& lUnreservableCap);
  int CalcAvail(YmIcOthLegsDom* pOthLegsP);
  int CalcAvailSci(YmIcOthLegsDom* pOthLegsP);
  void ReCalcAvail();
  void ReCalcAvailSci();
};

/////////////////////////////////////////////////////////////////////////////
// COthLegsAltView view

class COthLegsAltView : public COthLegsView
{
friend class COthLegsView;
	COthLegsAltView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(COthLegsAltView)

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COthLegsAltView)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHLEGSVIEW_H__A923F4D2_59AA_11D1_8376_400020302A31__INCLUDED_)
