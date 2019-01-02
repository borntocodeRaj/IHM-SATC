// ChildFrm.cpp : implementation of the CChildFrame class
//
#include "StdAfx.h"


#include "CTAChartHelpDlg.h"
#include "TrancheView.h"

#include "BktHistView.h"
#include "HistDptView.h"
#include "OthLegsView.h"
#include "RelTrancheView.h"
#include "ResaRailView.h"
#include "RRDHistView.h"
#include "VirtNestView.h"
#include "TrainSupplementView.h"
#include "VnlEngineView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, YM_PersistentChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, YM_PersistentChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CREATE()
  ON_WM_MDIACTIVATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI (ID_INDICATOR_COMM, OnUpdateComm)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
  m_bAlt = FALSE;  // default is principal window (not alternate)
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( YM_PersistentChildWnd::PreCreateWindow(cs)==0 )
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |= WS_MAXIMIZE;
	cs.style &= ~(LONG)FWS_ADDTOTITLE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	YM_PersistentChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	YM_PersistentChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (YM_PersistentChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_wndStatusBar.Create(this)||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	
	m_wndStatusBar.SetPaneText( 0, 
    ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_WAITING_TEXT) );

	return 0;
}

void CChildFrame::OnUpdateComm(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( TRUE );
}

void CChildFrame::ActivateFrame(int nCmdShow) 
{
  if ( IsIconic( ) )
  {
    WINDOWPLACEMENT wndpl;
    GetWindowPlacement( &wndpl );
    wndpl.showCmd = SW_RESTORE;
    SetWindowPlacement( &wndpl );
  }
	YM_PersistentChildWnd::ActivateFrame(nCmdShow);
}

void CChildFrame::OnMDIActivate( BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd )
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

  CMainFrame* pMainFrame = ((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd());
  CView* pView = GetActiveView();

  // Modify the following menus for all views...
  pMainFrame->ModifySystemMenu();
  pMainFrame->ModifyWindowMenu();
  pMainFrame->ModifyHelpMenu();
//DT10343-LSO-YI10166 Ajout du menu "Actualiser"
  pMainFrame->AddRefreshMenu();

  // Tranche View...
  pMainFrame->ModifyTrancheMenu(pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ));

  // Leg View...
  pMainFrame->ModifyLegMenu(pView->IsKindOf( RUNTIME_CLASS(CLegView) ), FALSE );

  // Leg Alt View...
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTRNFlag() )
    pMainFrame->ModifyLegMenu(pView->IsKindOf( RUNTIME_CLASS(CLegView) ), TRUE );

  // Related Tranche View...
 	if( pView->IsKindOf( RUNTIME_CLASS(CRelTrancheView) ) )
    pMainFrame->ModifyRelTrancheMenu(pView->IsKindOf( RUNTIME_CLASS(CRelTrancheAltView) ));

  // RRD History View...
 	if( pView->IsKindOf( RUNTIME_CLASS(CRRDHistView) ) )
    pMainFrame->ModifyRrdHistMenu();

  // VNL View...
 	if( pView->IsKindOf( RUNTIME_CLASS(CVnlEngineView) ) )
    pMainFrame->ModifyVNLMenu();
}

/////////////////////////////////////////////////////////////////////////////
// CSplitChildFrame

IMPLEMENT_DYNCREATE(CSplitChildFrame, CChildFrame)

BEGIN_MESSAGE_MAP(CSplitChildFrame, CChildFrame)
	//{{AFX_MSG_MAP(CSplitChildFrame)
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplitChildFrame construction/destruction

CSplitChildFrame::CSplitChildFrame()
{
}

CSplitChildFrame::~CSplitChildFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CSplitChildFrame::AssertValid() const
{
	CChildFrame::AssertValid();
}

void CSplitChildFrame::Dump(CDumpContext& dc) const
{
	CChildFrame::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSplitChildFrame message handlers

BOOL CSplitChildFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
  ASSERT(FALSE);
  return FALSE;  // should not create an instance of this, the base class ... use CSplitChildFrameV or CSplitChildFrameH
	return m_wndSplitter.Create( this, 1, 2, CSize( 10, 10 ), pContext );
}

IMPLEMENT_DYNCREATE(CSplitChildFrameV, CSplitChildFrame)

BEGIN_MESSAGE_MAP(CSplitChildFrameV, CSplitChildFrame)
	//{{AFX_MSG_MAP(CSplitChildFrameV)
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CSplitChildFrameV::CSplitChildFrameV()
{
}

CSplitChildFrameV::~CSplitChildFrameV()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSplitChildFrame message handlers

BOOL CSplitChildFrameV::OnCreateClient( LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	return m_wndSplitter.Create( this, 1, 2, CSize( 10, 10 ), pContext);
}

void CSplitChildFrameV::WriteSplitterConfig(CString szProfileKey) 
{ 
  int iColCount = 0;
  int cyCur0 = -1;
  int cyMin0;
  int cyCur1 = -1;
  int cyMin1;
  if (m_wndSplitter.GetSafeHwnd())
  {
//    CWnd* pActivePane = m_wndSplitter.GetActivePane();
    iColCount = m_wndSplitter.GetColumnCount( );
    if (iColCount > 0)
      m_wndSplitter.GetColumnInfo( 0, cyCur0, cyMin0 );
    if (iColCount > 1)
      m_wndSplitter.GetColumnInfo( 1, cyCur1, cyMin1 );
  }
  
  APP->WriteProfileInt( szProfileKey, SPLITTER_COL_COUNT, iColCount );
  APP->WriteProfileInt( szProfileKey, SPLITTER_COL_0_SIZE, cyCur0 );
  APP->WriteProfileInt( szProfileKey, SPLITTER_COL_1_SIZE, cyCur1 );
}

