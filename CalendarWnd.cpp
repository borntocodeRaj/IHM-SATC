// CalendarView implementation file
#include "StdAfx.h"

#include "CTA_Resource.h"

#include "TrancheView.h"
#include "CalendarWnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructeur appelée depuis la sélection mono ou multidate
// mm1,yy1 mois année début, mm2,yy2 mois année fin, yy sur 4 digits
CCalendarWnd::CCalendarWnd(int mm1, int yy1, int mm2, int yy2) 
:YMCalendar (mm1, yy1, mm2, yy2)
{
  SetImageList (IDB_STATUS_BITMAP);
  m_choix1 = CCalendarWnd::UNDEFINED;
  m_choix2 = CCalendarWnd::UNDEFINED;

  // j'utilise le domaine conges (DATE_DEB, DATE_FIN) pour récupérer, dpt_date min et max de la sc_tranches
  // lue pendant la phase d'initialisation, la donnée doit être prête.
  YM_Iterator<YmIcCongesDom>* pIterator = YM_Set<YmIcCongesDom>::FromKey( MINMAX_DPT_KEY)->CreateIterator();
  pIterator->First();  // il n'y a qu'une ligne dans le recordset
  YmIcCongesDom* pConges = (YmIcCongesDom*)pIterator->Current();
  //m_minDate = pConges->DateDeb();
  // 22/10/10 La date minimale n'est plus disponible dans YM_TD, car cette table est purgée 7 j après départ.
  // côté historique on garde les données 5 ans. C'est trop pour le calendrier.
  // on fixe la date minimale = aujourd'hui - 380 jours (1 an et 15 jours)
  RWDate today;
  m_minDate = today.julian() - 380;
  m_maxDate = pConges->DateFin();
  SetSelMode(DBLCLICK_SEL);
  m_szClient = App().m_pDatabase->GetClientParameter(); 
}

CCalendarWnd::~CCalendarWnd()
{
  PurgeTrancheResa();
}

void CCalendarWnd::SelDone()
{
}

bool CCalendarWnd::DblClickDone()
{
  YMDay* pDay1;
  pDay1 = GetSelDay1();
  YM_RecordSet* pCalendarTrainSet = 
    YM_Set<YmIcTrancheLisDom>::FromKey( CALENDAR_TRAIN_KEY);
  if (!pCalendarTrainSet->IsValid())
    return false;

  RWDate laDate (pDay1->Getdd(), pDay1->Getmm(), pDay1->Getyy());
  unsigned long juldate = laDate.julian();

  if (juldate > m_maxDateTranche)
    return false;
  if (juldate < m_minDateTranche)
    return false;

  YM_Iterator<YmIcTrancheLisDom>* pIterator = YM_Set<YmIcTrancheLisDom>::FromKey( CALENDAR_TRAIN_KEY)->CreateIterator();
  YmIcTrancheLisDom* pTrancheRecord = NULL;
  for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
  {
    pTrancheRecord = (YmIcTrancheLisDom*)pIterator->Current();
	if (pTrancheRecord->DptDate() == juldate)
	  break;  
  }
  delete pIterator;

  if (!pTrancheRecord || (pTrancheRecord->DptDate() != juldate))
    return false;

  CTrancheSet* pTrancheSet = (CTrancheSet*)YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
  bool bf = false;
  YM_Iterator<YmIcTrancheLisDom>* pIterator2 = YM_Set<YmIcTrancheLisDom>::FromKey( TRANCHE_KEY)->CreateIterator();
  for( pIterator2->First(); !pIterator2->Finished(); pIterator2->Next() ) 
  {
    YmIcTrancheLisDom* pTrancheLis = (YmIcTrancheLisDom*)pIterator2->Current();
	if ((pTrancheLis->TrancheNo() == pTrancheRecord->TrancheNo()) &&
		(pTrancheLis->DptDate() == pTrancheRecord->DptDate()))
	{
	  pTrancheView->LoadTranche(pTrancheLis);
	  bf = true;
	  break;
	}
  }
  delete pIterator2;
  if (!bf)
  {
    // il faut récupérer les infos manquantes par rapport à la liste des tranches
	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
    YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
	CString szClient = App().m_pDatabase->GetClientParameter();
	YmIcTrancheLisDom TrancheDom;
	TrancheDom.TrancheNo(pTrancheRecord->TrancheNo());
	TrancheDom.DptDate(pTrancheRecord->DptDate());
	TrancheDom.LastJX(pTrancheRecord->LastJX());
	TrancheDom.TrancheOrig(pTrancheRecord->TrancheOrig());
	pSQL->SetDomain(&TrancheDom);
	RWDBReader Reader( WinApp->m_pDatabase->Transact(pSQL, IDS_SQL_CALENDAR_MISSING) );
    Reader(); // la requête ne retourne qu'une ligne
    if (Reader.isValid())
	{
	  Reader >> TrancheDom;
	  pTrancheRecord->Entity(TrancheDom.Entity());
	  pTrancheRecord->TrancheOrigDesc(TrancheDom.TrancheOrigDesc());
	  pTrancheRecord->TrancheDestDesc(TrancheDom.TrancheDestDesc());
	  pTrancheRecord->TgvJumTrain(TrancheDom.TgvJumTrain());
	  pTrancheRecord->ScxCapa(TrancheDom.ScxCapa());
	  pTrancheRecord->ScxCount(TrancheDom.ScxCount());
	  pTrancheRecord->ScxEtanche(TrancheDom.ScxEtanche());
	  pTrancheRecord->DptDateRefIsNull(TrancheDom.DptDateRefIsNull());
	  pTrancheRecord->DptDateRef(TrancheDom.DptDateRef());
	}
    YmIcTrancheLisDom *ppTr = new YmIcTrancheLisDom (*pTrancheRecord);
    pTrancheSet->Add(ppTr);
	pTrancheView->AddInList(ppTr);
	pTrancheView->LoadTranche(ppTr);
	delete pSQL;
  }
  return true;
}

