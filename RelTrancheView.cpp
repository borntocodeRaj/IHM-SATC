// RelTrancheView.cpp : implementation file
//
#include "StdAfx.h"

#include "CTA_Resource.h"
#include "CTAChartHelpDlg.h"
#include "BktSciSphere.h"
#include "SpheresPrix.h"
#include "RelTrancheView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRelTrancheAltView
IMPLEMENT_DYNCREATE(CRelTrancheAltView, CRelTrancheView)

CRelTrancheAltView::CRelTrancheAltView()
:CRelTrancheView()
{
}

void CRelTrancheAltView::OnInitialUpdate() 
{
	((CChildFrame*)GetParentFrame())->SetAltFlag(TRUE);

	CRelTrancheView::OnInitialUpdate();  // base class method
}

/////////////////////////////////////////////////////////////////////////////
// CRelTrancheView

IMPLEMENT_DYNCREATE(CRelTrancheView, CView)

CRelTrancheView::CRelTrancheView()
{
	// set this view as the owner view in the chart object
	m_Chart.SetParentView(this);
	m_Chart.SetCurScaling(CCTAChart::eMaxScaleIdx);

	m_pMkt.szLegOrig = NULL;
	m_pMkt.szLegDest = NULL;

	m_iBarCount = 0;
	m_iNstLvlMaxCount = 0;
	m_bTabResized = FALSE;
	m_iCurTabSelection = -1;
	m_bResHoldScale = FALSE;
	m_bAttachLabelsToCapacity = FALSE;

	m_bMarketSelected = FALSE;
	m_pCriticalSection = new CCriticalSection;
	m_nbSpheresPrixMax = 0;//DM7978 - KGH - Sphere de prix CC/SC
	m_nbSpheresPrixLegende = 0;//DM7978 - KGH
}

CRelTrancheView::~CRelTrancheView()
{
	ClearTrancheInfoLabelArray();
	ClearCmptBktLabelArray();
	m_Chart.ClearChartDataArray();  // delete all graph datasets
	ClearRelTrancheList();

	if ( m_pCriticalSection )
	{
		delete m_pCriticalSection;
		m_pCriticalSection = NULL;
	}
}


BEGIN_MESSAGE_MAP(CRelTrancheView, CView)
	//{{AFX_MSG_MAP(CRelTrancheView)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_RELTRANCHE_TABCTRL, OnChartSelchange)
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_SCALE_MAX, OnScaleMax)
	ON_UPDATE_COMMAND_UI(ID_SCALE_MAX, OnUpdateScaleMax)
	ON_COMMAND(ID_SCALE_RESHOLD, OnScaleResHold)
	ON_UPDATE_COMMAND_UI(ID_SCALE_RESHOLD, OnUpdateScaleResHold)
	ON_COMMAND(ID_GRAPH_MKT_SELECT,OnGraphMktSelect)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_MKT_SELECT,OnUpdateGraphMktSelect)
	ON_COMMAND(ID_GRAPH_FLASH_LABEL,OnGraphDisplayFlashLabels)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_FLASH_LABEL,OnUpdateGraphDisplayFlashLabels)
	ON_COMMAND(ID_GRAPH_ROTATE_LABELS,OnGraphRotateLabels)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_ROTATE_LABELS,OnUpdateGraphRotateLabels)
	ON_COMMAND(ID_GRAPH_ATTACH_LABELS_CAPACITY,OnGraphAttachLabelsToCapacity)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_ATTACH_LABELS_CAPACITY,OnUpdateGraphAttachLabelsToCapacity)
	ON_COMMAND(ID_GRAPH_TRN_JOIN,OnGraphTRNJoin)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TRN_JOIN,OnUpdateGraphTRNJoin)
	ON_COMMAND(ID_GRAPH_TRANCHE_NO,OnGraphTrancheNo)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TRANCHE_NO,OnUpdateGraphTrancheNo)
	ON_COMMAND(ID_GRAPH_TRAIN_NO,OnGraphTrainNo)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TRAIN_NO,OnUpdateGraphTrainNo)
	ON_COMMAND(ID_GRAPH_ORIG_DEST,OnGraphOrigDest)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_ORIG_DEST,OnUpdateGraphOrigDest)
	ON_COMMAND(ID_GRAPH_SUPP_LEVEL,OnGraphSuppLevel)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_SUPP_LEVEL,OnUpdateGraphSuppLevel)
	ON_COMMAND(ID_TRANCHE_DPT_TIME,OnGraphTrancheDptTime)
	ON_UPDATE_COMMAND_UI(ID_TRANCHE_DPT_TIME,OnUpdateGraphTrancheDptTime)
	ON_COMMAND(ID_LEG_DPT_TIME,OnGraphLegDptTime)
	ON_UPDATE_COMMAND_UI(ID_LEG_DPT_TIME,OnUpdateGraphLegDptTime)
	ON_COMMAND_RANGE(ID_RELTRANCHE_MKT_SEL_BEG, ID_RELTRANCHE_MKT_SEL_END, OnMarketSelection)
	ON_COMMAND(ID_PRINT, OnPrint)
	ON_UPDATE_COMMAND_UI(ID_PRINT, OnUpdatePrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRelTrancheView drawing

void CRelTrancheView::OnDraw(CDC* pDC)
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);

	// if record set is valid and we have data 
	if ( pRelTrancheSet->IsValid() && m_Chart.GetChartDataArray()->GetSize() )
	{
	}
	else
	{
		// Gray the background
		RECT rect;
		CPen* pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
		CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(LTGRAY_BRUSH);
		GetClientRect(&rect);
		pDC->Rectangle(&rect);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
}

void CRelTrancheView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
	if ( pCTAChartData == NULL )
		return;

	COLORREF clrBk;
	COLORREF clrFt;
	if (pCTAChartData->GetGraphType() == CCTAChartData::eRecettes)
	{
		clrBk = (GetAltFlag()) ? ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextRecBkAlt() : 
	((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextRecBkMain();

	m_Chart.SetBackgroundColor (clrBk);
	clrFt = (GetAltFlag()) ? ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkAlt() : 
	((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();

	m_Chart.SetForegroundColor (clrFt);
	}
	else
	{
		clrBk = (GetAltFlag()) ? ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt() : 
	((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();

	m_Chart.SetBackgroundColor (clrBk);
	m_Chart.SetForegroundColor (RGB(0,0,0));   // noir
	m_Chart.SetYTitle("");
	}
	// Do not call CView::OnPaint() for painting messages
}

/////////////////////////////////////////////////////////////////////////////
// CRelTrancheView diagnostics

#ifdef _DEBUG
void CRelTrancheView::AssertValid() const
{
	CView::AssertValid();
}

void CRelTrancheView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCTADoc* CRelTrancheView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCTADoc)));
	return (CCTADoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRelTrancheView message handlers

int CRelTrancheView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create a chart window when the view is created
	CRect rect;
	GetClientRect(&rect);
	m_ChartTabCtrl.Create(CTA_TAB_STYLE, rect, this, IDC_RELTRANCHE_TABCTRL);
	GetClientRect(&rect);  // get client rectangle again, because it just changed...
	m_ChartTabCtrl.AdjustRect( FALSE, &rect );

	m_Chart.Create   ("", WS_CHILD | WS_VISIBLE, rect, &m_ChartTabCtrl, 0);

	m_bDisplayTrainNo = TRUE;
	m_bDisplayOrigDest = TRUE;
	m_bDisplayLegDptTime = TRUE;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrancheNoFlag() )
	{
		m_bDisplayTrancheNo = TRUE;
		m_bDisplayTrancheDptTime = TRUE;
	}
	else
	{
		m_bDisplayTrancheNo = FALSE;
		m_bDisplayTrancheDptTime = FALSE;
	}
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSupplementLevelFlag() )
		m_bDisplaySuppLevel = TRUE;
	else
		m_bDisplaySuppLevel = FALSE;

	return 0;
}

void CRelTrancheView::FrameSizeChange (UINT nType, int cx, int cy)
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	YM_RecordSet* pRelTgvSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELATEDTGV_ALT_KEY : RELATEDTGV_KEY);
	CString szClient = App().m_pDatabase->GetClientParameter();

	ASSERT( pRelTrancheSet != NULL );

	// if window has been minimized, we must destroy the help dialogs
	if ( nType == SIZE_MINIMIZED )
	{
		m_Chart.DestroyHelpDlg();
	}

	if (nType == SIZE_MINIMIZED) 
	{
		CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
		if (pRelTrancheSet->IsOpen())
		{
			// Close the record set and Reset the market SQL parameters...
			pRelTrancheSet->Close();
			m_pMkt.szLegOrig = NULL;
			m_pMkt.szLegDest = NULL;
			((CRelatedSet *)pDoc->m_pRelTrancheSet)->m_pMkt = NULL;
			((CRelatedSet *)pDoc->m_pRelTrancheSet)->m_szMktOrig.Empty();
			((CRelatedSet *)pDoc->m_pRelTrancheSet)->m_szMktDest.Empty();
		}
		if (pRelTgvSet->IsOpen())
		{
			pRelTgvSet->Close();
			((CRelatedSet *)pDoc->m_pRelTgvSet)->m_pMkt = NULL;
			((CRelatedSet *)pDoc->m_pRelTgvSet)->m_szMktOrig.Empty();
			((CRelatedSet *)pDoc->m_pRelTgvSet)->m_szMktDest.Empty();
		}
		return;
	}

	if (nType != SIZE_MINIMIZED) 
	{
		if (!pRelTrancheSet->IsOpen())
			pRelTrancheSet->Open();
		if ((szClient == SNCF_TRN) && !pRelTgvSet->IsOpen())
			pRelTgvSet->Open();
		return;
	}
}

void CRelTrancheView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (nStatus == 100)
	{ // appel de depuis CMainFrame::OnSelectionShowAllWindows suite a utilisation touches F3 ou F4
		// Si masquage (bShow = 0) on fait comme si la fenêtre était minimisée pour fermer les requêtes
		// sinon on fait comme si elle était restorée pour réouvir ses requêtes, d'où appel à 
		// FrameSizeChange. Remarque : cx et cy ne sont pas utilisé par FrameSizeChange, on met donc 0
		if (bShow)
			FrameSizeChange (SIZE_RESTORED, 0, 0);
		else
			FrameSizeChange (SIZE_MINIMIZED, 0, 0);  
	}
	else
		CView::OnShowWindow(bShow, nStatus);
}

void CRelTrancheView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	CString szTitleText, szTitleKey;
	szTitleText = ((CCTAApp*)APP)->GetResource()->LoadResString((GetAltFlag()) ? IDS_RELTRANCHE_SCI_TITLE : IDS_RELTRANCHE_TITLE);
	GetParentFrame()->GetWindowText(szTitleKey);
	GetParentFrame()->SetWindowText(szTitleText);
	((YM_PersistentChildWnd*)GetParentFrame())->SetChildKey (szTitleKey);
	((YM_PersistentChildWnd*)GetParentFrame())->SetChildView (this);
	CString szClient = App().m_pDatabase->GetClientParameter();
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	// Let's initialize this boolean here, because there's no place to do it in the RelTrancheSet construction
	if (szClient == SNCF_TRN)
	{
		if (GetAltFlag())
			((CRelatedSet *)pDoc->m_pRelTgvAltSet)->m_bMktSelect = FALSE;
		else
			((CRelatedSet *)pDoc->m_pRelTgvSet)->m_bMktSelect = FALSE;
	}
	if (GetAltFlag())
		((CRelatedSet *)pDoc->m_pRelTrancheAltSet)->m_bMktSelect = FALSE;
	else
		((CRelatedSet *)pDoc->m_pRelTrancheSet)->m_bMktSelect = FALSE;

	m_bTRNJoinFlag = (szClient == SNCF_TRN); // initialisation par defaut, sera ecrasé par la valeur de registry.

	YM_RecordSet* pRelTrancheSet = YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);	
	if( ! pRelTrancheSet->IsOpen() )
		pRelTrancheSet->Open();
	pRelTrancheSet->AddObserver( this ); // Ask for notification from this RecordSet
	OnUpdate( this, 0L, pRelTrancheSet );


	if (szClient == SNCF_TRN)
	{
		YM_RecordSet* pRelTgvSet = YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELATEDTGV_ALT_KEY : RELATEDTGV_KEY);	
		if( ! pRelTgvSet->IsOpen() )
			pRelTgvSet->Open();
		pRelTgvSet->AddObserver( this ); // Ask for notification from this RecordSet
		OnUpdate( this, 0L, pRelTgvSet );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// On change of the tab control, select the appropriate chart
