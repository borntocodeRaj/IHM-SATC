// LegView.cpp : implementation file
//
#include "StdAfx.h"

#include "CTA_Resource.h"


#include "ResaRailView.h"
#include "TrancheView.h"
#include "VirtNestView.h"
#include "LegListboxDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constants
//
const char CLegView::szAutoCopyAuthConst[] = "AutoCopyAuth";
const char CLegView::szPreserveDiffConst[] = "PreserveAuthDifference";
const char CLegView::szLoadPeakConst[] = "AutoLoadPeak";
BOOL* CLegView::m_pbSortTimeContext;

/////////////////////////////////////////////////////////////////////////////
// CLegAltView

IMPLEMENT_DYNCREATE(CLegAltView, CLegView)

CLegAltView::CLegAltView()
:CLegView()
{
}

void CLegAltView::OnInitialUpdate() 
{
  ((CChildFrame*)GetParentFrame())->SetAltFlag(TRUE);

	CLegView::OnInitialUpdate();  // base class method
}

/////////////////////////////////////////////////////////////////////////////
// CLegView

IMPLEMENT_DYNCREATE(CLegView, CListView)

CLegView::CLegView()
{
  FUNC_SORT* apSortFunctions = GetSortFuncArray();

  // initialize the array containing the sort functions
  apSortFunctions[STATUS] = SortLegEditStatus;
  apSortFunctions[LEG_ORIG] = SortLegOrig;
  apSortFunctions[LEG_DEST] = SortLegOrig;  // do the same as if the sort is on Leg Orig
  /// DM-7978 DT8805 - CLE
  apSortFunctions[CC_MAX_01] = SortCcMax1;
  apSortFunctions[CC_MAX_02] = SortCcMax2;
  apSortFunctions[CC_OUV_01] = SortCcOuv1;
  apSortFunctions[CC_OUV_02] = SortCcOuv2;
  /// FIN DM-7978 DT8805 - CLE
  apSortFunctions[DPT_TIME] = SortLegOrig;  // do the same as if the sort is on Leg Orig
  apSortFunctions[ARV_TIME] = SortLegOrig;  // do the same as if the sort is on Leg Orig

//  apSortFunctions[LEG_DEST] = SortDummy;
  apSortFunctions[OD_MANAGED] = SortLegODManaged;
  apSortFunctions[CRITICALITY] = SortLegCriticality;
  apSortFunctions[CRITICALITYREC] = SortLegCriticalityRec;
 
  m_iStatus = -1;
  m_iLegOrig = -1;
  m_iLegDest = -1;
/// DM-7978 DT8805 - CLE
  m_iCcMax01 = -1;
  m_iCcMax02 = -1;
  m_iCcOuv01 = -1;
  m_iCcOuv02 = -1;
/// FIN DM-7978 DT8805 - CLE
  m_iCmpt = -1;
  m_iODManaged = -1;
  m_iCriticality = -1;
  m_iCriticalityRec = -1;
  m_iLegCritCount = -1;
  m_iCritMessages = -1;
  m_iCapacity = -1;
  m_iResHoldTot = -1;
  m_iResHoldGrp = -1;
  m_iResNonYield = -1;
  m_iUnreservableCap = -1;
  m_iStandingCap = -1;

  m_pLegLoaded = NULL;
  m_pLegPrevious = NULL;

  m_bOvbAdjust = TRUE;
  m_bAutoCopyAuth = FALSE;
  m_bAdaptationFlag = FALSE;
  m_bSelPeak = TRUE;

  m_index = -1;
}

int CLegView::SortCcMax1(LPARAM lParam1, LPARAM lParam2)
{
	YmIcLegsDom* pRecord1 = (YmIcLegsDom*)lParam1;
	YmIcLegsDom* pRecord2 = (YmIcLegsDom*)lParam2;
	int result = CompareCC(pRecord1->ccMax1().c_str(), pRecord2->ccMax1().c_str());
	return result;
}

int CLegView::SortCcMax2(LPARAM lParam1, LPARAM lParam2)
{
	YmIcLegsDom* pRecord1 = (YmIcLegsDom*)lParam1;
	YmIcLegsDom* pRecord2 = (YmIcLegsDom*)lParam2;
	int result = CompareCC(pRecord1->ccMax2().c_str(), pRecord2->ccMax2().c_str());
	return result;
}


int CLegView::SortCcOuv1(LPARAM lParam1, LPARAM lParam2)
{
	YmIcLegsDom* pRecord1 = (YmIcLegsDom*)lParam1;
	YmIcLegsDom* pRecord2 = (YmIcLegsDom*)lParam2;
	int result = CompareCC(pRecord1->ccOuv1().c_str(), pRecord2->ccOuv1().c_str());
	return result;
}

int CLegView::SortCcOuv2(LPARAM lParam1, LPARAM lParam2)
{
	YmIcLegsDom* pRecord1 = (YmIcLegsDom*)lParam1;
	YmIcLegsDom* pRecord2 = (YmIcLegsDom*)lParam2;
	int result = CompareCC(pRecord1->ccOuv2().c_str(), pRecord2->ccOuv2().c_str());
	return result;
}

// NBN - Ano 89765 - Ajout permettant le tri par CC dans les tableaux
int CLegView::CompareCC(CString str1, CString str2)
{
	if(str1.GetLength()>3 || str2.GetLength()>3){
		return -1;
	}
	if(str1.GetLength()==2){
		str1.Replace(str1.Left(1),str1.Left(1)+"0");
	}
	if(str2.GetLength()==2){
		str2.Replace(str2.Left(1),str2.Left(1)+"0");
	}

	return strcmp(str1, str2);
}

CLegView::~CLegView()
{
  // free memory created for all column attributes
  ColAttrStruct* pColAttr = NULL;
  for (int i=0; i < GetColAttributeArray()->GetSize(); i++)
  {
    pColAttr = GetColAttributeArray()->GetAt(i);
    if (pColAttr)
    {
      free(pColAttr);
      pColAttr = NULL;
    }
  }
  GetColAttributeArray()->RemoveAll();
}