COLORREF CCalendarWnd::GetBkColor(YMDay* pDay)
{
  RWDate laDate (pDay->Getdd(), pDay->Getmm(), pDay->Getyy());
  unsigned long juldate = laDate.julian();
  if( ((CCTAApp*)APP)->GetDocument()->m_pSeasonsSet->IsValid() ) 
  {
    YM_Iterator<YmIcSeasonDom>* pIterator = YM_Set<YmIcSeasonDom>::FromKey( SEASONS_KEY)->CreateIterator();
    for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
    {
      YmIcSeasonDom* pSeason = (YmIcSeasonDom*)pIterator->Current();
	  if ((juldate >= pSeason->StartDate()) && (juldate <= pSeason->EndDate()))
	  {
	    char sc = pSeason->Season()[0];
	    switch (sc)
		{
		  case 'A' : delete pIterator; return RGB (255,220,255); // rose
		  case 'B' : delete pIterator; return RGB (255,255,200); // jaune
		  case 'C' : delete pIterator; return RGB (220,255,255); // bleu
		  case 'D' : delete pIterator; return RGB (220,255,220); // vert
		  default : delete pIterator; return RGB (220,220,220);
		}
	  }
	}
	delete pIterator;
	return RGB (255, 255, 255);
  }
  else
    return RGB (255, 255, 255);
}

CString CCalendarWnd::GetConges(YMDay* pDay)
{
  RWDate laDate (pDay->Getdd(), pDay->Getmm(), pDay->Getyy());
  unsigned long juldate = laDate.julian();
  CString sc;
  if( ((CCTAApp*)APP)->GetDocument()->m_pCongesSet->IsValid() ) 
  {
    YM_Iterator<YmIcCongesDom>* pIterator = YM_Set<YmIcCongesDom>::FromKey( CONGES_KEY)->CreateIterator();
    for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
    {
      YmIcCongesDom* pConges = (YmIcCongesDom*)pIterator->Current();
	  if ((juldate >= pConges->DateDeb()) && (juldate <= pConges->DateFin()))
	    sc += pConges->Zone()[0];
	}
	delete pIterator;
	return sc;
  }
  else
    return sc;
}


char CCalendarWnd::GetSpecialChar(YMDay* pDay, CString& comment)
{
  RWDate laDate (pDay->Getdd(), pDay->Getmm(), pDay->Getyy());
  unsigned long juldate = laDate.julian();

  if (juldate > m_maxDateTranche)
    return false;
  if (juldate < m_minDateTranche)
    return false;

  int reqKey = (juldate >= m_minDateOp) ? CALENDAR_TRAIN_KEY : CALENDAR_PAST_TRAIN_KEY;

  YM_RecordSet* pCalendarTrainSet = YM_Set<YmIcTrancheLisDom>::FromKey( reqKey);
  if (!pCalendarTrainSet->IsValid())
    return 0;

  YM_Iterator<YmIcTrancheLisDom>* pIterator = YM_Set<YmIcTrancheLisDom>::FromKey( reqKey)->CreateIterator();
  YmIcTrancheLisDom* pTrancheRecord = NULL;
  for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
  {
    pTrancheRecord = (YmIcTrancheLisDom*)pIterator->Current();
	if (pTrancheRecord->DptDate() == juldate)
	  break;  
  }
  delete pIterator;

  if (!pTrancheRecord || (pTrancheRecord->DptDate() != juldate))
    return 0;

  CString sEntityRrdList = "'" + ((CCTAApp*)APP)->GetDocument()->GetCurRrdList() + "'";
  // je remeet les quotes en début et fin pour tester le last_j_x avec des quotes autour
  CString sr;
  sr.Format ("'%d'", pTrancheRecord->LastJX());
  if (sEntityRrdList.Find (sr) >= 0)
    return '+';
  else
    return 0;
}

