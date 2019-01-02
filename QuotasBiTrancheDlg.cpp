// QuotasBiTrancheDlg.cpp : implementation file
//
#include "StdAfx.h"

#include "QuotaEdit.h"

#include "QuotasBiTrancheDlg.h"
#include "VnlEngineView.h"

#include "ResaRailView.h"
#include "TrancheView.h"
#include "ResTracer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CQuotasBiTrancheDlg* CQuotasBiTrancheDlg::m_pInstance = NULL; 

CQuotasBiTrancheDlg* CQuotasBiTrancheDlg::getInstance(YmIcTrancheLisDom* pTranche)
{
	if (m_pInstance == NULL && pTranche != NULL)
	{
		m_pInstance = new CQuotasBiTrancheDlg(pTranche);

		m_pInstance->m_quotas_view_si_cc_a.setListId(0);//SRE 76064 Evol - Add an id to lists. Purpose: find out which list triggered an event.
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_si_cc_a));//SRE 83311 - Refactor using vector to make navigation easier
		m_pInstance->m_quotas_view_si_cc_a._list_ctrl.beingEdited = false;//SRE 83311 - Workaround to catch the enter key in our CEdit (see PreTranslateMssage() function below)
		m_pInstance->m_quotas_view_si_scx_a.setListId(1);
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_si_scx_a));
		m_pInstance->m_quotas_view_si_scx_a._list_ctrl.beingEdited = false;
		m_pInstance->m_quotas_view_sd_cc_a.setListId(2);
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_sd_cc_a));
		m_pInstance->m_quotas_view_sd_cc_a._list_ctrl.beingEdited = false;
		m_pInstance->m_quotas_view_sd_scx_a.setListId(3);
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_sd_scx_a));
		m_pInstance->m_quotas_view_sd_scx_a._list_ctrl.beingEdited = false;
	
		m_pInstance->m_quotas_view_si_cc_b.setListId(4);
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_si_cc_b));
		m_pInstance->m_quotas_view_si_cc_b._list_ctrl.beingEdited = false;
		m_pInstance->m_quotas_view_si_scx_b.setListId(5);
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_si_scx_b));
		m_pInstance->m_quotas_view_si_scx_b._list_ctrl.beingEdited = false;
		m_pInstance->m_quotas_view_sd_cc_b.setListId(6);
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_sd_cc_b));
		m_pInstance->m_quotas_view_sd_cc_b._list_ctrl.beingEdited = false;
		m_pInstance->m_quotas_view_sd_scx_b.setListId(7);
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_sd_scx_b));
		m_pInstance->m_quotas_view_sd_scx_b._list_ctrl.beingEdited = false;
	}
	return m_pInstance;
}

void CQuotasBiTrancheDlg::deleteInstance()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

LONG CQuotasBiTrancheDlg::kbReachedEndList(UINT wParam, LPARAM lParam)//SRE 76064 Evol
{
	//Edit the previous / next list depending on the list which has throwed the event
	//Keyboard Downward case
	//SRE 83311 - Refactored the code to make the navigation easier
	Bascules::DirectionInfo	infos;//SRE 83311 - Due to the increased data to exchange with CEdit,
								  //SRE 83311 - we pass a pointer of class object between this and CEdit.
	int listID;

	infos = *((Bascules::DirectionInfo*)lParam);
	listID = wParam;

	if (infos.move == Bascules::BEGINNING && (listID - 1) >= 0)
	{
		m_pInstance->orderedView.at(listID - 1)->_list_ctrl.SetFocus();
		m_pInstance->orderedView.at(listID - 1)->_list_ctrl.EditPreviousElement(0, 0, 0);
	}
	else if (infos.move == Bascules::END && listID + 1 < (int)m_pInstance->orderedView.size())
	{
		m_pInstance->orderedView.at(listID + 1)->_list_ctrl.SetFocus();
		m_pInstance->orderedView.at(listID + 1)->_list_ctrl.EditNextElement(0, 0, 0);
	}
	else if (infos.move == Bascules::PREVIOUSLIST && listID - 1 >= 0)
	{
		if (m_pInstance->orderedView.at(listID - 1)->_list_ctrl.Type() == Quotas::SCX 
			&& m_pInstance->orderedView.at(listID)->_list_ctrl.Type() == Quotas::CC)
			infos.index -= 1;

		m_pInstance->orderedView.at(listID - 1)->_list_ctrl.SetFocus();
		m_pInstance->orderedView.at(listID - 1)->_list_ctrl.JumpOnPrevList(0, infos.index, infos.move);
	}
	else if (infos.move == Bascules::NEXTLIST && listID + 1 < (int)m_pInstance->orderedView.size())
	{
		if (m_pInstance->orderedView.at(listID + 1)->_list_ctrl.Type() == Quotas::SCX 
			&& m_pInstance->orderedView.at(listID)->_list_ctrl.Type() == Quotas::CC)
			infos.index -= 1;

		m_pInstance->orderedView.at(listID + 1)->_list_ctrl.SetFocus();
		m_pInstance->orderedView.at(listID + 1)->_list_ctrl.JumpOnNextList(0, infos.index, infos.move);
	}

	return 0;
}

CString CQuotasBiTrancheDlg::GetFirstLine(CString& msg)
{
	CString line1;
	int k = msg.FindOneOf ("\x0A\x0D");
	if (k == -1)
	{ // derniere ligne
		line1 = msg;
		msg.Empty();
	}
	else
	{
		line1 = msg.Left (k+1);
		msg = msg.Right(msg.GetLength() -k -1);
		line1.TrimLeft();
		line1.TrimRight();
	}
	return line1;
}
/////////////////////////////////////////////////////////////////////////////
// CQuotasBiTrancheDlg dialog

CQuotasBiTrancheDlg::CQuotasBiTrancheDlg()
: CDialog(CQuotasBiTrancheDlg::IDD, NULL)
{
	//{{AFX_DATA_INIT(CQuotasBiTrancheDlg)
	m_FlagTroncon = FALSE;
	m_FlagSousResa = FALSE;
	//}}AFX_DATA_INIT
	bFlagAppel = FALSE;
	m_szTitle = "";
	m_lRrdIndex = -1;
	m_VnlCount = 0;
	bVnlDeControle = FALSE;
	controlerCC0EgalCC1 = FALSE;
	COLORREF clrBkAlt = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt();
	m_pAltBrush = new CBrush(clrBkAlt);
	m_pStdBrush = new CBrush(RGB(255,255,255));
	CCTADoc *pDoc = ((CCTAApp*)APP)->GetDocument();
	controlerCC0EgalCC1 = pDoc->m_pGlobalSysParms->GetControlerCC0EgalCC1();
	m_nbBktVnlCC_A	= 0;
	m_nbBktVnlCC_B	= 0;
	m_nbBktVnlSC_A	= 0;
	m_nbBktVnlSC_B	= 0;
}

CQuotasBiTrancheDlg::CQuotasBiTrancheDlg(YmIcTrancheLisDom* pTranche)
: CDialog(CQuotasBiTrancheDlg::IDD, NULL)
{
	//{{AFX_DATA_INIT(CQuotasBiTrancheDlg)
	m_FlagTroncon = FALSE;
	m_FlagSousResa = FALSE;
	m_lDateDep    = pTranche->DptDate();
	m_sPeakLeg    = pTranche->PeakLeg();
	m_lRrdIndex   = pTranche->LastJX();
	//}}AFX_DATA_INIT
	CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
	YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
	if (!strcmp(pLoadedTranche->TrainNo(), pTranche->TrainNo()) )
	{ 
		CLegView* pLegView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetLegView();
		if (pLegView)
		{
		  // HRI, 11/08/2015 Anomalie BUGI 70914 (cf SFG Scénario, YI-6129, 
	      // étape 2	A partir de la fenêtre "liste des tranches", l'utilisateur sélectionne 
		  //la tranche / date / tronçon  sur laquelle il souhaite travailler
		  YmIcLegsDom* pLeg;
		  pLeg = pLegView->GetLoadedLeg();
		  if (pLeg == NULL)
		  {
			 pLeg = pLegView->FindFrontier();
		  }
		  if (pLeg)
		  {
			 m_sPeakLeg = pLeg->LegOrig();
		  }
		}
	}

	COLORREF clrBkAlt = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt();
	m_pAltBrush = new CBrush(clrBkAlt);
	m_pStdBrush = new CBrush(RGB(255,255,255));
	m_NumeroTrain = pTranche->TrainNo();
	bFlagAppel = TRUE;
	m_VnlCount = 0;
	CCTADoc *pDoc = ((CCTAApp*)APP)->GetDocument();
	controlerCC0EgalCC1 = pDoc->m_pGlobalSysParms->GetControlerCC0EgalCC1();
	bVnlDeControle = FALSE;
	m_Origine     = pTranche->TrancheOrig();
	m_Destination = pTranche->TrancheDest();
	((CCTAApp*)APP)->GetDocument()->GetEntitySurSousResa (pLoadedTranche->Entity(), "A", m_ovbA, m_unbA, m_ovg, true);
	((CCTAApp*)APP)->GetDocument()->GetEntitySurSousResa (pLoadedTranche->Entity(), "B", m_ovbB, m_unbB, m_ovg, true);
	// Chercher(); 01/12/12 suppression KDISN il faut le déplacer car initdialog non encore fait
	m_nbBktVnlCC_A	= 0;
	m_nbBktVnlCC_B	= 0;
	m_nbBktVnlSC_A	= 0;
	m_nbBktVnlSC_B	= 0;
}

/*
 * SRE 83311 - Override Pretranslate message to prevent
 * CDialog to catch our VK_ENTER key. Otherwise, we are
 * unable to modify the behaviour of ENTER in our Bascule.
 * In fact, we are directly calling EditNextElement function
 * of the appropriate list to edit our next line.
 */
BOOL CQuotasBiTrancheDlg::PreTranslateMessage(MSG* pMsg)
{
	//SRE 83311 - Use a workaround, as the GetFocus() trick doesn't work, event if set by hand...
	if (m_quotas_view_si_cc_a._list_ctrl.beingEdited		||	m_quotas_view_sd_cc_a._list_ctrl.beingEdited
		|| m_quotas_view_si_scx_a._list_ctrl.beingEdited	||	m_quotas_view_sd_scx_a._list_ctrl.beingEdited
		|| m_quotas_view_si_cc_b._list_ctrl.beingEdited		||	m_quotas_view_sd_cc_b._list_ctrl.beingEdited
		|| m_quotas_view_si_scx_b._list_ctrl.beingEdited	||	m_quotas_view_sd_scx_b._list_ctrl.beingEdited)
	
    if (pMsg->message == WM_KEYDOWN)
    {
		if (pMsg->wParam == VK_RETURN)
		{
			std::vector<QuotasAutorisationsListView*>::iterator it;
			for (it = this->orderedView.begin(); it != this->orderedView.end(); it++)
			{
				if ((*it)->_list_ctrl.beingEdited)
					(*it)->_list_ctrl.EditNextElement(1, 0, 0);
			}
			return 1;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

CQuotasBiTrancheDlg::~CQuotasBiTrancheDlg()
{

	if (m_pAltBrush != NULL)
	{
		delete m_pAltBrush;
		m_pAltBrush = NULL;
	}

	if (m_pStdBrush != NULL)
	{
		delete m_pStdBrush;
		m_pStdBrush = NULL;
	}
}


void CQuotasBiTrancheDlg::EditingSIMoiCallbackCC0(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur)
{
	CQuotasBiTrancheDlg* biTrancheDlg = CQuotasBiTrancheDlg::getInstance();
	if (biTrancheDlg != NULL)
	{
		QuotasAutorisationsListCtrl* listCcSi = NULL;
		QuotasAutorisationsListCtrl* listCcSd = NULL;
		//QuotasAutorisationsListCtrl* listScxSi = NULL;// NPI - Ano 70130

		if (espace_physique == 'A')
		{
			listCcSi = &biTrancheDlg->m_quotas_view_si_cc_a._list_ctrl;
			listCcSd = &biTrancheDlg->m_quotas_view_sd_cc_a._list_ctrl;
			//listScxSi = &biTrancheDlg->m_quotas_view_si_scx_a._list_ctrl;// NPI - Ano 70130
		}
		else if (espace_physique == 'B')
		{
			listCcSi = &biTrancheDlg->m_quotas_view_si_cc_b._list_ctrl;
			listCcSd = &biTrancheDlg->m_quotas_view_sd_cc_b._list_ctrl;		
			//listScxSi = &biTrancheDlg->m_quotas_view_si_scx_b._list_ctrl; // NPI - Ano 70130
		}
		else 
		{
			return ;
		}

		// NPI - Ano 70130
		//biTrancheDlg->UpdateListSICc0(listCcSi, listCcSd, listScxSi, indexation, valeur);
		biTrancheDlg->UpdateListSICc0(listCcSi, listCcSd, indexation, valeur);
		///
		biTrancheDlg->UpdateListCc(listCcSi, indexation + 1, valeur);

		// OHA : ano 77962
		biTrancheDlg->m_iDiff = 0;

		biTrancheDlg->OnUpdateDispoCC(listCcSi, true);


		listCcSi->RedrawItems(0, listCcSi->MaxIndexation());
		listCcSd->RedrawItems(0, listCcSd->MaxIndexation()); // NPI - Ano 70130

		/// check 
		biTrancheDlg->GrisageVnauVnas();
		biTrancheDlg->VerifFlagTroncon();
		biTrancheDlg->GrisageTroncon();
	}
}

void CQuotasBiTrancheDlg::EditingSIMoiCallbackCC(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur)
{
	CQuotasBiTrancheDlg* biTrancheDlg = CQuotasBiTrancheDlg::getInstance();
	if (biTrancheDlg != NULL && indexation > 0)
	{
		QuotasAutorisationsListCtrl* list = NULL;

		if (espace_physique == 'A')
			list = &biTrancheDlg->m_quotas_view_si_cc_a._list_ctrl;
		else if (espace_physique == 'B')
			list = &biTrancheDlg->m_quotas_view_si_cc_b._list_ctrl;
		else 
		{
			return ;
		}
		
		// TDR - Anomalie 79304
		biTrancheDlg->bSIFlagModif = TRUE;
		biTrancheDlg->UpdateListCc(list, indexation, valeur);
		// OHA : ano 77962
		biTrancheDlg->m_iDiff = 0;
		biTrancheDlg->OnUpdateDispoCC(list, true);
		list->RedrawItems(0, list->MaxIndexation());

		/// check control 
		biTrancheDlg->GrisageVnauVnas();
		biTrancheDlg->VerifFlagTroncon();
		biTrancheDlg->GrisageTroncon();
	}
}



void CQuotasBiTrancheDlg::EditingSDMoiCallbackCC0(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur)
{
	/// TODO
	CQuotasBiTrancheDlg* biTrancheDlg = CQuotasBiTrancheDlg::getInstance();
	if (biTrancheDlg != NULL)
	{
		QuotasAutorisationsListCtrl* listCcSd = NULL;
		QuotasAutorisationsListCtrl* listCcSi = NULL;
		//QuotasAutorisationsListCtrl* listScxSd = NULL;

		if (espace_physique == 'A')
		{
			listCcSd = &biTrancheDlg->m_quotas_view_sd_cc_a._list_ctrl;
			//listScxSd = &biTrancheDlg->m_quotas_view_sd_scx_a._list_ctrl;
			listCcSi = &biTrancheDlg->m_quotas_view_si_cc_a._list_ctrl;
		}
		else if (espace_physique == 'B')
		{
			listCcSd = &biTrancheDlg->m_quotas_view_sd_cc_b._list_ctrl;		
			//listScxSd = &biTrancheDlg->m_quotas_view_sd_scx_b._list_ctrl;
			listCcSi = &biTrancheDlg->m_quotas_view_si_cc_b._list_ctrl;
		}
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << espace_physique << " must be A or B " << std::endl;
			TRACE(oss.str().c_str());
			return ;
		}

		// NPI - Ano 70130
		//biTrancheDlg->UpdateListSDCc0(listCcSd, listCcSi, listScxSd, indexation, valeur);
		biTrancheDlg->UpdateListSDCc0(listCcSd, listCcSi, indexation, valeur);
		///
		biTrancheDlg->UpdateListCc(listCcSd, indexation + 1, valeur);
		// OHA : ano 77962
		biTrancheDlg->m_iDiff = 0;
		biTrancheDlg->OnUpdateDispoCC(listCcSd, false);

		listCcSd->RedrawItems(0, listCcSd->MaxIndexation());
		listCcSi->RedrawItems(0, listCcSi->MaxIndexation());
		//listScxSd->RedrawItems(0, listScxSd->MaxIndexation());

		/// check 
		biTrancheDlg->GrisageVnauVnas();
		biTrancheDlg->VerifFlagTroncon();
		biTrancheDlg->GrisageTroncon();

	}
}


void CQuotasBiTrancheDlg::EditingSDMoiCallbackCC(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur)
{
	CQuotasBiTrancheDlg* biTrancheDlg = CQuotasBiTrancheDlg::getInstance();
	if (biTrancheDlg != NULL && indexation > 0)
	{
		QuotasAutorisationsListCtrl* list = NULL;

		if (espace_physique == 'A')
			list = &biTrancheDlg->m_quotas_view_sd_cc_a._list_ctrl;
		else if (espace_physique == 'B')
			list = &biTrancheDlg->m_quotas_view_sd_cc_b._list_ctrl;
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << espace_physique << " must be A or B " << std::endl;
			TRACE(oss.str().c_str());
			return ;
		}

		// TDR - Anomalie 79304
		biTrancheDlg->bSDFlagModif = TRUE;
		biTrancheDlg->UpdateListCc(list, indexation, valeur);
		// OHA : ano 77962
		biTrancheDlg->m_iDiff = 0;
		biTrancheDlg->OnUpdateDispoCC(list, false);

		list->RedrawItems(0, list->MaxIndexation());
		
		/// check control 
		biTrancheDlg->GrisageVnauVnas();
		biTrancheDlg->VerifFlagTroncon();
		biTrancheDlg->GrisageTroncon();
	}
}




void CQuotasBiTrancheDlg::EditingSIMoiCallbackSCX(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur)
{
	CQuotasBiTrancheDlg* biTrancheDlg = CQuotasBiTrancheDlg::getInstance();
	if (biTrancheDlg != NULL)
	{
		QuotasAutorisationsListCtrl* list = NULL;

		if (espace_physique == 'A')
		{
			list = &biTrancheDlg->m_quotas_view_si_scx_a._list_ctrl;
			// TDR - Anomalie 79304
			biTrancheDlg->bSCIAFlagModif = TRUE;
		}
		else if (espace_physique == 'B')
		{
			list = &biTrancheDlg->m_quotas_view_si_scx_b._list_ctrl;
			// TDR - Anomalie 79304
			biTrancheDlg->bSCIBFlagModif = TRUE;
		}
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << espace_physique << " must be A or B " << std::endl;
			TRACE(oss.str().c_str());
			return ;
		}

		biTrancheDlg->UpdateListScx(list, indexation, valeur, true);

		list->RedrawItems(0, list->MaxIndexation());

		/// check control 
		biTrancheDlg->GrisageVnauVnas();
		biTrancheDlg->VerifFlagTroncon();
		biTrancheDlg->GrisageTroncon();
	}
}



void CQuotasBiTrancheDlg::EditingSDMoiCallbackSCX(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur)
{
	CQuotasBiTrancheDlg* biTrancheDlg = CQuotasBiTrancheDlg::getInstance();
	if (biTrancheDlg != NULL)
	{
		QuotasAutorisationsListCtrl* list = NULL;

		if (espace_physique == 'A') 
		{
			list = &biTrancheDlg->m_quotas_view_sd_scx_a._list_ctrl;
			// TDR - Anomalie 79304
			biTrancheDlg->bSCDAFlagModif = TRUE;
		}
		else if (espace_physique == 'B')
		{
			list = &biTrancheDlg->m_quotas_view_sd_scx_b._list_ctrl;
			// TDR - Anomalie 79304
			biTrancheDlg->bSCDBFlagModif = TRUE;
		}
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << espace_physique << " must be A or B " << std::endl;
			TRACE(oss.str().c_str());
			return ;
		}

		biTrancheDlg->UpdateListScx(list, indexation, valeur, false);

		list->RedrawItems(0, list->MaxIndexation());

		/// check control 
		biTrancheDlg->GrisageVnauVnas();
		biTrancheDlg->VerifFlagTroncon();
		biTrancheDlg->GrisageTroncon();
	}
}


void CQuotasBiTrancheDlg::EditingSIMoiCallbackSCX0(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur)
{
	CQuotasBiTrancheDlg* biTrancheDlg = CQuotasBiTrancheDlg::getInstance();
	if (biTrancheDlg != NULL)
	{
		QuotasAutorisationsListCtrl* listSi = NULL;
		QuotasAutorisationsListCtrl* listSiScx = NULL;

		if (espace_physique == 'A')
		{
			listSi = &biTrancheDlg->m_quotas_view_si_cc_a._list_ctrl;
			listSiScx = &biTrancheDlg->m_quotas_view_si_scx_a._list_ctrl;
		}
		else if (espace_physique == 'B')
		{
			listSi = &biTrancheDlg->m_quotas_view_si_cc_b._list_ctrl;
			listSiScx = &biTrancheDlg->m_quotas_view_si_scx_b._list_ctrl;
		}
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << espace_physique << " must be A or B " << std::endl;
			TRACE(oss.str().c_str());
			return ;
		}

		biTrancheDlg->UpdateListSISc0(listSiScx, listSi, indexation, valeur);
		biTrancheDlg->UpdateListScx(listSiScx, indexation + 1, valeur, true);

		listSiScx->RedrawItems(0, listSiScx->MaxIndexation());
		listSi->RedrawItems(0, listSi->MaxIndexation());

		/// check control 
		biTrancheDlg->GrisageVnauVnas();
		biTrancheDlg->VerifFlagTroncon();
		biTrancheDlg->GrisageTroncon();
	}
}




void CQuotasBiTrancheDlg::EditingSDMoiCallbackSCX0(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur)
{
	// HRI, 13/08/2015 : il n'y a pas de nest_level=0 en SC
	// A mon avis, on ne rentre jamais ici car le SCG est géré depuis des EditBox
	//AfxMessageBox("Entrée dans CQuotasBiTrancheDlg::EditingSDMoiCallbackSCX0", MB_OK);
	CQuotasBiTrancheDlg* biTrancheDlg = CQuotasBiTrancheDlg::getInstance();
	if (biTrancheDlg != NULL)
	{
		QuotasAutorisationsListCtrl* listSdScx = NULL;
		QuotasAutorisationsListCtrl* listSd = NULL;


		if (espace_physique == 'A')
		{
			listSdScx = &biTrancheDlg->m_quotas_view_sd_scx_a._list_ctrl;
			listSd = &biTrancheDlg->m_quotas_view_sd_cc_a._list_ctrl;

		}
		else if (espace_physique == 'B')
		{
			listSdScx = &biTrancheDlg->m_quotas_view_sd_scx_b._list_ctrl;
			listSd = &biTrancheDlg->m_quotas_view_sd_cc_b._list_ctrl;
		}
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << espace_physique << " must be A or B " << std::endl;
			TRACE(oss.str().c_str());
			return ;
		}

		biTrancheDlg->UpdateListSDSc0(listSdScx, listSd, indexation, valeur);
		biTrancheDlg->UpdateListScx(listSdScx, indexation + 1, valeur, false);

		listSdScx->RedrawItems(0, listSdScx->MaxIndexation());

		/// check control 
		biTrancheDlg->GrisageVnauVnas();
		biTrancheDlg->VerifFlagTroncon();
		biTrancheDlg->GrisageTroncon();
	}
}


void CQuotasBiTrancheDlg::UpdateListCc(QuotasAutorisationsListCtrl* list_ctrl, long indexation, long& valeur)
{
	

	if (list_ctrl != NULL && indexation > 0)
	{
		for(int i = indexation ; i < list_ctrl->MaxIndexation();  i++)
		{
			QuotasValue* valueAct = list_ctrl->GetValue(Quotas::ACT, i);
			QuotasValue* valueMoi = list_ctrl->GetValue(Quotas::MOI, i);
			QuotasValue* valueMoiAvant = list_ctrl->GetValue(Quotas::MOI, i -1);
			
			// Ne controler CC0 == CC1 que si l'indicateur de contrôle est à true
			if(controlerCC0EgalCC1)
			{	
				if (i == 1)
				{	// comparer CC0 et CC1
					if (valueMoi->_value != valueMoiAvant->_value)
					{
						AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_EQUAL_CC0_CC1), 
                              MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION );
						// remettre CC1 = CC0
						valueMoi->_value = valueMoiAvant->_value;
					}							
				}
			}
			
			if (valueMoi != NULL && valueAct != NULL && valueMoiAvant != NULL)
			{
				if (valueMoi->_value > valueMoiAvant->_value)
				{
					valueMoi->_value = valueMoiAvant->_value;
				}

				if ((m_iDiff == 0) && (!valueMoi->_bBucket))
				{
					m_iDiff = valueAct->_value - valueMoi->_value;
				}

				if (m_bOvbAdjustFlag)
				{					
					if(i == 1) 
					{
						if(!controlerCC0EgalCC1)
						{
							if (m_iDiff != 0)
							{
								valueMoi->_value = ((valueAct->_value - m_iDiff) < 0) ? 0 : (valueAct->_value - m_iDiff);
							}
							else
							{
								valueMoi->_value = valueAct->_value;
							}
						}	

					}
					else
					{
						if (m_iDiff != 0)
						{
							valueMoi->_value = ((valueAct->_value - m_iDiff) < 0) ? 0 : (valueAct->_value - m_iDiff);
						}
						else
						{
							valueMoi->_value = valueAct->_value;
						}
					}

				}

				if (valueMoi->_value != valueAct->_value)
				{
					// TDR - Anomalie 79304 Pas sa place ici cette fonction peut être appelée par les tranches DB également.
					//bSIFlagModif = TRUE;
					bFlagModifManuelle = TRUE;
				}

				valueMoi->_bBucket = FALSE;
				// ano 77962
				if ((indexation + 1) < list_ctrl->MaxIndexation())
				{
					QuotasValue* valueMoiApres = list_ctrl->GetValue(Quotas::MOI, (i + 1));
					// ano 77962
					valueMoiApres->_bBucket = TRUE;
				}
			}
		}
	}
}

// NPI - Ano 70130
// Réécriture de la fonction
/*void CQuotasBiTrancheDlg::UpdateListSICc0(QuotasAutorisationsListCtrl* list_ctrl_cc_si, QuotasAutorisationsListCtrl* list_ctrl_cc_sd, QuotasAutorisationsListCtrl* list_ctrl_scx, long indexation, long& valeur)
{
	if (list_ctrl_cc_si != NULL && list_ctrl_scx != NULL && list_ctrl_cc_sd != NULL && indexation == 0)
	{
		QuotasValue* valueMoi = list_ctrl_cc_si->GetValue(Quotas::MOI, 0);
		QuotasValue* valueAct = list_ctrl_cc_si->GetValue(Quotas::ACT, 0);
		QuotasValue* valueMoiAvant = list_ctrl_scx->GetValue(Quotas::MOI, 0);
		QuotasValue* valueSdMoi = list_ctrl_cc_sd->GetValue(Quotas::MOI, 0);

		int iRecap = 0;

		if (valueMoi != NULL && valueAct != NULL && valueMoiAvant != NULL && valueSdMoi != NULL)
		{
			long lValueMoi = valueMoi->_value;
			long lValueAct = valueAct->_value;
			long lValueMoiAvant = valueMoiAvant->_value;

			if (lValueMoi > lValueMoiAvant)
			{
				lValueMoi = lValueMoiAvant;
				valueMoi->_value = lValueMoi;
			}

			m_iDiff = lValueAct - lValueMoi;
			CString strSiDiff;
			strSiDiff.Format("%d", - m_iDiff);

			/// set diff if needed 

			/// 
			iRecap = lValueMoi;

			long value = valueSdMoi->_value;
			iRecap += value;
			
			CString sRecap;
			sRecap.Format("%d", iRecap);

			if (list_ctrl_cc_si->EspacePhysique() == 'A')
			{
				m_iCapaMoi_A.SetWindowText(sRecap);
				m_iCapaMoi_B.GetWindowText(sRecap);
				int capaMoiB = atoi(sRecap);
				iRecap += capaMoiB;

				/// set diff 
				m_SI_ADIFF.SetWindowText(strSiDiff);

				if (m_iDiff == 0) m_SI_ADIFF.SetTextColor(RGB(0,0,0));
				else if (m_iDiff > 0) m_SI_ADIFF.SetTextColor(RGB(255,0,0));
				else if (m_iDiff < 0) m_SI_ADIFF.SetTextColor(COLOR_OVB_MTRANCHE);
			}
			else if (list_ctrl_cc_si->EspacePhysique() == 'B')
			{
				m_iCapaMoi_B.SetWindowText(sRecap);
				m_iCapaMoi_A.GetWindowText(sRecap);
				int capaMoiA = atoi(sRecap);
				iRecap += capaMoiA;

				/// set diff 
				m_SI_BDIFF.SetWindowText(strSiDiff);
				if (m_iDiff == 0) m_SI_BDIFF.SetTextColor(RGB(0,0,0));
				else if (m_iDiff > 0) m_SI_BDIFF.SetTextColor(RGB(255,0,0));
				else if (m_iDiff < 0) m_SI_BDIFF.SetTextColor(COLOR_OVB_MTRANCHE);
			}

			/// 
			sRecap.Format("%d", iRecap);
			m_iCapaMoi_TOT.SetWindowText(sRecap);				

			if (m_bOvbAdjustFlag)
			{
				long val = lValueAct - m_iDiff;
				/// TODO 
			}

			if (lValueMoi != lValueAct)
				bSIFlagModif = TRUE;
		}
	}
}*/

void CQuotasBiTrancheDlg::UpdateListSICc0(QuotasAutorisationsListCtrl* list_ctrl_cc_si, QuotasAutorisationsListCtrl* list_ctrl_cc_sd, long indexation, long& valeur)
{
	if (list_ctrl_cc_si != NULL && list_ctrl_cc_sd != NULL && indexation == 0)
	{
		QuotasValue* valueAct = list_ctrl_cc_si->GetValue(Quotas::ACT, 0);
		QuotasValue* valueMoi = list_ctrl_cc_si->GetValue(Quotas::MOI, 0);
		QuotasValue* valueMoiApres = list_ctrl_cc_si->GetValue(Quotas::MOI, 1);
		QuotasValue* valueSdMoi = list_ctrl_cc_sd->GetValue(Quotas::MOI, 0);

		// ANO 96098 - NBN
		CString sMOImin;
		if (list_ctrl_cc_si->EspacePhysique() == 'A'){
			m_IDC_LABEL_SI_A_TOTAL_MOI.GetWindowText(sMOImin);
		}else{
			m_IDC_LABEL_SI_B_TOTAL_MOI.GetWindowText(sMOImin);
		}

		int iRecap = 0;

		if (valueMoi != NULL && valueAct != NULL && valueSdMoi != NULL)
		{
			if (valueMoi->_value < atoi(sMOImin))
			{
				
				CString newScg;
				newScg.Format("%ld", valueMoi->_value);

				if (list_ctrl_cc_si->EspacePhysique() == 'A'){
					m_IDC_LABEL_SI_A_TOTAL_MOI.SetWindowText(newScg);
					OnMajSCIAG();
				}else{
					m_IDC_LABEL_SI_B_TOTAL_MOI.SetWindowText(newScg);
					OnMajSCIBG();
				}
			}

			m_iDiff = valueAct->_value - valueMoi->_value;
			CString strSiDiff;
			strSiDiff.Format("%d", (-m_iDiff));

			/// set diff if needed  
			iRecap = valueMoi->_value + valueSdMoi->_value;
			
			CString sRecap;
			sRecap.Format("%d", iRecap);

			if (list_ctrl_cc_si->EspacePhysique() == 'A')
			{
				m_iCapaMoi_A.SetWindowText(sRecap);
				
				m_iCapaMoi_B.GetWindowText(sRecap);
				iRecap += atoi(sRecap);

				/// set diff 
				m_IDC_LABEL_SI_A_DIFF.SetWindowText(strSiDiff);

				if (m_iDiff == 0)
				{
					m_IDC_LABEL_SI_A_DIFF.SetTextColor(RGB(0,0,0));
				}
				else if (m_iDiff > 0)
				{
					m_IDC_LABEL_SI_A_DIFF.SetTextColor(RGB(255,0,0));
				}
				else if (m_iDiff < 0)
				{
					m_IDC_LABEL_SI_A_DIFF.SetTextColor(COLOR_OVB_MTRANCHE);
				}
			}
			else if (list_ctrl_cc_si->EspacePhysique() == 'B')
			{
				m_iCapaMoi_B.SetWindowText(sRecap);
				
				m_iCapaMoi_A.GetWindowText(sRecap);
				iRecap += atoi(sRecap);

				/// set diff 
				m_IDC_LABEL_SI_B_DIFF.SetWindowText(strSiDiff);
				if (m_iDiff == 0)
				{
					m_IDC_LABEL_SI_B_DIFF.SetTextColor(RGB(0,0,0));
				}
				else if (m_iDiff > 0)
				{
					m_IDC_LABEL_SI_B_DIFF.SetTextColor(RGB(255,0,0));
				}
				else if (m_iDiff < 0)
				{
					m_IDC_LABEL_SI_B_DIFF.SetTextColor(COLOR_OVB_MTRANCHE);
				}
			}

			sRecap.Format("%d", iRecap);
			m_iCapaMoi_TOT.SetWindowText(sRecap);

			if (valueMoi->_value != valueAct->_value)
			{
				bSIFlagModif = TRUE;
				bFlagModifManuelle = TRUE;
			}
			if ((indexation + 1) < list_ctrl_cc_si->MaxIndexation())
			{
				QuotasValue* valueMoiApres = list_ctrl_cc_si->GetValue(Quotas::MOI, (indexation + 1));
				valueMoiApres->_bBucket = TRUE;
			}
		}
	}
}
///

// NPI - Ano 70130
//Réécriture de la fonction
/*void CQuotasBiTrancheDlg::UpdateListSDCc0(QuotasAutorisationsListCtrl* list_ctrl_cc_sd, QuotasAutorisationsListCtrl* list_ctrl_cc_si, QuotasAutorisationsListCtrl* list_ctrl_scx, long indexation, long& valeur)
{
	if (list_ctrl_cc_si != NULL && list_ctrl_scx != NULL && list_ctrl_cc_sd != NULL)
	{
			QuotasValue* valueMoi = list_ctrl_cc_sd->GetValue(Quotas::MOI, 0);
			QuotasValue* valueAct = list_ctrl_cc_sd->GetValue(Quotas::ACT, 0);
			QuotasValue* valueMoiSi = list_ctrl_cc_si->GetValue(Quotas::MOI, 0);
			QuotasValue* valueMoiAvant = list_ctrl_scx->GetValue(Quotas::MOI, 0);

			int iRecap = 0;

			if (valueMoi != NULL && valueAct != NULL && valueMoiAvant != NULL && valueMoiSi != NULL)
			{
				long lValueMoi = valueMoi->_value;
				long lValueAct = valueAct->_value;
				long lValueMoiAvant = valueMoiAvant->_value;

				if (lValueMoi > lValueMoiAvant)
				{
					lValueMoi = lValueMoiAvant;
					valueMoi->_value = lValueMoi;
				}

				m_iDiff = lValueAct - lValueMoi;

				/// set bdiff 
				CString strDiff;
				strDiff.Format("%d", - m_iDiff);

				/// 
				iRecap = lValueMoi;

				/// 
				long value = valueMoiSi->_value;
				iRecap += value;
				
				CString sRecap;
				sRecap.Format("%d", iRecap);

				if (list_ctrl_cc_sd->EspacePhysique() == 'A')
				{
					m_iCapaMoi_A.SetWindowText(sRecap);
					m_iCapaMoi_B.GetWindowText(sRecap);
					int capaMoiB = atoi(sRecap);
					iRecap += capaMoiB;

					/// set diff 
					m_SD_ADIFF.SetWindowText(strDiff);		
					if (m_iDiff == 0) m_SD_ADIFF.SetTextColor(RGB(0,0,0));
					else if (m_iDiff > 0) m_SD_ADIFF.SetTextColor(RGB(255,0,0));
					else if (m_iDiff < 0) m_SD_ADIFF.SetTextColor(COLOR_OVB_MTRANCHE);
				}
				else if (list_ctrl_cc_sd->EspacePhysique() == 'B')
				{
					m_iCapaMoi_B.SetWindowText(sRecap);
					m_iCapaMoi_A.GetWindowText(sRecap);
					int capaMoiA = atoi(sRecap);
					iRecap += capaMoiA;

					/// set diff 
					m_SD_BDIFF.SetWindowText(strDiff);
					if (m_iDiff == 0) m_SD_BDIFF.SetTextColor(RGB(0,0,0));
					else if (m_iDiff > 0) m_SD_BDIFF.SetTextColor(RGB(255,0,0));
					else if (m_iDiff < 0) m_SD_BDIFF.SetTextColor(COLOR_OVB_MTRANCHE);
				}

				/// 
				sRecap.Format("%d", iRecap);
				m_iCapaMoi_TOT.SetWindowText(sRecap);				

				if (m_bOvbAdjustFlag)
				{
					long val = lValueAct - m_iDiff;
					/// TODO 
				}

				if (lValueMoi != lValueAct)
					bSDFlagModif = TRUE;
			}
	}
}*/