BEGIN_MESSAGE_MAP(CLegView, CListView)
  //{{AFX_MSG_MAP(CLegView)
  ON_WM_SIZE()
  ON_WM_CLOSE()
  ON_WM_PAINT()
  ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
  ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
  ON_WM_CONTEXTMENU()
  ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
  ON_WM_MOUSEMOVE()
  ON_WM_CHAR()
  ON_WM_DESTROY()
	//}}AFX_MSG_MAP
  ON_COMMAND(ID_LEG_LOAD,OnLegLoad)
  ON_UPDATE_COMMAND_UI(ID_LEG_LOAD, OnUpdateLegLoad)
  ON_COMMAND(ID_LEG_ALT_LOAD,OnLegLoad)
  ON_UPDATE_COMMAND_UI(ID_LEG_ALT_LOAD, OnUpdateLegLoad)
  ON_COMMAND(ID_LEG_SEND, OnLegSend)
  ON_COMMAND(ID_LEG_SEND_NOQUERY, OnLegSendNoQuery)
  ON_UPDATE_COMMAND_UI(ID_LEG_SEND, OnUpdateLegSend)
  ON_COMMAND(ID_LEG_ALT_SEND, OnLegSend)
  ON_COMMAND(ID_LEG_ALT_SEND_NOQUERY, OnLegSendNoQuery)
  ON_UPDATE_COMMAND_UI(ID_LEG_ALT_SEND, OnUpdateLegSend)
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
  ON_COMMAND(ID_LEG_NEXT,OnLegNext)
  ON_UPDATE_COMMAND_UI(ID_LEG_NEXT, OnUpdateLegNext)
  ON_COMMAND(ID_LEG_ALT_NEXT,OnLegNext)
  ON_UPDATE_COMMAND_UI(ID_LEG_ALT_NEXT, OnUpdateLegNext)
  ON_COMMAND(ID_LEG_PREVIOUS,OnLegPrev)
  ON_UPDATE_COMMAND_UI(ID_LEG_PREVIOUS, OnUpdateLegPrev)
  ON_COMMAND(ID_LEG_ALT_PREVIOUS,OnLegPrev)
  ON_UPDATE_COMMAND_UI(ID_LEG_ALT_PREVIOUS, OnUpdateLegPrev)
  ON_COMMAND(ID_LEG_UNDO,OnLegUndo)
  ON_UPDATE_COMMAND_UI(ID_LEG_UNDO, OnUpdateLegUndo)
  ON_COMMAND(ID_LEG_ALT_UNDO,OnLegUndo)
  ON_UPDATE_COMMAND_UI(ID_LEG_ALT_UNDO, OnUpdateLegUndo)
  ON_COMMAND(ID_LEG_DEFAULTSORT,OnLegDefaultSort)
  ON_UPDATE_COMMAND_UI(ID_LEG_DEFAULTSORT, OnUpdateLegDefaultSort)
  ON_COMMAND(ID_LEG_ALT_DEFAULTSORT,OnLegDefaultSort)
  ON_UPDATE_COMMAND_UI(ID_LEG_ALT_DEFAULTSORT, OnUpdateLegDefaultSort)
  ON_COMMAND(ID_LEG_REVERSESORT,OnLegReverseSort)
  ON_UPDATE_COMMAND_UI(ID_LEG_REVERSESORT, OnUpdateLegReverseSort)
  ON_COMMAND(ID_LEG_SELPEAK,OnLegSelPeak)
  ON_UPDATE_COMMAND_UI(ID_LEG_SELPEAK, OnUpdateLegSelPeak)
  ON_COMMAND(ID_LEG_ADJUSTMENT,OnLegAdjust)
  ON_UPDATE_COMMAND_UI(ID_LEG_ADJUSTMENT, OnUpdateLegAdjust)
  ON_COMMAND(ID_LEG_ALT_ADJUSTMENT,OnLegAdjust)
  ON_UPDATE_COMMAND_UI(ID_LEG_ALT_ADJUSTMENT, OnUpdateLegAdjust)
  ON_COMMAND(ID_LEG_AUTO_COPY_AUTH,OnLegAutoCopyAuth)
  ON_UPDATE_COMMAND_UI(ID_LEG_AUTO_COPY_AUTH, OnUpdateLegAutoCopyAuth)
  ON_COMMAND(ID_LEG_ALT_AUTO_COPY_AUTH,OnLegAutoCopyAuth)
  ON_UPDATE_COMMAND_UI(ID_LEG_ALT_AUTO_COPY_AUTH, OnUpdateLegAutoCopyAuth)
  ON_COMMAND(ID_LEG_CRITICAL_MESSAGES,  OnLegCriticalMessages)
  ON_UPDATE_COMMAND_UI(ID_LEG_CRITICAL_MESSAGES, OnUpdateLegCriticalMessages)
  ON_COMMAND(ID_LEG_ALT_CRITICAL_MESSAGES,  OnLegCriticalMessages)
  ON_UPDATE_COMMAND_UI(ID_LEG_ALT_CRITICAL_MESSAGES, OnUpdateLegCriticalMessages)
  ON_COMMAND(ID_LEG_HELP_DLG,  LegHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLegView drawing

CFont* CLegView::GetStdFont(CDC* pDC)
{
  return ((CCTAApp*)APP)->GetDocument()->GetStdFont(pDC);
}

CFont* CLegView::GetBoldFont(CDC* pDC)
{
  return ((CCTAApp*)APP)->GetDocument()->GetBoldFont(pDC);
}

void CLegView::CreateItemImageList()
{
  m_itemImageList.Create( IDB_STATUS_BITMAP, iBitMapSizeConst, 0, RGB( 0, 0, 255 ) );
}

void CLegView::DrawImageList(
                                 LPDRAWITEMSTRUCT lpDrawItemStruct, 
                                 UINT uiFlags, 
                                 COLORREF clrTextSave, 
                                 COLORREF clrBkSave, 
                                 COLORREF clrImage)
{
  CListCtrl& ListCtrl=GetListCtrl();
  int nItem = lpDrawItemStruct->itemID;
  CImageList* pImageList = GetListCtrl().GetImageList(LVSIL_SMALL);
  static _TCHAR szBuff[MAX_PATH];
  CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

  LV_ITEM lvi;
  lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
  lvi.iItem = nItem;
  lvi.iSubItem = 0;
  lvi.pszText = szBuff;
  lvi.cchTextMax = sizeof(szBuff);
  lvi.stateMask = 0xFFFF;   // get all state flags
  ListCtrl.GetItem(&lvi);

  CRect rcIcon;
//ListCtrl.GetItemRect(nItem, rcIcon, LVIR_ICON);
  ListCtrl.GetItemRect(nItem, rcIcon, LVIR_BOUNDS);
  if (pImageList)
  {
    UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
    COLORREF clrBkImage = 
      ( (m_nBkAlternate>0) && ((nItem/m_nBkAlternate)&1) ) ? m_clrBkAlternate : m_clrBkgnd;

  // if this item to be drawn is the currently loaded item, use the copy
  YmIcLegsDom* pLeg = (YM_Set<YmIcLegsDom>::FromKey(LEG_KEY))->GetRecord(lvi.lParam);
  YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);


    ////////////////////////////////////////////////////////////////////////////
    // Edit Status (1,2,4)?
    if( rcIcon.left+iBitMapSizeConst < rcIcon.right )
    { 
      if ( pLeg->EditStatus() == EDIT_STATUS_NOTSEEN) // Affichage vide
        ImageList_DrawEx(pImageList->m_hImageList, BMP_EDIT_NOTSEEN,
            pDC->m_hDC,rcIcon.left,rcIcon.top, iBitMapSizeConst, iBitMapSizeConst,
            clrBkImage, clrImage, uiFlags | nOvlImageMask);

      else if ( pLeg->EditStatus() & EDIT_STATUS_OKAY)
        ImageList_DrawEx(pImageList->m_hImageList, BMP_EDIT_NOMOD,
            pDC->m_hDC,rcIcon.left,rcIcon.top, iBitMapSizeConst, iBitMapSizeConst,
            clrBkImage, clrImage, uiFlags | nOvlImageMask);

      else if (pLeg->EditStatus() & EDIT_STATUS_AUTHMODNOTSENT)
	  {
	    if (!(pTranche->EditStatus() & EDIT_STATUS_OKAY_RHEALYS))
		{ // on affiche le triangle noir ici que si le train n'est pas passé par Rhealys
          ImageList_DrawEx(pImageList->m_hImageList, BMP_EDIT_AUTHMODNOTSENT,
              pDC->m_hDC,rcIcon.left,rcIcon.top, iBitMapSizeConst, iBitMapSizeConst,
              clrBkImage, clrImage, uiFlags | nOvlImageMask);
		}

	  }
    }

    ////////////////////////////////////////////////////////////////////////////
    // Sent Flag
	// HRRD n'existe plus, mais le symbole fleche verte a été réutilisé pour le
	// moteur VNAX, toutefois les identifiant de bitmpa n'on pas été renommé.
	// il faut comprendre ci-dessous que HRRD signifie Moteur VNAX
    rcIcon.left += iBitMapSizeConst;
    //if( rcIcon.left+iBitMapSizeConst < rcIcon.right )//DM8029 - Display ALLEO icons for SNCF mode (To be tested)
    //{
	  int bmpid = -1;
	  switch (pLeg->SentFlag())
	  {
		case 1 : bmpid = BMP_SENT; break;
	      case 2 : bmpid = BMP_SENT_COMMERCIAL; break;
		  case 3 : bmpid = BMP_SENT_USER_ET_COMMERC; break;
		  case 4 : bmpid = BMP_SENT_HRRD; break;
		  case 5 : bmpid = BMP_SENT_USER_ET_HRRD; break;
		  case 6 : bmpid = BMP_SENT_COMMERC_ET_HRRD; break;
		  case 7 : bmpid = BMP_SENT_U_C_H; break;
		  case 8 : bmpid = BMP_SENT_NOMOS; break;
		  case 9 : bmpid = BMP_SENT_PLUS_NOMOS; break;
		  case 10 : bmpid = BMP_SENT_COMMER_NOMOS; break;
		  case 11 : bmpid = BMP_SENT_USER_COMMER_NOMOS; break;
		  case 12 : bmpid = BMP_SENT_HRRD_NOMOS; break;
		  case 13 : bmpid = BMP_SENT_USER_HRRD_NOMOS; break;
		  case 14 : bmpid = BMP_SENT_COMMER_HRRD_NOMOS; break;
		  case 15 : bmpid = BMP_SENT_U_C_H_N; break;
		  default : bmpid = -1;
	  }  
	  if (bmpid != -1)
       ImageList_DrawEx(pImageList->m_hImageList, bmpid,
                       pDC->m_hDC,rcIcon.left,rcIcon.top, iBitMapSizeConst, iBitMapSizeConst,
                       clrBkImage,  clrImage, uiFlags | nOvlImageMask);
     
    //}

	////////////////////////////////////////////////////////////////////////////
	// DM 5882.1 SCX_SENT_FLAG, CC-1 et ensuite peut-être étanchéité
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSCXFlag() )
	{
	  CTrancheView* pTrancheView = GetTrancheView();
      if (pTrancheView)
	  {
	    rcIcon.left += iBitMapSizeConst;
		YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
        if( rcIcon.left+iBitMapSizeConst < rcIcon.right )
	    {
	      int bmpid = -1;
		  if (pLeg->ScxCount() > 0)
		  {
		    switch (pLeg->ScxSentFlag())
		    {
		      case 1 : bmpid = BMP_VNAS_SENT; break;
	          case 2 : bmpid = BMP_VNAS_SENT_COMMERCIAL; break;
		      case 3 : bmpid = BMP_VNAS_SENT_USER_ET_COMMERC; break;
		      case 4 : bmpid = BMP_VNAS_SENT_HRRD; break;
		      case 5 : bmpid = BMP_VNAS_SENT_USER_ET_HRRD; break;
		      case 6 : bmpid = BMP_VNAS_SENT_COMMERC_ET_HRRD; break;
		      case 7 : bmpid = BMP_VNAS_SENT_U_C_H; break;
		      default : bmpid = BMP_CCM1_SEUL;
			}
			if (pLoadedTranche && pLoadedTranche->ScxEtanche())
			  bmpid += 8;
			if (pLeg->ScxCapa() == 0)
			  bmpid += 16;
		  }  
	      if (bmpid != -1)
		  {
		    CImageList* pVnasImageList = pTrancheView->GetVnasImageList();
		    ImageList_DrawEx(pVnasImageList->m_hImageList, bmpid,
                             pDC->m_hDC,rcIcon.left,rcIcon.top, iBitMapSizeConst, iBitMapSizeConst,
                             clrBkImage, clrImage, uiFlags | nOvlImageMask);
		  }
		}
	  }
	}
	// affichage d'info ALLEO
	rcIcon.left += iBitMapSizeConst;
    if( rcIcon.left+iBitMapSizeConst < rcIcon.right )
	{
	  int bmpid = 0;
	  if (pTranche->EditStatus() & EDIT_STATUS_OKAY_RHEALYS)
	  { // tranche vue par Rhealys
	    if (pLeg->EditStatus() & EDIT_STATUS_AUTHMODNOTSENT)
		  bmpid = BMP_RHEALYS_NOTSENT; // triangle noir évidé, car VNAU non envoyé, mais en 8eme colonne
		else if (pLeg->SentFlag() & 16) // bit 5 du sent flag positionné, VNAU envoyé par ALLEO
		  bmpid = BMP_RHEALYS_SENT;
		// on n'affiche pas le smiley blanc dans la liste des tronçons
	  }
	  if (bmpid)
        ImageList_DrawEx(pImageList->m_hImageList, bmpid,
            pDC->m_hDC,rcIcon.left,rcIcon.top, iBitMapSizeConst, iBitMapSizeConst,
            clrBkImage, clrImage, uiFlags | nOvlImageMask);
	}
  }
}

CString CLegView::GetDispInfo(int iSubItem, LPARAM lParam, int iItem)
{
 YmIcLegsDom* pLeg = 
    (YM_Set<YmIcLegsDom>::FromKey(LEG_KEY))->GetRecord(lParam);

  if (iSubItem == m_iColsInserted - 1)  // reset the previous leg hold variable on the last column
    m_pLegPrevious = pLeg;
  long lTime;
  CString szBuffer;

  CTrancheView* pTrancheView = GetTrancheView();
  if (!pTrancheView)
    return szBuffer;

  YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
  if (!pLoadedTranche)
    return szBuffer;

  
  switch( (GetColId( iSubItem ) & ~REVERSE_ORDER_FLAG) ) 
  {
  case LEG_ORIG :
	  if ((pLeg->LegId() == 1) || (pLeg->LegId() == 3))
	    szBuffer = "!";
	  szBuffer += pLeg->LegOrig();
	  if ((pLeg->Mdi() == 1) || (pLeg->Mdi() == 3))
	    szBuffer += "%2%";
	  if (!strcmp (pLoadedTranche->PeakLeg(), pLeg->LegOrig()))
	    szBuffer += "%G%";
    break;
  case LEG_DEST :
	  if ((pLeg->LegId() == 2) || (pLeg->LegId() == 3))
	    szBuffer = "!";
	  szBuffer += pLeg->LegDest();
	  if ((pLeg->Mdi() == 2) || (pLeg->Mdi() == 3))
	    szBuffer += "%2%";
	  if (!strcmp (pLoadedTranche->PeakLeg(), pLeg->LegOrig()))
	    szBuffer += "%G%";
    break;
  case OD_MANAGED :
    if (pLeg->ODManaged())
	  szBuffer = "Yod";
	else
	  szBuffer.Empty();
    break;
  case CRITICALITY :
	if (pLeg->Criticality() == -1)
	  pLeg->Criticality(FirstCriticality(pLeg));
    szBuffer.Format( "%d", pLeg->Criticality() );
    break;
  case CRITICALITYREC :
    if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetMultipleCriticalRulesFlag())
	{
	  if (pLeg->CriticalityRec() == -1)
	    pLeg->CriticalityRec (CriticalityRev(pLeg));
	  szBuffer.Format( "%d", pLeg->CriticalityRec());
	}
	else
	  szBuffer = "0";
	szBuffer += "%3%";
    break;
	/// DM-7978 DT8805 - CLE
  case CC_MAX_01 :
	  szBuffer = pLeg->ccMax1().c_str();
	  if (pLeg->ccMaxUser1() != pLeg->ccMaxInd1())// YI-9160 (vert)
		szBuffer += "%6%";
	  break;
  case CC_MAX_02 :
	  szBuffer = pLeg->ccMax2().c_str();
	  if (pLeg->ccMaxUser2() != pLeg->ccMaxInd2())// YI-9160 (vert)
		szBuffer += "%6%";
	  break;
  case CC_OUV_01 : 
	  szBuffer = pLeg->ccOuv1().c_str();
	  
		if (pLeg->ccMaxUser1() < pLeg->ccOuvUser1())// YI-9158 (violet)
			szBuffer += "%7%";
	  break;
  case CC_OUV_02 : 
	  szBuffer = pLeg->ccOuv2().c_str();
	  if (pLeg->ccMaxUser2() < pLeg->ccOuvUser2())// YI-9158 (violet)
		szBuffer += "%7%";
	  break;
	  /// FIN DM-7978 DT8805 - CLE
  case DPT_TIME :
    lTime = pLeg->DptTime();
    szBuffer.Format( "%2.2u:%2.2u", lTime / 100, lTime % 100 );
    break;
  case ARV_TIME :
    lTime = pLeg->ArvTime();
    szBuffer.Format( "%2.2u:%2.2u", lTime / 100, lTime % 100 );
    break;
  default:
    szBuffer = "<???>";
    break;
  }
  return szBuffer; 
}

int CLegView::GetPropertyLevel(LPARAM lParam)
{
  // if this item to be drawn is the currently loaded item, use the copy
  YmIcLegsDom* pLeg = 
    (YM_Set<YmIcLegsDom>::FromKey(LEG_KEY))->GetRecord(lParam);

  if( pLeg->CriticalFlag() || (pLeg->CriticalityRec() > 14))  return 1;
  else  return 0;
}

CString CLegView::GetViewTitle()
{
  return ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_LEG_TITLE);
}

CString CLegView::GetColumnTitle(int iResId)
{
  return ((CCTAApp*)APP)->GetResource()->LoadResString(iResId);
}

CString CLegView::GetDefaultSortOrder()
{
  CString szDefaultOrder;
  szDefaultOrder.Format("%d<,%d>", CRITICALITY, LEG_ORIG);
  return szDefaultOrder;
}

