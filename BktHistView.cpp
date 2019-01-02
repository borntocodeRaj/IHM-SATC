// BktHistView.cpp : implementation file
//
#include "StdAfx.h"

#include "CTA_Resource.h"

#include "BktHistView.h"
#include "ResaRailView.h"
#include "TrancheView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define OFFSET_UNDERLINE  2
#define IDS_VIDE "   "
UINT cwid = 1; // global variable used to give an id to the icon window created in the view
int CBktHistJumLink::g_bktjumkey = 0;

/////////////////////////////////////////////////////////////////////////////
// Internal structures

CBktHistCmptMap::CBktHistCmptMap(BOOL bJumMap)
{
  m_pCommentIcon = NULL;
  m_pJumeauxLink = NULL;
  m_historef = 0;
  bCoeffMatriceCC8 = FALSE;
  m_bJumMap = bJumMap;
}

CBktHistCmptMap::~CBktHistCmptMap()
{
  POSITION pos = GetStartPosition();
  while (pos != NULL)
  {
    CString sCmpt;
    CBktHistBktArray* pBktHistBktArray;
    
    GetNextAssoc(pos, sCmpt, pBktHistBktArray);
	//4965.3, vérifions la présence de YmIcBktHistDom "virtuel", pour les détruire ici.
	if (!m_bJumMap)
	{ // 07/09/05 Plantage sur delete pBktHist si l'on passe par là pour les CBktHistDateMap correspondant
	  // aux trains influents voir dans CTA.h : m_pTempBktJumHist et m_pTempBktJumHistAlt
	  // Pas trouvé l'origine du bug, donc mise en place d'un antibug avec le  boolean m_bJumMap
	  // Vaux mieux perdre un peu de mémoire que de planter l'application...
	  YmIcBktHistDom* pBktHist = NULL;
	  for (int i = 0; i < pBktHistBktArray->GetSize(); i++)
	  {
	    pBktHist = pBktHistBktArray->GetAt (i);
	    if (pBktHist->TrancheNo () == 0)
		  delete pBktHist;
	  }
	}
    delete pBktHistBktArray;
    pBktHistBktArray = NULL;
  }
  if (m_pCommentIcon)
  {
    delete m_pCommentIcon;
    m_pCommentIcon = NULL;
  }
  if (m_pJumeauxLink)
  {
    delete m_pJumeauxLink;
    m_pJumeauxLink = NULL;
  }
}

void CBktHistCmptMap::Add(YmIcBktHistDom* pBktHist)
{
  CBktHistBktArray* pBktHistBktArray;
  
  if (pBktHist->HistoRef() > m_historef)
	m_historef = pBktHist->HistoRef();
  if (!Lookup(pBktHist->Cmpt(), pBktHistBktArray))
    SetAt(pBktHist->Cmpt(), pBktHistBktArray = new CBktHistBktArray());
  
  // Verifions si ce bucket n'est pas deja dans la liste.
  // Cela peut arriver avec la lecture des données historiques (DM 4228.7) qui peuvent 
  // intersecter sur les dates de la plage d'un mois un an auparavant
  YmIcBktHistDom* pBH = NULL;
  for (int i = 0; i < pBktHistBktArray->GetSize(); i++)  // loop through the buckets
  {
    pBH = pBktHistBktArray->GetAt(i);
	if (pBH && (pBH->NestLevel() == pBktHist->NestLevel()))
	  return;
  }
  pBktHistBktArray->Add(pBktHist);
}

int CBktHistCmptMap::GetNbrLines()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  int nLines = 0;
  CBktHistBktArray* pBktHistBktArray = NULL;
  YmIcBktHistDom* pBktHist = NULL;
  POSITION pos = GetStartPosition();
  while (pos != NULL)
  {
    CString sCmpt;
    
    int iOffset = 8;
	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAriDataFlag())
	  iOffset += 1;
    // count the number of lines we draw at the bucket level
    GetNextAssoc(pos, sCmpt, pBktHistBktArray);
    for (int i = 0; i < pBktHistBktArray->GetSize(); i++)
    {
      pBktHist = pBktHistBktArray->GetAt(i);
      nLines++;
    }
    nLines += iOffset;
    //    nLines += pBktHistBktArray->GetSize() + iOffset;
  }
  return nLines;
}

void CBktHistCmptMap::CalculAvecMatriceCC8()
{
  if (bCoeffMatriceCC8)
	return;
  YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  if (pTranche->Nature() == 3)
    return;
  CalculAvecMatrice (&(((CCTAApp*)APP)->m_MapMatriceCC8), FALSE);
  bCoeffMatriceCC8 = TRUE;
}

void CBktHistCmptMap::CalculAvecMatrice(CMapStringToOb* pMatrice, BOOL bForce)
{
  CString sCmpt;
  CBktHistBktArray* pBktHistBktArray;
  YmIcBktHistDom* pBktHist = NULL;
  CString szKey;
  CMapStringToOb  MapOldResa;
  CMapStringToOb  MapOldBktHist;
  CArray<int,int> NewResa;

  if (!pMatrice->GetCount()) // si la matrice est vide on ne fait rien.
    return;

  YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  int nbk = 0;
  int obk = 0;
  MapOldResa.RemoveAll();
  POSITION pos = GetStartPosition();
  while (pos != NULL)
  { 
	  GetNextAssoc(pos, sCmpt, pBktHistBktArray); 
	  for (int i = 0; i < pBktHistBktArray->GetSize(); i++)
	  {
	    pBktHist = pBktHistBktArray->GetAt(i);
		szKey.Format("%s/%s/%d",pTranche->Entity(),pBktHist->Cmpt(),pBktHist->NestLevel());
		MapOldResa.SetAt( szKey, (CObject*)pBktHist->ResHoldTot());
		szKey.Format("%s/%d",pBktHist->Cmpt(),pBktHist->NestLevel());
		MapOldBktHist.SetAt( szKey, (CObject*)pBktHist);
	  }
  }

  pos = GetStartPosition();
  while (pos != NULL)
  { 
	  GetNextAssoc(pos, sCmpt, pBktHistBktArray);
	  // nbk est le nombre de bucket pour le sCmpt dans la tranche courante.
	  ((CCTAApp*)APP)->m_nbBktByCmpt.Lookup (sCmpt, (CObject*&)nbk);
	  obk = pBktHistBktArray->GetSize();
	  BOOL aaa = FALSE;
	  if (nbk && ((nbk != obk) || bForce))
	  {
	    aaa = ((CCTAApp*)APP)->TransformeMatrice (pTranche->Entity(), sCmpt, &MapOldResa, NewResa, pMatrice);
	  }
 
	  if (aaa)
	  {
	  // Il faut creer des YmIcBktHistDom "virtuels"
		CString sbl;
		pBktHistBktArray->RemoveAll();
		YmIcBktHistDom* pLastMemoBktHist = NULL;
		int zz = bForce ? obk : nbk; // Si forcage (cas Notes), on garde le nombre de bucket initial
		for (int i = 0; i < zz; i++)
		{
		  szKey.Format("%s/%d", (LPCSTR)sCmpt, i);
		  YmIcBktHistDom* pMemoBktHist = NULL;
		  if (MapOldBktHist.Lookup (szKey, (CObject*&) pMemoBktHist))
		  {
		    pBktHist = new YmIcBktHistDom (*pMemoBktHist);
			pLastMemoBktHist = pMemoBktHist;
		  }
		  else
		  { // plus de nouveaux buckets que d'anciens, on en construit un nouveau
		    pBktHist = new YmIcBktHistDom (*pLastMemoBktHist);
			pBktHist->NestLevel(i);
			pBktHist->Recette(0);
			pBktHist->RecetteGaranti(0);
			char sss[3];
			sprintf (sss, "%s%d", (LPCSTR)sCmpt, i);
			pBktHist->BucketId(sss);
			sprintf (sss, "%d", i);
			pBktHist->BucketLabel(sss);
			pBktHist->AuthCur(-1);
		  }
		  pBktHist->Recalcule(TRUE);
		  if (i < NewResa.GetSize())
		  {
		    pBktHist->ResHoldTot (NewResa.GetAt(i));
		  }
		  else
		  {
		    pBktHist->ResHoldTot (0);
		  }
		  pBktHistBktArray->Add (pBktHist);
		}
		for (int i = 0; i < zz; i++)
		{ 
		  szKey.Format("%s/%d", (LPCSTR)sCmpt, i);
		  YmIcBktHistDom* pMemoBktHist = NULL;
		  if (MapOldBktHist.Lookup (szKey, (CObject*&) pMemoBktHist))
		  { // le pBktHist memorisé avait déjà été recalculé par une autre matrice
			if (pMemoBktHist && pMemoBktHist->Recalcule())
			  delete pMemoBktHist;
		  }
		}
	  }
  }  
}


void CBktHistCmptMap::MajOnRad(YmIcBktHistDom* pBktHist)
{
  CBktHistBktArray* pBktHistBktArray;
  CString sCmpt = pBktHist->Cmpt();
  if (Lookup(sCmpt, pBktHistBktArray))
  {
    YmIcBktHistDom* pBH = NULL;
	for (int i = 0; i < pBktHistBktArray->GetSize(); i++)
	{
	  pBH = pBktHistBktArray->GetAt(i);
	  if (pBH->NestLevel() == pBktHist->NestLevel())
	  {
		pBH->AuthCur(pBktHist->AuthCur());
		return;
	  }
	}
  }
}

CBktHistDateMap::CBktHistDateMap(BOOL bJumMap /*= TRUE*/ )
{
  m_ImageList.Create(IDB_STATUS_BITMAP, iBitMapSizeConst, 0, RGB(0, 0, 255));
  m_papa = NULL;
  m_bJumMap = bJumMap;
}

CBktHistDateMap::~CBktHistDateMap()
{
  Empty();
}

