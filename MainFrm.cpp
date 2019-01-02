// MainFrm.cpp : implementation of the CMainFrame class
//
#include "StdAfx.h"

#include "ResTracer.h"
#include "DatesHistoDlg.h"

#include "CTAChartHelpDlg.h"
#include "CTA_Resource.h"
#include "EntityDlg.h"
#include "EntityStatsDlg.h"
#include "TrancheView.h"

#include "ResaRailView.h"
#include "HistDptView.h"
#include "OthLegsView.h"
#include "RelTrancheView.h"
#include "RRDHistView.h"
#include "RecDHistView.h"
#include "BktHistView.h"
#include "VirtNestView.h"
#include "VNAXwnd.h"
#include "VnayDlg.h"
#include "Vnay.h"
#include "SendVnauDlg.h"
#include "QuickVnauDlg.h"
#include "VnlEngineView.h"
#include "DlgStnCodeTranslator.h"
#include "ComptagesView.h"
#include "TrainSupplementView.h"
#include "QuotasRhealysDlg.h"
#include "VnauListView.h"
#include "CalendarView.h"
#include "TraqueTarifView.h"
#include "EntityRrd.h"
#include "SuiviVnauView.h"
#include "SuiviVnaxView.h"
#include "SuiviTopIstView.h" 

#include "VnlEngineView.h"
#include "BktHistView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_COMM,
};

static UINT indicators_noncomm[] =
{
	ID_SEPARATOR          // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, YM_PersistentFrame)

BEGIN_MESSAGE_MAP(CMainFrame, YM_PersistentFrame)
	//{{AFX_MSG_MAP(CMainFrame)
  ON_WM_CREATE()
  ON_COMMAND(ID_WINDOW_CONFIGURATIONS, OnWindowConfigurations)
  ON_UPDATE_COMMAND_UI(ID_WINDOW_CONFIGURATIONS, OnUpdateWindowConfigurations)
  ON_COMMAND(ID_DEFAULT_FONT, OnDefaultFonte)
  ON_UPDATE_COMMAND_UI(ID_DEFAULT_FONT, OnUpdateConfigFonte)
  ON_COMMAND(ID_CONFIG_FONTE, OnConfigFonte)
  ON_UPDATE_COMMAND_UI(ID_CONFIG_FONTE, OnUpdateConfigFonte)
  ON_COMMAND(ID_SELECTION_REALTIME, OnRealTime)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_REALTIME, OnUpdateRealTime)
  ON_COMMAND(ID_RESA_RECONNECT, OnReconnect)
  ON_UPDATE_COMMAND_UI(ID_RESA_RECONNECT, OnUpdateReconnect)
  ON_COMMAND(ID_HISTO_LINK, OnHistoLink)
  ON_UPDATE_COMMAND_UI(ID_HISTO_LINK, OnUpdateHistoLink)
  ON_COMMAND(ID_WAITING_VNAU, OnWaitingVnau)
  ON_UPDATE_COMMAND_UI(ID_WAITING_VNAU, OnUpdateWaitingVnau)
  ON_COMMAND(ID_ENTITY_STATS, OnEntityStats)
  ON_UPDATE_COMMAND_UI(ID_ENTITY_STATS, OnUpdateEntityStats)
  ON_COMMAND(ID_TEST_RRD, OnTestRrd)
  ON_UPDATE_COMMAND_UI(ID_TEST_RRD, OnUpdateTestRrd)
  ON_COMMAND(ID_GRAPH_FLASH_LABEL, OnGraphDisplayFlashLabels)
  ON_UPDATE_COMMAND_UI(ID_GRAPH_FLASH_LABEL, OnUpdateGraphDisplayFlashLabels)
  ON_COMMAND(ID_TRANCHE_NEXT, OnTrancheNext)
  ON_UPDATE_COMMAND_UI(ID_TRANCHE_NEXT, OnUpdateTrancheNext)
  ON_COMMAND(ID_TRANCHE_PREVIOUS, OnTranchePrev)
  ON_UPDATE_COMMAND_UI(ID_TRANCHE_PREVIOUS, OnUpdateTranchePrev)
  ON_COMMAND(ID_TRANCHE_CRITICAL_MESSAGES,  OnTrancheCriticalMessages)
  ON_UPDATE_COMMAND_UI(ID_TRANCHE_CRITICAL_MESSAGES, OnUpdateTrancheCriticalMessages)
  ON_COMMAND(ID_TRANCHE_SORT_CRITICALITY, OnTrancheSortCriticality)
  ON_COMMAND(ID_TRANCHE_SORT_DATE_DPT, TrancheSortDptTrain)
  ON_COMMAND(ID_TRANCHE_SORT_DATE_ARR, TrancheSortArrTrain)
  ON_COMMAND(ID_TRANCHE_SORT_TRAIN_DATE, OnTrancheSortTrainDate)
  //ON_COMMAND(ID_TRANCHE_SORT_LEVEL_DATE_TIME, OnTrancheSortLevelDateTime) // NPI - DM7978 : souplesse tarifaire lot 2
  ON_UPDATE_COMMAND_UI(ID_TRANCHE_SORT_CRITICALITY, OnUpdateTrancheSortCrit)
  ON_UPDATE_COMMAND_UI(ID_TRANCHE_SORT_DATE_DPT, OnUpdateTrancheSortDptTrain)
  ON_UPDATE_COMMAND_UI(ID_TRANCHE_SORT_DATE_ARR, OnUpdateTrancheSortArrTrain)
  ON_UPDATE_COMMAND_UI(ID_TRANCHE_SORT_TRAIN_DATE, OnUpdateTrancheSortTrain)
  //ON_UPDATE_COMMAND_UI(ID_TRANCHE_SORT_LEVEL_DATE_TIME, OnUpdateTrancheSortLevel) // NPI - DM7978 : souplesse tarifaire lot 2
  ON_COMMAND(ID_TRANCHE_COMPTAGES_DEFINE_DATE, OnTrancheDefineComptagesRefDate)
  ON_UPDATE_COMMAND_UI(ID_TRANCHE_COMPTAGES_DEFINE_DATE, OnUpdateTrancheDefineComptagesRefDate)
  ON_COMMAND(ID_TRANCHE_ADAPTATION, OnTrancheAdaptation)
  ON_UPDATE_COMMAND_UI(ID_TRANCHE_ADAPTATION, OnUpdateTrancheAdaptation)
  ON_COMMAND(ID_TRANCHE_DEFAULT_TIME, OnTrancheDefaultTime)
  ON_UPDATE_COMMAND_UI(ID_TRANCHE_DEFAULT_TIME, OnUpdateTrancheDefaultTime)
  ON_COMMAND(ID_TRANCHE_DEPARTURE_TIME, OnTrancheDptTime)
  ON_UPDATE_COMMAND_UI(ID_TRANCHE_DEPARTURE_TIME, OnUpdateTrancheDptTime)
  ON_COMMAND(ID_TRANCHE_ARRIVAL_TIME, OnTrancheArrTime)
  ON_UPDATE_COMMAND_UI(ID_TRANCHE_ARRIVAL_TIME, OnUpdateTrancheArrTime)
  ON_COMMAND(ID_TRANCHE_HELP_DLG,  OnTrancheHelp)
  ON_UPDATE_COMMAND_UI(ID_TRANCHE_HELP_DLG,  OnUpdateTrancheHelp)
  ON_COMMAND(ID_LEG_HELP_DLG,  OnLegHelp)
  ON_UPDATE_COMMAND_UI(ID_LEG_HELP_DLG,  OnUpdateLegHelp)
  ON_COMMAND(ID_LEG_SEND, OnLegSend)
  ON_COMMAND(ID_LEG_SEND_NOQUERY, OnLegSendNoQuery)
  ON_UPDATE_COMMAND_UI(ID_LEG_SEND, OnUpdateLegSend)
  ON_COMMAND(ID_LEG_UNDO,OnLegUndo)
  ON_UPDATE_COMMAND_UI(ID_LEG_UNDO, OnUpdateLegUndo)
  ON_COMMAND(ID_LEG_CRITICAL_MESSAGES,  OnLegCriticalMessages)
  ON_UPDATE_COMMAND_UI(ID_LEG_CRITICAL_MESSAGES, OnUpdateLegCriticalMessages)
  ON_COMMAND(ID_LEG_NEXT,OnLegNext)
  ON_UPDATE_COMMAND_UI(ID_LEG_NEXT, OnUpdateLegNext)
  ON_COMMAND(ID_LEG_PREVIOUS,OnLegPrev)
  ON_UPDATE_COMMAND_UI(ID_LEG_PREVIOUS, OnUpdateLegPrev)
  ON_COMMAND(ID_LEG_PASTE_AUTH_VNAU,OnLegPasteAuthSelectedVnau)
  ON_UPDATE_COMMAND_UI(ID_LEG_PASTE_AUTH_VNAU, OnUpdateLegPasteAuthSelected)
  ON_COMMAND(ID_LEG_PASTE_AUTH_VNAS,OnLegPasteAuthSelectedVnas)
  ON_UPDATE_COMMAND_UI(ID_LEG_PASTE_AUTH_VNAS, OnUpdateLegPasteAuthSelectedVnas)
  ON_COMMAND(ID_LEG_PASTE_AUTH_LES2,OnLegPasteAuthSelectedLes2)
  ON_UPDATE_COMMAND_UI(ID_LEG_PASTE_AUTH_LES2, OnUpdateLegPasteAuthSelectedVnas)
  ON_COMMAND(ID_LEG_SELECT_DLG_VNAU,OnLegPasteAuthListVnau)
  ON_UPDATE_COMMAND_UI(ID_LEG_SELECT_DLG_VNAU, OnUpdateLegPasteAuthList)
  ON_COMMAND(ID_LEG_SELECT_DLG_VNAS,OnLegPasteAuthListVnas)
  ON_UPDATE_COMMAND_UI(ID_LEG_SELECT_DLG_VNAS, OnUpdateLegPasteAuthListVnas)
  ON_COMMAND(ID_LEG_SELECT_DLG_LES2, OnLegPasteAuthListLes2)
  ON_UPDATE_COMMAND_UI(ID_LEG_SELECT_DLG_LES2, OnUpdateLegPasteAuthListVnas)
  ON_COMMAND(ID_LEG_PASTE_AUTH_ALL_VNAU,OnLegPasteAuthAllVnau)
  ON_UPDATE_COMMAND_UI(ID_LEG_PASTE_AUTH_ALL_VNAU, OnUpdateLegPasteAuthAll)
  ON_COMMAND(ID_LEG_PASTE_AUTH_ALL_VNAS,OnLegPasteAuthAllVnas)
  ON_UPDATE_COMMAND_UI(ID_LEG_PASTE_AUTH_ALL_VNAS, OnUpdateLegPasteAuthAllVnas)
  ON_COMMAND(ID_LEG_PASTE_AUTH_ALL_LES2,OnLegPasteAuthAllLes2)
  ON_UPDATE_COMMAND_UI(ID_LEG_PASTE_AUTH_ALL_LES2, OnUpdateLegPasteAuthAllVnas)
  ON_COMMAND(ID_LEG_AUTO_COPY_AUTH,OnLegAutoCopyAuth)
  ON_UPDATE_COMMAND_UI(ID_LEG_AUTO_COPY_AUTH, OnUpdateLegAutoCopyAuth)
  ON_COMMAND(ID_LEG_DEFAULTSORT,OnLegDefaultSort)
  ON_UPDATE_COMMAND_UI(ID_LEG_DEFAULTSORT, OnUpdateLegDefaultSort)
  ON_COMMAND(ID_LEG_REVERSESORT,OnLegReverseSort)
  ON_UPDATE_COMMAND_UI(ID_LEG_REVERSESORT, OnUpdateLegReverseSort)
  ON_COMMAND(ID_LEG_SELPEAK,OnLegSelPeak)
  ON_UPDATE_COMMAND_UI(ID_LEG_SELPEAK, OnUpdateLegSelPeak)
  ON_COMMAND(ID_LEG_ADJUSTMENT,OnLegAdjust)
  ON_UPDATE_COMMAND_UI(ID_LEG_ADJUSTMENT, OnUpdateLegAdjust)
  ON_COMMAND(ID_SELECTION_ENTITY, OnSelectionEntity)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_ENTITY, OnUpdateSelectionEntity)
  ON_COMMAND(ID_SELECTION_TRANCHE, OnSelectionTranche)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_TRANCHE, OnUpdateSelectionTranche)
  ON_COMMAND(ID_SELECTION_LEG, OnSelectionLeg)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_LEG, OnUpdateSelectionLeg)
  ON_COMMAND(ID_SELECTION_RESA, OnSelectionResaRail)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_RESA, OnUpdateSelectionResaRail)
  ON_COMMAND(ID_SELECTION_AFF_PMOYEN, OnSelectionAffPMoyen)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_AFF_PMOYEN, OnUpdateSelectionAffPMoyen)
  ON_COMMAND(ID_SELECTION_HISTDPT, OnSelectionHistDpt)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_HISTDPT, OnUpdateSelectionHistDpt)
  ON_COMMAND(ID_SELECTION_HISTDPT_ALT, OnSelectionHistDptAlt)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_HISTDPT_ALT, OnUpdateSelectionHistDptAlt)
  ON_COMMAND(ID_SELECTION_BKTHIST, OnSelectionBktHist)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_BKTHIST, OnUpdateSelectionBktHist)
  ON_COMMAND(ID_SELECTION_BKTHIST_ALT, OnSelectionBktHistAlt)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_BKTHIST_ALT, OnUpdateSelectionBktHistAlt)
  ON_COMMAND(ID_SELECTION_RRDHIST, OnSelectionRRDHist)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_RRDHIST, OnUpdateSelectionRRDHist)
  ON_COMMAND(ID_SELECTION_RRDHIST_ALT, OnSelectionRRDHistAlt)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_RRDHIST_ALT, OnUpdateSelectionRRDHistAlt)
  ON_COMMAND(ID_SELECTION_RECDHIST, OnSelectionRecDHist)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_RECDHIST, OnUpdateSelectionRecDHist)
  ON_COMMAND(ID_SELECTION_RECDHIST_ALT, OnSelectionRecDHistAlt)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_RECDHIST_ALT, OnUpdateSelectionRecDHistAlt)
  ON_COMMAND(ID_SELECTION_OTHLEGS, OnSelectionOthLegs)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_OTHLEGS, OnUpdateSelectionOthLegs)
  ON_COMMAND(ID_SELECTION_OTHLEGS_ALT, OnSelectionOthLegsAlt)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_OTHLEGS_ALT, OnUpdateSelectionOthLegsAlt)
  ON_COMMAND(ID_SELECTION_RELTRAN, OnSelectionRelTranche)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_RELTRAN, OnUpdateSelectionRelTranche)
  ON_COMMAND(ID_SELECTION_RELTRAN_ALT, OnSelectionRelTrancheAlt)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_RELTRAN_ALT, OnUpdateSelectionRelTrancheAlt)
  ON_COMMAND(ID_SELECTION_VIRTNEST, OnSelectionVirtNest)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_VIRTNEST, OnUpdateSelectionVirtNest)
  ON_COMMAND(ID_SELECTION_VNAULIST, OnSelectionVnauList)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_VNAULIST, OnUpdateSelectionVnauList)
  ON_COMMAND(ID_SELECTION_CALENDRIER, OnSelectionCalendar)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_CALENDRIER, OnUpdateSelectionCalendar)
  ON_COMMAND(ID_SELECTION_TRAQUETARIF, OnSelectionTraqueTarif)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_TRAQUETARIF, OnUpdateSelectionTraqueTarif)
  ON_COMMAND(ID_SELECTION_SUIVI_VNAU, OnSelectionSuiviVnau)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_SUIVI_VNAU, OnUpdateSelectionSuiviVnau)
  ON_COMMAND(ID_SELECTION_SUIVI_VNAX, OnSelectionSuiviVnax)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_SUIVI_VNAX, OnUpdateSelectionSuiviVnax)
  ON_COMMAND(ID_SELECTION_SUIVI_TOPIST, OnSelectionSuiviTopIst)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_SUIVI_TOPIST, OnUpdateSelectionSuiviTopIst)
  ON_COMMAND(ID_SELECTION_COMPTAGES, OnSelectionComptages)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_COMPTAGES, OnUpdateSelectionComptages)
  ON_COMMAND(ID_SELECTION_VNAX, OnSelectionVNAX)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_VNAX, OnUpdateSelectionVNAX)
  ON_COMMAND(ID_SELECTION_MOTEUR_VNAX, OnSelectionMoteurVNAX)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_MOTEUR_VNAX, OnUpdateSelectionMoteurVNAX)
  ON_COMMAND(ID_VNAY_LOCAL, OnLocalVNAY)
  ON_UPDATE_COMMAND_UI(ID_VNAY_LOCAL, OnUpdateLocalVNAY)
  ON_COMMAND(ID_VNAY_SERVEUR, OnServeurVNAY)
  ON_UPDATE_COMMAND_UI(ID_VNAY_SERVEUR, OnUpdateServeurVNAY)
  //ON_COMMAND(ID_AUTOMATIC_VNL, OnAutomaticVNL)
  //ON_UPDATE_COMMAND_UI(ID_AUTOMATIC_VNL, OnUpdateAutomaticVNL)
  ON_COMMAND(ID_VNL_ENGINE, OnVNLEngine)
  ON_UPDATE_COMMAND_UI(ID_VNL_ENGINE, OnUpdateVNLEngine)
  ON_COMMAND(ID_DATE_HISTO, OnDatesHisto)
  ON_UPDATE_COMMAND_UI(ID_DATE_HISTO, OnUpdateDatesHisto)
  ON_COMMAND(ID_SELECTION_TRACER, OnSelectionTRACER)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_TRACER, OnUpdateSelectionTRACER)
  ON_COMMAND(ID_VIEW_MANUALVNAU, OnViewMANUALVNAU)
  ON_UPDATE_COMMAND_UI(ID_VIEW_MANUALVNAU, OnUpdateViewMANUALVNAU)
  ON_COMMAND(ID_VIEW_QUICKVNAU, OnViewQUICKVNAU)
  ON_UPDATE_COMMAND_UI(ID_VIEW_QUICKVNAU, OnUpdateViewQUICKVNAU)
  ON_COMMAND(ID_SELECTION_TRAIN_SUPPLEMENT, OnSelectionTrainSupplement)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_TRAIN_SUPPLEMENT, OnUpdateSelectionTrainSupplement)
  ON_COMMAND(ID_SELECTION_TRAIN_ADAPTATION, OnSelectionTrainAdaptation)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_TRAIN_ADAPTATION, OnUpdateSelectionTrainAdaptation)
  ON_COMMAND(ID_SELECTION_TRAIN_DELETED, OnSelectionTrainDeleted)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_TRAIN_DELETED, OnUpdateSelectionTrainDeleted)
  ON_COMMAND(ID_SHOW_WINDOWS_1, OnSelectionShowAllWindows1)
  ON_UPDATE_COMMAND_UI(ID_SHOW_WINDOWS_1, OnUpdateSelectionShowAllWindows1)
  ON_COMMAND(ID_HIDE_WINDOWS_1, OnSelectionHideAllWindows1)
  ON_UPDATE_COMMAND_UI(ID_HIDE_WINDOWS_1, OnUpdateSelectionHideAllWindows1)
  ON_COMMAND(ID_SHOW_WINDOWS_2, OnSelectionShowAllWindows2)
  ON_UPDATE_COMMAND_UI(ID_SHOW_WINDOWS_2, OnUpdateSelectionShowAllWindows2)
  ON_COMMAND(ID_HIDE_WINDOWS_2, OnSelectionHideAllWindows2)
  ON_UPDATE_COMMAND_UI(ID_HIDE_WINDOWS_2, OnUpdateSelectionHideAllWindows2)
  ON_COMMAND(ID_TOGGLE_SHOW_WINDOWS_1, OnSelectionToggleAllWindows1)
  ON_COMMAND(ID_TOGGLE_SHOW_WINDOWS_2, OnSelectionToggleAllWindows2)
  ON_COMMAND(ID_TOGGLE_RRD_DISPLAY, OnSelectionToggleRRDDisplay)
  ON_COMMAND(ID_FRONTBACK_RRDHIST, OnSelectionFrontBackRRDHist)
  ON_COMMAND(ID_WINDOWS_COLOR_1, OnSelectionWindowsColor1)
  ON_COMMAND(ID_WINDOWS_COLOR_DEFAULT_1, OnSelectionWindowsColorDefault1)
  ON_UPDATE_COMMAND_UI(ID_WINDOWS_COLOR_1, OnUpdateSelectionWindowsColor1)
  ON_UPDATE_COMMAND_UI(ID_WINDOWS_COLOR_DEFAULT_1, OnUpdateSelectionWindowsColor1)
  ON_COMMAND(ID_WINDOWS_COLOR_2, OnSelectionWindowsColor2)
  ON_COMMAND(ID_WINDOWS_COLOR_DEFAULT_2, OnSelectionWindowsColorDefault2)
  ON_UPDATE_COMMAND_UI(ID_WINDOWS_COLOR_2, OnUpdateSelectionWindowsColor2)
  ON_UPDATE_COMMAND_UI(ID_WINDOWS_COLOR_DEFAULT_2, OnUpdateSelectionWindowsColor2)
  ON_COMMAND(ID_SELECTION_STN_CODE_TRANSLATION,  OnStnCodeTranslate)
  ON_UPDATE_COMMAND_UI(ID_SELECTION_STN_CODE_TRANSLATION,  OnUpdateStnCodeTranslate)
  ON_COMMAND(ID_WINDOWS_RECETTE_COLOR_1, OnSelectionWindowsRecetteColor1)
  ON_UPDATE_COMMAND_UI(ID_WINDOWS_RECETTE_COLOR_1, OnUpdateSelectionWindowsRecetteColor1)
  ON_COMMAND(ID_WINDOWS_RECETTE_COLOR_2, OnSelectionWindowsRecetteColor2)
  ON_UPDATE_COMMAND_UI(ID_WINDOWS_RECETTE_COLOR_2, OnUpdateSelectionWindowsRecetteColor2)
  ON_COMMAND(ID_FONTS_RECETTE_COLOR_1, OnSelectionFontsRecetteColor1)
  ON_UPDATE_COMMAND_UI(ID_FONTS_RECETTE_COLOR_1, OnUpdateSelectionFontsRecetteColor1)
  ON_COMMAND(ID_FONTS_RECETTE_COLOR_2, OnSelectionFontsRecetteColor2)
  ON_UPDATE_COMMAND_UI(ID_FONTS_RECETTE_COLOR_2, OnUpdateSelectionFontsRecetteColor2)
  ON_COMMAND (ID_YAV_HISTDPT_TOGGLE, OnYavHistDptToggle)
  ON_UPDATE_COMMAND_UI (ID_YAV_HISTDPT_TOGGLE, OnUpdateYavHistDptToggle)
  ON_COMMAND (ID_J_X_ENTITY, OnJmXEntity)
  ON_UPDATE_COMMAND_UI (ID_J_X_ENTITY, OnUpdateJmXEntity)
  ON_WM_SIZE()
  ON_WM_CLOSE()
  ON_WM_TIMER()
  ON_UPDATE_COMMAND_UI (ID_INDICATOR_COMM, OnUpdateComm)
  ON_COMMAND(ID_VISUTRPREV_FAC, OnVisuPrevFac)
  ON_UPDATE_COMMAND_UI(ID_VISUTRPREV_FAC, OnUpdateVisuPrevFac)
  ON_COMMAND(ID_VISUTRPREV_SUS, OnVisuPrevSus)
  ON_UPDATE_COMMAND_UI(ID_VISUTRPREV_SUS, OnUpdateVisuPrevSus)
  ON_COMMAND(ID_VISUTRPREV_FERM, OnVisuFerm)
  ON_UPDATE_COMMAND_UI(ID_VISUTRPREV_FERM, OnUpdateVisuFerm)