int CLegView::SortLegEditStatus(LPARAM lParam1, LPARAM lParam2)
{
  YmIcLegsDom* pRecord1 = (YmIcLegsDom*)lParam1;
  YmIcLegsDom* pRecord2 = (YmIcLegsDom*)lParam2;
  int result = 0 - NumCmp( pRecord1->EditStatus(), pRecord2->EditStatus() );
  return result;
}
int CLegView::SortLegOrig(LPARAM lParam1, LPARAM lParam2)
{
  YmIcLegsDom* pRecord1 = (YmIcLegsDom*)lParam1;
  YmIcLegsDom* pRecord2 = (YmIcLegsDom*)lParam2;
  return( pRecord1->LegSeq() - pRecord2->LegSeq() );
}
int CLegView::SortLegCmpt(LPARAM lParam1, LPARAM lParam2)
{
  YmIcLegsDom* pRecord1 = (YmIcLegsDom*)lParam1;
  YmIcLegsDom* pRecord2 = (YmIcLegsDom*)lParam2;
  int iResult = ( strcmp(pRecord1->Cmpt(), pRecord2->Cmpt()) );
/*
  if (iResult == 0)
    iResult = ( pRecord1->LegSeq() - pRecord2->LegSeq() );
*/
  return iResult;
}
int CLegView::SortLegODManaged(LPARAM lParam1, LPARAM lParam2)
{
  YmIcLegsDom* pRecord1 = (YmIcLegsDom*)lParam1;
  YmIcLegsDom* pRecord2 = (YmIcLegsDom*)lParam2;
  int iResult = ( pRecord1->ODManaged() - pRecord2->ODManaged() );
/*
  if (iResult == 0)
    iResult = ( pRecord1->LegSeq() - pRecord2->LegSeq() );
*/
  return iResult;
}
int CLegView::SortLegCriticality(LPARAM lParam1, LPARAM lParam2)
{
  YmIcLegsDom* pRecord1 = (YmIcLegsDom*)lParam1;
  YmIcLegsDom* pRecord2 = (YmIcLegsDom*)lParam2;
  int iResult = ( pRecord1->Criticality() - pRecord2->Criticality() );
/*
  if (iResult == 0)
    iResult = ( pRecord1->LegSeq() - pRecord2->LegSeq() );
*/
  return iResult;
}
int CLegView::SortLegCriticalityRec(LPARAM lParam1, LPARAM lParam2)
{
  YmIcLegsDom* pRecord1 = (YmIcLegsDom*)lParam1;
  YmIcLegsDom* pRecord2 = (YmIcLegsDom*)lParam2;
  int iResult = ( pRecord1->CriticalityRec() - pRecord2->CriticalityRec() );

  return iResult;
}

int CLegView::SortDummy(LPARAM lParam1, LPARAM lParam2)
{
  return( 0 );
}

void CLegView::InsertColumns() 
{
  int iColIndex = 0;

  SetPropertyLevelColor (2, RGB (255, 0, 0));
  COLORREF clrFt;
  clrFt = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();
  SetPropertyLevelColor (3, clrFt);
/// DM-7978 DT8805 - CLE
  SetPropertyLevelColor (6, RGB (50, 200, 0)); // YI-9160 (vert) pour nouveau TOP
   SetPropertyLevelColor(7, COLOR_PANEDATE3);  // YI-9158 (violet)
///FIN DM-7978 DT8805 - CLE

  InsertColumn( m_iStatus = iColIndex++, STATUS, IDS_STATUS, LVCFMT_LEFT, iBitMapSizeConst*3 );

  InsertColumn( m_iLegOrig = iColIndex++, LEG_ORIG, IDS_LEG_ORIG, LVCFMT_LEFT, 80 );

  InsertColumn( m_iLegDest = iColIndex++, LEG_DEST, IDS_LEG_DEST, LVCFMT_LEFT, 80 );

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldODFlag() )
    InsertColumn( m_iODManaged = iColIndex++, OD_MANAGED, IDS_OD_MANAGED, LVCFMT_CENTER, 45 );

  InsertColumn( m_iCriticality = iColIndex++, CRITICALITY, IDS_CRITICALITY, LVCFMT_LEFT, 40 );

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetMultipleCriticalRulesFlag() )
    InsertColumn( m_iCriticalityRec = iColIndex++, CRITICALITYREC, IDS_CRITICALITY_REC, LVCFMT_LEFT, 40 );
    /// DM-7978 DT8805 - CLE
  // OBA - Ano 70146 - Begin - Attention aux valeurs en base de registre !!!
  BOOL bMasque = ((CCTAApp*)APP)->GetRhealysFlag();
  if (!bMasque)// NBN ANO 92261 - Ne pas afficher les colonnes pour ALLEO
  {
	InsertColumn(iColIndex++, CC_MAX_01, IDS_CC_MAX_1, LVCFMT_CENTER, 85);
	InsertColumn(iColIndex++, CC_MAX_02, IDS_CC_MAX_2, LVCFMT_CENTER, 85);
	InsertColumn(iColIndex++, CC_OUV_01, IDS_CC_OUV_1, LVCFMT_CENTER, 85);
	InsertColumn(iColIndex++, CC_OUV_02, IDS_CC_OUV_2, LVCFMT_CENTER, 85);
  }
  // OBA - Ano 70146 - End
 /// DM-7978 DT8805 - CLE 
  InsertColumn( m_iDptTime = iColIndex++, DPT_TIME, IDS_DPT_TIME, LVCFMT_LEFT, 60 );
  InsertColumn( m_iArvTime = iColIndex++, ARV_TIME, IDS_ARR_TIME, LVCFMT_LEFT, 60 );


  m_iColsInserted = iColIndex;
}

BOOL CLegView::AutoCopyConfirm()
{
  if ( ! ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAutoCopyAuthFlag() )
    return FALSE;
  CResaRailView* pResaRailView = GetResaRailView();
  if (!pResaRailView)
    return FALSE;

  CString s;
  s = ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_CONFIRM_COPYANDSEND_LEGS_ALL );
  if ( m_bAutoCopyAuth && ( pResaRailView->Modified() ) && (MessageBox (s, NULL, MB_YESNO) == IDYES) )
    return TRUE;
  else
    return FALSE;
}

CTrancheView* CLegView::GetTrancheView()
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
  //ASSERT(pTrView != NULL);
  CTrancheView* pTrancheView = (CTrancheView*)pTrView;
  return pTrancheView;
}

void CLegView::LoadLeg( YmIcLegsDom* pLeg, int iIndex )
{
  if ( m_pLegLoaded == pLeg )  // Is this a different leg than the one currently loaded ?
    return;

//  BOOL bAutoSelect = (m_pLegLoaded != NULL);

  // change the selected state of the next item
  LV_ITEM Item;
  Item.mask = LVIF_STATE;
  Item.iItem = iIndex;
  Item.iSubItem = 0;
  Item.state = LVIS_SELECTED;
  Item.stateMask = LVIS_SELECTED;
  GetListCtrl().SetItem( &Item );

  // We must update all the authorizations for the currently loaded leg
  //   and then update the edit status 
  //   for the tranche which WILL BE loaded (i.e., pLeg)

  // update all authorization and dmd dpt values as needed
  //   for the currently loaded leg (i.e., the leg we're leaving)
  if ( Adaptation() )  // TRN client only ... this flag set in ResaRailView, SetAdaptationFlag(TRUE)
  {
    UpdateLeg( 3, ALL_LEGS, NULL, CONFIRM_NEVER );
    m_bAdaptationFlag = FALSE;  // turn it off for the next leg
  }
  else if ( AutoCopyConfirm() )
  {
    UpdateLeg( 3, ALL_LEGS, NULL, CONFIRM_ONCE );
  }
  else
  {
    UpdateLeg( 3, CURRENTLY_LOADED_LEG );
  }

  int iLegSetKey = LEG_KEY;
  YM_RecordSet* pLegSet = YM_Set<YmIcLegsDom>::FromKey(iLegSetKey);

  // now set the edit status and do an update for the leg which will be loaded (i.e., the leg we're loading)

  // initial setting of the edit status for this leg
  if (!((CCTAApp*)APP)->GetRhealysFlag())
    pLeg->EditStatus(pLeg->EditStatus() | EDIT_STATUS_OKAY);

  if ( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetUpdateSynchroneFlag() )
  { // 17/05/06, suite a plainte des utilisateurs sur le temps de réponse globale, on remeet en place
	// l'update désynchronisé. Mais par sécurité je rend ceci paramétrable.
    YmIcLegsDom* pLegBis = new YmIcLegsDom(*pLeg);
    YM_Query* pUpdateQuery = 
      new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), TRUE);  // AutoDelete is TRUE
    pUpdateQuery->Start( pLegBis, IDS_SQL_UPDATE_LEG_LIST );
  }
  else
  {
    // Utilisation d'une requete synchrone de mise à jour de sc_legs pour eviter le problème
    // s'étant produiy le 25/10/2005, lorsque des deletes massifs étaient fait en base bpsn001 sur sc_legs
    // PB de desynchronisation, la query était détruite avant de passer dans le OnWorkFinished
    ((CCTAApp*)APP)->UpdateOneLeg (pLeg);
  }


  /// DM-7978 DT8805 - CLE - get CCMAX et CCOUV pour chaque legs
  ((CCTAApp*)APP)->GetDocument()->UpdateCCMaxCCOuvLegs(pLeg);

  // Set the member variable for the currently loaded leg
  m_pLegLoaded = pLeg;

	
  pLegSet->SetCurrent( (long)m_pLegLoaded, this );

  GetListCtrl().Invalidate();
  GetListCtrl().UpdateWindow();

  InfoStatus(NULL, pLeg);

  // refresh the virtual nesting view if it is displayed.
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
    CView* pView = pDoc->GetNextView( pos ); 
    if( pView->IsKindOf( RUNTIME_CLASS(CVirtNestView)) )
    {
      CVirtNestView* pVirtNestView = (CVirtNestView*)pView;
      pVirtNestView->PopulateList();
      break;
    }
  }
}

YmIcLegsDom* CLegView::FindLeg(CString szOrig, CString szDest, int& iIndex)
{
	// NPI - Ano 56245
	// Réécriture de la méthode pour ne pas modifier l'index passé en paramètre en cas de non match
	/*CListCtrl& ListCtrl = GetListCtrl();
	YmIcLegsDom* pLeg = NULL;
	for (iIndex = 0; iIndex < ListCtrl.GetItemCount(); iIndex++)
	{
		pLeg = (YmIcLegsDom*)ListCtrl.GetItemData(iIndex);
		ASSERT(pLeg != NULL);
		// if szDest parameter is empty, then search only the leg origin
		if ( (pLeg->LegOrig() == szOrig) && (szDest.IsEmpty() || (pLeg->LegDest() == szDest)) )  // must match orig and dest
		return pLeg;
	}
	return NULL;*/

	CListCtrl& ListCtrl = GetListCtrl();
	YmIcLegsDom* pLeg = NULL;
	for (int iCurIndex = 0; iCurIndex < ListCtrl.GetItemCount(); iCurIndex++)
	{
		// NPI - Ano 56791
		//pLeg = (YmIcLegsDom*)ListCtrl.GetItemData(iIndex);
		pLeg = (YmIcLegsDom*)ListCtrl.GetItemData(iCurIndex);
		///
		ASSERT(pLeg != NULL);
		// if szDest parameter is empty, then search only the leg origin
		if ((pLeg->LegOrig() == szOrig) && (szDest.IsEmpty() || (pLeg->LegDest() == szDest)))  // must match orig and dest
		{
			iIndex = iCurIndex;
			return pLeg;
		}
	}
	return NULL;
	///
}

// renvoi le tronçon frontière s'il existe. NULL sinon
YmIcLegsDom* CLegView::FindFrontier()
{
  CListCtrl& ListCtrl = GetListCtrl();
  YmIcLegsDom* pLeg = NULL;
  CString szOrig, szDest;
  for (int iIndex = 0; iIndex < ListCtrl.GetItemCount(); iIndex++)
  {
    pLeg = (YmIcLegsDom*)ListCtrl.GetItemData(iIndex);
    ASSERT(pLeg != NULL);
	szOrig = pLeg->LegOrig();
	szDest = pLeg->LegDest();
	if (szOrig.Left(2) != szDest.Left(2))
      return pLeg;
  }
  return NULL;
}