void CBktHistDateMap::Add(YmIcBktHistDom* pBktHist)
{
  CBktHistCmptMap* pBktHistCmptMap;
  CString s;
  long juju;

  if (!Lookup(pBktHist->DptDate(), pBktHistCmptMap))
  { 
    if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag())
    {
      // select the Comptages records for this pBktHist element
      YmIcComptagesDom ComptagesRecord;
      YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey( TRANCHE_KEY);
      YmIcLegsDom* pLeg  = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
 	  ComptagesRecord.FamilyNo(pTranche->FamilyNo());
	  ComptagesRecord.FamilyFlag(pTranche->FamilyFlag());
      ComptagesRecord.DptDate(pBktHist->DptDate());  // get the dpt date and cmpt from this bkt hist record
      ComptagesRecord.CmptFlag(0);  // do not constrain on compartment
      ComptagesRecord.Cmpt(pBktHist->Cmpt());

      YM_Query* pSQL = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
      pSQL->SetDomain(&ComptagesRecord);
	  int qref;
	  if (pBktHist->HistoRef())
		qref = IDS_SQL_SELECT_COMPTAGES_ONEHIST_TRN;
	  else
		qref = IDS_SQL_SELECT_COMPTAGES_TRN;
      RWDBReader Reader( ((CCTAApp*)APP)->m_pDatabase->Transact(pSQL, qref) );
      YmIcComptagesDom* pComptagesRecord = NULL;
      while (Reader())
      {
	if (Reader.isValid())
	{//	YmIcComptagesDom ComptagesRecord;
	  Reader >> ComptagesRecord;
	  pComptagesRecord = new YmIcComptagesDom(ComptagesRecord);
	  m_papa->m_ComptagesArray.Add(pComptagesRecord);

	  if (ComptagesRecord.LegOrig() == pLeg->LegOrig())
            break;  // found it !
        }
      }
      delete pSQL;
    }
    SetAt(pBktHist->DptDate(), pBktHistCmptMap = new CBktHistCmptMap(m_bJumMap));
    pBktHistCmptMap->m_lTrancheNo = pBktHist->TrancheNo();
    pBktHistCmptMap->m_szTrainNo = pBktHist->TrainNo();
    if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetForcementFlag())
    {
      pBktHistCmptMap->m_force_status = pBktHist->ForceStatus();
    }
    if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSupplementLevelFlag())
    {
		// OHA : ano 75272
		pBktHistCmptMap->m_supplement_level = pBktHist->SupplementLevel();
		//pBktHistCmptMap->m_supplement_level = pBktHist->NiveauSocial();
		pBktHistCmptMap->m_sup_flag = pBktHist->SupFlag();
    }
    if (m_papa)
    { // Si m_papa == NULL, CBktHistDateMap est ((CCTAApp*)APP)->m_pTempBktJumHist,
      // la table globale contenant les infos pour une tranche jumelle.
      
      s = pBktHist->CommentText();
      
      // do we have any comment text for this tranche ?
      if (s.GetLength())
      { 
        CRect vide(0, 0, 0, 0);
        pBktHistCmptMap->m_pCommentIcon = new CBktHistCommentIcon(m_ImageList.ExtractIcon(BMP_COMMENT), s);
        pBktHistCmptMap->m_pCommentIcon->Create(NULL, "", WS_CHILD | WS_VISIBLE | SS_GRAYRECT, vide, m_papa, cwid++);
      }
      if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetInfluentFlag())
      {
        juju = pBktHist->TgvJumeauxNo();
        int JumIconId = BMP_TRANCHE_INFLUENTE;
        if (juju > 0) // pBktHist->TgvJumeauxNo() == -1 indicate a jumeau (Train Influent) bkthist display.
          // see CBktHistJumLink::OnNotify
        {
          CRect vide(0, 0, 0, 0);
          YmIcLegsDom* pLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
          pBktHistCmptMap->m_pJumeauxLink = 
            new CBktHistJumLink(m_ImageList.ExtractIcon(JumIconId), juju, pBktHist->DptDate(), pLeg->LegOrig(), m_papa);
          pBktHistCmptMap->m_pJumeauxLink->Create(NULL, "", WS_CHILD | WS_VISIBLE | SS_GRAYRECT, vide, m_papa, cwid++);
        }
      }
    }
  }
  pBktHistCmptMap->Add(pBktHist);
}

void CBktHistDateMap::TakeCoeffHisto()
{
  return; 
  // Ne sert plus depuis décembre 2011, car les derniers trains avec 7 CC en B datent de décembre 2008.
  if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRecalcHistoBktFlag())
	return;
  RWDate DateNewRepartition;

  POSITION pos = GetStartPosition();
  CString szClient = ((CCTAApp*)APP)->m_pDatabase->GetClientParameter();
  while (pos != NULL)
  {
    long lDate;
    CBktHistCmptMap* pBktHistCmptMap;
    
    GetNextAssoc(pos, lDate, pBktHistCmptMap);
	if (szClient != THALYS)
	{
      pBktHistCmptMap->CalculAvecMatriceCC8(); // Ne fait rien si inutile
	}
  }
}

void CBktHistDateMap::Empty()
{
  POSITION pos = GetStartPosition();
  while (pos != NULL)
  {
    long lDate;
    CBktHistCmptMap* pBktHistCmptMap;
    
    GetNextAssoc(pos, lDate, pBktHistCmptMap);
    delete pBktHistCmptMap;
  }
  RemoveAll();
}

int CBktHistDateMap::GetNbrLines()
{
  int nLines = 0;
  POSITION pos = GetStartPosition();
  while (pos != NULL)
  {
    long lDate;
    CBktHistCmptMap* pBktHistCmptMap;
    
    GetNextAssoc(pos, lDate, pBktHistCmptMap);
    nLines = __max(nLines, pBktHistCmptMap->GetNbrLines());
  }
  return nLines;
}

void CBktHistDateMap::DisplayAllJum(BOOL display)
{
  if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetInfluentFlag())
  {
    POSITION pos = GetStartPosition();
    while (pos != NULL)
    {
      long lDate;
      CBktHistCmptMap* pBktHistCmptMap;
      
      GetNextAssoc(pos, lDate, pBktHistCmptMap);
      if (pBktHistCmptMap->m_pJumeauxLink)
        pBktHistCmptMap->m_pJumeauxLink->ForceDisplay(display);
    } 
  }
}

// If display is TRUE, CanDisplayAll return TRUE, if at least one TGV jum has not
// been opened.
// If display is FALSE, CanDisplayAll return TRUE, is already opened.
BOOL CBktHistDateMap::CanDisplayAll(BOOL display)
{
  BOOL yepeu = FALSE;
  POSITION pos = GetStartPosition();
  while (pos != NULL)
  {
    long lDate;
    CBktHistCmptMap* pBktHistCmptMap;
    
    GetNextAssoc(pos, lDate, pBktHistCmptMap);
    if (pBktHistCmptMap->m_pJumeauxLink)
      yepeu = pBktHistCmptMap->m_pJumeauxLink->CanDisplay(display);
    if (yepeu)
      return TRUE;
  }
  return FALSE;
}

void CBktHistDateMap::MajOnRad(YmIcBktHistDom* pBktHist)
{
  CBktHistCmptMap* pBktHistCmptMap;
  if (Lookup (pBktHist->DptDate(), pBktHistCmptMap))
  {
    pBktHistCmptMap->MajOnRad(pBktHist);
  }
}

CBktHistTcDate::CBktHistTcDate( LPCSTR txtdt, long lgdt, LPCTSTR pszKey )
{
  _szText = txtdt;
  _lgdate = lgdt;
  _sKey = pszKey;
}

CBktHistDateTable::CBktHistDateTable()
{
  m_reversed = FALSE;
}

CBktHistDateTable::~CBktHistDateTable()
{
  Empty();
}

void CBktHistDateTable::Empty()
{
  int i;
  CBktHistTcDate* ptcdate;
  
  for (i = 0; i < GetSize(); i++)
  { 
    ptcdate = GetAt(i);
    delete ptcdate;
  }
  RemoveAll();
  m_reversed = FALSE;
}

void CBktHistDateTable::Insert(CBktHistTcDate* ptcdate)
{
  int i;
  CBktHistTcDate* wptcdate;
  BOOL bjuminsert = (ptcdate->_szText.IsEmpty());
  
  for (i = 0; i < GetSize(); i++)
  {
    wptcdate = GetAt(i);
    if ((wptcdate->_sKey == ptcdate->_sKey) && wptcdate->_szText.IsEmpty() && ptcdate->_szText.IsEmpty())
    { // We are trying to insert a Jumeaux bucket history, which is already in CBktHistDateTable
      return;
    }


    if (!m_reversed && (wptcdate->_sKey > ptcdate->_sKey))
      break;
    if (m_reversed)
    {
      if (!bjuminsert && (wptcdate->_sKey <= ptcdate->_sKey))
        break;
      if (bjuminsert && (wptcdate->_sKey < ptcdate->_sKey))
        break;
    }
  }
  if (i == GetSize())
    SetAtGrow(i, ptcdate);
  else
    InsertAt(i, ptcdate);
}

void CBktHistDateTable::Renverse()
{
  CBktHistTcDate  *pe1, *pe2;
  int i, j;
  i = GetSize();
  for (j = 0; j < i/2; j++)
  {
    pe1 = GetAt(j);
    pe2 = GetAt(i - j - 1);
    SetAt(j, pe2);
    SetAt(i - j - 1, pe1);
  }
  m_reversed = !m_reversed;
}

/////////////////////////////////////////////////////////////////////////////
// CBktHistAltView

IMPLEMENT_DYNCREATE(CBktHistAltView, CBktHistView)

CBktHistAltView::CBktHistAltView()
:CBktHistView()
{
}

void CBktHistAltView::OnInitialUpdate() 
{
  ((CChildFrame*)GetParentFrame())->SetAltFlag(TRUE);
  
  CBktHistView::OnInitialUpdate();  // base class method
}

/////////////////////////////////////////////////////////////////////////////
// CBktHistView

IMPLEMENT_DYNCREATE(CBktHistView, CScrollView)

CBktHistView::CBktHistView()
{
  m_BktHistDateMap.m_papa = this;
  m_lCurTrancheNo = _lCurDptDate = 0;
  m_lCurSupplement = -1;
  m_bBktHistIsValid = FALSE;
  m_bBktHistoIsValid = FALSE;
  m_bRadDone = FALSE;
	m_flagUpdated = FALSE;

  // initialize the member variable roguewave date to january 1, 2001
  RWDate rwJanFirst2001(01, 01, 2001);
  m_rwJanFirst2001 = rwJanFirst2001;

    CCTAApp* app = (CCTAApp*)AfxGetApp();
    CString szClient = app->m_pDatabase->GetClientParameter();
    _bSimpleSortByDate = !((szClient == SNCF) || (szClient == SNCF_TRN)); 

  m_pItalFont = new CFont;
  CString szFontName = ((CCTAApp*)APP)->GetDocument()->GetFontName();
  long lFontHeight = ((CCTAApp*)APP)->GetDocument()->GetFontHeight();
  long lFontWeight = ((CCTAApp*)APP)->GetDocument()->GetFontWeight();
  m_pItalFont->CreateFont( lFontHeight, 0, 0, 0, lFontWeight, 1, 0, 0, 0, 0, 0, 0, 0, (LPCSTR)szFontName );
}

CBktHistView::~CBktHistView()
{
  if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag())
  {
    YmIcComptagesDom* pComptagesRecord = NULL;
    for (int i=0; i < m_ComptagesArray.GetSize(); i++)
    {
      pComptagesRecord = (YmIcComptagesDom*)m_ComptagesArray.GetAt(i);
      delete pComptagesRecord;
      pComptagesRecord = NULL;
    }
    m_ComptagesArray.RemoveAll();
  }
  if (m_pItalFont)
    delete m_pItalFont;
}

BEGIN_MESSAGE_MAP(CBktHistView, CScrollView)
  //{{AFX_MSG_MAP(CBktHistView)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_SHOWWINDOW()
  ON_WM_DESTROY()
  ON_WM_RBUTTONDOWN()
  //}}AFX_MSG_MAP
  ON_NOTIFY(TCN_SELCHANGE, IDC_TC_DATE, OnSelchange)
  ON_NOTIFY(TCN_SELCHANGE, IDC_TC_GLO_RED, OnSelchange)
  ON_COMMAND(ID_BKTHIST_SHOWJUM, OnShowAllJum)
  ON_COMMAND(ID_BKTHIST_MASKJUM, OnMaskAllJum)
  ON_COMMAND(ID_PRINT, CView::OnFilePrint)
  ON_UPDATE_COMMAND_UI(ID_PRINT,  OnUpdatePrint)
END_MESSAGE_MAP()

char CBktHistView::GetNextCmpt(char c)
{
  YM_Iterator<YmIcCmptDom>* pIterator =
    YM_Set<YmIcCmptDom>::FromKey(CMPT_KEY)->CreateIterator();
  YmIcCmptDom* pCmpt;
  
  for (pIterator->First(); !pIterator->Finished(); pIterator->Next())
  { 
    pCmpt = pIterator->Current();
    if (c == 0)
    { 
      delete pIterator;
      return * (pCmpt->Cmpt());
    }
    else if (* (pCmpt->Cmpt()) == c) 
      c = 0; // On prendra le suivant.
  }
  delete pIterator;
  return 0;
}