//DT10343-LSO-YI10166 Ajout du menu "Actualiser"
  ON_COMMAND(IDS_REFRESH_MENU_TEXT, OnRefresh)
	//}}AFX_MSG_MAP
  ON_MESSAGE(WM_LOAD_TRANCHES, OnLoadTranches)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bActivated = FALSE;

	m_pHelpDlg = NULL;
	m_HelpDlgPoint.x = -1;
	m_HelpDlgPoint.y = -1;

	m_pChartHelpDlg = NULL;
	m_ChartHelpDlgPoint.x = -1;
	m_ChartHelpDlgPoint.y = -1;

	m_iMainShow = SW_SHOW;
	m_iAltShow = SW_SHOW;
	m_bAltLastShow = FALSE;

	m_clrTextBkMain    = ::GetSysColor(COLOR_WINDOW);  // default
	m_clrTextBkAlt     = ::GetSysColor(COLOR_WINDOW);  // default
	m_clrTextRecBkMain = ::GetSysColor(COLOR_WINDOW);  // default
	m_clrTextRecBkAlt  = ::GetSysColor(COLOR_WINDOW);  // default
	m_clrFontRecBkMain = COLOR_RECETTE;
	m_clrFontRecBkAlt  = COLOR_RECETTE;

	m_sendVnauDlg = new CSendVnauDlg();
	m_quickVnauDlg = new CQuickVnauDlg();
}

CMainFrame::~CMainFrame()
{
	delete m_sendVnauDlg;
	delete m_quickVnauDlg;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_CTA_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
      !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
  UINT iId, iStyle;
  int cxWidth;
  m_wndStatusBar.GetPaneInfo(0, iId, iStyle, cxWidth);
  m_wndStatusBar.SetPaneInfo(0, iId, iStyle | SBPS_STRETCH, 500);

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIFrameWnd::PreCreateWindow(cs);
}

HMENU CMainFrame::GetWindowMenuPopup( HMENU hMenuBar )
{
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::ActivateFrame(int nCmdShow) 
{
  if (!m_bActivated) 
  {
    m_bActivated = TRUE;
    nCmdShow = SW_SHOWMAXIMIZED;
  }

	CMDIFrameWnd::ActivateFrame(nCmdShow);
}

void CMainFrame::OnWindowConfigurations() 
{
	// TODO: Add your command handler code here
	APP->GetPersistence()->ConfigurationDialog();
}

void CMainFrame::OnUpdateWindowConfigurations(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( ((CCTAApp*)APP)->GetDocument()->RefDataValid() );
}

void CMainFrame::OnConfigFonte() 
{
  LOGFONT lfont;

  lfont.lfHeight = ((CCTAApp*)APP)->GetDocument()->GetFontHeight();
  lfont.lfWidth = 0;
  lfont.lfEscapement = 0;
  lfont.lfOrientation = 0;
  lfont.lfWeight = ((CCTAApp*)APP)->GetDocument()->GetFontWeight();
  lfont.lfItalic = ((CCTAApp*)APP)->GetDocument()->GetFontItalic();
  lfont.lfUnderline = FALSE;
  lfont.lfStrikeOut = 0;
  lfont.lfCharSet = DEFAULT_CHARSET;
  lfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
  lfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  lfont.lfQuality = DEFAULT_QUALITY;
  lfont.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
  strcpy (lfont.lfFaceName, ((CCTAApp*)APP)->GetDocument()->GetFontName());

  CFontDialog dlgfont (&lfont, CF_SCREENFONTS | CF_FORCEFONTEXIST | CF_NOSCRIPTSEL |
							   CF_INITTOLOGFONTSTRUCT | CF_LIMITSIZE);
  dlgfont.m_cf.nSizeMin = 6;
  dlgfont.m_cf.nSizeMax = 14;

  if (dlgfont.DoModal() == IDOK)
  {
    CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

    CString fn = lfont.lfFaceName;
    pDoc->SetFontName(fn);
    pDoc->SetFontHeight(lfont.lfHeight);
    pDoc->SetFontWeight(lfont.lfWeight);
    pDoc->SetFontItalic(lfont.lfItalic);
    pDoc->ReloadScreenFonts();

    POSITION pos = pDoc->GetFirstViewPosition();
    while( pos != NULL ) 
    {
      CView* pView = pDoc->GetNextView( pos ); 

      // we must manually set the font for each graph, because changing the font in OnPaint causes a flash
      if ( pView->GetSafeHwnd() )
      {
  	    if( pView->IsKindOf(RUNTIME_CLASS(CHistDptView)) ) 
          ((CHistDptView*)pView)->SetGraphFont();
        else
  	    if( pView->IsKindOf(RUNTIME_CLASS(COthLegsView)) ) 
          ((COthLegsView*)pView)->SetGraphFont();
        else
  	    if( pView->IsKindOf(RUNTIME_CLASS(CRelTrancheView)) ) 
          ((CRelTrancheView*)pView)->SetGraphFont();
        else
  	    if( pView->IsKindOf(RUNTIME_CLASS(CRRDHistView)) ) 
          ((CRRDHistView*)pView)->SetGraphFont();
		else
	    if( pView->IsKindOf(RUNTIME_CLASS(CRecDHistView)) ) 
          ((CRecDHistView*)pView)->SetGraphFont();
        else
          pView->Invalidate();
      }
    }
    m_sendVnauDlg->Changefont();
	m_quickVnauDlg->Changefont();
  }
}

void CMainFrame::OnDefaultFonte()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pDoc->SetFontName("MS Sans Serif");
  pDoc->SetFontHeight(0xfffffff5);
  pDoc->SetFontWeight(FW_DONTCARE);
  pDoc->SetFontItalic(0);
  pDoc->ReloadScreenFonts();
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
    CView* pView = pDoc->GetNextView( pos ); 
    // we must manually set the font for each graph, because changing the font in OnPaint causes a flash
    if ( pView->GetSafeHwnd() )
    {
  	  if( pView->IsKindOf(RUNTIME_CLASS(CHistDptView)) ) 
        ((CHistDptView*)pView)->SetGraphFont();
      else
  	  if( pView->IsKindOf(RUNTIME_CLASS(COthLegsView)) ) 
        ((COthLegsView*)pView)->SetGraphFont();
      else
  	  if( pView->IsKindOf(RUNTIME_CLASS(CRelTrancheView)) ) 
        ((CRelTrancheView*)pView)->SetGraphFont();
      else
  	  if( pView->IsKindOf(RUNTIME_CLASS(CRRDHistView)) ) 
        ((CRRDHistView*)pView)->SetGraphFont();
      else
  	  if( pView->IsKindOf(RUNTIME_CLASS(CRecDHistView)) ) 
        ((CRecDHistView*)pView)->SetGraphFont();
      else
        pView->Invalidate();
    }
  }
  m_sendVnauDlg->Changefont();
  m_quickVnauDlg->Changefont();
}

void CMainFrame::OnUpdateConfigFonte(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( TRUE );
}

void CMainFrame::OnRealTime() 
{
  // if comm socket is not opened, attempt to open it
  
  if ( ! ((CCTAApp*)APP)->GetResComm()->GetAcceptedConnection() )
  {
	// try to open the socket
	((CCTAApp*)APP)->GetResComm()->OpenComm(TRUE);

	// we must assume it worked, 
	// if it didn't, when we attempt to send another command,
	//   we turn it back off then
	((CCTAApp*)APP)->SetRealTimeMode(TRUE);  // assume it worked
  }
  else
  {
	((CCTAApp*)APP)->SetRealTimeMode (!((CCTAApp*)APP)->GetRealTimeMode());

	if ( ((CCTAApp*)APP)->GetRealTimeMode() )
	{
	  // find the ResaRail View and send a vnl, which updates the view
	  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument(); 
      POSITION pos = pDoc->GetFirstViewPosition();
      while( pos != NULL ) 
 	  {
        CView* pView = pDoc->GetNextView( pos ); 
	  	if( pView->IsKindOf( RUNTIME_CLASS(CResaRailView) ) ) 
		{
		  ((CResaRailView*)pView)->SendVnl();  // send a vnl
		  break;
		}
	  }
    }
  }
}

void CMainFrame::OnUpdateRealTime(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( TRUE );
  pCmdUI->SetCheck( ((CCTAApp*)APP)->GetRealTimeMode() );
}

void CMainFrame::OnReconnect()
{
  DoReconnect(TRUE);
}
void CMainFrame::DoReconnect(BOOL resetbadtry /* = FALSE*/)
{
	/* Reconnexion automatique */
    ((CCTAApp*)APP)->_nbReconnection++;
    if (((CCTAApp*)APP)->_nbReconnection > 9)
    {
        ((CCTAApp*)APP)->StopReconnectTimer();
        AfxGetMainWnd()->MessageBox ("COMM HS : 10ème tentative de reconnexion échouée.\nArrêt de la reconnexion automatique.\nVous pourrez toujours vous reconnecter via le menu Système > Reconnexion.", "COMM service", MB_OK | MB_ICONSTOP);
    }
	/* Reconnexion automatique */

    ((CCTAApp*)APP)->GetResComm()->PurgeErrorTrace();
    ((CCTAApp*)APP)->GetResComm()->ProcessPending();
    ((CCTAApp*)APP)->GetResComm()->CloseComm();
    ((CCTAApp*)APP)->GetResComm()->OpenComm(resetbadtry);
    //((CCTAApp*)APP)->SetRealTimeMode (TRUE);
    SetTimer (SOCKET_BLOCKING_TIMER, 9 * 1000, NULL); // 9 secondes
}

void CMainFrame::OnUpdateReconnect(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( TRUE );
}

void CMainFrame::OnHistoLink()
{
  // Il faut forcer fermeture et reouverture des fenêtres concernées par les historiques.
  CView* pViewrrdhist = NULL;
  CView* pViewrrdhistalt = NULL;
  CView* pViewrecdhist = NULL;
  CView* pViewrecdhistalt = NULL;
  CView* pViewbkthist = NULL;
  CView* pViewbkthistalt = NULL;
  CView* pViewcomptages = NULL;
  CView* pViewhistdpt = NULL;
  CView* pViewhistdptalt = NULL;
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
	if ( IsRRDHistView( pView, FALSE ) )
	  pViewrrdhist = pView;
	else if ( IsRRDHistView( pView, TRUE ) )
	  pViewrrdhistalt = pView;
	else if ( IsRecDHistView( pView, FALSE ) )
	  pViewrecdhist = pView;
    else if ( IsRecDHistView( pView, TRUE ) )
	  pViewrecdhistalt = pView;
	else if ( IsBktHistView( pView, FALSE ) )
	  pViewbkthist = pView;
	else if ( IsBktHistView( pView, TRUE ) )
	  pViewbkthistalt = pView;
	else if ( IsComptagesView( pView ) )
	  pViewcomptages = pView;
	else if ( IsHistDptView( pView, FALSE ) )
	  pViewhistdpt = pView;
	else if ( IsHistDptView( pView, TRUE ) )
	  pViewhistdptalt = pView;
  }

  if (pViewrrdhist)
	pViewrrdhist->GetParentFrame()->SendMessage (WM_CLOSE);
  if (pViewrrdhistalt)
	pViewrrdhistalt->GetParentFrame()->SendMessage (WM_CLOSE);
  if (pViewrecdhist)
	pViewrecdhist->GetParentFrame()->SendMessage (WM_CLOSE);
  if (pViewrecdhistalt)
	pViewrecdhistalt->GetParentFrame()->SendMessage (WM_CLOSE);
  if (pViewbkthist)
	pViewbkthist->GetParentFrame()->SendMessage (WM_CLOSE);
  if (pViewbkthistalt)
	pViewbkthistalt->GetParentFrame()->SendMessage (WM_CLOSE);
  if (pViewcomptages)
	pViewcomptages->GetParentFrame()->SendMessage (WM_CLOSE);
  if (pViewhistdpt)
	pViewhistdpt->GetParentFrame()->SendMessage (WM_CLOSE);
  if (pViewhistdptalt)
	pViewhistdptalt->GetParentFrame()->SendMessage (WM_CLOSE);

   // Fermer les fenêtres avant de changer le flag.
  BOOL bHistoFlag = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag();
  ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->SetRRDHistoryFlag(!bHistoFlag);

  CString szRegistryKey = ((CCTAApp*)APP)->GetPersistence()->GetCurrentConfiguration();
  APP->WriteProfileInt( szRegistryKey, "HistoData", bHistoFlag ? 0 : 1);

  if (pViewrrdhist)
	OnSelectionRRDHist();
  if (pViewrrdhistalt)
	OnSelectionRRDHistAlt();
  if (pViewrecdhist)
	OnSelectionRecDHist();
  if (pViewrecdhistalt)
	OnSelectionRecDHistAlt();
  if (pViewbkthist)
	OnSelectionBktHist();
  if (pViewbkthistalt)
	OnSelectionBktHistAlt();
  if (pViewcomptages)
	OnSelectionComptages();
  if (pViewhistdpt)
	OnSelectionHistDpt();
  if (pViewhistdptalt)
	OnSelectionHistDptAlt();

}

void CMainFrame::OnUpdateHistoLink(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( TRUE );
  BOOL bHistoFlag = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag();
  pCmdUI->SetCheck(bHistoFlag);
}

void CMainFrame::OnWaitingVnau()
{
  ((CCTAApp*)APP)->VerifUnsentVnau(true);
}

void CMainFrame::OnUpdateWaitingVnau(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( ((CCTAApp*)APP)->GetRhealysFlag() );
}

void CMainFrame::OnYavHistDptToggle()
{
  CView* pViewhistdpt = NULL;
  CView* pViewhistdptalt = NULL;
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos );
    if ( IsHistDptView( pView, FALSE ) )
	  pViewhistdpt = pView;
	else if (IsHistDptView( pView, TRUE ) )
	  pViewhistdptalt = pView;
  }
  if (pViewhistdpt)
	pViewhistdpt->GetParentFrame()->SendMessage (WM_CLOSE);
  if (pViewhistdptalt)
	pViewhistdptalt->GetParentFrame()->SendMessage (WM_CLOSE);

  BOOL byav = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHistoDepLastYearFlag();
  byav = !byav;
  ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->SetHistoDepLastYearFlag(byav);

  if (pViewhistdpt)
	OnSelectionHistDpt();
  if (pViewhistdptalt)
	OnSelectionHistDptAlt();
}

void CMainFrame::OnUpdateYavHistDptToggle( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHistoDepLastYearFlag());
}

void CMainFrame::OnJmXEntity()
{
  CEntityRrd dlg(this);  // Entity Statistics Dialog
  dlg.DoModal();
}

void CMainFrame::OnUpdateJmXEntity(CCmdUI* pCmdUI)
{
   CString sEntities = ((CCTAApp*)APP)->GetDocument()->GetSelectedEntities();
   int curpos = 0;
   BOOL bok = FALSE;
   if (!((CCTAApp*)APP)->GetRhealysFlag())
   {
     CString stok = sEntities.Tokenize ("\',", curpos); 
     while (!stok.IsEmpty())
     {
	   stok.MakeUpper();
	   if (stok.Find("AUTO") == -1) // ce n'est pas une entité autotrain
	     bok = TRUE;
	   stok = sEntities.Tokenize ("\',", curpos); 
     }
   }
   pCmdUI->Enable( bok );
}


void CMainFrame::OnTestRrd()
{ CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pDoc->m_pScStatusSet->Close(); // pour forcer relecture
  pDoc->m_pScStatusSet->Open();
  pDoc->m_pHistoStatusSet->Close(); // pour forcer relecture
  pDoc->m_pHistoStatusSet->Open();
  pDoc->m_pBrmsDateSet->Close(); // pour forcer relecture
  pDoc->m_pBrmsDateSet->Open();
}

void CMainFrame::OnUpdateTestRrd(CCmdUI* pCmdUI)
{
  CString s = ((CCTAApp*)APP)->m_UserRole;
  pCmdUI->Enable( USERALLOWED(s));
}

void CMainFrame::OnUpdateComm(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( TRUE );
}

void CMainFrame::OnEntityStats()
{
  CEntityStatsDlg dlg(this);  // Entity Statistics Dialog
  dlg.DoModal();
}

void CMainFrame::OnUpdateEntityStats(CCmdUI* pCmdUI)
{
	YM_RecordSet* pTrancheSet = 
    YM_Set<YmIcTrancheLisDom>::FromKey( TRANCHE_KEY);

  pCmdUI->Enable( pTrancheSet->IsValid() );
}

void CMainFrame::OnGraphDisplayFlashLabels() 
{
  // toggle the flag to display the graph flash labels
  ((CCTAApp*)APP)->SetFlashLabelFlag(  ! ((CCTAApp*)APP)->GetFlashLabelFlag() );
}

void CMainFrame::OnUpdateGraphDisplayFlashLabels(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( TRUE );
  pCmdUI->SetCheck( ((CCTAApp*)APP)->GetFlashLabelFlag() );
}

////////////////////////////////////////////////////////////////////////
// Set the help dialog and its position
//
void CMainFrame::SetHelpDlg(CDialog* pHelpDlg)
{ 
  ASSERT(pHelpDlg != NULL);

  m_pHelpDlg = pHelpDlg; 
}

// Destroy the general view help dialog
void CMainFrame::DestroyHelpDlg()
{
  if (m_pHelpDlg && m_pHelpDlg->GetSafeHwnd( ))
  {
    m_pHelpDlg->DestroyWindow();
    delete m_pHelpDlg;
    m_pHelpDlg = NULL;
  }
}

void CMainFrame::SetChartHelpDlg(CCTAChartHelpDlg* pChartHelpDlg)
{ 
  ASSERT(pChartHelpDlg != NULL);

  m_pChartHelpDlg = pChartHelpDlg; 
}

////////////////////////////////////////////////////////////////////////
// Note:  The following menu commands are implemented in the CMainFrame
//   class because the Entity Dialog is not a true view, and because 
//   it is not a true view, these messages are not passed to the
//   CCTADoc instance.
//

