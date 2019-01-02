// RecDHistView.cpp : implementation file
//
#include "StdAfx.h"


#include "CTAChartHelpDlg.h"
#include "RecDHistView.h"
#include "DatesHistoDlg.h"
#include "ResaRailView.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WIDTH_PANE_INFO		200


////////////////////////////
//CRecHistoData
CRecHistoData::CRecHistoData(BOOL bAlt)
{

  m_MaxHistoRef = 0;
  bSci = bAlt;
  YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
        YM_Set<YmIcHistoEvolDom>::FromKey(bAlt ? HISTOEVOL_REC_ALT_KEY : HISTOEVOL_REC_KEY)->CreateIterator();
  YmIcHistoEvolDom* pHistoEvol = NULL;
  RWDate prevdate (1,1,1900);
  m_max_RecD = 0;
  m_min_RecD = 1000;
  for (int i = 0; i < 26; i++)
  {
    m_MaxBkt1[i] = 0;
	m_MaxBkt2[i] = 0;
	m_MaxBkt3[i] = 0;
  }
  char cc;
  int idxcc;
  int kk = bAlt ? 0 : 1; // si !bAlt on parle de CC, donc numérotée à partir de 0, sinon c'est des SCI numérotéses à partir de 1
  for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
  {
    pHistoEvol = pVariablesIterator->Current();
	if (pHistoEvol->RrdIndex() > m_max_RecD)
	  m_max_RecD = pHistoEvol->RrdIndex();
	if (pHistoEvol->RrdIndex() < m_min_RecD)
	  m_min_RecD = pHistoEvol->RrdIndex();
	cc = pHistoEvol->Cmpt()[0];
	idxcc = cc - 'A';

	//116770, récuperation de commentaire et histo_ref directement dans ((CCTAApp*)APP)->m_CurHistoDates
	// pour alléger la requête IDS_SQL_SELECT_RecD_DATES_HISTO
	if (pHistoEvol->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto1())
	{
	  pHistoEvol->Commentaire (((CCTAApp*)APP)->m_CurHistoDates.Commentaire1());
	  pHistoEvol->HistoRef(1);
	  if (m_MaxBkt1[cc - 'A'] < pHistoEvol->NestLevel()+kk)
	    m_MaxBkt1[cc - 'A'] = pHistoEvol->NestLevel()+kk;
	}
	else if (pHistoEvol->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto2())
	{
	  pHistoEvol->Commentaire (((CCTAApp*)APP)->m_CurHistoDates.Commentaire2());
	  pHistoEvol->HistoRef(2);
	  if (m_MaxBkt2[cc - 'A'] < pHistoEvol->NestLevel()+kk)
	    m_MaxBkt2[cc - 'A'] = pHistoEvol->NestLevel()+kk;
	}
	else if (pHistoEvol->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto3())
	{
	  pHistoEvol->Commentaire (((CCTAApp*)APP)->m_CurHistoDates.Commentaire3());
	  pHistoEvol->HistoRef(3);
	  if (m_MaxBkt3[cc - 'A'] < pHistoEvol->NestLevel()+kk)
	    m_MaxBkt3[cc - 'A'] = pHistoEvol->NestLevel()+kk;
	}
	if (pHistoEvol->HistoRef() > m_MaxHistoRef)
	  m_MaxHistoRef = pHistoEvol->HistoRef();
  }
  delete pVariablesIterator;

  //MAJ Historef sur info cmpt
  pVariablesIterator = YM_Set<YmIcHistoEvolDom>::FromKey(bAlt ? HISTOEVOL_RECCMPT_ALT_KEY : HISTOEVOL_RECCMPT_KEY)->CreateIterator();
  for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
  {
    pHistoEvol = pVariablesIterator->Current();
	if (pHistoEvol->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto1())
	{
	  pHistoEvol->Commentaire (((CCTAApp*)APP)->m_CurHistoDates.Commentaire1());
	  pHistoEvol->HistoRef(1);
	}
	else if (pHistoEvol->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto2())
	{
	  pHistoEvol->Commentaire (((CCTAApp*)APP)->m_CurHistoDates.Commentaire2());
	  pHistoEvol->HistoRef(2);
	}
	else if (pHistoEvol->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto3())
	{
	  pHistoEvol->Commentaire (((CCTAApp*)APP)->m_CurHistoDates.Commentaire3());
	  pHistoEvol->HistoRef(3);
	}
  }
  delete pVariablesIterator;
}

CRecHistoData::~CRecHistoData()
{
}


double CRecHistoData::GetRevenu (int histoIdx, char cmpt, long RecD, int bucket)
{
  if (cmpt > 'Z')
    cmpt -=32; // mise en majuscule
  YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
        YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_REC_ALT_KEY : HISTOEVOL_REC_KEY)->CreateIterator();
  YmIcHistoEvolDom* pHistoEvol = NULL;
  double rev = 0;
  for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
  {
    pHistoEvol = pVariablesIterator->Current();
	if ((pHistoEvol->HistoRef() == histoIdx) &&
		(pHistoEvol->Cmpt()[0] == cmpt) &&
		(pHistoEvol->RrdIndex() == RecD) &&
		(pHistoEvol->NestLevel() == bucket))
	{
	  rev = pHistoEvol->ResHoldIdv();
	  if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag())
	    rev += pHistoEvol->ResHoldGrp();
	  rev += 50; // ajout de 50 centimes pour avoir un arrondi
	  rev /= 100;
	  delete pVariablesIterator;
	  return rev;
	}
  }
  delete pVariablesIterator;
  return rev;
}

bool CRecHistoData::IsRrd(long rrd)
{
  YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
        YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_RECCMPT_ALT_KEY : HISTOEVOL_RECCMPT_KEY)->CreateIterator();
  YmIcHistoEvolDom* pHistoEvol = NULL;
  for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
  {
    pHistoEvol = pVariablesIterator->Current();
	if (pHistoEvol->RrdIndex() == rrd)
	{
	  delete pVariablesIterator;
	  return true;
	}
  }
  delete pVariablesIterator;
  return false;
}


double CRecHistoData::GetSumRevenu (int histoIdx, char cmpt, long RecD)
{
  if (cmpt > 'Z')
    cmpt -=32; // mise en majuscule
  YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
        YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_RECCMPT_ALT_KEY : HISTOEVOL_RECCMPT_KEY)->CreateIterator();
  YmIcHistoEvolDom* pHistoEvol = NULL;
  double rev = 0;
  for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
  {
    pHistoEvol = pVariablesIterator->Current();
	if ((pHistoEvol->HistoRef() == histoIdx) &&
		(pHistoEvol->Cmpt()[0] == cmpt) &&
		(pHistoEvol->RrdIndex() == RecD))
	{
	  rev =  + pHistoEvol->ResHoldIdvTot();
	  if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag())
	    rev += pHistoEvol->ResHoldGrpTot();
	  delete pVariablesIterator;
	  rev += 50; // ajout de 50 centimes pour avoir un arrondi
	  rev /= 100;
	  return rev;
	}
  }
  delete pVariablesIterator;
  return rev;
}

bool CRecHistoData::GetHoiTra (int histoIdx, char cmpt, long RecD)
{
  if (cmpt > 'Z')
    cmpt -=32; // mise en majuscule
  YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
        YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_RECCMPT_ALT_KEY : HISTOEVOL_RECCMPT_KEY)->CreateIterator();
  YmIcHistoEvolDom* pHistoEvol = NULL;
  bool bHoiTra = false;
  for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
  {
    pHistoEvol = pVariablesIterator->Current();
	if ((pHistoEvol->HistoRef() == histoIdx) &&
		(pHistoEvol->Cmpt()[0] == cmpt) &&
		(pHistoEvol->RrdIndex() == RecD))
	{
	  bHoiTra = !strcmp (pHistoEvol->HoiTra(), "I");
	  delete pVariablesIterator;
	  return bHoiTra;
	}
  }
  delete pVariablesIterator;
  return bHoiTra;
}


CString CRecHistoData::GetTitle (int histoIdx)
{
  CString szTitle;
  YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
        YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_RECCMPT_ALT_KEY : HISTOEVOL_RECCMPT_KEY)->CreateIterator();
  YmIcHistoEvolDom* pHistoEvol = NULL;
  for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
  {
    pHistoEvol = pVariablesIterator->Current();
	if (pHistoEvol->HistoRef() == histoIdx) 
	{
	  szTitle = pHistoEvol->Commentaire();
	  delete pVariablesIterator;
	  return szTitle;
	}
  }
  delete pVariablesIterator;
  return szTitle;
}

RWDate CRecHistoData::GetDateHisto (int histoIdx)
{
  RWDate DateHisto;
  DateHisto.julian(0);
  YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
        YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_RECCMPT_ALT_KEY : HISTOEVOL_RECCMPT_KEY)->CreateIterator();
  YmIcHistoEvolDom* pHistoEvol = NULL;
  for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
  {
    pHistoEvol = pVariablesIterator->Current();
	if (pHistoEvol->HistoRef() == histoIdx) 
	{
	  DateHisto.julian(pHistoEvol->DptDate());
	  delete pVariablesIterator;
	  return DateHisto;
	}
  }
  delete pVariablesIterator;
  return DateHisto;
}

int CRecHistoData::GetNbBucket(int histoIdx, char cmpt)
{
  if (cmpt > 'Z')
    cmpt -=32; // mise en majuscule
  switch (histoIdx)
  {
    case 1 : return m_MaxBkt1[cmpt - 'A'];
	case 2 : return m_MaxBkt2[cmpt - 'A'];
	case 3 : return m_MaxBkt3[cmpt - 'A'];
  }
  return 0;
}

CRecDataGraph::CRecDataGraph()
{
  pCmptEvol = NULL;
  pBktEvol = NULL;
  Revenu1 = INVALID_GRAPH_VALUE;
  Revenu2 = INVALID_GRAPH_VALUE;
  Revenu3 = INVALID_GRAPH_VALUE;
  HoiTra1 = false;
  HoiTra2 = false;
  HoiTra3 = false;
  RevTempsReel = INVALID_GRAPH_VALUE;
  rrd_index = -1;
  Prevision = -9999;
  pCmptPrevision = NULL;
}

void CRecDataGraph::AddPrevision (double prevoi)
{
  if (Prevision == -9999)
    Prevision = prevoi;
  else
    Prevision += prevoi;
}

double CRecDataGraph::GetPrevision()
{
  if (Prevision != -9999)
    return Prevision;
  return INVALID_GRAPH_VALUE;
}


/////////////////////////////////////////////////////////////////////////////
// CRecDHistAltView

IMPLEMENT_DYNCREATE(CRecDHistAltView, CRecDHistView)

CRecDHistAltView::CRecDHistAltView()
:CRecDHistView(TRUE)
{
}

void CRecDHistAltView::OnInitialUpdate() 
{
  ((CChildFrame*)GetParentFrame())->SetAltFlag(TRUE);

  CRecDHistView::OnInitialUpdate();  // base class method
}

/////////////////////////////////////////////////////////////////////////////
// CRecDHistView

IMPLEMENT_DYNCREATE(CRecDHistView, CView)

CRecDHistView::CRecDHistView(BOOL bAlt /*=FALSE*/)
{
	//if (bAlt)
	//	((CChildFrame*)GetParentFrame())->SetAltFlag(TRUE);
	if (((CCTAApp*)APP)->GetRhealysFlag() || !bAlt)
		m_bCmptPrevisionRevenuDataValid = 0;
	else
		m_bCmptPrevisionRevenuDataValid = 1;
    _bInitialized = FALSE;
    _bHasPaneDate = FALSE;
    _nSubViewID = 0;
	this->_AltFlag = bAlt;

    // set this view as the owner view in the chart object
    m_CmptChart.SetParentView(this);  
    m_BktChart.SetParentView(this);  
    m_CmptChart.SetCurScaling(CCTAChart::eMaxScaleIdx);
    m_BktChart.SetCurScaling(CCTAChart::eMaxScaleIdx);

  m_bCmptDataValid = FALSE;  // compartment data is invalid
  m_bBktDataValid  = FALSE;  // bucket data is invalid
  m_bCmptAvgDataValid = FALSE;  // Average compartment data is invalid
  m_bBktAvgDataValid  = FALSE;  // Average bucket data is invalid
  m_bHistoDataValid  = FALSE;  // historical data is invalid
  m_bPrevisionValid  = FALSE;  // prevision data is invalid
  m_bTabResized = FALSE;

  // select the cmpt chart by default
  m_iCurTabSelection = 0;
  m_pCurSelChart = &m_CmptChart;
  m_pNotSelChart = &m_BktChart;

  m_pPaneFont = new CFont();
  m_pPaneFont->CreateFont(
    0xfffffff8, // int nHeight
    0, // int nWidth
    0, // int nEscapement
    0, // int nOrientation
    FW_DONTCARE, // int nWeight
    0, // BYTE bItalic
    FALSE, // BYTE bUnderline
    0, // BYTE cStrikeOut
    ANSI_CHARSET, // BYTE nCharSet
    OUT_DEFAULT_PRECIS, // BYTE nOutPrecision
    CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision
    DEFAULT_QUALITY, // BYTE nQuality
    DEFAULT_PITCH | FF_SWISS, // BYTE nPitchAndFamily
    "MS Sans Serif");	// LPCTSTR lpszFacename

  // set the offsets for each curve on the graph
  int iOffset = 0;
  eCmptResHoldTot = iOffset++;
  eCmptResHoldIdv = iOffset++;
  eCmptResHoldAvg = iOffset++;
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
  {
    eCmptHoiTra = iOffset++;
	eCmptHoiTraIdv = iOffset++;
  }
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
  {
	// renverser par rapport à l'ordre normal pour forcer affichage de la plus récente au dessus 
    eCmptHisto3ResHoldTot = iOffset++;
    eCmptHisto2ResHoldTot = iOffset++;
	eCmptHisto1ResHoldTot = iOffset++;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
    {
      eCmptHisto3HoiTra = iOffset++;
	  eCmptHisto2HoiTra = iOffset++;
	  eCmptHisto1HoiTra = iOffset++;
    }
  }
  if (!((CCTAApp*)APP)->GetRhealysFlag() && !bAlt)
    eCmptPrevision = iOffset++;

  eCmptNumDataSets = iOffset++;

  iOffset = 0;

  eBktResHoldTot = iOffset++;
  eBktResHoldIdv = iOffset++;
  eBktResHoldAvg = iOffset++;
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
  {
    eBktHoiTra = iOffset++;
	eBktHoiTraIdv = iOffset++;
  }
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
  {
    eBktHisto3ResHoldTot = iOffset++;
    eBktHisto2ResHoldTot = iOffset++;
    eBktHisto1ResHoldTot = iOffset++;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
    {
      eBktHisto3HoiTra = iOffset++;
	  eBktHisto2HoiTra = iOffset++;
	  eBktHisto1HoiTra = iOffset++;
    }
  }
  //if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
  //  eBktPrevision = iOffset++;

  eBktNumDataSets = iOffset++;

  m_bRestored = FALSE;
  mp_HistoData = NULL;

  SetBoolSet (((CCTAApp*)APP)->m_RECDBoolSet);
}

CRecDHistView::~CRecDHistView()
{
  m_CmptChart.ClearChartDataArray();  // delete all graph datasets
  m_BktChart.ClearChartDataArray();  // delete all graph datasets
  if (m_pPaneFont)
	delete m_pPaneFont;
  if (mp_HistoData)
    delete mp_HistoData;
  EmptyInvalidHistoDates();
}

BEGIN_MESSAGE_MAP(CRecDHistView, CView)
	//{{AFX_MSG_MAP(CRecDHistView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(TCN_SELCHANGE, IDC_RecDHIST_TABCTRL, OnChartSelchange)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_SCALE_MAX, OnScaleMax)
	ON_UPDATE_COMMAND_UI(ID_SCALE_MAX, OnUpdateScaleMax)
	ON_COMMAND(ID_SCALE_AUTHORIZATIONS, OnScaleAuth)
	ON_UPDATE_COMMAND_UI(ID_SCALE_AUTHORIZATIONS, OnUpdateScaleAuth)
	ON_COMMAND(ID_SCALE_RESHOLD, OnScaleResHold)
	ON_UPDATE_COMMAND_UI(ID_SCALE_RESHOLD, OnUpdateScaleResHold)
	ON_COMMAND(ID_GRAPH_FLASH_LABEL,OnGraphDisplayFlashLabels)
	ON_COMMAND(ID_GRAPH_ROTATE_LABELS,OnGraphRotateLabels)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_ROTATE_LABELS,OnUpdateGraphRotateLabels)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_FLASH_LABEL,OnUpdateGraphDisplayFlashLabels)
	//ON_COMMAND(ID_RecDHIST_REVWIN,OnGraphDisplayRecDRevenue)
	//ON_UPDATE_COMMAND_UI(ID_RecDHIST_REVWIN,OnUpdateGraphDisplayRecDRevenue)
    ON_COMMAND(ID_PRINT, OnPrint)
    ON_UPDATE_COMMAND_UI(ID_PRINT, OnUpdateGen)
    ON_MESSAGE(WM_REFRESH_ALL, OnRefreshAll)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// Get the CTA data object from the tab item
//
CString CRecDHistView::GetTextFromTabItem(int iTabIdx)
{
  char szTabText[50];
  TC_ITEM TabCtrlItem;
  TabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
  TabCtrlItem.pszText = szTabText;
  TabCtrlItem.cchTextMax = sizeof(szTabText);
  GetTabCtrl()->GetItem( iTabIdx, &TabCtrlItem );
  return (szTabText);
}

///////////////////////////////////////////////////////////////////////////////
// Get the CTA data object from the tab item
//
CCTAChartData* CRecDHistView::GetDataFromTabItem()
{
  char szTabText[50];
  TC_ITEM TabCtrlItem;
  TabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
  TabCtrlItem.pszText = szTabText;
  TabCtrlItem.cchTextMax = sizeof(szTabText);
  GetTabCtrl()->GetItem( m_iCurTabSelection, &TabCtrlItem );
  m_szCurTabText = szTabText;
  LPARAM lParam = TabCtrlItem.lParam;
  return (CCTAChartData*)lParam;
}

/////////////////////////////////////////////////////////////////////////////
// CRecDHistView drawing

void CRecDHistView::OnDraw(CDC* pDC)
{
    ASSERT_VALID(GetDocument());

    YM_RecordSet* pCmptEvolSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_REC_ALT_KEY : CMPTEVOL_REC_KEY);
    YM_RecordSet* pBktEvolSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_REC_ALT_KEY : BKTEVOL_REC_KEY);
	YM_RecordSet* pCmptEvolAvgSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_REC_ALT_KEY : CMPTEVOL_AVG_REC_KEY);
    YM_RecordSet* pBktEvolAvgSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_REC_ALT_KEY : BKTEVOL_AVG_REC_KEY);

	YM_RecordSet* pHistoEvolSet = NULL;
    if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
	pHistoEvolSet = YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_REC_ALT_KEY : HISTOEVOL_REC_KEY);

	YM_RecordSet* pHistoEvolCmptSet = NULL;
    if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
	pHistoEvolCmptSet = YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_RECCMPT_ALT_KEY : HISTOEVOL_RECCMPT_KEY);


  // if record set is valid and we have data 
  if( 
    pCmptEvolSet->IsValid() && 
    pBktEvolSet->IsValid() && 
	pCmptEvolAvgSet->IsValid() && 
    pBktEvolAvgSet->IsValid() && 
    m_pCurSelChart->GetChartDataArray()->GetSize() &&
	( ! ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() || pHistoEvolSet->IsValid() ) )
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

void CRecDHistView::OnPaint() 
{
    CPaintDC dc(this); // device context for painting

    COLORREF clrBk;
	COLORREF clrFt;

	clrBk = (GetAltFlag()) ? 
	  ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextRecBkAlt() : 
	  ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextRecBkMain();
	
	clrFt = (GetAltFlag()) ? 
	  ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkAlt() : 
	  ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();


  // en cas de split, forcer systématiqument la rotation.
  DoGraphRotateLabels(((CCTAApp*)APP)->m_bRRDRotatelabel);
  /*if (m_CmptChart.GetBackgroundColor() != clrBk)  // did user change the background color ?
  {*/
    // set the area under the curve the same as the graph's background color
    m_CmptChart.SetDataStyleColor(CRecDHistChart::eResHoldStdvLowStyleIdx, clrBk);
    m_CmptChart.SetDataStyleColor(CRecDHistChart::eRevAvgFcstLowStyleIdx, clrBk);
    m_BktChart.SetDataStyleColor(CRecDHistChart::eResHoldStdvLowStyleIdx, clrBk);
    m_BktChart.SetDataStyleColor(CRecDHistChart::eRevAvgFcstLowStyleIdx, clrBk);
    m_CmptChart.SetGraphProperties(CCTAChartData::eCmpt);
    m_BktChart.SetGraphProperties(CCTAChartData::eBkt);
  //}	
  m_CmptChart.SetBackgroundColor (clrBk);
  m_BktChart.SetBackgroundColor  (clrBk);
  m_CmptChart.SetForegroundColor (clrFt);
  m_BktChart.SetForegroundColor  (clrFt);

  if (this->GetCmptChart() != NULL && this->GetCmptChart()->GetRedraw())
  {
	  this->GetCmptChart()->RedrawHelp();
  }

  // Do not call CView::OnPaint() for painting messages
}

/////////////////////////////////////////////////////////////////////////////
// CRecDHistView diagnostics

#ifdef _DEBUG
void CRecDHistView::AssertValid() const
{
	CView::AssertValid();
}

void CRecDHistView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCTADoc* CRecDHistView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCTADoc)));
	return (CCTADoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRecDHistView message handlers

int CRecDHistView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  // create and adjust the tab control
  CRect rect;
  GetClientRect(&rect);
  m_ChartTabCtrl.Create( CTA_TAB_STYLE, rect, this, IDC_RecDHIST_TABCTRL);
  GetClientRect(&rect);
  m_ChartTabCtrl.AdjustRect( FALSE, &rect );

  // create 2 chart windows when the view is created
  BOOL bRet;
  bRet = m_CmptChart.Create("", WS_CHILD, rect, &m_ChartTabCtrl, 0);
  bRet = m_BktChart.Create ("", WS_CHILD, rect, &m_ChartTabCtrl, 0);

  // set some chart properties
  m_CmptChart.SetGraphProperties(CCTAChartData::eCmpt);
  m_BktChart.SetGraphProperties(CCTAChartData::eBkt);

  return 0;
}