BOOL CBktHistView::GetAltFlag() const
{
	return ((CChildFrame*)GetParentFrame())->GetAltFlag(); 
}

void CBktHistView::CCUpdate(CDC*& pDC, YmIcBktHistDom*& pBktHist, const CBktHistBktArray*& pBktHistBktArray,
		CRect &rectb, CPoint &pt, const CSize &szField, const CPoint &ptOffset, CPen &revPen,
		long &lResHoldTotal, long &lResHoldGrpTotal, int &iSomRec, BOOL &bPrixGaranti,
		BOOL &Flag_BuildOK, COLORREF &clrFt, TEXTMETRIC &tm, const int &i, const int &bucketItemsSize)
{
	CString s;
	BOOL bDrawIt = TRUE;

	pBktHist = pBktHistBktArray->GetAt(i);

	// NBN ANO 92213 - YI-11708
	if (((CCTAApp*)APP)->GetRhealysFlag() && i > 0)
	{
		bDrawIt = FALSE;
	}

	if (bDrawIt)
	{
		// NestLevel
		rectb = CRect(pt, szField);
		if (GetAltFlag())
		{
			s.Format ("%s %d", pBktHist->ScxEtanche() ? "(E)" : "", pBktHist->NestLevel());//DT35037 - Lyria lot 3
		}
		else
		{
			s = pBktHist->BucketLabel();
		}
		pDC->DrawText(s, rectb, DT_RIGHT | DT_SINGLELINE); // Numéro de lignes (NestLevel)
		// ResHoldTot
		rectb += ptOffset;

		if (pBktHist->ResHoldTot() >= 0)
		{
			COLORREF crSaved;
			if (pBktHist->Recalcule())
			{
				// a été recalculée
				crSaved = pDC->SetTextColor( RGB(0,0,128) );	// bleu marine;
			}
			// Calcul de la nouvelle valeur avec les coefficients des 2 tableaux A et B		  
			DrawLongValue(pDC, pBktHist->ResHoldTot(), rectb, i == bucketItemsSize);
			if (pBktHist->Recalcule())
			{
				// a été recalculée
				pDC->SetTextColor(crSaved);
			}
		}
		else
		{
			DrawTextValue(pDC, "-", rectb, i == bucketItemsSize);
		}

		lResHoldTotal += pBktHist->ResHoldTot();
		lResHoldGrpTotal += pBktHist->ResHoldDptGrp();

		// AuthCur
		rectb += ptOffset;
		if (pBktHist->AuthCur() >= 0)
		{
			DrawLongValue(pDC, pBktHist->AuthCur(), rectb, FALSE); 
		}
		else
		{
			DrawTextValue(pDC, "-", rectb, FALSE);
		}

		// DM5350 Rajout de la colonne Recette + DM8029 - cache la recette pour ALLEO
		if (Flag_BuildOK && !((CCTAApp*)APP)->GetRhealysFlag())
		{
			COLORREF crSaved = pDC->SetTextColor( clrFt ); 
			CPen* pOldPen = pDC->SelectObject (&revPen);
			rectb += ptOffset;
			long iRev = pBktHist->Recette();
			if (bPrixGaranti)
			{
				iRev += pBktHist->RecetteGaranti();
			}
			DrawLongValue(pDC, iRev, rectb, i == bucketItemsSize); 
			pDC->SetTextColor( crSaved );
			pDC->SelectObject (pOldPen);
			iSomRec = iSomRec + iRev;
		}
		pt.y += tm.tmHeight + tm.tmExternalLeading;
	}
	else
	{
		lResHoldTotal += pBktHist->ResHoldTot();
		lResHoldGrpTotal += pBktHist->ResHoldDptGrp();
		iSomRec = iSomRec + pBktHist->Recette();
		if (bPrixGaranti)
		{
			iSomRec = iSomRec + pBktHist->RecetteGaranti();
		}
	}
}

