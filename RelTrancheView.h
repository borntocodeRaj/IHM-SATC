#if !defined(AFX_RELTRANCHEVIEW_H__98E620C2_85BC_11D1_837D_400020302A31__INCLUDED_)
// RelTrancheView.h : header file
#define AFX_RELTRANCHEVIEW_H__98E620C2_85BC_11D1_837D_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CRelTrancheChart;
class CRelTrancheView;

// typedef for the array which contains pointers to related tranches domain records
typedef CTypedPtrArray<CPtrArray, YmIcRelatedDom*> CRelTran_CmptBktLevelArray;

// typedef for storing the Tranche info labels
typedef struct 
{
  long lTrancheNo;
  long lLegSeq;
  long lResHoldTotSum;  // sum of ResHoldTot by leg (for all cmpts)
  long lDptTime;
  CRelTran_CmptBktLevelArray aRelTrancheRecs;  // array of YmIcRelatedDom records for this tranche
} RelTran_LegLevel;

// typedef for the array which contains pointers to structure at leg level
typedef CTypedPtrArray<CPtrArray, RelTran_LegLevel*> CRelTran_TrancheLevelArray;

// typedef for storing the items which are inserted into the market selection pop-up menu
typedef struct 
{
	const char * szLegOrig;
	const char * szLegDest;
} RelTran_Mkt;

// typedef for the array which contains pointers to mkts (for selecting markets)
typedef CTypedPtrArray<CPtrArray, YmIcLegsDom*> CRelTran_LegArray;
typedef CTypedPtrArray<CPtrArray, RelTran_Mkt*> CRelTran_MktArray;

// typedef for storing the Tranche info labels
typedef struct 
{
  int iPointIdx;
  YmIcRelatedDom* pRelTranche;
  CChart2DTextArea* pChartTextArea;
} TrancheInfoLabels;

typedef CTypedPtrArray<CPtrArray, TrancheInfoLabels*> CTrancheInfoLabelArray;

/////////////////////////////////////////////////////////////////////////////
// CRelTrancheHelp dialog
class CRelTrancheHelp : public CCTAChartHelpDlg
{
// Construction
public:
  CRelTrancheHelp(CRelTrancheChart* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CRelTrancheHelp)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRelTrancheHelp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  CRelTrancheView* GetParentView() { return ((CRelTrancheView*)(m_pParent)->GetParentView()); }
  CRelTrancheChart* GetParentChart() { return ((CRelTrancheChart*)m_pParent); }
  void GetTitle1( CString& szTitle );
  void GetTitle2( CString& szTitle );
  BOOL IsRecetteDataSet ();
  CSize GetExtentLongestLabel( CPaintDC& dc );
  BOOL GetTextLabel( int iDataSet, CString& szTextLabel, int iCurSelectSet );
  BOOL GetTextLabelSphere( int iDataSet, CString& szTextLabel, int iCurSelectSet );//DM7978 KGH  remplace GetTextLabel
  int GetCTADataStyleIndex( int iDataSet, int iCurSelectSet );
  CString GetValue( int iDataSet, int iCurSelectSet, int iCurSelectPt  );
  // DM 7879 KGH reimplementation de la methode  de la classe CCTAChartHelpDlg
  virtual BOOL DrawThisLabel( int iDataSet, CPaintDC& dc, int iCurSelectSet, CPoint& pointDraw, int iLeftMargin, int iRightMargin,BOOL bWithCheckBox);

	// Generated message map functions
	//{{AFX_MSG(CRelTrancheHelp)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CRelTrancheChart window

class CRelTrancheView;

class CRelTrancheChart : public CCTAChart
{
// Construction
public:
  CRelTrancheChart();
  ~CRelTrancheChart();// DM 7978 KGH

// Attributes
public:

  void SetGraphProperties(enum CCTAChartData::GraphType eGraphType);
  // NPI - Ano 70148
  void SetGraphPropertiesGeneral(enum CCTAChartData::GraphType eGraphType);
  void SetGraphPropertiesAlleo(enum CCTAChartData::GraphType eGraphType);
  ///
  CCTAChartHelpDlg* CreateHelpDlg();
  void ContextualDisplay(CPoint point, BOOL bInRange);
  BOOL GetRotateLabelsFlag() { return m_bRotateLabels; }
  void SetRotateLabelsFlag( bool bRotateLabels ) { m_bRotateLabels = bRotateLabels; }