//
void CRelTrancheView::OnChartSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// set the member variable which holds the currently selected tab...
	m_iCurTabSelection = m_ChartTabCtrl.GetCurSel();

	// are we out of bounds ?
	if ( ( m_Chart.GetChartDataArray()->GetSize() == 0 ) || ( m_iCurTabSelection < 0 ) )
		return;

	LPTSTR psz = _sLastTabUsed.GetBuffer( 255 );
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
	TabCtrlItem.pszText = psz;
	TabCtrlItem.cchTextMax = 255;
	GetTabCtrl()->GetItem( m_iCurTabSelection, &TabCtrlItem );
	_sLastTabUsed.ReleaseBuffer();

	if ( m_iCurTabSelection >= m_Chart.GetChartDataArray()->GetSize() )
		m_iCurTabSelection = 0;

	// set the appropriate data object as currently selected, then get it
	m_Chart.SetCurChartData( m_iCurTabSelection );
	CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
	if ( pCTAChartData == NULL )
		return;
	/* DM 5350 changement de la couleur de noir à marron pour la recette 
	et rajout pour la recette d'un titre sur l'axe Y : €uros */
	COLORREF clrBk;
	COLORREF clrFt;
	if (pCTAChartData->GetGraphType() != CCTAChartData::eRecettes)
	{
		clrBk = (GetAltFlag()) ? 
			((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt() : 
		((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();
		m_Chart.SetBackgroundColor (clrBk);
		m_Chart.SetForegroundColor (RGB(0,0,0));   // noir
		m_Chart.SetYTitle("");
	}
	else
	{
		clrBk = (GetAltFlag()) ? 
			((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextRecBkAlt() : 
		((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextRecBkMain();
		m_Chart.SetBackgroundColor (clrBk);
		clrFt = (GetAltFlag()) ? 
			((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkAlt() : 
		((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();
		m_Chart.SetForegroundColor (clrFt);
		/*m_Chart.SetYTitle("€uros");
		m_Chart.SetYTitleRotation(XRT_ROTATE_90);*/
	}
	ShowGraph();  // show the newly selected chart

	// force a redraw of the help dialog if up
	CCTAChartHelpDlg* pChartHelpDlg = m_Chart.GetHelpDlg();
	if (( pChartHelpDlg ) && (IsWindow(pChartHelpDlg->m_hWnd)) )
	{
		m_Chart.GetHelpDlg()->ClearHelpWin();
		m_Chart.GetHelpDlg()->Invalidate(FALSE);
		m_Chart.GetHelpDlg()->UpdateWindow();
	}
	*pResult = 0;
}

void CRelTrancheView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// If notification comes from a RecordSet, pHint is a RecordSet*
	if( pHint != NULL )
	{
		YM_RecordSet* pRelTrancheSet = 
			YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY );
		YM_RecordSet* pRelTgvSet = 
			YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELATEDTGV_ALT_KEY : RELATEDTGV_KEY );
		BOOL bYRev = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag();
		CString szClient = App().m_pDatabase->GetClientParameter();

		BOOL bRelTrancheSetValid = pRelTrancheSet->IsValid() &&
			((szClient != SNCF_TRN) || pRelTgvSet->IsValid() || !m_bTRNJoinFlag);
		if ( ( pHint == pRelTrancheSet ) )
		{
			if( !pRelTrancheSet->IsValid() ) 
			{
				// nettoyage, fait sur etat invalide de la requete principale
				m_Chart.DestroyHelpDlg();
				EnableWindow(FALSE);
				// Set the window tItle
				GetParentFrame()->SetWindowText(
					((CCTAApp*)APP)->GetResource()->LoadResString((GetAltFlag()) ? IDS_RELTRANCHE_SCI_TITLE : IDS_RELTRANCHE_TITLE));

				ClearGraph();
				if (m_bMarketSelected)  // set in OnMarketSelection(...)
				{
					//  begin critical section (will unlock when it goes out of scope)
					CSingleLock csl( m_pCriticalSection );
					csl.Lock();
					m_bMarketSelected = FALSE;  // reset

					// use the current leg for all parameters except Orig and Dest...
					YM_RecordSet* pLegSet = YM_Set<YmIcLegsDom>::FromKey (LEG_KEY);  
					((CRelatedSet *)pRelTrancheSet)->m_pMkt = (YmIcLegsDom*)pLegSet->GetCurrent();
					((CRelatedSet *)pRelTrancheSet)->m_szMktOrig = m_pMkt.szLegOrig;
					((CRelatedSet *)pRelTrancheSet)->m_szMktDest = m_pMkt.szLegDest;
					pRelTrancheSet->Open();
				}
			}
			// Reset the market SQL paramters
			((CRelatedSet *)pRelTrancheSet)->m_pMkt = NULL;
			((CRelatedSet *)pRelTrancheSet)->m_szMktOrig.Empty();
			((CRelatedSet *)pRelTrancheSet)->m_szMktDest.Empty();
		}

		if ( (szClient == SNCF_TRN) &&( pHint == pRelTgvSet ) )
		{
			if( !pRelTgvSet->IsValid() ) 
			{
				if (m_bMarketSelected)  // set in OnMarketSelection(...)
				{
					//  begin critical section (will unlock when it goes out of scope)
					CSingleLock csl( m_pCriticalSection );
					csl.Lock();

					// use the current leg for all parameters except Orig and Dest...
					YM_RecordSet* pLegSet = YM_Set<YmIcLegsDom>::FromKey (LEG_KEY);  
					((CRelatedSet *)pRelTgvSet)->m_pMkt = (YmIcLegsDom*)pLegSet->GetCurrent();
					((CRelatedSet *)pRelTgvSet)->m_szMktOrig = m_pMkt.szLegOrig;
					((CRelatedSet *)pRelTgvSet)->m_szMktDest = m_pMkt.szLegDest;
					pRelTgvSet->Open();
				}
			}
			// Reset the market SQL paramters
			((CRelatedSet *)pRelTgvSet)->m_pMkt = NULL;
			((CRelatedSet *)pRelTgvSet)->m_szMktOrig.Empty();
			((CRelatedSet *)pRelTgvSet)->m_szMktDest.Empty();
		}

		if ( bRelTrancheSetValid) 
		{
			EnableWindow(TRUE);
			if ( ! RestructureData() )  // Restructure the data
			{
				((CChildFrame*)GetParent())->GetStatusBar()->SetPaneOneAndLock(
					((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA) );
				return;
			}
			// Build all graphs
			ClearGraph();
			if (GetAltFlag())
				ReCalcAvailSci();
			m_sphereNonTrouvee = FALSE;	//DM7978 KGH / init de la variable 
			BuildGraph(CCTAChartData::eAvail);
			BuildGraph(CCTAChartData::eResHold);
			if (bYRev)
				BuildGraph(CCTAChartData::eRecettes);
			BuildTabs();  // Build the selecton tabs
			ResizeChart();
		}
	}
}

void CRelTrancheView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	ResizeChart();
}

void CRelTrancheView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CTypedPtrArray<CPtrArray, YmIcLegsDom*> paOrigLegDoms;
	CTypedPtrArray<CPtrArray, YmIcLegsDom*> paDestLegDoms;

	// create a popup contextual menu which contains all of the legs in the leg list
	CMenu menu;
	menu.CreatePopupMenu( );
	SetMenu( &menu );

	// get the current leg and set the current leg sequence...
	YM_RecordSet* pLegSet = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
	YmIcLegsDom* pLegCurrent = (YmIcLegsDom*)pLegSet->GetCurrent();
	int iCurLegSeq = pLegCurrent->LegSeq();  // get the leg sequence of the currently selected leg in the leg list

	// dynamically build the pop up menu
	// first, we must build an array containing a list of legs in sequence order
	YM_Iterator<YmIcLegsDom>* pLegIterator = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY)->CreateIterator();
	int iItemIdx = 0;
	CString szMenuString;

	// First, let's build 2 arrays; one contains all possible origins, the other all possible destinations
	YmIcLegsDom* pLeg = NULL;
	RelTran_Mkt* pMkt = NULL;
	for( pLegIterator->First(); !pLegIterator->Finished(); pLegIterator->Next() ) 
	{
		pLeg = pLegIterator->Current();
		if ( pLeg->LegSeq() <= iCurLegSeq )  // add to array containing orig legs
		{
			int i;
			for (i=0; i < paOrigLegDoms.GetSize(); i++)
			{
				if ( pLeg->LegSeq() < ( paOrigLegDoms.GetAt( i ) )->LegSeq() )
					break;
			}
			paOrigLegDoms.InsertAt( i, pLeg );
		}
		if ( pLeg->LegSeq() >= iCurLegSeq )  // put in array containing dest legs
		{
			int i;
			for (i=0; i < paDestLegDoms.GetSize(); i++)
			{
				if ( pLeg->LegSeq() < ( paDestLegDoms.GetAt( i ) )->LegSeq() )
					break;
			}
			paDestLegDoms.InsertAt( i, pLeg );
		}
	}
	delete pLegIterator;

	// build the array which contains the markets to put into the pop-up contextual menu
	int iOrigMax = paOrigLegDoms.GetUpperBound();
	int iDestMax = paDestLegDoms.GetUpperBound();
	for (int i = 0; i < m_paMktList.GetSize(); i++)
	{
		delete m_paMktList.GetAt( i );
		m_paMktList[ i ] = NULL;
	}
	m_paMktList.RemoveAll();  // remove all elements from the leg list array
	for ( int iOrigIdx = 0; iOrigIdx <= iOrigMax; iOrigIdx++ )
	{
		for ( int iDestIdx = 0; iDestIdx <= iDestMax; iDestIdx++ )
		{
			pMkt = (RelTran_Mkt*)malloc( sizeof( RelTran_Mkt ) );
			pMkt->szLegOrig = paOrigLegDoms.GetAt( iOrigIdx )->LegOrig();
			pMkt->szLegDest = paDestLegDoms.GetAt( iDestIdx )->LegDest();
			m_paMktList.Add( pMkt );
		}
	}

	// iterate through the leg list, adding each leg to the menu
	// Note that the menu ids are a range of ids
	char szMenuTxtBuf[50];  // size of 50 should be large enough for 2 station codes plus 1 blank space
	BOOL bFoundIt = FALSE;
	int iMktSelId = ID_RELTRANCHE_MKT_SEL_BEG;
	int iMktSelIdCurLeg = -1;
	int iArraySize = m_paMktList.GetSize();
	pMkt = NULL;
	for (iItemIdx = 0; iItemIdx < iArraySize; iItemIdx++ )
	{
		pMkt = m_paMktList.GetAt( iItemIdx );

		// create the text for the menu item
		strcpy( szMenuTxtBuf, (LPCSTR)pMkt->szLegOrig );
		strcat( szMenuTxtBuf, " " );
		strcat( szMenuTxtBuf, (LPCSTR)pMkt->szLegDest );

		menu.AppendMenu( MF_STRING, iMktSelId, szMenuTxtBuf );  // add this menu item

		// find which item to check on the pop-up contextual menu
		if ( ! ((CRelatedSet *)((CCTAApp*)APP)->GetDocument()->m_pRelTrancheSet)->m_bMktSelect )
		{
			// find the item to check based upon the currently selected leg
			if ( ( pMkt->szLegOrig == pLegCurrent->LegOrig() ) && 
				( pMkt->szLegDest == pLegCurrent->LegDest() ) )
				iMktSelIdCurLeg = iMktSelId;
		}
		else
		{
			// find the item to check based upon the currently selected market (previously selected from this menu)
			if ( ( pMkt->szLegOrig == m_pMkt.szLegOrig ) && ( pMkt->szLegDest == m_pMkt.szLegDest ) )
			{
				menu.CheckMenuItem( iMktSelId, MF_CHECKED );
				menu.EnableMenuItem( iMktSelId, MF_DISABLED );
				bFoundIt = TRUE;
			}
		}
		iMktSelId++;
	}

	// if we did not find the last selected market, then select the current leg by default
	if ( ! bFoundIt )
	{
		menu.CheckMenuItem( iMktSelIdCurLeg, MF_CHECKED );
		menu.EnableMenuItem( iMktSelIdCurLeg, MF_DISABLED );
	}

	// show the popup menu at the point of the cursor (point of the right single click)
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
	menu.Detach( );
}

void CRelTrancheView::OnDestroy() 
{
	CView::OnDestroy();
	CString szClient = App().m_pDatabase->GetClientParameter();

	// TODO: Add your message handler code here
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );
	pRelTrancheSet->RemoveObserver( this );
	pRelTrancheSet->Close();
	// clear the market SQL parameters in the record set, because we've closed it...
	((CRelatedSet *)pRelTrancheSet)->m_pMkt = NULL;
	((CRelatedSet *)pRelTrancheSet)->m_szMktOrig.Empty();
	((CRelatedSet *)pRelTrancheSet)->m_szMktDest.Empty();

	if (szClient == SNCF_TRN)
	{
		YM_RecordSet* pRelTgvSet = 
			YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELATEDTGV_ALT_KEY : RELATEDTGV_KEY);
		ASSERT( pRelTgvSet != NULL );
		pRelTgvSet->RemoveObserver( this );
		pRelTgvSet->Close();
		// clear the market SQL parameters in the record set, because we've closed it...
		((CRelatedSet *)pRelTgvSet)->m_pMkt = NULL;
		((CRelatedSet *)pRelTgvSet)->m_szMktOrig.Empty();
		((CRelatedSet *)pRelTgvSet)->m_szMktDest.Empty();
	}

	ClearTrancheInfoLabelArray();
	ClearCmptBktLabelArray();
}

void CRelTrancheView::OnScaleMax() 
{
	m_bResHoldScale = FALSE;
	m_Chart.ScaleGraph(CCTAChart::eMaxScaleIdx);
	Invalidate(FALSE);  // force a redraw
	UpdateWindow();
}

void CRelTrancheView::OnUpdateScaleMax(CCmdUI* pCmdUI)
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );

	pCmdUI->Enable( pRelTrancheSet->IsOpen() );
	pCmdUI->SetCheck( m_Chart.GetCurScaling() == CCTAChart::eMaxScaleIdx );
}

void CRelTrancheView::OnScaleResHold() 
{
	m_bResHoldScale = TRUE;
	m_Chart.ScaleGraph(CCTAChart::eResHoldScaleIdx);
	Invalidate(FALSE);  // force a redraw
	UpdateWindow();
}

void CRelTrancheView::OnUpdateScaleResHold(CCmdUI* pCmdUI) 
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );

	CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
	if ( pCTAChartData == NULL )
	{
		pCmdUI->Enable( pRelTrancheSet->IsOpen() );
		pCmdUI->SetCheck( m_Chart.GetCurScaling() == CCTAChart::eMaxScaleIdx );
	}
	else
	{
		pCmdUI->Enable( 
			( pRelTrancheSet->IsOpen() ) && 
			( pCTAChartData->GetGraphType() == CCTAChartData::eResHold ) );
		pCmdUI->SetCheck( m_Chart.GetCurScaling() == CCTAChart::eResHoldScaleIdx );
	}
}

void CRelTrancheView::OnGraphMktSelect() 
{
	CPoint PtOrig;
	GetCursorPos( &PtOrig ); 
	OnContextMenu( this, PtOrig );
}

void CRelTrancheView::OnUpdateGraphMktSelect(CCmdUI* pCmdUI) 
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );
	pCmdUI->Enable( pRelTrancheSet->IsOpen() );
}

void CRelTrancheView::OnGraphDisplayFlashLabels() 
{
	// toggle the flag to display the graph flash labels
	((CCTAApp*)APP)->SetFlashLabelFlag(  ! ((CCTAApp*)APP)->GetFlashLabelFlag() );
}

void CRelTrancheView::OnUpdateGraphDisplayFlashLabels(CCmdUI* pCmdUI) 
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );
	pCmdUI->Enable( pRelTrancheSet->IsOpen() );
	pCmdUI->SetCheck( ((CCTAApp*)APP)->GetFlashLabelFlag() );
}

void CRelTrancheView::OnGraphRotateLabels()
{
	m_Chart.SetRotateLabelsFlag( ! m_Chart.GetRotateLabelsFlag() );
	if ( m_Chart.GetRotateLabelsFlag() )
		m_Chart.SetXAnnotationRotation(XRT_ROTATE_90);
	else
		m_Chart.SetXAnnotationRotation(XRT_ROTATE_NONE);
}

void CRelTrancheView::OnUpdateGraphRotateLabels( CCmdUI* pCmdUI )
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );
	pCmdUI->Enable( pRelTrancheSet->IsOpen() );
	pCmdUI->SetCheck ( (m_Chart.GetRotateLabelsFlag()==1) ? 1 : 0 );
}

void CRelTrancheView::OnGraphUpdateLabels( BOOL& bFlag )
{
	bFlag = ! bFlag;
	CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
	if ( pCTAChartData == NULL )
		return;
	m_Chart.SetRepaint( FALSE );
	FormatBarLabels( pCTAChartData->GetGraphType() );
	m_Chart.SetRepaint( TRUE );
}

void CRelTrancheView::OnGraphAttachLabelsToCapacity()
{
	OnGraphUpdateLabels( m_bAttachLabelsToCapacity );
}

void CRelTrancheView::OnUpdateGraphAttachLabelsToCapacity( CCmdUI* pCmdUI )
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );
	pCmdUI->Enable( pRelTrancheSet->IsOpen() );
	pCmdUI->SetCheck ( m_bAttachLabelsToCapacity );
}

void CRelTrancheView::OnGraphTRNJoin() 
{
	// toggle the flag to join TRN with TGV production database
	SetTRNJoinFlag(  ! m_bTRNJoinFlag );

	YM_RecordSet* pRelTgvSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELATEDTGV_ALT_KEY : RELATEDTGV_KEY);
	if (pRelTgvSet->IsOpen())
	{
		pRelTgvSet->Close();
		pRelTgvSet->Open();
	}
}

void CRelTrancheView::OnUpdateGraphTRNJoin(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( TRUE );
	pCmdUI->SetCheck( GetTRNJoinFlag() );
}

void CRelTrancheView::OnGraphTrancheNo()
{
	OnGraphUpdateLabels( m_bDisplayTrancheNo );
}

void CRelTrancheView::OnUpdateGraphTrancheNo( CCmdUI* pCmdUI )
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );
	pCmdUI->Enable( pRelTrancheSet->IsOpen() );
	pCmdUI->SetCheck ( m_bDisplayTrancheNo );
}

void CRelTrancheView::OnGraphTrainNo()
{
	OnGraphUpdateLabels( m_bDisplayTrainNo );
}

void CRelTrancheView::OnUpdateGraphTrainNo( CCmdUI* pCmdUI )
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );
	pCmdUI->Enable( pRelTrancheSet->IsOpen() );
	pCmdUI->SetCheck ( m_bDisplayTrainNo );
}

void CRelTrancheView::OnGraphOrigDest()
{
	OnGraphUpdateLabels( m_bDisplayOrigDest );
}

void CRelTrancheView::OnUpdateGraphOrigDest( CCmdUI* pCmdUI )
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );
	pCmdUI->Enable( pRelTrancheSet->IsOpen() );
	pCmdUI->SetCheck ( m_bDisplayOrigDest );
}

void CRelTrancheView::OnGraphTrancheDptTime()
{
	OnGraphUpdateLabels( m_bDisplayTrancheDptTime );
}

void CRelTrancheView::OnUpdateGraphTrancheDptTime( CCmdUI* pCmdUI )
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );
	pCmdUI->Enable( pRelTrancheSet->IsOpen() );
	pCmdUI->SetCheck ( m_bDisplayTrancheDptTime );
}

void CRelTrancheView::OnGraphLegDptTime()
{
	OnGraphUpdateLabels( m_bDisplayLegDptTime );
}

void CRelTrancheView::OnUpdateGraphLegDptTime( CCmdUI* pCmdUI )
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );
	pCmdUI->Enable( pRelTrancheSet->IsOpen() );
	pCmdUI->SetCheck ( m_bDisplayLegDptTime );
}

void CRelTrancheView::OnGraphSuppLevel()
{
	OnGraphUpdateLabels( m_bDisplaySuppLevel );
}

void CRelTrancheView::OnUpdateGraphSuppLevel( CCmdUI* pCmdUI )
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );
	pCmdUI->Enable( pRelTrancheSet->IsOpen() );
	pCmdUI->SetCheck ( m_bDisplaySuppLevel );
}

void CRelTrancheView::OnPrint()
{
	m_Chart.Print();
}

void CRelTrancheView::OnUpdatePrint( CCmdUI* pCmdUI )
{
	YM_RecordSet* pRelTrancheSet = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	ASSERT( pRelTrancheSet != NULL );
	pCmdUI->Enable( pRelTrancheSet->IsOpen() );
}

