// HistDptView.cpp : implementation file
//
#include "StdAfx.h"


#include "CTAChartHelpDlg.h"
#include "HistDptView.h"
#include "TrancheView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Stockage des données resa de l'année précédente
CYavData::CYavData (unsigned long date, const char* cmpt, long resa, long auth, long rev)
{
  m_date = date;
  m_cmpt = cmpt[0];
  if (m_cmpt > 'Z')
	m_cmpt -= 32; // en minuscule, on le passe en majuscule.
  m_resa = resa;
  m_auth = auth;
  m_rev  = rev;
}


CYavArray::CYavArray (BOOL bAlt)
{
  YM_Iterator<YmIcHistDptDom>* pVariablesIterator =
        YM_Set<YmIcHistDptDom>::FromKey(bAlt ? HISTDPT_YAV_ALT_KEY : HISTDPT_YAV_KEY)->CreateIterator();
  YmIcHistDptDom* pHistoDpt = NULL;
  long rev;
  CYavData* pYav = NULL;
  for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
  {
    pHistoDpt = pVariablesIterator->Current();
	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRevenuFlag())
	{
	  rev = pHistoDpt->Revenu();
	  if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag())
		rev += pHistoDpt->RevenuGaranti();
	}
	else
	  rev = 0;
	pYav = new CYavData (pHistoDpt->DptDate(), pHistoDpt->Cmpt(), pHistoDpt->ResHoldDptTot(), pHistoDpt->AuthDpt(), rev);
	m_DataArray.Add ((CObject*)pYav); 
  }
  delete pVariablesIterator;
  if (!m_DataArray.GetCount())
    return;
}

CYavArray::~CYavArray()
{
  int i=0;
  CYavData* pYav;
  for (i = 0; i < m_DataArray.GetSize(); i++)
  {
    pYav = (CYavData*)m_DataArray.GetAt(i);
    delete pYav;
  }
  m_DataArray.RemoveAll();
}

// date et cmpt corresponde au point du graphique pour lequel on veut les données de l'année précédente.
CYavData* CYavArray::GetPrevYearData (unsigned long date, char cmpt)
{
  int i=0;
  CYavData* pYav;
  CYavData* pCeYav;
  long dist;
  long bestdist = 600;
  pCeYav = NULL;
  for (i = 0; i < m_DataArray.GetSize(); i++)
  {
    pYav = (CYavData*)m_DataArray.GetAt(i);
	if (cmpt > 'Z')
	  cmpt -= 32; // en minuscule, on le passe en majuscule.
	if (pYav->m_cmpt == cmpt)
	{
	  dist = date - pYav->m_date - 365;
	  dist = abs(dist); 
	  if (dist < bestdist)
	  {
	    pCeYav = pYav;
		bestdist = dist;
	  }
	}
  }
  return pCeYav;
}



/////////////////////////////////////////////////////////////////////////////
// CHistDptAltView

IMPLEMENT_DYNCREATE(CHistDptAltView, CHistDptView)

CHistDptAltView::CHistDptAltView()
:CHistDptView()
{
}

void CHistDptAltView::OnInitialUpdate() 
{
  ((CChildFrame*)GetParentFrame())->SetAltFlag(TRUE);

	CHistDptView::OnInitialUpdate();  // base class methode
}

/////////////////////////////////////////////////////////////////////////////
// CHistDptView

IMPLEMENT_DYNCREATE(CHistDptView, CView)

CHistDptView::CHistDptView()
{
  // set this view as the owner view in the chart object
  m_Chart.SetParentView(this); 
  m_Chart.SetCurScaling(((CCTAApp*)APP)->m_ScaleHDpt);
  m_Chart.SetRotateLabelsFlag( ((CCTAApp*)APP)->m_bHDptRotatelabel );
  m_Chart.SetLabelInterval( ((CCTAApp*)APP)->m_HDptInterval);

  m_bTabResized = FALSE;
  /* split */
  _nSubViewID = 0;
  /* split */

 

  m_bHistDptSetValid = FALSE;			 
  m_bHistDptYavSetValid = FALSE;	 

  m_pYavData = NULL;
}

CHistDptView::~CHistDptView()
{
  m_Chart.ClearChartDataArray();  // delete all graph datasets
  if (m_pYavData)
    delete m_pYavData;
}

BEGIN_MESSAGE_MAP(CHistDptView, CView)
	//{{AFX_MSG_MAP(CHistDptView)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_HISTDPT_TABCTRL, OnChartSelchange)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_SCALE_MAX, OnScaleMax)
	ON_UPDATE_COMMAND_UI(ID_SCALE_MAX, OnUpdateScaleMax)
	ON_COMMAND(ID_SCALE_AUTHORIZATIONS, OnScaleAuth)
	ON_UPDATE_COMMAND_UI(ID_SCALE_AUTHORIZATIONS, OnUpdateScaleAuth)
	ON_COMMAND(ID_SCALE_CAPACITY, OnScaleCap)
	ON_UPDATE_COMMAND_UI(ID_SCALE_CAPACITY, OnUpdateScaleCap)
	ON_COMMAND(ID_GRAPH_FLASH_LABEL,OnGraphDisplayFlashLabels)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_FLASH_LABEL,OnUpdateGraphDisplayFlashLabels)
	ON_COMMAND(ID_GRAPH_ROTATE_LABELS,OnGraphRotateLabels)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_ROTATE_LABELS,OnUpdateGraphRotateLabels)
	ON_COMMAND(ID_GRAPH_LABEL_INTERVAL_1,OnGraphLabelInterval1)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_LABEL_INTERVAL_1,OnUpdateGraphLabelInterval1)
	ON_COMMAND(ID_GRAPH_LABEL_INTERVAL_5,OnGraphLabelInterval5)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_LABEL_INTERVAL_5,OnUpdateGraphLabelInterval5)
	ON_COMMAND(ID_GRAPH_LABEL_INTERVAL_10,OnGraphLabelInterval10)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_LABEL_INTERVAL_10,OnUpdateGraphLabelInterval10)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistDptView drawing

void CHistDptView::OnDraw(CDC* pDC)
{
	ASSERT_VALID(GetDocument());

	YM_RecordSet* pHistDptSet = 
    YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );

  // if record set is valid and we have data 
  if( pHistDptSet->IsValid() && m_Chart.GetChartDataArray()->GetSize() )
  {
  }
  else
  {
  	RECT rect;
    CPen* pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
    CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(LTGRAY_BRUSH);
	  GetClientRect(&rect);
	  pDC->Rectangle(&rect);
    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
  }
}

void CHistDptView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

    int iCurSelTabIdx;    // get currently selected tab
    if ( ( iCurSelTabIdx = m_ChartTabCtrl.GetCurSel() ) < 0 )
        return;
    
    int iSizeCmpt = m_Chart.GetChartDataArray()->GetSize()/2;
    COLORREF clrBk;
	COLORREF clrFt;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag()  && iCurSelTabIdx < iSizeCmpt)
    {
	  clrBk = (GetAltFlag()) ? 
	  ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextRecBkAlt() : 
	  ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextRecBkMain();
	  m_Chart.SetBackgroundColor (clrBk);
	  clrFt = (GetAltFlag()) ? 
	  ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkAlt() : 
	  ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();
      m_Chart.SetForegroundColor (clrFt);
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

/////////////////////////////////////////////////////////////////////////////
// CHistDptView diagnostics

#ifdef _DEBUG
void CHistDptView::AssertValid() const
{
	CView::AssertValid();
}

void CHistDptView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCTADoc* CHistDptView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCTADoc)));
	return (CCTADoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHistDptView message handlers

int CHistDptView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CView::OnCreate(lpCreateStruct) == -1)
	return -1;
	
  // create and adjust the tab control
  CRect rect;
  GetClientRect(&rect);
  m_ChartTabCtrl.Create( CTA_TAB_STYLE, rect, this, IDC_HISTDPT_TABCTRL);
  GetClientRect(&rect);
  m_ChartTabCtrl.AdjustRect( FALSE, &rect );

  // create a chart window when the view is created
  BOOL bRet = m_Chart.Create("", WS_CHILD | WS_VISIBLE, rect, this, 0);

  // set some chart properties
  return 0;
}


