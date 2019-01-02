// OthLegsView.cpp : implementation file
//
#include "StdAfx.h"

#include "CTA_Resource.h"
#include "CTAChartHelpDlg.h"
#include "LegListboxDlg.h"
#include "OthLegsView.h"
#include "BktSciSphere.h"
#include "SpheresPrix.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COthLegsAltView

IMPLEMENT_DYNCREATE(COthLegsAltView, COthLegsView)

COthLegsAltView::COthLegsAltView()
:COthLegsView()
{
}

void COthLegsAltView::OnInitialUpdate() 
{
	((CChildFrame*)GetParentFrame())->SetAltFlag(TRUE);

	COthLegsView::OnInitialUpdate();  // base class method
}

/////////////////////////////////////////////////////////////////////////////
// COthLegsView

IMPLEMENT_DYNCREATE(COthLegsView, CView)

COthLegsView::COthLegsView()
{
	// set this view as the owner view in the chart object
	m_Chart.SetParentView(this);  
	m_Chart.SetCurScaling(CCTAChart::eMaxScaleIdx);

	m_iNstLvlMaxCount = 0;
	m_bTabResized = FALSE;
	m_iCurTabSelection = -1;

	// initialize the enumeration variables for chart 1 (XRT_DATA)
	// if we are graphing additional capacity, we do not graph Auth Dpt 
	//    because reservable capacity is always equal to auth dpt (ie, no overbooking)
	//    also (if we are graphing additional capacity), Dmd Dpt Fcst becomes Add Dmd Fcst

	eAuthDptFcst = -1;
	eDmdDptFcst = -1;
	eResHoldTot = -1;
	eNumberDataSets = -1;

	// NOT an Additional Capacity client

	eAuthDptFcst = 0;
	eResHoldTot = 1;
	eNumberDataSets = 2;

	// initialize the enumeration variables for chart 2 (XRT_DATA2)
	eEmptySpace1 = 0;
	eCapacity = 1;
	eEmptySpace2 = -1;
	eCapPlusUnres = -1;
	eEmptySpace3 = -1;
	eCapPlusStdgPlusUnres = -1;
	eNumberDataSets2 = -1;
	eNumberDataSets2 = eCapacity + 1;

	m_nbSpheresPrixMax = 0;//DM7978 - KGH - Sphere de prix CC/SC
	m_nbSpheresPrixLegende = 0;//DM7978 - KGH
}

COthLegsView::~COthLegsView()
{
	// NPI - Ano 70148
	//ClearCmptBktLabelArray(); DM7978 KGH
	ClearCmptBktLabelArray();
	///
}


BEGIN_MESSAGE_MAP(COthLegsView, CView)
	//{{AFX_MSG_MAP(COthLegsView)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_OTHLEGS_TABCTRL, OnChartSelchange)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_SCALE_MAX, OnScaleMax)
	ON_UPDATE_COMMAND_UI(ID_SCALE_MAX, OnUpdateScaleMax)
	ON_COMMAND(ID_SCALE_AUTHORIZATIONS, OnScaleAuthorizations)
	ON_UPDATE_COMMAND_UI(ID_SCALE_AUTHORIZATIONS, OnUpdateScaleAuthorizations)
	ON_COMMAND(ID_GRAPH_FLASH_LABEL, OnGraphDisplayFlashLabels)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_FLASH_LABEL, OnUpdateGraphDisplayFlashLabels)
	ON_COMMAND(ID_GRAPH_ROTATE_LABELS,OnGraphRotateLabels)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_ROTATE_LABELS,OnUpdateGraphRotateLabels)
	ON_COMMAND(ID_LEG_SELECT_DLG,OnSelectLegs)
	ON_UPDATE_COMMAND_UI(ID_LEG_SELECT_DLG, OnUpdateSelectLegs)
	ON_COMMAND(ID_PRINT, OnPrint)
	ON_UPDATE_COMMAND_UI(ID_PRINT, OnUpdatePrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COthLegsView drawing

void COthLegsView::OnDraw(CDC* pDC)
{
	ASSERT_VALID(GetDocument());

	YM_RecordSet* pOthLegsSet = 
		YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY);

	CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
	if ( pCTAChartData == NULL )
		return;

	// if record set is valid and we have data 
	if ( pOthLegsSet->IsValid() && pCTAChartData != NULL )
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

void COthLegsView::OnPaint() 
{
	CPaintDC dc(this);  // device context for painting
	COLORREF clrBk;
	COLORREF clrFt;
	if ( m_ChartTabCtrl.GetCurSel() == index_recette)
	{
		clrBk = (GetAltFlag()) ? 
			((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextRecBkAlt() : 
		((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextRecBkMain();
		m_Chart.SetBackgroundColor (clrBk);
		clrFt = (GetAltFlag()) ? 
			((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkAlt() : 
		((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();
		m_Chart.SetForegroundColor (clrFt); // marron
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
	// Do not call CView::OnPaint() for painting messages
}

/////////////////////////////////////////////////////////////////////////////
// COthLegsView diagnostics

#ifdef _DEBUG
void COthLegsView::AssertValid() const
{
	CView::AssertValid();
}

void COthLegsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCTADoc* COthLegsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCTADoc)));
	return (CCTADoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COthLegsView message handlers

int COthLegsView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create a chart window when the view is created
	CRect rect;
	GetClientRect(&rect);
	m_ChartTabCtrl.Create(CTA_TAB_STYLE, rect, this, IDC_OTHLEGS_TABCTRL);
	GetClientRect(&rect);  // get client rectangle again, because it just changed...
	m_ChartTabCtrl.AdjustRect( FALSE, &rect );

	m_Chart.Create   ("", WS_CHILD | WS_VISIBLE, rect, &m_ChartTabCtrl, 0);

	return 0;
}

void COthLegsView::FrameSizeChange (UINT nType, int cx, int cy)
{
	YM_RecordSet* pOthLegsSet = YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY);

	ASSERT( pOthLegsSet != NULL );

	if ((nType == SIZE_MINIMIZED) && pOthLegsSet->IsOpen())
		pOthLegsSet->Close(); 
	else if ((nType != SIZE_MINIMIZED) && !pOthLegsSet->IsOpen())
		pOthLegsSet->Open();
}

void COthLegsView::OnShowWindow(BOOL bShow, UINT nStatus)
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

void COthLegsView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	ASSERT_VALID(GetDocument());

	// Set text in status bar control
	((CChildFrame*)GetParent())->
		GetStatusBar()->SetPaneOneAndLock(
		((CCTAApp*)APP)->GetResource()->LoadResString(IDS_READING_HISTORY) );

	CString titi, toto;
	titi = ((CCTAApp*)APP)->GetResource()->LoadResString((GetAltFlag()) ? IDS_OTHLEGS_SCI_TITLE : IDS_OTHLEGS_TITLE);
	GetParentFrame()->GetWindowText(toto);
	GetParentFrame()->SetWindowText(titi);
	((YM_PersistentChildWnd*)GetParentFrame())->SetChildKey (toto);
	((YM_PersistentChildWnd*)GetParentFrame())->SetChildView (this);

	// insert all labels into the help dialog labels array
	//
	m_Chart.GetHelpDlgLblArray()->SetAtGrow( eResHoldTot, 
		((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_TOT) );

	if (!GetAltFlag())
	{ // ces infos n'existe que pour les buckets
		m_Chart.GetHelpDlgLblArray()->SetAtGrow( eAuthDptFcst, 
			((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AUTH_DPT) );
	}

	// capacity is a bit different because it is on the second chart data
	m_Chart.GetHelpDlgLblArray()->SetAtGrow( eNumberDataSets+eCapacity, 
		((CCTAApp*)APP)->GetResource()->LoadResString(GetAltFlag() ? IDS_CCM1 : IDS_CAPACITY) );

	YM_RecordSet* pOthLegsSet = YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY);
	pOthLegsSet->AddObserver( this );
	if( ! pOthLegsSet->IsOpen() )
		pOthLegsSet->Open();	
	OnUpdate( this, 0L, pOthLegsSet );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// On change of the tab control, select the appropriate chart
//
void COthLegsView::OnChartSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// set the member variable which holds the currently selected tab...
	m_iCurTabSelection = m_ChartTabCtrl.GetCurSel();

	LPTSTR psz = _sLastTabUsed.GetBuffer( 255 );
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
	TabCtrlItem.pszText = psz;
	TabCtrlItem.cchTextMax = 255;
	GetTabCtrl()->GetItem( m_iCurTabSelection, &TabCtrlItem );
	_sLastTabUsed.ReleaseBuffer();

	// are we out of bounds ?
	if ( ( m_Chart.GetChartDataArray()->GetSize() == 0 ) || ( m_iCurTabSelection < 0 ) )
		return;

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
		m_Chart.SetForegroundColor (clrFt); // marron
		/*m_Chart.SetYTitle("€uros");
		m_Chart.SetYTitleRotation(XRT_ROTATE_90);*/
	}
	ShowGraph();  // show the newly selected chart

	// force a redraw of the help dialog if up
	CCTAChartHelpDlg* pChartHelpDlg = m_Chart.GetHelpDlg();
	if (( pChartHelpDlg ) && (IsWindow(pChartHelpDlg->m_hWnd)) )
	{
		CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
		int iNumLabels = ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt ) ?
			COthLegsView::eNumberDataSets + COthLegsView::eNumberDataSets2 - 1 :
		GetMaxBucketCount();

		m_Chart.GetHelpDlg()->ClearHelpWin();
		m_Chart.GetHelpDlg()->Invalidate(FALSE);
		m_Chart.GetHelpDlg()->UpdateWindow();
	}
	*pResult = 0;
}

void COthLegsView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// If notification comes from a RecordSet, pHint is a RecordSet*
	if( pHint != NULL )
	{
		YM_RecordSet* pOthLegsSet = YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY);

		if( pHint == pOthLegsSet )
		{
			if (!pOthLegsSet->IsValid())
			{ // nettoyage, fait sur etat invalide de la requete principale
				m_Chart.DestroyHelpDlg(); // remove the help dialog if displayed
				EnableWindow( FALSE );
				m_apLegs.RemoveAll();
				ClearGraph();
			}
			else
			{
				// Build all graphs
				EnableWindow( TRUE );
				ClearGraph();
				BuildGraph(CCTAChartData::eCmpt);
				BuildGraph(CCTAChartData::eBkt);
				if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag())
					BuildGraph(CCTAChartData::eRecettes);
				BuildTabs();  // Build the selecton tabs
				ResizeChart();
			}
		}

	}
}

void COthLegsView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	ResizeChart();
}

void COthLegsView::OnDestroy() 
{
	CView::OnDestroy();

	YM_RecordSet* pOthLegsSet = YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY);
	ASSERT( pOthLegsSet != NULL );
	pOthLegsSet->RemoveObserver(this);
	pOthLegsSet->Close();
}

void COthLegsView::OnScaleMax() 
{
	m_Chart.ScaleGraph(CCTAChart::eMaxScaleIdx);
	Invalidate(FALSE);  // force a redraw
	UpdateWindow();
}

void COthLegsView::OnUpdateScaleMax(CCmdUI* pCmdUI) 
{
	ASSERT_VALID(GetDocument());

	YM_RecordSet* pOthLegsSet = YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY);

	ASSERT( pOthLegsSet != NULL );

	BOOL bRec = FALSE;
	CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
	if (pCTAChartData && (pCTAChartData->GetGraphType() == CCTAChartData::eRecettes))
		bRec = TRUE;
	pCmdUI->Enable( pOthLegsSet->IsOpen() );
	pCmdUI->SetCheck( bRec || (m_Chart.GetCurScaling() == CCTAChart::eMaxScaleIdx ));
}

