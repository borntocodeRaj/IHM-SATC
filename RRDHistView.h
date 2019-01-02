#if !defined(AFX_RRDHISTORY_H__5627CE71_5B48_11D1_8376_400020302A31__INCLUDED_)
#define AFX_RRDHISTORY_H__5627CE71_5B48_11D1_8376_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RRDHistView.h : header file

#define SDISPLAYALL	"111111111111111111111111"
#define SDISPLAYMEC	"111111100000011110000001"
#define SDISPLAYECH	"111111011111111111111111"
#define SDISPLAYHIS	"011111011111111111111111"

class CRRDHistChart;

/////////////////////////////////////////////////////////////////////////////
// CHistoData
class CHistoData
{
public:
  CHistoData(BOOL bAlt);
  ~CHistoData();
  int GetNbDate() { return m_MaxHistoRef; };
  double GetCapa (int histoIdx, char cmpt, long rrd);
  double GetAuth (int histoIdx, char cmpt, long rrd, int bucket);
  double GetResa (int histoIdx, char cmpt, long rrd, int bucket);
  double GetSumResa (int histoIdx, char cmpt, long rrd);
  bool GetHoiTra (int histoIdx, char cmpt, long rrd);
  bool IsRrd (long rrd);
  CString GetTitle (int histoIdx);
  RWDate  GetDateHisto (int histoIdx);


  // DM 4965.3 nombre de bucket pour le compartiment cmpt de la date historique d'indice histoIdx
  int GetNbBucket(int histoIdx, char cmpt);  // renvoi -1 si pas d'historique d'indice histoIdx
 
  CMapWordToPtr m_HistoMap; // Tableau de 1, 2 ou 3 CHistodate
  int m_MaxHistoRef;
  int m_max_rrd;
  int m_min_rrd;
  int m_MaxBkt1[26]; // tableau indexé de 'A'-'A' à 'Z'-'A'
  int m_MaxBkt2[26]; // tableau indexé de 'A'-'A' à 'Z'-'A'
  int m_MaxBkt3[26]; // tableau indexé de 'A'-'A' à 'Z'-'A'
  BOOL bSci;
};

/////////////////////////////////////////////////////////////////////////////
// CRRDHistHelp dialog
class CRRDHistHelp : public CCTAChartHelpDlg
{
// Construction
public:
  CRRDHistHelp(CRRDHistChart* pParent = NULL);
  BOOL PrevisionDispo;
  BOOL needRedraw;

// Dialog Data
	//{{AFX_DATA(CRRDHistHelp)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRRDHistHelp)
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
	//{{AFX_MSG(CRRDHistHelp)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
class CDataGraph
{
  public:
  CDataGraph();

  YmIcCmptEvolDom* pCmptEvol;		// Je met les deux pointeurs pour n'avoir d'une structure de données
  YmIcBktEvolDom*  pBktEvol;

  YmIcPrevisionDom* pCmptPrevision;

  double	AuthHist1;					// -1 valeur dummy.
  double	ResaHist1;
  bool      HoiTra1;
  double	AuthHist2;
  double	ResaHist2;
  bool      HoiTra2;
  double	AuthHist3;
  double	ResaHist3;
  bool	    HoiTra3;
  double	ResaTempsReel;				//Point supplementaire de la courbe eXXXXResHoldTot
  int	rrd_index;

	// DM7978 - LME - YI-5050 - ajout d'un booleen bJxHisto pour savoir si le j-x courant est un j-x historique
	bool bJxHisto;

  void AddPrevision (double prevoi);  // Ajoute une valeur de prévision
  double GetPrevision();

private:
  double	Prevision;
};

/////////////////////////////////////////////////////////////////////////////
// CRRDHistChart dialog

class CRRDHistView;
class CRRDHistChart : public CCTAChart
{
// Construction
public:
  CRRDHistChart::CRRDHistChart();

// Attributes
public:

  void SetGraphProperties(enum CCTAChartData::GraphType eGraphType);
  void ChangeHistoLabel(enum CCTAChartData::GraphType eGraphType,
					   CString szHisto1, CString szHisto2, CString szHisto3);
  CCTAChartHelpDlg* CreateHelpDlg();
  void ContextualDisplay(CPoint point, BOOL bInRange);
  bool GetRotateLabelsFlag() { return m_bRotateLabels; }
  void SetRotateLabelsFlag( bool bRotateLabels ) { m_bRotateLabels = bRotateLabels; }

  void SetPrevisionLabel(CString _previsionLabel) { m_PrevisionLabel = _previsionLabel; }


  CRRDHistView* GetParentView() { return ((CRRDHistView*)m_pParentView); }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRRDHistChart)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	CString m_PrevisionLabel;
    bool m_bRotateLabels;  // do we rotate the x-axis labels ?
	int m_graphtype; // Type de graph
	//{{AFX_MSG(CRRDHistChart)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CRRDHistView

class CComptagesView;

class CRRDHistView : public CView, public YM_ObsView
{
#define IDC_RRDHIST_TABCTRL 102

protected:
	CRRDHistView(BOOL bAlt = FALSE);           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRRDHistView)

// Attributes
public:
	CCTADoc* GetDocument();
  CRRDHistChart* GetCmptChart() { return &m_CmptChart; }
  CRRDHistChart* GetBktChart() { return &m_BktChart; }
  CTabCtrl* GetTabCtrl() { return &m_ChartTabCtrl; };

  CRRDHistChart* GetCurSelChart() { return m_pCurSelChart; } // chart currently selected by user
  CRRDHistChart* GetNotSelChart() { return m_pNotSelChart; } // chart currently NOT selected by user
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
  void RefreshAll(BOOL both = TRUE);
  void ChangeRRDDisplay(BOOL bOnF5);
  void RevMasqueCourbe (int graphtype, int set, BOOL b2ndSet);
  int debjxHoiTra, finjxHoiTra;

  BOOL GetAltFlag() const
  {
	  CFrameWnd *fuf;
	  fuf = GetParentFrame();
	  return ((CChildFrame*)GetParentFrame())->GetAltFlag();
  }

// Operations
public:
    BOOL CanCloseFrame() { return TRUE; };
    void FrameSizeChange (UINT nType, int cx, int cy);
    void WriteViewConfig();
    void RestoreViewConfig();
    CComptagesView* GetComptagesView();
    void    enablePaneDate( BOOL bEnable=TRUE );
    void    setPaneDateText( int iPane, LPCTSTR pszText );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRRDHistView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

public:

  int dernierJX;  
  // enumeration type for indexing of the Compartment graph lines
  int eCmptCapacity;  
  int eCmptOvbLevelCur;
  int eCmptResHoldTot;
  int eCmptHoiTra;
  int eCmptHoiTraIdv;
  int eCmptResHoldIdv;
  int eCmptResHoldAvg;
  int eCmptHisto1OvbLevel;
  int eCmptHisto1ResHoldTot;
  int eCmptHisto2OvbLevel;
  int eCmptHisto2ResHoldTot;
  int eCmptHisto3OvbLevel;
  int eCmptHisto3ResHoldTot;
  int eCmptHisto1HoiTra;
  int eCmptHisto2HoiTra;
  int eCmptHisto3HoiTra;
  int eCmptComptageOfferStdvHigh;
  int eCmptComptageOffer;
  int eCmptComptageOfferStdvLow;
  int eCmptComptageOccupStdvHigh;
  int eCmptComptageOccup;
  int eCmptComptageOccupStdvLow;
  int eCmptComptageHist1Offer;
  int eCmptComptageHist1Occup;
  int eCmptComptageHist2Offer;
  int eCmptComptageHist2Occup;
  int eCmptComptageHist3Offer;
  int eCmptComptageHist3Occup;
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
  int eBktAuthCur;
  int eBktResHoldAvailSum;
  int eBktResHoldTot;
  int eBktHoiTra;
  int eBktHoiTraIdv;
  int eBktResHoldIdv;
  int eBktResHoldAvg;
  int eBktHisto3AuthCur;
  int eBktHisto3ResHoldTot;
  int eBktHisto2AuthCur;
  int eBktHisto2ResHoldTot;
  int eBktHisto1AuthCur;
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

