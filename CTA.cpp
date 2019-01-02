// CTA.cpp : Defines the class behaviors for the application.
//
#include "StdAfx.h"

#include "CTA_Resource.h"

#include "CTAChartHelpDlg.h"
#include "TrancheView.h"

#include "ResaRailView.h"
#include "HistDptView.h"
#include "OthLegsView.h"
#include "RelTrancheView.h"
#include "RRDHistView.h"
#include "RecDHistView.h"
#include "BktHistView.h"
#include "VirtNestView.h"
#include "ComptagesView.h"
#include "ResTracer.h"
#include "VNAXWnd.h"
#include "VnlEngineView.h"
#include "TrainSupplementView.h"
#include "VnauListView.h"
#include "CalendarView.h"
#include "TraqueTarifView.h"
#include "WaitVnauDlg.h"
#include "SuiviVnauView.h"
#include "SuiviVnaxView.h"
#include "SuiviTopIstView.h"


// DM 7978 - Bouchon COMM pour tester moteur VNAX
#ifdef BOUCHON_COMM
	#include "CResCommBouchon.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString KDICLI = "KDISN";
/////////////////////////////////////////////////////////////////////////////
// CCTAApp

BEGIN_MESSAGE_MAP(CCTAApp, YM_WinApp)
	//{{AFX_MSG_MAP(CCTAApp)
	ON_COMMAND(ID_HELP_ABOUT, OnAppAbout)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCTAApp construction

CCTAApp::CCTAApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	/// NPI - Anomalie 57262
	// On force RogueWave à gérer les heures à la méthode européenne.
	// La méthode US provoque un problème sur le passage à l'heure d'été
	//	--> par défaut, gestion de l'heure d'été le 1er dimanche d'avril au lieu du dernier dimanche de mars...

	// NPI - Correction Memory Leak
	//RWZone::local(new RWZoneSimple(RWZone::Europe, RWZone::WeEu));
	this->_nbReconnection = 0;
	m_RWMyZone = new RWZoneSimple(RWZone::Europe, RWZone::WeEu);
	RWZone::local(m_RWMyZone);
	///
	///

	m_bRealTime = TRUE;
	m_bDisplayFlashLabels = FALSE;
	m_SuperUser = 0;
	m_bDvltMode = FALSE;
	m_bTrancheMenuModified = FALSE;
	m_bRelTrancheMenuModified = FALSE;
	m_pVnlEngineData = NULL;
	m_RRDdisplayMode = RRDDISPLAYALL;
	m_pResComm = NULL;
	m_bRRDRotatelabel = false;
	m_RRDBoolSet = SDISPLAYALL;
	m_RECDBoolSet = RDISPLAYALL;
	m_bHDptRotatelabel = false;
	m_HDptInterval = 5;
	m_ScaleRRD = CCTAChart::eMaxScaleIdx;
	m_ScaleHDpt = CCTAChart::eMaxScaleIdx;

	m_bSundayIsDOW1 = TRUE;  // Sunday is the beginning of the week by default
	m_bDoClose = TRUE;
	m_maxDateDailyTraffic = 0;
	m_bRhealys = FALSE;
	SetAttendEcranSelectionVnl(TRUE);
}

// NPI - Correction Memory Leak
CCTAApp::~CCTAApp()
{
	delete m_RWMyZone;
}
///

/////////////////////////////////////////////////////////////////////////////
// The one and only CCTAApp object

CCTAApp theApp;

/////////////////////////////////////////////////////////////////////////////
//  search a registry entry, and determine if a value is set to the parameter 'szValue'
//
enum CCTAApp::RegResult CCTAApp::QueryRegistryEntry(CString szRegSubKey, CString szValueName, CString szValue)
{
	HKEY hkResult;
	DWORD dType;
	// try to open the registry key ...
	long rtv = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szRegSubKey, 0, KEY_QUERY_VALUE, &hkResult);
	if (rtv == ERROR_SUCCESS)  // did we open this subkey ?
	{
		char szCharValue[256];
		unsigned char* pszCharValue = (unsigned char*)szCharValue;
		memset(szCharValue, '\0', sizeof(szCharValue));
		unsigned long lBuffSize;

		// try to find the value name ...
		if ( RegQueryValueEx(hkResult, szValueName, 0, &dType, pszCharValue, &lBuffSize) == ERROR_SUCCESS )
			return (!strcmp(szCharValue, szValue)) ? eValueMatched : eValueNotMatched;  // we found it, does it match the value passed ?
		else
			return eValueNameNotFound;  // did not find szValueName (Value Name)
	}
	else
	{
		return eKeyNotFound;  // did not find 
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCTAApp initialization

BOOL CCTAApp::InitInstance()
{
	// get the name of this module
	char szModuleName[100];
	GetModuleFileName(NULL, szModuleName, 100);

	// get the handle of this module and set it
	m_hModule = GetModuleHandle(szModuleName);

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED,  MB_ICONEXCLAMATION | MB_APPLMODAL);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.

	// Before we change the Registry Key,
	//   get the NLS_LANG variable from the Oracle registry entry (default is U.S.)

	enum CCTAApp::RegResult eRegResult = 
		QueryRegistryEntry("SOFTWARE\\ORACLE\\HOME1", "NLS_LANG", "AMERICAN_AMERICA.WE8ISO8859P1");
	if ( (eRegResult == eKeyNotFound) || (eRegResult == eValueNameNotFound) )
	{
		eRegResult = 
			QueryRegistryEntry("SOFTWARE\\ORACLE\\HOME0", "NLS_LANG", "AMERICAN_AMERICA.WE8ISO8859P1");  
		if ( (eRegResult == eKeyNotFound) || (eRegResult == eValueNameNotFound) )
		{
			eRegResult = 
				QueryRegistryEntry("SOFTWARE\\ORACLE", "NLS_LANG", "AMERICAN_AMERICA.WE8ISO8859P1");  
		}
	}
	if ( (eRegResult == eKeyNotFound) || (eRegResult == eValueNameNotFound) || (eRegResult == eValueNotMatched) )
		m_bSundayIsDOW1 = FALSE;
	else
		m_bSundayIsDOW1 = TRUE;

	// Parse command line for standard shell commands, DDE, file open
	YM_CommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	((CCTAApp*)APP)->SetNbConnHisto(cmdInfo.m_NbConnHisto);
	CreateResource(cmdInfo.m_cLanguage);
	m_bDvltMode = cmdInfo.m_dvltmode;
	m_SuperUser = cmdInfo.m_superuser;
	m_AutoReconnect = cmdInfo.m_autoreconnect;
	m_lasttranchevnau = 0;
	m_Client = cmdInfo.m_cClient; // SRE 78599 - Store client type from cmd line
	YM_ShadeButton::EnableShadeButton( true );

#ifdef MOTEURVNL_APPLICATION  // We must re-compile the application with the Compiler Directive MOTEURVNL for this client
	// Re-set the Application Name
	CString szAppName = GetResource()->LoadResString(IDS_CTA_TITLE);
	free((void*)m_pszAppName);
	m_pszAppName=_tcsdup(_T(szAppName));
	cmdInfo.m_nbThreads = 1;
#endif

	// set the registry key ... get the key from the recourse
	CString szKey = GetResource()->LoadResString(IDS_YMREGISTRY_RAILREV_PROJECT_KEY); 
	/*
	SetRegistryKey( LPCSTR(szKey) );
	m_pszProfileName = _tcsdup(_T("CTA"));
	*/
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	try {
		AfxOleInit(); // JZZ
	}
	catch (...)
	{
	}

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	// Document Template creation used for Views Instantiation
	CreateDocTemplates();

	// Initialize the worker thread object
	CreateWorkerThread( cmdInfo.m_nbThreads );

	// Initialize Database connection
	m_pDatabase = new YM_Database( cmdInfo.m_nbThreads );
	m_pDatabase->SetTrace(true);
	m_pDatabase->connectUserMasque = (cmdInfo.m_cClient == 'J');	
	
	if(cmdInfo.m_cClient == 'J'){
		// DT Performance ALLEO
		m_pDatabaseAlleo = new YM_Database(cmdInfo.m_nbThreads, true);
		m_pDatabaseAlleo->SetTrace(true);
		m_pDatabase->setSecondaryDbAddr(m_pDatabaseAlleo);
	}

	// this doesn't really belong here !!!
	m_pTempBktJumHist = new CBktHistDateMap(TRUE);
	m_pTempBktJumHistAlt = new CBktHistDateMap(TRUE);

	// create the document
	m_pDoc = new CCTADoc();

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if ( !pMainFrame->LoadFrame(IDR_CTA_MAINFRAME, WS_OVERLAPPEDWINDOW ) )
		return FALSE;
	CString szFrameTitle = GetResource()->LoadResString(IDS_CTA_TITLE);
	pMainFrame->SetWindowText (szFrameTitle);
	m_pMainWnd = pMainFrame;

#ifdef MOTEURVNL_APPLICATION  // We must re-compile the application with the Compiler Directive MOTEURVNL for this client
	// Re-set the Application Icon
	HICON hIcon = APP->LoadIcon(IDR_CTA_VNLENGINE);
	if (hIcon)
	{
		m_pMainWnd->SetIcon(hIcon, TRUE);
		m_pMainWnd->SetIcon(hIcon, FALSE);
	}
#endif

	// Create persistence object
	CreatePersistence(pMainFrame);

	// Restore values from registry for application
	((YM_WinApp*)AfxGetApp())->RestoreAppConfig();

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	pMainFrame->ShowControlBar (&pMainFrame->m_wndToolBar, FALSE, FALSE);

#ifdef _DEBUG_SQLPARMSFILE
	char* szFileName = "CtaSqlParms.txt";
	CFileException fileException;
	if ( !m_fSQLParmsFile.Open( szFileName, CFile::modeCreate | CFile::modeReadWrite, &fileException ) )
		TRACE( "Unable to open file %s, Error = %u\n", szFileName, fileException.m_cause );
#endif

	// Dispatch commands specified on the command line
	if( m_pDoc->OnNewDocument() == FALSE )
		return FALSE;

	return TRUE;
}