void COthLegsView::OnScaleAuthorizations() 
{
	m_Chart.ScaleGraph(CCTAChart::eAuthScaleIdx);
	Invalidate(FALSE);  // force a redraw
	UpdateWindow();
}

void COthLegsView::OnUpdateScaleAuthorizations(CCmdUI* pCmdUI) 
{
	ASSERT_VALID(GetDocument());

	YM_RecordSet* pOthLegsSet = YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY);

	ASSERT( pOthLegsSet != NULL );

	BOOL bRec = FALSE;
	CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
	if (pCTAChartData && (pCTAChartData->GetGraphType() == CCTAChartData::eRecettes))
		bRec = TRUE;

	pCmdUI->Enable( !bRec && pOthLegsSet->IsOpen() );
	pCmdUI->SetCheck( !bRec && (m_Chart.GetCurScaling() == CCTAChart::eAuthScaleIdx ));
}

void COthLegsView::OnGraphDisplayFlashLabels() 
{
	// toggle the flag to display the graph flash labels
	((CCTAApp*)APP)->SetFlashLabelFlag(  ! ((CCTAApp*)APP)->GetFlashLabelFlag() );
}

void COthLegsView::OnUpdateGraphDisplayFlashLabels(CCmdUI* pCmdUI) 
{
	YM_RecordSet* pOthLegsSet = YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY);
	pCmdUI->Enable( pOthLegsSet->IsOpen() );
	pCmdUI->SetCheck( ((CCTAApp*)APP)->GetFlashLabelFlag() );
}

void COthLegsView::OnGraphRotateLabels()
{
	m_Chart.SetRotateLabelsFlag( ! m_Chart.GetRotateLabelsFlag() );
	m_Chart.GetRotateLabelsFlag() == 1 ? 
		m_Chart.SetXAnnotationRotation(XRT_ROTATE_90) : 
	m_Chart.SetXAnnotationRotation(XRT_ROTATE_NONE);
}

// method invoked when menu item is selected
//
void COthLegsView::OnSelectLegs()
{
	// Display a dialog box with all other legs to allow the user to choose which legs to display
	CLegListboxDlg LegListDlg(this, TRUE, TRUE, FALSE, CLegListboxDlg::eSequence );

	int iDlgResult = LegListDlg.DoModal();
	if ( iDlgResult == IDOK )
	{
		int* iaSelBuf = LegListDlg.GetSelectedItemsIndexArray();  // array of indexes of selected legs
		int iSelItemCount = LegListDlg.GetSelectedItemCount();  // number of selected legs

		// reset up the array
		m_apLegs.SetSize( iSelItemCount );

		// loop through all the legs and set them in the leg array
		YmIcLegsDom* pLeg = NULL;
		for (int iIndex = 0; iIndex < iSelItemCount ; iIndex++)
		{
			pLeg = (YmIcLegsDom*)LegListDlg.GetSelectedItem( iIndex );
			m_apLegs[ iIndex ] = pLeg;
		}

		ClearGraph(); 
		BuildGraph(CCTAChartData::eCmpt);
		BuildGraph(CCTAChartData::eBkt);
		if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag())
			BuildGraph(CCTAChartData::eRecettes);
		BuildTabs();  // Build the selecton tabs
		ResizeChart();
	}
}

void COthLegsView::OnUpdateSelectLegs( CCmdUI* pCmdUI )
{
	YM_RecordSet* pOthLegsSet = YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY);
	if ( pOthLegsSet->IsOpen() )
		pCmdUI->Enable( TRUE );
	else
		pCmdUI->Enable( FALSE );
}

void COthLegsView::OnUpdateGraphRotateLabels( CCmdUI* pCmdUI )
{
	YM_RecordSet* pOthLegsSet = YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY);
	ASSERT( pOthLegsSet != NULL );
	if ( pOthLegsSet->IsOpen() )
	{
		pCmdUI->Enable( TRUE );
		pCmdUI->SetCheck ( (m_Chart.GetRotateLabelsFlag()==1) ? 1 : 0 );
		pCmdUI->SetCheck ( (m_Chart.GetRotateLabelsFlag()==1) ? 1 : 0 );
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}
}

void COthLegsView::OnPrint()
{
	m_Chart.Print();
}

void COthLegsView::OnUpdatePrint( CCmdUI* pCmdUI )
{
	YM_RecordSet* pOthLegsSet = YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY);
	ASSERT( pOthLegsSet != NULL );
	if ( pOthLegsSet->IsOpen() )
		pCmdUI->Enable( TRUE );
	else
		pCmdUI->Enable( FALSE );
}

