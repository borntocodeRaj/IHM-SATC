// RecDHistView.h : header file
#if !defined(AFX_RecDHISTORY_H__5627CE71_5B48_11D1_8376_400020302A31__INCLUDED_)
#define AFX_RecDHISTORY_H__5627CE71_5B48_11D1_8376_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define RDISPLAYALL	"11111111"
#define RDISPLAYMEC	"11110001"
#define RDISPLAYECH	"11101111"
#define RDISPLAYHIS	"01101111"


class CRecDHistChart;
class CRecDHistView;

/////////////////////////////////////////////////////////////////////////////
// CHistoData
class CRecHistoData
{
public:
  CRecHistoData(BOOL bAlt);
  ~CRecHistoData();
  int GetNbDate() { return m_MaxHistoRef; };
  double GetRevenu(int histoIdx, char cmpt, long RecD, int bucket);
  double GetSumRevenu(int histoIdx, char cmpt, long RecD);
  bool GetHoiTra (int histoIdx, char cmpt, long rrd);
  bool IsRrd (long rrd);
  CString GetTitle (int histoIdx);
  RWDate  GetDateHisto (int histoIdx);

  // DM 4965.3 nombre de bucket pour le compartiment cmpt de la date historique d'indice histoIdx
  int GetNbBucket(int histoIdx, char cmpt);  // renvoi -1 si pas d'historique d'indice histoIdx
 
  CMapWordToPtr m_HistoMap; // Tableau de 1, 2 ou 3 CRecHistodate
  int m_MaxHistoRef;
  int m_max_RecD;
  int m_min_RecD;
  int m_MaxBkt1[26]; // tableau indexé de 'A'-'A' à 'Z'-'A'
  int m_MaxBkt2[26]; // tableau indexé de 'A'-'A' à 'Z'-'A'
  int m_MaxBkt3[26]; // tableau indexé de 'A'-'A' à 'Z'-'A'
  BOOL bSci;
};

/////////////////////////////////////////////////////////////////////////////
// CRecDHistHelp dialog
class CRecDHistHelp : public CCTAChartHelpDlg
{
// Construction
public:
  CRecDHistHelp(CRecDHistChart* pParent = NULL);
  BOOL PrevisionDispo;
  BOOL needRedraw;

// Dialog Data
	//{{AFX_DATA(CRecDHistHelp)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecDHistHelp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  void GetTitle1( CString& szTitle );
  void GetTitle2( CString& szTitle );
  CSize GetExtentLongestLabel( CPaintDC& dc );
  BOOL GetTextLabel( int iDataSet, CString& szTextLabel, int iCurSelectSet );
  int GetCTADataStyleIndex( int iDataSet, int iCurSelectSet );
  CString GetValue( int iDataSet, int iCurSelectSet, int iCurSelectPt );
  int GetCheckBox( int iDataSet, int iCurSelectSet, int iCurSelectPt );
  void ClickDynaButton (int nId, int checked);

	// Generated message map functions
	//{{AFX_MSG(CRecDHistHelp)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

class CRecDataGraph
{
  public:
  CRecDataGraph();

  YmIcCmptEvolDom* pCmptEvol;		// Je met les deux pointeurs pour n'avoir d'une structure de données
  YmIcBktEvolDom*  pBktEvol;

  YmIcPrevisionDom* pCmptPrevision;

  double	Revenu1;					// -1 valeur dummy.
  double	Revenu2;
  double	Revenu3;
  bool      HoiTra1;
  bool      HoiTra2;
  bool      HoiTra3;
  double	RevTempsReel;
  int	rrd_index;

