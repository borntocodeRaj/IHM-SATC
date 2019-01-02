#if !defined(AFX_HISTDPTVIEW_H__4A1187C1_5520_11D1_8374_400020302A31__INCLUDED_)
#define AFX_HISTDPTVIEW_H__4A1187C1_5520_11D1_8374_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// HistDptView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHistDptHelp dialog
class CHistDptChart;
class CHistDptView;

class CHistDptHelp : public CCTAChartHelpDlg
{
// Construction
public:
  CHistDptHelp(CHistDptChart* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CHistDptHelp)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistDptHelp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  CHistDptView* GetParentView() { return ((CHistDptView*)(m_pParent)->GetParentView()); }
  CHistDptChart* GetParentChart() { return ((CHistDptChart*)m_pParent); }

  void GetTitle1( CString& szTitle );
  void GetTitle2( CString& szTitle );
  BOOL IsRecetteDataSet ();
  CSize GetExtentLongestLabel( CPaintDC& dc );
  BOOL GetTextLabel( int iDataSet, CString& szTextLabel, int iCurSelectSet );
  int GetCTADataStyleIndex( int iDataSet, int iCurSelectSet );

	// Generated message map functions
	//{{AFX_MSG(CHistDptHelp)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CHistDptChart window

class CHistDptView;

class CHistDptChart : public CCTAChart
{
// Construction
public:
  CHistDptChart();

// Attributes
public:

  void SetGraphProperties(enum CCTAChartData::GraphType eGraphType);
  void SetGraphPropertiesRecette();
  CCTAChartHelpDlg* CreateHelpDlg();
  void ContextualDisplay(CPoint point, BOOL bInRange);
  bool GetRotateLabelsFlag() { return m_bRotateLabels; }
  void SetRotateLabelsFlag( bool bRotateLabels ) { m_bRotateLabels = bRotateLabels; }
  int GetLabelInterval() { return m_iLabelInterval; }
  void SetLabelInterval( int iLabelInterval ) { m_iLabelInterval = iLabelInterval; }

// Operations
public:
  void ScaleGraph(enum GraphScaleIndex eGraphScale );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistDptChart)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:

  CHistDptView* GetParentView() { return ((CHistDptView*)m_pParentView); }

  bool m_bRotateLabels;  // do we rotate the x-axis labels ?
  int m_iLabelInterval;  // at what interval do we display the x-axis labels ?

	//{{AFX_MSG(CHistDptChart)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// Stockage des données resa de l'année précédente
class CYavData
{
  public:
	CYavData (unsigned long date, const char* cmpt, long resa, long auth, long rev);
	void AddRevenu (long plusrev) { m_rev += plusrev; }
	void AddResa (long plusresa) { m_resa += plusresa; }

	unsigned long m_date;
	char		  m_cmpt;
	long		  m_resa;
	long		  m_auth;
	long		  m_rev;
};

class CYavArray
{
  public:
	CYavArray (BOOL bAlt);
	~CYavArray();

	// date et cmpt corresponde au point du graphique pour lequel on veut les données de l'année précédente.
	CYavData* GetPrevYearData (unsigned long date, char cmpt);

  private:
    CObArray m_DataArray;
};

/////////////////////////////////////////////////////////////////////////////
// CHistDptView view

class CHistDptView : public CView, public YM_ObsView
{
#define IDC_HISTDPT_TABCTRL 100

protected:
	CHistDptView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CHistDptView)

// Attributes
public:
	CCTADoc* GetDocument();
  CTabCtrl* GetTabCtrl() { return &m_ChartTabCtrl; };
  CHistDptChart* GetChart() { return &m_Chart; }
  void SetGraphFont() { m_Chart.SetGraphFont(); }

  BOOL GetAltFlag() const { return ((CChildFrame*)GetParentFrame())->GetAltFlag(); }
  // renvoi la date de l'année précédente ou 0. bResa indique si on demande pour les resa ou les recettes
  unsigned long GetYavDate (unsigned long date, char cmpt);
  

// Operations
public:

  // Enumeration type for indexing of the graph lines
  int eAuthDpt;
  int eCapacity;
  int eDmdDptFcst;
  int eResHoldDptTot;
  int eTrafficRes;
  int eYavResHold;
  int eYavAuthDpt;
  int eNumberDataSets;
  int eRecette;
  int eYavRecette;
  int eNumberRecetteDataSets;

  BOOL CanCloseFrame() { return TRUE; };
  void FrameSizeChange (UINT nType, int cx, int cy);
  void WriteViewConfig();
  void RestoreViewConfig();
 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistDptView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHistDptView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

    // the tab control
    CTabCtrl	m_ChartTabCtrl; // the tab control
    CString	_sLastTabUsed;
    BOOL	m_bTabResized;
    int	_nSubViewID;

    // The graph
    CHistDptChart  m_Chart;

//  enum GraphIndex m_eScaleStatus;  // graph is currently scaled

	// Generated message map functions
protected:
	//{{AFX_MSG(CHistDptView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow (BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChartSelchange(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	//}}AFX_MSG
  afx_msg void OnScaleMax();
  afx_msg void OnUpdateScaleMax(CCmdUI* pCmdUI);
  afx_msg void OnScaleAuth();
  afx_msg void OnUpdateScaleAuth(CCmdUI* pCmdUI);
  afx_msg void OnScaleCap();
  afx_msg void OnUpdateScaleCap(CCmdUI* pCmdUI);
  afx_msg void OnGraphDisplayFlashLabels();
  afx_msg void OnUpdateGraphDisplayFlashLabels( CCmdUI* pCmdUI );
  afx_msg void OnGraphRotateLabels();
  afx_msg void OnUpdateGraphRotateLabels( CCmdUI* pCmdUI );
  afx_msg void OnGraphLabelInterval1();
  afx_msg void OnUpdateGraphLabelInterval1( CCmdUI* pCmdUI );
  afx_msg void OnGraphLabelInterval5();
  afx_msg void OnUpdateGraphLabelInterval5( CCmdUI* pCmdUI );
  afx_msg void OnGraphLabelInterval10();
  afx_msg void OnUpdateGraphLabelInterval10( CCmdUI* pCmdUI );
	DECLARE_MESSAGE_MAP()

  void ResizeChart();

  void BuildXLabels();
  void BuildGraphRecettes();
  void BuildGraph();
  void GraphLabelInterval (int itv);
  void OnScale (enum CCTAChart::GraphScaleIndex eGraphScale);

  void ShowGraph(CChart2DData* pChartDataObj);
//  void ShowGraph(CChart2DData* pChartDataObj, char cCmpt);

  void ClearGraph();
  CHistDptView* getOtherView();

  void BuildTabs();
  void ClearTabs();

  private:
  BOOL m_bHistDptSetValid;		 // indique validite des données resa
  BOOL m_bHistDptYavSetValid;	 // indique validite des données resa de l'an passé


  CYavArray* m_pYavData;
};

/////////////////////////////////////////////////////////////////////////////
// CHistDptAltView view

class CHistDptAltView : public CHistDptView
{
friend class CHistDptView;
	CHistDptAltView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CHistDptAltView)

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistDptAltView)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

};

#ifndef _DEBUG  // debug version in perfView.cpp
inline CCTADoc* CHistDptView::GetDocument()
   { return (CCTADoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTDPTVIEW_H__4A1187C1_5520_11D1_8374_400020302A31__INCLUDED_)
