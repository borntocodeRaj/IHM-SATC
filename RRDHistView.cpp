// RRDHistView.cpp : implementation file
//
#include "StdAfx.h"


#include "CTAChartHelpDlg.h"
#include "ComptagesView.h"
#include "RRDHistView.h"
#include "DatesHistoDlg.h"
#include "ResaRailView.h"

#include "TrancheView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Width of the panes of the status bar
#define WIDTH_PANE_INFO		200



////////////////////////////
//CHistoData
CHistoData::CHistoData(BOOL bAlt)
{
	m_MaxHistoRef = 0;
	bSci = bAlt; // depuis DM 5882, le bAlt s'applique au sous-contingent
	YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
		YM_Set<YmIcHistoEvolDom>::FromKey(bAlt ? HISTOEVOL_ALT_KEY : HISTOEVOL_KEY)->CreateIterator();
	YmIcHistoEvolDom* pHistoEvol = NULL;
	RWDate prevdate (1,1,1900);
	m_max_rrd = 0;
	m_min_rrd = 1000;
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
		if (pHistoEvol->RrdIndex() > m_max_rrd)
			m_max_rrd = pHistoEvol->RrdIndex();
		if (pHistoEvol->RrdIndex() < m_min_rrd)
			m_min_rrd = pHistoEvol->RrdIndex();
		cc = pHistoEvol->Cmpt()[0];
		idxcc = cc - 'A';

		//116770, récuperation de commentaire et histo_ref directement dans ((CCTAApp*)APP)->m_CurHistoDates
		// pour alléger la requête IDS_SQL_SELECT_RRD_DATES_HISTO
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
	pVariablesIterator = YM_Set<YmIcHistoEvolDom>::FromKey(bAlt ? HISTOEVOL_CMPT_ALT_KEY : HISTOEVOL_CMPT_KEY)->CreateIterator();
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

CHistoData::~CHistoData()
{
}

double CHistoData::GetCapa (int histoIdx, char cmpt, long rrd)
{
	if (cmpt > 'Z')
		cmpt -=32; // mise en majuscule
	YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
		YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_CMPT_ALT_KEY : HISTOEVOL_CMPT_KEY)->CreateIterator();
	YmIcHistoEvolDom* pHistoEvol = NULL;
	double capa = 0;
	for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
	{
		pHistoEvol = pVariablesIterator->Current();
		if ((pHistoEvol->HistoRef() == histoIdx) &&
			(pHistoEvol->Cmpt()[0] == cmpt) &&
			(pHistoEvol->RrdIndex() == rrd))
		{
			capa = pHistoEvol->Capacity();
			delete pVariablesIterator;
			return capa;
		}
	}
	delete pVariablesIterator;
	return capa;
}

bool CHistoData::IsRrd(long rrd)
{
	YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
		YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_ALT_KEY : HISTOEVOL_KEY)->CreateIterator();
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

double CHistoData::GetAuth (int histoIdx, char cmpt, long rrd, int bucket)
{
	if (cmpt > 'Z')
		cmpt -=32; // mise en majuscule
	YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
		YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_ALT_KEY : HISTOEVOL_KEY)->CreateIterator();
	YmIcHistoEvolDom* pHistoEvol = NULL;
	double auth = 0;
	for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
	{
		pHistoEvol = pVariablesIterator->Current();
		if ((pHistoEvol->HistoRef() == histoIdx) &&
			(pHistoEvol->Cmpt()[0] == cmpt) &&
			(pHistoEvol->RrdIndex() == rrd) &&
			(pHistoEvol->NestLevel() == bucket))
		{
			auth = pHistoEvol->AuthCur();
			delete pVariablesIterator;
			return auth;
		}
	}
	delete pVariablesIterator;
	return auth;
}

double CHistoData::GetResa (int histoIdx, char cmpt, long rrd, int bucket)
{
	if (cmpt > 'Z')
		cmpt -=32; // mise en majuscule
	YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
		YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_ALT_KEY : HISTOEVOL_KEY)->CreateIterator();
	YmIcHistoEvolDom* pHistoEvol = NULL;
	double resa = 0;
	for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
	{
		pHistoEvol = pVariablesIterator->Current();
		if ((pHistoEvol->HistoRef() == histoIdx) &&
			(pHistoEvol->Cmpt()[0] == cmpt) &&
			(pHistoEvol->RrdIndex() == rrd) &&
			(pHistoEvol->NestLevel() == bucket))
		{
			resa = pHistoEvol->ResHoldGrp() + pHistoEvol->ResHoldIdv();
			delete pVariablesIterator;
			return resa;
		}
	}
	delete pVariablesIterator;
	return resa;
}

double CHistoData::GetSumResa (int histoIdx, char cmpt, long rrd)
{
	if (cmpt > 'Z')
		cmpt -=32; // mise en majuscule
	YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
		YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_CMPT_ALT_KEY : HISTOEVOL_CMPT_KEY)->CreateIterator();
	YmIcHistoEvolDom* pHistoEvol = NULL;
	double resa = 0;
	for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
	{
		pHistoEvol = pVariablesIterator->Current();
		if ((pHistoEvol->HistoRef() == histoIdx) &&
			(pHistoEvol->Cmpt()[0] == cmpt) &&
			(pHistoEvol->RrdIndex() == rrd))
		{
			resa = pHistoEvol->ResHoldGrpTot() + pHistoEvol->ResHoldIdvTot();
			delete pVariablesIterator;
			return resa;
		}
	}
	delete pVariablesIterator;
	return resa;
}

bool CHistoData::GetHoiTra (int histoIdx, char cmpt, long rrd)
{
	if (cmpt > 'Z')
		cmpt -=32; // mise en majuscule
	YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
		YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_CMPT_ALT_KEY : HISTOEVOL_CMPT_KEY)->CreateIterator();
	YmIcHistoEvolDom* pHistoEvol = NULL;
	bool bHoiTra = false;
	for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
	{
		pHistoEvol = pVariablesIterator->Current();
		if ((pHistoEvol->HistoRef() == histoIdx) &&
			(pHistoEvol->Cmpt()[0] == cmpt) &&
			(pHistoEvol->RrdIndex() == rrd))
		{
			bHoiTra = !strcmp (pHistoEvol->HoiTra(), "I");
			delete pVariablesIterator;
			return bHoiTra;
		}
	}
	delete pVariablesIterator;
	return bHoiTra;
}



CString CHistoData::GetTitle (int histoIdx)
{
	CString szTitle;
	YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
		YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_CMPT_ALT_KEY : HISTOEVOL_CMPT_KEY)->CreateIterator();
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

RWDate CHistoData::GetDateHisto (int histoIdx)
{
	RWDate DateHisto;
	DateHisto.julian(0);
	YM_Iterator<YmIcHistoEvolDom>* pVariablesIterator =
		YM_Set<YmIcHistoEvolDom>::FromKey(bSci ? HISTOEVOL_CMPT_ALT_KEY : HISTOEVOL_CMPT_KEY)->CreateIterator();
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

int CHistoData::GetNbBucket(int histoIdx, char cmpt)
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


CDataGraph::CDataGraph()
{
	pCmptEvol = NULL;
	pBktEvol = NULL;
	pCmptPrevision = NULL;
	AuthHist1 = INVALID_GRAPH_VALUE;
	ResaHist1 = INVALID_GRAPH_VALUE;
	AuthHist2 = INVALID_GRAPH_VALUE;
	ResaHist2 = INVALID_GRAPH_VALUE;
	AuthHist3 = INVALID_GRAPH_VALUE;
	ResaHist3 = INVALID_GRAPH_VALUE;
	HoiTra1 = false;
	HoiTra2 = false;
	HoiTra3 = false;
	rrd_index = -1;
	ResaTempsReel = INVALID_GRAPH_VALUE;
	Prevision = -9999;
	bJxHisto = false;
}

void CDataGraph::AddPrevision (double prevoi)
{
	if (Prevision == -9999)
		Prevision = prevoi;
	else
		Prevision += prevoi;
}

double CDataGraph::GetPrevision()
{
	if (Prevision != -9999)
		return Prevision;
	/*if (pCmptEvol)
	return pCmptEvol->ResHoldGrp() + pCmptEvol->ResHoldIdv();*/
	return INVALID_GRAPH_VALUE;
}

/////////////////////////////////////////////////////////////////////////////
// CRRDHistAltView

IMPLEMENT_DYNCREATE(CRRDHistAltView, CRRDHistView)

CRRDHistAltView::CRRDHistAltView()
:CRRDHistView(TRUE)
{
}

void CRRDHistAltView::OnInitialUpdate() 
{
	((CChildFrame*)GetParentFrame())->SetAltFlag(TRUE);

	CRRDHistView::OnInitialUpdate();  // base class method
}

/////////////////////////////////////////////////////////////////////////////
// CRRDHistView

IMPLEMENT_DYNCREATE(CRRDHistView, CView)

CRRDHistView::CRRDHistView(BOOL bAlt /*=FALSE*/)
{
	if (!((CCTAApp*)APP)->GetRhealysFlag())
		m_bCmptPrevisionVenteDataValid = 0;
	else
		m_bCmptPrevisionVenteDataValid = 1;
	_bInitialized = FALSE;
	_bHasPaneDate = FALSE;
	_nSubViewID = 0;
	this->nature = -1;//DM8029 - add nature field, to display data (or do not) according to it

	// set this view as the owner view in the chart object
	m_CmptChart.SetParentView(this);  
	m_BktChart.SetParentView(this);  
	m_CmptChart.SetCurScaling(((CCTAApp*)APP)->m_ScaleRRD);
	m_BktChart.SetCurScaling(((CCTAApp*)APP)->m_ScaleRRD);

	m_bCmptDataValid = FALSE;  // compartment data is invalid
	m_bCmptAvgDataValid = FALSE; // average compartment data is invalid
	m_bBktDataValid  = FALSE;  // bucket data is invalid
	m_bBktAvgDataValid  = FALSE;  // bucket data is invalid
	m_bComptagesDataValid  = FALSE;  // comptages data is invalid
	m_bComptagesHistDataValid  = FALSE;  // historical comptages data is invalid
	m_bHistoDataValid  = FALSE;  // historical data is invalid
	m_bPrevisionValid  = FALSE;  // prevision data is invalid
	m_bTabResized = FALSE;

	// DM7978 - LME - YI-5050
	// oha : ano 78122
	// m_bCmptAlreadyBuild = FALSE;

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
		//FW_BOLD,
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
	eCmptCapacity = iOffset++;
	eCmptOvbLevelCur = iOffset++;
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
		eCmptHisto3OvbLevel = iOffset++;
		eCmptHisto3ResHoldTot = iOffset++;
		eCmptHisto2OvbLevel = iOffset++;
		eCmptHisto2ResHoldTot = iOffset++;
		eCmptHisto1OvbLevel = iOffset++;
		eCmptHisto1ResHoldTot = iOffset++;
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{
			eCmptHisto3HoiTra = iOffset++;
			eCmptHisto2HoiTra = iOffset++;
			eCmptHisto1HoiTra = iOffset++;
		}
	}

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !bAlt )  // Is this a Comptages client ?
	{
		eCmptComptageOfferStdvHigh = iOffset++;
		eCmptComptageOffer = iOffset++;
		eCmptComptageOfferStdvLow = iOffset++;
		eCmptComptageOccupStdvHigh = iOffset++;
		eCmptComptageOccup = iOffset++;
		eCmptComptageOccupStdvLow = iOffset++;
	}

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !bAlt)  // Is this a Comptages client ?
	{
		eCmptComptageHist3Offer = iOffset++;
		eCmptComptageHist3Occup = iOffset++;
		eCmptComptageHist2Offer = iOffset++;
		eCmptComptageHist2Occup = iOffset++;
		eCmptComptageHist1Offer = iOffset++;
		eCmptComptageHist1Occup = iOffset++;
	}

	//if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
	if (!((CCTAApp*)APP)->GetRhealysFlag() && !bAlt)
		eCmptPrevision = iOffset++;



	eCmptNumDataSets = iOffset++;  

	iOffset = 0;

	eBktAuthCur = iOffset++;
	eBktResHoldAvailSum = iOffset++;
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
		eBktHisto3AuthCur = iOffset++;
		eBktHisto3ResHoldTot = iOffset++;
		eBktHisto2AuthCur = iOffset++;
		eBktHisto2ResHoldTot = iOffset++;
		eBktHisto1AuthCur = iOffset++;
		eBktHisto1ResHoldTot = iOffset++;
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{
			eBktHisto3HoiTra = iOffset++;
			eBktHisto2HoiTra = iOffset++;
			eBktHisto1HoiTra = iOffset++;
		}
	}

	//if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
	//	eBktPrevision = iOffset++;

	eBktNumDataSets = iOffset++;


	m_bRestored = FALSE;
	mp_HistoData = NULL;

	m_histocomptages_date1 = -1;
	m_histocomptages_date2 = -1;
	m_histocomptages_date3 = -1;

	SetBoolSet (((CCTAApp*)APP)->m_RRDBoolSet);
}

CRRDHistView::~CRRDHistView()
{
	m_CmptChart.ClearChartDataArray();  // delete all graph datasets
	m_BktChart.ClearChartDataArray();  // delete all graph datasets
	if (m_pPaneFont)
		delete m_pPaneFont;
	if (mp_HistoData)
		delete mp_HistoData;
	EmptyInvalidHistoDates();
}

BEGIN_MESSAGE_MAP(CRRDHistView, CView)
	//{{AFX_MSG_MAP(CRRDHistView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(TCN_SELCHANGE, IDC_RRDHIST_TABCTRL, OnChartSelchange)
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
	ON_COMMAND(ID_PRINT, OnPrint)
	ON_UPDATE_COMMAND_UI(ID_PRINT, OnUpdateGen)
	ON_MESSAGE(WM_REFRESH_ALL, OnRefreshAll)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// Get the CTA data object from the tab item
//
CString CRRDHistView::GetTextFromTabItem(int iTabIdx)
{
	char szTabText[70]; // au lieu de 50
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
CCTAChartData* CRRDHistView::GetDataFromTabItem()
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
// CRRDHistView drawing

void CRRDHistView::OnDraw(CDC* pDC)
{
	ASSERT_VALID(GetDocument());

	YM_RecordSet* pCmptEvolSet = 
		YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_ALT_KEY : CMPTEVOL_KEY);
	YM_RecordSet* pCmptEvolAvgSet = 
		YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_ALT_KEY : CMPTEVOL_AVG_KEY);
	YM_RecordSet* pBktEvolSet = 
		YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_ALT_KEY : BKTEVOL_KEY);
	YM_RecordSet* pBktEvolAvgSet = 
		YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_ALT_KEY : BKTEVOL_AVG_KEY);

	YM_RecordSet* pComptagesSet = NULL;
	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetAltFlag())
		pComptagesSet = YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_KEY);

	YM_RecordSet* pComptagesHistSet = NULL;
	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag())
		pComptagesHistSet = YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_HIST_KEY);

	YM_RecordSet* pHistoEvolSet = NULL;
	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
		pHistoEvolSet = YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_ALT_KEY : HISTOEVOL_KEY);

	YM_RecordSet* pHistoEvolCmptSet = NULL;
	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
		pHistoEvolCmptSet = YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_CMPT_ALT_KEY : HISTOEVOL_CMPT_KEY);


	// DM7978 - LME - YI5050 - correction du bug de la taille de police reduite suite a reduction taille de fenetre
	const char cmpt = m_szCurTabText.GetAt(0);
	if(cmpt >= 'A' && cmpt <= 'Z')
	{
		// uniquement sur les cmpt
		if(IsCmptTabSelected())
		{
			m_pCurSelChart->SetHeaderHeight(16);
			m_pCurSelChart->SetHeaderWidth(16);
		}
	}
	// DM7978 - LME - FIN

	// if record set is valid and we have data 
	if( 
		pCmptEvolSet->IsValid() &&
		pCmptEvolAvgSet->IsValid() && 
		pBktEvolSet->IsValid() && 
		pBktEvolAvgSet->IsValid() &&
		m_pCurSelChart->GetChartDataArray()->GetSize() &&
		( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() || GetAltFlag() || pComptagesSet->IsValid() ) &&
		( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() || (pHistoEvolSet->IsValid() &&  pHistoEvolCmptSet->IsValid())) &&
		( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() || GetAltFlag() || pComptagesHistSet->IsValid() )
		)
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

void CRRDHistView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	COLORREF clrBk = (GetAltFlag()) ? 
		((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt() : 
	((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();

	// en cas de split, forcer systematiqument la rotation
	DoGraphRotateLabels(((CCTAApp*)APP)->m_bRRDRotatelabel);

	// set the area under the curve the same as the graph's background color
	m_CmptChart.SetDataStyleColor(CRRDHistChart::eResHoldStdvLowStyleIdx, clrBk);
	m_CmptChart.SetDataStyleColor(CRRDHistChart::eRevAvgFcstLowStyleIdx, clrBk);
	m_BktChart.SetDataStyleColor(CRRDHistChart::eResHoldStdvLowStyleIdx, clrBk);
	m_BktChart.SetDataStyleColor(CRRDHistChart::eRevAvgFcstLowStyleIdx, clrBk);
	m_CmptChart.SetGraphProperties(CCTAChartData::eCmpt);
	m_BktChart.SetGraphProperties(CCTAChartData::eBkt);

	m_CmptChart.SetBackgroundColor(clrBk);
	m_BktChart.SetBackgroundColor(clrBk);

	// DM7978 - LME - YI5050 - correction du bug de la taille de police reduite suite a reduction taille de fenetre
	const char cmpt = m_szCurTabText.GetAt(0);
	if(cmpt >= 'A' && cmpt <= 'Z')
	{
		// uniquement sur les cmpt
		if(IsCmptTabSelected())
		{
			m_pCurSelChart->SetHeaderHeight(16);
			m_pCurSelChart->SetHeaderWidth(16);
		}
	}
	// DM7978 - LME - FIN

	// Do not call CView::OnPaint() for painting messages
}

/////////////////////////////////////////////////////////////////////////////
// CRRDHistView diagnostics

#ifdef _DEBUG
void CRRDHistView::AssertValid() const
{
	CView::AssertValid();
}

void CRRDHistView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCTADoc* CRRDHistView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCTADoc)));
	return (CCTADoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRRDHistView message handlers

int CRRDHistView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create and adjust the tab control
	CRect rect;
	GetClientRect(&rect);
	m_ChartTabCtrl.Create( CTA_TAB_STYLE, rect, this, IDC_RRDHIST_TABCTRL);
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

CComptagesView* CRRDHistView::GetComptagesView()
{
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	CComptagesView* pComptagesView = NULL;
	while( pos != NULL ) 
	{
		CView* pView = pDoc->GetNextView( pos ); 
		if( pView->IsKindOf( RUNTIME_CLASS(CComptagesView) ) ) 
		{
			if ( pView->GetSafeHwnd() )
				pComptagesView = (CComptagesView*)pView;
			break;
		}
	}
	return pComptagesView;
}

void CRRDHistView::FrameSizeChange (UINT nType, int cx, int cy)
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
		YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_ALT_KEY : BKTEVOL_KEY);
	ASSERT( pBktEvolSet != NULL );

	if ((nType == SIZE_MINIMIZED) && pBktEvolSet->IsOpen())
		pBktEvolSet->Close(); 
	else  if ((nType != SIZE_MINIMIZED) && !pBktEvolSet->IsOpen())
		pBktEvolSet->Open();

	YM_RecordSet* pBktEvolAvgSet = 
		YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_ALT_KEY : BKTEVOL_AVG_KEY);
	ASSERT( pBktEvolAvgSet != NULL );

	if ((nType == SIZE_MINIMIZED) && pBktEvolAvgSet->IsOpen())
		pBktEvolAvgSet->Close(); 
	else  if ((nType != SIZE_MINIMIZED) && !pBktEvolAvgSet->IsOpen())
		pBktEvolAvgSet->Open();

	YM_RecordSet* pCmptEvolSet = 
		YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_ALT_KEY : CMPTEVOL_KEY);
	ASSERT( pCmptEvolSet != NULL );

	if ((nType == SIZE_MINIMIZED) && pCmptEvolSet->IsOpen())
		pCmptEvolSet->Close();
	else if ((nType != SIZE_MINIMIZED) && !pCmptEvolSet->IsOpen())
		pCmptEvolSet->Open();

	YM_RecordSet* pCmptEvolAvgSet = 
		YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_ALT_KEY : CMPTEVOL_AVG_KEY);
	ASSERT( pCmptEvolAvgSet != NULL );

	if ((nType == SIZE_MINIMIZED) && pCmptEvolAvgSet->IsOpen())
		pCmptEvolAvgSet->Close();
	else if ((nType != SIZE_MINIMIZED) && !pCmptEvolAvgSet->IsOpen())
		pCmptEvolAvgSet->Open();

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetAltFlag())
	{
		YM_RecordSet* pComptagesSet = YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_KEY);
		ASSERT( pComptagesSet != NULL );

		if ( (nType == SIZE_MINIMIZED) && pComptagesSet->IsOpen() && !GetComptagesView() )
			pComptagesSet->Close();
		else if ((nType != SIZE_MINIMIZED) && !pComptagesSet->IsOpen())
			pComptagesSet->Open();
	}
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	{
		YM_RecordSet* pHistoEvolSet =
			YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_ALT_KEY : HISTOEVOL_KEY);
		ASSERT( pHistoEvolSet != NULL );

		if ( (nType == SIZE_MINIMIZED) && pHistoEvolSet->IsOpen() )
			pHistoEvolSet->Close();
		else if ((nType != SIZE_MINIMIZED) && !pHistoEvolSet->IsOpen())
			pHistoEvolSet->Open();

		YM_RecordSet* pHistoEvolCmptSet =
			YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_CMPT_ALT_KEY : HISTOEVOL_CMPT_KEY);
		ASSERT( pHistoEvolCmptSet != NULL );

		if ( (nType == SIZE_MINIMIZED) && pHistoEvolCmptSet->IsOpen() )
			pHistoEvolCmptSet->Close();
		else if ((nType != SIZE_MINIMIZED) && !pHistoEvolCmptSet->IsOpen())
			pHistoEvolCmptSet->Open();
	}
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag())
	{
		YM_RecordSet* pComptagesHistSet = YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_HIST_KEY);
		ASSERT( pComptagesHistSet != NULL );

		if ( (nType == SIZE_MINIMIZED) && pComptagesHistSet->IsOpen() && !GetComptagesView() )
			pComptagesHistSet->Close();
		else if ((nType != SIZE_MINIMIZED) && !pComptagesHistSet->IsOpen())
			pComptagesHistSet->Open();
	}
}

void CRRDHistView::OnShowWindow(BOOL bShow, UINT nStatus)
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