  void AddPrevision (double prevoi);  // Ajoute une valeur de prévision
  double GetPrevision();

private:
  double	Prevision;
};

/////////////////////////////////////////////////////////////////////////////
// CRecDHistChart dialog
class CRecDHistChart : public CCTAChart
{
// Construction
public:
  CRecDHistChart::CRecDHistChart();

// Attributes
public:
	BOOL GetRedraw()
	{ 
		if (this->m_pHelpDlg != NULL)
		{
			
			return (((CRecDHistHelp*)this->m_pHelpDlg)->needRedraw);
		}

		return 0;
	}
	void RedrawHelp()
	{
		((CRecDHistHelp*)this->m_pHelpDlg)->needRedraw = 0;
		((CRecDHistHelp*)this->m_pHelpDlg)->Invalidate();
	}
	
  void SetGraphProperties(enum CCTAChartData::GraphType eGraphType);
  void ChangeHistoLabel(enum CCTAChartData::GraphType eGraphType,
					   CString szHisto1, CString szHisto2, CString szHisto3);
  CCTAChartHelpDlg* CreateHelpDlg();
  void ContextualDisplay(CPoint point, BOOL bInRange);
  bool GetRotateLabelsFlag() { return m_bRotateLabels; }
  void SetRotateLabelsFlag( bool bRotateLabels ) { m_bRotateLabels = bRotateLabels; }
  void SetPrevisionLabel(CString _previsionLabel) { m_PrevisionLabel = _previsionLabel; }


  CRecDHistView* GetParentView() { return ((CRecDHistView*)m_pParentView); }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecDHistChart)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	CString m_PrevisionLabel;
    bool m_bRotateLabels;  // do we rotate the x-axis labels ?
	int m_graphtype; // Type de graph
	//{{AFX_MSG(CRecDHistChart)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CRecDHistView
class CRecDHistView : public CView, public YM_ObsView
{
#define IDC_RecDHIST_TABCTRL 102

protected:
	CRecDHistView(BOOL bAlt = FALSE);           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRecDHistView)

// Attributes
public:
	CCTADoc* GetDocument();
  CRecDHistChart* GetCmptChart() { return &m_CmptChart; }
  CRecDHistChart* GetBktChart() { return &m_BktChart; }
  CTabCtrl* GetTabCtrl() { return &m_ChartTabCtrl; };

  CRecDHistChart* GetCurSelChart() { return m_pCurSelChart; } // chart currently selected by user
  CRecDHistChart* GetNotSelChart() { return m_pNotSelChart; } // chart currently NOT selected by user
  CCTAChartData* GetDataFromTabItem();  // get CTA data object from the tab control item
  CString GetTextFromTabItem(int iTabIdx);  // get CTA data object from the tab control item
  void UpdateHelpDialog();
  void SetGraphFont()
  {
    m_CmptChart.SetGraphFont();
    m_BktChart.SetGraphFont();
  }
  void BuildComptagesArray();
  void BuildComptagesHistArray();
  void GetDayTitle(CString& szDayTitle);
  void ChangeRecDDisplay(BOOL bOnF5);
  void RevMasqueCourbe (int graphtype, int set, BOOL b2ndSet);
  void RefreshAll(BOOL both = TRUE);
  BOOL GetAltFlag() const
  {
	return ((CChildFrame*)GetParentFrame())->GetAltFlag();
  }
  int debjxHoiTra, finjxHoiTra;

// Operations
public:
	bool _AltFlag;
    BOOL CanCloseFrame() { return TRUE; };
    void FrameSizeChange (UINT nType, int cx, int cy);
    void WriteViewConfig();
    void RestoreViewConfig();
    void    enablePaneDate( BOOL bEnable=TRUE );
    void    setPaneDateText( int iPane, LPCTSTR pszText );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecDHistView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

public:
  int dernierJX;
  
  // enumeration type for indexing of the Compartment graph lines
  int eCmptResHoldTot;
  int eCmptHoiTra;
  int eCmptHoiTraIdv;
  int eCmptResHoldIdv;
  int eCmptResHoldAvg;
  int eCmptHisto1ResHoldTot;
  int eCmptHisto2ResHoldTot;
  int eCmptHisto3ResHoldTot;
  int eCmptHisto1HoiTra;
  int eCmptHisto2HoiTra;
  int eCmptHisto3HoiTra;
  int eCmptPrevision;
  int eCmptNumDataSets;