  CHistoData* mp_HistoData;
  CString m_histocomptages_comment1;
  CString m_histocomptages_comment2;
  CString m_histocomptages_comment3;

   // Variables de paramétrage de l'affichage
  BOOL m_bDispEC;  // TRUE si on affiche l'écart-type.	CMPT et BKT
  BOOL m_bDispCapa; // TRUE si on affiche la capacité	CMPT 
  BOOL m_bDispAuthCur; // TRUE si on affiche Overbooking level en CMPT, et AuthCur en BKT (la courbe bleu foncé)
  BOOL m_bDispResHoldAvailSum; // TRUE si on affiche la courbe résa + dispo : BKT only
  BOOL m_bDispResHoldTot; // TRUE si on affiche la courbe résa idv + grp : CMPT et BKT
  BOOL m_bDispResHoldIdv; // TRUE si on affiche la courbe résa IDV : CMPT et BKT
  BOOL m_bDispResHoldAvg; // TRUE si on affiche la courbe moyenne des résa : CMPT et BKT
  BOOL m_bDispHisto1AuthCur; // TRUE si on affiche Historique 1 OvbLevel en CMPT, et AuthCur en BKT
  BOOL m_bDispHisto1ResHoldTot; // TRUE si on affiche la courbe Historique 1 résa idv + grp : CMPT et BKT
  BOOL m_bDispHisto2AuthCur; // TRUE si on affiche Historique 2 OvbLevel en CMPT, et AuthCur en BKT
  BOOL m_bDispHisto2ResHoldTot; // TRUE si on affiche la courbe Historique 2 résa idv + grp : CMPT et BKT
  BOOL m_bDispHisto3AuthCur; // TRUE si on affiche Historique 2 OvbLevel en CMPT, et AuthCur en BKT
  BOOL m_bDispHisto3ResHoldTot; // TRUE si on affiche la courbe Historique 2 résa idv + grp : CMPT et BKT
  BOOL m_bDispComptageOfferStvd; // TRUE si on affiche ecart type de l'offre en comptage
  BOOL m_bDispComptageOffer; // TRUE si on affiche l'offre en comptage
  BOOL m_bDispComptageOccupStvd; // TRUE si on affiche ecart type de l'occupation en comptage
  BOOL m_bDispComptageOccup; // TRUE si on affiche l'occupation en comptage
  BOOL m_bDispHisto1ComptageOffer; // TRUE si on affiche l'offre en Historique 1 comptage
  BOOL m_bDispHisto1ComptageOccup; // TRUE si on affiche l'occupation en Historique 1 comptage
  BOOL m_bDispHisto2ComptageOffer; // TRUE si on affiche l'offre en Historique 1 comptage
  BOOL m_bDispHisto2ComptageOccup; // TRUE si on affiche l'occupation en Historique 1 comptage
  BOOL m_bDispHisto3ComptageOffer; // TRUE si on affiche l'offre en Historique 1 comptage
  BOOL m_bDispHisto3ComptageOccup; // TRUE si on affiche l'occupation en Historique 1 comptage
  BOOL m_bDispPrevision; // TRUE si on affiche la courbe de prevision

	// DM7978 - LME - YI-5050
	bool m_bCmptAlreadyBuild;

// Implementation
protected:
	virtual ~CRRDHistView();
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
  long nature;//DM 8029 - Need to check the nature of the tranche, if = 3, display only the SCG
  CRRDHistChart m_CmptChart;
  CRRDHistChart m_BktChart;
  CRRDHistChart* m_pCurSelChart;  // chart currently selected by user
  CRRDHistChart* m_pNotSelChart;  // chart currently NOT selected by user