void CRelTrancheView::OnMarketSelection( unsigned int iMktSelected )
{
	//  begin critical section (will unlock when it goes out of scope)
	CSingleLock csl( m_pCriticalSection );
	csl.Lock();

	// the menu ids are a range, start from the beginning of the range
	int iMktSelId = ID_RELTRANCHE_MKT_SEL_BEG;
	int iSelectedItemIdx = iMktSelected - iMktSelId;  // get the offset of the selection
	ASSERT( iSelectedItemIdx < m_paMktList.GetSize() );
	RelTran_Mkt* m_pMktSelected = m_paMktList.GetAt( iSelectedItemIdx );
	m_pMkt.szLegOrig = m_pMktSelected->szLegOrig;
	m_pMkt.szLegDest = m_pMktSelected->szLegDest;

	// now re-issue the sql query with the new market
	m_bMarketSelected = TRUE;  // Boolean indicating we have closed the RecordSet (see OnUpdate(...))
	YM_RecordSet* pRelTrancheSet = YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY);
	pRelTrancheSet->Close();

	CString szClient = App().m_pDatabase->GetClientParameter();
	if (szClient == SNCF_TRN)
	{
		YM_RecordSet* pRelTgvSet = YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELATEDTGV_ALT_KEY : RELATEDTGV_KEY);
		pRelTgvSet->Close();
	}

	// remove all elements from the leg list array
	for (int i = 0; i < m_paMktList.GetSize(); i++)
	{
		delete m_paMktList.GetAt( i );
		m_paMktList[ i ] = NULL;
	}
	m_paMktList.RemoveAll();
}

///////////////////////////////////////////////////////
// Resize the chart window 
//

void CRelTrancheView::ResizeChart()
{
	// force a re-size of the chart to fit the window
	CRect rect;
	GetClientRect(&rect);
	m_ChartTabCtrl.MoveWindow(&rect);
	m_ChartTabCtrl.AdjustRect(FALSE, &rect);

	// is there a rectangle ?
	if ( (rect.Width() > 0) && (rect.Height() > 10) )
		m_Chart.MoveWindow(&rect);
	COLORREF clrBk;
	COLORREF clrFt;
	if ( m_ChartTabCtrl.GetCurSel() == index_recette)
	{
		clrFt = (GetAltFlag()) ? 
			((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkAlt() : 
		((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();
		m_Chart.SetForegroundColor (clrFt);
		/*m_Chart.SetYTitle("€uros");
		m_Chart.SetYTitleRotation(XRT_ROTATE_90);*/
	}
	else
	{	  
		clrBk = (GetAltFlag()) ? 
			((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt() : 
		((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();
		m_Chart.SetBackgroundColor (clrBk);
		m_Chart.SetForegroundColor (RGB(0,0,0));   // noir
		m_Chart.SetYTitle("");
	}
}

///////////////////////////////////////////////////////
// Get the text from the appropriate tab
//
CString CRelTrancheView::GetTabText(enum CCTAChartData::GraphType eGraphType)
{
	switch( eGraphType )
	{
	case CCTAChartData::eAvail:
		return ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AVAIL);
		break;
	case CCTAChartData::eResHold:
		return ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_TOT);
		break;
	case CCTAChartData::eDmdDptFcst:
		return ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DEMAND_FCST);
		break;
	case CCTAChartData::eRecettes:
		// DM7978 KGH modif du libelle de l'onglet pour CC et SC
		//return ((CCTAApp*)APP)->GetResource()->LoadResString(GetAltFlag() ? IDS_SCI : IDS_BUCKET);

		// NPI - Ano 70148
		//return ((CCTAApp*)APP)->GetResource()->LoadResString(GetAltFlag() ? IDS_SCI_SPHERES : IDS_BUCKET_SPHERES);

		if (((CCTAApp*)APP)->GetRhealysFlag())
		{
			return ((CCTAApp*)APP)->GetResource()->LoadResString(GetAltFlag() ? IDS_SCI : IDS_BUCKET);
		}
		else
		{
			return ((CCTAApp*)APP)->GetResource()->LoadResString(GetAltFlag() ? IDS_SCI_SPHERES : IDS_BUCKET_SPHERES);
		}
		// DM7978 KGH FIN
		break;
	default:
		return "";
		break;
	}
}

///////////////////////////////////////////////////////
// Clear the data object
//
void CRelTrancheView::ClearGraph()
{
	// clear the client area(s)
	m_Chart.ShowWindow( SW_HIDE );

	Invalidate(FALSE);
	UpdateWindow();

	// delete the entries from the tab array
	ClearTabs();
	m_Chart.ClearChartDataArray();  // delete all graph dataset

	ClearTrancheInfoLabelArray();
	ClearCmptBktLabelArray();

	char *szFooter[] = { "", NULL };
	m_Chart.SetFooterStrings(szFooter);

	// Set text in status bar control
	((CChildFrame*)GetParent())->
		GetStatusBar()->SetPaneOneAndLock(
		((CCTAApp*)APP)->GetResource()->LoadResString(IDS_READING_HISTORY) );
}

///////////////////////////////////////////////////////
// Format and set the tranche info labels above the first bar for each tranche
//
void CRelTrancheView::FormatBarLabels( enum CCTAChartData::GraphType eGraphType )
{
	int iArraySIze = m_paTrancheInfoLabels.GetSize();
	if ( iArraySIze == 0 )
		return;

	// get the current tranche because we'll display it in another color...
	YmIcTrancheLisDom* pTranche = 
		YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	long lCurTrancheNo = pTranche->TrancheNo();

	int iSetIdx = 0;
	int iPointIdx = 0;
	BYTE bIdx;
	char aszText[15][50];  // make it big enough...

	// loop through the tranche info label array to build the lables displayed above the first bar for each tranche...
	for (int iTrancheInfoLabelIdx = 0; iTrancheInfoLabelIdx < iArraySIze; iTrancheInfoLabelIdx++)
	{
		// build the label according to the user-selected configuration (menu option)
		bIdx = 0;
		memset( aszText, '\0', sizeof( aszText) );
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrancheNoFlag() )
		{
			if (m_bDisplayTrancheNo)
				sprintf( aszText[bIdx++], "#%d ", m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pRelTranche->TrancheNo() );
			if (m_bDisplayTrancheDptTime)
				sprintf( aszText[bIdx++], "%0.2d:%0.2d", 
				m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pRelTranche->TrancheDptTime() / 100, 
				m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pRelTranche->TrancheDptTime() % 100 );
		}
		if (m_bDisplayTrainNo)
			sprintf( aszText[bIdx++], "%s ", m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pRelTranche->TrainNo() );
		if (m_bDisplayOrigDest)
		{
			sprintf(aszText[bIdx++], "%s", m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pRelTranche->LegOrigLabel());
			sprintf(aszText[bIdx++], " %s", m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pRelTranche->LegDestLabel());
		}
		if (m_bDisplayLegDptTime)
			sprintf( aszText[bIdx++], "%0.2d:%0.2d", 
			m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pRelTranche->LegDptTime() / 100, 
			m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pRelTranche->LegDptTime() % 100 );
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSupplementLevelFlag() )
			if (m_bDisplaySuppLevel)
				sprintf( aszText[bIdx++], "N%d ", m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pRelTranche->SupplementLevel() );

		// allocate space for an array to give to olectra chart's text area
		char** apcText = (char**)malloc( sizeof(char*) * (bIdx+1) );
		BYTE b;
		for (b = 0; b < bIdx; b++ )
			apcText[b] = aszText[b];
		apcText[b] = NULL;

		// do we attach the bar labels to the capacity bar or to the "bucket" bars ?
		if (m_bAttachLabelsToCapacity)
		{
			// attach to capacity bar
			iSetIdx = XrtDataGetLastSet( m_Chart.GetData2() );
			m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetAttachDataset(XRT_DATASET2);
		}
		else
		{
			iSetIdx = XrtDataGetLastSet( m_Chart.GetData() );
			m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetAttachDataset(XRT_DATASET1);
		}

		iPointIdx = m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->iPointIdx;

		m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetAttachType(XRT_TEXT_ATTACH_DATA);
		m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetStrings(apcText);
		m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetAttachSet(iSetIdx);
		m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetAttachPoint(iPointIdx);
		m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetIsConnected(TRUE);
		m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetAnchor(XRT_ANCHOR_NORTH);
		m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetOffset(5);
		m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetIsShowing(TRUE);
		m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetForegroundColor(XrtColorBlack);

		// if this is the current tranche,  then set the text label blue
		if ( m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pRelTranche->TrancheNo() == lCurTrancheNo )
			m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetForegroundColor(XrtColorBlue);
		else if ( m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pRelTranche->DBLinked() )
			m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetForegroundColor(XrtColorDarkGreen);

		free( apcText );
	}
}

///////////////////////////////////////////////////////
// Iterate through the record set and build an array of distinct tranche records with highest Res Hold
//
BOOL CRelTrancheView::RestructureData()
{
	// initialize some variables, and clean up from any previous calls...
	m_iBarCount = 0;
	m_iNstLvlMaxCount = 0;
	ClearRelTrancheList(); // vidage avant remplissage

	// get the current tranche and leg because we need it later...
	YmIcTrancheLisDom* pCurTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	YmIcLegsDom* pCurLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);

	// Define the record set and iterator
	YM_Iterator<YmIcRelatedDom>* pIterator = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY)->CreateIterator();

	// build the data for the graph
	long lNumPts = pIterator->GetCount();
	if (lNumPts <= 0)
	{
		delete pIterator;
		return FALSE;
	}

	// loop through the record set and build an array of tranches with accumulated res hold 
	CString szCmptHold;
	RelTran_LegLevel* pLegLevel = NULL;
	YmIcRelatedDom* pRelTranche = NULL;
	BOOL bSkipThisRecord = FALSE;
	for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
	{
		pRelTranche = pIterator->Current();

		// if this is the current tranche, but not the current leg orig/dest, skip it...
		bSkipThisRecord = ( 
			(pRelTranche->TrancheNo() == pCurTranche->TrancheNo()) &&
			( strcmp( pRelTranche->LegOrig(), pCurLeg->LegOrig() )  ||
			strcmp( pRelTranche->LegDest(), pCurLeg->LegDest() ) ) ) ? TRUE : FALSE;

		if ( ! ( bSkipThisRecord ) )  // we want only the current leg on the current tranche
		{
			// if we have a different tranche OR a different leg OR we are on the current tranche / leg
			if ( pLegLevel == NULL ||
				( pRelTranche->TrancheNo() != pLegLevel->lTrancheNo ) || 
				( pRelTranche->LegSeq() != pLegLevel->lLegSeq ) )
			{
				pLegLevel = new RelTran_LegLevel;
				pLegLevel->lResHoldTotSum = (pRelTranche->CmptResHoldIdv()+pRelTranche->CmptResHoldGrp());  // reset the accumulator
				pLegLevel->lTrancheNo = pRelTranche->TrancheNo();
				pLegLevel->lLegSeq = pRelTranche->LegSeq();
				pLegLevel->lDptTime = pRelTranche->LegDptTime();  // used for sorting
				//          pLegLevel->lDptTime = pRelTranche->TrancheDptTime();
				m_paRelTrancheList.Add( pLegLevel );
				szCmptHold = (pRelTranche->Cmpt());  // reset cmpt hold
			}
			else
			{
				if ( szCmptHold != (pRelTranche->Cmpt()) )  // do we have a new cmpt ?
				{
					// accumulate ResHoldTot
					pLegLevel->lResHoldTotSum += (pRelTranche->CmptResHoldIdv()+pRelTranche->CmptResHoldGrp());
					szCmptHold = (pRelTranche->Cmpt());  // reset cmpt hold
				}
			}
			pLegLevel->aRelTrancheRecs.Add( pRelTranche );
		}
	}
	delete pIterator;

	CString szClient = App().m_pDatabase->GetClientParameter();
	if ((szClient == SNCF_TRN) && m_bTRNJoinFlag)
	{
		pLegLevel = NULL;
		YM_Iterator<YmIcRelatedDom>* pIterator2 = 
			YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELATEDTGV_ALT_KEY : RELATEDTGV_KEY)->CreateIterator();
		for( pIterator2->First(); !pIterator2->Finished(); pIterator2->Next() ) 
		{
			pRelTranche = pIterator2->Current();
			// Pas besoin de tester le bSkipRecord, toujour faux dans le cas des TGV encadarant des Corail
			// if we have a different tranche OR a different leg OR we are on the current tranche / leg
			if ( pLegLevel == NULL ||
				( pRelTranche->TrancheNo() != pLegLevel->lTrancheNo ) || 
				( pRelTranche->LegSeq() != pLegLevel->lLegSeq ) )
			{
				pLegLevel = new RelTran_LegLevel;
				pLegLevel->lResHoldTotSum = (pRelTranche->CmptResHoldIdv()+pRelTranche->CmptResHoldGrp());  // reset the accumulator
				pLegLevel->lTrancheNo = pRelTranche->TrancheNo();
				pLegLevel->lLegSeq = pRelTranche->LegSeq();
				pLegLevel->lDptTime = pRelTranche->LegDptTime();  // used for sorting
				//          pLegLevel->lDptTime = pRelTranche->TrancheDptTime();
				m_paRelTrancheList.Add( pLegLevel );
				szCmptHold = (pRelTranche->Cmpt());  // reset cmpt hold
			}
			else
			{
				if ( szCmptHold != (pRelTranche->Cmpt()) )  // do we have a new cmpt ?
				{
					// accumulate ResHoldTot
					pLegLevel->lResHoldTotSum += (pRelTranche->CmptResHoldIdv()+pRelTranche->CmptResHoldGrp());
					szCmptHold = (pRelTranche->Cmpt());  // reset cmpt hold
				}
			}
			pLegLevel->aRelTrancheRecs.Add( pRelTranche );
		}
		delete pIterator2;
	}

	// keep only one element per tranche, the one with the highest res hold
	int iKeepIdx = 0;
	int iDeleteIdx = -1;
	RelTran_LegLevel* pLegLevelMax = NULL;
	for( int i=1; i < m_paRelTrancheList.GetSize(); i++ )
	{
		pLegLevelMax = m_paRelTrancheList.GetAt( iKeepIdx );
		pLegLevel = m_paRelTrancheList.GetAt( i );
		if ( pLegLevel && pLegLevelMax &&
			( pLegLevel->lTrancheNo == pLegLevelMax->lTrancheNo ) ) // compare same tranche no
		{
			if ( pLegLevel->lResHoldTotSum > pLegLevelMax->lResHoldTotSum )
			{
				iDeleteIdx = iKeepIdx;  // delete the other one
				iKeepIdx = i;  // keep this one
			}
			else
			{
				iDeleteIdx = i;
			}
			// get rid of this element, we don't need it anymore...
			m_paRelTrancheList.GetAt( iDeleteIdx )->aRelTrancheRecs.RemoveAll();
			delete m_paRelTrancheList.GetAt( iDeleteIdx );
			m_paRelTrancheList.SetAt( iDeleteIdx, NULL );
		}
		else
		{
			iKeepIdx = i;
		}
	}

	// remove all unused elements...
	int iTrancheCount = m_paRelTrancheList.GetSize();
	int iTrancheIdx;
	int iCpt;
	for( iTrancheIdx = 0,  iCpt=0; iTrancheIdx < iTrancheCount; iTrancheIdx++, iCpt++ )
	{
		if ( m_paRelTrancheList.GetAt( iCpt ) == NULL )
			m_paRelTrancheList.RemoveAt( iCpt-- );
	}

	// sort in order of dpt time
	iTrancheCount = m_paRelTrancheList.GetSize();  // get new size
	BOOL bSwap;
	for(int i=0; i < iTrancheCount; i++ )
	{
		bSwap = FALSE;
		for( int j=0; j < (iTrancheCount-1)-i; j++ )
		{
			if ( m_paRelTrancheList.GetAt( j )->lDptTime > m_paRelTrancheList.GetAt( j+1 )->lDptTime )
			{
				bSwap = TRUE;
				pLegLevel = m_paRelTrancheList.GetAt( j );
				m_paRelTrancheList.SetAt( j, m_paRelTrancheList.GetAt( j+1 ) );
				m_paRelTrancheList.SetAt( j+1, pLegLevel );
			}
		}
		if ( ! bSwap )
			break;
	}

	// get some counts
	m_iBarCount  = 0;
	szCmptHold.Empty();
	long lTrancheHold = 0;
	int iNstLvlMaxCount = 0;
	int iRecCount;
	for(int i=0; i < iTrancheCount; i++ )
	{
		pLegLevel = m_paRelTrancheList.GetAt( i );
		iRecCount = pLegLevel->aRelTrancheRecs.GetSize();
		for ( int j=0; j < iRecCount; j++ )
		{
			pRelTranche = pLegLevel->aRelTrancheRecs.GetAt( j );
			if ( ( lTrancheHold != pRelTranche->TrancheNo() ) ||  
				( szCmptHold != (pRelTranche->Cmpt()) ) )  // do we have a new tranche or a new cmpt ?
			{
				m_iNstLvlMaxCount = __max( m_iNstLvlMaxCount, iNstLvlMaxCount );
				iNstLvlMaxCount = 1;
				m_iBarCount++;
				szCmptHold = (pRelTranche->Cmpt());
				lTrancheHold = pRelTranche->TrancheNo();
			}
			else
			{
				iNstLvlMaxCount++;
			}
		}
		m_iNstLvlMaxCount = __max( m_iNstLvlMaxCount, iNstLvlMaxCount );
	}
	return TRUE;
}