//////////////////////////////////////
// Write specific view configuration information to the registry
//
void COthLegsView::WriteViewConfig()
{
	// save the graph flags in the registry
	//
	CString szProfileKey;
	((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

	APP->WriteProfileInt( szProfileKey, GRAPH_SCALE_FACTOR, m_Chart.GetCurScaling() );
	APP->WriteProfileInt( szProfileKey, ROTATE_X_LABEL, m_Chart.GetRotateLabelsFlag() ? 1 : 0 );
	APP->WriteProfileString( szProfileKey, "LastTabUsed", _sLastTabUsed );
}

//////////////////////////////////////
// Restore specific view configuration information from the registry
//	
void COthLegsView::RestoreViewConfig()
{
	// restore the graph flags from the registry
	//
	CString szProfileKey;
	((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

	m_Chart.SetRepaint( FALSE );
	enum CCTAChart::GraphScaleIndex eCurScale = (enum CCTAChart::GraphScaleIndex)
		APP->GetProfileInt(szProfileKey, GRAPH_SCALE_FACTOR, (unsigned int)CCTAChart::eMaxScaleIdx);
	m_Chart.SetRotateLabelsFlag( !!APP->GetProfileInt(szProfileKey, ROTATE_X_LABEL, 0) );
	m_Chart.SetCurScaling(eCurScale);
	m_Chart.ScaleGraph(m_Chart.GetCurScaling());  // don't forget to rescale the graph
	m_Chart.SetRepaint( TRUE );
	_sLastTabUsed = APP->GetProfileString(szProfileKey, "LastTabUsed", "" );
}

///////////////////////////////////////////////////////
// Resize the chart window 
//
void COthLegsView::ResizeChart()
{
	// force a re-size of the chart to fit the window
	CRect rect;
	GetClientRect(&rect);
	m_ChartTabCtrl.MoveWindow(&rect);
	m_ChartTabCtrl.AdjustRect(FALSE, &rect);

	if ( (rect.Width() > 0) && (rect.Height() > 10) )
		m_Chart.MoveWindow(&rect);

	COLORREF clrBk;
	COLORREF clrFt;
	if ( m_ChartTabCtrl.GetCurSel() == index_recette)
	{
		clrBk = (GetAltFlag()) ? 
			((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextRecBkAlt() : 
		((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextRecBkMain();
		m_Chart.SetBackgroundColor (clrBk);
		clrFt = (GetAltFlag()) ? 
			((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkAlt() : 
		((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();
		m_Chart.SetForegroundColor (clrFt); // marron
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
CString COthLegsView::GetTabText(enum CCTAChartData::GraphType eGraphType)
{
	switch( eGraphType )
	{
	case CCTAChartData::eCmpt:
		return ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CMPT);
		break;
	case CCTAChartData::eBkt:
		// DM7978 KGH modif du libelle de l'onglet pour CC et SC
		//return ((CCTAApp*)APP)->GetResource()->LoadResString(GetAltFlag() ? IDS_SCI : IDS_BUCKET);
		// NPI Ano 70148
		//return ((CCTAApp*)APP)->GetResource()->LoadResString(GetAltFlag() ? IDS_SCI_SPHERES : IDS_BUCKET_SPHERES);
		
		if (((CCTAApp*)APP)->GetRhealysFlag())
		{
			return ((CCTAApp*)APP)->GetResource()->LoadResString(GetAltFlag() ? IDS_SCI : IDS_BUCKET);
		}
		else
		{
			return ((CCTAApp*)APP)->GetResource()->LoadResString(GetAltFlag() ? IDS_SCI_SPHERES : IDS_BUCKET_SPHERES);
		}
		///
		// DM7978 KGH FIN
		break;
	case CCTAChartData::eRecettes:
		// DM7978 KGH modif du libelle de l'onglet pour CC et SC
		//return ((CCTAApp*)APP)->GetResource()->LoadResString(GetAltFlag() ? IDS_SCI : IDS_BUCKET);
		return ((CCTAApp*)APP)->GetResource()->LoadResString(GetAltFlag() ? IDS_SCI_SPHERES : IDS_BUCKET_SPHERES);
		// DM7978 KGH FIN
		break;
	default:
		return "";
		break;
	}
}

// Is the leg which has been passed in the array of user-selected legs ?
BOOL COthLegsView::IsLegSelected( YmIcOthLegsDom* pOthLegs )
{
	int iArrayCount = m_apLegs.GetSize();
	if ( iArrayCount == 0 )  // by default, if the array has not yet been created, return TRUE
		return TRUE;

	for (int i=0; i < iArrayCount ; i++)
		if ( ! strcmp( m_apLegs[ i ]->LegOrig(), pOthLegs->LegOrig() ) )
			return TRUE;  // found it !
	return FALSE;  // did not find it !
}

///////////////////////////////////////////////////////
// Clear the data object
//
void COthLegsView::ClearGraph()
{
	// clear the client area(s)
	m_Chart.ShowWindow( SW_HIDE );
	Invalidate(FALSE);
	UpdateWindow();

	ClearTabs();  // delete the entries from the tab array
	m_Chart.ClearChartDataArray();  // delete graph dataset for this graph type
	m_Chart.GetHelpDlgCmptArray()->RemoveAll( );
	ClearCmptBktLabelArray();

	// Set text in status bar control
	((CChildFrame*)GetParent())->
		GetStatusBar()->SetPaneOneAndLock(
		((CCTAApp*)APP)->GetResource()->LoadResString(IDS_READING_HISTORY) );
}

///////////////////////////////////////////////////////
// Calculate the various capacity values
//
void COthLegsView::GetCapValues(
								YmIcOthLegsDom* pOthLegs, 
								long& lReservableCap, 
								long& lStandingCap, 
								long& lUnreservableCap)
{
	lReservableCap = pOthLegs->Capacity();
	lStandingCap = 0;
	lUnreservableCap = 0;
}


// Calculate availability based upon either Net Nesting or Threshold Nesting Method
//
int COthLegsView::CalcAvail(YmIcOthLegsDom* pOthLegsP)
{
	YM_Iterator<YmIcOthLegsDom>* pIterator = 
		YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY)->CreateIterator();

	BOOL bFoundLegCmpt = FALSE;
	BOOL bUseNestLevel = FALSE;
	int iAuth = 0;
	int iAvail = 0;
	int iResHoldAccum = 0;
	YmIcOthLegsDom* pOthLegs;
	for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
	{
		pOthLegs = pIterator->Current();

		// for Net Nesting Inventory Method, we accumulate ResHold using current and all lesser valued buckets in this cmpt
		// for Threshold Nesting Inventory Method, we accumulate ResHold using all buckets in this cmpt
		if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetInventoryNestingMethod())
			bUseNestLevel = (pOthLegs->NestLevel() >= pOthLegsP->NestLevel());
		else
			bUseNestLevel = TRUE;

		if (
			(!strcmp(pOthLegs->LegOrig(), pOthLegsP->LegOrig()) ) &&
			(!strcmp(pOthLegs->LegDest(), pOthLegsP->LegDest()) ) &&
			(!strcmp(pOthLegs->Cmpt(), pOthLegsP->Cmpt()) ) &&
			(bUseNestLevel)
			)
		{
			iResHoldAccum += 
				(pOthLegs->AuthResHoldIdv()+pOthLegs->AuthResHoldGrp());

			if (pOthLegs->NestLevel() == pOthLegsP->NestLevel())
				iAuth = pOthLegs->AuthFcBkt();

			bFoundLegCmpt = TRUE;  // found this Leg/Cmpt
		}
		else
		{
			if ( bFoundLegCmpt )
				break;  // small optimization...time to get out of the loop because we've already seen this Leg/Cmpt
		}
	}
	delete pIterator;
	iAvail = __max ( iAuth - iResHoldAccum, 0 );
	return iAvail;
}

// Recalculate availability based upon either Net Nesting or Threshold Nesting Method, 
//    and UPDATE THE RECORD SET !!!
//
void COthLegsView::ReCalcAvail()
{
	// loop through all records and recalculate the availability
	YM_Iterator<YmIcOthLegsDom>* pIterator = 
		YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY)->CreateIterator();

	CHelpDlgRecArray apTmp;  // build this array in reverse order (i.e., ascending bkt order)
	apTmp.SetSize(pIterator->GetCount());

	int iIdx = apTmp.GetSize()-1;
	int iAvail = 0;
	YmIcOthLegsDom* pOthLegs;
	for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
	{
		pOthLegs = pIterator->Current();
		apTmp.SetAt(iIdx--, pOthLegs);
		iAvail = CalcAvail( pOthLegs );
		pOthLegs->Availability(iAvail);
	}
	delete pIterator;

	pOthLegs = apTmp.GetAt(0);
	YmIcOthLegsDom* pOthLegsPrev = pOthLegs;
	char* pszLegOrig = (char*)pOthLegs->LegOrig();
	char* pszLegDest = (char*)pOthLegs->LegDest();
	char* pszCmpt = (char*)pOthLegs->Cmpt();
	for( iIdx = 1; iIdx < apTmp.GetSize(); iIdx++ )
	{
		pOthLegs = apTmp.GetAt(iIdx);
		// do we have the same orig/dest/cmpt ?
		if (
			(!strcmp(pOthLegs->LegOrig(), pOthLegsPrev->LegOrig()) ) &&
			(!strcmp(pOthLegs->LegDest(), pOthLegsPrev->LegDest()) ) &&
			(!strcmp(pOthLegs->Cmpt(), pOthLegsPrev->Cmpt()) )
			)
		{
			// ensure the availibility for this bkt is not less than a lower-valued bkt
			//   (e.g., avail for bkt 0 is not less than avail for bkt 1)
			if ( pOthLegsPrev->Availability() < pOthLegs->Availability() )
				pOthLegs->Availability( pOthLegsPrev->Availability() );  // reset it to the higher valued bkt avail level
		}
		pOthLegsPrev = pOthLegs;
	}
}

void COthLegsView::ReCalcAvailSci()
{
	// loop through all records and recalculate the availability
	YM_Iterator<YmIcOthLegsDom>* pIterator = 
		YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY)->CreateIterator();

	int iAvail = 0;
	YmIcOthLegsDom* pOthLegs;
	for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
	{
		pOthLegs = pIterator->Current();
		int AvailSCG = pOthLegs->Capacity() - pOthLegs->CmptResHoldGrp() - pOthLegs->CmptResHoldIdv();
		int AvailSCI = pOthLegs->AuthFcBkt() - pOthLegs->AuthResHoldGrp() - pOthLegs->AuthResHoldIdv();
		int AvailBkt0 = pOthLegs->AuthResHoldNY() - pOthLegs->CmptResHoldNY();
		// Pour eviter de créer d'autre membre dans le dommaine YmIcRelatedDom, je réutilise 
		// AUTH_RES_HOLD_NY pour récupérer l'autorisation en bucket 0
		// CMPT_RES_HOLD_NY pour récupérer la somme des ventes IDV + GRP dans l'espace physique.
		iAvail = __min (AvailSCG, AvailSCI);
		iAvail = __min (iAvail, AvailBkt0);
		pOthLegs->Availability(iAvail);
	}
	delete pIterator;
}

// Calcul le revenu pour chaque compartiment
int COthLegsView::CalcRevenu( YmIcOthLegsDom* pOthLegsP )
{
	YM_Iterator<YmIcOthLegsDom>* pIterator = 
		YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY)->CreateIterator();
	int iAuth = 0;
	int irevenu = 0;
	int iResHoldAccum = 0;
	BOOL bPrixGaranti = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag();
	YmIcOthLegsDom* pOthLegs;
	for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
	{
		pOthLegs = pIterator->Current();
		if (
			(!strcmp(pOthLegs->LegOrig(), pOthLegsP->LegOrig()) ) &&
			(!strcmp(pOthLegs->LegDest(), pOthLegsP->LegDest()) ) &&
			(!strcmp(pOthLegs->Cmpt(), pOthLegsP->Cmpt()) )
			)
		{
			irevenu += pOthLegs->Revenu();
			if (bPrixGaranti)
				irevenu += pOthLegs->RevenuGaranti();
		}
	}
	delete pIterator;
	return irevenu;
}

// NPI - Ano 70148
// La fonction BuildGraph sert désormais d'aiguillage suivant le fonctionnement de SATC
//	- classique --> BuilGraphGeneral (fontionnement par sphère de prix)
//	- ALLEO --> BuildGraphAlleo (fonctionnnement identique à ce qui se faisait avant souplesse)
void COthLegsView::BuildGraph(enum CCTAChartData::GraphType eGraphType)
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

//DM87978 KGH
//void COthLegsView::BuildGraph(enum CCTAChartData::GraphType eGraphType)
void COthLegsView::BuildGraphGeneral(enum CCTAChartData::GraphType eGraphType)
{	 
	//DM7978 KGH
	if (eGraphType == CCTAChartData::eCmpt) 
		m_Chart.GetHelpDlgCmptArray()->RemoveAll();// delete the elements of the help dialog array
	//DM7978FIN KGH

	// Set text in status bar control
	((CChildFrame*)GetParent())->GetStatusBar()->SetPaneOneAndLock( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BUILDING_GRAPH) );

	// DM7978 KGH
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	YmIcTrancheLisDom* pCurTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	CSpherePrix  spherePrixTrouvee;
	//si SC ou CC
	SpheresPrixMap mapSpherePrix = (GetAltFlag()) ? pDoc->m_sphereManager->getListeSpherePrixSC(pCurTranche->Nature(),pCurTranche->Entity()) : pDoc->m_sphereManager->getListeSpherePrixCC(pCurTranche->Nature(),pCurTranche->Entity());
	m_nbSpheresPrixMax = mapSpherePrix.size();
	m_nbSpheresPrixLegende = pDoc->m_sphereManager->getNbMaxSpheresPrix(mapSpherePrix);//sert pour la legende
	//si la liste des spheres est vide
	if(m_nbSpheresPrixMax <= 0)
	{
		((CChildFrame*)GetParent())->GetStatusBar()->SetPaneOneAndLock( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_SPHERE) );
		return;
	}
	//DM7978 KGH FIN

	YM_Iterator<YmIcOthLegsDom>* pIterator;
	pIterator = YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY)->CreateIterator();

	BOOL bPrixGaranti = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag();

	// build the data for the graph
	long lNumPts = pIterator->GetCount();
	if (lNumPts > 0)
	{
		m_Chart.SetRepaint( FALSE );

		CString szKey;
		CString szKeyHold;
		CString szLegOrigHold;
		char* pszLegDestHold;
		int iLegCount = 0;
		int iCmptMaxCount = 1;
		int iCmptCount = 0;
		int iNstLvlCount  = 1;

		// loop through the record set counting cmpts and bkts
		YmIcOthLegsDom* pOthLegs;
		for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
		{
			pOthLegs = pIterator->Current();

			// build the key of leg orig and cmpt for the data break
			szKey = pOthLegs->LegOrig();
			szKey += pOthLegs->Cmpt();

			if ( IsLegSelected( pOthLegs ) )  // do we graph this leg ?
			{
				if ( szKey != szKeyHold )  // did the key  change ?
				{
					szKeyHold = szKey;  // reset key

					// different leg origin ?
					if ( strcmp( (LPCSTR)szLegOrigHold,  pOthLegs->LegOrig() ) )
					{
						iLegCount++;  // new leg, increment leg count
						szLegOrigHold = pOthLegs->LegOrig();   // reset leg orig hold

						// hold the max number of compartments
						iCmptMaxCount = __max( iCmptCount, iCmptMaxCount );
						iCmptCount = 0;  // reset cmpt count
					}

					iCmptCount++;  // new compartment, increment cmpt count
					iCmptMaxCount = __max( iCmptCount, iCmptMaxCount );

					// hold the max number of bkts for any compartment
					m_iNstLvlMaxCount = __max( iNstLvlCount, m_iNstLvlMaxCount );
					iNstLvlCount = 1;  // reset bkt count
				} /* Fin szKey!= szKeyHold */

				iNstLvlCount++;
				//Ano 147812 sur train mono-tronçon et mono compartiment
				m_iNstLvlMaxCount = __max( iNstLvlCount, m_iNstLvlMaxCount );
			} /* Fin IsLegSelected */
		}  /* Fin for iterator */

		// allocate the arrays for the graph data
		// Note that the number of data points changes here
		int iBarCount = iLegCount * iCmptMaxCount;  // this is the number of bars we will graph
		if (iBarCount > 0)
		{
			double (*adXData);
			adXData = new double [iBarCount];

			// graphing different data sets for compartment graph;  different nest levels for bucket graph

			//DM-7978 KGH
			int iNumYElements = (eGraphType == CCTAChartData::eCmpt) ? iBarCount * eNumberDataSets : iBarCount * m_nbSpheresPrixMax;

			// initialiser les libelles des spheres 
			pDoc->m_sphereManager->initLabelSphere(mapSpherePrix, iBarCount, (GetAltFlag() != 0), OTH_LEGS);

			double (*adYDataSpherePrix);
			adYDataSpherePrix = new double[iNumYElements];
			memset( adYDataSpherePrix, 0, sizeof(double) * iNumYElements );
			//DM-7978 KGH FIN

			double (*adYData2);
			adYData2 = new double[iBarCount * eNumberDataSets2];
			memset( adYData2, 0, sizeof(double) * (iBarCount * eNumberDataSets2) );

			// transfer the data from the record set to the graph data structure
			szKeyHold.Empty();
			szLegOrigHold.Empty();
			pszLegDestHold = NULL;
			int iIndex  = 0;
			XrtValueLabel label;

			// create a new cta graph data object
			CCTAChartData* pCTAChartData = new CCTAChartData( &m_Chart, eGraphType, NULL, NULL );

			// following variables are used to create the help dialog labels array...
			CString szLegOrigHoldBkt;
			CString szLegDestHoldBkt;
			CString szCmptHoldBkt;
			int iBktAvail = 0;
			int iPrevBktAvail = 0;

			if (eGraphType != CCTAChartData::eRecettes) 
			{
				// Recalculate the availability values throughout the record set.
				//   Loop through all the records in the RecordSet, 
				//   and recalculate the availability exactly like we do in the ResaRail window CalcAvail() method
				if (GetAltFlag())
					ReCalcAvailSci();
				else
					ReCalcAvail();
			}

			int iNstLvlIdx = 0;  // nest level 
			for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
			{
				pOthLegs = pIterator->Current();
				long CapValue = pOthLegs->Capacity();
				double dBarThickness = (double)((float)CapValue * CAPACITY_BAR_THICKNESS);

				// DM7978 KGH
				// recuperer la sphere de prix qui correspond au compartiment et nestLevel actuel
				if(GetAltFlag())//SC		
				{
					spherePrixTrouvee = pDoc->m_sphereManager->findSpherePrixSC(pOthLegs->NestLevel(),pOthLegs->Cmpt(),pCurTranche->Nature(),pCurTranche->Entity());//chercher la sphere qui correspond a NestLevel et Cmpt
				}
				else //CC
				{
					spherePrixTrouvee = pDoc->m_sphereManager->findSpherePrixCC(pOthLegs->NestLevel(),pOthLegs->Cmpt(),pCurTranche->Nature(),pCurTranche->Entity());//chercher la sphere qui correspond a NestLevel et Cmpt
				}
				//si aucune sphere trouvee sortir de l'appli
				if(spherePrixTrouvee.getSphereId() == -1)
				{
					((CChildFrame*)GetParent())->GetStatusBar()->SetPaneOneAndLock( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_SPHERE) );
					return;
				}

				int positionSphere = spherePrixTrouvee.getPosition(); // position de la sphere selon son ordonnancement par rapport au autres spheres (commence par 0)
				// DM7978 KGH FIN

				//JMG, affiche de la dispo SCG au lieu de la capa SCG sur l'onglet dispo SCI.
				if (GetAltFlag())
				{
					if (eGraphType == CCTAChartData::eBkt)
					{
						int AvailScg = pOthLegs->Capacity() - pOthLegs->CmptResHoldGrp() - pOthLegs->CmptResHoldIdv();
						int AvailBkt0 = pOthLegs->AuthResHoldNY() - pOthLegs->CmptResHoldNY(); // Voir explication dans CalcAvailSci
						CapValue = __min (AvailScg, AvailBkt0);
					}
				}

				if ( IsLegSelected( pOthLegs ) )  // do we graph this leg ?
				{
					// for the compartment level graph, we only use a subset of the data points
					//    so, we must look for a break in the leg origin or compartment...
					// build the key of leg orig and cmpt for the data break
					szKey = pOthLegs->LegOrig();
					szKey += pOthLegs->Cmpt();

					if ( szKey != szKeyHold )  // cmpt level, only use first record of key break
					{
						szKeyHold = szKey;  // reset key

						iNstLvlIdx = 0;  // reset nest level index

						iBktAvail = iPrevBktAvail = 0;  // reset avail variables

						// data to be graphed
						// the Y2 graph data is common between the cmpt and bkt level graphs
						if(eGraphType != CCTAChartData::eRecettes )
						{
							adYData2[iIndex + (eEmptySpace1*iBarCount)] = (double)CapValue - dBarThickness;
							adYData2[iIndex + (eCapacity*iBarCount)] = dBarThickness;
						}

						if (eGraphType == CCTAChartData::eCmpt)
						{
							// there are inconsistencies between (ResHoldIdv + ResHoldGrp) and ResHoldInd...
							double dCmptResHoldTot = ((double)pOthLegs->CmptResHoldIdv() +    (double)pOthLegs->CmptResHoldGrp());

							// Add this record to the array used for the chart help dialog
							m_Chart.GetHelpDlgCmptArray()->Add( pOthLegs );

							// set the graph values in the olectra chart array...
							//DM7978 KGH
							adYDataSpherePrix[iIndex + (eResHoldTot*iBarCount)]   = dCmptResHoldTot;
							// If we are graphing Additional Capacity, we do not graph auth fcst, 
							adYDataSpherePrix[iIndex + (eAuthDptFcst*iBarCount)]  = (double)pOthLegs->AuthFcCmpt();
							//DM7978 KGH FIN
							// set some maximum graph values
							pCTAChartData->SetGraphMax( (double)pOthLegs->AuthFcCmpt() );
							pCTAChartData->SetGraphMax( (double)pOthLegs->DmdDptFcCmpt() );
							pCTAChartData->SetGraphMax( dCmptResHoldTot );
							pCTAChartData->SetGraphMax( (double)pOthLegs->Capacity() );
							pCTAChartData->SetAuthMax( (double)pOthLegs->AuthFcCmpt() );
							pCTAChartData->SetDmdDptFcstMax( (double)pOthLegs->DmdDptFcCmpt() );
						}/* Fin Graphique eCmpt */

						// set the label for the X-axis:  cmpt
						label.value = (iIndex+1);
						label.string = (char *)pOthLegs->Cmpt();
						m_Chart.SetValueLabel(XRT_AXIS_X, &label);

						// set the label for the X-axis:  leg origin
						label.value = (iIndex+1) - .5;
						if ( szLegOrigHold != pOthLegs->LegOrig() )  // station code change ?
						{
							label.string = (char *)pOthLegs->LegOrig();
							m_Chart.SetValueLabel(XRT_AXIS_X, &label);

							szLegOrigHold = pOthLegs->LegOrig();
							pszLegDestHold = (char*)pOthLegs->LegDest();
						}
						else
						{
							label.string = NULL;
							m_Chart.SetValueLabel(XRT_AXIS_X, &label);
						}
						adXData[iIndex] = iIndex + 1;
						iIndex++;
					}/* Fin Graphique */

					if (eGraphType == CCTAChartData::eBkt)
					{          
						// the value pOthLegs->Availability() may have been modified in the method RecalcAvail() (if CalcAvailFlag is set)
						if (GetAltFlag()) // Sous-contingent.
							iBktAvail = pOthLegs->Availability();
						else
							iBktAvail = __max( pOthLegs->Availability() - iPrevBktAvail, 0 );
						iPrevBktAvail = __max( pOthLegs->Availability(), 0 );

						// different compartment (or tranche)?
						if ( (szCmptHoldBkt.IsEmpty() || ((pOthLegs->Cmpt()) != szCmptHoldBkt)) || 
							( strcmp( pOthLegs->LegOrig(), szLegOrigHoldBkt ) ) ||
							( strcmp( pOthLegs->LegDest(), szLegDestHoldBkt ) ) )
						{
							// build labels for X-Axis
							// create the array containing the cmpt / bkt labels for the X-Axis and help window
							szCmptHoldBkt = (pOthLegs->Cmpt());    // reset hold variables
							szLegOrigHoldBkt = pOthLegs->LegOrig();
							szLegDestHoldBkt = pOthLegs->LegDest();
						}

						// DM7978 KGH
						//accumuler par sphere de prix
						adYDataSpherePrix[(iIndex-1) + (positionSphere*iBarCount)] += (double)iBktAvail;
						// DM7978 KGH FIN

						pCTAChartData->SetGraphMax( (double)iBktAvail );
						pCTAChartData->SetAuthMax( (double)iBktAvail );
						pCTAChartData->SetDmdDptFcstMax( (double)iBktAvail );
						iNstLvlIdx++;
					} /* Fin Graphique eBkt */
					if (eGraphType == CCTAChartData::eRecettes)
					{   
						// On accumule les revenus.
						//iBktAvail = pOthLegs->Availability() + iPrevBktAvail;
						//iPrevBktAvail += pOthLegs->Availability();
						// different compartment (or tranche)?
						if ( (szCmptHoldBkt.IsEmpty() || ((pOthLegs->Cmpt()) != szCmptHoldBkt)) || 
							( strcmp( pOthLegs->LegOrig(), szLegOrigHoldBkt ) ) ||
							( strcmp( pOthLegs->LegDest(), szLegDestHoldBkt ) ) )
						{
							// build labels for X-Axis
							// create the array containing the cmpt / bkt labels for the X-Axis and help window
							szCmptHoldBkt = (pOthLegs->Cmpt());    // reset hold variables
							szLegOrigHoldBkt = pOthLegs->LegOrig();
							szLegDestHoldBkt = pOthLegs->LegDest();
						}

						long iRev = pOthLegs->Revenu();
						if (bPrixGaranti)
							iRev += pOthLegs->RevenuGaranti();
						//DM-7978 KGH
						//accumuler par sphere de prix
						adYDataSpherePrix[(iIndex-1) + (positionSphere*iBarCount)] += iRev;
						//DM-7978 KGH FIN
						pCTAChartData->SetGraphMax( (double)iBktAvail );
						pCTAChartData->SetAuthMax( (double)iBktAvail );
						pCTAChartData->SetDmdDptFcstMax( (double)iBktAvail );
						iNstLvlIdx++;
					}/* Fin Graphique eRecettes */

				} /* IsLegSelected( pOthLegs ) */
			}  /* end of iteration through records Fin de For sur Iterateur */

			int iIndexLast = iIndex;
			for ( iIndex; iIndex < iBarCount; iIndex++ )
				adXData[iIndex] = iIndex + 1;

			// build the label for the last leg destination...
			if ( IsLegSelected( pOthLegs ) )  // did we graph this leg ?
				pszLegDestHold = (char*)pOthLegs->LegDest();

			// add the last leg destination
			label.value = (iIndexLast+1) - .5;
			label.string = pszLegDestHold;
			//        label.string = (char *)pOthLegs->LegDest();
			m_Chart.SetValueLabel(XRT_AXIS_X, &label);

			//DM7978 KGH
			int iPtCount = (eGraphType == CCTAChartData::eCmpt) ? eNumberDataSets : m_nbSpheresPrixMax;
			CChart2DData* pCmptChartDataSpherePrix = new CChart2DData(XRT_DATA_ARRAY, iPtCount, iBarCount);

			// set the Y elements Sphere
			for (iIndex = 0; iIndex < m_nbSpheresPrixMax; iIndex++)
				pCmptChartDataSpherePrix->SetYData(iIndex, adYDataSpherePrix+(iIndex*iBarCount), iBarCount, 0);

			// set the X elements
			for (iIndex = 0; iIndex <= iIndexLast; iIndex++)
				pCmptChartDataSpherePrix->SetXElement(NULL, iIndex, adXData[iIndex]);

			pCTAChartData->SetDataObj( pCmptChartDataSpherePrix );  // set the Olectra data object in the CTA data object //sphere
			//DM7978 KGH FIN
			CChart2DData* pCmptChartData2 = NULL;

			pCmptChartData2 = new CChart2DData(XRT_DATA_ARRAY, eNumberDataSets2, iBarCount);

			// set the Y elements for the 2nd graph (capacity)
			if ( eGraphType != CCTAChartData::eRecettes )
			{
				for (iIndex = 0; iIndex < eNumberDataSets2; iIndex++)
					pCmptChartData2->SetYData(iIndex,  adYData2+(iIndex*iBarCount), iBarCount, 0);

				// set the X elements for the 2nd graph (capacity)
				int iXBound = (eGraphType == CCTAChartData::eCmpt) ? iIndexLast : iBarCount;

				for (iIndex = 0; iIndex < iXBound; iIndex++)
					pCmptChartData2->SetXElement(NULL, iIndex, adXData[iIndex]);

				// hide the portion of the (stacked) bar below the capacity
				//   to make it look like a solid line
				pCmptChartData2->SetDisplay(eEmptySpace1, XRT_DISPLAY_HIDE);
			}
			pCTAChartData->SetDataObj2( pCmptChartData2 );  // set the Olectra data object in the CTA data object

			m_Chart.AddChartData( pCTAChartData, FALSE );  // add this Chart Data element to the array

			// cleanup
			//DM7978 KGH
			delete [ ] adYDataSpherePrix;
			adYDataSpherePrix = NULL;
			// DM7978 KGH FIN
			delete [ ] adXData;
			adXData = NULL;
			delete [ ] adYData2;
			adYData2 = NULL;
		} /* Fin iBarCount > 0 */
		m_Chart.SetRepaint( TRUE );
	} /* Fin de lNumPts >0 */
	else
	{
		// Set text in status bar control
		((CChildFrame*)GetParent())->GetStatusBar()->SetPaneOneAndLock( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA) );
	} /* Fin du else de lNumPts >0 */
	delete pIterator;
} /* Fin de BuildGraph */


//DM7978 KGH
// Build the Cmpt level graph
//void COthLegsView::BuildGraph(enum CCTAChartData::GraphType eGraphType)
void COthLegsView::BuildGraphAlleo(enum CCTAChartData::GraphType eGraphType)
{	 
	(eGraphType == CCTAChartData::eCmpt) ? m_Chart.GetHelpDlgCmptArray()->RemoveAll( ) : ClearCmptBktLabelArray(); // delete the elements of the help dialog array

	// Set text in status bar control
	((CChildFrame*)GetParent())->GetStatusBar()->SetPaneOneAndLock( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BUILDING_GRAPH) );

	YM_Iterator<YmIcOthLegsDom>* pIterator;
	pIterator = YM_Set<YmIcOthLegsDom>::FromKey((GetAltFlag()) ? OTHLEGS_ALT_KEY : OTHLEGS_KEY)->CreateIterator();

	BOOL bPrixGaranti = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag();

	// build the data for the graph
	long lNumPts = pIterator->GetCount();
	if (lNumPts > 0)
	{
		m_Chart.SetRepaint( FALSE );

		CString szKey;
		CString szKeyHold;
		CString szLegOrigHold;
		char* pszLegDestHold;
		int iLegCount = 0;
		int iCmptMaxCount = 1;
		int iCmptCount = 0;
		int iNstLvlCount  = 1;

		// loop through the record set counting cmpts and bkts
		YmIcOthLegsDom* pOthLegs;
		for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
		{
			pOthLegs = pIterator->Current();

			// build the key of leg orig and cmpt for the data break
			szKey = pOthLegs->LegOrig();
			szKey += pOthLegs->Cmpt();

			if ( IsLegSelected( pOthLegs ) )  // do we graph this leg ?
			{
				if ( szKey != szKeyHold )  // did the key  change ?
				{
					szKeyHold = szKey;  // reset key

					// different leg origin ?
					if ( strcmp( (LPCSTR)szLegOrigHold,  pOthLegs->LegOrig() ) )
					{
						iLegCount++;  // new leg, increment leg count
						szLegOrigHold = pOthLegs->LegOrig();   // reset leg orig hold

						// hold the max number of compartments
						iCmptMaxCount = __max( iCmptCount, iCmptMaxCount );
						iCmptCount = 0;  // reset cmpt count
					}

					iCmptCount++;  // new compartment, increment cmpt count
					iCmptMaxCount = __max( iCmptCount, iCmptMaxCount );

					// hold the max number of bkts for any compartment
					m_iNstLvlMaxCount = __max( iNstLvlCount, m_iNstLvlMaxCount );
					iNstLvlCount = 1;  // reset bkt count
				} // Fin szKey!= szKeyHold

				iNstLvlCount++;
				//Ano 147812 sur train mono-tronçon et mono compartiment
				m_iNstLvlMaxCount = __max( iNstLvlCount, m_iNstLvlMaxCount );
			} // Fin IsLegSelected 
		}  // Fin for iterator 

		// allocate the arrays for the graph data
		// Note that the number of data points changes here
		int iBarCount = iLegCount * iCmptMaxCount;  // this is the number of bars we will graph
		if (iBarCount > 0)
		{
			double (*adXData);
			adXData = new double [iBarCount];

			// graphing different data sets for compartment graph;  different nest levels for bucket graph

			int iNumYElements = (eGraphType == CCTAChartData::eCmpt) ? iBarCount * eNumberDataSets : iBarCount * m_iNstLvlMaxCount;

			double (*adYData);
			adYData = new double[iNumYElements];
			memset( adYData, 0, sizeof(double) * iNumYElements );

			double (*adYData2);
			adYData2 = new double[iBarCount * eNumberDataSets2];
			memset( adYData2, 0, sizeof(double) * (iBarCount * eNumberDataSets2) );

			// transfer the data from the record set to the graph data structure
			szKeyHold.Empty();
			szLegOrigHold.Empty();
			pszLegDestHold = NULL;
			int iIndex  = 0;
			XrtValueLabel label;

			// create a new cta graph data object
			CCTAChartData* pCTAChartData = new CCTAChartData( &m_Chart, eGraphType, NULL, NULL );

			// following variables are used to create the help dialog labels array...
			CString szLegOrigHoldBkt;
			CString szLegDestHoldBkt;
			CString szCmptHoldBkt;
			int iCmptBktLabelIdx = -1;
			int iBktLabelIdx = 0;

			int iBktAvail = 0;
			int iPrevBktAvail = 0;

			if (eGraphType != CCTAChartData::eRecettes) 
			{
				// Recalculate the availability values throughout the record set.
				//   Loop through all the records in the RecordSet, 
				//   and recalculate the availability exactly like we do in the ResaRail window CalcAvail() method
				if (GetAltFlag())
					ReCalcAvailSci();
				else
					ReCalcAvail();
			}

			int iNstLvlIdx = 0;  // nest level 
			for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
			{
				pOthLegs = pIterator->Current();
				long CapValue = pOthLegs->Capacity();
				double dBarThickness = (double)((float)CapValue * CAPACITY_BAR_THICKNESS);

				//JMG, affiche de la dispo SCG au lieu de la capa SCG sur l'onglet dispo SCI.
				if (GetAltFlag())
				{
					if (eGraphType == CCTAChartData::eBkt)
					{
						int AvailScg = pOthLegs->Capacity() - pOthLegs->CmptResHoldGrp() - pOthLegs->CmptResHoldIdv();
						int AvailBkt0 = pOthLegs->AuthResHoldNY() - pOthLegs->CmptResHoldNY(); // Voir explication dans CalcAvailSci
						CapValue = __min (AvailScg, AvailBkt0);
					}
				}

				if ( IsLegSelected( pOthLegs ) )  // do we graph this leg ?
				{
					// for the compartment level graph, we only use a subset of the data points
					//    so, we must look for a break in the leg origin or compartment...
					// build the key of leg orig and cmpt for the data break
					szKey = pOthLegs->LegOrig();
					szKey += pOthLegs->Cmpt();

					if ( szKey != szKeyHold )  // cmpt level, only use first record of key break
					{
						szKeyHold = szKey;  // reset key

						iNstLvlIdx = 0;  // reset nest level index

						iBktAvail = iPrevBktAvail = 0;  // reset avail variables

						// data to be graphed
						// the Y2 graph data is common between the cmpt and bkt level graphs
						if(eGraphType != CCTAChartData::eRecettes )
						{
							adYData2[iIndex + (eEmptySpace1*iBarCount)] = (double)CapValue - dBarThickness;
							adYData2[iIndex + (eCapacity*iBarCount)] = dBarThickness;
						}

						if (eGraphType == CCTAChartData::eCmpt)
						{
							// there are inconsistencies between (ResHoldIdv + ResHoldGrp) and ResHoldInd...
							double dCmptResHoldTot = ((double)pOthLegs->CmptResHoldIdv() +    (double)pOthLegs->CmptResHoldGrp());

							// Add this record to the array used for the chart help dialog
							m_Chart.GetHelpDlgCmptArray()->Add( pOthLegs );

							// set the graph values in the olectra chart array...
							adYData[iIndex + (eResHoldTot*iBarCount)]   = dCmptResHoldTot;
							// If we are graphing Additional Capacity, we do not graph auth fcst, 
							adYData[iIndex + (eAuthDptFcst*iBarCount)]  = (double)pOthLegs->AuthFcCmpt();
							// set some maximum graph values
							pCTAChartData->SetGraphMax( (double)pOthLegs->AuthFcCmpt() );
							pCTAChartData->SetGraphMax( (double)pOthLegs->DmdDptFcCmpt() );
							pCTAChartData->SetGraphMax( dCmptResHoldTot );
							pCTAChartData->SetGraphMax( (double)pOthLegs->Capacity() );
							pCTAChartData->SetAuthMax( (double)pOthLegs->AuthFcCmpt() );
							pCTAChartData->SetDmdDptFcstMax( (double)pOthLegs->DmdDptFcCmpt() );
						}// Fin Graphique eCmpt

						// set the label for the X-axis:  cmpt
						label.value = (iIndex+1);
						label.string = (char *)pOthLegs->Cmpt();
						m_Chart.SetValueLabel(XRT_AXIS_X, &label);

						// set the label for the X-axis:  leg origin
						label.value = (iIndex+1) - .5;
						if ( szLegOrigHold != pOthLegs->LegOrig() )  // station code change ?
						{
							label.string = (char *)pOthLegs->LegOrig();
							m_Chart.SetValueLabel(XRT_AXIS_X, &label);

							szLegOrigHold = pOthLegs->LegOrig();
							pszLegDestHold = (char*)pOthLegs->LegDest();
						}
						else
						{
							label.string = NULL;
							m_Chart.SetValueLabel(XRT_AXIS_X, &label);
						}
						adXData[iIndex] = iIndex + 1;
						iIndex++;
					}// Fin Graphique

					if (eGraphType == CCTAChartData::eBkt)
					{          
						// the value pOthLegs->Availability() may have been modified in the method RecalcAvail() (if CalcAvailFlag is set)
						if (GetAltFlag()) // Sous-contingent.
							iBktAvail = pOthLegs->Availability();
						else
							iBktAvail = __max( pOthLegs->Availability() - iPrevBktAvail, 0 );
						iPrevBktAvail = __max( pOthLegs->Availability(), 0 );

						// different compartment (or tranche)?
						if ( (szCmptHoldBkt.IsEmpty() || ((pOthLegs->Cmpt()) != szCmptHoldBkt)) || 
							( strcmp( pOthLegs->LegOrig(), szLegOrigHoldBkt ) ) ||
							( strcmp( pOthLegs->LegDest(), szLegDestHoldBkt ) ) )
						{
							// build labels for X-Axis
							// create the array containing the cmpt / bkt labels for the X-Axis and help window
							iCmptBktLabelIdx++;
							iBktLabelIdx = 0;
							CmptBktLabels* pCmptBktLabels = new CmptBktLabels;
							m_paCmptBktLabels.Add( pCmptBktLabels );

							//if ( m_paCmptBktLabels.GetSize() <= iCmptBktLabelIdx )
							//m_paCmptBktLabels.SetAtGrow( iCmptBktLabelIdx, new CmptBktLabels );

							m_paCmptBktLabels[ iCmptBktLabelIdx ]->szCmpt = (pOthLegs->Cmpt());

							szCmptHoldBkt = (pOthLegs->Cmpt());    // reset hold variables
							szLegOrigHoldBkt = pOthLegs->LegOrig();
							szLegDestHoldBkt = pOthLegs->LegDest();
						}

						// add the bucket to the X-Axis label for this compartment
						m_paCmptBktLabels[ iCmptBktLabelIdx ]->aszBktArray.Add( pOthLegs->BucketLabel() );
						//m_paCmptBktLabels[ iCmptBktLabelIdx ]->aszBktArray.SetAtGrow( iBktLabelIdx, pOthLegs->BucketLabel() );
						iBktLabelIdx++;

						adYData[ (iNstLvlIdx * iBarCount) + (iIndex-1) ]  = (double)iBktAvail;

						pCTAChartData->SetGraphMax( (double)iBktAvail );
						pCTAChartData->SetAuthMax( (double)iBktAvail );
						pCTAChartData->SetDmdDptFcstMax( (double)iBktAvail );
						iNstLvlIdx++;
					} // Fin Graphique eBkt
					if (eGraphType == CCTAChartData::eRecettes)
					{   
						// On accumule les revenus.
						//iBktAvail = pOthLegs->Availability() + iPrevBktAvail;
						//iPrevBktAvail += pOthLegs->Availability();
						// different compartment (or tranche)?
						if ( (szCmptHoldBkt.IsEmpty() || ((pOthLegs->Cmpt()) != szCmptHoldBkt)) || 
							( strcmp( pOthLegs->LegOrig(), szLegOrigHoldBkt ) ) ||
							( strcmp( pOthLegs->LegDest(), szLegDestHoldBkt ) ) )
						{
							// build labels for X-Axis
							// create the array containing the cmpt / bkt labels for the X-Axis and help window
							iCmptBktLabelIdx++;
							iBktLabelIdx = 0;

							CmptBktLabels* pCmptBktLabels = new CmptBktLabels;
							m_paCmptBktLabels.Add( pCmptBktLabels );
							m_paCmptBktLabels[ iCmptBktLabelIdx ]->szCmpt = (pOthLegs->Cmpt());

							szCmptHoldBkt = (pOthLegs->Cmpt());    // reset hold variables
							szLegOrigHoldBkt = pOthLegs->LegOrig();
							szLegDestHoldBkt = pOthLegs->LegDest();
						}

						// add the bucket to the X-Axis label for this compartment
						m_paCmptBktLabels[ iCmptBktLabelIdx ]->aszBktArray.Add(pOthLegs->BucketLabel());
						//m_paCmptBktLabels[ iCmptBktLabelIdx ]->aszBktArray.SetAtGrow( iBktLabelIdx, pOthLegs->BucketLabel() );
						iBktLabelIdx++;

						long iRev = pOthLegs->Revenu();
						if (bPrixGaranti)
							iRev += pOthLegs->RevenuGaranti();

						adYData[ (iNstLvlIdx * iBarCount) + (iIndex-1) ]  = iRev;
						pCTAChartData->SetGraphMax( (double)iBktAvail );
						pCTAChartData->SetAuthMax( (double)iBktAvail );
						pCTAChartData->SetDmdDptFcstMax( (double)iBktAvail );
						iNstLvlIdx++;
					}// Fin Graphique eRecettes

				} // IsLegSelected( pOthLegs )
			}  // end of iteration through records Fin de For sur Iterateur

			int iIndexLast = iIndex;
			for ( iIndex; iIndex < iBarCount; iIndex++ )
				adXData[iIndex] = iIndex + 1;

			// build the label for the last leg destination...
			if ( IsLegSelected( pOthLegs ) )  // did we graph this leg ?
				pszLegDestHold = (char*)pOthLegs->LegDest();

			// add the last leg destination
			label.value = (iIndexLast+1) - .5;
			label.string = pszLegDestHold;
			//        label.string = (char *)pOthLegs->LegDest();
			m_Chart.SetValueLabel(XRT_AXIS_X, &label);

			int iPtCount = (eGraphType == CCTAChartData::eCmpt) ? eNumberDataSets : m_iNstLvlMaxCount;
			CChart2DData* pCmptChartData = new CChart2DData(XRT_DATA_ARRAY, iPtCount, iBarCount);

			// set the Y elements
			for (iIndex = 0; iIndex < iPtCount; iIndex++)
				pCmptChartData->SetYData(iIndex, adYData+(iIndex*iBarCount), iBarCount, 0);

			// set the X elements
			for (iIndex = 0; iIndex <= iIndexLast; iIndex++)
				pCmptChartData->SetXElement(NULL, iIndex, adXData[iIndex]);

			pCTAChartData->SetDataObj( pCmptChartData );  // set the Olectra data object in the CTA data object
			CChart2DData* pCmptChartData2 = NULL;

			pCmptChartData2 = new CChart2DData(XRT_DATA_ARRAY, eNumberDataSets2, iBarCount);

			// set the Y elements for the 2nd graph (capacity)
			for (iIndex = 0; iIndex < eNumberDataSets2; iIndex++)
				pCmptChartData2->SetYData(iIndex,  adYData2+(iIndex*iBarCount), iBarCount, 0);

			// set the X elements for the 2nd graph (capacity)
			int iXBound = (eGraphType == CCTAChartData::eCmpt) ? iIndexLast : iBarCount;

			for (iIndex = 0; iIndex < iXBound; iIndex++)
				pCmptChartData2->SetXElement(NULL, iIndex, adXData[iIndex]);

			// hide the portion of the (stacked) bar below the capacity
			//   to make it look like a solid line
			pCmptChartData2->SetDisplay(eEmptySpace1, XRT_DISPLAY_HIDE);

			pCTAChartData->SetDataObj2( pCmptChartData2 );  // set the Olectra data object in the CTA data object

			m_Chart.AddChartData( pCTAChartData, FALSE );  // add this Chart Data element to the array

			// cleanup
			delete [ ] adYData;
			adYData = NULL;
			delete [ ] adXData;
			adXData = NULL;
			delete [ ] adYData2;
			adYData2 = NULL;
		} // Fin iBarCount > 0
		m_Chart.SetRepaint( TRUE );
	} // Fin de lNumPts >0/
	else
	{
		// Set text in status bar control
		((CChildFrame*)GetParent())->GetStatusBar()->SetPaneOneAndLock( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA) );
	} // Fin du else de lNumPts >0 
	delete pIterator;
} // Fin de BuildGraph /
///

///////////////////////////////////////////////////////////////
// Set the data and Show the graph
//
void COthLegsView::ShowGraph()
{
	CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
	if ( ( pCTAChartData == NULL ) || ( m_ChartTabCtrl.GetItemCount() == 0 ) )
	{
		// Set text in status bar control
		((CChildFrame*)GetParent())->GetStatusBar()->SetPaneOneAndLock( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA) );
		return;
	}

	m_Chart.RemoveFlashLabel();  // remove flash label if up
	m_Chart.SetRepaint( FALSE );

	// are we showing the compartment or the bucket graph ?
	m_Chart.SetGraphProperties(pCTAChartData->GetGraphType());

	// set the chart data object for the tab selected
	m_Chart.SetData( *(pCTAChartData->GetDataObj()) );

	// set the axis for the 2nd graph equal to the primary graph
	m_Chart.SetYAxisMin(m_Chart.GetYAxisMin());
	m_Chart.SetYAxisMax(pCTAChartData->GetGraphMax());

	m_Chart.SetData2( *(pCTAChartData->GetDataObj2()) );
	m_Chart.SetY2NumMethod(m_Chart.GetYNumMethod());
	m_Chart.SetY2Num(m_Chart.GetYNum());
	m_Chart.SetY2AxisMin(m_Chart.GetYAxisMin());
	m_Chart.SetY2AxisMax(pCTAChartData->GetGraphMax());
	m_Chart.SetY2AxisShow(FALSE);
	m_Chart.SetFrontDataset(XRT_DATASET2);

	// Set text in status bar control
	((CChildFrame*)GetParent())->GetStatusBar()->SetPaneOneAndLock( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DONE) );

	// show the selected graph and hide the other one...
	m_Chart.ScaleGraph(m_Chart.GetCurScaling());  // don't forget to rescale the graph
	m_Chart.SetRepaint( TRUE );
	m_Chart.ShowWindow( SW_SHOW );
}