//////////////////
// Entity List
//
LONG CMainFrame::OnLoadTranches( UINT wParam, LONG lParam )
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  //for (int i=0; i < 100; i++);
  //pDoc->m_pEntitySet->SetCurrent( lParam, pDoc->m_pEntitySet);
  pDoc->m_pEntitySet->SetCurrent( lParam);
  //for (int i=0; i < 100; i++);
  // NPI- Anomalie 57649
  // Appel à la fonction ci-dessous devenu inutile
  // De plus, l'appel au Reader dans cette fonction fait
  // pour une raison inconnue planter SATC...
  //((CCTAApp*)APP)->GetCoeffMatriceCC8();
  ///
  return 0L;
}

void CMainFrame::OnSelectionEntity() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument(); 

	ASSERT_VALID(pDoc);

  ((CCTAApp*)APP)->SaveModifications();
  CEntityDlg dlg(pDoc);
  int nResponse = dlg.DoModal();

  // Did the user click the OK button ?
  if (nResponse == IDOK)
  { 
    // Does the view already exist ?
    if ( ! pDoc->IsViewCreated(RUNTIME_CLASS(CTrancheView)) )
    {
      CString szViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_TRANLIST_KEY);
      ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
    }
  }
}

void CMainFrame::OnUpdateSelectionEntity(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( ((CCTAApp*)APP)->GetDocument()->RefDataValid() );
}

//////////////////
// Tranche List
//
void CMainFrame::OnSelectionTranche() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if ( ! pDoc->IsViewCreated(RUNTIME_CLASS(CTrancheView)) )
  {
      CString szViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_TRANLIST_KEY);
      ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  		CView* pView = pDoc->GetNextView( pos ); 
	  	if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
      {
  			pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionTranche(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( pDoc->IsViewCreated( RUNTIME_CLASS(CTrancheView) ) );
}

void CMainFrame::OnTrancheNext() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
    	((CTrancheView*)pView)->TrancheNext();
      return;
    }
  }
}

void CMainFrame::OnUpdateTrancheNext(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      if( ((CTrancheView*)pView)->GetListCtrl().GetItemCount() > 0 ) 
      {
	      int iIndex = 
          ((CTrancheView*)pView)->GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
        // enable the option if the last item is not selected
	      pCmdUI->Enable( iIndex != ((CTrancheView*)pView)->GetListCtrl().GetItemCount()-1 );
      }
      else
      {
        pCmdUI->Enable( FALSE );
      }
      return;
    }
  }
}

void CMainFrame::OnTranchePrev() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
    	((CTrancheView*)pView)->TranchePrev();
      return;
    }
  }
}

void CMainFrame::OnUpdateTranchePrev(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      if( ((CTrancheView*)pView)->GetListCtrl().GetItemCount() > 0 ) 
      {
	      int iIndex = 
          ((CTrancheView*)pView)->GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	      pCmdUI->Enable( iIndex > 0 );
      }
      else
      {
        pCmdUI->Enable( FALSE );
      }
      return;
    }
  }
}

void CMainFrame::OnTrancheCriticalMessages() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
    	((CTrancheView*)pView)->TrancheCriticalMessages();
      return;
    }
  }
}

void CMainFrame::OnUpdateTrancheCriticalMessages(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if (pDoc->m_pTrancheSet)
  	pCmdUI->Enable( pDoc->m_pTrancheSet->IsValid() );
  else
    pCmdUI->Enable( FALSE );

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
      pCmdUI->SetCheck( ((CTrancheView*)pView)->GetTooltipsToggle() );
  }
}

void CMainFrame::OnTrancheSortCriticality() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
    	((CTrancheView*)pView)->TrancheSortCriticality();
      return;
    }
  }
}

void CMainFrame::TrancheSortDptTrain() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      ((CTrancheView*)pView)->TrancheSortDptTrain();
      return;
    }
  }
}
void CMainFrame::TrancheSortArrTrain() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      ((CTrancheView*)pView)->TrancheSortArrTrain();
      return;
    }
  }
}
void CMainFrame::OnTrancheSortDate(BOOL bDpt /* = TRUE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      (bDpt) ? 
        ((CTrancheView*)pView)->TrancheSortDptTrain() : 
        ((CTrancheView*)pView)->TrancheSortArrTrain();
      return;
    }
  }
}

void CMainFrame::OnTrancheSortTrainDate() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
    	((CTrancheView*)pView)->TrancheSortTrainDate();
      return;
    }
  }
}

// NPI - DM7978 : souplesse tarifaire lot 2
/*void CMainFrame::OnTrancheSortLevelDateTime() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
    	((CTrancheView*)pView)->TrancheSortLevelDateTime();
      return;
    }
  }
}*/
///


void CMainFrame::OnUpdateTrancheSortCrit(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if (pDoc->m_pTrancheSet)
  	pCmdUI->Enable( pDoc->m_pTrancheSet->IsValid() );
  else
    pCmdUI->Enable( FALSE );

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      pCmdUI->SetCheck( ((CTrancheView*)pView)->IsSortOnCriticality() );
      return;
    }
  }
}



void CMainFrame::OnTrancheDefineComptagesRefDate() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      ((CTrancheView*)pView)->TrancheDefineComptagesRefDate();
      return;
    }
  }
}
void CMainFrame::OnUpdateTrancheDefineComptagesRefDate(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if (pDoc->m_pTrancheSet)
  	pCmdUI->Enable( pDoc->m_pTrancheSet->IsValid() );
  else
    pCmdUI->Enable( FALSE );
}

void CMainFrame::OnTrancheAdaptation()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      ((CTrancheView*)pView)->TrancheAdaptation();
      return;
    }
  }
}
void CMainFrame::OnUpdateTrancheAdaptation(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if (pDoc->m_pTrancheSet)
  	pCmdUI->Enable( pDoc->m_pTrancheSet->IsValid() );
  else
    pCmdUI->Enable( FALSE );
}


void CMainFrame::OnUpdateTrancheSortDptTrain(CCmdUI* pCmdUI) 
{
  OnUpdateTrancheSortDate(pCmdUI, TRUE);
}
void CMainFrame::OnUpdateTrancheSortArrTrain(CCmdUI* pCmdUI) 
{
  OnUpdateTrancheSortDate(pCmdUI, FALSE);
}
void CMainFrame::OnUpdateTrancheSortDate(CCmdUI* pCmdUI, BOOL bDpt /* = TRUE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if (pDoc->m_pTrancheSet)
  	pCmdUI->Enable( pDoc->m_pTrancheSet->IsValid() );
  else
    pCmdUI->Enable( FALSE );

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      BOOL bSortOnDepart = ((CTrancheView*)pView)->IsSortOnDateTrainDpt();
      BOOL bSortOnArrival = ((CTrancheView*)pView)->IsSortOnDateTrainArr();
      (bDpt) ?
        pCmdUI->SetCheck( bSortOnDepart ) :
        pCmdUI->SetCheck( bSortOnArrival );
      return;
    }
  }
}

void CMainFrame::OnUpdateTrancheSortTrain(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if (pDoc->m_pTrancheSet)
  	pCmdUI->Enable( pDoc->m_pTrancheSet->IsValid() );
  else
    pCmdUI->Enable( FALSE );

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      pCmdUI->SetCheck( ((CTrancheView*)pView)->IsSortOnTrainDate() );
      return;
    }
  }
}

// NPI - DM7978 : souplesse tarifaire lot 2
/*void CMainFrame::OnUpdateTrancheSortLevel(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if (pDoc->m_pTrancheSet)
  	pCmdUI->Enable( pDoc->m_pTrancheSet->IsValid() );
  else
    pCmdUI->Enable( FALSE );

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      pCmdUI->SetCheck( ((CTrancheView*)pView)->IsSortOnLevel() );
      return;
    }
  }
}*/
///

void CMainFrame::OnTrancheDefaultTime() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
    	((CTrancheView*)pView)->TrancheDefaultTime();
      return;
    }
  }
}

void CMainFrame::OnTrancheDptTime() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
    	((CTrancheView*)pView)->TrancheDptTime();
      return;
    }
  }
}

void CMainFrame::OnUpdateTrancheDefaultTime(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if (pDoc->m_pTrancheSet)
  	pCmdUI->Enable( pDoc->m_pTrancheSet->IsValid() );
  else
    pCmdUI->Enable( FALSE );

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      pCmdUI->SetCheck( 
        ((CTrancheView*)pView)->GetTrancheTimeContext() == CTrancheView::DEFAULT);
      return;
    }
  }
}

void CMainFrame::OnUpdateTrancheDptTime(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if (pDoc->m_pTrancheSet)
  	pCmdUI->Enable( pDoc->m_pTrancheSet->IsValid() );
  else
    pCmdUI->Enable( FALSE );

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      pCmdUI->SetCheck( 
        ((CTrancheView*)pView)->GetTrancheTimeContext() == CTrancheView::DEPARTURE);
      return;
    }
  }
}

void CMainFrame::OnTrancheArrTime() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
    	((CTrancheView*)pView)->TrancheArrTime();
      return;
    }
  }
}

void CMainFrame::OnUpdateTrancheArrTime(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if (pDoc->m_pTrancheSet)
  	pCmdUI->Enable( pDoc->m_pTrancheSet->IsValid() );
  else
    pCmdUI->Enable( FALSE );

  // find the tranche view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
      pCmdUI->SetCheck( 
        ((CTrancheView*)pView)->GetTrancheTimeContext() == CTrancheView::ARRIVAL);
      return;
    }
  }
}

void CMainFrame::OnTrancheHelp()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the tranche view and send it a message
  CView* pView;
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
    	((CTrancheView*)pView)->TrancheHelp();
      return;
    }
  }
}

void CMainFrame::OnUpdateTrancheHelp( CCmdUI* pCmdUI )
{
    pCmdUI->Enable( TRUE );
}

//////////////////
// Leg List
//
BOOL CMainFrame::IsLegView(CView* pView)
{
  BOOL bReturn =  (pView->IsKindOf( RUNTIME_CLASS(CLegView))) && (!pView->IsKindOf( RUNTIME_CLASS(CLegAltView)));
  return bReturn;
}
BOOL CMainFrame::IsLegViewCreated()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsLegView( pView) )
  	  return TRUE;  // View already exists, return TRUE
  }
  return FALSE;
}

void CMainFrame::OnSelectionLeg() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsLegViewCreated() )
  {
    CString szViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_LEGLIST_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  		CView* pView = pDoc->GetNextView( pos ); 
      if( IsLegView(pView) )
      {
  		pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionLeg(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( IsLegViewCreated() );
}


void CMainFrame::OnLegSend()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
    if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegSend();
      return;
    }
  }
}

void CMainFrame::OnLegSendNoQuery()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegSend( FALSE );
      return;
    }
  }
}

void CMainFrame::OnUpdateLegSend(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if (pDoc->m_pLegSet)
  	pCmdUI->Enable( pDoc->m_pLegSet->IsValid() /*&& ((CCTAApp*)APP)->GetRealTimeMode()*/);
  else
    pCmdUI->Enable( FALSE );
}

void CMainFrame::OnLegUndo()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegUndo();
      return;
    }
  }
}

void CMainFrame::OnUpdateLegUndo(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if (pDoc->m_pLegSet)
  	pCmdUI->Enable( pDoc->m_pLegSet->IsValid() );
  else
    pCmdUI->Enable( FALSE );
}

void CMainFrame::OnLegCriticalMessages() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegCriticalMessages();
      return;
    }
  }
}

void CMainFrame::OnUpdateLegCriticalMessages(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  if (pDoc->m_pLegSet)
  	pCmdUI->Enable( pDoc->m_pLegSet->IsValid() );
  else
    pCmdUI->Enable( FALSE );

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	if( IsLegView(pView) ) 
      pCmdUI->SetCheck( ((CLegView*)pView)->GetTooltipsToggle() );
    return;
  }
}

void CMainFrame::OnLegNext()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegNext();
      return;
    }
  }
}

void CMainFrame::OnUpdateLegNext(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      if( ((CLegView*)pView)->GetListCtrl().GetItemCount() > 0 ) 
      {
	      int iIndex = 
          ((CLegView*)pView)->GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
        pCmdUI->Enable( iIndex != (((CLegView*)pView)->GetListCtrl().GetItemCount() - 1) );
      }
      else
      {
        pCmdUI->Enable( FALSE );
      }
      return;
    }
  }
}

void CMainFrame::OnLegPrev()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      ((CLegView*)pView)->LegPrev();
      return;
    }
  }
}

void CMainFrame::OnUpdateLegPrev(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      if( ((CLegView*)pView)->GetListCtrl().GetItemCount() > 0 ) 
      {
	      int iIndex = 
          ((CLegView*)pView)->GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
        pCmdUI->Enable( iIndex > 0 );
      }
      else
      {
        pCmdUI->Enable( FALSE );
      }
      return;
    }
  }
}

void CMainFrame::OnLegPasteAuthSelectedVnau()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegPasteAuthSelected(1);
      return;
    }
  }
}

void CMainFrame::OnLegPasteAuthSelectedVnas()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegPasteAuthSelected(2);
      return;
    }
  }
}

void CMainFrame::OnLegPasteAuthSelectedLes2()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegPasteAuthSelected(3);
      return;
    }
  }
}

void CMainFrame::OnUpdateLegPasteAuthSelected(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  BOOL bCopie = FALSE;
  CResaRailView* pResaRailView = GetResaRailView();
  if (pResaRailView && pResaRailView->TrainAuto())
  {
    pCmdUI->Enable(FALSE);
	return;
  }
  
  CTrancheView* pTrancheView = GetTrancheView();
  if (pTrancheView)
  {
    YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
	if (pLoadedTranche)
	  bCopie = pLoadedTranche->Nature() == 0;
  }
  
  // find the leg view's list control
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
		BOOL bTest;
		bTest = ((CLegView*)pView)->GetListCtrl().GetItemCount() > 1;
		int nItemCount = ((CLegView*)pView)->GetListCtrl().GetItemCount();
		bTest = ((CLegView*)pView)->IsSelectedLegLoaded();
		bTest = bCopie;
		bTest = ((CLegView*)pView)->GetAutoCopyAuthFlag();

      pCmdUI->Enable( (((CLegView*)pView)->GetListCtrl().GetItemCount() > 1) &&
						! ((CLegView*)pView)->IsSelectedLegLoaded() && bCopie &&
            ! ((CLegView*)pView)->GetAutoCopyAuthFlag() );
      return;
    }
  }
}

void CMainFrame::OnUpdateLegPasteAuthSelectedVnas(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  BOOL bCopie = FALSE;
  CResaRailView* pResaRailView = GetResaRailView();
  if (pResaRailView && pResaRailView->TrainAuto())
  {
    pCmdUI->Enable(FALSE);
	return;
  }
  
  CTrancheView* pTrancheView = GetTrancheView();
  if (pTrancheView)
  {
    YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
	if (pLoadedTranche)
	{
	  bCopie = pLoadedTranche->Nature() == 0;
	  if (pLoadedTranche->ScxCount() <= 0)
	  {
        pCmdUI->Enable(FALSE);
	    return;
      }
	}
  }
  
  // find the leg view's list control
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      pCmdUI->Enable( (((CLegView*)pView)->GetListCtrl().GetItemCount() > 1) &&
						! ((CLegView*)pView)->IsSelectedLegLoaded() && bCopie &&
            ! ((CLegView*)pView)->GetAutoCopyAuthFlag() );
      return;
    }
  }
}

void CMainFrame::OnLegPasteAuthAllVnau()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegPasteAuthAll(1);
      return;
    }
  }
}

void CMainFrame::OnLegPasteAuthAllVnas()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegPasteAuthAll(2);
      return;
    }
  }
}

void CMainFrame::OnLegPasteAuthAllLes2()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegPasteAuthAll(3);
      return;
    }
  }
}

void CMainFrame::OnUpdateLegPasteAuthAll(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CResaRailView* pResaRailView = GetResaRailView();
  if (pResaRailView && pResaRailView->TrainAuto())
  {
    pCmdUI->Enable(FALSE);
	return;
  }

  BOOL bCopie = FALSE;
  
  CTrancheView* pTrancheView = GetTrancheView();
  if (pTrancheView)
  {
    YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
	if (pLoadedTranche)
		bCopie = pLoadedTranche->Nature() == 0;
  }
  
  // find the leg view's list control
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      pCmdUI->Enable( ((CLegView*)pView)->GetListCtrl().GetItemCount() > 1 && bCopie &&
        ! ((CLegView*)pView)->GetAutoCopyAuthFlag() );
      return;
    }
  }
}

void CMainFrame::OnUpdateLegPasteAuthAllVnas(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CResaRailView* pResaRailView = GetResaRailView();
  if (pResaRailView && pResaRailView->TrainAuto())
  {
    pCmdUI->Enable(FALSE);
	return;
  }

  BOOL bCopie = FALSE;
  
  CTrancheView* pTrancheView = GetTrancheView();
  if (pTrancheView)
  {
    YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
	if (pLoadedTranche)
	{
	  bCopie = pLoadedTranche->Nature() == 0;
	  if (pLoadedTranche->ScxCount() <= 0)
	  {
        pCmdUI->Enable(FALSE);
	    return;
      }
	}
  }
  
  // find the leg view's list control
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      pCmdUI->Enable( ((CLegView*)pView)->GetListCtrl().GetItemCount() > 1 && bCopie &&
        ! ((CLegView*)pView)->GetAutoCopyAuthFlag() );
      return;
    }
  }
}


void CMainFrame::OnLegPasteAuthListVnau()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  
  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegPasteAuthList(1);
      return;
    }
  }
}

void CMainFrame::OnLegPasteAuthListVnas()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  
  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegPasteAuthList(2);
      return;
    }
  }
}

void CMainFrame::OnLegPasteAuthListLes2()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  
  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegPasteAuthList(3);
      return;
    }
  }
}

void CMainFrame::OnUpdateLegPasteAuthList(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CResaRailView* pResaRailView = GetResaRailView();
  if (pResaRailView && pResaRailView->TrainAuto())
  {
    pCmdUI->Enable(FALSE);
	return;
  }

  BOOL bCopie = FALSE;
  
  CTrancheView* pTrancheView = GetTrancheView();
  if (pTrancheView)
  {
    YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
	if (pLoadedTranche)
		bCopie = pLoadedTranche->Nature() == 0;
  }
  
  // find the leg view's list control
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      pCmdUI->Enable( ((CLegView*)pView)->GetListCtrl().GetItemCount() > 1 && bCopie &&
        ! ((CLegView*)pView)->GetAutoCopyAuthFlag() );
      return;
    }
  }
}

void CMainFrame::OnUpdateLegPasteAuthListVnas(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CResaRailView* pResaRailView = GetResaRailView();
  if (pResaRailView && pResaRailView->TrainAuto())
  {
    pCmdUI->Enable(FALSE);
	return;
  }

  BOOL bCopie = FALSE;
  
  CTrancheView* pTrancheView = GetTrancheView();
  if (pTrancheView)
  {
    YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
	if (pLoadedTranche)
	{
	  bCopie = pLoadedTranche->Nature() == 0;
	  if (pLoadedTranche->ScxCount() <= 0)
	  {
        pCmdUI->Enable(FALSE);
	    return;
      }
	}
  }
  
  // find the leg view's list control
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      pCmdUI->Enable( ((CLegView*)pView)->GetListCtrl().GetItemCount() > 1 && bCopie &&
        ! ((CLegView*)pView)->GetAutoCopyAuthFlag() );
      return;
    }
  }
}

void CMainFrame::OnLegAutoCopyAuth()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      // toggle the flag
    	((CLegView*)pView)->SetAutoCopyAuthFlag(! ((CLegView*)pView)->GetAutoCopyAuthFlag());
      return;
    }
  }
}

void CMainFrame::OnUpdateLegAutoCopyAuth(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view's list control
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      pCmdUI->Enable( ((CLegView*)pView)->GetListCtrl().GetItemCount() > 0 );
      pCmdUI->SetCheck( ((CLegView*)pView)->GetAutoCopyAuthFlag() );
      return;
    }
  }
}

void CMainFrame::OnLegDefaultSort()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegDefaultSort();
      return;
    }
  }
}

void CMainFrame::OnUpdateLegDefaultSort(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      pCmdUI->Enable( ((CLegView*)pView)->GetListCtrl().GetItemCount() > 0 );
      pCmdUI->SetCheck( ((CLegView*)pView)->IsSortDefault() ) ;
      return;
    }
  }
}

void CMainFrame::OnLegReverseSort()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegReverseSort();
      return;
    }
  }
}

void CMainFrame::OnUpdateLegReverseSort(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      pCmdUI->Enable( ((CLegView*)pView)->GetListCtrl().GetItemCount() > 0 );
      pCmdUI->SetCheck( ((CLegView*)pView)->IsSortReverse() ) ;
      return;
    }
  }
}

void CMainFrame::OnLegSelPeak()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
    	((CLegView*)pView)->LegSelPeak();
      return;
    }
  }
}

void CMainFrame::OnUpdateLegSelPeak(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      pCmdUI->Enable( ((CLegView*)pView)->GetListCtrl().GetItemCount() > 0 );
      pCmdUI->SetCheck( ((CLegView*)pView)->IsSelPeak() ) ;
      return;
    }
  }
}