void CBktHistView::PaintIt(CDC* pDC)
{
	//DM7978 YM-5839
	int idx = m_tcGlobRed.GetCurSel();
	//END DM7978 YM-5839
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	CFont* pFontSaved = pDC->SelectObject(pDoc->GetStdFont(pDC));
	TEXTMETRIC  tm;
	pDC->GetTextMetrics(&tm);
	CPoint ptScrollPos = GetScrollPosition();
	//ptScrollPos.y += IDC_TC_SIZE + 100;
	unsigned long maxDailyDate = ((CCTAApp*)APP)->GetMaxDptDateDailyTraffic();
	BOOL bPrixGaranti = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag();

	BOOL Flag_BuildOK = 
		((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag();

	COLORREF clrFt;
	clrFt = (GetAltFlag()) ? 
		((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkAlt() : 
	((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();

	CPen revPen;
	revPen.CreatePen (PS_SOLID, 0, clrFt);

	int iSomRec =0;
	int iSomRecTotale =0;
	// if record set is valid and we have data 
	if (m_BktHistDateMap.GetCount() != 0)
	{
		int idt, wki;
		int oleft, otop;
		CBktHistTcDate* ptcDate;
		CBktHistTcDate* nptcDate;
		CBktHistDateTable lDateTable;

		// we might need currently selected leg for leg orig/dest searching sys parm
		YmIcLegsDom* pLeg  = 
			YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);

		for (idt = 0; idt < m_DateTable.GetSize(); idt++)
		{
			ptcDate = m_DateTable.GetAt(idt);
			nptcDate = new CBktHistTcDate( ptcDate->_szText, ptcDate->_lgdate, ptcDate->_sKey );
			lDateTable.SetAtGrow( idt, nptcDate );
			//lDateTable.Insert(nptcDate);
		}
		lDateTable.Renverse();
		// We will work on the copy of m_DateTable, We will insert in this copy the
		// possible additional columns containing TGV jumeaux info

		POSITION pos = ((CCTAApp*)APP)->GetTempBktJumHist(GetAltFlag())->GetStartPosition();
		while (pos != NULL)
		{
			long lDate;
			CBktHistCmptMap* pBktHistCmptMap;
			CString s, sKey;
			CBktHistTcDate* ptcDate;

			((CCTAApp*)APP)->GetTempBktJumHist(GetAltFlag())->GetNextAssoc(pos, lDate, pBktHistCmptMap);
			buildKey( lDate, pBktHistCmptMap, sKey );
			ptcDate = new CBktHistTcDate( "", lDate, sKey ); // an empty text identify a jumeau history.
			lDateTable.Insert( ptcDate );
			// CBktHistDateTable.Insert check if this ptcdate is not already in the list.
		}

		// PAINTING
		CRect recta;
		GetClientRect(recta);
		//recta.top += IDC_TC_SIZE;

		COLORREF clrBk = (GetAltFlag()) ? 
			((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt() : 
		((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();

		pDC->SetBkColor(clrBk);

		CRect rectz = recta;
		CPoint pt = GetScrollPosition();
		//pt.y += IDC_TC_SIZE;
		rectz += pt;
		pDC->FillSolidRect(rectz, clrBk);

		CSize szTotal = GetTotalSize();
		recta = CRect(0, IDC_TC_SIZE, szTotal.cx, IDC_TC_SIZE + (tm.tmHeight + tm.tmExternalLeading) * 3);
		COLORREF oldBkColor = pDC->GetBkColor();
		pDC->FillSolidRect(recta, GetSysColor(COLOR_MENU));
		pDC->SetBkColor(oldBkColor);
		recta.bottom = szTotal.cy;

		CSize szField(tm.tmAveCharWidth * 6, tm.tmHeight + tm.tmExternalLeading);
		CSize szLargField(tm.tmAveCharWidth * 8, tm.tmHeight + tm.tmExternalLeading);
		CPoint ptOffset(tm.tmAveCharWidth * 7, 0);
		CPoint ptExtent(tm.tmAveCharWidth * 3, 0);

		recta.top += tm.tmExternalLeading;
		CBktHistCmptMap* pBktHistCmptMap;
		long prevdate = 0;
		long lTotTrancheRev = 0;
		double dRevTrafficRes = 0;
		double dRevTrafficResGrp = 0;
		CString szprevdate = "";

		for (idt = 0; idt < lDateTable.GetSize(); idt++)
		{
			iSomRecTotale = 0;
			ptcDate = lDateTable.GetAt(idt);
			if (!ptcDate->_szText.IsEmpty())
			{
				m_BktHistDateMap.Lookup(ptcDate->_lgdate, pBktHistCmptMap);
				if (!prevdate)
				{
					prevdate = ptcDate->_lgdate;
				}
				else 
				{ // note that, for some clients we do not draw the vertical bar separating periods
					if ((abs(ptcDate->_lgdate - prevdate) > 200))  // 200 is an approximate test for break in year
					{ // on change de période, draw the vertical bar
						CRect rtrait;
						rtrait = recta;
						rtrait.left -= tm.tmAveCharWidth * 2;
						rtrait.right = rtrait.left + 3;
						COLORREF oldBkColor = pDC->GetBkColor();
						pDC->FillSolidRect(rtrait, RGB(128, 128, 128));
						pDC->SetBkColor(oldBkColor);
						prevdate = ptcDate->_lgdate;
					}
				}
			}
			else
			{
				((CCTAApp*)APP)->GetTempBktJumHist(GetAltFlag())->Lookup(ptcDate->_lgdate, pBktHistCmptMap);
			} 
			CPoint pt(recta.TopLeft());
			CRect rici;

			oleft = recta.left;
			otop = recta.top;

			switch (pBktHistCmptMap->m_historef)
			{
			case 1 :	pDC->SetTextColor (pDoc->m_pGlobalSysParms->GetGraphRGBValue(CCTAChart::eHisto1ResHoldTotStyleIdx)); break;
			case 2 :	pDC->SetTextColor (pDoc->m_pGlobalSysParms->GetGraphRGBValue(CCTAChart::eHisto2ResHoldTotStyleIdx)); break;
			case 3 :	pDC->SetTextColor (pDoc->m_pGlobalSysParms->GetGraphRGBValue(CCTAChart::eHisto3ResHoldTotStyleIdx)); break;
			default :	pDC->SetTextColor (RGB(0,0,0));	     break;
			}

			// display of the date
			recta.left += tm.tmAveCharWidth * 5;
			recta.top +=(tm.tmHeight + tm.tmExternalLeading) / 4;
			int OldBkMode = pDC->SetBkMode(TRANSPARENT);
			pDC->SelectObject(pDoc->GetTitleFont(pDC));
			CString ssz;
			ssz = ptcDate->_szText;
			if (pDoc->m_pGlobalSysParms->GetInfluentFlag())
			{
				if (!ssz.GetLength())
				{
					CString s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INFLUENT);
					ssz = szprevdate + "  " + s;
				}
			}
			// draw the date
			pDC->DrawText(ssz, recta, DT_LEFT | DT_TOP | DT_SINGLELINE);
			szprevdate = ptcDate->_szText;

			// display of TGV Jumeaux link
			rici.left = recta.left + tm.tmAveCharWidth * 12;
			rici.top = recta.top - 2;
			rici.right = rici.left + iBitMapSizeConst + 1;
			rici.bottom = rici.top + iBitMapSizeConst + 1;
			if (pBktHistCmptMap->m_pJumeauxLink)
			{
				rici -= ptScrollPos;
				pBktHistCmptMap->m_pJumeauxLink->MoveWindow((LPRECT)rici);
				rici += ptScrollPos;
			}

			pDC->SelectObject(pDoc->GetStdFont(pDC));

			// writing of the second line of the header
			rici = recta;
			rici.top = recta.top + (tm.tmHeight + tm.tmExternalLeading) + (tm.tmHeight + tm.tmExternalLeading) / 3;
			// display of supplement level
			if (pDoc->m_pGlobalSysParms->GetSupplementLevelFlag())
			{
				if (m_lCurSupplement != pBktHistCmptMap->m_supplement_level)
				{
					char ssup[4];
					CString sn;
					itoa(pBktHistCmptMap->m_supplement_level, ssup, 10);
					sn = "N";
					sn += ssup;
					pDC->DrawText(sn, rici, DT_LEFT | DT_TOP | DT_SINGLELINE);
				}
			}

			// display of Sup_flag
			rici.left += tm.tmAveCharWidth * 3;
			if (pDoc->m_pGlobalSysParms->GetSupplementTrainFlag())
			{
				if (pBktHistCmptMap->m_sup_flag)
				{
					HICON wrkhIcon = m_BktHistDateMap.m_ImageList.ExtractIcon(BMP_SUP);
					DrawIconEx(pDC->m_hDC, rici.left, rici.top -2, wrkhIcon,
						iBitMapSizeConst, iBitMapSizeConst, 0, NULL, DI_NORMAL);
				}
			}

			// display of the comment Icon
			rici.left += iBitMapSizeConst + 6;
			if (pBktHistCmptMap->m_pCommentIcon)
			{
				rici.top -= 2;
				rici.right = rici.left + iBitMapSizeConst + 1;
				rici.bottom = rici.top + iBitMapSizeConst + 1;
				rici -= ptScrollPos;
				pBktHistCmptMap->m_pCommentIcon->MoveWindow((LPRECT)rici);
				rici += ptScrollPos;
				rici.top += 2;
				rici.right = recta.right;
				rici.bottom = recta.bottom;
			}

			// display of forcement icon.
			if (pDoc->m_pGlobalSysParms->GetForcementFlag())
			{
				switch (pBktHistCmptMap->m_force_status)
				{
				case FORCE_STATUS_RECOMMENDED : 
					wki = BMP_FORCE_RECOMMEND;
					break;
				case FORCE_STATUS_ACCEPTED : 
					wki = BMP_FORCE_ACCEPTED;
					break;
				case FORCE_STATUS_REFUSED : 
					wki = BMP_FORCE_REFUSED;
					break;
				case FORCE_STATUS_MODIFIED : 
					wki = BMP_FORCE_MODIFIED;
					break;
				default : wki = FORCE_STATUS_NOACTION;
					break;
				}
			}
			else
			{
				wki = FORCE_STATUS_NOACTION;
			}
			if (wki != FORCE_STATUS_NOACTION)
			{ 
				HICON wrkhIcon = m_BktHistDateMap.m_ImageList.ExtractIcon(wki);
				DrawIconEx(pDC->m_hDC, rici.left + 4 + iBitMapSizeConst, rici.top, wrkhIcon,
					iBitMapSizeConst, iBitMapSizeConst, 0, NULL, DI_NORMAL);
			}

			// display for Tranche number, if different from the current one.
			rici.left += iBitMapSizeConst + tm.tmAveCharWidth * 5;
			if (m_szCurTrainNo != pBktHistCmptMap->m_szTrainNo)
			{ 
				CString szText = '(' + pBktHistCmptMap->m_szTrainNo + ')';
				pDC->DrawText(szText, rici, DT_LEFT | DT_TOP | DT_SINGLELINE);
			}

			pDC->SetBkMode(OldBkMode);
			recta.top = otop;
			recta.left = oleft;
			pt.y +=(tm.tmHeight + tm.tmExternalLeading) * 3 + OFFSET_UNDERLINE;
			lTotTrancheRev = 0;

			// faire un browse a partir de la liste ordonnée des Cmpt.
			char currentcmpt = 0;
			bool fini = false;
			int iOffsetCmptLevel = 7;
			BOOL bAriDateOK = TRUE; // vrai si pBktHist->DptDate() < maxDailyDate
			while (!fini)
			{
				CString s;
				CString sCmpt;
				CBktHistBktArray* pBktHistBktArray;
				long lResHoldTotal = 0;
				long lResHoldGrpTotal = 0;

				currentcmpt = GetNextCmpt(currentcmpt);
				if (!currentcmpt)
					fini = true;
				else
				{
					sCmpt = currentcmpt;
					if (pBktHistCmptMap->Lookup(sCmpt, pBktHistBktArray))
					{
						// Cmpt
						CRect rectb(pt, szField);   //debug

						if (GetAltFlag())
							s.Format ("s%s", sCmpt.MakeLower());
						else
							s = sCmpt;
						pDC->DrawText(s, rectb, DT_RIGHT | DT_SINGLELINE);

						// Labels
						rectb += ptOffset;
						DrawLabel(pDC, IDS_SEATS_SOLD, rectb, TRUE); // Libellé Vdu
						rectb += ptOffset;
						DrawLabel(pDC, IDS_BKT_AUTH, rectb, TRUE);   // Libellé Auto

						// DM5350 Ajout du libellé de la colonne recette
						if (Flag_BuildOK)
						{
							COLORREF crSaved = pDC->SetTextColor( clrFt );
							CPen* pOldPen = pDC->SelectObject (&revPen);
							rectb += ptOffset;
							DrawLabel(pDC, IDS_REC_EURO, rectb, TRUE); // Titre de la colonne
							pDC->SetTextColor( crSaved );
							pDC->SelectObject (pOldPen);
						}

						pt.y += tm.tmHeight + tm.tmExternalLeading + OFFSET_UNDERLINE;

						BOOL bDrawIt = TRUE;
						YmIcBktHistDom* pBktHist = NULL;

						//DM-7978-NCH
						int bucketItemsSize = pBktHistBktArray->GetSize();
						int iCCMax = 0;
						int iCCOuv = 0;

						int idx = m_tcGlobRed.GetCurSel();
						if (idx == 1) // si onglet réduit est sélectionné
						{
							if (GetAltFlag()) // si on clique sur reduit dans la fenetre des SCI
							{
								int nbsc = App().GetDocument()->m_pGlobalSysParms->GetNbScVisu();

								for (int i = 0; i < nbsc; i++)
								{
									this->CCUpdate(pDC, pBktHist, pBktHistBktArray,
										rectb, pt, szField, ptOffset, revPen,
										lResHoldTotal, lResHoldGrpTotal, iSomRec, bPrixGaranti,
										Flag_BuildOK, clrFt, tm, i, nbsc);
								}
							}
							else if (!GetAltFlag()) // si on clique sur réduit dans la fenetre des CC
							{
								YmIcLegsDom* pCurLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);

								int nbcc = App().GetDocument()->m_pGlobalSysParms->GetNbCcVisu();
								
								iCCMax = -1;
								iCCOuv = -1;

								if (pCurLeg != NULL)
								{
									char cmpt = sCmpt[0];
									YmIcUserCCDom dom;
									if (pCurLeg->get(cmpt, dom))
									{
										iCCMax = dom.getCCMaxUser();
										iCCOuv = dom.getLastOpenCCUser();
									}
								}

								if (iCCOuv < nbcc)
								{
									bucketItemsSize = nbcc - 1;

									// NPI - Ano 69341
									//for (int i = 0; i <= (nbcc - 1); i++)
									
									if (bucketItemsSize >= pBktHistBktArray->GetSize())
									{
										bucketItemsSize = pBktHistBktArray->GetSize() - 1;
									}

									for (int i = 0; i <= bucketItemsSize; i++)
									///
									{
										this->CCUpdate(pDC, pBktHist, pBktHistBktArray,
											rectb, pt, szField, ptOffset, revPen,
											lResHoldTotal, lResHoldGrpTotal, iSomRec, bPrixGaranti,
											Flag_BuildOK, clrFt, tm, i, bucketItemsSize);
									}
								}																						//fin if (iCCMax < nbcc)
								else
								{
									if (iCCMax <= iCCOuv - nbcc + 3)					//affiche CC0, CC1, CCm, (CC)i=(o-p+4)..o
									{
										int CCindex = 0;
										int trigIndex = iCCOuv - nbcc + 4;
										for (int i = 0; i < pBktHistBktArray->GetSize(); i++)
										{
											if (i == 0 || i == 1 || i == iCCMax ||
												((i >= trigIndex) && (i <= iCCOuv)))
											{
												this->CCUpdate(pDC, pBktHist, pBktHistBktArray,
													rectb, pt, szField, ptOffset, revPen,
													lResHoldTotal, lResHoldGrpTotal, iSomRec, bPrixGaranti,
													Flag_BuildOK, clrFt, tm, i, pBktHistBktArray->GetSize());
											}
										}
									}																					//end affiche CC0, CC1, CCm, (CC)i=(o-p+4)..o
									else																			//affiche CC0, CC1, (CC)i=(o-p+3)..o
									{
										int CCindex = 0;
										int trigIndex = iCCOuv - nbcc + 3;
										for (int i = 0; i < pBktHistBktArray->GetSize(); i++)
										{
											if (i == 0 || i == 1 || i == iCCMax ||
											 ((i >= trigIndex) && (i <= iCCOuv)))
											{
												this->CCUpdate(pDC, pBktHist, pBktHistBktArray,
													rectb, pt, szField, ptOffset, revPen,
													lResHoldTotal, lResHoldGrpTotal, iSomRec, bPrixGaranti,
													Flag_BuildOK, clrFt, tm, i, pBktHistBktArray->GetSize());
											} 
										}
									}
								}																						//end affiche CC0, CC1, (CC)i=(o-p+3)..o
							}
						}
						else // si l'onglet Global est sélectionné ou si on à des datas à afficher
							if (idx == 0)
							{
								int nbBkt = pBktHistBktArray->GetSize();
								for (int i = 0; i < pBktHistBktArray->GetSize(); i++)
								{
									this->CCUpdate(pDC, pBktHist, pBktHistBktArray,
										rectb, pt, szField, ptOffset, revPen,
										lResHoldTotal, lResHoldGrpTotal, iSomRec, bPrixGaranti,
										Flag_BuildOK, clrFt, tm, i, pBktHistBktArray->GetSize()-1);
								}
							} // END DM7978-NCH
							pt.y += OFFSET_UNDERLINE;

							// Res Hold Total
							rectb = CRect(pt, szField);
							rectb += ptOffset;
							COLORREF crSaved;
							if (pBktHist->Recalcule())	// a été recalculée, on se contente de regarder le dernier pBktHist
								crSaved = pDC->SetTextColor( RGB(0,0,128) ); // bleu marine;
							DrawLongValue(pDC, lResHoldTotal, rectb, FALSE); 
							if (pBktHist->Recalcule())	// a été recalculée
								pDC->SetTextColor( crSaved );

							//total recette Resaven
							if (Flag_BuildOK)
							{ // Somme des lignes de la colonne
								crSaved = pDC->SetTextColor( clrFt ); 
								rectb += ptOffset;
								rectb += ptOffset;
								DrawLongValue(pDC, iSomRec, rectb, FALSE);

								pDC->SetTextColor( crSaved );
								iSomRecTotale = iSomRecTotale + iSomRec;
								iSomRec = 0;
							}
							pt.y += tm.tmHeight + tm.tmExternalLeading; // Saut de 1,5 ligne
							pt.y += (tm.tmHeight + tm.tmExternalLeading) / 2;

							if (pDoc->m_pGlobalSysParms->GetAriDataFlag() && !GetAltFlag())
							{
								// Affichage Cap et Traffic sur une même ligne
								rectb = CRect(pt, CSize(tm.tmAveCharWidth * ((iOffsetCmptLevel * 2) - 9), tm.tmHeight + tm.tmExternalLeading));
								pDC->DrawText("Cap", rectb, DT_RIGHT | DT_SINGLELINE);
								rectb = CRect(pt, szField);
								rectb += CPoint(tm.tmAveCharWidth * 5, 0);
								if (pBktHist)
									DrawLongValue(pDC, pBktHist->Capacity(), rectb, FALSE); 

								crSaved = pDC->SetTextColor( RGB(0,0,255) ); // bleu
								rectb += CPoint(tm.tmAveCharWidth * 9, 0);
								s.LoadString(IDS_TRAFFIC);
								pDC->DrawText(s, rectb, DT_RIGHT | DT_SINGLELINE);
								if (pBktHist)
								{
									rectb = CRect(pt, szField);
									rectb += CPoint((tm.tmAveCharWidth * ((iOffsetCmptLevel * 3)-2)), 0);

									if (!pBktHist->TrafficResIsNull() && (pBktHist->DptDate() < maxDailyDate))
										DrawLongValue(pDC, pBktHist->TrafficRes(), rectb, FALSE);
									else
									{
										s.LoadString(IDS_NOT_AVAILABLE);
										pDC->DrawText(s, rectb, DT_RIGHT | DT_SINGLELINE);
									}
								}
								pDC->SetTextColor (crSaved);

								pt.y += tm.tmHeight + tm.tmExternalLeading;// Saut de ligne
								rectb = CRect(pt, CSize(tm.tmAveCharWidth * ((iOffsetCmptLevel * 2) - 9), tm.tmHeight + tm.tmExternalLeading));
								pDC->DrawText("Grp", rectb, DT_RIGHT | DT_SINGLELINE);
								rectb = CRect(pt, szField);
								rectb += CPoint(tm.tmAveCharWidth * 5, 0);
								if (pBktHist)
									DrawLongValue(pDC, lResHoldGrpTotal, rectb, FALSE);	

								// Données venant ARISTOTE Somme par Cmpt
								crSaved = pDC->SetTextColor( RGB(0,0,255) ); // bleu
								rectb += CPoint (tm.tmAveCharWidth * 7, 0);
								pDC->DrawText("Rec", rectb, DT_RIGHT | DT_SINGLELINE);
								rectb += CPoint(tm.tmAveCharWidth * 7, 0);
								if (pBktHist)
								{
									dRevTrafficRes = pBktHist->RevTrafficRes();
									rectb = CRect(pt, szLargField);
									rectb += CPoint(tm.tmAveCharWidth * ((iOffsetCmptLevel * 2)+5), 0);
									if (pBktHist->DptDate() < maxDailyDate)
									{
										CString s;
										s.Format ("%d €", (long)dRevTrafficRes);
										DrawTextValue(pDC, s, rectb, FALSE);
									}
									else
									{
										DrawLabel(pDC, IDS_NOT_AVAILABLE, rectb, FALSE);
										bAriDateOK = FALSE;
									}
									lTotTrancheRev +=(long)dRevTrafficRes;
								}
								pDC->SetTextColor (crSaved);
							} // if (pDoc->m_pGlobalSysParms->GetAriDataFlag())

							else
							{ // Affichage CAP et GRP sur une même ligne. je prèfère dupliquer du code que de jongler avec les décalage
								// selon présence ou non des données Aristote
								rectb = CRect(pt, CSize(tm.tmAveCharWidth * ((iOffsetCmptLevel * 2) - 9), tm.tmHeight + tm.tmExternalLeading));
								if (GetAltFlag())
									s = "CC-1";
								else
									s = "Cap";
								pDC->DrawText(s, rectb, DT_RIGHT | DT_SINGLELINE);
								rectb = CRect(pt, szField);
								rectb += CPoint(tm.tmAveCharWidth * 5, 0);
								if (pBktHist)
									DrawLongValue(pDC, pBktHist->Capacity(), rectb, FALSE);
								rectb += CPoint(tm.tmAveCharWidth * 9, 0);
								pDC->DrawText("Grp", rectb, DT_RIGHT | DT_SINGLELINE);
								rectb = CRect(pt, szField);
								rectb += CPoint((tm.tmAveCharWidth * ((iOffsetCmptLevel * 3)-2)), 0);
								if (pBktHist)
									DrawLongValue(pDC, lResHoldGrpTotal, rectb, FALSE);
							}

							pt.y += tm.tmHeight + tm.tmExternalLeading;

							// Comptages
							if (pDoc->m_pGlobalSysParms->GetComptagesFlag())
							{
								YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
								YmIcLegsDom* pLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
								YmIcComptagesDom* pComptages = NULL;
								if (pBktHist)
									pComptages = ((CCTAApp*)APP)->GetDocument()->GetComptagesRecord(
									&m_ComptagesArray,
									pTranche->FamilyNo(),
									pBktHist->DptDate(),
									pLeg->LegOrig(),
									pBktHist->Cmpt());

								//DM 4965.3, on met les deux valeurs comptages sur une même ligne
								rectb = CRect(pt, CSize(tm.tmAveCharWidth * ((iOffsetCmptLevel * 2) - 6), tm.tmHeight + tm.tmExternalLeading));
								pDC->DrawText("Cptage", rectb, DT_RIGHT | DT_SINGLELINE);
								rectb = CRect(pt, szField);
								rectb += CPoint(tm.tmAveCharWidth * 9, 0);
								if (pComptages)
									DrawLongValue(pDC, pComptages->NbOccupees(), rectb, FALSE);
								rectb += CPoint(tm.tmAveCharWidth * 8, 0);
								if  (pComptages)
								{
									CString scp;
									scp.Format ("%d%%", (long)((double)((double)pComptages->NbOccupees() / (double)pComptages->NbOffert()) * 100));
									pDC->DrawText(scp, rectb, DT_RIGHT | DT_SINGLELINE);
								}
								else
									DrawLabel(pDC, IDS_NOT_AVAILABLE, rectb, FALSE);
								pt.y += tm.tmHeight + tm.tmExternalLeading;
							}
							pt.y += (tm.tmHeight + tm.tmExternalLeading);
					}
				}
			} // while (!fini)

			if (!((CCTAApp*)APP)->GetRhealysFlag())
			{
				// Somme Totale des recettes des Cmpt
				pt.y += (tm.tmHeight + tm.tmExternalLeading) / 2;
				CString s;
				if (Flag_BuildOK)
				{
					CRect rectr = CRect(pt, CSize(tm.tmAveCharWidth * ((8 * 2) - 1), tm.tmHeight + tm.tmExternalLeading));
					s.LoadString(IDS_RECETTE_LEG);
					COLORREF crSaved = pDC->SetTextColor( clrFt );
					pDC->DrawText(s, rectr, DT_RIGHT | DT_SINGLELINE);
					rectr = CRect(pt, szLargField);
					if (Flag_BuildOK)
						rectr += CPoint(static_cast<int>(tm.tmAveCharWidth * iOffsetCmptLevel * 2.5), 0);// DM 5350 
					else
						rectr += CPoint(tm.tmAveCharWidth * iOffsetCmptLevel * 2, 0);// DM 5350 		  
					DrawLongValue(pDC, iSomRecTotale, rectr, FALSE);
					rectr += CPoint (tm.tmAveCharWidth *2, 0);
					pDC->DrawText("€", rectr, DT_RIGHT | DT_SINGLELINE);
					pDC->SetTextColor( crSaved );	  
				}

				if (pDoc->m_pGlobalSysParms->GetAriDataFlag() && !GetAltFlag())
				{
					// Affichage recette Aristote
					pt.y += (tm.tmHeight + tm.tmExternalLeading);
					COLORREF crSaved = pDC->SetTextColor( RGB (0,0, 255) );
					s.LoadString(IDS_RECETTE_TRAIN);
					CRect rectrA = CRect(pt, CSize(tm.tmAveCharWidth * ((8 * 2) - 1), tm.tmHeight + tm.tmExternalLeading));
					pDC->DrawText(s, rectrA, DT_RIGHT | DT_SINGLELINE);
					rectrA = CRect(pt, szLargField);
					if (Flag_BuildOK)
						rectrA += CPoint(static_cast<int>(tm.tmAveCharWidth * iOffsetCmptLevel * 2.5), 0);// DM 5350 
					else
						rectrA += CPoint(tm.tmAveCharWidth * iOffsetCmptLevel * 2, 0);// DM 5350
					if (bAriDateOK)
					{
						DrawLongValue(pDC, lTotTrancheRev, rectrA, FALSE);
						rectrA += CPoint (tm.tmAveCharWidth *2, 0);
						pDC->DrawText("€", rectrA, DT_RIGHT | DT_SINGLELINE);
					}
					else
						DrawLabel(pDC, IDS_NOT_AVAILABLE, rectrA, FALSE);
					pDC->SetTextColor( crSaved );
				}
			}

			int kk = 6;
			recta.left += tm.tmAveCharWidth * kk * 5;
		}
	}
	else
	{
		// Gray the background
		CRect rectg;
		GetClientRect(rectg);
		::FillRect(pDC->m_hDC, rectg, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));
	}
	pDC->SelectObject(pFontSaved);
}

void CBktHistView::DrawLabel(CDC* pDC, UINT nID, LPRECT lpRect, BOOL bUnderline)
{
  CString s;
  
  s.LoadString(nID);
  pDC->DrawText(s, lpRect, DT_CENTER | DT_SINGLELINE);
  if (bUnderline)
    Underline(pDC, lpRect);
}

void CBktHistView::DrawLongValue(CDC* pDC, long lValue, LPRECT lpRect, BOOL bUnderline)
{
  CString s;
  
  s.Format("%d", lValue);
  pDC->DrawText(s, lpRect, DT_RIGHT | DT_SINGLELINE);
  if (bUnderline)
    Underline(pDC, lpRect);
}

void CBktHistView::DrawTextValue(CDC* pDC, CString s, LPRECT lpRect, BOOL bUnderline)
{
  pDC->DrawText(s, lpRect, DT_RIGHT | DT_SINGLELINE);
  if (bUnderline)
    Underline(pDC, lpRect);
}

void CBktHistView::Underline(CDC* pDC, LPRECT lpRect)
{
  pDC->MoveTo(lpRect->left, lpRect->bottom);
  pDC->LineTo(lpRect->right, lpRect->bottom);
}


//////////////////////////////////////
// Write specific view configuration information to the registry
//
void CBktHistView::WriteViewConfig()
{
  // save the graph flags in the registry
  //
  CString szProfileKey;
  ((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

  CSplitChildFrame* pWnd = (CSplitChildFrame*)GetParentFrame();
  pWnd->WriteSplitterConfig(szProfileKey);
}

//////////////////////////////////////
// Restore specific view configuration information from the registry
//
void CBktHistView::RestoreViewConfig()
{
  // restore the graph flags from the registry
  //
  CString szProfileKey;
  ((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

  CSplitChildFrame* pWnd = (CSplitChildFrame*)GetParentFrame();
  pWnd->RestoreSplitterConfig(szProfileKey);

  // set the number of panes (columns) in the splitter window
  CSplitterWnd* pSplitterWnd = ((CSplitChildFrame*)GetParentFrame())->GetSplitterWnd();
  m_bColCount = pSplitterWnd->GetColumnCount( );
}

/////////////////////////////////////////////////////////////////////////////
// CBktHistView drawing

void CBktHistView::OnDraw(CDC* pDC)
{
  // NBN ANO 92249
  CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();

  // if record set is valid and we have data 
  if (m_BktHistDateMap.GetCount() != 0 && (!((CCTAApp*)APP)->GetRhealysFlag() || (!GetAltFlag() || pTrancheView->getCurrentTrancheNature() == 3)))
  {
		//DM-7978-YI-5849
			//TRACE("\n\n Before paintIt SCX\n\n");
			PaintIt(pDC);
    //End DM-7978-YI-5849  
	}
  else
 {
    // Gray the background
   CRect rect;
    GetClientRect(rect);
    ::FillRect(pDC->m_hDC, rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));
 }
}


void CBktHistView::FormatRWDate(CString& s, long lDate)
{
  struct tm tm_dpt_date;
  RWDate rw_date(lDate);
  rw_date.extract(&tm_dpt_date);

  SYSTEMTIME st;
  st.wYear    = tm_dpt_date.tm_year + 1900;
  st.wMonth   = tm_dpt_date.tm_mon + 1;
  st.wDayOfWeek = tm_dpt_date.tm_wday;
  st.wDay     = tm_dpt_date.tm_mday;
  st.wHour    = tm_dpt_date.tm_hour;
  st.wMinute    = tm_dpt_date.tm_min;
  st.wSecond    = tm_dpt_date.tm_sec;
  st.wMilliseconds= 0;

  RWDate rwDate(st.wDay, st.wMonth, st.wYear);
  char szCYear[10];
  itoa(rwDate.year(), szCYear, 10);
  CString szMFCYear(szCYear);
  szMFCYear = szMFCYear.Right(2);
  s.Format("%2.2u/%2.2u/%s", rwDate.dayOfMonth(), rwDate.month(), LPCTSTR(szMFCYear));
  return;
}

/////////////////////////////////////////////////////////////////////////////
// CBktHistView diagnostics

#ifdef _DEBUG
void CBktHistView::AssertValid() const
{
  CScrollView::AssertValid();
}

void CBktHistView::Dump(CDumpContext& dc) const
{
  CScrollView::Dump(dc);
}

CCTADoc* CBktHistView::GetDocument() // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCTADoc)));
  return (CCTADoc*)m_pDocument;
}

#endif //_DEBUG

void CBktHistView::SetStatusBarText(UINT nId)
{
  ((CChildFrame*)GetParentFrame())->
    GetStatusBar()->SetPaneOneAndLock(
    ((CCTAApp*)APP)->GetResource()->LoadResString(nId));
}

void CBktHistView::FrameSizeChange(UINT nType, int cx, int cy)
{
  YM_RecordSet* pBktHistSet = 
    YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHIST_ALT_KEY : BKTHIST_KEY);

  ASSERT(pBktHistSet != NULL);

  if ((nType == SIZE_MINIMIZED) && pBktHistSet->IsOpen())
    pBktHistSet->Close();
  else if ((nType != SIZE_MINIMIZED) && !pBktHistSet->IsOpen())
    pBktHistSet->Open();

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
  {
    YM_RecordSet* pBktHistoSet = YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHISTO_ALT_KEY : BKTHISTO_KEY);
    ASSERT(pBktHistoSet != NULL);
    if ((nType == SIZE_MINIMIZED) && pBktHistoSet->IsOpen())
      pBktHistoSet->Close();
    else if ((nType != SIZE_MINIMIZED) && !pBktHistoSet->IsOpen())
      pBktHistoSet->Open();
  }
}

void CBktHistView::OnShowWindow(BOOL bShow, UINT nStatus)
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
CBktHistView* CBktHistView::getOtherView()
{
    CWnd* parent = GetParent();
    
    for (CWnd* child = parent->GetWindow( GW_CHILD ); child; child = child->GetWindow( GW_HWNDNEXT )) {
	if (child->m_hWnd != m_hWnd && child->GetRuntimeClass() == GetRuntimeClass())
	    return (CBktHistView*)child;
    }
    return NULL;
}
/* split */

/////////////////////////////////////////////////////////////////////////////
// CBktHistView message handlers

void CBktHistView::OnInitialUpdate() 
{
  //DM-7978-YI-5849
  m_tcGlobRed.SetFont( ((CCTAApp*)APP)->GetDocument()->GetTitleFont(NULL) );
	CRect rect;
	GetClientRect(&rect);
	rect.bottom = rect.top + IDC_TC_SIZE;
  m_tcGlobRed.MoveWindow (rect);
  //End DM-7978-YI-5849
  CScrollView::OnInitialUpdate();
  CString titi, toto;
  ((CCTAApp*)APP)->GetTempBktJumHist(GetAltFlag())->m_papa = this;
  titi = ((CCTAApp*)APP)->GetResource()->LoadResString((GetAltFlag()) ? IDS_BKTHIST_SCI_TITLE : IDS_BKTHIST_TITLE);
  GetParentFrame()->GetWindowText(toto);
  GetParentFrame()->SetWindowText(titi);
  ((YM_PersistentChildWnd*)GetParentFrame())->SetChildKey(toto);
  ((YM_PersistentChildWnd*)GetParentFrame())->SetChildView(this);

  YM_RecordSet* pBktHistSet = 
    YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHIST_ALT_KEY : BKTHIST_KEY); 
  ASSERT(pBktHistSet != NULL);  
  if (! pBktHistSet->IsOpen())
    pBktHistSet->Open();
  pBktHistSet->AddObserver(this);

  if (!GetAltFlag())
  { // Alt = SCI, par de PrevAuth pour les SCI
    YM_RecordSet* pPrevAuthSet = 
      YM_Set<YmIcBktHistDom>::FromKey(PREVAUTH_KEY); 
    pPrevAuthSet->AddObserver(this);
  }

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
  {
	YM_RecordSet* pBktHistoSet = 
    YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHISTO_ALT_KEY : BKTHISTO_KEY);
    ASSERT(pBktHistoSet != NULL);
    if (! pBktHistoSet->IsOpen())
      pBktHistoSet->Open();
    pBktHistoSet->AddObserver(this);
  }

  cwid = 1;
  m_bRadDone = FALSE;
}

void CBktHistView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (m_flagUpdated == TRUE) {
		m_flagUpdated = FALSE;
		return;
	}
	if (!GetAltFlag())
	{
		// DM 6496, Remise à disposition, retour requête IDS_SQL_PREVAUTH
		YM_RecordSet* pPrevAuthSet = 
			YM_Set<YmIcBktHistDom>::FromKey(PREVAUTH_KEY);
		if (pHint == pPrevAuthSet)
		{
			if (pPrevAuthSet->IsValid())
			{
				MajFromRad(PREVAUTH_KEY);
				Invalidate();
			}
			return;
		}
	}

	BOOL bInvalidate = FALSE;  // do we invalidate the window?  we want to avoid a "flash"
	YM_RecordSet* pBktHistSet = YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHIST_ALT_KEY : BKTHIST_KEY);
	YM_RecordSet* pBktHistoSet = NULL;
	YM_RecordSet* pBktRevHistoSet = NULL;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
	{
		// Recupération dans la base historique
		pBktHistoSet = YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHISTO_ALT_KEY : BKTHISTO_KEY);
	}

	BOOL bPopulate = FALSE;
	YM_Iterator<YmIcBktHistDom>* pIterator = NULL;

	if (pHint != NULL && pHint == pBktHistSet)
	{
		bInvalidate = (m_bBktHistIsValid != (pBktHistSet->IsValid()));
		m_bBktHistIsValid = (pBktHistSet->IsValid());
		if (pBktHistSet->IsValid())
		{
			if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag() )
			{
				// clean up the Comptages array ... it's got old data
				YmIcComptagesDom* pComptagesRecord = NULL;
				for (int i=0; i < m_ComptagesArray.GetSize(); i++)
				{
					pComptagesRecord = (YmIcComptagesDom*)m_ComptagesArray.GetAt(i);
					delete pComptagesRecord;
					pComptagesRecord = NULL;
				}
				m_ComptagesArray.RemoveAll();
			}
		}
	}

	if (pHint != NULL && pHint == pBktHistoSet)
		m_bBktHistoIsValid = (pBktHistoSet->IsValid());

	BOOL bok = FALSE;
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
		bok = m_bBktHistoIsValid;
	else
		bok = TRUE;

	if (m_bBktHistIsValid && bok)
	{ // Le ou les recordset sont valides.
		int nbl = 0;
		BOOL bAltFlag = GetAltFlag();
		YM_Set<YmIcBktHistDom>* set = YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHIST_ALT_KEY : BKTHIST_KEY);
		pIterator = YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHIST_ALT_KEY : BKTHIST_KEY)->CreateIterator();
		nbl = pIterator->GetCount();
		delete pIterator;
		if (m_bBktHistoIsValid)
		{
			pIterator = YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHISTO_ALT_KEY : BKTHISTO_KEY)->CreateIterator();
			nbl += pIterator->GetCount();
			delete pIterator;
		}
		if (nbl)
		{
			SetStatusBarText(IDS_READ_DATA); // "Lecture des données..."
			bPopulate = TRUE;  // both RecordSets must be valid
		}
		else
		{
			SetStatusBarText(IDS_NO_DATA); // "Aucune donnée trouvée !"
			bPopulate = FALSE;
			return; // JMG 15/03/05 correction ano 130050
		}
	}

	// Populate or empty data structure
	if (bPopulate)
	{
		YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
		m_lCurTrancheNo = pTranche->TrancheNo();
		_lCurDptDate = pTranche->DptDate();
		m_szCurTrainNo = pTranche->TrainNo();
		// NPI - DM7978 : souplesse tarifaire lot 2
		/*if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSupplementLevelFlag())
		m_lCurSupplement = pTranche->SupplementLevel();*/

		// TODO : FAUT-IL GERER LE NIVEAU SOCIAL ICI??
		// On anticipe déjà
		// Code à décommenter si le niveau social (NIVEAU_SOCIAL) doit-être géré ici
		/*if (((CCTAApp*)APP)->GetSouplesseTarifaire())
		m_lCurSupplement = pTranche->NiveauSocial();*/
		///

		pIterator = YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHIST_ALT_KEY : BKTHIST_KEY)->CreateIterator();
		for (pIterator->First(); !pIterator->Finished(); pIterator->Next()) //la
		{
			YmIcBktHistDom* pBktHist = pIterator->Current();
			// Forcage des histo_ref. inutile si la date est vraiment en historique.
			m_BktHistDateMap.Add(pBktHist);

		}
		delete pIterator;

		if (m_bBktHistoIsValid)
		{
			pIterator = YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHISTO_ALT_KEY : BKTHISTO_KEY)->CreateIterator();
			YmIcBktHistDom* pBktHist = NULL;
			long iRev = 0;

			for (pIterator->First(); !pIterator->Finished(); pIterator->Next())
			{
				pBktHist = pIterator->Current();
				if ((strcmp (pBktHist->Cmpt(), pBktHist->BucketId()) == 0) || (strcmp (pBktHist->BucketId(), "X") == 0))
					m_BktHistDateMap.Add(pIterator->Current());
			}
			delete pIterator;
		}	
		bInvalidate = TRUE;

		if (!GetAltFlag()) // La transformation d'autorisation ne concerne que les CC, pas les SCX
			m_BktHistDateMap.TakeCoeffHisto();

		// Populate Date TabCtrl
		TC_ITEM tcItem;
		tcItem.mask = TCIF_TEXT | TCIF_PARAM;
		POSITION pos = m_BktHistDateMap.GetStartPosition();
		while (pos) //la
		{
			long lDate;
			CBktHistCmptMap* pBktHistCmptMap;
			CString s, sKey;
			CBktHistTcDate * ptcDate;

			m_BktHistDateMap.GetNextAssoc(pos, lDate, pBktHistCmptMap);
			FormatRWDate(s, lDate);
			buildKey( lDate, pBktHistCmptMap, sKey );
			ptcDate = new CBktHistTcDate(s, lDate, sKey );
			m_DateTable.Insert(ptcDate);
		}


		// Set text in status bar control
		SetStatusBarText(IDS_DONE);

		if (!GetAltFlag()) //la
		{ // Alt = SCI, pas de PREVAUTH pour les SCI
			CString szClient = ((CCTAApp*)APP)->m_pDatabase->GetClientParameter();
			if ((szClient == SNCF) || (szClient == SNCF_TRN))
			{
				YM_RecordSet* pPrevAuthSet = 
					YM_Set<YmIcBktHistDom>::FromKey(PREVAUTH_KEY);
				pPrevAuthSet->Close();
				pPrevAuthSet->Open();
				m_bRadDone = FALSE;
			}
		}
	}
	else {
		m_DateTable.Empty();
		m_BktHistDateMap.Empty();
		SetStatusBarText(IDS_WAITING_TEXT); //"En attente d'une entrée!"
		m_lCurTrancheNo = _lCurDptDate = 0;
	}

	// Compute font size for Scrollbar initialization
	UpdateScrollSize();
	if (bInvalidate)
		Invalidate();
}