void CLegView::LoadSelectedItem()
{
  int iIndex = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
  if (iIndex >= 0)
  {
    YmIcLegsDom* pLegHit = (YmIcLegsDom*)GetListCtrl().GetItemData(iIndex);

    if ( pLegHit != m_pLegLoaded )  // load only if it is different
      LoadLeg( pLegHit, iIndex );
  }
}

void CLegView::CtrlScgTranche()
{
  CListCtrl& ListCtrl = GetListCtrl();
  YmIcLegsDom* pLeg = NULL;
  long SumScg = 0;
  int iIndex;
  for (iIndex = 0; iIndex < ListCtrl.GetItemCount(); iIndex++)
  {
    pLeg = (YmIcLegsDom*)ListCtrl.GetItemData(iIndex);
	SumScg += pLeg->ScxCapa();
  }
  CTrancheView* pTrancheView = GetTrancheView();
  YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
  if (((SumScg == 0) && (pLoadedTranche->ScxCapa() != 0)) ||
	  ((SumScg != 0) && (pLoadedTranche->ScxCapa() == 0)))
  {
	pLoadedTranche->ScxCapa(SumScg);
	pTrancheView->Invalidate(FALSE);
	YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	pTranche->ScxCapa(SumScg); // toujours cette merde entre item chargé et courant. Je fais les deux pour être sûr.
  }
}

void CLegView::LegSend( BOOL bConfirm /* = TRUE */)
{
  // find the ResaRail view and force a send of a Vnau for the currently loaded leg
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  POSITION pos = pDoc->GetFirstViewPosition();
  while( pos != NULL ) 
  {
    CView* pView = pDoc->GetNextView( pos );    
    if( pView->IsKindOf( RUNTIME_CLASS(CResaRailView) ) )
    {
      if ( pView->GetSafeHwnd() )
        ((CResaRailView*)pView)->SaveAllModified( 3, NULL, CURRENTLY_LOADED_LEG, FALSE, FALSE );
      break;
    } 
  }
  LegNext();  // advance to the next leg...

  GetListCtrl().Invalidate();
  GetListCtrl().UpdateWindow();
}

// DM 7978 (souplesse tarifaire BUGI 69672), controle CC0=CC1 (plus AVT 17)
// si FALSE, empêcher la mise à jour des VNAU
// si TRUE, on pourra poursuivre les traitements.
BOOL CLegView::LegOKCC0EgalCC1(int AUorAS, 
							   enum LegUpdateMode eUpdateMode)
{
	// Si les VNAU ne sont pas concernés, pas de contrôle supplémentaire à effectuer
	if (AUorAS %2 != 1)
	{
		return TRUE;
	}
	CResaRailView* pResaRailView = GetResaRailView();
	if (pResaRailView == NULL)
	{
		return TRUE;
	}
	// DM 7978 : DT 13903 - avt 17
	// Ne controler CC0 == CC1 que si l'indicateur de contrôle est à true
	// AVT 17 : le contrôle ne doit être effectué que si YM_SYS_PARMS.DATE_BANG est <= date du jour
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	bool controlerCC0EgalCC1 = pDoc->m_pGlobalSysParms->GetControlerCC0EgalCC1();
	if (controlerCC0EgalCC1 == false)
	{
		return TRUE;
	}
	// Contrôler maintenant CC0 <> CC1
	//	merde ici
	BOOL bModif   = FALSE;
	BOOL controleNOK = FALSE;
    BOOL bForceAuthMod = TRUE;

	// Appel de la méthode CResarailView qui va vérifier CC0=CC1 ou pas
	controleNOK = pResaRailView->ControleEgaliteCC0CC1(
		m_pLegLoaded,
		eUpdateMode,
		bForceAuthMod,
		bModif);


	return !(controleNOK);
}
// Fin nouvelle méthode pour BUGI 69672

void CLegView::LegPasteAuthSelected(int AUorAS)
{
  if (!LegOKActDiffMoi(AUorAS))
    return;
  CString szConfirmMsg;
  CString szFormat;
  szFormat = ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_CONFIRM_COPYANDSEND_SEL_LEGS );
  szConfirmMsg.Format (szFormat, 1);
  if ( MessageBox (szConfirmMsg, NULL, MB_YESNO) == IDYES )
  {
	// DM 7978 (souplesse tarifaire BUGI 69672), controle CC0=CC1 (plus AVT 17)
	if (!LegOKCC0EgalCC1(AUorAS, CURRENTLY_LOADED_LEG))
	{
		return;
	}
    // Update the  "loaded" and "selected" leg with the "loaded" leg's authorizations
    UpdateLeg( AUorAS, CURRENTLY_LOADED_LEG, NULL, CONFIRM_NEVER );
    UpdateLeg( AUorAS, CURRENTLY_SELECTED_LEG, NULL, CONFIRM_NEVER );
  }
}

void CLegView::LegPasteAuthList(int AUorAS)
{
  if (!LegOKActDiffMoi(AUorAS))
    return;
  // Display a dialog box with all other legs
  //    to allow the user to choose which legs to copy auth to
  CLegListboxDlg LegListDlg(this, FALSE, FALSE);
  int iDlgResult = LegListDlg.DoModal();
  if ( iDlgResult == IDOK )
  {
    int* iaSelBuf = LegListDlg.GetSelectedItemsIndexArray();  // array of indexes of selected legs
    int iSelItemCount = LegListDlg.GetSelectedItemCount();  // number of selected legs

    CString szConfirmMsg;
    CString szFormat;
    szFormat = ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_CONFIRM_COPYANDSEND_SEL_LEGS );
    szConfirmMsg.Format (szFormat, iSelItemCount);
    if ( MessageBox (szConfirmMsg, NULL, MB_YESNO) == IDYES )
    {
	  // DM 7978 (souplesse tarifaire BUGI 69672), controle CC0=CC1 (plus AVT 17)
	  if (!LegOKCC0EgalCC1(AUorAS, CURRENTLY_LOADED_LEG))
	  {
		  return;
	  }
       // first, update the currently loaded leg
      UpdateLeg( AUorAS, CURRENTLY_LOADED_LEG, NULL, CONFIRM_NEVER );

      // then, loop through all the indeces and update each selected leg...
      YmIcLegsDom* pLeg = NULL;
      for (int iIndex = 0; iIndex < iSelItemCount ; iIndex++)
      {
        pLeg = (YmIcLegsDom*)LegListDlg.GetSelectedItem( iIndex );
        UpdateLeg( AUorAS, PARAMETER_PASSED_LEG, pLeg, CONFIRM_NEVER );
      }
    }
  }
}

void CLegView::LegPasteAuthAll(int AUorAS)
{
  if (!LegOKActDiffMoi(AUorAS))
    return;
  // Update ALL other legs with the currently "loaded" leg's authorizations
  CString s;
  s = ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_CONFIRM_COPYANDSEND_LEGS_ALL );
  if ( MessageBox (s, NULL, MB_YESNO) == IDYES )
  {
	  // DM 7978 (souplesse tarifaire BUGI 69672), controle CC0=CC1 (plus AVT 17)
	  if (!LegOKCC0EgalCC1(AUorAS, ALL_LEGS))
	  {
		  return;
	  }
	  UpdateLeg( AUorAS, ALL_LEGS, NULL, CONFIRM_NEVER );
  }
}

// DM 6496, controle si act diff moi non modifié fen resarail
// si oui pose question d'envoi de VNAU et renvoi vrai si user approuve et faux s'il refuse
// si non renvoi vrai.
BOOL CLegView::LegOKActDiffMoi(int AUorAS)
{
  CResaRailView* pResaRailView = GetResaRailView();
  if (pResaRailView && pResaRailView->ControlActDiffMoi(AUorAS))
  {
    CString s;
    s = ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_ACT_DIFF_MOI );
    return (MessageBox (s, NULL, MB_YESNO) == IDYES );
  }
  return TRUE;
}

void CLegView::LegNext()
{
  int iItemCount = GetListCtrl().GetItemCount();
  if( iItemCount > 1 )  // must have more than 1 item to select the next
  {
    LV_FINDINFO fi;
    fi.flags = LVFI_PARAM;
    fi.lParam = (LPARAM)m_pLegLoaded;
    int iIndex = (GetListCtrl().FindItem( &fi, -1 )) + 1;
    if (iIndex < iItemCount)  // item cannot be the last in the list
    {
      // set the edit status, then load the next item
      YmIcLegsDom* pLeg = (YmIcLegsDom*)GetListCtrl().GetItemData(iIndex);
      LoadLeg( pLeg, iIndex );
    }
    else
    {
      MessageBeep(MB_ICONEXCLAMATION);
    }
  }
}

void CLegView::LegPrev()
{
  int iItemCount = GetListCtrl().GetItemCount();
  if( iItemCount > 1 )  // must have more than 1 item to select the previous
  {
    LV_FINDINFO fi;
    fi.flags = LVFI_PARAM;
    fi.lParam = (LPARAM)m_pLegLoaded;
    int iIndex = (GetListCtrl().FindItem( &fi, -1 )) - 1;
    if (iIndex >= 0)  // item cannot be the first in the list
    {
      // load the next item
      YmIcLegsDom* pLeg = (YmIcLegsDom*)GetListCtrl().GetItemData(iIndex);
      LoadLeg( pLeg, iIndex );
    }
    else
    {
      MessageBeep(MB_ICONEXCLAMATION);
    }
  }
}

void CLegView::LegUndo()
{
  // get the selected item undo it
  int iIndex = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

  if( iIndex < 0)
    return;

  YmIcLegsDom* pLeg = (YmIcLegsDom*)GetListCtrl().GetItemData(iIndex);

  // reload this leg from the database by reloading the current tranche
  YM_RecordSet* pLegSet = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);

  m_pLegLoaded = pLeg;
  pLegSet->SetCurrent( (long)m_pLegLoaded, this );
}

// BOOL method to determine if the user has selected Default sort order
//    This method is used by OnUpdate... method for checking on or off the Leg Sort menu
BOOL CLegView::IsSortDefault()
{
  // if the user sorted on CRITICALITY (Descending), LEG_ORIG (Ascending), return TRUE, else return FALSE
  if (m_aiSortOrder.GetSize() == 2)
    if ( ((GetColId(m_aiSortOrder.GetAt(0)) & ~REVERSE_ORDER_FLAG) == CRITICALITY) &&
         (GetSortContext(m_aiSortOrder.GetAt(0)) == YM_ListView::DESCENDING) &&
        ((GetColId(m_aiSortOrder.GetAt(1)) & ~REVERSE_ORDER_FLAG) == LEG_ORIG) &&
         (GetSortContext(m_aiSortOrder.GetAt(1)) == YM_ListView::ASCENDING) )
      return TRUE;

  return FALSE;
}

// BOOL method to determine if the user has selected Reverse sort order
//    This method is used by OnUpdate... method for checking on or off the Leg Sort menu
BOOL CLegView::IsSortReverse()
{
  // if the user sorted on CRITICALITY (Descending), LEG_ORIG (Descending), return TRUE, else return FALSE
  if (m_aiSortOrder.GetSize() == 2)
    if ( ((GetColId(m_aiSortOrder.GetAt(0)) & ~REVERSE_ORDER_FLAG) == CRITICALITY) &&
         (GetSortContext(m_aiSortOrder.GetAt(0)) == YM_ListView::DESCENDING) &&
        ((GetColId(m_aiSortOrder.GetAt(1)) & ~REVERSE_ORDER_FLAG) == LEG_ORIG) &&
         (GetSortContext(m_aiSortOrder.GetAt(1)) == YM_ListView::DESCENDING) )
      return TRUE;

  return FALSE;
}


/////
// *** CANNED SORT PROCEDURES ***
/////

void CLegView::LegDefaultSort()
{
  // reverse the sort order of the legs

  // Clear all sort attibutes in the column attribute array
  // Set the sort attributes for the appropriate columns in the column attribute array
  ClearAllColAttributes();

  ColAttrStruct* pColAttribute = NULL;

  // default order is criticality descending, then leg sequence ascending
  // Primary Key is Criticality descending
  if ( (pColAttribute = GetColAttribute(CRITICALITY)) == NULL )
    return;
  SetColAttribute(pColAttribute->iColIndex, DESCENDING, TRUE);
  // Secondary Key is Leg Orig ascending
  if ( (pColAttribute = GetColAttribute(LEG_ORIG)) == NULL )
    return;
  SetColAttribute(pColAttribute->iColIndex, ASCENDING, TRUE);

  SortItems(CRITICALITY);
  RedrawColumnTitles();  // redraw all column titles
}