void CCTAApp::ToggleLoggingSQL()
{
	CString szLogging = " (Logging)";  // constant

	// Add the Database SID to the Mainframe title...
	CString szFrameTitle;
	((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd())->GetWindowText(szFrameTitle);

	CDumpContext* dc = APP->GetDumpContext();
	if (dc)
	{
		// logging was on...turn it off
		dc->Flush();
		dc->m_pFile->Close();
		delete dc->m_pFile;
		delete dc;
		dc = NULL;
		APP->SetDumpContext(NULL);

		int iIdx = szFrameTitle.Find( szLogging );
		if (iIdx >= 0)
		{
			CString szTemp = szFrameTitle.Left(szFrameTitle.GetLength() - szLogging.GetLength());
			szFrameTitle = szTemp;
		}
	}
	else
	{
		// logging was off...turn it on
		CFile* f = new CFile( "CtaSql.txt", CFile::modeCreate | CFile::modeWrite );
		APP->SetDumpContext(new CDumpContext( f ));

		szFrameTitle += szLogging;
	}

	((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd())->SetWindowText(szFrameTitle);
}

void CCTAApp::InitiateComm( CString szHostName, int iPortNumber )
{
	if ( m_pResComm == NULL )
	{
		// DM 7978 - FMA - Si bouchon actif, crée classe bouchon
		#ifdef BOUCHON_COMM
			m_pResComm = new CResCommBouchon();
		#else
			m_pResComm = new CResComm();
		#endif
		// DM 7978 - Fin
	}
	m_pResComm->SetHostName( szHostName );
	m_pResComm->SetPortNumber( iPortNumber );
	m_pResComm->OpenComm();
}

void CCTAApp::SetRealTimeMode (BOOL bTempReel)
{
	// find the ResaRail View and send a vnl, which updates the view
	CCTADoc* pDoc = GetDocument(); 
	POSITION pos = pDoc->GetFirstViewPosition();
	while( pos != NULL ) 
	{
		CView* pView = pDoc->GetNextView( pos ); 
		if( pView->IsKindOf( RUNTIME_CLASS(CResaRailView) ) ) 
		{
			CString titi,str;
			titi = GetResource()->LoadResString(IDS_RESARAIL_TITLE);
			if (bTempReel)
				str = GetResource()->LoadResString(IDS_REALTIMEON);
			else
				str = GetResource()->LoadResString(IDS_REALTIMEOFF);
			titi += "   ";
			titi += str;
			pView->GetParentFrame()->SetWindowText(titi);
		}
	}
	m_bRealTime = bTempReel;
}

void CCTAApp::CreateDocTemplates()
{
	// Tranche List View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_TRANLIST,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CTrancheView)) );

	// Leg List View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_LEGLIST,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CLegView)) );

	// Departure History View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_HISTDPT,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CSplitChildFrameH), // custom MDI child frame
		RUNTIME_CLASS(CHistDptView)) );

	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_HISTDPT_ALT,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CSplitChildFrameH), // custom MDI child frame
		RUNTIME_CLASS(CHistDptAltView)) );

	// Other Legs View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_OTHLEGS,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(COthLegsView)) );

	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_OTHLEGS_ALT,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(COthLegsAltView)) );

	// Related Tranches View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_RELTRANCHE,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CRelTrancheView)) );

	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_RELTRANCHE_ALT,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CRelTrancheAltView)) );

	// RRD History View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_RRDHIST,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CSplitChildFrameH), // custom MDI child frame
		RUNTIME_CLASS(CRRDHistView)) );

	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_RRDHIST_ALT,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CSplitChildFrameH), // custom MDI child frame
		RUNTIME_CLASS(CRRDHistAltView)) );

	if (m_Client != 'J') // SRE 78599 - Recette Histo should not be loaded for ALLEO
	{
		// RecD History View
		AddDocTemplate( new CMultiDocTemplate(
			IDR_CTA_RECDHIST,
			RUNTIME_CLASS(CCTADoc),
			RUNTIME_CLASS(CSplitChildFrameH), // custom MDI child frame
			RUNTIME_CLASS(CRecDHistView)) );

		AddDocTemplate( new CMultiDocTemplate(
			IDR_CTA_RECDHIST_ALT,
			RUNTIME_CLASS(CCTADoc),
			RUNTIME_CLASS(CSplitChildFrameH), // custom MDI child frame
			RUNTIME_CLASS(CRecDHistAltView)) );
	}

	// Bkt History View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_BKTHIST,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CSplitChildFrameV), // custom MDI child frame
		RUNTIME_CLASS(CBktHistView)) );

	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_BKTHIST_ALT,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CSplitChildFrameV), // custom MDI child frame
		RUNTIME_CLASS(CBktHistAltView)) );

	// ResaRail View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_RESARAIL,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CResaRailView)) );


	// Virtual Nesting View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_VIRTNEST,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CVirtNestView)) );

	// Comptages View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_COMPTAGES,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CComptagesView)) );

	// SEND VNAX View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_VNAX,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CVnaxWnd)) );

	// MOTEUR VNAX View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_MOTEUR_VNAX,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CVnaxAltWnd)) );

	// AUTOMATIC VNL View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_VNL,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CVnlWnd)) );

	//VNL & VNAU Tracer View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_TRACER,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CResTracerView)) );

	// VNL Engine View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_VNLENGINE,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CVnlEngineView)) );

	// TrainSupplement View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_TRAINSUPPLEMENT,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CTrainSupplementView)) );

	// TrainAdaptation View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_TRAINADAPTATION,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CTrainAdaptationView)) );

	// TrainDeleted View
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_TRAINDELETED,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CTrainDeletedView)) );

	// Historique des VNAU
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_VNAULIST,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CVnauListView)) );

	// Traque tarifaire
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_TRAQUETARIF,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CTraqueTarifView)) );

	// Calendrier
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_CALENDRIER,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CCalendarView)) );

	// Historique des VNAU
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_SUIVI_VNAU,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CSuiviVnauView)) );

	// Historique des VNAX
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_SUIVI_VNAX,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CSuiviVnaxView)) );

	// Historique des TOP & IST
	AddDocTemplate( new CMultiDocTemplate(
		IDR_CTA_SUIVI_TOPIST,
		RUNTIME_CLASS(CCTADoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CSuiviTopIstView)) );


}