void CRRDHistView::GetDayTitle(CString& szDayTitle)
{
	CString szTitle = ((CCTAApp*)APP)->GetResource()->LoadResString((GetAltFlag()) ? IDS_RRDHIST_SCI_TITLE : IDS_RRDHIST_TITLE);
	YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
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

void CRRDHistView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	if (getOtherView())
	{
		_nSubViewID = 1;
	}

	ASSERT_VALID(GetDocument());

	// for the splitter window, when a new view is created, do not go through this bit again...
	//  if ( ((CSplitChildFrame*)GetParentFrame())->GetInitialUpdateFlag() == FALSE )
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
		//    ((CSplitChildFrame*)GetParentFrame())->SetInitialUpdateFlag( TRUE );
	}

	YM_RecordSet* pCmptEvolSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_ALT_KEY : CMPTEVOL_KEY);
	ASSERT( pCmptEvolSet != NULL );
	YM_RecordSet* pCmptEvolAvgSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_ALT_KEY : CMPTEVOL_AVG_KEY);
	ASSERT( pCmptEvolAvgSet != NULL );
	YM_RecordSet* pBktEvolSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_ALT_KEY : BKTEVOL_KEY);
	ASSERT( pBktEvolSet  != NULL );
	YM_RecordSet* pBktEvolAvgSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_ALT_KEY : BKTEVOL_AVG_KEY);
	ASSERT( pBktEvolAvgSet  != NULL );
	YM_RecordSet* pCmptPrevisionVenteSet = NULL;
	if (!((CCTAApp*)APP)->GetRhealysFlag())
		pCmptPrevisionVenteSet = YM_Set<YmIcPrevisionDom>::FromKey(PREVISION_VENTE_KEY);

	// attempt to open the record sets
	if( ! pCmptEvolSet->IsOpen() )
		pCmptEvolSet->Open();

	if( ! pCmptEvolAvgSet->IsOpen() )
		pCmptEvolAvgSet->Open();

	if( ! pBktEvolSet->IsOpen() )
		pBktEvolSet->Open();

	if( ! pBktEvolAvgSet->IsOpen() )
		pBktEvolAvgSet->Open();

	if (!((CCTAApp*)APP)->GetRhealysFlag() && pCmptPrevisionVenteSet != NULL && !pCmptPrevisionVenteSet->IsOpen())
		pCmptPrevisionVenteSet->Open();


	// ask for notification from the RecordSets
	pCmptEvolSet->AddObserver(this);
	pCmptEvolAvgSet->AddObserver(this);
	pBktEvolSet->AddObserver(this);
	pBktEvolAvgSet->AddObserver(this);
	if (!((CCTAApp*)APP)->GetRhealysFlag())
		pCmptPrevisionVenteSet->AddObserver(this);

	OnUpdate(this, 0L, pCmptEvolSet);
	OnUpdate(this, 0L, pCmptEvolAvgSet);
	OnUpdate(this, 0L, pBktEvolSet);
	OnUpdate(this, 0L, pBktEvolAvgSet);
	if (!((CCTAApp*)APP)->GetRhealysFlag())
		OnUpdate(this, 0L, pCmptPrevisionVenteSet);

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetAltFlag() )
	{
		YM_RecordSet* pComptagesSet = YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_KEY);
		ASSERT( pComptagesSet != NULL );

		if( ! pComptagesSet->IsOpen() )
			pComptagesSet->Open();
		pComptagesSet->AddObserver( this );
		OnUpdate( this, 0L, pComptagesSet );
	}

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
			YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_ALT_KEY : HISTOEVOL_KEY);
		ASSERT( pHistoEvolSet != NULL );

		if( ! pHistoEvolSet->IsOpen() )
			pHistoEvolSet->Open();
		pHistoEvolSet->AddObserver( this );
		OnUpdate( this, 0L, pHistoEvolSet );

		YM_RecordSet* pHistoEvolCmptSet = 
			YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_CMPT_ALT_KEY : HISTOEVOL_CMPT_KEY);
		ASSERT( pHistoEvolCmptSet != NULL );

		if( ! pHistoEvolCmptSet->IsOpen() )
			pHistoEvolCmptSet->Open();
		pHistoEvolCmptSet->AddObserver( this );
		OnUpdate( this, 0L, pHistoEvolCmptSet );

		enablePaneDate();
	}

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag())
	{
		YM_RecordSet* pComptagesHistSet = 
			YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_HIST_KEY);
		ASSERT( pComptagesHistSet != NULL );

		if( ! pComptagesHistSet->IsOpen() )
			pComptagesHistSet->Open();
		pComptagesHistSet->AddObserver( this );
		OnUpdate( this, 0L, pComptagesHistSet );
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
			m_bDispHisto1AuthCur = FALSE;
			m_bDispHisto1ResHoldTot = FALSE;
			m_bDispHisto2AuthCur = FALSE;
			m_bDispHisto2ResHoldTot = FALSE;
			m_bDispHisto3AuthCur = FALSE;
			m_bDispHisto3ResHoldTot = FALSE;
			m_bDispHisto1ComptageOffer = FALSE;
			m_bDispHisto1ComptageOccup = FALSE;
			m_bDispHisto2ComptageOffer = FALSE;
			m_bDispHisto2ComptageOccup = FALSE;
			m_bDispHisto3ComptageOffer = FALSE;
			m_bDispHisto3ComptageOccup = FALSE;
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

BOOL CRRDHistView::DrawSBItem( YM_StatusBar* sb, LPDRAWITEMSTRUCT lpDrawItemStruct, LPVOID pData )
{
	CRRDHistView* This = (CRRDHistView*)pData;
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

void CRRDHistView::enablePaneDate( BOOL bEnable /*=TRUE*/ )
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

void CRRDHistView::setPaneDateText( int iPane, LPCTSTR pszText )
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


void CRRDHistView::OnChartSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// set the member variable which holds the currently selected tab...
	m_iCurTabSelection = m_ChartTabCtrl.GetCurSel();  // get currently selected tab
	m_szCurTabText = GetTextFromTabItem(m_iCurTabSelection);
	_sLastTabUsed = m_szCurTabText.Left(1);	// Only keep cmpt name 

	// DM7978 - LME - ajout d'un enumere pour savoir sur quel type d'onglet on a clique
	if(m_szCurTabText.GetLength() == 1) //le label de l'onglet ne comporte qu'un caractere
	{
		m_eTabSelected = eCmptTabSelected;
	}
	else
	{
		m_eTabSelected = eBktTabSelected;
	}

	CCTAChartData* pCTAChartData = GetDataFromTabItem();
	if ( pCTAChartData == NULL )
		return;

	// which chart is being selected (cmpt or bkt) ?
	if ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt )
	{
		m_pCurSelChart = &m_CmptChart;
		m_pNotSelChart = &m_BktChart;
	}
	else
	{
		m_pCurSelChart = &m_BktChart;
		m_pNotSelChart = &m_CmptChart;
	}

	// set the appropriate data object as currently selected, then get it
	m_pCurSelChart->SetCurChartData( pCTAChartData );
	DrawGraph();  // draw the graph

	UpdateHelpDialog();

	m_pCurSelChart->RemoveFlashLabel();  // remove flash label if up

	*pResult = 0;
}

void CRRDHistView::BuildComptagesArray()
{
	m_ComptagesArray.RemoveAll();
	YM_Iterator<YmIcComptagesDom>* pComptagesIterator = 
		YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_KEY)->CreateIterator();
	if (pComptagesIterator->GetCount() > 0)
		for( pComptagesIterator->First(); !pComptagesIterator->Finished(); pComptagesIterator->Next() )
			m_ComptagesArray.Add(pComptagesIterator->Current());
	delete pComptagesIterator;
}

void CRRDHistView::BuildComptagesHistArray()
{
	m_histocomptages_date1 = -1; 
	m_histocomptages_date2 = -1;
	m_histocomptages_date3 = -1;
	m_histocomptages_comment1 = "";
	m_histocomptages_comment2 = "";
	m_histocomptages_comment3 = "";
	int lastref = 0;
	m_ComptagesHistArray.RemoveAll();
	YM_Iterator<YmIcComptagesDom>* pComptagesIterator = 
		YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_HIST_KEY)->CreateIterator();
	YmIcComptagesDom *pRecord;
	int dada;
	if (pComptagesIterator->GetCount() > 0)
		for( pComptagesIterator->First(); !pComptagesIterator->Finished(); pComptagesIterator->Next() )
		{
			pRecord = pComptagesIterator->Current();
			while (pRecord->HistoRef() > lastref + 1)
			{ // Il manque une date historique
				m_ComptagesHistArray.Add (NULL);
				lastref++;
			}
			m_ComptagesHistArray.Add(pRecord);
			dada = pRecord->DptDate();

			switch (pRecord->HistoRef())
			{
			case 1 : 
				{
					m_histocomptages_date1 = dada;
					m_histocomptages_comment1 = pRecord->Commentaire();
					if (m_histocomptages_comment1.IsEmpty())
					{ 
						RWDate rwdada (dada);
						m_histocomptages_comment1 = rwdada.asString("%d/%m/%Y");
					}
					break;
				}
			case 2 : 
				{
					m_histocomptages_date2 = dada;
					m_histocomptages_comment2 = pRecord->Commentaire();
					if (m_histocomptages_comment2.IsEmpty())
					{ 
						RWDate rwdada (dada);
						m_histocomptages_comment2 = rwdada.asString("%d/%m/%Y");
					}
					break;
				}
			case 3 :
				{
					m_histocomptages_date3 = dada;
					m_histocomptages_comment3 = pRecord->Commentaire();
					if (m_histocomptages_comment3.IsEmpty())
					{ 
						RWDate rwdada (dada);
						m_histocomptages_comment3 = rwdada.asString("%d/%m/%Y");
					}
				}
			}
		}
		delete pComptagesIterator;
} 

long CRRDHistView::OnRefreshAll (UINT wParam, LONG lParam )
{
	RefreshAll();
	return 0;
}

void CRRDHistView::RefreshAll(BOOL both /* TRUE*/)
{
	BuildCmptGraphs();
	BuildBktGraphs();
	if ( m_bCmptDataValid && m_bCmptAvgDataValid && m_bBktDataValid && m_bBktAvgDataValid &&
		( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() || GetAltFlag() || m_bComptagesDataValid ) &&
		( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() || m_bHistoDataValid ) &&
		( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() || GetAltFlag() || m_bComptagesHistDataValid ) &&
		( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() || m_bPrevisionValid) )
	{
		BuildTabs();
		ResizeChart();
		Invalidate(FALSE);
		UpdateWindow();
	}

	if (both)
	{
		CRRDHistView* pView = getOtherView();
		if (pView)
			pView->RefreshAll (FALSE);
	}  
}

void CRRDHistView::ChangeRRDDisplay(BOOL bOnF5)
{
	if (bOnF5)
	{ 
		CString sb = SDISPLAYALL;
		switch (((CCTAApp*)APP)->m_RRDdisplayMode)
		{
		case RRDDISPLAYMEC:
			{
				sb = SDISPLAYMEC;
				break;
			}
		case RRDDISPLAYECH:
			{
				sb = SDISPLAYECH;
				break;
			}
		case RRDDISPLAYHIS:
			{
				sb = SDISPLAYHIS;
				break;
			}
		}
		SetBoolSet (sb);
	}
	RefreshAll (FALSE);

	CString sbb = GetBoolSet();
	((CCTAApp*)APP)->m_RRDBoolSet = sbb;

	// Même chose sur l'autre, si elle existe
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	CView* pView; 
	POSITION pos = pDoc->GetFirstViewPosition();
	while( pos != NULL ) 
	{
		pView = pDoc->GetNextView( pos ); 
		// do we have a spliter window activated (i.e., 2 views ?)
		if( pView->IsKindOf( RUNTIME_CLASS(CRRDHistView) ) && pView != this &&
			((CRRDHistView*)pView)->GetAltFlag() == GetAltFlag()) 
		{
			CString sbbis = ((CRRDHistView*)pView)->GetBoolSet();
			if (sbbis != sbb)
			{
				((CRRDHistView*)pView)->SetBoolSet(sbb);
				((CRRDHistView*)pView)->ChangeRRDDisplay(FALSE);
			}
		}
	}
}

void CRRDHistView::RevMasqueCourbe (int graphtype, int set, BOOL b2ndSet)
{
	if (b2ndSet)
	{
		m_bDispEC = !m_bDispEC;
		return;
	}
	if (graphtype == CCTAChartData::eBkt)
	{
		if (set == eBktAuthCur)
			m_bDispAuthCur = !m_bDispAuthCur;
		if (set == eBktResHoldAvailSum)
			m_bDispResHoldAvailSum = !m_bDispResHoldAvailSum;
		if (set == eBktResHoldTot)
			m_bDispResHoldTot = !m_bDispResHoldTot;
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{
			if (set == eBktHoiTra)
				m_bDispResHoldTot = !m_bDispResHoldTot;
			if (set == eBktHoiTraIdv)
				m_bDispResHoldIdv = !m_bDispResHoldIdv;
		}
		if (set == eBktResHoldIdv)
			m_bDispResHoldIdv = !m_bDispResHoldIdv;
		if (set == eBktResHoldAvg)
			m_bDispResHoldAvg = !m_bDispResHoldAvg;
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
		{
			if (set == eBktHisto3AuthCur)
				m_bDispHisto3AuthCur = !m_bDispHisto3AuthCur;
			if (set == eBktHisto3ResHoldTot)
				m_bDispHisto3ResHoldTot = !m_bDispHisto3ResHoldTot;
			if (set == eBktHisto2AuthCur)
				m_bDispHisto2AuthCur = !m_bDispHisto2AuthCur;
			if (set == eBktHisto2ResHoldTot)
				m_bDispHisto2ResHoldTot = !m_bDispHisto2ResHoldTot;
			if (set == eBktHisto1AuthCur)
				m_bDispHisto1AuthCur = !m_bDispHisto1AuthCur;
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
		//if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
		if (!((CCTAApp*)APP)->GetRhealysFlag() && !this->GetAltFlag())
		{
			if (set == eBktPrevision)
				m_bDispPrevision = !m_bDispPrevision;
		}
	}
	else if (graphtype == CCTAChartData::eCmpt)
	{
		if (set == eCmptCapacity) 
			m_bDispCapa = !m_bDispCapa;
		if (set == eCmptOvbLevelCur)
			m_bDispAuthCur = !m_bDispAuthCur;
		if (set == eCmptResHoldTot)
			m_bDispResHoldTot = !m_bDispResHoldTot;
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{
			if (set == eCmptHoiTra)
				m_bDispResHoldTot = !m_bDispResHoldTot;
			if (set == eCmptHoiTraIdv)
				m_bDispResHoldIdv = !m_bDispResHoldIdv;
		}
		if (set == eCmptResHoldIdv)
			m_bDispResHoldIdv = !m_bDispResHoldIdv;
		if (set == eCmptResHoldAvg)
			m_bDispResHoldAvg = !m_bDispResHoldAvg;
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
		{
			if (set == eCmptHisto1OvbLevel)
				m_bDispHisto1AuthCur = !m_bDispHisto1AuthCur;
			if (set == eCmptHisto1ResHoldTot)
				m_bDispHisto1ResHoldTot = !m_bDispHisto1ResHoldTot;
			if (set == eCmptHisto2OvbLevel)
				m_bDispHisto2AuthCur = !m_bDispHisto2AuthCur;
			if (set == eCmptHisto2ResHoldTot)
				m_bDispHisto2ResHoldTot = !m_bDispHisto2ResHoldTot;
			if (set == eCmptHisto3OvbLevel)
				m_bDispHisto3AuthCur = !m_bDispHisto3AuthCur;
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
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetAltFlag())  // Is this a Comptages client ?
		{
			if ((set == eCmptComptageOfferStdvHigh) || (set == eCmptComptageOfferStdvLow) )
				m_bDispComptageOfferStvd = !m_bDispComptageOfferStvd;
			if (set == eCmptComptageOffer)
				m_bDispComptageOffer = !m_bDispComptageOffer;
			if ((set == eCmptComptageOccupStdvHigh) || (set == eCmptComptageOccupStdvLow) )
				m_bDispComptageOccupStvd = !m_bDispComptageOccupStvd;
			if (set == eCmptComptageOccup)
				m_bDispComptageOccup = !m_bDispComptageOccup;
		}
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag())  // Is this a Comptages client ?
		{
			if (set == eCmptComptageHist1Offer)
				m_bDispHisto1ComptageOffer = !m_bDispHisto1ComptageOffer;
			if (set == eCmptComptageHist1Occup)
				m_bDispHisto1ComptageOccup = !m_bDispHisto1ComptageOccup;
			if (set == eCmptComptageHist2Offer)
				m_bDispHisto2ComptageOffer = !m_bDispHisto2ComptageOffer;
			if (set == eCmptComptageHist2Occup)
				m_bDispHisto2ComptageOccup = !m_bDispHisto2ComptageOccup;
			if (set == eCmptComptageHist3Offer)
				m_bDispHisto3ComptageOffer = !m_bDispHisto3ComptageOffer;
			if (set == eCmptComptageHist3Occup)
				m_bDispHisto3ComptageOccup = !m_bDispHisto3ComptageOccup;
		}
		//if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
		//{
		if (!((CCTAApp*)APP)->GetRhealysFlag() && !this->GetAltFlag())
			if (set == eCmptPrevision)
				m_bDispPrevision = !m_bDispPrevision;
		//}
	}
}

void CRRDHistView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	
	CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
			
	// NBN - Ano 92249
	if(!(!((CCTAApp*)APP)->GetRhealysFlag() || (!GetAltFlag() || pTrancheView->getCurrentTrancheNature() == 3))){
		ClearBktGraphs();
		ClearCmptGraphs();
			((CChildFrame*)GetParentFrame())->GetStatusBar()->SetPaneOneAndLock(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA));

		return;
	}
	
	ASSERT_VALID(GetDocument());
	YM_RecordSet* pCmptEvolSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_ALT_KEY : CMPTEVOL_KEY);
	YM_RecordSet* pCmptEvolAvgSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_ALT_KEY : CMPTEVOL_AVG_KEY);
	YM_RecordSet* pBktEvolSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_ALT_KEY : BKTEVOL_KEY);
	YM_RecordSet* pBktEvolAvgSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_ALT_KEY : BKTEVOL_AVG_KEY);
	YM_RecordSet* pCmptPrevisionVenteSet = NULL;
	if (!((CCTAApp*)APP)->GetRhealysFlag())
		pCmptPrevisionVenteSet = YM_Set<YmIcPrevisionDom>::FromKey(PREVISION_VENTE_KEY);
	YM_RecordSet* pComptagesSet = NULL;

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetAltFlag())
		pComptagesSet = YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_KEY);
	YM_RecordSet* pHistoEvolSet = NULL;
	YM_RecordSet* pHistoEvolCmptSet = NULL;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	{
		pHistoEvolSet = YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_ALT_KEY : HISTOEVOL_KEY);
		pHistoEvolCmptSet = YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_CMPT_ALT_KEY : HISTOEVOL_CMPT_KEY);
	}
	YM_RecordSet* pComptagesHistSet = NULL;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag())
		pComptagesHistSet = YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_HIST_KEY);
	/*YM_RecordSet* pPrevisionSet = NULL;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
		pPrevisionSet = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY);*/

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
	else if (!((CCTAApp*)APP)->GetRhealysFlag() && pHint != NULL && pHint == pCmptPrevisionVenteSet )  // bucket data set
	{
		//m_BktChart.DestroyHelpDlg();
		m_bCmptPrevisionVenteDataValid = pCmptPrevisionVenteSet->IsValid();
		if (pCmptPrevisionVenteSet->IsValid())
			BuildCmptGraphs();
		//EnableWindow( m_bCmptPrevisionVenteDataValid );
	}
	else if ( pHint != NULL && pHint == pComptagesSet )  // comptages data set
	{
		m_bComptagesDataValid = pComptagesSet->IsValid();
		if (m_bComptagesDataValid)
		{
			BuildComptagesArray();
			BuildCmptGraphs();
		}
	}
	else if ( pHint != NULL && pHint == pComptagesHistSet )  // comptages data set
	{
		m_bComptagesHistDataValid = pComptagesHistSet->IsValid();
		if (m_bComptagesHistDataValid)
		{
			BuildComptagesHistArray();
			BuildCmptGraphs();
		}
	}
	else if ( pHint != NULL && ((pHint == pHistoEvolSet) || (pHint == pHistoEvolCmptSet)))  // historiques data set
	{
		m_bHistoDataValid = (pHistoEvolSet->IsValid() && pHistoEvolCmptSet->IsValid());
		if (m_bHistoDataValid)
		{
			if (mp_HistoData)
				delete mp_HistoData;
			mp_HistoData = new CHistoData (GetAltFlag());	  
			
			BuildCmptGraphs();
			BuildBktGraphs();

		}
		// BuildCmptGraphs et BuildBktGraphs contienent un test pour attendre
		// la totalité de donnés (CMPT ou BKT) et historique
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
	if ( m_bCmptDataValid && m_bCmptAvgDataValid && m_bBktDataValid && m_bBktAvgDataValid &&
		( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() || GetAltFlag() || m_bComptagesDataValid ) &&
		( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() || m_bHistoDataValid ) &&
		( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() || GetAltFlag() || m_bComptagesHistDataValid ) &&
		( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() || m_bPrevisionValid))
	{
		BuildTabs();
		ResizeChart();
	}
	YM_StatusBar* sb = ((CChildFrame*)GetParentFrame())->GetStatusBar();
	if (sb)
		sb->Invalidate();
}

void CRRDHistView::OnSize(UINT nType, int cx, int cy) 
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

void CRRDHistView::OnScale (enum CCTAChart::GraphScaleIndex eGraphScale)
{
	m_CmptChart.ScaleGraph(eGraphScale);
	m_BktChart.ScaleGraph(eGraphScale);
	((CCTAApp*)APP)->m_ScaleRRD = eGraphScale;
	Invalidate(FALSE);  // force a redraw
	UpdateWindow();
} 

void CRRDHistView::OnScaleMax() 
{
	OnScale(CCTAChart::eMaxScaleIdx);
	CRRDHistView* pView = getOtherView();
	if (pView)
		pView->OnScale (CCTAChart::eMaxScaleIdx);
}

void CRRDHistView::OnUpdateScaleMax(CCmdUI* pCmdUI) 
{
	OnUpdateGen(pCmdUI);
	pCmdUI->SetCheck( m_pCurSelChart->GetCurScaling() == CCTAChart::eMaxScaleIdx );
}

void CRRDHistView::OnScaleAuth() 
{
	OnScale(CCTAChart::eAuthScaleIdx);
	CRRDHistView* pView = getOtherView();
	if (pView)
		pView->OnScale (CCTAChart::eAuthScaleIdx);
}

void CRRDHistView::OnUpdateScaleAuth(CCmdUI* pCmdUI) 
{
	OnUpdateGen(pCmdUI);
	pCmdUI->SetCheck( m_pCurSelChart->GetCurScaling() == CCTAChart::eAuthScaleIdx );
}

void CRRDHistView::OnScaleResHold() 
{
	OnScale(CCTAChart::eResHoldScaleIdx);
	CRRDHistView* pView = getOtherView();
	if (pView)
		pView->OnScale (CCTAChart::eResHoldScaleIdx);
}

void CRRDHistView::OnUpdateScaleResHold(CCmdUI* pCmdUI) 
{
	OnUpdateGen(pCmdUI);
	pCmdUI->SetCheck( m_pCurSelChart->GetCurScaling() ==  CCTAChart::eResHoldScaleIdx );
}

void CRRDHistView::OnGraphDisplayFlashLabels() 
{
	// toggle the flag to display the graph flash labels
	((CCTAApp*)APP)->SetFlashLabelFlag(  ! ((CCTAApp*)APP)->GetFlashLabelFlag() );
}