  // enumeration type for graphing the standard deviation "band"
  enum CmptGraphStdvIndex
  {
    eCmptResHoldStdvHigh = 0,
    eCmptResHoldStdvLow,
    eCmptStdvNumDataSets
  } ;

  // enumeration type for indexing of the Bucket graph lines
  int eBktResHoldTot;
  int eBktHoiTra;
  int eBktHoiTraIdv;
  int eBktResHoldIdv;
  int eBktResHoldAvg;
  int eBktHisto3ResHoldTot;
  int eBktHisto2ResHoldTot;
  int eBktHisto1ResHoldTot;
  int eBktHisto1HoiTra;
  int eBktHisto2HoiTra;
  int eBktHisto3HoiTra;
  int eBktPrevision;
  int eBktNumDataSets;

  // enumeration type for graphing the standard deviation "band"
  enum BktGraphStdvIndex
  {
    eBktResHoldStdvHigh = 0,
    eBktResHoldStdvLow,
    eBktStdvNumDataSets
  } ;

  CRecHistoData* mp_HistoData;

   // Variables de paramétrage de l'affichage
  BOOL m_bDispEC;  // TRUE si on affiche l'écart-type.	CMPT et BKT
  BOOL m_bDispCapa; // TRUE si on affiche la capacité	CMPT 
  BOOL m_bDispAuthCur; // TRUE si on affiche Overbooking level en CMPT, et AuthCur en BKT (la courbe bleu foncé)
  BOOL m_bDispResHoldAvailSum; // TRUE si on affiche la courbe résa + dispo : BKT only
  BOOL m_bDispResHoldTot; // TRUE si on affiche la courbe résa idv + grp : CMPT et BKT
  BOOL m_bDispResHoldIdv; // TRUE si on affiche la courbe résa IDV : CMPT et BKT
  BOOL m_bDispResHoldAvg; // TRUE si on affiche la courbe moyenne des résa : CMPT et BKT
  BOOL m_bDispHisto1ResHoldTot; // TRUE si on affiche la courbe Historique 1 résa idv + grp : CMPT et BKT
  BOOL m_bDispHisto2ResHoldTot; // TRUE si on affiche la courbe Historique 2 résa idv + grp : CMPT et BKT
  BOOL m_bDispHisto3ResHoldTot; // TRUE si on affiche la courbe Historique 2 résa idv + grp : CMPT et BKT
  BOOL m_bDispPrevision; // TRUE si on affiche la courbe de prevision

// Implementation
protected:
	virtual ~CRecDHistView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

  // the tab control
  CTabCtrl m_ChartTabCtrl; // the tab control
  CString   _sLastTabUsed;
  int	_nSubViewID;

  // array containing tab information and graph data objects
  BOOL m_bTabResized;  // has the tab already been resized ?
  BOOL	_bInitialized;	// Window is now initialized
  BOOL	_bHasPaneDate;	// View has two date panes in the status bar

  // the graphs
  CRecDHistChart m_CmptChart;
  CRecDHistChart m_BktChart;
  CRecDHistChart* m_pCurSelChart;  // chart currently selected by user
  CRecDHistChart* m_pNotSelChart;  // chart currently NOT selected by user

  BOOL m_bCmptDataValid;   // is the compartment data ready ?
  BOOL m_bCmptPrevisionRevenuDataValid;//DT Prévision
  BOOL m_bCmptAvgDataValid;   // is the compartment data ready ?
  BOOL m_bBktDataValid;       // is the bucket data ready ?
  BOOL m_bBktAvgDataValid;       // is the bucket data ready ?
  BOOL m_bComptagesDataValid;   // is the comptages data ready ?
  BOOL m_bHistoDataValid;   // is the historical data ready ?
  BOOL m_bComptagesHistDataValid;   // is the historical data ready ?
  BOOL m_bPtReelDataValid;
  BOOL m_bPrevisionValid;   // is the historical data ready ?