void CQuotasBiTrancheDlg::UpdateListSDCc0(QuotasAutorisationsListCtrl* list_ctrl_cc_sd, QuotasAutorisationsListCtrl* list_ctrl_cc_si, long indexation, long& valeur)
{
	if (list_ctrl_cc_si != NULL && list_ctrl_cc_sd != NULL && indexation == 0)
	{
		QuotasValue* valueAct = list_ctrl_cc_sd->GetValue(Quotas::ACT, 0);
		QuotasValue* valueMoi = list_ctrl_cc_sd->GetValue(Quotas::MOI, 0);
		QuotasValue* valueSiMoi = list_ctrl_cc_si->GetValue(Quotas::MOI, 0);

		CString sMOImin;
		// ANO 96098 - NBN
		if (list_ctrl_cc_sd->EspacePhysique() == 'A'){
			m_IDC_LABEL_SD_A_TOTAL_MOI.GetWindowText(sMOImin);
		}else{
			m_IDC_LABEL_SD_B_TOTAL_MOI.GetWindowText(sMOImin);
		}

		int iRecap = 0;

		if (valueMoi != NULL && valueAct != NULL && valueSiMoi != NULL)
		{
			if (valueMoi->_value < atoi(sMOImin))
			{
				
				CString newScg;
				newScg.Format("%ld", valueMoi->_value);

				if (list_ctrl_cc_sd->EspacePhysique() == 'A'){
					m_IDC_LABEL_SD_A_TOTAL_MOI.SetWindowText(newScg);
					OnMajSCDAG();
				}else{
					m_IDC_LABEL_SD_B_TOTAL_MOI.SetWindowText(newScg);
					OnMajSCDBG();
				}
			}

			m_iDiff = valueAct->_value - valueMoi->_value;
			CString strSiDiff;
			strSiDiff.Format("%d", (-m_iDiff));

			/// set diff if needed  
			iRecap = valueMoi->_value + valueSiMoi->_value;
			
			CString sRecap;
			sRecap.Format("%d", iRecap);

			if (list_ctrl_cc_sd->EspacePhysique() == 'A')
			{
				m_iCapaMoi_A.SetWindowText(sRecap);
				
				m_iCapaMoi_B.GetWindowText(sRecap);
				iRecap += atoi(sRecap);

				/// set diff 
				m_IDC_LABEL_SD_A_DIFF.SetWindowText(strSiDiff);

				if (m_iDiff == 0)
				{
					m_IDC_LABEL_SD_A_DIFF.SetTextColor(RGB(0,0,0));
				}
				else if (m_iDiff > 0)
				{
					m_IDC_LABEL_SD_A_DIFF.SetTextColor(RGB(255,0,0));
				}
				else if (m_iDiff < 0)
				{
					m_IDC_LABEL_SD_A_DIFF.SetTextColor(COLOR_OVB_MTRANCHE);
				}
			}
			else if (list_ctrl_cc_sd->EspacePhysique() == 'B')
			{
				m_iCapaMoi_B.SetWindowText(sRecap);
				
				m_iCapaMoi_A.GetWindowText(sRecap);
				iRecap += atoi(sRecap);

				/// set diff 
				m_IDC_LABEL_SD_B_DIFF.SetWindowText(strSiDiff);
				if (m_iDiff == 0)
				{
					m_IDC_LABEL_SD_B_DIFF.SetTextColor(RGB(0,0,0));
				}
				else if (m_iDiff > 0)
				{
					m_IDC_LABEL_SD_B_DIFF.SetTextColor(RGB(255,0,0));
				}
				else if (m_iDiff < 0)
				{
					m_IDC_LABEL_SD_B_DIFF.SetTextColor(COLOR_OVB_MTRANCHE);
				}
			}

			sRecap.Format("%d", iRecap);
			m_iCapaMoi_TOT.SetWindowText(sRecap);

			if (valueMoi->_value != valueAct->_value)
			{
				bSDFlagModif = TRUE;
				bFlagModifManuelle = TRUE;
			}
			if ((indexation + 1) < list_ctrl_cc_sd->MaxIndexation())
			{
				QuotasValue* valueMoiApres = list_ctrl_cc_sd->GetValue(Quotas::MOI, (indexation + 1));
				valueMoiApres->_bBucket = TRUE;
			}
		}
	}
}
///

// HRI, 14/08/2015 : ajout provenance SI ou SD
void CQuotasBiTrancheDlg::UpdateListScx(QuotasAutorisationsListCtrl* list_ctrl, long indexation, long& valeur, bool bSi)
{
	if (list_ctrl != NULL && indexation > 0 && indexation < list_ctrl->MaxIndexation())
	{
		QuotasValue* valueMoi = list_ctrl->GetValue(Quotas::MOI , indexation);
		QuotasValue* valueEtanche = list_ctrl->GetValue(Quotas::ETANCHE, indexation);
		QuotasValue* valueCC0Moi = NULL;
		QuotasValue* valueDispo0 = NULL;
		//	

		long lValueMoi = valueMoi->_value;
		long lValueEtanche = valueEtanche->_value;
		long lValueCCMoi0 = 0l;
		long lValueDispo0 = 0l;

		/// get sous contingent general 
		QuotasValue* valueScgMoi = list_ctrl->GetValue(Quotas::MOI, 0);
		/// autorisation CC-1
		long lValueScgMoi = valueScgMoi->_value;
		CString sg;
		CString strTotal;
		if (list_ctrl->EspacePhysique() == 'A')
		{
			if (bSi)
			{
				m_IDC_LABEL_SI_A_TOTAL_MOI.GetWindowText(sg);
				m_SI_A_SCX_RES_TOT.GetWindowText(strTotal);
				valueCC0Moi = m_quotas_view_si_cc_a._list_ctrl.GetValue(Quotas::MOI , 0);
				valueDispo0 = m_quotas_view_si_cc_a._list_ctrl.GetValue(Quotas::DISPO, 0);
			}
			else
			{
				m_IDC_LABEL_SD_A_TOTAL_MOI.GetWindowText(sg);
				m_SD_A_SCX_RES_TOT.GetWindowText(strTotal);
				valueCC0Moi = m_quotas_view_sd_cc_a._list_ctrl.GetValue(Quotas::MOI , 0);
				valueDispo0 = m_quotas_view_sd_cc_a._list_ctrl.GetValue(Quotas::DISPO, 0);
			}
		}
		else if (list_ctrl->EspacePhysique() == 'B')
		{
			if (bSi)
			{
				m_IDC_LABEL_SI_B_TOTAL_MOI.GetWindowText(sg);
				m_SI_B_SCX_RES_TOT.GetWindowText(strTotal);
				valueCC0Moi = m_quotas_view_si_cc_b._list_ctrl.GetValue(Quotas::MOI , 0);
				valueDispo0 = m_quotas_view_si_cc_b._list_ctrl.GetValue(Quotas::DISPO, 0);
			}
			else
			{
				m_IDC_LABEL_SD_B_TOTAL_MOI.GetWindowText(sg);
				m_SD_B_SCX_RES_TOT.GetWindowText(strTotal);
				valueCC0Moi = m_quotas_view_sd_cc_b._list_ctrl.GetValue(Quotas::MOI , 0);
				valueDispo0 = m_quotas_view_sd_cc_b._list_ctrl.GetValue(Quotas::DISPO, 0);
			}
		}
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << list_ctrl->EspacePhysique() << " but must be A or B " << std::endl;
			TRACE(oss.str().c_str());	
			return ;
		}
		lValueCCMoi0 = valueCC0Moi->_value;
		lValueScgMoi = atoi(sg);
		int totalResaCmpt = atoi(strTotal);

		// Règle de gestion
		if ( lValueMoi > lValueScgMoi)
		{
			lValueMoi = lValueScgMoi;
			valueMoi->_value = lValueMoi;
		}

		int sumVentePoreuse = 0;
		int sumDispoEtanche = 0;
		int sumAuthEtanche = 0;
		// Ajout HRI, 14/08
		cumulEtanche(list_ctrl, sumAuthEtanche, sumDispoEtanche);
		cumulVente(list_ctrl, sumVentePoreuse);

		if ((sumAuthEtanche > lValueScgMoi ) /// la somme des autorisations en SCI etanche doit etre inferieur au sous contingent général
			||  ((!lValueEtanche) && (lValueMoi > lValueScgMoi - sumAuthEtanche))) /// une sci non etanche doit etre inférieur à la SCG - la somme des auth étanches 
		{
			if (lValueEtanche)
			{
				lValueMoi -= sumAuthEtanche - lValueScgMoi;
			}
			else
			{
				lValueMoi = lValueScgMoi - sumAuthEtanche;
			}
			if (lValueMoi < 0)
				lValueMoi = 0;
			valueMoi->_value = lValueMoi;
		}
		// Recalcul au cas où modifié
		sumAuthEtanche = 0;
		sumDispoEtanche = 0;
		cumulEtanche(list_ctrl, sumAuthEtanche, sumDispoEtanche);

	    // vérification par rapport aux autres autorisations SCI non étanche
	    // les étanches ont été contrôlées par le test (sumAuthEtanche > isg)
		for (int i = 1; i < list_ctrl->MaxIndexation(); ++i)
		{
			QuotasValue* valueMoi2 = list_ctrl->GetValue(Quotas::MOI , i);
			QuotasValue* valueEtanche2 = list_ctrl->GetValue(Quotas::ETANCHE, i);
			if (!valueEtanche2->_value)
			{
				long iAuth = valueMoi2->_value;
				if (iAuth > lValueScgMoi - sumAuthEtanche)
				{
					iAuth = lValueScgMoi - sumAuthEtanche;
					if (iAuth < 0)
						iAuth = 0;
					valueMoi2->_value = iAuth;
				}
			}
		}

		// Calcul dispo
		long lDispoSCG = lValueScgMoi - sumAuthEtanche - sumVentePoreuse;
		if (lValueEtanche)
		{
			lValueDispo0 = lValueCCMoi0 - totalResaCmpt - sumDispoEtanche;
			if (lValueDispo0 < 0)
				lValueDispo0 = 0;
			valueDispo0->_value = lValueDispo0;
		}
		else
		{
			lValueDispo0 = valueDispo0->_value;
		}
		if (lDispoSCG > lValueDispo0)		// la dispo SCG ne peut être supérieure à celle de la CC0
		{
			lDispoSCG = lValueDispo0;
		}

		// On est bon, recalcul global des dispos de SC
		for (int i = 1; i < list_ctrl->MaxIndexation(); ++i)
		{
			QuotasValue* valueMoi2 = list_ctrl->GetValue(Quotas::MOI , i);
			QuotasValue* valueEtanche2 = list_ctrl->GetValue(Quotas::ETANCHE, i);
			QuotasValue* valueResa2 = list_ctrl->GetValue(Quotas::VENTES, i);
			QuotasValue* valueDispo2 = list_ctrl->GetValue(Quotas::DISPO, i);
			long lDisp2 = valueMoi2->_value - valueResa2->_value;
			if (!valueEtanche2->_value)
			{
				if (lDisp2 > lDispoSCG)
				{
					lDisp2 = lDispoSCG;
				}
			}
			valueDispo2->_value = lDisp2;
		}

		if (list_ctrl->EspacePhysique() == 'A')
		{
			// TDR - Anomalie 79304 Peut être également appelé par les tranches DB
			//bSCIAFlagModif = TRUE;
			bFlagModifManuelle = TRUE;
		}
		else if (list_ctrl->EspacePhysique() == 'B')
		{
			// TDR - Anomalie 79304 Peut être également appelé par les tranches DB
			//bSCIBFlagModif = TRUE;
			bFlagModifManuelle = TRUE;
		}
	}
	
	GrisageVnauVnas();
}

void CQuotasBiTrancheDlg::UpdateListSISc0(QuotasAutorisationsListCtrl* list_ctrl_si_scx, QuotasAutorisationsListCtrl* list_ctrl_si,  long indexation, long& valeur)
{
	if (list_ctrl_si_scx != NULL && list_ctrl_si != NULL && indexation == 0)
	{
		/// TODO 
		std::cout << " "  << std::endl;
		long oldValue = 0;
		
		QuotasValue* valueMoiSi0 = list_ctrl_si->GetValue(Quotas::MOI, 0);
		QuotasValue* valueMoiScg = list_ctrl_si_scx->GetValue(Quotas::MOI, 1);

		if (valueMoiSi0 != NULL && valueMoiScg != NULL)
		{
			long value = valueMoiSi0->_value;
			oldValue = valueMoiScg->_value;

			if (oldValue > value)
			{
				std::cout << " " << std::endl;

				return;
			}
		}

		int sumAuthEtanche = 0;
		int sumDispEtanche = 0;

		if (list_ctrl_si_scx->EspacePhysique() == 'A')
		{
			CumulEtancheSIA(sumAuthEtanche, sumDispEtanche);
		}
		else if (list_ctrl_si_scx->EspacePhysique() == 'B')
		{
			CumulEtancheSIB(sumAuthEtanche, sumDispEtanche);
		}
		else 
		{
			return;
		}

		bool bOk = true;
		if (oldValue < sumAuthEtanche)
		{
			bOk = false;
		}
		/// TODO ControlMaxAuthSci
		int diff = oldValue - sumAuthEtanche;

		for(int i = 1; i < list_ctrl_si_scx->MaxIndexation() ; i++)
		{
			QuotasValue* valueEtanche = list_ctrl_si_scx->GetValue(Quotas::ETANCHE, i);
			if (valueEtanche != NULL && valueEtanche->_value != 1)
			{
				bOk = ctrlMaxAuthSci(list_ctrl_si_scx, i, diff);
				if (! bOk)
					break;
			}
		}

		if (! bOk)
		{
			/// TODO set previous value 
			return ;
		}
		else  // HRI, 13/08/2015 : MàJ flags
		{
			if (list_ctrl_si_scx->EspacePhysique() == 'A')
			{
				bSCIAFlagModif = TRUE;
				bFlagModifManuelle = TRUE;
			}
			else if (list_ctrl_si_scx->EspacePhysique() == 'B')
			{
				bSCIBFlagModif = TRUE;
				bFlagModifManuelle = TRUE;
			}
		}
	}

}

void CQuotasBiTrancheDlg::UpdateListSDSc0(QuotasAutorisationsListCtrl* list_ctrl_sd_scx, QuotasAutorisationsListCtrl* list_ctrl_sd, long indexation, long& valeur)
{
	if (list_ctrl_sd_scx != NULL && list_ctrl_sd != NULL && indexation == 0)
	{
		/// TODO 
		std::cout << " "  << std::endl;
		
		QuotasValue* valueMoiSi0 = list_ctrl_sd->GetValue(Quotas::MOI, 0);
		QuotasValue* valueMoiScg = list_ctrl_sd_scx->GetValue(Quotas::MOI, 1);

		if (valueMoiSi0 != NULL && valueMoiScg != NULL)
		{
			long value = valueMoiSi0->_value;
			long oldValue = valueMoiScg->_value;

			if (oldValue > value)
			{
				std::cout << " " << std::endl;

				return;
			}
		}

		int sumAuthEtanche = 0;
		int sumDispEtanche = 0;

		if (list_ctrl_sd_scx->EspacePhysique() == 'A')
		{
			CumulEtancheSDA(sumAuthEtanche, sumDispEtanche);
		}
		else if (list_ctrl_sd_scx->EspacePhysique() == 'B')
		{
			CumulEtancheSDB(sumAuthEtanche, sumDispEtanche);
		}
		else 
		{
			return;
		}

		/// TODO ControlMaxAuthSci
		/// HRI, 13/08/2015 : y aurait-il quelque chose à faire sur ce TODO

		// HRI, 13/08/2015 : màj flag
		if (list_ctrl_sd_scx->EspacePhysique() == 'A')
		{
			bSCDAFlagModif = TRUE;
			bFlagModifManuelle = TRUE;
		}
		else if (list_ctrl_sd_scx->EspacePhysique() == 'B')
		{
			bSCDBFlagModif = TRUE;
			bFlagModifManuelle = TRUE;
		}
	}

}
	



void CQuotasBiTrancheDlg::InitCtrlQuotasList()
{
	/// Cmpt A 

	/// test 
	int max = App().GetDocument()->m_pGlobalSysParms->GetMaxCmptBkt('A');
	// N'afficher que les buckets donnés par le VNL (HRI, 11/08/2015)
	if (m_nbBktVnlCC_A > 0)
	{
		max = m_nbBktVnlCC_A;
	}


	/// si A (CC) SI (international)
	m_quotas_view_si_cc_a.Build (Quotas::CC, 'A', max);
	m_quotas_view_si_cc_a.ShowWindow(SW_HIDE);

	/// si A (CC) SD (domestique)
	m_quotas_view_sd_cc_a.Build (Quotas::CC, 'A', max);
	m_quotas_view_sd_cc_a.ShowWindow(SW_HIDE);

	for(int i = 0; i < max ; i++)
	{
		m_quotas_view_sd_cc_a._list_ctrl.SetEditable(Quotas::MOI, i , false);
		m_quotas_view_sd_cc_a._list_ctrl.SetGreyBackground(Quotas::MOI, i, true);
		m_quotas_view_sd_cc_a._list_ctrl.SetTextColor(Quotas::MOI, i, RGB(125,125,125));
	}

	max = App().GetDocument()->m_pGlobalSysParms->GetMaxSciBkt('A');
	// N'afficher que les SC donnés par le VNL (HRI, 11/08/2015)
	if (m_nbBktVnlSC_A >= 0)
	{
		max = m_nbBktVnlSC_A;
	}
	/// si A (SCX) SI (international)
	m_quotas_view_si_scx_a.Build (Quotas::SCX, 'A', max);
	m_quotas_view_si_scx_a.ShowWindow(SW_HIDE);

	/// si A (SCX) SD (domestique)
	m_quotas_view_sd_scx_a.Build (Quotas::SCX, 'A', max);
	m_quotas_view_sd_scx_a.ShowWindow(SW_HIDE);

	std::vector<QuotasValues>& listSiA = m_quotas_view_si_cc_a._list_ctrl.Datas();
	std::vector<QuotasValues>::iterator it;
	for(it = listSiA.begin(); it != listSiA.end(); ++it)
	{
		QuotasValues& value = (*it);
		if (value._nest_level._value == 0)
			value._moi._callback_edition = &EditingSIMoiCallbackCC0;
		else value._moi._callback_edition = &EditingSIMoiCallbackCC;
	}

	std::vector<QuotasValues>& listSdA = m_quotas_view_sd_cc_a._list_ctrl.Datas();
	for(it = listSdA.begin(); it != listSdA.end(); ++it)
	{
		QuotasValues& value = (*it);
		if (value._nest_level._value == 0)
			value._moi._callback_edition = &EditingSDMoiCallbackCC0;
		else value._moi._callback_edition = &EditingSDMoiCallbackCC;
	}

	std::vector<QuotasValues>& listSiScxA = m_quotas_view_si_scx_a._list_ctrl.Datas();
	for(it = listSiScxA.begin(); it != listSiScxA.end(); ++it)
	{
		QuotasValues& value = (*it);
		if (value._nest_level._value == 0)
			value._moi._callback_edition = &EditingSIMoiCallbackSCX0;
		else value._moi._callback_edition = &EditingSIMoiCallbackSCX;
	}

	std::vector<QuotasValues>& listSdScxA = m_quotas_view_sd_scx_a._list_ctrl.Datas();
	for(it = listSdScxA.begin(); it != listSdScxA.end(); ++it)
	{
		QuotasValues& value = (*it);
		if (value._nest_level._value == 0)
			value._moi._callback_edition = &EditingSDMoiCallbackSCX0;
		else value._moi._callback_edition = &EditingSDMoiCallbackSCX;
	}


	max = App().GetDocument()->m_pGlobalSysParms->GetMaxSciBkt('A');
	// N'afficher que les SC donnés par le VNL (HRI, 11/08/2015)
	if (m_nbBktVnlSC_A >= 0)
	{
		max = m_nbBktVnlSC_A;
	}

	for(int i = 0; i < max ; i++)
	{
		m_quotas_view_sd_scx_a._list_ctrl.SetEditable(Quotas::MOI, i , false);
		m_quotas_view_sd_scx_a._list_ctrl.SetGreyBackground(Quotas::MOI, i, true);
		m_quotas_view_sd_scx_a._list_ctrl.SetTextColor(Quotas::MOI, i, RGB(125,125,125));
	}

	/// Cmpt B
	max = App().GetDocument()->m_pGlobalSysParms->GetMaxCmptBkt('B');
	// N'afficher que les buckets donnés par le VNL (HRI, 11/08/2015)
	if (m_nbBktVnlCC_B > 0)
	{
		max = m_nbBktVnlCC_B;
	}	
	m_quotas_view_si_cc_b.Build (Quotas::CC, 'B', max);
	m_quotas_view_si_cc_b.ShowWindow(SW_HIDE);

	m_quotas_view_sd_cc_b.Build (Quotas::CC, 'B', max);
	m_quotas_view_sd_cc_b.ShowWindow(SW_HIDE);

	for(int i = 0; i < max ; i++)
	{
		m_quotas_view_sd_cc_b._list_ctrl.SetEditable(Quotas::MOI, i , false);
		m_quotas_view_sd_cc_b._list_ctrl.SetGreyBackground(Quotas::MOI, i, true);
		m_quotas_view_sd_cc_b._list_ctrl.SetTextColor(Quotas::MOI, i, RGB(125,125,125));
	}
	
	max = App().GetDocument()->m_pGlobalSysParms->GetMaxSciBkt('B');
	// N'afficher que les SC donnés par le VNL (HRI, 11/08/2015)
	if (m_nbBktVnlSC_B >= 0)
	{
		max = m_nbBktVnlSC_B;
	}

	m_quotas_view_si_scx_b.Build (Quotas::SCX, 'B', max);
	m_quotas_view_si_scx_b.ShowWindow(SW_HIDE);

	m_quotas_view_sd_scx_b.Build (Quotas::SCX, 'B', max);
	m_quotas_view_sd_scx_b.ShowWindow(SW_HIDE);

	for(int i = 0; i < max ; i++)
	{
		m_quotas_view_sd_scx_b._list_ctrl.SetEditable(Quotas::MOI, i , false);
		m_quotas_view_sd_scx_b._list_ctrl.SetGreyBackground(Quotas::MOI, i, true);
		m_quotas_view_sd_scx_b._list_ctrl.SetTextColor(Quotas::MOI, i, RGB(125,125,125));
	}

	std::vector<QuotasValues>& listSiB = m_quotas_view_si_cc_b._list_ctrl.Datas();
	for(it = listSiB.begin(); it != listSiB.end(); ++it)
	{
		QuotasValues& value = (*it);
		if (value._nest_level._value == 0)
			value._moi._callback_edition = &EditingSIMoiCallbackCC0;
		else value._moi._callback_edition = &EditingSIMoiCallbackCC;
	}

	std::vector<QuotasValues>& listSdB = m_quotas_view_sd_cc_b._list_ctrl.Datas();
	for(it = listSdB.begin(); it != listSdB.end(); ++it)
	{
		QuotasValues& value = (*it);
		if (value._nest_level._value == 0)
			value._moi._callback_edition = &EditingSDMoiCallbackCC0;
		else value._moi._callback_edition = &EditingSDMoiCallbackCC;
	}


	std::vector<QuotasValues>& listSiScxB = m_quotas_view_si_scx_b._list_ctrl.Datas();
	for(it = listSiScxB.begin(); it != listSiScxB.end(); ++it)
	{
		QuotasValues& value = (*it);
		if (value._nest_level._value == 0)
			value._moi._callback_edition = &EditingSIMoiCallbackSCX0;
		else value._moi._callback_edition = &EditingSIMoiCallbackSCX;
	}

	std::vector<QuotasValues>& listSdScxB = m_quotas_view_sd_scx_b._list_ctrl.Datas();
	for(it = listSdScxB.begin(); it != listSdScxB.end(); ++it)
	{
		QuotasValues& value = (*it);
		if (value._nest_level._value == 0)
			value._moi._callback_edition = &EditingSDMoiCallbackSCX0;
		else value._moi._callback_edition = &EditingSDMoiCallbackSCX;
	}

	/// CLE
	SwitchA();	
	/// CLE
}


void CQuotasBiTrancheDlg::SwitchA()
{
	m_quotas_view_si_cc_b.ShowWindow(SW_HIDE);
	m_quotas_view_sd_cc_b.ShowWindow(SW_HIDE);
	m_quotas_view_si_scx_b.ShowWindow(SW_HIDE);
	m_quotas_view_sd_scx_b.ShowWindow(SW_HIDE);

	m_quotas_view_si_cc_a.ShowWindow(SW_SHOW);
	m_quotas_view_sd_cc_a.ShowWindow(SW_SHOW);
	m_quotas_view_si_scx_a.ShowWindow(SW_SHOW);
	m_quotas_view_sd_scx_a.ShowWindow(SW_SHOW);

	this->ShowDynamicB(SW_HIDE);

	this->ShowStaticLabel(SW_SHOW);
	this->ShowDynamicA(SW_SHOW);
}

void CQuotasBiTrancheDlg::SwitchB()
{
	m_quotas_view_si_cc_a.ShowWindow(SW_HIDE);
	m_quotas_view_sd_cc_a.ShowWindow(SW_HIDE);
	m_quotas_view_si_scx_a.ShowWindow(SW_HIDE);
	m_quotas_view_sd_scx_a.ShowWindow(SW_HIDE);

	m_quotas_view_si_cc_b.ShowWindow(SW_SHOW);
	m_quotas_view_sd_cc_b.ShowWindow(SW_SHOW);
	m_quotas_view_si_scx_b.ShowWindow(SW_SHOW);
	m_quotas_view_sd_scx_b.ShowWindow(SW_SHOW);

	this->ShowDynamicA(SW_HIDE);

	this->ShowStaticLabel(SW_SHOW);
	this->ShowDynamicB(SW_SHOW);
}

void CQuotasBiTrancheDlg::ShowStaticLabel(int mode)
{
	CWnd* pItem;

	/* Tranche Internationale */
	pItem = this->GetDlgItem(IDC_TITRE_SI_GRP);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_TITRE_SI_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_TITRE_SI_CC1);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	/* Tranche Domestique */
	pItem = this->GetDlgItem(IDC_TITRE_SD_GRP);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_TITRE_SD_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_TITRE_SD_CC1);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

}
void CQuotasBiTrancheDlg::ShowDynamicA(int mode)
{
	CWnd* pItem;

	/* Tranche Internationale */
	pItem = this->GetDlgItem(IDC_LABEL_SI_A_CC_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SI_A_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);	
	pItem = this->GetDlgItem(IDC_LABEL_SI_A_SCX_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);	
	pItem = this->GetDlgItem(IDC_LABEL_SI_A_TOTAL_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SI_A_TOTAL_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	/* Tranche Domestique */
	pItem = this->GetDlgItem(IDC_LABEL_SD_A_CC_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_A_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_A_SCX_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_A_TOTAL_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_A_TOTAL_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
}
void CQuotasBiTrancheDlg::ShowDynamicB(int mode)
{
	CWnd* pItem;

	/* Tranche Internationale */
	pItem = this->GetDlgItem(IDC_LABEL_SI_B_CC_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SI_B_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);	
	pItem = this->GetDlgItem(IDC_LABEL_SI_B_SCX_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);	
	pItem = this->GetDlgItem(IDC_LABEL_SI_B_TOTAL_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SI_B_TOTAL_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	/* Tranche Domestique */
	pItem = this->GetDlgItem(IDC_LABEL_SD_B_CC_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_B_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_B_SCX_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_B_TOTAL_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_B_TOTAL_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

}


void CQuotasBiTrancheDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuotasBiTrancheDlg)
	//--SNCF INTERNATIONAL

	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_BITRANCHE_SI_CC_A, m_quotas_view_si_cc_a._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_BITRANCHE_SI_SCX_A, m_quotas_view_si_scx_a._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_BITRANCHE_SD_CC_A, m_quotas_view_sd_cc_a._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_BITRANCHE_SD_SCX_A, m_quotas_view_sd_scx_a._list_ctrl);

	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_BITRANCHE_SI_CC_B, m_quotas_view_si_cc_b._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_BITRANCHE_SI_SCX_B, m_quotas_view_si_scx_b._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_BITRANCHE_SD_CC_B, m_quotas_view_sd_cc_b._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_BITRANCHE_SD_SCX_B, m_quotas_view_sd_scx_b._list_ctrl);

	DDX_Control(pDX, IDC_LABEL_SI_A_CC_RES_TOT, m_SI_A_CC_RES_TOT);
	DDX_Control(pDX, IDC_LABEL_SD_A_CC_RES_TOT, m_SD_A_CC_RES_TOT);
	DDX_Control(pDX, IDC_LABEL_SI_B_CC_RES_TOT, m_SI_B_CC_RES_TOT);
	DDX_Control(pDX, IDC_LABEL_SD_B_CC_RES_TOT, m_SD_B_CC_RES_TOT);

	DDX_Control(pDX, IDC_LABEL_SI_A_SCX_RES_TOT, m_SI_A_SCX_RES_TOT);
	DDX_Control(pDX, IDC_LABEL_SD_A_SCX_RES_TOT, m_SD_A_SCX_RES_TOT);
	DDX_Control(pDX, IDC_LABEL_SI_B_SCX_RES_TOT, m_SI_B_SCX_RES_TOT);
	DDX_Control(pDX, IDC_LABEL_SD_B_SCX_RES_TOT, m_SD_B_SCX_RES_TOT);
	
	DDX_Control(pDX, IDC_LABEL_SD_A_GRP, m_IDC_LABEL_SD_A_GRP);
	DDX_Control(pDX, IDC_LABEL_SI_A_GRP, m_IDC_LABEL_SI_A_GRP);
	DDX_Control(pDX, IDC_LABEL_SD_B_GRP, m_IDC_LABEL_SD_B_GRP);
	DDX_Control(pDX, IDC_LABEL_SI_B_GRP, m_IDC_LABEL_SI_B_GRP);

	DDX_Control(pDX, IDC_LABEL_SD_A_DIFF, m_IDC_LABEL_SD_A_DIFF);
	DDX_Control(pDX, IDC_LABEL_SI_A_DIFF, m_IDC_LABEL_SI_A_DIFF);
	DDX_Control(pDX, IDC_LABEL_SD_B_DIFF, m_IDC_LABEL_SD_B_DIFF);
	DDX_Control(pDX, IDC_LABEL_SI_B_DIFF, m_IDC_LABEL_SI_B_DIFF);

	DDX_Control(pDX, IDC_LABEL_SD_A_TOTAL_ACT, m_IDC_LABEL_SD_A_TOTAL_ACT);
	DDX_Control(pDX, IDC_LABEL_SI_A_TOTAL_ACT, m_IDC_LABEL_SI_A_TOTAL_ACT);
	DDX_Control(pDX, IDC_LABEL_SD_B_TOTAL_ACT, m_IDC_LABEL_SD_B_TOTAL_ACT);
	DDX_Control(pDX, IDC_LABEL_SI_B_TOTAL_ACT, m_IDC_LABEL_SI_B_TOTAL_ACT);

	DDX_Control(pDX, IDC_LABEL_SD_A_TOTAL_MOI, m_IDC_LABEL_SD_A_TOTAL_MOI);
	DDX_Control(pDX, IDC_LABEL_SI_A_TOTAL_MOI, m_IDC_LABEL_SI_A_TOTAL_MOI);
	DDX_Control(pDX, IDC_LABEL_SD_B_TOTAL_MOI, m_IDC_LABEL_SD_B_TOTAL_MOI);
	DDX_Control(pDX, IDC_LABEL_SI_B_TOTAL_MOI, m_IDC_LABEL_SI_B_TOTAL_MOI);

	DDX_Control(pDX, IDC_CAPAA,m_CapaA);
	DDX_Control(pDX, IDC_CAPAB,m_CapaB);
	DDX_Control(pDX, IDC_CAPATOT,m_CapaTOT);
	DDX_Control(pDX, IDC_SOMME_MOI_A,m_iCapaMoi_A);
	DDX_Control(pDX, IDC_SOMME_MOI_B,m_iCapaMoi_B);
	DDX_Control(pDX, IDC_SOMME_MOI_TOT,m_iCapaMoi_TOT);
	//--TRONCON

	DDX_Control(pDX, IDC_COMBO_TRONCON, m_ComboTroncon);
	DDX_Control(pDX, IDC_LIST_TRONCON, m_ListTroncon);
	//--TITRE

	DDX_Control(pDX, IDC_STATIC_SI, m_SI);
	DDX_Control(pDX, IDC_STATIC_SD, m_SD);
	//--GRP
	
	//--TABLEAU DES TRONCON
	DDX_Control(pDX, IDC_TRONCON_COL_1, m_Troncon_Col_1);
	DDX_Control(pDX, IDC_TRONCON_COL_2, m_Troncon_Col_2);
	DDX_Control(pDX, IDC_TRONCON_COL_3, m_Troncon_Col_3);               
	DDX_Control(pDX, IDC_TRONCON_COL_4, m_Troncon_Col_4);
	DDX_Control(pDX, IDC_TRONCON_COL_5, m_Troncon_Col_5);               
	DDX_Control(pDX, IDC_TRONCON_COL_6, m_Troncon_Col_6);               
	DDX_Control(pDX, IDC_TRONCON_COL_7, m_Troncon_Col_7);               
	DDX_Control(pDX, IDC_TRONCON_COL_8, m_Troncon_Col_8);               
	DDX_Control(pDX, IDC_TRONCON_COL_9, m_Troncon_Col_9);               
	DDX_Control(pDX, IDC_TRONCON_COL_10, m_Troncon_Col_10);              
	DDX_Control(pDX, IDC_TRONCON_COL_11, m_Troncon_Col_11);              
	DDX_Control(pDX, IDC_TRONCON_COL_12, m_Troncon_Col_12);                       
	DDX_Control(pDX, IDC_SNI_A0_1, m_SNI_A0_1);                    
	DDX_Control(pDX, IDC_SNI_A0_2, m_SNI_A0_2);                     
	DDX_Control(pDX, IDC_SNI_A0_3, m_SNI_A0_3);                    
	DDX_Control(pDX, IDC_SNI_A0_4, m_SNI_A0_4);                    
	DDX_Control(pDX, IDC_SNI_A0_5, m_SNI_A0_5);                    
	DDX_Control(pDX, IDC_SNI_A0_6, m_SNI_A0_6);                    
	DDX_Control(pDX, IDC_SNI_A0_7, m_SNI_A0_7);                    
	DDX_Control(pDX, IDC_SNI_A0_8, m_SNI_A0_8);                    
	DDX_Control(pDX, IDC_SNI_A0_9, m_SNI_A0_9);                    
	DDX_Control(pDX, IDC_SNI_A0_10, m_SNI_A0_10);                   
	DDX_Control(pDX, IDC_SNI_A0_11, m_SNI_A0_11);                   
	DDX_Control(pDX, IDC_SNI_A0_12, m_SNI_A0_12);                                     
	DDX_Control(pDX, IDC_SND_A0_1, m_SND_A0_1);                   
	DDX_Control(pDX, IDC_SND_A0_2, m_SND_A0_2);                     
	DDX_Control(pDX, IDC_SND_A0_3, m_SND_A0_3);                    
	DDX_Control(pDX, IDC_SND_A0_4, m_SND_A0_4);                    
	DDX_Control(pDX, IDC_SND_A0_5, m_SND_A0_5);                    
	DDX_Control(pDX, IDC_SND_A0_6, m_SND_A0_6);                    
	DDX_Control(pDX, IDC_SND_A0_7, m_SND_A0_7);                    
	DDX_Control(pDX, IDC_SND_A0_8, m_SND_A0_8);                    
	DDX_Control(pDX, IDC_SND_A0_9, m_SND_A0_9);                    
	DDX_Control(pDX, IDC_SND_A0_10, m_SND_A0_10);                   
	DDX_Control(pDX, IDC_SND_A0_11, m_SND_A0_11);                   
	DDX_Control(pDX, IDC_SND_A0_12, m_SND_A0_12); 
	DDX_Control(pDX, IDC_SNI_DA0_1, m_SNI_DA0_1);                    
	DDX_Control(pDX, IDC_SNI_DA0_2, m_SNI_DA0_2);                     
	DDX_Control(pDX, IDC_SNI_DA0_3, m_SNI_DA0_3);                    
	DDX_Control(pDX, IDC_SNI_DA0_4, m_SNI_DA0_4);                    
	DDX_Control(pDX, IDC_SNI_DA0_5, m_SNI_DA0_5);                    
	DDX_Control(pDX, IDC_SNI_DA0_6, m_SNI_DA0_6);                    
	DDX_Control(pDX, IDC_SNI_DA0_7, m_SNI_DA0_7);                    
	DDX_Control(pDX, IDC_SNI_DA0_8, m_SNI_DA0_8);                    
	DDX_Control(pDX, IDC_SNI_DA0_9, m_SNI_DA0_9);                    
	DDX_Control(pDX, IDC_SNI_DA0_10, m_SNI_DA0_10);                   
	DDX_Control(pDX, IDC_SNI_DA0_11, m_SNI_DA0_11);                   
	DDX_Control(pDX, IDC_SNI_DA0_12, m_SNI_DA0_12);                                   
	DDX_Control(pDX, IDC_SND_DA0_1, m_SND_DA0_1);                   
	DDX_Control(pDX, IDC_SND_DA0_2, m_SND_DA0_2);                     
	DDX_Control(pDX, IDC_SND_DA0_3, m_SND_DA0_3);                    
	DDX_Control(pDX, IDC_SND_DA0_4, m_SND_DA0_4);                    
	DDX_Control(pDX, IDC_SND_DA0_5, m_SND_DA0_5);                    
	DDX_Control(pDX, IDC_SND_DA0_6, m_SND_DA0_6);                    
	DDX_Control(pDX, IDC_SND_DA0_7, m_SND_DA0_7);                    
	DDX_Control(pDX, IDC_SND_DA0_8, m_SND_DA0_8);                    
	DDX_Control(pDX, IDC_SND_DA0_9, m_SND_DA0_9);                    
	DDX_Control(pDX, IDC_SND_DA0_10, m_SND_DA0_10);                   
	DDX_Control(pDX, IDC_SND_DA0_11, m_SND_DA0_11);                   
	DDX_Control(pDX, IDC_SND_DA0_12, m_SND_DA0_12);  
	DDX_Control(pDX, IDC_Total_A0_1, m_Total_A0_1);                  
	DDX_Control(pDX, IDC_Total_A0_2, m_Total_A0_2);                  
	DDX_Control(pDX, IDC_Total_A0_3, m_Total_A0_3);                  
	DDX_Control(pDX, IDC_Total_A0_4, m_Total_A0_4);                  
	DDX_Control(pDX, IDC_Total_A0_5, m_Total_A0_5);                  
	DDX_Control(pDX, IDC_Total_A0_6, m_Total_A0_6);                  
	DDX_Control(pDX, IDC_Total_A0_7, m_Total_A0_7);                  
	DDX_Control(pDX, IDC_Total_A0_8, m_Total_A0_8);                  
	DDX_Control(pDX, IDC_Total_A0_9, m_Total_A0_9);                  
	DDX_Control(pDX, IDC_Total_A0_10, m_Total_A0_10);                 
	DDX_Control(pDX, IDC_Total_A0_11, m_Total_A0_11);                 
	DDX_Control(pDX, IDC_Total_A0_12, m_Total_A0_12);                                
	DDX_Control(pDX, IDC_SNI_B0_1, m_SNI_B0_1);                    
	DDX_Control(pDX, IDC_SNI_B0_2, m_SNI_B0_2);                    
	DDX_Control(pDX, IDC_SNI_B0_3, m_SNI_B0_3);                    
	DDX_Control(pDX, IDC_SNI_B0_4, m_SNI_B0_4);                    
	DDX_Control(pDX, IDC_SNI_B0_5, m_SNI_B0_5);                    
	DDX_Control(pDX, IDC_SNI_B0_6, m_SNI_B0_6);                    
	DDX_Control(pDX, IDC_SNI_B0_7, m_SNI_B0_7);                    
	DDX_Control(pDX, IDC_SNI_B0_8, m_SNI_B0_8);                    
	DDX_Control(pDX, IDC_SNI_B0_9, m_SNI_B0_9);                    
	DDX_Control(pDX, IDC_SNI_B0_10, m_SNI_B0_10);                   
	DDX_Control(pDX, IDC_SNI_B0_11, m_SNI_B0_11);                   
	DDX_Control(pDX, IDC_SNI_B0_12, m_SNI_B0_12);                                   
	DDX_Control(pDX, IDC_SND_B0_1, m_SND_B0_1);                    
	DDX_Control(pDX, IDC_SND_B0_2, m_SND_B0_2);                    
	DDX_Control(pDX, IDC_SND_B0_3, m_SND_B0_3);                    
	DDX_Control(pDX, IDC_SND_B0_4, m_SND_B0_4);                    
	DDX_Control(pDX, IDC_SND_B0_5, m_SND_B0_5);                    
	DDX_Control(pDX, IDC_SND_B0_6, m_SND_B0_6);                    
	DDX_Control(pDX, IDC_SND_B0_7, m_SND_B0_7);                    
	DDX_Control(pDX, IDC_SND_B0_8, m_SND_B0_8);                    
	DDX_Control(pDX, IDC_SND_B0_9, m_SND_B0_9);                    
	DDX_Control(pDX, IDC_SND_B0_10, m_SND_B0_10);                   
	DDX_Control(pDX, IDC_SND_B0_11, m_SND_B0_11);                   
	DDX_Control(pDX, IDC_SND_B0_12, m_SND_B0_12);
	DDX_Control(pDX, IDC_SNI_DB0_1, m_SNI_DB0_1);                    
	DDX_Control(pDX, IDC_SNI_DB0_2, m_SNI_DB0_2);                    
	DDX_Control(pDX, IDC_SNI_DB0_3, m_SNI_DB0_3);                    
	DDX_Control(pDX, IDC_SNI_DB0_4, m_SNI_DB0_4);                    
	DDX_Control(pDX, IDC_SNI_DB0_5, m_SNI_DB0_5);                    
	DDX_Control(pDX, IDC_SNI_DB0_6, m_SNI_DB0_6);                    
	DDX_Control(pDX, IDC_SNI_DB0_7, m_SNI_DB0_7);                    
	DDX_Control(pDX, IDC_SNI_DB0_8, m_SNI_DB0_8);                    
	DDX_Control(pDX, IDC_SNI_DB0_9, m_SNI_DB0_9);                    
	DDX_Control(pDX, IDC_SNI_DB0_10, m_SNI_DB0_10);                   
	DDX_Control(pDX, IDC_SNI_DB0_11, m_SNI_DB0_11);                   
	DDX_Control(pDX, IDC_SNI_DB0_12, m_SNI_DB0_12);                                
	DDX_Control(pDX, IDC_SND_DB0_1, m_SND_DB0_1);                    
	DDX_Control(pDX, IDC_SND_DB0_2, m_SND_DB0_2);                    
	DDX_Control(pDX, IDC_SND_DB0_3, m_SND_DB0_3);                    
	DDX_Control(pDX, IDC_SND_DB0_4, m_SND_DB0_4);                    
	DDX_Control(pDX, IDC_SND_DB0_5, m_SND_DB0_5);                    
	DDX_Control(pDX, IDC_SND_DB0_6, m_SND_DB0_6);                    
	DDX_Control(pDX, IDC_SND_DB0_7, m_SND_DB0_7);                    
	DDX_Control(pDX, IDC_SND_DB0_8, m_SND_DB0_8);                    
	DDX_Control(pDX, IDC_SND_DB0_9, m_SND_DB0_9);                    
	DDX_Control(pDX, IDC_SND_DB0_10, m_SND_DB0_10);                   
	DDX_Control(pDX, IDC_SND_DB0_11, m_SND_DB0_11);                   
	DDX_Control(pDX, IDC_SND_DB0_12, m_SND_DB0_12); 
	DDX_Control(pDX, IDC_Total_B0_1, m_Total_B0_1);                  
	DDX_Control(pDX, IDC_Total_B0_2, m_Total_B0_2);                  
	DDX_Control(pDX, IDC_Total_B0_3, m_Total_B0_3);                  
	DDX_Control(pDX, IDC_Total_B0_4, m_Total_B0_4);                  
	DDX_Control(pDX, IDC_Total_B0_5, m_Total_B0_5);                  
	DDX_Control(pDX, IDC_Total_B0_6, m_Total_B0_6);                  
	DDX_Control(pDX, IDC_Total_B0_7, m_Total_B0_7);                  
	DDX_Control(pDX, IDC_Total_B0_8, m_Total_B0_8);                  
	DDX_Control(pDX, IDC_Total_B0_9, m_Total_B0_9);                  
	DDX_Control(pDX, IDC_Total_B0_10, m_Total_B0_10);                 
	DDX_Control(pDX, IDC_Total_B0_11, m_Total_B0_11);                 
	DDX_Control(pDX, IDC_Total_B0_12, m_Total_B0_12);                 
	DDX_Control(pDX, IDC_A0_CAPA, m_A0_Capa);
	DDX_Control(pDX, IDC_B0_CAPA, m_B0_Capa);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CQuotasBiTrancheDlg, CDialog)
	//{{AFX_MSG_MAP(CQuotasBiTrancheDlg)
	ON_BN_CLICKED(IDC_BASCULE_QUOTAS_SWITCH_A, SwitchA)
	ON_BN_CLICKED(IDC_BASCULE_QUOTAS_SWITCH_B, SwitchB)
	ON_BN_CLICKED(IDC_UN_TRONCON, OnUnTroncon)
	ON_BN_CLICKED(IDC_TOUS_TRONCONS, OnTousTroncon)
	ON_BN_CLICKED(IDC_SEL_TRONCONS, OnSelTroncon)
	ON_BN_CLICKED(IDC_QR_QUITTER, OnQuitter)
	ON_BN_CLICKED(IDC_KEEP_ECART, OnKeepEcart)
	// SI Cmpt A
	// Combo Troncon
	ON_CBN_SELCHANGE(IDC_COMBO_TRONCON,OnMajTroncon)
	/* HRI 13/08/2015 : ajout gestion événement à la sortie des SCG */
	ON_EN_KILLFOCUS(IDC_LABEL_SI_A_TOTAL_MOI,OnMajSCIAG)
	ON_EN_KILLFOCUS(IDC_LABEL_SI_B_TOTAL_MOI,OnMajSCIBG)
	ON_EN_KILLFOCUS(IDC_LABEL_SD_A_TOTAL_MOI,OnMajSCDAG)
	ON_EN_KILLFOCUS(IDC_LABEL_SD_B_TOTAL_MOI,OnMajSCDBG)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_KB_LIST_END_REACHED, kbReachedEndList) //SRE 76064 Evol - Triggered by Quotas list views

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CQuotasBiTrancheDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// On va attendre d'avoir un VNL pour tailler cette liste
	// avec le bon nombre de buckets
	// Renvoi de cette méthode vers LireVnl()
	// HRI, le 10/08/2015
	InitCtrlQuotasList(); //NCH

	Chercher(); //01/12/12 supression KDISN, déplacer ici pour attendre InitDialog;
	bBucket = FALSE;
	//--SNCF INTERNATIONAL
	BlancFondStatic();
	RecapDispoVert();
	// Initialisation du flag pour repercuter l'écart sur les buckets

	CLegView* pLegView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetLegView();
	if ( pLegView)
		m_bOvbAdjustFlag = pLegView->GetOvbAdjustFlag();
	else
		m_bOvbAdjustFlag = false;
	((CButton*)GetDlgItem (IDC_KEEP_ECART))->SetCheck(m_bOvbAdjustFlag);
	m_iDiff = 0;
	bSIFlagModif = FALSE;
	bSDFlagModif = FALSE;
	bSCIAFlagModif = FALSE;
	bSCDAFlagModif = FALSE;
	bSCIBFlagModif = FALSE;
	bSCDBFlagModif = FALSE;
	bAllFlagModif = FALSE;
	bFlagModifManuelle = FALSE;
	GrisageVnauVnas();
	m_bFlag = FALSE;

	SetWindowText(m_szTitle);
	((CButton *)GetDlgItem(IDC_TOUS_TRONCONS))->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ICON_DANGER)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ICON_STOP)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ALARME)->SetWindowText("");

	return TRUE;
}