void CCTAApp::ForceVnlViewer()
{
	CMainFrame* pMainFrame;
	pMainFrame = (CMainFrame*)m_pMainWnd;
	pMainFrame->ForceVnlTracer();
}

void CCTAApp::StartReconnectTimer()
{
	if (!m_AutoReconnect) return;
	CMainFrame* pMainFrame = (CMainFrame*)m_pMainWnd;
	pMainFrame->SetTimer(RECONNECT_TIMER, 10 * 1000, NULL);

	MessageBeep(MB_ICONEXCLAMATION);
	MessageBeep(MB_ICONEXCLAMATION);
	MessageBeep(MB_ICONEXCLAMATION);
	// on fait une tentative toutes les xx secondes
}

void CCTAApp::StopReconnectTimer()
{
	CMainFrame* pMainFrame = (CMainFrame*)m_pMainWnd;
	pMainFrame->KillTimer(RECONNECT_TIMER);
	pMainFrame->KillTimer (SOCKET_BLOCKING_TIMER);
	this->_nbReconnection = 0;
}

void CCTAApp::CreateNewView( const CString& str, CDocument* pDoc )
{
	POSITION pos = GetFirstDocTemplatePosition();
	while( pos != NULL ) 
	{
		CString szWindowTitle;
		CDocTemplate* pDocTemplate = GetNextDocTemplate( pos );
		pDocTemplate->GetDocString( szWindowTitle,  CDocTemplate::docName );
		if( szWindowTitle == str ) 
		{
			CFrameWnd* pFrame = pDocTemplate->CreateNewFrame( pDoc, NULL );
			pFrame->SetWindowText( szWindowTitle );
			pDocTemplate->InitialUpdateFrame( pFrame, pDoc );
			break;
		}
	}
}

// If the window exists (i.e., was previously created) return its pointer, else return NULL
//    Note:  The window must have been derived from CChildFrame
CWnd* CCTAApp::IsWindowCreated( const CString& szWindowTitle )
{
	POSITION pos = m_pDoc->GetFirstViewPosition();
	while( pos != NULL ) 
	{
		CWnd* pWnd = m_pDoc->GetNextView( pos )->GetParentFrame();
		if( pWnd->IsKindOf( RUNTIME_CLASS(CChildFrame) ) ) 
			if( ((CChildFrame*)pWnd)->GetChildKey() == szWindowTitle )
				return pWnd;
	}
	return NULL;  // didn't find this one
}

void CCTAApp::NotCloseOpenPersistentChildWindows()
{
	m_bDoClose = FALSE;
	OpenPersistentChildWindows();
	m_bDoClose = TRUE;
}

void CCTAApp::OpenPersistentChildWindows()
{
	// Ne peux pas être dans RestoreAppConfig.
	CString szRegistryKey = GetPersistence()->GetCurrentConfiguration();
	BOOL bHistoFlag = GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag();
	int k = bHistoFlag ? 1 : 0;
	k = APP->GetProfileInt( szRegistryKey, "HistoData", k );
	GetDocument()->m_pGlobalSysParms->SetRRDHistoryFlag(k==1);

	CString szClient = APP->m_pDatabase->GetClientParameter();
	if (szClient == THALYS)
		KDICLI = "KDITH";

	//CString szVNAXViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_VNAX_KEY);
	//CString szVNLViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_VNL_KEY);
	//CString szTracerViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_TRACER_KEY);
	//CString szVirtNestViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_VIRTNEST_KEY);
	//CString szVNLEngineViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_VNL_ENGINE_KEY);

	CString szTrancheSupplementViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_TRAINSUPPLEMENT_KEY);
	CString szTrancheDeletedViewKey = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CTA_TRAINDELETED_KEY);

	BOOL bDisplayWindow, bForceWindow;

	POSITION pos = GetDocument()->GetFirstViewPosition();
	if (m_bDoClose)
	{
		//ano 133192, on force fermeture des fenêtre splitées. Elle seront réouverte si besoin
		//ano 145958, 145968, 145965, 145964; on ferme toutes les fenêtre à taquets
		// Mais si l'appel vient de FinalDocInitialization il ne faut pas fermer et réouvrir les fenêtres
		// car il peut y avoir des notifications dans les tuyaux, d'ou ajout du boolean m_bDoClose.  
		CView* pViewRRD = NULL;
		CView* pViewRRDAlt = NULL;
		CView* pViewRecD = NULL;
		CView* pViewRecDAlt = NULL;
		CView* pViewBktHist = NULL;
		CView* pViewBktHistAlt = NULL;
		CView* pViewOtherLegs = NULL;
		CView* pViewOtherLegsAlt = NULL;
		CView* pViewRelTranches = NULL;
		CView* pViewRelTranchesAlt = NULL;
		CView* pViewHistoDep = NULL;
		CView* pViewHistoDepAlt = NULL;
		CView* pViewVirtNest = NULL;
		CView* pViewVnauList = NULL;
		CView* pViewCalendar = NULL;
		CView* pViewTraqueTarif = NULL;
		CView* pView;

		CMainFrame* pMainFrame = ((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd());
		while( pos != NULL ) 
		{
			pView = GetDocument()->GetNextView( pos ); 
			if ( pMainFrame->IsRRDHistView( pView, FALSE ) )
				pViewRRD = pView;
			else if ( pMainFrame->IsRRDHistView( pView, TRUE ) )
				pViewRRDAlt = pView;
			else if ( pMainFrame->IsRecDHistView( pView, FALSE ) )
				pViewRecD = pView;
			else if ( pMainFrame->IsRecDHistView( pView, TRUE ) )
				pViewRecDAlt = pView;
			else if ( pMainFrame->IsBktHistView( pView, FALSE ) )
				pViewBktHist = pView;
			else if ( pMainFrame->IsBktHistView( pView, TRUE ) )
				pViewBktHistAlt = pView;
			else if ( pMainFrame->IsOthLegsView (pView, FALSE) )
				pViewOtherLegs = pView;
			else if ( pMainFrame->IsOthLegsView( pView, TRUE ) )
				pViewOtherLegsAlt = pView;
			else if ( pMainFrame->IsRelTrancheView (pView, FALSE) )
				pViewRelTranches = pView;
			else if ( pMainFrame->IsRelTrancheView( pView, TRUE ) )
				pViewRelTranchesAlt = pView;
			else if (pMainFrame->IsHistDptView (pView, FALSE) )
				pViewHistoDep = pView;
			else if ( pMainFrame->IsHistDptView( pView, TRUE ) )
				pViewHistoDepAlt = pView;
			else if (pMainFrame->IsVirtNestView (pView) )
				pViewVirtNest = pView;
			else if (pMainFrame->IsVnauListView (pView) )
				pViewVnauList = pView;
			else if (pMainFrame->IsCalendarView (pView) )
				pViewCalendar = pView;
			else if (pMainFrame->IsTraqueTarifView (pView) )
				pViewTraqueTarif = pView;
		}
		if (pViewRRD)
			pViewRRD->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewRRDAlt)
			pViewRRDAlt->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewRecD)
			pViewRecD->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewRecDAlt)
			pViewRecDAlt->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewBktHist)
			pViewBktHist->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewBktHistAlt)
			pViewBktHistAlt->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewOtherLegs)
			pViewOtherLegs->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewOtherLegsAlt)
			pViewOtherLegsAlt->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewRelTranches)
			pViewRelTranches->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewRelTranchesAlt)
			pViewRelTranchesAlt->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewHistoDep)
			pViewHistoDep->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewHistoDepAlt)
			pViewHistoDepAlt->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewVirtNest)
			pViewVirtNest->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewVnauList)
			pViewVnauList->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewCalendar)
			pViewCalendar->GetParentFrame()->SendMessage (WM_CLOSE);
		if (pViewTraqueTarif)
			pViewTraqueTarif->GetParentFrame()->SendMessage (WM_CLOSE);
	}



	// Iterate throw DocTemplates
	pos = GetFirstDocTemplatePosition();
	while( pos != NULL ) 
	{
		bDisplayWindow = TRUE;
		bForceWindow = FALSE;

		CString szWindowTitle;
		CDocTemplate* pDocTemplate = GetNextDocTemplate( pos );
		pDocTemplate->GetDocString( szWindowTitle,  CDocTemplate::docName );

		// check the client parameterization flags
		bDisplayWindow = TRUE;  // display the window by default 
		if ( (( szWindowTitle == szTrancheSupplementViewKey ) || ( szWindowTitle == szTrancheDeletedViewKey )) && ! m_bDvltMode )
		{
			// open automatically the Tranche Supplement View and Tranche Deleted View
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrainAdaptFlag() )
				bForceWindow = TRUE;
		}
		// JMG 16/03/2004. désactivation de l'ouverture automatique des fenêtres tranches supplémentaires et supprimées
		bForceWindow = FALSE;

		// This View has to be created if it doesn't exist and is in the configuration
		// or destroyed if exist and isn't in the configuration
		if( ( GetPersistence()->IsCurrentWindow( szWindowTitle ) && bDisplayWindow ) || bForceWindow )
		{
			if( !IsWindowCreated( szWindowTitle ) ) 
			{
				if ( !((((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRevenuFlag()) &&
					( szWindowTitle == CMPT_HIST || szWindowTitle == CMPT_HIST_ALT )) ) // Is this a Virt Nesting Window client ?
				{
					CFrameWnd* pFrame = pDocTemplate->CreateNewFrame( m_pDoc, NULL );
					pFrame->SetWindowText( szWindowTitle );
					pDocTemplate->InitialUpdateFrame( pFrame, m_pDoc );
				}
			}
		}
		else 
		{
			if( CWnd* pWnd = IsWindowCreated( szWindowTitle ) ) 
			{
				pWnd->SendMessage( WM_CLOSE );
			}
		}
	}
}