void CHistDptView::FrameSizeChange (UINT nType, int cx, int cy)
{
  YM_RecordSet* pHistDptSet = 
    YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );

	ASSERT( pHistDptSet != NULL );

  // if window has been minimized, we must destroy the help dialogs
  if ( nType == SIZE_MINIMIZED )
    m_Chart.DestroyHelpDlg();

  if ((nType == SIZE_MINIMIZED) && pHistDptSet->IsOpen())
	pHistDptSet->Close();
  else if ((nType != SIZE_MINIMIZED) && !pHistDptSet->IsOpen())
	pHistDptSet->Open();

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
	   ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHistoDepLastYearFlag())
  {
    YM_RecordSet* pHistDptYavSet = YM_Set<YmIcHistoEvolDom>::FromKey( (GetAltFlag()) ? HISTDPT_YAV_ALT_KEY : HISTDPT_YAV_KEY );
    if ((nType == SIZE_MINIMIZED) && pHistDptYavSet->IsOpen())
	  pHistDptYavSet->Close();
    else if ((nType != SIZE_MINIMIZED) && !pHistDptYavSet->IsOpen())
	  pHistDptYavSet->Open();
  } 	
}

void CHistDptView::OnShowWindow(BOOL bShow, UINT nStatus)
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

/* Split */
CHistDptView* CHistDptView::getOtherView()
{
    CWnd* parent = GetParent();
    
    for (CWnd* child = parent->GetWindow( GW_CHILD ); child; child = child->GetWindow( GW_HWNDNEXT )) {
	if (child->m_hWnd != m_hWnd && child->GetRuntimeClass() == GetRuntimeClass())
	    return (CHistDptView*)child;
    }
    return NULL;
}
/* split */

void CHistDptView::OnInitialUpdate() 
{
	 // initialize the enumeration variables for chart 1 (XRT_DATA)
  int iOffset = 0;
  if (!GetAltFlag())
    eAuthDpt = iOffset++;
  else
    eAuthDpt = -1;
  eCapacity = iOffset++;
  eDmdDptFcst = -1;
  eResHoldDptTot = iOffset++;
  if (!GetAltFlag())
    eTrafficRes = iOffset++;
  else
    eTrafficRes = -1;
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
	   ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHistoDepLastYearFlag())
  {
    eYavResHold = iOffset++;
    eYavAuthDpt = iOffset++;
  }
  else
  {
    eYavResHold = -1;
    eYavAuthDpt = -1;
  }
  eNumberDataSets = iOffset;

  iOffset = 0;
  eRecette = iOffset++;
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
	   ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHistoDepLastYearFlag())
    eYavRecette = iOffset++;
  else
    eYavRecette = -1;
  eNumberRecetteDataSets = iOffset;

    CView::OnInitialUpdate();
    if (getOtherView())
	   _nSubViewID = 1;

    CString titi, toto;
    titi = ((CCTAApp*)APP)->GetResource()->LoadResString((GetAltFlag()) ? IDS_HISTDPT_SCI_TITLE : IDS_HISTDPT_TITLE);
    GetParentFrame()->GetWindowText(toto);
    GetParentFrame()->SetWindowText(titi);
    ((YM_PersistentChildWnd*)GetParentFrame())->SetChildKey (toto);
    /* Split */
    if (!_nSubViewID)
	((YM_PersistentChildWnd*)GetParentFrame())->SetChildView (this);
    else {
	CString szProfileKey;
	((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);
	_sLastTabUsed = APP->GetProfileString(szProfileKey, "LastTabUsed_2", "" );
    }
	/* Split */

    // Ask for notification from this RecordSet   
	YM_RecordSet* pHistDptSet = YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );
    ASSERT( pHistDptSet != NULL );
	pHistDptSet->AddObserver( this );
    if (!pHistDptSet->IsOpen())
	  pHistDptSet->Open();
	OnUpdate( this, 0L, pHistDptSet );

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
		 ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHistoDepLastYearFlag())
    {
      YM_RecordSet* pHistDptYavSet = YM_Set<YmIcHistoEvolDom>::FromKey( (GetAltFlag()) ? HISTDPT_YAV_ALT_KEY : HISTDPT_YAV_KEY );
      ASSERT( pHistDptYavSet != NULL );
	  pHistDptYavSet->AddObserver( this );
      if (!pHistDptYavSet->IsOpen())
	    pHistDptYavSet->Open();
	  OnUpdate( this, 0L, pHistDptYavSet );
	}
}

void CHistDptView::OnChartSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
  // get the index of the currently selected tab, and use it to
  //  access the array of tab index to data object
  if (m_Chart.GetChartDataArray()->GetSize() == 0)
    return;

  int iCurSelTabIdx;    // get currently selected tab
  if ( ( iCurSelTabIdx = m_ChartTabCtrl.GetCurSel() ) < 0 )
    return;

  LPTSTR psz = _sLastTabUsed.GetBuffer( 255 );
  TC_ITEM TabCtrlItem;
  TabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
  TabCtrlItem.pszText = psz;
  TabCtrlItem.cchTextMax = 255;
  GetTabCtrl()->GetItem( iCurSelTabIdx, &TabCtrlItem );
  _sLastTabUsed.ReleaseBuffer();

  // set the appropriate data object as currently selected, then get it
  m_Chart.SetCurChartData( iCurSelTabIdx );
  CCTAChartData* pChartData = m_Chart.GetCurChartData();
  if ( pChartData == NULL )   return;

  int iSizeCmpt = m_Chart.GetChartDataArray()->GetSize()/2;

  COLORREF clrBk;
  COLORREF clrFt;
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag()  && iCurSelTabIdx < iSizeCmpt)
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
  else
  {
      clrBk = (GetAltFlag()) ? 
	    ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt() : 
	    ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();
	  m_Chart.SetBackgroundColor (clrBk);
	  m_Chart.SetForegroundColor (RGB(0,0,0));   // noir
      m_Chart.SetYTitle("");
  }

  ShowGraph( pChartData->GetDataObj() );  // show the newly selected chart

  m_Chart.RemoveFlashLabel();  // remove flash label if up

  // force a redraw of the help dialog if up
  CCTAChartHelpDlg* pChartHelpDlg = m_Chart.GetHelpDlg();
  if ( ( pChartHelpDlg ) && ( IsWindow( pChartHelpDlg->m_hWnd ) ) && ( pChartHelpDlg->GetSafeHwnd() ) )
  {
	  m_Chart.GetHelpDlg()->ClearHelpWin();
	  m_Chart.GetHelpDlg()->Invalidate(FALSE);
	  m_Chart.GetHelpDlg()->UpdateWindow();
  }

	*pResult = 0;
}

void CHistDptView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (!pHint)
	  return;

	YM_RecordSet* pHistDptSet = 
    YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );
	
    BOOL bYRev = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag();
	BOOL bHist = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
		         ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHistoDepLastYearFlag();
	YM_RecordSet* pHistDptYavSet = NULL;
	if ( bHist )
      pHistDptYavSet = YM_Set<YmIcHistoEvolDom>::FromKey( (GetAltFlag()) ? HISTDPT_YAV_ALT_KEY : HISTDPT_YAV_KEY );

	if (pHint == pHistDptSet)
	{
	  m_bHistDptSetValid = pHistDptSet->IsValid();
	  if (!pHistDptSet->IsValid())
	  { // nettoyage, fait sur etat invalide de la requete principale
	    m_Chart.DestroyHelpDlg();
	    EnableWindow( FALSE );
		YM_PersistentChildWnd* pParentFrame = (YM_PersistentChildWnd*)GetParentFrame();
        if ( pParentFrame == NULL)
          return;
		if ( ! pParentFrame->OnDeletion() )
          ClearGraph();
	  }
	}
	else if (pHint == pHistDptYavSet)
	  m_bHistDptYavSetValid = pHistDptYavSet->IsValid();

	// If notification comes from a RecordSet, pHint is a RecordSet*
	if ( m_bHistDptSetValid && (m_bHistDptYavSetValid || !bHist))
	{
	  if (m_bHistDptYavSetValid)
	  {
	    if (m_pYavData)
	      delete m_pYavData;
	    m_pYavData = new CYavArray(GetAltFlag());
	  }

      EnableWindow( TRUE );  
      ClearGraph();	// clean up the previous data structure(s)	
      if (bYRev && !((CCTAApp*)APP)->GetRhealysFlag())//DM8029 - YI-11703: do not display recette graphs on ALLEO 
	    BuildGraphRecettes();

      CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();

	  // NBN - Ano 92249
	  if(!((CCTAApp*)APP)->GetRhealysFlag() || (!GetAltFlag() || pTrancheView->getCurrentTrancheNature() == 3)){
		  BuildGraph();
		  BuildTabs();
	  }else{
		ClearGraph();
		((CChildFrame*)GetParentFrame())->GetStatusBar()->SetPaneOneAndLock(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA) );
	  }

      ResizeChart();
    }
}