void CMainFrame::OnLegAdjust()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the resa rail view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      // set the overbooking adjustment flag in the leg view
      ((CLegView*)pView)->SetOvbAdjustFlag( ! ((CLegView*)pView)->GetOvbAdjustFlag() );
      return;
    }
  }
}

void CMainFrame::OnUpdateLegAdjust(CCmdUI* pCmdUI)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	ASSERT_VALID(pDoc);

  // find the leg view and send it a message
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( IsLegView(pView) ) 
    {
      if( ((CLegView*)pView)->GetListCtrl().GetItemCount() > 0 ) 
      {
		// Ano 146023, doit être disponible quelque soit le tronçon sélectionné
		  pCmdUI->Enable( TRUE );
		  pCmdUI->SetCheck( ((CLegView*)pView)->GetOvbAdjustFlag() );
      }
      else
      {
        pCmdUI->SetCheck( ((CLegView*)pView)->GetOvbAdjustFlag() );  // check accordingly
        pCmdUI->Enable( FALSE );
      }
      return;
    }
  }
  // Pas de legview ouverte, on masque
  pCmdUI->SetCheck(FALSE);
  pCmdUI->Enable(FALSE);
}

void CMainFrame::OnLegHelp()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  // find the leg view and send it a message
  CView* pView;
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CLegView)) ) 
    {
    	((CLegView*)pView)->LegHelp();
      return;
    }
  }
}

void CMainFrame::OnUpdateLegHelp( CCmdUI* pCmdUI )
{
    pCmdUI->Enable( TRUE );
}

void CMainFrame::OnVisuPrevFac()
{
  CTrancheView* pTrancheView = GetTrancheView();
  if (pTrancheView)
	pTrancheView->OnVisuPrevFac();
}

void CMainFrame::OnUpdateVisuPrevFac( CCmdUI* pCmdUI )
{
  CTrancheView* pTrancheView = GetTrancheView();
  if (!pTrancheView || !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrPrevFlag())
  {
    pCmdUI->Enable(FALSE);
	return;
  }
  pCmdUI->SetCheck( pTrancheView->m_bVisuPrevFac );
  pCmdUI->Enable( pTrancheView->m_nbPrevFac > 0 );
}

void CMainFrame::OnVisuPrevSus()
{
  CTrancheView* pTrancheView = GetTrancheView();
  if (pTrancheView)
	pTrancheView->OnVisuPrevSus();
}

void CMainFrame::OnUpdateVisuPrevSus( CCmdUI* pCmdUI )
{
  CTrancheView* pTrancheView = GetTrancheView();
  if (!pTrancheView || !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrPrevFlag())
  {
    pCmdUI->Enable(FALSE);
	return;
  }
  pCmdUI->SetCheck( pTrancheView->m_bVisuPrevSus );
  pCmdUI->Enable( pTrancheView->m_nbPrevSus > 0 );
}

void CMainFrame::OnVisuFerm()
{
  CTrancheView* pTrancheView = GetTrancheView();
  if (pTrancheView)
	pTrancheView->OnVisuFerm();
}

void CMainFrame::OnUpdateVisuFerm( CCmdUI* pCmdUI )
{
  CTrancheView* pTrancheView = GetTrancheView();
  if (!pTrancheView || !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrPrevFlag())
  {
    pCmdUI->Enable(FALSE);
	return;
  }
  pCmdUI->SetCheck( pTrancheView->m_bVisuFerm );
  pCmdUI->Enable( pTrancheView->m_nbFerm > 0 );
}

//////////////////////
// Resa Rail
//
BOOL CMainFrame::IsResaRailView(CView* pView)
{
  BOOL bReturn =  ( pView->IsKindOf( RUNTIME_CLASS(CResaRailView) ));
  return bReturn;
}
BOOL CMainFrame::IsResaRailViewCreated()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsResaRailView( pView) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}
CRRDHistView* CMainFrame::GetRRDHistView(BOOL bAlt /* = FALSE */)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
	if ( IsRRDHistView( pView, bAlt ) )
  	  return (CRRDHistView*) pView; 
  }
  return NULL;
}

CRecDHistView* CMainFrame::GetRecDHistView(BOOL bAlt /* = FALSE */)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
	if ( IsRecDHistView( pView, bAlt ) )
  	  return (CRecDHistView*) pView; 
  }
  return NULL;
}

CTrancheView* CMainFrame::GetTrancheView(BOOL bAssert /*= TRUE*/)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CView* pView = NULL;
  CView* pTrView = NULL;
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	pView = pDoc->GetNextView( pos ); 
	if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) )
	{
	  pTrView = pView;
      break;
	}
  }
  /*if (bAssert)
    ASSERT(pTrView != NULL);*/
  CTrancheView* pTrancheView = (CTrancheView*)pTrView;
  return pTrancheView;
}

CLegView* CMainFrame::GetLegView()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CView* pView = NULL;
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CLegView) ) ) 
      break;
  }
  ASSERT(pView != NULL);
  CLegView* pLegView = (CLegView*)pView;
  return pLegView;
}

CResaRailView* CMainFrame::GetResaRailView()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsResaRailView( pView ) )
  	  return (CResaRailView*) pView; 
  }
  return NULL;
}

CBktHistView* CMainFrame::GetCBktHistView()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( pView->IsKindOf( RUNTIME_CLASS(CBktHistView) ) )
  	  return (CBktHistView*) pView; 
  }
  return NULL;
}

CBktHistAltView* CMainFrame::GetCBktHistAltView()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( pView->IsKindOf( RUNTIME_CLASS(CBktHistAltView) ) )
  	  return (CBktHistAltView*) pView; 
  }
  return NULL;
}

CVnlEngineView*	CMainFrame::GetVnlEngineView()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( pView->IsKindOf( RUNTIME_CLASS(CVnlEngineView) ) )
  	  return (CVnlEngineView*) pView; 
  }
  return NULL;
}



void CMainFrame::WarnOnVNAU (CString sWarn)
{
  sWarn.Trim();
  if (m_sRefTitle.IsEmpty())
  { // premier appel
    GetWindowText(m_sRefTitle);
  }
  if (sWarn != m_sWarnVnau)
  {
    CString szFrameTitle = m_sRefTitle;
	szFrameTitle += "   ";
	szFrameTitle += sWarn;
    m_sWarnVnau = sWarn;
	SetWindowText(szFrameTitle);
  }
}

void CMainFrame::OnSelectionResaRail() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsResaRailViewCreated() )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_RESARAIL_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  		CView* pView = pDoc->GetNextView( pos ); 
      if( IsResaRailView(pView) )
      {
  			pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionResaRail(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( IsResaRailViewCreated() );
}

void CMainFrame::OnSelectionAffPMoyen()
{
  CResaRailView* pResaRailView = GetResaRailView();
  if (pResaRailView)
  {
	pResaRailView->SetBoolPMoyen (!pResaRailView->GetBoolPMoyen());
	pResaRailView->Invalidate();
  }
}

void CMainFrame::OnUpdateSelectionAffPMoyen(CCmdUI* pCmdUI) 
{
  CResaRailView* pResaRailView = GetResaRailView();
  if (!pResaRailView)
  {
    pCmdUI->Enable(FALSE);
	return;
  }
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck (pResaRailView->GetBoolPMoyen());
}


//////////////////////
// History Departure
//
BOOL CMainFrame::IsHistDptView(CView* pView, BOOL bAlt /* = FALSE */)
{
  BOOL bReturn = (bAlt) ? 
    ( pView->IsKindOf( RUNTIME_CLASS(CHistDptAltView) ) ) :
    ( ( pView->IsKindOf( RUNTIME_CLASS(CHistDptView) ) ) && ( ! pView->IsKindOf( RUNTIME_CLASS(CHistDptAltView) ) ) );
  return bReturn;
}
BOOL CMainFrame::IsHistDptViewCreated(BOOL bAlt /* = FALSE */)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsHistDptView( pView, bAlt ) )
  	  return TRUE;  // View already exists, return TRUE
  }
  return FALSE;
}

void CMainFrame::OnSelectionHistDpt() 
{
  OnSelectionHistDptX(FALSE);
}
void CMainFrame::OnSelectionHistDptAlt() 
{
  OnSelectionHistDptX(TRUE);
}
void CMainFrame::OnSelectionHistDptX(BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsHistDptViewCreated(bAlt) )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString((bAlt) ? IDS_CTA_HISTDPT_ALT_KEY : IDS_CTA_HISTDPT_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  		CView* pView = pDoc->GetNextView( pos ); 
      if( IsHistDptView(pView, bAlt) )
      {
  			pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionHistDpt(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionHistDptX(pCmdUI, FALSE);
}
void CMainFrame::OnUpdateSelectionHistDptAlt(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionHistDptX(pCmdUI, TRUE);
}
void CMainFrame::OnUpdateSelectionHistDptX(CCmdUI* pCmdUI, BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();//DM 8029
  pTrancheView->getCurrentTrancheNature();//DM 8029

  pCmdUI->Enable(pDoc->RefDataValid() &&
	  (!((CCTAApp*)APP)->GetRhealysFlag() ||
	  (!bAlt || pTrancheView->getCurrentTrancheNature() == 3)));//DM 8029 - YI-11703: Check the nature to Grey our menu if we are not a nature 3 on ALLEO

  //pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( IsHistDptViewCreated(bAlt) );
}

//////////////////////
// Other Legs
//
BOOL CMainFrame::IsOthLegsView(CView* pView, BOOL bAlt /* = FALSE */)
{
  BOOL bReturn = (bAlt) ? 
    ( pView->IsKindOf( RUNTIME_CLASS(COthLegsAltView) ) ) :
    ( ( pView->IsKindOf( RUNTIME_CLASS(COthLegsView) ) ) && ( ! pView->IsKindOf( RUNTIME_CLASS(COthLegsAltView) ) ) );
  return bReturn;
}

BOOL CMainFrame::IsOthLegsViewCreated(BOOL bAlt /* = FALSE */)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsOthLegsView( pView, bAlt ) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}

void CMainFrame::OnSelectionOthLegs() 
{
  OnSelectionOthLegsX(FALSE);
}
void CMainFrame::OnSelectionOthLegsAlt() 
{
  OnSelectionOthLegsX(TRUE);
}
void CMainFrame::OnSelectionOthLegsX(BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsOthLegsViewCreated(bAlt) )
  {
    CString szViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString((bAlt) ? IDS_CTA_OTHLEGS_ALT_KEY : IDS_CTA_OTHLEGS_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  		CView* pView = pDoc->GetNextView( pos ); 
      if( IsOthLegsView(pView, bAlt) )
      {
  			pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionOthLegs(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionOthLegsX(pCmdUI, FALSE);
}
void CMainFrame::OnUpdateSelectionOthLegsAlt(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionOthLegsX(pCmdUI, TRUE);
}
void CMainFrame::OnUpdateSelectionOthLegsX(CCmdUI* pCmdUI, BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( IsOthLegsViewCreated(bAlt) );
}

//////////////////////
// Related Tranches
//
BOOL CMainFrame::IsRelTrancheView(CView* pView, BOOL bAlt /* = FALSE */)
{
  BOOL bReturn = (bAlt) ? 
    ( pView->IsKindOf( RUNTIME_CLASS(CRelTrancheAltView) ) ) :
    ( ( pView->IsKindOf( RUNTIME_CLASS(CRelTrancheView) ) ) && ( ! pView->IsKindOf( RUNTIME_CLASS(CRelTrancheAltView) ) ) );
  return bReturn;
}
BOOL CMainFrame::IsRelTrancheViewCreated(BOOL bAlt /* = FALSE */)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsRelTrancheView( pView, bAlt ) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}

void CMainFrame::OnSelectionRelTranche() 
{
  OnSelectionRelTrancheX(FALSE);
}
void CMainFrame::OnSelectionRelTrancheAlt() 
{
  OnSelectionRelTrancheX(TRUE);
}
void CMainFrame::OnSelectionRelTrancheX(BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsRelTrancheViewCreated(bAlt) )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString((bAlt) ? IDS_CTA_RELTRANCHE_ALT_KEY : IDS_CTA_RELTRANCHE_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  		CView* pView = pDoc->GetNextView( pos ); 
      if( IsRelTrancheView(pView, bAlt) )
      {
  			pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionRelTranche(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionRelTrancheX(pCmdUI, FALSE);
}
void CMainFrame::OnUpdateSelectionRelTrancheAlt(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionRelTrancheX(pCmdUI, TRUE);
}
void CMainFrame::OnUpdateSelectionRelTrancheX(CCmdUI* pCmdUI, BOOL bAlt /* = FALSE */)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( IsRelTrancheViewCreated(bAlt) );
}

//////////////////////
// RRD History
//
BOOL CMainFrame::IsRRDHistView(CView* pView, BOOL bAlt /* = FALSE */)
{
  BOOL bReturn = (bAlt) ? 
    ( pView->IsKindOf( RUNTIME_CLASS(CRRDHistAltView) ) ) :
    ( ( pView->IsKindOf( RUNTIME_CLASS(CRRDHistView) ) ) && ( ! pView->IsKindOf( RUNTIME_CLASS(CRRDHistAltView) ) ) );
  return bReturn;
}

BOOL CMainFrame::IsRRDHistViewCreated(BOOL bAlt /* = FALSE */)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsRRDHistView( pView, bAlt ) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}

void CMainFrame::OnSelectionRRDHist() 
{
  OnSelectionRRDHistX(FALSE);
}
void CMainFrame::OnSelectionRRDHistAlt() 
{
  OnSelectionRRDHistX(TRUE);
}
void CMainFrame::OnSelectionRRDHistX(BOOL bAlt /* = FALSE */)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsRRDHistViewCreated(bAlt) )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString((bAlt) ? IDS_CTA_RRDHIST_ALT_KEY : IDS_CTA_RRDHIST_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  		CView* pView = pDoc->GetNextView( pos ); 
      if( IsRRDHistView(pView, bAlt) )
      {
  			pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionRRDHist(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionRRDHistX(pCmdUI, FALSE);
}
void CMainFrame::OnUpdateSelectionRRDHistAlt(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionRRDHistX(pCmdUI, TRUE);
}
void CMainFrame::OnUpdateSelectionRRDHistX(CCmdUI* pCmdUI, BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
  pTrancheView->getCurrentTrancheNature();

  pCmdUI->Enable(pDoc->RefDataValid() &&
	  (!((CCTAApp*)APP)->GetRhealysFlag() ||
	  (!bAlt || pTrancheView->getCurrentTrancheNature() == 3)));//DM 8029 - YI-11703: Check the nature to Grey our menu if we are not a nature 3 on ALLEO


  //pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( IsRRDHistViewCreated(bAlt) );
}

//////////////////////
// RecD History
//
BOOL CMainFrame::IsRecDHistView(CView* pView, BOOL bAlt /* = FALSE */)
{
  BOOL bReturn = (bAlt) ? 
    ( pView->IsKindOf( RUNTIME_CLASS(CRecDHistAltView) ) ) :
    ( ( pView->IsKindOf( RUNTIME_CLASS(CRecDHistView) ) ) && ( ! pView->IsKindOf( RUNTIME_CLASS(CRecDHistAltView) ) ) );
  return bReturn;
}
BOOL CMainFrame::IsRecDHistViewCreated(BOOL bAlt /* = FALSE */)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsRecDHistView( pView, bAlt ) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}

void CMainFrame::OnSelectionRecDHist() 
{
  OnSelectionRecDHistX(FALSE);
}
void CMainFrame::OnSelectionRecDHistAlt() 
{
  OnSelectionRecDHistX(TRUE);
}
void CMainFrame::OnSelectionRecDHistX(BOOL bAlt /* = FALSE */)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsRecDHistViewCreated(bAlt) )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString((bAlt) ? IDS_CTA_RECDHIST_ALT_KEY : IDS_CTA_RECDHIST_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  		CView* pView = pDoc->GetNextView( pos ); 
      if( IsRecDHistView(pView, bAlt) )
      {
  			pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionRecDHist(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionRecDHistX(pCmdUI, FALSE);
}
void CMainFrame::OnUpdateSelectionRecDHistAlt(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionRecDHistX(pCmdUI, TRUE);
}
void CMainFrame::OnUpdateSelectionRecDHistX(CCmdUI* pCmdUI, BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( IsRecDHistViewCreated(bAlt) );
}


//////////////////////
// Bkt History
//
BOOL CMainFrame::IsBktHistView(CView* pView, BOOL bAlt /* = FALSE */)
{
  BOOL bReturn = (bAlt) ? 
    ( pView->IsKindOf( RUNTIME_CLASS(CBktHistAltView) ) ) :
    ( ( pView->IsKindOf( RUNTIME_CLASS(CBktHistView) ) ) && ( ! pView->IsKindOf( RUNTIME_CLASS(CBktHistAltView) ) ) );
  return bReturn;
}
BOOL CMainFrame::IsBktHistViewCreated(BOOL bAlt /* = FALSE */)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsBktHistView( pView, bAlt ) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}

void CMainFrame::OnSelectionBktHist() 
{
  OnSelectionBktHistX(FALSE);
}
void CMainFrame::OnSelectionBktHistAlt() 
{
  OnSelectionBktHistX(TRUE);
}
void CMainFrame::OnSelectionBktHistX(BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsBktHistViewCreated(bAlt) )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString((bAlt) ? IDS_CTA_BKTHIST_ALT_KEY : IDS_CTA_BKTHIST_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  		CView* pView = pDoc->GetNextView( pos ); 
      if( IsBktHistView(pView, bAlt) )
      {
  			pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionBktHist(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionBktHistX(pCmdUI, FALSE);
}
void CMainFrame::OnUpdateSelectionBktHistAlt(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionBktHistX(pCmdUI, TRUE);
}
void CMainFrame::OnUpdateSelectionBktHistX(CCmdUI* pCmdUI, BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
  pTrancheView->getCurrentTrancheNature();

  pCmdUI->Enable(pDoc->RefDataValid() &&
	  (!((CCTAApp*)APP)->GetRhealysFlag() ||
	  (!bAlt || pTrancheView->getCurrentTrancheNature() == 3)));//DM 8029 - YI-11703: Check the nature to Grey our menu if we are not a nature 3 on ALLEO

  pCmdUI->SetCheck ( IsBktHistViewCreated(bAlt) );
}

//////////////////////
// Virtual Nesting
//
BOOL CMainFrame::IsVirtNestView(CView* pView)
{
  BOOL bReturn = (pView->IsKindOf( RUNTIME_CLASS(CVirtNestView)));
  return bReturn;
}
BOOL CMainFrame::IsVirtNestViewCreated()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsVirtNestView( pView) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}


void CMainFrame::OnSelectionVirtNest() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsVirtNestViewCreated() )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_VIRTNEST_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  	  CView* pView = pDoc->GetNextView( pos ); 
      if( IsVirtNestView(pView) )
      {
  		pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}


void CMainFrame::OnUpdateSelectionVirtNest(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( IsVirtNestViewCreated() );
}


//////////////////////
// Vnau List
//
BOOL CMainFrame::IsVnauListView(CView* pView)
{
  BOOL bReturn = (pView->IsKindOf( RUNTIME_CLASS(CVnauListView)));
  return bReturn;
}
BOOL CMainFrame::IsVnauListViewCreated()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
	if ( IsVnauListView( pView) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}

//////////////////////
// TraqueTarif
//
BOOL CMainFrame::IsTraqueTarifView(CView* pView)
{
  BOOL bReturn = (pView->IsKindOf( RUNTIME_CLASS(CTraqueTarifView)));
  return bReturn;
}
BOOL CMainFrame::IsTraqueTarifViewCreated()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
	if ( IsTraqueTarifView( pView) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}

//////////////////////
// SuiviVnau
//
BOOL CMainFrame::IsSuiviVnauView(CView* pView)
{
  BOOL bReturn = (pView->IsKindOf( RUNTIME_CLASS(CSuiviVnauView)));
  return bReturn;
}
BOOL CMainFrame::IsSuiviVnauViewCreated()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
	if ( IsSuiviVnauView( pView) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}

//////////////////////
// SuiviTopIst
//
BOOL CMainFrame::IsSuiviTopIstView(CView* pView)
{
  BOOL bReturn = (pView->IsKindOf( RUNTIME_CLASS(CSuiviTopIstView)));
  return bReturn;
}
BOOL CMainFrame::IsSuiviTopIstViewCreated()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
	if ( IsSuiviTopIstView( pView) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}


CSuiviTopIstView* CMainFrame::GetSuiviTopIstView()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CView* pView = NULL;
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CSuiviTopIstView) ) ) 
      break;
  }
  ASSERT(pView != NULL);
  CSuiviTopIstView* pSuiviTopIstView = (CSuiviTopIstView*)pView;
  return pSuiviTopIstView;
}


//////////////////////
// SuiviVnax
//
BOOL CMainFrame::IsSuiviVnaxView(CView* pView)
{
  BOOL bReturn = (pView->IsKindOf( RUNTIME_CLASS(CSuiviVnaxView)));
  return bReturn;
}
BOOL CMainFrame::IsSuiviVnaxViewCreated()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
	if ( IsSuiviVnaxView( pView) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}