void CCTAApp::RestoreAppConfig()
{
	// Set current profile key
	CString szRegistryKey = GetPersistence()->GetCurrentConfiguration();

	m_bDisplayFlashLabels = 
		APP->GetProfileInt( szRegistryKey, "DisplayFlashLabels", m_bDisplayFlashLabels );
	m_RRDdisplayMode = 
		APP->GetProfileInt( szRegistryKey, "RRDDisplayMode", m_RRDdisplayMode );
	CMainFrame* pMainFrame = ((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd());
	pMainFrame->SetColorTextBkMain((COLORREF) APP->GetProfileInt( szRegistryKey, "ColorTextBkMain", pMainFrame->GetColorTextBkMain() ));
	pMainFrame->SetColorTextBkAlt((COLORREF) APP->GetProfileInt( szRegistryKey, "ColorTextBkAlt", pMainFrame->GetColorTextBkAlt() ));
	pMainFrame->SetColorTextRecBkMain((COLORREF) APP->GetProfileInt( szRegistryKey, "ColorTextRecBkMain", pMainFrame->GetColorTextRecBkMain() ));
	pMainFrame->SetColorTextRecBkAlt((COLORREF) APP->GetProfileInt( szRegistryKey, "ColorTextRecBkAlt", pMainFrame->GetColorTextRecBkAlt() ));
	pMainFrame->SetColorFontRecBkMain((COLORREF) APP->GetProfileInt( szRegistryKey, "ColorFontRecBkMain", pMainFrame->GetColorFontRecBkMain() ));
	pMainFrame->SetColorFontRecBkAlt((COLORREF) APP->GetProfileInt( szRegistryKey, "ColorFontRecBkAlt", pMainFrame->GetColorFontRecBkAlt() ));
}

void CCTAApp::WriteAppConfig()
{
	// Set current profile key
	CString szRegistryKey = GetPersistence()->GetCurrentConfiguration();

	APP->WriteProfileInt( szRegistryKey, "DisplayFlashLabels", m_bDisplayFlashLabels );

	CMainFrame* pMainFrame = ((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd());
	APP->WriteProfileInt( szRegistryKey, "ColorTextBkMain", pMainFrame->GetColorTextBkMain() );
	APP->WriteProfileInt( szRegistryKey, "ColorTextBkAlt", pMainFrame->GetColorTextBkAlt() );
	APP->WriteProfileInt( szRegistryKey, "RRDDisplayMode", m_RRDdisplayMode);
	APP->WriteProfileInt( szRegistryKey, "ColorTextRecBkMain", pMainFrame->GetColorTextRecBkMain() );
	APP->WriteProfileInt( szRegistryKey, "ColorTextRecBkAlt", pMainFrame->GetColorTextRecBkAlt() );
	APP->WriteProfileInt( szRegistryKey, "ColorFontRecBkMain", pMainFrame->GetColorFontRecBkMain() );
	APP->WriteProfileInt( szRegistryKey, "ColorFontRecBkAlt", pMainFrame->GetColorFontRecBkAlt() );
}

int CCTAApp::ConvertRWDayOfWeek(int iRwWeekDay)
{
	// If Oracle NLS_LANG is set to U.S. English, we consider Sunday as the first day of the week
	//   whereas RogueWave considers Monday as the first day of the week
	if(GetSundayIsDOW1())
	{
		if (iRwWeekDay == 7)
			iRwWeekDay = 1;
		else
			iRwWeekDay++;
	}
	return iRwWeekDay;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOXCTA };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BOOL CAboutDlg::OnInitDialog() 
{
	CString s = ((CCTAApp*)AfxGetApp())->GetResource()->LoadResString(IDS_CTA_VERSION_NO);
	((CStatic*)GetDlgItem(IDC_CTA_VERSION_NO))->SetWindowText(s);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CCTAApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

#ifdef _DEBUG
CCTADoc* CCTAApp::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDoc->IsKindOf(RUNTIME_CLASS(CCTADoc)));
	return (CCTADoc*)m_pDoc;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCTAApp commands

int CCTAApp::ExitInstance() 
{
	delete m_pTempBktJumHist;
	m_pTempBktJumHist = NULL;
	delete m_pTempBktJumHistAlt;
	m_pTempBktJumHistAlt = NULL;
	if ( m_pResComm != NULL )
		delete m_pResComm;
	delete m_pDoc;
	m_pDoc = NULL;

	// NPI - Correction Memory Leak
	if (m_pVnlEngineData)
		delete m_pVnlEngineData;

	return YM_WinApp::ExitInstance();
	// NPI - Correction Memory Leak
	/*if (m_pVnlEngineData)
		delete m_pVnlEngineData;*/
	///
}

BOOL CCTAApp::SaveModifications()
{
	BOOL bModified = FALSE;  // determine if a view requires updating before exit ...

	// find the ResaRail view and tell it to save all modified values
	CCTADoc* pDoc = GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	while( pos != NULL ) 
	{
		CView* pView = pDoc->GetNextView( pos ); 

		if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) ) 
			bModified = bModified | ((CTrancheView*)pView)->SaveAllModified();   // this will start an update

		if( ( pView->IsKindOf( RUNTIME_CLASS(CLegView) ) ) && ( ! pView->IsKindOf( RUNTIME_CLASS(CLegAltView) ) ) )
			bModified = bModified | ((CLegView*)pView)->SaveAllModified();   // this will start an update

		if( pView->IsKindOf( RUNTIME_CLASS(CLegAltView) ) ) 
			bModified = bModified | ((CLegAltView*)pView)->SaveAllModified();   // this will start an update

		if( pView->IsKindOf( RUNTIME_CLASS(CResaRailView) ) )  
			bModified = bModified | ((CResaRailView*)pView)->SaveAllModified(3);   // this may start some queries

		if( 
			pView->IsKindOf( RUNTIME_CLASS(CTrainSupplementView) ) &&
			! pView->IsKindOf( RUNTIME_CLASS(CTrainAdaptationView) ) &&
			! pView->IsKindOf( RUNTIME_CLASS(CTrainDeletedView) )
			) 
			bModified = bModified | ((CTrainSupplementView*)pView)->SaveAllModified();   // this may start some queries

		if( pView->IsKindOf( RUNTIME_CLASS(CTrainAdaptationView) ) ) 
			bModified = bModified | ((CTrainAdaptationView*)pView)->SaveAllModified();   // this may start some queries

		if( pView->IsKindOf( RUNTIME_CLASS(CTrainDeletedView) ) ) 
			bModified = bModified | ((CTrainDeletedView*)pView)->SaveAllModified();   // this may start some queries
	}
	return bModified;
}

