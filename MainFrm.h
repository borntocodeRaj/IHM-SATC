// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_MAINFRM_H__2017F0D9_3EE9_11D1_8369_400020302A31__INCLUDED_)
#define AFX_MAINFRM_H__2017F0D9_3EE9_11D1_8369_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CResaRailView;
class CRRDHistView;
class CRecDHistView;
class CTrancheView;
class CCalendarView;
class CSuiviTopIstView;
class CVnlEngineView;
class CBktHistView;
class CBktHistAltView;
class CCTAChartHelpDlg;
class CSendVnauDlg;
class CQuickVnauDlg;

class CMainFrame : public YM_PersistentFrame
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	virtual HMENU GetWindowMenuPopup( HMENU hMenuBar );

  YM_StatusBar *GetStatusBar() { return &m_wndStatusBar; };

  CDialog* GetHelpDlg() { return m_pHelpDlg; };
  void SetHelpDlg(CDialog* pViewHelpDlg);
  void DestroyHelpDlg();
  void SetHelpDlgPoint(CPoint& point) { m_HelpDlgPoint = point; };
  CPoint* GetHelpDlgPoint() { return &m_HelpDlgPoint; };

  CCTAChartHelpDlg* GetChartHelpDlg() { return m_pChartHelpDlg; };
  void SetChartHelpDlg(CCTAChartHelpDlg* pChartHelpDlg);

  void ForceVnlTracer() { OnSelectionTRACER();};

  // following methods are for modifying the menus according to client parameters
  int GetMenuPos( CMenu* pMainMenu, int iResId );
  void InsertWindowMenus0(CMenu* pSubMenu, int& iItemOffset);
  void InsertWindowMenus1(CMenu* pSubMenu, int& iItemOffset, BOOL bAlt = FALSE);
  void ModifySystemMenu();
  void ModifyWindowMenu();
  void ModifyHelpMenu();
  void ModifyTrancheMenu(BOOL bIsTrancheView = FALSE );
  void RemoveLegAltMenu();
  void ModifyLegMenu( BOOL bIsLegView = FALSE, BOOL bAlt = FALSE );
  void ModifyRelTrancheMenu(BOOL bAlt = FALSE);
  void ModifyRrdHistMenu();
  void ModifyRecdHistMenu();