void CRRDHistView::OnUpdateGraphDisplayFlashLabels(CCmdUI* pCmdUI) 
{
	OnUpdateGen(pCmdUI);
	pCmdUI->SetCheck( ((CCTAApp*)APP)->GetFlashLabelFlag() );
}

void CRRDHistView::DoGraphRotateLabels(bool brot)
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

void CRRDHistView::OnGraphRotateLabels()
{
	bool brot = m_CmptChart.GetRotateLabelsFlag();
	((CCTAApp*)APP)->m_bRRDRotatelabel = !brot;
	DoGraphRotateLabels(!brot);
	// Rotation de l'autre moitiée si elle existe
	CRRDHistView* pView = getOtherView();
	if (pView)
		pView->DoGraphRotateLabels(!brot);
}

void CRRDHistView::OnUpdateGraphRotateLabels( CCmdUI* pCmdUI )
{
	OnUpdateGen(pCmdUI);
	pCmdUI->SetCheck ( (m_CmptChart.GetRotateLabelsFlag()==1) ? 1 : 0 );
}


void CRRDHistView::OnPrint()
{
	CCTAChartData* pCTAChartData = m_pCurSelChart->GetCurChartData();
	if ( pCTAChartData == NULL )
		return;
	int iCurSelTab = m_ChartTabCtrl.GetCurSel();  // get currently selected tab

	// which graph should we draw ?
	(pCTAChartData->GetGraphType() == CCTAChartData::eCmpt) ? m_BktChart.Print() : m_CmptChart.Print();
}

void CRRDHistView::OnUpdateGen( CCmdUI* pCmdUI )
{
	YM_RecordSet* pCmptEvolSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_ALT_KEY : CMPTEVOL_KEY);
	YM_RecordSet* pCmptEvolAvgSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_ALT_KEY : CMPTEVOL_AVG_KEY);
	YM_RecordSet* pBktEvolSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_ALT_KEY : BKTEVOL_KEY);
	YM_RecordSet* pBktEvolAvgSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_ALT_KEY : BKTEVOL_AVG_KEY);

	ASSERT( pBktEvolSet != NULL );
	ASSERT( pBktEvolAvgSet != NULL );
	ASSERT( pCmptEvolSet != NULL );
	ASSERT( pCmptEvolAvgSet != NULL );

	pCmdUI->Enable( pCmptEvolSet->IsOpen() && pCmptEvolAvgSet->IsOpen() && pBktEvolSet->IsOpen() && pBktEvolAvgSet->IsOpen() );
}

void CRRDHistView::OnDestroy() 
{
	CView::OnDestroy();

	YM_PersistentChildWnd* pParentFrame = (YM_PersistentChildWnd*)GetParentFrame();
	if ( pParentFrame == NULL)
		return;

	// if we have another CRRDHistView instance, set it as the child view of the parent frame !
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	CView* pView; 
	POSITION pos = pDoc->GetFirstViewPosition();
	while( pos != NULL ) 
	{
		pView = pDoc->GetNextView( pos ); 
		// do we have a spliiter window activated (i.e., 2 views ?)
		if( pView->IsKindOf( RUNTIME_CLASS(CRRDHistView) ) && pView != this ) 
		{
			/*ano 147777*/ CRRDHistView* prrdview = (CRRDHistView*)pView;
			pParentFrame->SetChildView (prrdview);
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
	CRRDHistView* pRRDView = getOtherView();

	YM_RecordSet* pCmptEvolSet = 
		YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_ALT_KEY : CMPTEVOL_KEY);
	ASSERT( pCmptEvolSet != NULL );

	YM_RecordSet* pCmptEvolAvgSet = 
		YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_ALT_KEY : CMPTEVOL_AVG_KEY);
	ASSERT( pCmptEvolAvgSet != NULL );

	YM_RecordSet* pBktEvolSet = 
		YM_Set<CRRDHistBktEvol>::FromKey((GetAltFlag()) ? BKTEVOL_ALT_KEY : BKTEVOL_KEY);
	ASSERT( pBktEvolSet != NULL );

	YM_RecordSet* pBktEvolAvgSet = 
		YM_Set<CRRDHistBktEvol>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_ALT_KEY : BKTEVOL_AVG_KEY);
	ASSERT( pBktEvolAvgSet != NULL );

	pBktEvolSet->RemoveObserver(this);
	if (pRRDView)
		pBktEvolSet->RemoveObserver(pRRDView);
	pBktEvolSet->Close();
	pBktEvolAvgSet->RemoveObserver(this);
	if (pRRDView)
		pBktEvolAvgSet->RemoveObserver(pRRDView);
	pBktEvolAvgSet->Close();
	pCmptEvolSet->RemoveObserver(this);
	if (pRRDView)
		pCmptEvolSet->RemoveObserver(pRRDView);
	pCmptEvolSet->Close();
	pCmptEvolAvgSet->RemoveObserver(this);
	if (pRRDView)
		pCmptEvolAvgSet->RemoveObserver(pRRDView);
	pCmptEvolAvgSet->Close();


	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetAltFlag() )
	{
		if ( !GetComptagesView() )
		{
			YM_RecordSet* pComptagesSet = 
				YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_KEY);
			pComptagesSet->RemoveObserver(this);
			if (pRRDView)
				pComptagesSet->RemoveObserver(pRRDView);
			pComptagesSet->Close();
		}
	}
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag() )
	{
		if ( !GetComptagesView() )
		{
			YM_RecordSet* pComptagesHistSet = 
				YM_Set<YmIcComptagesDom>::FromKey(COMPTAGES_HIST_KEY);
			pComptagesHistSet->RemoveObserver(this);
			if (pRRDView)
				pComptagesHistSet->RemoveObserver(pRRDView);
			pComptagesHistSet->Close();
		}
	}

	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	{
		YM_RecordSet* pHistoEvolSet =
			YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_ALT_KEY : HISTOEVOL_KEY);
		pHistoEvolSet->RemoveObserver(this);
		if (pRRDView)
			pHistoEvolSet->RemoveObserver(pRRDView);
		pHistoEvolSet->Close();

		YM_RecordSet* pHistoEvolCmptSet =
			YM_Set<YmIcHistoEvolDom>::FromKey((GetAltFlag()) ? HISTOEVOL_CMPT_ALT_KEY : HISTOEVOL_CMPT_KEY);
		pHistoEvolCmptSet->RemoveObserver(this);
		if (pRRDView)
			pHistoEvolCmptSet->RemoveObserver(pRRDView);
		pHistoEvolCmptSet->Close();
	}
}

///////////////////////////////////////////////////////
// Resize the chart window 
//
void CRRDHistView::ResizeChart()
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
void CRRDHistView::ClearCmptGraphs()
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

double CRRDHistView::GetComptageValue(int iDataSet, YmIcComptagesDom* pRecord)
{
	if (pRecord)
	{
		if (iDataSet == eCmptComptageOfferStdvHigh)
		{
			return (pRecord->MoyOffMoisIsNull() || pRecord->EctOffMoisIsNull()) ? 
INVALID_GRAPH_VALUE : pRecord->MoyOffMois() + pRecord->EctOffMois();
		}
		else if (iDataSet == eCmptComptageOffer)
		{
			return pRecord->NbOffert();
		}
		else if (iDataSet == eCmptComptageOfferStdvLow)
		{
			return (pRecord->MoyOffMoisIsNull() || pRecord->EctOffMoisIsNull()) ? 
INVALID_GRAPH_VALUE : pRecord->MoyOffMois() - pRecord->EctOffMois();
		}
		else if (iDataSet == eCmptComptageOccupStdvHigh)
		{
			return (pRecord->MoyOccMoisIsNull() || pRecord->EctOccMoisIsNull()) ? 
INVALID_GRAPH_VALUE : pRecord->MoyOccMois() + pRecord->EctOccMois();
		}
		else if (iDataSet == eCmptComptageOccup)
		{
			return pRecord->NbOccupees();
		}
		else if (iDataSet == eCmptComptageOccupStdvLow)
		{
			return (pRecord->MoyOccMoisIsNull() || pRecord->EctOccMoisIsNull()) ? 
INVALID_GRAPH_VALUE : pRecord->MoyOccMois() - pRecord->EctOccMois();
		}
		else if ( (iDataSet == eCmptComptageHist1Offer) ||
			(iDataSet == eCmptComptageHist2Offer) ||
			(iDataSet == eCmptComptageHist3Offer) )
		{
			return pRecord->NbOffert();
		}
		else if ( (iDataSet == eCmptComptageHist1Occup) ||
			(iDataSet == eCmptComptageHist2Occup) ||
			(iDataSet == eCmptComptageHist3Occup) )
		{
			return pRecord->NbOccupees();
		}
	}
	return INVALID_GRAPH_VALUE;
}

CString CRRDHistView::MakeSzHisto (const char* szComment , unsigned long ldate)
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