void CHistDptView::OnSize(UINT nType, int cx, int cy) 
{
    CView::OnSize(nType, cx, cy);

    ResizeChart();
}

void CHistDptView::OnDestroy() 
{
    CView::OnDestroy();
    /* Split */
	YM_PersistentChildWnd* pParentFrame = (YM_PersistentChildWnd*)GetParentFrame();
    if ( pParentFrame == NULL)
        return;

  // if we have another CHistDptView instance, set it as the child view of the parent frame !
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CView* pView; 
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
    // do we have a spliiter window activated (i.e., 2 views ?)
    if( pView->IsKindOf( RUNTIME_CLASS(CHistDptView) ) && pView != this ) 
    {
	  CHistDptView* histdptview = (CHistDptView*) pView;
      pParentFrame->SetChildView (histdptview);
      break;
    }
  }
 // If this destroy is not consecutive to the Frame Destroy, this
  // means that we are simply closing one pane of the splitter window
  // So we don't have to delete the recordsets
  if ( ! pParentFrame->OnDeletion() )
  {
    return;
  }
  CHistDptView* pOtherView = getOtherView();

    /* Split */
    YM_RecordSet* pHistDptSet = YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );
    ASSERT( pHistDptSet != NULL );
	pHistDptSet->RemoveObserver (this);
	if (pOtherView)
	  pHistDptSet->RemoveObserver (pOtherView);
    pHistDptSet->Close();

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
		 ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHistoDepLastYearFlag())
    {
      YM_RecordSet* pHistDptYavSet = YM_Set<YmIcHistoEvolDom>::FromKey( (GetAltFlag()) ? HISTDPT_YAV_ALT_KEY : HISTDPT_YAV_KEY );
      ASSERT( pHistDptYavSet != NULL );
	  pHistDptYavSet->RemoveObserver (this);
	  if (pOtherView)
	    pHistDptYavSet->RemoveObserver (pOtherView);
      pHistDptYavSet->Close();
	}
}

// renvoi la date de l'année précédente ou 0. bResa indique si on demande pour les resa ou les recettes
unsigned long CHistDptView::GetYavDate (unsigned long date, char cmpt)
{
  unsigned long rval = 0;
  if (m_pYavData)
  {
    CYavData* pYavData;
	pYavData = m_pYavData->GetPrevYearData (date, cmpt);
	if (pYavData)
	  rval = pYavData->m_date;
  }
  return rval;
}

void CHistDptView::OnScale (enum CCTAChart::GraphScaleIndex eGraphScale)
{
  m_Chart.ScaleGraph(eGraphScale);
  ((CCTAApp*)APP)->m_ScaleHDpt = eGraphScale;
  Invalidate(FALSE);  // force a redraw
  UpdateWindow();
} 

void CHistDptView::OnScaleMax() 
{
    OnScale(CCTAChart::eMaxScaleIdx);
	CHistDptView* pView = getOtherView();
    if (pView)
		pView->OnScale(CCTAChart::eMaxScaleIdx);
}

void CHistDptView::OnUpdateScaleMax(CCmdUI* pCmdUI) 
{
    ASSERT_VALID(GetDocument());
    YM_RecordSet* pHistDptSet = YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );
    ASSERT( pHistDptSet != NULL );

	BOOL bRec = FALSE;
    CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
    if (pCTAChartData && (pCTAChartData->GetGraphType() == CCTAChartData::eRecettes))
    bRec = TRUE;

    pCmdUI->Enable( pHistDptSet->IsOpen() );
    BOOL bCheckIt = ( m_Chart.GetCurScaling() == CCTAChart::eMaxScaleIdx );
    pCmdUI->SetCheck( bRec || bCheckIt );
}

void CHistDptView::OnScaleAuth() 
{
    OnScale(CCTAChart::eAuthScaleIdx);
	CHistDptView* pView = getOtherView();
    if (pView)
	  pView->OnScale(CCTAChart::eAuthScaleIdx);
}

void CHistDptView::OnUpdateScaleAuth(CCmdUI* pCmdUI) 
{
    ASSERT_VALID(GetDocument());
    YM_RecordSet* pHistDptSet = YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );
    ASSERT( pHistDptSet != NULL );

	BOOL bRec = FALSE;
    CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
    if (pCTAChartData && (pCTAChartData->GetGraphType() == CCTAChartData::eRecettes))
    bRec = TRUE;

    pCmdUI->Enable( !bRec && pHistDptSet->IsOpen() );
    BOOL bCheckIt = ( m_Chart.GetCurScaling() == CCTAChart::eAuthScaleIdx );
    pCmdUI->SetCheck( !bRec && bCheckIt );
}

void CHistDptView::OnScaleCap() 
{
	OnScale(CCTAChart::eCapScaleIdx);
	CHistDptView* pView = getOtherView();
    if (pView)
	  pView->OnScale(CCTAChart::eAuthScaleIdx);
}

void CHistDptView::OnUpdateScaleCap(CCmdUI* pCmdUI) 
{
    ASSERT_VALID(GetDocument());
    YM_RecordSet* pHistDptSet = YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );
    ASSERT( pHistDptSet != NULL );

	BOOL bRec = FALSE;
    CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
    if (pCTAChartData && (pCTAChartData->GetGraphType() == CCTAChartData::eRecettes))
    bRec = TRUE;

    pCmdUI->Enable( !bRec && pHistDptSet->IsOpen() );
    BOOL bCheckIt = ( m_Chart.GetCurScaling() == CCTAChart::eCapScaleIdx );
    pCmdUI->SetCheck( !bRec && bCheckIt );
}

void CHistDptView::OnGraphDisplayFlashLabels() 
{
    // toggle the flag to display the graph flash labels
    ((CCTAApp*)APP)->SetFlashLabelFlag(  ! ((CCTAApp*)APP)->GetFlashLabelFlag() );
}

void CHistDptView::OnUpdateGraphDisplayFlashLabels(CCmdUI* pCmdUI) 
{
    YM_RecordSet* pHistDptSet = YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );
    ASSERT( pHistDptSet != NULL );
    pCmdUI->Enable( pHistDptSet->IsOpen() );
    pCmdUI->SetCheck( ((CCTAApp*)APP)->GetFlashLabelFlag() );
}

void CHistDptView::OnGraphRotateLabels()
{
	bool brot = !m_Chart.GetRotateLabelsFlag();
	((CCTAApp*)APP)->m_bHDptRotatelabel = brot;
    m_Chart.SetRotateLabelsFlag( brot );
    m_Chart.GetRotateLabelsFlag() == 1 ? 
    m_Chart.SetXAnnotationRotation(XRT_ROTATE_90) : 
    m_Chart.SetXAnnotationRotation(XRT_ROTATE_NONE);
    /* Split */
	CHistDptView* pView = getOtherView();
    if (pView)
    {    
	  pView->m_Chart.SetRotateLabelsFlag( brot );
      pView->m_Chart.GetRotateLabelsFlag() == 1 ? 
      pView->m_Chart.SetXAnnotationRotation(XRT_ROTATE_90) : 
      pView->m_Chart.SetXAnnotationRotation(XRT_ROTATE_NONE);
    }
	/* Split */

}

void CHistDptView::OnUpdateGraphRotateLabels( CCmdUI* pCmdUI )
{
    YM_RecordSet* pHistDptSet = YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );
    ASSERT( pHistDptSet != NULL );
    pCmdUI->Enable( pHistDptSet->IsOpen() );
    pCmdUI->SetCheck ( (m_Chart.GetRotateLabelsFlag()==1) ? 1 : 0 );
}