// Résolution anomalie 49789 : masquer les éléments de sous-menu nouveaux
// liés à la DM 7791
// HRI, le 24/10/2013
  void ModifyVNLMenu();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
  CToolBar      m_wndToolBar;
  CSendVnauDlg*	m_sendVnauDlg;
  CQuickVnauDlg* m_quickVnauDlg;
  BOOL bFlagFrontBack;// TRUE = Normal View; FALSE = Alt View  
  BOOL IsLegView(CView* pView);
  BOOL IsLegViewCreated();
  BOOL IsResaRailView(CView* pView);
  BOOL IsResaRailViewCreated();
  BOOL IsHistDptView(CView* pView, BOOL bAlt = FALSE);
  BOOL IsHistDptViewCreated(BOOL bAlt = FALSE);
  BOOL IsOthLegsView(CView* pView, BOOL bAlt = FALSE);
  BOOL IsOthLegsViewCreated(BOOL bAlt = FALSE);
  BOOL IsRelTrancheView(CView* pView, BOOL bAlt = FALSE);
  BOOL IsRelTrancheViewCreated(BOOL bAlt = FALSE);
  BOOL IsRRDHistView(CView* pView, BOOL bAlt = FALSE);
  BOOL IsRRDHistViewCreated(BOOL bAlt = FALSE);
  BOOL IsRecDHistView(CView* pView, BOOL bAlt = FALSE);
  BOOL IsRecDHistViewCreated(BOOL bAlt = FALSE);
  BOOL IsBktHistView(CView* pView, BOOL bAlt = FALSE);
  BOOL IsBktHistViewCreated(BOOL bAlt = FALSE);
  BOOL IsVirtNestView(CView* pView);
  BOOL IsVirtNestViewCreated();
  BOOL IsComptagesView(CView* pView);
  BOOL IsComptagesViewCreated();
  BOOL IsTrainSupplementView(CView* pView);
  BOOL IsTrainSupplementViewCreated();
  BOOL IsTrainAdaptationView(CView* pView);
  BOOL IsTrainAdaptationViewCreated();
  BOOL IsTrainDeletedView(CView* pView);
  BOOL IsTrainDeletedViewCreated();
  BOOL IsVnauListView(CView* pView);
  BOOL IsVnauListViewCreated();
  BOOL IsCalendarView(CView* pView);
  BOOL IsCalendarViewCreated();
  BOOL IsTraqueTarifView(CView* pView);
  BOOL IsTraqueTarifViewCreated();
  BOOL IsSuiviVnauView(CView* pView);
  BOOL IsSuiviVnauViewCreated();
  BOOL IsSuiviVnaxView(CView* pView);
  BOOL IsSuiviVnaxViewCreated();
  BOOL IsVnaxView(CView* pView, BOOL bAlt = FALSE);
  BOOL IsVnaxViewCreated(BOOL bAlt = FALSE);
  BOOL IsSuiviTopIstView(CView* pView);
  BOOL IsSuiviTopIstViewCreated();
  
  //LSO Ajout du menu de raffraichissement
  void AddRefreshMenu(void);

  CRRDHistView*     GetRRDHistView(BOOL bAlt = FALSE);
  CRecDHistView*    GetRecDHistView(BOOL bAlt = FALSE);
  CTrancheView*     GetTrancheView(BOOL bAssert = TRUE);
  CLegView*         GetLegView();
  CResaRailView*    GetResaRailView();
  CCalendarView*    GetCalendarView();
  CSuiviTopIstView* GetSuiviTopIstView();
	CBktHistView*			GetCBktHistView();
	CBktHistAltView*	GetCBktHistAltView();
  /// DM-7978 - CLE - permet de recupérer la vue vnl pour la notifier avec le bouchon 
  CVnlEngineView*	GetVnlEngineView();
  /// DM-7978 - CLE - 

  COLORREF GetColorTextBkMain() { return m_clrTextBkMain; }  // main windows background color
  void SetColorTextBkMain(COLORREF clrTextBkMain) { m_clrTextBkMain = clrTextBkMain; }  // main windows background color
  COLORREF GetColorTextBkAlt() { return m_clrTextBkAlt; } // alternate windows background color
  void SetColorTextBkAlt(COLORREF clrTextBkAlt) { m_clrTextBkAlt = clrTextBkAlt; } // alternate windows background color
  
  COLORREF GetColorTextRecBkMain() { return m_clrTextRecBkMain; }
  void SetColorTextRecBkMain(COLORREF clrTextRecBkMain) { m_clrTextRecBkMain = clrTextRecBkMain; }  // main windows background color
  COLORREF GetColorTextRecBkAlt() { return m_clrTextRecBkAlt; } // alternate windows background color
  void SetColorTextRecBkAlt(COLORREF clrTextRecBkAlt) { m_clrTextRecBkAlt = clrTextRecBkAlt; } // alternate windows background color
  
  COLORREF GetColorFontRecBkMain() { return m_clrFontRecBkMain; } // alternate windows background color
  void SetColorFontRecBkMain(COLORREF clrFontRecBkMain) { m_clrFontRecBkMain = clrFontRecBkMain; } // alternate windows background color
  COLORREF GetColorFontRecBkAlt() { return m_clrFontRecBkAlt; } // alternate windows background color
  void SetColorFontRecBkAlt(COLORREF clrFontRecBkAlt) { m_clrFontRecBkAlt = clrFontRecBkAlt; } // alternate windows background color

  //DM4802, mise en public pour être accessible de TrancheView.cpp
  void OnSelectionShowAllWindows(BOOL bAlt = FALSE, int iShow = SW_SHOW);

  //Le 06/06/07 Affichage d'un message en titre d'application si VNAU bloque
  void WarnOnVNAU (CString sWarn);

  //DM 5882.1, forcer affichage fenêtre CC et masquer SCI si tranche sans SCI
  void TestTrancheSansSCI();

protected:  // control bar embedded members

	YM_StatusBar  m_wndStatusBar;
  BOOL   m_bActivated;
  int m_iMainShow;  // main windows show or hide
  int m_iAltShow;  // alternate windows show or hide
  BOOL m_bAltLastShow;  // derniere demande show vrai si ALT, faux si Main
  COLORREF m_clrTextBkMain;  // main windows background color
  COLORREF m_clrTextBkAlt;  // alternate windows background color
  COLORREF m_clrTextRecBkMain;  // main fenetre recette background color
  COLORREF m_clrTextRecBkAlt;   // alternate fenetre recette background color
  COLORREF m_clrFontRecBkMain;  // main fenetre recette color Font
  COLORREF m_clrFontRecBkAlt;   // alternate fenetre recette color Font

  CDialog* m_pHelpDlg;  // the chart help dialog
  CCTAChartHelpDlg* m_pChartHelpDlg;  // the chart help dialog
  CPoint m_HelpDlgPoint;  // position of the help dialog
  CPoint m_ChartHelpDlgPoint;  // position of the help dialog