void CRRDHistView::UpdateHistoText()
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
	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag())
	{
		if (szHisto1.IsEmpty())
			szHisto1 = m_histocomptages_comment1;
		if (szHisto2.IsEmpty())
			szHisto2 = m_histocomptages_comment2;
		if (szHisto3.IsEmpty())
			szHisto3 = m_histocomptages_comment3;
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

void CRRDHistView::EmptyInvalidHistoDates()
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
int CRRDHistView::GetRealResaCmpt(char cmpt, int iLevelCur)
{
	/* iLevelCur indique le bucket, on retourne les réservation de celui-ci.
	si égal -1 on retourne la somme des réservations du compartiment.     */
	int iRealResaCmpt = -1;

	YM_VnlLeg* pVnlLeg;
	YM_VnlCmpt* pVnlCmpt = NULL;
	YM_VnlBkt* pVnlBkt = NULL;
	YM_VnlSci* pVnlSci = NULL;

	char vnlcmpt = cmpt;
	if (GetAltFlag())
		vnlcmpt -= 32; // remise en majuscule

	CResaRailView* pView = ((CMainFrame*) AfxGetApp()->GetMainWnd())->GetResaRailView();
	if (!pView) return iRealResaCmpt;
	CLegView* pLegView = pView->GetLegView();
	if (!pLegView) return iRealResaCmpt;
	YmIcLegsDom* pLegDom = pLegView->GetLoadedLeg();
	YM_Vnl* m_pVnl = pView->GetVnl();
	if (!m_pVnl) return iRealResaCmpt;

	YM_VnlLegIterator* LegIterator = m_pVnl->CreateVnlLegIterator();
	iRealResaCmpt = 0;
	for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
	{
		pVnlLeg = LegIterator->Current();
		if ((pLegDom->LegDest() == pVnlLeg->GetLegDest()) && 
			(pLegDom->LegOrig() == pVnlLeg->GetLegOrig()))
		{
			YM_VnlCmptIterator* CmptIterator = pVnlLeg->CreateVnlCmptIterator();

			for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
			{
				pVnlCmpt = CmptIterator->Current();
				if ( pVnlCmpt->GetCmptId() == vnlcmpt )  // is it this cmpt ?
				{
					if (GetAltFlag())
					{ // sous-contingent
						YM_VnlSciIterator* SciIterator = pVnlCmpt->CreateVnlSciIterator();
						for ( SciIterator->First(); ! SciIterator->Finished(); SciIterator->Next() )
						{
							pVnlSci = SciIterator->Current();
							if ( pVnlSci->GetNestLevel() == iLevelCur )  // is it this nest level ?
							{
								iRealResaCmpt = pVnlSci->GetBaseSold();
								delete SciIterator;
								delete CmptIterator;
								delete LegIterator;
								return iRealResaCmpt;
							}
							else if (iLevelCur == -1)
								iRealResaCmpt = iRealResaCmpt + pVnlSci->GetBaseSold();
						}
						delete SciIterator;
					}
					else
					{
						YM_VnlBktIterator* BktIterator = pVnlCmpt->CreateVnlBktIterator();
						for ( BktIterator->First(); ! BktIterator->Finished(); BktIterator->Next() )
						{
							pVnlBkt = BktIterator->Current();
							if ( pVnlBkt->GetNestLevel() == iLevelCur )  // is it this nest level ?
							{
								iRealResaCmpt = pVnlBkt->GetBaseSold();
								delete BktIterator;
								delete CmptIterator;
								delete LegIterator;
								return iRealResaCmpt;
							}
							else if (iLevelCur == -1)
								iRealResaCmpt = iRealResaCmpt + pVnlBkt->GetBaseSold();
						}
						delete BktIterator;
					}
				}
			}
			delete CmptIterator;
		}
	}
	delete LegIterator;
	return iRealResaCmpt;
}

void CRRDHistView::BuildCmptGraphs()
{
	if ( !m_bCmptDataValid || !m_bCmptAvgDataValid || !m_bCmptPrevisionVenteDataValid ||
		( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && !m_bHistoDataValid ) ||
		( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetAltFlag() && !m_bComptagesDataValid ) ||
		( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag() && !m_bComptagesHistDataValid ) ||
		( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() && !m_bPrevisionValid ) 
		)
		return;

	// clean up the previous data structure(s)
	ClearCmptGraphs();
	debjxHoiTra = finjxHoiTra = 999;

	UpdateHistoText();
	

	//DM7978 - LME - YI-5050
	// oha : ano 78122
	FillHistoDataSet();

	CObArray DataArray; // tableau de CDataGraph, un par reading day possible.
	DataArray.SetSize (GetDocument()->GetMaxRrdIndex()+2); // attention les J_X commence à -1.
	int i;
	for (i = 0; i < DataArray.GetSize(); i++)
		DataArray[i] = NULL;

	// Define the iterator to iterate through compartments
	YM_RecordSet* pCmptEvolSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_ALT_KEY : CMPTEVOL_KEY);

	YM_Iterator<CRRDHistCmptEvol>* pCmptIdxIterator =
		((CRRDHistCmptEvolSet*) pCmptEvolSet)->CreateKeyIterator();
	YM_RecordSet* pCmptEvolAvgSet = YM_Set<CRRDHistCmptEvol>::FromKey((GetAltFlag()) ? CMPTEVOL_AVG_ALT_KEY : CMPTEVOL_AVG_KEY);

	// set text in status bar control
	((CChildFrame*)GetParentFrame())->
		GetStatusBar()->SetPaneOneAndLock(
		((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BUILDING_GRAPH) );

	CStringArray aszXLabels;  // array to hold the X-axis labels (to be put into the TabToData object)
	YmIcTrancheLisDom* pTranche =  // Juste pour récupérer le last_rrd_index de la tranche
		YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	dernierJX = pTranche->LastJX();

	// Boucle sur les cmpt
	for( pCmptIdxIterator->First(); !pCmptIdxIterator->Finished(); pCmptIdxIterator->Next() ) 
	{ 
		// create a new cta graph data object for the compartment
		CCTAChartData* pCTAChartData = new CCTAChartData( &m_CmptChart, CCTAChartData::eCmpt, NULL, NULL );

		// loop through all the records for this compartment
		int iCmptCurrentRrdIndex = -1;  // reset current RRD index
		int iCmptNextRrdIndex = -1;  // pour affichage resa temps reel
		BOOL b_realTimeAdded  = FALSE;
		int minRrdIndex = 300;
		const char cmpt = pCmptIdxIterator->Current()->Cmpt()[0];

		// Define the iterator to iterate through records for this compartment
		YM_Iterator<CRRDHistCmptEvol>* pCmptRowIterator =
			((CRRDHistCmptEvolSet*) pCmptEvolSet)->CreateIterator( *(pCmptIdxIterator->Current()->Cmpt()) );
		for( pCmptRowIterator->First(); !pCmptRowIterator->Finished(); pCmptRowIterator->Next() ) 
		{
			YmIcCmptEvolDom* pCmptEvol = pCmptRowIterator->Current();
			this->nature = pCmptEvol->Nature();//DM8029 - set nature into our class
			// ano 525709, ajout du j-x courant dans la requête pour eviter retour vide, mais
			// non prise en compte de ce j-x dans l'affichage
			CString sjx;
			sjx.Format("%d",pCmptEvol->RrdIndex());
			if (((CCTAApp*)APP)->GetDocument()->GetCurRrdList().Find(sjx) >= 0)
			{
				CDataGraph* pDataGraph = new CDataGraph();
				pDataGraph->pCmptEvol = pCmptEvol;
				//if (dernierJX > pCmptEvol->RrdIndex())
				//	dernierJX = pCmptEvol->RrdIndex();
				pDataGraph->rrd_index = pCmptEvol->RrdIndex();
				DataArray.SetAt (pCmptEvol->RrdIndex()+1, (CObject*)pDataGraph);
				if (pCmptEvol->RrdIndex() < minRrdIndex)
					minRrdIndex = pCmptEvol->RrdIndex();

				// set the maximum values
				pCTAChartData->SetGraphMax( (double)pCmptEvol->Capacity() );  // is this the max graph value ?
				pCTAChartData->SetGraphMax( (double)pCmptEvol->OvbLevelCur() );  // is this the max graph value ?
				pCTAChartData->SetGraphMax( (double)(pCmptEvol->ResHoldIdv()+pCmptEvol->ResHoldGrp()) );  // is this the max graph value ?
				pCTAChartData->SetResHoldMax( (double)(pCmptEvol->ResHoldIdv()+pCmptEvol->ResHoldGrp()) );  // is this the max res hold value ?
				pCTAChartData->SetAuthMax( (double)pCmptEvol->Capacity() );
				//if ( pTranche->LastJX() == pCmptEvol->RrdIndex() )
				iCmptCurrentRrdIndex = pCmptEvol->RrdIndex(); // Contient le dernier rrd avec données de montée en charge resa.
			}
		}
		delete pCmptRowIterator;  // delete the row iterator



		// Define the iterator to iterate through records for this compartment
		YM_Iterator<CRRDHistCmptEvol>* pCmptAvgRowIterator =
			((CRRDHistCmptEvolSet*) pCmptEvolAvgSet)->CreateIterator( *(pCmptIdxIterator->Current()->Cmpt()) );
		for( pCmptAvgRowIterator->First(); !pCmptAvgRowIterator->Finished(); pCmptAvgRowIterator->Next() ) 
		{
			YmIcCmptEvolDom* pCmptEvol = pCmptAvgRowIterator->Current();
			CDataGraph* pDataGraph = (CDataGraph*)DataArray.GetAt(pCmptEvol->RrdIndex()+1);
			if (!pDataGraph)
			{
				pDataGraph = new CDataGraph();
				pDataGraph->pCmptEvol = pCmptEvol;
				pDataGraph->rrd_index = pCmptEvol->RrdIndex();
				DataArray.SetAt (pCmptEvol->RrdIndex()+1, (CObject*)pDataGraph);
			}
			pDataGraph->pCmptEvol->ResHoldAvg(pCmptEvol->ResHoldAvg());
			pDataGraph->pCmptEvol->ResHoldStdv(pCmptEvol->ResHoldStdv());
			if (pCmptEvol->RrdIndex() < minRrdIndex)
				minRrdIndex = pCmptEvol->RrdIndex();
			if ((pCmptEvol->RrdIndex() < iCmptCurrentRrdIndex) && (iCmptNextRrdIndex == -1))
				iCmptNextRrdIndex = pCmptEvol->RrdIndex();

			pCTAChartData->SetResHoldMax( (double)(pCmptEvol->ResHoldAvg() + pCmptEvol->ResHoldStdv()) );  // is this the max res hold value ?
		}
		delete pCmptAvgRowIterator;  // delete the row iterator


		if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRealTimeInRRDHistFlag())
		{ // DM 5350, ajout du temps reel dans la fenêtre montée en charge resa
			if ((iCmptCurrentRrdIndex > 0) && (iCmptCurrentRrdIndex > minRrdIndex))
			{
				int RealResaCmpt = GetRealResaCmpt(cmpt, -1 );
				if (RealResaCmpt >= 0)
				{
					CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(iCmptNextRrdIndex+1); // avant iCmptCurrentRrdIndex-1, mais +1 pour décalage du tableau
					if (!ppData)
					{
						ppData = new CDataGraph();
						ppData->rrd_index = iCmptNextRrdIndex;
						DataArray.SetAt (ppData->rrd_index + 1, (CObject*) ppData);
					}
					ppData->ResaTempsReel = (double)RealResaCmpt;
					b_realTimeAdded = TRUE;
				}
			}
		}	

		if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && mp_HistoData)
		{
			double hAuth, hResa;
			int mamax = __min (DataArray.GetSize()-1, mp_HistoData->m_max_rrd);
			for (i = mp_HistoData->m_min_rrd; i <= mamax; i++)
			{
				if (mp_HistoData->IsRrd(i))
				{
					CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i+1);
					if (!ppData)
					{
						ppData = new CDataGraph();
						ppData->rrd_index = i;
						DataArray.SetAt (i+1, (CObject*) ppData);
					}
					if (mp_HistoData->GetNbDate() >= 1)
					{
						hAuth = ppData->AuthHist1 = GetAltFlag() ? mp_HistoData->GetCapa(1, cmpt, i) : mp_HistoData->GetAuth (1, cmpt, i, 0);
						hResa = ppData->ResaHist1 = mp_HistoData->GetSumResa (1, cmpt, i);
						if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
							ppData->HoiTra1 = mp_HistoData->GetHoiTra (1, cmpt, i);
						if (hAuth != INVALID_GRAPH_VALUE)
						{
							pCTAChartData->SetGraphMax( hAuth );
							pCTAChartData->SetAuthMax( hAuth );
						}
						if (hResa != INVALID_GRAPH_VALUE)
						{
							pCTAChartData->SetGraphMax( hResa );
							pCTAChartData->SetResHoldMax( hResa );
						}
					}
					if (mp_HistoData->GetNbDate() >= 2)
					{
						hAuth = ppData->AuthHist2 = GetAltFlag() ? mp_HistoData->GetCapa(2, cmpt, i) : mp_HistoData->GetAuth (2, cmpt, i, 0);
						hResa = ppData->ResaHist2 = mp_HistoData->GetSumResa (2, cmpt, i);
						if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
							ppData->HoiTra2 = mp_HistoData->GetHoiTra (2, cmpt, i);
						if (hAuth != INVALID_GRAPH_VALUE)
						{
							pCTAChartData->SetGraphMax( hAuth );
							pCTAChartData->SetAuthMax( hAuth );
						}
						if (hResa != INVALID_GRAPH_VALUE)
						{
							pCTAChartData->SetGraphMax( hResa );
							pCTAChartData->SetResHoldMax( hResa );
						}
					}
					if (mp_HistoData->GetNbDate() >= 3)
					{
						hAuth = ppData->AuthHist3 = GetAltFlag() ? mp_HistoData->GetCapa(3, cmpt, i) : mp_HistoData->GetAuth (3, cmpt, i, 0);  
						hResa = ppData->ResaHist3 = mp_HistoData->GetSumResa (3, cmpt, i);
						if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
							ppData->HoiTra3 = mp_HistoData->GetHoiTra (3, cmpt, i);
						if (hAuth != INVALID_GRAPH_VALUE)
						{
							pCTAChartData->SetGraphMax( hAuth );
							pCTAChartData->SetAuthMax( hAuth );
						}
						if (hResa != INVALID_GRAPH_VALUE)
						{
							pCTAChartData->SetGraphMax( hResa );
							pCTAChartData->SetResHoldMax( hResa );
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
					CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(pResaBkt->RrdIndex()+1);
					if (!ppData)
					{
						ppData = new CDataGraph();
						ppData->rrd_index = pResaBkt->RrdIndex();
						DataArray.SetAt (pResaBkt->RrdIndex()+1, (CObject*) ppData);
					}
					ppData->AddPrevision (pResaBkt->ResHoldInd());
				}
			}
			delete pIteratorBkt;
		}*/

		//DT Prevision
		//long res = -1;
		if (!((CCTAApp*)APP)->GetRhealysFlag() && !this->GetAltFlag())
		{
			YM_Iterator<YmIcPrevisionDom>* pIteratorCmpt = YM_Set<YmIcPrevisionDom>::FromKey(PREVISION_VENTE_KEY)->CreateIterator();
			YmIcPrevisionDom* pPrevisionCmpt = NULL;
			for( pIteratorCmpt->First(); !pIteratorCmpt->Finished(); pIteratorCmpt->Next() )
			{
				pPrevisionCmpt = pIteratorCmpt->Current();
				if (tolower(pPrevisionCmpt->getCmpt()[0]) == tolower(cmpt))
				{
					int jx = pPrevisionCmpt->getJX();
					CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(pPrevisionCmpt->getJX()+1);
					if (!ppData)
					{
						ppData = new CDataGraph();
						ppData->rrd_index = pPrevisionCmpt->getJX();
						//TRACE("SECONDARY : %d\n", pPrevisionCmpt->getJX());
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
			// Si l'element n'est pas nul on incremente lNumPts
			if (DataArray.GetAt(i))
				lNumPts++;
		}
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetAltFlag())  // Is this a Comptages client ?
			lNumPts++;  // for Comptages clients, we add one point to graph the "Comptages"

		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag())  // Is this a Comptages History client ?
			lNumPts++;  // for Comptages History clients, we add one point to graph the "Comptages Hsitory"

		// Retrouver l'indice du dernier point pour les montées en charge de la tranche
		int lastCmptIndex = 0;
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				if (ppData->rrd_index == iCmptCurrentRrdIndex)
					i = 0;
				else
					lastCmptIndex ++;
			}
		}



		if ((debjxHoiTra == 999) && ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{ // retrouver premier et dernier j-x des horaires indicatifs sur la courbe de montée en charge.
			for (i= 0; i < DataArray.GetSize(); i++)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
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

		// remove the points which do not exist from current rrd to rrd = 0
		pChartData->SetFirstPoint(eCmptCapacity, 0);
		pChartData->SetLastPoint(eCmptCapacity, lastCmptIndex);
		pChartData->SetFirstPoint(eCmptOvbLevelCur, 0);
		pChartData->SetLastPoint(eCmptOvbLevelCur, lastCmptIndex);
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

		// remove all but the last points for "Comptages" (only for TRN client)
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetAltFlag() )  // Is this a Comptages client ?
		{
			int k;
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag() )
			{
				k = lNumPts -1;
				pChartData->SetFirstPoint(eCmptComptageHist1Offer, k);
				pChartData->SetFirstPoint(eCmptComptageHist1Occup, k);
				pChartData->SetFirstPoint(eCmptComptageHist2Offer, k);
				pChartData->SetFirstPoint(eCmptComptageHist2Occup, k);
				pChartData->SetFirstPoint(eCmptComptageHist3Offer, k);
				pChartData->SetFirstPoint(eCmptComptageHist3Occup, k);
			}
			else
				k = lNumPts;
			// Ne garder que le dernier point pour les courbes comptages
			pChartData->SetFirstPoint(eCmptComptageOfferStdvHigh, k-1);
			pChartData->SetLastPoint(eCmptComptageOfferStdvHigh, k-1);
			pChartData->SetFirstPoint(eCmptComptageOffer, k-1);
			pChartData->SetLastPoint(eCmptComptageOffer, k-1);
			pChartData->SetFirstPoint(eCmptComptageOfferStdvLow, k-1);
			pChartData->SetLastPoint(eCmptComptageOfferStdvLow, k-1);

			pChartData->SetFirstPoint(eCmptComptageOccupStdvHigh, k-1);
			pChartData->SetLastPoint(eCmptComptageOccupStdvHigh, k-1);
			pChartData->SetFirstPoint(eCmptComptageOccup, k-1);
			pChartData->SetLastPoint(eCmptComptageOccup, k-1);
			pChartData->SetFirstPoint(eCmptComptageOccupStdvLow, k-1);
			pChartData->SetLastPoint(eCmptComptageOccupStdvLow, k-1);

			// Ne pas prendre les points des colonnes comptages sur courbe moyenne et EC
			pChartData->SetLastPoint(eCmptResHoldAvg, k-2);
			pChartData2->SetLastPoint(eCmptResHoldStdvHigh, k-2);
			pChartData2->SetLastPoint(eCmptResHoldStdvLow, k-2);

			// Ne pas pas prendre les points de colonnes comptages pour les courbes historiques
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && mp_HistoData)
			{
				int nbd = mp_HistoData->GetNbDate();
				pChartData->SetLastPoint(eCmptHisto1OvbLevel, (nbd < 1) ? 0 : k-2);
				pChartData->SetLastPoint(eCmptHisto1ResHoldTot, (nbd < 1) ? 0 :k-2);
				pChartData->SetLastPoint(eCmptHisto2OvbLevel, (nbd < 2) ? 0 : k-2);
				pChartData->SetLastPoint(eCmptHisto2ResHoldTot, (nbd < 2) ? 0 : k-2);
				pChartData->SetLastPoint(eCmptHisto3OvbLevel, (nbd < 3) ? 0 : k-2);
				pChartData->SetLastPoint(eCmptHisto3ResHoldTot, (nbd < 3) ? 0 : k-2);

				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
				{
					pChartData->SetLastPoint(eCmptHisto1HoiTra, (nbd < 1) ? 0 :k-2);
					pChartData->SetLastPoint(eCmptHisto2HoiTra, (nbd < 2) ? 0 :k-2);
					pChartData->SetLastPoint(eCmptHisto3HoiTra, (nbd < 3) ? 0 :k-2);
				}
			}
		}

		// Masquage éventuel des données comptage
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetAltFlag())
		{
			if (! m_bDispComptageOfferStvd)
			{
				pChartData->SetDisplay(eCmptComptageOfferStdvHigh, XRT_DISPLAY_HIDE);
				pChartData->SetDisplay(eCmptComptageOfferStdvLow, XRT_DISPLAY_HIDE);
			}
			if (! m_bDispComptageOffer)
				pChartData->SetDisplay(eCmptComptageOffer, XRT_DISPLAY_HIDE);
			if (! m_bDispComptageOccupStvd)
			{
				pChartData->SetDisplay(eCmptComptageOccupStdvHigh, XRT_DISPLAY_HIDE);
				pChartData->SetDisplay(eCmptComptageOccupStdvLow, XRT_DISPLAY_HIDE);
			}
			if (! m_bDispComptageOccup)
				pChartData->SetDisplay(eCmptComptageOccup, XRT_DISPLAY_HIDE);
		}

		// Masquons les courbes historiques inutiles
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && mp_HistoData)
		{
			int nbd = mp_HistoData->GetNbDate();
			if (nbd < 3)
			{
				pChartData->SetLastPoint(eCmptHisto3OvbLevel, -1);
				pChartData->SetLastPoint(eCmptHisto3ResHoldTot, -1);
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
					pChartData->SetLastPoint(eCmptHisto3HoiTra, -1);
			}
			if (nbd < 2)
			{
				pChartData->SetLastPoint(eCmptHisto2OvbLevel, -1);
				pChartData->SetLastPoint(eCmptHisto2ResHoldTot, -1);
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
					pChartData->SetLastPoint(eCmptHisto2HoiTra, -1);
			}
			if (nbd < 1)
			{
				pChartData->SetLastPoint(eCmptHisto1OvbLevel, -1);
				pChartData->SetLastPoint(eCmptHisto1ResHoldTot, -1);
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
					pChartData->SetLastPoint(eCmptHisto1HoiTra, -1);
			}
			if (nbd >= 3)
			{
				if (!m_bDispHisto3AuthCur)
					pChartData->SetDisplay(eCmptHisto3OvbLevel, XRT_DISPLAY_HIDE);
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
				if (!m_bDispHisto2AuthCur)
					pChartData->SetDisplay(eCmptHisto2OvbLevel, XRT_DISPLAY_HIDE);
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
				if (!m_bDispHisto1AuthCur)
					pChartData->SetDisplay(eCmptHisto1OvbLevel, XRT_DISPLAY_HIDE);
				if (!m_bDispHisto1ResHoldTot)
					pChartData->SetDisplay(eCmptHisto1ResHoldTot, XRT_DISPLAY_HIDE);
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
				{
					if (!m_bDispHisto1ResHoldTot)
						pChartData->SetDisplay(eCmptHisto1HoiTra, XRT_DISPLAY_HIDE);
				}
			}	
		}

		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag())
		{
			if (m_histocomptages_date3 == -1)
			{
				pChartData->SetLastPoint(eCmptComptageHist3Offer, -1);
				pChartData->SetLastPoint(eCmptComptageHist3Occup, -1);
			}
			if (m_histocomptages_date2 == -1)
			{
				pChartData->SetLastPoint(eCmptComptageHist2Offer, -1);
				pChartData->SetLastPoint(eCmptComptageHist2Occup, -1);
			}
			if (m_histocomptages_date1 == -1)
			{
				pChartData->SetLastPoint(eCmptComptageHist1Offer, -1);
				pChartData->SetLastPoint(eCmptComptageHist1Occup, -1);
			}
			if (m_histocomptages_date3 == -1)
			{
				pChartData->SetDisplay(eCmptComptageHist3Offer, XRT_DISPLAY_HIDE);
				pChartData->SetDisplay(eCmptComptageHist3Occup, XRT_DISPLAY_HIDE);
			}
			else
			{
				if (!m_bDispHisto3ComptageOffer)
					pChartData->SetDisplay(eCmptComptageHist3Offer, XRT_DISPLAY_HIDE);
				if (!m_bDispHisto3ComptageOccup)
					pChartData->SetDisplay(eCmptComptageHist3Occup, XRT_DISPLAY_HIDE);
			}
			if (m_histocomptages_date2 == -1)
			{
				pChartData->SetDisplay(eCmptComptageHist2Offer, XRT_DISPLAY_HIDE);
				pChartData->SetDisplay(eCmptComptageHist2Occup, XRT_DISPLAY_HIDE);
			}
			else
			{
				if (!m_bDispHisto2ComptageOffer)
					pChartData->SetDisplay(eCmptComptageHist2Offer, XRT_DISPLAY_HIDE);
				if (!m_bDispHisto2ComptageOccup)
					pChartData->SetDisplay(eCmptComptageHist2Occup, XRT_DISPLAY_HIDE);
			}
			if (m_histocomptages_date1 == -1)
			{
				pChartData->SetDisplay(eCmptComptageHist1Offer, XRT_DISPLAY_HIDE);
				pChartData->SetDisplay(eCmptComptageHist1Occup, XRT_DISPLAY_HIDE);
			}
			else
			{
				if (!m_bDispHisto1ComptageOffer)
					pChartData->SetDisplay(eCmptComptageHist1Offer, XRT_DISPLAY_HIDE);
				if (!m_bDispHisto1ComptageOccup)
					pChartData->SetDisplay(eCmptComptageHist1Occup, XRT_DISPLAY_HIDE);
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
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				GetRRD( ppData->rrd_index, &szBuffer );
				aszXLabels.SetAtGrow( j, szBuffer );

				adXData[j]  = j + 1;
				j++;
			}
		}

		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetAltFlag() )  // Is this a Comptages client ?
		{  // for Comptages clients, we add one point to graph the "Comptages"
			szBuffer = "C";
			aszXLabels.SetAtGrow( j, szBuffer );
			adXData[j]  = j + 1;
			j++;
		}
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag() )  // Is this a Comptages History client ?
		{  // for Comptages History clients, we add one point to graph the "Comptages Hsitory"
			szBuffer = "C.H";
			aszXLabels.SetAtGrow( j, szBuffer );
			adXData[j]  = j + 1;
			j++;
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

		//courbe capacité
		j = 0;
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				if (ppData->pCmptEvol)
					adYData[j] = (double)ppData->pCmptEvol->Capacity();
				j++;
			}
		}
		pChartData->SetYData (eCmptCapacity, adYData, lNumPts, 0);


		j=0;
		memset(adYData, '\0', sizeof(double) * lNumPts);
		//Courbe Overbooking level
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				if (ppData->pCmptEvol)
					adYData[j] = (double)ppData->pCmptEvol->OvbLevelCur();
				j++;
			}
		}
		pChartData->SetYData (eCmptOvbLevelCur, adYData, lNumPts, 0);


		j=0;
		memset(adYData, '\0', sizeof(double) * lNumPts);
		//Courbe ResHoldAvg
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData )
			{
				if (ppData->pCmptEvol)
					adYData[j] = (! ppData->pCmptEvol->ResHoldAvgIsNull()) ?
					(double)ppData->pCmptEvol->ResHoldAvg() : INVALID_GRAPH_VALUE;
				j++;
			}
		}
		pChartData->SetYData (eCmptResHoldAvg, adYData, lNumPts, 0);

		j=0;
		memset(adYData, '\0', sizeof(double) * lNumPts);
		//Courbe ResHoldTot
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				if (ppData->pCmptEvol)
					adYData[j] = ((! ppData->pCmptEvol->ResHoldIdvIsNull()) && (! ppData->pCmptEvol->ResHoldGrpIsNull())) ?
					(double)(ppData->pCmptEvol->ResHoldIdv()+ppData->pCmptEvol->ResHoldGrp()) : INVALID_GRAPH_VALUE;
				if (ppData->ResaTempsReel != INVALID_GRAPH_VALUE)
					adYData[j] = ppData->ResaTempsReel;
				j++;
			}
		}
		pChartData->SetYData (eCmptResHoldTot, adYData, lNumPts, 0);

		j=0;
		memset(adYData, '\0', sizeof(double) * lNumPts);
		//Courbe ResHoldIdv
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
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
		//Courbe eCmptResHoldStdvHigh
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				if (ppData->pCmptEvol)
					adYData[j] = ((! ppData->pCmptEvol->ResHoldAvgIsNull()) && (! ppData->pCmptEvol->ResHoldStdvIsNull())) ?
					(double)(ppData->pCmptEvol->ResHoldAvg() + ppData->pCmptEvol->ResHoldStdv()) : INVALID_GRAPH_VALUE;
				j++;
			}
		}
		pChartData2->SetYData (eCmptResHoldStdvHigh, adYData, lNumPts, 0);

		j=0;
		memset(adYData, '\0', sizeof(double) * lNumPts);
		//Courbe eCmptResHoldStdvLow
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				if (ppData->pCmptEvol)
					adYData[j] = ((! ppData->pCmptEvol->ResHoldAvgIsNull()) && (! ppData->pCmptEvol->ResHoldStdvIsNull())) ?
					(double)( __max((ppData->pCmptEvol->ResHoldAvg() - ppData->pCmptEvol->ResHoldStdv()),0) ) : INVALID_GRAPH_VALUE;
				j++;
			}
		}
		pChartData2->SetYData (eCmptResHoldStdvLow, adYData, lNumPts, 0);

		// DM7978 - LME - YI-5050 - affichage d'une ligne du j-x histo jusqu'a la courbe de capacite
		//courbe jxHisto
		/*if(m_HistoDataSet.size() > 0)
		{
		memset(adYData, '\0', sizeof(double) * lNumPts);
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
		CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
		if (ppData)
		{
		if (ppData->pCmptEvol && jxHistoIdx == i && jxHistoIdx != -1)
		adYData[i] = (double)ppData->pCmptEvol->Capacity();
		}
		}
		pChartData->SetYData (eCmptJxHisto, adYData, lNumPts, 0);
		}*/
		// DM7978 - LME - FIN

		j=0;
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{
			//Courbe des horaires indicatifs, plaquée sur celle des résa totales
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					if (ppData->pCmptEvol)
						adYData[j] = ((! ppData->pCmptEvol->ResHoldIdvIsNull()) &&
						(! ppData->pCmptEvol->ResHoldGrpIsNull()) &&
						(!strcmp (ppData->pCmptEvol->HoiTra(), "I"))) ?
						(double)(ppData->pCmptEvol->ResHoldIdv()+ppData->pCmptEvol->ResHoldGrp()) : INVALID_GRAPH_VALUE;
					j++;
				}
			}
			pChartData->SetYData (eCmptHoiTra, adYData, lNumPts, 0);

			//idem sur la courbe des résa individuelles
			j=0;
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					if (ppData->pCmptEvol)
						adYData[j] = ((! ppData->pCmptEvol->ResHoldIdvIsNull()) &&
						(!strcmp (ppData->pCmptEvol->HoiTra(), "I"))) ?
						(double)(ppData->pCmptEvol->ResHoldIdv()) : INVALID_GRAPH_VALUE;
					j++;
				}
			}
			pChartData->SetYData (eCmptHoiTraIdv, adYData, lNumPts, 0);
		}

		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && mp_HistoData)
		{
			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eCmptHisto1OvbLevel
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->AuthHist1;
					j++;
				}
			}
			pChartData->SetYData (eCmptHisto1OvbLevel, adYData, lNumPts, 0);

			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eCmptHisto1ResHoldTot
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->ResaHist1;
					j++;
				}
			}
			pChartData->SetYData (eCmptHisto1ResHoldTot, adYData, lNumPts, 0);

			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eCmptHisto2OvbLevel
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->AuthHist2;
					j++;
				}
			}
			pChartData->SetYData (eCmptHisto2OvbLevel, adYData, lNumPts, 0);

			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eCmptHisto2ResHoldTot
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->ResaHist2;
					j++;
				}
			}
			pChartData->SetYData (eCmptHisto2ResHoldTot, adYData, lNumPts, 0);

			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eCmptHisto3OvbLevel
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->AuthHist3;
					j++;
				}
			}
			pChartData->SetYData (eCmptHisto3OvbLevel, adYData, lNumPts, 0);

			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eCmptHisto3ResHoldTot
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->ResaHist3;
					j++;
				}
			}
			pChartData->SetYData (eCmptHisto3ResHoldTot, adYData, lNumPts, 0);
		}

		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{
			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eCmptHisto1HoiTra
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->HoiTra1 ? ppData->ResaHist1 : INVALID_GRAPH_VALUE;
					j++;
				}
			}
			pChartData->SetYData (eCmptHisto1HoiTra, adYData, lNumPts, 0);

			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eCmptHisto2HoiTra
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->HoiTra2 ? ppData->ResaHist2 : INVALID_GRAPH_VALUE;
					j++;
				}
			}
			pChartData->SetYData (eCmptHisto2HoiTra, adYData, lNumPts, 0);

			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eCmptHisto3HoiTra
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->HoiTra3 ? ppData->ResaHist3 : INVALID_GRAPH_VALUE;
					j++;
				}
			}
			pChartData->SetYData (eCmptHisto3HoiTra, adYData, lNumPts, 0);

		}

		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetAltFlag())
		{
			YmIcLegsDom* pLeg = 
				YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
			int iDptDate = (! pTranche->DptDateRefIsNull()) ?
				pTranche->DptDateRef() : 
			((CCTAApp*)APP)->GetDocument()->GetReferenceDate(pTranche->DptDate(), -1);
			YmIcComptagesDom* pRecord = 
				((CCTAApp*)APP)->GetDocument()->GetComptagesRecord(
				&m_ComptagesArray,
				pTranche->FamilyNo(), 
				iDptDate, 
				pLeg->LegOrig(), 
				pCmptIdxIterator->Current()->Cmpt());
			memset(adYData, '\0', sizeof(double) * lNumPts);

			int cid = lNumPts-1;
			double td;
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag())
				cid --;
			td = adYData[cid] = GetComptageValue(eCmptComptageOfferStdvHigh, pRecord); // return INVALID_GRAPH_VALUE if pRecord NULL
			pChartData->SetYData (eCmptComptageOfferStdvHigh, adYData, lNumPts, 0);
			td = adYData[cid] = GetComptageValue(eCmptComptageOffer, pRecord);
			pChartData->SetYData (eCmptComptageOffer, adYData, lNumPts, 0);
			td = adYData[cid] = GetComptageValue(eCmptComptageOfferStdvLow, pRecord);
			pChartData->SetYData (eCmptComptageOfferStdvLow, adYData, lNumPts, 0);
			td = adYData[cid] = GetComptageValue(eCmptComptageOccupStdvHigh, pRecord);
			pChartData->SetYData (eCmptComptageOccupStdvHigh, adYData, lNumPts, 0);
			td = adYData[cid] = GetComptageValue(eCmptComptageOccup, pRecord);
			pChartData->SetYData (eCmptComptageOccup, adYData, lNumPts, 0);
			td = adYData[cid] = GetComptageValue(eCmptComptageOccupStdvLow, pRecord);
			pChartData->SetYData (eCmptComptageOccupStdvLow, adYData, lNumPts, 0);
		}

		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetAltFlag())
		{
			YmIcLegsDom* pLeg = 
				YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
			YmIcComptagesDom* pRecord;
			pRecord = ((CCTAApp*)APP)->GetDocument()->GetComptagesRecord(
				&m_ComptagesHistArray,
				pTranche->FamilyNo(), 
				m_histocomptages_date1, 
				pLeg->LegOrig(), 
				pCmptIdxIterator->Current()->Cmpt());
			memset(adYData, '\0', sizeof(double) * lNumPts);
			adYData[lNumPts] = GetComptageValue(eCmptComptageHist1Offer, pRecord);
			pChartData->SetYData (eCmptComptageHist1Offer, adYData, lNumPts-1, 0);
			adYData[lNumPts] = GetComptageValue(eCmptComptageHist1Occup, pRecord);
			pChartData->SetYData (eCmptComptageHist1Occup, adYData, lNumPts-1, 0);

			pRecord = ((CCTAApp*)APP)->GetDocument()->GetComptagesRecord(
				&m_ComptagesHistArray,
				pTranche->FamilyNo(), 
				m_histocomptages_date2, 
				pLeg->LegOrig(), 
				pCmptIdxIterator->Current()->Cmpt());
			adYData[lNumPts] = GetComptageValue(eCmptComptageHist2Offer, pRecord);
			pChartData->SetYData (eCmptComptageHist2Offer, adYData, lNumPts-1, 0);
			adYData[lNumPts] = GetComptageValue(eCmptComptageHist2Occup, pRecord);
			pChartData->SetYData (eCmptComptageHist2Occup, adYData, lNumPts-1, 0);

			pRecord = ((CCTAApp*)APP)->GetDocument()->GetComptagesRecord(
				&m_ComptagesHistArray,
				pTranche->FamilyNo(), 
				m_histocomptages_date3, 
				pLeg->LegOrig(), 
				pCmptIdxIterator->Current()->Cmpt());
			adYData[lNumPts] = GetComptageValue(eCmptComptageHist3Offer, pRecord);
			pChartData->SetYData (eCmptComptageHist3Offer, adYData, lNumPts-1, 0);
			adYData[lNumPts] = GetComptageValue(eCmptComptageHist3Occup, pRecord);
			pChartData->SetYData (eCmptComptageHist3Occup, adYData, lNumPts-1, 0);
		}

		/*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
		{
			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->pCmptPrevision->getPrevisionVente();
					j++;
				}
			}
			pChartData->SetYData (eCmptPrevision, adYData, lNumPts, 0);
		}*/

	//DT Prevision
		if (!((CCTAApp*)APP)->GetRhealysFlag() && !this->GetAltFlag())
		{
			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			int values_bypassed = 0;
			BOOL _continue = 1;
			int currentIndex = 0;
			int currentPrevIndex = 9999;
			int lastKnownIndex = 9999;

			for (i = DataArray.GetSize()-1; i>= 0 && _continue; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					if (ppData->pCmptPrevision)
					{
						pChartData->SetFirstPoint(eCmptPrevision, values_bypassed);
						_continue = 0;
					}
					values_bypassed = values_bypassed + 1;
				}
			}

			currentPrevIndex = values_bypassed;

			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					if (ppData->pCmptPrevision)
					{
						if (dernierJX > ppData->pCmptPrevision->getJX())
						{
							adYData[j] = ppData->pCmptPrevision->getPrevisionVente();
						}
						else
							adYData[j] = XRT_HUGE_VAL;
						//pChartData->SetLastPoint(eCmptPrevision, i);

					}
					else
						adYData[j] = XRT_HUGE_VAL;
					j++;
				}
			}
			pChartData->SetFirstPoint(eCmptPrevision, values_bypassed);
			pChartData->SetYData (eCmptPrevision, adYData, lNumPts, 0);
		}

		m_CmptChart.SetYAxisMin(0);
		m_CmptChart.SetY2AxisMin(0);

		// set the number of data points (ticks along the X axis)
		int iSize = aszXLabels.GetSize();
		pCTAChartData->m_aszXLabels.SetSize(iSize);

		for (i = 0; i < iSize; i++)
		{
			pCTAChartData->m_aszXLabels[i] = aszXLabels[i];
		}

		pCTAChartData->SetNumPts(lNumPts);

		// Masquage autres courbes si demandé  
		if (!m_bDispEC)
			pChartData2->SetDisplay(eCmptResHoldStdvHigh, XRT_DISPLAY_HIDE);
		if (!m_bDispCapa)
			pChartData->SetDisplay(eCmptCapacity, XRT_DISPLAY_HIDE);
		if (!m_bDispResHoldAvg)
			pChartData->SetDisplay(eCmptResHoldAvg, XRT_DISPLAY_HIDE);
		if (! m_bDispAuthCur || GetAltFlag())
			pChartData->SetDisplay(eCmptOvbLevelCur, XRT_DISPLAY_HIDE);
		if (! m_bDispResHoldTot)
			pChartData->SetDisplay(eCmptResHoldTot, XRT_DISPLAY_HIDE);
		if (! m_bDispResHoldIdv)
			pChartData->SetDisplay(eCmptResHoldIdv, XRT_DISPLAY_HIDE);

		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
		{
			if (! m_bDispResHoldTot)
				pChartData->SetDisplay(eCmptHoiTra, XRT_DISPLAY_HIDE);
			if (! m_bDispResHoldIdv)
				pChartData->SetDisplay(eCmptHoiTraIdv, XRT_DISPLAY_HIDE);
		}

		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag())
		{
			if (! m_bDispPrevision)
				pChartData->SetDisplay(eCmptPrevision, XRT_DISPLAY_HIDE);
		}

		//if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
		//{
		if (!((CCTAApp*)APP)->GetRhealysFlag() && !this->GetAltFlag())
			if (! m_bDispPrevision || YM_Set<YmIcPrevisionDom>::FromKey(PREVISION_VENTE_KEY)->IsEmpty())
				pChartData->SetDisplay(eCmptPrevision, XRT_DISPLAY_HIDE);
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
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
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
void CRRDHistView::DrawGraph()
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

	// DM7978 - LME - YI-5050 - affichage du j-x historique dans le header
	const char cmpt = m_szCurTabText.GetAt(0);
	// si on est en classe de controle (et non scx)
	if(cmpt >= 'A' && cmpt <= 'Z')
	{
		// uniquement sur les cmpt
		// NPI - ANo 70146
		//if(IsCmptTabSelected())
		if(m_szCurTabText.GetLength() == 1) //le label de l'onglet ne comporte qu'un caractere
		{
			m_eTabSelected = eCmptTabSelected;
		}
		else
		{
			m_eTabSelected = eBktTabSelected;
		}
		if (IsCmptTabSelected() && !((CCTAApp*)APP)->GetRhealysFlag())
		///
		{
			CString strLabel;
			// DM7978 - LME - YI-5050 - calcul du j-x histo // TODO recuperer le jx et ccmax 

			long jxHisto = -2;
			long ccMax = - 1; 
			if(m_HistoDataSet.size() > 0)
			{
				std::vector<YmIcHistoDataRRDDom> histoData = getHistoDataByCmpt(cmpt);
				jxHisto = CalculJxHisto(histoData);
				ccMax = getCcMax();
			}
			// cas ou jxHisto, ccMax ne sont pas calcules
			if (jxHisto == -2)
			{
				// CME 78122
				strLabel.Format("J-X = ? / CCmax = %c?", cmpt);
				//SRE 78122 - always display CCMax
				//strLabel.Format("J-X = ? / CCmax = %c%d", cmpt, ccMax);
			}
			else if (jxHisto != -2 && ccMax == -1)
			{
				strLabel.Format("J-X = %d / CCmax = %c?", jxHisto , cmpt);
			}
			else
			{
				strLabel.Format("J-X = %d / CCmax = %c%d", jxHisto, cmpt, ccMax);
			}
			char *szLabel = strLabel.GetBuffer(strLabel.GetLength()) ;
			char *szHeader[] = { szLabel , NULL };
			COLORREF headerColor = RGB(30,127,203);
			HFONT headerFont = /*CreateFont(
				16,                // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_BOLD,									 // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				ANSI_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				_T("Arial"));*/
			CreateFont(
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
			m_pCurSelChart->SetHeaderFont(headerFont);
			m_pCurSelChart->SetHeaderForegroundColor(headerColor); // TODO : recuperer la couleur de la capacité
			m_pCurSelChart->SetHeaderStrings(szHeader);
			m_pCurSelChart->SetHeaderAdjust(XRT_ADJUST_RIGHT);
		}
	}
	// DM7978 - LME - FIN
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
void CRRDHistView::ClearBktGraphs()
{
	// clear the client area
	m_BktChart.ShowWindow( SW_HIDE );
	Invalidate(FALSE);
	UpdateWindow();

	// delete the bucket entries from the tab array
	ClearTabs(CCTAChartData::eBkt);
	m_BktChart.ClearChartDataArray();  // delete all graph datasets

	// clear the header and footer
	/*char *szHeader[] = { "", NULL };
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
void CRRDHistChart::SetGraphProperties(enum CCTAChartData::GraphType eGraphType)
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
		SetNthSetLabel(GetParentView()->eBktAuthCur,
			(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AUTHORIZATIONS));
		SetNthSetLabel(GetParentView()->eBktResHoldAvailSum,
			(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLDAVAILSUM));
		SetNthSetLabel(GetParentView()->eBktResHoldTot,
			(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_TOT));
		SetNthSetLabel(GetParentView()->eBktResHoldIdv,
			(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_IDV));
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
			CString szLabelAuth = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AUTHORIZATIONS);
			CString szLabelResa = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESERVATIONS);
			CString szLabelA, szLabelR;
			CString szIncertain = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INCERTAIN);
			CString szConfirme = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CONFIRME);
			CString szIC;
			int incertain, confirme;

			szLabelA = szLabelAuth + " 1";
			szLabelR = szLabelResa + " 1";
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
			{
				if ( ((CCTAApp*)APP)->GetDocument()->GetHistoHoiTra(1, incertain, confirme))
				{
					szIC.Format (szIncertain, incertain, "");
					szLabelA = "Auto 1, ";
					szLabelA += szIC;
					if (confirme > -2)
					{
						szIC.Format (szConfirme, confirme, "");
						szLabelR = "Résa 1, ";
						szLabelR += szIC;
					}
				}
				SetNthSetLabel(GetParentView()->eBktHisto1HoiTra, (LPSTR)(LPCSTR)szLabelR);
			}
			SetNthSetLabel(GetParentView()->eBktHisto1AuthCur, (LPSTR)(LPCSTR)szLabelA);		
			SetNthSetLabel(GetParentView()->eBktHisto1ResHoldTot, (LPSTR)(LPCSTR)szLabelR);

			szLabelA = szLabelAuth + " 2";
			szLabelR = szLabelResa + " 2";
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
			{
				if ( ((CCTAApp*)APP)->GetDocument()->GetHistoHoiTra(2, incertain, confirme))
				{
					szIC.Format (szIncertain, incertain, "");
					szLabelA = "Auto 2, ";
					szLabelA += szIC;
					if (confirme > -2)
					{
						szIC.Format (szConfirme, confirme, "");
						szLabelR = "Résa 2, ";
						szLabelR += szIC;
					}
				}
				SetNthSetLabel(GetParentView()->eBktHisto2HoiTra, (LPSTR)(LPCSTR)szLabelR);
			}
			SetNthSetLabel(GetParentView()->eBktHisto2AuthCur, (LPSTR)(LPCSTR)szLabelA);		
			SetNthSetLabel(GetParentView()->eBktHisto2ResHoldTot, (LPSTR)(LPCSTR)szLabelR);

			szLabelA = szLabelAuth + " 3";
			szLabelR = szLabelResa + " 3";
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
			{
				if ( ((CCTAApp*)APP)->GetDocument()->GetHistoHoiTra(3, incertain, confirme))
				{
					szIC.Format (szIncertain, incertain, "");
					szLabelA = "Auto 3, ";
					szLabelA += szIC;
					if (confirme > -2)
					{
						szIC.Format (szConfirme, confirme, "");
						szLabelR = "Résa 3, ";
						szLabelR += szIC;
					}
				}
				SetNthSetLabel(GetParentView()->eBktHisto3HoiTra, (LPSTR)(LPCSTR)szLabelR);
			}
			SetNthSetLabel(GetParentView()->eBktHisto3AuthCur, (LPSTR)(LPCSTR)szLabelA);		
			SetNthSetLabel(GetParentView()->eBktHisto3ResHoldTot, (LPSTR)(LPCSTR)szLabelR);	
		}

		/*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
		{
			SetNthSetLabel(GetParentView()->eBktPrevision,
				(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_PREVISION));
		}*/

		SetNthSetLabel2(CRRDHistView::eBktResHoldStdvHigh,
			(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_STDV));
		SetNthSetLabel2(CRRDHistView::eBktResHoldStdvLow,
			(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_STDV));

		int nbb=0;
		XrtDataStyle *paBktDataStyles[18];
		paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eAuthStyleIdx);
		paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eBktResHoldAvailSumStyleIdx);
		paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eResHoldTotStyleIdx);
		paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eResHoldIdvStyleIdx);
		paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eResHoldAvgStyleIdx);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
		{
			paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHoiTraStyleIdx);
			paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHoiTraIdvStyleIdx);
		}
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
		{
			paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHisto3OvbLevelStyleIdx);
			paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHisto3ResHoldTotStyleIdx);
			paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHisto2OvbLevelStyleIdx);
			paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHisto2ResHoldTotStyleIdx);
			paBktDataStyles[nbb++] = m_aDataStyles.GetAt(eHisto1OvbLevelStyleIdx);
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
		while (nbb < 18)
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
		//SetDataStyleColor()
		SetNthSetLabel(GetParentView()->eCmptCapacity,
			(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(GetParentView()->GetAltFlag() ? IDS_CCM1 : IDS_CAPACITY));

		if (!GetParentView()->GetAltFlag())
			SetNthSetLabel(GetParentView()->eCmptOvbLevelCur,
			(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AUTHORIZATIONS));
		SetNthSetLabel(GetParentView()->eCmptResHoldTot,
			(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_TOT));
		SetNthSetLabel(GetParentView()->eCmptResHoldIdv,
			(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_IDV));
		SetNthSetLabel(GetParentView()->eCmptResHoldAvg,
			(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESHOLD_AVG));
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
		{
			SetNthSetLabel(GetParentView()->eCmptHoiTra,
				(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INCERTAIN));
			SetNthSetLabel(GetParentView()->eCmptHoiTraIdv,
				(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INCERTAIN));
		}
		/*
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
		{
		CString szLabelAuth = ((CCTAApp*)APP)->GetResource()->LoadResString(GetParentView()->GetAltFlag() ? IDS_CCM1 : IDS_AUTHORIZATIONS);
		CString szLabelResa = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESERVATIONS);
		CString szLabel;
		szLabel = szLabelAuth + " 1";
		SetNthSetLabel(GetParentView()->eCmptHisto1OvbLevel, (LPSTR)(LPCSTR)szLabel);		
		szLabel = szLabelResa + " 1";
		SetNthSetLabel(GetParentView()->eCmptHisto1ResHoldTot, (LPSTR)(LPCSTR)szLabel);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
		SetNthSetLabel(GetParentView()->eCmptHisto1HoiTra, (LPSTR)(LPCSTR)szLabel);
		szLabel = szLabelAuth + " 2";
		SetNthSetLabel(GetParentView()->eCmptHisto2OvbLevel, (LPSTR)(LPCSTR)szLabel);		
		szLabel = szLabelResa + " 2";
		SetNthSetLabel(GetParentView()->eCmptHisto2ResHoldTot, (LPSTR)(LPCSTR)szLabel);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
		SetNthSetLabel(GetParentView()->eCmptHisto2HoiTra, (LPSTR)(LPCSTR)szLabel);
		szLabel = szLabelAuth + " 3";
		SetNthSetLabel(GetParentView()->eCmptHisto3OvbLevel, (LPSTR)(LPCSTR)szLabel);		
		szLabel = szLabelResa + " 3";
		SetNthSetLabel(GetParentView()->eCmptHisto3ResHoldTot, (LPSTR)(LPCSTR)szLabel);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
		SetNthSetLabel(GetParentView()->eCmptHisto3HoiTra, (LPSTR)(LPCSTR)szLabel);
		}
		*/
		// Gestion des graphes historiques
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
		{
			CString szLabelAuth = ((CCTAApp*)APP)->GetResource()->LoadResString(GetParentView()->GetAltFlag() ? IDS_CCM1 : IDS_AUTHORIZATIONS);
			CString szLabelResa = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESERVATIONS);
			CString szLabelA, szLabelR;
			szLabelA = szLabelAuth + " 1";
			szLabelR = szLabelResa + " 1";
			CString szIncertain = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INCERTAIN);
			CString szConfirme = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CONFIRME);
			CString szIC;
			int incertain, confirme;
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
			{
				if ( ((CCTAApp*)APP)->GetDocument()->GetHistoHoiTra(1, incertain, confirme))
				{
					szIC.Format (szIncertain, incertain, "");
					szLabelA = GetParentView()->GetAltFlag() ? "CC-1 1, " : "Auto 1, ";
					szLabelA += szIC;
					if (confirme > -2)
					{
						szIC.Format (szConfirme, confirme, "");
						szLabelR = "Résa 1, ";
						szLabelR += szIC;
					}
				}
				SetNthSetLabel(GetParentView()->eCmptHisto1HoiTra, (LPSTR)(LPCSTR)szLabelR);
			}
			SetNthSetLabel(GetParentView()->eCmptHisto1OvbLevel, (LPSTR)(LPCSTR)szLabelA);		
			SetNthSetLabel(GetParentView()->eCmptHisto1ResHoldTot, (LPSTR)(LPCSTR)szLabelR);

			szLabelA = szLabelAuth + " 2";
			szLabelR = szLabelResa + " 2";
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
			{
				if ( ((CCTAApp*)APP)->GetDocument()->GetHistoHoiTra(2, incertain, confirme))
				{
					szIC.Format (szIncertain, incertain, "");
					szLabelA = GetParentView()->GetAltFlag() ? "CC-1 2, " : "Auto 2, ";
					szLabelA += szIC;
					if (confirme > -2)
					{
						szIC.Format (szConfirme, confirme, "");
						szLabelR = "Résa 2, ";
						szLabelR += szIC;
					}
				}
				SetNthSetLabel(GetParentView()->eCmptHisto2HoiTra, (LPSTR)(LPCSTR)szLabelR);
			}
			SetNthSetLabel(GetParentView()->eCmptHisto2OvbLevel, (LPSTR)(LPCSTR)szLabelA);		
			SetNthSetLabel(GetParentView()->eCmptHisto2ResHoldTot, (LPSTR)(LPCSTR)szLabelR);

			szLabelA = szLabelAuth + " 3";
			szLabelR = szLabelResa + " 3";
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
			{
				if ( ((CCTAApp*)APP)->GetDocument()->GetHistoHoiTra(3, incertain, confirme))
				{
					szIC.Format (szIncertain, incertain, "");
					szLabelA = GetParentView()->GetAltFlag() ? "CC-1 3, " : "Auto 3, ";
					szLabelA += szIC;
					if (confirme > -2)
					{
						szIC.Format (szConfirme, confirme, "");
						szLabelR = "Résa 3, ";
						szLabelR += szIC;
					}
				}
				SetNthSetLabel(GetParentView()->eCmptHisto3HoiTra, (LPSTR)(LPCSTR)szLabelR);
			}
			SetNthSetLabel(GetParentView()->eCmptHisto3OvbLevel, (LPSTR)(LPCSTR)szLabelA);		
			SetNthSetLabel(GetParentView()->eCmptHisto3ResHoldTot, (LPSTR)(LPCSTR)szLabelR);	
		}

		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetParentView()->GetAltFlag())  // Is this a Comptages client ?
		{
			SetNthSetLabel(GetParentView()->eCmptComptageOfferStdvHigh, "");
			SetNthSetLabel(GetParentView()->eCmptComptageOffer,
				(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMPTAGE_OFFER));
			SetNthSetLabel(GetParentView()->eCmptComptageOfferStdvLow,
				(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMPTAGE_OFFER_STDV));
			SetNthSetLabel(GetParentView()->eCmptComptageOccupStdvHigh, "");
			SetNthSetLabel(GetParentView()->eCmptComptageOccup,
				(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMPTAGE_OCCUP));
			SetNthSetLabel(GetParentView()->eCmptComptageOccupStdvLow,
				(LPSTR)(LPCSTR)((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMPTAGE_OCCUP_STDV));
		}
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !GetParentView()->GetAltFlag())
		{
			CString szLabelOffer = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMPTAGE_OFFER);
			CString szLabelOccup = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMPTAGE_OCCUP);
			CString szLabel;
			szLabel = szLabelOffer + " histo 1";
			SetNthSetLabel(GetParentView()->eCmptComptageHist1Offer, (LPSTR)(LPCSTR)szLabel);		
			szLabel = szLabelOccup + " histo 1";
			SetNthSetLabel(GetParentView()->eCmptComptageHist1Occup, (LPSTR)(LPCSTR)szLabel);
			szLabel = szLabelOffer + " histo 2";
			SetNthSetLabel(GetParentView()->eCmptComptageHist2Offer, (LPSTR)(LPCSTR)szLabel);		
			szLabel = szLabelOccup + " histo 2";
			SetNthSetLabel(GetParentView()->eCmptComptageHist2Occup, (LPSTR)(LPCSTR)szLabel);
			szLabel = szLabelOffer + " histo 3";
			SetNthSetLabel(GetParentView()->eCmptComptageHist3Offer, (LPSTR)(LPCSTR)szLabel);		
			szLabel = szLabelOccup + " histo 3";
			SetNthSetLabel(GetParentView()->eCmptComptageHist3Occup, (LPSTR)(LPCSTR)szLabel);

		}

		//if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
		//{
		if (!((CCTAApp*)APP)->GetRhealysFlag())
		{
			YM_RecordSet* pCmptPrevisionVenteSet = YM_Set<YmIcPrevisionDom>::FromKey(PREVISION_VENTE_KEY);
			bool jej = this->GetParentView()->GetAltFlag();
			//TRACE("GetYPointValue = %s\n", ((CRecDHistHelp*)m_pHelpDlg)->GetYPointValue(XRT_DATA, GetParentView()->eCmptPrevision, m_iCurSelectPt ));
			if (!((CRRDHistView*)this->GetParent())->GetAltFlag())
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
		XrtDataStyle *paCmptDataStyles[29];
		paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eCapStyleIdx);
		paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eAuthStyleIdx);
		paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eResHoldTotStyleIdx);
		paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eResHoldIdvStyleIdx);
		paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eResHoldAvgStyleIdx);
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
		{
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHoiTraStyleIdx);
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHoiTraIdvStyleIdx);
		}
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
		{
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto3OvbLevelStyleIdx);
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto3ResHoldTotStyleIdx);
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto2OvbLevelStyleIdx);
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto2ResHoldTotStyleIdx);
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto1OvbLevelStyleIdx);
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto1ResHoldTotStyleIdx);
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
			{
				paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto3HoiTraStyleIdx);
				paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto2HoiTraStyleIdx);
				paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eHisto1HoiTraStyleIdx);
			}
		}
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !GetParentView()->GetAltFlag())
		{
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eCmptComptageOfferStdvHighStyleIdx);
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eCmptComptageOfferStyleIdx);
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eCmptComptageOfferStdvLowStyleIdx);
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eCmptComptageOccupStdvHighStyleIdx);
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eCmptComptageOccupStyleIdx);
			paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eCmptComptageOccupStdvLowStyleIdx);
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag())
			{
				paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eCmptComptageHist3OfferStyleIdx);
				paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eCmptComptageHist3OccupStyleIdx);
				paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eCmptComptageHist2OfferStyleIdx);
				paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eCmptComptageHist2OccupStyleIdx);
				paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eCmptComptageHist1OfferStyleIdx);
				paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(eCmptComptageHist1OccupStyleIdx);
			}
		}
		else  /*if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())*/
			if (!((CCTAApp*)APP)->GetRhealysFlag() && !this->GetParentView()->GetAltFlag())
				paCmptDataStyles[nbc++] = m_aDataStyles.GetAt(ePrevisionStyleIdx);
		// On n'envisage pas ComptagesFlag et NomosFlag simultanement

		while (nbc < 29)
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