void CMainFrame::OnSelectionVnauList() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsVnauListViewCreated() )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_VNAULIST_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  	  CView* pView = pDoc->GetNextView( pos ); 
      if( IsVnauListView(pView) )
      {
  		pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}


void CMainFrame::OnUpdateSelectionVnauList(CCmdUI* pCmdUI) 
{
  CString szClient = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient();
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() /*&& ((szClient == SNCF) || (szClient == SNCF_TRN))*/);
  pCmdUI->SetCheck ( IsVnauListViewCreated() );
}

void CMainFrame::OnSelectionTraqueTarif() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsTraqueTarifViewCreated() )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_TRAQUETARIF_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  	  CView* pView = pDoc->GetNextView( pos ); 
      if( IsTraqueTarifView(pView) )
      {
  		pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionTraqueTarif(CCmdUI* pCmdUI) 
{
  CString szClient = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient();
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid()  && !((CCTAApp*)APP)->GetRhealysFlag() );
  pCmdUI->SetCheck ( IsTraqueTarifViewCreated() );
}

void CMainFrame::OnSelectionSuiviVnau() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsSuiviVnauViewCreated() )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_SUIVI_VNAU_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  	  CView* pView = pDoc->GetNextView( pos ); 
      if( IsSuiviVnauView(pView) )
      {
  		pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionSuiviVnau(CCmdUI* pCmdUI) 
{
  CString szClient = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient();
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid()  && !((CCTAApp*)APP)->GetRhealysFlag() );
  pCmdUI->SetCheck ( IsSuiviVnauViewCreated() );
}

void CMainFrame::OnSelectionSuiviVnax() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsSuiviVnaxViewCreated() )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_SUIVI_VNAX_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  	  CView* pView = pDoc->GetNextView( pos ); 
      if( IsSuiviVnaxView(pView) )
      {
  		pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionSuiviVnax(CCmdUI* pCmdUI) 
{
  CString szClient = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient();
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid()  && !((CCTAApp*)APP)->GetRhealysFlag() );
  pCmdUI->SetCheck ( IsSuiviVnaxViewCreated() );
}


void CMainFrame::OnSelectionSuiviTopIst() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsSuiviTopIstViewCreated() )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_SUIVI_TOPIST_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  	  CView* pView = pDoc->GetNextView( pos ); 
      if( IsSuiviTopIstView(pView) )
      {
  		pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionSuiviTopIst(CCmdUI* pCmdUI) 
{
  CString szClient = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient();
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CTrancheView* pTrancheView = GetTrancheView(FALSE);
  pCmdUI->Enable( pDoc->RefDataValid()  && !((CCTAApp*)APP)->GetRhealysFlag() &&
	              pTrancheView && pTrancheView->IsTrancheLoaded() );
  pCmdUI->SetCheck ( IsSuiviTopIstViewCreated() );
}

//////////////////////
// Vnau List
//
BOOL CMainFrame::IsCalendarView(CView* pView)
{
  BOOL bReturn = (pView->IsKindOf( RUNTIME_CLASS(CCalendarView)));
  return bReturn;
}

BOOL CMainFrame::IsCalendarViewCreated()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
	if ( IsCalendarView( pView) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}


void CMainFrame::OnSelectionCalendar() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsCalendarViewCreated() )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_CALENDRIER_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  	  CView* pView = pDoc->GetNextView( pos ); 
      if( IsCalendarView(pView) )
      {
  		pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}


void CMainFrame::OnUpdateSelectionCalendar(CCmdUI* pCmdUI) 
{
  CString szClient = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient();
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( true);
  pCmdUI->SetCheck ( IsCalendarViewCreated() );
}

CCalendarView* CMainFrame::GetCalendarView()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
	if ( IsCalendarView( pView ) )
  	  return (CCalendarView*) pView; 
  }
  return NULL;
}

//////////////////////
// Comptages
//
BOOL CMainFrame::IsComptagesView(CView* pView)
{
  BOOL bReturn = (pView->IsKindOf( RUNTIME_CLASS(CComptagesView) ) );
  return bReturn;
}
BOOL CMainFrame::IsComptagesViewCreated()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsComptagesView( pView ) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}

void CMainFrame::OnSelectionComptages() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsComptagesViewCreated() )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_COMPTAGES_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  	  CView* pView = pDoc->GetNextView( pos ); 
      if( IsComptagesView(pView) )
      {
  		pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionComptages(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( IsComptagesViewCreated() );
}


//////////////////
// TrainSupplement, TrainAdaptation, TrainDeleted View
//
BOOL CMainFrame::IsTrainSupplementView(CView* pView)
{
  if ( pView->IsKindOf( RUNTIME_CLASS(CTrainAdaptationView) ) )
    return FALSE;
  if ( pView->IsKindOf( RUNTIME_CLASS(CTrainDeletedView) ) )
    return FALSE;
  if ( pView->IsKindOf( RUNTIME_CLASS(CTrainSupplementView) ) )
    return TRUE;
  return FALSE;
}
BOOL CMainFrame::IsTrainSupplementViewCreated()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsTrainSupplementView( pView ) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}
BOOL CMainFrame::IsTrainAdaptationView(CView* pView)
{
  if ( pView->IsKindOf( RUNTIME_CLASS(CTrainAdaptationView) ) )
    return TRUE;
  return FALSE;
}
BOOL CMainFrame::IsTrainAdaptationViewCreated()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsTrainAdaptationView( pView ) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}
BOOL CMainFrame::IsTrainDeletedView(CView* pView)
{
  if ( pView->IsKindOf( RUNTIME_CLASS(CTrainDeletedView) ) )
    return TRUE;
  return FALSE;
}
BOOL CMainFrame::IsTrainDeletedViewCreated()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsTrainDeletedView( pView ) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}

void CMainFrame::OnSelectionTrainSupplement() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsTrainSupplementViewCreated() )
  {
    CString szViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_TRAINSUPPLEMENT_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  		CView* pView = pDoc->GetNextView( pos ); 
      if( IsTrainSupplementView(pView) )
      {
  			pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}
void CMainFrame::OnSelectionTrainAdaptation() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsTrainAdaptationViewCreated() )
  {
    CString szViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_TRAINADAPTATION_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  		CView* pView = pDoc->GetNextView( pos ); 
      if( IsTrainAdaptationView(pView) )
      {
  			pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}
void CMainFrame::OnSelectionTrainDeleted() 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  if ( ! IsTrainDeletedViewCreated() )
  {
    CString szViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_TRAINDELETED_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  		CView* pView = pDoc->GetNextView( pos ); 
      if( IsTrainDeletedView(pView) )
      {
  			pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionTrainSupplement(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( IsTrainSupplementViewCreated() );
}
void CMainFrame::OnUpdateSelectionTrainAdaptation(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( IsTrainAdaptationViewCreated() );
}
void CMainFrame::OnUpdateSelectionTrainDeleted(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( IsTrainDeletedViewCreated() );
}

// SEND VNAX et MOTEUR VNAX
BOOL CMainFrame::IsVnaxView(CView* pView, BOOL bAlt /* = FALSE */)
{
  BOOL bReturn = (bAlt) ? 
    ( pView->IsKindOf( RUNTIME_CLASS(CVnaxAltWnd) ) ) :
    ( ( pView->IsKindOf( RUNTIME_CLASS(CVnaxWnd) ) ) && ( ! pView->IsKindOf( RUNTIME_CLASS(CVnaxAltWnd) ) ) );
  return bReturn;
}

BOOL CMainFrame::IsVnaxViewCreated(BOOL bAlt /* = FALSE */)
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pView = NULL; 
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
		if ( IsVnaxView( pView, bAlt ) )
  	  return TRUE;  // View exists, return TRUE
  }
  return FALSE;
}

void CMainFrame::OnSelectionVNAX() 
{
  OnSelectionVnaxX(FALSE);
}

void CMainFrame::OnSelectionMoteurVNAX() 
{
  OnSelectionVnaxX(TRUE);
}

void CMainFrame::OnSelectionVnaxX(BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  if ( ! IsVnaxViewCreated(bAlt) )
  {
    CString szViewKey = 
      ((CCTAApp*)APP)->GetResource()->LoadResString((bAlt) ? IDS_CTA_MOTEUR_VNAX_KEY : IDS_CTA_VNAX_KEY);
    ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
  }
  else
  {
  	POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  	  CView* pView = pDoc->GetNextView( pos ); 
      if( IsVnaxView(pView, bAlt) )
      {
  		pView->GetParentFrame()->ActivateFrame();
        return;
      }
    }
  }
}

void CMainFrame::OnUpdateSelectionVNAX(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionVnaxX(pCmdUI, FALSE);
}
void CMainFrame::OnUpdateSelectionMoteurVNAX(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionVnaxX(pCmdUI, TRUE);
}
void CMainFrame::OnUpdateSelectionVnaxX(CCmdUI* pCmdUI, BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->Enable( pDoc->RefDataValid()  && !((CCTAApp*)APP)->GetRhealysFlag() );
  pCmdUI->SetCheck ( IsVnaxViewCreated(bAlt) );
}


//////////////////////
// Send VNAY
//
void CMainFrame::OnLocalVNAY() 
{
    CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
    CVnayObj VnayObj;
    CVnayDlg VnayDlg(this, &VnayObj, FALSE);
    if (VnayDlg.DoModal() == IDOK)
    {
      CVnayProcess *VnayProc = new CVnayProcess;
	  // le delete sera fait par l'objet lui-même lors de sa terminaison.
      if (!VnayProc->InitData (&VnayObj))
      {
        delete VnayProc;
        return;
      }
	  BeginWaitCursor();
      bool bb = VnayProc->PrepareData();
	  EndWaitCursor();
      if (bb)
	  {
		VnayProc->LetsGoCorail(); //DM5524, on passe par le même traitement pour TGV et Corail, car les trains Rhealys sont multitranche
	  }
    }
}

void CMainFrame::OnServeurVNAY() 
{
	CVnayOptim *VnayOptimObj = new CVnayOptim;
	// le delete sera fait par l'objet lui-même lors de sa terminaison.
	VnayOptimObj->Start();
}

void CMainFrame::OnUpdateLocalVNAY(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( (USERALLOWED(((CCTAApp*)APP)->m_UserRole) ) && !((CCTAApp*)APP)->GetRhealysFlag());
}

void CMainFrame::OnUpdateServeurVNAY(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( (USERALLOWED(((CCTAApp*)APP)->m_UserRole) ) && !((CCTAApp*)APP)->GetRhealysFlag());
}

//////////////////////
// Automatic VNL
//
void CMainFrame::OnAutomaticVNL() 
{
    CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

    if ( ! pDoc->IsViewCreated(RUNTIME_CLASS(CVnlWnd)) )
    {
      CString szViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_VNL_KEY);
      ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
    }
    else
    {
  	  POSITION pos = pDoc->GetFirstViewPosition();
  	  while( pos != NULL ) 
      {
  		  CView* pView = pDoc->GetNextView( pos ); 
	  	  if( pView->IsKindOf( RUNTIME_CLASS(CVnlWnd) ) ) 
        {
  			  pView->GetParentFrame()->ActivateFrame();
          return;
        }
      }
    }
}

void CMainFrame::OnUpdateAutomaticVNL(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( !((CCTAApp*)APP)->GetRhealysFlag() );

  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->SetCheck ( pDoc->IsViewCreated( RUNTIME_CLASS(CVnlWnd) ) );
}

//////////////////////
// Engine VNL
//
void CMainFrame::OnVNLEngine() 
{
	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
    CCTADoc* pDoc = WinApp->GetDocument();
	CString szViewKey = WinApp->GetResource()->LoadResString(IDS_CTA_VNL_ENGINE_KEY);

    if ( ! pDoc->IsViewCreated(RUNTIME_CLASS(CVnlEngineView)) )
    {
	  // On considère que le premier passage dans l'écran de sélection doit être forcé
	  WinApp->SetAttendEcranSelectionVnl(TRUE);
      WinApp->CreateNewView(szViewKey, pDoc);
	  POSITION pos = pDoc->GetFirstViewPosition();
  	  while( pos != NULL ) 
      {
		CView* pView = pDoc->GetNextView( pos ); 
		if( pView->IsKindOf( RUNTIME_CLASS(CVnlEngineView)) ) 
		{
  			// On tentera d'accéder à l'écran de sélection 
			// si la liste des stations est remplie
			WinApp->SetAttendEcranSelectionVnl(TRUE);
  			pView->GetParentFrame()->ActivateFrame();
			if (WinApp->GetDocument()->GetTgvStationMap().IsEmpty()==FALSE)
			{  
			  ((CVnlEngineView*)pView)->OnSelect();
			}
		}
	  }
    }
    else
    {
  	  POSITION pos = pDoc->GetFirstViewPosition();
  	  while( pos != NULL ) 
      {
  		  CView* pView = pDoc->GetNextView( pos ); 
	  	  if( pView->IsKindOf( RUNTIME_CLASS(CVnlEngineView) ) ) 
		  {
  			// On considère que le premier passage dans l'écran de sélection a déjà eu lieu
			  WinApp->SetAttendEcranSelectionVnl(FALSE);
  			pView->GetParentFrame()->ActivateFrame();


			return;
		  }
      }
    }
}

void CMainFrame::OnUpdateVNLEngine(CCmdUI* pCmdUI) 
{

  pCmdUI->Enable( TRUE );

  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->SetCheck ( pDoc->IsViewCreated( RUNTIME_CLASS(CVnlEngineView) ) );
}


//////////////////////
// Dates Historiques
//
void CMainFrame::OnDatesHisto() 
{
  CDatesHistoDlg dlg;
  if (dlg.DoModal() == IDOK)
  {
	YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	if (dlg.m_rwDptDate.julian() == pTranche->DptDate())
	{
	  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	  
	  // 30/05/06, ano 199734, forcer rechargement des dates historiques en sortie de la boite de dialogue
	  // de changement de date. Pour eviter une requête au changement de tranche, il y a un test en début de
	  // LoadCurHistoDates pour ne rien faire si entité et date de m_CurHistoDates sont déjà celles que l'on demande.
	  ((CCTAApp*)APP)->m_CurHistoDates.DptDate(0);
	  // 23/11/04, ano 116770, initialisation de m_queryDate en vue du chargement des
	  // dates historiques sur changement de tranches     
	  // NPI - DM7870 : Dates de référence
      //((CCTAApp*)APP)->LoadCurHistoDates(pTranche->DptDate(), pTranche->Entity());
      ((CCTAApp*)APP)->SetCurrentEntityNatureID(pTranche->Entity(), pTranche->Nature());
      ((CCTAApp*)APP)->LoadCurHistoDates(pTranche->DptDate());
      ///
	  
	  // MAj montée en charge résa
	  if (pDoc->m_pHistoEvolSet->IsOpen())
	  {
		pDoc->m_pHistoEvolSet->Close();
		pDoc->m_pHistoEvolSet->Open();
	  }
	  if (pDoc->m_pHistoEvolAltSet->IsOpen())
	  {
		pDoc->m_pHistoEvolAltSet->Close();
		pDoc->m_pHistoEvolAltSet->Open();
	  }

	  // Maj montée en charge revenu
	  if (pDoc->m_pHistoEvolRevSet->IsOpen())
	  {
		pDoc->m_pHistoEvolRevSet->Close();
		pDoc->m_pHistoEvolRevSet->Open();
	  }
	  if (pDoc->m_pHistoEvolRevAltSet->IsOpen())
	  {
		pDoc->m_pHistoEvolRevAltSet->Close();
		pDoc->m_pHistoEvolRevAltSet->Open();
	  }

	  // MAJ Comptages view
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() )
	  {
	    if (pDoc->m_pComptagesSet->IsOpen())
		{
		  pDoc->m_pComptagesSet->Close();
		  pDoc->m_pComptagesSet->Open();
		}
	    if (pDoc->m_pComptagesHistSet->IsOpen())
		{
		  pDoc->m_pComptagesHistSet->Close();
		  pDoc->m_pComptagesHistSet->Open();
		}
	    if (pDoc->m_pQuelCmptSet->IsOpen())
		{
		  pDoc->m_pQuelCmptSet->Close();
		  pDoc->m_pQuelCmptSet->Open();
		}
	  }

	  // MAJ BKT History
	  if (pDoc->m_pBktHistSet->IsOpen())
	  {
		pDoc->m_pBktHistSet->Close();
		pDoc->m_pBktHistSet->Open();
	  }
	  if (pDoc->m_pBktHistAltSet->IsOpen())
	  {
		pDoc->m_pBktHistAltSet->Close();
		pDoc->m_pBktHistAltSet->Open();
	  }
	  if (pDoc->m_pBktHistoSet->IsOpen())
	  {
		pDoc->m_pBktHistoSet->Close();
		pDoc->m_pBktHistoSet->Open();
	  }
	  if (pDoc->m_pBktHistoAltSet->IsOpen())
	  {
		pDoc->m_pBktHistoAltSet->Close();
		pDoc->m_pBktHistoAltSet->Open();
	  }
	}
  }
}
  

void CMainFrame::OnUpdateDatesHisto(CCmdUI* pCmdUI) 
{
  CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
  CString szEntity = WinApp->GetDocument()->GetEntities();
  pCmdUI->Enable(WinApp->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
	             !szEntity.IsEmpty() && !((CCTAApp*)APP)->GetRhealysFlag());
}

//////////////////////
// Tracer view for VNL and VNAU
//
void CMainFrame::OnSelectionTRACER() 
{
    CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

    if ( ! pDoc->IsViewCreated(RUNTIME_CLASS(CResTracerView)) )
    {
      CString szViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_TRACER_KEY);
      ((CCTAApp*)APP)->CreateNewView(szViewKey, pDoc);
    }
    else
    {
  	  POSITION pos = pDoc->GetFirstViewPosition();
  	  while( pos != NULL ) 
      {
  		  CView* pView = pDoc->GetNextView( pos ); 
	  	  if( pView->IsKindOf( RUNTIME_CLASS(CResTracerView) ) ) 
        {
  			  pView->GetParentFrame()->ActivateFrame();
          return;
        }
      }
    }
}

void CMainFrame::OnUpdateSelectionTRACER(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck ( pDoc->IsViewCreated( RUNTIME_CLASS(CResTracerView) ) );
}

void CMainFrame::OnViewMANUALVNAU()
{
 
    if (m_sendVnauDlg->GetSafeHwnd() && m_sendVnauDlg->IsWindowVisible())
      m_sendVnauDlg->Hide();
    else
      m_sendVnauDlg->Show();
}

void CMainFrame::OnUpdateViewMANUALVNAU(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( ((CCTAApp*)APP)->GetDocument()->RefDataValid()   &&
	              !((CCTAApp*)APP)->GetRhealysFlag() );
  pCmdUI->SetCheck ((m_sendVnauDlg->GetSafeHwnd() &&
				     m_sendVnauDlg->IsWindowVisible()) ? 1 : 0);
}

void CMainFrame::OnViewQUICKVNAU()
{
    if (m_quickVnauDlg->GetSafeHwnd() && m_quickVnauDlg->IsWindowVisible())
      m_quickVnauDlg->Hide();
    else
      m_quickVnauDlg->Show();
}

void CMainFrame::OnUpdateViewQUICKVNAU(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( ((CCTAApp*)APP)->GetDocument()->RefDataValid()  &&
	              !((CCTAApp*)APP)->GetRhealysFlag()  );
  pCmdUI->SetCheck ((m_quickVnauDlg->GetSafeHwnd() &&
				     m_quickVnauDlg->IsWindowVisible()) ? 1 : 0);
}

// Si la tranche chargée est sans SCI, on force l'affichage des fenêtres CC seules
void CMainFrame::TestTrancheSansSCI()
{
  CTrancheView* pTrancheView = GetTrancheView();
  if (!pTrancheView)
	return;
  YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
  if (!pLoadedTranche)
    return;
  if (pLoadedTranche->ScxCount() == 0)
  { // tranche sans sous-contingents
    if (m_iMainShow == SW_HIDE)		// Les fenêtres CC ne sont pas affichées
      OnSelectionToggleAllWindows1();
    if (m_iAltShow == SW_SHOW)		// Les fenêtres SCI sont affichées
      OnSelectionToggleAllWindows2();
	m_bAltLastShow = 0;
  }
}

void CMainFrame::OnSelectionToggleAllWindows1()
{
  m_iMainShow = ! m_iMainShow;
  (m_iMainShow) ? OnSelectionShowAllWindows1() : OnSelectionHideAllWindows1();
}

void CMainFrame::OnSelectionToggleAllWindows2 ()
{
  m_iAltShow = ! m_iAltShow;
  (m_iAltShow) ? OnSelectionShowAllWindows2() : OnSelectionHideAllWindows2();
}