void CRelTrancheView::ReCalcAvailSci()
{
	// loop through all records and recalculate the availability
	YM_Iterator<YmIcRelatedDom>* pIterator = 
		YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELTRANCHE_ALT_KEY : RELTRANCHE_KEY)->CreateIterator();

	int iAvail = 0;
	YmIcRelatedDom* pRelTranche;
	for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
	{
		pRelTranche = pIterator->Current();
		int AvailSCG = pRelTranche->Capacity() - pRelTranche->CmptResHoldGrp() - pRelTranche->CmptResHoldIdv();
		int AvailSCI = pRelTranche->AuthCur() - pRelTranche->BktResHoldGrp() - pRelTranche->BktResHoldIdv();
		int AvailBkt0 = pRelTranche->BktResHoldNY() - pRelTranche->CmptResHoldNY();
		// Pour eviter de créer d'autre membre dans le dommaine YmIcRelatedDom, je réutilise 
		// BKT_RES_HOLD_NY pour récupérer l'autorisation en bucket 0
		// CMPT_RES_HOLD_NY pour récupérer la somme des ventes IDV + GRP dans l'espace physique.
		iAvail = __min (AvailSCG, AvailSCI);
		iAvail = __min (iAvail, AvailBkt0);
		pRelTranche->Availability(iAvail);
	}
	delete pIterator;

	CString szClient = App().m_pDatabase->GetClientParameter();
	if ((szClient == SNCF_TRN) && m_bTRNJoinFlag)
	{
		YM_Iterator<YmIcRelatedDom>* pIterator2 = 
			YM_Set<YmIcRelatedDom>::FromKey((GetAltFlag()) ? RELATEDTGV_ALT_KEY : RELATEDTGV_KEY)->CreateIterator();
		for( pIterator2->First(); !pIterator2->Finished(); pIterator2->Next() ) 
		{
			pRelTranche = pIterator2->Current();
			int AvailSCG = pRelTranche->Capacity() - pRelTranche->CmptResHoldGrp() - pRelTranche->CmptResHoldIdv();
			int AvailSCI = pRelTranche->AuthCur() - pRelTranche->BktResHoldGrp() - pRelTranche->BktResHoldIdv();
			int AvailBkt0 = pRelTranche->BktResHoldNY() - pRelTranche->CmptResHoldNY();
			// Pour eviter de créer d'autre membre dans le dommaine YmIcRelatedDom, je réutilise 
			// BKT_RES_HOLD_NY pour récupérer l'autorisation en bucket 0
			// CMPT_RES_HOLD_NY pour récupérer la somme des ventes IDV + GRP dans l'espace physique.
			iAvail = __min (AvailSCG, AvailSCI);
			iAvail = __min (iAvail, AvailBkt0);
			pRelTranche->Availability(iAvail);
		}
		delete pIterator2;
	}
}

// NPI - Ano 70148
// La fonction BuildGraph sert désormais d'aiguillage suivant le fonctionnement de SATC
//	- classique --> BuilGraphGeneral (fontionnement par sphère de prix)
//	- ALLEO --> BuildGraphAlleo (fonctionnnement identique à ce qui se faisait avant souplesse)
void CRelTrancheView::BuildGraph(enum CCTAChartData::GraphType eGraphType)
{
	if (((CCTAApp*)APP)->GetRhealysFlag())
	{
		BuildGraphAlleo(eGraphType);
	}
	else
	{
		BuildGraphGeneral(eGraphType);
	}
}
///