// NPI - Ano 70148
// Réactivation de la méthode ci-dessous
//DM7978 KGH cette methode n'est plus utilisee
//////////////////////////////////////////////////////////
// Clear the array containing the tranche info for the labels
//
void COthLegsView::ClearCmptBktLabelArray()
{
	for (int i = 0; i < m_paCmptBktLabels.GetSize(); i++)
	{
		delete m_paCmptBktLabels[i];  // delete the text label
		m_paCmptBktLabels[i] = NULL;
	}
	m_paCmptBktLabels.RemoveAll( );
}
//DM7978 FIN KGH
///


//////////////////////////////////////
// Update the authorization values, and refresh the graph window
//
void COthLegsView::UpdateAuthValue( const char* szLegOrig, const char* szCmpt, const long lNestLevel, long lValue )
{
}

//////////////////////////////////////////////////////////
// Clear the tabs array and removes the corresponding tab
//
void COthLegsView::ClearTabs()
{
	// clean up the array containing the tabs and graph data objects
	m_ChartTabCtrl.DeleteAllItems( ); // remove the tab from the control
}

//////////////////////////////////////
// Build the tabs on the tab control
//
void COthLegsView::BuildTabs()
{
	int nLastTab = -1;
	int Flag_BuildOK;
	CCTAChartDataArray* apCTAChartData = m_Chart.GetChartDataArray();
	if (apCTAChartData->GetSize() > 0)
	{
		CCTAChartData* pCTAChartData = NULL;

		TC_ITEM TabCtrlItem;
		TabCtrlItem.mask = TCIF_TEXT;

		CString szBuffer;

		// loop through the tab index array, build and insert each tab
		for (int iIndex = 0; iIndex < apCTAChartData->GetSize(); iIndex++)
		{
			Flag_BuildOK = 1;
			pCTAChartData = m_Chart.GetChartData(iIndex);

			if ( pCTAChartData->GetGraphType() == CCTAChartData::eBkt )
			{
				szBuffer = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AVAIL);
				szBuffer += " ";
			}

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
		ShowGraph();
	}
	else
		m_iCurTabSelection = -1;

	if (nLastTab != -1) {
		m_ChartTabCtrl.SetCurSel( nLastTab );
		LRESULT result;
		OnChartSelchange( NULL, &result );
	}
}