void CMainFrame::OnSelectionToggleRRDDisplay()
{
  if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
	((CCTAApp*)APP)->m_RRDdisplayMode = RRDDISPLAYALL;
  // Si client sans historique, la touche F5 force le réaffichage de toutes les courbes
  else
  {
    if ( ((CCTAApp*)APP)->m_RRDdisplayMode == RRDDISPLAYALL)
	  ((CCTAApp*)APP)->m_RRDdisplayMode = RRDDISPLAYMEC;
    else if ( ((CCTAApp*)APP)->m_RRDdisplayMode == RRDDISPLAYMEC)
/*	  ((CCTAApp*)APP)->m_RRDdisplayMode = RRDDISPLAYECH;				Suppression de l'étape Historique + EC
    else if ( ((CCTAApp*)APP)->m_RRDdisplayMode == RRDDISPLAYECH) */
	  ((CCTAApp*)APP)->m_RRDdisplayMode = RRDDISPLAYHIS;
    else if ( ((CCTAApp*)APP)->m_RRDdisplayMode == RRDDISPLAYHIS)
	  ((CCTAApp*)APP)->m_RRDdisplayMode = RRDDISPLAYALL;
  }

  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
    CView* pView = pDoc->GetNextView( pos ); 
    if ( pView->GetSafeHwnd() && pView->IsKindOf(RUNTIME_CLASS(CRRDHistView)) ) 
	{
      ((CRRDHistView*)pView)->ChangeRRDDisplay(TRUE); 
	  pos = NULL; // On sort de la boucle car le double affichage est géré par ChangeRRDDisplay
	}
  }

  pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
    CView* pView = pDoc->GetNextView( pos ); 
    if ( pView->GetSafeHwnd() && pView->IsKindOf(RUNTIME_CLASS(CRecDHistView)) ) 
	{
      ((CRecDHistView*)pView)->ChangeRecDDisplay(TRUE); 
	  pos = NULL; // On sort de la boucle car le double affichage est géré par ChangeRRDDisplay
	}
  }
}

void CMainFrame::OnSelectionFrontBackRRDHist()
{
 if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRevenuFlag())
 {
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
    CView* pView = pDoc->GetNextView( pos );
    if ( (pView->IsKindOf(RUNTIME_CLASS(CRRDHistView)) ) ||
         (pView->IsKindOf(RUNTIME_CLASS(CRecDHistView)) ) )
	{
 	    CChildFrame* pFrame = (CChildFrame*)pView->GetParentFrame();
		if ( (pFrame->GetAltFlag() && m_bAltLastShow) ||
 			 (!pFrame->GetAltFlag() && !m_bAltLastShow))
		{
		  if(bFlagFrontBack)
            pView->GetParentFrame()->SetWindowPos(&CWnd::wndBottom,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		  else
            pView->GetParentFrame()->SetWindowPos(&CWnd::wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		}
	}
  }
  if(bFlagFrontBack)
    bFlagFrontBack = FALSE;
  else
    bFlagFrontBack = TRUE;
 }

}

void CMainFrame::OnSelectionShowAllWindows1()
{
  OnSelectionShowAllWindows(FALSE, SW_SHOW);
}
void CMainFrame::OnSelectionShowAllWindows2()
{
  OnSelectionShowAllWindows(TRUE, SW_SHOW);
}
void CMainFrame::OnSelectionHideAllWindows1()
{
  OnSelectionShowAllWindows(FALSE, SW_HIDE);
}
void CMainFrame::OnSelectionHideAllWindows2()
{
  OnSelectionShowAllWindows(TRUE, SW_HIDE);
}
void CMainFrame::OnSelectionShowAllWindows(BOOL bAlt /* = FALSE */, int iShow /* = SW_SHOW */)
{
  if (iShow == SW_SHOW)
	m_bAltLastShow = bAlt;
  else
    m_bAltLastShow = !bAlt;
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
    CChildFrame* pFrame = (CChildFrame*)pView->GetParentFrame();
    (bAlt) ? m_iAltShow = iShow : m_iMainShow = iShow;
	if( pFrame->GetAltFlag() == bAlt ) 
    {
      // don't affect the following windows ...
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
        continue;
	  if( pView->IsKindOf( RUNTIME_CLASS(CLegView) ) ) 
        continue;
	  if( pView->IsKindOf( RUNTIME_CLASS(CResaRailView) ) ) 
        continue;
	  if( pView->IsKindOf( RUNTIME_CLASS(CComptagesView) ) ) 
        continue;
	  if( pView->IsKindOf( RUNTIME_CLASS(CResTracerView) ) ) 
        continue;
	  if( pView->IsKindOf( RUNTIME_CLASS(CVnlEngineView) ) ) 
        continue;
      if( pView->IsKindOf( RUNTIME_CLASS(CVnlWnd) ) ) 
        continue;
      if ( pDoc->IsViewCreated(RUNTIME_CLASS(CVnaxWnd)) )
        continue;
	  if ( pDoc->IsViewCreated(RUNTIME_CLASS(CVnaxAltWnd)) )
        continue;

	  if (iShow != SW_HIDE)
	  {
		WINDOWPLACEMENT WinPlace;
		pView->GetParentFrame()->GetWindowPlacement (&WinPlace);
		if (WinPlace.showCmd == SW_SHOWMINIMIZED)            
		  pView->GetParentFrame()->ShowWindow(SW_SHOWMINIMIZED);
  		else
		{
		  pView->GetParentFrame()->ShowWindow(iShow);
		  pView->SendMessage (WM_SHOWWINDOW, iShow, 100);
		  pView->GetParentFrame()->ActivateFrame();
		  if (pView->IsKindOf( RUNTIME_CLASS(CLegView)))
			InfoStatus (NULL, ((CLegView*)pView)->GetLoadedLeg());
		}
	  }
      else
	  {
		pView->GetParentFrame()->ShowWindow(SW_HIDE);
		//pView->SendMessage (WM_SHOWWINDOW, SW_HIDE, bAlt ? 100 : 0);
		pView->SendMessage (WM_SHOWWINDOW, SW_HIDE, 0); // 0 systématiquement pour garder les requêtes vivantes.
	  }  
    }
	else if ((pView->IsKindOf( RUNTIME_CLASS(CLegView))) && (iShow == SW_HIDE))
	{ // On masque un choix, il faut donc montrer le status de l'autre
	  if (!bAlt && (iShow == SW_HIDE))
	  {
		// on masque choix et il n'y a pas de choix 2, on vide la barre de message
		GetStatusBar()->SetPaneOneAndLock ("");
	  }
	  else
	  InfoStatus (NULL, ((CLegView*)pView)->GetLoadedLeg());
	}
  }
}

void CMainFrame::OnUpdateSelectionShowAllWindows1(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionShowAllWindows(pCmdUI, FALSE, SW_SHOW);
}
void CMainFrame::OnUpdateSelectionShowAllWindows2(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionShowAllWindows(pCmdUI, TRUE, SW_SHOW);
}
void CMainFrame::OnUpdateSelectionHideAllWindows1(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionShowAllWindows(pCmdUI, FALSE, SW_HIDE);
}
void CMainFrame::OnUpdateSelectionHideAllWindows2(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionShowAllWindows(pCmdUI, TRUE, SW_HIDE);
}
void CMainFrame::OnUpdateSelectionShowAllWindows(CCmdUI* pCmdUI, BOOL bAlt /* = FALSE */, int iShow /* = SW_SHOW */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
  pCmdUI->SetCheck( (bAlt) ? (m_iAltShow == iShow) : (m_iMainShow == iShow) );
}

void CMainFrame::OnSelectionWindowsColor1()
{
  OnSelectionWindowsColor(FALSE, FALSE);
}
void CMainFrame::OnSelectionWindowsColorDefault1()
{
  OnSelectionWindowsColor(FALSE, TRUE);
}
void CMainFrame::OnSelectionWindowsColor2()
{
  OnSelectionWindowsColor(TRUE, FALSE);
}
void CMainFrame::OnSelectionWindowsColorDefault2()
{
  OnSelectionWindowsColor(TRUE, TRUE);
}
void CMainFrame::OnSelectionWindowsColor(BOOL bAlt /* = FALSE */, BOOL bDefault /* = FALSE */)
{
  COLORREF clrNew;

  if (bDefault)  // revert to default color
  {
    if (bAlt)  // which one ...
	{
      clrNew = m_clrTextBkAlt = ::GetSysColor(COLOR_WINDOW);
	  m_clrTextRecBkAlt = m_clrTextBkAlt;
	  m_clrFontRecBkAlt = COLOR_RECETTE;
	}
    else
	{
      clrNew = m_clrTextBkMain = ::GetSysColor(COLOR_WINDOW);
      m_clrTextRecBkMain = m_clrTextBkMain;
	  m_clrFontRecBkMain = COLOR_RECETTE;
	}
  }
  else  // change the color ...
  {
    COLORREF clrInit = 0;
    CColorDialog dlgColor( clrInit, 0, this );
    if (dlgColor.DoModal() != IDOK)
      return;

    clrNew = dlgColor.GetColor();  // let the user decide ...
    if (bAlt)  // set one or the other ...
      m_clrTextBkAlt = clrNew;
    else
      m_clrTextBkMain = clrNew;
  }

  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
    CChildFrame* pFrame = (CChildFrame*)pView->GetParentFrame();

	if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
      ((CTrancheView*)pView)->OnAdjustColorSelection(bAlt, clrNew);

	//TSE_Ano 79307 - BEGIN
	//if( pFrame->GetAltFlag() == bAlt ) //TSE_Ano 79307 _ Conmmntée pour activer les changements sur la partie SCi sur la fenêtre Résarail
	// {
	//CME_Ano 79307
	//if( pView->IsKindOf( RUNTIME_CLASS(CLegView) ) ) 
	//	((YM_ListView*)pView)->SetBackgroundColor(clrNew);  // must handle this view manually because of YM_ListView

	if( pView->IsKindOf( RUNTIME_CLASS(CComptagesView) ) ) 
		((YM_ListView*)pView)->SetBackgroundColor(clrNew);  // must handle this view manually because of YM_ListView

	// don't affect the following windows ...
	if( pView->IsKindOf( RUNTIME_CLASS(CResTracerView) ) ) 
		continue;
	if( pView->IsKindOf( RUNTIME_CLASS(CVnlEngineView) ) ) 
		continue;
	if( pView->IsKindOf( RUNTIME_CLASS(CVnlWnd) ) ) 
		continue;
	if ( pDoc->IsViewCreated(RUNTIME_CLASS(CVnaxWnd)) )
		continue;
	if ( pDoc->IsViewCreated(RUNTIME_CLASS(CVnaxAltWnd)) )
		continue;

	// affect the changes ...
	pView->Invalidate(TRUE);
	pView->UpdateWindow();
    //}//TSE_Ano 79307 _ Conmmntée pour activer les changements sur la partie SCi sur la fenêtre Résarail
  }
}
void CMainFrame::OnSelectionWindowsRecetteColor1()
{
  OnSelectionWindowsRecetteColor(FALSE, FALSE);
}
void CMainFrame::OnSelectionWindowsRecetteColor2()
{
  OnSelectionWindowsRecetteColor(TRUE, FALSE);
}
void CMainFrame::OnSelectionWindowsRecetteColor(BOOL bAlt /* = FALSE */, BOOL bDefault /* = FALSE */)
{
  COLORREF clrNew;

  if (bDefault)  // revert to default color
  {
    if (bAlt)  // which one ...
      clrNew = m_clrTextRecBkAlt = ::GetSysColor(COLOR_WINDOW);
    else
      clrNew = m_clrTextRecBkMain = ::GetSysColor(COLOR_WINDOW);
  }
  else  // change the color ...
  {
    COLORREF clrInit = 0;
    CColorDialog dlgColor( clrInit, 0, this );
    if (dlgColor.DoModal() != IDOK)
      return;

    clrNew = dlgColor.GetColor();  // let the user decide ...
    if (bAlt)  // set one or the other ...
      m_clrTextRecBkAlt = clrNew;
    else
      m_clrTextRecBkMain = clrNew;
  }

  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
   
      // affect the changes ...
      pView->Invalidate(TRUE);
      pView->UpdateWindow();
  }
}
void CMainFrame::OnSelectionFontsRecetteColor1()
{
  OnSelectionFontsRecetteColor(FALSE, FALSE);
}
void CMainFrame::OnSelectionFontsRecetteColor2()
{
  OnSelectionFontsRecetteColor(TRUE, FALSE);
}

void CMainFrame::OnSelectionFontsRecetteColor(BOOL bAlt /* = FALSE */, BOOL bDefault /* = FALSE */)
{
  COLORREF clrNew;

  if (bDefault)  // revert to default color
  {
    if (bAlt)  // which one ...
      clrNew = m_clrFontRecBkAlt = ::GetSysColor(COLOR_WINDOW);
    else
      clrNew = m_clrFontRecBkMain = ::GetSysColor(COLOR_WINDOW);
  }
  else  // change the color ...
  {
    COLORREF clrInit = 0;
    CColorDialog dlgColor( clrInit, 0, this );
    if (dlgColor.DoModal() != IDOK)
      return;

    clrNew = dlgColor.GetColor();  // let the user decide ...
    if (bAlt)  // set one or the other ...
      m_clrFontRecBkAlt = clrNew;
    else
      m_clrFontRecBkMain = clrNew;
  }

  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
   
      // affect the changes ...
      pView->Invalidate(TRUE);
      pView->UpdateWindow();
  }
}

void CMainFrame::OnUpdateSelectionWindowsColor1(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionWindowsColor(pCmdUI, FALSE);
}
void CMainFrame::OnUpdateSelectionWindowsColor2(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionWindowsColor(pCmdUI, TRUE);
}
void CMainFrame::OnUpdateSelectionWindowsColor(CCmdUI* pCmdUI, BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
}
void CMainFrame::OnUpdateSelectionWindowsRecetteColor1(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionWindowsRecetteColor(pCmdUI, FALSE);
}
void CMainFrame::OnUpdateSelectionWindowsRecetteColor2(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionWindowsRecetteColor(pCmdUI, TRUE);
}
void CMainFrame::OnUpdateSelectionWindowsRecetteColor(CCmdUI* pCmdUI, BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
}
void CMainFrame::OnUpdateSelectionFontsRecetteColor1(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionFontsRecetteColor(pCmdUI, FALSE);
}
void CMainFrame::OnUpdateSelectionFontsRecetteColor2(CCmdUI* pCmdUI) 
{
  OnUpdateSelectionFontsRecetteColor(pCmdUI, TRUE);
}
void CMainFrame::OnUpdateSelectionFontsRecetteColor(CCmdUI* pCmdUI, BOOL bAlt /* = FALSE */) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
}

void CMainFrame::OnStnCodeTranslate() 
{
  CDlgStnCodeTranslator dlg;
  dlg.DoModal();
}

void CMainFrame::OnUpdateStnCodeTranslate(CCmdUI* pCmdUI) 
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pCmdUI->Enable( pDoc->RefDataValid() );
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{

	YM_PersistentFrame::OnSize(nType, cx, cy);
	
	if (!cx | !cy) return;

	CRect vvRect;
	CRect sbRect (0, 0, 0, 0);
	if (m_wndStatusBar.IsVisible())
	  m_wndStatusBar.GetClientRect (&sbRect);
}

void CMainFrame::OnClose() 
{
  if (((CCTAApp*)APP)->GetRhealysFlag())
    ((CCTAApp*)APP)->VerifUnsentVnau(false);
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument(); 
	POSITION pos = pDoc->GetFirstViewPosition();
	while( pos != NULL )
	{
		CView* pView = pDoc->GetNextView(pos);
		ASSERT_VALID(pView);
		CFrameWnd* pFrame = pView->GetParentFrame();

//	  if( pFrame->IsKindOf( RUNTIME_CLASS(YM_PersistentChildWnd) ) ) 
      if ( ! ((CChildFrame*)pFrame)->CanCloseFrame() )
        return;
  }
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms != NULL )
    if (((CCTAApp*)APP)->GetResComm())
	  ((CCTAApp*)APP)->GetResComm()->CloseComm();
  CMDIFrameWnd::OnClose();
}

// Find the position of the menu item given the text
//
int CMainFrame::GetMenuPos( CMenu* pMenu, int iResId )
{
  // Find the menu item  ( NOTE MENU TEXT IS IN RESOURCE !!! )
  char szMenuText[50];  // this should be big enough
  CString szMenuSearchText = ((CCTAApp*)APP)->GetResource()->LoadResString( iResId );
//	for (int iPos = pMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)  // get text for each menu item
	for (int iPos = 0; (UINT)iPos < pMenu->GetMenuItemCount(); iPos++)  // get text for each menu item
	{
    pMenu->GetMenuString( iPos, szMenuText, 50, MF_BYPOSITION );  // get the text for this item
    if ( ! strcmp( szMenuText, szMenuSearchText ) )  // is this the one ?
      return iPos;  // found it
  }
  return -1;  // did not find it
}

// Modify the System menu for client parameterization
//
void CMainFrame::ModifySystemMenu() 
{
  // MoteurVNL client does not use RRD History window ...
#ifdef MOTEURVNL_APPLICATION  // We must re-compile the application with the Compiler Directive MOTEURVNL for this client
    return;
#endif

  CMenu* pMainMenu = GetMenu();  // get the main menu
  int iGetMenuPos = GetMenuPos( pMainMenu, IDS_SYSTEM_MENU_TEXT );  // find the menu position of the system menu
  if ( iGetMenuPos < 0 )  // This should not happen...if it does, the menu item text has changed and is different from string in resource !
  {
    AfxMessageBox( "Unable to find System Menu Item !!!",  MB_ICONEXCLAMATION | MB_APPLMODAL );
    return;
  }

  int iItemOffset = 0;  // offset increment

  // For ResaRail clients, we must add the "Real Time" and "Reconnection" options

  CMenu* pSubMenu = pMainMenu->GetSubMenu( iGetMenuPos );  // We found the System submenu...now get it
  iGetMenuPos = GetMenuPos( pSubMenu, IDS_REAL_TIME_MENU_TEXT );  // find the sub menu position of this text
  if ( iGetMenuPos >= 0 )
    return;  // already been inserted...get out now

  pSubMenu->InsertMenu( 
    SYSTEM_MENU_ITEM_OFFSET + iItemOffset++, 
    MF_BYPOSITION, 
    ID_SELECTION_REALTIME, 
    ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_REAL_TIME_MENU_TEXT ) );

  pSubMenu->InsertMenu( 
    SYSTEM_MENU_ITEM_OFFSET + iItemOffset++, 
    MF_BYPOSITION, 
    ID_RESA_RECONNECT, 
    ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_RECONNECTION_MENU_TEXT ) );
  
  if (((CCTAApp*)APP)->GetRhealysFlag())
    pSubMenu->InsertMenu( 
      SYSTEM_MENU_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION, 
      ID_WAITING_VNAU, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_WAITING_VNAU ) );

  pSubMenu->InsertMenu( 
    SYSTEM_MENU_ITEM_OFFSET + iItemOffset++, 
    MF_BYPOSITION, 
    ID_HISTO_LINK, 
    ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_HISTO_LINK ) );

    pSubMenu->InsertMenu( 
    SYSTEM_MENU_ITEM_OFFSET + iItemOffset++, 
    MF_BYPOSITION | MF_SEPARATOR );
}

// Modify the Help menu for client parameterization
//
void CMainFrame::ModifyHelpMenu() 
{
  CMenu* pMainMenu = GetMenu();  // get the main menu
  int iGetMenuPos = GetMenuPos( pMainMenu, IDS_HELP_MENU_TEXT );  // find the menu position of the help menu
  if ( iGetMenuPos < 0 )  // This should not happen...if it does, the menu item text has changed and is different from string in resource !
  {
    AfxMessageBox( "Unable to find System Menu Item !!!",  MB_ICONEXCLAMATION | MB_APPLMODAL );
    return;
  }

  int iItemOffset = 1;  // offset increment

  CMenu* pSubMenu = pMainMenu->GetSubMenu( iGetMenuPos );  // We found the System submenu...now get it
  iGetMenuPos = GetMenuPos( pSubMenu, IDS_STN_CODE_TRANSLATION_MENU_TEXT );  // find the sub menu position of this text
  if ( iGetMenuPos >= 0 )
    return;  // already been inserted...get out now

  pSubMenu->InsertMenu( 
    SYSTEM_MENU_ITEM_OFFSET + iItemOffset++, 
    MF_BYPOSITION | MF_SEPARATOR );

  pSubMenu->InsertMenu( 
    HELP_MENU_ITEM_OFFSET + iItemOffset++, 
    MF_BYPOSITION, 
    ID_SELECTION_STN_CODE_TRANSLATION, 
    ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_STN_CODE_TRANSLATION_MENU_TEXT ) );
}