BOOL CCTAApp::SaveAllModified() 
{
	BOOL bModified = SaveModifications();  // determine if a view requires updating before exit ...

	if ( bModified )  // something was modifed, so we generated some queries
	{
		// Set text in status bar control
		((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd())->GetStatusBar()->
			SetPaneOneAndLock(
			((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DB_INUPDATE) );

		// message box
		GetStatusBox()->Show();
		GetStatusBox()->SetText(IDS_DB_INUPDATE);

		// wait for all threads to finish their tasks
		// set the exiting flag to true, notifying threads
		SetExitFlag( TRUE );

		// wait for database activity to complete
		do
		{
			Sleep(1000);
		}  while( m_pDatabase->GetActiveConnectCount() );

		GetStatusBox()->Hide();
	}

	// Set text in status bar control
	((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd())->GetStatusBar()->
		SetPaneOneAndLock(
		((CCTAApp*)APP)->GetResource()->LoadResString(IDS_EXITING) );

	// message box
	GetStatusBox()->Show();
	GetStatusBox()->SetText(IDS_EXITING);

	// resarail connection and processing exit procedure
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms != NULL )
		if ( m_pResComm && !m_pResComm->CanExit() )
		{
			if (AfxMessageBox(GetResource()->LoadResString(IDS_RESA_INUPDATE), 
				MB_YESNO | MB_APPLMODAL) == IDYES)
			{
				// Set text in status bar control
				((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd())->GetStatusBar()->
					SetPaneOneAndLock(
					((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DONE) );

				GetStatusBox()->Hide();
				return FALSE;
			}
			else
			{
				m_pResComm->StopActivity();
			}
		}

		// kill all threads (destructor of YM_Thread waits for exit event to signal)
		SetExitFlag( TRUE );
		DestroyWorkerThread();  

		return YM_WinApp::SaveAllModified();
}

// NPI - DM7870 : Dates de référence
// Réécriture de la fonction LoadCurHistoDates
/*void CCTAApp::LoadCurHistoDates(unsigned long dptdate, CString szCurrentEntity)
{
	BOOL bHistoFlag = GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag();
	if (!bHistoFlag)
		return;
	CString curEntity = m_CurHistoDates.Entity();
	if ((curEntity == GetDocument()->GetEntities()) && (dptdate == m_CurHistoDates.DptDate()))
		return; // les données en mémoire sont bonnes. pas la peine de relire.
	//m_CurHistoDates.Entity(GetDocument()->GetEntities());
	m_CurHistoDates.Entity(szCurrentEntity);
	m_CurHistoDates.DptDate(dptdate);
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(&m_CurHistoDates);
	RWDBReader Reader( m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_DATE_HISTO) );
	YmIcDateHistoDom HistoDatesRecord;
	while (Reader())
	{
		if (Reader.isValid())
			Reader >> HistoDatesRecord;
	}
	delete pSQL;
	m_CurHistoDates = HistoDatesRecord;
}*/

void CCTAApp::LoadCurHistoDates(unsigned long dptdate)
{
	BOOL bHistoFlag = GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag();

	if (!bHistoFlag)
	{
		return;
	}

	CString curEntity = m_CurHistoDates.Entity();
	if ((dptdate == m_CurHistoDates.DptDate())
		&& GetDocument()->GetCurrentEntityNatureID() == m_CurHistoDates.EntityNatureID())
	{
		return; // les données en mémoire sont bonnes. Inutile de relire.
	}

	m_CurHistoDates.EntityNatureID(GetDocument()->GetCurrentEntityNatureID());
	m_CurHistoDates.DptDate(dptdate);

	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(&m_CurHistoDates);
	RWDBReader Reader( m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_DATE_HISTO) );
	
	YmIcDateHistoDom HistoDatesRecord;
	while (Reader())
	{
		if (Reader.isValid())
		{
			Reader >> HistoDatesRecord;
		}
	}
	delete pSQL;
	
	m_CurHistoDates = HistoDatesRecord;
}
///

// NPI - DM7870 : Dates de référence
void CCTAApp::SetCurrentEntityNatureID(CString szCurrentEntity, long lNature)
{
	YmIcEntiteNatureDom EntiteNatureDom;

	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(&EntiteNatureDom);

	EntiteNatureDom.Entity(szCurrentEntity);
	EntiteNatureDom.Nature(lNature);

	RWDBReader Reader(m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_ENTITES_NATURE));
	while (Reader())
	{
		if (Reader.isValid())
		{
			Reader >> EntiteNatureDom;
			GetDocument()->SetCurrentEntityNatureID(EntiteNatureDom.EntityNatureID());
		}
	}
	delete pSQL;
}
///

// NPI - DT12135
void CCTAApp::SetQueryEntityNatureID(CString szCurrentEntity, long lNature)
{
	YmIcEntiteNatureDom EntiteNatureDom;

	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(&EntiteNatureDom);

	EntiteNatureDom.Entity(szCurrentEntity);
	EntiteNatureDom.Nature(lNature);

	RWDBReader Reader(m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_ENTITES_NATURE));
	while (Reader())
	{
		if (Reader.isValid())
		{
			Reader >> EntiteNatureDom;
			m_queryEntityNatureID = EntiteNatureDom.EntityNatureID();
		}
	}
	delete pSQL;
}
///

void CCTAApp::GetNbBktByCmpt (YmIcTrancheLisDom* pTranche)
{
	// Cette fonction ne sert que lors de changement d'organisation des CC :
	// DM4965.3 (passage de 5 a 7 en décembre 2005), puis DM 6716 (ajout d'une 8eme CC en seconde en 2009)
	// Donc même si cette requête peut marcher sur toute base, autant protéger son utilisation
	// car pour les clients non SNCF c'est une simple perte de temp.
	// 04/12/07, sert aussi dans le cadre de la DM 6102, modification des classes de contrôle Thalys.
	if (!GetDocument()->m_pGlobalSysParms->GetRecalcHistoBktFlag())
		return;
	YmIcResaBktDom ResaBktDom;
	ResaBktDom.TrancheNo (pTranche->TrancheNo());
	ResaBktDom.DptDate (pTranche->DptDate());
	ResaBktDom.LegOrig (pTranche->TrancheOrig());
	ResaBktDom.RrdIndex (pTranche->LastJX());
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(&ResaBktDom);
	m_nbBktByCmpt.RemoveAll();
	RWDBReader Reader( m_pDatabase->Transact(pSQL, IDS_SELECT_NBBKT_BYCMPT) );
	while (Reader())
	{
		if (Reader.isValid())
		{
			Reader >> ResaBktDom;
			long nl = ResaBktDom.NestLevel();
			/* JMG DEBUG
			RWDate dada (pTranche->DptDate());
			RWDate dodo (20, 6, 2009);
			if ((dada > dodo) && (!stricmp ("B", ResaBktDom.Cmpt())))
			nl = 8;
			// fin JMG DEBUG */
			m_nbBktByCmpt.SetAt (ResaBktDom.Cmpt(), (CObject*)nl);
		}
	}
	delete pSQL;
}

void CCTAApp::GetGrpTranche(int iTrancheno,unsigned long DptDate, CString sLegOrig,int &iGrpA,int &iGrpB)
{
	YmIcResaBktDom ResaBktDom;
	ResaBktDom.TrancheNo (iTrancheno);
	ResaBktDom.DptDate (DptDate);
	ResaBktDom.LegOrig (sLegOrig);
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(&ResaBktDom);

	RWDBReader Reader( m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_GRP_RHEALYS) );
	Reader();
	if (Reader.isValid())
	{
		Reader >> ResaBktDom;
		iGrpA = ResaBktDom.ResHoldGrp();
	}
	Reader();
	if (Reader.isValid())
	{
		Reader >> ResaBktDom;
		iGrpB = ResaBktDom.ResHoldGrp();
	}
	delete pSQL;
}