private:
  CString m_sWarnVnau; // dernier message d'alarme sur VNAU bloqué, vide si OK.
  CString m_sRefTitle; // Titre de référence

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnWindowConfigurations();
    afx_msg void OnUpdateWindowConfigurations(CCmdUI* pCmdUI);
    afx_msg void OnDefaultFonte();
    afx_msg void OnConfigFonte();
    afx_msg void OnUpdateConfigFonte(CCmdUI* pCmdUI);
    afx_msg void OnRealTime();
    afx_msg void OnUpdateRealTime(CCmdUI* pCmdUI);
    afx_msg void OnReconnect();
    afx_msg void OnUpdateReconnect(CCmdUI* pCmdUI);
	afx_msg void OnHistoLink();
    afx_msg void OnUpdateHistoLink(CCmdUI* pCmdUI);
	afx_msg void OnWaitingVnau();
    afx_msg void OnUpdateWaitingVnau(CCmdUI* pCmdUI);
    afx_msg void OnEntityStats();
    afx_msg void OnUpdateEntityStats(CCmdUI* pCmdUI);
  	afx_msg void OnTestRrd();
    afx_msg void OnUpdateTestRrd(CCmdUI* pCmdUI);
  	afx_msg void OnTestRecd();
    afx_msg void OnUpdateTestRecd(CCmdUI* pCmdUI);
    afx_msg void OnGraphDisplayFlashLabels();
    afx_msg void OnUpdateGraphDisplayFlashLabels(CCmdUI* pCmdUI);
    afx_msg void OnTrancheNext();
    afx_msg void OnUpdateTrancheNext(CCmdUI* pCmdUI);
    afx_msg void OnTranchePrev();
    afx_msg void OnUpdateTranchePrev(CCmdUI* pCmdUI);
    afx_msg void OnTrancheCriticalMessages();
    afx_msg void OnUpdateTrancheCriticalMessages(CCmdUI* pCmdUI);
    afx_msg void OnTrancheSortCriticality();
    afx_msg void TrancheSortDptTrain();
    afx_msg void TrancheSortArrTrain();
    afx_msg void OnTrancheSortDate(BOOL bDpt = TRUE);
    afx_msg void OnTrancheSortTrainDate();
    //afx_msg void OnTrancheSortLevelDateTime(); // NPI - DM7978 : souplesse tarifaire lot 2
    afx_msg void OnUpdateTrancheSortCrit(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTrancheSortDptTrain(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTrancheSortArrTrain(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTrancheSortDate(CCmdUI* pCmdUI, BOOL bDpt = TRUE);
    afx_msg void OnUpdateTrancheSortTrain(CCmdUI* pCmdUI);
    //afx_msg void OnUpdateTrancheSortLevel(CCmdUI* pCmdUI); // NPI - DM7978 : souplesse tarifaire lot 2
    afx_msg void OnTrancheDefineComptagesRefDate() ;
    afx_msg void OnUpdateTrancheDefineComptagesRefDate(CCmdUI* pCmdUI) ;
    afx_msg void OnTrancheAdaptation() ;
    afx_msg void OnUpdateTrancheAdaptation(CCmdUI* pCmdUI) ;
    afx_msg void OnTrancheDefaultTime();
    afx_msg void OnUpdateTrancheDefaultTime(CCmdUI* pCmdUI);
    afx_msg void OnTrancheDptTime();
    afx_msg void OnUpdateTrancheDptTime(CCmdUI* pCmdUI);
    afx_msg void OnTrancheArrTime();
    afx_msg void OnUpdateTrancheArrTime(CCmdUI* pCmdUI);
    afx_msg void OnTrancheHelp();
    afx_msg void OnUpdateTrancheHelp( CCmdUI* pCmdUI );
    afx_msg void OnLegHelp();
    afx_msg void OnUpdateLegHelp( CCmdUI* pCmdUI );
    afx_msg void OnLegSend();
    afx_msg void OnLegSendNoQuery();
    afx_msg void OnUpdateLegSend(CCmdUI* pCmdUI);
    afx_msg void OnLegUndo();
    afx_msg void OnUpdateLegUndo(CCmdUI* pCmdUI);
    afx_msg void OnLegCriticalMessages();
    afx_msg void OnUpdateLegCriticalMessages(CCmdUI* pCmdUI);
    afx_msg void OnLegNext();
    afx_msg void OnUpdateLegNext(CCmdUI* pCmdUI);
    afx_msg void OnLegPrev();
    afx_msg void OnUpdateLegPrev(CCmdUI* pCmdUI);
    afx_msg void OnLegPasteAuthSelectedVnau();
	afx_msg void OnLegPasteAuthSelectedVnas();
	afx_msg void OnLegPasteAuthSelectedLes2();
    afx_msg void OnUpdateLegPasteAuthSelected( CCmdUI* pCmdUI );
	afx_msg void OnUpdateLegPasteAuthSelectedVnas( CCmdUI* pCmdUI );
    afx_msg void OnLegPasteAuthListVnau();
	afx_msg void OnLegPasteAuthListVnas();
	afx_msg void OnLegPasteAuthListLes2();
    afx_msg void OnUpdateLegPasteAuthList( CCmdUI* pCmdUI );
	afx_msg void OnUpdateLegPasteAuthListVnas( CCmdUI* pCmdUI );
    afx_msg void OnLegPasteAuthAllVnau();
	afx_msg void OnLegPasteAuthAllVnas();
	afx_msg void OnLegPasteAuthAllLes2();
    afx_msg void OnUpdateLegPasteAuthAll( CCmdUI* pCmdUI );
	afx_msg void OnUpdateLegPasteAuthAllVnas( CCmdUI* pCmdUI );
    afx_msg void OnLegAutoCopyAuth();
    afx_msg void OnUpdateLegAutoCopyAuth( CCmdUI* pCmdUI );
    afx_msg void OnLegDefaultSort();
    afx_msg void OnUpdateLegDefaultSort(CCmdUI* pCmdUI);
    afx_msg void OnLegReverseSort();
    afx_msg void OnUpdateLegReverseSort(CCmdUI* pCmdUI);
	afx_msg void OnLegSelPeak();
    afx_msg void OnUpdateLegSelPeak(CCmdUI* pCmdUI);
    afx_msg void OnLegAdjust();
    afx_msg void OnUpdateLegAdjust(CCmdUI* pCmdUI);
    afx_msg void OnSelectionEntity();
    afx_msg void OnUpdateSelectionEntity(CCmdUI* pCmdUI);
    afx_msg void OnSelectionTranche();
    afx_msg void OnUpdateSelectionTranche(CCmdUI* pCmdUI);
    afx_msg void OnSelectionLeg();
    afx_msg void OnUpdateSelectionLeg(CCmdUI* pCmdUI);
    afx_msg void OnSelectionResaRail();
    afx_msg void OnUpdateSelectionResaRail(CCmdUI* pCmdUI);
	afx_msg void OnSelectionAffPMoyen();
    afx_msg void OnUpdateSelectionAffPMoyen(CCmdUI* pCmdUI);
    afx_msg void OnSelectionHistDpt();
    afx_msg void OnUpdateSelectionHistDpt(CCmdUI* pCmdUI);
    afx_msg void OnSelectionHistDptAlt();
    afx_msg void OnUpdateSelectionHistDptAlt(CCmdUI* pCmdUI);
    afx_msg void OnSelectionBktHist();
    afx_msg void OnUpdateSelectionBktHist(CCmdUI* pCmdUI);
    afx_msg void OnSelectionBktHistAlt();
    afx_msg void OnUpdateSelectionBktHistAlt(CCmdUI* pCmdUI);
    afx_msg void OnSelectionRRDHist();
    afx_msg void OnUpdateSelectionRRDHist(CCmdUI* pCmdUI);
    afx_msg void OnSelectionRRDHistAlt();
    afx_msg void OnUpdateSelectionRRDHistAlt(CCmdUI* pCmdUI);
    afx_msg void OnSelectionRecDHist();
    afx_msg void OnUpdateSelectionRecDHist(CCmdUI* pCmdUI);
    afx_msg void OnSelectionRecDHistAlt();
    afx_msg void OnUpdateSelectionRecDHistAlt(CCmdUI* pCmdUI);
    afx_msg void OnSelectionOthLegs();
    afx_msg void OnUpdateSelectionOthLegs(CCmdUI* pCmdUI);
    afx_msg void OnSelectionOthLegsAlt();
    afx_msg void OnUpdateSelectionOthLegsAlt(CCmdUI* pCmdUI);
    afx_msg void OnSelectionRelTranche();
    afx_msg void OnUpdateSelectionRelTranche(CCmdUI* pCmdUI);
    afx_msg void OnSelectionRelTrancheAlt();
    afx_msg void OnUpdateSelectionRelTrancheAlt(CCmdUI* pCmdUI);
    afx_msg void OnSelectionVirtNest();
    afx_msg void OnUpdateSelectionVirtNest(CCmdUI* pCmdUI);
	afx_msg void OnSelectionVnauList();
    afx_msg void OnUpdateSelectionVnauList(CCmdUI* pCmdUI);
	afx_msg void OnSelectionCalendar();
    afx_msg void OnUpdateSelectionCalendar(CCmdUI* pCmdUI);
	afx_msg void OnSelectionTraqueTarif();
    afx_msg void OnUpdateSelectionTraqueTarif(CCmdUI* pCmdUI);
	afx_msg void OnSelectionSuiviVnau();
    afx_msg void OnUpdateSelectionSuiviVnau(CCmdUI* pCmdUI);
	afx_msg void OnSelectionSuiviVnax();
    afx_msg void OnUpdateSelectionSuiviVnax(CCmdUI* pCmdUI);
	afx_msg void OnSelectionSuiviTopIst();
    afx_msg void OnUpdateSelectionSuiviTopIst(CCmdUI* pCmdUI);
    afx_msg void OnSelectionComptages();
    afx_msg void OnUpdateSelectionComptages(CCmdUI* pCmdUI);
    afx_msg void OnSelectionTrainSupplement();
    afx_msg void OnUpdateSelectionTrainSupplement(CCmdUI* pCmdUI);
    afx_msg void OnSelectionTrainAdaptation();
    afx_msg void OnUpdateSelectionTrainAdaptation(CCmdUI* pCmdUI);
    afx_msg void OnSelectionTrainDeleted();
    afx_msg void OnUpdateSelectionTrainDeleted(CCmdUI* pCmdUI);
    afx_msg void OnSelectionVNAX();
    afx_msg void OnUpdateSelectionVNAX(CCmdUI* pCmdUI);
	afx_msg void OnSelectionMoteurVNAX();
    afx_msg void OnUpdateSelectionMoteurVNAX(CCmdUI* pCmdUI);
    afx_msg void OnLocalVNAY();
    afx_msg void OnUpdateLocalVNAY(CCmdUI* pCmdUI);
	afx_msg void OnServeurVNAY();
    afx_msg void OnUpdateServeurVNAY(CCmdUI* pCmdUI);
  	afx_msg void OnAutomaticVNL();
    afx_msg void OnUpdateAutomaticVNL(CCmdUI* pCmdUI);
  	afx_msg void OnVNLEngine();
    afx_msg void OnUpdateVNLEngine(CCmdUI* pCmdUI);
	afx_msg void OnDatesHisto();
	afx_msg void OnUpdateDatesHisto(CCmdUI* pCmdUI);
    afx_msg void OnSelectionTRACER();
    afx_msg void OnUpdateSelectionTRACER(CCmdUI* pCmdUI);
    afx_msg void OnViewMANUALVNAU();
    afx_msg void OnUpdateViewMANUALVNAU(CCmdUI* pCmdUI);
    afx_msg void OnViewQUICKVNAU();
    afx_msg void OnUpdateViewQUICKVNAU(CCmdUI* pCmdUI);
    afx_msg void OnSelectionShowAllWindows1();
    afx_msg void OnUpdateSelectionShowAllWindows1(CCmdUI* pCmdUI);
    afx_msg void OnSelectionHideAllWindows1();
    afx_msg void OnUpdateSelectionHideAllWindows1(CCmdUI* pCmdUI);
    afx_msg void OnSelectionShowAllWindows2();
    afx_msg void OnUpdateSelectionShowAllWindows2(CCmdUI* pCmdUI);
    afx_msg void OnSelectionHideAllWindows2();
    afx_msg void OnUpdateSelectionHideAllWindows2(CCmdUI* pCmdUI);
    afx_msg void OnSelectionToggleAllWindows1();
    afx_msg void OnSelectionToggleAllWindows2();
	afx_msg void OnSelectionToggleRRDDisplay();
	afx_msg void OnSelectionFrontBackRRDHist();
    afx_msg void OnSelectionWindowsColor1();
    afx_msg void OnSelectionWindowsColorDefault1();
    afx_msg void OnUpdateSelectionWindowsColor1(CCmdUI* pCmdUI);
    afx_msg void OnSelectionWindowsColor2();
    afx_msg void OnSelectionWindowsColorDefault2();
    afx_msg void OnUpdateSelectionWindowsColor2(CCmdUI* pCmdUI);
    afx_msg void OnStnCodeTranslate();
    afx_msg void OnUpdateStnCodeTranslate(CCmdUI* pCmdUI);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClose();
    afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelectionWindowsRecetteColor1();
    afx_msg void OnUpdateSelectionWindowsRecetteColor1(CCmdUI* pCmdUI);
	afx_msg void OnSelectionWindowsRecetteColor2();
    afx_msg void OnUpdateSelectionWindowsRecetteColor2(CCmdUI* pCmdUI);
    afx_msg void OnSelectionFontsRecetteColor1();
    afx_msg void OnUpdateSelectionFontsRecetteColor1(CCmdUI* pCmdUI);
    afx_msg void OnSelectionFontsRecetteColor2();
	afx_msg void OnUpdateSelectionFontsRecetteColor2(CCmdUI* pCmdUI);
	afx_msg void OnYavHistDptToggle();
	afx_msg void OnUpdateYavHistDptToggle(CCmdUI* pCmdUI);
	afx_msg void OnJmXEntity();
	afx_msg void OnUpdateJmXEntity(CCmdUI* pCmdUI);
	afx_msg void OnUpdateComm (CCmdUI *pCmdUI);
	afx_msg void OnVisuPrevFac();
    afx_msg void OnUpdateVisuPrevFac( CCmdUI* pCmdUI );
    afx_msg void OnVisuPrevSus();
    afx_msg void OnUpdateVisuPrevSus( CCmdUI* pCmdUI );
    afx_msg void OnVisuFerm();
    afx_msg void OnUpdateVisuFerm( CCmdUI* pCmdUI );
    afx_msg void OnRefresh();
	//}}AFX_MSG
	afx_msg LONG OnLoadTranches( UINT wParam, LONG lParam );
	DECLARE_MESSAGE_MAP()

  void OnUpdateSelectionShowAllWindows(CCmdUI* pCmdUI, BOOL bAlt = FALSE, int iShow = SW_SHOW);
  void OnSelectionWindowsColor(BOOL bAlt = FALSE, BOOL bDefault = FALSE);
  void OnUpdateSelectionWindowsColor(CCmdUI* pCmdUI, BOOL bAlt = FALSE);
  void DoReconnect (BOOL resetbadtry = FALSE);
  void OnSelectionWindowsRecetteColor(BOOL bAlt = FALSE, BOOL bDefault = FALSE);
  void OnUpdateSelectionWindowsRecetteColor(CCmdUI* pCmdUI, BOOL bAlt = FALSE);
  void OnSelectionFontsRecetteColor(BOOL bAlt = FALSE, BOOL bDefault = FALSE);
  void OnUpdateSelectionFontsRecetteColor(CCmdUI* pCmdUI, BOOL bAlt = FALSE);


  void OnSelectionHistDptX(BOOL bAlt = FALSE);
  void OnUpdateSelectionHistDptX(CCmdUI* pCmdUI, BOOL bAlt = FALSE);
  void OnSelectionBktHistX(BOOL bAlt = FALSE);
  void OnUpdateSelectionBktHistX(CCmdUI* pCmdUI, BOOL bAlt = FALSE);
  void OnSelectionRRDHistX(BOOL bAlt = FALSE);
  void OnUpdateSelectionRRDHistX(CCmdUI* pCmdUI, BOOL bAlt = FALSE);
  void OnSelectionRecDHistX(BOOL bAlt = FALSE);
  void OnUpdateSelectionRecDHistX(CCmdUI* pCmdUI, BOOL bAlt = FALSE);
  void OnSelectionOthLegsX(BOOL bAlt = FALSE);
  void OnUpdateSelectionOthLegsX(CCmdUI* pCmdUI, BOOL bAlt = FALSE);
  void OnSelectionRelTrancheX(BOOL bAlt = FALSE);
  void OnUpdateSelectionRelTrancheX(CCmdUI* pCmdUI, BOOL bAlt = FALSE);
  void OnSelectionVnaxX(BOOL bAlt = FALSE);
  void OnUpdateSelectionVnaxX(CCmdUI* pCmdUI, BOOL bAlt = FALSE);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__2017F0D9_3EE9_11D1_8369_400020302A31__INCLUDED_)