void CHistDptView::GraphLabelInterval(int itv)
{
    m_Chart.SetLabelInterval( itv );
	((CCTAApp*)APP)->m_HDptInterval = itv;
    m_Chart.SetRepaint( FALSE );
    BuildXLabels();
    m_Chart.SetRepaint( TRUE );

	CHistDptView* pView = getOtherView();
    if (pView)
    { 
	  pView->m_Chart.SetLabelInterval( itv );
      pView->m_Chart.SetRepaint( FALSE );
      pView->BuildXLabels();
      pView->m_Chart.SetRepaint( TRUE );
	}
}

void CHistDptView::OnGraphLabelInterval1()
{
    GraphLabelInterval(1);
}

void CHistDptView::OnUpdateGraphLabelInterval1( CCmdUI* pCmdUI )
{
    YM_RecordSet* pHistDptSet = YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );
    ASSERT( pHistDptSet != NULL );
    pCmdUI->Enable( pHistDptSet->IsOpen() );
    pCmdUI->SetCheck ( (m_Chart.GetLabelInterval()==1) ? 1 : 0 );
}

void CHistDptView::OnGraphLabelInterval5()
{
    GraphLabelInterval(5);
}

void CHistDptView::OnUpdateGraphLabelInterval5( CCmdUI* pCmdUI )
{
	YM_RecordSet* pHistDptSet = 
    YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );
	ASSERT( pHistDptSet != NULL );
  pCmdUI->Enable( pHistDptSet->IsOpen() );
  pCmdUI->SetCheck ( (m_Chart.GetLabelInterval()==5) ? 1 : 0 );
}

void CHistDptView::OnGraphLabelInterval10()
{
  GraphLabelInterval(10);
}

void CHistDptView::OnUpdateGraphLabelInterval10( CCmdUI* pCmdUI )
{
	YM_RecordSet* pHistDptSet = 
    YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );
	ASSERT( pHistDptSet != NULL );
  pCmdUI->Enable( pHistDptSet->IsOpen() );
  pCmdUI->SetCheck ( (m_Chart.GetLabelInterval()==10) ? 1 : 0 );
}

///////////////////////////////////////////////////////
// Resize the chart window 
//
void CHistDptView::ResizeChart()
{
  // force a re-size of the chart to fit the window
	CRect rect;
  GetClientRect(&rect);
  m_ChartTabCtrl.MoveWindow(&rect);
  m_ChartTabCtrl.AdjustRect(FALSE, &rect);

  // have we any chart data ?
  if (m_Chart.GetChartDataArray()->GetSize())
  {
    // is there a rectangle ?
    if ( (rect.Width() > 0) && (rect.Height() > 10) )
    {
      m_Chart.MoveWindow(&rect);
    }
  }
}

///////////////////////////////////////////////////////
// Clear the data objects for the compartment graphs
//
void CHistDptView::ClearGraph()
{
	// clear the client area
	m_Chart.ShowWindow( SW_HIDE );

	Invalidate(FALSE);
	UpdateWindow();

  ClearTabs();  // delete the compartment entries from the tab array
  m_Chart.ClearChartDataArray();  // delete all graph datasets
/*
  // clear the header and footer
  char *szHeader[] = { "", NULL };
  m_Chart.SetHeaderStrings(szHeader);
*/
  char *szFooter[] = { "", NULL };
  m_Chart.SetFooterStrings(szFooter);

  // Set text in status bar control
  ((CChildFrame*)GetParentFrame())->
    GetStatusBar()->SetPaneOneAndLock(
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_READING_HISTORY) );
}

///////////////////////////////////////////////////////
// (Re)Build the graph's X-axis labels
//
void CHistDptView::BuildXLabels()
{
  CCTAChartData* pChartData = m_Chart.GetCurChartData();
  if ( pChartData == NULL )
    return;

  CStringArray* paszLabels = pChartData->GetXLabels();
  int iArraySize = paszLabels->GetSize();

  int iXLabelInterval = m_Chart.GetLabelInterval();
  CString szLabel;
  for (int iIdx = 0; iIdx < iArraySize; iIdx++)
  {
    // produce a dpt date label on X axis for every 'iIntervalConst' points, where n = iXLabelInterval
    if ( (iIdx % iXLabelInterval) == 0)
    {
      szLabel = paszLabels->GetAt( iIdx );
      m_Chart.SetNthPointLabel(iIdx, (char *)LPCTSTR(szLabel));  // set label for this point
    }
    else
    {
      m_Chart.SetNthPointLabel(iIdx, "");  // clear the label for this point
    }
  }
}