void CRecDHistView::FrameSizeChange (UINT nType, int cx, int cy)
{
  // if window has been minimized, we must destroy the help dialogs
  if ( nType == SIZE_MINIMIZED )
  {
    m_CmptChart.DestroyHelpDlg();
    m_BktChart.DestroyHelpDlg();
    m_bRestored = FALSE;
  }
  else if ( nType == SIZE_RESTORED )
  {
    if (! m_bRestored)
    {
      CRect rect;
      m_ChartTabCtrl.GetWindowRect(&rect);
      m_ChartTabCtrl.AdjustRect(TRUE, &rect);
      m_ChartTabCtrl.MoveWindow(&rect);
      m_bRestored = TRUE;
    }
  }

  YM_RecordSet* pBktEvolSet = 
    YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_REC_ALT_KEY : BKTEVOL_REC_KEY);
  ASSERT( pBktEvolSet != NULL );

  if ((nType == SIZE_MINIMIZED) && pBktEvolSet->IsOpen())
    pBktEvolSet->Close(); 
  else  if ((nType != SIZE_MINIMIZED) && !pBktEvolSet->IsOpen())
    pBktEvolSet->Open();

  YM_RecordSet* pBktEvolAvgSet = 
    YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_REC_ALT_KEY : BKTEVOL_AVG_REC_KEY);
  ASSERT( pBktEvolAvgSet != NULL );

  if ((nType == SIZE_MINIMIZED) && pBktEvolAvgSet->IsOpen())
    pBktEvolAvgSet->Close(); 
  else  if ((nType != SIZE_MINIMIZED) && !pBktEvolAvgSet->IsOpen())
    pBktEvolAvgSet->Open();

  YM_RecordSet* pCmptEvolSet = 
    YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_REC_ALT_KEY : CMPTEVOL_REC_KEY);
  ASSERT( pCmptEvolSet != NULL );

  if ((nType == SIZE_MINIMIZED) && pCmptEvolSet->IsOpen())
    pCmptEvolSet->Close();
  else if ((nType != SIZE_MINIMIZED) && !pCmptEvolSet->IsOpen())
    pCmptEvolSet->Open();

  YM_RecordSet* pCmptEvolAvgSet = 
    YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_REC_ALT_KEY : CMPTEVOL_AVG_REC_KEY);
  ASSERT( pCmptEvolAvgSet != NULL );

  if ((nType == SIZE_MINIMIZED) && pCmptEvolAvgSet->IsOpen())
    pCmptEvolAvgSet->Close();
  else if ((nType != SIZE_MINIMIZED) && !pCmptEvolAvgSet->IsOpen())
    pCmptEvolAvgSet->Open();

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
  {
	YM_RecordSet* pHistoEvolSet =
	  YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_REC_ALT_KEY : HISTOEVOL_REC_KEY);
	ASSERT( pHistoEvolSet != NULL );

	if ( (nType == SIZE_MINIMIZED) && pHistoEvolSet->IsOpen() )
      pHistoEvolSet->Close();
    else if ((nType != SIZE_MINIMIZED) && !pHistoEvolSet->IsOpen())
      pHistoEvolSet->Open();

	YM_RecordSet* pHistoEvolCmptSet =
	  YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_RECCMPT_ALT_KEY : HISTOEVOL_RECCMPT_KEY);
	ASSERT( pHistoEvolCmptSet != NULL );

	if ( (nType == SIZE_MINIMIZED) && pHistoEvolCmptSet->IsOpen() )
      pHistoEvolCmptSet->Close();
    else if ((nType != SIZE_MINIMIZED) && !pHistoEvolCmptSet->IsOpen())
      pHistoEvolCmptSet->Open();
  }
}

void CRecDHistView::OnShowWindow(BOOL bShow, UINT nStatus)
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

void CRecDHistView::GetDayTitle(CString& szDayTitle)
{
    CString szTitle = ((CCTAApp*)APP)->GetResource()->LoadResString((GetAltFlag()) ? IDS_RECDHIST_SCI_TITLE : IDS_RECDHIST_TITLE);
    YmIcTrancheLisDom* pTranche = 
          YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	RWDate today;
	int iRrdVal = pTranche->DptDate() - today.julian() +1;
	if (iRrdVal > 0)
	  szTitle += '-';
	else if (iRrdVal < 0)
	{
	  szTitle += '+';
	  iRrdVal = -iRrdVal;
	}
    szDayTitle.Format("%s%d", szTitle, iRrdVal);
}

void CRecDHistView::OnInitialUpdate() 
{
    CView::OnInitialUpdate();
    if (getOtherView())
	_nSubViewID = 1;

    ASSERT_VALID(GetDocument());

  {
    CString szWndTitle = "";
    GetDayTitle(szWndTitle);
	CString szWndKey;
    GetParentFrame()->GetWindowText(szWndKey);
    GetParentFrame()->SetWindowText(szWndTitle);
    ((YM_PersistentChildWnd*)GetParentFrame())->SetChildKey (szWndKey);

    if (!_nSubViewID)
	((YM_PersistentChildWnd*)GetParentFrame())->SetChildView (this);
    else {
	CString szProfileKey;
	((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);
	_sLastTabUsed = APP->GetProfileString(szProfileKey, "LastTabUsed_2", "" );
    }
  }

  YM_RecordSet* pCmptEvolSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_REC_ALT_KEY : CMPTEVOL_REC_KEY);
  ASSERT( pCmptEvolSet != NULL );
  YM_RecordSet* pBktEvolSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_REC_ALT_KEY : BKTEVOL_REC_KEY);
  ASSERT( pBktEvolSet  != NULL );
  YM_RecordSet* pCmptEvolAvgSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_REC_ALT_KEY : CMPTEVOL_AVG_REC_KEY);
  ASSERT( pCmptEvolAvgSet != NULL );
  YM_RecordSet* pBktEvolAvgSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_REC_ALT_KEY : BKTEVOL_AVG_REC_KEY);
  ASSERT( pBktEvolAvgSet  != NULL );
  YM_RecordSet* pCmptPrevisionRevenuSet = NULL;
  if (!((CCTAApp*)APP)->GetRhealysFlag())
	pCmptPrevisionRevenuSet = YM_Set<YmIcPrevisionDom>::FromKey(PREVISION_REVENU_KEY);


  // attempt to open the record sets
	if( ! pCmptEvolSet->IsOpen() )
		pCmptEvolSet->Open();

	if( ! pBktEvolSet->IsOpen() )
		pBktEvolSet->Open();

	if( ! pCmptEvolAvgSet->IsOpen() )
		pCmptEvolAvgSet->Open();

	if( ! pBktEvolAvgSet->IsOpen() )
		pBktEvolAvgSet->Open();

	if (!((CCTAApp*)APP)->GetRhealysFlag() && pCmptPrevisionRevenuSet != NULL && !pCmptPrevisionRevenuSet->IsOpen())
		pCmptPrevisionRevenuSet->Open();


	// ask for notification from the RecordSets
	pCmptEvolSet->AddObserver(this);
	pBktEvolSet->AddObserver(this);
	pCmptEvolAvgSet->AddObserver(this);
	pBktEvolAvgSet->AddObserver(this);
	if (!((CCTAApp*)APP)->GetRhealysFlag())
		pCmptPrevisionRevenuSet->AddObserver(this);

  OnUpdate( this, 0L, pCmptEvolSet );
  OnUpdate( this, 0L, pBktEvolSet );
  OnUpdate( this, 0L, pCmptEvolAvgSet );
  OnUpdate( this, 0L, pBktEvolAvgSet );
  if (!((CCTAApp*)APP)->GetRhealysFlag())
	OnUpdate( this, 0L, pCmptPrevisionRevenuSet );

  if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
  {
    YM_RecordSet* pHistoHoiTraISet = YM_Set<YmIcHistoEvolDom>::FromKey(HISTO_HOITRAI_KEY);
    ASSERT(pHistoHoiTraISet != NULL); 
    if (! pHistoHoiTraISet->IsOpen())
      pHistoHoiTraISet->Open();
    //pHistoHoiTraISet->AddObserver(this);

	YM_RecordSet* pHistoHoiTraRSet = YM_Set<YmIcHistoEvolDom>::FromKey(HISTO_HOITRAR_KEY);
    ASSERT(pHistoHoiTraRSet != NULL); 
    if (! pHistoHoiTraRSet->IsOpen())
      pHistoHoiTraRSet->Open();
    //pHistoHoiTraRSet->AddObserver(this);
  }

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
  {
    YM_RecordSet* pHistoEvolSet = 
      YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_REC_ALT_KEY : HISTOEVOL_REC_KEY);
  	ASSERT( pHistoEvolSet != NULL );

	  if( ! pHistoEvolSet->IsOpen() )
		  pHistoEvolSet->Open();
  	pHistoEvolSet->AddObserver( this );
    OnUpdate( this, 0L, pHistoEvolSet );

	YM_RecordSet* pHistoEvolCmptSet = 
      YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_RECCMPT_ALT_KEY : HISTOEVOL_RECCMPT_KEY);
  	ASSERT( pHistoEvolCmptSet != NULL );

	  if( ! pHistoEvolCmptSet->IsOpen() )
		  pHistoEvolCmptSet->Open();
  	pHistoEvolCmptSet->AddObserver( this );
    OnUpdate( this, 0L, pHistoEvolCmptSet );

	enablePaneDate();
  }

  /*if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
  { // Les previsions, le Open est fait par la fenêtre Resarail
    YM_RecordSet* pPrevisionSet = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY);
	ASSERT( pPrevisionSet != NULL );
	pPrevisionSet->AddObserver( this );
	OnUpdate( this, 0L, pPrevisionSet );
  }*/

   _bInitialized = TRUE;

   // Initialisation des booleans d'affichage. A l'ouverture de la fenêtre, tout est TRUE.
   switch (((CCTAApp*)APP)->m_RRDdisplayMode)
   {
     case RRDDISPLAYMEC:
	 {
       m_bDispHisto1ResHoldTot = FALSE;
       m_bDispHisto2ResHoldTot = FALSE;
       m_bDispHisto3ResHoldTot = FALSE;
	   break;
	 }
	 case RRDDISPLAYECH:
	 {
	   m_bDispResHoldAvg = FALSE;
	 }
	 case RRDDISPLAYHIS:
	 {
	   m_bDispResHoldAvg = FALSE;
	   m_bDispEC = FALSE;
	 }
   }
}

BOOL CRecDHistView::DrawSBItem( YM_StatusBar* sb, LPDRAWITEMSTRUCT lpDrawItemStruct, LPVOID pData )
{
    CRecDHistView* This = (CRecDHistView*)pData;
    COLORREF cr = RGB(0,0,0);
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	// inversion des couleurs pour mettre la plus récente à droite.
    switch (lpDrawItemStruct->itemID) {
	case 1:	cr = pDoc->m_pGlobalSysParms->GetGraphRGBValue(CCTAChart::eHisto3ResHoldTotStyleIdx);  break;
	case 2:	cr = pDoc->m_pGlobalSysParms->GetGraphRGBValue(CCTAChart::eHisto2ResHoldTotStyleIdx);  break;
	case 3:	cr = pDoc->m_pGlobalSysParms->GetGraphRGBValue(CCTAChart::eHisto1ResHoldTotStyleIdx);  break;
    }

    // Attach to a CDC object
    CDC dc;
    dc.Attach(lpDrawItemStruct->hDC);

    // Get the pane rectangle and calculate text coordinates
    CRect rect(&lpDrawItemStruct->rcItem);

    dc.SetTextColor( cr );
    dc.SetBkMode( TRANSPARENT );
    CString s = sb->GetPaneText( lpDrawItemStruct->itemID );
    dc.DrawText( s, rect, DT_CENTER | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER );

    // Detach from the CDC object, otherwise the hDC will be
    // destroyed when the CDC object goes out of scope
    dc.Detach();

    return TRUE;
}

void CRecDHistView::enablePaneDate( BOOL bEnable /*=TRUE*/ )
{
    YM_StatusBar* sb = ((CChildFrame*)GetParentFrame())->GetStatusBar();
    if (sb && sb->m_hWnd) {
	if (_bHasPaneDate) {
	    UINT uT[] = { 1, 2, 3, 4 };
	    sb->SetIndicators( uT, 4 );
	    sb->SetPaneInfo( 0, 0, SBPS_STRETCH|SBPS_NOBORDERS, WIDTH_PANE_INFO );
	}
	else {
	    CDC* dc = sb->GetDC();
	    CSize s = dc->GetTextExtent( "00/00/000" );

	    UINT uT[] = { 1, 2, 3, 4 };
	    sb->SetIndicators( uT, 4 );
	    sb->SetPaneInfo( 0, 0, SBPS_STRETCH|SBPS_NOBORDERS, WIDTH_PANE_INFO );
	    sb->SetPaneInfo( 1, 0, SBPS_NORMAL|SBPS_OWNERDRAW,  s.cx );
	    sb->SetPaneInfo( 2, 0, SBPS_NORMAL|SBPS_OWNERDRAW,  s.cx );
		sb->SetPaneInfo( 3, 0, SBPS_NORMAL|SBPS_OWNERDRAW,  s.cx );
	    sb->setDrawItemProc( DrawSBItem, this );
	    sb->Invalidate();
	    Invalidate();
	}
    }
}

void CRecDHistView::setPaneDateText( int iPane, LPCTSTR pszText )
{
    YM_StatusBar* sb = ((CChildFrame*)GetParentFrame())->GetStatusBar();
	BOOL bpd = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag();
    if (bpd && sb && sb->m_hWnd && (iPane == 1 || iPane == 2 || iPane == 3))
	{
	  sb->SetFont (m_pPaneFont);
	  sb->SetPaneText( iPane, "??? ???" );
	  sb->SetPaneText( iPane, pszText );
	  sb->Invalidate();
	}
}

void CRecDHistView::OnChartSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // set the member variable which holds the currently selected tab...
    m_iCurTabSelection = m_ChartTabCtrl.GetCurSel();  // get currently selected tab
    m_szCurTabText = GetTextFromTabItem(m_iCurTabSelection);
    _sLastTabUsed = m_szCurTabText.Left(1);	// Only keep cmpt name 

    CCTAChartData* pCTAChartData = GetDataFromTabItem();
    if ( pCTAChartData == NULL )
        return;

    COLORREF clrFt;
	clrFt = (GetAltFlag()) ? 
	  ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkAlt() : 
	  ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();

    // which chart is being selected (cmpt or bkt) ?
    if ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt )
    {
	m_pCurSelChart = &m_CmptChart;
	m_pNotSelChart = &m_BktChart;
	m_CmptChart.SetForegroundColor (clrFt);
    /*m_CmptChart.SetYTitle("€uros");
	m_CmptChart.SetYTitleRotation(XRT_ROTATE_90);*/
    }
    else
    {
	m_pCurSelChart = &m_BktChart;
	m_pNotSelChart = &m_CmptChart;
	m_BktChart.SetForegroundColor  (clrFt);
   /* m_BktChart.SetYTitle("€uros");
	m_BktChart.SetYTitleRotation(XRT_ROTATE_90);*/
    }

    // set the appropriate data object as currently selected, then get it
    m_pCurSelChart->SetCurChartData( pCTAChartData );
    DrawGraph();  // draw the graph

    UpdateHelpDialog();

    m_pCurSelChart->RemoveFlashLabel();  // remove flash label if up

    *pResult = 0;
}


long CRecDHistView::OnRefreshAll (UINT wParam, LONG lParam )
{
	RefreshAll();
	return 0;
}

void CRecDHistView::RefreshAll(BOOL both /* TRUE*/)
{
  BuildCmptGraphs();
  BuildBktGraphs();
  if ( m_bCmptDataValid && m_bBktDataValid && m_bCmptAvgDataValid && m_bBktAvgDataValid &&
	   ( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() || m_bHistoDataValid ) &&
	   ( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() || m_bPrevisionValid) )
  {
    BuildTabs();
    ResizeChart();
    Invalidate(FALSE);
    UpdateWindow();
  }

  if (both)
  {
	CRecDHistView* pView = getOtherView();
	if (pView)
	  pView->RefreshAll (FALSE);
  }
}

void CRecDHistView::ChangeRecDDisplay(BOOL bOnF5)
{
  if (bOnF5)
  { 
	CString sb = RDISPLAYALL;
    switch (((CCTAApp*)APP)->m_RRDdisplayMode)
    {
     case RRDDISPLAYMEC:
	 {
	    sb = RDISPLAYMEC;
	   break;
	 }
	 case RRDDISPLAYECH:
	 {
	   sb = RDISPLAYECH;
	   break;
	 }
	 case RRDDISPLAYHIS:
	 {
	   sb = RDISPLAYHIS;
	   break;
	 }
    }
    SetBoolSet (sb);
  }
  RefreshAll (FALSE);
  
  CString sbb = GetBoolSet();
  ((CCTAApp*)APP)->m_RECDBoolSet = sbb;

  // Même chose sur l'autre, si elle existe
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CView* pView; 
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
    // do we have a spliter window activated (i.e., 2 views ?)
    if( pView->IsKindOf( RUNTIME_CLASS(CRecDHistView) ) && pView != this ) 
    {
      CString sbbis = ((CRecDHistView*)pView)->GetBoolSet();
	  if (sbbis != sbb)
	  {
        ((CRecDHistView*)pView)->SetBoolSet(sbb);
        ((CRecDHistView*)pView)->ChangeRecDDisplay(FALSE);
	  }
    }
  }
}

void CRecDHistView::RevMasqueCourbe (int graphtype, int set, BOOL b2ndSet)
{
  if (b2ndSet)
  {
	m_bDispEC = !m_bDispEC;
	return;
  }
  if (graphtype == CCTAChartData::eBkt)
  {
	if (set == eBktResHoldTot)
	  m_bDispResHoldTot = !m_bDispResHoldTot;
	if (set == eBktResHoldIdv)
	  m_bDispResHoldIdv = !m_bDispResHoldIdv;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	{
	  if (set == eBktHoiTra)
	    m_bDispResHoldTot = !m_bDispResHoldTot;
	  if (set == eBktHoiTraIdv)
	    m_bDispResHoldIdv = !m_bDispResHoldIdv;
	}
    if (set == eBktResHoldAvg)
	  m_bDispResHoldAvg = !m_bDispResHoldAvg;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	{
      if (set == eBktHisto3ResHoldTot)
		m_bDispHisto3ResHoldTot = !m_bDispHisto3ResHoldTot;
      if (set == eBktHisto2ResHoldTot)
		m_bDispHisto2ResHoldTot = !m_bDispHisto2ResHoldTot;
      if (set == eBktHisto1ResHoldTot)
		m_bDispHisto1ResHoldTot = !m_bDispHisto1ResHoldTot;
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	  {
	    if (set == eBktHisto3HoiTra)
		  m_bDispHisto3ResHoldTot = !m_bDispHisto3ResHoldTot;
		if (set == eBktHisto2HoiTra)
		  m_bDispHisto2ResHoldTot = !m_bDispHisto2ResHoldTot;
		if (set == eBktHisto1HoiTra)
		  m_bDispHisto1ResHoldTot = !m_bDispHisto1ResHoldTot;
	  }
	}
	if (!((CCTAApp*)APP)->GetRhealysFlag() && !this->GetAltFlag())
	  if (set == eBktPrevision)
	    m_bDispPrevision = !m_bDispPrevision;
  }
  else if (graphtype == CCTAChartData::eCmpt)
  {
	if (set == eCmptResHoldTot)
	  m_bDispResHoldTot = !m_bDispResHoldTot;
	if (set == eCmptResHoldIdv)
	  m_bDispResHoldIdv = !m_bDispResHoldIdv;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	{
	  if (set == eCmptHoiTra)
	    m_bDispResHoldTot = !m_bDispResHoldTot;
	  if (set == eCmptHoiTraIdv)
	    m_bDispResHoldIdv = !m_bDispResHoldIdv;
	}
	if (set == eCmptResHoldAvg)
	  m_bDispResHoldAvg = !m_bDispResHoldAvg;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	{
	  if (set == eCmptHisto1ResHoldTot)
		m_bDispHisto1ResHoldTot = !m_bDispHisto1ResHoldTot;
	  if (set == eCmptHisto2ResHoldTot)
		m_bDispHisto2ResHoldTot = !m_bDispHisto2ResHoldTot;
 	  if (set == eCmptHisto3ResHoldTot)
		m_bDispHisto3ResHoldTot = !m_bDispHisto3ResHoldTot;
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	  {
	    if (set == eCmptHisto1HoiTra)
		  m_bDispHisto1ResHoldTot = !m_bDispHisto1ResHoldTot;
		if (set == eCmptHisto2HoiTra)
		  m_bDispHisto2ResHoldTot = !m_bDispHisto2ResHoldTot;
		if (set == eCmptHisto3HoiTra)
		  m_bDispHisto3ResHoldTot = !m_bDispHisto3ResHoldTot;
	  }
	}
	if (!((CCTAApp*)APP)->GetRhealysFlag() && !this->GetAltFlag())
	  if (set == eCmptPrevision)
	    m_bDispPrevision = !m_bDispPrevision;
  }
}

void CRecDHistView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
    ASSERT_VALID(GetDocument());
      
	

    YM_RecordSet* pCmptEvolSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_REC_ALT_KEY : CMPTEVOL_REC_KEY);
    YM_RecordSet* pBktEvolSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_REC_ALT_KEY : BKTEVOL_REC_KEY);
	YM_RecordSet* pCmptEvolAvgSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_REC_ALT_KEY : CMPTEVOL_AVG_REC_KEY);
    YM_RecordSet* pBktEvolAvgSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_REC_ALT_KEY : BKTEVOL_AVG_REC_KEY);

	YM_RecordSet* pHistoEvolSet = NULL;
    if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	pHistoEvolSet = YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_REC_ALT_KEY : HISTOEVOL_REC_KEY);

	YM_RecordSet* pHistoEvolCmptSet = NULL;
    if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	pHistoEvolCmptSet = YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_RECCMPT_ALT_KEY : HISTOEVOL_RECCMPT_KEY);

	/*YM_RecordSet* pPrevisionSet = NULL;
    if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
	pPrevisionSet = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY);*/

	YM_RecordSet* pCmptPrevisionRevenuSet = NULL;
	if (!((CCTAApp*)APP)->GetRhealysFlag() && !GetAltFlag())
		pCmptPrevisionRevenuSet = YM_Set<YmIcPrevisionDom>::FromKey(PREVISION_REVENU_KEY);


	// If notification comes from a RecordSet, pHint is a RecordSet*
  if( pHint != NULL && pHint == pCmptEvolSet ) // compartment data set
  {
    m_CmptChart.DestroyHelpDlg();
	m_bCmptDataValid = pCmptEvolSet->IsValid();
    ( pCmptEvolSet->IsValid() ) ? BuildCmptGraphs() : ClearCmptGraphs(); 
	// BuildCmptGraphs contient un test pour attendre la totalité de donnés CMPT et historique
	EnableWindow( m_bCmptDataValid );
  }
  else if( pHint != NULL && pHint == pCmptEvolAvgSet ) // compartment data set
  {
    m_CmptChart.DestroyHelpDlg();
	m_bCmptAvgDataValid = pCmptEvolAvgSet->IsValid();
    ( pCmptEvolAvgSet->IsValid() ) ? BuildCmptGraphs() : ClearCmptGraphs(); 
	// BuildCmptGraphs contient un test pour attendre la totalité de donnés CMPT et historique
	EnableWindow( m_bCmptAvgDataValid );
  }
  else if ( pHint != NULL && pHint == pBktEvolSet )  // bucket data set
  {
    m_BktChart.DestroyHelpDlg();
	m_bBktDataValid = pBktEvolSet->IsValid();
    ( pBktEvolSet->IsValid() ) ? BuildBktGraphs() : ClearBktGraphs();   
	EnableWindow( m_bBktDataValid );
  }
  else if ( pHint != NULL && pHint == pBktEvolAvgSet )  // bucket data set
  {
    m_BktChart.DestroyHelpDlg();
	m_bBktAvgDataValid = pBktEvolAvgSet->IsValid();
    ( pBktEvolAvgSet->IsValid() ) ? BuildBktGraphs() : ClearBktGraphs();   
	EnableWindow( m_bBktAvgDataValid );
  }
  else if ( pHint != NULL && (pHint == pHistoEvolSet || pHint == pHistoEvolCmptSet))  // historiques data set
  {
	m_bHistoDataValid = (pHistoEvolSet->IsValid() && pHistoEvolCmptSet->IsValid());
	if (m_bHistoDataValid)
	{
	  if (mp_HistoData)
        delete mp_HistoData;
      mp_HistoData = new CRecHistoData (GetAltFlag());	  
	  
	  CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();

	  BuildCmptGraphs();
	  BuildBktGraphs();
	  
	}
  }
  else if (!((CCTAApp*)APP)->GetRhealysFlag() && pHint != NULL && pHint == pCmptPrevisionRevenuSet )  // bucket data set
  {
		//m_BktChart.DestroyHelpDlg();
		m_bCmptPrevisionRevenuDataValid = pCmptPrevisionRevenuSet->IsValid();
		if (pCmptPrevisionRevenuSet->IsValid())
			BuildCmptGraphs();
		//EnableWindow( m_bCmptPrevisionVenteDataValid );
  }
  /*else if ( pHint != NULL && pHint == pPrevisionSet )  // bucket data set
  {
	m_bPrevisionValid = pPrevisionSet->IsValid();
	if (m_bPrevisionValid)
	{
	 BuildCmptGraphs();
	 BuildBktGraphs();
	}
  }*/

  // build the tabs when all the data is ready
  if ( m_bCmptDataValid && m_bBktDataValid && m_bCmptAvgDataValid && m_bBktAvgDataValid &&
	   ( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() || m_bHistoDataValid ) &&
	   ( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() || m_bPrevisionValid ))
  {
    BuildTabs();
    ResizeChart();
  }
  YM_StatusBar* sb = ((CChildFrame*)GetParentFrame())->GetStatusBar();
  if (sb)
	sb->Invalidate();

}