  int m_iCurTabSelection;
  CString m_szCurTabText;

  CFont* m_pPaneFont;

//  enum GraphScale m_eScaleStatus;  // graph is currently scaled

	// Generated message map functions
protected:
	//{{AFX_MSG(CRecDHistView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow (BOOL bShow, UINT nStatus);
	afx_msg void OnChartSelchange(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnScaleMax();
	afx_msg void OnUpdateScaleMax(CCmdUI* pCmdUI);
	afx_msg void OnScaleAuth();
	afx_msg void OnUpdateScaleAuth(CCmdUI* pCmdUI);
	afx_msg void OnScaleResHold();
	afx_msg void OnUpdateScaleResHold(CCmdUI* pCmdUI);
	//}}AFX_MSG
  afx_msg void OnGraphDisplayFlashLabels();
  afx_msg void OnUpdateGraphDisplayFlashLabels( CCmdUI* pCmdUI );
  afx_msg void OnPrint();
  afx_msg void OnUpdateGen( CCmdUI* pCmdUI );
  afx_msg void OnGraphRotateLabels();
  afx_msg void OnUpdateGraphRotateLabels( CCmdUI* pCmdUI );
  afx_msg long OnRefreshAll( UINT wParam, LONG lParam );
	DECLARE_MESSAGE_MAP()

  void ResizeChart();
  
  int  GetRealResaCmpt(char cmpt, int iLevelCur);
  void BuildCmptGraphs();
  void BuildBktGraphs();
  void DoGraphRotateLabels(bool brot);

  void DrawGraph();

  void ClearCmptGraphs();
  void ClearBktGraphs();
  CString MakeSzHisto (const char* szComment , unsigned long ldate);
  void UpdateHistoText();

  //DM 4965.3, chaine listant les dates historiques invalides
  CMapStringToOb m_InvalidHistoDatesByCmpt; // tableau de CString indexé par le compartiment
  void EmptyInvalidHistoDates();

  void BuildTabs();
  void ClearTabs(enum CCTAChartData::GraphType eGraphType);
  CString GetBoolSet(); // renvoi sous forme de string la combinaison de Boolean d'affichage
  void SetBoolSet (CString sBoolSet); // initialise la combinaison de Boolean d'affichage.

  BOOL GetRecD( long  lRecDIndex, CString * szRecD );
  double GetComptageValue(int iDataSet, YmIcComptagesDom* pRecord);

  BOOL m_bRestored;  // to solve a problem with the tab control and its scroll button
  CPtrArray m_ComptagesArray;  // this is used in the CCTADoc::GetComptagesRecord(...) method
  CPtrArray m_ComptagesHistArray;  // this is used in the CCTADoc::GetComptagesRecord(...) method
  int m_histocomptages_date1; // date historique comptage 1
  int m_histocomptages_date2; // date historique comptage 2
  int m_histocomptages_date3; // date historique comptage 3
 
  static BOOL WINAPIV DrawSBItem( YM_StatusBar* sb, LPDRAWITEMSTRUCT lpDrawItemStruct, LPVOID pData );

  CRecDHistView* getOtherView();
  void OnScale (enum CCTAChart::GraphScaleIndex eGraphScale);
};

#ifndef _DEBUG  // debug version in perfView.cpp
inline CCTADoc* CRecDHistView::GetDocument()
   { return (CCTADoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecDHistAltView view
class CRecDHistAltView : public CRecDHistView
{
friend class CRecDHistView;
	CRecDHistAltView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRecDHistAltView)

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecDHistAltView)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RecDHISTORY_H__5627CE71_5B48_11D1_8376_400020302A31__INCLUDED_)