bool CCalendarWnd::IsUnusedDate (YMDay* pDay, COLORREF& color)
{
  color = RGB(192,192,192);
  RWDate laDate (pDay->Getdd(), pDay->Getmm(), pDay->Getyy());
  unsigned long juldate = laDate.julian();

  if (juldate > m_maxDate)
    return false;
  if (juldate < m_minDate)
    return false;

  int reqKey = (juldate >= m_minDateOp) ? CALENDAR_TRAIN_KEY : CALENDAR_PAST_TRAIN_KEY;

  YM_RecordSet* pCalendarTrainSet = YM_Set<YmIcTrancheLisDom>::FromKey( reqKey);
  if (!pCalendarTrainSet->IsValid())
    return 0;

  YM_Iterator<YmIcTrancheLisDom>* pIterator = YM_Set<YmIcTrancheLisDom>::FromKey( reqKey)->CreateIterator();
  YmIcTrancheLisDom* pTrancheRecord = NULL;
  for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
  {
    pTrancheRecord = (YmIcTrancheLisDom*)pIterator->Current();
	if (pTrancheRecord->DptDate() == juldate)
	  break;  
  }
  delete pIterator;
  if (!pTrancheRecord)
    return false;
  if (pTrancheRecord->DptDate() == juldate)
    return false;
  return true;
}

bool CCalendarWnd::GetCursor (YMDay* pDay, int& val1, int& val2, int& full, CString& sInfo,
							  COLORREF& color1, COLORREF& color2, COLORREF& bckColor)
{
  if ((m_choix1 != CCalendarWnd::TAUX_PREMIERE) &&
	  (m_choix1 != CCalendarWnd::TAUX_SECONDE))
    return false;

  sInfo.Empty();
  RWDate laDate (pDay->Getdd(), pDay->Getmm(), pDay->Getyy());
  unsigned long juldate = laDate.julian();

  if (juldate > m_maxDateTranche)
    return false;
  if (juldate < m_minDateTranche)
    return false;

  val1 = -1;
  val2 = -1;
  full = -1;
  int j1 = -1;
  int j2 = -1;
  CString sCmpt;
  int capa;
  int reshold;
  int qcm = (m_choix1 == CCalendarWnd::TAUX_PREMIERE) ? 1 : 2;

  YmIcHistoEvolDom* pRecord = NULL;

  YmIcTrancheLisDom* pTranche = 
    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
  CObArray *pResaDays;
  WORD tno = static_cast<WORD>(pTranche->TrancheNo());
  if (AllResaTranchesMap.Lookup(tno, (CObject*&) pResaDays))
  {
    int i;
	for (i = 0; i < pResaDays->GetCount(); i++)
	{
	  pRecord = (YmIcHistoEvolDom*)pResaDays->GetAt(i);
	  if (pRecord)
	  {
	    if (pRecord->DptDate() == juldate)
	    {
	      if (FindInfoPnData (pRecord->PnData(), pRecord->NbNest(), sCmpt, capa, reshold, qcm))
	      {
		    if (val1 == -1)
		    {
		      val1 = reshold;
		      full = capa;
		      j1 = pRecord->RrdIndex();
		    }
		    else if (val2 == -1)
		    {
		      val2 = reshold;
		      j2 = pRecord->RrdIndex();
			}
		  }
		}
	    else if (pRecord->DptDate() > juldate)
	      break;
	  }
	}
  }

  if (val1 == -1)
    return false;

  if (j1 == -1) // apres départ
	sInfo.Format("%s cap:%d, resa:%d", sCmpt, full, val1);
  else
  {
	if ( val2 != -1)
	  sInfo.Format ("%s, cap:%d, resa à j-%d:%d, à j-%d:%d", sCmpt, full, j2, val2, j1, val1);
	else
	  sInfo.Format ("%s, cap:%d, resa à j-%d:%d", sCmpt, full, j1, val1);
  }
  
  if (m_choix1 == CCalendarWnd::TAUX_PREMIERE)
  {
    color1 = color_A1;
	color2 = color_A2;
  }
  else
  {
    color1 = color_B1;
	color2 = color_B2;
  }
  bckColor = color_Full;
  return true;
}