void CRecDHistView::OnSize(UINT nType, int cx, int cy) 
{
    CView::OnSize(nType, cx, cy);
    // if window has been minimized, we must destroy the help dialogs
    if (nType == SIZE_MINIMIZED) {
	m_CmptChart.DestroyHelpDlg();
	m_BktChart.DestroyHelpDlg();
    }
	else 
	{
	  BOOL yaPaneDate = FALSE;
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
		    _bInitialized) yaPaneDate = TRUE;
	  YM_StatusBar* sb = ((CChildFrame*)GetParentFrame())->GetStatusBar();
      if (sb && sb->m_hWnd && !_bHasPaneDate && yaPaneDate)
	  {
	    CDC* dc = sb->GetDC();
	    CSize s = dc->GetTextExtent( "00/00/000" );
		if (cx > WIDTH_PANE_INFO + 3*s.cx)
		{
	      sb->SetPaneInfo( 0, 0, SBPS_STRETCH|SBPS_NOBORDERS, WIDTH_PANE_INFO );
	      sb->SetPaneInfo( 1, 0, SBPS_NORMAL|SBPS_OWNERDRAW,  s.cx );
	      sb->SetPaneInfo( 2, 0, SBPS_NORMAL|SBPS_OWNERDRAW,  s.cx );
          sb->SetPaneInfo( 3, 0, SBPS_NORMAL|SBPS_OWNERDRAW,  s.cx );
		}
		else if (cx < WIDTH_PANE_INFO)
		{
		  sb->SetPaneInfo( 0, 0, SBPS_STRETCH|SBPS_NOBORDERS, 0 );
	      sb->SetPaneInfo( 1, 0, SBPS_NORMAL|SBPS_OWNERDRAW,  (cx / 3) - 10);
	      sb->SetPaneInfo( 2, 0, SBPS_NORMAL|SBPS_OWNERDRAW,  (cx / 3) - 10);
          sb->SetPaneInfo( 3, 0, SBPS_NORMAL|SBPS_OWNERDRAW,  (cx / 3) + 20);
		}
		else if (cx < WIDTH_PANE_INFO + 3*s.cx)
		{
		  sb->SetPaneInfo( 0, 0, SBPS_STRETCH|SBPS_NOBORDERS,
			  (cx - 3*s.cx) > 0 ? (cx - 3*s.cx) : 0);
	      sb->SetPaneInfo( 1, 0, SBPS_NORMAL|SBPS_OWNERDRAW,  s.cx-10);
	      sb->SetPaneInfo( 2, 0, SBPS_NORMAL|SBPS_OWNERDRAW,  s.cx-10);
          sb->SetPaneInfo( 3, 0, SBPS_NORMAL|SBPS_OWNERDRAW,  s.cx+20);
		}

		sb->setDrawItemProc( DrawSBItem, this );
	    sb->Invalidate();

	  }
	}
    ResizeChart();
}

void CRecDHistView::OnScale (enum CCTAChart::GraphScaleIndex eGraphScale)
{
  m_CmptChart.ScaleGraph(eGraphScale);
  m_BktChart.ScaleGraph(eGraphScale);
  Invalidate(FALSE);  // force a redraw
  UpdateWindow();
} 

void CRecDHistView::OnScaleMax() 
{
    OnScale(CCTAChart::eMaxScaleIdx);
	CRecDHistView* pView = getOtherView();
    if (pView)
	  pView->OnScale (CCTAChart::eMaxScaleIdx);
}

void CRecDHistView::OnUpdateScaleMax(CCmdUI* pCmdUI) 
{
    OnUpdateGen (pCmdUI);
    pCmdUI->SetCheck( m_pCurSelChart->GetCurScaling() == CCTAChart::eMaxScaleIdx );
}

void CRecDHistView::OnScaleAuth() 
{
  OnScale(CCTAChart::eAuthScaleIdx);
  CRecDHistView* pView = getOtherView();
  if (pView)
	pView->OnScale (CCTAChart::eAuthScaleIdx);
}

void CRecDHistView::OnUpdateScaleAuth(CCmdUI* pCmdUI) 
{
    OnUpdateGen (pCmdUI);
    pCmdUI->SetCheck( m_pCurSelChart->GetCurScaling() == CCTAChart::eAuthScaleIdx );
}

void CRecDHistView::OnScaleResHold() 
{
  OnScale(CCTAChart::eResHoldScaleIdx);
  CRecDHistView* pView = getOtherView();
  if (pView)
	pView->OnScale (CCTAChart::eResHoldScaleIdx);
}

void CRecDHistView::OnUpdateScaleResHold(CCmdUI* pCmdUI) 
{
    OnUpdateGen (pCmdUI);
    pCmdUI->SetCheck( m_pCurSelChart->GetCurScaling() ==  CCTAChart::eResHoldScaleIdx );
}

void CRecDHistView::OnGraphDisplayFlashLabels() 
{
  // toggle the flag to display the graph flash labels
  ((CCTAApp*)APP)->SetFlashLabelFlag(  ! ((CCTAApp*)APP)->GetFlashLabelFlag() );
}

void CRecDHistView::OnUpdateGraphDisplayFlashLabels(CCmdUI* pCmdUI) 
{
  OnUpdateGen (pCmdUI);
  pCmdUI->SetCheck( ((CCTAApp*)APP)->GetFlashLabelFlag() );
}

void CRecDHistView::DoGraphRotateLabels(bool brot)
{
  m_CmptChart.SetRotateLabelsFlag( brot );
  m_CmptChart.GetRotateLabelsFlag() == true ? 
  m_CmptChart.SetXAnnotationRotation(XRT_ROTATE_90) : 
  m_CmptChart.SetXAnnotationRotation(XRT_ROTATE_NONE);

  m_BktChart.SetRotateLabelsFlag( brot );
  m_BktChart.GetRotateLabelsFlag() == true ? 
  m_BktChart.SetXAnnotationRotation(XRT_ROTATE_90) : 
  m_BktChart.SetXAnnotationRotation(XRT_ROTATE_NONE);
}

void CRecDHistView::OnGraphRotateLabels()
{
  bool brot = m_CmptChart.GetRotateLabelsFlag();
  ((CCTAApp*)APP)->m_bRRDRotatelabel = !brot;
  DoGraphRotateLabels(!brot);
  // Rotation de l'autre moitiée si elle existe
  CRecDHistView* pView = getOtherView();
  if (pView)
	pView->DoGraphRotateLabels(!brot);
}

void CRecDHistView::OnUpdateGraphRotateLabels( CCmdUI* pCmdUI )
{
  OnUpdateGen (pCmdUI);
  pCmdUI->SetCheck ( (m_CmptChart.GetRotateLabelsFlag()==1) ? 1 : 0 );
}


void CRecDHistView::OnPrint()
{
  CCTAChartData* pCTAChartData = m_pCurSelChart->GetCurChartData();
  if ( pCTAChartData == NULL )
    return;
  int iCurSelTab = m_ChartTabCtrl.GetCurSel();  // get currently selected tab

  // which graph should we draw ?
  (pCTAChartData->GetGraphType() == CCTAChartData::eCmpt) ? m_BktChart.Print() : m_CmptChart.Print();
}

void CRecDHistView::OnUpdateGen( CCmdUI* pCmdUI )
{
	YM_RecordSet* pCmptEvolSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_REC_ALT_KEY : CMPTEVOL_REC_KEY);
	YM_RecordSet* pCmptEvolAvgSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_REC_ALT_KEY : CMPTEVOL_AVG_REC_KEY);
	YM_RecordSet* pBktEvolSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_REC_ALT_KEY : BKTEVOL_REC_KEY);
	YM_RecordSet* pBktEvolAvgSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_REC_ALT_KEY : BKTEVOL_AVG_REC_KEY);

  ASSERT( pBktEvolSet != NULL );
  ASSERT( pCmptEvolSet != NULL );
  ASSERT( pBktEvolAvgSet != NULL );
  ASSERT( pCmptEvolAvgSet != NULL );

	pCmdUI->Enable( pCmptEvolSet->IsOpen() && pBktEvolSet->IsOpen() && pCmptEvolAvgSet->IsOpen() && pBktEvolAvgSet->IsOpen() );
}

void CRecDHistView::OnDestroy() 
{
  CView::OnDestroy();

  YM_PersistentChildWnd* pParentFrame = (YM_PersistentChildWnd*)GetParentFrame();
  if ( pParentFrame == NULL)
    return;

  // if we have another CRecDHistView instance, set it as the child view of the parent frame !
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CView* pView; 
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
    pView = pDoc->GetNextView( pos ); 
    // do we have a spliiter window activated (i.e., 2 views ?)
    if( pView->IsKindOf( RUNTIME_CLASS(CRecDHistView) ) && pView != this ) 
    {
	  /*ano 147777*/ CRecDHistView* pRecDview = (CRecDHistView*)pView;
      pParentFrame->SetChildView (pRecDview);
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
   CRecDHistView* pRecDView = getOtherView();
 
  YM_RecordSet* pCmptEvolSet = 
    YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_REC_ALT_KEY : CMPTEVOL_REC_KEY);
  ASSERT( pCmptEvolSet != NULL );

  YM_RecordSet* pBktEvolSet = 
    YM_Set<CRRDHistBktEvol>::FromKey((GetAltFlag()) ? BKTEVOL_REC_ALT_KEY : BKTEVOL_REC_KEY);
  ASSERT( pBktEvolSet != NULL );

  YM_RecordSet* pCmptEvolAvgSet = 
    YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_REC_ALT_KEY : CMPTEVOL_AVG_REC_KEY);
  ASSERT( pCmptEvolAvgSet != NULL );

  YM_RecordSet* pBktEvolAvgSet = 
    YM_Set<CRRDHistBktEvol>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_REC_ALT_KEY : BKTEVOL_AVG_REC_KEY);
  ASSERT( pBktEvolAvgSet != NULL );

  pBktEvolSet->RemoveObserver(this);
  if (pRecDView)
    pBktEvolSet->RemoveObserver(pRecDView);
  pBktEvolSet->Close();
  pCmptEvolSet->RemoveObserver(this);
  if (pRecDView)
    pCmptEvolSet->RemoveObserver(pRecDView);
  pCmptEvolSet->Close();

  pBktEvolAvgSet->RemoveObserver(this);
  if (pRecDView)
    pBktEvolAvgSet->RemoveObserver(pRecDView);
  pBktEvolAvgSet->Close();
  pCmptEvolAvgSet->RemoveObserver(this);
  if (pRecDView)
    pCmptEvolAvgSet->RemoveObserver(pRecDView);
  pCmptEvolAvgSet->Close();

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
  {
	YM_RecordSet* pHistoEvolSet =
	  YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_REC_ALT_KEY : HISTOEVOL_REC_KEY);
	pHistoEvolSet->RemoveObserver(this);
	if (pRecDView)
	  pHistoEvolSet->RemoveObserver (pRecDView);
	pHistoEvolSet->Close();

	YM_RecordSet* pHistoEvolCmptSet =
	  YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_RECCMPT_ALT_KEY : HISTOEVOL_RECCMPT_KEY);
	pHistoEvolCmptSet->RemoveObserver(this);
	if (pRecDView)
	  pHistoEvolCmptSet->RemoveObserver (pRecDView);
	pHistoEvolCmptSet->Close();
  }
}

///////////////////////////////////////////////////////
// Resize the chart window 
//
void CRecDHistView::ResizeChart()
{
  // force a re-size of the chart to fit the window
	CRect rect;
  GetClientRect(&rect);
  m_ChartTabCtrl.MoveWindow(&rect);
  m_ChartTabCtrl.AdjustRect(FALSE, &rect);

  // have we any chart data ?
  if (m_pCurSelChart->GetChartDataArray()->GetSize())
  {
    // is there a rectangle ?
    if ( (rect.Width() > 0) && (rect.Height() > 10) )
    {
      m_CmptChart.MoveWindow(&rect);
      m_BktChart.MoveWindow(&rect);
    }
  }
}

///////////////////////////////////////////////////////
// Clear the data objects for the compartment graphs
//
void CRecDHistView::ClearCmptGraphs()
{
	// clear the client area
	m_CmptChart.ShowWindow( SW_HIDE );
	Invalidate(FALSE);
	UpdateWindow();

  // delete the compartment entries from the tab array
  ClearTabs(CCTAChartData::eCmpt);
  m_CmptChart.ClearChartDataArray();  // delete all graph datasets

  char *szFooter[] = { "", NULL };
  m_CmptChart.SetFooterStrings(szFooter);

  // Set text in status bar control
  ((CChildFrame*)GetParentFrame())->
    GetStatusBar()->SetPaneOneAndLock(
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_READING_HISTORY) );
}

CString CRecDHistView::MakeSzHisto (const char* szComment , unsigned long ldate)
{
	CString szRtn;
	if (strlen (szComment))
	{
		szRtn = szComment;
		return szRtn;
	}
	// DM7870
	/*RWDate dummy (1,1,1900);
	if (ldate == dummy.julian())
		return szRtn;*/

	if (ldate == 0)
	{
		return szRtn;
	}
	///
	RWDate dada (ldate);
	szRtn = dada.asString("%d/%m/%Y");
	return szRtn;
}

void CRecDHistView::UpdateHistoText()
{
  //DM 4965.3, on profite de cette fonction pour construire la liste de date historiques invalides, par compartiment
  EmptyInvalidHistoDates();

  // Set pane text for historical data
  CString szHisto1, szHisto2, szHisto3;
  if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
  {
	 if (mp_HistoData)
	 {		
	   szHisto1 = mp_HistoData->GetTitle(1);
	   szHisto2 = mp_HistoData->GetTitle(2);
	   szHisto3 = mp_HistoData->GetTitle(3);
	 }
	 if (szHisto1.IsEmpty() || szHisto2.IsEmpty() || szHisto3.IsEmpty())
	 { // Pas de données dans mp_HistoData, regardons s'il y en a dans CUR_DATE_HISTO_KEY
	   // Le recordset doc->m_pCurDateHistoSet a été ajouté pour l'ano 116770, il est mis a jour
	   // a chaque changement de tranche ou d'action sur la table sc_histo_date
	   YmIcTrancheLisDom* pTranche = 
          YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);	   
	   if (((CCTAApp*)APP)->m_CurHistoDates.DptDate() == pTranche->DptDate())
	   { 
		 szHisto1 = MakeSzHisto (((CCTAApp*)APP)->m_CurHistoDates.Commentaire1(), ((CCTAApp*)APP)->m_CurHistoDates.DateHisto1());
		 szHisto2 = MakeSzHisto (((CCTAApp*)APP)->m_CurHistoDates.Commentaire2(), ((CCTAApp*)APP)->m_CurHistoDates.DateHisto2());
		 szHisto3 = MakeSzHisto (((CCTAApp*)APP)->m_CurHistoDates.Commentaire3(), ((CCTAApp*)APP)->m_CurHistoDates.DateHisto3());
	   }
	 }
  }
  // INversion pour mettre la plus récente a droite
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
  {
    setPaneDateText ( 3, szHisto1 );
    setPaneDateText ( 2, szHisto2 );
    setPaneDateText ( 1, szHisto3 );
    m_CmptChart.ChangeHistoLabel(CCTAChartData::eCmpt, szHisto1, szHisto2, szHisto3);
    m_BktChart.ChangeHistoLabel(CCTAChartData::eBkt, szHisto1, szHisto2, szHisto3);
  }
  //DM 4965.3, construction de la liste des dates historiques invalides par compartiment
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRecalcHistoBktFlag() && mp_HistoData)
  {
	CMapStringToOb* ptrMap;
	ptrMap = &(((CCTAApp*)APP)->m_nbBktByCmpt);
	POSITION pos = ptrMap->GetStartPosition();
	long nbk = 0;
	CString szCmpt;
	CString szinvalid;
	int kkk;
    RWDate DateHisto;
    long lDateHisto;
	while (pos)
	{
	  ptrMap->GetNextAssoc (pos, szCmpt, (CObject*&)nbk);
	  kkk = mp_HistoData->GetNbBucket (1, szCmpt.GetAt(0));
	  DateHisto = mp_HistoData->GetDateHisto(1);
	  lDateHisto = DateHisto.julian();
	  if ((kkk >= 0) && (kkk < nbk))
	  {
        szinvalid += szHisto1;
		if (szHisto1.GetLength())
		  szinvalid += "  ";
	  }
	  kkk = mp_HistoData->GetNbBucket (2, szCmpt.GetAt(0));
	  DateHisto = mp_HistoData->GetDateHisto(2);
	  lDateHisto = DateHisto.julian();
	  if ((kkk >= 0) && (kkk < nbk)) 
	  {
        szinvalid += szHisto2;
		if (szHisto2.GetLength())
		  szinvalid += "  ";
	  }
	  kkk = mp_HistoData->GetNbBucket (3, szCmpt.GetAt(0));
	  DateHisto = mp_HistoData->GetDateHisto(3);
	  lDateHisto = DateHisto.julian();
	  if ((kkk >= 0) && (kkk < nbk))
	  {
        szinvalid += szHisto3;
		if (szHisto3.GetLength())
		  szinvalid += "  ";
	  }
	  if (szinvalid.GetLength())
	  {
		szinvalid += ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_HISTO_INVALIDE);
		CString* psz = new CString (szinvalid);
		szinvalid.Empty();
        m_InvalidHistoDatesByCmpt.SetAt (szCmpt, (CObject*)psz);
	  }
	}
  }
}

void CRecDHistView::EmptyInvalidHistoDates()
{
  CString* psz;
  CString  szKey;
  POSITION pos = m_InvalidHistoDatesByCmpt.GetStartPosition();
  while (pos)
  {
	  m_InvalidHistoDatesByCmpt.GetNextAssoc (pos, szKey, (CObject*&)psz);
	  if (psz)
		delete psz;
  }
  m_InvalidHistoDatesByCmpt.RemoveAll();
}



///////////////////////////////////////////////////////
// Build the data objects for the compartment graphs
//
int CRecDHistView::GetRealResaCmpt(char cmpt, int iLevelCur)
{
 int valeur = -1;
 CResaRailView* pView = ((CMainFrame*) AfxGetApp()->GetMainWnd())->GetResaRailView();
 if (!pView || !pView->GetVnl()) // si pas de fen resarail ou pas de VNL, on affiche pas le points supplémentaire
 {
   return valeur;
 }
 if (GetAltFlag())
   valeur = pView->GetRealTimeSciRevenu (cmpt, iLevelCur);
 else
   valeur = pView->GetRealTimeRevenu (cmpt, iLevelCur);
 return valeur;
}