//DM7978 KGH
///////////////////////////////////////////////////////
// Build the graph
//
//void CRelTrancheView::BuildGraph(enum CCTAChartData::GraphType eGraphType)
void CRelTrancheView::BuildGraphGeneral(enum CCTAChartData::GraphType eGraphType)
{
	ClearTrancheInfoLabelArray();
	m_Chart.SetRepaint( FALSE );

	// Set text in status bar control
	((CChildFrame*)GetParent())->GetStatusBar()->SetPaneOneAndLock(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BUILDING_GRAPH) );

	// transfer the data from the record set to the graph data structure
	YmIcRelatedDom* pRelTranche = NULL;
	XrtValueLabel label;

	BOOL bPrixGaranti = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag();

	// DM7978 KGH
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	YmIcTrancheLisDom* pCurTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	const char * entite_courante = pCurTranche->Entity();
	CSpherePrix  spherePrixTrouvee;
	//si SC ou CC
	SpheresPrixMap mapSpherePrix = (GetAltFlag()) ? pDoc->m_sphereManager->getListeSpherePrixSC(pCurTranche->Nature(),entite_courante) : pDoc->m_sphereManager->getListeSpherePrixCC(pCurTranche->Nature(),entite_courante);
	m_nbSpheresPrixMax = mapSpherePrix.size();
	m_nbSpheresPrixLegende = pDoc->m_sphereManager->getNbMaxSpheresPrix(mapSpherePrix);//sert pour la legende
	
	////si la liste des spheres est vide 
	if(m_nbSpheresPrixMax <= 0)
	{
		m_sphereNonTrouvee = TRUE;
		return;
	}
	//DM7978 KGH FIN

	// graph data
	double (*adXData);
	adXData = new double [m_iBarCount];
	memset( adXData, 0, sizeof(double) * m_iBarCount );

	// ADD 1 TO THE MAXIMUM NUMBER OF NEST LEVELS (BUCKETS)
	//  THIS IS A KLUGE TO GET AROUND A BUG IN OLECTRA CHART
	//  THE PROBLEM IS THE DISPLAY OF A TEXT AREA ABOVE A STACKING BAR CHART

	// DM-7978 KGH // creation du tableau des donnes pour les spheres de prix
	double (*adYDataSpherePrix);
	adYDataSpherePrix = new double[(m_nbSpheresPrixMax) * m_iBarCount];
	memset( adYDataSpherePrix, 0, sizeof(double) * (m_nbSpheresPrixMax * m_iBarCount) );
	//DM-7978 KGH FIN

	double (*adYData2);
	adYData2 = new double[eNumberDataSets2 * m_iBarCount];
	memset( adYData2, 0, sizeof(double) * (eNumberDataSets2 * m_iBarCount) );

	// create a new cta graph data object
	CCTAChartData* pCTAChartData = new CCTAChartData( &m_Chart, eGraphType );

	int  iSetIdx = -1;
	int iTrancheInfoLabelIdx = 0;
	int iBktCount = 0;

	CString szCmptHold;
	int iIndex = -1;

	int iBktAvail, iPrevBktAvail;
	iBktAvail = iPrevBktAvail = 0;

	// now loop again to build the graph
	// calculate the width of the bar to display for capacity (a percentage of first record's capacity)
	if ( m_paRelTrancheList.GetSize() <= 0 )
		return;
	RelTran_LegLevel* pLegLevel = m_paRelTrancheList.GetAt( 0 );
	if ((pLegLevel == NULL) || (pLegLevel->aRelTrancheRecs.GetSize() <= 0))
		return;
	pRelTranche = pLegLevel->aRelTrancheRecs.GetAt( 0 );
	long CapValue = pRelTranche->Capacity(); 
	double dBarThickness = (double)((float)CapValue * CAPACITY_BAR_THICKNESS);

	// loop through the tranches
	long lTrancheHold = -1;
	int iTrancheCount = m_paRelTrancheList.GetSize();
	int iRecCount = 0;
	double dMaxResHoldValue = 0;
	double dMaxGraphValue = 0;
	
	// DM7978 KGH
	// initialiser les libelles des spheres 
	pDoc->m_sphereManager->initLabelSphere(mapSpherePrix, m_iBarCount,(GetAltFlag() != 0), REL_TRANCHE);
	// DM7978 KGH FIN 

	for( int iTrancheIdx = 0; iTrancheIdx < iTrancheCount; iTrancheIdx++ )
	{
		// build the graph
		// loop through the records for this tranche
		pLegLevel = m_paRelTrancheList.GetAt( iTrancheIdx );
		iRecCount = pLegLevel->aRelTrancheRecs.GetSize();
		for ( int iBktIdx = 0; iBktIdx < iRecCount; iBktIdx++ )
		{
			long lRecette;
			pRelTranche = pLegLevel->aRelTrancheRecs.GetAt( iBktIdx );

			// DM7978 KGH			
			// recuperer la sphere de prix qui correspond au compartiment et nestLevel actuel et la nature de l'entite
			if(GetAltFlag())//SC
			{ 
				// NPI - Ano 70000
				//spherePrixTrouvee = pDoc->m_sphereManager->findSpherePrixSC(pRelTranche->NestLevel(),pRelTranche->Cmpt(),pRelTranche->Nature(),entite_courante);//chercher la sphere qui correspond a NestLevel et Cmpt
				spherePrixTrouvee = pDoc->m_sphereManager->findSpherePrixSC(pRelTranche->NestLevel(),pRelTranche->Cmpt(),pRelTranche->Nature(), pRelTranche->Entity());//chercher la sphere qui correspond a NestLevel et Cmpt
				///
			}
			else //CC
			{
				// NPI - Ano 70000
				//spherePrixTrouvee = pDoc->m_sphereManager->findSpherePrixCC(pRelTranche->NestLevel(),pRelTranche->Cmpt(),pRelTranche->Nature(),entite_courante);//chercher la sphere qui correspond a NestLevel et Cmpt
				spherePrixTrouvee = pDoc->m_sphereManager->findSpherePrixCC(pRelTranche->NestLevel(),pRelTranche->Cmpt(),pRelTranche->Nature(), pRelTranche->Entity());//chercher la sphere qui correspond a NestLevel et Cmpt
				///
			}
			//si aucune sphere trouvee 
			if(spherePrixTrouvee.getSphereId() == -1)
			{
				m_sphereNonTrouvee = TRUE;
				return;
			}
			
			int positionSphere = spherePrixTrouvee.getPosition(); // position de la sphere selon son ordonnancement par rapport au autres spheres (commence par 0)
			// DM7978 KGH FIN

			//JMG, affiche de la dispo SCG au lieu de la capa SCG sur l'onglet dispo SCI.
			if ((eGraphType == CCTAChartData::eAvail) && GetAltFlag())
			{
				int AvailScg = pRelTranche->Capacity() - pRelTranche->CmptResHoldGrp() - pRelTranche->CmptResHoldIdv();
				int AvailBkt0 = pRelTranche->BktResHoldNY() - pRelTranche->CmptResHoldNY(); // Voir explication dans CalcAvailSci
				CapValue = __min (AvailScg, AvailBkt0);
				//dBarThickness = (double)((float)CapValue * CAPACITY_BAR_THICKNESS);
			}
			else
			{
				CapValue = pRelTranche->Capacity();
				//dBarThickness = (double)((float)CapValue * CAPACITY_BAR_THICKNESS);
			}

			lRecette = pRelTranche->Revenu();
			if (bPrixGaranti)
				lRecette += pRelTranche->RevenuGaranti();

			// FORCE NEGATIVE VALUES TO ZERO !!!
			pRelTranche->Availability( __max( pRelTranche->Availability(), 0 ) );
			pRelTranche->DmdDptFc( __max( pRelTranche->DmdDptFc(), 0 ) );

			// different compartment (or tranche)?
			if ( (szCmptHold.IsEmpty() || ((pRelTranche->Cmpt()) != szCmptHold)) || 
				( pRelTranche->TrancheNo() != lTrancheHold ) )
			{
				iSetIdx = 0;
				iBktCount = 1;

				// set the availability for the lowest valued bucket
				//    because we receive the bucket data in DESCENDING ORDER 
				iBktAvail = iPrevBktAvail = pRelTranche->Availability();
				// build labels for X-Axis
				// create the array containing the cmpt / bkt labels for the X-Axis and help window				
				szCmptHold = (pRelTranche->Cmpt());    // reset cmpt hold

				iIndex++;
				label.value = (iIndex+1);          // compartment
				label.string = (char *)pRelTranche->Cmpt();  
				m_Chart.SetValueLabel(XRT_AXIS_X, &label);

				// do we have a new tranche ?
				if ( pRelTranche->TrancheNo() != lTrancheHold )
				{
					// do we need to increase the size of the current tranche text labels array ?
					// Note that this is where we allocate memory for this array, and
					//    this is where we fill those elements with data which we have here.
					//    most of the other elements are filled in the method FormatBarLabels( )
					if ( m_paTrancheInfoLabels.GetSize() <= iTrancheInfoLabelIdx )
					{
						m_paTrancheInfoLabels.SetAtGrow( iTrancheInfoLabelIdx, new TrancheInfoLabels );
						m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea = new CChart2DTextArea( &m_Chart );

						HFONT hFont = m_Chart.GetCtaHFont();
						m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetFont( hFont );
						DeleteObject( hFont );
					}

					// Fill the tranche info label array with the necessary data...
					m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->iPointIdx = iIndex;
					m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pRelTranche = pRelTranche;
					iTrancheInfoLabelIdx++;
				}
				adXData[iIndex] = iIndex + 1;
				lTrancheHold = pRelTranche->TrancheNo();

				// set the max values because we're about to reset the accumulators
				pCTAChartData->SetResHoldMax( dMaxResHoldValue );
				pCTAChartData->SetGraphMax( dMaxGraphValue );

				// reset the accumulators for the max because of the nested buckets
				switch( eGraphType )  // what are we graphing ?
				{
				case CCTAChartData::eAvail:
					dMaxGraphValue = (double)iBktAvail;
					break;
				case CCTAChartData::eResHold:
					dMaxGraphValue = (double)(pRelTranche->BktResHoldIdv()+pRelTranche->BktResHoldGrp());
					dMaxResHoldValue = (double)(pRelTranche->BktResHoldIdv()+pRelTranche->BktResHoldGrp());  // scaling option
					break;
				case CCTAChartData::eRecettes:
					dMaxGraphValue = (double)(lRecette);
					dMaxResHoldValue = (double)(lRecette);  // scaling option
					break;
				case CCTAChartData::eDmdDptFcst:
					dMaxGraphValue = (double)pRelTranche->DmdDptFc();
					break;
				}
			}
			else
			{
				iBktCount ++;

				// we must calculate the nested availability for this bucket
				//    don't forget that we get the buckets in DESCENDING ORDER !!!
				if ( eGraphType == CCTAChartData::eAvail )  // only if we're graphing availability
				{
					if (GetAltFlag()) // Sous-contingent.
						iBktAvail = pRelTranche->Availability();
					else
						iBktAvail = __max( pRelTranche->Availability() - iPrevBktAvail, 0 );
					iPrevBktAvail = pRelTranche->Availability();
				}

				// accumulate the max because of the nested buckets
				switch( eGraphType )  // what are we graphing ?
				{
				case CCTAChartData::eAvail:
					dMaxGraphValue += (double)iBktAvail;
					break;
				case CCTAChartData::eResHold:
					dMaxGraphValue += (double)(pRelTranche->BktResHoldIdv()+pRelTranche->BktResHoldGrp());
					dMaxResHoldValue += (double)(pRelTranche->BktResHoldIdv()+pRelTranche->BktResHoldGrp());  // scaling option
					break;
				case CCTAChartData::eRecettes:
					dMaxGraphValue += (double)(lRecette);
					dMaxResHoldValue += (double)(lRecette);  // scaling option
					break;
				case CCTAChartData::eDmdDptFcst:
					dMaxGraphValue += (double)pRelTranche->DmdDptFc();
					break;
				}

				iSetIdx++;
			}
			//DM7978 KGH
			int iNormalOrderIdxSpherePrix = iIndex + (positionSphere*m_iBarCount);
			//DM7978 KGH FIN
			double dGraphValue = 0;
			switch( eGraphType )  // what are we graphing ?
			{
			case CCTAChartData::eAvail:
				dGraphValue = (double)iBktAvail;
				break;
			case CCTAChartData::eResHold:
				dGraphValue = (double)(pRelTranche->BktResHoldIdv()+pRelTranche->BktResHoldGrp());
				break;
			case CCTAChartData::eRecettes:
				dGraphValue = (double)(lRecette);
				break;
			case CCTAChartData::eDmdDptFcst:
				dGraphValue = (double)pRelTranche->DmdDptFc();
				break;
			}
			// adjust the graph value -- do not allow a value > -.5 and < 0
			if ( ( dGraphValue > -.5 ) && ( dGraphValue < 0 ) )  dGraphValue = 0;

			//DM7978 KGH
			//accumuler par sphere de prix
			adYDataSpherePrix[iNormalOrderIdxSpherePrix] += dGraphValue;
			//DM7978 KGH FIN

			// add capacity...
			if (eGraphType != CCTAChartData::eRecettes)
			{
				adYData2[iIndex + (eEmptySpace*m_iBarCount)] = (double)CapValue - dBarThickness;
				adYData2[iIndex + (eCapacity*m_iBarCount)] = dBarThickness;
			}
			else
			{
				adYData2[iIndex + (eEmptySpace*m_iBarCount)] = 0;
				adYData2[iIndex + (eCapacity*m_iBarCount)] = 0;
			}

			pCTAChartData->SetGraphMax( dMaxGraphValue );  // is this the max graph value ?
			pCTAChartData->SetGraphMax( (double)pRelTranche->Capacity() );  // is this the max graph value ?
		}  // end of iteration through records
	}
	pCTAChartData->SetNestLevelCount(iBktCount);

	// add the last compartment
	label.value = (iIndex+1);
	label.string = (char*)pRelTranche->Cmpt();
	m_Chart.SetValueLabel(XRT_AXIS_X, &label);

	CChart2DData *pData2 = new CChart2DData(XRT_DATA_ARRAY, eNumberDataSets2, m_iBarCount);
	CChart2DData *pDataSpherePrix =  new CChart2DData(XRT_DATA_ARRAY, (m_nbSpheresPrixMax), m_iBarCount);//DM7978 KGH

	// set the X elements
	for (iIndex = 0; iIndex < m_iBarCount; iIndex++){
		pDataSpherePrix->SetXElement(NULL, iIndex, adXData[iIndex]);
	}

	// set the Y elements
	for (iIndex = 0; iIndex < (m_nbSpheresPrixMax); iIndex++){
		pDataSpherePrix->SetYData(iIndex, adYDataSpherePrix+(iIndex*m_iBarCount), m_iBarCount, 0);
	}
	// DM7978 KGH FIN

	if ( eGraphType != CCTAChartData::eRecettes )
	{
	// set the X elements for the 2nd graph (capacity)
	for (iIndex = 0; iIndex < m_iBarCount; iIndex++){
		pData2->SetXElement(NULL, iIndex, adXData[iIndex]);
	}

	// set the Y elements for the 2nd graph (capacity)
	for (iIndex = 0; iIndex < eNumberDataSets2; iIndex++){
		pData2->SetYData(iIndex,  adYData2+(iIndex*m_iBarCount), m_iBarCount, 0);
	}

	// hide the portion of the (stacked) bar below the capacity
	//   to make it look like a solid line
	pData2->SetDisplay(eEmptySpace, XRT_DISPLAY_HIDE);
	}

	// set the Olectra data objects in the CTA data object
	// DM7978 - KGH 
	pCTAChartData->SetDataObj(pDataSpherePrix);
	// DM7978 - KGH FIN
	pCTAChartData->SetDataObj2(pData2);

	m_Chart.AddChartData( pCTAChartData, FALSE );  // add this Chart Data element to the array

	// cleanup
	// DM7978 - KGH
	delete [ ] adYDataSpherePrix;
	adYDataSpherePrix = NULL;
	// DM7978 - KGH FIN
	delete [ ] adYData2;
	adYData2 = NULL;
	delete [ ] adXData;
	adXData = NULL;

	m_Chart.SetRepaint( TRUE );
}
//DM7978 KGH
//void CRelTrancheView::BuildGraph(enum CCTAChartData::GraphType eGraphType)
void CRelTrancheView::BuildGraphAlleo(enum CCTAChartData::GraphType eGraphType)
{
	ClearTrancheInfoLabelArray();
  ClearCmptBktLabelArray();

  m_Chart.SetRepaint( FALSE );

  // Set text in status bar control
  ((CChildFrame*)GetParent())->
    GetStatusBar()->SetPaneOneAndLock(
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BUILDING_GRAPH) );

  // transfer the data from the record set to the graph data structure
  YmIcRelatedDom* pRelTranche = NULL;
  XrtValueLabel label;

  BOOL bPrixGaranti = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag();

  // graph data
  double (*adXData);
  adXData = new double [m_iBarCount];
  memset( adXData, 0, sizeof(double) * m_iBarCount );

  // ADD 1 TO THE MAXIMUM NUMBER OF NEST LEVELS (BUCKETS)
  //  THIS IS A KLUGE TO GET AROUND A BUG IN OLECTRA CHART
  //  THE PROBLEM IS THE DISPLAY OF A TEXT AREA ABOVE A STACKING BAR CHART
  double (*adYData);
  adYData = new double[(m_iNstLvlMaxCount+1) * m_iBarCount];
  memset( adYData, 0, sizeof(double) * ((m_iNstLvlMaxCount+1) * m_iBarCount) );

  double (*adYData2);
  adYData2 = new double[eNumberDataSets2 * m_iBarCount];
  memset( adYData2, 0, sizeof(double) * (eNumberDataSets2 * m_iBarCount) );

  // create a new cta graph data object
  CCTAChartData* pCTAChartData = 
  new CCTAChartData( &m_Chart, eGraphType );

  int  iSetIdx = -1;
  int iTrancheInfoLabelIdx = 0;
  int iCmptBktLabelIdx = -1;
  int iBktLabelIdx = 0;
  int iBktCount = 0;

  CString szCmptHold;
  int iIndex = -1;

  int iBktAvail, iPrevBktAvail;
  iBktAvail = iPrevBktAvail = 0;

  // now loop again to build the graph
  // calculate the width of the bar to display for capacity (a percentage of first record's capacity)
  if ( m_paRelTrancheList.GetSize() <= 0 )
    return;
  RelTran_LegLevel* pLegLevel = m_paRelTrancheList.GetAt( 0 );
  if ((pLegLevel == NULL) || (pLegLevel->aRelTrancheRecs.GetSize() <= 0))
    return;
  pRelTranche = pLegLevel->aRelTrancheRecs.GetAt( 0 );
  long CapValue = pRelTranche->Capacity(); 
  double dBarThickness = (double)((float)CapValue * CAPACITY_BAR_THICKNESS);

  // loop through the tranches
  long lTrancheHold = -1;
  int iTrancheCount = m_paRelTrancheList.GetSize();
  int iRecCount = 0;
  double dMaxResHoldValue = 0;
  double dMaxGraphValue = 0;
  for( int iTrancheIdx = 0; iTrancheIdx < iTrancheCount; iTrancheIdx++ )
  {
    // build the graph

    // loop through the records for this tranche
    pLegLevel = m_paRelTrancheList.GetAt( iTrancheIdx );
    iRecCount = pLegLevel->aRelTrancheRecs.GetSize();
    for ( int iBktIdx = 0; iBktIdx < iRecCount; iBktIdx++ )
    {
	  long lRecette;
      pRelTranche = pLegLevel->aRelTrancheRecs.GetAt( iBktIdx );

	  //JMG, affiche de la dispo SCG au lieu de la capa SCG sur l'onglet dispo SCI.
	  if ((eGraphType == CCTAChartData::eAvail) && GetAltFlag())
	  {
		int AvailScg = pRelTranche->Capacity() - pRelTranche->CmptResHoldGrp() - pRelTranche->CmptResHoldIdv();
		int AvailBkt0 = pRelTranche->BktResHoldNY() - pRelTranche->CmptResHoldNY(); // Voir explication dans CalcAvailSci
		CapValue = __min (AvailScg, AvailBkt0);
		//dBarThickness = (double)((float)CapValue * CAPACITY_BAR_THICKNESS);
	  }
	  else
	  {
	    CapValue = pRelTranche->Capacity();
		//dBarThickness = (double)((float)CapValue * CAPACITY_BAR_THICKNESS);
	  }

	  lRecette = pRelTranche->Revenu();
	  if (bPrixGaranti)
		lRecette += pRelTranche->RevenuGaranti();

      // FORCE NEGATIVE VALUES TO ZERO !!!
      pRelTranche->Availability( __max( pRelTranche->Availability(), 0 ) );
      pRelTranche->DmdDptFc( __max( pRelTranche->DmdDptFc(), 0 ) );

      // different compartment (or tranche)?
      if ( (szCmptHold.IsEmpty() || ((pRelTranche->Cmpt()) != szCmptHold)) || 
        ( pRelTranche->TrancheNo() != lTrancheHold ) )
      {
        iSetIdx = 0;
        iBktCount = 1;

        // set the availability for the lowest valued bucket
        //    because we receive the bucket data in DESCENDING ORDER 
        iBktAvail = iPrevBktAvail = pRelTranche->Availability();

        // build labels for X-Axis
        // create the array containing the cmpt / bkt labels for the X-Axis and help window
        iCmptBktLabelIdx++;
        iBktLabelIdx = 0;
        CmptBktLabels* pCmptBktLabels = new CmptBktLabels;
        m_paCmptBktLabels.Add( pCmptBktLabels );
/*
        if ( m_paCmptBktLabels.GetSize() <= iCmptBktLabelIdx )
          m_paCmptBktLabels.SetAtGrow( iCmptBktLabelIdx, new CmptBktLabels );
*/
        m_paCmptBktLabels[ iCmptBktLabelIdx ]->szCmpt = (pRelTranche->Cmpt());

        szCmptHold = (pRelTranche->Cmpt());    // reset cmpt hold

        iIndex++;
        label.value = (iIndex+1);          // compartment
        label.string = (char *)pRelTranche->Cmpt();
        m_Chart.SetValueLabel(XRT_AXIS_X, &label);

        // do we have a new tranche ?
        if ( pRelTranche->TrancheNo() != lTrancheHold )
        {
          // do we need to increase the size of the current tranche text labels array ?
          // Note that this is where we allocate memory for this array, and
          //    this is where we fill those elements with data which we have here.
          //    most of the other elements are filled in the method FormatBarLabels( )
          if ( m_paTrancheInfoLabels.GetSize() <= iTrancheInfoLabelIdx )
          {
            m_paTrancheInfoLabels.SetAtGrow( iTrancheInfoLabelIdx, new TrancheInfoLabels );
            m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea = new CChart2DTextArea( &m_Chart );

            HFONT hFont = m_Chart.GetCtaHFont();
            m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pChartTextArea->SetFont( hFont );
            DeleteObject( hFont );
          }

          // Fill the tranche info label array with the necessary data...
          m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->iPointIdx = iIndex;
          m_paTrancheInfoLabels[iTrancheInfoLabelIdx]->pRelTranche = pRelTranche;
          iTrancheInfoLabelIdx++;
        }
        adXData[iIndex] = iIndex + 1;
        lTrancheHold = pRelTranche->TrancheNo();

        // set the max values because we're about to reset the accumulators
        pCTAChartData->SetResHoldMax( dMaxResHoldValue );
        pCTAChartData->SetGraphMax( dMaxGraphValue );

        // reset the accumulators for the max because of the nested buckets
        switch( eGraphType )  // what are we graphing ?
        {
          case CCTAChartData::eAvail:
            dMaxGraphValue = (double)iBktAvail;
            break;
          case CCTAChartData::eResHold:
            dMaxGraphValue = (double)(pRelTranche->BktResHoldIdv()+pRelTranche->BktResHoldGrp());
            dMaxResHoldValue = (double)(pRelTranche->BktResHoldIdv()+pRelTranche->BktResHoldGrp());  // scaling option
            break;
          case CCTAChartData::eRecettes:
			dMaxGraphValue = (double)(lRecette);
            dMaxResHoldValue = (double)(lRecette);  // scaling option
			break;
          case CCTAChartData::eDmdDptFcst:
            dMaxGraphValue = (double)pRelTranche->DmdDptFc();
            break;
        }
      }
      else
      {
        iBktCount ++;

        // we must calculate the nested availability for this bucket
        //    don't forget that we get the buckets in DESCENDING ORDER !!!
        if ( eGraphType == CCTAChartData::eAvail )  // only if we're graphing availability
        {
          if (GetAltFlag()) // Sous-contingent.
			iBktAvail = pRelTranche->Availability();
		  else
			iBktAvail = __max( pRelTranche->Availability() - iPrevBktAvail, 0 );
          iPrevBktAvail = pRelTranche->Availability();
        }

        // accumulate the max because of the nested buckets
        switch( eGraphType )  // what are we graphing ?
        {
          case CCTAChartData::eAvail:
            dMaxGraphValue += (double)iBktAvail;
            break;
          case CCTAChartData::eResHold:
            dMaxGraphValue += (double)(pRelTranche->BktResHoldIdv()+pRelTranche->BktResHoldGrp());
            dMaxResHoldValue += (double)(pRelTranche->BktResHoldIdv()+pRelTranche->BktResHoldGrp());  // scaling option
            break;
		  case CCTAChartData::eRecettes:
            dMaxGraphValue += (double)(lRecette);
            dMaxResHoldValue += (double)(lRecette);  // scaling option
            break;
          case CCTAChartData::eDmdDptFcst:
            dMaxGraphValue += (double)pRelTranche->DmdDptFc();
            break;
        }

        iSetIdx++;
      }

      // add the bucket to the X-Axis label for this compartment
      m_paCmptBktLabels[ iCmptBktLabelIdx ]->aszBktArray.Add( pRelTranche->BucketLabel() );
//      m_paCmptBktLabels[ iCmptBktLabelIdx ]->aszBktArray.SetAtGrow( iBktLabelIdx, pRelTranche->BucketLabel() );
      iBktLabelIdx++;

      int iNormalOrderIdx = iIndex + (iSetIdx*m_iBarCount);
      double dGraphValue = 0;
      switch( eGraphType )  // what are we graphing ?
      {
        case CCTAChartData::eAvail:
          dGraphValue = (double)iBktAvail;
          break;
        case CCTAChartData::eResHold:
          dGraphValue = (double)(pRelTranche->BktResHoldIdv()+pRelTranche->BktResHoldGrp());
          break;
		case CCTAChartData::eRecettes:
          dGraphValue = (double)(lRecette);
          break;
        case CCTAChartData::eDmdDptFcst:
          dGraphValue = (double)pRelTranche->DmdDptFc();
          break;
      }
      // adjust the graph value -- do not allow a value > -.5 and < 0
      if ( ( dGraphValue > -.5 ) && ( dGraphValue < 0 ) )
        dGraphValue = 0;

      adYData[ iNormalOrderIdx ] = dGraphValue;  // set this value in the Y data set

      // add capacity...
	  if (eGraphType != CCTAChartData::eRecettes)
	  {
		adYData2[iIndex + (eEmptySpace*m_iBarCount)] = (double)CapValue - dBarThickness;
		adYData2[iIndex + (eCapacity*m_iBarCount)] = dBarThickness;
	  }
	  else
	  {
		adYData2[iIndex + (eEmptySpace*m_iBarCount)] = 0;
		adYData2[iIndex + (eCapacity*m_iBarCount)] = 0;
	  }

      pCTAChartData->SetGraphMax( dMaxGraphValue );  // is this the max graph value ?
      pCTAChartData->SetGraphMax( (double)pRelTranche->Capacity() );  // is this the max graph value ?
    }  // end of iteration through records
  }
  pCTAChartData->SetNestLevelCount(iBktCount);

  // add the last compartment
  label.value = (iIndex+1);
  label.string = (char*)pRelTranche->Cmpt();
  m_Chart.SetValueLabel(XRT_AXIS_X, &label);

  CChart2DData *pData = 
    new CChart2DData(XRT_DATA_ARRAY, (m_iNstLvlMaxCount+1), m_iBarCount);

  CChart2DData *pData2 = 
    new CChart2DData(XRT_DATA_ARRAY, eNumberDataSets2, m_iBarCount);

  // set the X elements
  for (iIndex = 0; iIndex < m_iBarCount; iIndex++)
    pData->SetXElement(NULL, iIndex, adXData[iIndex]);

  // set the Y elements
  for (iIndex = 0; iIndex < (m_iNstLvlMaxCount+1); iIndex++)
    pData->SetYData(iIndex, adYData+(iIndex*m_iBarCount), m_iBarCount, 0);

/*  if ( eGraphType != CCTAChartData::eRecettes )
  {*/
	    // set the X elements for the 2nd graph (capacity)
		for (iIndex = 0; iIndex < m_iBarCount; iIndex++)
			pData2->SetXElement(NULL, iIndex, adXData[iIndex]);

		// set the Y elements for the 2nd graph (capacity)
		for (iIndex = 0; iIndex < eNumberDataSets2; iIndex++)
 			pData2->SetYData(iIndex,  adYData2+(iIndex*m_iBarCount), m_iBarCount, 0);
  
		// hide the portion of the (stacked) bar below the capacity
		//   to make it look like a solid line
		pData2->SetDisplay(eEmptySpace, XRT_DISPLAY_HIDE);
 /* }*/

  // set the Olectra data objects in the CTA data object
  pCTAChartData->SetDataObj(pData);
  pCTAChartData->SetDataObj2(pData2);

  m_Chart.AddChartData( pCTAChartData, FALSE );  // add this Chart Data element to the array

  // cleanup
  delete [ ] adYData;
  adYData = NULL;
  delete [ ] adYData2;
  adYData2 = NULL;
  delete [ ] adXData;
  adXData = NULL;

  m_Chart.SetRepaint( TRUE );
}