LPCTSTR CBktHistView::buildKey( long lDate, CBktHistCmptMap* pBktHistCmptMap, CString & sKey )
{
    if (_bSimpleSortByDate)
	sKey.Format( "%08lx", lDate );	// Simply sort by date
    else {
	// Define sort key:
	// 1) Date with historef
	// 2) Equivalent day last year (same day of week)
	// 3) Sort by date
	sKey.Format( "%d-%d-%08lx", pBktHistCmptMap->m_historef != 0, _lCurDptDate == lDate+52*7, lDate );
	//    TRACE("DptDate=%d \t %d\n", _lCurDptDate, lDate );

	/*CString s;
	FormatRWDate(s, lDate);
	TRACE("Key=%s\t%s\n", (LPCTSTR)sKey, (LPCTSTR)s );*/
    }
    return sKey;
}

void CBktHistView::UpdateScrollSize()
{
  TEXTMETRIC  tm;
  int nbcol;
  CClientDC dc(this);
  CFont* pFontSaved = dc.SelectObject(((CCTAApp*)APP)->GetDocument()->GetStdFont(NULL));
  dc.GetTextMetrics(&tm);
  dc.SelectObject(pFontSaved);
  BOOL Flag_BuildOK = 
	  	((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag();

  int nw = 30;
  int nl;
  if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetComptagesFlag())
	nl = 0;
  else
	nl = -2;

  if (!((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAriDataFlag())
    nl -= 1;

  if (Flag_BuildOK)
    nl++;

  nbcol = m_BktHistDateMap.GetCount() + ((CCTAApp*)APP)->GetTempBktJumHist(GetAltFlag())->GetCount();
  // Calculate size
  int nbl = m_BktHistDateMap.GetNbrLines() + nl;
  if (nbl < 0)
    nbl = 0;
  CRect tc_rect(CPoint(0, 0), CSize(tm.tmAveCharWidth * nw * nbcol, (tm.tmHeight + tm.tmExternalLeading) * nbl));

  // Adjust Scrollbars
  SetScrollSizes(MM_TEXT, tc_rect.Size());
}

void CBktHistView::UpdateSisterAlso()
{
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

  POSITION pos = pDoc->GetFirstViewPosition();
  while (pos != NULL) 
  {
    CView* pView = pDoc->GetNextView(pos); 
    if (pView->IsKindOf(RUNTIME_CLASS(CBktHistView)))
    { 
      CBktHistView* pBHV = (CBktHistView*)pView;
      pBHV->UpdateScrollSize();
      pBHV->Invalidate();
      pBHV->UpdateWindow();
    }
  }
}

int CBktHistView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

  //DM-7978-YI-5849
	CRect rect;
	GetClientRect(&rect);
	rect.bottom = rect.top + IDC_TC_SIZE;
  m_tcGlobRed.Create( CTA_TAB_STYLE,
											rect, this, IDC_TC_GLO_RED );

  TC_ITEM	tcItem;
  CString s;

  tcItem.mask = TCIF_TEXT;

  s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TC_GLO);
  tcItem.pszText = (LPSTR)(LPCSTR)s;
  tcItem.cchTextMax = s.GetLength();
  m_tcGlobRed.InsertItem (0, &tcItem);

  s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TC_RED);
  tcItem.pszText = (LPSTR)(LPCSTR)s;
  tcItem.cchTextMax = s.GetLength();
  m_tcGlobRed.InsertItem (1, &tcItem);
  //End DM-7978-YI-5849

  SetScrollSizes(MM_TEXT, CSize(0, 0));

  return 0;
}