void CRecDHistView::BuildCmptGraphs()
{
  CString szWndTitle = "";
  GetDayTitle(szWndTitle);
  CString szWndKey;
  GetParentFrame()->GetWindowText(szWndKey);
  GetParentFrame()->SetWindowText(szWndTitle);
    ((YM_PersistentChildWnd*)GetParentFrame())->SetChildKey (szWndKey);

  if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag())
    return;

  BOOL bPrixGaranti = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag();

  if ( !m_bCmptDataValid || !m_bCmptAvgDataValid || !m_bCmptPrevisionRevenuDataValid ||
	  ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && !m_bHistoDataValid ) &&
	  ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() && !m_bPrevisionValid ) )
        	return;
  // clean up the previous data structure(s)
  ClearCmptGraphs();
  debjxHoiTra = finjxHoiTra = 999;
  UpdateHistoText();

  CObArray DataArray; // tableau de CRecDataGraph, un par reading day possible.
  DataArray.SetSize (GetDocument()->GetMaxRrdIndex()+2); // attention les J_X commence à -1.
  int i;
  for (i = 0; i < DataArray.GetSize(); i++)
    DataArray[i] = NULL;

  // Define the iterator to iterate through compartments
  YM_RecordSet* pCmptEvolSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_REC_ALT_KEY : CMPTEVOL_REC_KEY);
  YM_Iterator<CRRDHistCmptEvol>* pCmptIdxIterator =
		((CRRDHistCmptEvolSet*) pCmptEvolSet)->CreateKeyIterator();
  YM_RecordSet* pCmptEvolAvgSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_REC_ALT_KEY : CMPTEVOL_AVG_REC_KEY);

  // set text in status bar control
  ((CChildFrame*)GetParentFrame())->
    GetStatusBar()->SetPaneOneAndLock(
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BUILDING_GRAPH) );
  dernierJX = 99999;

  CStringArray aszXLabels;  // array to hold the X-axis labels (to be put into the TabToData object)
  YmIcTrancheLisDom* pTranche =  // Juste pour récupérer le last_rrd_index de la tranche
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  dernierJX = pTranche->LastJX();

  

  for( pCmptIdxIterator->First(); !pCmptIdxIterator->Finished(); pCmptIdxIterator->Next() ) 
  {
	// create a new cta graph data object
    CCTAChartData* pCTAChartData = new CCTAChartData( &m_CmptChart, CCTAChartData::eCmpt, NULL, NULL );
    int maxRecDhist = 0;

    // loop through all the records for this compartment
    int iIndex = 0;
	int lastRecDIndex = 0;
	int iCmptCurrentRecDIndex = -1;  // reset current RecD index
	int iCmptNextRecDIndex = -1;  // pour affichage revenu temps reel
    BOOL b_realTimeAdded  = FALSE;
	char cmpt = pCmptIdxIterator->Current()->Cmpt()[0];
	int minRrdIndex = 300;

	// Define the iterator to iterate through records for this compartment
	YM_Iterator<CRRDHistCmptEvol>* pCmptRowIterator =
		((CRRDHistCmptEvolSet*) pCmptEvolSet)->CreateIterator( *(pCmptIdxIterator->Current()->Cmpt()) );
    for( pCmptRowIterator->First(); !pCmptRowIterator->Finished(); pCmptRowIterator->Next() ) 
    {
      YmIcCmptEvolDom* pCmptEvol = pCmptRowIterator->Current();
	  cmpt = pCmptEvol->Cmpt()[0];
	  // ano 525709, ajout du j-x courant dans la requête pour eviter retour vide, mais
	  // non prise en compte de ce j-x dans l'affichage
	  CString sjx;
	  sjx.Format("%d",pCmptEvol->RrdIndex());
	  if (((CCTAApp*)APP)->GetDocument()->GetCurRrdList().Find(sjx) >= 0)
	  {
	    CRecDataGraph* pDataGraph = new CRecDataGraph();
	    pDataGraph->pCmptEvol = pCmptEvol;
		
		//if (dernierJX > pCmptEvol->RrdIndex())
		//	dernierJX = pCmptEvol->RrdIndex();
	    pDataGraph->rrd_index = pCmptEvol->RrdIndex();
	    DataArray.SetAt (pCmptEvol->RrdIndex()+1, (CObject*)pDataGraph); 
	    if (pCmptEvol->RrdIndex() < minRrdIndex)
	      minRrdIndex = pCmptEvol->RrdIndex();
	    //if ( pTranche->LastJX() == pCmptEvol->RrdIndex() )
	    iCmptCurrentRecDIndex = pCmptEvol->RrdIndex(); // Contient le dernier rrd avec données de montée en charge resa.
	  }
	}
	delete pCmptRowIterator;  // delete the row iterator

	// Define the iterator to iterate through records for this compartment
	YM_Iterator<CRRDHistCmptEvol>* pCmptAvgRowIterator =
		((CRRDHistCmptEvolSet*) pCmptEvolAvgSet)->CreateIterator( *(pCmptIdxIterator->Current()->Cmpt()) );
    for( pCmptAvgRowIterator->First(); !pCmptAvgRowIterator->Finished(); pCmptAvgRowIterator->Next() ) 
    {
      YmIcCmptEvolDom* pCmptEvol = pCmptAvgRowIterator->Current();
	  CRecDataGraph* pDataGraph = (CRecDataGraph*)DataArray.GetAt(pCmptEvol->RrdIndex()+1);
	  if (!pDataGraph)
	  {
		pDataGraph = new CRecDataGraph();
		pDataGraph->pCmptEvol = pCmptEvol;
	    pDataGraph->rrd_index = pCmptEvol->RrdIndex();
	    DataArray.SetAt (pCmptEvol->RrdIndex()+1, (CObject*)pDataGraph);
	  }
	  pDataGraph->pCmptEvol->ResHoldAvg(pCmptEvol->ResHoldAvg());
	  pDataGraph->pCmptEvol->ResHoldStdv(pCmptEvol->ResHoldStdv());
	  pDataGraph->pCmptEvol->RecHoldAvg(pCmptEvol->RecHoldAvg());
	  pDataGraph->pCmptEvol->RecHoldStdv(pCmptEvol->RecHoldStdv());
	  if (pCmptEvol->RrdIndex() < minRrdIndex)
	    minRrdIndex = pCmptEvol->RrdIndex();
	  if ((pCmptEvol->RrdIndex() < iCmptCurrentRecDIndex) && (iCmptNextRecDIndex == -1))
	    iCmptNextRecDIndex = pCmptEvol->RrdIndex();
	}
	delete pCmptAvgRowIterator;  // delete the row iterator

	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRealTimeInRRDHistFlag())
	{ 
	  if ((iCmptCurrentRecDIndex > 0) && (iCmptCurrentRecDIndex > minRrdIndex))
	  {
		int RealRevCmpt = GetRealResaCmpt(cmpt, -1 );
		if (RealRevCmpt >= 0)
		{
		  CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(iCmptNextRecDIndex+1); // avant iCmptCurrentRrdIndex-1, mais +1 pour décalage du tableau
		  if (!ppData)
		  {
		    ppData = new CRecDataGraph();
			ppData->rrd_index = iCmptNextRecDIndex;
			DataArray.SetAt (ppData->rrd_index+1, (CObject*) ppData);
		  }
		  ppData->RevTempsReel = (double)RealRevCmpt;
		  b_realTimeAdded = TRUE;
		}
	  }
	}			  
          
	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && mp_HistoData)
	{
	  double hRev;
	  int mamax = __min (DataArray.GetSize()-1, mp_HistoData->m_max_RecD);
	  for (i = mp_HistoData->m_min_RecD; i <= mamax; i++)
	  {
	    if (mp_HistoData->IsRrd(i))
	    {
	      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i+1);
		  if (!ppData)
		  {
		    ppData = new CRecDataGraph();
		    ppData->rrd_index = i;
		    DataArray.SetAt (i+1, (CObject*) ppData);
		  }
		  if (mp_HistoData->GetNbDate() >= 1)
		  {
		    hRev = ppData->Revenu1 = mp_HistoData->GetSumRevenu (1, cmpt, i);
			if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
			  ppData->HoiTra1 = mp_HistoData->GetHoiTra (1, cmpt, i);
	        if (hRev != INVALID_GRAPH_VALUE)
	        {
	          pCTAChartData->SetGraphMax( hRev );
	          pCTAChartData->SetResHoldMax( hRev );
	        }
		  }
		  if (mp_HistoData->GetNbDate() >= 2)
		  {
		    hRev = ppData->Revenu2 = mp_HistoData->GetSumRevenu (2, cmpt, i);
			if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
			  ppData->HoiTra2 = mp_HistoData->GetHoiTra (2, cmpt, i);
	        if (hRev != INVALID_GRAPH_VALUE)
	        {
	          pCTAChartData->SetGraphMax( hRev );
	          pCTAChartData->SetResHoldMax( hRev );
	        }
		  }
		  if (mp_HistoData->GetNbDate() >= 3)
		  {
		    hRev = ppData->Revenu3 = mp_HistoData->GetSumRevenu (3, cmpt, i);
			if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
			  ppData->HoiTra3 = mp_HistoData->GetHoiTra (3, cmpt, i);
	        if (hRev != INVALID_GRAPH_VALUE)
	        {
	          pCTAChartData->SetGraphMax( hRev );
	          pCTAChartData->SetResHoldMax( hRev );
	        }
		  }
		}
	  } 
	}
    /*if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
	{
	  long res = -1;
      YM_Iterator<YmIcResaBktDom>* pIteratorBkt = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY)->CreateIterator();
      YmIcResaBktDom* pResaBkt = NULL;
      for( pIteratorBkt->First(); !pIteratorBkt->Finished(); pIteratorBkt->Next() )
      {
        pResaBkt = pIteratorBkt->Current();
	    if ( (tolower(pResaBkt->Cmpt()[0]) == tolower(cmpt)) &&
			 (!GetAltFlag() || ((pResaBkt->NestLevel() == 0) && (pResaBkt->Scx() > 0)) ) )
	    {
		  CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(pResaBkt->RrdIndex()+1);
		  if (!ppData)
		  {
		    ppData = new CRecDataGraph();
		    ppData->rrd_index = pResaBkt->RrdIndex();
		    DataArray.SetAt (pResaBkt->RrdIndex()+1, (CObject*) ppData);
		  }
		  ppData->AddPrevision (pResaBkt->RrdRecette());
	    }
      }
      delete pIteratorBkt;
	}*/
	//DT Prevision
	//long res = -1;
	if (!((CCTAApp*)APP)->GetRhealysFlag() && !this->GetAltFlag() && !(YM_Set<YmIcPrevisionDom>::FromKey(PREVISION_REVENU_KEY)->IsEmpty()))
	{
		YM_Iterator<YmIcPrevisionDom>* pIteratorCmpt = YM_Set<YmIcPrevisionDom>::FromKey(PREVISION_REVENU_KEY)->CreateIterator();
		YmIcPrevisionDom* pPrevisionCmpt = NULL;
		for( pIteratorCmpt->First(); !pIteratorCmpt->Finished(); pIteratorCmpt->Next() )
		{
			pPrevisionCmpt = pIteratorCmpt->Current();
			if (tolower(pPrevisionCmpt->getCmpt()[0]) == tolower(cmpt))
			{
				int jx = pPrevisionCmpt->getJX();
				CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(pPrevisionCmpt->getJX()+1);
				if (!ppData)
				{
					ppData = new CRecDataGraph();
					ppData->rrd_index = pPrevisionCmpt->getJX();
					TRACE("pCTAChartData->GetGraphMax() = %f < %d\n", pCTAChartData->GetGraphMax(), pPrevisionCmpt->getPrevisionRevenu());
					TRACE("Dernier JX = %d\n", dernierJX);
					if (dernierJX > pPrevisionCmpt->getJX())
						DataArray.SetAt (pPrevisionCmpt->getJX()+1, (CObject*) ppData);
				}
				ppData->pCmptPrevision = pPrevisionCmpt;
			}
		}
		delete pIteratorCmpt;
	}

	// Le nombre de points
    int lNumPts = 0; 
    for (i = 0; i < DataArray.GetSize(); i++)
    {
	  if (DataArray.GetAt(i))
	    lNumPts++;
    }
	// Retrouver l'indice du dernier point pour les montées en charge de la tranche
	int lastCmptIndex = 0;
    for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData)
	  {
		if (ppData->rrd_index == iCmptCurrentRecDIndex)
	      i = 0;
	    else
	     lastCmptIndex ++;
	  }
    }

	if ((debjxHoiTra == 999) && ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	{ // retrouver premier et dernier j-x des horaires indicatifs sur la courbe de montée en charge.
      for (i= 0; i < DataArray.GetSize(); i++)
	  {
	    CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	    if (ppData)
	    {
		  YmIcCmptEvolDom* pCmptEvol = ppData->pCmptEvol;
		  if (pCmptEvol)
		  {
		    if (!strcmp (pCmptEvol->HoiTra(), "I"))
			{ 
		      if ((debjxHoiTra == 999) || (pCmptEvol->RrdIndex() > debjxHoiTra))
			    debjxHoiTra = pCmptEvol->RrdIndex();
			  if (pCmptEvol->RrdIndex() < finjxHoiTra)
			    finjxHoiTra = pCmptEvol->RrdIndex();
			}
		  }
		}
	  }
	}

	// clean up and reallocate the array to hold X-axis labels
    aszXLabels.RemoveAll();
    aszXLabels.SetSize(lNumPts);

    // create the chart data object...set the size in the next if construct
    CChart2DData *pChartData = NULL;
    // create the 2nd chart data object
    CChart2DData *pChartData2 = NULL;
    pChartData = new CChart2DData(XRT_DATA_GENERAL, eCmptNumDataSets, lNumPts);
    pChartData2 = new CChart2DData(XRT_DATA_GENERAL, eCmptStdvNumDataSets, lNumPts);

    // remove the points which do not exist from current RecD to RecD = 0
    pChartData->SetFirstPoint(eCmptResHoldIdv, 0);
    pChartData->SetLastPoint(eCmptResHoldIdv, lastCmptIndex);
	pChartData->SetFirstPoint(eCmptResHoldTot, 0);
    pChartData->SetLastPoint(eCmptResHoldTot, b_realTimeAdded ? lastCmptIndex+1 : lastCmptIndex);

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	{
	  pChartData->SetFirstPoint(eCmptHoiTra, 0);
      pChartData->SetLastPoint(eCmptHoiTra, lastCmptIndex);
	  pChartData->SetFirstPoint(eCmptHoiTraIdv, 0);
      pChartData->SetLastPoint(eCmptHoiTraIdv, lastCmptIndex);
	  if (debjxHoiTra == 999)
	  {
	    pChartData->SetDisplay(eCmptHoiTra, XRT_DISPLAY_HIDE);
		pChartData->SetDisplay(eCmptHoiTraIdv, XRT_DISPLAY_HIDE);
	  }
	}

	double adXData[MAXNBJMXPLUS5]; // taille max
    memset(adXData, '\0', sizeof(double) * MAXNBJMXPLUS5);
	double adYData[MAXNBJMXPLUS5]; // taille max
    memset(adYData, '\0', sizeof(double) * MAXNBJMXPLUS5);

	// AFFECTATION DES VALEURS DANS LES DIFFERENTES COURBES
    int j = 0;
	// Les indices de reading day
	CString szBuffer;
	for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData)
	  {
		GetRecD( ppData->rrd_index, &szBuffer );
	    aszXLabels.SetAtGrow( j, szBuffer );
	    adXData[j]  = j + 1;
	    j++;
	  }
	}
	int iSet;
    int iPoint;
	for (iSet = 0; iSet < eCmptNumDataSets; iSet++)
    {
      for (iPoint = 0; iPoint < lNumPts; iPoint++)
         pChartData->SetXElement(iSet, iPoint, adXData[iPoint]);
    }
    for (iSet = 0; iSet < eCmptStdvNumDataSets; iSet++)
    {
       for (iPoint = 0; iPoint < lNumPts; iPoint++)
         pChartData2->SetXElement(iSet, iPoint, adXData[iPoint]);
    }
    
	double dRev = 0;
	double dAvg = 0;
	double dStdB = 0;
	//courbe eCmptResHoldAvg
	j = 0;
	for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData)
	  {
		if (ppData->pCmptEvol)
		{
		  dAvg = (! ppData->pCmptEvol->ResHoldAvgIsNull()) ? (double)ppData->pCmptEvol->ResHoldAvg() : INVALID_GRAPH_VALUE;
	      if (bPrixGaranti && !ppData->pCmptEvol->RecHoldAvgIsNull())
		    dAvg += (double)ppData->pCmptEvol->RecHoldAvg();
		  adYData[j] = dAvg;
		  pCTAChartData->SetGraphMax( dAvg ); 
		}
	    j++;
	  }
    }
	pChartData->SetYData (eCmptResHoldAvg, adYData, lNumPts, 0);

	
	j = 0;
	memset(adYData, '\0', sizeof(double) * lNumPts);
	//courbe eCmptResHoldTot
	for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData)
	  {
		if (ppData->pCmptEvol)
		{
		  dRev = (! ppData->pCmptEvol->ResHoldIdvIsNull()) ?
			  (double)(ppData->pCmptEvol->ResHoldIdv()) : INVALID_GRAPH_VALUE;
	      if (bPrixGaranti && !ppData->pCmptEvol->ResHoldGrpIsNull()) // recette prix garanti placée dans res_hold_grp
		    dRev += (double)(ppData->pCmptEvol->ResHoldGrp());
		}
		else
		  dRev = 0;
		if (ppData->RevTempsReel != INVALID_GRAPH_VALUE)
		  dRev = ppData->RevTempsReel;
		adYData[j] = dRev;
		pCTAChartData->SetGraphMax( dRev ); 
        pCTAChartData->SetResHoldMax( dRev );
	    j++;
	  }
    }
	pChartData->SetYData (eCmptResHoldTot, adYData, lNumPts, 0);
	
	j=0;
	memset(adYData, '\0', sizeof(double) * lNumPts);
	//Courbe ResHoldIdv
	for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData)
	  {
		if (ppData->pCmptEvol)
		  adYData[j] = (! ppData->pCmptEvol->ResHoldIdvIsNull()) ?
             (double)ppData->pCmptEvol->ResHoldIdv() : INVALID_GRAPH_VALUE;
	    j++;
	  }
    }
	pChartData->SetYData (eCmptResHoldIdv, adYData, lNumPts, 0);

	j=0;
	memset(adYData, '\0', sizeof(double) * lNumPts);
	//Courbe eCmptResHoldStdvLow
	for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData)
	  {
		if (ppData->pCmptEvol)
		{
		  dStdB  = ( (! ppData->pCmptEvol->ResHoldAvgIsNull()) && (! ppData->pCmptEvol->ResHoldStdvIsNull()) ) ?
            (double)( __max((ppData->pCmptEvol->ResHoldAvg() - ppData->pCmptEvol->ResHoldStdv()),0) ) : INVALID_GRAPH_VALUE;
		  if (bPrixGaranti && !ppData->pCmptEvol->RecHoldAvgIsNull() && !ppData->pCmptEvol->RecHoldStdvIsNull())
		    dStdB += __max((ppData->pCmptEvol->RecHoldAvg() - ppData->pCmptEvol->RecHoldStdv()),0);
		  adYData[j] = dStdB;
		}
		j++;
	  }
    }
	pChartData2->SetYData (eCmptResHoldStdvLow, adYData, lNumPts, 0);

	j=0;
	memset(adYData, '\0', sizeof(double) * lNumPts);
	//Courbe eCmptResHoldStdvHigh
	for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData)
	  {
		if (ppData->pCmptEvol)
		{
		  dAvg = ( (! ppData->pCmptEvol->ResHoldAvgIsNull()) && (! ppData->pCmptEvol->ResHoldStdvIsNull()) ) ?
            (double)(ppData->pCmptEvol->ResHoldAvg() + ppData->pCmptEvol->ResHoldStdv()) : INVALID_GRAPH_VALUE;
		  if (bPrixGaranti && !ppData->pCmptEvol->RecHoldAvgIsNull() && !ppData->pCmptEvol->RecHoldStdvIsNull())
		    dAvg += (double)(ppData->pCmptEvol->RecHoldAvg() + ppData->pCmptEvol->RecHoldStdv());
		  adYData[j] = dAvg;
		  pCTAChartData->SetGraphMax( dAvg );
		}
		j++;
	  }
    }
	pChartData2->SetYData (eCmptResHoldStdvHigh, adYData, lNumPts, 0);

	j=0;
    if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	{
	  //Courbe des horaires indicatifs, plaquée sur celle des résa totales
	  for (i = DataArray.GetSize()-1; i>= 0; i--)
      {
        CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	    if (ppData)
	    {
		  if (ppData->pCmptEvol)
		  {
		    dRev = (! ppData->pCmptEvol->ResHoldIdvIsNull()) ?
			  (double)(ppData->pCmptEvol->ResHoldIdv()) : INVALID_GRAPH_VALUE;
	        if (bPrixGaranti && !ppData->pCmptEvol->ResHoldGrpIsNull()) // recette prix garanti placée dans res_hold_grp
		    dRev += (double)(ppData->pCmptEvol->ResHoldGrp());
			adYData[j] = (!strcmp (ppData->pCmptEvol->HoiTra(), "I")) ? dRev : INVALID_GRAPH_VALUE;
		  }  
	      j++;
	    }
      }
	  pChartData->SetYData (eCmptHoiTra, adYData, lNumPts, 0);

	  //idem sur la courbe des résa individuelles
	  j=0;
	  for (i = DataArray.GetSize()-1; i>= 0; i--)
      {
        CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	    if (ppData)
	    {
		  if (ppData->pCmptEvol)
		  adYData[j] = ((! ppData->pCmptEvol->ResHoldIdvIsNull()) &&
		                (!strcmp (ppData->pCmptEvol->HoiTra(), "I")))?
             (double)ppData->pCmptEvol->ResHoldIdv() : INVALID_GRAPH_VALUE;
	      j++;
	    }
      }
	  pChartData->SetYData (eCmptHoiTraIdv, adYData, lNumPts, 0);
	}

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && mp_HistoData)
	{
	  j=0;
	  memset(adYData, '\0', sizeof(double) * lNumPts);
	  //Courbe eCmptHisto1ResHoldTot
	  for (i = DataArray.GetSize()-1; i>= 0; i--)
      {
        CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	    if (ppData)
	    {
		  adYData[j] = ppData->Revenu1;
		  j++;
	    }
	  }
	  pChartData->SetYData (eCmptHisto1ResHoldTot, adYData, lNumPts, 0);

	  j=0;
	  memset(adYData, '\0', sizeof(double) * lNumPts);
	  //Courbe eCmptHisto2ResHoldTot
	  for (i = DataArray.GetSize()-1; i>= 0; i--)
      {
        CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	    if (ppData)
	    {
		  adYData[j] = ppData->Revenu2;
		  j++;
	    }
	  }
	  pChartData->SetYData (eCmptHisto2ResHoldTot, adYData, lNumPts, 0);

	  j=0;
	  memset(adYData, '\0', sizeof(double) * lNumPts);
	  //Courbe eCmptHisto3ResHoldTot
	  for (i = DataArray.GetSize()-1; i>= 0; i--)
      {
        CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	    if (ppData)
	    {
		  adYData[j] = ppData->Revenu3;
		  j++;
	    }
	  }
	  pChartData->SetYData (eCmptHisto3ResHoldTot, adYData, lNumPts, 0);

	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	  {
	    j=0;
	    memset(adYData, '\0', sizeof(double) * lNumPts);
	    //Courbe eCmptHisto1HoiTra
	    for (i = DataArray.GetSize()-1; i>= 0; i--)
        {
          CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	      if (ppData)
	      {
		    adYData[j] = ppData->HoiTra1 ? ppData->Revenu1 : INVALID_GRAPH_VALUE;
		    j++;
	      }
	    }
	    pChartData->SetYData (eCmptHisto1HoiTra, adYData, lNumPts, 0);

	    j=0;
	    memset(adYData, '\0', sizeof(double) * lNumPts);
	    //Courbe eCmptHisto2HoiTra
	    for (i = DataArray.GetSize()-1; i>= 0; i--)
        {
          CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	      if (ppData)
	      {
		    adYData[j] = ppData->HoiTra2 ? ppData->Revenu2 : INVALID_GRAPH_VALUE;
		    j++;
	      }
	    }
	    pChartData->SetYData (eCmptHisto2HoiTra, adYData, lNumPts, 0);

        j=0;
	    memset(adYData, '\0', sizeof(double) * lNumPts);
	    //Courbe eCmptHisto3HoiTra
	    for (i = DataArray.GetSize()-1; i>= 0; i--)
        {
          CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	      if (ppData)
	      {
		    adYData[j] = ppData->HoiTra3 ? ppData->Revenu3 : INVALID_GRAPH_VALUE;
		    j++;
	      }
	    }
	    pChartData->SetYData (eCmptHisto3HoiTra, adYData, lNumPts, 0);
	  }
	}

	/*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
	{
	  j=0;
	  memset(adYData, '\0', sizeof(double) * lNumPts);
	  for (i = DataArray.GetSize()-1; i>= 0; i--)
      {
        CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	    if (ppData)
	    {
		  adYData[j] = ppData->GetPrevision();
		  j++;
	    }
	  }
	  pChartData->SetYData (eCmptPrevision, adYData, lNumPts, 0);
	}*/

	//DT Prevision
	if (!((CCTAApp*)APP)->GetRhealysFlag() && !this->GetAltFlag())
	{
		j=0;
		int values_bypassed = 0;
		int currentIndex = 0;
		memset(adYData, '\0', sizeof(double) * lNumPts);
		BOOL _continue = 1;
		int currentPrevIndex = 9999;
		int lastKnownIndex = 9999;
		for (i = DataArray.GetSize()-1; i>= 0 && _continue; i--)
		{
			CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				if (ppData->pCmptPrevision)
				{
					pChartData->SetFirstPoint(eCmptPrevision, values_bypassed);//A décaler sous la boucle
					_continue = 0;
				}
				values_bypassed = values_bypassed + 1;
			}
		}

		currentPrevIndex = values_bypassed;

		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				currentPrevIndex = currentPrevIndex + 1;
				if (ppData->pCmptPrevision)
				{
					//ppData->
					TRACE("rrd_index = %d\n", ppData->rrd_index);
					TRACE("Dernier JX = %d\n", dernierJX);
					if (dernierJX > ppData->pCmptPrevision->getJX())
					{
						adYData[j] = ppData->pCmptPrevision->getPrevisionRevenu();
						int rev = ppData->pCmptPrevision->getPrevisionRevenu();
						lastKnownIndex = currentPrevIndex;
						
						//pChartData->SetLastPoint(eCmptPrevision, (DataArray.GetSize()-1 - 10));
					}
					else
						adYData[j] = XRT_HUGE_VAL;
					//pChartData->SetFirstPoint(eCmptPrevision, values_bypassed - 1);
					//pChartData->SetLastPoint(eCmptPrevision, (values_injected + (values_bypassed - 1)) -1);
				}
				else
					adYData[j] = XRT_HUGE_VAL;
				j++;

			}
		}
		
		//pChartData->SetLastPoint(eCmptPrevision, lastKnownIndex);
		pChartData->SetYData (eCmptPrevision, adYData, lNumPts, 0);
	}

	// Masquons les courbes historiques inutiles
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && mp_HistoData)
	{
	  int nbd = mp_HistoData->GetNbDate();
	  if (nbd < 3)
	  {
		pChartData->SetLastPoint(eCmptHisto3ResHoldTot, -1);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
          pChartData->SetLastPoint(eCmptHisto3HoiTra, -1);
	  }
	  if (nbd < 2)
	  {
		pChartData->SetLastPoint(eCmptHisto2ResHoldTot, -1);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
          pChartData->SetLastPoint(eCmptHisto2HoiTra, -1);
	  }
	  if (nbd < 1)
	  {
		pChartData->SetLastPoint(eCmptHisto1ResHoldTot, -1);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
          pChartData->SetLastPoint(eCmptHisto1HoiTra, -1);
	  }
	  if (nbd >= 3)
	  {
		if (!m_bDispHisto3ResHoldTot)
		  pChartData->SetDisplay(eCmptHisto3ResHoldTot, XRT_DISPLAY_HIDE);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{
		  if (!m_bDispHisto3ResHoldTot)
		    pChartData->SetDisplay(eCmptHisto3HoiTra, XRT_DISPLAY_HIDE);
		}
	  }
	  if (nbd >= 2)
	  {
		if (!m_bDispHisto2ResHoldTot)
		  pChartData->SetDisplay(eCmptHisto2ResHoldTot, XRT_DISPLAY_HIDE);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{
		  if (!m_bDispHisto2ResHoldTot)
		    pChartData->SetDisplay(eCmptHisto2HoiTra, XRT_DISPLAY_HIDE);
		}
	  }
	  if (nbd >= 1)
	  {
		if (!m_bDispHisto1ResHoldTot)
		  pChartData->SetDisplay(eCmptHisto1ResHoldTot, XRT_DISPLAY_HIDE);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{
		  if (!m_bDispHisto1ResHoldTot)
		    pChartData->SetDisplay(eCmptHisto1HoiTra, XRT_DISPLAY_HIDE);
		}
	  }	 
	}

    m_CmptChart.SetYAxisMin(0);
    m_CmptChart.SetY2AxisMin(0);

    // set the number of data points (ticks along the X axis)
    int iSize = aszXLabels.GetSize();
    pCTAChartData->m_aszXLabels.SetSize(iSize);
    for (i = 0; i < iSize; i++)
      pCTAChartData->m_aszXLabels[i] = aszXLabels[i];

    pCTAChartData->SetNumPts(lNumPts);

	// Masquage autres courbes si demandé  
	if (!m_bDispEC)
	  pChartData2->SetDisplay(eCmptResHoldStdvHigh, XRT_DISPLAY_HIDE);// Ecart type
	if (!m_bDispResHoldAvg)
	  pChartData->SetDisplay(eCmptResHoldAvg, XRT_DISPLAY_HIDE); // Moyenne
    if (! m_bDispResHoldTot)
	  pChartData->SetDisplay(eCmptResHoldTot, XRT_DISPLAY_HIDE); // Rev total
    if (! m_bDispResHoldIdv)
	  pChartData->SetDisplay(eCmptResHoldIdv, XRT_DISPLAY_HIDE);

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	{
	  if (! m_bDispResHoldTot)
	    pChartData->SetDisplay(eCmptHoiTra, XRT_DISPLAY_HIDE);
	   if (! m_bDispResHoldIdv)
	    pChartData->SetDisplay(eCmptHoiTraIdv, XRT_DISPLAY_HIDE);
	}

	/*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
	{*/
	if (!((CCTAApp*)APP)->GetRhealysFlag() && !this->GetAltFlag())
	  if (! m_bDispPrevision || YM_Set<YmIcPrevisionDom>::FromKey(PREVISION_REVENU_KEY)->IsEmpty())
	  {
		  pChartData->SetDisplay(eCmptPrevision, XRT_DISPLAY_HIDE);
		  //pChartData->SetFirstPoint(eCmptPrevision, 0);
		  //pChartData->SetLastPoint(eCmptPrevision, 0);
	  }
	//}


    // add the Olectra chart data objects and the cmpt and bkt to the CTA data object
    pCTAChartData->SetDataObj(pChartData);
    pCTAChartData->SetDataObj2(pChartData2);
    pCTAChartData->SetCmpt(*(pCmptIdxIterator->Current()->Cmpt()));
    pCTAChartData->SetCmptSeq(pCmptIdxIterator->Current()->CmptSeq());
    m_CmptChart.AddChartData( pCTAChartData );  // add this CTA Chart Data element to the array

	// Nettoyage du DataArray avant de lire le deuxième compartiment
    for (i = 0; i < DataArray.GetSize(); i++)
	{
	  CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData)
	    delete ppData;
	  DataArray[i] = NULL;
	}
  }  // end for loop through compartments

  delete pCmptIdxIterator;  // delete the index iterator
  m_CmptChart.SetGraphFont();
}