void CQuotasBiTrancheDlg::BlancFondStatic()
{

}

void CQuotasBiTrancheDlg::RecapDispoVert()
{
	
	COLORREF vdis = RGB (0, 128, 0);
	m_SNI_DA0_1.SetTextColor (vdis);                    
	m_SNI_DA0_2.SetTextColor (vdis);                     
	m_SNI_DA0_3.SetTextColor (vdis);                    
	m_SNI_DA0_4.SetTextColor (vdis);                    
	m_SNI_DA0_5.SetTextColor (vdis);                    
	m_SNI_DA0_6.SetTextColor (vdis);                    
	m_SNI_DA0_7.SetTextColor (vdis);                    
	m_SNI_DA0_8.SetTextColor (vdis);                    
	m_SNI_DA0_9.SetTextColor (vdis);                    
	m_SNI_DA0_10.SetTextColor (vdis);                   
	m_SNI_DA0_11.SetTextColor (vdis);                   
	m_SNI_DA0_12.SetTextColor (vdis);                                   
	m_SND_DA0_1.SetTextColor (vdis);                   
	m_SND_DA0_2.SetTextColor (vdis);                     
	m_SND_DA0_3.SetTextColor (vdis);                    
	m_SND_DA0_4.SetTextColor (vdis);                    
	m_SND_DA0_5.SetTextColor (vdis);                    
	m_SND_DA0_6.SetTextColor (vdis);                    
	m_SND_DA0_7.SetTextColor (vdis);                    
	m_SND_DA0_8.SetTextColor (vdis);                    
	m_SND_DA0_9.SetTextColor (vdis);                    
	m_SND_DA0_10.SetTextColor (vdis);                   
	m_SND_DA0_11.SetTextColor (vdis);                   
	m_SND_DA0_12.SetTextColor (vdis);                                  
	m_SNI_DB0_1.SetTextColor (vdis);                    
	m_SNI_DB0_2.SetTextColor (vdis);                    
	m_SNI_DB0_3.SetTextColor (vdis);                    
	m_SNI_DB0_4.SetTextColor (vdis);                    
	m_SNI_DB0_5.SetTextColor (vdis);                    
	m_SNI_DB0_6.SetTextColor (vdis);                    
	m_SNI_DB0_7.SetTextColor (vdis);                    
	m_SNI_DB0_8.SetTextColor (vdis);                    
	m_SNI_DB0_9.SetTextColor (vdis);                    
	m_SNI_DB0_10.SetTextColor (vdis);                   
	m_SNI_DB0_11.SetTextColor (vdis);                   
	m_SNI_DB0_12.SetTextColor (vdis);                              
	m_SND_DB0_1.SetTextColor (vdis);                    
	m_SND_DB0_2.SetTextColor (vdis);                    
	m_SND_DB0_3.SetTextColor (vdis);                    
	m_SND_DB0_4.SetTextColor (vdis);                    
	m_SND_DB0_5.SetTextColor (vdis);                    
	m_SND_DB0_6.SetTextColor (vdis);                    
	m_SND_DB0_7.SetTextColor (vdis);                    
	m_SND_DB0_8.SetTextColor (vdis);                    
	m_SND_DB0_9.SetTextColor (vdis);                    
	m_SND_DB0_10.SetTextColor (vdis);                   
	m_SND_DB0_11.SetTextColor (vdis);                   
	m_SND_DB0_12.SetTextColor (vdis);                                     
	
}

void CQuotasBiTrancheDlg::SciFondStatic()
{
	COLORREF clrBkSci = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt();
}

// SI Cmpt A 
void CQuotasBiTrancheDlg::SIRemplissageA()
{
	m_quotas_view_si_cc_a._list_ctrl.SetLine(m_pVnlBkt->GetNestLevel(), m_pVnlBkt->GetBaseSold(), m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAvail());

	if (m_pVnlBkt->GetNestLevel() == 0)
	{
		if (m_pVnlCmpt != NULL)
			m_iCapaA = m_pVnlCmpt->GetCapacity();
	}
}

void CQuotasBiTrancheDlg::SIRemplissageFinA()
{
	/// DM-7978 - CLE
	m_iSomme = m_quotas_view_si_cc_b._list_ctrl.Sum(Quotas::VENTES, 0, m_quotas_view_si_cc_a._list_ctrl.MaxIndexation());

	/// TODO
	CString sTemp;
	sTemp.Format("%d",m_iSomme);
	sTemp.Format("%d",0);

	//NCH
	m_IDC_LABEL_SI_A_DIFF.SetWindowText(sTemp);
}
// SI Cmpt B


void CQuotasBiTrancheDlg::SIRemplissageB()
{
	m_quotas_view_si_cc_b._list_ctrl.SetLine(m_pVnlBkt->GetNestLevel(), m_pVnlBkt->GetBaseSold(), m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAvail());
	if (m_pVnlBkt->GetNestLevel() == 0)
	{
		if (m_pVnlCmpt != NULL)
			m_iCapaB = m_pVnlCmpt->GetCapacity();
}
}

void CQuotasBiTrancheDlg::SIRemplissageFinB()
{
	/// DM-7978 - CLE
	CString sTemp;
	m_iSomme = m_quotas_view_si_cc_b._list_ctrl.Sum(Quotas::VENTES, 0, m_quotas_view_si_cc_b._list_ctrl.MaxIndexation());

	/// TODO
	sTemp.Format("%d",m_iSomme);
	//m_SI_BRES_TOT.SetWindowText(sTemp);    // colonne Vdu+res Total
	sTemp.Format("%d",0);
	//m_SI_BDIFF.SetWindowText(sTemp);         // Champ Diff

	// DM7978 - NCH
	m_IDC_LABEL_SI_B_DIFF.SetWindowText(sTemp);
}


void CQuotasBiTrancheDlg::SDRemplissage(QuotasAutorisationsListCtrl& ctrl)
{
	//bool is_inter = this->TronconInter();
	bool autoriseModif = true;

	long moi = m_pVnlBkt->GetAuth();
	
	/* Ano 92999 - NBN
	if (is_inter)
	{
		if (moi != 0)
		{
			moi = 0;
			this->bSDFlagModif = TRUE;
		}
	}
	*/

	ctrl.SetLine(m_pVnlBkt->GetNestLevel(), m_pVnlBkt->GetBaseSold(), m_pVnlBkt->GetAuth(), 
		moi, m_pVnlBkt->GetAvail());

	// Si tronconInter() return true, rendre l'édition de quota inaccessible 

	// Désactive l'édition si le tronçon est inter
	ctrl.SetEditable (Quotas::MOI, m_pVnlBkt->GetNestLevel(), autoriseModif);

	// Mettre le background en gris
	ctrl.SetGreyBackground(Quotas::MOI, m_pVnlBkt->GetNestLevel(), !autoriseModif);

	if (!autoriseModif)
	{
		// Mettre le background en gris, font en gris foncé
		ctrl.SetTextColor(Quotas::MOI, m_pVnlBkt->GetNestLevel(), RGB(128, 128, 128));
	}
	else
	{
		ctrl.SetTextColor(Quotas::MOI, m_pVnlBkt->GetNestLevel(), RGB(0,0,255));
		
	}
}

void CQuotasBiTrancheDlg::SDRemplissageA()
{
	SDRemplissage(m_quotas_view_sd_cc_a._list_ctrl);
}
	

void CQuotasBiTrancheDlg::SDRemplissageFinA()
{
	CString sTemp;
	sTemp.Format("%d",m_iSomme);
	//m_SD_ARES_TOT.SetWindowText(sTemp);    // colonne Vdu+res Total
	sTemp.Format("%d",0);
	//m_SD_ADIFF.SetWindowText(sTemp);         // Champ Diff

	//NCH
	m_IDC_LABEL_SD_A_DIFF.SetWindowText(sTemp);
}


// DM7978 - NCH
void CQuotasBiTrancheDlg::SDRemplissageB()
{
	SDRemplissage(m_quotas_view_sd_cc_b._list_ctrl);
}


void CQuotasBiTrancheDlg::SDRemplissageFinB()
{
	CString sTemp;
	sTemp.Format("%d",m_iSomme);
	//TODO
	//m_SD_BRES_TOT.SetWindowText(sTemp);    // colonne Vdu+res Total
	sTemp.Format("%d",0);
	//m_SD_BDIFF.SetWindowText(sTemp);         // Champ Diff

	// DM7978 - NCH
	m_IDC_LABEL_SD_B_DIFF.SetWindowText(sTemp);
}


// DM7978 - NCH
void CQuotasBiTrancheDlg::SCIRemplissageA()
{
	m_quotas_view_si_scx_a._list_ctrl.SetLine(m_pVnlSci->GetNestLevel(), m_pVnlSci->GetBaseSold(), m_pVnlSci->GetAuth(), 
		m_pVnlSci->GetAuth(), m_pVnlSci->GetAvail(), m_pVnlSci->GetEtanche());
}


// DM7978 - NCH
void CQuotasBiTrancheDlg::SCIRemplissageB()
{
	/// be careful if etanche prefix du numero de ligne par (E)

	m_quotas_view_si_scx_b._list_ctrl.SetLine(m_pVnlSci->GetNestLevel(), m_pVnlSci->GetBaseSold(), m_pVnlSci->GetAuth(), 
		m_pVnlSci->GetAuth(), m_pVnlSci->GetAvail(), m_pVnlSci->GetEtanche());
}



void CQuotasBiTrancheDlg::SCDRemplissage(QuotasAutorisationsListCtrl& ctrl)
{
	int nest_level = m_pVnlSci->GetNestLevel();
	//bool is_inter = this->TronconInter();
	// NBN ANO 92999
	bool autoriseModifs = true;

	long moi = m_pVnlSci->GetAuth();
	/*
	if (is_inter)
	{
		if (moi != 0)
		{
			moi = 0;
			bSCDAFlagModif = TRUE;
		}
	}
	*/

	ctrl.SetLine(nest_level, m_pVnlSci->GetBaseSold(), m_pVnlSci->GetAuth(), moi, m_pVnlSci->GetAvail(), m_pVnlSci->GetEtanche());
	
	// Si tronconInter() return true, rendre l'édition de quota inaccessible 
	

	// NBN ANO 92999
	ctrl.SetEditable (Quotas::MOI, nest_level, autoriseModifs);

	// Mettre le background en gris
	ctrl.SetGreyBackground(Quotas::MOI, nest_level, !autoriseModifs);

	// NBN ANO 92999
	if (!autoriseModifs)
	{
		// Mettre le background en gris, font en gris foncé
		
		ctrl.SetTextColor(Quotas::MOI, nest_level, RGB(128, 128, 128));

		/// check act 
		QuotasValue* value = ctrl.GetValue(Quotas::ACT, nest_level);
		if (value != NULL)
		{
			if (value->_value != 0)
			{
				if (ctrl.EspacePhysique() == 'A')
					bSCDAFlagModif = TRUE;
				else if (ctrl.EspacePhysique() == 'B')
					bSCDBFlagModif = TRUE;
			}
		}
	}
	else
	{
		ctrl.SetTextColor(Quotas::MOI, nest_level, RGB(0,0,255));
	}
}

void CQuotasBiTrancheDlg::SCDRemplissageA()
{
	SCDRemplissage(m_quotas_view_sd_scx_a._list_ctrl);
}


// DM7978 - NCH
void CQuotasBiTrancheDlg::SCDRemplissageB()
{
	SCDRemplissage(m_quotas_view_sd_scx_b._list_ctrl);
}


void CQuotasBiTrancheDlg::RecapRemplissage()
{
	/// TODO
	CString sACT,sSom,sMOI,sEcart,sCapa,sTot;
	int iSomActA,iSomActB;
	int iSomMoiA,iSomMoiB;
	int iEcartA,iEcartB;

	// Cmpt A première ligne
	
	/// init 
	iSomActA = 0;
	iSomMoiA = 0;
	iSomMoiB = 0;
	iSomActB = 0;
	iEcartA = 0;
	iEcartB = 0;

	/// CLE
	QuotasValue* valueSIA_ACT = m_quotas_view_si_cc_a._list_ctrl.GetValue(Quotas::ACT, 0);
	if (valueSIA_ACT != NULL)
	{
		iSomActA += valueSIA_ACT->_value;
	}

	QuotasValue* valueSDA_ACT = m_quotas_view_sd_cc_a._list_ctrl.GetValue(Quotas::ACT, 0);
	if (valueSDA_ACT != NULL)
	{
		iSomActA += valueSDA_ACT->_value;
	}

	QuotasValue* valueSIB_ACT = m_quotas_view_si_cc_b._list_ctrl.GetValue(Quotas::ACT, 0);
	if (valueSIB_ACT != NULL)
	{
		iSomActB += valueSIB_ACT->_value;
	}

	QuotasValue* valueSDB_ACT = m_quotas_view_sd_cc_b._list_ctrl.GetValue(Quotas::ACT, 0);
	if (valueSDB_ACT != NULL)
	{
		iSomActB += valueSDB_ACT->_value;
	}

	QuotasValue* valueSIA_MOI = m_quotas_view_si_cc_a._list_ctrl.GetValue(Quotas::MOI, 0);
	if (valueSIA_MOI != NULL)
	{
		iSomMoiA += valueSIA_MOI->_value;
	}

	QuotasValue* valueSDA_MOI = m_quotas_view_sd_cc_a._list_ctrl.GetValue(Quotas::MOI, 0);
	if (valueSDA_MOI != NULL)
	{
		iSomMoiA += valueSDA_MOI->_value;
	}

	QuotasValue* valueSIB_MOI = m_quotas_view_si_cc_b._list_ctrl.GetValue(Quotas::MOI, 0);
	if (valueSIB_MOI != NULL)
	{
		iSomMoiB += valueSIB_MOI->_value;
	}

	QuotasValue* valueSDB_MOI = m_quotas_view_sd_cc_b._list_ctrl.GetValue(Quotas::MOI, 0);
	if (valueSDB_MOI != NULL)
	{
		iSomMoiB += valueSDB_MOI->_value;
	}


	/// CLE
	sSom.Format("%d",(iSomMoiA));
	m_iCapaMoi_A.SetWindowText(sSom);

	sCapa.Format("%d",(m_iCapaA));
	m_CapaA.SetWindowText(sCapa); 
	m_A0_Capa.SetWindowText(sCapa); 

	iEcartA = iSomActA - m_iCapaA;
	sEcart.Format("%d",(iEcartA));

	// Cmpt B deuxième ligne
	sSom.Format("%d",(iSomMoiB));
	m_iCapaMoi_B.SetWindowText(sSom);

	sCapa.Format("%d",(m_iCapaB));
	m_CapaB.SetWindowText(sCapa); 
	m_B0_Capa.SetWindowText(sCapa); 

	iEcartB = iSomActB - m_iCapaB;
	sEcart.Format("%d",(iEcartB));

	// ligne total
	sSom.Format("%d",iSomMoiA + iSomMoiB);
	m_iCapaMoi_TOT.SetWindowText(sSom);
	sCapa.Format("%d", m_iCapaA + m_iCapaB);
	m_CapaTOT.SetWindowText(sCapa); 

	CString sTemp;
	sTemp.Format("%d", ((m_ovbA * m_iCapaA) +50) / 100);
	GetDlgItem(IDC_MAXA)->SetWindowText (sTemp);
	sTemp.Format("%d", ((m_ovbB * m_iCapaB) +50) / 100);
	GetDlgItem(IDC_MAXB)->SetWindowText (sTemp);
	sTemp.Format("%d", ((m_ovg * (m_iCapaA + m_iCapaB)) +50) / 100);
	GetDlgItem(IDC_MAXTOT)->SetWindowText (sTemp);

	VerifFlagTroncon();
	GrisageTroncon();

}

void CQuotasBiTrancheDlg::GrpRemplissage()
{
	CString sGrp;
	/* NCH */
	sGrp.Format("%d",m_SI_AGrp);
	m_IDC_LABEL_SD_A_GRP.SetWindowText(sGrp);
	sGrp.Format("%d",m_SD_AGrp);
	m_IDC_LABEL_SI_A_GRP.SetWindowText(sGrp);
	sGrp.Format("%d",m_SI_BGrp);
	m_IDC_LABEL_SD_B_GRP.SetWindowText(sGrp);
	sGrp.Format("%d",m_SD_BGrp);
	m_IDC_LABEL_SI_B_GRP.SetWindowText(sGrp);
}


void CQuotasBiTrancheDlg::AnalyseVnlSI(const CString& sOrigine,YM_Vnl &pVnlSI)
{
	YM_VnlLeg* pVnlLeg;
	YM_VnlCmpt* pVnlCmpt;
	YM_VnlBkt* pVnlBkt;
	YM_VnlSci* pVnlSci;
	m_iSomme = 0;
	m_iCapaA = 0;
	m_iCapaB = 0;
	CString sTroncon;
	int iCount = 0;

	int iCountbad = m_ListTroncon.GetCount();
	iCountbad = m_ComboTroncon.GetCount();


	// Boucle sur le Leg
	YM_VnlLegIterator* LegIterator = pVnlSI.CreateVnlLegIterator();
	for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
	{
		pVnlLeg = LegIterator->Current();
		if(pVnlLeg->DepartsNextDay())
			sTroncon.Format("%s...%s",pVnlLeg->GetLegOrig(),pVnlLeg->GetLegDest());
		else
			sTroncon.Format("%s - %s",pVnlLeg->GetLegOrig(),pVnlLeg->GetLegDest());
		if (!m_bFlag)
		{
			m_ListTroncon.InsertString(iCount, sTroncon);
			m_ComboTroncon.InsertString(iCount, sTroncon);
		}
		iCount++;
		if (strcmp(pVnlLeg->GetLegOrig(),sOrigine) == 0)
			m_ComboTroncon.SetCurSel(iCount-1);

		// Boucle sur les cmpt A et B
		YM_VnlCmptIterator* CmptIterator = pVnlLeg->CreateVnlCmptIterator();
		for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
		{
			pVnlCmpt = CmptIterator->Current();
			// Boucle sur les buckets 0 à 6 ou 7
			YM_VnlBktIterator* BktIterator = pVnlCmpt->CreateVnlBktIterator();
			m_pVnlCmpt = pVnlCmpt;
			for ( BktIterator->First(); ! BktIterator->Finished(); BktIterator->Next() )
			{
				pVnlBkt = BktIterator->Current();
				m_pVnlBkt = pVnlBkt;
				if(strcmp(pVnlCmpt->GetCmptId(),"A") == 0)
				{
					if (pVnlBkt->GetNestLevel() == 0)
					{
						SumBkt0A += m_pVnlBkt->GetAuth();
						SumCapA += m_pVnlCmpt->GetCapacity();
						TabTronconSIA(iCount,pVnlLeg->GetLegOrig() ,m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAvail());
					}
					if (strcmp(pVnlLeg->GetLegOrig(),sOrigine) == 0)
					{
						// DM 7978 - FMA - remplissage générique
						SIRemplissageA();
					}
				}
				else // Cmpt B
				{  
					if (pVnlBkt->GetNestLevel() == 0)
					{
						SumBkt0B += m_pVnlBkt->GetAuth();
						SumCapB += m_pVnlCmpt->GetCapacity();
						TabTronconSIB(iCount, m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAvail());
					}
					if (strcmp(pVnlLeg->GetLegOrig(),sOrigine) == 0)
					{
						SIRemplissageB();
					}
			 }
		 }
			delete BktIterator;
			if (strcmp(pVnlLeg->GetLegOrig(),sOrigine) == 0)
			{
				// traitement de fin d'espace physique
				if (strcmp(pVnlCmpt->GetCmptId(),"A") == 0)
				{
//					masquerUnsusedBucketSIA (pVnlBkt->GetNestLevel());
					SIRemplissageFinA();

					/*CString sTemp;
					int maxBktSize = App().GetDocument()->m_pGlobalSysParms->GetMaxCmptBkt('A');
					sTemp.Format("%ld",m_quotas_view_sd_cc_a._list_ctrl.Sum(Quotas::VENTES, 0, maxBktSize));
					m_SD_B_CC_RES_TOT.SetWindowText(sTemp);*/

					CString sTemp;
					sTemp.Format("%ld",m_quotas_view_si_cc_a._list_ctrl.Sum(Quotas::VENTES, 0, App().GetDocument()->m_pGlobalSysParms->GetMaxCmptBkt('A')));
					m_SI_A_CC_RES_TOT.SetWindowText(sTemp);
				}
				else // Cmpt B
				{
//					masquerUnsusedBucketSIB (pVnlBkt->GetNestLevel());
					SIRemplissageFinB();

					CString sTemp;
					sTemp.Format("%ld",m_quotas_view_si_cc_b._list_ctrl.Sum(Quotas::VENTES, 0, App().GetDocument()->m_pGlobalSysParms->GetMaxCmptBkt('B')));
					m_SI_B_CC_RES_TOT.SetWindowText(sTemp);
				}
		 }
			m_iSomme = 0; //somme des ventes initialisé ici, car présence SCI non obligatoire

			if (m_YaSci)
			{
				if (strcmp(pVnlLeg->GetLegOrig(),sOrigine) == 0)
				{
					CString sTemp;
					sTemp.Format("%d",pVnlCmpt->GetScgCapa());
					if(strcmp(pVnlCmpt->GetCmptId(),"A") == 0)
					{
						m_IDC_LABEL_SI_A_TOTAL_ACT.SetWindowText(sTemp); //NCH
						/*
						m_SCI_AACT_G.SetWindowText(sTemp);
						m_SCI_AMOI_G.SetWindowText(sTemp);
						*/
						m_IDC_LABEL_SI_A_TOTAL_MOI.SetWindowText(sTemp); //NCH
						m_valeurPrecAG_I = atoi(sTemp); // Rajout HRI, 13/08/2015

						sTemp.Format("%d",pVnlCmpt->GetScgSold());
						/*
						m_SCI_ARES_TOT.SetWindowText(sTemp);
						*/

						m_SI_A_SCX_RES_TOT.SetWindowText(sTemp); //NCH
//						ValScIA[0] = pVnlCmpt->GetScgCapa();
					}
					else
					{
						m_IDC_LABEL_SI_B_TOTAL_ACT.SetWindowText(sTemp); //NCH
						/*
						m_SCI_BACT_G.SetWindowText(sTemp);
						m_SCI_BMOI_G.SetWindowText(sTemp);
						*/
						m_IDC_LABEL_SI_B_TOTAL_MOI.SetWindowText(sTemp); //NCH
						m_valeurPrecBG_I = atoi(sTemp);
						
						sTemp.Format("%d",pVnlCmpt->GetScgSold());
						/*
						m_SCI_BRES_TOT.SetWindowText(sTemp);
						*/
						m_SI_B_SCX_RES_TOT.SetWindowText(sTemp); //NCH
//						ValScIB[0] = pVnlCmpt->GetScgCapa();
					}
				}
				YM_VnlSciIterator* SciIterator = pVnlCmpt->CreateVnlSciIterator();
				for ( SciIterator->First(); ! SciIterator->Finished(); SciIterator->Next() )
				{
					pVnlSci = SciIterator->Current();
					m_pVnlSci = pVnlSci;
					if(strcmp(pVnlCmpt->GetCmptId(),"A") == 0)
					{
						if (strcmp(pVnlLeg->GetLegOrig(),sOrigine) == 0)
						{
							//DM7978 - NCH
							SCIRemplissageA();
						}
					}
					else // Cmpt B
					{
						if (strcmp(pVnlLeg->GetLegOrig(),sOrigine) == 0)
						{
							//DM7978 - NCH
							SCIRemplissageB();
						}
					}
				}
				delete SciIterator;
				// traitement de fin d'espace physique
			} // if m_yaSci
		}

		delete CmptIterator;
	}
	m_iTroncon = iCount;
	delete LegIterator;
	m_bFlag =TRUE;
}

void CQuotasBiTrancheDlg::AnalyseVnlSD(const CString& sOrigine,YM_Vnl &pVnlSD)
{
	YM_VnlLeg* pVnlLeg;
	YM_VnlCmpt* pVnlCmpt;
	YM_VnlBkt* pVnlBkt;
	YM_VnlSci* pVnlSci;
	m_iSomme = 0;
	int iCount = 0;
	m_bSNDetranger = FALSE;

	// Boucle sur le Leg
	YM_VnlLegIterator* LegIterator = pVnlSD.CreateVnlLegIterator();
	for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
	{
		pVnlLeg = LegIterator->Current();
		iCount++;
		// Boucle sur les cmpt A et B
		YM_VnlCmptIterator* CmptIterator = pVnlLeg->CreateVnlCmptIterator();
		for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
		{
			pVnlCmpt = CmptIterator->Current();
			// Boucle sur les buckets 0 à 6
			YM_VnlBktIterator* BktIterator = pVnlCmpt->CreateVnlBktIterator();
			m_pVnlCmpt = pVnlCmpt;
			for ( BktIterator->First(); ! BktIterator->Finished(); BktIterator->Next() )
			{
				pVnlBkt = BktIterator->Current();
				m_pVnlBkt = pVnlBkt;
				if(pVnlCmpt->GetCmptId() == "A")
				{
					if (pVnlBkt->GetNestLevel() == 0)
					{
						SumBkt0A += m_pVnlBkt->GetAuth();
						TabTronconSDA(iCount, m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAvail());
						if ((m_pVnlBkt->GetAuth() > 0) &&
							((pVnlLeg->GetLegOrig().Left(2) != "FR") || (pVnlLeg->GetLegDest().Left(2) != "FR")))
							m_bSNDetranger = TRUE;
					}
					if (strcmp(pVnlLeg->GetLegOrig(),sOrigine) == 0)
					{
						// DM7978 - NCH
						SDRemplissageA();
					}

				}
				else // Cmpt B
				{  
					if (pVnlBkt->GetNestLevel() == 0)
					{
						SumBkt0B += m_pVnlBkt->GetAuth();
						TabTronconSDB(iCount, m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAvail());
					}
					if (strcmp(pVnlLeg->GetLegOrig(),sOrigine) == 0)
					{
						//DM7978 - NCH
						SDRemplissageB();
					}
				}  
		 }
			delete BktIterator;
			if (strcmp(pVnlLeg->GetLegOrig(),sOrigine) == 0)
			{
				// traitement de fin d'espace physique
				if (strcmp(pVnlCmpt->GetCmptId(),"A") == 0)
				{
					SDRemplissageFinA();

					CString sTemp;
					sTemp.Format("%ld",m_quotas_view_sd_cc_a._list_ctrl.Sum(Quotas::VENTES, 0, App().GetDocument()->m_pGlobalSysParms->GetMaxCmptBkt('A')));
					
					m_SD_A_CC_RES_TOT.SetWindowText(sTemp);
				}
				else // Cmpt B
				{
					SDRemplissageFinB();

					CString sTemp;
					sTemp.Format("%ld",m_quotas_view_sd_cc_b._list_ctrl.Sum(Quotas::VENTES, 0, App().GetDocument()->m_pGlobalSysParms->GetMaxCmptBkt('B')));
					
					m_SD_B_CC_RES_TOT.SetWindowText(sTemp);
				}
		 }
			m_iSomme = 0; //somme des ventes initialisé ici, car présence SCI non obligatoire

			if (m_YaSci)
			{
				if (strcmp(pVnlLeg->GetLegOrig(),sOrigine) == 0)
				{
					CString sTemp;
					sTemp.Format("%d",pVnlCmpt->GetScgCapa());
					if(strcmp(pVnlCmpt->GetCmptId(),"A") == 0)
					{
						// SCG : zone importante à bien gérer 
						m_IDC_LABEL_SD_A_TOTAL_ACT.SetWindowText(sTemp); // NCH
						m_valeurPrecAG_D = atoi(sTemp);  // Rajout HRI, 13/08/2015

						bSCDAFlagModif = TRUE;

						m_IDC_LABEL_SD_A_TOTAL_MOI.EnableWindow(TRUE);   // NCH
						m_IDC_LABEL_SD_A_TOTAL_MOI.SetWindowText(sTemp); // NCH						

						sTemp.Format("%d",pVnlCmpt->GetScgSold());
						m_SD_A_SCX_RES_TOT.SetWindowText(sTemp);					// NCH
					}
					else
					{
						m_IDC_LABEL_SD_B_TOTAL_ACT.SetWindowText(sTemp); // NCH

						bSCDBFlagModif = TRUE;
						
						m_IDC_LABEL_SD_B_TOTAL_MOI.EnableWindow(TRUE);		//NCH
						m_IDC_LABEL_SD_B_TOTAL_MOI.SetWindowText(sTemp);	//NCH
						m_valeurPrecBG_D = atoi(sTemp);  // Rajout HRI, 13/08/2015
						
						sTemp.Format("%d",pVnlCmpt->GetScgSold());
						m_SD_B_SCX_RES_TOT.SetWindowText(sTemp);		//NCH
					}
				}
				YM_VnlSciIterator* SciIterator = pVnlCmpt->CreateVnlSciIterator();
				for ( SciIterator->First(); ! SciIterator->Finished(); SciIterator->Next() )
				{
					pVnlSci = SciIterator->Current();
					m_pVnlSci = pVnlSci;
					if(strcmp(pVnlCmpt->GetCmptId(),"A") == 0)
					{
						if (strcmp(pVnlLeg->GetLegOrig(),sOrigine) == 0)
						{
							//DM7978-NCH
							SCDRemplissageA();
						}
					}
					else // Cmpt B
					{
						if (strcmp(pVnlLeg->GetLegOrig(),sOrigine) == 0)
						{
							//DM7978-NCH
							SCDRemplissageB();
						}
					}
				}
				delete SciIterator;
				// traitement de fin d'espace physique
			} // if m_YaSci
		}
		delete CmptIterator;
	}
	delete LegIterator;



}

void CQuotasBiTrancheDlg::SansSci()
{
	/// TODO 
	m_quotas_view_si_scx_a.ShowWindow(SW_HIDE);
	m_quotas_view_sd_scx_b.ShowWindow(SW_HIDE);
	m_quotas_view_si_scx_a.ShowWindow(SW_HIDE);
	m_quotas_view_si_scx_b.ShowWindow(SW_HIDE);
}