void CBktHistView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
  CSplitterWnd* pSplitterWnd = ((CSplitChildFrame*)GetParentFrame())->GetSplitterWnd();
  int iColCount = pSplitterWnd->GetColumnCount( );
  if ( (iColCount != m_bColCount) && iColCount > 1 )
  {
    m_bColCount = iColCount;

    YM_RecordSet* pBktHistSet = YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHIST_ALT_KEY : BKTHIST_KEY);
    pBktHistSet->Close();
    pBktHistSet->Open();

    if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
    {
      YM_RecordSet* pBktHistoSet = YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHISTO_ALT_KEY : BKTHISTO_KEY);
      pBktHistoSet->Close();
      pBktHistoSet->Open();
    }
  }

  CClientDC dc(this);
  OnPrepareDC(&dc);
}

void CBktHistView::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult)
{
  *pResult = 0;
	CtReload();
}

void CBktHistView::CtReload()
{
	int idx = m_tcGlobRed.GetCurSel();

	CBktHistView *pCBktHistView				= ((CMainFrame*) AfxGetApp()->GetMainWnd())->GetCBktHistView();
	CBktHistAltView *pCBktHistAltView = ((CMainFrame*) AfxGetApp()->GetMainWnd())->GetCBktHistAltView();
	CResaRailView		*pCResaRailView		= ((CMainFrame*) AfxGetApp()->GetMainWnd())->GetResaRailView();

	if (GetAltFlag())
	{
		if (pCBktHistView != NULL)
		{
			pCBktHistView->ChangeSelTC(idx);

			CBktHistView* otherView = pCBktHistView->getOtherView();
			if(otherView != NULL)
				otherView->ChangeSelTC(idx);
		}
	}
	else
	{
		if (pCBktHistAltView != NULL)
		{
			pCBktHistAltView->ChangeSelTC(idx);
			CBktHistView* otherAltView = pCBktHistAltView->getOtherView();
			if(otherAltView != NULL)
				otherAltView->ChangeSelTC(idx);
		}
	}

	if (pCResaRailView != NULL)
		pCResaRailView->ChangeSelTC(idx);

	m_flagUpdated = TRUE;
	Invalidate(TRUE);
	UpdateWindow();
}