//////////////////////////////////
// Draw the right graph
//
void CRecDHistView::DrawGraph()
{
  CCTAChartData* pCTAChartData = m_pCurSelChart->GetCurChartData();
  if ( pCTAChartData == NULL )
    return;

  m_pCurSelChart->SetRepaint( FALSE );

  m_pCurSelChart->SetData( *(pCTAChartData->GetDataObj()) );
  m_pCurSelChart->SetData2( *(pCTAChartData->GetDataObj2()) );

  int iPoint;
  CString szBuffer;
  int iNumPts = pCTAChartData->GetNumPts();
  m_CmptChart.SetRepaint( FALSE );
  m_BktChart.SetRepaint( FALSE );
  m_pCurSelChart->SetXLabels(NULL);
  for (iPoint = 0; iPoint < iNumPts; iPoint++)
  {
    szBuffer = pCTAChartData->m_aszXLabels[iPoint];
    m_pCurSelChart->SetNthPointLabel(iPoint, (char *)LPCTSTR(szBuffer));
    XrtValueLabel label;
    label.value = (iPoint+1);
    label.string = (char *)LPCTSTR(szBuffer);
    m_pCurSelChart->SetValueLabel(XRT_AXIS_X, &label);
  }

  CString szGraphFooter;
  CString szClient = App().m_pDatabase->GetClientParameter();
  //DM4965.3
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRecalcHistoBktFlag() &&
	   ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() &&
	   (m_szCurTabText.GetLength() > 1) && ((m_szCurTabText.GetAt(1) != '0') || (szClient == THALYS)) )
  {
    // Si affichage d'une bucket > 0, et qu'il y a ai donné historique on indique lesquelles sont invalides
	CString sKeyCmpt = m_szCurTabText.GetAt(0);
	CString *psz = NULL;
    if (m_InvalidHistoDatesByCmpt.Lookup (sKeyCmpt, (CObject*&)psz))
	  szGraphFooter = *psz;
  }
  else if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetCompactRRDHistViewFlag())
    szGraphFooter = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RRDHISTORY_FOOTER);

  char *szFooter[] = { (LPSTR)(LPCSTR)szGraphFooter, NULL };
  m_pCurSelChart->SetFooterStrings(szFooter);

  SetWindowText( (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RRDHISTORY_Y_TITLE) );
  // NPI - Anomalie 41228
  // Pour une raison inconnue, si le nombre de points en abscisse est > 31, l'axe des abscisses est décentré et disproportionné
  // On force donc systématiquement les deux valeurs ci-dessous pour corriger ce problème
  m_pCurSelChart->SetXAxisMax(iNumPts);
  m_pCurSelChart->SetXAxisMin(1);
  ///
  m_pCurSelChart->ScaleGraph(m_pCurSelChart->GetCurScaling());

  m_CmptChart.SetRepaint( TRUE );
  m_BktChart.SetRepaint( TRUE );

  m_pNotSelChart->ShowWindow( SW_HIDE );
	m_pCurSelChart->ShowWindow( SW_SHOW );
}

///////////////////////////////////////////////////////
// Clear the data objects for the bucket graphs
//
void CRecDHistView::ClearBktGraphs()
{
	// clear the client area
	m_BktChart.ShowWindow( SW_HIDE );
	Invalidate(FALSE);
	UpdateWindow();

  // delete the bucket entries from the tab array
  ClearTabs(CCTAChartData::eBkt);
  m_BktChart.ClearChartDataArray();  // delete all graph datasets
/*
  // clear the header and footer
  char *szHeader[] = { "", NULL };
  m_BktChart.SetHeaderStrings(szHeader);
*/
  char *szFooter[] = { "", NULL };
  m_BktChart.SetFooterStrings(szFooter);

  // Set text in status bar control
  ((CChildFrame*)GetParentFrame())->
    GetStatusBar()->SetPaneOneAndLock(
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_READING_HISTORY) );
}


//////////////////////////////////////////
// Set the compartment graph properties
//
void CRecDHistChart::SetGraphProperties(enum CCTAChartData::GraphType eGraphType)
{
  SetType(XRT_TYPE_PLOT);
  SetType2(XRT_TYPE_AREA);

  SetXAnnotationMethod(XRT_ANNO_VALUE_LABELS);

  SetLegendShow(FALSE);

  SetGraphShowOutlines(FALSE);  // we never want to show the outline
//  SetGraphShowOutlines(((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetGraphOutlineFlag());
  m_graphtype = eGraphType;
  if (eGraphType == CCTAChartData::eBkt)  // bucket
  {
    // set some chart properties for the Bucket Chart
    SetNthSetLabel(GetParentView()->eBktResHoldTot,
      (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(	IDS_TRANCHE_REVENU));
    SetNthSetLabel(GetParentView()->eBktResHoldIdv,
      (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_REVENU_TICKET));
	SetNthSetLabel(GetParentView()->eBktResHoldAvg,
      (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_AVG));

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
	{
	  SetNthSetLabel(GetParentView()->eBktHoiTra,
        (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INCERTAIN));
	  SetNthSetLabel(GetParentView()->eBktHoiTraIdv,
        (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INCERTAIN));
	}

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
	{
	  CString szLabelResa = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RECETTES);
	  CString szIncertainConf = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INCER_CONF);
	  CString szIncertain = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INCERTAIN);
	  CString szLabel;
	  CString szIC;
	  int incertain, confirme;
	  szLabel = szLabelResa + " 1";
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	  {
		if ( ((CCTAApp*)APP)->GetDocument()->GetHistoHoiTra(1, incertain, confirme))
		{
		  szLabel = "€ 1, ";
		  if (confirme == -2)
		    szIC.Format (szIncertain, incertain, "");
		  else
			szIC.Format (szIncertainConf, incertain, confirme);
		  szLabel += szIC;
		}
		SetNthSetLabel(GetParentView()->eBktHisto1HoiTra, (LPSTR)(LPCSTR)szLabel);
	  }
	  SetNthSetLabel(GetParentView()->eBktHisto1ResHoldTot, (LPSTR)(LPCSTR)szLabel);

	  szLabel = szLabelResa + " 2";
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	  {
		if ( ((CCTAApp*)APP)->GetDocument()->GetHistoHoiTra(2, incertain, confirme))
		{
		  szLabel = "€ 2, ";
		  if (confirme == -2)
		    szIC.Format (szIncertain, incertain, "");
		  else
			szIC.Format (szIncertainConf, incertain, confirme);
		  szLabel += szIC;
		}
		SetNthSetLabel(GetParentView()->eBktHisto1HoiTra, (LPSTR)(LPCSTR)szLabel);
	  }
	  SetNthSetLabel(GetParentView()->eBktHisto2ResHoldTot, (LPSTR)(LPCSTR)szLabel);

	  szLabel = szLabelResa + " 3";
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	  {
		if ( ((CCTAApp*)APP)->GetDocument()->GetHistoHoiTra(3, incertain, confirme))
		{
		  szLabel = "€ 3, ";
		  if (confirme == -2)
		    szIC.Format (szIncertain, incertain, "");
		  else
			szIC.Format (szIncertainConf, incertain, confirme);
		  szLabel += szIC;
		}
		SetNthSetLabel(GetParentView()->eBktHisto1HoiTra, (LPSTR)(LPCSTR)szLabel);
	  }
	  SetNthSetLabel(GetParentView()->eBktHisto3ResHoldTot, (LPSTR)(LPCSTR)szLabel);
	}

	/*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
	{
	  SetNthSetLabel(GetParentView()->eBktPrevision,
        (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_PREVISION));
	}*/


	SetNthSetLabel2(CRecDHistView::eBktResHoldStdvHigh,
    (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_STDV));
	SetNthSetLabel2(CRecDHistView::eBktResHoldStdvLow,
    (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_STDV));

	int nbb=0;
    XrtDataStyle *paBktDataStyles[13];
	paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eResHoldTotStyleIdx);
	paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eRevTicketeStyleIdx);
    paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eResHoldAvgStyleIdx);
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
	{
	  paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHoiTraStyleIdx);
	  paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHoiTraIdvStyleIdx);
	}
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
	{
	  paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHisto3ResHoldTotStyleIdx);
	  paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHisto2ResHoldTotStyleIdx);
	  paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHisto1ResHoldTotStyleIdx);
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
	  {
	    paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHisto3HoiTraStyleIdx);
		paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHisto2HoiTraStyleIdx);
		paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHisto1HoiTraStyleIdx);
	  }
	}
	//if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
	if (!((CCTAApp*)APP)->GetRhealysFlag() && !this->GetParentView()->GetAltFlag())
	  paBktDataStyles[nbb++] = m_aDataStyles.GetAt(ePrevisionStyleIdx);
	while (nbb < 13)
	    paBktDataStyles[nbb++] = NULL;
    SetDataStyles(paBktDataStyles);
	
    XrtDataStyle *paBktDataStyles2[] = 
    {
      m_aDataStyles.GetAt(eResHoldStdvHighStyleIdx),
      m_aDataStyles.GetAt(eResHoldStdvLowStyleIdx),
      NULL
    };
    SetDataStyles2(paBktDataStyles2);
  }
  else  // compartment
  {
    // set some chart properties for the Compartment Chart
    SetNthSetLabel(GetParentView()->eCmptResHoldTot,
    (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TRANCHE_REVENU));
	SetNthSetLabel(GetParentView()->eCmptResHoldIdv,
    (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_REVENU_TICKET));
	SetNthSetLabel(GetParentView()->eCmptResHoldAvg,
    (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_AVG));

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
	{
	  CString szLabelResa = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RECETTES);
	  CString szIncertainConf = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INCER_CONF);
	  CString szIncertain = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INCERTAIN);
	  CString szLabel;
	  CString szIC;
	  int incertain, confirme;
	  szLabel = szLabelResa + " 1";
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	  {
		if ( ((CCTAApp*)APP)->GetDocument()->GetHistoHoiTra(1, incertain, confirme))
		{
		  szLabel = "€ 1, ";
		  if (confirme == -2)
		    szIC.Format (szIncertain, incertain, "");
		  else
			szIC.Format (szIncertainConf, incertain, confirme);
		  szLabel += szIC;
		}
		SetNthSetLabel(GetParentView()->eCmptHisto1HoiTra, (LPSTR)(LPCSTR)szLabel);
	  }
	  SetNthSetLabel(GetParentView()->eCmptHisto1ResHoldTot, (LPSTR)(LPCSTR)szLabel);

	  szLabel = szLabelResa + " 2";
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	  {
		if ( ((CCTAApp*)APP)->GetDocument()->GetHistoHoiTra(2, incertain, confirme))
		{
		  szLabel = "€ 2, ";
		  if (confirme == -2)
		    szIC.Format (szIncertain, incertain, "");
		  else
			szIC.Format (szIncertainConf, incertain, confirme);
		  szLabel += szIC;
		}
		SetNthSetLabel(GetParentView()->eCmptHisto1HoiTra, (LPSTR)(LPCSTR)szLabel);
	  }
	  SetNthSetLabel(GetParentView()->eCmptHisto2ResHoldTot, (LPSTR)(LPCSTR)szLabel);

	  szLabel = szLabelResa + " 3";
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	  {
		if ( ((CCTAApp*)APP)->GetDocument()->GetHistoHoiTra(3, incertain, confirme))
		{
		  szLabel = "€ 3, ";
		  if (confirme == -2)
		    szIC.Format (szIncertain, incertain, "");
		  else
			szIC.Format (szIncertainConf, incertain, confirme);
		  szLabel += szIC;
		}
		SetNthSetLabel(GetParentView()->eCmptHisto1HoiTra, (LPSTR)(LPCSTR)szLabel);
	  }
	  SetNthSetLabel(GetParentView()->eCmptHisto3ResHoldTot, (LPSTR)(LPCSTR)szLabel);
	}

	  /*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
	  {*/
		if (!((CCTAApp*)APP)->GetRhealysFlag())
		{
			//XrtDataHandle hXrtData;
			YM_RecordSet* pCmptPrevisionRevenuSet = YM_Set<YmIcPrevisionDom>::FromKey(PREVISION_REVENU_KEY);
			bool jej = this->GetParentView()->GetAltFlag();
			//TRACE("GetYPointValue = %s\n", ((CRecDHistHelp*)m_pHelpDlg)->GetYPointValue(XRT_DATA, GetParentView()->eCmptPrevision, m_iCurSelectPt ));
			if (!((CRecDHistView*)this->GetParent())->_AltFlag)
			{
				//if (pCmptPrevisionRevenuSet->IsEmpty()/* || ((CRecDHistHelp*)m_pHelpDlg)->GetYPointValue(XRT_DATA, GetParentView()->eCmptPrevision, m_iCurSelectPt ) == XRT_HUGE_VAL*/)//DT Prevision
				SetNthSetLabel(GetParentView()->eCmptPrevision, this->m_PrevisionLabel.GetBuffer());
				//else
					//SetNthSetLabel(GetParentView()->eCmptPrevision, (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_PREVISION));
			}
		}
	  //}

	  SetNthSetLabel2(GetParentView()->eCmptResHoldStdvHigh,
      (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_STDV));
	  SetNthSetLabel2(GetParentView()->eCmptResHoldStdvLow,
      (LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_STDV));

	  int nbc=0;
      XrtDataStyle *paCmptDataStyles[13];
	  paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eResHoldTotStyleIdx);
	  paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eRevTicketeStyleIdx);
      paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eResHoldAvgStyleIdx);
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
	  {
	    paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHoiTraStyleIdx);
		paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHoiTraIdvStyleIdx);
	  }
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
	  {
		paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto3ResHoldTotStyleIdx);
		paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto2ResHoldTotStyleIdx);
		paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto1ResHoldTotStyleIdx);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
	    {
	      paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto3HoiTraStyleIdx);
		  paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto2HoiTraStyleIdx);
		  paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto1HoiTraStyleIdx);
	    }
	  }
	  //if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
	  if (!((CCTAApp*)APP)->GetRhealysFlag() && !this->GetParentView()->GetAltFlag())
	    paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(ePrevisionStyleIdx);
	  
	  while (nbc < 13)
	    paCmptDataStyles[nbc++] = NULL;
      SetDataStyles(paCmptDataStyles);

      XrtDataStyle *paCmptDataStyles2[] = 
      {
        m_aDataStyles.GetAt(eResHoldStdvHighStyleIdx),
        m_aDataStyles.GetAt(eResHoldStdvLowStyleIdx),
        NULL
      };
      SetDataStyles2(paCmptDataStyles2);
  }

  // set default axis minimum
  SetYAxisMin(0);
  SetY2AxisMin(0);

  CCTAChart::SetGraphFont();
}

void CRecDHistChart::ChangeHistoLabel(enum CCTAChartData::GraphType eGraphType,
									 CString szHisto1, CString szHisto2, CString szHisto3)
{
  if (eGraphType == CCTAChartData::eBkt)  // bucket
  {
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && 
		    ((CRecDHistView*)GetParentView())->mp_HistoData)
	  {
		int nbd = ((CRecDHistView*)GetParentView())->mp_HistoData->GetNbDate();
		CString szLabelResa = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RECETTES);
		CString szLabel;
		if (nbd >= 1)
		{
		  szLabel = szLabelResa + " " + szHisto1;
		  SetNthSetLabel(GetParentView()->eBktHisto1ResHoldTot, (LPSTR)(LPCSTR)szLabel);
		  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		    SetNthSetLabel(GetParentView()->eBktHisto1HoiTra, (LPSTR)(LPCSTR)szLabel);
		}
		if (nbd >= 2)
		{
		  szLabel = szLabelResa + " " + szHisto2;
		  SetNthSetLabel(GetParentView()->eBktHisto2ResHoldTot, (LPSTR)(LPCSTR)szLabel);
		  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		    SetNthSetLabel(GetParentView()->eBktHisto2HoiTra, (LPSTR)(LPCSTR)szLabel);
		}
		if (nbd >= 3)
		{
		  szLabel = szLabelResa + " " + szHisto3;
		  SetNthSetLabel(GetParentView()->eBktHisto3ResHoldTot, (LPSTR)(LPCSTR)szLabel);
		  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		    SetNthSetLabel(GetParentView()->eBktHisto3HoiTra, (LPSTR)(LPCSTR)szLabel);
		}	
	  }
  }
  else  // compartment
  {
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && 
		    ((CRecDHistView*)GetParentView())->mp_HistoData)
	  {
		int nbd = ((CRecDHistView*)GetParentView())->mp_HistoData->GetNbDate();
		CString szLabelResa = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RECETTES);
		CString szLabel;
		if (nbd >= 1)
		{
		  szLabel = szLabelResa + " " + szHisto1;
		  SetNthSetLabel(GetParentView()->eCmptHisto1ResHoldTot, (LPSTR)(LPCSTR)szLabel);
		  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
		    SetNthSetLabel(GetParentView()->eCmptHisto1HoiTra, (LPSTR)(LPCSTR)szLabel);
		}
		if (nbd >= 2)
		{
		  szLabel = szLabelResa + " " + szHisto2;
		  SetNthSetLabel(GetParentView()->eCmptHisto2ResHoldTot, (LPSTR)(LPCSTR)szLabel);
		  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
		    SetNthSetLabel(GetParentView()->eCmptHisto2HoiTra, (LPSTR)(LPCSTR)szLabel);
		}
		if (nbd >= 3)
		{
		  szLabel = szLabelResa + " " + szHisto3;
		  SetNthSetLabel(GetParentView()->eCmptHisto3ResHoldTot, (LPSTR)(LPCSTR)szLabel);
		  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
		    SetNthSetLabel(GetParentView()->eCmptHisto3HoiTra, (LPSTR)(LPCSTR)szLabel);
		}	
	  }
  }
}

CCTAChartHelpDlg* CRecDHistChart::CreateHelpDlg()
{
  m_pHelpDlg = new CRecDHistHelp( this );
  m_pHelpDlg->Create( this );
  ((CMainFrame*) AfxGetApp()->GetMainWnd())->SetChartHelpDlg(m_pHelpDlg);
  return (m_pHelpDlg);
}

void CRecDHistChart::ContextualDisplay(CPoint point, BOOL bInRange)
{
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetControlRRDCurveFlag() )
  { // On ne fait cela que pour la SNCF..
    if (!((CCTAApp*)APP)->GetFlashLabelFlag())
	  return; // On ne fait se traitement que si le FlashLabel est actif.
			  // Sinon l'utilisateur n'aura pas l'indication précise de ce qu'il va détruire.
	XrtPickResult pick;
	BOOL bFound = FALSE;

	if (m_graphtype == CCTAChartData::eBkt)  // bucket
	{
	  if ( Pick ( XRT_DATASET1, point.x, point.y, &pick, XRT_XFOCUS|XRT_YFOCUS ) == XRT_RGN_IN_GRAPH && 
       pick.distance <= FRONTBACKDIST)
	  { // dataset1
		((CRecDHistView*)GetParentView())->RevMasqueCourbe (CCTAChartData::eBkt, pick.set, FALSE);
		bFound = TRUE;
	  }
	  else if ( Pick ( XRT_DATASET2, point.x, point.y, &pick, XRT_XFOCUS|XRT_YFOCUS ) == XRT_RGN_IN_GRAPH && 
       pick.distance <= FRONTBACKDIST)
	  { // dataset2 : ecart-type.
		((CRecDHistView*)GetParentView())->RevMasqueCourbe (CCTAChartData::eBkt, pick.set, TRUE);
		bFound = TRUE;
	  }
	}
	else  // Compartiment
	{  
      if ( Pick ( XRT_DATASET1, point.x, point.y, &pick, XRT_XFOCUS|XRT_YFOCUS ) == XRT_RGN_IN_GRAPH && 
       pick.distance <= FRONTBACKDIST)
	  { // dataset1
		((CRecDHistView*)GetParentView())->RevMasqueCourbe (CCTAChartData::eCmpt, pick.set, FALSE);
		bFound = TRUE;
	  }
	  else if ( Pick ( XRT_DATASET2, point.x, point.y, &pick, XRT_XFOCUS|XRT_YFOCUS ) == XRT_RGN_IN_GRAPH && 
       pick.distance <= FRONTBACKDIST)
	  { // dataset2 : ecart-type.
		((CRecDHistView*)GetParentView())->RevMasqueCourbe (CCTAChartData::eCmpt, pick.set, TRUE);
		bFound = TRUE;
	  }	  
	}
	if (bFound)
	    ((CRecDHistView*)GetParentView())->ChangeRecDDisplay(FALSE);
    return;
  }
  return;
}