// Change le label des graphes historiques
void CRRDHistChart::ChangeHistoLabel(enum CCTAChartData::GraphType eGraphType,
																		 CString szHisto1, CString szHisto2, CString szHisto3)
{
	if (eGraphType == CCTAChartData::eBkt)  // bucket
	{
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && 
			((CRRDHistView*)GetParentView())->mp_HistoData)
		{
			int nbd = ((CRRDHistView*)GetParentView())->mp_HistoData->GetNbDate();
			CString szLabelAuth = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AUTHORIZATIONS);
			CString szLabelResa = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESERVATIONS);
			CString szLabel;
			if (nbd >= 1)
			{
				szLabel = szLabelAuth + " " + szHisto1;
				SetNthSetLabel(GetParentView()->eBktHisto1AuthCur, (LPSTR)(LPCSTR)szLabel);		
				szLabel = szLabelResa + " " + szHisto1;
				SetNthSetLabel(GetParentView()->eBktHisto1ResHoldTot, (LPSTR)(LPCSTR)szLabel);
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
					SetNthSetLabel(GetParentView()->eBktHisto1HoiTra, (LPSTR)(LPCSTR)szLabel);
			}
			if (nbd >= 2)
			{
				szLabel = szLabelAuth + " " + szHisto2;
				SetNthSetLabel(GetParentView()->eBktHisto2AuthCur, (LPSTR)(LPCSTR)szLabel);		
				szLabel = szLabelResa + " " + szHisto2;
				SetNthSetLabel(GetParentView()->eBktHisto2ResHoldTot, (LPSTR)(LPCSTR)szLabel);
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
					SetNthSetLabel(GetParentView()->eBktHisto2HoiTra, (LPSTR)(LPCSTR)szLabel);
			}
			if (nbd >= 3)
			{
				szLabel = szLabelAuth + " " + szHisto3;
				SetNthSetLabel(GetParentView()->eBktHisto3AuthCur, (LPSTR)(LPCSTR)szLabel);		
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
			((CRRDHistView*)GetParentView())->mp_HistoData)
		{
			int nbd = ((CRRDHistView*)GetParentView())->mp_HistoData->GetNbDate();
			CString szLabelAuth = ((CCTAApp*)APP)->GetResource()->LoadResString(GetParentView()->GetAltFlag() ? IDS_CCM1 : IDS_AUTHORIZATIONS);
			CString szLabelResa = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESERVATIONS);
			CString szLabel;
			if (nbd >= 1)
			{
				szLabel = szLabelAuth + " " + szHisto1;
				SetNthSetLabel(GetParentView()->eCmptHisto1OvbLevel, (LPSTR)(LPCSTR)szLabel);		
				szLabel = szLabelResa + " " + szHisto1;
				SetNthSetLabel(GetParentView()->eCmptHisto1ResHoldTot, (LPSTR)(LPCSTR)szLabel);
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
					SetNthSetLabel(GetParentView()->eCmptHisto1HoiTra, (LPSTR)(LPCSTR)szLabel);
			}
			if (nbd >= 2)
			{
				szLabel = szLabelAuth + " " + szHisto2;
				SetNthSetLabel(GetParentView()->eCmptHisto2OvbLevel, (LPSTR)(LPCSTR)szLabel);		
				szLabel = szLabelResa + " " + szHisto2;
				SetNthSetLabel(GetParentView()->eCmptHisto2ResHoldTot, (LPSTR)(LPCSTR)szLabel);
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
					SetNthSetLabel(GetParentView()->eCmptHisto2HoiTra, (LPSTR)(LPCSTR)szLabel);
			}
			if (nbd >= 3)
			{
				szLabel = szLabelAuth + " " + szHisto3;
				SetNthSetLabel(GetParentView()->eCmptHisto3OvbLevel, (LPSTR)(LPCSTR)szLabel);		
				szLabel = szLabelResa + " " + szHisto3;
				SetNthSetLabel(GetParentView()->eCmptHisto3ResHoldTot, (LPSTR)(LPCSTR)szLabel);
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
					SetNthSetLabel(GetParentView()->eCmptHisto3HoiTra, (LPSTR)(LPCSTR)szLabel);
			}	
		}
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() &&
			!((CRRDHistView*)GetParentView())->GetAltFlag())
		{
			CString szLabelOffer = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMPTAGE_OFFER);
			CString szLabelOccup = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMPTAGE_OCCUP);
			CString szLabel;
			szLabel = szLabelOffer + " " + szHisto1;
			SetNthSetLabel(GetParentView()->eCmptComptageHist1Offer, (LPSTR)(LPCSTR)szLabel);		
			szLabel = szLabelOccup + " " + szHisto1;
			SetNthSetLabel(GetParentView()->eCmptComptageHist1Occup, (LPSTR)(LPCSTR)szLabel);
			szLabel = szLabelOffer + " " + szHisto2;
			SetNthSetLabel(GetParentView()->eCmptComptageHist2Offer, (LPSTR)(LPCSTR)szLabel);		
			szLabel = szLabelOccup + " " + szHisto2;
			SetNthSetLabel(GetParentView()->eCmptComptageHist2Occup, (LPSTR)(LPCSTR)szLabel);
			szLabel = szLabelOffer + " " + szHisto3;
			SetNthSetLabel(GetParentView()->eCmptComptageHist3Offer, (LPSTR)(LPCSTR)szLabel);		
			szLabel = szLabelOccup + " " + szHisto3;
			SetNthSetLabel(GetParentView()->eCmptComptageHist3Occup, (LPSTR)(LPCSTR)szLabel);
		}
	}
}