// Fin ajout fonctions pour DM 4965.3

//DM 4948: ne pas visualiser les recettes des tranches dont la date de circulation
// est >= max (dpt_date) from sc_daily_traffic
unsigned long CCTAApp::GetMaxDptDateDailyTraffic()
{
	if (m_maxDateDailyTraffic)
		return m_maxDateDailyTraffic;

	CString szClient = m_pDatabase->GetClientParameter();
	if ((szClient == THALYS) || (szClient == EUROSTAR))
	{ // La DM 4948 ne concerne pas Eurostar et Thalys
		m_maxDateDailyTraffic = 9999999;
		return m_maxDateDailyTraffic;
	}

	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	YmIcBktHistDom truc;
	pSQL->SetDomain(&truc);
	RWDBReader Reader( m_pDatabase->Transact(pSQL, IDS_SELECT_MAX_DATE_DAILY_TRAFFIC) );
	while (Reader())
	{
		if (Reader.isValid())
			Reader >> truc;
	}
	delete pSQL;
	m_maxDateDailyTraffic = truc.DptDate();
	if (!m_maxDateDailyTraffic) // si rien lu, le fen histo par bucket affichera N/A en données traffic
		m_maxDateDailyTraffic = 1;
	return m_maxDateDailyTraffic;
}

//Antibug de ce qui c'est passé le 25/10/2005
//Suite a des delete massifs sur sc_legs, il y avait un problème sur la mise à jour de sc_legs
// Il semble que la query de mise à jour d'un tronçon activé en autodelete soit détruite avant
void CCTAApp::UpdateOneLeg (YmIcLegsDom* pRecord)
{
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(pRecord);
	m_pDatabase->Transact(pSQL, IDS_SQL_UPDATE_LEG_LIST);
	delete pSQL;
}

// DM 5480, train auto. Les autorisations de train auto doivent être systématiquement affectée à l'ensemble
// des tronçons. On profite de cette DM pour créer un fonction plus efficasse pour la commande "Coller autorisation partout"
// cette requête ajoute 1 au sent_flag des legs de la tranche si mod (sent_flag, 2) = 0.
void CCTAApp::UpdateAllLegs (YmIcLegsDom* pRecord)
{
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(pRecord);
	m_pDatabase->Transact(pSQL, IDS_SQL_UPDATE_ALL_LEGS);
	delete pSQL;
}

void CCTAApp::UpdateSciAllLegs (YmIcLegsDom* pRecord)
{
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(pRecord);
	m_pDatabase->Transact(pSQL, IDS_SQL_UPDATE_SCI_ALL_LEGS);
	delete pSQL;
}

// DM 5524 Rhealys. Met a jour le CriticalFlag sur la tranche/date indiquée. Si bSet=TRUE on positionne, sinon on retire
// Le positionnement porte sur le deuxième bit du CRITICAL_FLAG
void CCTAApp::UpdRhealysCriticalFlag (BOOL bSet, int tranche_no, unsigned long ldate)
{
	YmIcTrancheLisDom TrancheDom;
	TrancheDom.TrancheNo (tranche_no);
	TrancheDom.DptDate (ldate);
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(&TrancheDom);
	if (bSet)
		m_pDatabase->Transact(pSQL, IDS_SET_CRIT_RHEALYS);
	else
		m_pDatabase->Transact(pSQL, IDS_UNSET_CRIT_RHEALYS);
	delete pSQL;
}

// Sur envoi de VNAU par la boîte de répartition des quotas, positionne le bit 1 du sent_flag sur la tranche/date
void CCTAApp::SetTrancheSentFlag (bool bscx, int tranche_no, unsigned long ldate)
{
	YmIcTrancheLisDom TrancheDom;
	TrancheDom.TrancheNo (tranche_no);
	TrancheDom.DptDate (ldate);
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(&TrancheDom);
	m_pDatabase->Transact(pSQL, bscx ? IDS_SET_BIT1_SCX_SENT_FLAG : IDS_SET_BIT1_SENT_FLAG);
	delete pSQL;
}

void CCTAApp::SetLegSentFlag (bool bscx, int tranche_no, CString sLegOrig, unsigned long ldate)
{
	YmIcLegsDom LegDom;
	LegDom.TrancheNo (tranche_no);
	LegDom.DptDate (ldate);
	LegDom.LegOrig (sLegOrig);
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(&LegDom);
	m_pDatabase->Transact(pSQL, bscx ? IDS_SET_LEG_BIT1_SCX_SENT_FLAG : IDS_SET_LEG_BIT1_SENT_FLAG);
	delete pSQL;
}