///////////////////////////////////////////////////////
// Format the (variable) X axis label
//
char* CRelTrancheView::FormatVarTranInfoText( char* szLabelText, YmIcRelatedDom* pRelTranche )
{
	CString szTempLabelText;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrancheNoFlag() )
	{
		if ( m_bDisplayTrancheNo )
		{
			CString szTemp;
			szTemp.Format("#%d  ", pRelTranche->TrancheNo() );
			szTempLabelText += szTemp;
		}
		/*
		if ( m_bDisplayTrancheDptTime )
		{
		CString szTemp;
		szTemp.Format("%0.2d:%0.2d  ", pRelTranche->TrancheDptTime() / 100, pRelTranche->TrancheDptTime() % 100 );
		szTempLabelText += szTemp;
		}
		*/
	}
	if ( m_bDisplayTrainNo )
	{
		CString szTemp;
		szTemp.Format("%s  ", pRelTranche->TrainNo() );
		szTempLabelText += szTemp;
	}
	if (m_bDisplayOrigDest)
	{
		CString szTemp;
		szTemp.Format("%s-%s  ", pRelTranche->LegOrigLabel(), pRelTranche->LegDestLabel());
		szTempLabelText += szTemp;
	}
	/*
	if ( m_bDisplayLegDptTime )
	{
	CString szTemp;
	szTemp.Format("%0.2d:%0.2d  ", pRelTranche->LegDptTime() / 100, pRelTranche->LegDptTime() % 100 );
	szTempLabelText += szTemp;
	}
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSupplementLevelFlag() )
	if ( m_bDisplaySuppLevel )
	{
	CString szTemp;
	szTemp.Format("N%d  ", pRelTranche->SupplementLevel() );
	szTempLabelText += szTemp;
	}
	*/

	szLabelText[0] = '\0';
	if ( ! szTempLabelText.IsEmpty() )
		strcpy( szLabelText, (LPCSTR)szTempLabelText );

	return szLabelText;
}

///////////////////////////////////////////////////////
// Format the X axis title
//
void CRelTrancheView::FormatXTitleText( CString& szTitleText )
{
	/*
	CString szTitleTextSupp;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrancheNoFlag() )
	{
	if ( m_bDisplayTrancheNo )
	{
	szTitleTextSupp += "(";
	szTitleTextSupp += 
	((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TRANCHE_NO);
	}
	if ( m_bDisplayDptTime )
	{
	szTitleTextSupp.IsEmpty() ? szTitleTextSupp += "(" : szTitleTextSupp += "/";
	szTitleTextSupp += 
	((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DPT_TIME);
	}
	}
	if ( m_bDisplayTrainNo )
	{
	szTitleTextSupp.IsEmpty() ? szTitleTextSupp += "(" : szTitleTextSupp += "/";
	szTitleTextSupp += 
	((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TRAIN_NO);
	}
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSupplementLevelFlag() )
	if ( m_bDisplaySuppLevel )
	{
	szTitleTextSupp.IsEmpty() ? szTitleTextSupp += "(" : szTitleTextSupp += "/";
	szTitleTextSupp += 
	((CCTAApp*)APP)->GetResource()->LoadResString(IDS_SUPPLEMENT_LEVEL);
	}
	if ( ! szTitleTextSupp.IsEmpty() )
	szTitleTextSupp += ")";

	szTitleText = 
	((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RELATEDTRANCHE_FOOTER);
	szTitleText += " ";
	szTitleText += szTitleTextSupp;
	*/
	szTitleText = 
		((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RELATEDTRANCHE_FOOTER);
}

////////////////////////////////////////////////////////////////////
// Draw the graph -- use the tab control data to tell us which one.
//
void CRelTrancheView::ShowGraph()
{
	// get the currently selected chart data element 
	CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
	//DM7978 KGH 
	// NPI - Ano 70148
	/*// affichage du message indiquant 
	if(m_sphereNonTrouvee)
	{
		((CChildFrame*)GetParent())->GetStatusBar()->SetPaneOneAndLock(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_SPHERE) );
		return;
	}*/

	if (((CCTAApp*)APP)->GetRhealysFlag())
	{
		if (m_sphereNonTrouvee)
		{
			((CChildFrame*)GetParent())->GetStatusBar()->SetPaneOneAndLock(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_SPHERE) );
			return;
		}
	}
	//DM7978 KGH FIN
	if ( ( pCTAChartData == NULL ) || ( m_ChartTabCtrl.GetItemCount() == 0 ) )
	{
		// Set text in status bar control
		((CChildFrame*)GetParent())->
			GetStatusBar()->SetPaneOneAndLock(
			((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA) );
		return;
	}

	m_Chart.SetGraphFont();  // set some chart properties
	m_Chart.RemoveFlashLabel();  // remove flash label if up
	m_Chart.SetRepaint( FALSE );  // avoid the flash, turn repaint off here, and back on later

	m_Chart.SetGraphProperties(pCTAChartData->GetGraphType());
	// set the graph data object for the tab selected
	m_Chart.SetData( *(pCTAChartData->GetDataObj()) );
	m_Chart.SetData2( *(pCTAChartData->GetDataObj2()) );

	// display the window title
	//  we must determine if the graph is based upon the selected leg or a selected market
	YM_RecordSet* pLegSet = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
	YmIcLegsDom* pLeg = (YmIcLegsDom*)pLegSet->GetCurrent();
	CString szTitleText = 
		((CCTAApp*)APP)->GetResource()->LoadResString((GetAltFlag()) ? IDS_RELTRANCHE_SCI_TITLE : IDS_RELTRANCHE_TITLE) +
		" : " + 
		( ( ((CRelatedSet *)((CCTAApp*)APP)->GetDocument()->m_pRelTrancheSet)->m_bMktSelect  ) ? m_pMkt.szLegOrig : pLeg->LegOrig() ) +
		" - " +
		( ( ((CRelatedSet *)((CCTAApp*)APP)->GetDocument()->m_pRelTrancheSet)->m_bMktSelect  ) ? m_pMkt.szLegDest : pLeg->LegDest() );
	GetParentFrame()->SetWindowText(szTitleText);

	/* test JMG le 18/10/2005, on enlève le texte "Espace Physique", pour gagner de la place
	CString szGraphFooter;
	FormatXTitleText( szGraphFooter );
	char *szFooter[] = { (LPSTR)(LPCSTR)szGraphFooter, NULL };
	m_Chart.SetFooterStrings(szFooter);
	*/

	// format and attach the tranche info text labels...
	FormatBarLabels( pCTAChartData->GetGraphType() );

	// Set text in status bar control
	((CChildFrame*)GetParent())->
		GetStatusBar()->SetPaneOneAndLock(
		((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DONE) );

	m_Chart.SetY2AxisMin(m_Chart.GetYAxisMin());
	m_Chart.SetY2AxisShow(FALSE);
	m_Chart.SetFrontDataset(XRT_DATASET2);

	// this gets complicated because we must know if the user has scaled for res hold...
	if ( (( pCTAChartData->GetGraphType() == CCTAChartData::eResHold ) ||
		( pCTAChartData->GetGraphType() == CCTAChartData::eResHold ))&& ( m_bResHoldScale ) )
		m_Chart.SetCurScaling(CCTAChart::eResHoldScaleIdx);
	else
		m_Chart.SetCurScaling(CCTAChart::eMaxScaleIdx);

	m_Chart.ScaleGraph(m_Chart.GetCurScaling());  // don't forget to rescale the graph
	m_Chart.SetRepaint( TRUE );  // okay, paint it !
	m_Chart.ShowWindow( SW_SHOW );
}

// NPI - Ano 70148
// Réactivation de la méthode ci-dessous
//DM7978 KGH cette methode n'est plus utilisee
//////////////////////////////////////////////////////////
// Clear the array containing the tranche info for the labels
//
void CRelTrancheView::ClearCmptBktLabelArray()
{
	for (int i = 0; i < m_paCmptBktLabels.GetSize(); i++)
	{
		delete m_paCmptBktLabels[i];  // delete the text label
		m_paCmptBktLabels[i] = NULL;
	}
	m_paCmptBktLabels.RemoveAll( );
}
//DM7978 FIN KGH
//



//////////////////////////////////////////////////////////
// Clear the array containing the tranche info for the labels
//
void CRelTrancheView::ClearTrancheInfoLabelArray()
{
	for (int i = 0; i < m_paTrancheInfoLabels.GetSize(); i++)
	{
		delete m_paTrancheInfoLabels[i]->pChartTextArea;
		m_paTrancheInfoLabels[i]->pChartTextArea = NULL;
		delete m_paTrancheInfoLabels[i];  // delete the text label
		m_paTrancheInfoLabels[i] = NULL;
	}
	m_paTrancheInfoLabels.RemoveAll( );
}

//////////////////////////////////////////////////////////
// Clear the array containing the tranche info for the labels
//
void CRelTrancheView::ClearRelTrancheList()
{
	int iTrancheCount = m_paRelTrancheList.GetSize();
	for( int iTrancheIdx = 0; iTrancheIdx < iTrancheCount; iTrancheIdx++ )
	{
		RelTran_LegLevel* pLegLevel = m_paRelTrancheList.GetAt( iTrancheIdx );
		if (pLegLevel)
			delete pLegLevel;
		m_paRelTrancheList[ iTrancheIdx ] = NULL;
	}
	m_paRelTrancheList.RemoveAll();
}

//////////////////////////////////////////////////////////
// Clear the tabs array and removes the corresponding tab
//
void CRelTrancheView::ClearTabs()
{
	// clean up the array containing the tabs and graph data objects
	m_ChartTabCtrl.DeleteAllItems( ); // remove the tab from the control
}

//////////////////////////////////////
// Build the tabs on the tab control
//
void CRelTrancheView::BuildTabs()
{
	int nLastTab = -1;
	int Flag_BuildOK;
	CCTAChartDataArray* apCTAChartData = m_Chart.GetChartDataArray();

	if (apCTAChartData->GetSize() > 0)
	{
		TC_ITEM TabCtrlItem;
		TabCtrlItem.mask = TCIF_TEXT;
		CCTAChartData* pCTAChartData = NULL;
		CString szBuffer;

		// loop through the tab index array, build and insert each tab
		for (int iIndex=0; iIndex < apCTAChartData->GetSize(); iIndex++)
		{
			Flag_BuildOK = 1;
			szBuffer = "";
			pCTAChartData = m_Chart.GetChartData(iIndex);

			if ( pCTAChartData->GetGraphType() == CCTAChartData::eRecettes )
			{
				szBuffer = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RECETTES);
				szBuffer += " ";
				index_recette = iIndex;
				if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag())
					Flag_BuildOK = 0; // Ce n'est pas un client "revenu	  
			}
			if (Flag_BuildOK == 1)
			{
				szBuffer += GetTabText( pCTAChartData->GetGraphType() );

				TabCtrlItem.pszText = (LPSTR)(LPCSTR)szBuffer;
				if (_sLastTabUsed == szBuffer)
					nLastTab = iIndex;
				m_ChartTabCtrl.InsertItem( iIndex, &TabCtrlItem );
			}
		}

		if (!m_bTabResized) // We enlarge a little
		{
			CRect TabRect;
			CSize TabSize;
			m_ChartTabCtrl.GetItemRect( 0, &TabRect );
			//DM7978 KGH pour afficher les libelles des onglets
			TabSize.cx = long( (TabRect.right - TabRect.left) * 1.5 );  // resize the tabs to 1/2 of their width
			//TabSize.cx = long( (TabRect.right - TabRect.left) * 1.25 );  // resize the tabs to 1/4 of their width
			//DM7978 KGH FIN
			TabSize.cy = TabRect.bottom - TabRect.top;
			m_ChartTabCtrl.SetItemSize( TabSize );
			m_bTabResized = TRUE;
		}

		if ( (m_iCurTabSelection < 0) || (m_iCurTabSelection >= m_ChartTabCtrl.GetItemCount( )) )
			m_iCurTabSelection = 0;

		m_Chart.SetCurChartData( m_iCurTabSelection );
		m_ChartTabCtrl.SetCurSel( m_iCurTabSelection );
	}
	else
		m_iCurTabSelection = -1;

	ShowGraph();

	if (nLastTab != -1)
	{
		m_ChartTabCtrl.SetCurSel( nLastTab );
		LRESULT result;
		OnChartSelchange( NULL, &result );
	}
}

//////////////////////////////////////////////
// Convert the selected graph point into an index into the tranche info labels array
//
int CRelTrancheView::GetTrancheInfoLabelIdx( int iPointIdx )
{
	int iLabelCount = m_paTrancheInfoLabels.GetSize();
	int iIdx;
	for (iIdx = 0; iIdx < iLabelCount; iIdx++ )
	{
		if ( m_paTrancheInfoLabels[ iIdx ]->iPointIdx  == iPointIdx )
			return iIdx;
		if ( m_paTrancheInfoLabels[ iIdx ]->iPointIdx  > iPointIdx )
			return iIdx - 1;
	}
	return iIdx - 1;
}

//////////////////////////////////////
// Write specific view configuration information to the registry
//
void CRelTrancheView::WriteViewConfig()
{
	// save the graph flags in the registry
	//
	CString szProfileKey;
	((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

	APP->WriteProfileInt( szProfileKey, REG_ATTACH_LABELS_TO_CAPACITY, m_bAttachLabelsToCapacity );
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrancheNoFlag() )
	{
		APP->WriteProfileInt( szProfileKey, REG_DISPLAY_TRANCHE_NO, m_bDisplayTrancheNo );
		APP->WriteProfileInt( szProfileKey, REG_DISPLAY_TRANCHE_DPT_TIME, m_bDisplayTrancheDptTime );
	}
	APP->WriteProfileInt( szProfileKey, REG_DISPLAY_TRAIN_NO, m_bDisplayTrainNo );
	APP->WriteProfileInt( szProfileKey, REG_DISPLAY_LEG_ORIG, m_bDisplayOrigDest );
	APP->WriteProfileInt( szProfileKey, REG_DISPLAY_LEG_DPT_TIME, m_bDisplayLegDptTime );
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSupplementLevelFlag() )
		APP->WriteProfileInt( szProfileKey, REG_DISPLAY_SUPP_LEVEL, m_bDisplaySuppLevel );
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTRNFlag() )
		APP->WriteProfileInt( szProfileKey, REG_DISPLAY_TGV_DATA, m_bTRNJoinFlag );

	if ( m_bResHoldScale)
		APP->WriteProfileInt( szProfileKey, GRAPH_SCALE_FACTOR, CCTAChart::eResHoldScaleIdx );
	else
		APP->WriteProfileInt( szProfileKey, GRAPH_SCALE_FACTOR, m_Chart.GetCurScaling() );
	APP->WriteProfileString( szProfileKey, "LastTabUsed", _sLastTabUsed );
}