void CQuotasBiTrancheDlg::OnMajTroncon()
{
	CString sSelTroncon;
	CString sOrigTroncon;
	int iIndex = 0;
	if ((bSIFlagModif || bSDFlagModif))
	{
		if ( AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_MODIF_DONE),
			AfxGetAppName(), MB_OKCANCEL | MB_APPLMODAL | MB_ICONEXCLAMATION) == IDCANCEL )
		{// On remet l'ancien tronçon
			if (sLastTroncon.IsEmpty())
		 {
			 m_ComboTroncon.SetCurSel (0);
			 return;
		 }
		 for (iIndex = 0; iIndex < m_ComboTroncon.GetCount(); iIndex++)
		 {
			 m_ComboTroncon.GetLBText(iIndex, sOrigTroncon);
			 sOrigTroncon = sOrigTroncon.Left(5);
			 if (sOrigTroncon == sLastTroncon)
			 {
				 m_ComboTroncon.SetCurSel (iIndex);
				 return;
			 }
		 }
		 return;
		}
	}   
	m_ComboTroncon.GetWindowText(sSelTroncon);
	sOrigTroncon = sSelTroncon.Left(5);
	StartSendVnl(sOrigTroncon); // Affichage du troncon selectionne
	bSIFlagModif = FALSE;
	bSDFlagModif = FALSE;
	bSCIAFlagModif = FALSE;
	bSCDAFlagModif = FALSE;
	bSCIBFlagModif = FALSE;
	bSCDBFlagModif = FALSE;
	bFlagModifManuelle = FALSE;

	GrisageVnauVnas();
}
bool CQuotasBiTrancheDlg::TronconInter() // renvoi vrai si la tronçon courant est "international"
{
	CString sSelTroncon;
	CString ss1, ss2;
	m_ComboTroncon.GetWindowText(sSelTroncon);
	ss1 = sSelTroncon.Left(2);
	ss2 = sSelTroncon.Mid(8,2);
	if ((ss1 == "FR") && (ss2 == "FR"))
		return false;
	else
		return true;
}

void CQuotasBiTrancheDlg::StartSendVnl(const CString& sTronconVisu)
{
	clearListControl();
	SumBkt0A = 0;
	SumBkt0B = 0;
	SumCapA = 0;
	SumCapB = 0;
	m_VnlCount = 1;
	sLastTroncon = sTronconVisu;
	SendNextVnl();
}


void CQuotasBiTrancheDlg::SendNextVnl()
{
	if (m_VnlCount > 2)
		return;

	CString sd, msg;
	RWDate dada;
	dada.julian (m_lDateDep);
	sd.Format ( "%2.2u", dada.dayOfMonth());
	sd += MONTH_LIST_ENGLISH.Mid( (dada.month()-1)*3, 3 );

	switch (m_VnlCount)
	{
	case 1 : 
		msg.Format ("VNL%d", m_trancheno1);
		break;
	case 2 :
		msg.Format ("VNL%d", m_trancheno2);
		break;
	default : 
		return;
	}
	msg += '/';
	msg += sd;
	msg += "/SC"; 

#ifdef _DEBUG
	TRACE("Envoi vnl " + msg + "\r\n");
#endif

	std::cout << " " << std::endl;
	/// DM-7978 - CLE 
	CYMVnlBouchon::getInstance()->setBiTranche(m_trancheno1, m_trancheno2);
	((CCTAApp*)APP)->GetResComm()->SendVnl (this, msg);
	/// DM-7978 - CLE
	std::cout << " " << std::endl;
}

void CQuotasBiTrancheDlg::LireVnl(const CString& reponse)
{
	CString sTronconVisu;
	YM_Vnl pVnl;
	int iGrpA = 0;
	int iGrpB = 0;

	int errorParsing = pVnl.Parse(reponse);  // parse the VNL response
  	// Anomalie 67837 : SATC 8.0.2 : consolidation du code
	if (errorParsing == VNL_PARSE_ERROR)
	{
		CString msgErrorParsing = 
			((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VNL_ERROR) + 
				": " + reponse;
		AfxMessageBox(msgErrorParsing, MB_OK);
		return;
	}
	m_YaSci = pVnl.YaSci();

	// HRI, 10/08/2015 : pour n'afficher que les CC conformes au VNL
	bool avantInitListe = false;
	if (m_nbBktVnlCC_A == 0 && m_nbBktVnlCC_B == 0)
	{
		avantInitListe = true;
	}
	// Détermination du nombre de CC selon le VNL
	// (exemple : si le VNL ne renvoie que les CC A0 à A6, m_nbBktVnlCC_A sera égal à 7)
	if (pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(0)->GetCmptId() == "A")
		m_nbBktVnlCC_A  = pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(0)->m_pVnlBktList.GetSize();
	if (pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(1)->GetCmptId() == "B")
		m_nbBktVnlCC_B  = pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(1)->m_pVnlBktList.GetSize();
	if (m_YaSci)
	{
		if (pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(0)->GetCmptId() == "A")
			m_nbBktVnlSC_A  = pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(0)->m_pVnlSciList.GetSize()+1;
		if (pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(1)->GetCmptId() == "B")
			m_nbBktVnlSC_B  = pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(1)->m_pVnlSciList.GetSize()+1;
	}
	
	// Dès qu'on dispose du premier VNL et donc de son nombre de buckets
	// on peut afficher la liste en la taillant comme il faut
	if (avantInitListe == true)
	{
		InitCtrlQuotasList();  // HRI, afficher les tableaux une fois terminé les 3 VNL
	}
	// Fin modif HRI du 10/08/2015
	//JMG DEBUG
	// pVnl.DummySci();
	// fin JMG DEBUG
	if (sLastTroncon == "")               // Si Troncon Peakleg vide
		sTronconVisu = pVnl.GetLegOrig(); // Affichage du premier troncon
	else 
		sTronconVisu = sLastTroncon;

	// HRI, 11/08/2015 : on teste plutôt la tranche internationale que le compteur
	//if (m_VnlCount==1) 
	if (pVnl.getTrancheNo() == pVnl.GetNatTranche(YM_Vnl::SN_BI_INTER)) // HRI, 10/08/2015
	{
		AnalyseVnlSI(sTronconVisu,pVnl);
		((CCTAApp*)APP)->GetGrpTranche(m_trancheno1,m_lDateDep,sTronconVisu,iGrpA,iGrpB);
		m_SI_AGrp = iGrpA;
		m_SI_BGrp = iGrpB;
		m_VnlCount++;
		if (m_VnlCount <= 2)
			SendNextVnl();

	}

	// HRI, 11/08/2015 : on teste plutôt la tranche internationale que le compteur
	//if (m_VnlCount==2)
	if (pVnl.getTrancheNo() == pVnl.GetNatTranche(YM_Vnl::SN_BI_DOM)) // HRI, 10/08/2015
	{
		AnalyseVnlSD(sTronconVisu,pVnl);
		((CCTAApp*)APP)->GetGrpTranche(m_trancheno2,m_lDateDep,sTronconVisu,iGrpA,iGrpB);
		m_SD_AGrp = iGrpA;
		m_SD_BGrp = iGrpB;
		m_VnlCount++;
		if (m_VnlCount <= 2)
			SendNextVnl();
	}
	if (m_VnlCount > 2)
	{
		FinVnl();
		if (!m_YaSci)
			SansSci();
	}

}

void CQuotasBiTrancheDlg::clearListControl()
{
	bool is_inter = this->TronconInter();

	m_quotas_view_sd_cc_a._list_ctrl.clearData(is_inter);
	m_quotas_view_sd_scx_a._list_ctrl.clearData(is_inter);
	m_quotas_view_sd_cc_b._list_ctrl.clearData(is_inter);
	m_quotas_view_sd_scx_b._list_ctrl.clearData(is_inter);

	m_quotas_view_si_cc_a._list_ctrl.clearData(false);
	m_quotas_view_si_scx_a._list_ctrl.clearData(false);
	m_quotas_view_si_cc_b._list_ctrl.clearData(false);
	m_quotas_view_si_scx_b._list_ctrl.clearData(false);
	
	m_quotas_view_sd_cc_a._list_ctrl.RedrawItems(0, m_quotas_view_sd_cc_a._list_ctrl.MaxIndexation());
	m_quotas_view_sd_scx_a._list_ctrl.RedrawItems(0, m_quotas_view_sd_scx_a._list_ctrl.MaxIndexation());
	m_quotas_view_sd_cc_b._list_ctrl.RedrawItems(0, m_quotas_view_sd_cc_b._list_ctrl.MaxIndexation());
	m_quotas_view_sd_scx_b._list_ctrl.RedrawItems(0, m_quotas_view_sd_scx_b._list_ctrl.MaxIndexation());

	m_quotas_view_si_cc_a._list_ctrl.RedrawItems(0, m_quotas_view_si_cc_a._list_ctrl.MaxIndexation());
	m_quotas_view_si_scx_a._list_ctrl.RedrawItems(0, m_quotas_view_si_scx_a._list_ctrl.MaxIndexation());
	m_quotas_view_si_cc_b._list_ctrl.RedrawItems(0, m_quotas_view_si_cc_b._list_ctrl.MaxIndexation());
	m_quotas_view_si_scx_b._list_ctrl.RedrawItems(0, m_quotas_view_si_scx_b._list_ctrl.MaxIndexation());
}

void CQuotasBiTrancheDlg::FinVnl()
{
	//m_SI_ADIFF.SetTextColor(RGB(0,0,0));
	m_IDC_LABEL_SI_A_DIFF.SetTextColor(RGB(0,0,0)); //NCH
	//m_SI_BDIFF.SetTextColor(RGB(0,0,0));
	m_IDC_LABEL_SI_B_DIFF.SetTextColor(RGB(0,0,0)); //NCH
	//m_SD_ADIFF.SetTextColor(RGB(0,0,0));
	m_IDC_LABEL_SD_A_DIFF.SetTextColor(RGB(0,0,0)); //NCH
	//m_SD_BDIFF.SetTextColor(RGB(0,0,0));
	m_IDC_LABEL_SD_B_DIFF.SetTextColor(RGB(0,0,0)); //NCH
	RecapRemplissage();
	GrpRemplissage();
	BlancFondStatic();
	RecapDispoVert();
	SciFondStatic();
	m_bFlagOkGlobal = TRUE;
	m_bYaSousResa = FALSE;
	TabTroncon_A_Total();
	TabTroncon_B_Total();
	TabTroncon_Glob_Total();
	TabTronconHide();

	// On a pu trouver des CC ou SC domestique troncon internationaux
	// avec des autorisations > 0, ce qui va donner lieu à des VNAU/VNAS
	// On passe les méthodes ci-dessous
	// HRI, 11/08/2015
	GrisageVnauVnas();
	VerifFlagTroncon();
	GrisageTroncon();

	/// DM-7978 - CLE
	SwitchA();
	/// DM-7978 

	//if ((SumBkt0A == SumCapA) && (SumBkt0B == SumCapB))
	if (m_bSNDetranger)
	{
		AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_SND_ETRANGER),
			AfxGetAppName(), MB_ICONINFORMATION | MB_APPLMODAL);
	}
	else  // HRI, 11/08/2015 : on remet le test sur les SumBkt == SumCap
	if (m_bFlagOkGlobal && (SumBkt0A == SumCapA) && (SumBkt0B == SumCapB))
	{
		int nid = 0;

		if (m_bYaSousResa)
			nid = IDS_YA_SOUSRESA;
		else 
			nid = IDS_QUOTA_OK;

		AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(nid),
			AfxGetAppName(), MB_ICONINFORMATION | MB_APPLMODAL);
		((CCTAApp*)APP)->UpdRhealysCriticalFlag (FALSE, m_trancheno1, m_lDateDep);
		((CCTAApp*)APP)->UpdRhealysCriticalFlag (FALSE, m_trancheno2, m_lDateDep);
		CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
		pTrancheView->UpdRhealysCriticalFlag (FALSE, m_NumeroTrain, m_lDateDep);
	}
	else if (!m_bFlagOkGlobal)
	{
		AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TROP_SURRESA), AfxGetAppName(), 
		MB_ICONSTOP | MB_APPLMODAL);
	}
	if (bVnlDeControle)
	{
		FinSendVnau();
		bVnlDeControle = FALSE;
	}
}

void CQuotasBiTrancheDlg::TabTronconSIA(int icount,const CString& sLegOrig, long lAuth, long lDisp)
{
	/// TODO
	
	CString sTemp;
	switch(icount)
	{
	case 1 :
		m_Troncon_Col_1.ShowWindow(SW_SHOW);
		m_SNI_A0_1.ShowWindow(SW_SHOW);
		m_SNI_DA0_1.ShowWindow(SW_SHOW);
		m_Troncon_Col_1.SetWindowText(sLegOrig); 
		sTemp.Format("%d",lAuth);
		m_SNI_A0_1.SetWindowText(sTemp);
		sTemp.Format("%d",lDisp);
		m_SNI_DA0_1.SetWindowText(sTemp);
		break;
	case 2 :
		m_Troncon_Col_2.ShowWindow(SW_SHOW);
		m_SNI_A0_2.ShowWindow(SW_SHOW);
		m_SNI_DA0_2.ShowWindow(SW_SHOW);
		m_Troncon_Col_2.SetWindowText(sLegOrig); 
		sTemp.Format("%d",lAuth);
		m_SNI_A0_2.SetWindowText(sTemp);
		sTemp.Format("%d",lDisp);
		m_SNI_DA0_2.SetWindowText(sTemp);
		break;
	case 3 :
		m_Troncon_Col_3.ShowWindow(SW_SHOW);
		m_SNI_A0_3.ShowWindow(SW_SHOW);
		m_SNI_DA0_3.ShowWindow(SW_SHOW);
		m_Troncon_Col_3.SetWindowText(sLegOrig); 
		sTemp.Format("%d",lAuth);
		m_SNI_A0_3.SetWindowText(sTemp);
		sTemp.Format("%d",lDisp);
		m_SNI_DA0_3.SetWindowText(sTemp);
		break;
	case 4 :
		m_Troncon_Col_4.ShowWindow(SW_SHOW);
		m_SNI_A0_4.ShowWindow(SW_SHOW);
		m_SNI_DA0_4.ShowWindow(SW_SHOW);
		m_Troncon_Col_4.SetWindowText(sLegOrig);
		sTemp.Format("%d",lAuth);
		m_SNI_A0_4.SetWindowText(sTemp);
		sTemp.Format("%d",lDisp);
		m_SNI_DA0_4.SetWindowText(sTemp);
		break;
	case 5 :
		m_Troncon_Col_5.ShowWindow(SW_SHOW);
		m_SNI_A0_5.ShowWindow(SW_SHOW);
		m_SNI_DA0_5.ShowWindow(SW_SHOW);
		m_Troncon_Col_5.SetWindowText(sLegOrig);
		sTemp.Format("%d",lAuth);
		m_SNI_A0_5.SetWindowText(sTemp);
		sTemp.Format("%d",lDisp);
		m_SNI_DA0_5.SetWindowText(sTemp);
		break;
	case 6 :
		m_Troncon_Col_6.ShowWindow(SW_SHOW);
		m_SNI_A0_6.ShowWindow(SW_SHOW);
		m_SNI_DA0_6.ShowWindow(SW_SHOW);
		m_Troncon_Col_6.SetWindowText(sLegOrig);
		sTemp.Format("%d",lAuth);
		m_SNI_A0_6.SetWindowText(sTemp);
		sTemp.Format("%d",lDisp);
		m_SNI_DA0_6.SetWindowText(sTemp);
		break;
	case 7 :
		m_Troncon_Col_7.ShowWindow(SW_SHOW);
		m_SNI_A0_7.ShowWindow(SW_SHOW);
		m_SNI_DA0_7.ShowWindow(SW_SHOW);
		m_Troncon_Col_7.SetWindowText(sLegOrig); 
		sTemp.Format("%d",lAuth);
		m_SNI_A0_7.SetWindowText(sTemp);
		sTemp.Format("%d",lDisp);
		m_SNI_DA0_7.SetWindowText(sTemp);
		break;
	case 8 :
		m_Troncon_Col_8.ShowWindow(SW_SHOW);
		m_SNI_A0_8.ShowWindow(SW_SHOW);
		m_SNI_DA0_8.ShowWindow(SW_SHOW);
		m_Troncon_Col_8.SetWindowText(sLegOrig); 
		sTemp.Format("%d",lAuth);
		m_SNI_A0_8.SetWindowText(sTemp);
		sTemp.Format("%d",lDisp);
		m_SNI_DA0_8.SetWindowText(sTemp);
		break;
	case 9 :
		m_Troncon_Col_9.ShowWindow(SW_SHOW);
		m_SNI_A0_9.ShowWindow(SW_SHOW);
		m_SNI_DA0_9.ShowWindow(SW_SHOW);
		m_Troncon_Col_9.SetWindowText(sLegOrig); 
		sTemp.Format("%d",lAuth);
		m_SNI_A0_9.SetWindowText(sTemp);
		sTemp.Format("%d",lDisp);
		m_SNI_DA0_9.SetWindowText(sTemp);
		break;
	case 10 :
		m_Troncon_Col_10.ShowWindow(SW_SHOW);
		m_SNI_A0_10.ShowWindow(SW_SHOW);
		m_SNI_DA0_10.ShowWindow(SW_SHOW);
		m_Troncon_Col_10.SetWindowText(sLegOrig); 
		sTemp.Format("%d",lAuth);
		m_SNI_A0_10.SetWindowText(sTemp);
		sTemp.Format("%d",lDisp);
		m_SNI_DA0_10.SetWindowText(sTemp);
		break;
	case 11 :
		m_Troncon_Col_11.ShowWindow(SW_SHOW);
		m_SNI_A0_11.ShowWindow(SW_SHOW);
		m_SNI_DA0_11.ShowWindow(SW_SHOW);
		m_Troncon_Col_11.SetWindowText(sLegOrig); 
		sTemp.Format("%d",lAuth);
		m_SNI_A0_11.SetWindowText(sTemp);
		sTemp.Format("%d",lDisp);
		m_SNI_DA0_11.SetWindowText(sTemp);
		break;
	case 12 :
		m_Troncon_Col_12.ShowWindow(SW_SHOW);
		m_SNI_A0_12.ShowWindow(SW_SHOW);
		m_SNI_DA0_12.ShowWindow(SW_SHOW);
		m_Troncon_Col_12.SetWindowText(sLegOrig); 
		sTemp.Format("%d",lAuth);
		m_SNI_A0_12.SetWindowText(sTemp);
		sTemp.Format("%d",lDisp);
		m_SNI_DA0_12.SetWindowText(sTemp);
		break;
	}
	
}

void CQuotasBiTrancheDlg::TabTronconSIB(int icount, long lAuth, long lDisp)
{
	/// TODO

	CString sTemp;
	switch(icount)
	{
	case 1 :
		m_SNI_B0_1.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SNI_B0_1.SetWindowText(sTemp);
		m_SNI_DB0_1.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SNI_DB0_1.SetWindowText(sTemp);
		break;
	case 2 :
		m_SNI_B0_2.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SNI_B0_2.SetWindowText(sTemp);
		m_SNI_DB0_2.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SNI_DB0_2.SetWindowText(sTemp);
		break;
	case 3 :
		m_SNI_B0_3.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SNI_B0_3.SetWindowText(sTemp);
		m_SNI_DB0_3.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SNI_DB0_3.SetWindowText(sTemp);
		break;
	case 4 :
		m_SNI_B0_4.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SNI_B0_4.SetWindowText(sTemp);
		m_SNI_DB0_4.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SNI_DB0_4.SetWindowText(sTemp);
		break;
	case 5 :
		m_SNI_B0_5.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SNI_B0_5.SetWindowText(sTemp);
		m_SNI_DB0_5.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SNI_DB0_5.SetWindowText(sTemp);
		break;
	case 6 :
		m_SNI_B0_6.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SNI_B0_6.SetWindowText(sTemp);
		m_SNI_DB0_6.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SNI_DB0_6.SetWindowText(sTemp);
		break;
	case 7 :
		m_SNI_B0_7.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SNI_B0_7.SetWindowText(sTemp);
		m_SNI_DB0_7.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SNI_DB0_7.SetWindowText(sTemp);
		break;
	case 8 :
		m_SNI_B0_8.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SNI_B0_8.SetWindowText(sTemp);
		m_SNI_DB0_8.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SNI_DB0_8.SetWindowText(sTemp);
		break;
	case 9 :
		m_SNI_B0_9.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SNI_B0_9.SetWindowText(sTemp);
		m_SNI_DB0_9.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SNI_DB0_9.SetWindowText(sTemp);
		break;
	case 10 :
		m_SNI_B0_10.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SNI_B0_10.SetWindowText(sTemp);
		m_SNI_DB0_10.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SNI_DB0_10.SetWindowText(sTemp);
		break;
	case 11 :
		m_SNI_B0_11.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SNI_B0_11.SetWindowText(sTemp);
		m_SNI_DB0_11.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SNI_DB0_11.SetWindowText(sTemp);
		break;
	case 12 :
		m_SNI_B0_12.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SNI_B0_12.SetWindowText(sTemp);
		m_SNI_DB0_12.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SNI_DB0_12.SetWindowText(sTemp);
		break;
	}

}

void CQuotasBiTrancheDlg::TabTronconSDA(int icount, long lAuth, long lDisp)
{
	/// TODO
	CString sTemp;
	switch(icount)
	{
	case 1 :
		m_SND_A0_1.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_A0_1.SetWindowText(sTemp);
		m_SND_DA0_1.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DA0_1.SetWindowText(sTemp);
		break;
	case 2 :
		m_SND_A0_2.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_A0_2.SetWindowText(sTemp);
		m_SND_DA0_2.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DA0_2.SetWindowText(sTemp);
		break;
	case 3 :
		m_SND_A0_3.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_A0_3.SetWindowText(sTemp);
		m_SND_DA0_3.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DA0_3.SetWindowText(sTemp);
		break;
	case 4 :
		m_SND_A0_4.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_A0_4.SetWindowText(sTemp);
		m_SND_DA0_4.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DA0_4.SetWindowText(sTemp);
		break;
	case 5 :
		m_SND_A0_5.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_A0_5.SetWindowText(sTemp);
		m_SND_DA0_5.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DA0_5.SetWindowText(sTemp);
		break;
	case 6 :
		m_SND_A0_6.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_A0_6.SetWindowText(sTemp);
		m_SND_DA0_6.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DA0_6.SetWindowText(sTemp);
		break;
	case 7 :
		m_SND_A0_7.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_A0_7.SetWindowText(sTemp);
		m_SND_DA0_7.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DA0_7.SetWindowText(sTemp);
		break;
	case 8 :
		m_SND_A0_8.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_A0_8.SetWindowText(sTemp);
		m_SND_DA0_8.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DA0_8.SetWindowText(sTemp);
		break;
	case 9 :
		m_SND_A0_9.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_A0_9.SetWindowText(sTemp);
		m_SND_DA0_9.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DA0_9.SetWindowText(sTemp);
		break;
	case 10 :
		m_SND_A0_10.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_A0_10.SetWindowText(sTemp);
		m_SND_DA0_10.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DA0_10.SetWindowText(sTemp);
		break;
	case 11 :
		m_SND_A0_11.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_A0_11.SetWindowText(sTemp);
		m_SND_DA0_11.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DA0_11.SetWindowText(sTemp);
		break;
	case 12 :
		m_SND_A0_12.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_A0_12.SetWindowText(sTemp);
		m_SND_DA0_12.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DA0_12.SetWindowText(sTemp);
		break;
	}
}

void CQuotasBiTrancheDlg::TabTronconSDB(int icount, long lAuth, long lDisp)
{
	/// TODO
	CString sTemp;
	switch(icount)
	{
	case 1 :
		m_SND_B0_1.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_B0_1.SetWindowText(sTemp);
		m_SND_DB0_1.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DB0_1.SetWindowText(sTemp);
		break;
	case 2 :
		m_SND_B0_2.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_B0_2.SetWindowText(sTemp);
		m_SND_DB0_2.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DB0_2.SetWindowText(sTemp);
		break;
	case 3 :
		m_SND_B0_3.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_B0_3.SetWindowText(sTemp);
		m_SND_DB0_3.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DB0_3.SetWindowText(sTemp);
		break;
	case 4 :
		m_SND_B0_4.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_B0_4.SetWindowText(sTemp);
		m_SND_DB0_4.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DB0_4.SetWindowText(sTemp);
		break;
	case 5 :
		m_SND_B0_5.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_B0_5.SetWindowText(sTemp);
		m_SND_DB0_5.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DB0_5.SetWindowText(sTemp);
		break;
	case 6 :
		m_SND_B0_6.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_B0_6.SetWindowText(sTemp);
		m_SND_DB0_6.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DB0_6.SetWindowText(sTemp);
		break;
	case 7 :
		m_SND_B0_7.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_B0_7.SetWindowText(sTemp);
		m_SND_DB0_7.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DB0_7.SetWindowText(sTemp);
		break;
	case 8 :
		m_SND_B0_8.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_B0_8.SetWindowText(sTemp);
		m_SND_DB0_8.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DB0_8.SetWindowText(sTemp);
		break;
	case 9 :
		m_SND_B0_9.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_B0_9.SetWindowText(sTemp);
		m_SND_DB0_9.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DB0_9.SetWindowText(sTemp);
		break;
	case 10 :
		m_SND_B0_10.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_B0_10.SetWindowText(sTemp);
		m_SND_DB0_10.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DB0_10.SetWindowText(sTemp);
		break;
	case 11 :
		m_SND_B0_11.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_B0_11.SetWindowText(sTemp);
		m_SND_DB0_11.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DB0_11.SetWindowText(sTemp);
		break;
	case 12 :
		m_SND_B0_12.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_SND_B0_12.SetWindowText(sTemp);
		m_SND_DB0_12.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_SND_DB0_12.SetWindowText(sTemp);
		break;
	}
}

void CQuotasBiTrancheDlg::UnTotal (YM_Static &m_Total, int caparef, int maxref, int Som)
{
	char tot[6];
	if (Som == caparef)
		m_Total.SetTextColor(RGB(0,0,0));
	else if (Som < caparef)
	{
		m_Total.SetTextColor(RGB(255,0,0));
		m_bYaSousResa = TRUE;
	}
	else if (Som > caparef)
		m_Total.SetTextColor(COLOR_OVB_MTRANCHE);
	m_Total.ShowWindow(SW_SHOW);
	m_Total.SetWindowText(itoa(Som, tot, 10));
	if (Som > maxref)
		m_Total.SetBkColor(RGB(255,255, 128));
	else
		m_Total.SetBkColor (::GetSysColor (COLOR_WINDOW));
	m_Total.Invalidate();
}
void CQuotasBiTrancheDlg::TabTroncon_A_Total()
{
	int i;
	int iSom;
	CString sTemp, sMax;

	GetDlgItem(IDC_MAXA)->GetWindowText (sMax);
	int iMaxA = atoi (sMax);

	for (i=1;i<(m_iTroncon+1);i++)
		switch(i)
	{
		case 1 :
			m_SNI_A0_1.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_1.GetWindowText(sTemp);
			iSom += atoi(sTemp);   
			UnTotal (m_Total_A0_1, m_iCapaA, iMaxA, iSom);

			break;
		case 2 :
			m_SNI_A0_2.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_2.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_2, m_iCapaA, iMaxA, iSom);

			break;
		case 3 :
			m_SNI_A0_3.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_3.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_3, m_iCapaA, iMaxA, iSom);

			break;
		case 4 :
			m_SNI_A0_4.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_4.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_4, m_iCapaA, iMaxA, iSom);

			break;
		case 5 :
			m_SNI_A0_5.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_5.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_5, m_iCapaA, iMaxA, iSom);

			break;
		case 6 :
			m_SNI_A0_6.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_6.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_6, m_iCapaA, iMaxA, iSom);

			break;
		case 7 :
			m_SNI_A0_7.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_7.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_7, m_iCapaA, iMaxA, iSom);

			break;
		case 8 :
			m_SNI_A0_8.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_8.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_8, m_iCapaA, iMaxA, iSom);

			break;
		case 9 :
			m_SNI_A0_9.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_9.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_9, m_iCapaA, iMaxA, iSom);

			break;
		case 10 :
			m_SNI_A0_10.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_10.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_10, m_iCapaA, iMaxA, iSom);

			break;
		case 11 :
			m_SNI_A0_11.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_11.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_11, m_iCapaA, iMaxA, iSom);

			break;
		case 12 :
			m_SNI_A0_12.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_12.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_12, m_iCapaA, iMaxA, iSom);

			break;
	}

}

void CQuotasBiTrancheDlg::TabTroncon_B_Total()
{
	int i;
	int iSom;
	CString sTemp, sMax;

	GetDlgItem(IDC_MAXB)->GetWindowText (sMax);
	int iMaxB = atoi (sMax);

	for (i=1;i<(m_iTroncon+1);i++)
	{
		switch(i)
		{
		case 1 :
			m_SNI_B0_1.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_1.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_1, m_iCapaB, iMaxB, iSom);
			break;
		case 2 :
			m_SNI_B0_2.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_2.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_2, m_iCapaB, iMaxB, iSom);
			break;
		case 3 :
			m_SNI_B0_3.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_3.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_3, m_iCapaB, iMaxB, iSom);
			break;
		case 4 :
			m_SNI_B0_4.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_4.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_4, m_iCapaB, iMaxB, iSom);
			break;
		case 5 :
			m_SNI_B0_5.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_5.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_5, m_iCapaB, iMaxB, iSom);
			break;
		case 6 :
			m_SNI_B0_6.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_6.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_6, m_iCapaB, iMaxB, iSom);
			break;
		case 7 :
			m_SNI_B0_7.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_7.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_7, m_iCapaB, iMaxB, iSom);
			break;
		case 8 :
			m_SNI_B0_8.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_8.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_8, m_iCapaB, iMaxB, iSom);
			break;
		case 9 :
			m_SNI_B0_9.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_9.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_9, m_iCapaB, iMaxB, iSom);
			break;
		case 10 :
			m_SNI_B0_10.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_10.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_10, m_iCapaB, iMaxB, iSom);
			break;
		case 11 :
			m_SNI_B0_11.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_11.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_11, m_iCapaB, iMaxB, iSom);
			break;
		case 12 :
			m_SNI_B0_12.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_12.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_12, m_iCapaB, iMaxB, iSom);
			break;
		}
	}

}
void CQuotasBiTrancheDlg::GlobTotal (YM_Static &m_TotalA, YM_Static &m_TotalB)
{
	CString sTemp, sMax;
	int iTotA, iTotB;

	GetDlgItem(IDC_MAXTOT)->GetWindowText (sMax);
	int iMaxG = atoi (sMax);
	GetDlgItem(IDC_MAXA)->GetWindowText (sMax);
	int iMaxA = atoi (sMax);
	GetDlgItem(IDC_MAXB)->GetWindowText (sMax);
	int iMaxB = atoi (sMax);

	m_TotalA.GetWindowText(sTemp);
	iTotA = atoi(sTemp);
	m_TotalB.GetWindowText(sTemp);
	iTotB = atoi(sTemp);


	if (iTotA + iTotB > iMaxG)
	{
		m_bFlagOkGlobal = FALSE;
		if (iTotA > iMaxA)
		{
			m_TotalA.SetBkColor(RGB (255,0,0));
			m_TotalA.Invalidate();
		}
		if (iTotB > iMaxB)
		{
			m_TotalB.SetBkColor(RGB (255,0,0));
			m_TotalB.Invalidate();
		}

	}
}

void CQuotasBiTrancheDlg::TabTroncon_Glob_Total()
{
	int i;
	CString sTemp, sMax;

	for (i=1;i<(m_iTroncon+1);i++)
	{
		switch(i)
		{
		case 1 :
			GlobTotal (m_Total_A0_1, m_Total_B0_1);
			break;
		case 2 :
			GlobTotal (m_Total_A0_2, m_Total_B0_2);
			break;
		case 3 :
			GlobTotal (m_Total_A0_3, m_Total_B0_3);
			break;
		case 4 :
			GlobTotal (m_Total_A0_4, m_Total_B0_4);
			break;
		case 5 :
			GlobTotal (m_Total_A0_5, m_Total_B0_5);
			break;
		case 6 :
			GlobTotal (m_Total_A0_6, m_Total_B0_6);
			break;
		case 7 :
			GlobTotal (m_Total_A0_7, m_Total_B0_7);
			break;
		case 8 :
			GlobTotal (m_Total_A0_8, m_Total_B0_8);
			break;
		case 9 :
			GlobTotal (m_Total_A0_9, m_Total_B0_9);
			break;
		case 10 :
			GlobTotal (m_Total_A0_10, m_Total_B0_10);
			break;
		case 11 :
			GlobTotal (m_Total_A0_11, m_Total_B0_11);
			break;
		case 12 :
			GlobTotal (m_Total_A0_12, m_Total_B0_12);
			break;
		}
	}
}

void CQuotasBiTrancheDlg::TabTronconHide()
{
	int i;
	for (i=m_iTroncon+1;i<15;i++)
	{
		switch(i)
		{
		case 1 :
			m_Troncon_Col_1.ShowWindow(SW_HIDE);
			m_SNI_A0_1.ShowWindow(SW_HIDE);
			m_SND_A0_1.ShowWindow(SW_HIDE);
			m_Total_A0_1.ShowWindow(SW_HIDE);
			m_SNI_B0_1.ShowWindow(SW_HIDE);
			m_SND_B0_1.ShowWindow(SW_HIDE);
			m_Total_B0_1.ShowWindow(SW_HIDE);
			m_SNI_DA0_1.ShowWindow(SW_HIDE);
			m_SND_DA0_1.ShowWindow(SW_HIDE);
			m_SNI_DB0_1.ShowWindow(SW_HIDE);
			m_SND_DB0_1.ShowWindow(SW_HIDE);
			break;
		case 2 :
			m_Troncon_Col_2.ShowWindow(SW_HIDE);
			m_SNI_A0_2.ShowWindow(SW_HIDE);
			m_SND_A0_2.ShowWindow(SW_HIDE);
			m_Total_A0_2.ShowWindow(SW_HIDE);
			m_SNI_B0_2.ShowWindow(SW_HIDE);
			m_SND_B0_2.ShowWindow(SW_HIDE);
			m_Total_B0_2.ShowWindow(SW_HIDE);
			m_SNI_DA0_2.ShowWindow(SW_HIDE);
			m_SND_DA0_2.ShowWindow(SW_HIDE);
			m_SNI_DB0_2.ShowWindow(SW_HIDE);
			m_SND_DB0_2.ShowWindow(SW_HIDE);
			break;
		case 3 :
			m_Troncon_Col_3.ShowWindow(SW_HIDE);
			m_SNI_A0_3.ShowWindow(SW_HIDE);
			m_SND_A0_3.ShowWindow(SW_HIDE);
			m_Total_A0_3.ShowWindow(SW_HIDE);
			m_SNI_B0_3.ShowWindow(SW_HIDE);
			m_SND_B0_3.ShowWindow(SW_HIDE);
			m_Total_B0_3.ShowWindow(SW_HIDE);
			m_SNI_DA0_3.ShowWindow(SW_HIDE);
			m_SND_DA0_3.ShowWindow(SW_HIDE);
			m_SNI_DB0_3.ShowWindow(SW_HIDE);
			m_SND_DB0_3.ShowWindow(SW_HIDE);
			break;
		case 4 :
			m_Troncon_Col_4.ShowWindow(SW_HIDE);
			m_SNI_A0_4.ShowWindow(SW_HIDE);
			m_SND_A0_4.ShowWindow(SW_HIDE);
			m_Total_A0_4.ShowWindow(SW_HIDE);
			m_SNI_B0_4.ShowWindow(SW_HIDE);
			m_SND_B0_4.ShowWindow(SW_HIDE);
			m_Total_B0_4.ShowWindow(SW_HIDE);
			m_SNI_DA0_4.ShowWindow(SW_HIDE);
			m_SND_DA0_4.ShowWindow(SW_HIDE);
			m_SNI_DB0_4.ShowWindow(SW_HIDE);
			m_SND_DB0_4.ShowWindow(SW_HIDE);
			break;
		case 5 :
			m_Troncon_Col_5.ShowWindow(SW_HIDE);
			m_SNI_A0_5.ShowWindow(SW_HIDE);
			m_SND_A0_5.ShowWindow(SW_HIDE);
			m_Total_A0_5.ShowWindow(SW_HIDE);
			m_SNI_B0_5.ShowWindow(SW_HIDE);
			m_SND_B0_5.ShowWindow(SW_HIDE);
			m_Total_B0_5.ShowWindow(SW_HIDE);
			m_SNI_DA0_5.ShowWindow(SW_HIDE);
			m_SND_DA0_5.ShowWindow(SW_HIDE);
			m_SNI_DB0_5.ShowWindow(SW_HIDE);
			m_SND_DB0_5.ShowWindow(SW_HIDE);
			break;
		case 6 :
			m_Troncon_Col_6.ShowWindow(SW_HIDE);
			m_SNI_A0_6.ShowWindow(SW_HIDE);
			m_SND_A0_6.ShowWindow(SW_HIDE);
			m_Total_A0_6.ShowWindow(SW_HIDE);
			m_SNI_B0_6.ShowWindow(SW_HIDE);
			m_SND_B0_6.ShowWindow(SW_HIDE);
			m_Total_B0_6.ShowWindow(SW_HIDE);
			m_SNI_DA0_6.ShowWindow(SW_HIDE);
			m_SND_DA0_6.ShowWindow(SW_HIDE);
			m_SNI_DB0_6.ShowWindow(SW_HIDE);
			m_SND_DB0_6.ShowWindow(SW_HIDE);
			break;
		case 7 :
			m_Troncon_Col_7.ShowWindow(SW_HIDE);
			m_SNI_A0_7.ShowWindow(SW_HIDE);
			m_SND_A0_7.ShowWindow(SW_HIDE);
			m_Total_A0_7.ShowWindow(SW_HIDE);
			m_SNI_B0_7.ShowWindow(SW_HIDE);
			m_SND_B0_7.ShowWindow(SW_HIDE);
			m_Total_B0_7.ShowWindow(SW_HIDE);
			m_SNI_DA0_7.ShowWindow(SW_HIDE);
			m_SND_DA0_7.ShowWindow(SW_HIDE);
			m_SNI_DB0_7.ShowWindow(SW_HIDE);
			m_SND_DB0_7.ShowWindow(SW_HIDE);
			break;
		case 8 :
			m_Troncon_Col_8.ShowWindow(SW_HIDE);
			m_SNI_A0_8.ShowWindow(SW_HIDE);
			m_SND_A0_8.ShowWindow(SW_HIDE);
			m_Total_A0_8.ShowWindow(SW_HIDE);
			m_SNI_B0_8.ShowWindow(SW_HIDE);
			m_SND_B0_8.ShowWindow(SW_HIDE);
			m_Total_B0_8.ShowWindow(SW_HIDE);
			m_SNI_DA0_8.ShowWindow(SW_HIDE);
			m_SND_DA0_8.ShowWindow(SW_HIDE);
			m_SNI_DB0_8.ShowWindow(SW_HIDE);
			m_SND_DB0_8.ShowWindow(SW_HIDE);
			break;
		case 9 :
			m_Troncon_Col_9.ShowWindow(SW_HIDE);
			m_SNI_A0_9.ShowWindow(SW_HIDE);
			m_SND_A0_9.ShowWindow(SW_HIDE);
			m_Total_A0_9.ShowWindow(SW_HIDE);
			m_SNI_B0_9.ShowWindow(SW_HIDE);
			m_SND_B0_9.ShowWindow(SW_HIDE);
			m_Total_B0_9.ShowWindow(SW_HIDE);
			m_SNI_DA0_9.ShowWindow(SW_HIDE);
			m_SND_DA0_9.ShowWindow(SW_HIDE);
			m_SNI_DB0_9.ShowWindow(SW_HIDE);
			m_SND_DB0_9.ShowWindow(SW_HIDE);
			break;
		case 10 :
			m_Troncon_Col_10.ShowWindow(SW_HIDE);
			m_SNI_A0_10.ShowWindow(SW_HIDE);
			m_SND_A0_10.ShowWindow(SW_HIDE);
			m_Total_A0_10.ShowWindow(SW_HIDE);
			m_SNI_B0_10.ShowWindow(SW_HIDE);
			m_SND_B0_10.ShowWindow(SW_HIDE);
			m_Total_B0_10.ShowWindow(SW_HIDE);
			m_SNI_DA0_10.ShowWindow(SW_HIDE);
			m_SND_DA0_10.ShowWindow(SW_HIDE);
			m_SNI_DB0_10.ShowWindow(SW_HIDE);
			m_SND_DB0_10.ShowWindow(SW_HIDE);
			break;
		case 11 :
			m_Troncon_Col_11.ShowWindow(SW_HIDE);
			m_SNI_A0_11.ShowWindow(SW_HIDE);
			m_SND_A0_11.ShowWindow(SW_HIDE);
			m_Total_A0_11.ShowWindow(SW_HIDE);
			m_SNI_B0_11.ShowWindow(SW_HIDE);
			m_SND_B0_11.ShowWindow(SW_HIDE);
			m_Total_B0_11.ShowWindow(SW_HIDE);
			m_SNI_DA0_11.ShowWindow(SW_HIDE);
			m_SND_DA0_11.ShowWindow(SW_HIDE);
			m_SNI_DB0_11.ShowWindow(SW_HIDE);
			m_SND_DB0_11.ShowWindow(SW_HIDE);
			break;
		case 12 :
			m_Troncon_Col_12.ShowWindow(SW_HIDE);
			m_SNI_A0_12.ShowWindow(SW_HIDE);
			m_SND_A0_12.ShowWindow(SW_HIDE);
			m_Total_A0_12.ShowWindow(SW_HIDE);
			m_SNI_B0_12.ShowWindow(SW_HIDE);
			m_SND_B0_12.ShowWindow(SW_HIDE);
			m_Total_B0_12.ShowWindow(SW_HIDE);
			m_SNI_DA0_12.ShowWindow(SW_HIDE);
			m_SND_DA0_12.ShowWindow(SW_HIDE);
			m_SNI_DB0_12.ShowWindow(SW_HIDE);
			m_SND_DB0_12.ShowWindow(SW_HIDE);
			break;
		}
	}
}