void CSplitChildFrameV::RestoreSplitterConfig(CString szProfileKey) 
{ 
  int iColCount = m_wndSplitter.GetColumnCount( );
  int iColCountProf = APP->GetProfileInt( szProfileKey, SPLITTER_COL_COUNT, 0 );
  int cyCur0 = APP->GetProfileInt( szProfileKey, SPLITTER_COL_0_SIZE, -1 );
  int cyCur1 = APP->GetProfileInt( szProfileKey, SPLITTER_COL_1_SIZE, -1 );
  int cyMin0=10; 
  int cyMin1=10;
  if (m_wndSplitter.GetSafeHwnd())
  {
//    CWnd* pActivePane = m_wndSplitter.GetActivePane();
    if ( iColCount < 2 && cyCur0 >= 0 )
      m_wndSplitter.SplitColumn( cyCur0 );
    if ( iColCountProf > 0 && cyCur0 >= 0 )
      m_wndSplitter.SetColumnInfo( 0, cyCur0, cyMin0 );
    if ( iColCountProf > 1 && cyCur1 >= 0 )
      m_wndSplitter.SetColumnInfo( 1, cyCur1, cyMin1 );

    m_wndSplitter.RecalcLayout();
  }
}

IMPLEMENT_DYNCREATE(CSplitChildFrameH, CSplitChildFrame)

BEGIN_MESSAGE_MAP(CSplitChildFrameH, CSplitChildFrame)
	//{{AFX_MSG_MAP(CSplitChildFrameH)
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CSplitChildFrameH::CSplitChildFrameH()
{
}

CSplitChildFrameH::~CSplitChildFrameH()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSplitChildFrame message handlers

BOOL CSplitChildFrameH::OnCreateClient( LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	return m_wndSplitter.Create( this, 2, 1, CSize( 10, 10 ), pContext, WS_CHILD | WS_VISIBLE | WS_VSCROLL | SPLS_DYNAMIC_SPLIT );
}

void CSplitChildFrameH::WriteSplitterConfig(CString szProfileKey) 
{ 
  int iRowCount = 0;
  int cyCur0 = -1, cyMin0, cyCur1 = -1, cyMin1;
  if (m_wndSplitter.GetSafeHwnd())
  {
//    CWnd* pActivePane = m_wndSplitter.GetActivePane();
    iRowCount = m_wndSplitter.GetRowCount( );
    if (iRowCount > 0)
      m_wndSplitter.GetRowInfo( 0, cyCur0, cyMin0 );
    if (iRowCount > 1)
      m_wndSplitter.GetRowInfo( 1, cyCur1, cyMin1 );
  }
  
  APP->WriteProfileInt( szProfileKey, SPLITTER_ROW_COUNT, iRowCount );
  APP->WriteProfileInt( szProfileKey, SPLITTER_ROW_0_SIZE, cyCur0 );
  APP->WriteProfileInt( szProfileKey, SPLITTER_ROW_1_SIZE, cyCur1 );
}

void CSplitChildFrameH::RestoreSplitterConfig(CString szProfileKey) 
{ 
  int iRowCount = m_wndSplitter.GetRowCount( );
  int iRowCountProf = APP->GetProfileInt( szProfileKey, SPLITTER_ROW_COUNT, 0 );
  int cyCur0 = APP->GetProfileInt( szProfileKey, SPLITTER_ROW_0_SIZE, -1 );
  int cyCur1 = APP->GetProfileInt( szProfileKey, SPLITTER_ROW_1_SIZE, -1 );
  int cyMin0=10; 
  int cyMin1=10;
  if (m_wndSplitter.GetSafeHwnd())
  {
//    CWnd* pActivePane = m_wndSplitter.GetActivePane();
    if ( iRowCount < 2 && cyCur0 >= 0 )
      m_wndSplitter.SplitRow( cyCur0 );
    if ( iRowCountProf > 0 && cyCur0 >= 0 )
      m_wndSplitter.SetRowInfo( 0, cyCur0, cyMin0 );
    if ( iRowCountProf > 1 && cyCur1 >= 0 )
      m_wndSplitter.SetRowInfo( 1, cyCur1, cyMin1 );

    m_wndSplitter.RecalcLayout();
  }
}

BOOL CSplitChildFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	return 0;
	
	return CChildFrame::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CSplitChildFrameH::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
  return 0;
  
	return CSplitChildFrame::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CSplitChildFrameV::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
  return 0;
  
	return CSplitChildFrame::OnMouseWheel(nFlags, zDelta, pt);
}

void CChildFrame::OnClose() 
{
  CView* pView = GetActiveView();

  // Train Supplement / Adaptation View ...
 	if( 
    ( pView->IsKindOf( RUNTIME_CLASS(CTrainSupplementView) ) ) &&
    ( ! pView->IsKindOf( RUNTIME_CLASS(CTrainAdaptationView) ) ) &&
    ( ! pView->IsKindOf( RUNTIME_CLASS(CTrainDeletedView) ) )
    )
  {
    if ( ! ((CTrainSupplementView*)pView)->OnClose() )
      return;
  }

 	if( pView->IsKindOf( RUNTIME_CLASS(CTrainAdaptationView) ) )
  {
    if ( ! ((CTrainAdaptationView*)pView)->OnClose() )
      return;
  }

 	if( pView->IsKindOf( RUNTIME_CLASS(CTrainDeletedView) ) )
  {
    if ( ! ((CTrainDeletedView*)pView)->OnClose() )
      return;
  }

	YM_PersistentChildWnd::OnClose();
}