//////////////////////////////////////
// Restore specific view configuration information from the registry
//
void CRelTrancheView::RestoreViewConfig()
{
	// restore the graph flags from the registry
	//
	CString szProfileKey;
	((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

	m_bAttachLabelsToCapacity = APP->GetProfileInt(szProfileKey, REG_ATTACH_LABELS_TO_CAPACITY, 0);
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrancheNoFlag() )
	{
		m_bDisplayTrancheNo = APP->GetProfileInt(szProfileKey, REG_DISPLAY_TRANCHE_NO, 1);
		m_bDisplayTrancheDptTime = APP->GetProfileInt(szProfileKey, REG_DISPLAY_TRANCHE_DPT_TIME, 1);
	}
	m_bDisplayTrainNo = APP->GetProfileInt(szProfileKey, REG_DISPLAY_TRAIN_NO, 1);
	m_bDisplayOrigDest = APP->GetProfileInt(szProfileKey, REG_DISPLAY_LEG_ORIG, 1);
	m_bDisplayLegDptTime = APP->GetProfileInt(szProfileKey, REG_DISPLAY_LEG_DPT_TIME, 1);
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSupplementLevelFlag() )
		m_bDisplaySuppLevel = APP->GetProfileInt(szProfileKey, REG_DISPLAY_SUPP_LEVEL, 1);
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTRNFlag() )
		m_bTRNJoinFlag = APP->GetProfileInt(szProfileKey, REG_DISPLAY_TGV_DATA, 1);

	_sLastTabUsed = APP->GetProfileString(szProfileKey, "LastTabUsed", "" );

	m_Chart.SetRepaint( FALSE );
	enum CCTAChart::GraphScaleIndex eCurScale = (enum CCTAChart::GraphScaleIndex)
		APP->GetProfileInt(szProfileKey, GRAPH_SCALE_FACTOR, (unsigned int)CCTAChart::eMaxScaleIdx);
	m_Chart.SetCurScaling(eCurScale);
	m_Chart.ScaleGraph(m_Chart.GetCurScaling());  // don't forget to rescale the graph
	FormatBarLabels();
	m_Chart.SetRepaint( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CRelTrancheChart

BEGIN_MESSAGE_MAP(CRelTrancheChart, CCTAChart)
	//{{AFX_MSG_MAP(CRelTrancheChart)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////
// Constructor
//
CRelTrancheChart::CRelTrancheChart()
{
	m_bRotateLabels = FALSE;
	m_paSphereDataStyles = NULL;//DM7978 KGH
}

//DM7978 KGH
CRelTrancheChart::~CRelTrancheChart()
{
	if (m_paSphereDataStyles != NULL)
	{
		// NPI - Ano 70149
		for (int i = 0 ; i < m_nSpherePrix + 1; i++)
		{
			XrtDataStyle* dataStyle = m_paSphereDataStyles[i];
			if (dataStyle != NULL)
			{
				delete dataStyle;
			}
		}
		///

		delete [] m_paSphereDataStyles;
		m_paSphereDataStyles = NULL;
	}
}
//DM7978 KGH FIN


//////////////////////////////////////////
// Set the graph properties 
//

// NPI - Ano 70148
// La fonction SetGraphProperties sert désormais d'aiguillage suivant le fonctionnement de SATC
//	- classique --> SetGraphPropertiesGeneral (fontionnement par sphère de prix)
//	- ALLEO --> SetGraphPropertiesAlleo (fonctionnnement identique à ce qui se faisait avant souplesse)
void CRelTrancheChart::SetGraphProperties(enum CCTAChartData::GraphType eGraphType)
{
	if (((CCTAApp*)APP)->GetRhealysFlag())
	{
		SetGraphPropertiesAlleo(eGraphType);
	}
	else
	{
		SetGraphPropertiesGeneral(eGraphType);
	}
}
///

//DM7978 KGH
//cette methode remplace l'ancienne afin d'afficher le graphe par spheres de prix
//void CRelTrancheChart::SetGraphProperties(enum CCTAChartData::GraphType eGraphType)
void CRelTrancheChart::SetGraphPropertiesGeneral(enum CCTAChartData::GraphType eGraphType)
{
	CCTAChart::SetGraphFont();

	SetType(XRT_TYPE_STACKING_BAR);
	SetType2(XRT_TYPE_STACKING_BAR);

	// for the stacked bar, we must accumulate the dataset values for the set 
	//   to be displayed on the flash label (see CCTAChart)
	m_bAccumulateSets_DataSet2 = TRUE;

	( m_bRotateLabels ) ? SetXAnnotationRotation(XRT_ROTATE_90) : SetXAnnotationRotation(XRT_ROTATE_NONE);

	SetXAnnotationMethod(XRT_ANNO_VALUE_LABELS);

	SetGraphShowOutlines(((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetGraphOutlineFlag());

	// define the set labels (legend) for each bar component (bkt)
	SetLegendShow(FALSE);  // but turn it off

	// we invoke these methods here because we no longer need to dynamically
	//    set these properties based upon the type of graph selected 
	//    (i.e., no longer a need to reverse the data)

	//recuperer les couleurs des spheres de prix et creer des styles pour les contenir
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	CRelTrancheView* parentView = (CRelTrancheView*)GetParentView();
	bool altFlag = (parentView->GetAltFlag() != 0);
	YmIcTrancheLisDom* pCurTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);

	//SpheresPrixMap mapSpherePrix = (altFlag) ? pDoc->m_sphereManager->getListeSpherePrixSC(pCurTranche->Nature(),pCurTranche->Entity()) : pDoc->m_sphereManager->getListeSpherePrixCC(pCurTranche->Nature(),pCurTranche->Entity());
	//const int nb_spheresPrix = mapSpherePrix.size();
	
	std::list<COLORREF> listeCouleurRGB  = pDoc->m_sphereManager->getListeCouleurSpheres(altFlag,pCurTranche->Nature(),pCurTranche->Entity());
	// NPI - Ano 70148
	//const int nb_spheresPrix = listeCouleurRGB.size();
	m_nSpherePrix = listeCouleurRGB.size();
	///
	
	if (m_paSphereDataStyles == NULL) // creer les nb_spheresPrix styles pour les spheres de prix + NULL a la fin
	{
		// NPI - Ano 70148
		//m_paSphereDataStyles = new XrtDataStyle*[nb_spheresPrix + 1];
		m_paSphereDataStyles = new XrtDataStyle*[m_nSpherePrix + 1];
		///
	}

	// NPI - Ano 70148
	//for(int i = 0 ; i < nb_spheresPrix + 1; i++)	// initialiser les style
	for(int i = 0 ; i < m_nSpherePrix + 1; i++)	// initialiser les style
	///
	{
		m_paSphereDataStyles[i] = NULL;
	}
	int currentIdx = 0;

	std::list<COLORREF>::iterator it;
	for(it = listeCouleurRGB.begin(); it != listeCouleurRGB.end(); ++ it)
	{
		XrtDataStyle* dataStyle = new XrtDataStyle;

		dataStyle->width = 1;  // line width
		dataStyle->lpat = XRT_LPAT_SOLID;  // line pattern
		dataStyle->color = (*it);  
		dataStyle->pcolor = (*it);// point color
		dataStyle->point = XRT_POINT_DOT;  // point style
		dataStyle->psize = CTA_GPH_DEFAULT_POINT_SIZE;  // point size

		m_paSphereDataStyles[currentIdx++] = dataStyle;
	
	}

	SetDataStyles(m_paSphereDataStyles);  // set the data styles sphere

	if (eGraphType != CCTAChartData::eRecettes)
	{
		CRelTrancheView* parentView = (CRelTrancheView*)GetParentView();
		SetNthSetLabel2(CRelTrancheView::eCapacity,	(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(parentView->GetAltFlag() ? IDS_CCM1 : IDS_CAPACITY));
		
		XrtDataStyle *paCmptDataStyles2[] = {
			m_aDataStyles.GetAt(eVoidStyleIdx),  // empty space
				m_aDataStyles.GetAt(eCapStyleIdx),  // capacity
				NULL
		};
		SetDataStyles2(paCmptDataStyles2);
	}
	else
	{
		XrtDataStyle *paCmptDataStyles2[] = {NULL};
		SetDataStyles2(paCmptDataStyles2);
	}

	// set default axis minimum
	SetYAxisMin(0);
	SetY2AxisMin(0);

}

//void CRelTrancheChart::SetGraphProperties(enum CCTAChartData::GraphType eGraphType)
void CRelTrancheChart::SetGraphPropertiesAlleo(enum CCTAChartData::GraphType eGraphType)
{
	CCTAChart::SetGraphFont();

	SetType(XRT_TYPE_STACKING_BAR);
	SetType2(XRT_TYPE_STACKING_BAR);

	// for the stacked bar, we must accumulate the dataset values for the set 
	//   to be displayed on the flash label (see CCTAChart)
	m_bAccumulateSets_DataSet2 = TRUE;

	( m_bRotateLabels ) ? SetXAnnotationRotation(XRT_ROTATE_90) : SetXAnnotationRotation(XRT_ROTATE_NONE);

	SetXAnnotationMethod(XRT_ANNO_VALUE_LABELS);

	SetGraphShowOutlines(((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetGraphOutlineFlag());

	// define the set labels (legend) for each bar component (bkt)
	SetLegendShow(FALSE);  // but turn it off

	// we invoke these methods here because we no longer need to dynamically
	//    set these properties based upon the type of graph selected 
	//    (i.e., no longer a need to reverse the data)
	XrtDataStyle *paCmptDataStyles[] = {
	m_aDataStyles.GetAt(eBucket0StyleIdx),
	m_aDataStyles.GetAt(eBucket1StyleIdx),
	m_aDataStyles.GetAt(eBucket2StyleIdx),
	m_aDataStyles.GetAt(eBucket3StyleIdx),
	m_aDataStyles.GetAt(eBucket4StyleIdx),
	m_aDataStyles.GetAt(eBucket5StyleIdx),
	m_aDataStyles.GetAt(eBucket6StyleIdx),
	m_aDataStyles.GetAt(eBucket7StyleIdx),
	m_aDataStyles.GetAt(eBucket8StyleIdx),
	m_aDataStyles.GetAt(eBucket9StyleIdx),
	NULL
	};

	SetDataStyles(paCmptDataStyles);  // set the data styles sphere


	if (eGraphType != CCTAChartData::eRecettes)
	{
		CRelTrancheView* papa = (CRelTrancheView*)GetParentView();
		SetNthSetLabel2(CRelTrancheView::eCapacity,	(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(papa->GetAltFlag() ? IDS_CCM1 : IDS_CAPACITY));
		
		XrtDataStyle *paCmptDataStyles2[] = {
			m_aDataStyles.GetAt(eVoidStyleIdx),  // empty space
				m_aDataStyles.GetAt(eCapStyleIdx),  // capacity
				NULL
		};
		SetDataStyles2(paCmptDataStyles2);
	}
	else
	{
		XrtDataStyle *paCmptDataStyles2[] = {NULL};
		SetDataStyles2(paCmptDataStyles2);
	}

	// set default axis minimum
	SetYAxisMin(0);
	SetY2AxisMin(0);

}
//DM7978 KGH FIN
///

CCTAChartHelpDlg* CRelTrancheChart::CreateHelpDlg()
{
	m_pHelpDlg = new CRelTrancheHelp( this );
	m_pHelpDlg->Create( this );
	((CMainFrame*) AfxGetApp()->GetMainWnd())->SetChartHelpDlg(m_pHelpDlg);
	//  ((CMainFrame*) AfxGetApp()->GetMainWnd())->SetHelpDlg(m_pHelpDlg);
	return (m_pHelpDlg);
}

void CRelTrancheChart::ContextualDisplay(CPoint point, BOOL bInRange)
{
	return;
}

////////////////////////////
// Get the value to display in the flash label
//
double CRelTrancheChart::GetFlashLabelValue(XrtDataHandle& hXrtData, int iCurFlashSet, int iCurFlashPt, BOOL bAccumulate)
{
	// THIS IS TO GET AROUND AN OLECTRA CHART BUG
	// WE HAD TO ADD ONE EXTRA SET, AND NOW MUST FILTER IT OUT !!!
	if ( iCurFlashSet > XrtDataGetLastSet( hXrtData ) )
		return INVALID_GRAPH_VALUE;
	if ( iCurFlashSet >= (((CRelTrancheView*)GetParentView())->GetMaxBucketCount()) )
		return INVALID_GRAPH_VALUE;

	double dFlashValue = 0;
	double dThisValue = 0;
	if ( (GetCurChartData()->GetGraphType() != CCTAChartData::eRecettes) )
	{
		// accumulate all sets for this point...
		for( int iThisSet = 0; iThisSet <= iCurFlashSet; iThisSet++ )
		{
			dThisValue = XrtDataGetYElement( hXrtData, iThisSet, iCurFlashPt );
			if ( dThisValue > 0 )
				dFlashValue = __max( dFlashValue, 0 );
			dFlashValue += dThisValue;
		}
	}
	else
	{
		dFlashValue = XrtDataGetYElement( hXrtData, iCurFlashSet, iCurFlashPt );
	}
	return dFlashValue;
}

/////////////////////////////////////////////////////////////////////////////
// CRelTrancheHelp dialog

CRelTrancheHelp::CRelTrancheHelp(CRelTrancheChart* pParent /*=NULL*/)
: CCTAChartHelpDlg(pParent)
{
}

void CRelTrancheHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRelTrancheHelp)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRelTrancheHelp, CCTAChartHelpDlg)
	//{{AFX_MSG_MAP(CRelTrancheHelp)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////
int CRelTrancheView::CalcRevenu( YmIcRelatedDom* pRelatedP, CString szCmpt )
{
	RelTran_LegLevel* pLegLevel = NULL;
	YmIcRelatedDom* pRelTrancheResa = NULL;
	int total_revenu = 0;
	BOOL bPrixGaranti = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag();
	for( int i=0; i < m_paRelTrancheList.GetSize(); i++ )
	{
		pLegLevel = m_paRelTrancheList.GetAt( i );
		if (pLegLevel->lTrancheNo == pRelatedP->TrancheNo())
		{
			for ( int iBktIdx = 0; iBktIdx < pLegLevel->aRelTrancheRecs.GetSize(); iBktIdx++ )
			{
				pRelTrancheResa = pLegLevel->aRelTrancheRecs.GetAt( iBktIdx );
				if (szCmpt == pRelTrancheResa->Cmpt())
				{
					total_revenu += pRelTrancheResa->Revenu();
					if (bPrixGaranti)
						total_revenu += pRelTrancheResa->RevenuGaranti();
				}
			}
		}
	}
	return total_revenu;
}

int CRelTrancheView::CalcResa( YmIcRelatedDom* pRelatedP, CString szCmpt )
{
	RelTran_LegLevel* pLegLevel = NULL;
	YmIcRelatedDom* pRelTrancheResa = NULL;
	int total_resa = 0;
	for( int i=0; i < m_paRelTrancheList.GetSize(); i++ )
	{
		pLegLevel = m_paRelTrancheList.GetAt( i );
		if (pLegLevel->lTrancheNo == pRelatedP->TrancheNo())
		{
			for ( int iBktIdx = 0; iBktIdx < pLegLevel->aRelTrancheRecs.GetSize(); iBktIdx++ )
			{
				pRelTrancheResa = pLegLevel->aRelTrancheRecs.GetAt( iBktIdx );
				if (szCmpt == pRelTrancheResa->Cmpt())
					total_resa += pRelTrancheResa->BktResHoldIdv()+pRelTrancheResa->BktResHoldGrp();
			}
		}
	}
	return total_resa;
}

// CRelTrancheHelp message handlers

// Build the 1st title
//
void CRelTrancheHelp::GetTitle1( CString& szTitle )
{
	// get the train number and the compartment for the selected data point
	// because the train number is between the compartments (on the x axis labelling),
	//   we must determine the index of the label for this point.
	CTrancheInfoLabelArray* paTrancheInfoLabels = 
		((CRelTrancheView*)m_pParent->GetParentView())->GetTrancheInfoLabelArray();
	int iIdx = ((CRelTrancheView*)m_pParent->GetParentView())->GetTrancheInfoLabelIdx( m_iCurSelectPt );
	if ( iIdx < 0 )
		return;
	TrancheInfoLabels* pTrancheInfoLabel = paTrancheInfoLabels->GetAt( iIdx );
	char szLabelText[50];
	((CRelTrancheView*)m_pParent->GetParentView())->
		FormatVarTranInfoText( szLabelText, pTrancheInfoLabel->pRelTranche );
	szTitle = szLabelText;
}

// Build the 2st title
//
void CRelTrancheHelp::GetTitle2( CString& szTitle )
{
	int iRevenu = 0;
	CTrancheInfoLabelArray* paTrancheInfoLabels = 
		((CRelTrancheView*)m_pParent->GetParentView())->GetTrancheInfoLabelArray();
	int iIdx = ((CRelTrancheView*)m_pParent->GetParentView())->GetTrancheInfoLabelIdx( m_iCurSelectPt );
	if ( iIdx < 0 )
		return;
	TrancheInfoLabels* pTrancheInfoLabel = paTrancheInfoLabels->GetAt( iIdx );

	YmIcRelatedDom* pRelTran = pTrancheInfoLabel->pRelTranche;

	// get the Dpt Date of the selected data point for the 2nd title
	CString szTitleMem = "";

	CCTAChartData* pCTAChartData = 
		((CRelTrancheView*)m_pParent->GetParentView())->GetChart()->GetCurChartData();
	if ( pCTAChartData == NULL )
		return;

	// get the text of the tab to be used in the 2nd title
	XrtValueLabel **aXlabels;
	XrtGetValues(m_pParent->m_hChart, XRT_XLABELS, &aXlabels, NULL);

	CString szCmpt = (*(aXlabels[m_iCurSelectPt])).string;  // compartment

	szTitleMem.Format( "%s: %s %s",
		((CRelTrancheView*)m_pParent->GetParentView())->GetTabText( pCTAChartData->GetGraphType() ),
		((CCTAApp*)APP)->GetResource()->LoadResString(((CRelTrancheView*)m_pParent->GetParentView())->GetAltFlag() ? IDS_SOUS_CONT : IDS_CMPT),
		szCmpt );

	if (pCTAChartData->GetGraphType() == CCTAChartData::eRecettes)
	{
		iRevenu = ((CRelTrancheView*)m_pParent->GetParentView())->CalcRevenu( pRelTran, szCmpt );
		szTitle.Format( "%s: %s %d %s",szTitleMem,"Recette :",iRevenu,"€");
	}
	else if (pCTAChartData->GetGraphType() == CCTAChartData::eResHold)
	{
		iRevenu = ((CRelTrancheView*)m_pParent->GetParentView())->CalcResa( pRelTran, szCmpt );
		szTitle.Format( "%s: %d",szTitleMem,iRevenu);
	}
	else  szTitle.Format( "%s",szTitleMem);
}

/* DM 5350 Permet de retourner True si le type de graphe courant est eRecette*/
BOOL CRelTrancheHelp::IsRecetteDataSet ()
{
	CCTAChartData* pCTAChartData = 
		((CRelTrancheView*)m_pParent->GetParentView())->GetChart()->GetCurChartData();

	if (pCTAChartData->GetGraphType() == CCTAChartData::eRecettes)
		return true;
	else return false;
}

// Get the longest label to be printed.  This is used for sizing the width
//
CSize CRelTrancheHelp::GetExtentLongestLabel( CPaintDC& dc )
{
	// we only have bucket and capacity labels, so compare the two...
	CSize sizeBktLabel(0, 0);
	sizeBktLabel = dc.GetOutputTextExtent( "W W" );  // biggest possible

	CSize sizeCapLabel(0, 0);
	CString szCapLabel = 
		XrtGetNthSetLabel2( m_pParent->m_hChart, CRelTrancheView::eCapacity );
	sizeCapLabel = dc.GetOutputTextExtent( szCapLabel );

	if ( sizeCapLabel.cx > sizeBktLabel.cx ) 
		return sizeCapLabel;
	else
		return sizeBktLabel;
}

// Get the text label to be drawn
//// DM7978 KGH		
// cette methode remplace GetTextLabel
BOOL CRelTrancheHelp::GetTextLabelSphere( int iDataSet, CString& szTextLabel, int iCurSelectSet )
{
	szTextLabel.Empty();
	//recuperer le compartment de l'histogramme selectione
	//cela servira a le comparer avec celui du label
	// get the text of the tab to be used in the 2nd title
	XrtValueLabel **aXlabels;
	XrtGetValues(m_pParent->m_hChart, XRT_XLABELS, &aXlabels, NULL);
	CString szCmptCurent = (*(aXlabels[m_iCurSelectPt])).string;  // compartment
	szCmptCurent.MakeUpper();

	if (iDataSet == XRT_DATA)
	{
		CmptSphereLabelArray aLabels = ((CCTAApp*)APP)->GetDocument()->m_sphereManager->getLabelSphereArray((GetParentView()->GetAltFlag() != 0),REL_TRANCHE);

		if ((unsigned)m_iCurSelectPt >= aLabels.size())
			return FALSE;
		SphereLibelleMap pLabel = aLabels.at(m_iCurSelectPt);

		if ((unsigned)iCurSelectSet >= pLabel.size())
			return FALSE;

		// if the bucket label is all numeric, we display the compartment with the bucket label,
		// else we display only the bucket label
		CmptLibelle cmptLibelle = pLabel[iCurSelectSet];
		CString szCmptDuLabel = cmptLibelle.sCmpt;
		if(szCmptDuLabel.Find(szCmptCurent) == -1)//les cmpt sont differents du coup on ne dessine pas ce label
			return FALSE;
		szTextLabel = cmptLibelle.sLibelle;
		BOOL bIsNumeric = TRUE;
		for(int i=0; i<szTextLabel.GetLength(); i++)
		{
			if(!isdigit(szTextLabel[i]))
			{ bIsNumeric = FALSE; break; }  // not all numeric, set boolean and get out of loop
		}
	}
	else  //  ( iDataSet == XRT_DATA2 )
	{
		szTextLabel = XrtGetNthSetLabel2( m_pParent->m_hChart, iCurSelectSet );
	}
	return TRUE;
}
// DM7978 KGH FIN
// cette methode est remplacee par GetTextLabelSphere
BOOL CRelTrancheHelp::GetTextLabel( int iDataSet, CString& szTextLabel, int iCurSelectSet )
{
	szTextLabel.Empty();
	if ( iDataSet == XRT_DATA )
	{
		CCmptBktLabelArray* aLabels = ((CRelTrancheView*)m_pParent->GetParentView())->GetCmptBktLabelArray();
		if ( m_iCurSelectPt >= aLabels->GetSize() )
			return FALSE;
		
		CmptBktLabels* pLabel = aLabels->GetAt( m_iCurSelectPt );
		if ( iCurSelectSet >= pLabel->aszBktArray.GetSize() )
			return FALSE;

		// if the bucket label is all numeric, we display the compartment with the bucket label,
		// else we display only the bucket label
		szTextLabel = pLabel->aszBktArray.GetAt( iCurSelectSet );
		BOOL bIsNumeric = TRUE;
		for(int i=0; i<szTextLabel.GetLength(); i++)
		{
			if(!isdigit(szTextLabel[i]))
			{ 
				bIsNumeric = FALSE;
				break;
			}  // not all numeric, set boolean and get out of loop
		}
		
		if(bIsNumeric)  // all numeric...add the cmpt
			szTextLabel.Format( "%s %s", pLabel->szCmpt, pLabel->aszBktArray.GetAt( iCurSelectSet ) );
	}
	else  //  ( iDataSet == XRT_DATA2 )
	{
		szTextLabel = XrtGetNthSetLabel2( m_pParent->m_hChart, iCurSelectSet );
	}
	return TRUE;
}

// Get the value to be drawn
//
CString CRelTrancheHelp::GetValue( int iDataSet, int iCurSelectSet, int iCurSelectPt  )
{
	CString szValue;
	double dValue = 0;

	CCTAChartData* pCTAChartData = 
		((CRelTrancheView*)m_pParent->GetParentView())->GetChart()->GetCurChartData();
	if ( pCTAChartData == NULL )
		return szValue;

	if ( iDataSet == XRT_DATA )
	{
		// THIS IS TO GET AROUND AN OLECTRA CHART BUG
		// WE HAD TO ADD ONE EXTRA SET, AND NOW MUST FILTER IT OUT !!!
		if ( iCurSelectSet >= (((CRelTrancheView*)m_pParent->GetParentView())->GetMaxBucketCount())+1 )
			return szValue;

		if ( pCTAChartData->GetGraphType() == CCTAChartData::eAvail )  // the availability values are nested...
		{
			if (((CRelTrancheView*)m_pParent->GetParentView())->GetAltFlag())
			{
				dValue = XrtDataGetYElement( m_hXrtData, iCurSelectSet, m_iCurSelectPt );
			}
			else
			{
				double dThisValue = 0;
				// accumulate all sets for this point...
				for( int iThisSet = 0; iThisSet <= iCurSelectSet; iThisSet++ )
				{
					dThisValue = XrtDataGetYElement( m_hXrtData, iThisSet, iCurSelectPt );
					// we want to avoid accumulating negative values, because this makes an incorrect bar component value.
					// to avoid accumulating with negative values, if we have, up to this point accumulated negative values,
					//    let's reset the accumulated value (dValue) to zero
					if ( dThisValue > 0 )
						dValue = __max( dValue, 0 );
					dValue += dThisValue;
				}
			}
		}
		else
		{
			dValue = XrtDataGetYElement( m_hXrtData, iCurSelectSet, m_iCurSelectPt );
		}
	}
	else  // iDataSet == XRT_DATA2
	{
		// because it is a stacked bar, we must accumulate the pieces of the bar...
		for (int iSet = 0; iSet <= iCurSelectSet; iSet++)
			dValue += XrtDataGetYElement( m_hXrtData2, iSet, m_iCurSelectPt );
	}
	szValue.Format( "%3.0f", dValue );
	return szValue;
}

// Translate the dataset to a cta graph style (color)
//
int CRelTrancheHelp::GetCTADataStyleIndex( int iDataSet, int iCurSelectSet )
{
	if ( iDataSet == XRT_DATA )
	{
		// DM 7879 KGH	//retourner l'index dans le tableau m_paSphereDataStyles
		/*return CCTAChart::eBucket0StyleIdx + iCurSelectSet;*/

		// NPI - Ano 70148
		//return iCurSelectSet;

		if (((CCTAApp*)APP)->GetRhealysFlag())
		{
			return CCTAChart::eBucket0StyleIdx + iCurSelectSet;
		}
		else
		{
			return iCurSelectSet;
		}
		// DM 7879 KGH FIN
	}
	else  // iDataSet == XRT_DATA2
	{
		if ( iCurSelectSet == CRelTrancheView::eCapacity )
			return CCTAChart::eCapStyleIdx;
	}
	return -1;
}

// DM7978 KGH reimplementation de la methode  de la classe CCTAChartHelpDlg
BOOL CRelTrancheHelp::DrawThisLabel( int iDataSet, CPaintDC& dc, int iCurSelectSet, CPoint& pointDraw, int iLeftMargin, int iRightMargin, BOOL bWithCheckBox)
{
	//  get the index which points to the style for which we draw the color-coded rectangle...
	//  note that if this method returns -1, we get out immediately because we don't want to deal with it
	int iCtaDataStyleIdx = GetCTADataStyleIndex( iDataSet, iCurSelectSet );  // virtual method
	if ( iCtaDataStyleIdx < 0 )
		return FALSE;  // we report that we didn't draw anything

	// get the text label to be displayed
	CString szTextLabel;
	//DM7978 KGH remplacer GetTextLabel par GetTextLabelSphere
	//if ( ! GetTextLabel( iDataSet, szTextLabel, iCurSelectSet ) )  // virtual method

	// NPI - Ano 70148
	/*if ( ! GetTextLabelSphere( iDataSet, szTextLabel, iCurSelectSet ) )
		// DM7978 KGH FIN
		return FALSE;  */

	if (((CCTAApp*)APP)->GetRhealysFlag())
	{
		if (!GetTextLabel(iDataSet, szTextLabel, iCurSelectSet))
			return FALSE;
	}
	else
	{
		if (!GetTextLabelSphere(iDataSet, szTextLabel, iCurSelectSet))
			return FALSE;
	}
	///

	// get the numeric value (as a CString) to be displayed
	CString szValue = GetValue( iDataSet, iCurSelectSet, m_iCurSelectPt  );  // virtual method
	if ( szValue.IsEmpty() )  // if GetValue returns an empty string, we don't draw anything...
		return FALSE;
	int checkidc = GetCheckBox ( iDataSet, iCurSelectSet, m_iCurSelectPt );

	m_sizeTextLabel = dc.GetOutputTextExtent( szTextLabel );

	// create rect objects to draw the label and the value
	CRect rectTextLabel(pointDraw, m_sizeTextLabel);    // create a rect object for the text label
	CRect rectTextValue(pointDraw, m_sizeTextValue);    // create a rect object for the value

	// set a rectangle for the text label to be drawn
	rectTextLabel.SetRect( pointDraw.x, pointDraw.y, pointDraw.x + m_sizeTextLabel.cx, pointDraw.y + m_sizeTextLabel.cy );
	
	// set the default system background color for the drawing
	dc.SetBkColor( ::GetSysColor(COLOR_WINDOW) );
	CBrush brushBkgd( ::GetSysColor(COLOR_WINDOW) );

	// now draw the text label
	dc.FillRect( &rectTextLabel, &brushBkgd );
	dc.DrawText( szTextLabel, rectTextLabel, DT_RIGHT | DT_SINGLELINE );

	// set a rectangle for the value to be drawn
	rectTextValue.SetRect( iRightMargin - m_sizeTextValue.cx, pointDraw.y, iRightMargin, pointDraw.y + m_sizeTextValue.cy);
	// now draw the value
	dc.FillRect( &rectTextValue, &brushBkgd );
	dc.DrawText( szValue, rectTextValue, DT_RIGHT | DT_SINGLELINE );

	// now draw the color-coded rectangle...
	/*
	int iCtaDataStyleIdx = GetCTADataStyleIndex( iDataSet, iCurSelectSet );  // virtual method
	if ( iCtaDataStyleIdx < 0 )
	return;
	*/
	CBitmap bmpBrush;
	CBrush brush;
	COLORREF clrHoldTextColor = dc.GetTextColor( );  // hold the text color
	//DM7978 KGH 
	XrtDataStyle* style = NULL;
	if (iDataSet == XRT_DATA)	//recuperer le style des spheres dans le tableau m_paSphereDataStyles
	{
		// NPI - Ano 70148
		//style = GetParentChart()->GetSphereDataStylesAt(iCtaDataStyleIdx);
		
		if (((CCTAApp*)APP)->GetRhealysFlag())
		{
			style = m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx);
		}
		else
		{
			style = GetParentChart()->GetSphereDataStylesAt(iCtaDataStyleIdx);
		}
	}
	else	// si c pour la capacite recuperer le style de la capacite
	{
		style = m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx);
	}

	//switch ( m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->fpat )
	switch (style->fpat )
		//DM7978 KGH FIN
	{
	case XRT_FPAT_25_PERCENT :
		//DM7978 KGH
		//dc.SetTextColor( m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->color );
		dc.SetTextColor( style->color );
		//DM7978 KGH FIN
		bmpBrush.LoadBitmap( IDB_PATTERN25 );
		brush.CreatePatternBrush( &bmpBrush );
		break;
	case XRT_FPAT_VERT_STRIPE :
		//DM7978 KGH
		//brush.CreateHatchBrush( HS_CROSS, m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->color );
		brush.CreateHatchBrush( HS_CROSS, style->color );
		//DM7978 KGH FIN
		break;
	case  XRT_FPAT_HORIZ_STRIPE :
		//DM7978 KGH
		//brush.CreateHatchBrush( HS_HORIZONTAL, m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->color );
		brush.CreateHatchBrush( HS_HORIZONTAL, style->color );
		//DM7978 KGH FIN
		break;
	case XRT_FPAT_DIAG_HATCHED :
		//DM7978 KGH
		//brush.CreateHatchBrush( HS_FDIAGONAL, m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->color );
		brush.CreateHatchBrush( HS_FDIAGONAL, style->color );
		//DM7978 KGH FIN
		break;
	case XRT_FPAT_CROSS_HATCHED :
		//DM7978 KGH
		//brush.CreateHatchBrush( HS_DIAGCROSS, m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->color );
		brush.CreateHatchBrush( HS_DIAGCROSS, style->color );
		//DM7978 KGH FIN
		break;
	case XRT_FPAT_SOLID :
	default :
		//DM7978 KGH
		//brush.CreateSolidBrush( m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->color );
		brush.CreateSolidBrush( style->color );
		//DM7978 KGH FIN
		break;
	}

	// position the color-coded rectangle (legend box) just to the left of the text
	CRect rectBox( iLeftMargin + (bWithCheckBox ? CHECKBOXWIDTH : 0), rectTextLabel.top, (rectTextLabel.left - SPACING), rectTextLabel.bottom );
	dc.SelectObject(&brush);
	dc.Rectangle(&rectBox);

	// restore the text color
	dc.SetTextColor( clrHoldTextColor );

	// eventuelle creation checkBox
	if (checkidc)
	{ // checkidc negatif signifie que la case n'est pas checké.
		int idcbox = (checkidc < 0) ? -checkidc : checkidc;
		CDynaButton *pButton = (CDynaButton*)GetDlgItem (idcbox);
		if (!pButton)
		{ // La checkbox n'existe pas il faut la creer
			CRect rectButton (iLeftMargin, rectTextLabel.top, iLeftMargin+CHECKBOXWIDTH, rectTextLabel.bottom);
			pButton = new CDynaButton();
			BOOL bb = pButton->Create ("", BS_CHECKBOX | WS_CHILD | WS_VISIBLE, rectButton, this, idcbox);
			// correction de l'ano 159666, utilisation de BS_CHECKBOX au lieu de BS_AUTOCHECKBOX
			// EN mode autocheck on passait parfois sur un double clic qui faisait que le check n'était plus en 
			// phase avec l'affichage de la courbe.
			if (bb)
				m_ListCheckIdc.Add(idcbox);
		}
		pButton->SetCheck (checkidc > 0);
	}

	return TRUE;
}
//DM7978 KGH FIN

void CRelTrancheHelp::OnPaint()
{
	// paint all text and graphics on the client area of the dialog
	//
	CPaintDC dc(this); // device context for painting
	CFont* pOldFont = dc.SelectObject( GetHelpTextFont() );

	// set the initial size of the help dialog window

	// DM7978 KGH adapter la fenetre au nb max de spheres au lieu du nb max bucket
	/* int iMaxBucketCount = ((CRelTrancheView*)m_pParent->GetParentView())->GetMaxBucketCount();
	InitHelpWin( ((CRelTrancheView*)m_pParent->GetParentView())->GetMaxBucketCount() + 	CRelTrancheView::eNumberDataSets2 - 1 ); */

	// NPI - Ano 70148
	/*CRect rectWnd;
	GetWindowRect( &rectWnd );
	rectWnd.right = 1000;

	int iMaxSpheresPrix = ((CRelTrancheView*)m_pParent->GetParentView())->GetMaxSpheresPrixLegende();
	InitHelpWin(iMaxSpheresPrix + CRelTrancheView::eNumberDataSets2 - 1 );*/

	if (((CCTAApp*)APP)->GetRhealysFlag())
	{
		int iMaxBucketCount = ((CRelTrancheView*)m_pParent->GetParentView())->GetMaxBucketCount();
		InitHelpWin( ((CRelTrancheView*)m_pParent->GetParentView())->GetMaxBucketCount() + 	CRelTrancheView::eNumberDataSets2 - 1 );
	}
	else
	{
		CRect rectWnd;
		GetWindowRect( &rectWnd );
		rectWnd.right = 1000;

		int iMaxSpheresPrix = ((CRelTrancheView*)m_pParent->GetParentView())->GetMaxSpheresPrixLegende();
		InitHelpWin(iMaxSpheresPrix + CRelTrancheView::eNumberDataSets2 - 1 );
	}
	///
	// DM7978 KGH FIN

	// draw the titles and return the point where drawing is to continue
	CPoint pointDraw = DrawTitles( dc );

	// draw the labels and the corresponding values
	DrawLabels( dc, pointDraw, TRUE );

	// Do not call CDialog::OnPaint() for painting messages
}