CCTAChartHelpDlg* CRRDHistChart::CreateHelpDlg()
{
	m_pHelpDlg = new CRRDHistHelp( this );
	m_pHelpDlg->Create( this );
	((CMainFrame*) AfxGetApp()->GetMainWnd())->SetChartHelpDlg(m_pHelpDlg);
	//  ((CMainFrame*) AfxGetApp()->GetMainWnd())->SetHelpDlg(m_pHelpDlg);
	return (m_pHelpDlg);
}

void CRRDHistChart::ContextualDisplay(CPoint point, BOOL bInRange)
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
				((CRRDHistView*)GetParentView())->RevMasqueCourbe (CCTAChartData::eBkt, pick.set, FALSE);
				bFound = TRUE;
			}
			else if ( Pick ( XRT_DATASET2, point.x, point.y, &pick, XRT_XFOCUS|XRT_YFOCUS ) == XRT_RGN_IN_GRAPH && 
				pick.distance <= FRONTBACKDIST)
			{ // dataset2 : ecart-type.
				((CRRDHistView*)GetParentView())->RevMasqueCourbe (CCTAChartData::eBkt, pick.set, TRUE);
				bFound = TRUE;
			}
		}
		else  // Compartiment
		{  
			if ( Pick ( XRT_DATASET1, point.x, point.y, &pick, XRT_XFOCUS|XRT_YFOCUS ) == XRT_RGN_IN_GRAPH && 
				pick.distance <= FRONTBACKDIST)
			{ // dataset1
				((CRRDHistView*)GetParentView())->RevMasqueCourbe (CCTAChartData::eCmpt, pick.set, FALSE);
				bFound = TRUE;
			}
			else if ( Pick ( XRT_DATASET2, point.x, point.y, &pick, XRT_XFOCUS|XRT_YFOCUS ) == XRT_RGN_IN_GRAPH && 
				pick.distance <= FRONTBACKDIST)
			{ // dataset2 : ecart-type.
				((CRRDHistView*)GetParentView())->RevMasqueCourbe (CCTAChartData::eCmpt, pick.set, TRUE);
				bFound = TRUE;
			}	  
		}
		if (bFound)
			((CRRDHistView*)GetParentView())->ChangeRRDDisplay(FALSE);
		return;
	}
	return;
}

///////////////////////////////////////////////////////
// Get the Reading Day from the RRD Index
//
BOOL CRRDHistView::GetRRD( long  lRrdIndex, CString * szRRD )
{
	szRRD->Format( "%+d", -lRrdIndex );  // format it for the X axis label
	return true;
}

///////////////////////////////////////////////////////
// Build the data objects for the bucket graphs
//
void CRRDHistView::BuildBktGraphs()
{
	// Si les donnees bkt et moyenne bkt sont pretes 
	if ( !m_bBktDataValid || !m_bBktAvgDataValid ||
		( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() && !m_bCmptDataValid) ||
		( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && !m_bHistoDataValid ) ||
		( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() && !m_bPrevisionValid) )
		return;
	// clean up the previous data structure(s)
	ClearBktGraphs();

	CObArray DataArray; // tableau de CDataGraph, un par reading day possible.
	DataArray.SetSize (GetDocument()->GetMaxRrdIndex()+2); // attention les J_X commence à -1. (MaxRrdIndex = 150)

	// initialisation du tableau de donnees
	int i;
	for (i = 0; i < DataArray.GetSize(); i++)
		DataArray[i] = NULL;

	// Define the iterator to iterate through compartments/buckets
	YM_RecordSet* pBktEvolSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_ALT_KEY : BKTEVOL_KEY);
	YM_Iterator<CRRDHistBktEvol> *pBktIdxIterator = ((CRRDHistBktEvolSet*) pBktEvolSet)->CreateKeyIterator();
	YM_RecordSet* pBktEvolAvgSet = YM_Set<YmIcBktEvolDom>::FromKey((GetAltFlag()) ? BKTEVOL_AVG_ALT_KEY : BKTEVOL_AVG_KEY);

	// set text in status bar control
	((CChildFrame*)GetParentFrame())->
		GetStatusBar()->SetPaneOneAndLock(
		((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BUILDING_GRAPH) );

	CStringArray aszXLabels;  // array to hold the X-axis labels (to be put into the TabToData object)
	YmIcTrancheLisDom* pTranche =  // Juste pour récupérer le last_rrd_index de la tranche
		YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);

	// Boucle sur tous les buckets
	for( pBktIdxIterator->First(); !pBktIdxIterator->Finished(); pBktIdxIterator->Next() )
	{
		CRRDHistBktEvol* pRrdHistBktEvol = pBktIdxIterator->Current();
		// Define the iterator to iterate through records for this compartment
		YM_Iterator<CRRDHistBktEvol> *pBktRowIterator =
			((CRRDHistBktEvolSet*) pBktEvolSet)->CreateIterator( pRrdHistBktEvol->Key() );

		// create a new cta graph data object
		CCTAChartData* pCTAChartData = new CCTAChartData( &m_BktChart, CCTAChartData::eBkt );

		int iBktCurrentRrdIndex = -1;  // reset current RRD index for the bucket level
		int iBktNextRrdIndex = -1;  // pour affichage resa temps reel
		BOOL b_realTimeAdded  = FALSE;
		int minRrdIndex = 300;
		char cmpt;
		long nestlevel;

		// Boucle sur tous les j-x du bucket concerne
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
				CDataGraph* pDataGraph = new CDataGraph();
				pDataGraph->pBktEvol = pBktEvol;
				pDataGraph->rrd_index = pBktEvol->RrdIndex();
				DataArray.SetAt (pBktEvol->RrdIndex()+1, (CObject*)pDataGraph);
				if (pBktEvol->RrdIndex() < minRrdIndex)
					minRrdIndex = pBktEvol->RrdIndex();

				// Deux premieres lignes ne servent a rien ?! SetGraphMax ecrase au troisieme appel...
				pCTAChartData->SetGraphMax( (double)pBktEvol->AuthCur() );  // is this the max graph value ?
				pCTAChartData->SetGraphMax( (double)(pBktEvol->ResHoldIdv()+ pBktEvol->ResHoldGrp()) );  // is this the max graph value ?
				pCTAChartData->SetGraphMax( (double)((pBktEvol->ResHoldIdv()+ pBktEvol->ResHoldGrp()) + pBktEvol->Availability()) );  // is this the max graph value ?
				pCTAChartData->SetResHoldMax( (double)(pBktEvol->ResHoldIdv()+ pBktEvol->ResHoldGrp()) );  // is this the max res hold value ?
				pCTAChartData->SetAuthMax( (double)pBktEvol->AuthCur() );
				pCTAChartData->setScxEtanche(pBktEvol->ScxEtanche());//DT35037 - Lyria lot 3

				//if ( pTranche->LastJX() == pBktEvol->RrdIndex() )
				iBktCurrentRrdIndex = pBktEvol->RrdIndex(); // Contient le dernier rrd avec données de montée en charge resa.
			}
			else
			{
				std::cout << "Erreur !" << std::endl;
			}
		}
		delete pBktRowIterator;

		YM_Iterator<CRRDHistBktEvol> *pBktAvgRowIterator =
			((CRRDHistBktEvolSet*) pBktEvolAvgSet)->CreateIterator( pRrdHistBktEvol->Key() );
		for( pBktAvgRowIterator->First(); !pBktAvgRowIterator->Finished(); pBktAvgRowIterator->Next() ) 
		{
			YmIcBktEvolDom* pBktEvol = pBktAvgRowIterator->Current();
			CDataGraph* pDataGraph = (CDataGraph*)DataArray.GetAt(pBktEvol->RrdIndex()+1);
			if (!pDataGraph)
			{
				pDataGraph = new CDataGraph();
				pDataGraph->pBktEvol = pBktEvol;
				pDataGraph->rrd_index = pBktEvol->RrdIndex();
				DataArray.SetAt (pBktEvol->RrdIndex()+1, (CObject*)pDataGraph);
			}
			pDataGraph->pBktEvol->ResHoldAvg(pBktEvol->ResHoldAvg());
			pDataGraph->pBktEvol->ResHoldStdv(pBktEvol->ResHoldStdv());
			if (pBktEvol->RrdIndex() < minRrdIndex)
				minRrdIndex = pBktEvol->RrdIndex();
			if ((pBktEvol->RrdIndex() < iBktCurrentRrdIndex) && (iBktNextRrdIndex == -1))
				iBktNextRrdIndex = pBktEvol->RrdIndex();

			pCTAChartData->SetGraphMax( (double)(pBktEvol->ResHoldAvg() + pBktEvol->ResHoldStdv()) );  // is this the max graph value ?
			pCTAChartData->SetResHoldMax( (double)(pBktEvol->ResHoldAvg() + pBktEvol->ResHoldStdv()) );  // is this the max res hold value ?
		}
		delete pBktAvgRowIterator;

		if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRealTimeInRRDHistFlag())
		{ // DM 5350, ajout du temps reel dans la fenêtre montée en charge resa
			if ((iBktCurrentRrdIndex > 0) && (iBktCurrentRrdIndex > minRrdIndex))
			{
				int RealResaCmpt = GetRealResaCmpt(cmpt, nestlevel );
				if (RealResaCmpt >= 0)
				{
					CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(iBktNextRrdIndex+1); // avant iBktCurrentRrdIndex-1, mais +1 pour décalage du tableau
					if (!ppData)
					{
						ppData = new CDataGraph();
						ppData->rrd_index = iBktNextRrdIndex;
						DataArray.SetAt (ppData->rrd_index +1, (CObject*) ppData);
					}
					ppData->ResaTempsReel = (double)RealResaCmpt;
					b_realTimeAdded = TRUE;
				}
			}
		}

		if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && mp_HistoData)
		{
			double hAuth, hResa;
			int mamax = __min (DataArray.GetSize()-1, mp_HistoData->m_max_rrd);
			for (i = mp_HistoData->m_min_rrd; i <= mamax; i++)
			{
				if (mp_HistoData->IsRrd(i))
				{
					CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i+1);
					if (!ppData)
					{
						ppData = new CDataGraph();
						ppData->rrd_index = i;
						DataArray.SetAt (i+1, (CObject*) ppData);
					}
					if (mp_HistoData->GetNbDate() >= 1)
					{
						hAuth = ppData->AuthHist1 = mp_HistoData->GetAuth (1, cmpt, i, nestlevel);
						hResa = ppData->ResaHist1 = mp_HistoData->GetResa (1, cmpt, i, nestlevel);
						if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
							ppData->HoiTra1 = mp_HistoData->GetHoiTra (1, cmpt, i);
						if (hAuth != INVALID_GRAPH_VALUE)
						{
							pCTAChartData->SetGraphMax( hAuth );
							pCTAChartData->SetAuthMax( hAuth );
						}
						if (hResa != INVALID_GRAPH_VALUE)
						{
							pCTAChartData->SetGraphMax( hResa );
							pCTAChartData->SetResHoldMax( hResa );
						}
					}
					if (mp_HistoData->GetNbDate() >= 2)
					{
						hAuth = ppData->AuthHist2 = mp_HistoData->GetAuth (2, cmpt, i, nestlevel);
						hResa = ppData->ResaHist2 = mp_HistoData->GetResa (2, cmpt, i, nestlevel);
						if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
							ppData->HoiTra2 = mp_HistoData->GetHoiTra (2, cmpt, i);
						if (hAuth != INVALID_GRAPH_VALUE)
						{
							pCTAChartData->SetGraphMax( hAuth );
							pCTAChartData->SetAuthMax( hAuth );
						}
						if (hResa != INVALID_GRAPH_VALUE)
						{
							pCTAChartData->SetGraphMax( hResa );
							pCTAChartData->SetResHoldMax( hResa );
						}
					}
					if (mp_HistoData->GetNbDate() >= 3)
					{
						hAuth = ppData->AuthHist3 = mp_HistoData->GetAuth (3, cmpt, i, nestlevel);
						hResa = ppData->ResaHist3 = mp_HistoData->GetResa (3, cmpt, i, nestlevel);
						if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
							ppData->HoiTra3 = mp_HistoData->GetHoiTra (3, cmpt, i);
						if (hAuth != INVALID_GRAPH_VALUE)
						{
							pCTAChartData->SetGraphMax( hAuth );
							pCTAChartData->SetAuthMax( hAuth );
						}
						if (hResa != INVALID_GRAPH_VALUE)
						{
							pCTAChartData->SetGraphMax( hResa );
							pCTAChartData->SetResHoldMax( hResa );
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
					CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(pResaBkt->RrdIndex()+1);
					if (!ppData)
					{
						ppData = new CDataGraph();
						ppData->rrd_index = pResaBkt->RrdIndex();
						DataArray.SetAt (pResaBkt->RrdIndex()+1, (CObject*) ppData);
					}
					ppData->AddPrevision (pResaBkt->ResHoldInd());
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

		// Retrouver l'indice du dernier point pour les montées en charge de la tranche
		int lastCmptIndex = 0;
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				if (ppData->rrd_index == iBktCurrentRrdIndex)
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

		// remove the points which do not exist from current rrd to rrd = 0
		pChartData->SetFirstPoint(eBktAuthCur, 0);
		pChartData->SetLastPoint(eBktAuthCur, lastCmptIndex);
		pChartData->SetFirstPoint(eBktResHoldIdv, 0);
		pChartData->SetLastPoint(eBktResHoldIdv, lastCmptIndex);
		pChartData->SetFirstPoint(eBktResHoldTot, 0);
		pChartData->SetLastPoint(eBktResHoldTot, b_realTimeAdded ? lastCmptIndex+1 : lastCmptIndex);
		pChartData->SetFirstPoint(eBktResHoldAvailSum, 0);
		pChartData->SetLastPoint(eBktResHoldAvailSum, lastCmptIndex);

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
				pChartData->SetLastPoint(eBktHisto3AuthCur, -1);
				pChartData->SetLastPoint(eBktHisto3ResHoldTot, -1);
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
					pChartData->SetLastPoint(eBktHisto3HoiTra, -1);
			}
			if (nbd < 2)
			{
				pChartData->SetLastPoint(eBktHisto2AuthCur, -1);
				pChartData->SetLastPoint(eBktHisto2ResHoldTot, -1);
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
					pChartData->SetLastPoint(eBktHisto2HoiTra, -1);
			}
			if (nbd < 1)
			{
				pChartData->SetLastPoint(eBktHisto1AuthCur, -1);
				pChartData->SetLastPoint(eBktHisto1ResHoldTot, -1);
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag())
					pChartData->SetLastPoint(eBktHisto1HoiTra, -1);
			}
			if (nbd >= 3)
			{
				if (!m_bDispHisto3AuthCur)
					pChartData->SetDisplay(eBktHisto3AuthCur, XRT_DISPLAY_HIDE);
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
				if (!m_bDispHisto2AuthCur)
					pChartData->SetDisplay(eBktHisto2AuthCur, XRT_DISPLAY_HIDE);
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
				if (!m_bDispHisto1AuthCur)
					pChartData->SetDisplay(eBktHisto1AuthCur, XRT_DISPLAY_HIDE);
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
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				GetRRD( ppData->rrd_index, &szBuffer );
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

		//courbe eBktAuthCur
		j=0;
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				if (ppData->pBktEvol)
					adYData[j] = (double)ppData->pBktEvol->AuthCur();
				j++;
			}
		}
		pChartData->SetYData (eBktAuthCur, adYData, lNumPts, 0);

		j=0;
		memset(adYData, '\0', sizeof(double) * lNumPts);
		//Courbe eBktResHoldTot
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData )
			{
				if (ppData->pBktEvol)
					adYData[j] = ( (! ppData->pBktEvol->ResHoldIdvIsNull()) && (! ppData->pBktEvol->ResHoldGrpIsNull()) ) ?
					(double)(ppData->pBktEvol->ResHoldIdv()+ppData->pBktEvol->ResHoldGrp()) : INVALID_GRAPH_VALUE;
				if (ppData->ResaTempsReel != INVALID_GRAPH_VALUE)
					adYData[j] = ppData->ResaTempsReel;
				j++;
			}
		}
		pChartData->SetYData (eBktResHoldTot, adYData, lNumPts, 0);

		j=0;
		memset(adYData, '\0', sizeof(double) * lNumPts);
		//Courbe eBktResHoldIdv
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
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
		//Courbe eBktResHoldAvailSum
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData )
			{
				if (ppData->pBktEvol)
					adYData[j] = ( (! ppData->pBktEvol->ResHoldIdvIsNull()) && (! ppData->pBktEvol->ResHoldGrpIsNull()) ) ?
					(double)( (ppData->pBktEvol->ResHoldIdv() + ppData->pBktEvol->ResHoldGrp()) + ppData->pBktEvol->Availability() )
					: INVALID_GRAPH_VALUE;
				j++;
			}
		}
		pChartData->SetYData (eBktResHoldAvailSum, adYData, lNumPts, 0);

		j=0;
		memset(adYData, '\0', sizeof(double) * lNumPts);
		//Courbe eBktResHoldAvg
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData )
			{
				if (ppData->pBktEvol)
					adYData[j] = (! ppData->pBktEvol->ResHoldAvgIsNull()) ?
					(double)ppData->pBktEvol->ResHoldAvg() : INVALID_GRAPH_VALUE;
				j++;
			}
		}
		pChartData->SetYData (eBktResHoldAvg, adYData, lNumPts, 0);

		j=0;
		memset(adYData, '\0', sizeof(double) * lNumPts);
		//Courbe eBktResHoldStdvHigh
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				if (ppData->pBktEvol)
					adYData[j] = ( (! ppData->pBktEvol->ResHoldAvgIsNull()) && (! ppData->pBktEvol->ResHoldStdvIsNull()) ) ?
					(double)(ppData->pBktEvol->ResHoldAvg() + ppData->pBktEvol->ResHoldStdv()) : INVALID_GRAPH_VALUE;
				j++;
			}
		}
		pChartData2->SetYData (eBktResHoldStdvHigh, adYData, lNumPts, 0);

		j=0;
		memset(adYData, '\0', sizeof(double) * lNumPts);
		//Courbe eBktResHoldStdvLow
		for (i = DataArray.GetSize()-1; i>= 0; i--)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData)
			{
				if (ppData->pBktEvol)
					adYData[j] = ( (! ppData->pBktEvol->ResHoldAvgIsNull()) && (! ppData->pBktEvol->ResHoldStdvIsNull()) ) ?
					(double)( __max((ppData->pBktEvol->ResHoldAvg() - ppData->pBktEvol->ResHoldStdv()),0) ) : INVALID_GRAPH_VALUE;
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
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData )
				{
					if (ppData->pBktEvol)
						adYData[j] = ( (! ppData->pBktEvol->ResHoldIdvIsNull()) &&
						(! ppData->pBktEvol->ResHoldGrpIsNull()) &&
						( ppData->pBktEvol->RrdIndex() <= debjxHoiTra ) &&
						( ppData->pBktEvol->RrdIndex() >= finjxHoiTra )) ?
						(double)(ppData->pBktEvol->ResHoldIdv()+ppData->pBktEvol->ResHoldGrp()) : INVALID_GRAPH_VALUE;
					j++;
				}
			}
			pChartData->SetYData (eBktHoiTra, adYData, lNumPts, 0);

			//idem sur les résa individuelles
			j = 0;
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData )
				{
					if (ppData->pBktEvol)
						adYData[j] = ( (! ppData->pBktEvol->ResHoldIdvIsNull()) &&
						( ppData->pBktEvol->RrdIndex() <= debjxHoiTra ) &&
						( ppData->pBktEvol->RrdIndex() >= finjxHoiTra )) ?
						(double)(ppData->pBktEvol->ResHoldIdv()) : INVALID_GRAPH_VALUE;
					j++;
				}
			}
			pChartData->SetYData (eBktHoiTraIdv, adYData, lNumPts, 0);
		}

		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() && mp_HistoData)
		{
			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eBktHisto1AuthCur
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->AuthHist1;
					j++;
				}
			}
			pChartData->SetYData (eBktHisto1AuthCur, adYData, lNumPts, 0);

			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eBktHisto1ResHoldTot
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->ResaHist1;
					j++;
				}
			}
			pChartData->SetYData (eBktHisto1ResHoldTot, adYData, lNumPts, 0);

			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eBktHisto2AuthCur
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->AuthHist2;
					j++;
				}
			}
			pChartData->SetYData (eBktHisto2AuthCur, adYData, lNumPts, 0);

			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eBktHisto2ResHoldTot
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->ResaHist2;
					j++;
				}
			}
			pChartData->SetYData (eBktHisto2ResHoldTot, adYData, lNumPts, 0);

			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eBktHisto3AuthCur
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->AuthHist3;
					j++;
				}
			}
			pChartData->SetYData (eBktHisto3AuthCur, adYData, lNumPts, 0);

			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			//Courbe eBktHisto3ResHoldTot
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->ResaHist3;
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
					CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
					if (ppData)
					{
						adYData[j] = ppData->HoiTra1 ? ppData->ResaHist1 : INVALID_GRAPH_VALUE;
						j++;
					}
				}
				pChartData->SetYData (eBktHisto1HoiTra, adYData, lNumPts, 0);

				j=0;
				memset(adYData, '\0', sizeof(double) * lNumPts);
				//Courbe eBktHisto2HoiTra
				for (i = DataArray.GetSize()-1; i>= 0; i--)
				{
					CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
					if (ppData)
					{
						adYData[j] = ppData->HoiTra2 ? ppData->ResaHist2 : INVALID_GRAPH_VALUE;
						j++;
					}
				}
				pChartData->SetYData (eBktHisto2HoiTra, adYData, lNumPts, 0);

				j=0;
				memset(adYData, '\0', sizeof(double) * lNumPts);
				//Courbe eBktHisto3HoiTra
				for (i = DataArray.GetSize()-1; i>= 0; i--)
				{
					CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
					if (ppData)
					{
						adYData[j] = ppData->HoiTra3 ? ppData->ResaHist3 : INVALID_GRAPH_VALUE;
						j++;
					}
				}
				pChartData->SetYData (eBktHisto3HoiTra, adYData, lNumPts, 0);
			}
		}


		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
		{
			j=0;
			memset(adYData, '\0', sizeof(double) * lNumPts);
			for (i = DataArray.GetSize()-1; i>= 0; i--)
			{
				CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
				if (ppData)
				{
					adYData[j] = ppData->GetPrevision();
					j++;
				}
			}
			pChartData->SetYData (eBktPrevision, adYData, lNumPts, 0);
		}

		// Masquage autre courbe si demandé
		// DM4965.3, a ce niveau le pRrdHistBktEvol donne CMPT et nestlevel, on peut donc
		// masqué les courbes par classes de contrôle.
		if (!m_bDispEC)
			pChartData2->SetDisplay(eBktResHoldStdvHigh, XRT_DISPLAY_HIDE);
		if (! m_bDispResHoldAvg)
			pChartData->SetDisplay(eBktResHoldAvg, XRT_DISPLAY_HIDE);
		if (! m_bDispAuthCur)
			pChartData->SetDisplay(eBktAuthCur, XRT_DISPLAY_HIDE);
		if (! m_bDispResHoldAvailSum)
			pChartData->SetDisplay(eBktResHoldAvailSum, XRT_DISPLAY_HIDE);
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
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
		{
			if (! m_bDispPrevision)
				pChartData->SetDisplay(eBktPrevision, XRT_DISPLAY_HIDE);
		}

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
		pCTAChartData->SetBktId(pBktIdxIterator->Current()->BucketId());
		pCTAChartData->SetNestLevel(pBktIdxIterator->Current()->NestLevel());
		m_BktChart.AddChartData( pCTAChartData );  // add this CTA Chart Data element to the array

		// Nettoyage du DataArray avant de lire le deuxième compartiment
		for (i = 0; i < DataArray.GetSize(); i++)
		{
			CDataGraph* ppData = (CDataGraph*)DataArray.GetAt(i);
			if (ppData)
				delete ppData;
			DataArray[i] = NULL;
		}
	}
	delete pBktIdxIterator;  // delete the index iterator

	m_BktChart.SetGraphFont();
}