void CHistDptView::BuildGraphRecettes()
{
    YM_RecordSet* pHistDptRevSet = YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );
    BOOL bPrixGaranti = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag();

    // Define the iterator to iterate through compartments
    YM_Iterator<CHistDpt>* pCmptIterator = ((CHistDptSet*) pHistDptRevSet)->CreateKeyIterator();
    long lNumPts = pCmptIterator->GetCount();
    if (lNumPts > 0)
    {
      m_Chart.SetRepaint( FALSE );

      // Set text in status bar control
      ((CChildFrame*)GetParentFrame())->
       GetStatusBar()->SetPaneOneAndLock(
        ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BUILDING_GRAPH) );

      // we might need currently selected leg for leg orig/dest searching sys parm
      YmIcLegsDom* pLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
      YmIcHistDptDom* pHistDpt = NULL;

      // iterate through the compartments
      for( pCmptIterator->First(); !pCmptIterator->Finished(); pCmptIterator->Next() ) 
      {
	    YmIcHistDptDom* Current = pCmptIterator->Current();
        // Define the iterator to iterate through records for this compartment
		YM_Iterator<CHistDpt>* pCmptDateIterator =
		  ((CHistDptSet*) pHistDptRevSet)->CreateIterator( *(pCmptIterator->Current()->Cmpt()) );

        long lNumPts = pCmptDateIterator->GetCount();  // how many recs for this cmpt ?  // DM5350
        if (lNumPts > 0)
        {
		  // transfer the data from the record set to the graph data structure
		  // first, put all data from sql query into 2 arrays (for x and y)
		  //
		  double (*adXData);
		  adXData = new double [lNumPts];

 		  double (*adYData);
		  adYData = new double[eNumberRecetteDataSets * lNumPts];
          memset( adYData, 0 , sizeof(double) * eNumberRecetteDataSets * lNumPts );

		  // determine the labeling interval for the X axis
		  int iXLabelInterval = m_Chart.GetLabelInterval();
		  if ( lNumPts < iXLabelInterval )
				iXLabelInterval = 1;             // label the X axis every 1 point
		  else
				iXLabelInterval = m_Chart.GetLabelInterval();

		  // create an array of X-axis labels for this compartment
		  CStringArray aszLabels;

		  // loop through all the records for this compartment
		  int iCmptDateIdx = 0;  // index by Cmpt / Date

		  // create a new cta graph data object
		  CCTAChartData* pCTAChartData = 
		  new CCTAChartData( &m_Chart, CCTAChartData::eRecettes, NULL, NULL, 
		      *(pCmptIterator->Current()->Cmpt()), pCmptIterator->Current()->CmptSeq() );
		  
		  long lRev = 0;
		  for( pCmptDateIterator->First(); !pCmptDateIterator->Finished(); pCmptDateIterator->Next() ) 
		  {
			pHistDpt = (YmIcHistDptDom*)pCmptDateIterator->Current();
			// adjust negative values to zero
			pHistDpt->DmdDptFc( __max( pHistDpt->DmdDptFc(), 0 ) );

			// when setting the graph values (from the database), if the value is NULL,
			//   we set to INVALID_GRAPH_VALUE, then later display in the graph help dialog as "N/A"
			//   see CCTAChartHelpDlg::GetValue()		
			lRev = pHistDpt->Revenu();
			if (bPrixGaranti)
			  lRev += pHistDpt->RevenuGaranti();
			adYData[iCmptDateIdx + (eRecette*lNumPts) ] = (double)lRev; 
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
				 ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHistoDepLastYearFlag() && m_pYavData)
			{
				CYavData* pYavData = m_pYavData->GetPrevYearData (pHistDpt->DptDate(), pHistDpt->Cmpt()[0]);
				adYData[iCmptDateIdx + (eYavRecette*lNumPts)] = pYavData ? pYavData->m_rev : INVALID_GRAPH_VALUE;
				if (pYavData)
				  pCTAChartData->SetGraphMax ((double)pYavData->m_rev);
			}

			// set graph maximum values for scaling
			pCTAChartData->SetGraphMax( (double)pHistDpt->AuthDpt());
			pCTAChartData->SetAuthMax( (double)pHistDpt->AuthDpt());

			// Format the x-axis label which is the dpt date
			// only display the last 2 digits of the date (Y2K compliant)
			RWDate rwDate = pHistDpt->DptDate();            // get the dpt date as a roguewave date
			char szCYear[10];
			itoa( rwDate.year(), szCYear, 10 );
			CString szMFCYear( szCYear );
			szMFCYear = szMFCYear.Right( 2 );
			CString pszBuffer;
			pszBuffer.Format( "%2.2u/%2.2u/%s", rwDate.dayOfMonth(), rwDate.month(), LPCTSTR(szMFCYear) );

			// Must add the dpt date (x-axis label) to the array, because we don't set ALL dpt dates
			//   in the chart's array of labels.  This means we can't access them later for the help window.
			//   So, by keeping an array of ALL dpt dates, we can access any of them later.
			aszLabels.SetAtGrow( iCmptDateIdx, pszBuffer );

			// produce a dpt date label on X axis for every 'iIntervalConst' points, where n = iXLabelInterval
			if ( (iCmptDateIdx % iXLabelInterval) == 0)
				m_Chart.SetNthPointLabel(iCmptDateIdx, (char *)LPCTSTR(pszBuffer));  // set label for this point
			else
				m_Chart.SetNthPointLabel(iCmptDateIdx, "");  // clear the label for this point

			adXData[iCmptDateIdx] = iCmptDateIdx+1;
			iCmptDateIdx++;
		  }  // Fin de boucle sur les Points

		  // create the chart data object
		  CChart2DData *pChartData = 
		  new CChart2DData(XRT_DATA_ARRAY, eNumberRecetteDataSets, lNumPts);

		  // set the Y elements
		  for (iCmptDateIdx = 0; iCmptDateIdx < eNumberRecetteDataSets; iCmptDateIdx++)
			pChartData->SetYData(iCmptDateIdx, adYData+(iCmptDateIdx*lNumPts), lNumPts, 0);

		  // set the X elements
		  for (iCmptDateIdx = 0; iCmptDateIdx < lNumPts; iCmptDateIdx++)
			pChartData->SetXElement(NULL, iCmptDateIdx, adXData[iCmptDateIdx]);

		  // here we must make a copy of the string array containing the labels and put it in the tab object
		  CStringArray* paszLabels = pCTAChartData->GetXLabels();
		  int iLabelArraySize = aszLabels.GetSize();
		  for (iCmptDateIdx = 0; iCmptDateIdx < iLabelArraySize; iCmptDateIdx++)
			paszLabels->SetAtGrow( iCmptDateIdx, aszLabels.GetAt( iCmptDateIdx ) );

		  pCTAChartData->SetDataObj( pChartData );  // set the Olectra data object in the CTA data object

		  m_Chart.AddChartData( pCTAChartData, false );  // add this Chart Data element to the array

		  // cleanup the temporary arrays
		  delete [ ] adYData;
		  adYData = NULL;
		  delete [ ] adXData;
		  adXData = NULL;
		}

	    delete pCmptDateIterator;  // delete the row iterator

      }  // Fin de la boucle sur les compartiments
      m_Chart.SetRepaint( TRUE );
    }
    delete pCmptIterator;  // delete the cmpt index iterator
}
///////////////////////////////////////////////////////
// Build the graph
//
void CHistDptView::BuildGraph()
{
    YM_RecordSet* pHistDptSet = YM_Set<CHistDpt>::FromKey( (GetAltFlag()) ? HISTDPT_ALT_KEY : HISTDPT_KEY );

    // Define the iterator to iterate through compartments
    YM_Iterator<CHistDpt>* pCmptIterator = ((CHistDptSet*) pHistDptSet)->CreateKeyIterator();
    long lNumPts = pCmptIterator->GetCount();
    if (lNumPts > 0)
    {
      m_Chart.SetRepaint( FALSE );

      // Set text in status bar control
      ((CChildFrame*)GetParentFrame())->
        GetStatusBar()->SetPaneOneAndLock(
          ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BUILDING_GRAPH) );

      // we might need currently selected leg for leg orig/dest searching sys parm
      YmIcLegsDom* pLeg = YM_Set<YmIcLegsDom>::FromKey (LEG_KEY);
      YmIcHistDptDom* pHistDpt = NULL;

      // iterate through the compartments
      for( pCmptIterator->First(); !pCmptIterator->Finished(); pCmptIterator->Next() ) 
      {
	    YmIcHistDptDom* Current = pCmptIterator->Current();
        // Define the iterator to iterate through records for this compartment
		YM_Iterator<CHistDpt>* pCmptDateIterator =
				((CHistDptSet*) pHistDptSet)->CreateIterator( *(pCmptIterator->Current()->Cmpt()) );

        long lNumPts = pCmptDateIterator->GetCount();  // how many recs for this cmpt ?  // DM5350
        if (lNumPts > 0)
        {
		  // transfer the data from the record set to the graph data structure
		  // first, put all data from sql query into 2 arrays (for x and y)
		  //
		  double (*adXData);
		  adXData = new double [lNumPts];

 		  double (*adYData);
		  adYData = new double[eNumberDataSets * lNumPts];

		  // determine the labeling interval for the X axis
		  int iXLabelInterval = m_Chart.GetLabelInterval();
		  if ( lNumPts < iXLabelInterval )
				iXLabelInterval = 1;             // label the X axis every 1 point
		  else
				iXLabelInterval = m_Chart.GetLabelInterval();

		  // create an array of X-axis labels for this compartment
		  CStringArray aszLabels;

		  // loop through all the records for this compartment
		  int iCmptDateIdx = 0;  // index by Cmpt / Date

		  // create a new cta graph data object
		  CCTAChartData* pCTAChartData = 
		  new CCTAChartData( &m_Chart, CCTAChartData::eCmpt, NULL, NULL, 
		      *(pCmptIterator->Current()->Cmpt()), pCmptIterator->Current()->CmptSeq() );
		 
		  for( pCmptDateIterator->First(); !pCmptDateIterator->Finished(); pCmptDateIterator->Next() ) 
		  {
			pHistDpt = (YmIcHistDptDom*)pCmptDateIterator->Current();
			// adjust negative values to zero
			pHistDpt->DmdDptFc( __max( pHistDpt->DmdDptFc(), 0 ) );

			// when setting the graph values (from the database), if the value is NULL,
			//   we set to INVALID_GRAPH_VALUE, then later display in the graph help dialog as "N/A"
			//   see CCTAChartHelpDlg::GetValue()
			if (!GetAltFlag())
			  adYData[iCmptDateIdx + (eAuthDpt*lNumPts)] = (double)pHistDpt->AuthDpt();

			adYData[iCmptDateIdx + (eCapacity*lNumPts)] = (double)pHistDpt->Capacity();
			adYData[iCmptDateIdx + (eResHoldDptTot*lNumPts)] = (! pHistDpt->ResHoldDptTotIsNull()) ?
					(double)pHistDpt->ResHoldDptTot() : INVALID_GRAPH_VALUE;
			if (!GetAltFlag())
			  adYData[iCmptDateIdx + (eTrafficRes*lNumPts)] = (! pHistDpt->TrafficResIsNull()) ?
					(double)pHistDpt->TrafficRes() : INVALID_GRAPH_VALUE;
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
				 ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHistoDepLastYearFlag() && m_pYavData)
			{
				CYavData* pYavData = m_pYavData->GetPrevYearData (pHistDpt->DptDate(), pHistDpt->Cmpt()[0]);
				adYData[iCmptDateIdx + (eYavResHold*lNumPts)] = pYavData ? pYavData->m_resa : INVALID_GRAPH_VALUE;
				adYData[iCmptDateIdx + (eYavAuthDpt*lNumPts)] = pYavData ? pYavData->m_auth : INVALID_GRAPH_VALUE;
				if (pYavData)
				{
				  pCTAChartData->SetGraphMax( (double)pYavData->m_resa);
				  pCTAChartData->SetGraphMax( (double)pYavData->m_auth);
				}
			}

			// set graph maximum values for scaling
			if (!GetAltFlag())
			{
			  pCTAChartData->SetGraphMax( (double)pHistDpt->AuthDpt() );
			  pCTAChartData->SetGraphMax( (double)pHistDpt->DmdDptFc() );
			  pCTAChartData->SetGraphMax( (double)pHistDpt->TrafficRes() );
			}		
			pCTAChartData->SetGraphMax( (double)pHistDpt->Capacity() );
			pCTAChartData->SetGraphMax( (double)pHistDpt->ResHoldDptTot() );	
			pCTAChartData->SetAuthMax( (double)pHistDpt->AuthDpt() );
			pCTAChartData->SetCapMax( (double)pHistDpt->Capacity() );
			pCTAChartData->SetDmdDptFcstMax( (double)pHistDpt->DmdDptFc() );
			pCTAChartData->SetResHoldMax( (double)pHistDpt->ResHoldDptTot() );
			pCTAChartData->SetTrafficMax( (double)pHistDpt->TrafficRes() );

			// Format the x-axis label which is the dpt date
			// only display the last 2 digits of the date (Y2K compliant)
			RWDate rwDate = pHistDpt->DptDate();            // get the dpt date as a roguewave date
			char szCYear[10];
			itoa( rwDate.year(), szCYear, 10 );
			CString szMFCYear( szCYear );
			szMFCYear = szMFCYear.Right( 2 );
			CString pszBuffer;
			pszBuffer.Format( "%2.2u/%2.2u/%s", rwDate.dayOfMonth(), rwDate.month(), LPCTSTR(szMFCYear) );

			// Must add the dpt date (x-axis label) to the array, because we don't set ALL dpt dates
			//   in the chart's array of labels.  This means we can't access them later for the help window.
			//   So, by keeping an array of ALL dpt dates, we can access any of them later.
			aszLabels.SetAtGrow( iCmptDateIdx, pszBuffer );

			// produce a dpt date label on X axis for every 'iIntervalConst' points, where n = iXLabelInterval
			if ( (iCmptDateIdx % iXLabelInterval) == 0)
				m_Chart.SetNthPointLabel(iCmptDateIdx, (char *)LPCTSTR(pszBuffer));  // set label for this point
			else
				m_Chart.SetNthPointLabel(iCmptDateIdx, "");  // clear the label for this point

			adXData[iCmptDateIdx] = iCmptDateIdx+1;
			iCmptDateIdx++;
		  }  // Fin de boucle sur les Points

		  // create the chart data object
		  CChart2DData *pChartData = 
		  new CChart2DData(XRT_DATA_ARRAY, eNumberDataSets, lNumPts);

		  // set the Y elements
		  for (iCmptDateIdx = 0; iCmptDateIdx < eNumberDataSets; iCmptDateIdx++)
			pChartData->SetYData(iCmptDateIdx, adYData+(iCmptDateIdx*lNumPts), lNumPts, 0);

		  // set the X elements
		  for (iCmptDateIdx = 0; iCmptDateIdx < lNumPts; iCmptDateIdx++)
			pChartData->SetXElement(NULL, iCmptDateIdx, adXData[iCmptDateIdx]);

		  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
			   ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHistoDepLastYearFlag() && !m_pYavData)
		  {
			pChartData->SetLastPoint(eYavResHold, -1);
			pChartData->SetLastPoint(eYavAuthDpt, -1);
		  }
          

		  // here we must make a copy of the string array containing the labels and put it in the tab object
		  CStringArray* paszLabels = pCTAChartData->GetXLabels();
		  int iLabelArraySize = aszLabels.GetSize();
		  for (iCmptDateIdx = 0; iCmptDateIdx < iLabelArraySize; iCmptDateIdx++)
			paszLabels->SetAtGrow( iCmptDateIdx, aszLabels.GetAt( iCmptDateIdx ) );

		  pCTAChartData->SetDataObj( pChartData );  // set the Olectra data object in the CTA data object

		  m_Chart.AddChartData( pCTAChartData, false );  // add this Chart Data element to the array

		  // cleanup the temporary arrays
		  delete [ ] adYData;
		  adYData = NULL;
		  delete [ ] adXData;
		  adXData = NULL;
		} // Fin lNumPts > 0
		delete pCmptDateIterator;  // delete the row iterator
      }  // Fin de la boucle sur les compartiments
      m_Chart.SetRepaint( TRUE );
    }
    delete pCmptIterator;  // delete the cmpt index iterator
}