///////////////////////////////////////////////////////
// Get the Reading Day from the RecD Index
//
BOOL CRecDHistView::GetRecD( long  lRecDIndex, CString * szRecD )
{
  szRecD->Format( "%+d", -lRecDIndex );  // format it for the X axis label
  return true;
}


///////////////////////////////////////////////////////
// Build the data objects for the bucket graphs
//
void CRecDHistView::BuildBktGraphs()
{
  if ( !m_bBktDataValid || !m_bBktAvgDataValid ||
	   ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() && !m_bCmptDataValid) ||
	   ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && !m_bHistoDataValid ) &&
	   ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() && !m_bPrevisionValid) )
	return;
  // clean up the previous data structure(s)
  ClearBktGraphs();

  if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag())
    return;

  CObArray DataArray; // tableau de CRecDataGraph, un par reading day possible.
  DataArray.SetSize (GetDocument()->GetMaxRrdIndex()+2); // attention les J_X commence à -1.
  int i;
  for (i = 0; i < DataArray.GetSize(); i++)
    DataArray[i] = NULL;

  BOOL bPrixGaranti = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag();

  // Define the iterator to iterate through compartments/buckets
  YM_RecordSet* pBktEvolSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_REC_ALT_KEY : BKTEVOL_REC_KEY);
  YM_Iterator<CRRDHistBktEvol> *pBktIdxIterator =
    ((CRRDHistBktEvolSet*) pBktEvolSet)->CreateKeyIterator();
  YM_RecordSet* pBktEvolAvgSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_REC_ALT_KEY : BKTEVOL_AVG_REC_KEY);

  // set text in status bar control
  ((CChildFrame*)GetParentFrame())->
    GetStatusBar()->SetPaneOneAndLock(
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BUILDING_GRAPH) );

  CStringArray aszXLabels;  // array to hold the X-axis labels (to be put into the TabToData object)
  YmIcTrancheLisDom* pTranche =  // Juste pour récupérer le last_rrd_index de la tranche
      YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  
  for( pBktIdxIterator->First(); !pBktIdxIterator->Finished(); pBktIdxIterator->Next() )
  {
	CRRDHistBktEvol* pRRDHistBktEvol = pBktIdxIterator->Current();
    // Define the iterator to iterate through records for this compartment
	YM_Iterator<CRRDHistBktEvol> *pBktRowIterator =
			((CRRDHistBktEvolSet*) pBktEvolSet)->CreateIterator( pRRDHistBktEvol->Key() );

    // create a new cta graph data object
    CCTAChartData* pCTAChartData = new CCTAChartData( &m_BktChart, CCTAChartData::eBkt );

    // loop through all the buckets for this compartment
	int iBktCurrentRecDIndex = -1;  // reset current RecD index for the bucket level 
	int iBktNextRecDIndex = -1;  // pour affichage resa temps reel
    BOOL b_realTimeAdded  = FALSE;
	int minRrdIndex = 300;
    char cmpt;
    long nestlevel;
    for( pBktRowIterator->First(); !pBktRowIterator->Finished(); pBktRowIterator->Next() ) 
    {
      YmIcBktEvolDom* pBktEvol = pBktRowIterator->Current();
	  cmpt = pBktEvol->Cmpt()[0];
	  nestlevel = pBktEvol->NestLevel();
	  // ano 525709, ajout du j-x courant dans la requête pour eviter retour vide, mais
	  // non prise en compte de ce j-x dans l'affichage
	  CString sjx;
	  sjx.Format("%d",pBktEvol->RrdIndex());
	  if (((CCTAApp*)APP)->GetDocument()->GetCurRrdList().Find(sjx) >= 0)
	  {
        // FORCE AVAILABILITY TO ZERO FOR NEGATIVE VALUES !!!
        pBktEvol->Availability( __max( pBktEvol->Availability(), 0 ) );
        CRecDataGraph* pDataGraph = new CRecDataGraph();
	    pDataGraph->pBktEvol = pBktEvol;
	    pDataGraph->rrd_index = pBktEvol->RrdIndex();
	    DataArray.SetAt (pBktEvol->RrdIndex()+1, (CObject*)pDataGraph);
	    cmpt = pBktEvol->Cmpt()[0];
	    nestlevel = pBktEvol->NestLevel();
	    if (pBktEvol->RrdIndex() < minRrdIndex)
	      minRrdIndex = pBktEvol->RrdIndex();
        //if ( pTranche->LastJX() == pBktEvol->RrdIndex() )
	    iBktCurrentRecDIndex = pBktEvol->RrdIndex(); // Contient le dernier rrd avec données de montée en charge resa.
		pCTAChartData->setScxEtanche(pBktEvol->ScxEtanche());//DT35037 - Lyria lot 3 - BUG-i 103454 - Correction de l'affichage du (E) pour montée en charge recette
	  }
	}
	delete pBktRowIterator;

	YM_Iterator<CRRDHistBktEvol> *pBktAvgRowIterator =
			((CRRDHistBktEvolSet*) pBktEvolAvgSet)->CreateIterator( pRRDHistBktEvol->Key() );
	for( pBktAvgRowIterator->First(); !pBktAvgRowIterator->Finished(); pBktAvgRowIterator->Next() ) 
    {
      YmIcBktEvolDom* pBktEvol = pBktAvgRowIterator->Current();
	  CRecDataGraph* pDataGraph = (CRecDataGraph*)DataArray.GetAt(pBktEvol->RrdIndex()+1);
	  if (!pDataGraph)
	  {
		pDataGraph = new CRecDataGraph();
		pDataGraph->pBktEvol = pBktEvol;
	    pDataGraph->rrd_index = pBktEvol->RrdIndex();
	    DataArray.SetAt (pBktEvol->RrdIndex()+1, (CObject*)pDataGraph);
	  }
	  pDataGraph->pBktEvol->ResHoldAvg(pBktEvol->ResHoldAvg());
	  pDataGraph->pBktEvol->ResHoldStdv(pBktEvol->ResHoldStdv());
	  pDataGraph->pBktEvol->RecHoldAvg(pBktEvol->RecHoldAvg());
	  pDataGraph->pBktEvol->RecHoldStdv(pBktEvol->RecHoldStdv());
	  if (pBktEvol->RrdIndex() < minRrdIndex)
	     minRrdIndex = pBktEvol->RrdIndex();
	  if ((pBktEvol->RrdIndex() < iBktCurrentRecDIndex) && (iBktNextRecDIndex == -1))
	    iBktNextRecDIndex = pBktEvol->RrdIndex();
	}
	delete pBktAvgRowIterator;

	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRealTimeInRRDHistFlag())
	{ // DM 5350, ajout du temps reel dans la fenêtre montée en charge resa
	  if ((iBktCurrentRecDIndex > 0) && (iBktCurrentRecDIndex > minRrdIndex))
	  {
        int RealResaCmpt = GetRealResaCmpt(cmpt, nestlevel);                 
	    if (RealResaCmpt >= 0)
		{
		  CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(iBktNextRecDIndex+1); // avant iBktCurrentRrdIndex-1, mais +1 pour décalage du tableau
		  if (!ppData)
		  {
		    ppData = new CRecDataGraph();
			ppData->rrd_index = iBktNextRecDIndex;
			DataArray.SetAt (ppData->rrd_index+1, (CObject*) ppData);
		  }
	      if (ppData)
		    ppData->RevTempsReel = (double)RealResaCmpt;
		  b_realTimeAdded = TRUE;
		}
	  }
	}

	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && mp_HistoData)
	{
	  double hRev;
	  int mamax = __min (DataArray.GetSize() -1, mp_HistoData->m_max_RecD);
	  for (i = mp_HistoData->m_min_RecD; i <= mamax; i++)
	  {
	    if (mp_HistoData->IsRrd(i))
	    {
	      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i+1);
		  if (!ppData)
		  {
		    ppData = new CRecDataGraph();
		    ppData->rrd_index = i;
		    DataArray.SetAt (i+1, (CObject*) ppData);
		  }
		  if (mp_HistoData->GetNbDate() >= 1)
		  {
		    hRev = ppData->Revenu1 = mp_HistoData->GetRevenu (1, cmpt, i, nestlevel);
			if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
			  ppData->HoiTra1 = mp_HistoData->GetHoiTra (1, cmpt, i);
		    if (hRev != INVALID_GRAPH_VALUE)
	        {
	          pCTAChartData->SetGraphMax( hRev );
	          pCTAChartData->SetResHoldMax( hRev );
	        }
		  }
		  if (mp_HistoData->GetNbDate() >= 2)
		  {
		    hRev = ppData->Revenu2 = mp_HistoData->GetRevenu (2, cmpt, i, nestlevel);
			if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
			  ppData->HoiTra2 = mp_HistoData->GetHoiTra (2, cmpt, i);
		    if (hRev != INVALID_GRAPH_VALUE)
	        {
	          pCTAChartData->SetGraphMax( hRev );
	          pCTAChartData->SetResHoldMax( hRev );
	        }
		  }
		  if (mp_HistoData->GetNbDate() >= 3)
		  {
		    hRev = ppData->Revenu3 = mp_HistoData->GetRevenu (3, cmpt, i, nestlevel);
			if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
			  ppData->HoiTra3 = mp_HistoData->GetHoiTra (3, cmpt, i);
		    if (hRev != INVALID_GRAPH_VALUE)
	        {
	          pCTAChartData->SetGraphMax( hRev );
	          pCTAChartData->SetResHoldMax( hRev );
	        }
		  }
		}
	  }
	}

	/*if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
	{
	  long res = -1;
      YM_Iterator<YmIcResaBktDom>* pIteratorBkt = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY)->CreateIterator();
      YmIcResaBktDom* pResaBkt = NULL;
      for( pIteratorBkt->First(); !pIteratorBkt->Finished(); pIteratorBkt->Next() )
      {
        pResaBkt = pIteratorBkt->Current();
	    if (( tolower(pResaBkt->Cmpt()[0]) == tolower(cmpt)) &&
			( (!GetAltFlag() && (pResaBkt->NestLevel() == nestlevel) ) ||
			  (GetAltFlag() && (pResaBkt->Scx() == nestlevel) && (pResaBkt->NestLevel() == 0)) ) )
	    {
		  CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(pResaBkt->RrdIndex()+1);
		  if (!ppData)
		  {
		    ppData = new CRecDataGraph();
		    ppData->rrd_index = pResaBkt->RrdIndex();
		    DataArray.SetAt (pResaBkt->RrdIndex()+1, (CObject*) ppData);
		  }
		  ppData->AddPrevision (pResaBkt->RrdRecette());
	    }
      }
      delete pIteratorBkt;
	}*/
   
    // Le nombre de points
    int lNumPts = 0; 
    for (i = 0; i < DataArray.GetSize(); i++)
    {
	  if (DataArray.GetAt(i))
	    lNumPts++;
    }

	// Retrouver l'indice du dernier point pour les montée en charge de la tranche
    int lastCmptIndex = 0;
    for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData)
	  {
		if (ppData->rrd_index == iBktCurrentRecDIndex)
	      i = 0;
	    else
	     lastCmptIndex ++;
	  }
    }

	// clean up and reallocate the array to hold X-axis labels
    aszXLabels.RemoveAll();
    aszXLabels.SetSize(lNumPts);

    // create the chart data object
    CChart2DData *pChartData = new CChart2DData(XRT_DATA_GENERAL, eBktNumDataSets, lNumPts);
    // create the 2nd chart data object
    CChart2DData *pChartData2 = new CChart2DData(XRT_DATA_GENERAL, eBktStdvNumDataSets, lNumPts);

    // remove the points which do not exist from current RecD to RecD = 0
    pChartData->SetFirstPoint(eBktResHoldTot, 0);
	pChartData->SetLastPoint(eBktResHoldTot, b_realTimeAdded ? lastCmptIndex+1 : lastCmptIndex);
    pChartData->SetFirstPoint(eBktResHoldIdv, 0);
    pChartData->SetLastPoint(eBktResHoldIdv, lastCmptIndex);

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	{
	  pChartData->SetFirstPoint(eBktHoiTra, 0);
      pChartData->SetLastPoint(eBktHoiTra, lastCmptIndex);
	  pChartData->SetFirstPoint(eBktHoiTraIdv, 0);
      pChartData->SetLastPoint(eBktHoiTraIdv, lastCmptIndex);
	  if (debjxHoiTra == 999)
	  {
	    pChartData->SetDisplay(eBktHoiTra, XRT_DISPLAY_HIDE);
		pChartData->SetDisplay(eBktHoiTraIdv, XRT_DISPLAY_HIDE);
	  }
	}

	// Masquons les courbes historiques inutiles
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && mp_HistoData)
	{
	  int nbd = mp_HistoData->GetNbDate();
	  if (nbd < 3)
	  {
		pChartData->SetLastPoint(eBktHisto3ResHoldTot, -1);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
		  pChartData->SetLastPoint(eBktHisto3HoiTra, -1);
	  }
	  if (nbd < 2)
	  {
		pChartData->SetLastPoint(eBktHisto2ResHoldTot, -1);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
		  pChartData->SetLastPoint(eBktHisto2HoiTra, -1);
	  }
	  if (nbd < 1)
	  {
		pChartData->SetLastPoint(eBktHisto1ResHoldTot, -1);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
		  pChartData->SetLastPoint(eBktHisto1HoiTra, -1);
	  }
	  if (nbd >= 3)
	  {
		if (!m_bDispHisto3ResHoldTot)
		  pChartData->SetDisplay(eBktHisto3ResHoldTot, XRT_DISPLAY_HIDE);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{
		  if (!m_bDispHisto3ResHoldTot)
		    pChartData->SetDisplay(eBktHisto3HoiTra, XRT_DISPLAY_HIDE);
		}
	  }
	  if (nbd >= 2)
	  {
		if (!m_bDispHisto2ResHoldTot)
		  pChartData->SetDisplay(eBktHisto2ResHoldTot, XRT_DISPLAY_HIDE);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{
		  if (!m_bDispHisto2ResHoldTot)
		    pChartData->SetDisplay(eBktHisto2HoiTra, XRT_DISPLAY_HIDE);
		}
	  }
	  if (nbd >= 1)
	  {
		if (!m_bDispHisto1ResHoldTot)
		  pChartData->SetDisplay(eBktHisto1ResHoldTot, XRT_DISPLAY_HIDE);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{
		  if (!m_bDispHisto1ResHoldTot)
		    pChartData->SetDisplay(eBktHisto1HoiTra, XRT_DISPLAY_HIDE);
		}
	  }	 
	}

	double adXData[MAXNBJMXPLUS5]; // taille max
    memset(adXData, '\0', sizeof(double) * MAXNBJMXPLUS5);
	double adYData[MAXNBJMXPLUS5]; // taille max
    memset(adYData, '\0', sizeof(double) * MAXNBJMXPLUS5);

	// AFFECTATION DES VALEURS DANS LES DIFFERENTES COURBES
	int j = 0;
	// Les indices de reading day
	CString szBuffer;
	
	for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData)
	  {
		GetRecD( ppData->rrd_index, &szBuffer );
	    aszXLabels.SetAtGrow( j, szBuffer );
	    adXData[j]  = j + 1;
	    j++;
	  }
	}
	
	int iSet;
    int iPoint;
	for (iSet = 0; iSet < eBktNumDataSets; iSet++)
    {
      for (iPoint = 0; iPoint < lNumPts; iPoint++)
         pChartData->SetXElement(iSet, iPoint, adXData[iPoint]);
    }
    for (iSet = 0; iSet < eBktStdvNumDataSets; iSet++)
    {
       for (iPoint = 0; iPoint < lNumPts; iPoint++)
         pChartData2->SetXElement(iSet, iPoint, adXData[iPoint]);
    }

	double dRev;
	double dAvg;
	j=0;
	//Courbe eBktResHoldTot
	for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData )
	  {
	    if (ppData->pBktEvol)
		{
		  dRev = !ppData->pBktEvol->ResHoldIdvIsNull() ?		// recette tickete, placée dans res_hold_idv
            (double)(ppData->pBktEvol->ResHoldIdv()) : INVALID_GRAPH_VALUE;
		  if (bPrixGaranti && !ppData->pBktEvol->ResHoldGrpIsNull()) // recette prix garanti placée dans res_hold_grp
		    dRev += (double)(ppData->pBktEvol->ResHoldGrp());
		}
		else dRev = 0;
		if (ppData->RevTempsReel != INVALID_GRAPH_VALUE)
		  dRev = ppData->RevTempsReel;
		adYData[j] = dRev;
		pCTAChartData->SetGraphMax( dRev );
	    pCTAChartData->SetResHoldMax( dRev );
	    j++;
	  }
    }
	pChartData->SetYData (eBktResHoldTot, adYData, lNumPts, 0);

	j=0;
	memset(adYData, '\0', sizeof(double) * lNumPts);
	//Courbe eBktResHoldIdv
	for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData )
	  {
		if (ppData->pBktEvol)
		  adYData[j] = (! ppData->pBktEvol->ResHoldIdvIsNull()) ?
          (double)ppData->pBktEvol->ResHoldIdv() : INVALID_GRAPH_VALUE;
	    j++;
	  }
    }
	pChartData->SetYData (eBktResHoldIdv, adYData, lNumPts, 0);

	j=0;
	memset(adYData, '\0', sizeof(double) * lNumPts);
	//Courbe eBktResHoldAvg
	for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData )
	  {
		if (ppData->pBktEvol)
		{
		  dAvg = (! ppData->pBktEvol->ResHoldAvgIsNull()) ? (double)ppData->pBktEvol->ResHoldAvg() : INVALID_GRAPH_VALUE;
		  if (bPrixGaranti && !ppData->pBktEvol->RecHoldAvgIsNull())
		    dAvg += (double)ppData->pBktEvol->RecHoldAvg();
		  adYData[j] = dAvg;
		  pCTAChartData->SetGraphMax( dAvg );
		}
	    j++;
	  }
    }
	pChartData->SetYData (eBktResHoldAvg, adYData, lNumPts, 0);

	j=0;
	memset(adYData, '\0', sizeof(double) * lNumPts);
	//Courbe eBktResHoldStdvHigh
	for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData)
	  {
		if (ppData->pBktEvol)
		{
		  dAvg = ( (! ppData->pBktEvol->ResHoldAvgIsNull()) && (! ppData->pBktEvol->ResHoldStdvIsNull()) ) ?
          (double)(ppData->pBktEvol->ResHoldAvg() + ppData->pBktEvol->ResHoldStdv()) : INVALID_GRAPH_VALUE;
		  if (bPrixGaranti && !ppData->pBktEvol->RecHoldAvgIsNull() && !ppData->pBktEvol->RecHoldStdvIsNull())
		    dAvg += (double)(ppData->pBktEvol->RecHoldAvg() + ppData->pBktEvol->RecHoldStdv());
		  adYData[j] = dAvg;
		  pCTAChartData->SetGraphMax( dAvg );
		}
	    j++;
	  }
    }
	pChartData2->SetYData (eBktResHoldStdvHigh, adYData, lNumPts, 0);

	j=0;
	memset(adYData, '\0', sizeof(double) * lNumPts);
	//Courbe eBktResHoldStdvLow
	for (i = DataArray.GetSize()-1; i>= 0; i--)
    {
      CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData)
	  {
		if (ppData->pBktEvol)
		{
		  dAvg  = ( (! ppData->pBktEvol->ResHoldAvgIsNull()) && (! ppData->pBktEvol->ResHoldStdvIsNull()) ) ?
          (double)( __max((ppData->pBktEvol->ResHoldAvg() - ppData->pBktEvol->ResHoldStdv()),0) ) : INVALID_GRAPH_VALUE;
		  if (bPrixGaranti && !ppData->pBktEvol->RecHoldAvgIsNull() && !ppData->pBktEvol->RecHoldStdvIsNull())
		    dAvg += __max((ppData->pBktEvol->RecHoldAvg() - ppData->pBktEvol->RecHoldStdv()),0);
		  adYData[j] = dAvg;
		}
		j++;
	  }
    }
	pChartData2->SetYData (eBktResHoldStdvLow, adYData, lNumPts, 0);


    j=0;
    if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	{
	  //Courbe des horaires indicatifs, plaquée sur celle des résa totales
	  for (i = DataArray.GetSize()-1; i>= 0; i--)
      {
        CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	    if (ppData )
	    {
		  if (ppData->pBktEvol)
		  {
		    dRev = !ppData->pBktEvol->ResHoldIdvIsNull() ?		// recette tickete, placée dans res_hold_idv
              (double)(ppData->pBktEvol->ResHoldIdv()) : INVALID_GRAPH_VALUE;
		    if (bPrixGaranti && !ppData->pBktEvol->ResHoldGrpIsNull()) // recette prix garanti placée dans res_hold_grp
		      dRev += (double)(ppData->pBktEvol->ResHoldGrp());
			if (( ppData->pBktEvol->RrdIndex() > debjxHoiTra ) ||
			  ( ppData->pBktEvol->RrdIndex() < finjxHoiTra ))
		      dRev = INVALID_GRAPH_VALUE;
            adYData[j] = dRev;
		  }
	      j++;
	    }
      }
	  pChartData->SetYData (eBktHoiTra, adYData, lNumPts, 0);

	  //idem sur les résa individuelles
	  j = 0;
	  for (i = DataArray.GetSize()-1; i>= 0; i--)
      {
        CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	    if (ppData )
	    {
		  if (ppData->pBktEvol)
		  {
		    dRev = !ppData->pBktEvol->ResHoldIdvIsNull() ?		// recette tickete, placée dans res_hold_idv
              (double)(ppData->pBktEvol->ResHoldIdv()) : INVALID_GRAPH_VALUE;
			if (( ppData->pBktEvol->RrdIndex() > debjxHoiTra ) ||
			  ( ppData->pBktEvol->RrdIndex() < finjxHoiTra ))
		      dRev = INVALID_GRAPH_VALUE;
            adYData[j] = dRev;
		  }
	      j++;
	    }
      }
	  pChartData->SetYData (eBktHoiTraIdv, adYData, lNumPts, 0);
	}


    if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && mp_HistoData)
	{
	  j=0;
	  memset(adYData, '\0', sizeof(double) * lNumPts);
	  for (i = DataArray.GetSize()-1; i>= 0; i--)
      {
        CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	    if (ppData)
	    {
		  adYData[j] = ppData->Revenu1;
		  j++;
	    }
	  }
	  pChartData->SetYData (eBktHisto1ResHoldTot, adYData, lNumPts, 0);

	  j=0;
	  memset(adYData, '\0', sizeof(double) * lNumPts);
	  for (i = DataArray.GetSize()-1; i>= 0; i--)
      {
        CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	    if (ppData)
	    {
		  adYData[j] = ppData->Revenu2;
		  j++;
	    }
	  }
	  pChartData->SetYData (eBktHisto2ResHoldTot, adYData, lNumPts, 0);

	  j=0;
	  memset(adYData, '\0', sizeof(double) * lNumPts);
	  for (i = DataArray.GetSize()-1; i>= 0; i--)
      {
        CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	    if (ppData)
	    {
		  adYData[j] = ppData->Revenu3;
		  j++;
	    }
	  }
	  pChartData->SetYData (eBktHisto3ResHoldTot, adYData, lNumPts, 0);
 
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	  {
	    j=0;
	    memset(adYData, '\0', sizeof(double) * lNumPts);
	    //Courbe eBktHisto1HoiTra
	    for (i = DataArray.GetSize()-1; i>= 0; i--)
        {
          CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	      if (ppData)
	      {
			adYData[j] = ppData->HoiTra1 ? ppData->Revenu1 : INVALID_GRAPH_VALUE;
		    j++;
	      }
	    }
	    pChartData->SetYData (eBktHisto1HoiTra, adYData, lNumPts, 0);

        j=0;
	    memset(adYData, '\0', sizeof(double) * lNumPts);
	    //Courbe eBktHisto2HoiTra
	    for (i = DataArray.GetSize()-1; i>= 0; i--)
        {
          CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	      if (ppData)
	      {
			adYData[j] = ppData->HoiTra2 ? ppData->Revenu2 : INVALID_GRAPH_VALUE;
		    j++;
	      }
	    }
	    pChartData->SetYData (eBktHisto2HoiTra, adYData, lNumPts, 0);

		j=0;
	    memset(adYData, '\0', sizeof(double) * lNumPts);
	    //Courbe eBktHisto3HoiTra
	    for (i = DataArray.GetSize()-1; i>= 0; i--)
        {
          CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	      if (ppData)
	      {
			adYData[j] = ppData->HoiTra3 ? ppData->Revenu3 : INVALID_GRAPH_VALUE;
		    j++;
	      }
	    }
	    pChartData->SetYData (eBktHisto3HoiTra, adYData, lNumPts, 0);
	  }

	}

	/*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
	{
	  j=0;
	  memset(adYData, '\0', sizeof(double) * lNumPts);
	  for (i = DataArray.GetSize()-1; i>= 0; i--)
      {
        CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	    if (ppData)
	    {
		  adYData[j] = ppData->GetPrevision();
		  j++;
	    }
	  }
	  pChartData->SetYData (eBktPrevision, adYData, lNumPts, 0);
	}*/
	  
	// Masquage autre courbe si demandé
	// DM4965.3, a ce niveau le pRRDHistBktEvol donne CMPT et nestlevel, on peut donc
	// masquer les courbes par classes de contrôle.
	if (!m_bDispEC)
	  pChartData2->SetDisplay(eBktResHoldStdvHigh, XRT_DISPLAY_HIDE); // Ecart type
	if (! m_bDispResHoldAvg)
	  pChartData->SetDisplay(eBktResHoldAvg, XRT_DISPLAY_HIDE);       // Moyenne
    if (! m_bDispResHoldTot)
	  pChartData->SetDisplay(eBktResHoldTot, XRT_DISPLAY_HIDE);
	if (! m_bDispResHoldIdv)
	  pChartData->SetDisplay(eBktResHoldIdv, XRT_DISPLAY_HIDE);

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	{
	  if (! m_bDispResHoldTot)
	    pChartData->SetDisplay(eBktHoiTra, XRT_DISPLAY_HIDE);
	  if (! m_bDispResHoldIdv)
	    pChartData->SetDisplay(eBktHoiTraIdv, XRT_DISPLAY_HIDE);
	}

	/*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
	{
	  if (! m_bDispPrevision)
	    pChartData->SetDisplay(eBktPrevision, XRT_DISPLAY_HIDE);
	}*/

    m_BktChart.SetYAxisMin(0);
    m_BktChart.SetY2AxisMin(0);

    // set the number of data points (ticks along the X axis)
    int iSize = aszXLabels.GetSize();
    pCTAChartData->m_aszXLabels.SetSize(iSize);
    for (i = 0; i < iSize; i++)
      pCTAChartData->m_aszXLabels[i] = aszXLabels[i];

    pCTAChartData->SetNumPts(lNumPts);

    // add the Olectra chart data objects and the cmpt and bkt to the CTA data object
    pCTAChartData->SetDataObj(pChartData);
    pCTAChartData->SetDataObj2(pChartData2);
    pCTAChartData->SetCmpt(*(pBktIdxIterator->Current()->Cmpt()));
    pCTAChartData->SetCmptSeq(pBktIdxIterator->Current()->CmptSeq());
    pCTAChartData->SetBktLabel(pBktIdxIterator->Current()->BucketLabel());
    pCTAChartData->SetNestLevel(pBktIdxIterator->Current()->NestLevel());
    m_BktChart.AddChartData( pCTAChartData );  // add this CTA Chart Data element to the array

	// Nettoyage du DataArray avant de lire le deuxième compartiment
    for (i = 0; i < DataArray.GetSize(); i++)
	{
	  CRecDataGraph* ppData = (CRecDataGraph*)DataArray.GetAt(i);
	  if (ppData)
	    delete ppData;
	  DataArray[i] = NULL;
	}

  }  // end for loop through compartments

  delete pBktIdxIterator;  // delete the index iterator

  m_BktChart.SetGraphFont();
}