  double GetFlashLabelValue(XrtDataHandle& hXrtData, int iCurFlashSet, int iCurFlashPt, BOOL bAccumulate);
  XrtDataStyle * GetSphereDataStylesAt(int dataStyleIdx){return m_paSphereDataStyles[dataStyleIdx];} //DM7978 KGH retourner le style de la sphere a l'index demande

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRelTrancheChart)
	//}}AFX_VIRTUAL

// Implementation
public:
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CRelTrancheChart)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  BOOL m_bRotateLabels;  // do we rotate the x-axis labels ?
private:
	XrtDataStyle **m_paSphereDataStyles;// DM7978 KGH Style des spheres de prix
	int m_nSpherePrix; // NPI - Ano 70149
};


/////////////////////////////////////////////////////////////////////////////
// CRelTrancheView dialog

class CRelTrancheView : public CView, public YM_ObsView
{
friend class CRelTrancheChart;

#define IDC_RELTRANCHE_TABCTRL 104

#define REG_ATTACH_LABELS_TO_CAPACITY  "ATTACH_LABELS_TO_CAPACITY"
#define REG_DISPLAY_TRANCHE_NO  "DISPLAY_TRANCHE_NO"
#define REG_DISPLAY_TRAIN_NO  "DISPLAY_TRAIN_NO"
#define REG_DISPLAY_LEG_ORIG  "DISPLAY_LEG_ORIG"
#define REG_DISPLAY_SUPP_LEVEL  "DISPLAY_SUPP_LEVEL"
#define REG_DISPLAY_TRANCHE_DPT_TIME   "DISPLAY_TRANCHE_DPT_TIME"
#define REG_DISPLAY_LEG_DPT_TIME   "DISPLAY_LEG_DPT_TIME"
#define REG_DISPLAY_TGV_DATA   "DISPLAY_TGV_DATA"  // uniquely for TRN client

protected:
	CRelTrancheView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRelTrancheView)

// Attributes
public:
  int index_recette;

  CCTADoc* GetDocument();
  CRelTrancheChart* GetChart() { return &m_Chart; }
  CTabCtrl* GetTabCtrl() { return &m_ChartTabCtrl; }
  void SetGraphFont() { m_Chart.SetGraphFont(); }
  int GetMaxBucketCount() { return m_iNstLvlMaxCount; }
  CCmptBktLabelArray* GetCmptBktLabelArray() { return &m_paCmptBktLabels; }
  CTrancheInfoLabelArray* GetTrancheInfoLabelArray() { return &m_paTrancheInfoLabels; }
  int GetTrancheInfoLabelIdx( int iPointIdx );
  CString GetTabText(enum CCTAChartData::GraphType eGraphType);
  BOOL GetTRNJoinFlag() const { return m_bTRNJoinFlag; }
  void SetTRNJoinFlag(bool bTRNJoinFlag) { m_bTRNJoinFlag = bTRNJoinFlag; }
  int CalcRevenu(YmIcRelatedDom* pRelTranche, CString szCmpt); // renvoi la somme des revenus de l'espace physique
  int CalcResa(YmIcRelatedDom* pRelTranche, CString szCmpt); // renvoi la somme des résa de l'espace physique
  void ReCalcAvailSci(); // Si SCI, recalcule les dispo.
  int GetMaxSpheresPrixLegende() { return m_nbSpheresPrixLegende; } // DM 7978 KGH //retourne le nb max des spheres de prix pour la fenetre legende
 

  BOOL GetAltFlag() const { return ((CChildFrame*)GetParentFrame())->GetAltFlag(); }

  enum GraphScaleIndex
  {
    eResHold = 0,
    eDmdDptFcst
  } ;

  enum Graph2Index
  {
    eEmptySpace,
    eCapacity,
    eNumberDataSets2
  } ;

// Operations
public:
 	BOOL CanCloseFrame() { return TRUE; }
	void FrameSizeChange (UINT nType, int cx, int cy);
  void WriteViewConfig();
  void RestoreViewConfig();
  char* FormatVarTranInfoText( char* szLabelText, YmIcRelatedDom* pRelTranche );