void CLegView::LegReverseSort()
{
  // reverse the sort order of the legs
  // Clear all sort attibutes in the column attribute array
  // Set the sort attributes for the appropriate columns in the column attribute array
  ClearAllColAttributes();

  ColAttrStruct* pColAttribute = NULL;

  // default order is criticality descending, then leg sequence ascending
  // Primary Key is Criticality descending
  if ( (pColAttribute = GetColAttribute(CRITICALITY)) == NULL )
    return;
  SetColAttribute(pColAttribute->iColIndex, DESCENDING, TRUE);
  // Secondary Key is Leg Orig descending
  if ( (pColAttribute = GetColAttribute(LEG_ORIG)) == NULL )
    return;
  SetColAttribute(pColAttribute->iColIndex, DESCENDING, TRUE);

  SortItems(CRITICALITY);
  RedrawColumnTitles();  // redraw all column titles
}

void CLegView::LegSelPeak()
{
  m_bSelPeak = !m_bSelPeak;
  if (m_bSelPeak)
  { // Il faut sélectionner le PeakLeg
    CTrancheView* pTrancheView = GetTrancheView();
    if (pTrancheView)
    {
      // Anomalie SATC 7.4.8 48254 : Menu Tronçon/Sélection Peak Leg : Sur les entités UNKNOWN et NONGERE, apparition d'un bug
	  // rajouter l'initialisation de la variable ci-dessous à NULL
	  // Anomalie retrouvée en qualif 49801 de la version 7.5.1 du SATC
	  YmIcLegsDom* pLoadLeg = NULL;
	  int iIndex = 0;
      YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
      YmIcLegsDom* pPeakLeg = FindLeg(pLoadedTranche->PeakLeg(), "", iIndex);
      if (pPeakLeg)
        pLoadLeg = pPeakLeg;  // found the peak leg ... load it
      // Anomalie SATC 7.4.8 48254 : Menu Tronçon/Sélection Peak Leg : Sur les entités UNKNOWN et NONGERE, apparition d'un bug
	  // tester la variable ci-dessous à NULL
	  if (pLoadLeg != NULL)
	  {
          LoadLeg( pLoadLeg, iIndex );  // load 1st element in leg list by default
	  }
	}
  }
   CString szProfileKey;
  ((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);
  APP->WriteProfileInt(szProfileKey, (LPCSTR)szLoadPeakConst, m_bSelPeak);
}


CResaRailView* CLegView::GetResaRailView()
{
  return ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetResaRailView ();
}

// Set the parameter for the SQL Update
BOOL CLegView::SetUpdateParm(
                             enum LegUpdateMode eUpdateMode, 
                             YmIcLegsDom** pLegIn,
                             YmIcLegsDom** pLegOut, 
                             char** pszLegOrigParm)
{
  CListCtrl& ListCtrl = GetListCtrl();
  BOOL bForceAuthMod = FALSE;  // default to update of the currently loaded leg
  switch( eUpdateMode )   // how do we set the leg origin parameter ?
  {
    case CURRENTLY_LOADED_LEG:
    {
      // set the LegOrig parameter for the SQL update stmt to be the leg origin
      //   for this "loaded" leg which means that the "Like" clause in the stmt
      //   will update only this leg

      *pLegOut = m_pLegLoaded;
      *pszLegOrigParm = (char*)(*pLegOut)->LegOrig();

      break;
    }
    case CURRENTLY_SELECTED_LEG:
    {
      // set the LegOrig parameter for the SQL update stmt to be the leg origin
      //   for this "selected" leg which means that the "Like" clause in the stmt
      //   will update only this leg
      //
      // set this flag true because we are updating a leg other than the currently
      //   loaded leg.  this means that the auth values will not be flagged as "modified",
      //   and, unless we "force" them, they will not be updated.
      bForceAuthMod = TRUE;

      // get the selected item (not the loaded item)
      int iIndex = ListCtrl.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
      LPARAM lParam = ListCtrl.GetItemData(iIndex);
      *pLegOut = (YM_Set<YmIcLegsDom>::FromKey(LEG_KEY))->GetRecord(lParam);
      *pszLegOrigParm = (char*)(*pLegOut)->LegOrig();
      break;
    }
    case PARAMETER_PASSED_LEG:
    {
      // set the LegOrig parameter for the SQL update stmt to be the leg origin
      //   for the leg which is passed in as a parameter.
      //   this means that the "Like" clause in the stmt will update only this leg
      //
      // set this flag true because we are updating a leg other than the currently
      //   loaded leg.  this means that the auth values will not be flagged as "modified",
      //   and, unless we "force" them, they will not be updated.
      bForceAuthMod = TRUE;

      *pszLegOrigParm = (char*)(*pLegIn)->LegOrig();
      *pLegOut = *pLegIn;
      break;
    }
    case ALL_LEGS:
    {
      // set this flag true because we are updating a leg other than the currently
      //   loaded leg.  this means that the auth values will not be flagged as "modified",
      //   and, unless we "force" them, they will not be updated.
      bForceAuthMod = TRUE;

      // set the LegOrig parameter for the SQL update stmt to be a percent sign
      //   which means that the "Like" clause in the stmt will update ALL legs
      *pszLegOrigParm = "%";

      break;
    }
    default:
    {
      break;
    }
  }
  return bForceAuthMod;
}

void CLegView::UpdateLeg(int AUorAS,
                         enum LegUpdateMode eUpdateMode, 
                         YmIcLegsDom* pLegIn /* = NULL */, 
                         enum LegConfirmUpdateMode eConfirmMode /* = CONFIRM_ALWAYS */)
{
  // We will update either ALL legs in the current leg list OR
  //   only the LOADED leg OR only the SELECTED (NOT LOADED) leg 
  //   depending upon the parameter passed in.
  //   NOTE: all updated values are FROM the currently selected leg.
  //   don't forget to set the status for all appropriate legs...
  //

  // if we've not loaded any leg yet, let's not update anything
  if (m_pLegLoaded == NULL)  // set in LoadLeg(...)
    return;

  if (m_bAdaptationFlag)
  {
    eUpdateMode = ALL_LEGS;
	pLegIn = NULL;
	eConfirmMode = CONFIRM_NEVER;
  }
/*
  if (GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED) < 0)
    return;
*/
  COLORREF clrFt;
  clrFt = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();
  SetPropertyLevelColor (3, clrFt);

  CResaRailView* pResaRailView = GetResaRailView();

  // The variable 'bForceAuthMod' is TRUE when the currently loaded leg
  //  is NOT the leg which is being updated.  The reason for this flag is,
  //  when we are updating the auth values, we need to know whether or
  //  not to consider the flag which tells us when the auth values for a bkt
  //  are modified.  That is, if we are updating the auth values for a leg 
  //  OTHER THAN the currently loaded leg, we do not care about the flags,
  //  we just update without asking questions, and do not reset the flag.
  //  This happens in ResaRailView::UpdateBuckets.
  //
  YmIcLegsDom* pLegOut = NULL;
  char* pszLegOrigParm = 0;  // parameter for the SQL update statements
  BOOL bForceAuthMod = SetUpdateParm(eUpdateMode, &pLegIn, &pLegOut, &pszLegOrigParm);
  ASSERT( pszLegOrigParm );

  // Update the compartments and buckets for the leg(s)
  //
  BOOL bConfirm;
  if ( pszLegOrigParm[0] != '%' )
  {
    // Update and send VNAU for only the specified leg...
    (eConfirmMode == CONFIRM_ALWAYS || eConfirmMode == CONFIRM_ONCE ) ?
      bConfirm = TRUE : bConfirm = FALSE;
    if ( pResaRailView )
	{
      pResaRailView->SaveAllModified( AUorAS, pLegOut, eUpdateMode, bForceAuthMod, bConfirm );
	}
  }
  else  // the SQL parameter for leg_orig was "%", meaning update ALL legs
  {
    // Dm 5480, dans le cadre des trains auto, a été réalisé une methode pResaRailView->SaveOnAllLegs()
    // Il convient de l'utiliser plutôt que de boucler sur l'ensemble des tronçons.
    // Note: S'il y avait besoin de confirmation, cela a déjà été fait par AutoCopyConfirm.
      if ( pResaRailView )  
          pResaRailView->SaveOnAllLegs(AUorAS);
  }

  GetListCtrl().Invalidate();
  GetListCtrl().UpdateWindow();
}

void CLegView::LegCriticalMessages()
{
    m_bTooltips = !m_bTooltips;
}

void CLegView::LegHelp()
{
  // first, destroy the existing help dialog
  ((CMainFrame*) AfxGetApp()->GetMainWnd())->DestroyHelpDlg();
  
  // set the help dlg window as member variable here and in CMainFrame
  CreateHelpDlg();
  DisplayHelpDlg();
}

CCTAViewHelpDlg* CLegView::CreateHelpDlg()
{
  m_pHelpDlg = new CLegViewHelpDlg( this );
  m_pHelpDlg->Create( this );
  ((CMainFrame*) AfxGetApp()->GetMainWnd())->SetHelpDlg(m_pHelpDlg);
  return (m_pHelpDlg);
}

void CLegView::DisplayHelpDlg()
{
  ASSERT(m_pHelpDlg != NULL);

  // try to position it where the user last left it
  CPoint* pPrevPoint = 
    ((CMainFrame*) AfxGetApp()->GetMainWnd())->GetHelpDlgPoint();

  m_pHelpDlg->SetWindowPos( NULL, pPrevPoint->x, pPrevPoint->y, 0, 0, 
                            SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW );
}

// return TRUE if the selected leg is the loaded leg, else FALSE 
BOOL CLegView::IsSelectedLegLoaded()
{
  if( GetListCtrl().GetItemCount() > 0 ) 
  {
    int iIndex = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
    if(  iIndex >= 0 )
    {
      YmIcLegsDom * lp = (YmIcLegsDom*)GetListCtrl().GetItemData(iIndex);
      return ( m_pLegLoaded == (YmIcLegsDom*)GetListCtrl().GetItemData(iIndex) );
    }
    else
      return ( FALSE );
  }
  else
  {
    return ( FALSE );
  }
}


/////////////////////////////////////////////////////////////////////////////
// CLegView message handlers

void CLegView::EnableToolTips() 
{
  // enable tooltips only if this client does NOT have multiple critical rules ...
  GetListCtrl().EnableToolTips( true ); 
}

CString CLegView::GetToolTipText(LVITEM* pItem) const 
{
  CString szText;
  YM_RecordSet* pRecordSet = GetRecordSet();
  YmIcLegsDom* pLeg = (YmIcLegsDom*)pItem->lParam;
  int todo = 0;
  if (pRecordSet && pRecordSet->IsValid())
  {
    if ( (pItem->iSubItem == LEG_ORIG) )
      szText = pLeg->LegOrigDesc();
    else if ( (pItem->iSubItem == LEG_DEST) )
      szText = pLeg->LegDestDesc();
    else if ( (pItem->iSubItem == STATUS) ) // actions commerciales
	{
	  todo = 1; //szText = pTrancheView->BuildTDLJKToolTip (pLoadedTranche, pLeg, false);
	  			// requête de récup des MDI
	  CString szClient = App().m_pDatabase->GetClientParameter();
	  if ((szClient == SNCF) || (szClient == SNCF_TRN))
	  {
	    YmIcLegsDom LegDom;
        YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	    if ( ( CString(pTranche->OdFermees()) == "ODI" ) ||
			 ( CString(pTranche->OdFermees()) == "PDI" ) )
	    {
	      YM_Query* pSQL = new YM_Query (*((CCTAApp*)APP)->m_pDatabase, FALSE);
	      LegDom.TrancheNo(pTranche->TrancheNo());
	      LegDom.DptDate(pTranche->DptDate());
	      pSQL->SetDomain(&LegDom);
	      LegDom.J_X(pTranche->LastJX());
	      RWDBReader Reader( ((CCTAApp*)APP)->m_pDatabase->Transact(pSQL, IDS_SELECT_OD_FERMEES));
	      while (Reader())
	      {
	        if (Reader.isValid())
	        {
		      Reader >> LegDom;
			  szText += "ODI  " + CString(LegDom.LegOrig()) + "-" + CString(LegDom.LegDest()) + "\r\n";
		    }
	      }
	      delete pSQL;
	    }
	  }
	}
	else  // criticité
	  todo = 2; //szText = pTrancheView->BuildTDLJKToolTip (pLoadedTranche, pLeg, true);
	if (todo)
	{
	  YM_Iterator<YmIcTDLJKDom>* pIterator = 
        YM_Set<YmIcTDLJKDom>::FromKey( TDLJK_KEY)->CreateIterator();
      int iIndex = 0;
      YmIcTDLJKDom* pRecord = NULL;
	  CString limess;
	  char sty[2];
	  sty[0] = (todo==1) ? 'O' : 'I';
	  sty[1] = 0;
	  for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
      {
        pRecord = (YmIcTDLJKDom*)pIterator->Current();
	    if ((pRecord->TrancheNo() == pLeg->TrancheNo()) &&
		    (pRecord->DptDate() == pLeg->DptDate()) &&
			(!strcmp (pRecord->LegOrig(), pLeg->LegOrig())) &&
		    (!strcmp (pRecord->TypeRegle(), sty)))
	    {
	      if (!szText.IsEmpty())
		     szText += "\r\n";
	      limess.Format ("%02d %s", pRecord->Criticality(), pRecord->Message());
	      szText += limess;
	    }
      }
      delete pIterator;
	}
  }
  szText.TrimRight();
  return szText;
}

void CLegView::OnPaint()
{
  // CME - Ano 79307
  //COLORREF clrBk = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();
  GetListCtrl().SetBkColor(m_clrItemBK);

  COLORREF clrFt;
  clrFt = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();
  SetPropertyLevelColor (3, clrFt);

  YM_ListView::OnPaint();
}

void CLegView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  int iIndex;
  UINT uiFlags = LVHT_ONITEMICON | LVHT_ONITEMLABEL;
  CPoint clPoint(point);
  GetListCtrl().ScreenToClient( &clPoint );

  YM_ListView::OnContextMenu(pWnd, point);  // base class implementation

  if (OnHeaderCtrlHitTest(clPoint))  // is cursor over the header control ?
    return;  // do nothing

  if( (iIndex = GetListCtrl().HitTest( clPoint, &uiFlags ) ) >= 0 )
  {
    LPARAM lParam = GetListCtrl().GetItemData(iIndex);
    YmIcLegsDom* pLegHit = 
      (YM_Set<YmIcLegsDom>::FromKey(LEG_KEY))->GetRecord(lParam);

    BOOL bIsLegSel = (YmIcLegsDom*)lParam == m_pLegLoaded;
    BOOL bCopie = FALSE;
	BOOL bVnas = FALSE;
	CTrancheView* pTrancheView = GetTrancheView();
    if (pTrancheView)
    {
      YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
	  if (pLoadedTranche)
	  {
		bCopie = pLoadedTranche->Nature() == 0;
		bVnas = pLoadedTranche->ScxCount() > 0;
	  }
	}
	// DM 5480 train auto
	CResaRailView* pResaRailView = GetResaRailView();
	if (pResaRailView && bCopie && pResaRailView->TrainAuto())
	  bCopie = FALSE;
   
    CMenu menu;
	menu.LoadMenu(IDR_CTA_LEGLIST_POP);
	
    
    // enable and check the various menu items
	
    menu.EnableMenuItem( ID_LEG_LOAD, bIsLegSel ? MF_DISABLED | MF_GRAYED : MF_ENABLED );
    menu.CheckMenuItem(  ID_LEG_LOAD, bIsLegSel ? MF_CHECKED : MF_UNCHECKED );
	menu.EnableMenuItem( ID_LEG_SEND, 
        ! bIsLegSel || ( GetListCtrl().GetItemCount() <= 0 ) /*|| ( ! ((CCTAApp*)APP)->GetRealTimeMode() ) */?  
        MF_DISABLED | MF_GRAYED : MF_ENABLED );
	
    menu.EnableMenuItem( ID_LEG_PASTE_AUTH_VNAU, bIsLegSel || m_bAutoCopyAuth || !bCopie ? MF_DISABLED | MF_GRAYED : MF_ENABLED );
	menu.EnableMenuItem( ID_LEG_PASTE_AUTH_VNAS, bIsLegSel || m_bAutoCopyAuth || !bCopie  || !bVnas ? MF_DISABLED | MF_GRAYED : MF_ENABLED );
	menu.EnableMenuItem( ID_LEG_PASTE_AUTH_LES2, bIsLegSel || m_bAutoCopyAuth || !bCopie  || !bVnas ? MF_DISABLED | MF_GRAYED : MF_ENABLED );
    menu.EnableMenuItem( ID_LEG_SELECT_DLG_VNAU,
      ( GetListCtrl().GetItemCount() > 1 ) && ! m_bAutoCopyAuth && bCopie ? MF_ENABLED : MF_DISABLED | MF_GRAYED );
	menu.EnableMenuItem( ID_LEG_SELECT_DLG_VNAS,
      ( GetListCtrl().GetItemCount() > 1 ) && ! m_bAutoCopyAuth && bCopie && bVnas ? MF_ENABLED : MF_DISABLED | MF_GRAYED );
	menu.EnableMenuItem( ID_LEG_SELECT_DLG_LES2,
      ( GetListCtrl().GetItemCount() > 1 ) && ! m_bAutoCopyAuth && bCopie && bVnas ? MF_ENABLED : MF_DISABLED | MF_GRAYED );
	menu.EnableMenuItem( ID_LEG_PASTE_AUTH_ALL_VNAU, 
      ( GetListCtrl().GetItemCount() > 1 ) && ! m_bAutoCopyAuth && bCopie ? MF_ENABLED : MF_DISABLED | MF_GRAYED );
    menu.EnableMenuItem( ID_LEG_PASTE_AUTH_ALL_VNAS, 
      ( GetListCtrl().GetItemCount() > 1 ) && ! m_bAutoCopyAuth && bCopie && bVnas ? MF_ENABLED : MF_DISABLED | MF_GRAYED );
	menu.EnableMenuItem( ID_LEG_PASTE_AUTH_ALL_LES2, 
      ( GetListCtrl().GetItemCount() > 1 ) && ! m_bAutoCopyAuth && bCopie && bVnas ? MF_ENABLED : MF_DISABLED | MF_GRAYED );
    menu.EnableMenuItem( ID_LEG_UNDO, bIsLegSel ? MF_ENABLED : MF_DISABLED | MF_GRAYED );
    menu.EnableMenuItem( ID_LEG_ADJUSTMENT, bIsLegSel ? MF_ENABLED : MF_DISABLED | MF_GRAYED );
    menu.CheckMenuItem(  ID_LEG_ADJUSTMENT, m_bOvbAdjust ? MF_CHECKED : MF_UNCHECKED );
    menu.CheckMenuItem(  ID_LEG_CRITICAL_MESSAGES, m_bTooltips ? MF_CHECKED : MF_UNCHECKED );
    // show the popup menu at the point of the cursor (point of the right single click)
    // menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    //menu.Detach( );
  }
}