// Insert the basic Windows selections into the menu
//
void CMainFrame::InsertWindowMenus0(CMenu* pSubMenu, int& iItemOffset)
{
  int iItemPos = WINDOW_MENU_LEG_ITEM_OFFSET;

  // Add all the basic windows (Bkt Hist, Cmpt Hist, ...)
  pSubMenu->InsertMenu(       // Leg List
    iItemPos + (iItemOffset++), 
    MF_BYPOSITION, 
    ID_SELECTION_LEG, 
    ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_LEG_MENU_TEXT ) );
  //iItemPos = WINDOW_MENU_BKT_HIST_ITEM_OFFSET;
 
  pSubMenu->InsertMenu(         // Resa-Rail
    iItemPos + (iItemOffset++), 
    MF_BYPOSITION, 
    ID_SELECTION_RESA, 
    ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_RESA_RAIL_MENU_TEXT ) );

  pSubMenu->InsertMenu(         // Virtual Nesting
    iItemPos + (iItemOffset++), 
    MF_BYPOSITION, 
    ID_SELECTION_VIRTNEST, 
    ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_VIRT_NEST_MENU_TEXT ) );

  pSubMenu->InsertMenu(         // Vnau List
    iItemPos + (iItemOffset++), 
    MF_BYPOSITION, 
    ID_SELECTION_VNAULIST, 
    ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_VNAU_LIST_MENU_TEXT ) );

  if ( (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient() == SNCF) ||
	   (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient() == SNCF_TRN) )
	{
      if (!((CCTAApp*)APP)->GetRhealysFlag())
		pSubMenu->InsertMenu( 
          iItemPos + (iItemOffset++), 
          MF_BYPOSITION, 
          ID_SELECTION_TRAQUETARIF, 
          ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_TRAQUETARIF_MENU_TEXT ) );
	}

  pSubMenu->InsertMenu(         // Calendrier
    iItemPos + (iItemOffset++), 
    MF_BYPOSITION, 
    ID_SELECTION_CALENDRIER, 
    ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_CALENDRIER_MENU_TEXT ) );

  if ( (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient() == SNCF) ||
	   (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient() == SNCF_TRN) )
	{
      if (!((CCTAApp*)APP)->GetRhealysFlag())
	  {
		pSubMenu->InsertMenu( 
          iItemPos + (iItemOffset++), 
          MF_BYPOSITION, 
          ID_SELECTION_SUIVI_VNAU, 
          ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_SUIVI_VNAU_MENU_TEXT ) );
		pSubMenu->InsertMenu( 
          iItemPos + (iItemOffset++), 
          MF_BYPOSITION, 
          ID_SELECTION_SUIVI_VNAX, 
          ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_SUIVI_VNAX_MENU_TEXT ) );
		pSubMenu->InsertMenu( 
          iItemPos + (iItemOffset++), 
          MF_BYPOSITION, 
          ID_SELECTION_SUIVI_TOPIST, 
          ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_SUIVI_TOPIST_MENU_TEXT ) );
	  }
	}


  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTRNFlag() )  // Is this a TRN client ?
  {
    if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() )  // Is this a Comptages client ?
    {
      pSubMenu->InsertMenu(         // Comptages
        iItemPos + (iItemOffset++), 
        MF_BYPOSITION, 
        ID_SELECTION_COMPTAGES, 
        ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_COMPTAGES_MENU_TEXT ) );
    }
  }
}

void CMainFrame::InsertWindowMenus1(CMenu* pSubMenu, int& iItemOffset, BOOL bAlt /* = FALSE */)
{
  int iItemPos = WINDOW_MENU_BKT_HIST_ITEM_OFFSET;

  // Add all the basic windows (Bkt Hist, Cmpt Hist, ...)
  pSubMenu->InsertMenu( 
    iItemPos + (iItemOffset++), 
    MF_BYPOSITION | MF_SEPARATOR );
  pSubMenu->InsertMenu(       // Bucket History
    iItemPos + (iItemOffset++), 
    MF_BYPOSITION, 
    (bAlt) ? ID_SELECTION_BKTHIST_ALT : ID_SELECTION_BKTHIST, 
	((CCTAApp*)APP)->GetResource()->LoadResString( (bAlt) ? IDS_BKT_HIST_MENU_TEXT_ALT : IDS_BKT_HIST_MENU_TEXT ) );
  pSubMenu->InsertMenu(         // Departure History
    iItemPos + (iItemOffset++), 
    MF_BYPOSITION, 
    (bAlt) ? ID_SELECTION_HISTDPT_ALT : ID_SELECTION_HISTDPT, 
    ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_DPT_HIST_MENU_TEXT ) );

  if (!((CCTAApp*)APP)->GetRhealysFlag() || !bAlt)//DM 8029 - YI-11623: Hide "Other legs" for ALLEO
	pSubMenu->InsertMenu(         // Other Legs
		iItemPos + (iItemOffset++), 
		MF_BYPOSITION, 
		(bAlt) ? ID_SELECTION_OTHLEGS_ALT : ID_SELECTION_OTHLEGS, 
		((CCTAApp*)APP)->GetResource()->LoadResString( IDS_OTHER_LEGS_MENU_TEXT ) );

  if (!((CCTAApp*)APP)->GetRhealysFlag() || !bAlt)//DM 8029 - YI-11623: Hide "Related Tranches" for ALLEO
	pSubMenu->InsertMenu(         // Related Tranches
		iItemPos + (iItemOffset++), 
		MF_BYPOSITION, 
		(bAlt) ? ID_SELECTION_RELTRAN_ALT : ID_SELECTION_RELTRAN, 
		((CCTAApp*)APP)->GetResource()->LoadResString( IDS_REL_TRAN_MENU_TEXT ) );

  pSubMenu->InsertMenu(         // RRD History
    iItemPos + (iItemOffset++), 
    MF_BYPOSITION, 
    (bAlt) ? ID_SELECTION_RRDHIST_ALT : ID_SELECTION_RRDHIST, 
    ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_RRD_HIST_MENU_TEXT ) );
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRevenuFlag() )  // Is this a Virt Nesting Window client ?
  {
    pSubMenu->InsertMenu(         // RECD History
      iItemPos + (iItemOffset++), 
      MF_BYPOSITION, 
      (bAlt) ? ID_SELECTION_RECDHIST_ALT : ID_SELECTION_RECDHIST, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_RECD_HIST_MENU_TEXT ) );
  }
  
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSCXFlag() )  // Is this a TRN client ?
  {    
    pSubMenu->InsertMenu( 
    iItemPos + (iItemOffset++), 
    MF_BYPOSITION | MF_SEPARATOR );

    // Menu items to Show / Hide all windows in this group (e.g., ALT)
    //
    pSubMenu->InsertMenu(         // Show All
      iItemPos + (iItemOffset++), 
      MF_BYPOSITION, 
      (bAlt) ? ID_SHOW_WINDOWS_2 : ID_SHOW_WINDOWS_1, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_SHOW_WINDOWS_MENU_TEXT ) );

    pSubMenu->InsertMenu(         // Hide All
      iItemPos + (iItemOffset++), 
      MF_BYPOSITION, 
      (bAlt) ? ID_HIDE_WINDOWS_2 : ID_HIDE_WINDOWS_1, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_HIDE_WINDOWS_MENU_TEXT ) );

    // Menu items to change window color
    //
    pSubMenu->InsertMenu( 
    iItemPos + (iItemOffset++), 
    MF_BYPOSITION | MF_SEPARATOR );

    pSubMenu->InsertMenu(         // Color
      iItemPos + (iItemOffset++), 
      MF_BYPOSITION, 
      (bAlt) ? ID_WINDOWS_COLOR_2 : ID_WINDOWS_COLOR_1, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_WINDOWS_COLOR_MENU_TEXT ) );
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRevenuFlag() )  // Is this a Virt Nesting Window client ?
    {
      pSubMenu->InsertMenu(         // couleur Windows recette
        iItemPos + (iItemOffset++), 
        MF_BYPOSITION, 
        (bAlt) ? ID_WINDOWS_RECETTE_COLOR_2 : ID_WINDOWS_RECETTE_COLOR_1, 
        ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_WINDOWS_COLOR_RECETTE ) );
      pSubMenu->InsertMenu(         // Couleur font recette
        iItemPos + (iItemOffset++), 
        MF_BYPOSITION, 
        (bAlt) ? ID_FONTS_RECETTE_COLOR_2 : ID_FONTS_RECETTE_COLOR_1, 
        ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_FONTS_COLOR_RECETTE ) );
    }
    pSubMenu->InsertMenu(         // Color
      iItemPos + (iItemOffset++), 
      MF_BYPOSITION, 
      (bAlt) ? ID_WINDOWS_COLOR_DEFAULT_2 : ID_WINDOWS_COLOR_DEFAULT_1, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_WINDOWS_COLOR_DEFAULT_MENU_TEXT ) );
  }
}

// Modify the Window menu for client parameterization
//
void CMainFrame::ModifyWindowMenu() 
{
  // MoteurVNL client does not need modification of the Window Menu ... all menu options exist in the resource.
#ifdef MOTEURVNL_APPLICATION  // We must re-compile the application with the Compiler Directive MOTEURVNL for this client
  return;
#endif

  CMenu* pMainMenu = GetMenu();  // get the main menu
  int iGetMenuPos = GetMenuPos( pMainMenu, IDS_WINDOW_MENU_TEXT );
  if ( iGetMenuPos < 0 )  // This should not happen...if it does, the menu item text has changed and is different from string in resource !
  {
    AfxMessageBox( "Unable to find Window Menu Item !!!",  MB_ICONEXCLAMATION | MB_APPLMODAL );
    return;
  }

  CMenu* pSubMenu = pMainMenu->GetSubMenu( iGetMenuPos );  // We found the Window submenu...now get it

  int iItemOffset = 0;  // offset increment

  iGetMenuPos = GetMenuPos( pSubMenu, IDS_LEG_MENU_TEXT );  // find the sub menu position of this text
  if ( iGetMenuPos >= 0 )
    return;  // already been here...get out now

  RemoveLegAltMenu();  // remove the Leg Alt menu item from the main menu...this is not a TRN client.
  InsertWindowMenus0(pSubMenu, iItemOffset);

  int iItemPos = WINDOW_MENU_LEG_ALT_ITEM_OFFSET;

    // insert this (Popup Menu) item 
    iGetMenuPos = GetMenuPos( pSubMenu, IDS_CHOIX_CC );  // find the sub menu position of this text
    if ( iGetMenuPos >= 0 )
      return;  // already been inserted...get out now

    pSubMenu->InsertMenu(     // insert a separator
      iItemPos + (iItemOffset++), 
      MF_BYPOSITION | MF_SEPARATOR );

    // first selection of windows
    CMenu NewMenu;
    NewMenu.CreateMenu( );
    HMENU hNewMenu = NewMenu.GetSafeHmenu( );
    BOOL b = pSubMenu->InsertMenu( 
        iItemPos + (iItemOffset++), 
        MF_POPUP | MF_BYPOSITION, 
        (UINT)hNewMenu, 
        ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_CHOIX_CC )  );
    InsertWindowMenus1(&NewMenu, iItemOffset, FALSE);  // insert all principal windows in the menu
    hNewMenu = NewMenu.Detach( );

  //DM 8029 - YI-11623 : Activate "Choix Sci" menu for ALLEO client
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSCXFlag() || ((CCTAApp*)APP)->GetRhealysFlag())
  {
    // IS a SCX client...insert 2 pop-up menus, each with a selection of SCX / CC windows
    // second selection of windows
    NewMenu.CreateMenu( );
    hNewMenu = NewMenu.GetSafeHmenu( );
    b = pSubMenu->InsertMenu( 
        iItemPos + (iItemOffset++), 
        MF_POPUP | MF_BYPOSITION, 
        (UINT)hNewMenu, 
        ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_CHOIX_SCI )  );
    InsertWindowMenus1(&NewMenu, iItemOffset, TRUE);  // insert all principal windows in the menu
    hNewMenu = NewMenu.Detach( );
  }
  
  // Is this a ResaRail client ?
  // For ResaRail clients, we must add all VNL, VNAU and VNAX window options
  
    iGetMenuPos = GetMenuPos( pSubMenu, IDS_VNL_VNAU_TRACER_MENU_TEXT );  // find the sub menu position of this text
    if ( iGetMenuPos >= 0 )
      return;  // already been inserted...get out now

	pSubMenu->InsertMenu(         // Resa-Rail
      WINDOW_MENU_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION, 
      ID_SELECTION_AFF_PMOYEN, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_AFF_PMOYEN_MENU_TEXT ) );

    pSubMenu->InsertMenu( 
      WINDOW_MENU_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION | MF_SEPARATOR );

	pSubMenu->InsertMenu( 
      WINDOW_MENU_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION, 
      ID_SELECTION_TRACER, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_VNL_VNAU_TRACER_MENU_TEXT ) );

    pSubMenu->InsertMenu( 
      WINDOW_MENU_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION, 
      ID_SELECTION_VNAX, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_SEND_VNAX_MENU_TEXT ) );

    if ( 
         ! (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient() == EUROSTAR) &&
         ! (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient() == THALYS)
      )
    {
	  pSubMenu->InsertMenu( 
      WINDOW_MENU_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION, 
      ID_SELECTION_MOTEUR_VNAX, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_MOTEUR_VNAX_MENU_TEXT ) );

	  CMenu NewMenu;
      NewMenu.LoadMenu( IDR_CTA_VNAY_ITEM );
      HMENU hNewMenu = NewMenu.GetSafeHmenu( );
      hNewMenu = NewMenu.Detach( );

      // insert this (Popup Menu) item 
      pSubMenu->InsertMenu( 
        WINDOW_MENU_ITEM_OFFSET + iItemOffset++, 
        MF_POPUP | MF_BYPOSITION, 
        (UINT)hNewMenu, 
        ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_SEND_VNAY_MENU_TEXT )  );
    }

    pSubMenu->InsertMenu( 
      WINDOW_MENU_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION, 
      ID_VIEW_MANUALVNAU, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_MANUAL_VNAU_MENU_TEXT ) );
 
    pSubMenu->InsertMenu( 
      WINDOW_MENU_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION, 
      ID_VIEW_QUICKVNAU, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_MANUAL_QUICK_VNAU_TEXT ) );

	// NPI - Ano 69954
	// La condition aussi est à mettre en commentaire!!
    //if ( ! ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTRNFlag() )
      /// DM-7978 - CLE
	  /*
		pSubMenu->InsertMenu( 
        WINDOW_MENU_ITEM_OFFSET + iItemOffset++, 
        MF_BYPOSITION, 
        ID_AUTOMATIC_VNL, 
        ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_AUTO_VNL_MENU_TEXT ) );
		*/
	///

	pSubMenu->InsertMenu( 
    WINDOW_MENU_ITEM_OFFSET + iItemOffset++, 
    MF_BYPOSITION, 
    ID_VNL_ENGINE, 
    ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_VNL_ENGINE_MENU_TEXT ) );

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrainAdaptFlag() )  // Train Supplement ?
  {
    pSubMenu->InsertMenu( 
    WINDOW_MENU_ITEM_OFFSET + (iItemOffset++), 
    MF_BYPOSITION | MF_SEPARATOR );
    // JMG 16/03/2004, désactivation des fenêtres train supplémentaires et supprimés
	  pSubMenu->InsertMenu( 
        WINDOW_MENU_ITEM_OFFSET + iItemOffset++, 
        MF_BYPOSITION, 
        ID_SELECTION_TRAIN_SUPPLEMENT, 
        ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_TRAIN_SUPPLEMENT_MENU_TEXT ) );

	  pSubMenu->InsertMenu( 
        WINDOW_MENU_ITEM_OFFSET + iItemOffset++, 
        MF_BYPOSITION, 
        ID_SELECTION_TRAIN_DELETED, 
        ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_TRAIN_DELETED_MENU_TEXT ) );

	  pSubMenu->InsertMenu( 
        WINDOW_MENU_ITEM_OFFSET + iItemOffset++, 
        MF_BYPOSITION, 
        ID_SELECTION_TRAIN_ADAPTATION, 
        ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_TRAIN_ADAPTATION_MENU_TEXT ) );
  }

  // Menu items to change window color
  //
  if (! ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSCXFlag() )
  {
    pSubMenu->InsertMenu( 
    WINDOW_MENU_ITEM_OFFSET + (iItemOffset++), 
    MF_BYPOSITION | MF_SEPARATOR );

    pSubMenu->InsertMenu(         // Color
      WINDOW_MENU_ITEM_OFFSET + (iItemOffset++), 
      MF_BYPOSITION, 
      ID_WINDOWS_COLOR_1, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_WINDOWS_COLOR_MENU_TEXT ) );
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRevenuFlag() )  // Is this a Virt Nesting Window client ?
    {
      pSubMenu->InsertMenu(         // couleur Windows recette
        WINDOW_MENU_ITEM_OFFSET + (iItemOffset++), 
        MF_BYPOSITION, 
        ID_WINDOWS_RECETTE_COLOR_1, 
        ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_WINDOWS_COLOR_RECETTE ) );
      pSubMenu->InsertMenu(         // Couleur font recette
        WINDOW_MENU_ITEM_OFFSET + (iItemOffset++), 
        MF_BYPOSITION, 
        ID_FONTS_RECETTE_COLOR_1, 
        ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_FONTS_COLOR_RECETTE ) );
    }
    pSubMenu->InsertMenu(         // Color
      WINDOW_MENU_ITEM_OFFSET + (iItemOffset++), 
      MF_BYPOSITION, 
      ID_WINDOWS_COLOR_DEFAULT_1, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_WINDOWS_COLOR_DEFAULT_MENU_TEXT ) );
  }
}
// Modify the VNL menu for client parameterization
// Résolution anomalie 49789 : masquer les éléments de sous-menu nouveaux
// liés à la DM 7791
// HRI, le 24/10/2013
//
void CMainFrame::ModifyVNLMenu()
{
	  // MoteurVNL client does not use Tranche List ...
#ifdef MOTEURVNL_APPLICATION  // We must re-compile the application with the Compiler Directive MOTEURVNL for this client
  return;
#endif
  // Méthode sans effet dans le contexte SNCF
  // Mais pas pour Alleo
  BOOL flagAffTop = (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTopFlag()) || ((CCTAApp*)APP)->GetRhealysFlag();
  // Donc pour le contexte SNCF TGV + CORAIL + TRAIN_AUTO, on quitte la méthode
  if (flagAffTop == FALSE)
  {
	  return;
  }
  int iItemOffset = 0;  // offset increment
  CMenu* pMainMenu = GetMenu();  // get the main menu
  int iGetMenuPos = GetMenuPos( pMainMenu, IDS_MENU_VNL_VNL );  // find the menu position of this text
  if ( iGetMenuPos < 0 )  // This should not happen...if it does, the menu item text has changed and is different from string in resource !
  {
     return;
  }
  CMenu* pSubMenu = pMainMenu->GetSubMenu( iGetMenuPos );    // We found the VNL submenu...now get it
  if (pSubMenu == NULL) 
  {
	  return;
  }

  // suppression item Criticités
  int iCriticitePos = GetMenuPos( pSubMenu, IDS_MENU_VNL_CRITICITES );  // find the menu position of this text
  if ( iCriticitePos > 0 )
  {
      pSubMenu->DeleteMenu(iCriticitePos, MF_BYPOSITION);
  }
  // suppression item IST
  int iISTPos = GetMenuPos( pSubMenu, IDS_MENU_VNL_IST );  // find the menu position of this text
  if ( iISTPos > 0 )
  {
      pSubMenu->DeleteMenu(iISTPos, MF_BYPOSITION);
  }
  // suppression item Peak leg, alias tronçon dimensionnant
  int iPeakLegPos = GetMenuPos( pSubMenu, IDS_MENU_VNL_PEAK_LEG );  // find the menu position of this text
  if ( iPeakLegPos > 0 )
  {
      pSubMenu->DeleteMenu(iPeakLegPos, MF_BYPOSITION);
  }


}
// Modify the Tranche menu for client parameterization
//
void CMainFrame::ModifyTrancheMenu(BOOL bIsTrancheView /* = FALSE */) 
{
  // MoteurVNL client does not use Tranche List ...
#ifdef MOTEURVNL_APPLICATION  // We must re-compile the application with the Compiler Directive MOTEURVNL for this client
  return;
#endif

  int iItemOffset = 0;  // offset increment
  CMenu* pMainMenu = GetMenu();  // get the main menu
  int iGetMenuPos = GetMenuPos( pMainMenu, IDS_TRANCHE_MENU_TEXT );  // find the menu position of this text
  if ( iGetMenuPos < 0 )  // This should not happen...if it does, the menu item text has changed and is different from string in resource !
  {
    AfxMessageBox( "Unable to find Tranche Menu Item !!!",  MB_ICONEXCLAMATION | MB_APPLMODAL );
    return;
  }
  CMenu* pSubMenu = pMainMenu->GetSubMenu( iGetMenuPos );    // We found the Tranche submenu...now get it

  // These menu items are for all views, not just the Tranche View...exclusive to the Tranche View follow.

  // add the SORT on Supplement Level option for all views (i.e., not just for the tranche view)
  int iSortPos = GetMenuPos( pSubMenu, IDS_TRANCHE_SORT_MENU_TEXT );  // find the menu position of this text
  if ( iSortPos < 0 )  // This should not happen...if it does, the menu item text has changed and is different from string in resource !
  {
    AfxMessageBox( "Unable to find Tranche Sort Menu Item !!!",  MB_ICONEXCLAMATION | MB_APPLMODAL );
    iSortPos = 0;
  }

  // NPI - DM7978 : souplesse tarifaire lot 2
  /*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSupplementLevelFlag() )  // Is this a Supplement Level client ?
  {
    // insert the supplement level sort order into the tranche sort sub menu
    CMenu* pSortSubMenu = pSubMenu->GetSubMenu( iSortPos );    // We found the Tranche Sort submenu...now get it
    CString szMenuText = ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_SUPPLEMENT_LEVEL_ORDER_MENU_TEXT );

    // have we already added the supplement level sort option ?
    int iSortSubPos = GetMenuPos( pSortSubMenu, IDS_SUPPLEMENT_LEVEL_ORDER_MENU_TEXT );  // find the menu position of this text
    if ( iSortSubPos < 0 )
      pSortSubMenu->AppendMenu( MF_STRING, ID_TRANCHE_SORT_LEVEL_DATE_TIME,  szMenuText );
  }*/	
  ///

  // Is this a ODManaged Client ?
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldODFlag() )
  {
	// have we already added the "train no grouping/date" sort option ?
    int iGetMenuPos = GetMenuPos( pSubMenu, IDS_ODMANAGED_MENU_ITEM );  // find the menu position of this text
    if ( iGetMenuPos < 0 )  // insert this (Single Menu) item
    {
      iItemOffset = ++iSortPos;
      pSubMenu->InsertMenu( 
        iItemOffset, 
        MF_BYPOSITION, 
        ID_TRANCHE_ODMANAGED, 
        ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_ODMANAGED_MENU_ITEM ) );
    }
  }

  iItemOffset++;

  // Is this a TRN client ?
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTRNFlag() )
  {
    int iGetMenuPos = GetMenuPos( pSubMenu, IDS_TRANCHE_COMPTAGES_DEFINE_DATE_MENU_TEXT );  // find the menu position of this text
	if ( iGetMenuPos < 0 ) 
	{
      if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() )  // Is this a Comptages client ?
      {
        pSubMenu->InsertMenu( 
          iItemOffset++, 
          MF_BYPOSITION | MF_SEPARATOR );

        // insert the menu for Comptages
        pSubMenu->InsertMenu( 
          iItemOffset++, 
          MF_BYPOSITION, 
          ID_TRANCHE_COMPTAGES_DEFINE_DATE, 
          ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_TRANCHE_COMPTAGES_DEFINE_DATE_MENU_TEXT) );
      }
	}

	iGetMenuPos = GetMenuPos( pSubMenu, IDS_TRANCHE_ADAPTATION_MENU_TEXT );  // find the menu position of this text
	if ( iGetMenuPos < 0 ) 
	{
      if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrainAdaptFlag() )  // Is this a Supplement/Adaptation client ?
      {
        pSubMenu->InsertMenu( 
          iItemOffset++, 
          MF_BYPOSITION | MF_SEPARATOR );

        // insert the menu for Comptages
        pSubMenu->InsertMenu( 
          iItemOffset++, 
          MF_BYPOSITION, 
          ID_TRANCHE_ADAPTATION, 
          ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_TRANCHE_ADAPTATION_MENU_TEXT) );
      }
	}
  }

  // if the tranche view is not the active view (i.e., the view who called this method) get out now !
  if ( ! bIsTrancheView )
    return;

  // !!!!!!!!!!!
  // the modifications from this point down are only for the tranche view...
  // !!!!!!!!!!!

  // this menu is modified only for the tranche view, therefore we set a flag when we've done it once, never to do it again...
  if ( ((CCTAApp*)APP)->GetTranMenuModFlag() )
    return;  // get out because it's already been modified
  else
    ((CCTAApp*)APP)->SetTranMenuModFlag(TRUE);  // flag it so we don't do it again

  BOOL bInsertSeparator = FALSE;

  int iCommentPos = GetMenuPos( pSubMenu, IDS_TRANCHE_COMMENT_MENU_TEXT );  // find the menu position of this text
  if ( iCommentPos < 0 )  // This should not happen...if it does, the menu item text has changed and is different from string in resource !
  {
    AfxMessageBox( "Unable to find Comment Menu Item !!!",  MB_ICONEXCLAMATION | MB_APPLMODAL );
    iCommentPos = 0;
  }

  iItemOffset = iCommentPos+2;

  // Is this a Forcement client ?
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetForcementFlag() && !((CCTAApp*)APP)->GetRhealysFlag())
  {
    CMenu NewMenu;
    NewMenu.LoadMenu( IDR_CTA_FORCEMENT_ITEM );
    HMENU hNewMenu = NewMenu.GetSafeHmenu( );
    hNewMenu = NewMenu.Detach( );

    // insert this (Popup Menu) item 
    pSubMenu->InsertMenu( 
      iItemOffset++, 
      MF_POPUP | MF_BYPOSITION, 
      (UINT)hNewMenu, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_FORCEMENT_MENU_TEXT )  );

    bInsertSeparator = TRUE;
  }

  // Is this a Supplement Level client ?
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSupplementTrainFlag() && !((CCTAApp*)APP)->GetRhealysFlag() )
  {
    // insert this (Single Menu) item
    pSubMenu->InsertMenu( 
      iItemOffset++, 
      MF_BYPOSITION, 
      ID_TRANCHE_SUPPLEMENT_LEVEL, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_SUPPLEMENT_TRAIN_MENU_TEXT ) );

    bInsertSeparator = TRUE;
  }

  if ( bInsertSeparator )  // do we need a separator here ?
  {
    pSubMenu->InsertMenu( 
      iItemOffset++, 
      MF_BYPOSITION | MF_SEPARATOR );

    bInsertSeparator = FALSE;
  }

  // Is this a TGV Jumeaux client ?
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTGVJumeauxFlag() && !((CCTAApp*)APP)->GetRhealysFlag() )
  {
    int iResID = ( ! ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTRNFlag() ) ?
      IDS_TGV_JUM_MENU_TEXT : IDS_TRN_JUM_MENU_TEXT;

    pSubMenu->InsertMenu( 
      iItemOffset++, 
      MF_BYPOSITION, 
      ID_TRANCHE_TGVJUMEAU, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( iResID ) );

      bInsertSeparator = TRUE;
  }

  // Is this a Influent Train/Tranche client ?
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetInfluentFlag() && !((CCTAApp*)APP)->GetRhealysFlag() )
  {
    // insert this (Single Menu) item
    pSubMenu->InsertMenu( 
      iItemOffset++, 
      MF_BYPOSITION, 
      ID_TRAIN_INFLUENT, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_INFLUENT_TRAIN_MENU_TEXT ) );

    // insert this (Single Menu) item
    pSubMenu->InsertMenu( 
      iItemOffset++, 
      MF_BYPOSITION, 
      ID_TRANCHE_INFLUENT_PERIOD, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_INFLUENT_TRANCHES_MENU_TEXT ) );
  }
  // Is this a client using TOP  ?
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTopFlag() && !((CCTAApp*)APP)->GetRhealysFlag() )
  {
     // insert this (Single Menu) item
    pSubMenu->InsertMenu( 
      iItemOffset++, 
      MF_BYPOSITION, 
      ID_TRANCHE_TOP, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_TOP_MENU_TEXT ) );

    // insert this (Single Menu) item
    pSubMenu->InsertMenu( 
      iItemOffset++, 
      MF_BYPOSITION, 
      ID_TRANCHE_SENSIBILITE, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_SENSIBILITE_MENU_TEXT ) );
  }

  // NPI - DM7838 : souplesse tarifaire
  if (((CCTAApp*)APP)->GetSouplesseTarifaire(/*TRUE*/))
  {
	  // insert this (Single Menu) item
	  pSubMenu->InsertMenu( 
		iItemOffset++, 
		MF_BYPOSITION, 
		ID_TRANCHE_NIVEAU, 
		((CCTAApp*)APP)->GetResource()->LoadResString( IDS_NIVEAU_MENU_TEXT ) );
  }

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
  {
	pSubMenu->InsertMenu( 
      iItemOffset++, 
      MF_BYPOSITION, 
      ID_SANS_NOMOS, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_SANS_NOMOS ) );
	pSubMenu->InsertMenu( 
      iItemOffset++, 
      MF_BYPOSITION, 
      ID_ACTIV_NOMOS_TO_DEP, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_ACTIV_NOMOS_TO_DEP ) );
	pSubMenu->InsertMenu( 
      iItemOffset++, 
      MF_BYPOSITION, 
      ID_EXCLU_NOMOS_TO_DEP, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_EXCLU_NOMOS_TO_DEP ) );
  }

  bInsertSeparator = TRUE;

  if ( bInsertSeparator )  // do we need a separator here ?
  {
    pSubMenu->InsertMenu( 
      iItemOffset++, 
      MF_BYPOSITION | MF_SEPARATOR );

    bInsertSeparator = FALSE;
  }
}