  BOOL m_bCmptDataValid;   // is the compartment data ready ?
  BOOL m_bCmptAvgDataValid; // is the average compartment data ready ?
  BOOL m_bCmptPrevisionVenteDataValid; // is the average compartment data ready ?
  BOOL m_bBktDataValid;       // is the bucket data ready ?
  BOOL m_bBktAvgDataValid;       // is the bucket data ready ?
  BOOL m_bComptagesDataValid;   // is the comptages data ready ?
  BOOL m_bHistoDataValid;   // is the historical data ready ?
  BOOL m_bComptagesHistDataValid;   // is the historical data ready ?
  BOOL m_bPrevisionValid;   // is the historical data ready ?

  int m_iCurTabSelection;
  CString m_szCurTabText;

	// DM7978 - LME - YI-4625
	enum CmptOrBktTab
  {
    eCmptTabSelected,
    eBktTabSelected
  }; 
	CmptOrBktTab m_eTabSelected;
	inline bool IsCmptTabSelected() const {return (m_eTabSelected == eCmptTabSelected);}
	inline bool IsBktTabSelected() const {return (m_eTabSelected == eBktTabSelected);}
	// DM7978 - LME - FIN

  CFont* m_pPaneFont;

//  enum GraphScale m_eScaleStatus;  // graph is currently scaled

	// Generated message map functions
protected:
	//{{AFX_MSG(CRRDHistView)
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
	//DM7978 - LME - YI5401 - ajout des onglets global/réduit
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//DM7978 - LME - FIN
  afx_msg void OnGraphDisplayFlashLabels();
  afx_msg void OnUpdateGraphDisplayFlashLabels( CCmdUI* pCmdUI );
  afx_msg void OnPrint();
  afx_msg void OnUpdateGen( CCmdUI* pCmdUI );
  afx_msg void OnGraphRotateLabels();
  afx_msg void OnUpdateGraphRotateLabels( CCmdUI* pCmdUI );
  afx_msg long OnRefreshAll( UINT wParam, LONG lParam );
	DECLARE_MESSAGE_MAP()
	
	// DM7978 - LME - YI-5050 -vecteur donnees historiques
	std::vector<YmIcHistoDataRRDDom> m_HistoDataSet;
	std::vector<YmIcHistoDataRRDDom> getHistoDataByCmpt(const char cmpt);
	inline long getCcMax() const {return m_lCcMax;}
	inline void setCcMax(const long& ccMax) {m_lCcMax = ccMax;}
	void FillHistoDataSet();

  void ResizeChart();

  int  GetRealResaCmpt(char cmpt, int iLevelCur);
  void BuildCmptGraphs();
  void BuildBktGraphs();
  void DoGraphRotateLabels(bool brot);

  void DrawGraph();

	// DM7978 - LME - YI-5055 - calcul du j-x historique
	long CalculJxHisto(const std::vector<YmIcHistoDataRRDDom>& histoData);
	long m_lCcMax;
	

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

  BOOL GetRRD( long  lRrdIndex, CString * szRRD );
  double GetComptageValue(int iDataSet, YmIcComptagesDom* pRecord);

  BOOL m_bRestored;  // to solve a problem with the tab control and its scroll button
  CPtrArray m_ComptagesArray;  // this is used in the CCTADoc::GetComptagesRecord(...) method
  CPtrArray m_ComptagesHistArray;  // this is used in the CCTADoc::GetComptagesRecord(...) method
  int m_histocomptages_date1; // date historique comptage 1
  int m_histocomptages_date2; // date historique comptage 2
  int m_histocomptages_date3; // date historique comptage 3
 
  static BOOL WINAPIV DrawSBItem( YM_StatusBar* sb, LPDRAWITEMSTRUCT lpDrawItemStruct, LPVOID pData );

  CRRDHistView* getOtherView();
  void OnScale (enum CCTAChart::GraphScaleIndex eGraphScale);
};

#ifndef _DEBUG  // debug version in perfView.cpp
inline CCTADoc* CRRDHistView::GetDocument()
   { return (CCTADoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
// CRRDHistAltView view

class CRRDHistAltView : public CRRDHistView
{
friend class CRRDHistView;
	CRRDHistAltView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRRDHistAltView)

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRRDHistAltView)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RRDHISTORY_H__5627CE71_5B48_11D1_8376_400020302A31__INCLUDED_)