/////////////////////////////////////////////////////////////////////////////
// COthLegsChart

BEGIN_MESSAGE_MAP(COthLegsChart, CCTAChart)
	//{{AFX_MSG_MAP(COthLegsChart)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////
// Constructor
//
COthLegsChart::COthLegsChart()
{
	m_bRotateLabels = FALSE;
	m_paSphereDataStyles = NULL; //DM7978 KGH
	m_nSpherePrix = -1;
}

//DM7978 KGH
COthLegsChart::~COthLegsChart()
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

// ScaleGraph surchargée pour ne pas s'appliquer au graphique revenu
void COthLegsChart::ScaleGraph(enum GraphScaleIndex eGraphScale )
{
	CCTAChartData* pCTAChartData = GetCurChartData();
	if (pCTAChartData && (pCTAChartData->GetGraphType() == CCTAChartData::eRecettes))
	{
		CCTAChart::ScaleGraph (CCTAChart::eMaxScaleIdx);
		SetCurScaling(eGraphScale);
	}
	else
		CCTAChart::ScaleGraph (eGraphScale);
}

//////////////////////////////////////////
// Set the graph properties
//
void COthLegsChart::SetGraphProperties(enum CCTAChartData::GraphType eGraphType)
{
	CCTAChart::SetGraphFont();

	SetXAnnotationMethod(XRT_ANNO_VALUE_LABELS);

	( m_bRotateLabels ) ? 
		SetXAnnotationRotation(XRT_ROTATE_90) :
	SetXAnnotationRotation(XRT_ROTATE_NONE);

	SetGraphShowOutlines(((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetGraphOutlineFlag());

	// define the set labels (legend) for each bar component (bkt)
	SetLegendShow(FALSE);  // but turn it off

	if (eGraphType == CCTAChartData::eCmpt)
	{
		SetBarClusterOverlap(20);  // 100 means that there is no overlap of bars
		SetBarClusterWidth(70);  // widen the cluster width a bit to make it more clear

		// set the label for each line...
		SetNthSetLabel(GetParentView()->eResHoldTot, (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_TOT));

		if (!GetParentView()->GetAltFlag())
		{ // ces infos n'existe que pour les buckets
			SetNthSetLabel(GetParentView()->eAuthDptFcst, (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AUTH_DPT));
		}

		SetNthSetLabel2(GetParentView()->eCapacity, (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(GetParentView()->GetAltFlag()? IDS_CCM1 : IDS_CAPACITY));
		SetNthSetLabel2(GetParentView()->eCapPlusUnres, (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CAP_PLUS_UNRES));
		SetNthSetLabel2(GetParentView()->eCapPlusStdgPlusUnres, (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CAP_PLUS_STDG_PLUS_UNRES));

		// NOT an Additional Capacity client
		SetType(XRT_TYPE_BAR);
		XrtDataStyle *paCmptDataStyles[] = {
			m_aDataStyles.GetAt(eAuthStyleIdx),
				m_aDataStyles.GetAt(eResHoldTotStyleIdx),
				NULL
		};
		SetDataStyles(paCmptDataStyles);

	}
	else
	{
		SetType(XRT_TYPE_STACKING_BAR);

		// we invoke these methods here because we no longer need to dynamically
		//    set these properties based upon the type of graph selected 
		//    (i.e., no longer a need to reverse the data)

		// DM7978 KGH
		//cette partie est remplacee par les couleur des spheres de prix
		/*XrtDataStyle *paCmptDataStyles[] = {
		m_aDataStyles.GetAt(eBucket9StyleIdx),
		m_aDataStyles.GetAt(eBucket1StyleIdx),
		m_aDataStyles.GetAt(eBucket2StyleIdx),
		m_aDataStyles.GetAt(eBucket3StyleIdx),
		m_aDataStyles.GetAt(eBucket4StyleIdx),
		m_aDataStyles.GetAt(eBucket5StyleIdx),
		m_aDataStyles.GetAt(eBucket6StyleIdx),
		m_aDataStyles.GetAt(eBucket7StyleIdx),
		m_aDataStyles.GetAt(eBucket8StyleIdx),
		m_aDataStyles.GetAt(eBucket0StyleIdx),
		NULL
		};*/

		// NPI - Ano 70148
		/*//recuperer les couleurs des spheres de prix et creer des styles pour les contenir
		CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
		COthLegsView* parentView = (COthLegsView*)GetParentView();
		bool altFlag = (parentView->GetAltFlag() != 0);

		YmIcTrancheLisDom* pCurTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
		// DM7978 KGH
		SpheresPrixMap mapSpherePrix;
		mapSpherePrix =  (altFlag) ? pDoc->m_sphereManager->getListeSpherePrixSC(pCurTranche->Nature(),pCurTranche->Entity()) : pDoc->m_sphereManager->getListeSpherePrixCC(pCurTranche->Nature(),pCurTranche->Entity()) ;
		const int nb_spheresPrix =  mapSpherePrix.size();

		if (m_paSphereDataStyles == NULL) // creer les nb_spheresPrix styles pour les spheres de prix + NULL a la fin
			m_paSphereDataStyles = new XrtDataStyle*[nb_spheresPrix + 1];

		for(int i = 0 ; i < nb_spheresPrix + 1; i++)	// initialiser les style
			m_paSphereDataStyles[i] = NULL;

		int currentIdx = 0;
		std::list<COLORREF> listeCouleurRGB = pDoc->m_sphereManager->getListeCouleurSpheres(altFlag,pCurTranche->Nature(),pCurTranche->Entity());

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
		//SetDataStyles(paCmptDataStyles);  // set the data styles
		SetDataStyles(m_paSphereDataStyles);  // set the data styles*/

		if (((CCTAApp*)APP)->GetRhealysFlag())
		{
			XrtDataStyle *paCmptDataStyles[] = 
			{
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
			SetDataStyles(paCmptDataStyles);  // set the data styles
		}
		else
		{
			//recuperer les couleurs des spheres de prix et creer des styles pour les contenir
			CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
			COthLegsView* parentView = (COthLegsView*)GetParentView();
			bool altFlag = (parentView->GetAltFlag() != 0);

			YmIcTrancheLisDom* pCurTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
			// DM7978 KGH
			SpheresPrixMap mapSpherePrix;
			mapSpherePrix =  (altFlag) ? pDoc->m_sphereManager->getListeSpherePrixSC(pCurTranche->Nature(),pCurTranche->Entity()) : pDoc->m_sphereManager->getListeSpherePrixCC(pCurTranche->Nature(),pCurTranche->Entity()) ;
			//const int nb_spheresPrix =  mapSpherePrix.size();
			m_nSpherePrix = mapSpherePrix.size(); 

			if (m_paSphereDataStyles == NULL) // creer les nb_spheresPrix styles pour les spheres de prix + NULL a la fin
			{
				// NPI - Ano 70149
				//m_paSphereDataStyles = new XrtDataStyle*[nb_spheresPrix + 1];
				m_paSphereDataStyles = new XrtDataStyle*[m_nSpherePrix + 1];
				///
			}

			// NPI - Ano 70149
			//for(int i = 0 ; i < nb_spheresPrix + 1; i++)	// initialiser les style
			for(int i = 0 ; i < m_nSpherePrix + 1; i++)	// initialiser les style
			///
			{
				m_paSphereDataStyles[i] = NULL;
			}

			int currentIdx = 0;
			std::list<COLORREF> listeCouleurRGB = pDoc->m_sphereManager->getListeCouleurSpheres(altFlag,pCurTranche->Nature(),pCurTranche->Entity());

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
			SetDataStyles(m_paSphereDataStyles);  // set the data styles
		}
		///
		//DM7978 KGH FIN
	}

	// capacity for both graphs...
	// for the stacked bar, we must accumulate the dataset values for the set 
	//   to be displayed on the flash label (see CCTAChart)
	SetType2(XRT_TYPE_STACKING_BAR);
	m_bAccumulateSets_DataSet2 = TRUE;

	XrtDataStyle *paCmptDataStyles2[] = {
		m_aDataStyles.GetAt(eVoidStyleIdx),  // empty space
			m_aDataStyles.GetAt(eCapStyleIdx),  // capacity
			NULL
	};
	SetDataStyles2(paCmptDataStyles2);

	// set default axis minimum
	SetYAxisMin(0);
	SetY2AxisMin(0);

}

CCTAChartHelpDlg* COthLegsChart::CreateHelpDlg()
{
	m_pHelpDlg = new COthLegsHelp( this );
	m_pHelpDlg->Create( this );
	((CMainFrame*) AfxGetApp()->GetMainWnd())->SetChartHelpDlg(m_pHelpDlg);
	return (m_pHelpDlg);
}

void COthLegsChart::ContextualDisplay(CPoint point, BOOL bInRange)
{
	((COthLegsView*)GetParentView())->OnSelectLegs();  // note that we do not care about being "In Range"
	return;
}

////////////////////////////
// Get the value to display in the flash label
//
double COthLegsChart::GetFlashLabelValue(XrtDataHandle& hXrtData, int iCurFlashSet, int iCurFlashPt, BOOL bAccumulate)
{
	if ( (GetCurChartData()->GetGraphType() == CCTAChartData::eCmpt) )
	{
		if ( bAccumulate )
		{
			// we gotta do some accumulatin'...
			double dFlashValue = 0;
			double dThisValue = 0;
			// accumulate all sets for this point...
			for( int iThisSet = 0; iThisSet <= iCurFlashSet; iThisSet++ )
			{
				dThisValue = XrtDataGetYElement( hXrtData, iThisSet, iCurFlashPt );
				if ( dThisValue > 0 )
					dFlashValue = __max( dFlashValue, 0 );
				dFlashValue += dThisValue;
			}
			return dFlashValue;
		}
		else
		{
			// for the compartment level graph, we simply invoke the base class method...
			return CCTAChart::GetFlashLabelValue(hXrtData, iCurFlashSet, iCurFlashPt, bAccumulate);
		}
	}
	else if ( (GetCurChartData()->GetGraphType() == CCTAChartData::eBkt) ) // bucket level graph
	{
		// we gotta do some accumulatin'...
		double dFlashValue = 0;
		double dThisValue = 0;
		// accumulate all sets for this point...
		for( int iThisSet = 0; iThisSet <= iCurFlashSet; iThisSet++ )
		{
			dThisValue = XrtDataGetYElement( hXrtData, iThisSet, iCurFlashPt );
			if ( dThisValue > 0 )
				dFlashValue = __max( dFlashValue, 0 );
			dFlashValue += dThisValue;
		}
		return dFlashValue;
	}
	else // recette
	{
		// we gotta do some accumulatin'...
		double dFlashValue = 0;
		double dThisValue = 0;
		dFlashValue = XrtDataGetYElement( hXrtData, iCurFlashSet, iCurFlashPt );
		return dFlashValue;
	}
}

/////////////////////////////////////////////////////////////////////////////
// COthLegsHelp dialog

COthLegsHelp::COthLegsHelp(COthLegsChart* pParent /*=NULL*/)
: CCTAChartHelpDlg(pParent)
{
}

void COthLegsHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COthLegsHelp)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COthLegsHelp, CCTAChartHelpDlg)
	//{{AFX_MSG_MAP(COthLegsHelp)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////
// COthLegsHelp message handlers

// Build the 1st title
//
void COthLegsHelp::GetTitle1( CString& szTitle )
{
	CHelpDlgRecArray* paTemp = NULL;
	YmIcOthLegsDom* pOthLegs = NULL;

	paTemp = GetParentChart()->GetHelpDlgCmptArray();
	int iIdx = __min( __max(m_iCurSelectPt,0), paTemp->GetUpperBound() );  // > 0 && < max idx
	if ( iIdx < paTemp->GetSize() )
		pOthLegs = paTemp->GetAt( iIdx );

	CString sss;
	sss.Format( ":  %s - %s", 
		pOthLegs->LegOrig(), pOthLegs->LegDest() );  // dlg window title 1

	CCTAChartData* pCTAChartData = GetParentChart()->GetCurChartData();
	if (pCTAChartData->GetGraphType() == CCTAChartData::eCmpt)
		szTitle.Format ("Resa %s", sss);
	else if (pCTAChartData->GetGraphType() == CCTAChartData::eBkt)
		szTitle.Format ("Dispo %s", sss);
	else
		szTitle = sss;
}

// Build the 2nd title
//
void COthLegsHelp::GetTitle2( CString& szTitle )
{
	CString szTitleMem = "";
	CHelpDlgRecArray* paTemp = NULL;
	YmIcOthLegsDom* pOthLegs = NULL;

	paTemp = GetParentChart()->GetHelpDlgCmptArray();
	int iIdx = __min( __max(m_iCurSelectPt,0), paTemp->GetUpperBound() );  // > 0 && < max idx
	if ( iIdx < paTemp->GetSize() )
		pOthLegs = paTemp->GetAt( iIdx );

	szTitleMem += ((CCTAApp*)APP)->GetResource()->LoadResString(GetParentView()->GetAltFlag() ? IDS_SOUS_CONT : IDS_CMPT);
	szTitleMem += " ";
	szTitleMem += pOthLegs->Cmpt();

	CCTAChartData* pCTAChartData = GetParentChart()->GetCurChartData();
	if (pCTAChartData->GetGraphType() == CCTAChartData::eRecettes)
	{
		int iRevenu = GetParentView()->CalcRevenu( pOthLegs );
		szTitle.Format( "%s: %s %d",szTitleMem,"Recette :",iRevenu);
	}
}

/* DM 5350 Permet de retourner True si le type de graphe courant est eRecette*/
BOOL COthLegsHelp::IsRecetteDataSet ()
{
	CCTAChartData* pCTAChartData = GetParentChart()->GetCurChartData();

	if (pCTAChartData->GetGraphType() == CCTAChartData::eRecettes)
		return true;
	else return false;
}

// Get the longest label to be printed.  This is used for sizing the width
//
CSize COthLegsHelp::GetExtentLongestLabel( CPaintDC& dc )
{
	// loop through all the labels, and find the longest one (varies from language to language)
	int iLongest = 0;
	CString szTextLabel;

	// must loop through the enumerated variables representing sets of data
	CSize sizeTextLabel(0, 0);
	CCTAChartData* pCTAChartData = GetParentChart()->GetCurChartData();  // get graph type

	for( int i = 0; i < GetParentChart()->GetHelpDlgLblArray()->GetSize(); i++ )
	{
		if ( i < GetParentChart()->GetHelpDlgLblArray()->GetSize() )
			szTextLabel = GetParentChart()->GetHelpDlgLblArray()->GetAt( i );
		sizeTextLabel = dc.GetOutputTextExtent( szTextLabel );  // get the text extent
		if ( sizeTextLabel.cx > iLongest )
			iLongest = sizeTextLabel.cx;
	}
	sizeTextLabel.cx = iLongest;
	return sizeTextLabel;
}


CString COthLegsHelp::GetValue( int iDataSet, int iCurSelectSet, int iCurSelectPt  )
{
	CString szValue;
	double dValue = 0;
	CCTAChartData* pCTAChartData = GetParentChart()->GetCurChartData();  // get graph type

	if ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt )
	{
		if ( iDataSet == XRT_DATA )
		{
			dValue = XrtDataGetYElement( m_hXrtData, iCurSelectSet, m_iCurSelectPt );
		}
		else  // iDataSet == XRT_DATA2
		{
			// because it is a stacked bar, we must accumulate the pieces of the bar...
			for (int iSet = 0; iSet <= iCurSelectSet; iSet++)
				dValue += XrtDataGetYElement( m_hXrtData2, iSet, m_iCurSelectPt );
		}
		szValue.Format( "%3.0f", dValue );
	}
	else if ( pCTAChartData->GetGraphType() == CCTAChartData::eBkt )
	{
		if ( iDataSet == XRT_DATA )
		{
			if (((COthLegsView*)m_pParent->GetParentView())->GetAltFlag())
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
			// because it is a stacked bar, we must accumulate the pieces of the bar...
			for (int iSet = 0; iSet <= iCurSelectSet; iSet++)
				dValue += XrtDataGetYElement( m_hXrtData2, iSet, m_iCurSelectPt );
		}
		szValue.Format( "%3.0f", dValue );
	}
	else // Recette
	{
		dValue = XrtDataGetYElement( m_hXrtData, iCurSelectSet, m_iCurSelectPt );
		szValue.Format( "%3.0f", dValue );
	}
	return szValue;
}


// Get the text label to be drawn
////methode remplacee par GetTextLabelSphere
BOOL COthLegsHelp::GetTextLabel( int iDataSet, CString& szTextLabel, int iCurSelectSet )
{
	szTextLabel.Empty();

	CCTAChartData* pCTAChartData = GetParentChart()->GetCurChartData();  // get graph type
	if ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt )
	{
		if ( iDataSet == XRT_DATA2 )  // this is an artificial adjustment from the way things are normally done...
			iCurSelectSet += ( GetParentView()->eNumberDataSets  );  // adjust for the offset because array is contiguous

		// make sure we don't go too far...
		if ( iCurSelectSet >= GetParentChart()->GetHelpDlgLblArray()->GetSize() )
			return FALSE;

		szTextLabel = GetParentChart()->GetHelpDlgLblArray()->GetAt( iCurSelectSet );
		
		return TRUE;
	}
	else  // bucket level graph
	{
		if ( iDataSet == XRT_DATA )
		{
			CCmptBktLabelArray* aLabels = GetParentView()->GetCmptBktLabelArray();
			if ( m_iCurSelectPt >= aLabels->GetSize() )
				return FALSE;
			
			CmptBktLabels* pLabel = aLabels->GetAt( m_iCurSelectPt );
			if ( iCurSelectSet >= pLabel->aszBktArray.GetSize() )
				return FALSE;
			
			//if the bucket label is all numeric, we display the compartment with the bucket label,
			//else we display only the bucket label
			szTextLabel = pLabel->aszBktArray.GetAt( iCurSelectSet );
			BOOL bIsNumeric = TRUE;
			for (int i=0; i<szTextLabel.GetLength(); i++)
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
	return FALSE;
}

//DM7978 KGH
//remplace GetTextLabel
BOOL COthLegsHelp::GetTextLabelSphere( int iDataSet, CString& szTextLabel, int iCurSelectSet )
{
	szTextLabel.Empty();

	CCTAChartData* pCTAChartData = GetParentChart()->GetCurChartData();  // get graph type
	if ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt )
	{
		if ( iDataSet == XRT_DATA2 )  // this is an artificial adjustment from the way things are normally done...
			iCurSelectSet += ( GetParentView()->eNumberDataSets  );  // adjust for the offset because array is contiguous

		// make sure we don't go too far...
		if ( iCurSelectSet >= GetParentChart()->GetHelpDlgLblArray()->GetSize() )
			return FALSE;

		szTextLabel = GetParentChart()->GetHelpDlgLblArray()->GetAt( iCurSelectSet );
		return TRUE;
	}
	else  // bucket level graph
	{
		if ( iDataSet == XRT_DATA )
		{
			CmptSphereLabelArray aLabels = ((CCTAApp*)APP)->GetDocument()->m_sphereManager->getLabelSphereArray((GetParentView()->GetAltFlag() != 0), OTH_LEGS);
			if ((unsigned)m_iCurSelectPt >= aLabels.size())
				return FALSE;
			SphereLibelleMap pLabel = aLabels.at(m_iCurSelectPt);
			if ((unsigned)iCurSelectSet >= pLabel.size())
				return FALSE;
			//recuperer le comartment de l'histo selectionne et le comparer avec celui du label
			// get the text of the tab to be used in the 2nd title
			CHelpDlgRecArray* paTemp = NULL;
			YmIcOthLegsDom* pOthLegs = NULL;
			paTemp = GetParentChart()->GetHelpDlgCmptArray();
			int iIdx = __min( __max(m_iCurSelectPt,0), paTemp->GetUpperBound() );  // > 0 && < max idx
			if ( iIdx < paTemp->GetSize() )
				pOthLegs = paTemp->GetAt( iIdx );
			CString szCmptCurent = pOthLegs->Cmpt();  // compartment de l'histo selectionne
			szCmptCurent.MakeUpper();

			CmptLibelle cmptLibelle = pLabel[iCurSelectSet];
			CString szCmptDuLabel = cmptLibelle.sCmpt;
			if(szCmptDuLabel.Find(szCmptCurent) == -1)//les cmpt sont differents du coup on ne dessine pas ce label
				return FALSE;
			szTextLabel = cmptLibelle.sLibelle;
		}
		else  //  ( iDataSet == XRT_DATA2 )
		{
			szTextLabel = XrtGetNthSetLabel2( m_pParent->m_hChart, iCurSelectSet );
		}
		return TRUE;
	}
	return FALSE;
}
//DM7978 KGH FIN

// Translate the dataset to a cta graph style (color)
//
int COthLegsHelp::GetCTADataStyleIndex( int iDataSet, int iCurSelectSet )
{
	CCTAChartData* pCTAChartData = GetParentChart()->GetCurChartData();
	if ( pCTAChartData == NULL )
		return -1;

	if ( iDataSet == XRT_DATA )
	{
		if ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt )
		{
			if ( iCurSelectSet == GetParentView()->eAuthDptFcst )
				return CCTAChart::eAuthStyleIdx;
			if ( iCurSelectSet == GetParentView()->eDmdDptFcst )
				return CCTAChart::eDmdDptFcstStyleIdx;
			if ( iCurSelectSet == GetParentView()->eResHoldTot )
				return CCTAChart::eResHoldTotStyleIdx;
		}
		else  // bucket level graph
		{
			// DM 7879 KGH	//retourner l'index dans le tableau m_paSphereDataStyles
			//return CCTAChart::eBucket0StyleIdx + iCurSelectSet;
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
	}
	else  // iDataSet == XRT_DATA2
	{
		if ( iCurSelectSet == GetParentView()->eCapacity )
			return CCTAChart::eCapStyleIdx;
		if ( iCurSelectSet == GetParentView()->eCapPlusUnres )
			return CCTAChart::eUnreservableCapStyleIdx;
		if ( iCurSelectSet == GetParentView()->eCapPlusStdgPlusUnres )
			return CCTAChart::eStandingCapStyleIdx;
	}
	return -1;
}

// DM 7879 KGH reimplementation de la methode  de la classe CCTAChartHelpDlg
BOOL COthLegsHelp::DrawThisLabel( int iDataSet, CPaintDC& dc, int iCurSelectSet, CPoint& pointDraw, int iLeftMargin, int iRightMargin, BOOL bWithCheckBox)
{
	//  get the index which points to the style for which we draw the color-coded rectangle...
	//  note that if this method returns -1, we get out immediately because we don't want to deal with it
	int iCtaDataStyleIdx = GetCTADataStyleIndex( iDataSet, iCurSelectSet );  // virtual method
	if ( iCtaDataStyleIdx < 0 )
		return FALSE;  // we report that we didn't draw anything

	// get the text label to be displayed
	CString szTextLabel;
	//DM7978 KGH remplacer la methode GetTextLabel par GetTextLabelSphere
	//if ( ! GetTextLabel( iDataSet, szTextLabel, iCurSelectSet ) )  // virtual method
	// NPI - Ano 70148
	/*if ( ! GetTextLabelSphere( iDataSet, szTextLabel, iCurSelectSet ) )  
		//DM7978 KGH
		return FALSE;*/

	if (((CCTAApp*)APP)->GetRhealysFlag())
	{
		if (!GetTextLabel(iDataSet, szTextLabel, iCurSelectSet))
			return FALSE;
	}
	else
	{
		if (!GetTextLabelSphere( iDataSet, szTextLabel, iCurSelectSet))
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
	CCTAChartData* pCTAChartData = GetParentChart()->GetCurChartData();
	if ( pCTAChartData == NULL )
		return FALSE;

	XrtDataStyle* style = NULL;
	if (iDataSet == XRT_DATA)	//recuperer le style des spheres dans le tableau m_paSphereDataStyles
	{
		if ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt )//si onglet Esp Phys (eCmpt) recuperer le style associe
		{
			style =  m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx);
		}
		else //Ebkt || eRecettes: recuperer le style des spheres
		{
			// NPI - Ano 70148
			//style = GetParentChart()->GetSphereDataStylesAt(iCtaDataStyleIdx);
			if (((CCTAApp*)APP)->GetRhealysFlag())
			{
				style =  m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx);
			}
			else
			{
				style = GetParentChart()->GetSphereDataStylesAt(iCtaDataStyleIdx);
			}
			///
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

void COthLegsHelp::OnPaint()
{
	// paint all text and graphics on the client area of the dialog
	//
	CPaintDC dc(this); // device context for painting
	CFont* pOldFont = dc.SelectObject( GetHelpTextFont() );

	int iAugCap = 1;  // 1 depending upon additonal capacity flag

	// set the initial size of the help dialog window
	// note: we do not count the white space under the capacity portion of the bar
	CCTAChartData* pCTAChartData = GetParentChart()->GetCurChartData();
	if ( pCTAChartData == NULL )
		return;
	// DM7978 KGH adapter la fenetre au nb max de spheres au lieu du nb max bucket
	//int iNumLabels = ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt ) ? GetParentView()->eNumberDataSets + iAugCap : GetParentView()->GetMaxBucketCount() + iAugCap;
	// NPI - Ano 70148
	/*int iMaxSpheresPrix = GetParentView()->GetMaxSpheresPrixLegende();
	int iNumLabels = ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt ) ? GetParentView()->eNumberDataSets + iAugCap : iMaxSpheresPrix + iAugCap;*/

	int iNumLabels = 0;
	if (((CCTAApp*)APP)->GetRhealysFlag())
	{
		iNumLabels = ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt ) ? GetParentView()->eNumberDataSets + iAugCap : GetParentView()->GetMaxBucketCount() + iAugCap;
	}
	else
	{
		iNumLabels = ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt ) ? GetParentView()->eNumberDataSets + iAugCap : GetParentView()->GetMaxSpheresPrixLegende() + iAugCap;
	}
	// DM7978 KGH FIN

	InitHelpWin( iNumLabels );

	// draw the titles and return the point where drawing is to continue
	CPoint pointDraw = DrawTitles( dc );

	// draw the labels and the corresponding values
	if (pCTAChartData->GetGraphType() == CCTAChartData::eCmpt)
		DrawLabels( dc, pointDraw );
	else
		DrawLabels( dc, pointDraw, TRUE );  // draw the labels in reverse order for the buckets

	// Do not call CDialog::OnPaint() for painting messages
}