bool CCalendarWnd::GetDisplayInfo (YMDay* pDay, int col, int& bmpId, CString& sBuffer,
								   CString& sInfo, bool& bital, bool& bbold, COLORREF& color, COLORREF& bckcolor)
{
  bmpId = -1;
  sBuffer.Empty();
  sInfo.Empty();
  bital = false;
  bbold = false;
  color = RGB(0,0,0);
  RWDate laDate (pDay->Getdd(), pDay->Getmm(), pDay->Getyy());
  unsigned long juldate = laDate.julian();

  if (juldate > m_maxDateTranche)
    return false;
  if (juldate < m_minDateTranche)
    return false;

  int reqKey = (juldate >= m_minDateOp) ? CALENDAR_TRAIN_KEY : CALENDAR_PAST_TRAIN_KEY;

  YM_RecordSet* pCalendarTrainSet = YM_Set<YmIcTrancheLisDom>::FromKey( reqKey);
  if (!pCalendarTrainSet->IsValid())
    return 0;

  YM_Iterator<YmIcTrancheLisDom>* pIterator = YM_Set<YmIcTrancheLisDom>::FromKey( reqKey)->CreateIterator();
  YmIcTrancheLisDom* pTrancheRecord = NULL;
  for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
  {
    pTrancheRecord = (YmIcTrancheLisDom*)pIterator->Current();
	if (pTrancheRecord->DptDate() == juldate)
	  break;  
  }
  delete pIterator;

  if (!pTrancheRecord || (pTrancheRecord->DptDate() != juldate))
    return false;

  if (col == 3)
  { // signal d'alerte sur tranche fermée
	if (pTrancheRecord->NotOpenFlag())
	{
	  bmpId = BMP_REVIEW_NOGO;
	  return true;
	}
	else
	  return false;
  }

  choixAffichage choix;
  if (col == 1)
    choix = m_choix1;
  else if (col == 2)
    choix = m_choix2;
  else
	choix = CCalendarWnd::UNDEFINED;

  if (choix == CCalendarWnd::UNDEFINED)
    return false;

  switch (choix)
  {
    case CCalendarWnd::TOP:
	{ 
	  sBuffer = pTrancheRecord->TopRR();
	  if (sBuffer.IsEmpty())
	    sBuffer = "- ";
	  else
	  {
	    long tt = atoi ((LPCSTR)sBuffer);
	    sInfo = pTrancheRecord->TopDescription();
	    if (tt < 10)
	    { bckcolor = color_t0; }
	    else if (tt < 20)
	    { bckcolor = color_t1; }
	    else if (tt < 30)
	    { bckcolor = color_t2; }
	    else if (tt < 40)
	    { bckcolor = color_t3; }
	    else if (tt < 50)
	    { bckcolor = color_t4; }
	    else if (tt < 60)
	    { bckcolor = color_t5;
		  color = RGB(255,255,255); } // rouge sombre, j'écris en blanc
	    else if (tt < 70)
	    { bckcolor = color_t6; }
	    else if (tt < 80)
	    { bckcolor = color_t7; }
	  }
	  return true;
	}
	case CCalendarWnd::COMMENT: 
	{
	  sInfo = pTrancheRecord->CommentText();
	  if (sInfo.GetLength())
	    bmpId = BMP_COMMENT;
	  else
	    sBuffer = "- ";
	  return true;
	}
	case CCalendarWnd::VNAU:
	{ 
	  RWDate today;
	  YmIcDcDateDom* pDCDate = YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY);
	  if (m_szClient == THALYS)
	  { // pas de DM 6850, on affiche la même chose que dans la liste des tranches et
	    // donc uniquement pour les reading days.
	    if (pTrancheRecord->LastDate() == pDCDate->DcDate())
		{ // c'est un reading day
		  switch (pTrancheRecord->SentFlag())
		  {
		    case 1 : bmpId = BMP_SENT; break;
	        case 2 : bmpId = BMP_SENT_COMMERCIAL; break;
		    case 3 : bmpId = BMP_SENT_USER_ET_COMMERC; break;
		    case 4 : bmpId = BMP_SENT_HRRD; break;
		    case 5 : bmpId = BMP_SENT_USER_ET_HRRD; break;
		    case 6 : bmpId = BMP_SENT_COMMERC_ET_HRRD; break;
		    case 7 : bmpId = BMP_SENT_U_C_H; break;
		    case 8 : bmpId = BMP_SENT_NOMOS; break;
		    case 9 : bmpId = BMP_SENT_PLUS_NOMOS; break;
		    case 10 : bmpId = BMP_SENT_COMMER_NOMOS; break;
		    case 11 : bmpId = BMP_SENT_USER_COMMER_NOMOS; break;
		    case 12 : bmpId = BMP_SENT_HRRD_NOMOS; break;
		    case 13 : bmpId = BMP_SENT_USER_HRRD_NOMOS; break;
		    case 14 : bmpId = BMP_SENT_COMMER_HRRD_NOMOS; break;
		    case 15 : bmpId = BMP_SENT_U_C_H_N; break;
		    default : sBuffer = "- ";
		  }
		}
	    return true;
	  }
	  if ((pTrancheRecord->LastDate() == pDCDate->DcDate())
		  && (pTrancheRecord->SentFlag() & 1))
	   // envoi de VNAU utilisateur depuis le dernier batch
	    bmpId = BMP_SENT;
	  else
	  {
	     YM_RecordSet* pCalendarTDLJKSet = 
           YM_Set<YmIcTDLJKDom>::FromKey( CALENDAR_TDLJK_KEY);
         if (pCalendarTDLJKSet->IsValid())
		 {
		   YM_Iterator<YmIcTDLJKDom>* pIterator = YM_Set<YmIcTDLJKDom>::FromKey( CALENDAR_TDLJK_KEY)->CreateIterator();
           YmIcTDLJKDom* pTDLJKRecord = NULL;
		   int mjx = 0;
		   int kk = 0;
		   CString limess;
           for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
           {
             pTDLJKRecord = (YmIcTDLJKDom*)pIterator->Current();
			 if ((pTDLJKRecord->DptDate() == juldate) && (!strcmp (pTDLJKRecord->TypeRegle(), "O")) &&
				 (pTDLJKRecord->DptDate() - pTDLJKRecord->J_x() >= today.julian() - 7) &&
				 (pTDLJKRecord->DptDate() - pTDLJKRecord->J_x() < today.julian()) )
			 {
			   kk = today.julian() - pTDLJKRecord->DptDate() + pTDLJKRecord->J_x(); // entre 1 et 7
			   if (mjx == 0)
				 mjx = kk; // la requête est ordonnée sur dpt_date, j-x. le premier est le + petit
			   if (mjx == kk)
			   { // On affiche les messages VNAU commerciaux que sur la dernière nuit où il y en a eu.
			     RWDate dada (pTDLJKRecord->DptDate() - pTDLJKRecord->J_x());
				 limess.Format ("%s: %02d %s %s", dada.asString ("%d/%m/%y"), pTDLJKRecord->Criticality(),
					                                 pTDLJKRecord->LegOrig(), pTDLJKRecord->Message());
			     if (!sInfo.IsEmpty())
			       sInfo += "\r\n";
			     sInfo += limess;
			   }
			 }
           }
           delete pIterator;
		   if ( mjx)
		     bmpId = BMP_VNAU_M1J - mjx + 1;
		   else
		     sBuffer = "- ";
		 }
		 else
		   sBuffer = "- ";
	  }
	    
	  return true;
	} 
	case CCalendarWnd::CRITIC:
	{ 
	  YM_RecordSet* pCalendarTDLJKSet = YM_Set<YmIcTDLJKDom>::FromKey( CALENDAR_TDLJK_KEY);
      if (pCalendarTDLJKSet->IsValid())
	  {
		YM_Iterator<YmIcTDLJKDom>* pIterator = YM_Set<YmIcTDLJKDom>::FromKey( CALENDAR_TDLJK_KEY)->CreateIterator();
        YmIcTDLJKDom* pTDLJKRecord = NULL;
		int mjx = 0;
		int firstcrit = -1;
		CString limess;
        for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
        {
          pTDLJKRecord = (YmIcTDLJKDom*)pIterator->Current();
		  if ((pTDLJKRecord->DptDate() == juldate) && (!strcmp (pTDLJKRecord->TypeRegle(), "I")))
		  {
			if (mjx == 0)
			  mjx = pTDLJKRecord->J_x(); // la requête est ordonnée sur dpt_date, j-x. le premier est le + petit
			if (mjx == pTDLJKRecord->J_x())
			{ // On affiche les messages critiques du dernier traitement uniquement
			  RWDate dada (pTDLJKRecord->DptDate() - pTDLJKRecord->J_x());
			  if (firstcrit == -1)
			  {
			    firstcrit = pTDLJKRecord->Criticality();
				pTrancheRecord->Criticality(firstcrit);
			  }
			  limess.Format ("%s: %02d %s %s", dada.asString ("%d/%m/%y"), pTDLJKRecord->Criticality(),
					                                pTDLJKRecord->LegOrig(), pTDLJKRecord->Message());
			  if (!sInfo.IsEmpty())
			    sInfo += "\r\n";
			  sInfo += limess;
			}
		  }
        }
        delete pIterator;
		if (firstcrit == -1)
		 pTrancheRecord->Criticality(0); 
	  }

	  long cc = pTrancheRecord->Criticality(); 
	  if ( cc == 0)
	  {
	    sBuffer = "- ";
		return true;
	  }
	  YmIcDcDateDom* pDCDate = YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY);
	  if (pTrancheRecord->LastDate() != pDCDate->DcDate())
	    bckcolor = RGB (255,200, 150); // fond orange saumon.
	  else
	    bbold = true; // en gras si rrd.

	  
	  
	    sBuffer.Format("%d", cc);
		if (cc < 11)
	    {
	      color = color_l1;
	    }
	    else if (cc < 21)
	    {
	      color = color_l2;
	    }
	    else if (cc < 31)
	    {
	      color = color_l3;
	    }
	    else
		  color = color_l4;
        
	   
	  return true;
	} 
	case CCalendarWnd::TGVJUM:
	{ 
	  long tj = pTrancheRecord->TgvJumPleinFlag();
	  switch (tj)
	  {
	    case 1 : bmpId = BMP_EDIT_TGVMODSENT; return true;
		case 2 : bmpId = BMP_EDIT_TGVJUM_80PC; return true;
		case 3 : bmpId = BMP_EDIT_TGVJUM_100PC; return true;
		default : sBuffer = "- "; return true;
	  }
	} 
	case CCalendarWnd::AFFAIRE:
	{ 
	  long aa = pTrancheRecord->AffDemande();
	  if (aa)
	  {
	    if (aa < 101)
		{ color = color_l1;}
		else if (aa < 201)
		{ color = color_l2; }
		else if (aa < 300)
		{ color = color_l3; }
		else
		  color = color_l4;
		sBuffer.Format ("%d", aa);
		CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
		CString mess = WinApp->GetResource()->LoadResString(IDS_AFFAIRE_CALENDAR);
		sInfo.Format (mess, pTrancheRecord->AffDemande(), pTrancheRecord->AffAttrib(),
					        pTrancheRecord->AfdAttrib());
		return true;
	  }
	  else
	  {
	    sBuffer = "- ";
        return true;
	  }
	}
	case CCalendarWnd::SERVICE:
	{
	  if (pTrancheRecord->Service() & 1)
	    sBuffer += "o";		//Oshibori
	  if (pTrancheRecord->Service() & 2)
	    sBuffer += "d";		//Disney
	  if (pTrancheRecord->Service() & 4)
	    sBuffer += "f";		//Famille
	  if (sBuffer.IsEmpty())
	    sBuffer = "- ";
	  CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	  CString mess = WinApp->GetResource()->LoadResString(IDS_AFFAIRE_CALENDAR);
	  sInfo.Format (mess, pTrancheRecord->AffDemande(), pTrancheRecord->AffAttrib(),
					        pTrancheRecord->AfdAttrib());
	  return true;
	}
	case CCalendarWnd::NOMOS:
	{
	  if (pTrancheRecord->DebExclu() > 0)
	  {
	     RWDate today;
	     sBuffer.Format( "%s", "MA" );
		 int avdep = pTrancheRecord->DptDate() - today.julian();
		 if (pTrancheRecord->FinExclu() >= avdep)
		   color = RGB (128, 128, 128); // gestion manuelle terminée, affichage en gris
		 else if (pTrancheRecord->DebExclu() >= avdep)
		   color = RGB (255, 0, 0); // gestion manuelle en cours, affichage en rouge.
		 // sinon, gestion manuelle a venir, on laisse l'affichage en noir.
		 CString s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_MANUEL_ENTRE);
		 sInfo.Format (s, pTrancheRecord->DebExclu(), pTrancheRecord->FinExclu());
	  }
	  else
	    sBuffer = "AU";
	  return true;
	}
	case CCalendarWnd::DESSERTE:
	{ 
	  if (pTrancheRecord->DesserteFlag())
	  {
	    bmpId = BMP_DESSERTE_FLAG;
		return true;
	  }
	  else
	  {
	    sBuffer = "- ";
        return true;
	  }
	} 
	case CCalendarWnd::GESTIONOD:
	{ 
	  long god = pTrancheRecord->ODManaged();
	  switch (god)
	  {
	    case 1 : bmpId = BMP_YIELDOD_YOD; return true;
		case 2 : bmpId = BMP_YIELDOD_YT; return true;
		case 3 : bmpId = BMP_YIELDOD_OD; return true;
		default : sBuffer = "- "; return true;
	  }
	} 
	case CCalendarWnd::REVISION:
	{ 
	  long rv = pTrancheRecord->ReviewFlag();
	  switch (rv)
	  {
	    case 1 : bmpId = BMP_REVIEW_REVIEWIT; return true;
		case 2 : bmpId = BMP_REVIEW_NEVER; return true;
		default : sBuffer = "- "; return true;
	  }
	} 
	case CCalendarWnd::NIVEAU:
	{ 
	  // NPI - DM7978 : souplesse tarifaire lot 2
	  /*if (pTrancheRecord->SupplementLevel())
	  {
		sBuffer.Format ("%d", pTrancheRecord->SupplementLevel());
		bbold = true;
		return true;
	  }
	  else
	  {
	    sBuffer = "- ";
        return true;
	  }*/

	  if (pTrancheRecord->NiveauSocial())
	  {
		sBuffer.Format ("%d", pTrancheRecord->NiveauSocial());
		bbold = true;
		return true;
	  }
	  else
	  {
	    sBuffer = "- ";
        return true;
	  }
	  ///
	} 
    case CCalendarWnd::EQUIP:
	{ 
	  CString szClient = App().m_pDatabase->GetClientParameter();
	  sBuffer = pTrancheRecord->CodeEquip();
      if (szClient != SNCF_TRN)
	  { // On affiche que le dernier caractère, c'est suffisant
		sBuffer = sBuffer.Right(1);
		color = RGB (0, 128, 0);
		bital = true;
	  }
	  if (sBuffer.IsEmpty())
	    sBuffer = "- ";
	  // sinon  cas Corail, on affiche tout.
      return true;
	}
	case CCalendarWnd::FORCEMENT:
	{ 
	  long fr = pTrancheRecord->ForceStatus();
	  switch (fr)
	  {
	    case 1 : bmpId = BMP_FORCE_RECOMMEND; return true;
		case 2 : bmpId = BMP_FORCE_ACCEPTED; return true;
		case 3 : bmpId = BMP_FORCE_REFUSED; return true;
		case 4 : bmpId = BMP_FORCE_MODIFIED; return true;
		default :  sBuffer = "- "; return true;
	  }
	}
	case CCalendarWnd::SUPPLEMENTAIRE:
	{ 
	  if (pTrancheRecord->SupFlag())
	  {
	    bmpId = BMP_SUP;
		return true;
	  }
	  else
	  {
	    sBuffer = "- ";
        return true;
	  }
	}
	case CCalendarWnd::TAUX_PREMIERE:
	case CCalendarWnd::TAUX_SECONDE:
	{ // si on passe là avec ce cas, c'est que la fonction GetCursor a renvoyé false
	  sBuffer = "- ";
	  return true;
	}
	
	default : return false;
  }	
}