BOOL CQuotasBiTrancheDlg::GetTrancheNo (CString msg)
{
	CString line1,sTitre;
	//  CString sVisu = "";
	char    pstline[128];
	char    *pszt;
	int nb = 1;
	m_nbTrancheOK = 0;
	m_trancheno1  = 0;
	m_trancheno2  = 0;
	// traitement du retour de la commande KDISN
	// On elimine les deux premières lignes
	line1 = GetFirstLine (msg);
	line1 = GetFirstLine (msg);
	while (!msg.IsEmpty())
	{
		line1 = GetFirstLine (msg);	  
		strcpy (pstline, (LPCSTR)line1);
		pszt = strtok(pstline, " \t");	// leg orig
		pszt = strtok(NULL, " \t");		// leg dest
		pszt = strtok(NULL, " \t");		// -
		while (pszt)
		{
			pszt = strtok(NULL, " \t");		// SNxxxxxx			: xxxxxx est le num tranche
			if (pszt != NULL)
			{
				CString st = pszt;
				st = st.Right (st.GetLength() - 2);
				if (nb == 1)
					m_trancheno1 = atoi (st);
				else if (nb == 2)
				{
					m_trancheno2 = atoi (st);
					m_nbTrancheOK = 2;
				}
				nb++;
			}
		}	
	}

	if (m_nbTrancheOK != 2)
	{
		AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TRAIN_NON_BITRANCHE), AfxGetAppName(), 
			MB_ICONERROR | MB_APPLMODAL);
		return FALSE;
	}
	else 
	{ 
		if (m_lRrdIndex < 0)
			m_lRrdIndex = ((CCTAApp*)APP)->GetLastRrdIndexOnLastBatch (m_trancheno1, m_lDateDep);
		
		
		sTitre.Format("SNCF International  -  %d",m_trancheno1);
		m_SI.SetWindowText(sTitre);
		sTitre.Format("SNCF France  -  %d",m_trancheno2);
		m_SD.SetWindowText(sTitre);
		StartSendVnl(m_sPeakLeg); // Affichage du troncon Peakleg
		return TRUE;
	}
}

// récupération des numéro de tranches en base de données.
// badtrain revient à true si train trouvé, mais pas bi-tranche
BOOL CQuotasBiTrancheDlg::LireTrancheNo (bool &badtrain) 
{
	CString sTitre;
	YmIcTrancheLisDom TrancheDom;
	TrancheDom.TrainNo (m_NumeroTrain);
	TrancheDom.DptDate (m_lDateDep);
	int nb = 0;
	int nl = 0;
	badtrain = false; /* epic fail */
	m_nbTrancheOK = 0;
	m_trancheno1  = 0;
	m_trancheno2  = 0;
	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
	pSQL->SetDomain(&TrancheDom);
	RWDBReader Reader( WinApp->m_pDatabase->Transact(pSQL, IDS_SQL_GET_MULTI_TRANCHE_NO) );
	while (Reader())
	{
		if (Reader.isValid())
		{
			Reader >> TrancheDom;
			nl++;
			if (TrancheDom.Nature() == 4)
			{
				m_trancheno1 = TrancheDom.TrancheNo();
				nb++;
			}
			else if (TrancheDom.Nature() == 5)
			{
				m_trancheno2 = TrancheDom.TrancheNo();
				nb++;
			}
		}
	}
	delete pSQL;
	if (nb == 2)
	{
		m_nbTrancheOK = 2;
		if (m_lRrdIndex < 0)
			m_lRrdIndex = ((CCTAApp*)APP)->GetLastRrdIndexOnLastBatch (m_trancheno1, m_lDateDep);
		sTitre.Format("SNCF International  -  %d",m_trancheno1);
		m_SI.SetWindowText(sTitre);
		sTitre.Format("SNCF France  -  %d",m_trancheno2);
		m_SD.SetWindowText(sTitre);
		StartSendVnl(m_sPeakLeg); // Affichage du troncon Peakleg

		return TRUE;
	}
	else
	{
		if (nl)
		{ // on a trouvé le train, mais ce n'est pas un bi-tranche
			AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TRAIN_NON_BITRANCHE), AfxGetAppName(), 
				MB_ICONERROR | MB_APPLMODAL);
			badtrain = true;
		}
		return FALSE;
	}
}

CString CQuotasBiTrancheDlg::MakeStrDate (RW4Byte date, BOOL bannee /*= FALSE*/)
{
	RWDate dada;
	CString sd, sa;
	dada.julian(date);

	sd.Format ( "%2.2u", dada.dayOfMonth());
	sd += MONTH_LIST_ENGLISH.Mid( (dada.month()-1)*3, 3 );
	if (bannee)
	{
		sa.Format ( "%2.2u",dada.year() % 100);
		sd += sa;
	}
	return sd;
}

void CQuotasBiTrancheDlg::OnNotify( YM_Observable* pObject )
{
	if( pObject->IsKindOf( RUNTIME_CLASS(YM_Msg) ) )
	{
		YM_Msg* pMsg = (YM_Msg*)pObject;
		if (pMsg->GetType() == YM_Msg::SOC_REP_ERROR)
		{
			// NBN - ANO 98856
			CString leVnl;
			leVnl.Format ("%s", pMsg->GetMsg());

			int PosFound = leVnl.Find("SC01", 100);
			CString serr = leVnl.Left((PosFound > -1 && ((CCTAApp*)APP)->GetRhealysFlag()) ? PosFound : leVnl.GetLength());
			//

			AfxGetMainWnd()->MessageBox(serr, AfxGetAppName(),MB_ICONERROR | MB_APPLMODAL);
			if ((pMsg->GetNat() == YM_Msg::MSG_DISPO) ||
				(pMsg->GetNat() == YM_Msg::MSG_VNL))
				OnCancel();
			else
				SendNextVnau();  // On envoi quand même les VNAU suivants
		} 
		else if (pMsg->GetNat() == YM_Msg::MSG_DISPO)
			GetTrancheNo (pMsg->GetMsg());
		else if (pMsg->GetNat() == YM_Msg::MSG_VNL)
			LireVnl(pMsg->GetMsg());
		else
			SendNextVnau();
	}
}


HBRUSH CQuotasBiTrancheDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
/*
	if (( nCtlColor == CTLCOLOR_EDIT) &&
		(pWnd == &m_SI_AMOI_0 || pWnd == &m_SI_AMOI_1 || pWnd == &m_SI_AMOI_2 ||
		pWnd == &m_SI_AMOI_3 || pWnd == &m_SI_AMOI_4 || pWnd == &m_SI_AMOI_5 ||
		pWnd == &m_SI_AMOI_6 || pWnd == &m_SI_BMOI_0 || pWnd == &m_SI_BMOI_1 ||
		pWnd == &m_SI_BMOI_2 || pWnd == &m_SI_BMOI_3 || pWnd == &m_SI_BMOI_4 ||
		pWnd == &m_SI_BMOI_5 || pWnd == &m_SI_BMOI_6 || pWnd == &m_SI_BMOI_7 ||
		pWnd == &m_SD_AMOI_0 || pWnd == &m_SD_AMOI_1 || pWnd == &m_SD_AMOI_2 ||
		pWnd == &m_SD_AMOI_3 || pWnd == &m_SD_AMOI_4 || pWnd == &m_SD_AMOI_5 ||
		pWnd == &m_SD_AMOI_6 || pWnd == &m_SD_BMOI_0 || pWnd == &m_SD_BMOI_1 ||
		pWnd == &m_SD_BMOI_2 || pWnd == &m_SD_BMOI_3 || pWnd == &m_SD_BMOI_4 ||
		pWnd == &m_SD_BMOI_5 || pWnd == &m_SD_BMOI_6 || pWnd == &m_SD_BMOI_7 ))
	{
		pDC->SetTextColor( RGB( 0, 0, 255 ) );
	}
	else if (( nCtlColor == CTLCOLOR_EDIT) &&
		( pWnd == &m_SCI_AMOI_G || pWnd == &m_SCI_AMOI_1 || pWnd == &m_SCI_AMOI_2 ||
		pWnd == &m_SCI_AMOI_3 || pWnd == &m_SCI_AMOI_4 || pWnd == &m_SCI_AMOI_5 ||
		pWnd == &m_SCI_AMOI_6 || pWnd == &m_SCI_BMOI_G || pWnd == &m_SCI_BMOI_1 ||
		pWnd == &m_SCI_BMOI_2 || pWnd == &m_SCI_BMOI_3 || pWnd == &m_SCI_BMOI_4 ||
		pWnd == &m_SCI_BMOI_5 || pWnd == &m_SCI_BMOI_6 || pWnd == &m_SCI_BMOI_7 ||
		pWnd == &m_SCD_AMOI_G || pWnd == &m_SCD_AMOI_1 || pWnd == &m_SCD_AMOI_2 ||
		pWnd == &m_SCD_AMOI_3 || pWnd == &m_SCD_AMOI_4 || pWnd == &m_SCD_AMOI_5 ||
		pWnd == &m_SCD_AMOI_6 || pWnd == &m_SCD_BMOI_G || pWnd == &m_SCD_BMOI_1 ||
		pWnd == &m_SCD_BMOI_2 || pWnd == &m_SCD_BMOI_3 || pWnd == &m_SCD_BMOI_4 ||
		pWnd == &m_SCD_BMOI_5 || pWnd == &m_SCD_BMOI_6 || pWnd == &m_SCD_BMOI_7 ||
		pWnd == &m_SCI_AMOI_7 || pWnd == &m_SCD_AMOI_7 
		/*||
		pWnd == &m_IDC_LABEL_SI_A_TOTAL_MOI || pWnd == &m_IDC_LABEL_SI_B_TOTAL_MOI ||
		pWnd == &m_IDC_LABEL_SD_A_TOTAL_MOI || pWnd == &m_IDC_LABEL_SD_B_TOTAL_MOI
		*/
	/*
		))
	{
		COLORREF clrBkAlt = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt();
		pDC->SetBkColor (clrBkAlt);
		pDC->SetBkMode(TRANSPARENT);
		hbr = *m_pAltBrush;
		pDC->SetTextColor( RGB( 0, 0, 255 ) );
	}
	else if (( nCtlColor == CTLCOLOR_STATIC ) &&
		(pWnd == &m_SCI_ADISPO_1 || pWnd == &m_SCI_ADISPO_2 ||
		pWnd == &m_SCI_ADISPO_3 || pWnd == &m_SCI_ADISPO_4 || pWnd == &m_SCI_ADISPO_5 ||
		pWnd == &m_SCI_ADISPO_6 || pWnd == &m_SCI_BDISPO_1 ||
		pWnd == &m_SCI_BDISPO_2 || pWnd == &m_SCI_BDISPO_3 || pWnd == &m_SCI_BDISPO_4 ||
		pWnd == &m_SCI_BDISPO_5 || pWnd == &m_SCI_BDISPO_6 || pWnd == &m_SCI_BDISPO_7 ||
		pWnd == &m_SCD_ADISPO_1 || pWnd == &m_SCD_ADISPO_2 ||
		pWnd == &m_SCD_ADISPO_3 || pWnd == &m_SCD_ADISPO_4 || pWnd == &m_SCD_ADISPO_5 ||
		pWnd == &m_SCD_ADISPO_6 || pWnd == &m_SCD_BDISPO_1 ||
		pWnd == &m_SCD_BDISPO_2 || pWnd == &m_SCD_BDISPO_3 || pWnd == &m_SCD_BDISPO_4 ||
		pWnd == &m_SCD_BDISPO_5 || pWnd == &m_SCD_BDISPO_6 || pWnd == &m_SCD_BDISPO_7 ||
		pWnd == &m_SCI_ADISPO_7 || pWnd == &m_SCD_ADISPO_7))
	{
		COLORREF clrBkAlt = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt();
		pDC->SetBkColor (clrBkAlt);
		pDC->SetBkMode(TRANSPARENT);
		hbr = *m_pAltBrush;
	}
	else if (( nCtlColor == CTLCOLOR_STATIC ) &&
		(pWnd == &m_SI_ADISPO_0 || pWnd == &m_SI_ADISPO_1 || pWnd == &m_SI_ADISPO_2 ||
		pWnd == &m_SI_ADISPO_3 || pWnd == &m_SI_ADISPO_4 || pWnd == &m_SI_ADISPO_5 ||
		pWnd == &m_SI_ADISPO_6 || pWnd == &m_SI_BDISPO_0 || pWnd == &m_SI_BDISPO_1 ||
		pWnd == &m_SI_BDISPO_2 || pWnd == &m_SI_BDISPO_3 || pWnd == &m_SI_BDISPO_4 ||
		pWnd == &m_SI_BDISPO_5 || pWnd == &m_SI_BDISPO_6 || pWnd == &m_SI_BDISPO_7 ||
		pWnd == &m_SD_ADISPO_0 || pWnd == &m_SD_ADISPO_1 || pWnd == &m_SD_ADISPO_2 ||
		pWnd == &m_SD_ADISPO_3 || pWnd == &m_SD_ADISPO_4 || pWnd == &m_SD_ADISPO_5 ||
		pWnd == &m_SD_ADISPO_6 || pWnd == &m_SD_BDISPO_0 || pWnd == &m_SD_BDISPO_1 ||
		pWnd == &m_SD_BDISPO_2 || pWnd == &m_SD_BDISPO_3 || pWnd == &m_SD_BDISPO_4 ||
		pWnd == &m_SD_BDISPO_5 || pWnd == &m_SD_BDISPO_6 || pWnd == &m_SD_BDISPO_7 ))

	{
		pDC->SetBkColor (RGB(255,255,255));
		pDC->SetBkMode(TRANSPARENT);
		hbr = *m_pStdBrush;
	}*/
	return hbr;
}

void CQuotasBiTrancheDlg::OnOK()
{
	CWnd* pWnd = GetFocus();
	pWnd->PostMessage(WM_KEYDOWN, 0x09, 0);
	//CDialog::OnOK();
}

// Methode de mise à jour des valeurs de bucket suivantes
// SI Cmpt A

/*
void CQuotasBiTrancheDlg::OnMajSIA0()
{
	//  calcul de l'écart
	CString sACT,sMOI,sDiff,sRecap,sMOImin;
	int iRecap;
	m_SI_AACT_0.GetWindowText(sACT);
	m_SI_AMOI_0.GetWindowText(sMOI);
	m_IDC_LABEL_SI_A_TOTAL_MOI.GetWindowText(sMOImin); // NCH
	m_SCI_AMOI_G.GetWindowText(sMOImin);
	if (atoi(sMOI) < atoi(sMOImin))
	{
		sMOI = sMOImin;
		m_SI_AMOI_0.SetWindowText(sMOImin);
		AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_REDUCE_CCM1), 
			MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION );
	}
	m_iDiff = atoi(sACT) - atoi(sMOI);
	sDiff.Format("%d",(-m_iDiff));
	m_SI_ADIFF.SetWindowText(sDiff);
	m_IDC_LABEL_SI_A_DIFF.SetWindowText(sDiff); //NCH
	if (m_iDiff == 0)
	{
		m_SI_ADIFF.SetTextColor(RGB(0,0,0));
		m_IDC_LABEL_SI_A_DIFF.SetTextColor(RGB(0,0,0)); // NCH
	}
	else if (m_iDiff > 0)
	{
		m_SI_ADIFF.SetTextColor(RGB(255,0,0));
		m_IDC_LABEL_SI_A_DIFF.SetTextColor(RGB(255,0,0)); // NCH
	}
	else if (m_iDiff < 0)
	{
		m_SI_ADIFF.SetTextColor(COLOR_OVB_MTRANCHE);
		m_IDC_LABEL_SI_A_DIFF.SetTextColor(COLOR_OVB_MTRANCHE); // NCH
	}
	//  Mise à jour de la colonne MOI de RECAP
	iRecap = atoi(sMOI);
	m_SD_AMOI_0.GetWindowText(sRecap);
	iRecap += atoi(sRecap);
	sRecap.Format("%d",iRecap);
	m_iCapaMoi_A.SetWindowText(sRecap);

	m_iCapaMoi_B.GetWindowText(sRecap);
	iRecap += atoi(sRecap);
	sRecap.Format("%d",iRecap);
	m_iCapaMoi_TOT.SetWindowText(sRecap);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	VerifFlagTroncon();
	GrisageTroncon();
	bBucket = TRUE;
	OnMajSIA1();
	
}
void CQuotasBiTrancheDlg::OnMajSIA1()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SI_AACT_1.GetWindowText(sACT);
	m_SI_AMOI_1.GetWindowText(sMOI);
	m_SI_AMOI_0.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SI_AMOI_1.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		if (m_iDiff != 0)
			m_SI_AMOI_1.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? "0" : sDiff));
		else
			m_SI_AMOI_1.SetWindowText(sACT);
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SI_AMOI_1.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSIA2();
}
void CQuotasBiTrancheDlg::OnMajSIA2()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SI_AACT_2.GetWindowText(sACT);
	m_SI_AMOI_2.GetWindowText(sMOI);
	m_SI_AMOI_1.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SI_AMOI_2.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		if (m_iDiff != 0)
			m_SI_AMOI_2.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? "0" : sDiff));
		else
			m_SI_AMOI_2.SetWindowText(sACT);
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SI_AMOI_2.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSIA3();
}
void CQuotasBiTrancheDlg::OnMajSIA3()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SI_AACT_3.GetWindowText(sACT);
	m_SI_AMOI_3.GetWindowText(sMOI);
	m_SI_AMOI_2.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SI_AMOI_3.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SI_AMOI_3.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? "0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SI_AMOI_3.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSIA4();
}
void CQuotasBiTrancheDlg::OnMajSIA4()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SI_AACT_4.GetWindowText(sACT);
	m_SI_AMOI_4.GetWindowText(sMOI);
	m_SI_AMOI_3.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SI_AMOI_4.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SI_AMOI_4.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SI_AMOI_4.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSIA5();
}
void CQuotasBiTrancheDlg::OnMajSIA5()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SI_AACT_5.GetWindowText(sACT);
	m_SI_AMOI_5.GetWindowText(sMOI);
	m_SI_AMOI_4.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SI_AMOI_5.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SI_AMOI_5.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SI_AMOI_5.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSIA6();
}
void CQuotasBiTrancheDlg::OnMajSIA6()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SI_AACT_6.GetWindowText(sACT);
	m_SI_AMOI_6.GetWindowText(sMOI);
	m_SI_AMOI_5.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SI_AMOI_6.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SI_AMOI_6.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SI_AMOI_6.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	m_iDiff = 0;
	MajDispoSIA();
	bBucket = FALSE;
}
void CQuotasBiTrancheDlg::MajDispoSIA()
{
	CString sDISPO,sMOI,sRes;
	int iSRes, iDispo, iDispoAvant;

	m_SI_ARES_0.GetWindowText(sRes);
	iSRes = atoi(sRes);
	m_SI_ARES_1.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SI_ARES_2.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SI_ARES_3.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SI_ARES_4.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SI_ARES_5.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SI_ARES_6.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SI_AMOI_0.GetWindowText(sMOI);
	int sumAuthEtanche = 0;
	int sumDispEtanche = 0;
	CumulEtancheSIA(sumAuthEtanche, sumDispEtanche);
	iDispoAvant = (atoi(sMOI) - iSRes - sumDispEtanche);
	sDISPO.Format("%d",iDispoAvant);
	m_SI_ADISPO_0.SetWindowText(sDISPO); //Dispo 0

	m_SI_ARES_0.GetWindowText(sRes);
	m_SI_AMOI_1.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SI_ADISPO_1.SetWindowText(sDISPO); //Dispo 1

	m_SI_ARES_1.GetWindowText(sRes);
	m_SI_AMOI_2.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SI_ADISPO_2.SetWindowText(sDISPO); //Dispo 2

	m_SI_ARES_2.GetWindowText(sRes);
	m_SI_AMOI_3.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SI_ADISPO_3.SetWindowText(sDISPO); //Dispo 3

	m_SI_ARES_3.GetWindowText(sRes);
	m_SI_AMOI_4.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SI_ADISPO_4.SetWindowText(sDISPO); //Dispo 4

	m_SI_ARES_4.GetWindowText(sRes);
	m_SI_AMOI_5.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SI_ADISPO_5.SetWindowText(sDISPO); //Dispo 5

	m_SI_ARES_5.GetWindowText(sRes);
	m_SI_AMOI_6.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SI_ADISPO_6.SetWindowText(sDISPO); //Dispo 6

	//   BlancFondStatic(); mettre à blanc colonne DISPO + RECAP
	m_SI_ADISPO_0.SetBkColor(RGB(255,255,255));
	m_SI_ADISPO_1.SetBkColor(RGB(255,255,255));
	m_SI_ADISPO_2.SetBkColor(RGB(255,255,255));
	m_SI_ADISPO_3.SetBkColor(RGB(255,255,255));
	m_SI_ADISPO_4.SetBkColor(RGB(255,255,255));
	m_SI_ADISPO_5.SetBkColor(RGB(255,255,255));
	m_SI_ADISPO_6.SetBkColor(RGB(255,255,255));
}

// SI Cmpt B
void CQuotasBiTrancheDlg::OnMajSIB0()
{
	//  calcul de l'écart
	CString sACT,sMOI,sDiff,sRecap,sMOImin;
	int iRecap;
	m_SI_BACT_0.GetWindowText(sACT);
	m_SI_BMOI_0.GetWindowText(sMOI);
	m_SCI_BMOI_G.GetWindowText(sMOImin);
	m_IDC_LABEL_SI_B_TOTAL_MOI.GetWindowText(sMOImin); //NCH
	if (atoi(sMOI) < atoi(sMOImin))
	{
		sMOI = sMOImin;
		m_SI_BMOI_0.SetWindowText(sMOImin);
		AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_REDUCE_CCM1), 
			MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION );
	}
	m_iDiff = atoi(sACT) - atoi(sMOI);
	sDiff.Format("%d",(-m_iDiff));
	m_SI_BDIFF.SetWindowText(sDiff);
	m_IDC_LABEL_SI_B_DIFF.SetWindowText(sDiff); //NCH
	if (m_iDiff == 0)
	{
		m_SI_BDIFF.SetTextColor(RGB(0,0,0));
		m_IDC_LABEL_SI_B_DIFF.SetTextColor(RGB(0,0,0)); // NCH
	}
	else if (m_iDiff > 0)
	{
		m_SI_BDIFF.SetTextColor(RGB(255,0,0));
		m_IDC_LABEL_SI_B_DIFF.SetTextColor(RGB(255,0,0)); // NCH
	}
	else if (m_iDiff < 0)
	{
		m_SI_BDIFF.SetTextColor(COLOR_OVB_MTRANCHE);
		m_IDC_LABEL_SI_B_DIFF.SetTextColor(COLOR_OVB_MTRANCHE); // NCH
	}
	//  Mise à jour de la colonne MOI de RECAP
	iRecap = atoi(sMOI);
	m_SD_BMOI_0.GetWindowText(sRecap);
	iRecap += atoi(sRecap);
	sRecap.Format("%d",iRecap);
	m_iCapaMoi_B.SetWindowText(sRecap);

	m_iCapaMoi_A.GetWindowText(sRecap);
	iRecap += atoi(sRecap);
	sRecap.Format("%d",iRecap);
	m_iCapaMoi_TOT.SetWindowText(sRecap);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	VerifFlagTroncon();
	GrisageTroncon();
	bBucket = TRUE;
	OnMajSIB1();
}
void CQuotasBiTrancheDlg::OnMajSIB1()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SI_BACT_1.GetWindowText(sACT);
	m_SI_BMOI_1.GetWindowText(sMOI);
	m_SI_BMOI_0.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SI_BMOI_1.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SI_BMOI_1.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SI_BMOI_1.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSIB2();
}
void CQuotasBiTrancheDlg::OnMajSIB2()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SI_BACT_2.GetWindowText(sACT);
	m_SI_BMOI_2.GetWindowText(sMOI);
	m_SI_BMOI_1.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SI_BMOI_2.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SI_BMOI_2.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SI_BMOI_2.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSIB3();
}
void CQuotasBiTrancheDlg::OnMajSIB3()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SI_BACT_3.GetWindowText(sACT);
	m_SI_BMOI_3.GetWindowText(sMOI);
	m_SI_BMOI_2.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SI_BMOI_3.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SI_BMOI_3.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SI_BMOI_3.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();bBucket = TRUE;
	OnMajSIB4();
}
void CQuotasBiTrancheDlg::OnMajSIB4()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SI_BACT_4.GetWindowText(sACT);
	m_SI_BMOI_4.GetWindowText(sMOI);
	m_SI_BMOI_3.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SI_BMOI_4.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SI_BMOI_4.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SI_BMOI_4.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSIB5();
}
void CQuotasBiTrancheDlg::OnMajSIB5()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SI_BACT_5.GetWindowText(sACT);
	m_SI_BMOI_5.GetWindowText(sMOI);
	m_SI_BMOI_4.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SI_BMOI_5.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SI_BMOI_5.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SI_BMOI_5.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSIB6();
}
void CQuotasBiTrancheDlg::OnMajSIB6()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SI_BACT_6.GetWindowText(sACT);
	m_SI_BMOI_6.GetWindowText(sMOI);
	m_SI_BMOI_5.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SI_BMOI_6.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SI_BMOI_6.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SI_BMOI_6.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSIB7();
}
void CQuotasBiTrancheDlg::OnMajSIB7()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SI_BACT_7.GetWindowText(sACT);
	m_SI_BMOI_7.GetWindowText(sMOI);
	m_SI_BMOI_6.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SI_BMOI_7.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SI_BMOI_7.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SI_BMOI_7.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSIFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SI
	GrisageVnauVnas();
	m_iDiff = 0;
	MajDispoSIB();
	bBucket = FALSE;
}
void CQuotasBiTrancheDlg::MajDispoSIB()
{
	CString sDISPO,sMOI,sRes;
	int iSRes, iDispo, iDispoAvant;

	m_SI_BRES_0.GetWindowText(sRes);
	iSRes = atoi(sRes);
	m_SI_BRES_1.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SI_BRES_2.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SI_BRES_3.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SI_BRES_4.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SI_BRES_5.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SI_BRES_6.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SI_BRES_7.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SI_BMOI_0.GetWindowText(sMOI);
	int sumAuthEtanche = 0;
	int sumDispEtanche = 0;
	CumulEtancheSIB(sumAuthEtanche, sumDispEtanche);
	iDispoAvant = (atoi(sMOI) - iSRes - sumDispEtanche);
	sDISPO.Format("%d",iDispoAvant);
	m_SI_BDISPO_0.SetWindowText(sDISPO); //Dispo 0

	m_SI_BRES_0.GetWindowText(sRes);
	m_SI_BMOI_1.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SI_BDISPO_1.SetWindowText(sDISPO); //Dispo 1

	m_SI_BRES_1.GetWindowText(sRes);
	m_SI_BMOI_2.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SI_BDISPO_2.SetWindowText(sDISPO); //Dispo 2

	m_SI_BRES_2.GetWindowText(sRes);
	m_SI_BMOI_3.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SI_BDISPO_3.SetWindowText(sDISPO); //Dispo 3

	m_SI_BRES_3.GetWindowText(sRes);
	m_SI_BMOI_4.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SI_BDISPO_4.SetWindowText(sDISPO); //Dispo 4

	m_SI_BRES_4.GetWindowText(sRes);
	m_SI_BMOI_5.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SI_BDISPO_5.SetWindowText(sDISPO); //Dispo 5

	m_SI_BRES_5.GetWindowText(sRes);
	m_SI_BMOI_6.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SI_BDISPO_6.SetWindowText(sDISPO); //Dispo 6

	m_SI_BRES_6.GetWindowText(sRes);
	m_SI_BMOI_7.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SI_BDISPO_7.SetWindowText(sDISPO); //Dispo 7

	//   BlancFondStatic();
	m_SI_BDISPO_0.SetBkColor(RGB(255,255,255));
	m_SI_BDISPO_1.SetBkColor(RGB(255,255,255));
	m_SI_BDISPO_2.SetBkColor(RGB(255,255,255));
	m_SI_BDISPO_3.SetBkColor(RGB(255,255,255));
	m_SI_BDISPO_4.SetBkColor(RGB(255,255,255));
	m_SI_BDISPO_5.SetBkColor(RGB(255,255,255));
	m_SI_BDISPO_6.SetBkColor(RGB(255,255,255));
	m_SI_BDISPO_7.SetBkColor(RGB(255,255,255));
}

// SD Cmpt A
void CQuotasBiTrancheDlg::OnMajSDA0()
{
	//  calcul de l'écart
	CString sACT,sMOI,sDiff,sRecap,sMOImin;
	int iRecap;
	m_SD_AACT_0.GetWindowText(sACT);
	m_SD_AMOI_0.GetWindowText(sMOI);
	m_SCD_AMOI_G.GetWindowText(sMOImin);
	m_IDC_LABEL_SD_A_TOTAL_MOI.GetWindowText(sMOImin); // NCH
	if (atoi(sMOI) < atoi(sMOImin))
	{
		sMOI = sMOImin;
		m_SD_AMOI_0.SetWindowText(sMOImin);
		AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_REDUCE_CCM1), 
			MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION );
	}
	m_iDiff = atoi(sACT) - atoi(sMOI);
	sDiff.Format("%d",(-m_iDiff));
	m_SD_ADIFF.SetWindowText(sDiff);
	m_IDC_LABEL_SD_A_DIFF.SetWindowText(sDiff); //NCH
	if (m_iDiff == 0)
	{
		m_SD_ADIFF.SetTextColor(RGB(0,0,0));
		m_IDC_LABEL_SD_A_DIFF.SetTextColor(RGB(0,0,0)); //NCH
	}
	else if (m_iDiff > 0)
	{
		m_SD_ADIFF.SetTextColor(RGB(255,0,0));
		m_IDC_LABEL_SD_A_DIFF.SetTextColor(RGB(255,0,0)); //NCH
	}
	else if (m_iDiff < 0)
	{
		m_SD_ADIFF.SetTextColor(COLOR_OVB_MTRANCHE);
		m_IDC_LABEL_SD_A_DIFF.SetTextColor(COLOR_OVB_MTRANCHE); //NCH
	}
	//  Mise à jour de la colonne MOI de RECAP
	iRecap = atoi(sMOI);
	m_SI_AMOI_0.GetWindowText(sRecap);
	iRecap += atoi(sRecap);
	sRecap.Format("%d",iRecap);
	m_iCapaMoi_A.SetWindowText(sRecap);

	m_iCapaMoi_B.GetWindowText(sRecap);
	iRecap += atoi(sRecap);
	sRecap.Format("%d",iRecap);
	m_iCapaMoi_TOT.SetWindowText(sRecap);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	VerifFlagTroncon();
	GrisageTroncon();
	bBucket = TRUE;
	OnMajSDA1();
}
void CQuotasBiTrancheDlg::OnMajSDA1()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SD_AACT_1.GetWindowText(sACT);
	m_SD_AMOI_1.GetWindowText(sMOI);
	m_SD_AMOI_0.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SD_AMOI_1.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{ 
		m_SD_AMOI_1.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SD_AMOI_1.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSDA2();
}
void CQuotasBiTrancheDlg::OnMajSDA2()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SD_AACT_2.GetWindowText(sACT);
	m_SD_AMOI_2.GetWindowText(sMOI);
	m_SD_AMOI_1.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SD_AMOI_2.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SD_AMOI_2.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SD_AMOI_2.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSDA3();
}
void CQuotasBiTrancheDlg::OnMajSDA3()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SD_AACT_3.GetWindowText(sACT);
	m_SD_AMOI_3.GetWindowText(sMOI);
	m_SD_AMOI_2.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SD_AMOI_3.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SD_AMOI_3.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SD_AMOI_3.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSDA4();
}
void CQuotasBiTrancheDlg::OnMajSDA4()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SD_AACT_4.GetWindowText(sACT);
	m_SD_AMOI_4.GetWindowText(sMOI);
	m_SD_AMOI_3.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SD_AMOI_4.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SD_AMOI_4.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SD_AMOI_4.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSDA5();
}
void CQuotasBiTrancheDlg::OnMajSDA5()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SD_AACT_5.GetWindowText(sACT);
	m_SD_AMOI_5.GetWindowText(sMOI);
	m_SD_AMOI_4.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SD_AMOI_5.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SD_AMOI_5.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SD_AMOI_5.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSDA6();
}
void CQuotasBiTrancheDlg::OnMajSDA6()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SD_AACT_6.GetWindowText(sACT);
	m_SD_AMOI_6.GetWindowText(sMOI);
	m_SD_AMOI_5.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SD_AMOI_6.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SD_AMOI_6.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SD_AMOI_6.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	m_iDiff = 0;
	MajDispoSDA();
	bBucket = FALSE;
}
void CQuotasBiTrancheDlg::MajDispoSDA()
{
	CString sDISPO,sMOI,sRes;
	int iSRes, iDispo, iDispoAvant;

	m_SD_ARES_0.GetWindowText(sRes);
	iSRes = atoi(sRes);
	m_SD_ARES_1.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SD_ARES_2.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SD_ARES_3.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SD_ARES_4.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SD_ARES_5.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SD_ARES_6.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SD_AMOI_0.GetWindowText(sMOI);
	int sumAuthEtanche = 0;
	int sumDispEtanche = 0;
	CumulEtancheSDA(sumAuthEtanche, sumDispEtanche);
	iDispoAvant = (atoi(sMOI) - iSRes - sumDispEtanche);
	sDISPO.Format("%d",iDispoAvant);
	m_SD_ADISPO_0.SetWindowText(sDISPO); //Dispo 0

	m_SD_ARES_0.GetWindowText(sRes);
	m_SD_AMOI_1.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SD_ADISPO_1.SetWindowText(sDISPO); //Dispo 1

	m_SD_ARES_1.GetWindowText(sRes);
	m_SD_AMOI_2.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SD_ADISPO_2.SetWindowText(sDISPO); //Dispo 2

	m_SD_ARES_2.GetWindowText(sRes);
	m_SD_AMOI_3.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SD_ADISPO_3.SetWindowText(sDISPO); //Dispo 3

	m_SD_ARES_3.GetWindowText(sRes);
	m_SD_AMOI_4.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SD_ADISPO_4.SetWindowText(sDISPO); //Dispo 4

	m_SD_ARES_4.GetWindowText(sRes);
	m_SD_AMOI_5.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SD_ADISPO_5.SetWindowText(sDISPO); //Dispo 5

	m_SD_ARES_5.GetWindowText(sRes);
	m_SD_AMOI_6.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SD_ADISPO_6.SetWindowText(sDISPO); //Dispo 6

	//   BlancFondStatic();
	m_SD_ADISPO_0.SetBkColor(RGB(255,255,255));
	m_SD_ADISPO_1.SetBkColor(RGB(255,255,255));
	m_SD_ADISPO_2.SetBkColor(RGB(255,255,255));
	m_SD_ADISPO_3.SetBkColor(RGB(255,255,255));
	m_SD_ADISPO_4.SetBkColor(RGB(255,255,255));
	m_SD_ADISPO_5.SetBkColor(RGB(255,255,255));
	m_SD_ADISPO_6.SetBkColor(RGB(255,255,255));
}

// SD Cmpt B
void CQuotasBiTrancheDlg::OnMajSDB0()
{
	//  calcul de l'écart
	CString sACT,sMOI,sDiff,sRecap,sMOImin;
	int iRecap;
	m_SD_BACT_0.GetWindowText(sACT);
	m_SD_BMOI_0.GetWindowText(sMOI);
	m_SCD_BMOI_G.GetWindowText(sMOImin);
	m_IDC_LABEL_SD_B_TOTAL_MOI.GetWindowText(sMOImin); //NCH
	if (atoi(sMOI) < atoi(sMOImin))
	{
		sMOI = sMOImin;
		m_SD_BMOI_0.SetWindowText(sMOImin);
		AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_REDUCE_CCM1), 
			MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION );
	}
	m_iDiff = atoi(sACT) - atoi(sMOI);
	sDiff.Format("%d",(-m_iDiff));
	m_SD_BDIFF.SetWindowText(sDiff);
	m_IDC_LABEL_SD_B_DIFF.SetWindowText(sDiff); //NCH
	if (m_iDiff == 0)
	{
		m_SD_BDIFF.SetTextColor(RGB(0,0,0));
		m_IDC_LABEL_SD_B_DIFF.SetTextColor(RGB(0,0,0)); //NCH
	}
	else if (m_iDiff > 0)
	{
		m_SD_BDIFF.SetTextColor(RGB(255,0,0));
		m_IDC_LABEL_SD_B_DIFF.SetTextColor(RGB(255,0,0)); //NCH
	}
	else if (m_iDiff < 0)
	{
		m_SD_BDIFF.SetTextColor(COLOR_OVB_MTRANCHE);
		m_IDC_LABEL_SD_B_DIFF.SetTextColor(COLOR_OVB_MTRANCHE); //NCH
	}
	//  Mise à jour de la colonne MOI de RECAP
	iRecap = atoi(sMOI);
	m_SI_BMOI_0.GetWindowText(sRecap);
	iRecap += atoi(sRecap);
	sRecap.Format("%d",iRecap);
	m_iCapaMoi_B.SetWindowText(sRecap);

	m_iCapaMoi_A.GetWindowText(sRecap);
	iRecap += atoi(sRecap);
	sRecap.Format("%d",iRecap);
	m_iCapaMoi_TOT.SetWindowText(sRecap);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	VerifFlagTroncon();
	GrisageTroncon();
	bBucket = TRUE;
	OnMajSDB1();
}
void CQuotasBiTrancheDlg::OnMajSDB1()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SD_BACT_1.GetWindowText(sACT);
	m_SD_BMOI_1.GetWindowText(sMOI);
	m_SD_BMOI_0.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SD_BMOI_1.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SD_BMOI_1.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SD_BMOI_1.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSDB2();
}
void CQuotasBiTrancheDlg::OnMajSDB2()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SD_BACT_2.GetWindowText(sACT);
	m_SD_BMOI_2.GetWindowText(sMOI);
	m_SD_BMOI_1.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SD_BMOI_2.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SD_BMOI_2.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SD_BMOI_2.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSDB3();
}
void CQuotasBiTrancheDlg::OnMajSDB3()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SD_BACT_3.GetWindowText(sACT);
	m_SD_BMOI_3.GetWindowText(sMOI);
	m_SD_BMOI_2.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SD_BMOI_3.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SD_BMOI_3.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SD_BMOI_3.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSDB4();
}
void CQuotasBiTrancheDlg::OnMajSDB4()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SD_BACT_4.GetWindowText(sACT);
	m_SD_BMOI_4.GetWindowText(sMOI);
	m_SD_BMOI_3.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SD_BMOI_4.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SD_BMOI_4.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SD_BMOI_4.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSDB5();
}
void CQuotasBiTrancheDlg::OnMajSDB5()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SD_BACT_5.GetWindowText(sACT);
	m_SD_BMOI_5.GetWindowText(sMOI);
	m_SD_BMOI_4.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SD_BMOI_5.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SD_BMOI_5.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SD_BMOI_5.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSDB6();
}
void CQuotasBiTrancheDlg::OnMajSDB6()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SD_BACT_6.GetWindowText(sACT);
	m_SD_BMOI_6.GetWindowText(sMOI);
	m_SD_BMOI_5.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SD_BMOI_6.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SD_BMOI_6.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SD_BMOI_6.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	bBucket = TRUE;
	OnMajSDB7();
}
void CQuotasBiTrancheDlg::OnMajSDB7()
{
	CString sACT,sMOI,sMOIAvant,sDiff;
	m_SD_BACT_7.GetWindowText(sACT);
	m_SD_BMOI_7.GetWindowText(sMOI);
	m_SD_BMOI_6.GetWindowText(sMOIAvant);
	if (atoi(sMOI) > atoi(sMOIAvant))
	{
		sMOI = sMOIAvant;
		m_SD_BMOI_7.SetWindowText(sMOIAvant);
	}
	if ((m_iDiff == 0) && (!bBucket))
		m_iDiff = atoi(sACT) - atoi(sMOI);

	sDiff.Format("%d",(atoi(sACT) - m_iDiff));

	if (m_bOvbAdjustFlag)
	{
		m_SD_BMOI_7.SetWindowText( (((atoi(sACT) - m_iDiff) < 0) ? 
			"0" : sDiff));
	}
	//  else if (atoi(sMOI) > atoi(sMOIAvant))
	//	  m_SD_BMOI_7.SetWindowText(sMOIAvant);

	if (atoi(sMOI) != atoi(sACT))
		bSDFlagModif = TRUE; // Flag indiquant une modification pour mise a jour tranche SD
	GrisageVnauVnas();
	m_iDiff = 0;
	MajDispoSDB();
	bBucket = FALSE;
}
void CQuotasBiTrancheDlg::MajDispoSDB()
{
	CString sDISPO,sMOI,sRes;
	int iSRes, iDispo, iDispoAvant;

	m_SD_BRES_0.GetWindowText(sRes);
	iSRes = atoi(sRes);
	m_SD_BRES_1.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SD_BRES_2.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SD_BRES_3.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SD_BRES_4.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SD_BRES_5.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SD_BRES_6.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SD_BRES_7.GetWindowText(sRes);
	iSRes += atoi(sRes);
	m_SD_BMOI_0.GetWindowText(sMOI);
	iDispoAvant = (atoi(sMOI) - iSRes);
	int sumAuthEtanche = 0;
	int sumDispEtanche = 0;
	CumulEtancheSDB(sumAuthEtanche, sumDispEtanche);
	iDispoAvant = (atoi(sMOI) - iSRes - sumDispEtanche);
	sDISPO.Format("%d",iDispoAvant);
	m_SD_BDISPO_0.SetWindowText(sDISPO); //Dispo 0

	m_SD_BRES_0.GetWindowText(sRes);
	m_SD_BMOI_1.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SD_BDISPO_1.SetWindowText(sDISPO); //Dispo 1

	m_SD_BRES_1.GetWindowText(sRes);
	m_SD_BMOI_2.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SD_BDISPO_2.SetWindowText(sDISPO); //Dispo 2

	m_SD_BRES_2.GetWindowText(sRes);
	m_SD_BMOI_3.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SD_BDISPO_3.SetWindowText(sDISPO); //Dispo 3

	m_SD_BRES_3.GetWindowText(sRes);
	m_SD_BMOI_4.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SD_BDISPO_4.SetWindowText(sDISPO); //Dispo 4

	m_SD_BRES_4.GetWindowText(sRes);
	m_SD_BMOI_5.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SD_BDISPO_5.SetWindowText(sDISPO); //Dispo 5

	m_SD_BRES_5.GetWindowText(sRes);
	m_SD_BMOI_6.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SD_BDISPO_6.SetWindowText(sDISPO); //Dispo 6

	m_SD_BRES_6.GetWindowText(sRes);
	m_SD_BMOI_7.GetWindowText(sMOI);
	iSRes -= atoi(sRes);
	iDispo = atoi(sMOI) - iSRes;
	if(iDispo < iDispoAvant)
	{
		iDispoAvant = iDispo;
		sDISPO.Format("%d",iDispo);
	}
	m_SD_BDISPO_7.SetWindowText(sDISPO); //Dispo 7

	//   BlancFondStatic();
	m_SD_BDISPO_0.SetBkColor(RGB(255,255,255));
	m_SD_BDISPO_1.SetBkColor(RGB(255,255,255));
	m_SD_BDISPO_2.SetBkColor(RGB(255,255,255));
	m_SD_BDISPO_3.SetBkColor(RGB(255,255,255));
	m_SD_BDISPO_4.SetBkColor(RGB(255,255,255));
	m_SD_BDISPO_5.SetBkColor(RGB(255,255,255));
	m_SD_BDISPO_6.SetBkColor(RGB(255,255,255));
	m_SD_BDISPO_7.SetBkColor(RGB(255,255,255));
}
*/