void CBktHistView::OnShowAllJum()
{
  m_BktHistDateMap.DisplayAllJum(TRUE);
}

void CBktHistView::OnMaskAllJum()
{
  m_BktHistDateMap.DisplayAllJum(FALSE);
}

void CBktHistView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  CMenu menu;
  menu.LoadMenu(IDR_CTA_BKTHIST_POP);
  menu.EnableMenuItem(ID_BKTHIST_SHOWJUM, m_BktHistDateMap.CanDisplayAll(TRUE) ?
    MF_ENABLED : MF_DISABLED | MF_GRAYED);
  menu.EnableMenuItem(ID_BKTHIST_MASKJUM, m_BktHistDateMap.CanDisplayAll(FALSE) ?
    MF_ENABLED : MF_DISABLED | MF_GRAYED);

  menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CBktHistView::OnRButtonDown(UINT nFlags, CPoint point)
{
  if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetInfluentFlag())
  {
    CPoint pp = point;

    ClientToScreen(&pp);
    OnContextMenu(this, pp);
  }
  else
  {
    CScrollView::OnRButtonDown(nFlags, point);
  }
}

void CBktHistView::OnDestroy()
{
  CView::OnDestroy();

  YM_PersistentChildWnd* pParentFrame = (YM_PersistentChildWnd*)GetParentFrame();
  if (pParentFrame == NULL)
    return;

  // if we have another CBktHistView instance, set it as the child view of the parent frame !
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  CView* pView; 
  POSITION pos = pDoc->GetFirstViewPosition();
  while (pos != NULL) 
  {
    pView = pDoc->GetNextView(pos); 
    // do we have a spliiter window activated (i.e., 2 views ?)
    if (pView->IsKindOf(RUNTIME_CLASS(CBktHistView)) && pView != this) 
    {
	  /*ano 147777*/ CBktHistView* pbktview = (CBktHistView*)pView;
      pParentFrame->SetChildView(pbktview);
      break;
    }
  }

  // If this destroy is not consecutive to the Frame Destroy, this
  // means that we are simply closing one pane of the splitter window
  // So we don't have to delete the recordsets
  if (! pParentFrame->OnDeletion())
  {
    return;
  }

  CBktHistView* pOtherView = getOtherView();
  
  YM_RecordSet* pBktHistSet = 
    YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHIST_ALT_KEY : BKTHIST_KEY); 
  ASSERT(pBktHistSet != NULL);
  pBktHistSet->RemoveObserver (this);
  if (pOtherView)
    pBktHistSet->RemoveObserver (pOtherView);
  pBktHistSet->Close();

  if (!GetAltFlag())
  { //Alt = SCI, pas de prevauth pour les SCI
    YM_RecordSet* pPrevAuthSet = 
      YM_Set<YmIcBktHistDom>::FromKey(PREVAUTH_KEY); 
    ASSERT(pPrevAuthSet != NULL);
    pPrevAuthSet->RemoveObserver (this);
    if (pOtherView)
      pPrevAuthSet->RemoveObserver (pOtherView);
    pPrevAuthSet->Close();
  }

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRRDHistoryFlag() )
  {
	YM_RecordSet* pBktHistoSet = 
      YM_Set<YmIcBktHistDom>::FromKey((GetAltFlag()) ? BKTHISTO_ALT_KEY : BKTHISTO_KEY);
    ASSERT(pBktHistoSet != NULL);
	pBktHistoSet->RemoveObserver (this);
	if (pOtherView)
      pBktHistoSet->RemoveObserver (pOtherView);
    pBktHistoSet->Close();
  }

	// DM-7978 
/*
	YM_RecordSet* pUserCC = YM_Set<YmIcUserCCDom>::FromKey(CC_USER_KEY);
  ASSERT(pUserCC != NULL);  
  pUserCC->Close();
	pUserCC->RemoveObserver(this);
	*/
	// DM-7978 

}

void CBktHistView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
  // TODO: Add your specialized code here and/or call the base class
  
  CScrollView::OnBeginPrinting(pDC, pInfo);
}

void CBktHistView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
  // TODO: Add your specialized code here and/or call the base class
  
  CScrollView::OnEndPrinting(pDC, pInfo);
}