//////////////////////////////////////////////////////////
// Clear the tabs array and removes the corresponding tab
//
void CRecDHistView::ClearTabs(enum CCTAChartData::GraphType eGraphType)
{
  // clean up the array containing the tabs and graph data objects
  m_ChartTabCtrl.DeleteAllItems( ); // remove the tab from the control
}

//////////////////////////////////////
// Build the tabs on the tab control
//
void CRecDHistView::BuildTabs()
{
    int nLastTab = -1;

  // do we have any data at all ?
  int iCmptArraySize = m_CmptChart.GetChartDataArray()->GetSize();
  int iBktArraySize = m_BktChart.GetChartDataArray()->GetSize();
  if ( ( iCmptArraySize == 0) && (iBktArraySize == 0) )
  {
    // Set text in status bar control
    ((CChildFrame*)GetParentFrame())->
      GetStatusBar()->SetPaneOneAndLock(
        ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA) );
  }
  else
  {
    TC_ITEM TabCtrlItem;
    TabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
    TabCtrlItem.lParam = NULL;

    CString szBuffer;

    CCTAChartData* pCmptChartData = NULL;
    CCTAChartData* pBktChartData = NULL;

    // this is the fun part...we have 2 arrays (1 for cmpts, 1 for bkts),
    //    and, we must loop through both to build the tabs in the correct order.
    int iCCmptSeq = -1;
    int iBCmptSeq = -1;
    int iCmptIdx = 0;
    int iBktIdx = 0;
    int iTabIndex = 0;
    BOOL bMoreCmpts, bMoreBkts;
    BOOL bContinue = TRUE;
    int iSortKeyCmpt = 100000;
    int iSortKeyBkt = 100000;
    while ( bContinue )
    {
      if ( bMoreCmpts = ( iCmptIdx < iCmptArraySize ) )
      {
        pCmptChartData = m_CmptChart.GetChartData(iCmptIdx);
        iCCmptSeq = pCmptChartData->GetCmptSeq();
        iSortKeyCmpt = 
          CHART_DATA_SORT_KEY(pCmptChartData->GetCmptSeq(), pCmptChartData->GetCmpt()[0], pCmptChartData->GetNestLevel());
      }
      if ( bMoreBkts = ( iBktIdx < iBktArraySize ) )
      {
        pBktChartData = m_BktChart.GetChartData(iBktIdx);
        iBCmptSeq = pBktChartData->GetCmptSeq();
        iSortKeyBkt = CHART_DATA_SORT_KEY(pBktChartData->GetCmptSeq(), pBktChartData->GetCmpt()[0], pBktChartData->GetNestLevel());
      }

      if ( bMoreCmpts && ( iSortKeyCmpt <= iSortKeyBkt ) ) // build this tab from the cmpt
      {
        szBuffer.Format( "%s", pCmptChartData->GetCmpt() );
        iCmptIdx++;
        TabCtrlItem.lParam = (LPARAM)pCmptChartData;
      }
      else  // build this tab from the bucket (if we have some more)
      {
        if ( bMoreBkts )
        {
          // if the bucket label is all numeric, we display the compartment with the bucket label,
          // else we display only the bucket label
          szBuffer = pBktChartData->GetBktLabel();
          BOOL bIsNumeric = TRUE;
          for(int i=0; i<szBuffer.GetLength(); i++)
          {
            if(!isdigit(szBuffer[i]))
            { bIsNumeric = FALSE; break; }  // not all numeric, set boolean and get out of loop
          }
          if(bIsNumeric)  // all numeric...add the cmpt
			szBuffer.Format( "%s%s%s", pBktChartData->GetCmpt(), pBktChartData->GetBktLabel(), pBktChartData->getScxEtanche() ? "(E)" : "" );//DT35037 - Lyria lot 3

         iBktIdx++;
         TabCtrlItem.lParam = (LPARAM)pBktChartData;
        }
      }

      if ( ((bMoreCmpts) || (bMoreBkts)) && TabCtrlItem.lParam )
      {
        TabCtrlItem.pszText = (LPSTR)(LPCSTR)szBuffer;
	    if (_sLastTabUsed == szBuffer)
	      nLastTab = iTabIndex;
	    m_ChartTabCtrl.InsertItem( iTabIndex++, &TabCtrlItem );
        TabCtrlItem.lParam = NULL;
      }
      else
        bContinue = FALSE;  // we're done...let's get out !
    }

    // resize the tabs to make them smaller
    if (m_ChartTabCtrl.GetItemCount())
    {
      if ( ! m_bTabResized )
      {
        CRect TabRect;
        CSize TabSize;
        // reduce the size of the tab (button) to 1/4
				// DM7978 - LME - YI-4625 - modification de la valeur du coefficient de reduction pour que les onglets s'affichent correctement quand CC > 9
				double dTabReduction = TAB_RED_COEFF; // originalement a 0.25
				// DM7978 - LME - FIN
        m_ChartTabCtrl.GetItemRect( 0, &TabRect );
        TabSize.cx = long( (TabRect.right - TabRect.left) * dTabReduction );  // resize the tabs to 1/3 of their width
        TabSize.cy = TabRect.bottom - TabRect.top;
        m_ChartTabCtrl.SetItemSize( TabSize );
        m_bTabResized = TRUE;
      }

      CString szTabLabel;
      m_iCurTabSelection = 0;
      for (int i = 0; i < m_ChartTabCtrl.GetItemCount( ); i++)
      {
        szTabLabel = GetTextFromTabItem(i);
        if ( szTabLabel == m_szCurTabText )
        {
          m_iCurTabSelection = i;  // found it, let's use it...
					nLastTab = i; // correctif ano 159669
          break;
        }
      }
      int iItemCount = m_ChartTabCtrl.GetItemCount();
      m_ChartTabCtrl.SetCurSel( m_iCurTabSelection );  // select the first tab by default

      // which chart is selected (cmpt or bkt) ?
      CCTAChartData* pCTAChartData = GetDataFromTabItem();
      if ( pCTAChartData == NULL )
        return;

      COLORREF clrFt;
	  clrFt = (GetAltFlag()) ? 
	  ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkAlt() : 
	  ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();

	  if ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt )
      {
	    m_pCurSelChart = &m_CmptChart;
	    m_pNotSelChart = &m_BktChart;
	    m_CmptChart.SetForegroundColor (clrFt); 
        /*m_CmptChart.SetYTitle("€uros");
	    m_CmptChart.SetYTitleRotation(XRT_ROTATE_90);*/
      }
      else
      {
	    m_pCurSelChart = &m_BktChart;
	    m_pNotSelChart = &m_CmptChart;
	    m_BktChart.SetForegroundColor (clrFt); 
        /*m_BktChart.SetYTitle("€uros");
	    m_BktChart.SetYTitleRotation(XRT_ROTATE_90);*/
      }

      // set the appropriate data object as currently selected, then get it
      m_pCurSelChart->SetCurChartData( pCTAChartData );
      DrawGraph();
    }

    // Set text in status bar control
    ((CChildFrame*)GetParentFrame())->GetStatusBar()->SetPaneOneAndLock(
        ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DONE) );

    if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
    {
	  int idres;
	  CString sb = GetBoolSet();
	  if (sb == RDISPLAYALL)
		idres = IDS_FULL_DISPLAY;
	  else if (sb == RDISPLAYMEC)
		idres = IDS_MEC_DISPLAY;
	  else if (sb == RDISPLAYECH)
		idres = IDS_HEC_DISPLAY;
	  else if (sb == RDISPLAYHIS)
		idres = IDS_HIS_DISPLAY;
	  else
		idres = IDS_PERSONAL;
	  ((CChildFrame*)GetParentFrame())->
	    GetStatusBar()->SetPaneOneAndLock( ((CCTAApp*)APP)->GetResource()->LoadResString(idres) );

	  if ((sb == RDISPLAYALL) && (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() ))
	  {
	    int incertain, confirme;
	    YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
		CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
		if ( ((CCTAApp*)APP)->GetDocument()->GetHoiTra(pTranche, incertain, confirme))
		{
		  CString s, ss;
		  if (incertain == -2)
		  {
		    s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NOTINC_CONF);
			ss.Format (s, confirme);
		  }
		  else
		  {
		    if (confirme >= -1)
		    {
		      s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INCERTAIN_CONFIRME);
			  ss.Format (s, incertain, confirme);
		    }
		    else
		    {
		      s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INCERTAIN);
			  ss.Format (s, incertain, "  ");
		    }
		  }
          ((CChildFrame*)GetParentFrame())-> GetStatusBar()->SetPaneOneAndLock (ss);
		}
	  }
    }
  }
  if (nLastTab != -1)
  {
	m_ChartTabCtrl.SetCurSel( nLastTab );
	LRESULT result;
	OnChartSelchange( NULL, &result );
  }
  // ano 144682, pour forcer recalcul de l'affichage des tab si la zone n'est pas assez large pour tout afficher.
  CRect rect;
  m_ChartTabCtrl.GetClientRect(&rect);
  m_ChartTabCtrl.PostMessage (WM_SIZE, SIZE_RESTORED, MAKELPARAM (rect.right, rect.bottom));
}

//////////////////////////////////////
// Update the help dialog when the graph changes due to user selection
//
void CRecDHistView::UpdateHelpDialog()
{
  // manage the help dialogs
  CCTAChartHelpDlg* pHelpDlg = m_pCurSelChart->GetHelpDlg();
  CCTAChartHelpDlg* pNotHelpDlg = m_pNotSelChart->GetHelpDlg();

  if ( ( pHelpDlg ) && ( IsWindow( pHelpDlg->m_hWnd ) ) && ( pHelpDlg->GetSafeHwnd() ) )
  {
	  pHelpDlg->Invalidate(TRUE);
	  pHelpDlg->UpdateWindow();
  }
  else
  {
    // was the help window displayed for the other graph?
    if ( pNotHelpDlg )
    {
      m_pNotSelChart->DestroyHelpDlg();
      m_pCurSelChart->CreateHelpDlg();
      CPoint point(0,0);
      m_pCurSelChart->DisplayHelpDlg(point );
    }
  }
}

CString CRecDHistView::GetBoolSet()
{
  char cBoolSet[25];
  cBoolSet[0] = m_bDispEC ? '1' : '0';
  cBoolSet[1] = m_bDispResHoldTot ? '1' : '0';
  cBoolSet[2] = m_bDispResHoldIdv ? '1' : '0';
  cBoolSet[3] = m_bDispResHoldAvg ? '1' : '0';
  cBoolSet[4] = m_bDispHisto1ResHoldTot ? '1' : '0';
  cBoolSet[5] = m_bDispHisto2ResHoldTot ? '1' : '0';
  cBoolSet[6] = m_bDispHisto3ResHoldTot ? '1' : '0';
  cBoolSet[7] = m_bDispPrevision ? '1' : '0';
  cBoolSet[8] = 0;

  CString s(cBoolSet);
  return s;
}

void CRecDHistView::SetBoolSet (CString sBoolSet)
{
  m_bDispEC = (sBoolSet[0] == '1');
  m_bDispResHoldTot = (sBoolSet[1] == '1'); 
  m_bDispResHoldIdv = (sBoolSet[2] == '1');
  m_bDispResHoldAvg = (sBoolSet[3] == '1');
  m_bDispHisto1ResHoldTot = (sBoolSet[4] == '1');
  m_bDispHisto2ResHoldTot = (sBoolSet[5] == '1');
  m_bDispHisto3ResHoldTot = (sBoolSet[6] == '1');
  m_bDispPrevision = (sBoolSet[7] == '1');
}

//////////////////////////////////////
// Write specific view configuration information to the registry
//
void CRecDHistView::WriteViewConfig()
{
  // save the graph flags in the registry
  //
  CString szProfileKey;
  ((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

  CSplitChildFrame* pWnd = (CSplitChildFrame*)GetParentFrame();
  pWnd->WriteSplitterConfig(szProfileKey);

  APP->WriteProfileInt( szProfileKey, GRAPH_SCALE_FACTOR, m_pCurSelChart->GetCurScaling() );
  APP->WriteProfileInt( szProfileKey, ROTATE_X_LABEL, ((CCTAApp*)APP)->m_bRRDRotatelabel ? 1 : 0 );
  APP->WriteProfileString (szProfileKey, "DisplayCurve", GetBoolSet());
  APP->WriteProfileString( szProfileKey, "LastTabUsed", _sLastTabUsed );
  if (!_nSubViewID) {
      CRecDHistView* view = getOtherView();
      if (view) {
	  APP->WriteProfileString( szProfileKey, "LastTabUsed_2", view->_sLastTabUsed );
      }
  }
}

//////////////////////////////////////
// Restore specific view configuration information from the registry
//
void CRecDHistView::RestoreViewConfig()
{
    // restore the graph flags from the registry
    //
    CString szProfileKey;
    ((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

    enum CCTAChart::GraphScaleIndex eCurScale = (enum CCTAChart::GraphScaleIndex)
    APP->GetProfileInt(szProfileKey, GRAPH_SCALE_FACTOR, (unsigned int)CCTAChart::eMaxScaleIdx);
    CString sbs = APP->GetProfileString(szProfileKey, "DisplayCurve", RDISPLAYALL);
	if (sbs.GetLength() < 8)
	  sbs = RDISPLAYALL;
    SetBoolSet (sbs);
    ((CCTAApp*)APP)->m_RECDBoolSet = sbs;

    m_CmptChart.SetRepaint( FALSE );
    m_BktChart.SetRepaint( FALSE );
    m_CmptChart.SetCurScaling(eCurScale);
    m_BktChart.SetCurScaling(eCurScale);
    m_CmptChart.ScaleGraph(m_CmptChart.GetCurScaling());
    m_BktChart.ScaleGraph(m_BktChart.GetCurScaling());
    m_BktChart.SetRepaint( TRUE );
    m_CmptChart.SetRepaint( TRUE );

    CSplitChildFrame* pWnd = (CSplitChildFrame*)GetParentFrame();
    pWnd->RestoreSplitterConfig(szProfileKey);

    int rotated = APP->GetProfileInt(szProfileKey, ROTATE_X_LABEL, 0);
    m_CmptChart.SetRotateLabelsFlag( !rotated );
    //if (rotated)	  // Par default ce n'est pas tourné.
	OnGraphRotateLabels();
    _sLastTabUsed = APP->GetProfileString(szProfileKey, "LastTabUsed", "" );
}

/////////////////////////////////////////////////////////////////////////////
// CRecDHistChart

BEGIN_MESSAGE_MAP(CRecDHistChart, CCTAChart)
	//{{AFX_MSG_MAP(CRecDHistChart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////
// constructor
//
CRecDHistChart::CRecDHistChart()
{
  m_bRotateLabels = ((CCTAApp*)APP)->m_bRRDRotatelabel;
  this->m_PrevisionLabel = "";
  /*m_bRotateLabels == true ? 
  SetXAnnotationRotation(XRT_ROTATE_90) : 
  SetXAnnotationRotation(XRT_ROTATE_NONE); */
}

/////////////////////////////////////////////////////////////////////////////
// CRecDHistHelp dialog

CRecDHistHelp::CRecDHistHelp(CRecDHistChart* pParent /*=NULL*/)
  : CCTAChartHelpDlg(pParent)
{
	this->PrevisionDispo = 2;
	this->needRedraw = 0;
}

void CRecDHistHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecDHistHelp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRecDHistHelp, CCTAChartHelpDlg)
	//{{AFX_MSG_MAP(CRecDHistHelp)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// build the 1st title
void CRecDHistHelp::GetTitle1( CString& szTitle )
{
  // get the Cmpt of the selected data point for the 1st title
  CCTAChartData* pChartData = 
    ((CRecDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
  if ( pChartData == NULL )
    return;

  CString szRecD = XrtGetNthPointLabel(m_pParent->m_hChart, m_iCurSelectPt);
  szTitle.Format("%s: %s", 
    ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RRD),
    szRecD);
}

// build the 2nd title
void CRecDHistHelp::GetTitle2( CString& szTitle )
{
//  szTitle = " ";
  // get the Cmpt of the selected data point for the 1st title
  CCTAChartData* pChartData = 
    ((CRecDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
  if ( pChartData == NULL )
    return;

  int iCurSelTab = 
    ((CRecDHistView*)m_pParent->GetParentView())->GetTabCtrl()->GetCurSel();  // get currently selected tab

  // get the Compartment (text of the tab) for the title
  char szTabText[50];
  TC_ITEM TabCtrlItem;
  TabCtrlItem.mask = TCIF_TEXT;
  TabCtrlItem.pszText = szTabText;
  TabCtrlItem.cchTextMax = sizeof(szTabText);
  ((CRecDHistView*)m_pParent->GetParentView())->
      GetTabCtrl()->GetItem(iCurSelTab, &TabCtrlItem);

  CString szRecD = XrtGetNthPointLabel(m_pParent->m_hChart, m_iCurSelectPt);
  if (pChartData->GetGraphType() == CCTAChartData::eCmpt)
  {
    szTitle.Format("%s %s", 
		((CCTAApp*)APP)->GetResource()->LoadResString(((CRecDHistView*)m_pParent->GetParentView())->GetAltFlag() ? IDS_SOUS_CONT : IDS_CMPT),
      TabCtrlItem.pszText);
  }
  else
  {
    szTitle.Format("%s %s", 
		((CCTAApp*)APP)->GetResource()->LoadResString(((CRecDHistView*)m_pParent->GetParentView())->GetAltFlag() ? IDS_SCI : IDS_BUCKET),
      TabCtrlItem.pszText);
  }
}

// Get the longest label to be printed.  This is used for sizing the width
//
CSize CRecDHistHelp::GetExtentLongestLabel( CPaintDC& dc )
{
  CSize sizeTextLabel(0, 0);

  // get the Cmpt of the selected data point for the 1st title
  CCTAChartData* pChartData = 
    ((CRecDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
  if ( pChartData == NULL )
    return sizeTextLabel;

  int iBeg, iEnd, iBeg2, iEnd2;
  if (pChartData->GetGraphType() == CCTAChartData::eCmpt)
  {
    //if ( ! ((CRecDHistView*)m_pParent->GetParentView())->GetRevenueDisplay() )  // are we graphing auth or rev data ?
    //{
      iBeg = 0;
      iEnd = ((CRecDHistView*)m_pParent->GetParentView())->eCmptNumDataSets;
      iBeg2 = 0;
      iEnd2 = ((CRecDHistView*)m_pParent->GetParentView())->eCmptResHoldStdvLow;
  }
  else  //  we are graphing bucket data...
  {
    iBeg = ((CRecDHistView*)m_pParent->GetParentView())->eBktResHoldTot;
    iEnd = ((CRecDHistView*)m_pParent->GetParentView())->eBktNumDataSets;
    iBeg2 = ((CRecDHistView*)m_pParent->GetParentView())->eBktResHoldStdvHigh;
    iEnd2 = ((CRecDHistView*)m_pParent->GetParentView())->eBktResHoldStdvLow;
  }

  // loop through all the labels, and find the longest one (varies from language to language)
  int iLongest = 0;
  CString szTextLabel;

  // must loop through the enumerated variables representing sets of data
  int i;
  for( i = iBeg; i < iEnd; i++ )
  {
    szTextLabel = XrtGetNthSetLabel( m_pParent->m_hChart, i );
    sizeTextLabel = dc.GetOutputTextExtent( szTextLabel );  // get the text extent
    if ( sizeTextLabel.cx > iLongest )
      iLongest = sizeTextLabel.cx;
  }
  for( i = iBeg2; i < iEnd2; i++ )
  {
    szTextLabel = XrtGetNthSetLabel2( m_pParent->m_hChart, i );
    sizeTextLabel = dc.GetOutputTextExtent( szTextLabel );  // get the text extent
    if ( sizeTextLabel.cx > iLongest )
      iLongest = sizeTextLabel.cx;
  }
  sizeTextLabel.cx = iLongest;
  return sizeTextLabel;
}

// Get the text label to be drawn
//
BOOL CRecDHistHelp::GetTextLabel( int iDataSet, CString& szTextLabel, int iCurSelectSet )
{
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
  { // grosse astuce, si  hoiTraFlag, les courbes correspondantes ont la même position (5) 
    // dans les deux dataset cmpt et bkt. mais je teste les deux pour faire plus joli.
    if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eCmptHoiTra )
	  return FALSE;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eCmptHoiTraIdv )
	  return FALSE;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eBktHoiTra )
	  return FALSE;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eBktHoiTraIdv )
	  return FALSE;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eCmptHisto1HoiTra )
	  return FALSE;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eCmptHisto2HoiTra )
	  return FALSE;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eCmptHisto3HoiTra )
	  return FALSE;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eBktHisto1HoiTra )
	  return FALSE;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eBktHisto2HoiTra )
	  return FALSE;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eBktHisto3HoiTra )
	  return FALSE;
  }
  if ( iDataSet == XRT_DATA )
    szTextLabel = XrtGetNthSetLabel( m_pParent->m_hChart, iCurSelectSet );
  else
    szTextLabel = XrtGetNthSetLabel2( m_pParent->m_hChart, iCurSelectSet );

  return TRUE;
}