/*
// SI SCI Cmpt A
void CQuotasBiTrancheDlg::OnMajSCIAG() {ControleScIAG ();}
void CQuotasBiTrancheDlg::OnMajSCIA1() {ControleScIA (1);}
void CQuotasBiTrancheDlg::OnMajSCIA2() {ControleScIA (2);}
void CQuotasBiTrancheDlg::OnMajSCIA3() {ControleScIA (3);}
void CQuotasBiTrancheDlg::OnMajSCIA4() {ControleScIA (4);}
void CQuotasBiTrancheDlg::OnMajSCIA5() {ControleScIA (5);}
void CQuotasBiTrancheDlg::OnMajSCIA6() {ControleScIA (6);}
void CQuotasBiTrancheDlg::OnMajSCIA7() {ControleScIA (7);}

// SI SCI Cmpt B
void CQuotasBiTrancheDlg::OnMajSCIBG(){ControleScIBG ();}
void CQuotasBiTrancheDlg::OnMajSCIB1(){ControleScIB (1);}
void CQuotasBiTrancheDlg::OnMajSCIB2(){ControleScIB (2);}
void CQuotasBiTrancheDlg::OnMajSCIB3(){ControleScIB (3);}
void CQuotasBiTrancheDlg::OnMajSCIB4(){ControleScIB (4);}
void CQuotasBiTrancheDlg::OnMajSCIB5(){ControleScIB (5);}
void CQuotasBiTrancheDlg::OnMajSCIB6(){ControleScIB (6);}
void CQuotasBiTrancheDlg::OnMajSCIB7(){ControleScIB (7);};

// SD SCI Cmpt A
void CQuotasBiTrancheDlg::OnMajSCDAG(){ControleScDAG ();}
void CQuotasBiTrancheDlg::OnMajSCDA1(){ControleScDA (1);}
void CQuotasBiTrancheDlg::OnMajSCDA2(){ControleScDA (2);}
void CQuotasBiTrancheDlg::OnMajSCDA3(){ControleScDA (3);}
void CQuotasBiTrancheDlg::OnMajSCDA4(){ControleScDA (4);}
void CQuotasBiTrancheDlg::OnMajSCDA5(){ControleScDA (5);}
void CQuotasBiTrancheDlg::OnMajSCDA6(){ControleScDA (6);}
void CQuotasBiTrancheDlg::OnMajSCDA7(){ControleScDA (7);}

// SD Cmpt B
void CQuotasBiTrancheDlg::OnMajSCDBG(){ControleScDBG ();}
void CQuotasBiTrancheDlg::OnMajSCDB1(){ControleScDB (1);}
void CQuotasBiTrancheDlg::OnMajSCDB2(){ControleScDB (2);}
void CQuotasBiTrancheDlg::OnMajSCDB3(){ControleScDB (3);}
void CQuotasBiTrancheDlg::OnMajSCDB4(){ControleScDB (4);}
void CQuotasBiTrancheDlg::OnMajSCDB5(){ControleScDB (5);}
void CQuotasBiTrancheDlg::OnMajSCDB6(){ControleScDB (6);}
void CQuotasBiTrancheDlg::OnMajSCDB7(){ControleScDB (7);}
*/

// NPI - Ano 70130
/*void CQuotasBiTrancheDlg::cumulVenteEtEtanche(QuotasAutorisationsListCtrl* list, int& sumVente, int& sumAuth, int& sumDisp)
{
	if (list == NULL)
		return ;

	for(int i = 1 ; i < list->MaxIndexation(); i++)
	{
		QuotasValue* moi = list->GetValue(Quotas::MOI, i);
		QuotasValue* etanche = list->GetValue(Quotas::ETANCHE, i);
		QuotasValue* vente = list->GetValue(Quotas::VENTES, i);

		if (moi != NULL && etanche != NULL && vente != NULL)
		{
			if (etanche->_value)
			{
				/// CumulEtanche
				sumAuth += moi->_value;

				int ids = moi->_value - vente->_value;
				if (ids > 0)
					sumDisp += ids;
			}
			else
			{
				/// CumuVente 
				sumVente += vente->_value;
			}
		}
	}
}*/

void CQuotasBiTrancheDlg::cumulEtanche(QuotasAutorisationsListCtrl* list, int& sumAuth, int& sumDisp)
{
	if (list == NULL)
		return ;

	for (int i = 1 ; i < list->MaxIndexation(); i++)
	{
		QuotasValue* moi = list->GetValue(Quotas::MOI, i);
		QuotasValue* etanche = list->GetValue(Quotas::ETANCHE, i);
		QuotasValue* vente = list->GetValue(Quotas::VENTES, i);

		if (moi != NULL && etanche != NULL && vente != NULL)
		{
			if (etanche->_value) // HRI, 14/08/2015 (conditionné à étanche)
			{
				/// CumulEtanche
				sumAuth += moi->_value;

				int ids = moi->_value - vente->_value;
				if (ids > 0)
					sumDisp += ids;
			}
		}
	}
}

void CQuotasBiTrancheDlg::cumulVente(QuotasAutorisationsListCtrl* list, int& sumVente)
{
	if (list == NULL)
		return ;

	for (int i = 1 ; i < list->MaxIndexation(); i++)
	{
		QuotasValue* moi = list->GetValue(Quotas::MOI, i);
		QuotasValue* etanche = list->GetValue(Quotas::ETANCHE, i);
		QuotasValue* vente = list->GetValue(Quotas::VENTES, i);

		if (moi != NULL && etanche != NULL && vente != NULL)
		{
			/// CumuVente 
			sumVente += vente->_value;
		}
	}
}
///


/*
void CQuotasBiTrancheDlg::CumulEtanche(CQuotaEdit* pSCI_MOI, YM_Static* pSCI_RES, int &SumAuth, int &SumDisp)
{
	CString sc, sv;
	int isc, isv, isd;
	pSCI_MOI->GetWindowText(sc);		// autorisation du SCI étanche
	isc = atoi (sc);
	SumAuth += isc;				// cumul de la somme des autorisation étanche
	pSCI_RES->GetWindowText(sv);
	isv = atoi (sv);					// ventes de cet étanche
	isd = isc - isv;					// dispo de l'étanche
	if (isd < 0)
		isd = 0;
	SumDisp += isd;
}

void CQuotasBiTrancheDlg::CumulVente(YM_Static* pSCI_RES, int &SumVente)
{
	CString sv;
	int isv;
	pSCI_RES->GetWindowText(sv);
	isv = atoi (sv);
	SumVente += isv;
}
*/

	/*
bool CQuotasBiTrancheDlg::ControleScIA(int nsci)
{
	/// TODO


	// On admet le cas 0 pour la simple mise à jour des dispo
	if ((nsci < 0) || (nsci > 7))
		return false;

	CQuotaEdit* pSCI_MOI = NULL;
	CQuotaEdit* pSCI_CURMOI = NULL;
	YM_Static* pSCI_RES = NULL;
	YM_Static* pSCI_DISP = NULL;

	if (nsci)
		switch (nsci)
	{
		case 1 : pSCI_CURMOI = &m_SCI_AMOI_1; break;
		case 2 : pSCI_CURMOI = &m_SCI_AMOI_2; break;
		case 3 : pSCI_CURMOI = &m_SCI_AMOI_3; break;
		case 4 : pSCI_CURMOI = &m_SCI_AMOI_4; break;
		case 5 : pSCI_CURMOI = &m_SCI_AMOI_5; break;
		case 6 : pSCI_CURMOI = &m_SCI_AMOI_6; break;
		case 7 : pSCI_CURMOI = &m_SCI_AMOI_7; break;
	}

	int sumAuthEtanche = 0;
	int sumDispEtanche = 0; 
	int sumVentePoreuse = 0;
	CString sg, s0, ksc, sres;
	int isg, is0, iksc, itotres;

	if (nsci)
	{
		pSCI_CURMOI->GetWindowText(ksc); // autorisation demandée
		iksc = atoi (ksc);
		if (iksc == ValScIA[nsci])
			return false; // on n'a rien changé.
	}
	else iksc = 0;

	m_IDC_LABEL_SI_A_TOTAL_MOI.GetWindowText(sg); // NCH
	m_SCI_AMOI_G.GetWindowText(sg);
	isg = atoi (sg);    // valeur autorisation de la CC-1

	m_SI_ARES_TOT.GetWindowText(sres);
	itotres = atoi (sres);  // total des resa du compartiment.

	m_SI_AMOI_0.GetWindowText(s0);
	is0 = atoi (s0);  // autorisation en bucket 0.

	if (nsci && (iksc > isg))				// elle ne peut être supérieure à la SCG
	{
		ksc.Format ("%d", ValScIA[nsci]);
		pSCI_CURMOI->SetWindowText(ksc);
		return false;
	}

	for (int k = 1; k <= 7; k++)
	{
		switch (k)
		{
		case 1 : pSCI_MOI = &m_SCI_AMOI_1; pSCI_RES = &m_SCI_ARES_1; break;
		case 2 : pSCI_MOI = &m_SCI_AMOI_2; pSCI_RES = &m_SCI_ARES_2; break;
		case 3 : pSCI_MOI = &m_SCI_AMOI_3; pSCI_RES = &m_SCI_ARES_3; break;
		case 4 : pSCI_MOI = &m_SCI_AMOI_4; pSCI_RES = &m_SCI_ARES_4; break;
		case 5 : pSCI_MOI = &m_SCI_AMOI_5; pSCI_RES = &m_SCI_ARES_5; break;
		case 6 : pSCI_MOI = &m_SCI_AMOI_6; pSCI_RES = &m_SCI_ARES_6; break;
		case 7 : pSCI_MOI = &m_SCI_AMOI_7; pSCI_RES = &m_SCI_ARES_7; break;
		}
		if (EtanScIA[k])
			CumulEtanche (pSCI_MOI, pSCI_RES, sumAuthEtanche, sumDispEtanche);
		else
			CumulVente (pSCI_RES, sumVentePoreuse);
	}

	if (nsci)
	{
		if ((sumAuthEtanche > isg) // la somme des autorisations en SCI étanche doit être inférieure au sous-contingent
			|| (!EtanScIA[nsci] && (iksc > isg - sumAuthEtanche)) ) // une SCI non étanche doit être inférieure à la SCG - la somme des auth étanches
		{
			ksc.Format ("%d", ValScIA[nsci]);
			pSCI_CURMOI->SetWindowText(ksc);
			return false; 
		}

		// vérification par rapport aux autres autorisations SCI non étanche
		// les étanches ont été contrôlées par le test (sumAuthEtanche > isg)
		for (int l = 1; l <= 7; l++)
		{
			switch (l)
			{
			case 1 : pSCI_MOI = &m_SCI_AMOI_1; break;
			case 2 : pSCI_MOI = &m_SCI_AMOI_2; break;
			case 3 : pSCI_MOI = &m_SCI_AMOI_3; break;
			case 4 : pSCI_MOI = &m_SCI_AMOI_4; break;
			case 5 : pSCI_MOI = &m_SCI_AMOI_5; break;
			case 6 : pSCI_MOI = &m_SCI_AMOI_6; break;
			case 7 : pSCI_MOI = &m_SCI_AMOI_7; break;
			}
			if (!EtanScIA[l])
			{
				CString sauth;
				int iauth;
				pSCI_MOI->GetWindowText(sauth); // auth du sci
				iauth = atoi (sauth);
				if (iauth > isg - sumAuthEtanche)
				{
					ksc.Format ("%d", ValScIA[nsci]);
					pSCI_CURMOI->SetWindowText(ksc);
					return false;
				}
			}
		}
	}

	CString sdisp0;
	int disp0;
	// calcul dispo du SCG = auth du SCG - somme auth étanche - somme ventes non étanche
	int dispscg = isg - sumAuthEtanche - sumVentePoreuse;
	// OK, et si c'est une SCI étanche, maj de la dispo en CC0
	if (nsci && EtanScIA[nsci])
	{
		disp0 = is0 - itotres - sumDispEtanche;
		if (disp0 < 0)
			disp0 = 0;
		sdisp0.Format("%d",disp0);
		m_SI_ADISPO_0.SetWindowText(sdisp0); //Dispo 0
	}
	else
	{  
		m_SI_ADISPO_0.GetWindowText(sdisp0);
		disp0 = atoi (sdisp0);		// dispo de la CC0
	} 
	if (dispscg > disp0)		// la dispo SCG ne peut être supérieure à celle de la CC0
		dispscg = disp0;

	// On est bon, recalcul global des dispos de SCIA
	for (int j=1; j<= 7; j++)
	{
		switch (j)
		{
		case 1 : pSCI_MOI = &m_SCI_AMOI_1; pSCI_RES = &m_SCI_ARES_1; pSCI_DISP = &m_SCI_ADISPO_1; break;
		case 2 : pSCI_MOI = &m_SCI_AMOI_2; pSCI_RES = &m_SCI_ARES_2; pSCI_DISP = &m_SCI_ADISPO_2; break;
		case 3 : pSCI_MOI = &m_SCI_AMOI_3; pSCI_RES = &m_SCI_ARES_3; pSCI_DISP = &m_SCI_ADISPO_3; break;
		case 4 : pSCI_MOI = &m_SCI_AMOI_4; pSCI_RES = &m_SCI_ARES_4; pSCI_DISP = &m_SCI_ADISPO_4; break;
		case 5 : pSCI_MOI = &m_SCI_AMOI_5; pSCI_RES = &m_SCI_ARES_5; pSCI_DISP = &m_SCI_ADISPO_5; break;
		case 6 : pSCI_MOI = &m_SCI_AMOI_6; pSCI_RES = &m_SCI_ARES_6; pSCI_DISP = &m_SCI_ADISPO_6; break;
		case 7 : pSCI_MOI = &m_SCI_AMOI_7; pSCI_RES = &m_SCI_ARES_7; pSCI_DISP = &m_SCI_ADISPO_7; break;
		}

		CString sres, sauth, sdisp;
		int ires, iauth, idisp;
		pSCI_MOI->GetWindowText(sauth); // auth du sci
		iauth = atoi (sauth);
		pSCI_RES->GetWindowText(sres); // résa du sci
		ires = atoi (sres);
		idisp = iauth - ires;		// dispo par défaut du SCI : auth - vente
		if (!EtanScIA[j])
		{ // non étanche 
			if (idisp > dispscg)
				idisp = dispscg; 
		}
		sdisp.Format ("%d", idisp);
		pSCI_DISP->SetWindowText(sdisp);
	}

	if (nsci)
	{
		//la valeur est OK, on la mémorise.
		ValScIA[nsci] = iksc;
		bSCIAFlagModif = true;
		GrisageVnauVnas();
	}
	
	return true;
}
*/

/*
	
bool CQuotasBiTrancheDlg::ControleScIB(int nsci)
{
	/// TODO

	// On admet le cas 0 pour la simple mise à jour des dispo
	if ((nsci < 0) || (nsci > 7))
		return false;

	CQuotaEdit* pSCI_MOI = NULL;
	CQuotaEdit* pSCI_CURMOI = NULL;
	YM_Static* pSCI_RES = NULL;
	YM_Static* pSCI_DISP = NULL;

	if (nsci)
		switch (nsci)
	{
		case 1 : pSCI_CURMOI = &m_SCI_BMOI_1; break;
		case 2 : pSCI_CURMOI = &m_SCI_BMOI_2; break;
		case 3 : pSCI_CURMOI = &m_SCI_BMOI_3; break;
		case 4 : pSCI_CURMOI = &m_SCI_BMOI_4; break;
		case 5 : pSCI_CURMOI = &m_SCI_BMOI_5; break;
		case 6 : pSCI_CURMOI = &m_SCI_BMOI_6; break;
		case 7 : pSCI_CURMOI = &m_SCI_BMOI_7; break;
	}

	int sumAuthEtanche = 0;
	int sumDispEtanche = 0; 
	int sumVentePoreuse = 0;
	CString sg, s0, ksc, sres;
	int isg, is0, iksc, itotres;

	if (nsci)
	{
		pSCI_CURMOI->GetWindowText(ksc); // autorisation demandée
		iksc = atoi (ksc);
		if (iksc == ValScIB[nsci])
			return false; // on n'a rien changé.
	}
	else
		iksc = 0;

	m_SCI_BMOI_G.GetWindowText(sg);
	m_IDC_LABEL_SI_B_TOTAL_MOI.GetWindowText(sg); //NCH
	isg = atoi (sg);    // valeur autorisation de la CC-1

	m_SI_BRES_TOT.GetWindowText(sres);
	itotres = atoi (sres);  // total des resa du compartiment.

	m_SI_BMOI_0.GetWindowText(s0);
	is0 = atoi (s0);  // autorisation en bucket 0.

	if (nsci && (iksc > isg))				// elle ne peut être supérieure à la SCG
	{
		ksc.Format ("%d", ValScIB[nsci]);
		pSCI_CURMOI->SetWindowText(ksc);
		return false;
	}

	for (int k = 1; k <= 7; k++)
	{
		switch (k)
		{
		case 1 : pSCI_MOI = &m_SCI_BMOI_1; pSCI_RES = &m_SCI_BRES_1; break;
		case 2 : pSCI_MOI = &m_SCI_BMOI_2; pSCI_RES = &m_SCI_BRES_2; break;
		case 3 : pSCI_MOI = &m_SCI_BMOI_3; pSCI_RES = &m_SCI_BRES_3; break;
		case 4 : pSCI_MOI = &m_SCI_BMOI_4; pSCI_RES = &m_SCI_BRES_4; break;
		case 5 : pSCI_MOI = &m_SCI_BMOI_5; pSCI_RES = &m_SCI_BRES_5; break;
		case 6 : pSCI_MOI = &m_SCI_BMOI_6; pSCI_RES = &m_SCI_BRES_6; break;
		case 7 : pSCI_MOI = &m_SCI_BMOI_7; pSCI_RES = &m_SCI_BRES_7; break;
		}
		if (EtanScIB[k])
			CumulEtanche (pSCI_MOI, pSCI_RES, sumAuthEtanche, sumDispEtanche);
		else
			CumulVente (pSCI_RES, sumVentePoreuse);
	}

	if (nsci)
	{
		if ((sumAuthEtanche > isg) // la somme des autorisations en SCI étanche doit être inférieure au sous-contingent
			|| (!EtanScIB[nsci] && (iksc > isg - sumAuthEtanche)) ) // une SCI non étanche doit être inférieure à la SCG - la somme des auth étanches
		{
			ksc.Format ("%d", ValScIB[nsci]);
			pSCI_CURMOI->SetWindowText(ksc);
			return false; 
		}

		// vérification par rapport aux autres autorisations SCI non étanche
		// les étanches ont été contrôlées par le test (sumAuthEtanche > isg)
		for (int l = 1; l <= 7; l++)
		{
			switch (l)
			{
			case 1 : pSCI_MOI = &m_SCI_BMOI_1; break;
			case 2 : pSCI_MOI = &m_SCI_BMOI_2; break;
			case 3 : pSCI_MOI = &m_SCI_BMOI_3; break;
			case 4 : pSCI_MOI = &m_SCI_BMOI_4; break;
			case 5 : pSCI_MOI = &m_SCI_BMOI_5; break;
			case 6 : pSCI_MOI = &m_SCI_BMOI_6; break;
			case 7 : pSCI_MOI = &m_SCI_BMOI_7; break;
			}
			if (!EtanScIB[l])
			{
				CString sauth;
				int iauth;
				pSCI_MOI->GetWindowText(sauth); // auth du sci
				iauth = atoi (sauth);
				if (iauth > isg - sumAuthEtanche)
				{
					ksc.Format ("%d", ValScIB[nsci]);
					pSCI_CURMOI->SetWindowText(ksc);
					return false;
				}
			}
		}
	}

	CString sdisp0;
	int disp0;
	// calcul dispo du SCG = auth du SCG - somme auth étanche - somme ventes non étanche
	int dispscg = isg - sumAuthEtanche - sumVentePoreuse;
	// OK, et si c'est une SCI étanche, maj de la dispo en CC0
	if (nsci && EtanScIB[nsci])
	{
		disp0 = is0 - itotres - sumDispEtanche;
		if (disp0 < 0)
			disp0 = 0;
		sdisp0.Format("%d",disp0);
		m_SI_BDISPO_0.SetWindowText(sdisp0); //Dispo 0
	}
	else
	{  
		m_SI_BDISPO_0.GetWindowText(sdisp0);
		disp0 = atoi (sdisp0);		// dispo de la CC0
	} 
	if (dispscg > disp0)
		dispscg = disp0;

	// On est bon, recalcul global des dispos de SCIB
	for (int j=1; j<= 7; j++)
	{
		switch (j)
		{
		case 1 : pSCI_MOI = &m_SCI_BMOI_1; pSCI_RES = &m_SCI_BRES_1; pSCI_DISP = &m_SCI_BDISPO_1; break;
		case 2 : pSCI_MOI = &m_SCI_BMOI_2; pSCI_RES = &m_SCI_BRES_2; pSCI_DISP = &m_SCI_BDISPO_2; break;
		case 3 : pSCI_MOI = &m_SCI_BMOI_3; pSCI_RES = &m_SCI_BRES_3; pSCI_DISP = &m_SCI_BDISPO_3; break;
		case 4 : pSCI_MOI = &m_SCI_BMOI_4; pSCI_RES = &m_SCI_BRES_4; pSCI_DISP = &m_SCI_BDISPO_4; break;
		case 5 : pSCI_MOI = &m_SCI_BMOI_5; pSCI_RES = &m_SCI_BRES_5; pSCI_DISP = &m_SCI_BDISPO_5; break;
		case 6 : pSCI_MOI = &m_SCI_BMOI_6; pSCI_RES = &m_SCI_BRES_6; pSCI_DISP = &m_SCI_BDISPO_6; break;
		case 7 : pSCI_MOI = &m_SCI_BMOI_7; pSCI_RES = &m_SCI_BRES_7; pSCI_DISP = &m_SCI_BDISPO_7; break;
		}

		CString sres, sauth, sdisp;
		int ires, iauth, idisp;
		pSCI_MOI->GetWindowText(sauth); // auth du sci
		iauth = atoi (sauth);
		pSCI_RES->GetWindowText(sres); // résa du sci
		ires = atoi (sres);
		idisp = iauth - ires;		// dispo par défaut du SCI : auth - vente
		if (!EtanScIB[j])
		{ // non étanche 
			if (idisp > dispscg)
				idisp = dispscg; 
		}
		sdisp.Format ("%d", idisp);
		pSCI_DISP->SetWindowText(sdisp);
	}

	if (nsci)
	{
		//la valeur est OK, on la mémorise.
		ValScIB[nsci] = iksc;
		bSCIBFlagModif = true;
		GrisageVnauVnas();
	}
	
	return true;
}
*/


/*
bool CQuotasBiTrancheDlg::ControleScDA(int nsci)
{

	
	// On admet le cas 0 pour la simple mise à jour des dispo
	if ((nsci < 0) || (nsci > 7))
		return false;

	CQuotaEdit* pSCD_MOI = NULL;
	CQuotaEdit* pSCD_CURMOI = NULL;
	YM_Static* pSCD_RES = NULL;
	YM_Static* pSCD_DISP = NULL;

	if (nsci)
		switch (nsci)
	{
		case 1 : pSCD_CURMOI = &m_SCD_AMOI_1; break;
		case 2 : pSCD_CURMOI = &m_SCD_AMOI_2; break;
		case 3 : pSCD_CURMOI = &m_SCD_AMOI_3; break;
		case 4 : pSCD_CURMOI = &m_SCD_AMOI_4; break;
		case 5 : pSCD_CURMOI = &m_SCD_AMOI_5; break;
		case 6 : pSCD_CURMOI = &m_SCD_AMOI_6; break;
		case 7 : pSCD_CURMOI = &m_SCD_AMOI_7; break;
	}

	int sumAuthEtanche = 0;
	int sumDispEtanche = 0; 
	int sumVentePoreuse = 0;
	CString sg, s0, ksc, sres;
	int isg, is0, iksc, itotres;

	if (nsci)
	{
		pSCD_CURMOI->GetWindowText(ksc); // autorisation demandée
		iksc = atoi (ksc);
		if (iksc == ValScDA[nsci])
			return false; // on n'a rien changé.
	}
	else
		iksc = 0;

	m_SCD_AMOI_G.GetWindowText(sg);
	m_IDC_LABEL_SD_A_TOTAL_MOI.GetWindowText(sg); // NCH
	isg = atoi (sg);    // valeur autorisation de la CC-1

	m_SD_ARES_TOT.GetWindowText(sres);
	itotres = atoi (sres);  // total des resa du compartiment.

	m_SD_AMOI_0.GetWindowText(s0);
	is0 = atoi (s0);  // autorisation en bucket 0.

	if (nsci && (iksc > isg))				// elle ne peut être supérieure à la SCG
	{
		ksc.Format ("%d", ValScDA[nsci]);
		pSCD_CURMOI->SetWindowText(ksc);
		return false;
	}

	for (int k = 1; k <= 7; k++)
	{
		switch (k)
		{
		case 1 : pSCD_MOI = &m_SCD_AMOI_1; pSCD_RES = &m_SCD_ARES_1; break;
		case 2 : pSCD_MOI = &m_SCD_AMOI_2; pSCD_RES = &m_SCD_ARES_2; break;
		case 3 : pSCD_MOI = &m_SCD_AMOI_3; pSCD_RES = &m_SCD_ARES_3; break;
		case 4 : pSCD_MOI = &m_SCD_AMOI_4; pSCD_RES = &m_SCD_ARES_4; break;
		case 5 : pSCD_MOI = &m_SCD_AMOI_5; pSCD_RES = &m_SCD_ARES_5; break;
		case 6 : pSCD_MOI = &m_SCD_AMOI_6; pSCD_RES = &m_SCD_ARES_6; break;
		case 7 : pSCD_MOI = &m_SCD_AMOI_7; pSCD_RES = &m_SCD_ARES_7; break;
		}
		if (EtanScDA[k])
			CumulEtanche (pSCD_MOI, pSCD_RES, sumAuthEtanche, sumDispEtanche);
		else
			CumulVente (pSCD_RES, sumVentePoreuse);
	}

	if (nsci)
	{
		if ((sumAuthEtanche > isg) // la somme des autorisations en SCI étanche doit être inférieure au sous-contingent
			|| (!EtanScDA[nsci] && (iksc > isg - sumAuthEtanche)) ) // une SCI non étanche doit être inférieure à la SCG - la somme des auth étanches
		{
			ksc.Format ("%d", ValScDA[nsci]);
			pSCD_CURMOI->SetWindowText(ksc);
			return false; 
		}

		// vérification par rapport aux autres autorisations SCI non étanche
		// les étanches ont été contrôlées par le test (sumAuthEtanche > isg)
		for (int l = 1; l <= 7; l++)
		{
			switch (l)
			{
			case 1 : pSCD_MOI = &m_SCD_AMOI_1; break;
			case 2 : pSCD_MOI = &m_SCD_AMOI_2; break;
			case 3 : pSCD_MOI = &m_SCD_AMOI_3; break;
			case 4 : pSCD_MOI = &m_SCD_AMOI_4; break;
			case 5 : pSCD_MOI = &m_SCD_AMOI_5; break;
			case 6 : pSCD_MOI = &m_SCD_AMOI_6; break;
			case 7 : pSCD_MOI = &m_SCD_AMOI_7; break;
			}
			if (!EtanScDA[l])
			{
				CString sauth;
				int iauth;
				pSCD_MOI->GetWindowText(sauth); // auth du sci
				iauth = atoi (sauth);
				if (iauth > isg - sumAuthEtanche)
				{
					ksc.Format ("%d", ValScDA[nsci]);
					pSCD_CURMOI->SetWindowText(ksc);
					return false;
				}
			}
		}
	}

	CString sdisp0;
	int disp0;
	// calcul dispo du SCG = auth du SCG - somme auth étanche - somme ventes non étanche
	int dispscg = isg - sumAuthEtanche - sumVentePoreuse;
	// OK, et si c'est une SCI étanche, maj de la dispo en CC0
	if (nsci && EtanScDA[nsci])
	{
		disp0 = is0 - itotres - sumDispEtanche;
		if (disp0 < 0)
			disp0 = 0;
		sdisp0.Format("%d",disp0);
		m_SD_ADISPO_0.SetWindowText(sdisp0); //Dispo 0
	}
	else
	{  
		m_SD_ADISPO_0.GetWindowText(sdisp0);
		disp0 = atoi (sdisp0);		// dispo de la CC0
	} 
	if (dispscg > disp0)		// la dispo SCG ne peut être supérieure à celle de la CC0
		dispscg = disp0;

	// On est bon, recalcul global des dispos de SCIA
	for (int j=1; j<= 7; j++)
	{
		switch (j)
		{
		case 1 : pSCD_MOI = &m_SCD_AMOI_1; pSCD_RES = &m_SCD_ARES_1; pSCD_DISP = &m_SCD_ADISPO_1; break;
		case 2 : pSCD_MOI = &m_SCD_AMOI_2; pSCD_RES = &m_SCD_ARES_2; pSCD_DISP = &m_SCD_ADISPO_2; break;
		case 3 : pSCD_MOI = &m_SCD_AMOI_3; pSCD_RES = &m_SCD_ARES_3; pSCD_DISP = &m_SCD_ADISPO_3; break;
		case 4 : pSCD_MOI = &m_SCD_AMOI_4; pSCD_RES = &m_SCD_ARES_4; pSCD_DISP = &m_SCD_ADISPO_4; break;
		case 5 : pSCD_MOI = &m_SCD_AMOI_5; pSCD_RES = &m_SCD_ARES_5; pSCD_DISP = &m_SCD_ADISPO_5; break;
		case 6 : pSCD_MOI = &m_SCD_AMOI_6; pSCD_RES = &m_SCD_ARES_6; pSCD_DISP = &m_SCD_ADISPO_6; break;
		case 7 : pSCD_MOI = &m_SCD_AMOI_7; pSCD_RES = &m_SCD_ARES_7; pSCD_DISP = &m_SCD_ADISPO_7; break;
		}

		CString sres, sauth, sdisp;
		int ires, iauth, idisp;
		pSCD_MOI->GetWindowText(sauth); // auth du sci
		iauth = atoi (sauth);
		pSCD_RES->GetWindowText(sres); // résa du sci
		ires = atoi (sres);
		idisp = iauth - ires;		// dispo par défaut du SCI : auth - vente
		if (!EtanScDA[j])
		{ // non étanche 
			if (idisp > dispscg)
				idisp = dispscg; 
		}
		sdisp.Format ("%d", idisp);
		pSCD_DISP->SetWindowText(sdisp);
	}

	if (nsci)
	{
		//la valeur est OK, on la mémorise.
		ValScDA[nsci] = iksc;
		bSCDAFlagModif = true;
		GrisageVnauVnas();
	}

	

	return true;
}
*/