void CCalendarWnd::ChangeAffichage (choixAffichage chx1, choixAffichage chx2, bool bInvalidate/* = true*/)
{
  m_choix1 = chx1;
  m_choix2 = chx2;
  if (bInvalidate)
	Invalidate();
}

void CCalendarWnd::PositionOnToday()
{
  RWDate today;
  YMDay* pDay = FindDay (today.dayOfMonth(), today.month(), today.year());
  if (pDay)
    EnsureVisible (pDay);
}

bool CCalendarWnd::FindInfoPnData (LPCSTR pndata, int nbnest, CString& sCmpt,
		                 int& capa, int& reshold, int qcm)
{
  CString sData = pndata;
  if (qcm == 1)
    sData = sData.Left (nbnest*22 + 5);
  else
	sData = sData.Mid (nbnest*22 + 5);
  int ll = sData.GetLength();
  if ( ll < 27)
    return false;
  sCmpt = sData.Left(1);
  CString ss = sData.Mid (1,4);
  capa = atoi (ss);
  if (!capa)
    return false;
  int i, k;
  reshold = 0;
  k = 15;
  for (i = 0; i < nbnest; i++)
  {
    if (k+(i*22)+4 < ll)
	{
      ss = sData.Mid(k+(i*22), 4);
	  reshold += atoi (ss);
	}
  }
  return true;
}