// Sur ouverture bascule par ALLEO, positionne le bit EDIT_STATUS_OKAY_RHEALYS du edit_status
void CCTAApp::SetTrancheEditStatus (int tranche_no, unsigned long ldate, int valbit)
{
	YmIcTrancheLisDom TrancheDom;
	TrancheDom.TrancheNo (tranche_no);
	TrancheDom.DptDate (ldate);
	TrancheDom.EditStatus(valbit);
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(&TrancheDom);
	m_pDatabase->Transact(pSQL, IDS_SET_BITX_EDIT_STATUS);
	delete pSQL;
}

void CCTAApp::SetLegEditStatus (int tranche_no, CString sLegOrig, unsigned long ldate, int valbit)
{

	YmIcLegsDom LegDom;
	LegDom.TrancheNo (tranche_no);
	LegDom.DptDate (ldate);
	LegDom.LegOrig (sLegOrig);
	LegDom.EditStatus(valbit);
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(&LegDom);
	m_pDatabase->Transact(pSQL, IDS_SET_LEG_BITX_EDIT_STATUS);
	delete pSQL;

}

BOOL CCTAApp::IsEntityRhealys (CString sEntity)
{
	CString szRhealys = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRhealysIdent();
	if (szRhealys.IsEmpty())
		return FALSE;

	BOOL bRhealys = FALSE;
	YM_Iterator<YmIcEntitiesDom>* pIterator =
		YM_Set<YmIcEntitiesDom>::FromKey( ENTITY_KEY)->CreateIterator();
	for(pIterator->First(); !pIterator->Finished(); pIterator->Next() )
	{
		YmIcEntitiesDom* pEntity = (YmIcEntitiesDom*)pIterator->Current();

		if (pEntity->Entity() == sEntity)
		{
			CString sEntityDescription;
			sEntityDescription = pEntity->Description();
			sEntityDescription.MakeUpper();
			if (sEntityDescription.Find(szRhealys) >= 0)
				bRhealys = TRUE;
			break;
		}
	}
	delete pIterator;
	return bRhealys;
}