/*
bool CQuotasBiTrancheDlg::ControleScDB(int nsci)
{
	// On admet le cas 0 pour la simple mise à jour des dispo


	if ((nsci < 0) || (nsci > 7))
		return false;

	CQuotaEdit* pSCD_MOI = NULL;
	CQuotaEdit* pSCD_CURMOI = NULL;
	YM_Static* pSCD_RES = NULL;
	YM_Static* pSCD_DISP = NULL;

	if(nsci)
		switch (nsci)
	{
		case 1 : pSCD_CURMOI = &m_SCD_BMOI_1; break;
		case 2 : pSCD_CURMOI = &m_SCD_BMOI_2; break;
		case 3 : pSCD_CURMOI = &m_SCD_BMOI_3; break;
		case 4 : pSCD_CURMOI = &m_SCD_BMOI_4; break;
		case 5 : pSCD_CURMOI = &m_SCD_BMOI_5; break;
		case 6 : pSCD_CURMOI = &m_SCD_BMOI_6; break;
		case 7 : pSCD_CURMOI = &m_SCD_BMOI_7; break;
	}

	int sumAuthEtanche = 0;
	int sumDispEtanche = 0; 
	int sumVentePoreuse = 0;
	CString sg, s0, ksc, sres;
	int isg, is0, iksc, itotres;

	if (nsci)
	{
		pSCD_CURMOI->GetWindowText(ksc); // autorisation demandée
		iksc = atoi (ksc);
		if (iksc == ValScDB[nsci])
			return false; // on n'a rien changé.
	}
	else
		iksc = 0;

	m_SCD_BMOI_G.GetWindowText(sg);
	m_IDC_LABEL_SD_B_TOTAL_MOI.GetWindowText(sg); //NCH
	isg = atoi (sg);    // valeur autorisation de la CC-1

	m_SD_BRES_TOT.GetWindowText(sres);
	itotres = atoi (sres);  // total des resa du compartiment.

	m_SD_BMOI_0.GetWindowText(s0);
	is0 = atoi (s0);  // autorisation en bucket 0.

	if (nsci && (iksc > isg))				// elle ne peut être supérieure à la SCG
	{
		ksc.Format ("%d", ValScDB[nsci]);
		pSCD_CURMOI->SetWindowText(ksc);
		return false;
	}

	for (int k = 1; k <= 7; k++)
	{
		switch (k)
		{
		case 1 : pSCD_MOI = &m_SCD_BMOI_1; pSCD_RES = &m_SCD_BRES_1; break;
		case 2 : pSCD_MOI = &m_SCD_BMOI_2; pSCD_RES = &m_SCD_BRES_2; break;
		case 3 : pSCD_MOI = &m_SCD_BMOI_3; pSCD_RES = &m_SCD_BRES_3; break;
		case 4 : pSCD_MOI = &m_SCD_BMOI_4; pSCD_RES = &m_SCD_BRES_4; break;
		case 5 : pSCD_MOI = &m_SCD_BMOI_5; pSCD_RES = &m_SCD_BRES_5; break;
		case 6 : pSCD_MOI = &m_SCD_BMOI_6; pSCD_RES = &m_SCD_BRES_6; break;
		case 7 : pSCD_MOI = &m_SCD_BMOI_7; pSCD_RES = &m_SCD_BRES_7; break;
		}
		if (EtanScDB[k])
			CumulEtanche (pSCD_MOI, pSCD_RES, sumAuthEtanche, sumDispEtanche);
		else
			CumulVente (pSCD_RES, sumVentePoreuse);
	}
	if (nsci)
	{
		if ((sumAuthEtanche > isg) // la somme des autorisations en SCI étanche doit être inférieure au sous-contingent
			|| (!EtanScDB[nsci] && (iksc > isg - sumAuthEtanche)) ) // une SCI non étanche doit être inférieure à la SCG - la somme des auth étanches
		{
			ksc.Format ("%d", ValScDB[nsci]);
			pSCD_CURMOI->SetWindowText(ksc);
			return false; 
		}

		// vérification par rapport aux autres autorisations SCI non étanche
		// les étanches ont été contrôlées par le test (sumAuthEtanche > isg)
		for (int l = 1; l <= 7; l++)
		{
			switch (l)
			{
			case 1 : pSCD_MOI = &m_SCD_BMOI_1; break;
			case 2 : pSCD_MOI = &m_SCD_BMOI_2; break;
			case 3 : pSCD_MOI = &m_SCD_BMOI_3; break;
			case 4 : pSCD_MOI = &m_SCD_BMOI_4; break;
			case 5 : pSCD_MOI = &m_SCD_BMOI_5; break;
			case 6 : pSCD_MOI = &m_SCD_BMOI_6; break;
			case 7 : pSCD_MOI = &m_SCD_BMOI_7; break;
			}
			if (!EtanScDB[l])
			{
				CString sauth;
				int iauth;
				pSCD_MOI->GetWindowText(sauth); // auth du sci
				iauth = atoi (sauth);
				if (iauth > isg - sumAuthEtanche)
				{
					ksc.Format ("%d", ValScDB[nsci]);
					pSCD_CURMOI->SetWindowText(ksc);
					return false;
				}
			}
		}
	}

	CString sdisp0;
	int disp0;
	// calcul dispo du SCG = auth du SCG - somme auth étanche - somme ventes non étanche
	int dispscg = isg - sumAuthEtanche - sumVentePoreuse;
	// OK, et si c'est une SCI étanche, maj de la dispo en CC0
	if (nsci && EtanScDB[nsci])
	{
		disp0 = is0 - itotres - sumDispEtanche;
		if (disp0 < 0)
			disp0 = 0;
		sdisp0.Format("%d",disp0);
		m_SD_BDISPO_0.SetWindowText(sdisp0); //Dispo 0
	}
	else
	{  
		m_SD_BDISPO_0.GetWindowText(sdisp0);
		disp0 = atoi (sdisp0);		// dispo de la CC0
	} 
	if (dispscg > disp0)		// la dispo SCG ne peut être supérieure à celle de la CC0
		dispscg = disp0;

	// On est bon, recalcul global des dispos de SCIA
	for (int j=1; j<= 7; j++)
	{
		switch (j)
		{
		case 1 : pSCD_MOI = &m_SCD_BMOI_1; pSCD_RES = &m_SCD_BRES_1; pSCD_DISP = &m_SCD_BDISPO_1; break;
		case 2 : pSCD_MOI = &m_SCD_BMOI_2; pSCD_RES = &m_SCD_BRES_2; pSCD_DISP = &m_SCD_BDISPO_2; break;
		case 3 : pSCD_MOI = &m_SCD_BMOI_3; pSCD_RES = &m_SCD_BRES_3; pSCD_DISP = &m_SCD_BDISPO_3; break;
		case 4 : pSCD_MOI = &m_SCD_BMOI_4; pSCD_RES = &m_SCD_BRES_4; pSCD_DISP = &m_SCD_BDISPO_4; break;
		case 5 : pSCD_MOI = &m_SCD_BMOI_5; pSCD_RES = &m_SCD_BRES_5; pSCD_DISP = &m_SCD_BDISPO_5; break;
		case 6 : pSCD_MOI = &m_SCD_BMOI_6; pSCD_RES = &m_SCD_BRES_6; pSCD_DISP = &m_SCD_BDISPO_6; break;
		case 7 : pSCD_MOI = &m_SCD_BMOI_7; pSCD_RES = &m_SCD_BRES_7; pSCD_DISP = &m_SCD_BDISPO_7; break;
		}

		CString sres, sauth, sdisp;
		int ires, iauth, idisp;
		pSCD_MOI->GetWindowText(sauth); // auth du sci
		iauth = atoi (sauth);
		pSCD_RES->GetWindowText(sres); // résa du sci
		ires = atoi (sres);
		idisp = iauth - ires;		// dispo par défaut du SCI : auth - vente
		if (!EtanScDB[j])
		{ // non étanche 
			if (idisp > dispscg)
				idisp = dispscg; 
		}
		sdisp.Format ("%d", idisp);
		pSCD_DISP->SetWindowText(sdisp);
	}

	if (nsci)
	{
		//la valeur est OK, on la mémorise.
		ValScDB[nsci] = iksc;
		bSCDBFlagModif = true;
		GrisageVnauVnas();
	}
	
	return true;
}
*/

/*
// vérification qu'une autorisation (non étanche) est inférieure à maxval.
bool CQuotasBiTrancheDlg::CtrlMaxAuthSci (CQuotaEdit* pMOI, int maxval)
{
	CString sc;
	int isc;
	pMOI->GetWindowText(sc);
	isc = atoi (sc);
	if (isc > maxval)
		return false;
	else
		return true;
}
*/

bool CQuotasBiTrancheDlg::ctrlMaxAuthSci(QuotasAutorisationsListCtrl* list, int indexation, int maxVal)
{
	if (list != NULL)
	{
		QuotasValue* values =  list->GetValue(Quotas::MOI, indexation);

		if (values != NULL)
		{
			int value = values->_value;
			if (value > maxVal)
				return false;
			else return true;
		}
	}

	return false;
}




//void CQuotasBiTrancheDlg::OnUpdateDISPO(QuotasAutorisationsListCtrl* list, bool bSi)
void CQuotasBiTrancheDlg::OnUpdateDispoCC(QuotasAutorisationsListCtrl* list, bool bSi)
{
	if (list == NULL)
		return ;
	
	if (list->Type() != Quotas::CC)
		return;

	int iDispo = 0;
	int iRes = 0;
	int iDispoAvant = 0;

	for(int i = 0; i < list->MaxIndexation() ; i++)
	{
		QuotasValue* value = list->GetValue(Quotas::VENTES, i);
		if (value != NULL)
			iRes += value->_value;
	}

	/// recuperation AMOI_0 
	QuotasValue* valueMOI = list->GetValue(Quotas::MOI, 0);
	QuotasValue* valueDISPO = list->GetValue(Quotas::DISPO, 0);

	if (valueMOI != NULL && valueDISPO != NULL)
	{
		int sumAuthEtanche = 0;
		int sumDispEtanche = 0;

		if (bSi)
		{
			if (list->EspacePhysique() == 'A')
				CumulEtancheSIA(sumAuthEtanche, sumDispEtanche);
			else if (list->EspacePhysique() == 'B')
				CumulEtancheSIB( sumAuthEtanche, sumDispEtanche);
		}
		else
		{
			if (list->EspacePhysique() == 'A')
				CumulEtancheSDA(sumAuthEtanche, sumDispEtanche);
			else if (list->EspacePhysique() == 'B')
				CumulEtancheSDB(sumAuthEtanche, sumDispEtanche);
		}

		iDispoAvant = valueMOI->_value - iRes - sumDispEtanche;
		valueDISPO->_value = iDispoAvant;
	}

	/// pour tous les autres 
	for (int i = 1; i < list->MaxIndexation(); i++)
	{
		/// RES d'avant 
		QuotasValue* valueVENTE = list->GetValue(Quotas::VENTES, i - 1);
		/// MOI 
		QuotasValue* valueMOI = list->GetValue(Quotas::MOI, i);
		QuotasValue* valueDISPO = list->GetValue(Quotas::DISPO, i);
		
		if (valueVENTE != NULL && valueMOI != NULL && valueDISPO != NULL)
		{
			iRes -= valueVENTE->_value;
			int iDispo = valueMOI->_value - iRes;
			
			if (iDispo < iDispoAvant)
			{
				iDispoAvant = iDispo;						
			}
			valueDISPO->_value = iDispo;
			valueDISPO->_grey_background = false;
		}
	}
}

void CQuotasBiTrancheDlg::CumulEtancheSIA(int &sumAuthEtanche, int &sumDispEtanche)
{
	/// DM-7978 - CLE 
	//int sum = 0; // NPI - Ano 70130
	sumAuthEtanche = 0;
	sumDispEtanche = 0;
	// NPI - Ano 70130
	//cumulVenteEtEtanche(&m_quotas_view_si_scx_a._list_ctrl, sum, sumAuthEtanche, sumDispEtanche);
	cumulEtanche(&m_quotas_view_si_scx_a._list_ctrl, sumAuthEtanche, sumDispEtanche);
	///
	/// DM-7978 - CLE
}


void CQuotasBiTrancheDlg::CumulEtancheSIB(int &sumAuthEtanche, int &sumDispEtanche)
{
	/// DM-7978 - CLE
	//int sum = 0; // NPI - Ano 70130
	sumAuthEtanche = 0;
	sumDispEtanche = 0;
	// NPI - Ano 70130
	//cumulVenteEtEtanche(&m_quotas_view_si_scx_b._list_ctrl, sum, sumAuthEtanche, sumDispEtanche);
	cumulEtanche(&m_quotas_view_si_scx_b._list_ctrl, sumAuthEtanche, sumDispEtanche);
	///
	/// DM-7978 - CLE
}



void CQuotasBiTrancheDlg::CumulEtancheSDA(int &sumAuthEtanche, int &sumDispEtanche)
{
	/// DM-7978 - CLE
	//int sum = 0; // NPI - Ano 70130
	sumAuthEtanche = 0;
	sumDispEtanche = 0;
	// NPI - Ano 70130
	//cumulVenteEtEtanche(&m_quotas_view_sd_scx_a._list_ctrl, sum, sumAuthEtanche, sumDispEtanche);
	cumulEtanche(&m_quotas_view_sd_scx_a._list_ctrl, sumAuthEtanche, sumDispEtanche);
	///
	/// DM-7978 - CLE
}


void CQuotasBiTrancheDlg::CumulEtancheSDB(int &sumAuthEtanche, int &sumDispEtanche)
{
	/// DM-7978 - CLE
	//int sum = 0; // NPI - Ano 70130
	sumAuthEtanche = 0;
	sumDispEtanche = 0;
	// NPI - Ano 70130
	//cumulVenteEtEtanche(&m_quotas_view_sd_scx_b._list_ctrl, sum, sumAuthEtanche, sumDispEtanche);
	cumulEtanche(&m_quotas_view_sd_scx_b._list_ctrl, sumAuthEtanche, sumDispEtanche);
	///
	/// DM-7978 - CLE
}

// ------------------------------------------------
// Cumul global scx etanche + max scx non étanche
// pour la tranche internationale cmpt A
// HRI, 14/08/2015
// ------------------------------------------------
void CQuotasBiTrancheDlg::SommeGlobaleSIA(int &sumAuthEtanche, int &sumDispEtanche)
{
	sumAuthEtanche = 0;
	sumDispEtanche = 0;
	SommeGlobaleSCX(&m_quotas_view_si_scx_a._list_ctrl, sumAuthEtanche, sumDispEtanche);
}

// ------------------------------------------------
// Cumul global scx etanche + max scx non étanche
// pour la tranche internationale cmpt B
// HRI, 14/08/2015
// ------------------------------------------------
void CQuotasBiTrancheDlg::SommeGlobaleSIB(int &sumAuthEtanche, int &sumDispEtanche)
{
	sumAuthEtanche = 0;
	sumDispEtanche = 0;
	SommeGlobaleSCX(&m_quotas_view_si_scx_b._list_ctrl, sumAuthEtanche, sumDispEtanche);
}


// ------------------------------------------------
// Cumul global scx etanche + max scx non étanche
// pour la tranche domestique cmpt A
// HRI, 14/08/2015
// ------------------------------------------------
void CQuotasBiTrancheDlg::SommeGlobaleSDA(int &sumAuthEtanche, int &sumDispEtanche)
{
	sumAuthEtanche = 0;
	sumDispEtanche = 0;
	SommeGlobaleSCX(&m_quotas_view_sd_scx_a._list_ctrl, sumAuthEtanche, sumDispEtanche);
}

// ------------------------------------------------
// Cumul global scx etanche + max scx non étanche
// pour la tranche domestique cmpt B
// HRI, 14/08/2015
// ------------------------------------------------
void CQuotasBiTrancheDlg::SommeGlobaleSDB(int &sumAuthEtanche, int &sumDispEtanche)
{
	sumAuthEtanche = 0;
	sumDispEtanche = 0;
	SommeGlobaleSCX(&m_quotas_view_sd_scx_b._list_ctrl, sumAuthEtanche, sumDispEtanche);
}
// ------------------------------------------------
// Cumul global scx etanche + max scx non étanche
// par list de scx
// HRI, 14/08/2015
// ------------------------------------------------
void CQuotasBiTrancheDlg::SommeGlobaleSCX(QuotasAutorisationsListCtrl* list, int& sumAuth, int& sumDisp)
{
	if (list == NULL)
		return ;

	// HRI, 13/08/2015
	// le cumul etanche = max(auto des scx non étanches + auto des scx étanches

	int maxScxNonEtanche=0; // HRI, 13/08/2015
	int sommeScxEtanche=0; // HRI, 13/08/2015
	
	for (int i = 1 ; i < list->MaxIndexation(); i++)
	{
		QuotasValue* indexation = list->GetValue(Quotas::INDEXATION, i);
		QuotasValue* moi = list->GetValue(Quotas::MOI, i);
		QuotasValue* etanche = list->GetValue(Quotas::ETANCHE, i);
		QuotasValue* dispo = list->GetValue(Quotas::DISPO, i);
		QuotasValue* vente = list->GetValue(Quotas::VENTES, i);

		if (moi != NULL && etanche != NULL && vente != NULL)
		{
			if (etanche->_value == 0 && moi->_value > maxScxNonEtanche)
			{
				maxScxNonEtanche = moi->_value;
			}
			else if (etanche->_value == 1)
			{
				sommeScxEtanche += moi->_value;
			}
			/// CumulEtanche
			//sumAuth += moi->_value;

			int ids = moi->_value - vente->_value;
			if (ids > 0)
				sumDisp += ids;
		}
	}
	sumAuth = maxScxNonEtanche + sommeScxEtanche;

}


void CQuotasBiTrancheDlg::VerifFlagTroncon()
{
	/// TODO 
	CString sMOI,sRecap, sMax, sMess;
	int     iMoiA, iMoiB, iCapA, iCapB, iMax, ova, ovb, ovg, iMoiTot, iCapTot;
	int countOK = 0;
	m_FlagTroncon = TRUE;
	m_FlagSousResa = FALSE;
	m_iCapaMoi_A.GetWindowText(sMOI);
	iMoiA = atoi(sMOI);
	iMoiTot = iMoiA;
	m_CapaA.GetWindowText(sRecap);
	iCapA = atoi(sRecap);
	iCapTot = iCapA;
	if (iMoiA > iCapA)
		m_iCapaMoi_A.SetTextColor (COLOR_OVB_MTRANCHE);
	else if (iMoiA < iCapA)
	{
		m_iCapaMoi_A.SetTextColor (RGB (255, 0, 0));
		sMess = "SOUS-RESA A";
		m_FlagSousResa = TRUE;
	}
	else
		m_iCapaMoi_A.SetTextColor (RGB (0, 0, 0));

	m_iCapaMoi_B.GetWindowText(sMOI);
	iMoiB = atoi(sMOI);
	iMoiTot += iMoiB;
	m_CapaB.GetWindowText(sRecap);
	iCapB = atoi(sRecap);
	iCapTot += iCapB;
	if (iMoiB > iCapB)
		m_iCapaMoi_B.SetTextColor (COLOR_OVB_MTRANCHE);
	else if (iMoiB < iCapB)
	{
		m_iCapaMoi_B.SetTextColor (RGB (255, 0, 0));
		sMess = "SOUS-RESA B";
		m_FlagSousResa = TRUE;
	}
	else
		m_iCapaMoi_B.SetTextColor (RGB (0, 0, 0));

	if (iCapA)
	{
		ova = ((iMoiA * 100)+(iCapA/2)) / iCapA;
		GetDlgItem(IDC_MAXA)->GetWindowText (sMax);
		iMax = atoi (sMax);
		if (iMoiA > iMax)
		{
			m_iCapaMoi_A.SetBkColor (RGB (255,255, 128));
			sMess.Format ("Sur résa A dépassée\n %d%% > %d%%", ova, m_ovbA);
		}
		else
			m_iCapaMoi_A.SetBkColor (::GetSysColor(COLOR_WINDOW));
	}

	if (iCapB)
	{
		ovb = ((iMoiB * 100)+(iCapB/2)) / iCapB;
		GetDlgItem(IDC_MAXB)->GetWindowText (sMax);
		iMax = atoi (sMax);
		if (iMoiB > iMax)
		{
			m_iCapaMoi_B.SetBkColor (RGB (255,255, 128));
			sMess.Format ("Sur résa B dépassée\n %d%% > %d%%", ovb, m_ovbB);
		}
		else
			m_iCapaMoi_B.SetBkColor (::GetSysColor(COLOR_WINDOW));
	}

	GetDlgItem(IDC_ICON_STOP)->ShowWindow(SW_HIDE); 
	if (sMess.GetLength())
		GetDlgItem(IDC_ICON_DANGER)->ShowWindow(SW_SHOW);
	else 
		GetDlgItem(IDC_ICON_DANGER)->ShowWindow(SW_HIDE);

	if (iCapTot)
	{
		ovg = ((iMoiTot * 100) +(iCapTot/2)) / iCapTot;
		GetDlgItem(IDC_MAXTOT)->GetWindowText (sMax);
		if (iMoiTot > iCapTot)
			m_iCapaMoi_TOT.SetTextColor (COLOR_OVB_MTRANCHE);
		else if (iMoiTot < iCapTot)
			m_iCapaMoi_TOT.SetTextColor (RGB (255, 0, 0));

		iMax = atoi (sMax);
		if (iMoiTot > iMax)
		{
			GetDlgItem(IDC_ICON_DANGER)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_ICON_STOP)->ShowWindow(SW_SHOW);
			m_iCapaMoi_TOT.SetBkColor (RGB (255, 0, 0));
			sMess.Format ("Sur résa A+B dépassée\n %d%% > %d%%", ovg, m_ovg);
			m_FlagTroncon = FALSE;
		}
		else
			m_iCapaMoi_TOT.SetBkColor (::GetSysColor(COLOR_WINDOW));
	}

	GetDlgItem(IDC_ALARME)->SetWindowText(sMess);
}


void CQuotasBiTrancheDlg::MajAuthSI(int NoTranche, const CString& sLegOrig, const CString& sCmpt)
{
	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();

	if (m_lRrdIndex < 0)
		return;

	YmIcResaBktDom pResaBkt;

	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);

	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
	pSQL->SetDomain(&pResaBkt);

	if (sCmpt == 'A')
	{
		std::vector<QuotasValues>& list = m_quotas_view_si_cc_a._list_ctrl.Datas();
		std::vector<QuotasValues>::iterator it;

		for(it = list.begin(); it != list.end(); ++it)
		{
			QuotasValues& values = (*it);
			int nestLevel = values._nest_level._value;
			int value = values._moi._value;

			pResaBkt.NestLevel(nestLevel);
			pResaBkt.AuthCur(value);
			pResaBkt.AuthUser(value);

			WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH);
		}
	}
	else if (sCmpt == 'B')
	{
		std::vector<QuotasValues>& list = m_quotas_view_si_cc_b._list_ctrl.Datas();
		std::vector<QuotasValues>::iterator it;
		for(it = list.begin(); it != list.end(); ++it)
		{
			QuotasValues& values = (*it);
			int nestLevel = values._nest_level._value;
			int value = values._moi._value;

			pResaBkt.NestLevel(nestLevel);
			pResaBkt.AuthCur(value);
			pResaBkt.AuthUser(value);

			WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH);
		}
	}
	else 
	{
		
	}


	delete pSQL;
}


void CQuotasBiTrancheDlg::MajAuthSD(int NoTranche, const CString& sLegOrig, const CString& sCmpt)
{
	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();

	if (m_lRrdIndex < 0)
		return;

	YmIcResaBktDom pResaBkt;
	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);

	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
	pSQL->SetDomain(&pResaBkt);

	if (sCmpt == 'A')
	{
		std::vector<QuotasValues>& list = m_quotas_view_sd_cc_a._list_ctrl.Datas();
		std::vector<QuotasValues>::iterator it;

		for(it = list.begin(); it != list.end(); ++it)
		{
			QuotasValues& values = (*it);
			int nestLevel = values._nest_level._value;
			int value = values._moi._value;

			pResaBkt.NestLevel(nestLevel);
			pResaBkt.AuthCur(value);
			pResaBkt.AuthUser(value);

			WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH);
		}
	}
	else if (sCmpt == 'B')
	{
		std::vector<QuotasValues>& list = m_quotas_view_sd_cc_b._list_ctrl.Datas();
		std::vector<QuotasValues>::iterator it;
		for(it = list.begin(); it != list.end(); ++it)
		{
			QuotasValues& values = (*it);
			int nestLevel = values._nest_level._value;
			int value = values._moi._value;

			pResaBkt.NestLevel(nestLevel);
			pResaBkt.AuthCur(value);
			pResaBkt.AuthUser(value);

			WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH);
		}
	}
	else 
	{
		
	}

	delete pSQL;
}


void CQuotasBiTrancheDlg::MajAuthSciSD(int NoTranche, const CString& sLegOrig, const CString& sCmpt)
{
	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();

	if (m_lRrdIndex < 0)
		return;

	
	YmIcResaBktDom pResaBkt;
	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);	

	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
	pSQL->SetDomain(&pResaBkt);


	if (sCmpt == 'A')
	{
		std::vector<QuotasValues>& list = m_quotas_view_sd_scx_a._list_ctrl.Datas();
		std::vector<QuotasValues>::iterator it;

		for(it = list.begin(); it != list.end(); ++it)
		{
			QuotasValues& values = (*it);
			int nestLevel = values._nest_level._value;
			int value = values._moi._value;

			pResaBkt.NestLevel(nestLevel);
			pResaBkt.AuthCur(value);
			pResaBkt.AuthUser(value);

			WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);
		}

	}
	else if (sCmpt == 'B')
	{
		std::vector<QuotasValues>& list = m_quotas_view_sd_scx_b._list_ctrl.Datas();
		std::vector<QuotasValues>::iterator it;
		for(it = list.begin(); it != list.end(); ++it)
		{
			QuotasValues& values = (*it);
			int nestLevel = values._nest_level._value;
			int value = values._moi._value;

			pResaBkt.NestLevel(nestLevel);
			pResaBkt.AuthCur(value);
			pResaBkt.AuthUser(value);

			WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);
		}
	}
	else 
	{
		
	}

	delete pSQL;
}


void CQuotasBiTrancheDlg::MajAuthSciSI(int NoTranche, const CString& sLegOrig, const CString& sCmpt)
{
	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();

	if (m_lRrdIndex < 0)
		return;

	YmIcResaBktDom pResaBkt;
	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);	

	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
	pSQL->SetDomain(&pResaBkt);

	if (sCmpt == 'A')
	{
		std::vector<QuotasValues>& list = m_quotas_view_si_scx_a._list_ctrl.Datas();
		std::vector<QuotasValues>::iterator it;

		for(it = list.begin(); it != list.end(); ++it)
		{
			QuotasValues& values = (*it);
			int nestLevel = values._nest_level._value;
			int value = values._moi._value;

			pResaBkt.NestLevel(nestLevel);
			pResaBkt.AuthCur(value);
			pResaBkt.AuthUser(value);
			
			WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);
		}

	}
	else if (sCmpt == 'B')
	{
		std::vector<QuotasValues>& list = m_quotas_view_si_scx_b._list_ctrl.Datas();
		std::vector<QuotasValues>::iterator it;
		for(it = list.begin(); it != list.end(); ++it)
		{
			QuotasValues& values = (*it);
			int nestLevel = values._nest_level._value;
			int value = values._moi._value;

			pResaBkt.NestLevel(nestLevel);
			pResaBkt.AuthCur(value);
			pResaBkt.AuthUser(value);

			WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);
		}
	}
	else 
	{
		
	}

	delete pSQL;
}


/*
void CQuotasBiTrancheDlg::MajAuthSci(int NoTranche, CString sLegOrig, CString sCmpt,
																		 int iMoiG, int iMoi1, int iMoi2, int iMoi3,
																		 int iMoi4, int iMoi5, int iMoi6, int iMoi7)
{
	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();

	if (m_lRrdIndex < 0)
		return;


	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);

	YmIcResaBktDom pResaBkt;
	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);



	pResaBkt.NestLevel(ilevel);
	
	pSQL->SetDomain(&pResaBkt);


	delete pSQL;

	/// TODO
	/*
	if (m_lRrdIndex < 0)
		return;
	YmIcResaBktDom pResaBkt;
	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);
	pResaBkt.NestLevel(ilevel);
	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
	pSQL->SetDomain(&pResaBkt);

	ilevel++;
	pResaBkt.NestLevel(ilevel);
	pResaBkt.AuthCur(iMoi1);
	pResaBkt.AuthUser(iMoi1);
	WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);

	ilevel++;
	pResaBkt.NestLevel(ilevel);
	pResaBkt.AuthCur(iMoi2);
	pResaBkt.AuthUser(iMoi2);
	WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);

	ilevel++;
	pResaBkt.NestLevel(ilevel);
	pResaBkt.AuthCur(iMoi3);
	pResaBkt.AuthUser(iMoi3);
	WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);

	ilevel++;
	pResaBkt.NestLevel(ilevel);
	pResaBkt.AuthCur(iMoi4);
	pResaBkt.AuthUser(iMoi4);
	WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);

	ilevel++;
	pResaBkt.NestLevel(ilevel);
	pResaBkt.AuthCur(iMoi5);
	pResaBkt.AuthUser(iMoi5);
	WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);

	ilevel++;
	pResaBkt.NestLevel(ilevel);
	pResaBkt.AuthCur(iMoi6);
	pResaBkt.AuthUser(iMoi6);
	WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);

	ilevel++;
	pResaBkt.NestLevel(ilevel);
	pResaBkt.AuthCur(iMoi7);
	pResaBkt.AuthUser(iMoi7);
	WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);


	YmIcResaCmptDom pResaCmpt;
	pResaCmpt.TrancheNo(NoTranche);
	pResaCmpt.DptDate(m_lDateDep);
	pResaCmpt.LegOrig(sLegOrig);
	pResaCmpt.Cmpt(sCmpt);
	pResaCmpt.RrdIndex(m_lRrdIndex);
	pResaCmpt.Capacity(iMoiG);
	pSQL->SetDomain(&pResaCmpt);
	WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_CMPT_SCI);

	delete pSQL;
}
	*/

void CQuotasBiTrancheDlg::GrisageTroncon()
{
	((CButton *)GetDlgItem(IDC_UN_TRONCON))->EnableWindow(m_FlagTroncon);
	//((CButton *)GetDlgItem(IDC_TOUS_TRONCONS))->EnableWindow(m_FlagTroncon);
	((CButton *)GetDlgItem(IDC_SEL_TRONCONS))->EnableWindow(m_FlagTroncon);
}

void CQuotasBiTrancheDlg::GrisageVnauVnas()
{
	((CWnd*)GetDlgItem (IDC_VNAU))->EnableWindow(bSIFlagModif || bSDFlagModif);
	((CWnd*)GetDlgItem (IDC_VNAS))->EnableWindow(bSCIAFlagModif || bSCDAFlagModif || bSCIBFlagModif || bSCDBFlagModif);
}

BOOL CQuotasBiTrancheDlg::GetSelTroncon(const CString& sChoix, CString &sListTroncon)
{
	CString szText;
	int iIndex = 0;
	int (*m_aiSelBuf);
	int m_iSelectedCount;

	int iListCount =  m_ListTroncon.GetCount();
	m_aiSelBuf = new int [iListCount];
	if(sChoix == "TOUS")
		m_iSelectedCount = iListCount;
	else
		m_iSelectedCount = m_ListTroncon.GetSelItems( iListCount, m_aiSelBuf );

	for (iIndex = 0; iIndex < m_iSelectedCount ; iIndex++)
	{
		if(iIndex>0) sListTroncon += ",";
		if(sChoix == "TOUS")
			m_ListTroncon.GetText( iIndex,szText );
		else
			m_ListTroncon.GetText( m_aiSelBuf[ iIndex ],szText );
		sListTroncon += szText;
	}
	delete [] m_aiSelBuf;
	return TRUE;
}

void CQuotasBiTrancheDlg::SetTitle(const CString& NewTitle)
{
	m_szTitle = NewTitle;
}

void CQuotasBiTrancheDlg::Chercher()
{
	// Evolution 01/12/2012
	// ne plus utiliser le KDISN, mais lire en base.
	bool badtrain = false;
	if (LireTrancheNo(badtrain))
		return;

	if (!badtrain)
	{
		// Rien trouve en base, on essai le KDISN
		CString vnlsent;
		vnlsent.Format("%s%s/%s%s%s","KDISN",m_NumeroTrain,
			MakeStrDate (m_lDateDep,TRUE),m_Origine,m_Destination);
		// Lancement de la requete de recherche
		if (((CCTAApp*)APP)->GetResComm()->SendDispo (this, vnlsent) != -1)
			return;
	}
}