// Modify the Leg menu for client parameterization
//
void CMainFrame::RemoveLegAltMenu() 
{
  // get the position of the Leg Menu
  CMenu* pMainMenu = GetMenu();  // get the main menu
  int iGetMenuPos = GetMenuPos( pMainMenu, IDS_LEG_ALT_MENU_TEXT  );
  if ( iGetMenuPos < 0 )  // didn't find it, there is a problem ...
    return;

  BOOL bResult = pMainMenu->RemoveMenu( iGetMenuPos, MF_BYPOSITION );
  DrawMenuBar( );
}

void CMainFrame::ModifyLegMenu(BOOL bIsLegView /* = FALSE */, BOOL bAlt /* = FALSE */) 
{
  // MoteurVNL client does not use Leg List ...
#ifdef MOTEURVNL_APPLICATION  // We must re-compile the application with the Compiler Directive MOTEURVNL for this client
  return;
#endif

  RemoveLegAltMenu();

  CMenu* pMainMenu = GetMenu();  // get the main menu
  int iGetMenuPos = GetMenuPos( pMainMenu, (bAlt) ? IDS_LEG_ALT_MENU_TEXT : IDS_LEG_MENU_TEXT );
  if ( iGetMenuPos < 0 )
  {
    // This should not happen (it may happen for the leg view)...
    //    if it does, the menu item text has changed and is different from string in resource !
    /*if ( ! bIsLegView )  
      AfxMessageBox( "Unable to find Leg Menu Item !!!",  MB_ICONEXCLAMATION | MB_APPLMODAL );*/
    return;
  }

  // let's add the options...
  CMenu* pSubMenu = pMainMenu->GetSubMenu( iGetMenuPos );  // We found the Leg submenu...now get it
  int iItemOffset = (bIsLegView) ? 2 : 0;  // offset increment (note difference for Leg View menu
  BOOL bInsertSeparator = FALSE;

    iGetMenuPos = GetMenuPos( pSubMenu, IDS_LEG_SEND );  // find the sub menu position of this text
    if ( iGetMenuPos >= 0 )
      return;  // already been inserted...get out now

    // insert this (Single Menu) item
    pSubMenu->InsertMenu( 
      LEG_MENU_SEND_RESA_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION, 
      ID_LEG_SEND,
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_LEG_SEND ) );

    pSubMenu->InsertMenu( 
      LEG_MENU_SEND_RESA_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION | MF_SEPARATOR );

  // Is this a "Auto Copy Authorizations to Other Legs" client ?
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAutoCopyAuthFlag() )
  {
    iGetMenuPos = GetMenuPos( pSubMenu, IDS_AUTO_COPY_AUTH_MENU_TEXT );  // find the sub menu position of this text
    if ( iGetMenuPos >= 0 )
      return;  // already been inserted...get out now

    // insert this (Single Menu) item
    pSubMenu->InsertMenu( 
      LEG_MENU_AUTO_COPY_AUTH_OFFSET + iItemOffset++, 
      MF_BYPOSITION, 
      ID_LEG_AUTO_COPY_AUTH, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_AUTO_COPY_AUTH_MENU_TEXT ) );
  }
}

// Modify the Related Tranche menu for client parameterization
//
void CMainFrame::ModifyRelTrancheMenu(BOOL bAlt /*= FALSE*/) 
{
  // MoteurVNL client does not use Related Tranche window ...
#ifdef MOTEURVNL_APPLICATION  // We must re-compile the application with the Compiler Directive MOTEURVNL for this client
  return;
#endif

  // this menu is modified only for the Rel Tranche View, therefore we set a flag when we've done it once, never to do it again...
  if ( ((CCTAApp*)APP)->GetRelTranMenuModFlag(bAlt) )
    return;  // get out because it's already been modified
  else
    ((CCTAApp*)APP)->SetRelTranMenuModFlag(TRUE, bAlt);  // so we don't do it again

  CMenu* pMainMenu = GetMenu();  // get the main menu
  int iGetMenuPos = GetMenuPos( pMainMenu, IDS_GRAPH_MENU_TEXT );  // find the menu position of this text
  if ( iGetMenuPos < 0 )  // This should not happen...if it does, the menu item text has changed and is different from string in resource !
  {
    AfxMessageBox( "Unable to find Related Tranche Graph Menu Item !!!",  MB_ICONEXCLAMATION | MB_APPLMODAL );
    return;
  }

  // Okay, we found the Related Tranche Graph submenu...now get it
  CMenu* pSubMenu = pMainMenu->GetSubMenu( iGetMenuPos );
  int iItemOffset = 0;  // offset increment

  // insert this (Single Menu) item
  pSubMenu->InsertMenu( 
    RELTRAN_TRANCHE_MENU_ITEM_OFFSET + iItemOffset++, 
    MF_BYPOSITION, 
    ID_GRAPH_ATTACH_LABELS_CAPACITY, 
    ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_RELTRAN_ATTACH_LABELS_MENU_TEXT ) );

  // Is this a TRN client ?
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTRNFlag() )
  {
    // insert this (Single Menu) item
    pSubMenu->InsertMenu( 
      RELTRAN_TRNJOIN_MENU_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION, 
      ID_GRAPH_TRN_JOIN, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_RELTRAN_TRN_JOIN_MENU_TEXT ) );
  }

  iItemOffset++;  // skip down one

  // Is this a Tranche No client ?
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrancheNoFlag() )
  {
    // insert this (Single Menu) item
    pSubMenu->InsertMenu( 
      RELTRAN_TRANCHE_MENU_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION, 
      ID_GRAPH_TRANCHE_NO, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_RELTRAN_TRANCHENO_MENU_TEXT ) );

    iItemOffset++;  // skip down one

    // insert this (Single Menu) item
    pSubMenu->InsertMenu( 
      RELTRAN_TRANCHE_MENU_ITEM_OFFSET + iItemOffset, 
      MF_BYPOSITION, 
      ID_TRANCHE_DPT_TIME, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_RELTRAN_TRANCHEDPTTIME_MENU_TEXT ) );
  }

  // Is this a Supplement Level client ?
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSupplementLevelFlag() )
  {
    // insert this (Single Menu) item
    pSubMenu->InsertMenu( 
      RELTRAN_SUPPLVL_MENU_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION, 
      ID_GRAPH_SUPP_LEVEL, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_RELTRAN_TRANCHESUPLVL_MENU_TEXT ) );
  }
}

// Modify the Rrd History menu for client parameterization
//
void CMainFrame::ModifyRrdHistMenu() 
{
  // MoteurVNL client does not use RRD History window ...
#ifdef MOTEURVNL_APPLICATION  // We must re-compile the application with the Compiler Directive MOTEURVNL for this client
  return;
#endif

  // this menu is modified only for the Rrd History View, therefore we set a flag when we've done it once, never to do it again...
  if ( ((CCTAApp*)APP)->GetRrdHistMenuModFlag() )
    return;  // get out because it's already been modified
  else
    ((CCTAApp*)APP)->SetRrdHistMenuModFlag(TRUE);  // so we don't do it again

  CMenu* pMainMenu = GetMenu();  // get the main menu
  int iGetMenuPos = GetMenuPos( pMainMenu, IDS_GRAPH_MENU_TEXT );  // find the menu position of this text
  if ( iGetMenuPos < 0 )  // This should not happen...if it does, the menu item text has changed and is different from string in resource !
  {
    AfxMessageBox( "Unable to find Rrd History Graph Menu Item !!!",  MB_ICONEXCLAMATION | MB_APPLMODAL );
    return;
  }

  // Okay, we found the Rrd History Graph submenu...now get it
  CMenu* pSubMenu = pMainMenu->GetSubMenu( iGetMenuPos );
  int iItemOffset = 0;  // offset increment

  /* obsolete, laisse ici pour exemple au cas ou....
  // Is this a Rrd History Revenue client ?
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRrdHistRevFlag() )
  {
    pSubMenu->InsertMenu( 
      RRDHIST_REVWIN_MENU_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION | MF_SEPARATOR );

    // insert this (Single Menu) item
    pSubMenu->InsertMenu( 
      RRDHIST_REVWIN_MENU_ITEM_OFFSET + iItemOffset++, 
      MF_BYPOSITION, 
      ID_RRDHIST_REVWIN, 
      ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_RRDHIST_REVWIN_MENU_TEXT ) );
  }
  */
}

// Modify the Recd History menu for client parameterization
//
void CMainFrame::ModifyRecdHistMenu() 
{
  // MoteurVNL client does not use RECD History window ...
#ifdef MOTEURVNL_APPLICATION  // We must re-compile the application with the Compiler Directive MOTEURVNL for this client
  return;
#endif

  // this menu is modified only for the Recd History View, therefore we set a flag when we've done it once, never to do it again...
  if ( ((CCTAApp*)APP)->GetRecdHistMenuModFlag() )
    return;  // get out because it's already been modified
  else
    ((CCTAApp*)APP)->SetRecdHistMenuModFlag(TRUE);  // so we don't do it again

  CMenu* pMainMenu = GetMenu();  // get the main menu
  int iGetMenuPos = GetMenuPos( pMainMenu, IDS_GRAPH_MENU_TEXT );  // find the menu position of this text
  if ( iGetMenuPos < 0 )  // This should not happen...if it does, the menu item text has changed and is different from string in resource !
  {
    AfxMessageBox( "Unable to find Rrd History Graph Menu Item !!!",  MB_ICONEXCLAMATION | MB_APPLMODAL );
    return;
  }

  // Okay, we found the Rrd History Graph submenu...now get it
  CMenu* pSubMenu = pMainMenu->GetSubMenu( iGetMenuPos );
  int iItemOffset = 0;  // offset increment
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == TIMER_TEST_VNAU)
  {
	  if (((CCTAApp*)APP)->GetResComm()->YaOldVnauPending(TOO_OLD_PENDING))
	    ((CCTAApp*)APP)->GetResComm()->SendTest(FALSE);
 	}
	else if (nIDEvent == PRESENCE_TIMER_ID)
	{
	  ((CCTAApp*)APP)->GetResComm()->VerifPresence();
	  KillTimer (nIDEvent);
	}
	else if (nIDEvent == RECONNECT_TIMER)
	{
		DoReconnect();
		//MessageBeep(MB_ICONEXCLAMATION);
	}
	else if (nIDEvent == SOCKET_BLOCKING_TIMER)
	{
	  ((CCTAApp*)APP)->GetResComm()->KillIfBlocking();
	  KillTimer (SOCKET_BLOCKING_TIMER);
	}
	YM_PersistentFrame::OnTimer(nIDEvent);
}

//DT10343-LSO-YI10166 Ajout du menu "Actualiser"
void CMainFrame::AddRefreshMenu()
{
	// OBA - Ano 70154
	if (((CCTAApp*)APP)->GetRhealysFlag())
	{
		return;
	}

	CMenu* pMainMenu = GetMenu();  // On récupere le menu 

	// NPI - DM7978
	int iGetMenuPos = GetMenuPos( pMainMenu, IDS_GHOST_MENU_TEXT);  
	if (iGetMenuPos >= 0)// on regarde si on a pas déjà ajouté notre menu
	{
		return;
	}

	// On ajoute le menu
	pMainMenu->AppendMenu(MF_BYPOSITION | MF_DISABLED | MF_GRAYED, IDS_GHOST_MENU_TEXT, 
		((CCTAApp*)APP)->GetResource()->LoadResString(IDS_GHOST_MENU_TEXT));
	///

	iGetMenuPos = GetMenuPos( pMainMenu, IDS_REFRESH_MENU_TEXT);
	if (iGetMenuPos >= 0)// on regarde si on a pas déjà ajouté notre menu
	{
		return;
	}

	// On ajoute le menu
	pMainMenu->AppendMenu(MF_BYPOSITION, IDS_REFRESH_MENU_TEXT, 
		((CCTAApp*)APP)->GetResource()->LoadResString(IDS_REFRESH_MENU_TEXT));
}
//DT10343-LSO-YI10166 FIN
//DT10343-LSO-YI10167 Evenement lors du clic sur actualiser
void CMainFrame::OnRefresh() 
{
  // ON récupère un pointeur sur la fenetre des tranches
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
  	CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
    {
		// TDR - Anomalie 78938
		((CTrancheView*)pView)->SetIsRefresh(true);
		//SRE 72153 - 78938 Save the Top Index position, before the list is cleared
		((CTrancheView*)pView)->SaveSelectionPosition();
		
		// OBA Ano 70573 true -> recharge la même tranche
		((CTrancheView*)pView)->TrancheReload(true);
      return;
    }
  }
}
//DT10343-LSO-YI10167 FIN