void CCTAApp::AllEntityRhealys()
{
	BOOL bRhealys;
	CString szRhealys = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRhealysIdent();
	if (szRhealys.IsEmpty())
		bRhealys = FALSE;
	else
	{
		bRhealys = TRUE;
		YM_Iterator<YmIcEntitiesDom>* pIterator =
			YM_Set<YmIcEntitiesDom>::FromKey( ENTITY_KEY)->CreateIterator();
		for(pIterator->First(); !pIterator->Finished(); pIterator->Next() )
		{
			YmIcEntitiesDom* pEntity = (YmIcEntitiesDom*)pIterator->Current();
			CString sEntityDescription;
			sEntityDescription = pEntity->Description();
			sEntityDescription.MakeUpper();
			if (sEntityDescription.Find(szRhealys) < 0)
		 {
			 bRhealys = FALSE;
			 break;
		 }
		}
		delete pIterator;
	}
	m_bRhealys = bRhealys;
	if (bRhealys)
	{
		((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->SetRecalcHistoBktFlag(FALSE);
		((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->SetSCXFlag(false);
		((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->SetRevenuFlag(false);
	}
}

void CCTAApp::AllEntityAutoTrain()
{
	BOOL bAutoTrain = TRUE;
	YM_Iterator<YmIcEntitiesDom>* pIterator =
		YM_Set<YmIcEntitiesDom>::FromKey( ENTITY_KEY)->CreateIterator();
	for(pIterator->First(); !pIterator->Finished(); pIterator->Next() )
	{
		YmIcEntitiesDom* pEntity = (YmIcEntitiesDom*)pIterator->Current();
		CString sEntity;
		sEntity = pEntity->Entity();
		sEntity.MakeUpper();
		if (sEntity.Find("AUTO") < 0)
		{
			bAutoTrain = FALSE;
			break;
		}
	}
	delete pIterator; // NPI - Correction Memory Leak
	m_bAutoTrain = bAutoTrain;
}

long CCTAApp::GetLastRrdIndexOnLastBatch (long tranche, unsigned long ldate)
{
	YmIcTrancheLisDom TrancheDom;
	TrancheDom.TrancheNo (tranche);
	TrancheDom.DptDate (ldate);
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(&TrancheDom);
	long lastrrd = -1;
	RWDBReader Reader( m_pDatabase->Transact(pSQL, IDS_GET_LAST_RRD_INDEX) );
	while (Reader())
	{
		if (Reader.isValid())
		{
			Reader >> TrancheDom;
			lastrrd = TrancheDom.LastJX();
		}
	}
	delete pSQL;
	return lastrrd;
}

void CCTAApp::GetCoeffMatriceCC8()
{
	CString szkey;
	YmIcMatriceNotesDom MatriceNotes;
	MatriceNotes.Entity(GetDocument()->GetEntities());//recuperation de la liste des entity
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	pSQL->SetDomain(&MatriceNotes);
	RWDBReader Reader( m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_MATRICE_CC8));
	while (Reader())
	{
		if (Reader.isValid())
		{
			Reader >> MatriceNotes;
			szkey.Format("%s/%s/%d/%s/%d",MatriceNotes.Entity(),
				MatriceNotes.CmptDest(),
				MatriceNotes.NestLevelDest(),
				MatriceNotes.CmptSource(),
				MatriceNotes.NestLevelSource());
			m_MapMatriceCC8.SetAt(szkey,(CObject*)(MatriceNotes.Pct()));
		}
	}
	delete pSQL;
}


BOOL CCTAApp::TransformeMatrice (CString sEntity, CString sCmpt, CMapStringToOb* pMapOldResa,
								 CArray<int,int>& NewResa, CMapStringToOb* pMatrice)
{
	CString sKey,szkey, sKeyold;
	YmIcMatriceNotesDom MatriceNotes;
	long Matricevalue;
	POSITION pos;
	int nbk; // nombre de bucket pour le sCmpt dans la tranche courante.
	m_nbBktByCmpt.Lookup (sCmpt, (CObject*&)nbk);
	if (!nbk)
		return FALSE;
	NewResa.RemoveAll();
	NewResa.SetSize (nbk);
	long value;
	BOOL rtn = FALSE;
	for (int j = 0; j < nbk; j++)
		NewResa[j] = 0;

	float fvalue;
	long  lvalue;
	for (int i = 0; i < nbk; i++)
	{
		fvalue = 0;
		pos = pMatrice->GetStartPosition();
		szkey.Format("%s/%s/%d",sEntity, sCmpt, i);
		while(pos) 
		{
			pMatrice->GetNextAssoc(pos, sKey, (CObject*&)Matricevalue);
			if(sKey.Find(szkey, 0) >= 0)
			{   
				sKeyold.Format("%s%s",sEntity,sKey.Right(4));
				if(pMapOldResa->Lookup(sKeyold,(CObject*&)value))
					fvalue += value * Matricevalue;
				rtn = TRUE; // il y a eu recalcul
			}
		}
		fvalue = static_cast<float>(fvalue/100.0);
		lvalue = static_cast<long>(fvalue);
		if (fvalue >= 0)
		{
			if ((fvalue-lvalue) < 0.5)
				NewResa[i] = lvalue;
			else
				NewResa[i] = lvalue + 1;
		}
		else
		{
			if ((lvalue-fvalue) < 0.5)
				NewResa[i] = lvalue;
			else
				NewResa[i] = lvalue - 1;
		}
	}
	return rtn;
}


// Verification de la date de fin du Resaven
void CCTAApp::VerifyResaven()
{
	YmIcLegsDom LeDom;	// n'importe quel domaine avec une dpt_date
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	RWDBReader Reader( m_pDatabase->Transact(pSQL, IDS_SQL_VERIF_RESAVEN));
	while (Reader())
	{
		if (Reader.isValid())
		{
			Reader >> LeDom;
		}
	}
	RWDate today;
	if (LeDom.DptDate() == today.julian() -1)
		AfxMessageBox(IDS_RESAVEN_HIER,  MB_ICONEXCLAMATION | MB_APPLMODAL);
	else if ((LeDom.DptDate() > 0) && (LeDom.DptDate() < today.julian() -1))
	{
		CString ss = GetResource()->LoadResString(IDS_ALERTE_RESAVEN);
		RWDate dada (LeDom.DptDate());
		CString mess;
		mess.Format (ss, dada.asString("%d/%m/%Y"));
		AfxMessageBox(mess,  MB_ICONSTOP | MB_APPLMODAL);
	}
	delete pSQL;
}

// DM 6977, vérification de VNAU ALLEO non envoyé
int CCTAApp::YaUnsentRhealysVNAU()
{
	YmIcVnauDom VnauDom;
	int nbwaitvnau = 0;
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	RWDBReader Reader( m_pDatabase->Transact(pSQL, IDS_SELECT_UNSENT_ALLEO_VNAU));
	while (Reader())
	{
		if (Reader.isValid())
		{
			Reader >> VnauDom;
			nbwaitvnau = VnauDom.StatusSend();
		}
	}
	delete pSQL;
	return nbwaitvnau;
}

//DM8029, vérification de VNAS ALLEO non envoyé
int CCTAApp::CheckRhealysVNASSent()
{
	YmIcVnauDom VnauDom;
	int nbwaitvnas = 0;
	YM_Query* pSQL = new YM_Query (*m_pDatabase, FALSE);
	RWDBReader Reader( m_pDatabase->Transact(pSQL, IDS_SELECT_UNSENT_ALLEO_VNAS));
	while (Reader())
	{
		if (Reader.isValid())
		{
			Reader >> VnauDom;
			nbwaitvnas = VnauDom.StatusSend();
		}
	}
	delete pSQL;
	return nbwaitvnas;
}

void CCTAApp::VerifUnsentVnau(bool bMessIfNot)
{
	int nbwaitvnau = YaUnsentRhealysVNAU();
	int nbwaitvnas = CheckRhealysVNASSent();//DM8029 - verif si les VNAS ont été envoyés
	if (nbwaitvnau || nbwaitvnas)
	{
		CWaitVnauDlg wDlg;
		wDlg.m_nbwaitvnau = nbwaitvnau;
		wDlg.m_nbwaitvnas = nbwaitvnas;//DM8029 - rajout du nombre de vnas dans la boite de dialogue
		wDlg.DoModal();
	}
	else if (bMessIfNot)
		AfxMessageBox( GetResource()->LoadResString(IDS_NO_WAITING_VNAU), MB_OK);
}

//DM 7171 récup d'une sensibilité // renvoi la description du sous-type, lue dans YM_SENSIBILITE, vide si non trouvée
CString CCTAApp::SensiDesc(CString sstype)
{
	YM_Iterator<YmIcTypeTrainDom>* pIterator = 
		YM_Set<YmIcTypeTrainDom>::FromKey(SENSIBILITE_KEY)->CreateIterator();
	YmIcTypeTrainDom* pTop;
	CString sst, srst;
	for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
	{
		pTop = pIterator->Current();
		sst = pTop->SousTypeTrain();
		if (sstype == sst)
		{
			srst = pTop->Description();
			delete pIterator;
			return srst;
		}
	}
	delete pIterator;
	return srst;
}

CString CCTAApp::TopDesc(CString stop, unsigned long ddate)
{
	YM_Iterator<YmIcTypeTrainDom>* pIterator = 
		YM_Set<YmIcTypeTrainDom>::FromKey(TOP_KEY)->CreateIterator();
	YmIcTypeTrainDom* pTop;
	CString sst, srst;
	for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
	{
		pTop = pIterator->Current();
		if (!ddate || ((pTop->DateDebut() <= ddate) && (pTop->DateFin() >= ddate)))
		{
			sst = pTop->TypeTrain();
			if (sst == stop)
			{
				srst = pTop->Description();
				delete pIterator;
				return srst;
			}
		}
	}
	delete pIterator;
	return srst;
}

// NPI - DM7978 : souplesse tarifaire lot 2
BOOL CCTAApp::GetSouplesseTarifaire(BOOL bGestionAffichageItemMenu /*= FALSE*/, YmIcTrancheLisDom* pTrancheLoaded /*= NULL*/)
{
	BOOL bSouplesseTarifaire = FALSE;

	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNiveauGammeFlag())
	{
		if (bGestionAffichageItemMenu)
		{
			CString szClient = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient();
			if (szClient == SNCF)
			{
				// TGV
				// Le niveau de gamme n'est pas modifiable dans SATC ALLEO
				if (((pTrancheLoaded->Nature() == 0) || (pTrancheLoaded->Nature() == 2) || (pTrancheLoaded->Nature() == 5))
					&& (!((CCTAApp*)APP)->GetRhealysFlag()))
				{
					// Modification autorisée si Nature est égale à :
					//	- 0 : Trains domestiques
					//	- 2 : Trains ALLEO SNCF dom
					//	- 5 : train interSNCF dom
					bSouplesseTarifaire = TRUE;
				}
			}
			else if (szClient == SNCF_TRN)
			{
				// CORAIL
				if ((IsTrainDeNuit(pTrancheLoaded)) || ((CString)pTrancheLoaded->TrancheTypology() == "O"))
				{
					// Modification autorisée si TrancheTypology est égale à :
					//	- C : 1ere classe couchette, 2nde classe couchette (Train de nuit)
					//	- L : Voiture lit 1ere classe, Voiture lit 2nde classe (Train de nuit)
					//	- O : Classe Auto
					bSouplesseTarifaire = TRUE;
				}
				else
				{
					// Tous les trains de jours ne sont pas modifiables
					bSouplesseTarifaire = FALSE;
				}
			}
		}
		else
		{
			bSouplesseTarifaire = TRUE;
		}
	}

	return bSouplesseTarifaire;
}

// NPI - DM7978 : souplesse tarifaire lot 2
BOOL CCTAApp::IsTrainDeNuit(YmIcTrancheLisDom* pTranche)
{
	BOOL bIsTrainDeNuit = FALSE;

	if (((CString)pTranche->TrancheTypology() == "C") || ((CString)pTranche->TrancheTypology() == "L"))
	{
		// Modification autorisée si TrancheTypology est égale à :
		//	- C : 1ere classe couchette, 2nde classe couchette
		//	- L : Voiture lit 1ere classe, Voiture lit 2nde classe
		bIsTrainDeNuit = TRUE;
	}
	else
	{
		YmIcCmptTrancheDom CmptTrancheDom;
		CmptTrancheDom.DptDate(pTranche->DptDate());
		CmptTrancheDom.TrancheNo(pTranche->TrancheNo());
		CmptTrancheDom.TrainNo(pTranche->TrainNo());

		YM_Query* pSQL = new YM_Query (*(m_pDatabase), FALSE);
		pSQL->SetDomain(&CmptTrancheDom);

		RWDBReader Reader(m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_CMPT_TRANCHE));
		while (Reader())
		{
			// On parcourt le recordset à la recherche d'un compartiment F
			if (Reader.isValid())
			{
				Reader >> CmptTrancheDom;
				if ((CString)CmptTrancheDom.Cmpt() == "F")
				{
					// Modification autorisée si Compartiment est égale à :
					//	- F : 2nde classe siege inclinable
					bIsTrainDeNuit = TRUE;
					break;
				}
			}
		}

		delete pSQL;
	}

	return bIsTrainDeNuit;
}