void CCalendarWnd::PurgeTrancheResa()
{
  POSITION pos = AllResaTranchesMap.GetStartPosition();
  CObArray *pResaDays;
  WORD tno;
  while (pos != NULL)
  {
	AllResaTranchesMap.GetNextAssoc(pos, tno, (CObject*&)pResaDays);
	if (pResaDays)
	{
	  int i;
	  YmIcHistoEvolDom* pRecord = NULL;
	  for (i = 0; i < pResaDays->GetCount(); i++)
	  {
		pRecord = (YmIcHistoEvolDom*)pResaDays->GetAt(i);
		if (pRecord)
		  delete pRecord;
	  }
	  pResaDays->RemoveAll();
	  delete pResaDays;
	}
  }
  AllResaTranchesMap.RemoveAll();
}

void CCalendarWnd::ReadTrancheResa(int trancheNo, int familyNo, CString familyFlag)
{
  CObArray *pResaDays;
  WORD tno = trancheNo;
  if (AllResaTranchesMap.Lookup(tno, (CObject*&) pResaDays))
    return;

  choixAffichage m_choix = m_choix1;
  m_choix1 = CCalendarWnd::TOP;
  Invalidate();

  pResaDays = new CObArray();
  AllResaTranchesMap.SetAt (tno, (CObject*)pResaDays);
	
   // découpage en 4 requêtes pour eviter une trop grosse volumétrie
  YmIcHistoEvolDom RecordAp;
  YmIcHistoEvolDom* pRecord = NULL;
  YmIcCmptEvolDom RecordAv;

  unsigned long prevdate = 0;
  RecordAv.TrancheNo(trancheNo);
  RecordAv.FamilyNo(familyNo);
  YM_Query* pSQL = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
  pSQL->SetDomain (&RecordAv);
  ((CCTAApp*)APP)->m_pDatabase->SetTrace(true);
  int rq;
  if (familyFlag == "T")
    rq = IDS_SQL_CALENDAR_RESHOLD_AV1;
  else
    rq = IDS_SQL_CALENDAR_RESHOLD_AVF1;
  RWDBReader Reader( ((CCTAApp*)APP)->m_pDatabase->Transact(pSQL, rq) );
  while(Reader())
  { // On reconstruit un pndata pour garder le code déjà en place et avoir le même traitement
    // pour les dates passée et les dates à venir
    if (Reader.isValid())
	{
      Reader >> RecordAv;
	  if (prevdate != RecordAv.DptDate())
	  {
	    pRecord = new YmIcHistoEvolDom();
		pRecord->TrancheNo(tno);
		pRecord->DptDate(RecordAv.DptDate());
		pRecord->NbNest(1);
		pRecord->RrdIndex(-1);
		CString pn;
		pn.Format ("%c%04d0011112222%04d00000000",
			RecordAv.Cmpt()[0], RecordAv.Capacity(), RecordAv.ResHoldInd());
		pRecord->PnData((LPCSTR)pn);
	    pResaDays->Add((CObject*)pRecord);
		prevdate = RecordAv.DptDate();
	  }
	  else
	  {
		CString pn = pRecord->PnData();
		CString pn2;
        pn2.Format ("%c%04d0011112222%04d00000000",
			RecordAv.Cmpt()[0], RecordAv.Capacity(), RecordAv.ResHoldInd());
		pn += pn2;
		pRecord->PnData((LPCSTR)pn);
	  }
	}
  }

  prevdate = 0;
  int lastjmx = 999;
  RecordAv.TrancheNo(trancheNo);
  pSQL->SetDomain (&RecordAv);
  ((CCTAApp*)APP)->m_pDatabase->SetTrace(true);
  RWDBReader Reader2( ((CCTAApp*)APP)->m_pDatabase->Transact(pSQL, IDS_SQL_CALENDAR_RESHOLD_AP1) );
  while(Reader2())
  { // On reconstruit un pndata pour garder le code déjà en place et avoir le même traitement
    // pour les dates passée et les dates à venir
    if (Reader2.isValid())
	{
      Reader2 >> RecordAv;
	  if ((prevdate != RecordAv.DptDate()) || (lastjmx != RecordAv.RrdIndex()))
	  {
	    pRecord = new YmIcHistoEvolDom();
		pRecord->TrancheNo(tno);
		pRecord->DptDate(RecordAv.DptDate());
		pRecord->NbNest(1);
		pRecord->RrdIndex(RecordAv.RrdIndex());
		CString pn;
		pn.Format ("%c%04d0011112222%04d00000000",
			RecordAv.Cmpt()[0], RecordAv.Capacity(), RecordAv.ResHoldInd());
		pRecord->PnData((LPCSTR)pn);
	    pResaDays->Add((CObject*)pRecord);
		prevdate = RecordAv.DptDate();
		lastjmx = RecordAv.RrdIndex();
	  }
	  else
	  {
		CString pn = pRecord->PnData();
		CString pn2;
        pn2.Format ("%c%04d0011112222%04d00000000",
			RecordAv.Cmpt()[0], RecordAv.Capacity(), RecordAv.ResHoldInd());
		pn += pn2;
		pRecord->PnData((LPCSTR)pn);
	  }
	}
  }
  // NBN - ANO 74720
  delete pSQL;	

  ((CCTAApp*)APP)->m_pDatabase->SetTrace(false);
  if (pResaDays->GetCount() == 0)
  { // tableau vide !
	AllResaTranchesMap.RemoveKey(tno);
	delete pResaDays;
  }
  m_choix1 = m_choix;
}
    