//////////////////////////////////
// Draw the graph
//
void CHistDptView::ShowGraph(CChart2DData* pChartDataObj)
{
  m_Chart.SetRepaint( FALSE );

  CCTAChartData* pCTAChartData = m_Chart.GetCurChartData();
  m_Chart.SetGraphProperties(pCTAChartData->GetGraphType());
  m_Chart.SetGraphFont();

  // set the chart data object for the tab selected
  m_Chart.SetData( *pChartDataObj );

  /* 17/04/07	suppression du footer
  CString szGraphFooter = 
    ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DPTHISTORY_FOOTER);
  char *szFooter[] = { (LPSTR)(LPCSTR)szGraphFooter, NULL };
  m_Chart.SetFooterStrings(szFooter);
  */

  BuildXLabels();
  ResizeChart();
  m_Chart.ScaleGraph(m_Chart.GetCurScaling());  // don't forget to rescale the graph
  m_Chart.SetRepaint( TRUE );
  m_Chart.ShowWindow( SW_SHOW );
}

//////////////////////////////////////////////////////////
// Clear the tabs array and removes the corresponding tab
//
void CHistDptView::ClearTabs()
{
  // clean up the array containing the tabs and graph data objects
  m_ChartTabCtrl.DeleteAllItems( ); // remove the tab from the control
}