// Get the text label to be drawn
//
CString CRecDHistHelp::GetValue( int iDataSet, int iCurSelectSet, int iCurSelectPt  )
{
  CString szValue;
  double dValue = 0;

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
  {
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eCmptHoiTra )
	  return szValue;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eCmptHoiTraIdv )
	  return szValue;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eBktHoiTra )
	  return szValue;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eBktHoiTraIdv )
	  return szValue;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eCmptHisto1HoiTra )
	  return szValue;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eCmptHisto2HoiTra )
	  return szValue;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eCmptHisto3HoiTra )
	  return szValue;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eBktHisto1HoiTra )
	  return szValue;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eBktHisto2HoiTra )
	  return szValue;
	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eBktHisto3HoiTra )
	  return szValue;
  }


  if ( iDataSet == XRT_DATA )
  {
    if ( XrtDataGetLastPoint( m_hXrtData, iCurSelectSet ) < iCurSelectPt )
    	return szValue;  // szValue is empty, so we shouldn't draw anything...
    if ( XrtDataGetFirstPoint( m_hXrtData, iCurSelectSet ) > iCurSelectPt )
      return szValue;  // szValue is empty, so we shouldn't draw anything...
  }
  else if ( iDataSet == XRT_DATA2 )
  {
    if ( XrtDataGetLastPoint( m_hXrtData2, iCurSelectSet ) < iCurSelectPt )
      return szValue;  // szValue is empty, so we shouldn't draw anything...
    if ( XrtDataGetFirstPoint( m_hXrtData2, iCurSelectSet ) > iCurSelectPt )
      return szValue;  // szValue is empty, so we shouldn't draw anything...
  }
  else
  {
    return szValue;
  }

  if ( iDataSet == XRT_DATA )
  {
    dValue = XrtDataGetYElement( m_hXrtData, iCurSelectSet, m_iCurSelectPt );
	

	if ( iCurSelectSet == ((CRecDHistView*)m_pParent->GetParentView())->eCmptPrevision )
	{
		if (dValue == INVALID_GRAPH_VALUE || YM_Set<YmIcPrevisionDom>::FromKey(PREVISION_REVENU_KEY)->IsEmpty() || dValue == XRT_HUGE_VAL)
		{
			if (this->PrevisionDispo != 0)
			{
				this->PrevisionDispo = 0;
				needRedraw = 1;
			}
			(((CRecDHistView*)m_pParent->GetParentView()))->GetCmptChart()->SetNthSetLabel(((CRecDHistView*)m_pParent->GetParentView())->eCmptPrevision, "Prévision indisponible");
			(((CRecDHistView*)m_pParent->GetParentView()))->GetCmptChart()->SetPrevisionLabel("Prévision indisponible");
			CString xValueStr = XrtGetNthPointLabel(m_pParent->m_hChart, m_iCurSelectPt);
			//DT Prevision - Récupération du JX pour lequel on souhaite afficher la légende
			int xValue = 999;
			if (xValueStr[0] == '-') //Transformation du J en int, puis transformation en J-X
			{
				char *xValueChar;
				xValueChar = &(xValueStr.GetBuffer()[1]);
				xValue = atoi(xValueChar);
			}
			else
			{
				char *xValueChar;
				xValueChar = (xValueStr.GetBuffer());
				xValue = atoi(xValueChar) * (-1);
			}

			if (xValue < ((CRecDHistView*)m_pParent->GetParentView())->dernierJX)//activation / désactivation prévision dans la légende en fonction du dernierJX collecté
				szValue = " ";//DT Prevision affiche " " à la place du nbr d'auto prevision
			else
				szValue = "";//Désactivation de la ligne prévision
		}
		else
		{
			if (this->PrevisionDispo != 1)
			{
				this->PrevisionDispo = 1;
				needRedraw = 1;
			}
			(((CRecDHistView*)m_pParent->GetParentView()))->GetCmptChart()->SetPrevisionLabel("Prévision");
			(((CRecDHistView*)m_pParent->GetParentView()))->GetCmptChart()->SetNthSetLabel(((CRecDHistView*)m_pParent->GetParentView())->eCmptPrevision, "Prévision");
			szValue.Format( "%3.0f", dValue );
		}
		//(((CRecDHistView*)m_pParent->GetParentView()))->GetCmptChart()->UpdateData(1);
		//(((CRecDHistView*)m_pParent->GetParentView()))->GetCmptChart()->UpdateWindow();
		
		//(((CRecDHistView*)m_pParent->GetParentView()))->GetCmptChart()->SetGraphProperties(CCTAChartData::eCmpt);
		if (needRedraw)
		{
			// NBN - ANO 106725
			this->UpdateWindow();			
		}
		//(((CRecDHistView*)m_pParent->GetParentView()))->Invalidate();
	}
    else if (dValue == INVALID_GRAPH_VALUE)
      szValue = "N/A";
    else
      szValue.Format( "%3.0f", dValue );
  }
  else  // iDataSet == XRT_DATA2
  {
    if ( iCurSelectSet == CRecDHistView::eBktResHoldStdvHigh )
    {
      // here we force the "fourchette" to be drawn as a low to a high value
      double dValueLow = XrtDataGetYElement( m_hXrtData2, CRecDHistView::eBktResHoldStdvLow, m_iCurSelectPt );
      double dValueHigh = XrtDataGetYElement( m_hXrtData2, CRecDHistView::eBktResHoldStdvHigh, m_iCurSelectPt );
      if (dValueLow == INVALID_GRAPH_VALUE || dValueHigh == INVALID_GRAPH_VALUE)
		szValue = "N/A";
      else
        szValue.Format( "%3.0f - %3.0f", dValueLow, dValueHigh );
    }
    else
    {
      szValue.Empty();  // indicate that we should not draw any value...
    }
  }
  return szValue;
}

// Send 0 if no check box have to be displayed
// Send button identifier if a check box have to be displayed.
// Positive value if checked, else negative value.
int CRecDHistHelp::GetCheckBox( int iDataSet, int iCurSelectSet, int iCurSelectPt)
{
   if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetControlRRDCurveFlag() )
	return 0; // on reserve cette fonction qu'a la SNCF et Thalys

  int rtn = 0;

  // get the Cmpt of the selected data point for the 1st title
  CCTAChartData* pChartData = 
    ((CRecDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
  if ( pChartData == NULL )
    return 0;

  if ( iDataSet == XRT_DATA )
  {
    if ( XrtDataGetLastPoint( m_hXrtData, iCurSelectSet ) < iCurSelectPt )
      return 0;  
    if ( XrtDataGetFirstPoint( m_hXrtData, iCurSelectSet ) > iCurSelectPt )
      return 0; 
  }
  else if ( iDataSet == XRT_DATA2 )
  {
    if ( XrtDataGetLastPoint( m_hXrtData2, iCurSelectSet ) < iCurSelectPt )
      return 0;  // szValue is empty, so we shouldn't draw anything...
    if ( XrtDataGetFirstPoint( m_hXrtData2, iCurSelectSet ) > iCurSelectPt )
      return 0;  // szValue is empty, so we shouldn't draw anything...
  }
  else
  {
    return 0;
  }

  CRecDHistView* ppapa = (CRecDHistView*)m_pParent->GetParentView();
  if ( iDataSet == XRT_DATA )
  {
	int rtnval = 10000 + iCurSelectSet;
	if (pChartData->GetGraphType() == CCTAChartData::eCmpt)
	{
	  if ( iCurSelectSet == ppapa->eCmptResHoldTot)
		return (ppapa->m_bDispResHoldTot ? rtnval : -rtnval);
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	  { // certainement inutile, mais sans danger.
	    if ( iCurSelectSet == ppapa->eCmptHoiTra)
		  return (ppapa->m_bDispResHoldTot ? rtnval : -rtnval);
		if ( iCurSelectSet == ppapa->eCmptHoiTraIdv)
		  return (ppapa->m_bDispResHoldIdv ? rtnval : -rtnval);
	  }
	  if ( iCurSelectSet == ppapa->eCmptResHoldIdv)
		return (ppapa->m_bDispResHoldIdv ? rtnval : -rtnval);
	  if ( iCurSelectSet == ppapa->eCmptResHoldAvg)
		return (ppapa->m_bDispResHoldAvg ? rtnval : -rtnval);
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	  {
		if ( iCurSelectSet == ppapa->eCmptHisto1ResHoldTot)
		  return (ppapa->m_bDispHisto1ResHoldTot ? rtnval : -rtnval);
		if ( iCurSelectSet == ppapa->eCmptHisto2ResHoldTot)
		  return (ppapa->m_bDispHisto2ResHoldTot ? rtnval : -rtnval);
		if ( iCurSelectSet == ppapa->eCmptHisto3ResHoldTot)
		  return (ppapa->m_bDispHisto3ResHoldTot ? rtnval : -rtnval);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
		{
		  if ( iCurSelectSet == ppapa->eCmptHisto1HoiTra)
		    return (ppapa->m_bDispHisto1ResHoldTot ? rtnval : -rtnval);
		  if ( iCurSelectSet == ppapa->eCmptHisto2HoiTra)
		    return (ppapa->m_bDispHisto2ResHoldTot ? rtnval : -rtnval);
		  if ( iCurSelectSet == ppapa->eCmptHisto3HoiTra)
		    return (ppapa->m_bDispHisto3ResHoldTot ? rtnval : -rtnval);
		}
	  }
	  /*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
	  {*/
	  if (!((CCTAApp*)APP)->GetRhealysFlag() && !ppapa->GetAltFlag())
	    if ( iCurSelectSet == ppapa->eCmptPrevision)
		  return (ppapa->m_bDispPrevision ? rtnval : -rtnval);
	  //}
	}
	else // eBkt
	{
	  if ( iCurSelectSet == ppapa->eBktResHoldTot)
		return (ppapa->m_bDispResHoldTot ? rtnval : -rtnval);
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	  { // certainement inutile, mais sans danger.
	    if ( iCurSelectSet == ppapa->eBktHoiTra)
		  return (ppapa->m_bDispResHoldTot ? rtnval : -rtnval);
		if ( iCurSelectSet == ppapa->eBktHoiTraIdv)
		  return (ppapa->m_bDispResHoldIdv ? rtnval : -rtnval);
	  }
	  if ( iCurSelectSet == ppapa->eBktResHoldIdv)
		return (ppapa->m_bDispResHoldIdv ? rtnval : -rtnval);
	  if ( iCurSelectSet == ppapa->eBktResHoldAvg)
		return (ppapa->m_bDispResHoldAvg ? rtnval : -rtnval);
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	  {
		if ( iCurSelectSet == ppapa->eBktHisto1ResHoldTot)
		  return (ppapa->m_bDispHisto1ResHoldTot ? rtnval : -rtnval);
		if ( iCurSelectSet == ppapa->eBktHisto2ResHoldTot)
		  return (ppapa->m_bDispHisto2ResHoldTot ? rtnval : -rtnval);
		if ( iCurSelectSet == ppapa->eBktHisto3ResHoldTot)
		  return (ppapa->m_bDispHisto3ResHoldTot ? rtnval : -rtnval);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	    {
		  if ( iCurSelectSet == ppapa->eBktHisto1HoiTra)
		    return (ppapa->m_bDispHisto1ResHoldTot ? rtnval : -rtnval);
		  if ( iCurSelectSet == ppapa->eBktHisto2HoiTra)
		    return (ppapa->m_bDispHisto2ResHoldTot ? rtnval : -rtnval);
		  if ( iCurSelectSet == ppapa->eBktHisto3HoiTra)
		    return (ppapa->m_bDispHisto3ResHoldTot ? rtnval : -rtnval);
		}
	  }
	  /*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
	  {
	    if ( iCurSelectSet == ppapa->eBktPrevision)
		  return (ppapa->m_bDispPrevision ? rtnval : -rtnval);
	  }*/
	}
  }
  else // iDataSet == XRT_DATA2
	return (ppapa->m_bDispEC ? 20000 : -20000);
  return 0;
}

void CRecDHistHelp::ClickDynaButton( int nId, int checked)
{
  TRACE ("Click on DynaButton nId=%d, checked=%d\n", nId, checked);
  CCTAChartData* pChartData = 
    ((CRecDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
  if (nId >= 20000) // XRT_DATA2
    ((CRecDHistView*)m_pParent->GetParentView())->RevMasqueCourbe (pChartData->GetGraphType(), nId - 20000, TRUE);
  else
	((CRecDHistView*)m_pParent->GetParentView())->RevMasqueCourbe (pChartData->GetGraphType(), nId - 10000, FALSE);
  ((CRecDHistView*)m_pParent->GetParentView())->ChangeRecDDisplay(FALSE);
}

// Translate the dataset to a cta graph style (color)
//
int CRecDHistHelp::GetCTADataStyleIndex( int iDataSet, int iCurSelectSet )
{
  // get the Cmpt of the selected data point for the 1st title
  CCTAChartData* pChartData = 
    ((CRecDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
  if ( pChartData == NULL )
    return CCTAChart::eVoidStyleIdx;

    if (pChartData->GetGraphType() == CCTAChartData::eCmpt)
    {
        if ( iDataSet == XRT_DATA )
        {
          // translate the dataset to a cta graph style (color)
          if ( (((CRecDHistView*)m_pParent->GetParentView())->eCmptResHoldAvg == iCurSelectSet) )
              return CCTAChart::eResHoldAvgStyleIdx;
          if ( (((CRecDHistView*)m_pParent->GetParentView())->eCmptResHoldTot == iCurSelectSet) )
              return CCTAChart::eResHoldTotStyleIdx;
		  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		  { 
		    if ( (((CRecDHistView*)m_pParent->GetParentView())->eCmptHoiTra == iCurSelectSet) )
              return CCTAChart::eHoiTraStyleIdx;
			if ( (((CRecDHistView*)m_pParent->GetParentView())->eCmptHoiTraIdv == iCurSelectSet) )
              return CCTAChart::eHoiTraIdvStyleIdx;
		  }
          if ( (((CRecDHistView*)m_pParent->GetParentView())->eCmptResHoldIdv == iCurSelectSet) )
              return CCTAChart::eRevTicketeStyleIdx;

		  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
		  {
            if ( (((CRecDHistView*)m_pParent->GetParentView())->eCmptHisto1ResHoldTot == iCurSelectSet) )
                return CCTAChart::eHisto1ResHoldTotStyleIdx;
            if ( (((CRecDHistView*)m_pParent->GetParentView())->eCmptHisto2ResHoldTot == iCurSelectSet) )
                return CCTAChart::eHisto2ResHoldTotStyleIdx;
            if ( (((CRecDHistView*)m_pParent->GetParentView())->eCmptHisto3ResHoldTot == iCurSelectSet) )
                return CCTAChart::eHisto3ResHoldTotStyleIdx;
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		    {
			  if ( (((CRecDHistView*)m_pParent->GetParentView())->eCmptHisto1HoiTra == iCurSelectSet) )
                return CCTAChart::eHisto1HoiTraStyleIdx;
			  if ( (((CRecDHistView*)m_pParent->GetParentView())->eCmptHisto2HoiTra == iCurSelectSet) )
                return CCTAChart::eHisto2HoiTraStyleIdx;
			  if ( (((CRecDHistView*)m_pParent->GetParentView())->eCmptHisto3HoiTra == iCurSelectSet) )
                return CCTAChart::eHisto3HoiTraStyleIdx;
			}
		  }
		  /*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
		  {*/
		  //if (!((CRecDHistView*)m_pParent->GetParentView()-> )
		    if (!((CCTAApp*)APP)->GetRhealysFlag() && (((CRecDHistView*)m_pParent->GetParentView())->eCmptPrevision == iCurSelectSet) )
              return CCTAChart::ePrevisionStyleIdx;
		  //}
        }
        else  // data set is XRT_DATA2
        {
          // translate the dataset to a cta graph style (color)
          switch ( iCurSelectSet )
          {
		  case /*((CRecDHistView*)m_pParent->GetParentView())->*/CRecDHistView::eCmptResHoldStdvHigh : // JZZ portage Visual.net
            return CCTAChart::eResHoldStdvHighStyleIdx;
          case /*((CRecDHistView*)m_pParent->GetParentView())->*/CRecDHistView::eCmptResHoldStdvLow : // JZZ portage Visual.net
            return CCTAChart::eResHoldStdvHighStyleIdx;  // same as Stdv High because we want the same color and style for both
          }
        }
    }
    else  // Bucket graph
    {
      if ( iDataSet == XRT_DATA )
      {
		// translate the dataset to a cta graph style (color)
        if ( (((CRecDHistView*)m_pParent->GetParentView())->eBktResHoldAvg == iCurSelectSet) )
            return CCTAChart::eResHoldAvgStyleIdx;
        if ( (((CRecDHistView*)m_pParent->GetParentView())->eBktResHoldTot == iCurSelectSet) )
            return CCTAChart::eResHoldTotStyleIdx;
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{ 
		  if ( (((CRecDHistView*)m_pParent->GetParentView())->eBktHoiTra == iCurSelectSet) )
            return CCTAChart::eHoiTraStyleIdx;
		  if ( (((CRecDHistView*)m_pParent->GetParentView())->eBktHoiTraIdv == iCurSelectSet) )
            return CCTAChart::eHoiTraIdvStyleIdx;
		}
		if ( (((CRecDHistView*)m_pParent->GetParentView())->eBktResHoldIdv == iCurSelectSet) )
            return CCTAChart::eRevTicketeStyleIdx;
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
		{
          if ( (((CRecDHistView*)m_pParent->GetParentView())->eBktHisto1ResHoldTot == iCurSelectSet) )
              return CCTAChart::eHisto1ResHoldTotStyleIdx;
          if ( (((CRecDHistView*)m_pParent->GetParentView())->eBktHisto2ResHoldTot == iCurSelectSet) )
              return CCTAChart::eHisto2ResHoldTotStyleIdx;
          if ( (((CRecDHistView*)m_pParent->GetParentView())->eBktHisto3ResHoldTot == iCurSelectSet) )
              return CCTAChart::eHisto3ResHoldTotStyleIdx;
		  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	      {
		    if ( (((CRecDHistView*)m_pParent->GetParentView())->eBktHisto1HoiTra == iCurSelectSet) )
              return CCTAChart::eHisto1HoiTraStyleIdx;
			if ( (((CRecDHistView*)m_pParent->GetParentView())->eBktHisto2HoiTra == iCurSelectSet) )
              return CCTAChart::eHisto2HoiTraStyleIdx;
			if ( (((CRecDHistView*)m_pParent->GetParentView())->eBktHisto3HoiTra == iCurSelectSet) )
              return CCTAChart::eHisto3HoiTraStyleIdx;
		  }
		}
		/*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
		{
		   if ( (((CRecDHistView*)m_pParent->GetParentView())->eBktPrevision == iCurSelectSet) )
            return CCTAChart::ePrevisionStyleIdx;
		}*/
                                                //   the enumerated value for eBktResHoldAvailSum is the same as
                                                //   eTrafficRes, and because Traffic Res is not used in this graph, 
                                                //   there is no danger of duplication.
      }
      else  // data set is XRT_DATA2
      {
        // translate the dataset to a cta graph style (color)
        switch ( iCurSelectSet )
        {
        case CRecDHistView::eBktResHoldStdvHigh :
            return CCTAChart::eResHoldStdvHighStyleIdx;
         case CRecDHistView::eBktResHoldStdvLow :
            return CCTAChart::eResHoldStdvHighStyleIdx;  // same as Stdv High because we want the same color and style for both
        }
      }
    }
    return -1;
}

void CRecDHistHelp::OnPaint()
{
  // paint all text and graphics on the client area of the dialog
  //
	CPaintDC dc(this); // device context for painting
  CFont* pOldFont = dc.SelectObject( GetHelpTextFont() );

  // get the Cmpt of the selected data point for the 1st title
  CCTAChartData* pChartData = 
    ((CRecDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
  if ( pChartData == NULL )
    return;

  int iNumberLabels = 0;
  if (pChartData->GetGraphType() == CCTAChartData::eCmpt)
  {
      // note that we've combined stdv high and stdv low onto one line, therefore decrease the number of labels by 1
      //   then we've decreased by 4 because of the addition of comptage for TRN client
      iNumberLabels = 
        ((CRecDHistView*)m_pParent->GetParentView())->eCmptNumDataSets + 
        ((CRecDHistView*)m_pParent->GetParentView())->eCmptStdvNumDataSets - 1;

	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	    iNumberLabels -= 2;

	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	  { 
		if (((CRecDHistView*)m_pParent->GetParentView())->mp_HistoData)
		  iNumberLabels -= (3 - ((CRecDHistView*)m_pParent->GetParentView())->mp_HistoData->GetNbDate());
		else                
		  iNumberLabels -= 3;
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		  iNumberLabels -= 3;
	  }
  }
  else //bkt
  {
    iNumberLabels = ((CRecDHistView*)m_pParent->GetParentView())->eBktNumDataSets +
					((CRecDHistView*)m_pParent->GetParentView())->eBktStdvNumDataSets - 1;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	    iNumberLabels -= 2;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	{ 
	  if (((CRecDHistView*)m_pParent->GetParentView())->mp_HistoData)
		iNumberLabels -= (3 - ((CRecDHistView*)m_pParent->GetParentView())->mp_HistoData->GetNbDate());
	  else
		iNumberLabels -= 3;
	  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	    iNumberLabels -= 3;
	}
  }

  // set the initial size of the help dialog window
  InitHelpWin( iNumberLabels );

  // draw the titles and return the point where drawing is to continue
  CPoint pointDraw = DrawTitles( dc );

  // draw the labels and the corresponding values
  DrawLabels( dc, pointDraw, FALSE,
	         /*((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetControlRRDCurveFlag()*/0 );

  if (this->needRedraw)
  {
	  this->needRedraw = 0;
	  Invalidate();
	  //Invalidate();
  }
  // Do not call CDialog::OnPaint() for painting messages
}

CRecDHistView* CRecDHistView::getOtherView()
{
    CWnd* parent = GetParent();
    
    for (CWnd* child = parent->GetWindow( GW_CHILD ); child; child = child->GetWindow( GW_HWNDNEXT )) {
	if (child->m_hWnd != m_hWnd && child->GetRuntimeClass() == GetRuntimeClass())
	    return (CRecDHistView*)child;
    }
    return NULL;
}