//////////////////////////////////////////////////////////
// Clear the tabs array and removes the corresponding tab
//
void CRRDHistView::ClearTabs(enum CCTAChartData::GraphType eGraphType)
{
	// clean up the array containing the tabs and graph data objects
	m_ChartTabCtrl.DeleteAllItems( ); // remove the tab from the control
}

//////////////////////////////////////
// Build the tabs on the tab control
//
void CRRDHistView::BuildTabs()
{
	int nLastTab = -1;

	//AfxMessageBox(IDS_SQL_SELECT_RRD_HIST_CMPT);

	CString szWndTitle = "";
	GetDayTitle(szWndTitle);
	GetParentFrame()->SetWindowText(szWndTitle);
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
		BOOL bModeSc = FALSE;
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
				bModeSc = (szBuffer == "a" || szBuffer == "b");
			}
			else  // build this tab from the bucket (if we have some more)
			{
				if (bMoreBkts)	//DM 8029 - For nature 3 (SNCF & ALLEO) only display SCGs
				{									//YI-11625 & YI-11620
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
					{
						szBuffer.Format( "%s%s%s", pBktChartData->GetCmpt(), pBktChartData->GetBktLabel(), pBktChartData->getScxEtanche() ? "(E)" : "" );//DT35037 - Lyria lot 3
					}
					iBktIdx++;
					TabCtrlItem.lParam = (LPARAM)pBktChartData;
				}
			}

			if ( ((bMoreCmpts) || (bMoreBkts)) && TabCtrlItem.lParam )
			{
				TabCtrlItem.pszText = (LPSTR)(LPCSTR)szBuffer;
				if (_sLastTabUsed == szBuffer)
					nLastTab = iTabIndex;
				// OHA : ano 76055
				//if (!((CCTAApp*)APP)->GetRhealysFlag() || (szBuffer.GetLength() == 1) || ((szBuffer.Right(1) == "0") && (szBuffer.GetLength() == 2)))
				//	m_ChartTabCtrl.InsertItem( iTabIndex++, &TabCtrlItem );
				
				// NBN - DT ALLEO YI-11625 & YI-11620
				if(!((CCTAApp*)APP)->GetRhealysFlag()){
					// SNCF
					if(!bModeSc || (this->nature != 3 && bModeSc) || (bModeSc && this->nature == 3 && szBuffer.GetLength() == 1)){
						m_ChartTabCtrl.InsertItem( iTabIndex++, &TabCtrlItem );
					}
				}
				else{
					// ALLEO
					if((!bModeSc && ((szBuffer.GetLength() == 1) || ((szBuffer.Right(1) == "0") && (szBuffer.GetLength() == 2)))) || (this->nature != 3 && bModeSc) || (bModeSc && this->nature == 3 && szBuffer.GetLength() == 1)){
						m_ChartTabCtrl.InsertItem( iTabIndex++, &TabCtrlItem );
					}
				}
				
				
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
				// DM7978 - LME - YI-4625 - on augmente un peu le coefficient de reduction pour que les onglets s'affichent correctement quand CC > 9
				double dTabReduction = TAB_RED_COEFF; // originalement a 0.25
				// DM7978 - LME - FIN
				BOOL b_ok = m_ChartTabCtrl.GetItemRect( 0, &TabRect );
				TabSize.cx = long( (TabRect.right - TabRect.left) * dTabReduction );  // resize the tabs to 1/3 of their width
				TabSize.cy = TabRect.bottom - TabRect.top;
				CSize size = m_ChartTabCtrl.SetItemSize( TabSize );
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
			if ( pCTAChartData->GetGraphType() == CCTAChartData::eCmpt )
			{
				m_pCurSelChart = &m_CmptChart;
				m_pNotSelChart = &m_BktChart;
			}
			else
			{
				m_pCurSelChart = &m_BktChart;
				m_pNotSelChart = &m_CmptChart;
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
			if (sb == SDISPLAYALL)
				idres = IDS_FULL_DISPLAY;
			else if (sb == SDISPLAYMEC)
				idres = IDS_MEC_DISPLAY;
			else if (sb == SDISPLAYECH)
				idres = IDS_HEC_DISPLAY;
			else if (sb == SDISPLAYHIS)
				idres = IDS_HIS_DISPLAY;
			else
				idres = IDS_PERSONAL;
			((CChildFrame*)GetParentFrame())->
				GetStatusBar()->SetPaneOneAndLock( ((CCTAApp*)APP)->GetResource()->LoadResString(idres) );

			if ((sb == SDISPLAYALL) && (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() ))
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
void CRRDHistView::UpdateHelpDialog()
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

CString CRRDHistView::GetBoolSet()
{
	char cBoolSet[25];
	cBoolSet[0] = m_bDispEC ? '1' : '0';
	cBoolSet[1] = m_bDispCapa ? '1' : '0';
	cBoolSet[2] = m_bDispAuthCur ? '1' : '0';
	cBoolSet[3] = m_bDispResHoldAvailSum ? '1' : '0';
	cBoolSet[4] = m_bDispResHoldTot ? '1' : '0';
	cBoolSet[5] = m_bDispResHoldIdv ? '1' : '0';
	cBoolSet[6] = m_bDispResHoldAvg ? '1' : '0';
	cBoolSet[7] = m_bDispHisto1AuthCur ? '1' : '0';
	cBoolSet[8] = m_bDispHisto1ResHoldTot ? '1' : '0';
	cBoolSet[9] = m_bDispHisto2AuthCur ? '1' : '0';
	cBoolSet[10] = m_bDispHisto2ResHoldTot ? '1' : '0';
	cBoolSet[11] = m_bDispHisto3AuthCur ? '1' : '0';
	cBoolSet[12] = m_bDispHisto3ResHoldTot ? '1' : '0';
	cBoolSet[13] = m_bDispComptageOfferStvd ? '1' : '0';
	cBoolSet[14] = m_bDispComptageOffer ? '1' : '0';
	cBoolSet[15] = m_bDispComptageOccupStvd ? '1' : '0';
	cBoolSet[16] = m_bDispComptageOccup ? '1' : '0';
	cBoolSet[17] = m_bDispHisto1ComptageOffer ? '1' : '0';
	cBoolSet[18] = m_bDispHisto1ComptageOccup ? '1' : '0';
	cBoolSet[19] = m_bDispHisto2ComptageOffer ? '1' : '0';
	cBoolSet[20] = m_bDispHisto2ComptageOccup ? '1' : '0';
	cBoolSet[21] = m_bDispHisto3ComptageOffer ? '1' : '0';
	cBoolSet[22] = m_bDispHisto3ComptageOccup ? '1' : '0';
	cBoolSet[23] = m_bDispPrevision ? '1' : '0';
	cBoolSet[24] = 0;

	CString s(cBoolSet);
	return s;
}

void CRRDHistView::SetBoolSet (CString sBoolSet)
{
	m_bDispEC = (sBoolSet[0] == '1');
	m_bDispCapa = (sBoolSet[1] == '1'); 
	m_bDispAuthCur = (sBoolSet[2] == '1');
	m_bDispResHoldAvailSum = (sBoolSet[3] == '1');
	m_bDispResHoldTot = (sBoolSet[4] == '1'); 
	m_bDispResHoldIdv = (sBoolSet[5] == '1');
	m_bDispResHoldAvg = (sBoolSet[6] == '1');
	m_bDispHisto1AuthCur = (sBoolSet[7] == '1');
	m_bDispHisto1ResHoldTot = (sBoolSet[8] == '1');
	m_bDispHisto2AuthCur = (sBoolSet[9] == '1');
	m_bDispHisto2ResHoldTot = (sBoolSet[10] == '1');
	m_bDispHisto3AuthCur = (sBoolSet[11] == '1');
	m_bDispHisto3ResHoldTot = (sBoolSet[12] == '1');
	m_bDispComptageOfferStvd = (sBoolSet[13] == '1');
	m_bDispComptageOffer = (sBoolSet[14] == '1');
	m_bDispComptageOccupStvd = (sBoolSet[15] == '1');
	m_bDispComptageOccup = (sBoolSet[16] == '1');
	m_bDispHisto1ComptageOffer = (sBoolSet[17] == '1');
	m_bDispHisto1ComptageOccup = (sBoolSet[18] == '1');
	m_bDispHisto2ComptageOffer = (sBoolSet[19] == '1');
	m_bDispHisto2ComptageOccup = (sBoolSet[20] == '1');
	m_bDispHisto3ComptageOffer = (sBoolSet[21] == '1');
	m_bDispHisto3ComptageOccup = (sBoolSet[22] == '1');
	m_bDispPrevision = (sBoolSet[23] == '1'); 
}

//////////////////////////////////////
// Write specific view configuration information to the registry
//
void CRRDHistView::WriteViewConfig()
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
		CRRDHistView* view = getOtherView();
		if (view) {
			APP->WriteProfileString( szProfileKey, "LastTabUsed_2", view->_sLastTabUsed );
		}
	}
}

//////////////////////////////////////
// Restore specific view configuration information from the registry
//
void CRRDHistView::RestoreViewConfig()
{
	// restore the graph flags from the registry
	//
	CString szProfileKey;
	((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

	enum CCTAChart::GraphScaleIndex eCurScale = (enum CCTAChart::GraphScaleIndex)
		APP->GetProfileInt(szProfileKey, GRAPH_SCALE_FACTOR, (unsigned int)CCTAChart::eMaxScaleIdx);
	CString sbs = APP->GetProfileString(szProfileKey, "DisplayCurve", SDISPLAYALL);
	if (sbs.GetLength() < 24)
		sbs = SDISPLAYALL;
	SetBoolSet (sbs);
	((CCTAApp*)APP)->m_RRDBoolSet = sbs;

	CSplitChildFrame* pWnd = (CSplitChildFrame*)GetParentFrame();
	pWnd->RestoreSplitterConfig(szProfileKey);

	int rotated = APP->GetProfileInt(szProfileKey, ROTATE_X_LABEL, 0);
	m_CmptChart.SetRotateLabelsFlag( !rotated );
	//if (rotated)	  // Par default ce n'est pas tourné.
	OnGraphRotateLabels();
	switch (eCurScale)
	{
	case CCTAChart::eMaxScaleIdx : OnScaleMax(); break;
	case CCTAChart::eAuthScaleIdx : OnScaleAuth(); break;
	case CCTAChart::eResHoldScaleIdx : OnScaleResHold(); break;
	}

	_sLastTabUsed = APP->GetProfileString(szProfileKey, "LastTabUsed", "" );
}

/////////////////////////////////////////////////////////////////////////////
// CRRDHistChart

BEGIN_MESSAGE_MAP(CRRDHistChart, CCTAChart)
	//{{AFX_MSG_MAP(CRRDHistChart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////
// constructor
//
CRRDHistChart::CRRDHistChart()
{
	m_bRotateLabels = ((CCTAApp*)APP)->m_bRRDRotatelabel;
	this->m_PrevisionLabel = "";
	/*m_bRotateLabels == true ? 
	SetXAnnotationRotation(XRT_ROTATE_90) : 
	SetXAnnotationRotation(XRT_ROTATE_NONE); */
}

/////////////////////////////////////////////////////////////////////////////
// CRRDHistHelp dialog

CRRDHistHelp::CRRDHistHelp(CRRDHistChart* pParent /*=NULL*/)
: CCTAChartHelpDlg(pParent)
{
		this->PrevisionDispo = 2;
		this->needRedraw = 0;
}

void CRRDHistHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRRDHistHelp)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRRDHistHelp, CCTAChartHelpDlg)
	//{{AFX_MSG_MAP(CRRDHistHelp)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// build the 1st title