//////////////////////////////////////
// Build the tabs on the tab control
//
void CHistDptView::BuildTabs()
{
  CCTAChartDataArray* apChartData = m_Chart.GetChartDataArray();
  if (apChartData->GetSize() == 0)
  {
    // Set text in status bar control
    ((CChildFrame*)GetParentFrame())->
      GetStatusBar()->SetPaneOneAndLock(
	((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA) );
  }
  else
  {
    CCTAChartData *pCTAChartData;

    int nLastTab = -1;
    TC_ITEM TabCtrlItem;
    TabCtrlItem.mask = TCIF_TEXT;

    CString szBuffer;
	
	int iSizeCmpt = 0;
	int iNbCmpt=0;
	BOOL bRev = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag();
	if ( bRev && !((CCTAApp*)APP)->GetRhealysFlag() )//SRE Defect 3889 - Regression Evol Sensibilité - Rajout d'une verif sur le FLAG Alleo pour ne pas activer les onglets Recettes
    {	
		    iSizeCmpt = apChartData->GetSize()/2;
			for (int iTabIdx=0; iTabIdx < iSizeCmpt; iTabIdx++)
			{
				pCTAChartData = apChartData->GetAt(iTabIdx);

				szBuffer.Format( "Recettes %s", pCTAChartData->GetCmpt() );
				if (_sLastTabUsed == szBuffer)	nLastTab = iTabIdx;

				TabCtrlItem.pszText = (LPSTR)(LPCSTR)szBuffer;
				m_ChartTabCtrl.InsertItem( iNbCmpt, &TabCtrlItem );
				iNbCmpt++;
			}
	}
	else
	{
			iSizeCmpt = apChartData->GetSize();
	}

    // loop through the tab index array, build and insert each tab
    for (int iTabIdx=0; iTabIdx < iSizeCmpt; iTabIdx++)
    {
      pCTAChartData = apChartData->GetAt(iTabIdx);

      szBuffer.Format( "Résas %s", pCTAChartData->GetCmpt() );
      if (_sLastTabUsed == szBuffer)
	  nLastTab = iTabIdx+iNbCmpt;

      TabCtrlItem.pszText = (LPSTR)(LPCSTR)szBuffer;
      m_ChartTabCtrl.InsertItem( iTabIdx+iNbCmpt, &TabCtrlItem );
    }

    // select the first tab by default
    if (apChartData->GetSize())
    {
      if ( ! m_bTabResized )
      {
	    // resize the tabs to 1/3 of their size
	    CRect TabRect;
	    CSize TabSize;
	    m_ChartTabCtrl.GetItemRect( 0, &TabRect );
        //	TabSize.cx = long( (TabRect.right - TabRect.left) * .33 );
	    TabSize.cx = long( (TabRect.right - TabRect.left));
	    TabSize.cy = TabRect.bottom - TabRect.top;
	    m_ChartTabCtrl.SetItemSize( TabSize );
	    m_bTabResized = TRUE;
      }

      // set the default selected data object and tab item to the first one (index zero)
      m_Chart.SetCurChartData( 0 );
      m_ChartTabCtrl.SetCurSel(0);
      pCTAChartData = apChartData->GetAt(0);

	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag() )
	  {
		COLORREF clrFt;
	    clrFt = (GetAltFlag()) ? 
	      ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkAlt() : 
	      ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();
        m_Chart.SetForegroundColor (clrFt);
        /*m_Chart.SetYTitle("€uros");
	    m_Chart.SetYTitleRotation(XRT_ROTATE_90);*/
	  }

      ShowGraph( pCTAChartData->GetDataObj() );
    }

    // Set text in status bar control
    ((CChildFrame*)GetParentFrame())->
      GetStatusBar()->SetPaneOneAndLock(
	((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DONE) );

    if (nLastTab != -1)
	{
		m_ChartTabCtrl.SetCurSel( nLastTab );
		LRESULT result;
		OnChartSelchange( NULL, &result );
    }
  }
}

//////////////////////////////////////
// Write specific view configuration information to the registry
//
void CHistDptView::WriteViewConfig()
{
    // save the graph flags in the registry
    //
    CString szProfileKey;
    ((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);
    /* split */
    CSplitChildFrame* pWnd = (CSplitChildFrame*)GetParentFrame();
    pWnd->WriteSplitterConfig(szProfileKey);
    /* split */

    APP->WriteProfileInt( szProfileKey, GRAPH_SCALE_FACTOR, m_Chart.GetCurScaling() );
    APP->WriteProfileInt( szProfileKey, ROTATE_X_LABEL, m_Chart.GetRotateLabelsFlag() ? 1 : 0 );
	APP->WriteProfileInt( szProfileKey, LABEL_INTERVAL, m_Chart.GetLabelInterval() );
    APP->WriteProfileString( szProfileKey, "LastTabUsed", _sLastTabUsed );
    /* split */
	if (!_nSubViewID) 
	{
      CHistDptView* view = getOtherView();
      if (view)
	  {
	  APP->WriteProfileString( szProfileKey, "LastTabUsed_2", view->_sLastTabUsed );
	  }
    }
	/* split */

}

//////////////////////////////////////
// Restore specific view configuration information from the registry
//
void CHistDptView::RestoreViewConfig()
{
    // restore the graph flags from the registry
    //
    CString szProfileKey;
    ((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

    enum CCTAChart::GraphScaleIndex eCurScale = (enum CCTAChart::GraphScaleIndex)
    APP->GetProfileInt(szProfileKey, GRAPH_SCALE_FACTOR, (unsigned int)CCTAChart::eMaxScaleIdx);
	int interval = APP->GetProfileInt (szProfileKey, LABEL_INTERVAL, 5);

	GraphLabelInterval(interval);
    m_Chart.SetRotateLabelsFlag( !APP->GetProfileInt(szProfileKey, ROTATE_X_LABEL, 0) );
    OnGraphRotateLabels();
    m_Chart.SetCurScaling(eCurScale);
    m_Chart.ScaleGraph(m_Chart.GetCurScaling());  // don't forget to rescale the graph
    _sLastTabUsed = APP->GetProfileString(szProfileKey, "LastTabUsed", "" );

	CSplitChildFrame* pWnd = (CSplitChildFrame*)GetParentFrame();
    pWnd->RestoreSplitterConfig(szProfileKey);
    //m_Chart.SetRepaint( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CHistDptChart

BEGIN_MESSAGE_MAP(CHistDptChart, CCTAChart)
	//{{AFX_MSG_MAP(CHistDptChart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////
// constructor
//
CHistDptChart::CHistDptChart()
{
  m_bRotateLabels = FALSE;
  m_iLabelInterval = 5;
}

// ScaleGraph surchargée pour ne pas s'appliquer au graphique revenu
void CHistDptChart::ScaleGraph(enum GraphScaleIndex eGraphScale )
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
void CHistDptChart::SetGraphProperties(enum CCTAChartData::GraphType eGraphType)
{
  SetType(XRT_TYPE_PLOT);
  ( m_bRotateLabels ) ? 
    SetXAnnotationRotation(XRT_ROTATE_90) :
    SetXAnnotationRotation(XRT_ROTATE_NONE);
  SetXAnnotationMethod(XRT_ANNO_POINT_LABELS);

  // set the label for each line
    SetLegendShow(FALSE);  // but turn it off
  if (eGraphType == CCTAChartData::eCmpt)
  {
    XrtDataStyle **paCmptDataStyles = (XrtDataStyle **)malloc( (GetParentView()->eNumberDataSets+1) * sizeof(XrtDataStyle *) ) ;
    
	if (!GetParentView()->GetAltFlag())
	{
      SetNthSetLabel(GetParentView()->eAuthDpt,
        (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AUTH_DPT));
      paCmptDataStyles[ GetParentView()->eAuthDpt ] = (m_aDataStyles.GetAt(eAuthStyleIdx));

	  SetNthSetLabel(GetParentView()->eTrafficRes,
      (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TRAFFIC_RES));
      paCmptDataStyles[ GetParentView()->eTrafficRes ] = (m_aDataStyles.GetAt(eTrafficResStyleIdx));
	}

    SetNthSetLabel(GetParentView()->eCapacity,
		(  LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(GetParentView()->GetAltFlag() ? IDS_CCM1 : IDS_CAPACITY));
    paCmptDataStyles[ GetParentView()->eCapacity ] = (m_aDataStyles.GetAt(eCapStyleIdx));

    SetNthSetLabel(GetParentView()->eResHoldDptTot,
      (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_TOT));
    paCmptDataStyles[ GetParentView()->eResHoldDptTot ] = (m_aDataStyles.GetAt(eResHoldTotStyleIdx));

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
		 ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHistoDepLastYearFlag())
    {
	  SetNthSetLabel(GetParentView()->eYavResHold, "Résa");
      paCmptDataStyles[ GetParentView()->eYavResHold ] = (m_aDataStyles.GetAt(eHisto1ResHoldTotStyleIdx));

	  CString ssl;
	  if (GetParentView()->GetAltFlag())
	  {
		ssl = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CCM1);
		ssl += "  ";
	  }
	  else
	    ssl = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AUTHORIZATIONS);

	  SetNthSetLabel(GetParentView()->eYavAuthDpt, (LPSTR)(LPCSTR)ssl);
      paCmptDataStyles[ GetParentView()->eYavAuthDpt ] = (m_aDataStyles.GetAt(eHisto1OvbLevelStyleIdx));
	}

    paCmptDataStyles[GetParentView()->eNumberDataSets] = NULL;

    SetDataStyles(paCmptDataStyles);
    free (paCmptDataStyles);
    paCmptDataStyles = NULL;
  }
  else
  {
	XrtDataStyle **paCmptDataStyles = (XrtDataStyle **)malloc( (GetParentView()->eNumberRecetteDataSets+1) * sizeof(XrtDataStyle *) ) ;
    
    SetNthSetLabel(GetParentView()->eRecette, (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RECETTES));
    paCmptDataStyles[ GetParentView()->eRecette ] = (m_aDataStyles.GetAt(eResHoldTotStyleIdx));

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
		 ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHistoDepLastYearFlag())
    {
	  SetNthSetLabel(GetParentView()->eYavRecette, (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RECETTES));
      paCmptDataStyles[ GetParentView()->eYavRecette ] = (m_aDataStyles.GetAt(eHisto1OvbLevelStyleIdx));
	}

	paCmptDataStyles[GetParentView()->eNumberRecetteDataSets] = NULL;

    SetDataStyles(paCmptDataStyles);
	free (paCmptDataStyles);
    paCmptDataStyles = NULL;
  }

  // set default axis minimum
  SetYAxisMin(0);
  SetY2AxisMin(0);

  CCTAChart::SetGraphFont();
}


CCTAChartHelpDlg* CHistDptChart::CreateHelpDlg()
{
  m_pHelpDlg = new CHistDptHelp( this );
  m_pHelpDlg->Create( this );
  ((CMainFrame*) AfxGetApp()->GetMainWnd())->SetChartHelpDlg(m_pHelpDlg);
  return (m_pHelpDlg);
}

void CHistDptChart::ContextualDisplay(CPoint point, BOOL bInRange)
{
  return;
}

///////////////////////////////////////
// CHistDptChart message handlers


/////////////////////////////////////////////////////////////////////////////
// CHistDptHelp dialog

CHistDptHelp::CHistDptHelp(CHistDptChart* pParent /*=NULL*/)
  : CCTAChartHelpDlg(pParent)
{
}

void CHistDptHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHistDptHelp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHistDptHelp, CCTAChartHelpDlg)
	//{{AFX_MSG_MAP(CHistDptHelp)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////
// CHistDptHelp message handlers

// build the 1st title
void CHistDptHelp::GetTitle1( CString& szTitle )
{
  // get the Dpt Date of the selected data point for the 2nd title
  CCTAChartData* pChartData = 
    ((CHistDptView*)m_pParent->GetParentView())->GetChart()->GetCurChartData();
  if ( pChartData == NULL )
    return;
  CStringArray* paszLabels = pChartData->GetXLabels();

  if ( m_iCurSelectPt < paszLabels->GetSize() )
  {
    CString szDptDate = paszLabels->GetAt( m_iCurSelectPt );
    szTitle.Format("%s: %s", 
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DPT_DATE),
      szDptDate);  // dlg window title
  }
  else
  {
    szTitle.Empty();
  }

  return;
}