void CLegView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
  if( pHint == NULL )
    return;

  // If notification comes from a RecordSet, pHint is a RecordSet*
  int iLegKey = LEG_KEY;
  YM_RecordSet* pLegSet = YM_Set<YmIcLegsDom>::FromKey(iLegKey);
  if( pHint == pLegSet )
  {
    EnableWindow( pLegSet->IsValid() );
    CTrancheView* pTrancheView = GetTrancheView();
	YmIcTrancheLisDom* pLoadedTranche = NULL;
    if (pTrancheView)
      pLoadedTranche = pTrancheView->GetLoadedTranche();

    if( pLegSet->IsValid() ) 
    {
      LV_ITEM Item;
      GetListCtrl().DeleteAllItems();
      YmIcLegsDom* pLeg = NULL;
      YM_Iterator<YmIcLegsDom>* pIterator = 
      YM_Set<YmIcLegsDom>::FromKey(iLegKey)->CreateIterator();
      int iIndex = 0;
      for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
      {
        pLeg = pIterator->Current();

		/// DM-7978 DT8805 - CLE - get CCMAX et CCOUV pour chaque legs
		((CCTAApp*)APP)->GetDocument()->UpdateCCMaxCCOuvLegs(pLeg);

        Item.mask = LVIF_TEXT | LVIF_PARAM;
        Item.iItem = iIndex++;
        Item.iSubItem = 0;
        Item.pszText = LPSTR_TEXTCALLBACK;
        Item.lParam = (LONG)pIterator->Current();
        GetListCtrl().InsertItem( &Item );
      }
      delete pIterator;

      if( GetListCtrl().GetItemCount() > 0 ) 
      {
        // sort the items in default order, this sets the member variable m_lCurSortIndex
        SortItems(CRITICALITY);

		int nIndex = 0;
		if (GetListCtrl().GetItemCount() > m_index && m_index >= 0)
			nIndex = m_index;
        YmIcLegsDom* pLoadLeg = (YmIcLegsDom*)GetListCtrl().GetItemData(nIndex);  // default leg to load

        // if TRN Client AND the primary Leg List (ie, not alternate leg list)
        if ( m_bSelPeak )
        {
          // get the Peak Leg from the Tranche View
		  if (pLoadedTranche && m_index == -1)
		  {
            YmIcLegsDom* pPeakLeg = FindLeg(pLoadedTranche->PeakLeg(), "", nIndex);
            if (pPeakLeg)
              pLoadLeg = pPeakLeg;  // found the peak leg ... load it
          }
        }

        LoadLeg( pLoadLeg, nIndex );  // load 1st element in leg list by default

		m_index = -1;
        // Set text in status bar control leg list window
        ((CChildFrame*)GetParentFrame())->
          GetStatusBar()->SetPaneOneAndLock(
            ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DONE) );
      }
      else
      {
        // Set text in status bar control leg list window
        ((CChildFrame*)GetParentFrame())->
          GetStatusBar()->SetPaneOneAndLock(
            ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA) );
      }
    }
    else    // record set is not valid 
    {
      GetListCtrl().DeleteAllItems();

      // Set text in status bar control leg list window
      ((CChildFrame*)GetParentFrame())->
      GetStatusBar()->SetPaneOneAndLock(
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_READING_LEGS) );

      m_pLegLoaded = NULL;
    }
	if (pLoadedTranche)
	{
	  CString sTitle = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_LEG_TITLE);
	  if (!strlen (pLoadedTranche->PeakLeg()))
	    sTitle += ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_PEAK_LEG);
	  else
	  {
	    int index =0;
	    YmIcLegsDom* pPeakLeg = FindLeg(pLoadedTranche->PeakLeg(), "", index);
		if (pPeakLeg)
		{
		  CString sss;
		  sss.Format ("    Peak: %s - %s", pPeakLeg->LegOrig(), pPeakLeg->LegDest());
		  sTitle += sss;
		}
	  }
		
	  CString chKey = ((YM_PersistentChildWnd*)GetParentFrame())->GetChildKey ();
	  if (!chKey.IsEmpty()) // Ne pas modifier le titre tant que la childKey n'est pas en place
			GetParentFrame()->SetWindowText(sTitle);  // sinon pb sauvegarde configuration
 
	}
  }
}



int CLegView::CriticalityRev(YmIcLegsDom* pLeg)
{
  YM_RecordSet* pTDLJKSet = YM_Set<YmIcTDLJKDom>::FromKey( TDLJK_KEY);
  if (pTDLJKSet->IsValid())
  {
	YM_Iterator<YmIcTDLJKDom>* pIterator2 = YM_Set<YmIcTDLJKDom>::FromKey( TDLJK_KEY)->CreateIterator();
    int icount = 0;
    YmIcTDLJKDom* pRecord = NULL;
	CString mess1;
	int crit1 = 0;
	for( pIterator2->First(); !pIterator2->Finished(); pIterator2->Next() ) 
    {
      pRecord = (YmIcTDLJKDom*)pIterator2->Current();
	  if ((pRecord->TrancheNo() == pLeg->TrancheNo()) &&
		  (pRecord->DptDate() == pLeg->DptDate()) &&
		  (!strcmp (pRecord->LegOrig(), pLeg->LegOrig())) &&
		  (!strcmp (pRecord->TypeRegle(), "I")))
	  {
		icount++;
		if (icount == 1)
		{
		  mess1 = pRecord->Message();
		  crit1 = pRecord->Criticality();
		}
		else if ((mess1 != pRecord->Message()) || (crit1 != pRecord->Criticality()))
		{
		  delete pIterator2;
		  return (pRecord->Criticality());	
		}
	  }
	}
	delete pIterator2;
	return 0;
  }
  return -1;
}