void CQuotasBiTrancheDlg::BuildVnau(const CString& sListTroncon) 
{
	char delim[4];
	strcpy (delim, ",");

	char* pbis;
	char* tokbis;
	CString stokbis;
	CString sLegsOrig;
	pbis =(char*)malloc (sListTroncon.GetLength()+1);
	strcpy (pbis, sListTroncon);
	tokbis = strtok (pbis,delim);
	while (tokbis)
	{
		stokbis = tokbis;
		if (!sLegsOrig.IsEmpty())
			sLegsOrig += "','";
		sLegsOrig += stokbis.Left(5);
		tokbis = strtok (NULL, delim);
	}
	free (pbis);

//	int iMoiA0, iMoiA1, iMoiA2, iMoiA3, iMoiA4, iMoiA5, iMoiA6, iMoiA7;
//	int iMoiB0, iMoiB1, iMoiB2, iMoiB3, iMoiB4, iMoiB5, iMoiB6, iMoiB7;
	CString sVnau,sSI,sSD,sDB, ssw;
	CString sCmpt;
	char* pbuf;
	char* tok;
	CString stok;
	pbuf =(char*)malloc (sListTroncon.GetLength()+1);
	strcpy (pbuf, sListTroncon);
	tok = strtok (pbuf,delim);
	CQuotaVnau *pQuotaVnau;
	BOOL bt1, bt2;
	bt1 = bt2 = TRUE;
	BOOL bFirst = TRUE;
	unsigned long datevnau; // date mise dans le texte du VNAU
	CTrancheView *pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
	CLegView* pLegView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetLegView();


	while (tok)
	{
		stok = tok;
		datevnau = m_lDateDep;
		if (stok.Find ("-") < 0)
			datevnau += 1;

		if (bSIFlagModif /*|| bAllFlagModif*/)
		{   
			CString sswFormat;
			/// CLE 
			std::vector<QuotasValues>& listSIA = m_quotas_view_si_cc_a._list_ctrl.Datas();
			std::vector<QuotasValues>::iterator it;
			for(it = listSIA.begin(); it != listSIA.end(); ++it)
			{
				QuotasValues& value = (*it);
				int nestLevel = value._nest_level._value;
				int moi = value._moi._value;

				if (nestLevel == 0)
					sSI.Format("A%d-%d", nestLevel, moi);
				else
				{
					ssw.Format("/A%d-%d", nestLevel, moi);
					sSI += ssw;
				}
			}

			std::vector<QuotasValues>& listSIB = m_quotas_view_si_cc_b._list_ctrl.Datas();
			for(it = listSIB.begin(); it != listSIB.end(); ++it)
			{
				QuotasValues& value = (*it);
				int nestLevel = value._nest_level._value;
				int moi = value._moi._value;
				ssw.Format("/B%d-%d", nestLevel, moi);
				sSI += ssw;
			}

			/// CLE

			/*
			// Lecture des valeurs MOI
			LectureMoiSI(iMoiA0, iMoiA1, iMoiA2, iMoiA3, iMoiA4, iMoiA5, iMoiA6,
				iMoiB0, iMoiB1, iMoiB2, iMoiB3, iMoiB4, iMoiB5, iMoiB6, iMoiB7);
			// Constitution de la chaîne VNAU (VNAUN°Tranche/DateOD/A0-valeur/A1-valeur/...
			sSI.Format ("A0-%d", iMoiA0);
			if (m_BktMaxSIA >= 1) { ssw.Format("/A1-%d", iMoiA1); sSI += ssw; }
			if (m_BktMaxSIA >= 2) { ssw.Format("/A2-%d", iMoiA2); sSI += ssw; }
			if (m_BktMaxSIA >= 3) { ssw.Format("/A3-%d", iMoiA3); sSI += ssw; }
			if (m_BktMaxSIA >= 4) { ssw.Format("/A4-%d", iMoiA4); sSI += ssw; }
			if (m_BktMaxSIA >= 5) { ssw.Format("/A5-%d", iMoiA5); sSI += ssw; }
			if (m_BktMaxSIA >= 6) { ssw.Format("/A6-%d", iMoiA6); sSI += ssw; }
			ssw.Format("/B0-%d",iMoiB0);
			sSI += ssw;
			if (m_BktMaxSIB >= 1) { ssw.Format("/B1-%d", iMoiB1); sSI += ssw; }
			if (m_BktMaxSIB >= 2) { ssw.Format("/B2-%d", iMoiB2); sSI += ssw; }
			if (m_BktMaxSIB >= 3) { ssw.Format("/B3-%d", iMoiB3); sSI += ssw; }
			if (m_BktMaxSIB >= 4) { ssw.Format("/B4-%d", iMoiB4); sSI += ssw; }
			if (m_BktMaxSIB >= 5) { ssw.Format("/B5-%d", iMoiB5); sSI += ssw; }
			if (m_BktMaxSIB >= 6) { ssw.Format("/B6-%d", iMoiB6); sSI += ssw; }
			if (m_BktMaxSIB >= 7) { ssw.Format("/B7-%d", iMoiB7); sSI += ssw; }
			*/

			sVnau.Format("VNAU%d/%s%s%s/%s",m_trancheno1,MakeStrDate(datevnau,FALSE),
				stok.Left(5),stok.Right(5),sSI);
			// Lancement de la requete de mise a jour sur le troncon courant
			pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 1);
			m_VnauList.Add ((CObject*)pQuotaVnau);

			if (bt1)
			{
				((CCTAApp*)APP)->SetTrancheSentFlag (false, m_trancheno1, m_lDateDep);
				pTrancheView->SetSentFlag (m_trancheno1, m_lDateDep, 1);
				bt1 = FALSE;
			}
			pLegView->SetSentFlag (m_trancheno1, stok.Left(5), m_lDateDep, 1);

			if (bFirst)
			{
				((CCTAApp*)APP)->SetLegSentFlag (false, m_trancheno1, sLegsOrig, m_lDateDep);
				sCmpt = "A";
				
				MajAuthSI(m_trancheno1, sLegsOrig, sCmpt);
				/*
				MajAuth(m_trancheno1, sLegsOrig, sCmpt, iMoiA0, iMoiA1, iMoiA2, iMoiA3, 
					iMoiA4, iMoiA5, iMoiA6, 0);*/

				sCmpt = "B";
				MajAuthSI(m_trancheno1, sLegsOrig, sCmpt);

				/*
				MajAuth(m_trancheno1, sLegsOrig, sCmpt, iMoiB0, iMoiB1, iMoiB2, iMoiB3,
					iMoiB4, iMoiB5, iMoiB6, iMoiB7);*/
			}
		}
		if (bSDFlagModif /*|| bAllFlagModif*/)
		{   
			// Lecture des valeurs MOI
			/*
			LectureMoiSD(iMoiA0, iMoiA1, iMoiA2, iMoiA3, iMoiA4, iMoiA5, iMoiA6,
				iMoiB0, iMoiB1, iMoiB2, iMoiB3, iMoiB4, iMoiB5, iMoiB6, iMoiB7);
			*/

			// Constitution de la chaîne VNAU (VNAUN°Tranche/DateOD/A0-valeur/A1-valeur/...

			std::vector<QuotasValues>& listSDA = m_quotas_view_sd_cc_a._list_ctrl.Datas();
			std::vector<QuotasValues>::iterator it;

			for(it = listSDA.begin(); it != listSDA.end(); ++it)
			{
				QuotasValues& values = (*it);
				int value = values._moi._value;
				int nestLevel = values._nest_level._value;

				if (nestLevel == 0)
				{
					sSD.Format("A0-%d", value);	
				}
				else
				{
					ssw.Format("/A%d-%d",nestLevel, value);
					sSD += ssw;
				}
			}

			
			std::vector<QuotasValues>& listSDB = m_quotas_view_sd_cc_b._list_ctrl.Datas();

			for(it = listSDB.begin(); it != listSDB.end(); ++it)
			{
				QuotasValues& values = (*it);
				int value = values._moi._value;
				int nestLevel = values._nest_level._value;
				// HRI, 11/08/2015 :pb dans l'élaboration du VNAU Domestique
				ssw.Format("/B%d-%d", nestLevel, value);
				sSD += ssw;
			}

			/*
			sSD.Format ("A0-%d", iMoiA0);
			if (m_BktMaxSDA >= 1) { ssw.Format("/A1-%d", iMoiA1); sSD += ssw; }
			if (m_BktMaxSDA >= 2) { ssw.Format("/A2-%d", iMoiA2); sSD += ssw; }
			if (m_BktMaxSDA >= 3) { ssw.Format("/A3-%d", iMoiA3); sSD += ssw; }
			if (m_BktMaxSDA >= 4) { ssw.Format("/A4-%d", iMoiA4); sSD += ssw; }
			if (m_BktMaxSDA >= 5) { ssw.Format("/A5-%d", iMoiA5); sSD += ssw; }
			if (m_BktMaxSDA >= 6) { ssw.Format("/A6-%d", iMoiA6); sSD += ssw; }
			ssw.Format("/B0-%d",iMoiB0);
			sSD += ssw;
			if (m_BktMaxSDB >= 1) { ssw.Format("/B1-%d", iMoiB1); sSD += ssw; }
			if (m_BktMaxSDB >= 2) { ssw.Format("/B2-%d", iMoiB2); sSD += ssw; }
			if (m_BktMaxSDB >= 3) { ssw.Format("/B3-%d", iMoiB3); sSD += ssw; }
			if (m_BktMaxSDB >= 4) { ssw.Format("/B4-%d", iMoiB4); sSD += ssw; }
			if (m_BktMaxSDB >= 5) { ssw.Format("/B5-%d", iMoiB5); sSD += ssw; }
			if (m_BktMaxSDB >= 6) { ssw.Format("/B6-%d", iMoiB6); sSD += ssw; }
			if (m_BktMaxSDB >= 7) { ssw.Format("/B7-%d", iMoiB7); sSD += ssw; }
			*/

			sVnau.Format("VNAU%d/%s%s%s/%s",m_trancheno2,MakeStrDate(datevnau,FALSE),
				stok.Left(5),stok.Right(5),sSD);
			// Lancement de la requete de mise a jour sur le troncon courant
			pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 2);
			m_VnauList.Add ((CObject*)pQuotaVnau);

			if (bt2)
			{
				((CCTAApp*)APP)->SetTrancheSentFlag (false, m_trancheno2, m_lDateDep);
				pTrancheView->SetSentFlag (m_trancheno2, m_lDateDep, 1);
				bt2 = FALSE;
			}

			pLegView->SetSentFlag (m_trancheno2, stok.Left(5), m_lDateDep, 1);

			if (bFirst)
			{
				((CCTAApp*)APP)->SetLegSentFlag (false, m_trancheno2, sLegsOrig, m_lDateDep);
				sCmpt = "A";
				/*
				MajAuth(m_trancheno2, sLegsOrig, sCmpt, iMoiA0, iMoiA1, iMoiA2, iMoiA3, 
					iMoiA4, iMoiA5, iMoiA6, 0);*/

				MajAuthSD(m_trancheno2, sLegsOrig, sCmpt);

				sCmpt = "B";
				/*
				MajAuth(m_trancheno2, sLegsOrig, sCmpt, iMoiB0, iMoiB1, iMoiB2, iMoiB3,
					iMoiB4, iMoiB5, iMoiB6, iMoiB7);
				*/
				MajAuthSD(m_trancheno2, sLegsOrig, sCmpt);
			}
		}

		if (bSCIAFlagModif || bSCIBFlagModif /*|| bAllFlagModif*/)
		{   

			/*
			// Lecture des valeurs MOI
			LectureSciSI(iMoiA0, iMoiA1, iMoiA2, iMoiA3, iMoiA4, iMoiA5, iMoiA6, iMoiA7,
				iMoiB0, iMoiB1, iMoiB2, iMoiB3, iMoiB4, iMoiB5, iMoiB6, iMoiB7);
			*/

			// Constitution du VNAS
			if (bSCIAFlagModif /*|| bAllFlagModif*/)
			{
				// Constitution de la chaîne VNAS (VNASN°Tranche/DateOD/AG-valeur/A1-valeur/...

				std::vector<QuotasValues>& listSIScxA = m_quotas_view_si_scx_a._list_ctrl.Datas();
				std::vector<QuotasValues>::iterator it;

				// Les autorisations VNAS commence par le SCG
				// HRI, le 15/08/2015
				CString sTemp;
				m_IDC_LABEL_SI_A_TOTAL_MOI.GetWindowText(sTemp); //NCH
				int value = atoi(sTemp);
				sSI.Format("AG-%d", value);
				for( it = listSIScxA.begin(); it != listSIScxA.end(); ++it)
				{
					QuotasValues& values = (*it);
					int nestLevel = values._nest_level._value;
					value = values._moi._value;
					// HRI, 13/08/2015 : le SCG n'est pas le bucket 0 : il est dans une EditBox
					//if (nestLevel == 0)
					//{
					//	sSI.Format("AG-%d", value);
					//}
					//else
					if (nestLevel > 0)
					{
						ssw.Format("/A%d-%d", nestLevel, value);
						sSI += ssw;
					}
				}

/*
				sSI.Format ("AG-%d", iMoiA0);
				if (m_SciMaxSIA >= 1) { ssw.Format("/A1-%d", iMoiA1); sSI += ssw; }
				if (m_SciMaxSIA >= 2) { ssw.Format("/A2-%d", iMoiA2); sSI += ssw; }
				if (m_SciMaxSIA >= 3) { ssw.Format("/A3-%d", iMoiA3); sSI += ssw; }
				if (m_SciMaxSIA >= 4) { ssw.Format("/A4-%d", iMoiA4); sSI += ssw; }
				if (m_SciMaxSIA >= 5) { ssw.Format("/A5-%d", iMoiA5); sSI += ssw; }
				if (m_SciMaxSIA >= 6) { ssw.Format("/A6-%d", iMoiA6); sSI += ssw; }
				if (m_SciMaxSIA >= 7) { ssw.Format("/A7-%d", iMoiA7); sSI += ssw; }
*/
				
				sVnau.Format("VNAS%d/%s%s%s/%s",m_trancheno1,MakeStrDate(datevnau,FALSE),
					stok.Left(5),stok.Right(5),sSI);

				pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 1);
				m_VnauList.Add ((CObject*)pQuotaVnau);
				sCmpt = "A";
				if (bFirst)
					MajAuthSciSI(m_trancheno1, sLegsOrig, sCmpt);
					/*
					MajAuthSci(m_trancheno1, sLegsOrig, sCmpt, iMoiA0, iMoiA1, iMoiA2, iMoiA3, 
					iMoiA4, iMoiA5, iMoiA6, iMoiA7);
					*/
			}
			if (bSCIBFlagModif /*|| bAllFlagModif*/)
			{
				std::vector<QuotasValues>& listSIScxB = m_quotas_view_si_scx_b._list_ctrl.Datas();
				std::vector<QuotasValues>::iterator it;

				// Les autorisations commence par le SCG
				// HRI, le 15/08/2015
				CString sTemp;
				m_IDC_LABEL_SI_B_TOTAL_MOI.GetWindowText(sTemp); //NCH
				int value = atoi(sTemp);
				sSI.Format("BG-%d", value);
				for(it = listSIScxB.begin(); it != listSIScxB.end(); ++it)
				{
					QuotasValues& values = (*it);
					int nestLevel = values._nest_level._value;
					value = values._moi._value;

					// HRI, 13/08/2015 : le SCG n'est pas le bucket 0 : il est dans une EditBox
					//if (nestLevel == 0)
					//{
					//	sSI.Format ("BG-%d", value);
					//}
					//else
					if (nestLevel > 0)
					{
						ssw.Format("/B%d-%d", nestLevel, value);
						sSI += ssw;
					}
				}

				/*
				sSI.Format ("BG-%d", iMoiB0);
				if (m_SciMaxSIB >= 1) { ssw.Format("/B1-%d", iMoiB1); sSI += ssw; }
				if (m_SciMaxSIB >= 2) { ssw.Format("/B2-%d", iMoiB2); sSI += ssw; }
				if (m_SciMaxSIB >= 3) { ssw.Format("/B3-%d", iMoiB3); sSI += ssw; }
				if (m_SciMaxSIB >= 4) { ssw.Format("/B4-%d", iMoiB4); sSI += ssw; }
				if (m_SciMaxSIB >= 5) { ssw.Format("/B5-%d", iMoiB5); sSI += ssw; }
				if (m_SciMaxSIB >= 6) { ssw.Format("/B6-%d", iMoiB6); sSI += ssw; }
				if (m_SciMaxSIB >= 7) { ssw.Format("/B7-%d", iMoiB7); sSI += ssw; }
				*/

				sVnau.Format("VNAS%d/%s%s%s/%s",m_trancheno1,MakeStrDate(datevnau,FALSE),
					stok.Left(5),stok.Right(5),sSI);
				pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 1);
				m_VnauList.Add ((CObject*)pQuotaVnau);
				sCmpt = "B";
				if (bFirst)
					/// CLE
					MajAuthSciSI(m_trancheno1, sLegsOrig, sCmpt);
					/*
					MajAuthSci(m_trancheno1, sLegsOrig, sCmpt, iMoiB0, iMoiB1, iMoiB2, iMoiB3,
					iMoiB4, iMoiB5, iMoiB6, iMoiB7);
					*/
			}
			if (bt1)
			{
				((CCTAApp*)APP)->SetTrancheSentFlag (true, m_trancheno1, m_lDateDep);
				pTrancheView->SetScxSentFlag (m_trancheno1, m_lDateDep, 1);
				bt1 = FALSE;
			}
			if (bFirst)
				((CCTAApp*)APP)->SetLegSentFlag (true, m_trancheno1, sLegsOrig, m_lDateDep);
			pLegView->SetScxSentFlag (m_trancheno1, stok.Left(5), m_lDateDep, 1);
		}

		if (bSCDAFlagModif || bSCDBFlagModif /*|| bAllFlagModif*/)
		{   

			// Lecture des valeurs MOI
			/*
			LectureSciSD(iMoiA0, iMoiA1, iMoiA2, iMoiA3, iMoiA4, iMoiA5, iMoiA6, iMoiA7,
				iMoiB0, iMoiB1, iMoiB2, iMoiB3, iMoiB4, iMoiB5, iMoiB6, iMoiB7);
			*/


			// Constitution du VNAS
			if (bSCDAFlagModif /*|| bAllFlagModif*/)
			{

				std::vector<QuotasValues>& list = m_quotas_view_sd_scx_a._list_ctrl.Datas();
				std::vector<QuotasValues>::iterator it;
				// Les autorisations commence par le SCG
				// HRI, le 15/08/2015
				CString sTemp;
				m_IDC_LABEL_SD_A_TOTAL_MOI.GetWindowText(sTemp); //NCH
				int value = atoi(sTemp);
				sSI.Format("AG-%d", value);
				for(it = list.begin(); it != list.end(); ++it)
				{
					QuotasValues& values = (*it);
					int nestLevel = values._nest_level._value;
					int value = values._moi._value;

					// HRI, 13/08/2015 : le SCG n'est pas le bucket 0 : il est dans une EditBox
					//if (nestLevel == 0)
					//{
					//	sSI.Format("AG-%d", value);
					//}
					//else
					if (nestLevel > 0)
					{
						ssw.Format("/A%d-%d", nestLevel, value);
						sSI += ssw;
					}
				}

				/*
				sSI.Format ("AG-%d", iMoiA0);
				if (m_SciMaxSDA >= 1) { ssw.Format("/A1-%d", iMoiA1); sSI += ssw; }
				if (m_SciMaxSDA >= 2) { ssw.Format("/A2-%d", iMoiA2); sSI += ssw; }
				if (m_SciMaxSDA >= 3) { ssw.Format("/A3-%d", iMoiA3); sSI += ssw; }
				if (m_SciMaxSDA >= 4) { ssw.Format("/A4-%d", iMoiA4); sSI += ssw; }
				if (m_SciMaxSDA >= 5) { ssw.Format("/A5-%d", iMoiA5); sSI += ssw; }
				if (m_SciMaxSDA >= 6) { ssw.Format("/A6-%d", iMoiA6); sSI += ssw; }
				if (m_SciMaxSDA >= 7) { ssw.Format("/A7-%d", iMoiA7); sSI += ssw; }*/

				sVnau.Format("VNAS%d/%s%s%s/%s",m_trancheno2,MakeStrDate(datevnau,FALSE),
					stok.Left(5),stok.Right(5),sSI);

				pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 1);
				m_VnauList.Add ((CObject*)pQuotaVnau);
				sCmpt = "A";
				if (bFirst)
					/// CLE
					MajAuthSciSD(m_trancheno2, sLegsOrig, sCmpt);
					/*
					MajAuthSci(m_trancheno2, sLegsOrig, sCmpt, iMoiA0, iMoiA1, iMoiA2, iMoiA3, 
					iMoiA4, iMoiA5, iMoiA6, iMoiA7);
					*/
			}
			if (bSCDBFlagModif /*|| bAllFlagModif*/)
			{
				// HRI, 13/08/2015 : erreur sur le tableau concerné (prendre sd au lieu de si)
				//std::vector<QuotasValues>& list = m_quotas_view_si_scx_b._list_ctrl.Datas();
				std::vector<QuotasValues>& list = m_quotas_view_sd_scx_b._list_ctrl.Datas();
				std::vector<QuotasValues>::iterator it;
				// Les autorisations commence par le SCG
				// HRI, le 15/08/2015
				CString sTemp;
				m_IDC_LABEL_SD_B_TOTAL_MOI.GetWindowText(sTemp); //NCH
				int value = atoi(sTemp);
				sSI.Format("BG-%d", value);
				for(it = list.begin(); it != list.end(); ++it)
				{
					 QuotasValues& values = (*it);
					 int nestLevel = values._nest_level._value;
					 int value = values._moi._value;

					// HRI, 13/08/2015 : le SCG n'est pas le bucket 0 : il est dans une EditBox
					//if (nestLevel == 0)
					//{
					//	sSI.Format("AG-%d", value);
					//}
					//else
					if (nestLevel > 0)
					{
						ssw.Format("/B%d-%d", nestLevel, value);
						sSI += ssw; 
					}
				}

				/*
				sSI.Format ("BG-%d", iMoiB0);
				if (m_SciMaxSDB >= 1) { ssw.Format("/B1-%d", iMoiB1); sSI += ssw; }
				if (m_SciMaxSDB >= 2) { ssw.Format("/B2-%d", iMoiB2); sSI += ssw; }
				if (m_SciMaxSDB >= 3) { ssw.Format("/B3-%d", iMoiB3); sSI += ssw; }
				if (m_SciMaxSDB >= 4) { ssw.Format("/B4-%d", iMoiB4); sSI += ssw; }
				if (m_SciMaxSDB >= 5) { ssw.Format("/B5-%d", iMoiB5); sSI += ssw; }
				if (m_SciMaxSDB >= 6) { ssw.Format("/B6-%d", iMoiB6); sSI += ssw; }
				if (m_SciMaxSDB >= 7) { ssw.Format("/B7-%d", iMoiB7); sSI += ssw; }
				*/

				sVnau.Format("VNAS%d/%s%s%s/%s",m_trancheno2,MakeStrDate(datevnau,FALSE),
					stok.Left(5),stok.Right(5),sSI);

				pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 1);
				m_VnauList.Add ((CObject*)pQuotaVnau);
				sCmpt = "B";
				if (bFirst)
					/// CLE
					MajAuthSciSD(m_trancheno2, sLegsOrig, sCmpt);
					/*
					MajAuthSci(m_trancheno2, sLegsOrig, sCmpt, iMoiB0, iMoiB1, iMoiB2, iMoiB3,
					iMoiB4, iMoiB5, iMoiB6, iMoiB7);
					*/
			}
			if (bt2)
			{
				((CCTAApp*)APP)->SetTrancheSentFlag (true, m_trancheno2, m_lDateDep);
				pTrancheView->SetScxSentFlag (m_trancheno2, m_lDateDep, 1);
				bt2 = FALSE;
			}
			if (bFirst)
				((CCTAApp*)APP)->SetLegSentFlag (true, m_trancheno2, sLegsOrig, m_lDateDep);
			pLegView->SetScxSentFlag (m_trancheno2, stok.Left(5), m_lDateDep, 1);
		}
		tok = strtok (NULL, delim);
		bFirst = FALSE;
	}
	free (pbuf);
	bSIFlagModif = FALSE;
	bSDFlagModif = FALSE;
	bSCIAFlagModif = FALSE;
	bSCDAFlagModif = FALSE;
	bSCIBFlagModif = FALSE;
	bSCDBFlagModif = FALSE;
	bFlagModifManuelle = FALSE;
	GrisageVnauVnas();
}

void CQuotasBiTrancheDlg::PurgeVnauList()
{
	int i;
	CQuotaVnau* pQuotaVnau;
	for (i = 0; i < m_VnauList.GetSize(); i++)
	{
		pQuotaVnau = (CQuotaVnau*)m_VnauList.GetAt(i);
		if (pQuotaVnau)
			delete pQuotaVnau;
	}
	m_VnauList.RemoveAll();
}

void CQuotasBiTrancheDlg::InitSendVnau()
{
	((CButton *)GetDlgItem(IDC_QR_QUITTER))->EnableWindow(FALSE);
	CWnd* pWnd = GetDlgItem (IDC_NBVNAU);
	CString sMess;
	if (m_lRrdIndex >= 0)
		sMess = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_MAJ_AUTH);
	else
		sMess.Format ("VNAU 0 / %d", m_VnauList.GetSize());
	pWnd->SetWindowText (sMess);

}

void CQuotasBiTrancheDlg::FinSendVnau()
{
	((CButton *)GetDlgItem(IDC_QR_QUITTER))->EnableWindow(TRUE);
	CWnd* pWnd = GetDlgItem (IDC_NBVNAU);
	pWnd->SetWindowText ("");
}

void CQuotasBiTrancheDlg::SendNextVnau()
{
	int i;
	CQuotaVnau* pQuotaVnau;
	for (i = 0; i < m_VnauList.GetSize(); i++)
	{
		pQuotaVnau = (CQuotaVnau*)m_VnauList.GetAt(i);
		if (pQuotaVnau && !pQuotaVnau->m_bSent)
		{
			VNAU_MSG_TYPE tyVnau;
			if (pQuotaVnau->m_nature == 4)
				tyVnau = VNAU_NAT1_TYPE;
			else 
				tyVnau = VNAU_NAT2_TYPE;

			((CCTAApp*)APP)->GetResComm()->SendVnau (this, pQuotaVnau->m_mess, FALSE, pQuotaVnau->m_bCrossMidnight, tyVnau);
			pQuotaVnau->m_bSent = TRUE;
			CWnd* pWnd = GetDlgItem (IDC_NBVNAU);
			CString sMess;
			sMess.Format ("VNAU : %d / %d", i+1, m_VnauList.GetSize());
			pWnd->SetWindowText (sMess);
			return;
		}
	}
	// Si on est ici, c'est que tous les VNAU ont été envoyé, on fait donc le ménage.
	PurgeVnauList();
	bVnlDeControle = TRUE;
	StartSendVnl (sLastTroncon); 
}

BOOL CQuotasBiTrancheDlg::MessageNbVnauVnas(const CString& sListTroncon) 
{
	CString sMess;
	char* pbuf;
	char* tok;
	CString stok;
	char delim[4];
	strcpy (delim, ",");
	pbuf =(char*)malloc (sListTroncon.GetLength()+1);
	strcpy (pbuf, sListTroncon);
	tok = strtok (pbuf,delim);
	int ileg = 0;
	int iVnau = 0;
	int iVnas = 0;
	int iModif = 0; 
	while (tok)
	{
		ileg++;
		stok = tok;
		tok = strtok (NULL, delim);
	}
	if ( bSIFlagModif )
		iModif++;
	if ( bSDFlagModif )
		iModif++;
	iVnau = ileg * iModif;
	iModif = 0;
	if ( bSCIAFlagModif )
		iModif++;
	if ( bSCDAFlagModif )
		iModif++;
	if ( bSCIBFlagModif )
		iModif++;
	if ( bSCDBFlagModif )
		iModif++;
	iVnas = ileg * iModif;
	CString sVnauSend = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VNAU_VNAS_SENT);
	sMess.Format(sVnauSend, iVnau, iVnas );
	free (pbuf);
	if (AfxGetMainWnd()->MessageBox(sMess, AfxGetAppName(),MB_OKCANCEL | MB_ICONQUESTION | MB_APPLMODAL) == IDCANCEL)
		return FALSE;
	return TRUE;
}
BOOL CQuotasBiTrancheDlg::CtrlSousResaOK()
{
	if (!m_FlagSousResa)
		return TRUE;
	else
	{
		CString sVnauSend = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CONFIRM_SOUSRESA);
		if (AfxMessageBox( sVnauSend, MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION )  == IDOK)
			return TRUE;
		else
			return FALSE;
	}
}

void CQuotasBiTrancheDlg::OnUnTroncon() 
{
	CString sVnau,sSI,sSD;
	CString sListTroncon;
	if (!CtrlSousResaOK())
		return;
	m_ComboTroncon.GetWindowText(sListTroncon);
	if (!MessageNbVnauVnas(sListTroncon))
		return;
	InitSendVnau();
	BuildVnau(sListTroncon);
	SendNextVnau();
}

void CQuotasBiTrancheDlg::OnTousTroncon() 
{
	CString sListTroncon;
	BOOL bRep;
	// Lancement de la requete de mise a jour sur tous les troncons
	if (!CtrlSousResaOK())
		return;
	bRep = GetSelTroncon("TOUS", sListTroncon);
	if (bRep)
	{
		bAllFlagModif = TRUE;
		InitSendVnau();
		BuildVnau(sListTroncon);
		bAllFlagModif = FALSE;
		SendNextVnau();
	}
}
void CQuotasBiTrancheDlg::OnSelTroncon() 
{
	CString sListTroncon;
	BOOL bRep;
	// Lancement de la requete de mise a jour sur les troncons selectionnés
	bRep = GetSelTroncon("SEL", sListTroncon);
	CString sCurTroncon;
	m_ComboTroncon.GetWindowText(sCurTroncon);
	CString sTestTroncon = sListTroncon + "," + sCurTroncon;
	if (!ControlCrossFrontier (sTestTroncon))
	{
		AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NOT_MIX_COPY), 
			MB_OK | MB_APPLMODAL | MB_ICONSTOP );
		return;
	}
	if (!CtrlSousResaOK())
		return;
	if (bRep)
	{
		bAllFlagModif = TRUE;
		InitSendVnau();
		BuildVnau(sListTroncon);
		bAllFlagModif = FALSE;
		SendNextVnau();
	}
}

bool CQuotasBiTrancheDlg::ControlCrossFrontier (const CString& sListTroncon)
{
	char* pbuf;
	char* tok;
	CString stok, orig, dest;
	char delim[4];
	strcpy (delim, ",");
	bool bFrenchOnly = true;
	bool bNonFrOnly = true;
	pbuf =(char*)malloc (sListTroncon.GetLength()+1);
	strcpy (pbuf, sListTroncon);
	tok = strtok (pbuf,delim);
	while (tok)
	{
		stok = tok;
		orig = stok.Left(5);
		dest = stok.Right(5);
		if ((orig.Left(2) == "FR") && (dest.Left(2) == "FR"))
			bNonFrOnly = false;
		else
			bFrenchOnly = false;
		tok = strtok (NULL, delim);
	}
	free (pbuf);
	return (bNonFrOnly || bFrenchOnly);
}

void CQuotasBiTrancheDlg::OnQuitter() 
{
	if ( bFlagModifManuelle && (bSIFlagModif || bSDFlagModif || bSCIAFlagModif || bSCDAFlagModif || bSCIBFlagModif || bSCDBFlagModif))
	{
		if ( AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_MODIF_DONE),
			AfxGetAppName(), MB_OKCANCEL | MB_APPLMODAL | MB_ICONEXCLAMATION) == IDCANCEL )
			return;
	}
	//    OnCancel();
	CDialog::OnCancel();
}

void CQuotasBiTrancheDlg::OnKeepEcart() 
{
	m_bOvbAdjustFlag = (((CButton *)GetDlgItem(IDC_KEEP_ECART))->GetCheck());
}

void CQuotasBiTrancheDlg::OnCancel() 
{	
	OnQuitter();
}


// SI SCI Cmpt A : rajout HRICHARD (le SCG international A est dans une EditBox)
// On reprend une partie du code de l'ancien SATC qu'on adapte
void CQuotasBiTrancheDlg::OnMajSCIAG() 
{
	ControleScIAG ();
}

// SI SCI Cmpt B : rajout HRICHARD (le SCG international B est dans une EditBox)
// On reprend une partie du code de l'ancien SATC qu'on adapte
void CQuotasBiTrancheDlg::OnMajSCIBG() 
{
	ControleScIBG ();
}

// SD SCI Cmpt A : rajout HRICHARD (le SCG domestique A est dans une EditBox)
// On reprend une partie du code de l'ancien SATC qu'on adapte
void CQuotasBiTrancheDlg::OnMajSCDAG()
{
	ControleScDAG ();
}

// SD Cmpt B
// SD SCI Cmpt A : rajout HRICHARD (le SCG domestique A est dans une EditBox)
// On reprend une partie du code de l'ancien SATC qu'on adapte
void CQuotasBiTrancheDlg::OnMajSCDBG()
{
	ControleScDBG ();
}

// -----------------------------------------------------------------------------
// ControleScIAC : SCG A de la tranche internationaleuuh, c'est la lutte finale
// Réalisé par déduction de la même méthode dans l'ancien SATC
// -----------------------------------------------------------------------------
bool CQuotasBiTrancheDlg::ControleScIAG()
{
	int sumAuthEtanche = 0;
	int sumDispEtanche = 0; 
	CString sg, s0;
	int isg, is0;

	CString capaA;

	m_CapaA.GetWindowText(capaA);
	int intCapaA = atoi(capaA);
	// Le SCG est ici dans le composant IDC_LABEL_SI_A_TOTAL_MOI
	// Pourquoi a-t-il changé de nom par rapport à l'ancien SATC (mystère) ?

	m_IDC_LABEL_SI_A_TOTAL_MOI.GetWindowText(sg);
	isg = atoi (sg);    // valeur autorisation de la CC-1

	if (m_valeurPrecAG_I == isg) // on a rien changé
	{
		return false;
	}

	// Liste des CC => elle permet de retrouver la CC-0
	QuotasAutorisationsListCtrl* listCc = NULL;
	QuotasAutorisationsListCtrl* listSc = NULL;
	listCc = &m_quotas_view_si_cc_a._list_ctrl;
	is0 = listCc->Datas()[0]._moi._value;  // autorisation en bucket 0.

	listSc = &m_quotas_view_si_scx_a._list_ctrl;
	raboterSCX(listSc, isg);
	//listSc->RedrawItems(0, listSc->MaxIndexation());

	if (isg > is0)				// la SCG ne peut être supérieure à la CC0
	{
		sg.Format ("%d", (is0 > intCapaA) ? intCapaA : is0);
		m_IDC_LABEL_SI_A_TOTAL_MOI.SetWindowText(sg);
		return false;
	}
	SommeGlobaleSIA (sumAuthEtanche, sumDispEtanche);

	bool bOK = true;
	if (isg < sumAuthEtanche) // la somme des autorisations en SCI étanche doit être inférieure au sous-contingent
	{
		bOK = false;
	}

	if (!bOK)
	{
		sg.Format ("%d", m_valeurPrecAG_I);
		m_IDC_LABEL_SI_A_TOTAL_MOI.SetWindowText(sg);
		return false; 
	}
	// OK pour le SCG
	m_valeurPrecAG_I = isg;
	bSCIAFlagModif = TRUE;
	bFlagModifManuelle = TRUE;

	return true;
}


void CQuotasBiTrancheDlg::raboterSCX(QuotasAutorisationsListCtrl* ctr, long maValue){

	long authMax = 0;
	long value = 0;

	for (int i = 1 ; i < ctr->MaxIndexation(); i++)
	{
		QuotasValue* moi = ctr->GetValue(Quotas::MOI, i);

		value = (moi->_value);

		if (value > maValue)
			ctr->SetValue(Quotas::MOI, i, maValue);

	}

	ctr->RedrawItems(0, ctr->MaxIndexation());
}

// -----------------------------------------------------------------------------
// ControleScIAC : SCG B de la tranche internationaleuuh, c'est la lutte finale
// Réalisé par déduction de la même méthode dans l'ancien SATC
// -----------------------------------------------------------------------------
bool CQuotasBiTrancheDlg::ControleScIBG()
{
	int sumAuthEtanche = 0;
	int sumDispEtanche = 0; 
	CString sg, s0;
	int isg, is0;


	CString capaB;

	m_CapaB.GetWindowText(capaB);
	int intCapaB = atoi(capaB);
	// Le SCG est ici dans le composant IDC_LABEL_SI_B_TOTAL_MOI
	// Pourquoi a-t-il changé de nom par rapport à l'ancien SATC (mystère) ?

	m_IDC_LABEL_SI_B_TOTAL_MOI.GetWindowText(sg);
	isg = atoi (sg);    // valeur autorisation de la CC-1

	if (m_valeurPrecBG_I == isg) // on a rien changé
	{
		return false;
	}

	// Liste des CC => elle permet de retrouver la CC-0
	QuotasAutorisationsListCtrl* listCc = NULL;
	QuotasAutorisationsListCtrl* listSc = NULL;
	listCc = &m_quotas_view_si_cc_b._list_ctrl;
	is0 = listCc->Datas()[0]._moi._value;  // autorisation en bucket 0.

	listSc = &m_quotas_view_si_scx_b._list_ctrl;
	raboterSCX(listSc, isg);
	//listSc->RedrawItems(0, listSc->MaxIndexation());

	if (isg > is0)				// la SCG ne peut être supérieure à la CC0
	{
		sg.Format ("%d", (is0 > intCapaB) ? intCapaB : is0);
		m_IDC_LABEL_SI_B_TOTAL_MOI.SetWindowText(sg);
		return false;
	}
	SommeGlobaleSIB (sumAuthEtanche, sumDispEtanche);

	bool bOK = true;
	if (isg < sumAuthEtanche) // la somme des autorisations en SCI étanche doit être inférieure au sous-contingent
	{
		bOK = false;
	}

	if (!bOK)
	{
		sg.Format ("%d", m_valeurPrecBG_I);
		m_IDC_LABEL_SI_B_TOTAL_MOI.SetWindowText(sg);
		return false; 
	}
	// OK pour le SCG
	m_valeurPrecBG_I = isg;
	bSCIBFlagModif = TRUE;
	bFlagModifManuelle = TRUE;

	return true;

}

bool CQuotasBiTrancheDlg::ControleScDAG()
{
	int sumAuthEtanche = 0;
	int sumDispEtanche = 0; 
	CString sg, s0;
	int isg, is0;


	// Le SCG est ici dans le composant IDC_LABEL_SD_A_TOTAL_MOI
	// Pourquoi a-t-il changé de nom par rapport à l'ancien SATC (mystère) ?

	m_IDC_LABEL_SD_A_TOTAL_MOI.GetWindowText(sg);
	isg = atoi (sg);    // valeur autorisation de la CC-1

	if (m_valeurPrecAG_D == isg) // on a rien changé
	{
		return false;
	}

	CString capaA;

	m_CapaA.GetWindowText(capaA);
	int intCapaA = atoi(capaA);

	// Liste des CC => elle permet de retrouver la CC-0
	QuotasAutorisationsListCtrl* listCc = NULL;
	QuotasAutorisationsListCtrl* listSc = NULL;
	listCc = &m_quotas_view_sd_cc_a._list_ctrl;
	is0 = listCc->Datas()[0]._moi._value;  // autorisation en bucket 0.

	listSc = &m_quotas_view_sd_scx_a._list_ctrl;
	raboterSCX(listSc, isg);
	//listSc->RedrawItems(0, listSc->MaxIndexation());

	if (isg > is0)				// la SCG ne peut être supérieure à la CC0
	{
		sg.Format ("%d", (is0 > intCapaA) ? intCapaA : is0);
		m_IDC_LABEL_SD_A_TOTAL_MOI.SetWindowText(sg);
		return false;
	}
	SommeGlobaleSDA (sumAuthEtanche, sumDispEtanche);

	bool bOK = true;
	if (isg < sumAuthEtanche) // la somme des autorisations en SCI étanche doit être inférieure au sous-contingent
	{
		bOK = false;
	}

	if (!bOK)
	{
		sg.Format ("%d", m_valeurPrecAG_D);
		m_IDC_LABEL_SD_A_TOTAL_MOI.SetWindowText(sg);
		return false; 
	}
	// OK pour le SCG
	m_valeurPrecAG_D = isg;
	bSCDAFlagModif = TRUE;
	bFlagModifManuelle = TRUE;
	return true;
}


bool CQuotasBiTrancheDlg::ControleScDBG()
{
	int sumAuthEtanche = 0;
	int sumDispEtanche = 0; 
	CString sg, s0;
	int isg, is0;

	CString capaB;

	m_CapaB.GetWindowText(capaB);
	int intCapaB = atoi(capaB);
	// Le SCG est ici dans le composant IDC_LABEL_SD_B_TOTAL_MOI
	// Pourquoi a-t-il changé de nom par rapport à l'ancien SATC (mystère) ?

	m_IDC_LABEL_SD_B_TOTAL_MOI.GetWindowText(sg);
	isg = atoi (sg);    // valeur autorisation de la CC-1

	if (m_valeurPrecBG_D == isg) // on a rien changé
	{
		return false;
	}

	// Liste des CC => elle permet de retrouver la CC-0
	QuotasAutorisationsListCtrl* listCc = NULL;
	QuotasAutorisationsListCtrl* listSc = NULL;
	listCc = &m_quotas_view_sd_cc_b._list_ctrl;
	is0 = listCc->Datas()[0]._moi._value;  // autorisation en bucket 0.

	listSc = &m_quotas_view_sd_scx_b._list_ctrl;
	raboterSCX(listSc, isg);
	//listSc->RedrawItems(0, listSc->MaxIndexation());

	if (isg > is0)				// la SCG ne peut être supérieure à la CC0
	{
		sg.Format ("%d", (is0 > intCapaB) ? intCapaB : is0);
		m_IDC_LABEL_SD_B_TOTAL_MOI.SetWindowText(sg);
		return false;
	}
	SommeGlobaleSDB (sumAuthEtanche, sumDispEtanche);

	bool bOK = true;
	if (isg < sumAuthEtanche) // la somme des autorisations en SCI étanche doit être inférieure au sous-contingent
	{
		bOK = false;
	}

	if (!bOK)
	{
		sg.Format ("%d", m_valeurPrecBG_D);
		m_IDC_LABEL_SD_B_TOTAL_MOI.SetWindowText(sg);
		return false; 
	}
	// OK pour le SCG
	m_valeurPrecBG_D = isg;
	bSCDBFlagModif = TRUE;
	bFlagModifManuelle = TRUE;

	return true;
}