BOOL CBktHistView::OnPreparePrinting(CPrintInfo* pInfo) 
{
  return DoPreparePrinting(pInfo);
  
  // return CScrollView::OnPreparePrinting(pInfo);
}

void CBktHistView::OnUpdatePrint(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
}

//DM 6496 remise à disposition
// construit une string avec liste de cmpt/tranche/nest_level/date
// devant être verifiés dans sc_prevauth.     
// Si bHist, on ne ramene que les données venant de la base historique.
// exemple A',1234,5,TO_DATE('2008/06/24','YYYY/MM/DD')),('B',1234,6,TO_DATE('2008/06/24','YYYY/MM/DD
// Cette string sert d'entree à la requete IDS_SQL_PREVAUTH. Il y aura ajout de quote en début et fin par le dte
// Les doubles parentheses de début et fin sont dans la requete afin que le premier et dernier caractère de
// la string insérée dans la requête soient des simple quote
// Ne renvoi pas de string vide si rien, mais une série de valeurs bidon pour passer la requête
CString CBktHistView::GetTDforRAD()
{
  CString rtn, ss;

  POSITION pos = m_BktHistDateMap.GetStartPosition();
  while (pos)
  {
    long lDate;
    CBktHistCmptMap* pBktHistCmptMap;
    CString s, sKey;
    m_BktHistDateMap.GetNextAssoc(pos, lDate, pBktHistCmptMap);
	if (pBktHistCmptMap)
	{
	  POSITION pos2 = pBktHistCmptMap->GetStartPosition();
      CString sCmpt;
	  CBktHistBktArray* pBktHistBktArray;
	  YmIcBktHistDom* pBktHist;
	  while (pos2 != NULL)
      { 
	    pBktHistCmptMap->GetNextAssoc(pos2, sCmpt, pBktHistBktArray);
		for (int i = 0; i < pBktHistBktArray->GetSize(); i++)
	    {
	      pBktHist = pBktHistBktArray->GetAt(i);
		  if (pBktHist->AuthCur() == 0) 
		  {
			  RWDate dada (pBktHist->DptDate());
			  ss.Format ("%s',%d,%d,TO_DATE('%s','YYYY/MM/DD",
				  pBktHist->Cmpt(), pBktHist->TrancheNo(), pBktHist->NestLevel(),
				  dada.asString("%Y/%m/%d"));
			  if (!rtn.IsEmpty())
			    rtn += "')),('";
			  rtn += ss;
		  }
	    }
	  }
	}
  }
  if (rtn.IsEmpty())
    rtn = "Z',0, 0, TO_DATE('2000/01/01','YYYY/MM/DD";
  return rtn;
}

void CBktHistView::MajFromRad(int reqKey)
{
  if (m_bRadDone)
    return;
  YM_Iterator<YmIcBktHistDom>* pIterator = NULL;
  pIterator = YM_Set<YmIcBktHistDom>::FromKey(reqKey)->CreateIterator();
  for (pIterator->First(); !pIterator->Finished(); pIterator->Next())
  {
	YmIcBktHistDom* pBktHist = pIterator->Current();
	// Forcage des histo_ref. inutile si la date est vraiment en historique.
	m_BktHistDateMap.MajOnRad(pBktHist);  
  }
  delete pIterator;
  m_bRadDone = TRUE;
}

void CBktHistView::ChangeSelTC(const int & idx) 
{ 
	int oldIdx = m_tcGlobRed.GetCurSel();
	if (oldIdx != idx)
	{
		m_tcGlobRed.SetCurSel(idx);
		CtReload();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBktHistCommentIcon

CBktHistCommentIcon::CBktHistCommentIcon(HICON hicon, CString comment)
{
  m_Commentaire = comment;
  m_hIcon = hicon;
}

CBktHistCommentIcon::~CBktHistCommentIcon()
{
}


BEGIN_MESSAGE_MAP(CBktHistCommentIcon, CWnd)
  //{{AFX_MSG_MAP(CBktHistCommentIcon)
  ON_WM_LBUTTONDOWN()
  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBktHistCommentIcon message handlers

void CBktHistCommentIcon::OnLButtonDown(UINT nFlags, CPoint point)
{
  CString titi = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMMENT_TITLE);
  MessageBox(m_Commentaire,  titi, MB_APPLMODAL);

  CWnd::OnLButtonDown(nFlags, point);
}


void CBktHistCommentIcon::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  
  int prevmode = dc.SetMapMode(MM_TEXT);
  CRect rect;
  GetClientRect(rect);
  dc.FillSolidRect(rect, GetSysColor(COLOR_MENU));
  DrawIconEx(dc.m_hDC, 0, 0, m_hIcon, iBitMapSizeConst, iBitMapSizeConst, 0, NULL, DI_NORMAL);
  dc.SetMapMode(prevmode); 
  // Do not call CStatic::OnPaint() for painting messages
}


/////////////////////////////////////////////////////////////////////////////
// CBktHistJumLink

CBktHistJumLink::CBktHistJumLink(HICON hicon, long jumno, unsigned long dptdate,
               const char* legorig, CBktHistView* papa)
{
  m_hIcon = hicon;
  m_jumno = jumno;
  m_dptdate = dptdate;
  m_legorig = legorig;
  m_pBktJumHistSet = NULL;
  m_papa = papa;
  m_rKey = 0;
}

CBktHistJumLink::~CBktHistJumLink()
{
  if (m_pBktJumHistSet)
  {
    delete m_pBktJumHistSet;
    m_pBktJumHistSet = NULL;
  }
}

void CBktHistJumLink::OnNotify(YM_Observable* pObject)
{
  if (pObject->IsKindOf(RUNTIME_CLASS(YM_RecordSet)))
  {
    YM_RecordSet* pRecordSet = (YM_RecordSet*)pObject;

    if (pRecordSet == m_pBktJumHistSet) 
    {
      if (m_pBktJumHistSet->IsValid())
      {
        if (m_rKey)
        { // Le systeme qui reessaye 3 trois fois une requete si elle ne
          // retourne rien, fait que l'on peut être notifié plusieurs fois
          // La technique de remise a "" de m_rKey pour savoir si la
          // requete a ete traite ou non n'est donc pas valable. D'ou cette
          // protection
          YM_Iterator<YmIcBktHistDom>* pVariablesIterator =
            YM_Set<YmIcBktHistDom>::FromKey(m_rKey)->CreateIterator();
          YmIcBktHistDom* pVarGen;

          if (pVariablesIterator)
          {
            for (pVariablesIterator->First(); !pVariablesIterator->Finished(); pVariablesIterator->Next())
            {
              pVarGen = (YmIcBktHistDom*)pVariablesIterator->Current();
              pVarGen->TgvJumeauxNo(-1);
		      if (pVarGen->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto1())
	            pVarGen->HistoRef(1);
	          else if (pVarGen->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto2())
	            pVarGen->HistoRef(2);
	          else if (pVarGen->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto3())
	            pVarGen->HistoRef(3);
              ((CCTAApp*)APP)->GetTempBktJumHist(m_papa->GetAltFlag())->Add(pVarGen); 
            }
            delete pVariablesIterator;
          }
        }
        m_papa->SetStatusBarText(IDS_DONE);
		if (!m_papa->GetAltFlag()) // La transformation d'autorisation ne concerne que les CC, pas les SCX
		  ((CCTAApp*)APP)->GetTempBktJumHist(m_papa->GetAltFlag())->TakeCoeffHisto(); //DM4965.3
        m_papa->UpdateSisterAlso();
		m_rKey = 0;
      }
      else
        m_papa->SetStatusBarText(IDS_READING_HISTORY);
    }
  }
}

BEGIN_MESSAGE_MAP(CBktHistJumLink, CWnd)
  //{{AFX_MSG_MAP(CBktHistJumLink)
  ON_WM_LBUTTONDOWN()
  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBktHistJumLink::ForceDisplay(BOOL display)
{
  CPoint pt(0, 0);
  CBktHistCmptMap* pBktHistCmptMap = NULL;
  if (((CCTAApp*)APP)->GetTempBktJumHist(m_papa->GetAltFlag())->Lookup(m_dptdate, pBktHistCmptMap))
  { // Ce Jumeau est déja affiché
    if (!display)
      OnLButtonDown(0, pt);  // il faut le fermer
    // Sinon il est deja affiche, donc rien a faire.
  }
  else
  { // Il n'est pas déja affiché
    if (display)
      OnLButtonDown(0, pt);
  }
}

// If display is TRUE, CanDisplay return TRUE if the Twin column is close
// If display is FALSE, CanDisplay return TRUE if the Twin column is opened
BOOL CBktHistJumLink::CanDisplay(BOOL display)
{
  CBktHistCmptMap* pBktHistCmptMap = NULL;
  BOOL dejaaff;
  dejaaff = (((CCTAApp*)APP)->GetTempBktJumHist(m_papa->GetAltFlag())->Lookup(m_dptdate, pBktHistCmptMap));
  if (display && !dejaaff)
    return TRUE;
  if (!display && dejaaff)
    return TRUE;
  return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// CBktHistJumLink message handlers

void CBktHistJumLink::OnLButtonDown(UINT nFlags, CPoint point)
{
  if (!m_papa)
    return;
  if (m_rKey)
    return;   // tout est interdit tant que l'on attend la notification

  CBktHistCmptMap* pBktHistCmptMap = NULL;
  if (((CCTAApp*)APP)->GetTempBktJumHist(m_papa->GetAltFlag())->Lookup(m_dptdate, pBktHistCmptMap))
  { // Ce Jumeau est déja affiché, donc on le détruit
    ((CCTAApp*)APP)->GetTempBktJumHist(m_papa->GetAltFlag())->RemoveKey(m_dptdate);
    if (pBktHistCmptMap)
    {
      delete pBktHistCmptMap;
      pBktHistCmptMap = NULL;
    }
    if (m_pBktJumHistSet)
    {
      delete m_pBktJumHistSet;
      m_pBktJumHistSet = NULL;
    }
    m_papa->UpdateSisterAlso();
    return;
  }

  if (m_pBktJumHistSet)
  { 
    // Le recordSet existe, la notification a été faite, mais il n'y a rien d'affiche
    // Ceci signifie que la colonne a ete detruite depuis l'autre pane, donc cela
    // ne sert a rien de garder le record Set
    delete m_pBktJumHistSet;
    m_pBktJumHistSet = NULL;
  }
  else // On va lancer la requete.
  {
    g_bktjumkey++;
    char zkey[30];
    itoa(g_bktjumkey, zkey, 10);
    m_rKey = BKT_JUM_QUERY_KEY;
    m_rKey += g_bktjumkey;
    m_pBktJumHistSet = new CBktHistJumSet(App().m_pDatabase, m_jumno, m_dptdate, m_legorig);
	if (m_papa->GetAltFlag())
	{
	  m_pBktJumHistSet->SetTransIdSelect(IDS_SQL_SELECT_BKT_JUM_SCI);
	}
    m_pBktJumHistSet->SetKey(m_rKey);
    m_pBktJumHistSet->AddObserver(this);
    m_pBktJumHistSet->Open();
  }


  // CWnd::OnLButtonDown(nFlags, point);
}


void CBktHistJumLink::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  
  int prevmode = dc.SetMapMode(MM_TEXT);
  CRect rect;
  GetClientRect(rect);
  dc.FillSolidRect(rect, GetSysColor(COLOR_MENU));
  DrawIconEx(dc.m_hDC, 0, 0, m_hIcon, iBitMapSizeConst, iBitMapSizeConst, 0, NULL, DI_NORMAL);
  dc.SetMapMode(prevmode); 
}