int CLegView::FirstCriticality(YmIcLegsDom* pLeg)
{
  YM_RecordSet* pTDLJKSet = YM_Set<YmIcTDLJKDom>::FromKey( TDLJK_KEY);
  if (pTDLJKSet->IsValid())
  {
	YM_Iterator<YmIcTDLJKDom>* pIterator2 = YM_Set<YmIcTDLJKDom>::FromKey( TDLJK_KEY)->CreateIterator();
    YmIcTDLJKDom* pRecord = NULL;
	for( pIterator2->First(); !pIterator2->Finished(); pIterator2->Next() ) 
    {
      pRecord = (YmIcTDLJKDom*)pIterator2->Current();
	  if ((pRecord->TrancheNo() == pLeg->TrancheNo()) &&
		  (pRecord->DptDate() == pLeg->DptDate()) &&
		  (!strcmp (pRecord->LegOrig(), pLeg->LegOrig())) &&
		  (!strcmp (pRecord->TypeRegle(), "I")))
	  {
		delete pIterator2;
		return (pRecord->Criticality());	
	  }
	}
	delete pIterator2;
	return 0;
  }
  return -1;
}

BOOL CLegView::DrawHeaderImage()  // do we draw header images or simply text ?
{
  return ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetHeaderImageFlag() );
}

void CLegView::WriteViewConfig() 
{
  // write view specific information to the registry
  //
  YM_ListView::WriteViewConfig();

  CString szProfileKey;
  ((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

  // save the auto copy auth flag in the registry
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAutoCopyAuthFlag() )
    APP->WriteProfileInt(szProfileKey, (LPCSTR)szAutoCopyAuthConst, m_bAutoCopyAuth);

  // save the preserve differences flag in the registry
  APP->WriteProfileInt(szProfileKey, (LPCSTR)szPreserveDiffConst, m_bOvbAdjust);

}
 
void CLegView::RestoreViewConfig() 
{
  // restore view specific information from the registry
  //
  YM_ListView::RestoreViewConfig();

  CString szProfileKey;
  ((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);

  RestoreColumnWidth();

  // save the auto copy auth flag in the registry
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAutoCopyAuthFlag() )
    m_bAutoCopyAuth = APP->GetProfileInt(szProfileKey, (LPCSTR)szAutoCopyAuthConst, FALSE);

  // save the preserve differences flag in the registry
  m_bOvbAdjust = APP->GetProfileInt(szProfileKey, (LPCSTR)szPreserveDiffConst, TRUE);
   // CME Ano 79307
  //m_clrItemBK = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();
  m_clrItemBK = RGB (255, 255, 255);
}

BOOL CLegView::SaveAllModified() 
{
  // manually launch the query to update the selected tranche...
  // create new tranche record and copy from record set
  int iLegKey = LEG_KEY;
  YM_RecordSet* pLegSet = YM_Set<YmIcLegsDom>::FromKey(iLegKey);
  if (! pLegSet->IsValid())
    return FALSE;

  YmIcLegsDom* pRecord = new YmIcLegsDom(*(YM_Set<YmIcLegsDom>::FromKey(iLegKey)));

  if ( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetUpdateSynchroneFlag() )
  { // 17/05/06, suite a plainte des utilisateurs sur le temps de réponse globale, on remeet en place
	// l'update désynchronisé. Mais par sécurité je rend ceci paramétrable.
    YM_Query* pUpdateQuery = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), TRUE);
    pUpdateQuery->Start( pRecord, IDS_SQL_UPDATE_LEG_LIST );
  }
  else
  {
    // Utilisation d'une requete synchrone de mise à jour de sc_legs pour eviter le problème
    // s'étant produiy le 25/10/2005, lorsque des deletes massifs étaient fait en base bpsn001 sur sc_legs
    // PB de desynchronisation, la query était détruite avant de passer dans le OnWorkFinished
    ((CCTAApp*)APP)->UpdateOneLeg (pRecord);
    delete pRecord;
  }

  return TRUE;
}

void CLegView::OnLegLoad()
{
  // get the selected item and load it
  int iIndex = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

  if( iIndex < 0 )
    return;

  YmIcLegsDom* pLeg = 
    (YmIcLegsDom*)GetListCtrl().GetItemData(iIndex);

  LoadLeg( pLeg, iIndex );
}

void CLegView::OnUpdateLegLoad( CCmdUI* pCmdUI )
{
  BOOL bLoaded = IsSelectedLegLoaded();
  pCmdUI->SetCheck( bLoaded );
  pCmdUI->Enable( !bLoaded );
}

// paste the authorizations of the loaded leg to the selected leg
void CLegView::OnLegPasteAuthSelectedVnau()
{
  LegPasteAuthSelected(1);
}

// paste the authorizations of the loaded leg to the selected leg
void CLegView::OnLegPasteAuthSelectedVnas()
{
  LegPasteAuthSelected(2);
}

// paste the authorizations of the loaded leg to the selected leg
void CLegView::OnLegPasteAuthSelectedLes2()
{
  LegPasteAuthSelected(3);
}

void CLegView::OnUpdateLegPasteAuthSelected( CCmdUI* pCmdUI )
{
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
  pCmdUI->Enable( GetListCtrl().GetItemCount() > 1 && ! IsSelectedLegLoaded() && ! m_bAutoCopyAuth && bCopie );
}

void CLegView::OnUpdateLegPasteAuthSelectedVnas( CCmdUI* pCmdUI )
{
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
  pCmdUI->Enable( GetListCtrl().GetItemCount() > 1 && ! IsSelectedLegLoaded() && ! m_bAutoCopyAuth && bCopie );
}

// paste the authorizations of the loaded leg to the legs selected in the dialog listbox
void CLegView::OnLegPasteAuthListVnau()
{
  LegPasteAuthList(1);
}
// paste the authorizations of the loaded leg to the legs selected in the dialog listbox
void CLegView::OnLegPasteAuthListVnas()
{
  LegPasteAuthList(2);
}
// paste the authorizations of the loaded leg to the legs selected in the dialog listbox
void CLegView::OnLegPasteAuthListLes2()
{
  LegPasteAuthList(3);
}

void CLegView::OnUpdateLegPasteAuthList( CCmdUI* pCmdUI )
{
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
  pCmdUI->Enable( GetListCtrl().GetItemCount() > 1 && ! m_bAutoCopyAuth && bCopie );
}

void CLegView::OnUpdateLegPasteAuthListVnas( CCmdUI* pCmdUI )
{
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
  pCmdUI->Enable( GetListCtrl().GetItemCount() > 1 && ! m_bAutoCopyAuth && bCopie );
}

// paste the authorizations of the loaded leg to all other legs
void CLegView::OnLegPasteAuthAllVnau()
{
  LegPasteAuthAll(1);
}
// paste the authorizations of the loaded leg to all other legs
void CLegView::OnLegPasteAuthAllVnas()
{
  LegPasteAuthAll(2);
}
// paste the authorizations of the loaded leg to all other legs
void CLegView::OnLegPasteAuthAllLes2()
{
  LegPasteAuthAll(3);
}

void CLegView::OnUpdateLegPasteAuthAll( CCmdUI* pCmdUI )
{
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
  pCmdUI->Enable( GetListCtrl().GetItemCount() > 1 && ! m_bAutoCopyAuth && bCopie );
}

void CLegView::OnUpdateLegPasteAuthAllVnas( CCmdUI* pCmdUI )
{
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
  pCmdUI->Enable( GetListCtrl().GetItemCount() > 1 && ! m_bAutoCopyAuth && bCopie );
}

void CLegView::OnLegSend()
{
  LegSend();
}

void CLegView::OnLegSendNoQuery()
{
  LegSend( FALSE );
}

void CLegView::OnUpdateLegSend( CCmdUI* pCmdUI )
{
  pCmdUI->Enable( IsSelectedLegLoaded() /*&& ((CCTAApp*)APP)->GetRealTimeMode()*/ );
}

void CLegView::OnLegNext()
{
  LegNext();
}

void CLegView::OnUpdateLegNext( CCmdUI* pCmdUI )
{
  if( GetListCtrl().GetItemCount() > 0 ) 
  {
    // enable the option if the last item is not selected
    int iIndex = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
    pCmdUI->Enable( iIndex != (GetListCtrl().GetItemCount() - 1) );
  }
  else
  {
    pCmdUI->Enable( FALSE );
  }
}

void CLegView::OnLegPrev()
{
  LegPrev();
}

void CLegView::OnUpdateLegPrev( CCmdUI* pCmdUI )
{
  if( GetListCtrl().GetItemCount() > 0 ) 
    pCmdUI->Enable( GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED) > 0 );
  else
    pCmdUI->Enable( FALSE );
}

void CLegView::OnLegUndo()
{
  LegUndo();
}

void CLegView::OnUpdateLegUndo( CCmdUI* pCmdUI )
{
  pCmdUI->Enable( IsSelectedLegLoaded() );
}

void CLegView::OnLegAdjust()
{
  m_bOvbAdjust = ! m_bOvbAdjust;
}

void CLegView::OnUpdateLegAdjust( CCmdUI* pCmdUI )
{
  // Ano 146023, doit être disponible quelque soit le tronçon sélectionné
  //pCmdUI->Enable( IsSelectedLegLoaded() );
	pCmdUI->Enable( TRUE );
  pCmdUI->SetCheck( m_bOvbAdjust );
}

void CLegView::OnLegAutoCopyAuth()
{
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAutoCopyAuthFlag() )
    m_bAutoCopyAuth = ! m_bAutoCopyAuth;
}

void CLegView::OnUpdateLegAutoCopyAuth( CCmdUI* pCmdUI )
{
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAutoCopyAuthFlag() )
  {
    pCmdUI->Enable( GetListCtrl().GetItemCount() > 0 );
    pCmdUI->SetCheck( m_bAutoCopyAuth );
  }
}

void CLegView::OnLegCriticalMessages()
{
  LegCriticalMessages();
}

void CLegView::OnUpdateLegCriticalMessages( CCmdUI* pCmdUI )
{
  if( GetListCtrl().GetItemCount() <= 0 ) 
  {
    pCmdUI->Enable( FALSE );
    return;
  }
  pCmdUI->SetCheck( m_bTooltips );
}

void CLegView::OnLegDefaultSort()
{
  LegDefaultSort();
}

void CLegView::OnUpdateLegDefaultSort( CCmdUI* pCmdUI )
{
  pCmdUI->Enable( GetListCtrl().GetItemCount() > 0 );
  pCmdUI->SetCheck( IsSortDefault() ) ;
}

void CLegView::OnLegReverseSort()
{
  LegReverseSort();
}

void CLegView::OnUpdateLegReverseSort( CCmdUI* pCmdUI )
{
  pCmdUI->Enable( GetListCtrl().GetItemCount() > 0 );
  pCmdUI->SetCheck( IsSortReverse() ) ;
}

void CLegView::OnLegSelPeak()
{
  LegSelPeak();
}

void CLegView::OnUpdateLegSelPeak( CCmdUI* pCmdUI )
{
  pCmdUI->Enable( GetListCtrl().GetItemCount() > 0 );
  pCmdUI->SetCheck( IsSelPeak() ) ;
}


/////////////////////////////////////////////////////////////////////////////
// CLegViewHelpDlg dialog

CLegViewHelpDlg::CLegViewHelpDlg(CLegView* pParent /*=NULL*/)
  : CCTAViewHelpDlg(pParent)
{
}

void CLegViewHelpDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLegViewHelpDlg)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLegViewHelpDlg, CCTAViewHelpDlg)
  //{{AFX_MSG_MAP(CLegViewHelpDlg)
  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CLegViewHelpDlg::DrawLine( 
                                   CPaintDC& dc, CImageList* pImageList, 
                                   int iIconId, CPoint& ptDrawIcon, 
                                   int iLabelId, CPoint& ptDrawTxt )
{
  // Extract and Draw the icon
  DrawIconEx (dc.m_hDC, ptDrawIcon.x, ptDrawIcon.y, pImageList->ExtractIcon(iIconId),
    iBitMapSizeConst, iBitMapSizeConst, 0, NULL, DI_NORMAL );

  // Draw the text
  CString szText;
  szText.Format( "%s", 
    ((CCTAApp*)APP)->GetResource()->LoadResString(iLabelId));
  CSize sizeText = dc.GetOutputTextExtent( szText );
  CRect rectText( ptDrawTxt.x, ptDrawTxt.y, ptDrawTxt.x+sizeText.cx, ptDrawTxt.y+sizeText.cy);
  dc.DrawText( szText, rectText, DT_RIGHT | DT_SINGLELINE );

  ptDrawIcon.Offset(0, (sizeText.cy * 1));  // skip down 1 line
  ptDrawTxt.Offset(0, (sizeText.cy * 1));  // skip down 1 line
}