void CRRDHistHelp::GetTitle1( CString& szTitle )
{
	// get the Cmpt of the selected data point for the 1st title
	CCTAChartData* pChartData = 
		((CRRDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
	if ( pChartData == NULL )
		return;

	CString szRRD = XrtGetNthPointLabel(m_pParent->m_hChart, m_iCurSelectPt);
	szTitle.Format("%s: %s", 
		((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RRD),
		szRRD);
}

// build the 2nd title
void CRRDHistHelp::GetTitle2( CString& szTitle )
{
	//  szTitle = " ";
	// get the Cmpt of the selected data point for the 1st title
	CCTAChartData* pChartData = 
		((CRRDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
	if ( pChartData == NULL )
		return;

	int iCurSelTab = 
		((CRRDHistView*)m_pParent->GetParentView())->GetTabCtrl()->GetCurSel();  // get currently selected tab

	// get the Compartment (text of the tab) for the title
	char szTabText[50];
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;
	TabCtrlItem.pszText = szTabText;
	TabCtrlItem.cchTextMax = sizeof(szTabText);
	((CRRDHistView*)m_pParent->GetParentView())->
		GetTabCtrl()->GetItem(iCurSelTab, &TabCtrlItem);

	CString szRRD = XrtGetNthPointLabel(m_pParent->m_hChart, m_iCurSelectPt);
	if (pChartData->GetGraphType() == CCTAChartData::eCmpt)
	{
		szTitle.Format("%s %s", 
			((CCTAApp*)APP)->GetResource()->LoadResString(((CRRDHistView*)m_pParent->GetParentView())->GetAltFlag() ? IDS_SOUS_CONT : IDS_CMPT),
			TabCtrlItem.pszText);
	}
	else
	{
		szTitle.Format("%s %s", 
			((CCTAApp*)APP)->GetResource()->LoadResString(((CRRDHistView*)m_pParent->GetParentView())->GetAltFlag() ?  IDS_SCI : IDS_BUCKET),
			TabCtrlItem.pszText);
	}
}

// Get the longest label to be printed.  This is used for sizing the width
//
CSize CRRDHistHelp::GetExtentLongestLabel( CPaintDC& dc )
{
	CSize sizeTextLabel(0, 0);

	// get the Cmpt of the selected data point for the 1st title
	CCTAChartData* pChartData = 
		((CRRDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
	if ( pChartData == NULL )
		return sizeTextLabel;

	int iBeg, iEnd, iBeg2, iEnd2;
	if (pChartData->GetGraphType() == CCTAChartData::eCmpt)
	{
		//if ( ! ((CRRDHistView*)m_pParent->GetParentView())->GetRevenueDisplay() )  // are we graphing auth or rev data ?
		//{
		iBeg = 0;
		iEnd = ((CRRDHistView*)m_pParent->GetParentView())->eCmptNumDataSets;
		iBeg2 = 0;
		iEnd2 = ((CRRDHistView*)m_pParent->GetParentView())->eCmptResHoldStdvLow;
		/*}
		else  // we are graphing revenue data...
		{
		iBeg = 0;
		iEnd = CRRDHistView::eRevGraphNumDataSets;
		iBeg2 = 0;
		iEnd2 = CRRDHistView::eRevGraphRangeNumDataSets;
		}
		*/
	}
	else  //  we are graphing bucket data...
	{
		iBeg = ((CRRDHistView*)m_pParent->GetParentView())->eBktAuthCur;
		iEnd = ((CRRDHistView*)m_pParent->GetParentView())->eBktNumDataSets;
		iBeg2 = ((CRRDHistView*)m_pParent->GetParentView())->eBktResHoldStdvHigh;
		iEnd2 = ((CRRDHistView*)m_pParent->GetParentView())->eBktResHoldStdvLow;
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
BOOL CRRDHistHelp::GetTextLabel( int iDataSet, CString& szTextLabel, int iCurSelectSet )
{
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	{ // grosse astuce, si  hoiTraFlag, les courbes correspondantes ont la même position (5) 
		// dans les deux dataset cmpt et bkt. mais je teste les deux pour faire plus joli.
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptHoiTra )
			return FALSE;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptHoiTraIdv )
			return FALSE;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eBktHoiTra )
			return FALSE;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eBktHoiTraIdv )
			return FALSE;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto1HoiTra )
			return FALSE;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto2HoiTra )
			return FALSE;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto3HoiTra )
			return FALSE;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eBktHisto1HoiTra )
			return FALSE;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eBktHisto2HoiTra )
			return FALSE;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eBktHisto3HoiTra )
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
CString CRRDHistHelp::GetValue( int iDataSet, int iCurSelectSet, int iCurSelectPt  )
{
	CString szValue;
	double dValue = 0;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
	{
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptHoiTra )
			return szValue;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptHoiTraIdv )
			return szValue;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eBktHoiTra )
			return szValue;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eBktHoiTraIdv )
			return szValue;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto1HoiTra )
			return szValue;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto2HoiTra )
			return szValue;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto3HoiTra )
			return szValue;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eBktHisto1HoiTra )
			return szValue;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eBktHisto2HoiTra )
			return szValue;
		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eBktHisto3HoiTra )
			return szValue;
	}

	if ( iDataSet == XRT_DATA )
	{
		//debug DM 7093
		/*
		int lp = XrtDataGetLastPoint( m_hXrtData, iCurSelectSet );
		int fp = XrtDataGetFirstPoint( m_hXrtData, iCurSelectSet );
		TRACE ("iCurSet=%d, fp=%d, lp=%d, iCur=%d\n", iCurSelectSet, fp, lp , iCurSelectSet );
		*/
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

		if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOfferStdvLow )
		{
			// here we force the "fourchette" to be drawn as a low to a high value
			double dValueLow = XrtDataGetYElement( m_hXrtData, ((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOfferStdvLow, m_iCurSelectPt );
			double dValueHigh = XrtDataGetYElement( m_hXrtData, ((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOfferStdvHigh, m_iCurSelectPt );
			if (dValueLow == INVALID_GRAPH_VALUE || dValueHigh == INVALID_GRAPH_VALUE)
				szValue = "N/A";
			else
				szValue.Format( "%3.0f - %3.0f", dValueLow, dValueHigh );
		}
		else if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOccupStdvLow )
		{
			// here we force the "fourchette" to be drawn as a low to a high value
			double dValueLow = XrtDataGetYElement( m_hXrtData, ((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOccupStdvLow, m_iCurSelectPt );
			double dValueHigh = XrtDataGetYElement( m_hXrtData, ((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOccupStdvHigh, m_iCurSelectPt );
			if (dValueLow == INVALID_GRAPH_VALUE || dValueHigh == INVALID_GRAPH_VALUE)
				szValue = "N/A";
			else
				szValue.Format( "%3.0f - %3.0f", dValueLow, dValueHigh );
		}
		else if ( ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOfferStdvHigh ) || 
			( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOccupStdvHigh ) )
		{
			szValue.Empty();  // indicate that we should not draw any value...
		}
		else
		{
			dValue = XrtDataGetYElement( m_hXrtData, iCurSelectSet, m_iCurSelectPt );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if ( iCurSelectSet == ((CRRDHistView*)m_pParent->GetParentView())->eCmptPrevision )
			{
				if (dValue == INVALID_GRAPH_VALUE || YM_Set<YmIcPrevisionDom>::FromKey(PREVISION_VENTE_KEY)->IsEmpty() || dValue == XRT_HUGE_VAL)
				{
					if (this->PrevisionDispo != 0)
					{
						this->PrevisionDispo = 0;
						needRedraw = 1;
					}
					(((CRRDHistView*)m_pParent->GetParentView()))->GetCmptChart()->SetNthSetLabel(((CRRDHistView*)m_pParent->GetParentView())->eCmptPrevision, "Prévision indisponible");
					(((CRRDHistView*)m_pParent->GetParentView()))->GetCmptChart()->SetPrevisionLabel("Prévision indisponible");
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

					if (xValue < ((CRRDHistView*)m_pParent->GetParentView())->dernierJX)//activation / désactivation prévision dans la légende en fonction du dernierJX collecté
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
					(((CRRDHistView*)m_pParent->GetParentView()))->GetCmptChart()->SetPrevisionLabel("Prévision");
					(((CRRDHistView*)m_pParent->GetParentView()))->GetCmptChart()->SetNthSetLabel(((CRRDHistView*)m_pParent->GetParentView())->eCmptPrevision, "Prévision");
					szValue.Format( "%3.0f", dValue );
				}
				//(((CRecDHistView*)m_pParent->GetParentView()))->GetCmptChart()->UpdateData(1);
				//(((CRecDHistView*)m_pParent->GetParentView()))->GetCmptChart()->UpdateWindow();
				
				//(((CRecDHistView*)m_pParent->GetParentView()))->GetCmptChart()->SetGraphProperties(CCTAChartData::eCmpt);
				if (needRedraw)
				{
					// NBN - ANO 106725
					this->UpdateWindow();
					//this->SetRedraw(1);
					//this->Invalidate(0);
				}
				//(((CRecDHistView*)m_pParent->GetParentView()))->Invalidate();
			}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			else if (dValue == INVALID_GRAPH_VALUE)
				szValue = "N/A";//Mettre à "" dans le cas de Prévision
			else
				szValue.Format( "%3.0f", dValue );//Prev == 16 !
		}
	}
	else  // iDataSet == XRT_DATA2
	{
		if ( iCurSelectSet == CRRDHistView::eBktResHoldStdvHigh )
		{
			// here we force the "fourchette" to be drawn as a low to a high value
			double dValueLow = XrtDataGetYElement( m_hXrtData2, CRRDHistView::eBktResHoldStdvLow, m_iCurSelectPt );
			double dValueHigh = XrtDataGetYElement( m_hXrtData2, CRRDHistView::eBktResHoldStdvHigh, m_iCurSelectPt );
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
int CRRDHistHelp::GetCheckBox( int iDataSet, int iCurSelectSet, int iCurSelectPt)
{
	if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetControlRRDCurveFlag() )
		return 0; // on reserve cette fonction qu'a la SNCF et Thalys

	int rtn = 0;

	// get the Cmpt of the selected data point for the 1st title
	CCTAChartData* pChartData = 
		((CRRDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
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

	CRRDHistView* ppapa = (CRRDHistView*)m_pParent->GetParentView();
	if ( iDataSet == XRT_DATA )
	{
		int rtnval = 10000 + iCurSelectSet;
		if (pChartData->GetGraphType() == CCTAChartData::eCmpt)
		{
			if ( iCurSelectSet == ppapa->eCmptCapacity)
				return (ppapa->m_bDispCapa ? rtnval : -rtnval);
			if ( iCurSelectSet == ppapa->eCmptOvbLevelCur)
				if (ppapa->GetAltFlag())
					return 0;
				else
					return (ppapa->m_bDispAuthCur ? rtnval : -rtnval);
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
				if ( iCurSelectSet == ppapa->eCmptHisto1OvbLevel)
					return (ppapa->m_bDispHisto1AuthCur ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eCmptHisto1ResHoldTot)
					return (ppapa->m_bDispHisto1ResHoldTot ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eCmptHisto2OvbLevel)
					return (ppapa->m_bDispHisto2AuthCur ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eCmptHisto2ResHoldTot)
					return (ppapa->m_bDispHisto2ResHoldTot ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eCmptHisto3OvbLevel)
					return (ppapa->m_bDispHisto3AuthCur ? rtnval : -rtnval);
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
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() && !ppapa->GetAltFlag()) 
			{
				if ( iCurSelectSet == ppapa->eCmptComptageOffer)
					return (ppapa->m_bDispComptageOffer ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eCmptComptageOfferStdvLow)
					return (ppapa->m_bDispComptageOfferStvd ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eCmptComptageOccup)
					return (ppapa->m_bDispComptageOccup ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eCmptComptageOccupStdvLow)
					return (ppapa->m_bDispComptageOccupStvd ? rtnval : -rtnval);
			}
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() && !ppapa->GetAltFlag())  
			{
				if ( iCurSelectSet == ppapa->eCmptComptageHist1Offer)
					return (ppapa->m_bDispHisto1ComptageOffer ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eCmptComptageHist1Occup)
					return (ppapa->m_bDispHisto1ComptageOccup ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eCmptComptageHist2Offer)
					return (ppapa->m_bDispHisto2ComptageOffer ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eCmptComptageHist2Occup)
					return (ppapa->m_bDispHisto2ComptageOccup ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eCmptComptageHist3Offer)
					return (ppapa->m_bDispHisto3ComptageOffer ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eCmptComptageHist3Occup)
					return (ppapa->m_bDispHisto3ComptageOccup ? rtnval : -rtnval);
			}
			//if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
			//{
			if (!((CCTAApp*)APP)->GetRhealysFlag())
				if ( iCurSelectSet == ppapa->eCmptPrevision)
					return (ppapa->m_bDispPrevision ? rtnval : -rtnval);
			//}
		}
		else // eBkt
		{
			if ( iCurSelectSet == ppapa->eBktResHoldAvailSum)
				return (ppapa->m_bDispResHoldAvailSum ? rtnval : -rtnval);
			if ( iCurSelectSet == ppapa->eBktAuthCur)
				return (ppapa->m_bDispAuthCur ? rtnval : -rtnval);
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
				if ( iCurSelectSet == ppapa->eBktHisto1AuthCur)
					return (ppapa->m_bDispHisto1AuthCur ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eBktHisto1ResHoldTot)
					return (ppapa->m_bDispHisto1ResHoldTot ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eBktHisto2AuthCur)
					return (ppapa->m_bDispHisto2AuthCur ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eBktHisto2ResHoldTot)
					return (ppapa->m_bDispHisto2ResHoldTot ? rtnval : -rtnval);
				if ( iCurSelectSet == ppapa->eBktHisto3AuthCur)
					return (ppapa->m_bDispHisto3AuthCur ? rtnval : -rtnval);
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
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
			{
				if ( iCurSelectSet == ppapa->eBktPrevision)
					return (ppapa->m_bDispPrevision ? rtnval : -rtnval);
			}
		}
	}
	else // iDataSet == XRT_DATA2
		return (ppapa->m_bDispEC ? 20000 : -20000);
	return 0;
}

void CRRDHistHelp::ClickDynaButton( int nId, int checked)
{
	TRACE ("Click on DynaButton nId=%d, checked=%d\n", nId, checked);
	CCTAChartData* pChartData = 
		((CRRDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
	if (nId >= 20000) // XRT_DATA2
		((CRRDHistView*)m_pParent->GetParentView())->RevMasqueCourbe (pChartData->GetGraphType(), nId - 20000, TRUE);
	else
		((CRRDHistView*)m_pParent->GetParentView())->RevMasqueCourbe (pChartData->GetGraphType(), nId - 10000, FALSE);
	((CRRDHistView*)m_pParent->GetParentView())->ChangeRRDDisplay(FALSE);
}

// Translate the dataset to a cta graph style (color)
//
int CRRDHistHelp::GetCTADataStyleIndex( int iDataSet, int iCurSelectSet )
{
	// get the Cmpt of the selected data point for the 1st title
	CCTAChartData* pChartData = 
		((CRRDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
	if ( pChartData == NULL )
		return CCTAChart::eVoidStyleIdx;

	if (pChartData->GetGraphType() == CCTAChartData::eCmpt)
	{
		if ( iDataSet == XRT_DATA )
		{
			// translate the dataset to a cta graph style (color)
			if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptCapacity == iCurSelectSet) )
				return CCTAChart::eCapStyleIdx;
			if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptOvbLevelCur == iCurSelectSet) )
			{
				if (((CRRDHistView*)m_pParent->GetParentView())->GetAltFlag())
					return -1;
				else
					return CCTAChart::eAuthStyleIdx;
			}
			if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptResHoldAvg == iCurSelectSet) )
				return CCTAChart::eResHoldAvgStyleIdx;
			if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptResHoldTot == iCurSelectSet) )
				return CCTAChart::eResHoldTotStyleIdx;
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
			{ 
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptHoiTra == iCurSelectSet) )
					return CCTAChart::eHoiTraStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptHoiTraIdv == iCurSelectSet) )
					return CCTAChart::eHoiTraIdvStyleIdx;
			}
			if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptResHoldIdv == iCurSelectSet) )
				return CCTAChart::eResHoldIdvStyleIdx;

			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
			{
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto1OvbLevel == iCurSelectSet) )
					return CCTAChart::eHisto1OvbLevelStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto1ResHoldTot == iCurSelectSet) )
					return CCTAChart::eHisto1ResHoldTotStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto2OvbLevel == iCurSelectSet) )
					return CCTAChart::eHisto2OvbLevelStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto2ResHoldTot == iCurSelectSet) )
					return CCTAChart::eHisto2ResHoldTotStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto3OvbLevel == iCurSelectSet) )
					return CCTAChart::eHisto3OvbLevelStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto3ResHoldTot == iCurSelectSet) )
					return CCTAChart::eHisto3ResHoldTotStyleIdx;
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
				{
					if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto1HoiTra == iCurSelectSet) )
						return CCTAChart::eHisto1HoiTraStyleIdx;
					if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto2HoiTra == iCurSelectSet) )
						return CCTAChart::eHisto2HoiTraStyleIdx;
					if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptHisto3HoiTra == iCurSelectSet) )
						return CCTAChart::eHisto3HoiTraStyleIdx;
				}
			}

			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() &&
				!((CRRDHistView*)m_pParent->GetParentView())->GetAltFlag())  // Is this a Comptages client ?
			{
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOfferStdvHigh == iCurSelectSet) )
					return CCTAChart::eCmptComptageOfferStdvHighStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOffer == iCurSelectSet) )
					return CCTAChart::eCmptComptageOfferStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOfferStdvLow == iCurSelectSet) )
					return CCTAChart::eCmptComptageOfferStdvLowStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOccupStdvHigh == iCurSelectSet) )
					return CCTAChart::eCmptComptageOccupStdvHighStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOccup == iCurSelectSet) )
					return CCTAChart::eCmptComptageOccupStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageOccupStdvLow == iCurSelectSet) )
					return CCTAChart::eCmptComptageOccupStdvLowStyleIdx;
			}

			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() &&
				!((CRRDHistView*)m_pParent->GetParentView())->GetAltFlag())  // Is this a Comptages client ?
			{
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageHist1Offer == iCurSelectSet) )
					return CCTAChart::eCmptComptageHist1OfferStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageHist1Occup == iCurSelectSet) )
					return CCTAChart::eCmptComptageHist1OccupStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageHist2Offer == iCurSelectSet) )
					return CCTAChart::eCmptComptageHist2OfferStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageHist2Occup == iCurSelectSet) )
					return CCTAChart::eCmptComptageHist2OccupStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageHist3Offer == iCurSelectSet) )
					return CCTAChart::eCmptComptageHist3OfferStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptComptageHist3Occup == iCurSelectSet) )
					return CCTAChart::eCmptComptageHist3OccupStyleIdx;
			}
			//if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
			//{
			if (!((CCTAApp*)APP)->GetRhealysFlag() && !((CRRDHistView*)m_pParent->GetParentView())->GetAltFlag())
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eCmptPrevision == iCurSelectSet) )
					return CCTAChart::ePrevisionStyleIdx;
			//}
		}
		else  // data set is XRT_DATA2
		{
			// translate the dataset to a cta graph style (color)
			switch ( iCurSelectSet )
			{
			case /*((CRRDHistView*)m_pParent->GetParentView())->*/CRRDHistView::eCmptResHoldStdvHigh : // JZZ portage Visual.net
				return CCTAChart::eResHoldStdvHighStyleIdx;
			case /*((CRRDHistView*)m_pParent->GetParentView())->*/CRRDHistView::eCmptResHoldStdvLow : // JZZ portage Visual.net
				return CCTAChart::eResHoldStdvHighStyleIdx;  // same as Stdv High because we want the same color and style for both
			}
		}
	}
	else  // Bucket graph
	{
		if ( iDataSet == XRT_DATA )
		{
			// translate the dataset to a cta graph style (color)
			if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktAuthCur == iCurSelectSet) )
				return CCTAChart::eAuthStyleIdx;
			if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktResHoldAvg == iCurSelectSet) )
				return CCTAChart::eResHoldAvgStyleIdx;
			if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktResHoldTot == iCurSelectSet) )
				return CCTAChart::eResHoldTotStyleIdx;
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
			{ 
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktHoiTra == iCurSelectSet) )
					return CCTAChart::eHoiTraStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktHoiTraIdv == iCurSelectSet) )
					return CCTAChart::eHoiTraIdvStyleIdx;
			}
			if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktResHoldIdv == iCurSelectSet) )
				return CCTAChart::eResHoldIdvStyleIdx;
			if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktResHoldAvailSum == iCurSelectSet) )
				return CCTAChart::eBktResHoldAvailSumStyleIdx;

			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
			{
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktHisto1AuthCur == iCurSelectSet) )
					return CCTAChart::eHisto1OvbLevelStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktHisto1ResHoldTot == iCurSelectSet) )
					return CCTAChart::eHisto1ResHoldTotStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktHisto2AuthCur == iCurSelectSet) )
					return CCTAChart::eHisto2OvbLevelStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktHisto2ResHoldTot == iCurSelectSet) )
					return CCTAChart::eHisto2ResHoldTotStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktHisto3AuthCur == iCurSelectSet) )
					return CCTAChart::eHisto3OvbLevelStyleIdx;
				if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktHisto3ResHoldTot == iCurSelectSet) )
					return CCTAChart::eHisto3ResHoldTotStyleIdx;
				if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
				{
					if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktHisto1HoiTra == iCurSelectSet) )
						return CCTAChart::eHisto1HoiTraStyleIdx;
					if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktHisto2HoiTra == iCurSelectSet) )
						return CCTAChart::eHisto2HoiTraStyleIdx;
					if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktHisto3HoiTra == iCurSelectSet) )
						return CCTAChart::eHisto3HoiTraStyleIdx;
				}
			}
			//if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
			//{
				//if ( (((CRRDHistView*)m_pParent->GetParentView())->eBktPrevision == iCurSelectSet) )
				//	return CCTAChart::ePrevisionStyleIdx;
			//}
			//   the enumerated value for eBktResHoldAvailSum is the same as
			//   eTrafficRes, and because Traffic Res is not used in this graph, 
			//   there is no danger of duplication.
		}
		else  // data set is XRT_DATA2
		{
			// translate the dataset to a cta graph style (color)
			switch ( iCurSelectSet )
			{
			case CRRDHistView::eBktResHoldStdvHigh :
				return CCTAChart::eResHoldStdvHighStyleIdx;
			case CRRDHistView::eBktResHoldStdvLow :
				return CCTAChart::eResHoldStdvHighStyleIdx;  // same as Stdv High because we want the same color and style for both
			}
		}
	}
	return -1;
}

void CRRDHistHelp::OnPaint()
{
	// paint all text and graphics on the client area of the dialog
	//
	CPaintDC dc(this); // device context for painting
	CFont* pOldFont = dc.SelectObject( GetHelpTextFont() );

	// get the Cmpt of the selected data point for the 1st title
	CCTAChartData* pChartData = 
		((CRRDHistView*)m_pParent->GetParentView())->GetCurSelChart()->GetCurChartData();
	if ( pChartData == NULL )
		return;

	int iNumberLabels = 0;
	if (pChartData->GetGraphType() == CCTAChartData::eCmpt)
	{
		// note that we've combined stdv high and stdv low onto one line, therefore decrease the number of labels by 1
		//   then we've decreased by 4 because of the addition of comptage for TRN client
		iNumberLabels = 
			((CRRDHistView*)m_pParent->GetParentView())->eCmptNumDataSets + 
			((CRRDHistView*)m_pParent->GetParentView())->eCmptStdvNumDataSets - 1;

		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
			iNumberLabels -= 2;

		// Reduce number by 6 for Comptages
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() &&
			!((CRRDHistView*)m_pParent->GetParentView())->GetAltFlag())  // Is this a Comptages client ?
			iNumberLabels -= 6;
		// Reduce number by 6 for historical Comptages
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesHistoryFlag() &&
			!((CRRDHistView*)m_pParent->GetParentView())->GetAltFlag())  // Is this a Comptages client ?
			iNumberLabels -= 6;
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
		{ 
			if (((CRRDHistView*)m_pParent->GetParentView())->mp_HistoData)
				iNumberLabels -= (3 - ((CRRDHistView*)m_pParent->GetParentView())->mp_HistoData->GetNbDate())*2;
			else
				iNumberLabels -= 6;
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
				iNumberLabels -= 3;
		}
		if ( ((CRRDHistView*)m_pParent->GetParentView())->GetAltFlag())
			iNumberLabels -=1;
	}
	else
	{
		iNumberLabels = ((CRRDHistView*)m_pParent->GetParentView())->eBktNumDataSets +
			((CRRDHistView*)m_pParent->GetParentView())->eBktStdvNumDataSets - 1;
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHoiTraFlag() )
			iNumberLabels -= 2;
		if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
		{ 
			if (((CRRDHistView*)m_pParent->GetParentView())->mp_HistoData)
				iNumberLabels -= (3 - ((CRRDHistView*)m_pParent->GetParentView())->mp_HistoData->GetNbDate())*2;
			else
				iNumberLabels -= 6;
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
		((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetControlRRDCurveFlag() );

	if (this->needRedraw)
	{
		this->needRedraw = 0;
		Invalidate();
		//Invalidate();
	}
	// Do not call CDialog::OnPaint() for painting messages
}

CRRDHistView* CRRDHistView::getOtherView()
{
	CWnd* parent = GetParent();

	for (CWnd* child = parent->GetWindow( GW_CHILD ); child; child = child->GetWindow( GW_HWNDNEXT )) {
		if (child->m_hWnd != m_hWnd && child->GetRuntimeClass() == GetRuntimeClass())
			return (CRRDHistView*)child;
	}
	return NULL;
}


// DM7978 - LME - YI-5050 - recuperation des CcMax, CcOuv et J-x histo
// cette methode renvoie un vector des donnees historiques pour un cmpt
// A appeler avant la methode CalculJxHisto !
std::vector<YmIcHistoDataRRDDom> CRRDHistView::getHistoDataByCmpt(const char cmpt)
{
	std::vector<YmIcHistoDataRRDDom> vector;

	for(std::vector<YmIcHistoDataRRDDom>::size_type i = 0; i != m_HistoDataSet.size(); i++) 
	{
		if(m_HistoDataSet[i].getCmpt() == cmpt)
		{
			vector.push_back(m_HistoDataSet[i]);
		}
	}

	return vector;
}

void CRRDHistView::FillHistoDataSet()
{
	YmIcHistoDataRRDDom dom;
	YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	dom.setTrancheNo(pTranche->TrancheNo());
	
	// OHA : ano 78122
	// dom.setDptDate(pTranche->DptDate());
	// (CCTAApp*)APP)->m_CurHistoDates.DateHisto1()
	CString szHisto1 = MakeSzHisto (((CCTAApp*)APP)->m_CurHistoDates.Commentaire1(), ((CCTAApp*)APP)->m_CurHistoDates.DateHisto1());
	
	if (!szHisto1.IsEmpty())
	{
		dom.setDptDate(((CCTAApp*)APP)->m_CurHistoDates.DateHisto1());
	}
	else 
	{	// gerer le cas ou szHisto1 est vide
		dom.setDptDate(pTranche->DptDate());
	}
	// OHA : ano 78122
	// CResaRailView* pView = ((CMainFrame*) AfxGetApp()->GetMainWnd())->GetResaRailView();
	CLegView* pLegView = ((CMainFrame*) AfxGetApp()->GetMainWnd())->GetLegView();


	
	YmIcLegsDom* pLegDom = pLegView->GetLoadedLeg();

	dom.setLegOrig(pLegDom->LegOrig());

	CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query(* (WinApp->m_pDatabase), FALSE);
	pSQL->SetDomain(&dom);
	RWDBReader reader(WinApp->m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_RRD_HIST_CMPT_HISTO));

	m_HistoDataSet.clear();
	while(reader())
	{
		if (reader.isValid())
		{
			reader >> dom;
			m_HistoDataSet.push_back(dom);
		}
	}

	delete pSQL;
}
// DM7978 - LME - FIN

// DM7978 - LME - YI-5055 - calcul du j-x historique
// Parcours le vector des donnees historiques pour calculer le j-x historique
// Attention les donnees doivent etre prealablement triees par j-x dans l'ordre croissant !
// Ici aucune méthode de tri n'est appelee car la requete IDS_SQL_SELECT_RRD_HIST_CMPT_HISTO
// renvoie les donnees triees (ORDER BY J_X)
long CRRDHistView::CalculJxHisto(const std::vector<YmIcHistoDataRRDDom>& histoData)
{
	long ccMaxHisto = -1; 
	long currentJx = -2;
	long previousJx = -2;
	YmIcHistoDataRRDDom histo;

	if(histoData.size() > 0)
	{
		histo = histoData[0];

		// CME 78122 
		// On verifie que le premier element de la liste des donnees historiques correspond au j+1
		if(histo.getJX() == -1)
		{
			// Recuperation de la ccMaxHisto
			ccMaxHisto = histo.getCcMax();

			if((ccMaxHisto != -1) && (histo.getLastOpenCc() == ccMaxHisto))
			{
				previousJx = -1;
			}
			else 
			{
				return -2;
			}
		}
		// Sinon on renvoie j+2 (jour inexistant) pour savoir que le j+1 n'est pas dans la liste 
		else
		{
			return -2;
		}

		// Calcul du j-x historique
		for(std::size_t i = 1; i < histoData.size(); i++) 
		{
			histo = histoData[i];
			currentJx = histo.getJX();

			if((histo.getLastOpenCc() != ccMaxHisto) || (histo.getCcMax() != ccMaxHisto))
			{
				YmIcHistoDataRRDDom previousHisto = histoData[i-1];
				setCcMax(previousHisto.getCcMax());
				return previousJx;
			}
			else if(i == histoData.size() - 1)
			{
				setCcMax(histo.getCcMax());
				return currentJx;
			}
			else
			{
				previousJx = currentJx;
			}
		}
	}

	return currentJx;
}

// DM7978 - LME - FIN