// build the 2nd title
void CHistDptHelp::GetTitle2( CString& szTitle )
{
  // get the Cmpt of the selected data point for the 1st title
  CCTAChartData* pChartData = 
    ((CHistDptView*)m_pParent->GetParentView())->GetChart()->GetCurChartData();
  if ( pChartData == NULL )
    return;
  szTitle.Format( "%s %s", 
	  ((CCTAApp*)APP)->GetResource()->LoadResString(((CHistDptView*)m_pParent->GetParentView())->GetAltFlag() ? IDS_SOUS_CONT : IDS_CMPT),
    pChartData->GetCmpt() );
  return;
}

/* DM 5350 Permet de retourner True si le type de graphe courant est eRecette*/
BOOL CHistDptHelp::IsRecetteDataSet ()
{
 CCTAChartData* pCTAChartData = GetParentChart()->GetCurChartData();
 
 if (pCTAChartData->GetGraphType() == CCTAChartData::eRecettes)
	  return true;
 else return false;
}

// Get the longest label to be printed.  This is used for sizing the width
//
CSize CHistDptHelp::GetExtentLongestLabel( CPaintDC& dc )
{
  // loop through all the labels, and find the longest one (varies from language to language)
  int iLongest = 0;
  CString szTextLabel;
  CSize sizeTextLabel(0, 0);
  CCTAChartData* pChartData = 
    ((CHistDptView*)m_pParent->GetParentView())->GetChart()->GetCurChartData();
  if (pChartData->GetGraphType() == CCTAChartData::eCmpt)
  {
    for( int i = 0; i < GetParentView()->eNumberDataSets; i++ )
    {
      szTextLabel = XrtGetNthSetLabel( m_pParent->m_hChart, i );
	  if (i >= GetParentView()->eYavResHold) // affichage année précédente
        szTextLabel += "00/00/00000";
      sizeTextLabel = dc.GetOutputTextExtent( szTextLabel );  // get the text extent
      if ( sizeTextLabel.cx > iLongest )
        iLongest = sizeTextLabel.cx;
    }
  }
  else // recettes
  {
    for( int i = 0; i < GetParentView()->eNumberRecetteDataSets; i++ )
    {
      szTextLabel = XrtGetNthSetLabel( m_pParent->m_hChart, i );
	  if (i == GetParentView()->eYavRecette) // affichage année précédente
        szTextLabel += "00/00/00000";
      sizeTextLabel = dc.GetOutputTextExtent( szTextLabel );  // get the text extent
      if ( sizeTextLabel.cx > iLongest )
        iLongest = sizeTextLabel.cx;
    }
  }
  sizeTextLabel.cx = iLongest;
  return sizeTextLabel;
}

// Get the text label to be drawn
//
BOOL CHistDptHelp::GetTextLabel( int iDataSet, CString& szTextLabel, int iCurSelectSet )
{
  CCTAChartData* pChartData = ((CHistDptView*)m_pParent->GetParentView())->GetChart()->GetCurChartData();
  if ( pChartData == NULL )
    return TRUE;

  szTextLabel = XrtGetNthSetLabel(m_pParent->m_hChart, iCurSelectSet);
  if (   ( (pChartData->GetGraphType() == CCTAChartData::eCmpt) &&
	       ((iCurSelectSet == GetParentView()->eYavResHold) || (iCurSelectSet == GetParentView()->eYavAuthDpt)) )
	  || ( (pChartData->GetGraphType() == CCTAChartData::eRecettes) && (iCurSelectSet == GetParentView()->eYavRecette) ) )
  {   
    CStringArray* paszLabels = pChartData->GetXLabels();
    if ( m_iCurSelectPt < paszLabels->GetSize() )
    {
	  // récupération de la date du point courant
      CString szDptDate = paszLabels->GetAt( m_iCurSelectPt );
      int dd = atoi (szDptDate.Left(2));
	  int mm = atoi (szDptDate.Mid (3,2));
	  int aa = atoi (szDptDate.Right(2)) + 2000;
      RWDate dada (dd, mm, aa);  
	  unsigned long YavJulian = ((CHistDptView*)m_pParent->GetParentView())->GetYavDate (dada.julian(), pChartData->GetCmpt()[0]);
	  if (YavJulian)
	  {
	    RWDate daYav (YavJulian);
		CString szdaYav;
	    szdaYav.Format( "%2.2u/%2.2u/%2.2u", daYav.dayOfMonth(), daYav.month(), daYav.year() % 100 );
	    szTextLabel += " ";
		szTextLabel += szdaYav;
	  }
    }
  }
  return TRUE;
}

// Translate the dataset to a cta graph style (color)
//
int CHistDptHelp::GetCTADataStyleIndex( int iDataSet, int iCurSelectSet )
{
  // translate the dataset to a cta graph style (color)
  CCTAChartData* pChartData = 
    ((CHistDptView*)m_pParent->GetParentView())->GetChart()->GetCurChartData();
  if (pChartData->GetGraphType() == CCTAChartData::eCmpt)
  {  
    if ( iCurSelectSet == GetParentView()->eAuthDpt )
        return CCTAChart::eAuthStyleIdx;
    if ( iCurSelectSet == GetParentView()->eCapacity )
        return CCTAChart::eCapStyleIdx;
    if ( iCurSelectSet == GetParentView()->eDmdDptFcst )
        return CCTAChart::eDmdDptFcstStyleIdx;
    if ( iCurSelectSet == GetParentView()->eResHoldDptTot )
        return CCTAChart::eResHoldTotStyleIdx;
    if ( iCurSelectSet == GetParentView()->eTrafficRes )
        return CCTAChart::eTrafficResStyleIdx;
    if ( iCurSelectSet == GetParentView()->eYavResHold )
        return CCTAChart::eHisto1ResHoldTotStyleIdx;
    if ( iCurSelectSet == GetParentView()->eYavAuthDpt )
        return CCTAChart::eHisto1OvbLevelStyleIdx;
  }
  else
  {
    if ( iCurSelectSet == GetParentView()->eRecette )
	  return CCTAChart::eResHoldTotStyleIdx;
	if ( iCurSelectSet == GetParentView()->eYavRecette )
	  return CCTAChart::eHisto1ResHoldTotStyleIdx;
  }

  return -1;
}

void CHistDptHelp::OnPaint()
{
  // paint all text and graphics on the client area of the dialog
  //
  CPaintDC dc(this); // device context for painting
  CFont* pOldFont = dc.SelectObject( GetHelpTextFont() );

  CCTAChartData* pChartData = 
    ((CHistDptView*)m_pParent->GetParentView())->GetChart()->GetCurChartData();
  // set the initial size of the help dialog window
  if (pChartData->GetGraphType() == CCTAChartData::eCmpt)   
    InitHelpWin( GetParentView()->eNumberDataSets );
  else
    InitHelpWin (GetParentView()->eNumberRecetteDataSets);

  // draw the titles and return the point where drawing is to continue
  CPoint pointDraw = DrawTitles( dc );

  // draw the labels and the corresponding values
  DrawLabels( dc, pointDraw );

	// Do not call CDialog::OnPaint() for painting messages
}