//////////////////////////////////////
// CLegViewHelpDlg message handlers

void CLegViewHelpDlg::OnPaint()
{
#define NUMBER_BUTTONS 3
#define BMP_TXT_SPACING 10  // randomly chosen spacing between bitmap and text
#define BORDER 30  // border on either side of bitmap and text, randomly chosen
#define NUM_ITEMS 3  // number of bitmaps to be drawn

  int iNumItems = NUM_ITEMS;
  
  iNumItems+=7;

  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSCXFlag() )
    iNumItems+=3;
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
    iNumItems+=2;

  CString szClient = App().m_pDatabase->GetClientParameter();
  if (szClient == SNCF)
    iNumItems+=2; // pour les icones spécifiques ALLEO
  CPaintDC dc(this); // device context for painting
  // initialize the system-wide selected font
//  CFont* pOldFont = dc.SelectObject( ((CCTAApp*)APP)->GetDocument()->GetStdFont(NULL) );
  CFont* pOldFont = dc.SelectObject( GetHelpTextFont() );

  // get the text extents of the longest help text to be drawn
  CString szText;
  szText.Format("%s", 
    ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_SENT_UCH));
  CSize sizeText = dc.GetOutputTextExtent( szText );

  // determine the difference between the height of client and frame
  CRect rectDlg;
  CRect rectClient;
  GetWindowRect( &rectDlg );
  GetClientRect( &rectClient );
  long lHeightDiff = rectDlg.Height() - rectClient.Height();

  // calculate the space in which to draw
  int iDrawRegionHeight = (sizeText.cy) * (2 + iNumItems);
  int iDrawRegionWidth = iBitMapSizeConst + BMP_TXT_SPACING + sizeText.cx + (BORDER*2);

  // adjust the button
  // adjust the bottom of the dialog box and resize the window
  CRect rectButton;
  GetDlgItem( IDC_CLOSE_BUTTON )->GetWindowRect(&rectButton);
  rectDlg.bottom = rectDlg.top + lHeightDiff + iDrawRegionHeight + rectButton.Height();
  rectDlg.right = rectDlg.left+iDrawRegionWidth;
  SetWindowPos( NULL, rectDlg.left, rectDlg.top, 
                rectDlg.Width(), rectDlg.Height(), 
                SWP_NOZORDER | SWP_SHOWWINDOW );

  long lButtonSpacing = 
    (rectClient.Width() - (rectButton.Width()*NUMBER_BUTTONS)) / (NUMBER_BUTTONS+1);
  long lButtonTop     = (rectClient.bottom - rectButton.Height() - sizeText.cy/2);
  GetDlgItem( IDC_CLOSE_BUTTON )->GetWindowRect(&rectButton);
  GetDlgItem( IDC_CLOSE_BUTTON )->SetWindowPos( NULL, 
                (lButtonSpacing*2)+rectButton.Width(), lButtonTop, 0, 0,
                SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW );

  // perform some calculations to tell us where to begin drawing
  CPoint ptDrawIcon( 
    (iDrawRegionWidth - (iBitMapSizeConst + sizeText.cx)) / 2,
    (iDrawRegionHeight - (sizeText.cy*iNumItems)) / 2 );
  CPoint ptDrawTxt( ptDrawIcon.x + iBitMapSizeConst + BMP_TXT_SPACING, ptDrawIcon.y );
  CRect rectText;

  // begin drawing the bitmaps and the text labels
  //
  CImageList* pImageList = ((CLegView*)m_pParent)->GetImageList();
  DrawLine( dc, pImageList, BMP_EDIT_NOTSEEN, ptDrawIcon, IDS_EDIT_NOTSEEN, ptDrawTxt );
  DrawLine( dc, pImageList, BMP_EDIT_NOMOD, ptDrawIcon, IDS_EDIT_NOMOD, ptDrawTxt );
  // draw the auth sent to CRS bitmaps depending upon the client parameterization
  DrawLine( dc, pImageList, BMP_SENT, ptDrawIcon, IDS_SENT, ptDrawTxt );
  DrawLine( dc, pImageList, BMP_SENT_COMMERCIAL, ptDrawIcon, IDS_SENT_COMMERCIAL, ptDrawTxt );
  DrawLine( dc, pImageList, BMP_SENT_USER_ET_COMMERC, ptDrawIcon, IDS_SENT_USER_ET_COMMERC, ptDrawTxt );
  //réutilisation des flèche vertes pour le moteur VNAX, puisqu'il n'y a plus de règle co HHRD.
  DrawLine( dc, pImageList, BMP_SENT_HRRD, ptDrawIcon, IDS_SENTBY_VNAX, ptDrawTxt );
  DrawLine( dc, pImageList, BMP_SENT_USER_ET_HRRD, ptDrawIcon, IDS_VNAX_ET_USER, ptDrawTxt );
  DrawLine( dc, pImageList, BMP_SENT_COMMERC_ET_HRRD, ptDrawIcon, IDS_VNAX_ET_COMMERC, ptDrawTxt );
  DrawLine( dc, pImageList, BMP_SENT_U_C_H, ptDrawIcon, IDS_VNAX_U_C, ptDrawTxt );
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag() )
  {
	DrawLine( dc, pImageList, BMP_SENT_NOMOS, ptDrawIcon, IDS_SENT_NOMOS, ptDrawTxt );
	DrawLine( dc, pImageList, BMP_SENT_PLUS_NOMOS, ptDrawIcon, IDS_SENT_PLUS_NOMOS, ptDrawTxt );
  }
  else
  { // expérimentation APPIA mise au placard, pas d'info la dessus dans l'aide
	//DrawLine( dc, pImageList, BMP_SENT_NOMOS, ptDrawIcon, IDS_SENTBY_APPIA, ptDrawTxt );
	//DrawLine( dc, pImageList, BMP_SENT_PLUS_NOMOS, ptDrawIcon, IDS_PLUS_APPIA, ptDrawTxt );
  }
  DrawLine( dc, pImageList, BMP_EDIT_AUTHMODNOTSENT, ptDrawIcon, IDS_EDIT_AUTHMODNOTSENT, ptDrawTxt );
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSCXFlag() )
  {
    CTrancheView* pTrancheView = ((CLegView*)m_pParent)->GetTrancheView();
	if (pTrancheView)
	{
      CImageList* pVnasImageList = pTrancheView->GetVnasImageList();
	  DrawLine( dc, pVnasImageList, BMP_CCM1_SEUL, ptDrawIcon, IDS_CCM1_VNAS, ptDrawTxt );
	  DrawLine( dc, pVnasImageList, BMP_CCM1_SEUL+8, ptDrawIcon, IDS_ETANCHE_VNAS, ptDrawTxt );
	  DrawLine( dc, pVnasImageList, BMP_CCM1_SEUL+16, ptDrawIcon, IDS_LOCKCCM1_VNAS, ptDrawTxt );
	}
  }
  if (szClient == SNCF)
  {
	DrawLine( dc, pImageList, BMP_RHEALYS_NOTSENT, ptDrawIcon, IDS_WAITING_ALLEO, ptDrawTxt );
	DrawLine( dc, pImageList, BMP_RHEALYS_SENT, ptDrawIcon, IDS_SENT_ALLEO, ptDrawTxt );
  }

  // Do not call CDialog::OnPaint() for painting messages
}

void CLegView::OnInitialUpdate() 
{
  // CME - Ano 79307
  //m_clrItemBK = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();
  m_clrItemBK = RGB (255, 255, 255);
  YM_ListView::OnInitialUpdate();  // base class method

   CString szProfileKey;
  ((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);
  m_bSelPeak = APP->GetProfileInt(szProfileKey, (LPCSTR)szLoadPeakConst, TRUE);
}

// passe en revue la liste des tranches en recherchant celles pour lequel edit_status|2 == 2
// qui signifie VNAU non envoyé, pour désactiver ce flag et positionner le sent_flag à 1 (flèche noire) 
void CLegView::RhealysSendVnau()
{
   int iCount = GetListCtrl().GetItemCount();
   int eds = 0;
   int sf = 0;
   for (int iIndex = 0; iIndex < iCount; iIndex++)
   {
     YmIcLegsDom* pLegLoop = (YmIcLegsDom*)GetListCtrl().GetItemData(iIndex);
	 eds = pLegLoop->EditStatus();
     if ( (eds & EDIT_STATUS_AUTHMODNOTSENT) )  // triangle noir
     {
	    eds = eds & ~EDIT_STATUS_AUTHMODNOTSENT; // on enlève le triangle noir
        pLegLoop->EditStatus (eds);
		sf = pLegLoop->SentFlag();
		sf = sf  | 16; // on positionne le bit 5 du sent_flag
		pLegLoop->SentFlag(sf);
     }
   }

   YmIcLegsDom* pLeg = (YM_Set<YmIcLegsDom>::FromKey(LEG_KEY));
   eds = pLeg->EditStatus();
   if ( (eds & EDIT_STATUS_AUTHMODNOTSENT) )  // triangle noir
   {
	 eds = eds & ~EDIT_STATUS_AUTHMODNOTSENT; // on enlève le triangle noir
     pLeg->EditStatus (eds);
	 sf = pLeg->SentFlag();
	 sf = sf  | 16; // on positionne le bit 5 du sent_flag
	 pLeg->SentFlag(sf);
   }

   GetListCtrl().Invalidate();
   GetListCtrl().UpdateWindow();
}

void CLegView::SetEditStatus (int tranche_no, CString leg_orig, unsigned long ldate, int valbit)
{
  SetFlagTruc (3, tranche_no, leg_orig, ldate, valbit);
}
void CLegView::SetSentFlag (int tranche_no, CString leg_orig, unsigned long ldate, int valbit)
{
  SetFlagTruc (1, tranche_no, leg_orig, ldate, valbit);
}
void CLegView::SetScxSentFlag (int tranche_no, CString leg_orig, unsigned long ldate, int valbit)
{
  SetFlagTruc (2, tranche_no, leg_orig, ldate, valbit);
}

void CLegView::SetFlagTruc (int truc, int tranche_no, CString leg_orig, unsigned long ldate, int valbit)
{
  YmIcLegsDom* pLeg = NULL;
  int iLegKey = LEG_KEY;
  long s;
  YM_Iterator<YmIcLegsDom>* pIterator = YM_Set<YmIcLegsDom>::FromKey(iLegKey)->CreateIterator();
  for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
  {
     pLeg = pIterator->Current();
	 if ((pLeg->TrancheNo() == tranche_no) && (pLeg->DptDate() == ldate) && !strcmp(pLeg->LegOrig(), (LPCSTR)leg_orig))
	 {
	   if (truc == 2)
	   {
	     s = pLeg->ScxSentFlag();
	     s = s | valbit;
	     pLeg->ScxSentFlag(s);
	   }
	   else if (truc == 1)
	   {
	     s = pLeg->SentFlag();
	     s = s | valbit;
	     pLeg->SentFlag(s);
	   }
	   else if (truc == 3)
	   {
	     s = pLeg->EditStatus();
	     s = s | valbit;
	     pLeg->EditStatus(s);
	   }
	 }
  }
  delete pIterator;
  pLeg = (YM_Set<YmIcLegsDom>::FromKey(iLegKey));
  if ((pLeg->TrancheNo() == tranche_no) && (pLeg->DptDate() == ldate) && !strcmp(pLeg->LegOrig(), (LPCSTR)leg_orig))
  {
	if (truc == 2)
	{
	  s = pLeg->ScxSentFlag();
	  s = s | valbit;
	  pLeg->ScxSentFlag(s);
	}
	else if (truc == 1)
	{
	  s = pLeg->SentFlag();
	  s = s | valbit;
      pLeg->SentFlag(s);
	}
	else if (truc == 3)
	{
	  s = pLeg->EditStatus();
	  s = s | valbit;
	  pLeg->EditStatus(s);
	}
  }
  Invalidate();
}

void CLegView::SaveCurrentIndex()
{
	// OBA Ano 70573 recharge le même tronçon en cas de raffraichissement
	m_index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
}