/*
  void ScaleGraph(enum GraphScaleIndex eGraphScale);
  void SetCurrentScaling (enum GraphScaleIndex eScale) { m_eScaleStatus = eScale; }
  enum GraphScaleIndex GetCurrentScaling() { return m_eScaleStatus; }
*/
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRelTrancheView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:

	virtual ~CRelTrancheView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

    // the tab control
    CTabCtrl	m_ChartTabCtrl; // the tab control
    CString	_sLastTabUsed;


  int m_iCurTabSelection;  // index of the currently selected tab

  // array containing legs in sequence order
  CRelTran_MktArray  m_paMktList;

  // The graph
  CRelTrancheChart  m_Chart;

  // array containing tranche level pointers to arrays of cmpt/bkt level records
  CRelTran_TrancheLevelArray  m_paRelTrancheList;
  int m_iBarCount;

  CTrancheInfoLabelArray m_paTrancheInfoLabels;
  CCmptBktLabelArray m_paCmptBktLabels;

  CString m_szMktOrig;
  CString m_szMktDest;
  int m_iDptTime;
  RelTran_Mkt m_pMkt;  // the market (leg) for which the analyst is currently studying

  BOOL m_bAttachLabelsToCapacity;  // do we attach the labels to the capacity bar ?
  BOOL m_bDisplayTrancheNo;
  BOOL m_bDisplayTrainNo;
  BOOL m_bDisplayOrigDest;
  BOOL m_bDisplaySuppLevel;
  BOOL m_bDisplayTrancheDptTime;
  BOOL m_bDisplayLegDptTime;
  BOOL m_bTabResized;
  BOOL m_bResHoldScale;
  BOOL m_bMarketSelected;
  BOOL m_bTRNJoinFlag;

  int m_iNstLvlMaxCount;  // Max of the number of nest levels (max bucket count)
  int m_nbSpheresPrixMax;   //DM7978 - KGH -  nb max des spheres de prix CC/SC
  int m_nbSpheresPrixLegende;  	//DM7978 - KGH - nb max des spheres de prix CC/SC pour la fenetre legende
  bool m_sphereNonTrouvee; // DM7978 - KGH

	CCriticalSection* m_pCriticalSection;

//  enum GraphScaleIndex m_eScaleStatus;  // graph is currently scaled

	// Generated message map functions
protected:
	//{{AFX_MSG(CRelTrancheView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow (BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChartSelchange(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	//}}AFX_MSG
  afx_msg void OnScaleMax();
  afx_msg void OnUpdateScaleMax(CCmdUI* pCmdUI);
  afx_msg void OnScaleResHold();
  afx_msg void OnUpdateScaleResHold(CCmdUI* pCmdUI);
  afx_msg void OnGraphMktSelect();
  afx_msg void OnUpdateGraphMktSelect( CCmdUI* pCmdUI );
  afx_msg void OnGraphDisplayFlashLabels();
  afx_msg void OnUpdateGraphDisplayFlashLabels( CCmdUI* pCmdUI );
  afx_msg void OnGraphRotateLabels();
  afx_msg void OnUpdateGraphRotateLabels( CCmdUI* pCmdUI );
  afx_msg void OnGraphAttachLabelsToCapacity();
  afx_msg void OnUpdateGraphAttachLabelsToCapacity( CCmdUI* pCmdUI );
  afx_msg void OnGraphTRNJoin();
  afx_msg void OnUpdateGraphTRNJoin( CCmdUI* pCmdUI );
  afx_msg void OnGraphTrancheNo();
  afx_msg void OnUpdateGraphTrancheNo( CCmdUI* pCmdUI );
  afx_msg void OnGraphTrainNo();
  afx_msg void OnUpdateGraphTrainNo( CCmdUI* pCmdUI );
  afx_msg void OnGraphOrigDest();
  afx_msg void OnUpdateGraphOrigDest( CCmdUI* pCmdUI );
  afx_msg void OnGraphSuppLevel();
  afx_msg void OnUpdateGraphSuppLevel( CCmdUI* pCmdUI );
  afx_msg void OnGraphTrancheDptTime();
  afx_msg void OnUpdateGraphTrancheDptTime( CCmdUI* pCmdUI );
  afx_msg void OnGraphLegDptTime();
  afx_msg void OnUpdateGraphLegDptTime( CCmdUI* pCmdUI );
  afx_msg void OnPrint();
  afx_msg void OnUpdatePrint( CCmdUI* pCmdUI );
  afx_msg void OnMarketSelection(unsigned int i);
	DECLARE_MESSAGE_MAP()

  void OnGraphUpdateLabels( BOOL& bFlag );

  void ResizeChart();

  BOOL RestructureData();
  void BuildGraph(enum CCTAChartData::GraphType eGraphType);
  // NPI - Ano 70148
  void BuildGraphGeneral(enum CCTAChartData::GraphType eGraphType);
  void BuildGraphAlleo(enum CCTAChartData::GraphType eGraphType);
  ///
  void FormatBarLabels( enum CCTAChartData::GraphType eGraphType = CCTAChartData::eAvail );
  void FormatXTitleText( CString& szTitleText );

  void ShowGraph();
  void ClearGraph();

  void BuildTabs();
  void ClearTabs();
  void ClearTrancheInfoLabelArray();
  void ClearCmptBktLabelArray();
  void ClearRelTrancheList();
};

/////////////////////////////////////////////////////////////////////////////
// CRelTrancheAltView view

class CRelTrancheAltView : public CRelTrancheView
{
friend class CRelTrancheView;
	CRelTrancheAltView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRelTrancheAltView)

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRelTrancheAltView)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RELTRANCHEVIEW_H__98E620C2_85BC_11D1_837D_400020302A31__INCLUDED_)
