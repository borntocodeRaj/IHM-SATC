// QuotasRhealysDlg.cpp : implementation file
//
#include "StdAfx.h"

#include "QuotaEdit.h"

#include "QuotasRhealysDlg.h"
#include "VnlEngineView.h"

#include "ResaRailView.h"
#include "TrancheView.h"
#include "ResTracer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CQuotasRhealysDlg* CQuotasRhealysDlg::m_pInstance = NULL; 

CQuotasRhealysDlg* CQuotasRhealysDlg::getInstance(YmIcTrancheLisDom* pTranche)
{
	int i = 0;

	if (m_pInstance == NULL && pTranche != NULL)
	{
		m_pInstance = new CQuotasRhealysDlg(pTranche);
		
		//A Compartments ------------------------------------
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_si_a));//SRE 83311 - Refactor using vector to make navigation easier
		m_pInstance->m_quotas_view_si_a.setListId(i);//SRE 76064 - Affect an ID to lists
		m_pInstance->m_quotas_view_si_a._list_ctrl.beingEdited = false;//SRE 83311 - Workaround to catch the enter key in our CEdit (see PreTranslateMssage() function below)
		i++;

		if(!((CCTAApp*)APP)->GetRhealysFlag()){
			// NBN
			m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_si_sca));//SRE 83311 - Refactor using vector to make navigation easier
			m_pInstance->m_quotas_view_si_sca.setListId(i);//SRE 76064 - Affect an ID to lists
			m_pInstance->m_quotas_view_si_sca._list_ctrl.beingEdited = false;//SRE 83311 - Workaround to catch the enter key in our CEdit (see PreTranslateMssage() function below)
			i++;
		}

		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_sd_a));
		m_pInstance->m_quotas_view_sd_a.setListId(i);//to know which lists triggered an event.
		m_pInstance->m_quotas_view_sd_a._list_ctrl.beingEdited = false;
		i++;

		if(!((CCTAApp*)APP)->GetRhealysFlag()){
			// NBN
			m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_sd_sca));
			m_pInstance->m_quotas_view_sd_sca.setListId(i);//to know which lists triggered an event.
			m_pInstance->m_quotas_view_sd_sca._list_ctrl.beingEdited = false;
			i++;
		}

		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_sdb_a));
		m_pInstance->m_quotas_view_sdb_a.setListId(i);
		m_pInstance->m_quotas_view_sdb_a._list_ctrl.beingEdited = false;
		i++;

		// NBN
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_sdb_sca));
		m_pInstance->m_quotas_view_sdb_sca.setListId(i);
		m_pInstance->m_quotas_view_sdb_sca._list_ctrl.beingEdited = false;
		i++;

		//B Compartments ---------------------------------------------------------
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_si_b));
		m_pInstance->m_quotas_view_si_b.setListId(i);
		m_pInstance->m_quotas_view_si_b._list_ctrl.beingEdited = false;
		i++;

		if(!((CCTAApp*)APP)->GetRhealysFlag()){
			// NBN
			m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_si_scb));
			m_pInstance->m_quotas_view_si_scb.setListId(i);
			m_pInstance->m_quotas_view_si_scb._list_ctrl.beingEdited = false;
			i++;
		}

		// NBN
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_sd_b));
		m_pInstance->m_quotas_view_sd_b.setListId(i);
		m_pInstance->m_quotas_view_sd_b._list_ctrl.beingEdited = false;
		i++;

		if(!((CCTAApp*)APP)->GetRhealysFlag()){
			m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_sd_scb));
			m_pInstance->m_quotas_view_sd_scb.setListId(i);
			m_pInstance->m_quotas_view_sd_scb._list_ctrl.beingEdited = false;
			i++;
		}
		
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_sdb_b));
		m_pInstance->m_quotas_view_sdb_b.setListId(i);
		m_pInstance->m_quotas_view_sdb_b._list_ctrl.beingEdited = false;
		i++;

		// NBN
		m_pInstance->orderedView.push_back(&(m_pInstance->m_quotas_view_sdb_scb));
		m_pInstance->m_quotas_view_sdb_scb.setListId(i);
		m_pInstance->m_quotas_view_sdb_scb._list_ctrl.beingEdited = false;
		
	}

	return m_pInstance;
}

void CQuotasRhealysDlg::deleteInstance()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CString CQuotasRhealysDlg::GetFirstLine(CString& msg)
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
// CQuotasRhealysDlg dialog

CQuotasRhealysDlg::CQuotasRhealysDlg()
: CDialog(CQuotasRhealysDlg::IDD, NULL)
{
	//{{AFX_DATA_INIT(CQuotasRhealysDlg)
	m_NumeroTrain = _T("");
	m_FlagTroncon = FALSE;
	//}}AFX_DATA_INIT
	bFlagAppel = FALSE;
	m_szTitle = "";
	m_startdate = CTime::GetCurrentTime();
	m_lRrdIndex = -1;
	m_VnlCount = 0;
	bVnlDeControle = FALSE;
	controlerCC0EgalCC1 = FALSE;
	CCTADoc *pDoc = ((CCTAApp*)APP)->GetDocument();
	controlerCC0EgalCC1 = pDoc->m_pGlobalSysParms->GetControlerCC0EgalCC1();

	m_nbBktVnlA	= 0;  // HRI, 10/08/2015
	m_nbBktVnlB	= 0;  // HRI, 10/08/2015
	
	m_nbScxVnlA = 0;
	m_nbScxVnlB = 0;

	i=10;
}

CQuotasRhealysDlg::CQuotasRhealysDlg(YmIcTrancheLisDom* pTranche)
: CDialog(CQuotasRhealysDlg::IDD, NULL)
{
	//{{AFX_DATA_INIT(CQuotasRhealysDlg)
	m_FlagTroncon = FALSE;
	m_NumeroTrain = pTranche->TrainNo();
	m_Origine     = pTranche->TrancheOrig();
	m_Destination = pTranche->TrancheDest();
	m_lDateDep    = pTranche->DptDate();
	m_sPeakLeg    = pTranche->PeakLeg();
	m_lRrdIndex   = pTranche->LastJX();
	//}}AFX_DATA_INIT

	CLegView* pLegView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetLegView();
	if (pLegView)
	{
		// HRI, 11/08/2015 Anomalie BUGI 70914 (cf SFG Scénario, YI-6129, 
	    // étape 2	A partir de la fenêtre "liste des tranches", l'utilisateur sélectionne 
		//la tranche / date / tronçon  sur laquelle il souhaite travailler
		YmIcLegsDom* pLeg;
		pLeg = pLegView->GetLoadedLeg();
		if (pLeg == NULL )
		{
			pLeg = pLegView->FindFrontier();
		}
		if (pLeg)
		{
			m_sPeakLeg = pLeg->LegOrig();
		}
	}
	RWDate rwBegDate;
	rwBegDate.julian( m_lDateDep );
	CTime m_startdate2(rwBegDate.year(),rwBegDate.month(),rwBegDate.dayOfMonth(),0,0,0);
	m_startdate = m_startdate2;
	bFlagAppel = TRUE;
	m_VnlCount = 0;
	bVnlDeControle = FALSE;
	CCTADoc *pDoc = ((CCTAApp*)APP)->GetDocument();
	controlerCC0EgalCC1 = pDoc->m_pGlobalSysParms->GetControlerCC0EgalCC1();

	m_nbBktVnlA	= 0;  // HRI, 10/08/2015
	m_nbBktVnlB	= 0;  // HRI, 10/08/2015
	//Chercher(); 01/12/12 suppression KDISN il faut le déplacer car initdialog non encore fait
	m_nbScxVnlA = 0;
	m_nbScxVnlB = 0;

	i=0;
}

/*
 * SRE 83311 - Override Pretranslate message to prevent
 * CDialog to catch our VK_ENTER key. Otherwise, we are
 * unable to modify the behaviour of ENTER in our Bascule.
 * In fact, we are directly calling EditNextElement function
 * of the appropriate list to edit our next line.
 */
BOOL CQuotasRhealysDlg::PreTranslateMessage(MSG* pMsg)
{
	//SRE 83311 - Use a workaround, as the GetFocus() trick doesn't work, event if set by hand...
	if (m_quotas_view_si_a._list_ctrl.beingEdited		||	m_quotas_view_sd_a._list_ctrl.beingEdited
		|| m_quotas_view_sdb_a._list_ctrl.beingEdited	||	m_quotas_view_si_b._list_ctrl.beingEdited
		|| m_quotas_view_sdb_sca._list_ctrl.beingEdited	||	m_quotas_view_si_scb._list_ctrl.beingEdited
		|| m_quotas_view_sdb_scb._list_ctrl.beingEdited	||	m_quotas_view_si_sca._list_ctrl.beingEdited
		|| m_quotas_view_sd_scb._list_ctrl.beingEdited	||	m_quotas_view_sd_sca._list_ctrl.beingEdited
		|| m_quotas_view_sd_b._list_ctrl.beingEdited	||	m_quotas_view_sdb_b._list_ctrl.beingEdited)
	
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

CQuotasRhealysDlg::~CQuotasRhealysDlg()
{

}

void CQuotasRhealysDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuotasRhealysDlg)
	//--LIST TRITRANCHE : INTERNATIONAL / DOMESTIQUE / DB
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_TRITRANCHE_SI_A, m_quotas_view_si_a._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_TRITRANCHE_SD_A, m_quotas_view_sd_a._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_TRITRANCHE_SDB_A, m_quotas_view_sdb_a._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_TRITRANCHE_SDB_SCA, m_quotas_view_sdb_sca._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_TRITRANCHE_SI_SCA, m_quotas_view_si_sca._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_TRITRANCHE_SD_SCA, m_quotas_view_sd_sca._list_ctrl);

	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_TRITRANCHE_SI_B, m_quotas_view_si_b._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_TRITRANCHE_SD_B, m_quotas_view_sd_b._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_TRITRANCHE_SDB_B, m_quotas_view_sdb_b._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_TRITRANCHE_SDB_SCB, m_quotas_view_sdb_scb._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_TRITRANCHE_SI_SCB, m_quotas_view_si_scb._list_ctrl);
	DDX_Control(pDX, IDC_LISTCTRL_QUOTAS_TRITRANCHE_SD_SCB, m_quotas_view_sd_scb._list_ctrl);

	/* Controles en fin de tableau, totaux, grp, diff */
	DDX_Control(pDX, IDC_LABEL_SI_A_RES_TOT, m_SI_A_RES_TOT);
	DDX_Control(pDX, IDC_LABEL_SI_A_DIFF, m_SI_A_DIFF);
	DDX_Control(pDX, IDC_LABEL_SI_A_GRP, m_SI_A_GRP);
	DDX_Control(pDX, IDC_LABEL_SD_A_RES_TOT, m_SD_A_RES_TOT);
	DDX_Control(pDX, IDC_LABEL_SD_A_DIFF, m_SD_A_DIFF);
	DDX_Control(pDX, IDC_LABEL_SD_A_GRP, m_SD_A_GRP);
	DDX_Control(pDX, IDC_LABEL_SDB_A_DIFF, m_DB_A_DIFF);
	DDX_Control(pDX, IDC_LABEL_SI_B_RES_TOT, m_SI_B_RES_TOT);
	DDX_Control(pDX, IDC_LABEL_SI_B_DIFF, m_SI_B_DIFF);
	DDX_Control(pDX, IDC_LABEL_SI_B_GRP, m_SI_B_GRP);
	DDX_Control(pDX, IDC_LABEL_SD_B_RES_TOT, m_SD_B_RES_TOT);
	DDX_Control(pDX, IDC_LABEL_SD_B_DIFF, m_SD_B_DIFF);
	DDX_Control(pDX, IDC_LABEL_SD_B_GRP, m_SD_B_GRP);
	DDX_Control(pDX, IDC_LABEL_SDB_B_DIFF, m_DB_B_DIFF);

	/*NBN*/
	DDX_Control(pDX, IDC_LABEL_SI_SCA_RES_TOT, m_SI_SCA_RES_TOT);
	DDX_Control(pDX, IDC_LABEL_SI_SCB_RES_TOT, m_SI_SCB_RES_TOT);
	DDX_Control(pDX, IDC_LABEL_SD_SCA_RES_TOT, m_SD_SCA_RES_TOT);
	DDX_Control(pDX, IDC_LABEL_SD_SCB_RES_TOT, m_SD_SCB_RES_TOT);

	DDX_Control(pDX, IDC_LABEL_SIA_SCG_ACT, m_SIA_SCG_ACT);
    DDX_Control(pDX, IDC_LABEL_SIB_SCG_ACT, m_SIB_SCG_ACT);
    DDX_Control(pDX, IDC_LABEL_SIA_SCG_MOI, m_SIA_SCG_MOI);
    DDX_Control(pDX, IDC_LABEL_SIB_SCG_MOI, m_SIB_SCG_MOI);
    DDX_Control(pDX, IDC_LABEL_SDA_SCG_ACT, m_SDA_SCG_ACT);
    DDX_Control(pDX, IDC_LABEL_SDB_SCG_ACT, m_SDB_SCG_ACT);
    DDX_Control(pDX, IDC_LABEL_SDA_SCG_MOI, m_SDA_SCG_MOI);
    DDX_Control(pDX, IDC_LABEL_SDB_SCG_MOI, m_SDB_SCG_MOI);

	//--CAPA
	DDX_Control(pDX, IDC_CAPAA,m_CapaA);
	DDX_Control(pDX, IDC_CAPAB,m_CapaB);
	DDX_Control(pDX, IDC_SOMME_MOI_A,m_iCapaMoi_A);
	DDX_Control(pDX, IDC_SOMME_MOI_B,m_iCapaMoi_B);

	//--CHERCHER
	DDX_Text(pDX, IDC_QR_TRAIN, m_NumeroTrain);
	DDX_Text(pDX, IDC_QR_ORIGINE, m_Origine);
	DDV_MaxChars(pDX, m_Origine, 5);
	DDX_Text(pDX, IDC_QR_DESTINATION, m_Destination);
	DDV_MaxChars(pDX, m_Destination, 5);
	DDX_Control(pDX, IDC_DATE_DEPART, m_ctlStartDate);
	DDX_DateTimeCtrl(pDX, IDC_DATE_DEPART, m_startdate);

	//--TRONCON
	DDX_Control(pDX, IDC_COMBO_TRONCON, m_ComboTroncon);
	DDX_Control(pDX, IDC_LIST_TRONCON, m_ListTroncon);
	//	DDX_Control(pDX, IDC_UN_TRONCON, m_Un_Troncon);
	//	DDX_Control(pDX, IDC_TOUS_TRONCONS, m_Tous_Troncon);
	//	DDX_Control(pDX, IDC_SEL_TRONCONS, m_Sel_Troncon);
	
	//--TITRE
	DDX_Control(pDX, IDC_STATIC_SI, m_SI);
	DDX_Control(pDX, IDC_STATIC_SD, m_SD);
	DDX_Control(pDX, IDC_STATIC_DB, m_DB);
	DDX_Control(pDX, IDC_STATIC_DO, m_DO);
	DDX_Control(pDX, IDC_STATIC_SI_SC, m_SI_SC);
	DDX_Control(pDX, IDC_STATIC_SD_SC, m_SD_SC);
	DDX_Control(pDX, IDC_TITRE_DO_CC, m_DO_CC);
	
	//--GRP
	//DDX_Control(pDX, IDC_SI_AGRP, m_SI_AGRP);
	//DDX_Control(pDX, IDC_SI_BGRP, m_SI_BGRP);
	//DDX_Control(pDX, IDC_SD_AGRP, m_SD_AGRP);
	//DDX_Control(pDX, IDC_SD_BGRP, m_SD_BGRP);
	
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
	DDX_Control(pDX, IDC_DB_A0_1, m_DB_A0_1);                     
	DDX_Control(pDX, IDC_DB_A0_2, m_DB_A0_2);                     
	DDX_Control(pDX, IDC_DB_A0_3, m_DB_A0_3);                     
	DDX_Control(pDX, IDC_DB_A0_4, m_DB_A0_4);                     
	DDX_Control(pDX, IDC_DB_A0_5, m_DB_A0_5);                     
	DDX_Control(pDX, IDC_DB_A0_6, m_DB_A0_6);                     
	DDX_Control(pDX, IDC_DB_A0_7, m_DB_A0_7);                     
	DDX_Control(pDX, IDC_DB_A0_8, m_DB_A0_8);                     
	DDX_Control(pDX, IDC_DB_A0_9, m_DB_A0_9);                     
	DDX_Control(pDX, IDC_DB_A0_10, m_DB_A0_10);                    
	DDX_Control(pDX, IDC_DB_A0_11, m_DB_A0_11);                    
	DDX_Control(pDX, IDC_DB_A0_12, m_DB_A0_12);

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
	DDX_Control(pDX, IDC_DB_DA0_1, m_DB_DA0_1);                     
	DDX_Control(pDX, IDC_DB_DA0_2, m_DB_DA0_2);                     
	DDX_Control(pDX, IDC_DB_DA0_3, m_DB_DA0_3);                     
	DDX_Control(pDX, IDC_DB_DA0_4, m_DB_DA0_4);                     
	DDX_Control(pDX, IDC_DB_DA0_5, m_DB_DA0_5);                     
	DDX_Control(pDX, IDC_DB_DA0_6, m_DB_DA0_6);                     
	DDX_Control(pDX, IDC_DB_DA0_7, m_DB_DA0_7);                     
	DDX_Control(pDX, IDC_DB_DA0_8, m_DB_DA0_8);                     
	DDX_Control(pDX, IDC_DB_DA0_9, m_DB_DA0_9);                     
	DDX_Control(pDX, IDC_DB_DA0_10, m_DB_DA0_10);                    
	DDX_Control(pDX, IDC_DB_DA0_11, m_DB_DA0_11);                    
	DDX_Control(pDX, IDC_DB_DA0_12, m_DB_DA0_12); 

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
	DDX_Control(pDX, IDC_DB_B0_1, m_DB_B0_1);                     
	DDX_Control(pDX, IDC_DB_B0_2, m_DB_B0_2);                     
	DDX_Control(pDX, IDC_DB_B0_3, m_DB_B0_3);                     
	DDX_Control(pDX, IDC_DB_B0_4, m_DB_B0_4);                     
	DDX_Control(pDX, IDC_DB_B0_5, m_DB_B0_5);                     
	DDX_Control(pDX, IDC_DB_B0_6, m_DB_B0_6);                     
	DDX_Control(pDX, IDC_DB_B0_7, m_DB_B0_7);                     
	DDX_Control(pDX, IDC_DB_B0_8, m_DB_B0_8);                     
	DDX_Control(pDX, IDC_DB_B0_9, m_DB_B0_9);                     
	DDX_Control(pDX, IDC_DB_B0_10, m_DB_B0_10);                    
	DDX_Control(pDX, IDC_DB_B0_11, m_DB_B0_11);                    
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
	DDX_Control(pDX, IDC_DB_B0_12, m_DB_B0_12); 
	DDX_Control(pDX, IDC_DB_DB0_1, m_DB_DB0_1);                     
	DDX_Control(pDX, IDC_DB_DB0_2, m_DB_DB0_2);                     
	DDX_Control(pDX, IDC_DB_DB0_3, m_DB_DB0_3);                     
	DDX_Control(pDX, IDC_DB_DB0_4, m_DB_DB0_4);                     
	DDX_Control(pDX, IDC_DB_DB0_5, m_DB_DB0_5);                     
	DDX_Control(pDX, IDC_DB_DB0_6, m_DB_DB0_6);                     
	DDX_Control(pDX, IDC_DB_DB0_7, m_DB_DB0_7);                     
	DDX_Control(pDX, IDC_DB_DB0_8, m_DB_DB0_8);                     
	DDX_Control(pDX, IDC_DB_DB0_9, m_DB_DB0_9);                     
	DDX_Control(pDX, IDC_DB_DB0_10, m_DB_DB0_10);                    
	DDX_Control(pDX, IDC_DB_DB0_11, m_DB_DB0_11);                    
	DDX_Control(pDX, IDC_DB_DB0_12, m_DB_DB0_12); 
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

BEGIN_MESSAGE_MAP(CQuotasRhealysDlg, CDialog)
	//{{AFX_MSG_MAP(CQuotasRhealysDlg)
	ON_BN_CLICKED(IDC_BASCULE_QUOTAS_SWITCH_A, SwitchA)
	ON_BN_CLICKED(IDC_BASCULE_QUOTAS_SWITCH_B, SwitchB)
	ON_BN_CLICKED(IDC_QR_CHERCHER, OnChercher)
	ON_BN_CLICKED(IDC_UN_TRONCON, OnUnTroncon)
	ON_BN_CLICKED(IDC_TOUS_TRONCONS, OnTousTroncon)
	ON_BN_CLICKED(IDC_SEL_TRONCONS, OnSelTroncon)
	ON_BN_CLICKED(IDC_QR_QUITTER, OnQuitter)
	ON_BN_CLICKED(IDC_KEEP_ECART, OnKeepEcart)



	ON_EN_KILLFOCUS(IDC_LABEL_SIA_SCG_MOI,OnMajAuthoSIA)
	ON_EN_KILLFOCUS(IDC_LABEL_SIB_SCG_MOI,OnMajAuthoSIB)
	ON_EN_KILLFOCUS(IDC_LABEL_SDA_SCG_MOI,OnMajAuthoSDA)
	ON_EN_KILLFOCUS(IDC_LABEL_SDB_SCG_MOI,OnMajAuthoSDB)

		// Combo Troncon
	ON_CBN_SELCHANGE(IDC_COMBO_TRONCON,OnMajTroncon)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_KB_LIST_END_REACHED, kbReachedEndList)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CQuotasRhealysDlg::OnMajAuthoSIB()
{
	int sumAuthEtanche = 0;
	int sumDispEtanche = 0; 
	CString sg, s0;
	int isg, is0, isc0;

	m_SIB_SCG_MOI.GetWindowText(sg);
	isg = atoi (sg);    // valeur autorisation de la CC-1

	CString capaB;

	m_CapaB.GetWindowText(capaB);
	int intCapaB = atoi(capaB);

	if (m_valeurPrecSCB_SI == isg) // on a rien changé
	{
		return ;
	}

	// Liste des CC => elle permet de retrouver la CC-0
	QuotasAutorisationsListCtrl* listCc = NULL;
	listCc = &m_quotas_view_si_b._list_ctrl;
	is0 = listCc->Datas()[0]._moi._value;  // autorisation en bucket 0.

	if (isg > is0)				// la SCG ne peut être supérieure à la CC0
	{
		sg.Format ("%d", (is0 > intCapaB) ? intCapaB : is0);
		m_SIB_SCG_MOI.SetWindowText(sg);
		return ;
	}

	QuotasAutorisationsListCtrl* listSc = NULL;
	listSc = &m_quotas_view_si_scb._list_ctrl;
	isc0 = listSc->Datas()[0]._moi._value;  // autorisation en bucket 0.

	raboterSCX(listSc, isg);

	if (isg < getMaxAuthScxControl(listSc))				// la SCG ne peut être supérieure à la SC0
	{
		sg.Format ("%d", m_valeurPrecSCB_SI);
		m_SIB_SCG_MOI.SetWindowText(sg);
		return ;
	}

	SommeGlobaleSIB (sumAuthEtanche, sumDispEtanche);

	bool bOK = true;
	if (isg < sumAuthEtanche) // la somme des autorisations en SCI étanche doit être inférieure au sous-contingent
	{
		bOK = false;
	}

	if (!bOK)
	{
		sg.Format ("%d", m_valeurPrecSCB_SI);
		m_SIB_SCG_MOI.SetWindowText(sg);
		return ; 
	}
	// OK pour le SCG
	m_valeurPrecSCB_SI = isg;
	bSISCBFlagModif = TRUE;
	bFlagModifManuelle = TRUE;

}

void CQuotasRhealysDlg::OnMajAuthoSDB()
{
	int sumAuthEtanche = 0;
	int sumDispEtanche = 0; 
	CString sg, s0;
	int isg, is0, isc0;

	m_SDB_SCG_MOI.GetWindowText(sg);
	isg = atoi (sg);    // valeur autorisation de la CC-1

	CString capaB;

	m_CapaB.GetWindowText(capaB);
	int intCapaB = atoi(capaB);

	if (m_valeurPrecSCB_SD == isg) // on a rien changé
	{
		return ;
	}

	// Liste des CC => elle permet de retrouver la CC-0
	QuotasAutorisationsListCtrl* listCc = NULL;
	listCc = &m_quotas_view_sd_b._list_ctrl;
	is0 = listCc->Datas()[0]._moi._value;  // autorisation en bucket 0.

	if (isg > is0)				// la SCG ne peut être supérieure à la CC0
	{
		sg.Format ("%d", (is0 > intCapaB) ? intCapaB : is0);
		m_SDB_SCG_MOI.SetWindowText(sg);
		return ;
	}

	QuotasAutorisationsListCtrl* listSc = NULL;
	listSc = &m_quotas_view_sd_scb._list_ctrl;
	isc0 = listSc->Datas()[0]._moi._value;  // autorisation en bucket 0.

	raboterSCX(listSc, isg);

	if (isg < getMaxAuthScxControl(listSc))				// la SCG ne peut être supérieure à la SC0
	{
		sg.Format ("%d", m_valeurPrecSCB_SD);
		m_SDB_SCG_MOI.SetWindowText(sg);
		return ;
	}

	SommeGlobaleSDB (sumAuthEtanche, sumDispEtanche);

	bool bOK = true;
	if (isg < sumAuthEtanche) // la somme des autorisations en SCI étanche doit être inférieure au sous-contingent
	{
		bOK = false;
	}

	if (!bOK)
	{
		sg.Format ("%d", m_valeurPrecSCB_SD);
		m_SDB_SCG_MOI.SetWindowText(sg);
		return ; 
	}
	// OK pour le SCG
	m_valeurPrecSCB_SD = isg;
	bSDSCBFlagModif = TRUE;
	bFlagModifManuelle = TRUE;
}

void CQuotasRhealysDlg::OnMajAuthoSDA()
{
	int sumAuthEtanche = 0;
	int sumDispEtanche = 0; 
	CString sg, s0;
	int isg, is0, isc0;

	CString capaA;

	m_CapaA.GetWindowText(capaA);
	int intCapaA = atoi(capaA);

	m_SDA_SCG_MOI.GetWindowText(sg);
	isg = atoi (sg);    // valeur autorisation de la CC-1

	if (m_valeurPrecSCA_SD == isg) // on a rien changé
	{
		return ;
	}

	// Liste des CC => elle permet de retrouver la CC-0
	QuotasAutorisationsListCtrl* listCc = NULL;
	listCc = &m_quotas_view_sd_a._list_ctrl;
	is0 = listCc->Datas()[0]._moi._value;  // autorisation en bucket 0.

	QuotasAutorisationsListCtrl* listSc = NULL;
	listSc = &m_quotas_view_sd_sca._list_ctrl;
	isc0 = listSc->Datas()[0]._moi._value;  // autorisation en bucket 0.

	if (isg > is0)				// la SCG ne peut être supérieure à la CC0
	{
		sg.Format ("%d", (is0 > intCapaA) ? intCapaA : is0);
		m_SDA_SCG_MOI.SetWindowText(sg);
		return ;
	}

	raboterSCX(listSc, isg);

	if (isg < getMaxAuthScxControl(listSc))				// la SCG ne peut être supérieure à la SC0
	{
		sg.Format ("%d", m_valeurPrecSCA_SD);
		m_SDA_SCG_MOI.SetWindowText(sg);
		return ;
	}

	SommeGlobaleSDA (sumAuthEtanche, sumDispEtanche);

	bool bOK = true;
	if (isg < sumAuthEtanche) // la somme des autorisations en SCI étanche doit être inférieure au sous-contingent
	{
		bOK = false;
	}

	if (!bOK)
	{
		sg.Format ("%d", m_valeurPrecSCA_SD);
		m_SDA_SCG_MOI.SetWindowText(sg);
		return ; 
	}
	// OK pour le SCG
	m_valeurPrecSCA_SD = isg;
	bSDSCAFlagModif = TRUE;
	bFlagModifManuelle = TRUE;
}

int CQuotasRhealysDlg::getMaxAuthScxControl(QuotasAutorisationsListCtrl* ctr){

	long authMax = 0;
	long value = 0;

	for (int i = 1 ; i < ctr->MaxIndexation(); i++)
	{
		QuotasValue* moi = ctr->GetValue(Quotas::MOI, i);

		value = (moi->_value);

		authMax = (authMax > value) ? authMax : value;

	}

	return authMax;

}

void CQuotasRhealysDlg::OnMajAuthoSIA()
{
	int sumAuthEtanche = 0;
	int sumDispEtanche = 0; 
	CString sg, s0;
	int isg, is0, isc0;

	CString capaA;

	m_CapaA.GetWindowText(capaA);
	int intCapaA = atoi(capaA);

	m_SIA_SCG_MOI.GetWindowText(sg);
	isg = atoi (sg);    // valeur autorisation de la CC-1

	if (m_valeurPrecSCA_SI == isg) // on a rien changé
	{
		return ;
	}

	// Liste des CC => elle permet de retrouver la CC-0
	QuotasAutorisationsListCtrl* listCc = NULL;
	listCc = &m_quotas_view_si_a._list_ctrl;
	is0 = listCc->Datas()[0]._moi._value;  // autorisation en bucket 0.

	if (isg > is0)				// la SCG ne peut être supérieure à la CC0
	{
		sg.Format ("%d", (is0 > intCapaA) ? intCapaA : is0);
		m_SIA_SCG_MOI.SetWindowText(sg);
		return ;
	}

	QuotasAutorisationsListCtrl* listSc = NULL;
	listSc = &m_quotas_view_si_sca._list_ctrl;
	isc0 = listSc->Datas()[0]._moi._value;  // autorisation en bucket 0.

	raboterSCX(listSc, isg);

	if (isg < getMaxAuthScxControl(listSc))				// la SCG ne peut être supérieure à la SC0
	{
		sg.Format ("%d", m_valeurPrecSCA_SI);
		m_SIA_SCG_MOI.SetWindowText(sg);
		return ;
	}

	SommeGlobaleSIA (sumAuthEtanche, sumDispEtanche);

	bool bOK = true;
	if (isg < sumAuthEtanche) // la somme des autorisations en SCI étanche doit être inférieure au sous-contingent
	{
		bOK = false;
	}

	if (!bOK)
	{
		sg.Format ("%d", m_valeurPrecSCA_SI);
		m_SIA_SCG_MOI.SetWindowText(sg);
		return ; 
	}
	// OK pour le SCG
	m_valeurPrecSCA_SI = isg;
	bSISCAFlagModif = TRUE;
	bFlagModifManuelle = TRUE;

}

void CQuotasRhealysDlg::raboterSCX(QuotasAutorisationsListCtrl* ctr, long maValue){

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


// ------------------------------------------------
// Cumul global scx etanche + max scx non étanche
// pour la tranche internationale cmpt A
// HRI, 14/08/2015
// ------------------------------------------------
void CQuotasRhealysDlg::SommeGlobaleSIA(int &sumAuthEtanche, int &sumDispEtanche)
{
	sumAuthEtanche = 0;
	sumDispEtanche = 0;
	SommeGlobaleSCX(&m_quotas_view_si_sca._list_ctrl, sumAuthEtanche, sumDispEtanche);
}

// ------------------------------------------------
// Cumul global scx etanche + max scx non étanche
// pour la tranche internationale cmpt B
// HRI, 14/08/2015
// ------------------------------------------------
void CQuotasRhealysDlg::SommeGlobaleSIB(int &sumAuthEtanche, int &sumDispEtanche)
{
	sumAuthEtanche = 0;
	sumDispEtanche = 0;
	SommeGlobaleSCX(&m_quotas_view_si_scb._list_ctrl, sumAuthEtanche, sumDispEtanche);
}


// ------------------------------------------------
// Cumul global scx etanche + max scx non étanche
// pour la tranche domestique cmpt A
// HRI, 14/08/2015
// ------------------------------------------------
void CQuotasRhealysDlg::SommeGlobaleSDA(int &sumAuthEtanche, int &sumDispEtanche)
{
	sumAuthEtanche = 0;
	sumDispEtanche = 0;
	SommeGlobaleSCX(&m_quotas_view_sd_sca._list_ctrl, sumAuthEtanche, sumDispEtanche);
}

// ------------------------------------------------
// Cumul global scx etanche + max scx non étanche
// pour la tranche domestique cmpt B
// HRI, 14/08/2015
// ------------------------------------------------
void CQuotasRhealysDlg::SommeGlobaleSDB(int &sumAuthEtanche, int &sumDispEtanche)
{
	sumAuthEtanche = 0;
	sumDispEtanche = 0;
	SommeGlobaleSCX(&m_quotas_view_sd_scb._list_ctrl, sumAuthEtanche, sumDispEtanche);
}
// ------------------------------------------------
// Cumul global scx etanche + max scx non étanche
// par list de scx
// HRI, 14/08/2015
// ------------------------------------------------
void CQuotasRhealysDlg::SommeGlobaleSCX(QuotasAutorisationsListCtrl* list, int& sumAuth, int& sumDisp)
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

LONG CQuotasRhealysDlg::kbReachedEndList(UINT wParam, LONG lParam) //SRE 76064 Evol
{
	//Search for the list which has triggered the WM_KB_LIST_END_REACHED event and edit
	//the previous or next one depending on lParam (which is in fact an Bascules::basculeType enum.
	//Keyboard Downward case

	Bascules::DirectionInfo infos;
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

void CQuotasRhealysDlg::InitCtrlQuotasList()
{
	//FIX ano 964 KGH
	int maxA, maxB, maxScA, maxScB;//KGH
	if(((CCTAApp*)APP)->GetRhealysFlag()) // si ALLEO afficher seulement CC0
	{
		maxA = maxB = 1;
		maxScA = maxScB = 1;
	}
	else
	{
		maxA = App().GetDocument()->m_pGlobalSysParms->GetMaxCmptBkt('A');//KGH
		maxB = App().GetDocument()->m_pGlobalSysParms->GetMaxCmptBkt('B');//KGH
		maxScA = App().GetDocument()->m_pGlobalSysParms->GetMaxSciBkt('A');
		maxScB = App().GetDocument()->m_pGlobalSysParms->GetMaxSciBkt('B');
	}
	//fin FIX ano 964 KGH
	//CQuotasRhealysDlg* triTrancheDlg = CQuotasRhealysDlg::getInstance();

	// N'afficher que les buckets donnés par le VNL 
	if (m_nbBktVnlA > 0 && m_nbBktVnlA < maxA)
	{
		maxA = m_nbBktVnlA;
	}
	if (m_nbBktVnlB > 0 && m_nbBktVnlB < maxB)
	{
		maxB = m_nbBktVnlB;
	}

	if (m_nbScxVnlA > 0 && m_nbScxVnlA < maxScA)
	{
		maxScA = m_nbScxVnlA;
	}
	if (m_nbScxVnlB > 0 && m_nbScxVnlB < maxScB)
	{
		maxScB = m_nbScxVnlB;
	}

	std::vector<QuotasValues>::iterator it;

	// HRI, 12/08/2015 : cas particulier du SATC ALLEO
	if (((CCTAApp*)APP)->GetRhealysFlag())
	{
		for (int i=0; i < 100; i++)
		{
			m_quotas_si_a_Svg_Alleo[i] = 0;
			m_quotas_sd_a_Svg_Alleo[i] = 0;
			m_quotas_si_b_Svg_Alleo[i] = 0;
			m_quotas_sd_b_Svg_Alleo[i] = 0;
			//NBN
			m_quotas_si_sca_Svg_Alleo[i] = 0;
			m_quotas_sd_sca_Svg_Alleo[i] = 0;
			m_quotas_si_scb_Svg_Alleo[i] = 0;
			m_quotas_sd_scb_Svg_Alleo[i] = 0;
		}
	}


	// Espace physique A
	// Construction des Listes (Compartiment A)
	m_quotas_view_si_a.Build (Quotas::CC, 'A', maxA);
	m_quotas_view_si_a.ShowWindow(SW_HIDE);
	m_quotas_view_sd_a.Build (Quotas::CC, 'A', maxA);
	m_quotas_view_sd_a.ShowWindow(SW_HIDE);
	m_quotas_view_sdb_a.Build (Quotas::CC, 'A', 1);
	m_quotas_view_sdb_a.ShowWindow(SW_HIDE);

	// NBN	
	m_quotas_view_si_sca.Build (Quotas::SCX, 'A', maxScA+1);
	m_quotas_view_si_sca.ShowWindow(SW_HIDE);
	m_quotas_view_sd_sca.Build (Quotas::SCX, 'A', maxScA+1);
	m_quotas_view_sd_sca.ShowWindow(SW_HIDE);
	m_quotas_view_sdb_sca.Build (Quotas::SCX, 'A', 2);
	m_quotas_view_sdb_sca.ShowWindow(SW_HIDE);

	//m_DO.SetWindowText("Domestique DB");
	//m_DO_CC.SetBkColor(RGB(255,255,255));
	//m_DO.ShowWindow(SW_HIDE);


	// Affectation de callbacks pour la modification des champs de la ListView
	std::vector<QuotasValues>& listSiA = m_quotas_view_si_a._list_ctrl.Datas();
	std::vector<QuotasValues>& listSdA = m_quotas_view_sd_a._list_ctrl.Datas();	
	std::vector<QuotasValues>& listSdbA = m_quotas_view_sdb_a._list_ctrl.Datas();

	// NBN	
	std::vector<QuotasValues>& listSiScA = m_quotas_view_si_sca._list_ctrl.Datas();
	std::vector<QuotasValues>& listSdScA = m_quotas_view_sd_sca._list_ctrl.Datas();	
	std::vector<QuotasValues>& listSdbScA = m_quotas_view_sdb_sca._list_ctrl.Datas();

	for(it = listSiA.begin(); it != listSiA.end(); ++it)
	{
		QuotasValues& value = (*it);
		value._moi._callback_edition = &EditingMoiCallbackSI;
	}
	for(it = listSdA.begin(); it != listSdA.end(); ++it)
	{
		QuotasValues& value = (*it);
		value._moi._callback_edition = &EditingMoiCallbackSD;
	}
	for(it = listSdbA.begin(); it != listSdbA.end(); ++it)
	{
		QuotasValues& value = (*it);
		value._moi._callback_edition = &EditingMoiCallbackDB;
	}

	// NBN
	for(it = listSiScA.begin(); it != listSiScA.end(); ++it)
	{
		QuotasValues& value = (*it);
		value._moi._callback_edition = &EditingMoiCallbackSISC;
	}
	for(it = listSdScA.begin(); it != listSdScA.end(); ++it)
	{
		QuotasValues& value = (*it);
		value._moi._callback_edition = &EditingMoiCallbackSDSC;
	}
	for(it = listSdbScA.begin(); it != listSdbScA.end(); ++it)
	{
		QuotasValues& value = (*it);
		value._moi._callback_edition = &EditingMoiCallbackDBSC;
	}


	// Espace physique B
	// Construction des Listes (Compartiment B)
	m_quotas_view_si_b.Build (Quotas::CC, 'B', maxB);
	m_quotas_view_si_b.ShowWindow(SW_HIDE);
	m_quotas_view_sd_b.Build (Quotas::CC, 'B', maxB);
	m_quotas_view_sd_b.ShowWindow(SW_HIDE);
	m_quotas_view_sdb_b.Build (Quotas::CC, 'B', 1);
	m_quotas_view_sdb_b.ShowWindow(SW_HIDE);

	// NBN
	m_quotas_view_si_scb.Build (Quotas::SCX, 'B', maxScB+1);
	m_quotas_view_si_scb.ShowWindow(SW_HIDE);
	m_quotas_view_sd_scb.Build (Quotas::SCX, 'B', maxScB+1);
	m_quotas_view_sd_scb.ShowWindow(SW_HIDE);
	m_quotas_view_sdb_scb.Build (Quotas::SCX, 'B', 2);
	m_quotas_view_sdb_scb.ShowWindow(SW_HIDE);

	std::vector<QuotasValues>& listSiB = m_quotas_view_si_b._list_ctrl.Datas();
	std::vector<QuotasValues>& listSdB = m_quotas_view_sd_b._list_ctrl.Datas();	
	std::vector<QuotasValues>& listSdbB = m_quotas_view_sdb_b._list_ctrl.Datas();

	// NBN
	std::vector<QuotasValues>& listSiScB = m_quotas_view_si_scb._list_ctrl.Datas();
	std::vector<QuotasValues>& listSdScB = m_quotas_view_sd_scb._list_ctrl.Datas();	
	std::vector<QuotasValues>& listSdbScB = m_quotas_view_sdb_scb._list_ctrl.Datas();

	// international
	for(it = listSiB.begin(); it != listSiB.end(); ++it)
	{
		QuotasValues& value = (*it);
		//value._moi._bBucket = TRUE;
		value._moi._callback_edition = &EditingMoiCallbackSI;
	}
	// Domestique
	for(it = listSdB.begin(); it != listSdB.end(); ++it)
	{
		QuotasValues& value = (*it);
		//value._moi._bBucket = TRUE;
		value._moi._callback_edition = &EditingMoiCallbackSD;
	}
	// DB
	for(it = listSdbB.begin(); it != listSdbB.end(); ++it)
	{
		QuotasValues& value = (*it);
		//value._moi._bBucket = TRUE;
		value._moi._callback_edition = &EditingMoiCallbackDB;
	}

	// NBN
		// international
	for(it = listSiScB.begin(); it != listSiScB.end(); ++it)
	{
		QuotasValues& value = (*it);
		//value._moi._bBucket = TRUE;
		value._moi._callback_edition = &EditingMoiCallbackSISC;
	}
	// Domestique
	for(it = listSdScB.begin(); it != listSdScB.end(); ++it)
	{
		QuotasValues& value = (*it);
		//value._moi._bBucket = TRUE;
		value._moi._callback_edition = &EditingMoiCallbackSDSC;
	}
	// DB
	for(it = listSdbScB.begin(); it != listSdbScB.end(); ++it)
	{
		QuotasValues& value = (*it);
		//value._moi._bBucket = TRUE;
		value._moi._callback_edition = &EditingMoiCallbackDBSC;
	}



	SwitchA();
}

void CQuotasRhealysDlg::EditingMoiCallbackSI(quotas_type_t type, char espace_physique, 
											 quotas_values_t type_value, long indexation, long& valeur)
{
	CQuotasRhealysDlg* triTrancheDlg = CQuotasRhealysDlg::getInstance();

	if (triTrancheDlg != NULL)
	{
		QuotasAutorisationsListCtrl* listSi = NULL;
		QuotasAutorisationsListCtrl* listSd = NULL;
		QuotasAutorisationsListCtrl* listSdb = NULL;


		if (espace_physique == 'A')
		{
			listSi = &triTrancheDlg->m_quotas_view_si_a._list_ctrl;
            listSd = &triTrancheDlg->m_quotas_view_sd_a._list_ctrl;
			listSdb = &triTrancheDlg->m_quotas_view_sdb_a._list_ctrl;

		}
		else if (espace_physique == 'B')
		{
			listSi = &triTrancheDlg->m_quotas_view_si_b._list_ctrl;
			listSd = &triTrancheDlg->m_quotas_view_sd_b._list_ctrl;
			listSdb = &triTrancheDlg->m_quotas_view_sdb_b._list_ctrl;


		}
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << espace_physique << " must be A or B " << std::endl;
			TRACE(oss.str().c_str());
			return ;
		}

		if (listSi != NULL)
		{
			BOOL bFlag = triTrancheDlg->bSIFlagModif;
			if (indexation == 0)
			{
                triTrancheDlg->UpdateListSIcc0(listSi, listSd, listSdb, indexation, valeur, espace_physique);
                // NBN ANO 83648
                if (((CCTAApp*)APP)->GetRhealysFlag()){
                    triTrancheDlg->UpdateDispo(listSi,espace_physique);
				}
                // OHA : ano 77962

				triTrancheDlg->m_iDiff = 0;
			}
			else if (indexation > 0)
			{
				triTrancheDlg->UpdateListccX(listSi, indexation, valeur, bFlag,espace_physique);
				// OHA : ano 77962
				triTrancheDlg->m_iDiff = 0;
				triTrancheDlg->bSIFlagModif = bFlag;
			}
			triTrancheDlg->VerifFlagTroncon();
			triTrancheDlg->GrisageTroncon();
		}
	}
}

void CQuotasRhealysDlg::EditingMoiCallbackSISC(quotas_type_t type, char espace_physique, 
											 quotas_values_t type_value, long indexation, long& valeur)
{
	CQuotasRhealysDlg* triTrancheDlg = CQuotasRhealysDlg::getInstance();

	if (triTrancheDlg != NULL)
	{
		QuotasAutorisationsListCtrl* listSiSc = NULL;


		if (espace_physique == 'A')		
		{
			listSiSc = &triTrancheDlg->m_quotas_view_si_sca._list_ctrl;
			triTrancheDlg->bSISCAFlagModif = true;
		}
		else if (espace_physique == 'B')
		{
			listSiSc = &triTrancheDlg->m_quotas_view_si_scb._list_ctrl;
			triTrancheDlg->bSISCBFlagModif = true;
		}
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << espace_physique << " must be A or B " << std::endl;
			TRACE(oss.str().c_str());
			return ;
		}

		if (listSiSc != NULL)
		{
			BOOL bFlag = FALSE;

			if (indexation == 0)
			{
				// Impossible de modifier la SC0
			}
			else if (indexation > 0)
			{
				triTrancheDlg->UpdateListScx(listSiSc, indexation, valeur, true);
				listSiSc->RedrawItems(indexation, listSiSc->MaxIndexation());
			}
			triTrancheDlg->VerifFlagTroncon();
			triTrancheDlg->GrisageTroncon();

		}
	}
}


void CQuotasRhealysDlg::UpdateListScx(QuotasAutorisationsListCtrl* list_ctrl, long indexation, long& valeur, bool bSi)
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
		//QuotasValue* valueScgMoi = list_ctrl->GetValue(Quotas::MOI, 0);
		/// autorisation CC-1
		//long lValueScgMoi = valueScgMoi->_value;
		CString sg;
		CString strTotal;
		if (list_ctrl->EspacePhysique() == 'A')
		{
			if (bSi)
			{
				m_SIA_SCG_MOI.GetWindowText(sg);
				m_SIA_SCG_ACT.GetWindowText(strTotal);
 				valueCC0Moi = m_quotas_view_si_a._list_ctrl.GetValue(Quotas::MOI , 0);
				valueDispo0 = m_quotas_view_si_a._list_ctrl.GetValue(Quotas::DISPO, 0);

			}
			else 
			{
				m_SDA_SCG_MOI.GetWindowText(sg);
				m_SDA_SCG_ACT.GetWindowText(strTotal);
				valueCC0Moi = m_quotas_view_sd_a._list_ctrl.GetValue(Quotas::MOI , 0);
				valueDispo0 = m_quotas_view_sd_a._list_ctrl.GetValue(Quotas::DISPO, 0);

			}

		}
		else if (list_ctrl->EspacePhysique() == 'B')
		{
			if (bSi)
			{
				m_SIB_SCG_MOI.GetWindowText(sg);
				m_SIB_SCG_ACT.GetWindowText(strTotal);
				valueCC0Moi = m_quotas_view_si_b._list_ctrl.GetValue(Quotas::MOI , 0);
				valueDispo0 = m_quotas_view_si_b._list_ctrl.GetValue(Quotas::DISPO, 0);
			}
			else 
			{
				m_SDB_SCG_MOI.GetWindowText(sg);
				m_SDB_SCG_ACT.GetWindowText(strTotal);
				valueCC0Moi = m_quotas_view_sd_b._list_ctrl.GetValue(Quotas::MOI , 0);
				valueDispo0 = m_quotas_view_sd_b._list_ctrl.GetValue(Quotas::DISPO, 0);
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
		long lValueScgMoi = atoi(sg);
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

}

void CQuotasRhealysDlg::UpdateListDBScx(QuotasAutorisationsListCtrl* list_ctrl, long indexation, long& valeur)
{
	if (list_ctrl != NULL && indexation > 0 && indexation < list_ctrl->MaxIndexation())
	{
		QuotasValue* valueMoi = list_ctrl->GetValue(Quotas::MOI , indexation);
		QuotasValue* valueAct = list_ctrl->GetValue(Quotas::ACT , indexation);
		QuotasValue* valueEtanche = list_ctrl->GetValue(Quotas::ETANCHE, indexation);
		QuotasValue* valueCC0Moi = NULL;
		QuotasValue* valueDispo0 = NULL;
		//	

		long lValueMoi = valueMoi->_value;
		long lValueEtanche = valueEtanche->_value;
		long lValueCCMoi0 = 0l;
		long lValueDispo0 = 0l;
		long lValueScgMoi = 0l;
		int totalResaCmpt = valueAct->_value;


		/// get sous contingent general 
		//QuotasValue* valueScgMoi = list_ctrl->GetValue(Quotas::MOI, 0);
		/// autorisation CC-1
		//long lValueScgMoi = valueScgMoi->_value;
		CString sg;
		CString strTotal;
		if (list_ctrl->EspacePhysique() == 'A')
		{

			//lValueScgMoi = lValueMoi;
	

			valueCC0Moi = m_quotas_view_sdb_a._list_ctrl.GetValue(Quotas::MOI , 0);
			valueDispo0 = m_quotas_view_sdb_a._list_ctrl.GetValue(Quotas::DISPO, 0);
		}
		else if (list_ctrl->EspacePhysique() == 'B')
		{
			//lValueScgMoi = lValueMoi;

			valueCC0Moi = m_quotas_view_sdb_b._list_ctrl.GetValue(Quotas::MOI , 0);
			valueDispo0 = m_quotas_view_sdb_b._list_ctrl.GetValue(Quotas::DISPO, 0);		
		}
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << list_ctrl->EspacePhysique() << " but must be A or B " << std::endl;
			TRACE(oss.str().c_str());	
			return ;
		}
		lValueCCMoi0 = valueCC0Moi->_value;
		lValueScgMoi = valueCC0Moi->_value;
		//long lValueScgMoi = atoi(sg);
		//int totalResaCmpt = atoi(strTotal);

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

}


void CQuotasRhealysDlg::cumulEtanche(QuotasAutorisationsListCtrl* list, int& sumAuth, int& sumDisp)
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
			if (etanche->_value >= 0) // HRI, 14/08/2015 (conditionné à étanche)
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

void CQuotasRhealysDlg::cumulVente(QuotasAutorisationsListCtrl* list, int& sumVente)
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

void CQuotasRhealysDlg::EditingMoiCallbackSD(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur)
{
	CQuotasRhealysDlg* triTrancheDlg = CQuotasRhealysDlg::getInstance();

	if (triTrancheDlg != NULL)
	{
		QuotasAutorisationsListCtrl* listSi = NULL;
		QuotasAutorisationsListCtrl* listSd = NULL;
		QuotasAutorisationsListCtrl* listSdb = NULL;


		if (espace_physique == 'A')
		{
			listSi = &triTrancheDlg->m_quotas_view_si_a._list_ctrl;
			listSd = &triTrancheDlg->m_quotas_view_sd_a._list_ctrl;
			listSdb = &triTrancheDlg->m_quotas_view_sdb_a._list_ctrl;

		}
		else if (espace_physique == 'B')
		{
			listSi = &triTrancheDlg->m_quotas_view_si_b._list_ctrl;
			listSd = &triTrancheDlg->m_quotas_view_sd_b._list_ctrl;
			listSdb = &triTrancheDlg->m_quotas_view_sdb_b._list_ctrl;

		}
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << espace_physique << " must be A or B " << std::endl;
			TRACE(oss.str().c_str());
			return ;
		}
		if (listSd != NULL)
		{	
			BOOL bFlag = triTrancheDlg->bSDFlagModif;
			if (indexation == 0)
			{
				triTrancheDlg->UpdateListSDcc0(listSi, listSd, listSdb, indexation, valeur,espace_physique);
				// OHA : ano 77962
				triTrancheDlg->m_iDiff = 0;
			}				
			else if (indexation > 0)
			{
				triTrancheDlg->UpdateListccX(listSd, indexation, valeur, bFlag,espace_physique);
				// OHA : ano 77962
				triTrancheDlg->m_iDiff = 0;
				triTrancheDlg->bSDFlagModif = bFlag;
			}

			triTrancheDlg->VerifFlagTroncon();
			triTrancheDlg->GrisageTroncon();
		}
	}
}

void CQuotasRhealysDlg::EditingMoiCallbackSDSC(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur)
{
	CQuotasRhealysDlg* triTrancheDlg = CQuotasRhealysDlg::getInstance();

	if (triTrancheDlg != NULL)
	{
		QuotasAutorisationsListCtrl* listSdSc = NULL;


		if (espace_physique == 'A')		
		{
			listSdSc = &triTrancheDlg->m_quotas_view_sd_sca._list_ctrl;
			triTrancheDlg->bSDSCAFlagModif = true;
		}
		else if (espace_physique == 'B')
		{
			listSdSc = &triTrancheDlg->m_quotas_view_sd_scb._list_ctrl;
			triTrancheDlg->bSDSCBFlagModif = true;
		}
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << espace_physique << " must be A or B " << std::endl;
			TRACE(oss.str().c_str());
			return ;
		}

		if (indexation == 0)
		{
			// Impossible de modifier la SC0
		}
		else if (indexation > 0)
		{
			triTrancheDlg->UpdateListScx(listSdSc, indexation, valeur, false);
			listSdSc->RedrawItems(indexation, listSdSc->MaxIndexation());
		}
		triTrancheDlg->VerifFlagTroncon();
		triTrancheDlg->GrisageTroncon();

		
	}
}

void CQuotasRhealysDlg::EditingMoiCallbackDB(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur)
{
	CQuotasRhealysDlg* triTrancheDlg = CQuotasRhealysDlg::getInstance();
	if (triTrancheDlg != NULL)
	{
		QuotasAutorisationsListCtrl* listSi = NULL;
		QuotasAutorisationsListCtrl* listSd = NULL;
		QuotasAutorisationsListCtrl* listSdb = NULL;

		QuotasAutorisationsListCtrl* listSdbSc = NULL;


		if (espace_physique == 'A')
		{
			listSi = &triTrancheDlg->m_quotas_view_si_a._list_ctrl;
			listSd = &triTrancheDlg->m_quotas_view_sd_a._list_ctrl;
			listSdb = &triTrancheDlg->m_quotas_view_sdb_a._list_ctrl;

			listSdbSc = &triTrancheDlg->m_quotas_view_sdb_sca._list_ctrl;

		}
		else if (espace_physique == 'B')
		{
			listSi = &triTrancheDlg->m_quotas_view_si_b._list_ctrl;
			listSd = &triTrancheDlg->m_quotas_view_sd_b._list_ctrl;
			listSdb = &triTrancheDlg->m_quotas_view_sdb_b._list_ctrl;

			listSdbSc = &triTrancheDlg->m_quotas_view_sdb_scb._list_ctrl;

		}
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << espace_physique << " must be A or B " << std::endl;
			TRACE(oss.str().c_str());
			return ;
		}
		if (listSdb != NULL)
		{	
			if (indexation == 0){

				if(!CheckDBInput(listSdb,listSdbSc,espace_physique)){
					return;
				}

				triTrancheDlg->UpdateListSDB(listSi, listSd, listSdb, indexation, valeur);
			}

			triTrancheDlg->VerifFlagTroncon();
			triTrancheDlg->GrisageTroncon();
		}
	}
}

void CQuotasRhealysDlg::checkScxDB(){

	CQuotasRhealysDlg* triTrancheDlg = CQuotasRhealysDlg::getInstance();
	QuotasAutorisationsListCtrl* list = NULL;

	CString moiScg;
			
	QuotasValue* valueCC0Moi = NULL;
	long lValueCCMoi0 = 0l;
	QuotasValue* valueSCXMoi = NULL;
	long lValueSCXMoi0 = 0l;

	// A
	// -----------
	list = &triTrancheDlg->m_quotas_view_sdb_sca._list_ctrl;
	// Modif de la CC0, verifier si il faut diminuer la SCX
	valueCC0Moi = m_quotas_view_sdb_a._list_ctrl.GetValue(Quotas::MOI , 0);
	lValueCCMoi0 = valueCC0Moi->_value;
	valueSCXMoi = m_quotas_view_sdb_sca._list_ctrl.GetValue(Quotas::MOI , 1);
	lValueSCXMoi0 = valueSCXMoi->_value;

	if(lValueCCMoi0 < lValueSCXMoi0){
		m_quotas_view_sdb_sca._list_ctrl.SetValue(Quotas::MOI , 1,lValueCCMoi0);
		list->RedrawItems(1, list->MaxIndexation());
	}

	// B
	// -----------
	list = &triTrancheDlg->m_quotas_view_sdb_scb._list_ctrl;
	// Modif de la CC0, verifier si il faut diminuer la SCX
	valueCC0Moi = m_quotas_view_sdb_b._list_ctrl.GetValue(Quotas::MOI , 0);
	lValueCCMoi0 = valueCC0Moi->_value;
	valueSCXMoi = m_quotas_view_sdb_scb._list_ctrl.GetValue(Quotas::MOI , 1);
	lValueSCXMoi0 = valueSCXMoi->_value;

	if(lValueCCMoi0 < lValueSCXMoi0){
		m_quotas_view_sdb_scb._list_ctrl.SetValue(Quotas::MOI , 1,lValueCCMoi0);
		list->RedrawItems(1, list->MaxIndexation());
	}	
}


void CQuotasRhealysDlg::EditingMoiCallbackDBSC(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur)
{
	CQuotasRhealysDlg* triTrancheDlg = CQuotasRhealysDlg::getInstance();

	if (triTrancheDlg != NULL)
	{
		QuotasAutorisationsListCtrl* listDBSc = NULL;


		if (espace_physique == 'A')		
		{
			listDBSc = &triTrancheDlg->m_quotas_view_sdb_sca._list_ctrl;
			triTrancheDlg->bDBSCAFlagModif = true;
		}
		else if (espace_physique == 'B')
		{
			listDBSc = &triTrancheDlg->m_quotas_view_sdb_scb._list_ctrl;
			triTrancheDlg->bDBSCBFlagModif = true;
		}
		else 
		{
			std::ostringstream oss;
			oss << " bad espace physique " << espace_physique << " must be A or B " << std::endl;
			TRACE(oss.str().c_str());
			return ;
		}

		if (indexation == 0)
		{
			// Impossible de modifier la SC0
		}
		else if (indexation > 0)
		{
			triTrancheDlg->UpdateListDBScx(listDBSc, indexation, valeur);
			listDBSc->RedrawItems(indexation, listDBSc->MaxIndexation());
		}
		triTrancheDlg->VerifFlagTroncon();
		triTrancheDlg->GrisageTroncon();

		
	}
}

bool CQuotasRhealysDlg::CheckRhealysCC0(int valueCC0, int valueSCG){

	if(((CCTAApp*)APP)->GetRhealysFlag()){
	
		// DT37288 - Si la CC0 à une valeur d'autorisation inférieure à celle du SCG 
		// Un message indique à l'utilisateur que la modif n'est pas possible et qu'il faut d'abord se rapprocher du CORM pour baisser la valeur du SCG
		return (!(valueCC0 < valueSCG));

	}

	return true;
}

void CQuotasRhealysDlg::UpdateListSIcc0(QuotasAutorisationsListCtrl* list_ctrl_si, QuotasAutorisationsListCtrl* list_ctrl_sd, QuotasAutorisationsListCtrl* list_ctrl_sdb, long indexation, long& valeur, CString espace_physique)
{
	if (list_ctrl_si != NULL && list_ctrl_sdb != NULL && list_ctrl_sd != NULL && indexation == 0)
	{
		QuotasValue* valueSiAct = list_ctrl_si->GetValue(Quotas::ACT, indexation);
		QuotasValue* valueSiMoi = list_ctrl_si->GetValue(Quotas::MOI, indexation);
		QuotasValue* valueSdMoi = list_ctrl_sd->GetValue(Quotas::MOI, indexation);
		QuotasValue* valueSdbMoi = list_ctrl_sdb->GetValue(Quotas::MOI, indexation);

		int iRecap = 0;

		CString sMOImin;
		
		if (espace_physique == 'A'){
			m_SIA_SCG_MOI.GetWindowText(sMOImin);
		}else{
			m_SIB_SCG_MOI.GetWindowText(sMOImin);
		}
		
		// Verif sur la modif CC0
		if (!CheckRhealysCC0(valueSiMoi->_value,atoi(sMOImin))){
			// Garder l'ancienne valeur si la CC0 est inferieur à la SCG
			valueSiMoi->_value = valueSiAct->_value;

			AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_MODIF_SCG), 
                            MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION );

			return;
		}

		// OHA : ano 77962
		if (valueSiMoi != NULL && valueSiAct != NULL && valueSdMoi != NULL)
		{
			if (valueSiMoi->_value < atoi(sMOImin) && !((CCTAApp*)APP)->GetRhealysFlag())
			{
				
				CString newScg;
				newScg.Format("%ld", valueSiMoi->_value);

				if (espace_physique == 'A'){
					m_SIA_SCG_MOI.SetWindowText(newScg);
					OnMajAuthoSIA();
				}else{
					m_SIB_SCG_MOI.SetWindowText(newScg);
					OnMajAuthoSIB();
				}
			}

			m_iDiff = valueSiAct->_value - valueSiMoi->_value;
			CString strSiDiff;
			strSiDiff.Format("%d", (-m_iDiff));

			/// set diff if needed  
			iRecap = valueSiMoi->_value + valueSdMoi->_value + valueSdbMoi->_value;
			
			CString sRecap;
			sRecap.Format("%d", iRecap);

			if (list_ctrl_si->EspacePhysique() == 'A')
			{
				m_iCapaMoi_A.SetWindowText(sRecap);
				
				/// set diff 
				m_SI_A_DIFF.SetWindowText(strSiDiff);

				if (m_iDiff == 0)
				{
					m_SI_A_DIFF.SetTextColor(RGB(0,0,0));
				}
				else if (m_iDiff > 0)
				{
					m_SI_A_DIFF.SetTextColor(RGB(255,0,0));
				}
				else if (m_iDiff < 0)
				{
					m_SI_A_DIFF.SetTextColor(COLOR_OVB_MTRANCHE);
				}
			}
			else if (list_ctrl_si->EspacePhysique() == 'B')
			{
				m_iCapaMoi_B.SetWindowText(sRecap);
				
				/// set diff 
				m_SI_B_DIFF.SetWindowText(strSiDiff);
				if (m_iDiff == 0)
				{
					m_SI_B_DIFF.SetTextColor(RGB(0,0,0));
				}
				else if (m_iDiff > 0)
				{
					m_SI_B_DIFF.SetTextColor(RGB(255,0,0));
				}
				else if (m_iDiff < 0)
				{
					m_SI_B_DIFF.SetTextColor(COLOR_OVB_MTRANCHE);
				}
			}


			if (valueSiMoi->_value != valueSiAct->_value)
			{
				bSIFlagModif = TRUE;
				bFlagModifManuelle = TRUE;
			}
			if ((indexation + 1) < list_ctrl_si->MaxIndexation())
			{
				QuotasValue* valueMoiApres = list_ctrl_si->GetValue(Quotas::MOI, (indexation + 1));
				valueMoiApres->_bBucket = TRUE;
			}
			
			UpdateListccX(list_ctrl_si, indexation + 1, valeur, bSIFlagModif,list_ctrl_si->EspacePhysique());	
			
		}	
		list_ctrl_si->RedrawItems(indexation, list_ctrl_si->MaxIndexation());
	}
}

/*
void CQuotasRhealysDlg::UpdateListSISCcc0(QuotasAutorisationsListCtrl* list_ctrl_sisc, QuotasAutorisationsListCtrl* list_ctrl_sdsc, QuotasAutorisationsListCtrl* list_ctrl_sdbsc, long indexation, long& valeur)
{
	if (list_ctrl_sisc != NULL && list_ctrl_sdbsc != NULL && list_ctrl_sdsc != NULL && indexation == 0)
	{
		QuotasValue* valueSiscAct = list_ctrl_sisc->GetValue(Quotas::ACT, indexation);
		QuotasValue* valueSiscMoi = list_ctrl_sisc->GetValue(Quotas::MOI, indexation);
		QuotasValue* valueSdscMoi = list_ctrl_sdsc->GetValue(Quotas::MOI, indexation);
		QuotasValue* valueSdbscMoi = list_ctrl_sdbsc->GetValue(Quotas::MOI, indexation);

		int iRecap = 0;
		// OHA : ano 77962
		if (valueSiscMoi != NULL && valueSiscAct != NULL && valueSdscMoi != NULL)
		{
			
			m_iDiff = valueSiscAct->_value - valueSiscMoi->_value;
			CString strSiDiff;
			strSiDiff.Format("%d", (-m_iDiff));

			/// set diff if needed  
			iRecap = valueSiscMoi->_value + valueSdscMoi->_value + valueSdbscMoi->_value;
			
			CString sRecap;
			sRecap.Format("%d", iRecap);

	
			if (valueSiscMoi->_value != valueSiscAct->_value)
			{
				if (list_ctrl_sisc->EspacePhysique() == 'A'){
					bSISCAFlagModif = TRUE;
					bFlagModifManuelle = TRUE;
				}
				if (list_ctrl_sisc->EspacePhysique() == 'B'){
					bSISCBFlagModif = TRUE;
					bFlagModifManuelle = TRUE;
				}			
				
			}
			if ((indexation + 1) < list_ctrl_sisc->MaxIndexation())
			{
				QuotasValue* valueMoiApres = list_ctrl_sisc->GetValue(Quotas::MOI, (indexation + 1));
				valueMoiApres->_bBucket = TRUE;
			}
			
			if (list_ctrl_sisc->EspacePhysique() == 'A'){
				UpdateListccX(list_ctrl_sisc, indexation + 1, valeur, bSISCAFlagModif);	
			} else if (list_ctrl_sisc->EspacePhysique() == 'B'){
				UpdateListccX(list_ctrl_sisc, indexation + 1, valeur, bSISCBFlagModif);	
			}	
			
		}	
		list_ctrl_sisc->RedrawItems(indexation, list_ctrl_sisc->MaxIndexation());
	}
}
*/

void CQuotasRhealysDlg::UpdateListSDcc0(QuotasAutorisationsListCtrl* list_ctrl_si, QuotasAutorisationsListCtrl* list_ctrl_sd, QuotasAutorisationsListCtrl* list_ctrl_sdb, long indexation, long& valeur, CString espace_physique)
{
	if (list_ctrl_si != NULL && list_ctrl_sdb != NULL && list_ctrl_sd != NULL && indexation == 0)
	{
		QuotasValue* valueSdAct = list_ctrl_sd->GetValue(Quotas::ACT, indexation);
		QuotasValue* valueSdMoi = list_ctrl_sd->GetValue(Quotas::MOI, indexation);
		QuotasValue* valueSiMoi = list_ctrl_si->GetValue(Quotas::MOI, indexation);
		QuotasValue* valueSdbMoi = list_ctrl_sdb->GetValue(Quotas::MOI, indexation);

		int iRecap = 0;

		CString sMOImin;

		if (espace_physique == 'A'){
			m_SDA_SCG_MOI.GetWindowText(sMOImin);
		}else{
			m_SDB_SCG_MOI.GetWindowText(sMOImin);
		}		

		// Verif sur la modif CC0
		if (!CheckRhealysCC0(valueSdMoi->_value,atoi(sMOImin))){
			// Garder l'ancienne valeur si la CC0 est inferieur à la SCG
			valueSdMoi->_value = valueSdAct->_value;

			AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_MODIF_SCG), 
                            MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION );

			return;
		}

		// OHA : ano 77962
		if (valueSdMoi != NULL && valueSdAct != NULL && valueSiMoi != NULL)
		{
			if (valueSdMoi->_value < atoi(sMOImin) && !((CCTAApp*)APP)->GetRhealysFlag())
			{
				
				CString newScg;
				newScg.Format("%ld", valueSdMoi->_value);

				if (espace_physique == 'A'){
					m_SDA_SCG_MOI.SetWindowText(newScg);
					OnMajAuthoSDA();
				}else{
					m_SDB_SCG_MOI.SetWindowText(newScg);
					OnMajAuthoSDB();
				}
			}

			m_iDiff = valueSdAct->_value - valueSdMoi->_value;
			CString strSiDiff;
			strSiDiff.Format("%d", (-m_iDiff));

			/// set diff if needed  
			iRecap = valueSiMoi->_value + valueSdMoi->_value + valueSdbMoi->_value;
			
			CString sRecap;
			sRecap.Format("%d", iRecap);

			if (list_ctrl_sd->EspacePhysique() == 'A')
			{
				m_iCapaMoi_A.SetWindowText(sRecap);
				
				/// set diff 
				m_SD_A_DIFF.SetWindowText(strSiDiff);

				if (m_iDiff == 0)
				{
					m_SD_A_DIFF.SetTextColor(RGB(0,0,0));
				}
				else if (m_iDiff > 0)
				{
					m_SD_A_DIFF.SetTextColor(RGB(255,0,0));
				}
				else if (m_iDiff < 0)
				{
					m_SD_A_DIFF.SetTextColor(COLOR_OVB_MTRANCHE);
				}
			}
			else if (list_ctrl_sd->EspacePhysique() == 'B')
			{
				m_iCapaMoi_B.SetWindowText(sRecap);
				
				/// set diff 
				m_SD_B_DIFF.SetWindowText(strSiDiff);
				if (m_iDiff == 0)
				{
					m_SD_B_DIFF.SetTextColor(RGB(0,0,0));
				}
				else if (m_iDiff > 0)
				{
					m_SD_B_DIFF.SetTextColor(RGB(255,0,0));
				}
				else if (m_iDiff < 0)
				{
					m_SD_B_DIFF.SetTextColor(COLOR_OVB_MTRANCHE);
				}
			}

			if (valueSdMoi->_value != valueSdAct->_value)
			{
				bSDFlagModif = TRUE; 
				bFlagModifManuelle = TRUE;
			}
			if ((indexation + 1) < list_ctrl_sd->MaxIndexation())
			{
				QuotasValue* valueMoiApres = list_ctrl_sd->GetValue(Quotas::MOI, (indexation + 1));
				valueMoiApres->_bBucket = TRUE;
			}
			
			UpdateListccX(list_ctrl_sd, indexation + 1, valeur, bSDFlagModif,list_ctrl_sd->EspacePhysique());		
		}
			
		list_ctrl_sd->RedrawItems(indexation, list_ctrl_sd->MaxIndexation());
	}
}

/*
Permet de faire une verif lors de la modif de la CC0 sur la tranche 3
La CC0 doit être supérieur à la CC-1
*/
BOOL CQuotasRhealysDlg::CheckDBInput(QuotasAutorisationsListCtrl* list_ctrl_db,QuotasAutorisationsListCtrl* list_ctrl_dbScx, char espace_physique){

	QuotasValue* valueMoiDb = list_ctrl_db->GetValue(Quotas::MOI, 0);
	QuotasValue* valueMoiDbScx = list_ctrl_dbScx->GetValue(Quotas::MOI, 1);
	
	// Vérif CC0 > CC-1
	if (valueMoiDb != NULL && valueMoiDbScx != NULL){
	
		if(valueMoiDb->_value < valueMoiDbScx ->_value){

			// On remet l'ancienne valeur
			list_ctrl_dbScx->SetValue(Quotas::MOI,1,list_ctrl_db->GetValue(Quotas::MOI, 0)->_value);

		}
	}

	return true;
}

void CQuotasRhealysDlg::UpdateListSDB(QuotasAutorisationsListCtrl* list_ctrl_si, QuotasAutorisationsListCtrl* list_ctrl_sd, QuotasAutorisationsListCtrl* list_ctrl_sdb, long indexation, long& valeur)
{
	if (list_ctrl_si != NULL && list_ctrl_sdb != NULL && list_ctrl_sd != NULL && indexation == 0)
	{
		QuotasValue* valueSdbAct = list_ctrl_sdb->GetValue(Quotas::ACT, indexation);
		QuotasValue* valueSdbMoi = list_ctrl_sdb->GetValue(Quotas::MOI, indexation);

		QuotasValue* valueSiMoi = list_ctrl_si->GetValue(Quotas::MOI, indexation);
		QuotasValue* valueSdMoi = list_ctrl_sd->GetValue(Quotas::MOI, indexation);

		int iRecap = 0;

		if (valueSdbMoi != NULL && valueSdbAct != NULL)
		{

			long lValueMoi = valueSdbMoi->_value;
			long lValueAct = valueSdbAct->_value;
			int ldiff = lValueAct - lValueMoi;
			// OHA : ano 77962
			//m_iDiff = ldiff;								 // HRI, 10/08/2015
			CString sDiff;
			sDiff.Format("%d",(-ldiff));			 // HRI, 10/08/2015

			if (list_ctrl_sdb->EspacePhysique() == 'A')
			{
				m_DB_A_DIFF.SetWindowText(sDiff);
				if (ldiff == 0) m_DB_A_DIFF.SetTextColor(RGB(0,0,0));
				else if (ldiff > 0) m_DB_A_DIFF.SetTextColor(RGB(255,0,0));
				else if (ldiff < 0) m_DB_A_DIFF.SetTextColor(COLOR_OVB_MTRANCHE);
			}
			else if (list_ctrl_sdb->EspacePhysique() == 'B')
			{
				m_DB_B_DIFF.SetWindowText(sDiff);
				if (ldiff == 0) m_DB_B_DIFF.SetTextColor(RGB(0,0,0));
				else if (ldiff > 0) m_DB_B_DIFF.SetTextColor(RGB(255,0,0));
				else if (ldiff < 0) m_DB_B_DIFF.SetTextColor(COLOR_OVB_MTRANCHE);
			}

			long lrecap = lValueMoi;
			lrecap += valueSiMoi->_value;
			lrecap += valueSdMoi->_value;

			CString sRecap;
			sRecap.Format("%ld", lrecap);
			if (list_ctrl_si->EspacePhysique() == 'A')
				m_iCapaMoi_A.SetWindowText(sRecap);
			else if (list_ctrl_si->EspacePhysique() == 'B')
				m_iCapaMoi_B.SetWindowText(sRecap);

			if (lValueMoi != lValueAct)
			{
				bDBFlagModif = TRUE;
				bFlagModifManuelle = TRUE;
			}
			CString sDISPO;
			QuotasValue* valueSdbRes = list_ctrl_sdb->GetValue(Quotas::VENTES, indexation);
			QuotasValue* valueSdbDispo = list_ctrl_sdb->GetValue(Quotas::DISPO, indexation);
			
			long lValueRes = valueSdbRes->_value;
			long lDispoAvant = 0;

			lDispoAvant = lValueMoi - lValueRes;
		
			valueSdbDispo->_value = lDispoAvant;
			valueSdbDispo->_grey_background = false;
		}
		list_ctrl_sdb->RedrawItems(indexation, list_ctrl_sdb->MaxIndexation());
	}
}

// Ano 83648 - NBN - Ajout espace_physique
void CQuotasRhealysDlg::UpdateListccX(QuotasAutorisationsListCtrl* list_ctrl, long indexation, long& valeur, BOOL &bFlagModif,char espace_physique)
{


	if (list_ctrl != NULL && indexation > 0)
	{
		for(int i = indexation; i < list_ctrl->MaxIndexation();  i++)
		{
			QuotasValue* valueMoi = list_ctrl->GetValue(Quotas::MOI, i);
			QuotasValue* valueAct = list_ctrl->GetValue(Quotas::ACT, i);
			QuotasValue* valueMoiAvant = list_ctrl->GetValue(Quotas::MOI, i-1);
			
			// OHA : ano 77962
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
					bFlagModif = TRUE;
					bFlagModifManuelle = TRUE;
				}

				valueMoi->_bBucket = FALSE;
				if ((indexation + 1) < list_ctrl->MaxIndexation())
				{
					QuotasValue* valueMoiApres = list_ctrl->GetValue(Quotas::MOI, (i + 1));
					valueMoiApres->_bBucket = TRUE;
				}	
			}				
		}

		this->UpdateDispo(list_ctrl,espace_physique);
	}
	list_ctrl->RedrawItems(0, list_ctrl->MaxIndexation());
}

// Ano 83648 - NBN - Ajout espace_physique
void CQuotasRhealysDlg::UpdateDispo(QuotasAutorisationsListCtrl* list,char espace_physique)
{
	if (list == NULL)
		return ;

	long lsres = 0;
	long ldispo = 0;
	long ldispo_prev = 0;
	long lmoi = 0;

	// NBN ANO 83648 - Pour SATC classique prendre les ventes IHH
	// Pour SATC ALLEO elles sont cachées donc récupérer celles sauvegardée	
	if (((CCTAApp*)APP)->GetRhealysFlag())
	{
		lsres = (espace_physique=='A') ? iSommeVentesA : iSommeVentesB;
	}else{
		for (int i = 0; i < list->MaxIndexation(); i++)
		{
			QuotasValue* valueRes = list->GetValue(Quotas::VENTES, i);

			if (valueRes != NULL)
			{
				lsres += valueRes->_value;
			}
		}
	}

	for (int i = 0; i < list->MaxIndexation(); i++)
	{
		if (i == 0)
		{
			QuotasValue* valueMoi = list->GetValue(Quotas::MOI, 0);
			QuotasValue* valueDispo = list->GetValue(Quotas::DISPO, 0);

			if (valueMoi != NULL && valueDispo != NULL)
			{
				lmoi = valueMoi->_value;
				ldispo_prev = lmoi - lsres;	
				valueDispo->_value = ldispo_prev;
			}
		}
		else if (i > 0)
		{
			QuotasValue* valueRes = list->GetValue(Quotas::VENTES, i - 1);
			QuotasValue* valueMoi = list->GetValue(Quotas::MOI, i);
			QuotasValue* valueDispo = list->GetValue(Quotas::DISPO, i);

			if (valueRes != NULL && valueMoi != NULL && valueDispo != NULL)
			{
				lsres -= valueRes->_value;
				ldispo = valueMoi->_value - lsres;
				if (ldispo < ldispo_prev)
				{
					ldispo_prev = ldispo;
				}
				valueDispo->_value = ldispo_prev;
			}
		}
	}
	for (int i = 0; i < list->MaxIndexation(); i++)
	{
		QuotasValue* valueDispo = list->GetValue(Quotas::DISPO, i);

		valueDispo->_grey_background = false;
	}
}

BOOL CQuotasRhealysDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// On va attendre d'avoir un VNL pour tailler cette liste
	// avec le bon nombre de buckets
	// Renvoi de cette méthode vers LireVnl()
	// HRI, le 10/08/2015
	InitCtrlQuotasList(); //NCH

	Chercher(); //01/12/12 supression KDISN, déplacer ici pour attendre InitDialog;

	m_ctlStartDate.SetTime (&m_startdate);
	bBucket = TRUE; // Toujours égal à TRUE pour ALLEO
	//--SNCF INTERNATIONAL
	//BlancFondStatic();
	RecapDispoVert();
	// Initialisation du flag pour repercuter l'écart sur les buckets

	CLegView* pLegView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetLegView();
	if ( pLegView)
		m_bOvbAdjustFlag = pLegView->GetOvbAdjustFlag();
	else
		m_bOvbAdjustFlag = false;

	if (((CCTAApp*)APP)->GetRhealysFlag())
	{
		m_bOvbAdjustFlag = false;
		((CButton*)GetDlgItem (IDC_KEEP_ECART))->ShowWindow(SW_HIDE);
		masquerBucketForRhealys ();
		HideScSiDbForAlleo();
	}
	((CButton*)GetDlgItem (IDC_KEEP_ECART))->SetCheck(m_bOvbAdjustFlag);

	m_iDiff = 0;
	bSIFlagModif = FALSE;
	bSDFlagModif = FALSE;
	bDBFlagModif = FALSE;
	bSISCAFlagModif =FALSE;
	bSDSCAFlagModif =FALSE;
	bDBSCAFlagModif =FALSE;
	bSISCBFlagModif =FALSE;
	bSDSCBFlagModif =FALSE;
	bDBSCBFlagModif =FALSE;
	bAllFlagModif = FALSE;
	bFlagModifManuelle = FALSE;  // Si modif manuelle demandée

	m_bFlag = FALSE;

	SetWindowText(m_szTitle);
	((CButton *)GetDlgItem(IDC_TOUS_TRONCONS))->ShowWindow(SW_HIDE);

	return TRUE;
}

void CQuotasRhealysDlg::SwitchA()
{
	m_quotas_view_si_a.ShowWindow(SW_SHOW);
	m_quotas_view_sd_a.ShowWindow(SW_SHOW);
	m_quotas_view_sdb_a.ShowWindow(SW_SHOW);

	m_quotas_view_si_b.ShowWindow(SW_HIDE);
	m_quotas_view_sd_b.ShowWindow(SW_HIDE);
	m_quotas_view_sdb_b.ShowWindow(SW_HIDE);
	
	if(((CCTAApp*)APP)->GetRhealysFlag()){

		m_quotas_view_si_sca.ShowWindow(SW_HIDE);
		m_quotas_view_sd_sca.ShowWindow(SW_HIDE);
		// NBN
		m_quotas_view_si_scb.ShowWindow(SW_HIDE);
		m_quotas_view_sd_scb.ShowWindow(SW_HIDE);
	}else{
			// NBN
		m_quotas_view_si_sca.ShowWindow(SW_SHOW);
		m_quotas_view_sd_sca.ShowWindow(SW_SHOW);
			// NBN
		m_quotas_view_si_scb.ShowWindow(SW_HIDE);
		m_quotas_view_sd_scb.ShowWindow(SW_HIDE);	
	}

	// NBN
	m_quotas_view_sdb_sca.ShowWindow(SW_SHOW);
	m_quotas_view_sdb_scb.ShowWindow(SW_HIDE);


	this->ShowDynamicB(SW_HIDE);
	this->ShowStaticLabel(SW_SHOW);
	this->ShowDynamicA(SW_SHOW);

	if(((CCTAApp*)APP)->GetRhealysFlag()){
		this->ShowHideForAlleo(SW_HIDE);
	}
	else{
		//this->ShowHideForAlleo(SW_SHOW);
	}

	
}

void CQuotasRhealysDlg::ShowHideForAlleo(int mode){

	CWnd* pItem;

	pItem = this->GetDlgItem(IDC_LABEL_SI_SCB_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	pItem = this->GetDlgItem(IDC_LABEL_SI_SCA_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	pItem = this->GetDlgItem(IDC_LABEL_SD_SCA_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	pItem = this->GetDlgItem(IDC_LABEL_SD_SCB_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	pItem = this->GetDlgItem(IDC_TITRE_SI_CC);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	pItem = this->GetDlgItem(IDC_TITRE_SD_CC);
	if (pItem != NULL)
		pItem->ShowWindow(mode);


	pItem = this->GetDlgItem(IDC_LABEL_SIA_SCG_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SDA_SCG_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SIB_SCG_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SDB_SCG_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	pItem = this->GetDlgItem(IDC_LABEL_SIA_SCG_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SIB_SCG_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SDA_SCG_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SDB_SCG_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

		

}


void CQuotasRhealysDlg::SwitchB()
{
	m_quotas_view_si_a.ShowWindow(SW_HIDE);
	m_quotas_view_sd_a.ShowWindow(SW_HIDE);
	m_quotas_view_sdb_a.ShowWindow(SW_HIDE);

	m_quotas_view_si_b.ShowWindow(SW_SHOW);
	m_quotas_view_sd_b.ShowWindow(SW_SHOW);
	m_quotas_view_sdb_b.ShowWindow(SW_SHOW);


	if(((CCTAApp*)APP)->GetRhealysFlag()){

		m_quotas_view_si_scb.ShowWindow(SW_HIDE);
		m_quotas_view_sd_scb.ShowWindow(SW_HIDE);
		// NBN
		m_quotas_view_si_sca.ShowWindow(SW_HIDE);
		m_quotas_view_sd_sca.ShowWindow(SW_HIDE);
	}else{
			// NBN
		m_quotas_view_si_scb.ShowWindow(SW_SHOW);
		m_quotas_view_sd_scb.ShowWindow(SW_SHOW);
			// NBN
		m_quotas_view_si_sca.ShowWindow(SW_HIDE);
		m_quotas_view_sd_sca.ShowWindow(SW_HIDE);	
	}

	// NBN
	m_quotas_view_sdb_scb.ShowWindow(SW_SHOW);
	m_quotas_view_sdb_sca.ShowWindow(SW_HIDE);

	this->ShowDynamicA(SW_HIDE);
	this->ShowStaticLabel(SW_SHOW);
	this->ShowDynamicB(SW_SHOW);

	if(((CCTAApp*)APP)->GetRhealysFlag()){
		this->ShowHideForAlleo(SW_HIDE);
	}
	else{
		//this->ShowHideForAlleo(SW_SHOW);
	}
}

void CQuotasRhealysDlg::ShowStaticLabel(int mode)
{
	CWnd* pItem;

	pItem = this->GetDlgItem(IDC_TITRE_SI_GRP);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_TITRE_SI_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_TITRE_SD_GRP);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_TITRE_SD_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_TITRE_SDB_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
}

void CQuotasRhealysDlg::ShowDynamicA(int mode)
{
	CWnd* pItem;

	pItem = this->GetDlgItem(IDC_LABEL_SI_A_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SI_A_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SI_A_GRP);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SI_SCA_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	pItem = this->GetDlgItem(IDC_LABEL_SD_A_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_A_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_A_GRP);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_SCA_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	pItem = this->GetDlgItem(IDC_LABEL_SDB_A_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	pItem = this->GetDlgItem(IDC_LABEL_SIA_SCG_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SIA_SCG_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SDA_SCG_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SDA_SCG_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	pItem = this->GetDlgItem(IDC_LABEL_SIA_SCG_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SDA_SCG_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SIA_SCG_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SDA_SCG_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);


}

void CQuotasRhealysDlg::ShowDynamicB(int mode)
{
	CWnd* pItem;

	pItem = this->GetDlgItem(IDC_LABEL_SI_B_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SI_B_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SI_B_GRP);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_B_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_B_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_B_GRP);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SDB_B_DIFF);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SI_SCB_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SD_SCB_RES_TOT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

	pItem = this->GetDlgItem(IDC_LABEL_SIB_SCG_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SIB_SCG_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SDB_SCG_ACT);
	if (pItem != NULL)
		pItem->ShowWindow(mode);
	pItem = this->GetDlgItem(IDC_LABEL_SDB_SCG_MOI);
	if (pItem != NULL)
		pItem->ShowWindow(mode);

}

void CQuotasRhealysDlg::RecapDispoVert()
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
	m_DB_DA0_1.SetTextColor (vdis);                     
	m_DB_DA0_2.SetTextColor (vdis);                     
	m_DB_DA0_3.SetTextColor (vdis);                     
	m_DB_DA0_4.SetTextColor (vdis);                     
	m_DB_DA0_5.SetTextColor (vdis);                     
	m_DB_DA0_6.SetTextColor (vdis);                     
	m_DB_DA0_7.SetTextColor (vdis);                     
	m_DB_DA0_8.SetTextColor (vdis);                     
	m_DB_DA0_9.SetTextColor (vdis);                     
	m_DB_DA0_10.SetTextColor (vdis);                    
	m_DB_DA0_11.SetTextColor (vdis);                    
	m_DB_DA0_12.SetTextColor (vdis);
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
	m_DB_DB0_1.SetTextColor (vdis);                     
	m_DB_DB0_2.SetTextColor (vdis);                     
	m_DB_DB0_3.SetTextColor (vdis);                     
	m_DB_DB0_4.SetTextColor (vdis);                     
	m_DB_DB0_5.SetTextColor (vdis);                     
	m_DB_DB0_6.SetTextColor (vdis);                     
	m_DB_DB0_7.SetTextColor (vdis);                     
	m_DB_DB0_8.SetTextColor (vdis);                     
	m_DB_DB0_9.SetTextColor (vdis);                     
	m_DB_DB0_10.SetTextColor (vdis);                    
	m_DB_DB0_11.SetTextColor (vdis);                    
	m_DB_DB0_12.SetTextColor (vdis);
}

void CQuotasRhealysDlg::masquerBucketForRhealys()
{
	// TODO: RHEALYS
}

void CQuotasRhealysDlg::SISCRemplissageA() //NBN
{

	if(((CCTAApp*)APP)->GetRhealysFlag()) // si ALLEO afficher seulement CC0
	{
		m_quotas_si_sca_Svg_Alleo[m_pVnlScx->GetNestLevel()] = m_pVnlScx->GetAuth();

	}else{
		if (m_pVnlScx->GetNestLevel() <= (int) m_quotas_view_si_sca._list_ctrl.Datas().size())
		{
			m_quotas_view_si_sca._list_ctrl.SetLine(m_pVnlScx->GetNestLevel(), m_pVnlScx->GetBaseSold(),
				m_pVnlScx->GetAuth(), m_pVnlScx->GetAuth(), m_pVnlScx->GetAvail());
		}

		if (m_pVnlScx->GetNestLevel() == 0)
			m_iscSomme = m_pVnlScx->GetBaseSold();
		else
			m_iscSomme += m_pVnlScx->GetBaseSold();
	}
}


void CQuotasRhealysDlg::SIRemplissageA() //NCH
{
	if (m_pVnlBkt->GetNestLevel() <= (int) m_quotas_view_si_a._list_ctrl.Datas().size())
	{
		m_quotas_view_si_a._list_ctrl.SetLine(m_pVnlBkt->GetNestLevel(), m_pVnlBkt->GetBaseSold(),
			m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAvail());
	}
	// Cas particulier du SATC ALLEO
	// view de sauvegarde des données initiales des tranches de nature 1 (internationale) et 2 (domestique)
	// On en aura besoin au moment de la génération du VNAU
	// HRI, 12/08/2015
	if(((CCTAApp*)APP)->GetRhealysFlag()) // si ALLEO afficher seulement CC0
	{
		//CString toto;
		//toto.Format("CQuotasRhealysDlg::SIRemplissageA()avant nestLevel=%d", m_pVnlBkt->GetNestLevel());
		//AfxMessageBox(toto, MB_OK);
		m_quotas_si_a_Svg_Alleo[m_pVnlBkt->GetNestLevel()] = m_pVnlBkt->GetAuth();
		//AfxMessageBox("CQuotasRhealysDlg::SIRemplissageA()apres", MB_OK);
	}

	if (m_pVnlBkt->GetNestLevel() == 0)
		m_iSomme = m_pVnlBkt->GetBaseSold();
	else
		m_iSomme += m_pVnlBkt->GetBaseSold();
}

void CQuotasRhealysDlg::SIRemplissageFinA()
{
	CString sTemp;
	sTemp.Format("%d",m_iSomme);
	m_SI_A_RES_TOT.SetWindowText(sTemp);
	sTemp.Format("%d",0);
	m_SI_A_DIFF.SetWindowText(sTemp);
	// NBN ANO 83648
    iSommeVentesA = m_iSomme;
}

void CQuotasRhealysDlg::SISCRemplissageFinA()
{
	CString sTemp;
	sTemp.Format("%d",m_iscSomme);
	m_SI_SCA_RES_TOT.SetWindowText(sTemp);

	sTemp.Format("%d",m_pVnlCmpt->GetScgCapa());
	m_SIA_SCG_MOI.SetWindowText(sTemp); //NBN
	m_SIA_SCG_ACT.SetWindowText(sTemp); //NBN

	m_valeurPrecSCA_SI = atoi(sTemp);

	if(((CCTAApp*)APP)->GetRhealysFlag())
	{
		// Indice 0 est la SCG
		m_quotas_si_sca_Svg_Alleo[0] = m_pVnlCmpt->GetScgCapa();
	}

}

void CQuotasRhealysDlg::SISCRemplissageFinB()
{
	CString sTemp;
	sTemp.Format("%d",m_iscSomme);
	m_SI_SCB_RES_TOT.SetWindowText(sTemp);	 //NCH
				//NCH
	sTemp.Format("%d",m_pVnlCmpt->GetScgCapa());
	m_SIB_SCG_MOI.SetWindowText(sTemp); //NBN
	m_SIB_SCG_ACT.SetWindowText(sTemp); //NBN

	m_valeurPrecSCB_SI = atoi(sTemp);

	if(((CCTAApp*)APP)->GetRhealysFlag())
	{
		// Indice 0 est la SCG
		m_quotas_si_scb_Svg_Alleo[0] = m_pVnlCmpt->GetScgCapa();
	}
}


void CQuotasRhealysDlg::SIRemplissageB()
{
	if (m_pVnlBkt->GetNestLevel() <= (int) m_quotas_view_si_b._list_ctrl.Datas().size())
	{
		m_quotas_view_si_b._list_ctrl.SetLine(m_pVnlBkt->GetNestLevel(), m_pVnlBkt->GetBaseSold(),
			m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAvail());
	}
	// Cas particulier du SATC ALLEO
	// view de sauvegarde des données initiales des tranches de nature 1 (internationale) et 2 (domestique)
	// On en aura besoin au moment de la génération du VNAU
	// HRI, 12/08/2015
	if(((CCTAApp*)APP)->GetRhealysFlag()) // si ALLEO afficher seulement CC0
	{
		//CString toto;
		//toto.Format("CQuotasRhealysDlg::SIRemplissageB()avant nestLevel=%d", m_pVnlBkt->GetNestLevel());
		//AfxMessageBox(toto, MB_OK);
		m_quotas_si_b_Svg_Alleo[m_pVnlBkt->GetNestLevel()] = m_pVnlBkt->GetAuth();
		//AfxMessageBox("CQuotasRhealysDlg::SIRemplissageB()après", MB_OK);
	}

	if (m_pVnlBkt->GetNestLevel() == 0)
		m_iSomme = m_pVnlBkt->GetBaseSold();
	else
		m_iSomme += m_pVnlBkt->GetBaseSold();
}

void CQuotasRhealysDlg::SISCRemplissageB() // NBN
{

	if(((CCTAApp*)APP)->GetRhealysFlag()) // si ALLEO afficher seulement CC0
	{
		m_quotas_si_scb_Svg_Alleo[m_pVnlScx->GetNestLevel()] = m_pVnlScx->GetAuth();
	}else{
		if (m_pVnlScx->GetNestLevel() <= (int) m_quotas_view_si_scb._list_ctrl.Datas().size())
		{
			m_quotas_view_si_scb._list_ctrl.SetLine(m_pVnlScx->GetNestLevel(), m_pVnlScx->GetBaseSold(),
				m_pVnlScx->GetAuth(), m_pVnlScx->GetAuth(), m_pVnlScx->GetAvail());
		}

		if (m_pVnlScx->GetNestLevel() == 0)
			m_iscSomme = m_pVnlScx->GetBaseSold();
		else
			m_iscSomme += m_pVnlScx->GetBaseSold();
	}

}



void CQuotasRhealysDlg::SIRemplissageFinB()
{
	CString sTemp;
	sTemp.Format("%d",m_iSomme);
	m_SI_B_RES_TOT.SetWindowText(sTemp);	 //NCH
	sTemp.Format("%d",0);
	m_SI_B_DIFF.SetWindowText(sTemp);				//NCH
	// NBN ANO 83648
    iSommeVentesB = m_iSomme;

}

void CQuotasRhealysDlg::SDRemplissageA()
{
	// NBN - ANO 89531 - Reour arrière. Les colonnes Act et Moi dans la bascule de quotas ne doivent pas être forcées à la valeur 0
	// --------------------------------------------------------------------------------------------------------------------------------
	// HRI, 11/08/2015 : pas d'autorisation sur les tronçons internationaux
	// de la tranche domestique
	//long moi = m_pVnlBkt->GetAuth();
	//if (TronconInter() && (m_pVnlBkt->GetAuth()>0))
	//{	    
	//	moi = 0;
	//}
	QuotasAutorisationsListCtrl& ctrl = m_quotas_view_sd_a._list_ctrl;

	// NBN - ANO 89531 - Il faudrait retirer le blocage sur la colonne Acte et sur la colonne Moi 
	//bool autoriseModifs = TronconInter();
	bool autoriseModifs = true;

	// NPI - cast pour suppression warning C4018
	//if (m_pVnlBkt->GetNestLevel() <= m_quotas_view_sd_a._list_ctrl.Datas().size())
	if (m_pVnlBkt->GetNestLevel() < (int)m_quotas_view_sd_a._list_ctrl.Datas().size())
	{
		// Désactive l'édition si le tronçon est inter
		ctrl.SetEditable (Quotas::MOI, m_pVnlBkt->GetNestLevel(), autoriseModifs);

		// Mettre le background en gris
		ctrl.SetGreyBackground(Quotas::MOI, m_pVnlBkt->GetNestLevel(), !autoriseModifs);

		if (!autoriseModifs)
		{
			// Mettre le background en gris, font en gris foncé
			ctrl.SetTextColor(Quotas::MOI, m_pVnlBkt->GetNestLevel(), RGB(128, 128, 128));
		}
		else
		{
			ctrl.SetTextColor(Quotas::MOI, m_pVnlBkt->GetNestLevel(), RGB(0,0,255));
		}
		// Fin rajout HRI 11/08/2015

		m_quotas_view_sd_a._list_ctrl.SetLine(m_pVnlBkt->GetNestLevel(), m_pVnlBkt->GetBaseSold(),
			m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAvail());
	}

	// Cas particulier du SATC ALLEO
	// view de sauvegarde des données initiales des tranches de nature 1 (internationale) et 2 (domestique)
	// On en aura besoin au moment de la génération du VNAU
	// HRI, 12/08/2015
	if(((CCTAApp*)APP)->GetRhealysFlag()) // si ALLEO afficher seulement CC0
	{
		// NBN - ANO 89531 - Reour arrière. Les colonnes Act et Moi dans la bascule de quotas ne doivent pas être forcées à la valeur 0
		m_quotas_sd_a_Svg_Alleo[m_pVnlBkt->GetNestLevel()] = m_pVnlBkt->GetAuth();
	}

	if (m_pVnlBkt->GetNestLevel() == 0)
		m_iSomme = m_pVnlBkt->GetBaseSold();
	else
		m_iSomme += m_pVnlBkt->GetBaseSold();
}

void CQuotasRhealysDlg::SDSCRemplissageA() // NBN
{
	if(((CCTAApp*)APP)->GetRhealysFlag()) // si ALLEO afficher seulement CC0
	{
		m_quotas_sd_sca_Svg_Alleo[m_pVnlScx->GetNestLevel()] = m_pVnlScx->GetAuth();

	}else{

		QuotasAutorisationsListCtrl& ctrl = m_quotas_view_sd_sca._list_ctrl;

		// NBN - ANO 89531 - Il faudrait retirer le blocage sur la colonne Acte et sur la colonne Moi 
		//bool autoriseModifs = TronconInter();
		bool autoriseModifs = true;

		if (m_pVnlScx->GetNestLevel() < (int)m_quotas_view_sd_sca._list_ctrl.Datas().size())
		///
		{
			// Désactive l'édition si le tronçon est inter
			ctrl.SetEditable (Quotas::MOI, m_pVnlScx->GetNestLevel(), autoriseModifs);

			// Mettre le background en gris
			ctrl.SetGreyBackground(Quotas::MOI, m_pVnlScx->GetNestLevel(), !autoriseModifs);

			if (!autoriseModifs)
			{
				// Mettre le background en gris, font en gris foncé
				ctrl.SetTextColor(Quotas::MOI, m_pVnlScx->GetNestLevel(), RGB(128, 128, 128));
			}
			else
			{
				ctrl.SetTextColor(Quotas::MOI, m_pVnlScx->GetNestLevel(), RGB(0,0,255));
			}

			m_quotas_view_sd_sca._list_ctrl.SetLine(m_pVnlScx->GetNestLevel(), m_pVnlScx->GetBaseSold(),
				m_pVnlScx->GetAuth(), m_pVnlScx->GetAuth(), m_pVnlScx->GetAvail());
		}


		if (m_pVnlScx->GetNestLevel() == 0)
			m_iscSomme = m_pVnlScx->GetBaseSold();
		else
			m_iscSomme += m_pVnlScx->GetBaseSold();
	}
}

void CQuotasRhealysDlg::SDRemplissageFinA()
{
	CString sTemp;
	sTemp.Format("%d",m_iSomme);
	m_SD_A_RES_TOT.SetWindowText(sTemp);		//NCH
	sTemp.Format("%d",0);
	m_SD_A_DIFF.SetWindowText(sTemp);				//NCH
}

void CQuotasRhealysDlg::SDSCRemplissageFinA()
{
	CString sTemp;
	sTemp.Format("%d",m_iscSomme);
	m_SD_SCA_RES_TOT.SetWindowText(sTemp);	

	sTemp.Format("%d",m_pVnlCmpt->GetScgCapa());
	m_SDA_SCG_MOI.SetWindowText(sTemp); //NBN
	m_SDA_SCG_ACT.SetWindowText(sTemp); //NBN

	m_valeurPrecSCA_SD = atoi(sTemp);

	if(((CCTAApp*)APP)->GetRhealysFlag())
	{
		// Indice 0 est la SCG
		m_quotas_sd_sca_Svg_Alleo[0] = m_pVnlCmpt->GetScgCapa();
	}

}

void CQuotasRhealysDlg::SDRemplissageB()
{
	
	// NBN - ANO 89531 - Reour arrière. Les colonnes Act et Moi dans la bascule de quotas ne doivent pas être forcées à la valeur 0
	// --------------------------------------------------------------------------------------------------------------------------------
	// HRI, 11/08/2015 : pas d'autorisation sur les tronçons internationaux
	// de la tranche domestique
	//long moi = m_pVnlBkt->GetAuth();
	//if (TronconInter() && (m_pVnlBkt->GetAuth()>0))
	//{	    
	//	moi = 0;
	//}

	QuotasAutorisationsListCtrl& ctrl = m_quotas_view_sd_b._list_ctrl;

	// NBN - ANO 89531 - Il faudrait retirer le blocage sur la colonne Acte et sur la colonne Moi 
	//bool autoriseModifs = TronconInter();
	bool autoriseModifs = true;

	// NPI - cast pour suppression warning C4018
	//if (m_pVnlBkt->GetNestLevel() <= m_quotas_view_sd_a._list_ctrl.Datas().size())
	if (m_pVnlBkt->GetNestLevel() < (int)m_quotas_view_sd_b._list_ctrl.Datas().size())
	///
	{
		// Désactive l'édition si le tronçon est inter
		ctrl.SetEditable (Quotas::MOI, m_pVnlBkt->GetNestLevel(), autoriseModifs);

		// Mettre le background en gris
		ctrl.SetGreyBackground(Quotas::MOI, m_pVnlBkt->GetNestLevel(), !autoriseModifs);

		if (!autoriseModifs)
		{
			// Mettre le background en gris, font en gris foncé
			ctrl.SetTextColor(Quotas::MOI, m_pVnlBkt->GetNestLevel(), RGB(128, 128, 128));
		}
		else
		{
			ctrl.SetTextColor(Quotas::MOI, m_pVnlBkt->GetNestLevel(), RGB(0,0,255));
		}
		// Fin rajout HRI 11/08/2015

		m_quotas_view_sd_b._list_ctrl.SetLine(m_pVnlBkt->GetNestLevel(), m_pVnlBkt->GetBaseSold(),
			m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAvail());
	}
	// Cas particulier du SATC ALLEO
	// view de sauvegarde des données initiales des tranches de nature 1 (internationale) et 2 (domestique)
	// On en aura besoin au moment de la génération du VNAU
	// HRI, 12/08/2015
	if(((CCTAApp*)APP)->GetRhealysFlag()) // si ALLEO afficher seulement CC0, mais conserver les CCx
	{
		//CString toto;
		//toto.Format("CQuotasRhealysDlg::SDRemplissageB()avant nestLevel=%d", m_pVnlBkt->GetNestLevel());
		//AfxMessageBox(toto, MB_OK);
		m_quotas_sd_b_Svg_Alleo[m_pVnlBkt->GetNestLevel()] = m_pVnlBkt->GetAuth();
		//AfxMessageBox("CQuotasRhealysDlg::SDRemplissageB()aPRES", MB_OK);
	}
	if (m_pVnlBkt->GetNestLevel() == 0)
		m_iSomme = m_pVnlBkt->GetBaseSold();
	else
		m_iSomme += m_pVnlBkt->GetBaseSold();
}


void CQuotasRhealysDlg::SDSCRemplissageB()
{
	// NBN - ANO 89531 - Reour arrière. Les colonnes Act et Moi dans la bascule de quotas ne doivent pas être forcées à la valeur 0
	// --------------------------------------------------------------------------------------------------------------------------------
	//long moi = m_pVnlScx->GetAuth();
	//if (TronconInter() && (m_pVnlScx->GetAuth()>0))
	//{
	    //bSDSCAFlagModif = TRUE;
	//	moi = 0;
	//}

	if(((CCTAApp*)APP)->GetRhealysFlag()) // si ALLEO afficher seulement CC0
	{
		m_quotas_sd_scb_Svg_Alleo[m_pVnlScx->GetNestLevel()] = m_pVnlScx->GetAuth();

	}else{

		QuotasAutorisationsListCtrl& ctrl = m_quotas_view_sd_scb._list_ctrl;

		// NBN - ANO 89531 - Il faudrait retirer le blocage sur la colonne Acte et sur la colonne Moi 
		//bool autoriseModifs = TronconInter();
		bool autoriseModifs = true;

		// NPI - cast pour suppression warning C4018
		//if (m_pVnlBkt->GetNestLevel() <= m_quotas_view_sd_a._list_ctrl.Datas().size())
		if (m_pVnlScx->GetNestLevel() < (int)m_quotas_view_sd_scb._list_ctrl.Datas().size())
		///
		{
			// Désactive l'édition si le tronçon est inter
			ctrl.SetEditable (Quotas::MOI, m_pVnlScx->GetNestLevel(), autoriseModifs);

			// Mettre le background en gris
			ctrl.SetGreyBackground(Quotas::MOI, m_pVnlScx->GetNestLevel(), !autoriseModifs);

			if (!autoriseModifs)
			{
				// Mettre le background en gris, font en gris foncé
				ctrl.SetTextColor(Quotas::MOI, m_pVnlScx->GetNestLevel(), RGB(128, 128, 128));
			}
			else
			{
				ctrl.SetTextColor(Quotas::MOI, m_pVnlScx->GetNestLevel(), RGB(0,0,255));
			}
			// Fin rajout HRI 11/08/2015

			m_quotas_view_sd_scb._list_ctrl.SetLine(m_pVnlScx->GetNestLevel(), m_pVnlScx->GetBaseSold(),
				m_pVnlScx->GetAuth(), m_pVnlScx->GetAuth(), m_pVnlScx->GetAvail());
		}


		if (m_pVnlScx->GetNestLevel() == 0)
			m_iscSomme = m_pVnlScx->GetBaseSold();
		else
			m_iscSomme += m_pVnlScx->GetBaseSold();
	}
}

void CQuotasRhealysDlg::SDRemplissageFinB()
{
	CString sTemp;
	sTemp.Format("%d",m_iSomme);
	m_SD_B_RES_TOT.SetWindowText(sTemp);	//NCH
	sTemp.Format("%d",0);
	m_SD_B_DIFF.SetWindowText(sTemp);			//NCH

	// Ici, si on a détecté des autorisations > 0 sur le tronçon international
	// de la tranche domestique, on émet un message
	if (bSDFlagModif && !((CCTAApp*)APP)->GetRhealysFlag())
	{
		AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_SND_ETRANGER),
			AfxGetAppName(), MB_ICONINFORMATION | MB_APPLMODAL);

	}
}


void CQuotasRhealysDlg::SDSCRemplissageFinB()
{
	CString sTemp;
	sTemp.Format("%d",m_iscSomme);
	m_SD_SCB_RES_TOT.SetWindowText(sTemp);	


	sTemp.Format("%d",m_pVnlCmpt->GetScgCapa());
	m_SDB_SCG_MOI.SetWindowText(sTemp); //NBN
	m_SDB_SCG_ACT.SetWindowText(sTemp); //NBN

	m_valeurPrecSCB_SD = atoi(sTemp);


	
}

void CQuotasRhealysDlg::DBRemplissageA()
{
	CString sTemp;
	m_quotas_view_sdb_a._list_ctrl.SetLine(m_pVnlBkt->GetNestLevel(), m_pVnlBkt->GetBaseSold(),
		m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAvail());
	sTemp.Format("%d",0);
	m_DB_A_DIFF.SetWindowText(sTemp);
}

void CQuotasRhealysDlg::DBRemplissageB()
{
	CString sTemp;
	m_quotas_view_sdb_b._list_ctrl.SetLine(m_pVnlBkt->GetNestLevel(), m_pVnlBkt->GetBaseSold(),
		m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAuth(), m_pVnlBkt->GetAvail());
	sTemp.Format("%d",0);
	m_DB_B_DIFF.SetWindowText(sTemp); //NCH
}

void CQuotasRhealysDlg::DBSCRemplissageA()
{
	CString sTemp;

	// Désactive l'édition si le tronçon est inter
	m_quotas_view_sdb_sca._list_ctrl.SetEditable (Quotas::MOI, m_pVnlScx->GetNestLevel(), !TronconDE());

	// Mettre le background en gris
	m_quotas_view_sdb_sca._list_ctrl.SetGreyBackground(Quotas::MOI, m_pVnlScx->GetNestLevel(), TronconDE());

	if (TronconDE())
	{
		// Mettre le background en gris, font en gris foncé
		m_quotas_view_sdb_sca._list_ctrl.SetTextColor(Quotas::MOI, m_pVnlScx->GetNestLevel(), RGB(128, 128, 128));
	}
	else
	{
		m_quotas_view_sdb_sca._list_ctrl.SetTextColor(Quotas::MOI, m_pVnlScx->GetNestLevel(), RGB(0,0,255));
	}

	// ANO 89416 NBN - SCG = SC1
    m_quotas_view_sdb_sca._list_ctrl.SetLine(m_pVnlScx->GetNestLevel(), m_pVnlScx->GetBaseSold(),
        m_pVnlScx->GetAuth(), m_pVnlScx->GetAuth(), m_pVnlScx->GetAvail());

	sTemp.Format("%d",0);
	
}

void CQuotasRhealysDlg::DBSCRemplissageB()
{
	CString sTemp;

		// Désactive l'édition si le tronçon est inter
	m_quotas_view_sdb_scb._list_ctrl.SetEditable (Quotas::MOI, m_pVnlScx->GetNestLevel(), !TronconDE());

	// Mettre le background en gris
	m_quotas_view_sdb_scb._list_ctrl.SetGreyBackground(Quotas::MOI, m_pVnlScx->GetNestLevel(), TronconDE());

	if (TronconDE())
	{
		// Mettre le background en gris, font en gris foncé
		m_quotas_view_sdb_scb._list_ctrl.SetTextColor(Quotas::MOI, m_pVnlScx->GetNestLevel(), RGB(128, 128, 128));
	}
	else
	{
		m_quotas_view_sdb_scb._list_ctrl.SetTextColor(Quotas::MOI, m_pVnlScx->GetNestLevel(), RGB(0,0,255));
	}

	// ANO 89416 NBN - SCG = SC1
    m_quotas_view_sdb_scb._list_ctrl.SetLine(m_pVnlScx->GetNestLevel(), m_pVnlScx->GetBaseSold(),
        m_pVnlScx->GetAuth(), m_pVnlScx->GetAuth(), m_pVnlScx->GetAvail());

	sTemp.Format("%d",0);
	
}

void CQuotasRhealysDlg::RecapRemplissage()
{
	CString sACT,sSom,sMOI,sEcart,sCapa,sTot;
	int iSomActA,iSomActB=0;
	int iSomMoiA,iSomMoiB=0;
	int iEcartA,iEcartB=0;


	// ANOMALIE 70134 : [SATC] 8.0.6 : Bascule de Quotas impossible sur tri-tranche
	// TODO : comment sont initialisés sACT et sMOI ici ?

	// Cmpt A première ligne
	std::vector<QuotasValues>& listSiA = m_quotas_view_si_a._list_ctrl.Datas();
	std::vector<QuotasValues>::iterator iti;
	iti = listSiA.begin();
	QuotasValues& valueI = (*iti);
	std::vector<QuotasValues>& listSdA = m_quotas_view_sd_a._list_ctrl.Datas();
	std::vector<QuotasValues>::iterator itd;
	itd = listSdA.begin();
	QuotasValues& valueD = (*itd);
	std::vector<QuotasValues>& listSdbA = m_quotas_view_sdb_a._list_ctrl.Datas();
	std::vector<QuotasValues>::iterator itdb;
	itdb = listSdbA.begin();
	QuotasValues& valueDB = (*itdb);
													   

	// ICI  : colonne Act A0 pour la tranche internationale
	iSomActA = valueI._act._value; // atoi(sACT);
	// ICI : colonne Act A0 pour la tranche domestique
	iSomActA += valueD._act._value; // atoi(sACT);
	// ICI : colonne Act A0 pour la tranche DB
	iSomActA += valueDB._act._value; //atoi(sACT);
	sSom.Format("%d",(iSomActA));

	// ICI : colonne Moi A0 pour la tranche internationale
	iSomMoiA = valueI._moi._value; //atoi(sMOI);
	// ICI : colonne Moi A0 pour la tranche domestique
	iSomMoiA += valueD._moi._value; //atoi(sMOI);
	// ICI : colonne Act A0 pour la tranche DB
	iSomMoiA += valueDB._moi._value; //atoi(sMOI);
	sSom.Format("%d",(iSomMoiA));
	m_iCapaMoi_A.SetWindowText(sSom);

	sCapa.Format("%d",(m_iCapaA));
	m_CapaA.SetWindowText(sCapa); 
	m_A0_Capa.SetWindowText(sCapa); 

	iEcartA = iSomActA - m_iCapaA;
	sEcart.Format("%d",(iEcartA));

	// ANOMALIE 70134 : [SATC] 8.0.6 : Bascule de Quotas impossible sur tri-tranche
	// TODO : comment sont initialisés sACT et sMOI ici ?
	// Cmpt A première ligne
	std::vector<QuotasValues>& listSiB = m_quotas_view_si_b._list_ctrl.Datas();
	std::vector<QuotasValues>::iterator iti2;
	iti2 = listSiB.begin();
	QuotasValues& valueI2 = (*iti2);
	std::vector<QuotasValues>& listSdB = m_quotas_view_sd_b._list_ctrl.Datas();
	std::vector<QuotasValues>::iterator itd2;
	itd2 = listSdB.begin();
	QuotasValues& valueD2 = (*itd2);
	std::vector<QuotasValues>& listSdbB = m_quotas_view_sdb_b._list_ctrl.Datas();
	std::vector<QuotasValues>::iterator itdb2;
	itdb2 = listSdbB.begin();
	QuotasValues& valueDB2 = (*itdb2);

	// Cmpt B deuxième ligne
	// ICI : colonne Act B0 pour la tranche internationale
	iSomActB  = valueI2._act._value; // atoi(sACT);
	// ICI : colonne Act B0 pour la tranche domestique
	iSomActB += valueD2._act._value; // atoi(sACT);
	// ICI : colonne Act B0 pour la tranche DB
	iSomActB += valueDB2._act._value; //atoi(sACT);
	sSom.Format("%d",(iSomActB));

	// ICI : colonne Moi B0 pour la tranche internationale
	iSomMoiB  = valueI2._moi._value; //atoi(sMOI);
	// ICI : colonne Moi B0 pour la tranche domestique
	iSomMoiB  += valueD2._moi._value; //atoi(sMOI);
	// ICI : colonne Moi B0 pour la tranche DB
	iSomMoiB  += valueDB2._moi._value; //atoi(sMOI);
	sSom.Format("%d",(iSomMoiB));
	m_iCapaMoi_B.SetWindowText(sSom);

	sCapa.Format("%d",(m_iCapaB));
	m_CapaB.SetWindowText(sCapa); 
	m_B0_Capa.SetWindowText(sCapa); 

	iEcartB = iSomActB - m_iCapaB;
	sEcart.Format("%d",(iEcartB));
	VerifFlagTroncon();
	GrisageTroncon();
}

void CQuotasRhealysDlg::GrpRemplissage()
{
	CString sGrp;
	sGrp.Format("%d",m_SI_AGrp);
	m_SI_A_GRP.SetWindowText(sGrp);
	sGrp.Format("%d",m_SI_BGrp);
	m_SI_B_GRP.SetWindowText(sGrp); //NCH
	sGrp.Format("%d",m_SD_AGrp);
	m_SD_A_GRP.SetWindowText(sGrp); //NCH
	sGrp.Format("%d",m_SD_BGrp);
	m_SD_B_GRP.SetWindowText(sGrp); //NCH
}

void CQuotasRhealysDlg::SetAuthFromBaseInVnl (YM_Vnl* pVnl, int nature)
{
	YmIcResaBktDom ResaBktDom;
	long tno;
	YM_VnlLeg* pVnlLeg;
	YM_VnlCmpt* pVnlCmpt;
	YM_VnlBkt* pVnlBkt;
	
	switch (nature)
	{
	case 1 : tno = m_trancheno1; break;
	case 2 : tno = m_trancheno2; break;
	case 3 : tno = m_trancheno3; break;
	default : return;
	}
	ResaBktDom.TrancheNo (tno);
	ResaBktDom.DptDate (m_lDateDep);
	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
	pSQL->SetDomain(&ResaBktDom);
	RWDBReader Reader( WinApp->m_pDatabase->Transact(pSQL, IDS_SELECT_ALL_BKT0_AUTH) );
	// ano 505566, on récupère toutes les bucket et non seulement la zéro, mais je change pas le nom de la requête
	while (Reader())
	{
		if (Reader.isValid())
		{
			Reader >> ResaBktDom;
			YM_VnlLegIterator* LegIterator = pVnl->CreateVnlLegIterator();
			for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
			{
				pVnlLeg = LegIterator->Current();
				if (pVnlLeg->GetLegOrig() == ResaBktDom.LegOrig())
				{
					YM_VnlCmptIterator* CmptIterator = pVnlLeg->CreateVnlCmptIterator();
					for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
					{
						pVnlCmpt = CmptIterator->Current();
						if (pVnlCmpt->GetCmptId() == ResaBktDom.Cmpt())
						{

							// Bucket

							YM_VnlBktIterator* BktIterator = pVnlCmpt->CreateVnlBktIterator();
							for (BktIterator->First(); ! BktIterator->Finished(); BktIterator->Next() )
							{


								pVnlBkt = BktIterator->Current();
								if (pVnlBkt->GetNestLevel() == ResaBktDom.NestLevel() && ResaBktDom.Scx() == 0)
								{				
									int vva = pVnlBkt->GetAuth();
									int bva = ResaBktDom.AuthCur();
									int vdi = pVnlBkt->GetAvail();
									int ndi = vdi + bva - vva;
									//int ndi = ResaBktDom.AuthCur() - pVnlBkt->GetBaseSold();
									if (ndi < 0)
										ndi = 0;
									pVnlBkt->SetAvail(ndi);
									//SRE 79430 - Don't overwrite VNL authorization value
									pVnlBkt->SetAuth(bva);
								}
							}
							delete BktIterator;						
						}
					}
					delete CmptIterator;
				}
			}
			delete LegIterator;
		}
	}
	
	delete pSQL;
}


void CQuotasRhealysDlg::SetAuthScxFromBaseInVnl (YM_Vnl* pVnl, int nature)
{
	YmIcResaBktDom ResaBktDom;
	long tno;
	YM_VnlLeg* pVnlLeg;
	YM_VnlCmpt* pVnlCmpt;
	YM_VnlSci* pVnlScx;

	switch (nature)
	{
	case 1 : tno = m_trancheno1; break;
	case 2 : tno = m_trancheno2; break;
	case 3 : tno = m_trancheno3; break;
	default : return;
	}
	ResaBktDom.TrancheNo (tno);
	ResaBktDom.DptDate (m_lDateDep);
	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
	pSQL->SetDomain(&ResaBktDom);
	RWDBReader Reader( WinApp->m_pDatabase->Transact(pSQL, IDS_SELECT_ALL_BKT0_AUTH) );
	// ano 505566, on récupère toutes les bucket et non seulement la zéro, mais je change pas le nom de la requête
	while (Reader())
	{
		if (Reader.isValid())
		{
			Reader >> ResaBktDom;
			YM_VnlLegIterator* LegIterator = pVnl->CreateVnlLegIterator();
			for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
			{
				pVnlLeg = LegIterator->Current();
				if (pVnlLeg->GetLegOrig() == ResaBktDom.LegOrig())
				{
					YM_VnlCmptIterator* CmptIterator = pVnlLeg->CreateVnlCmptIterator();
					for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
					{
						pVnlCmpt = CmptIterator->Current();
						if (pVnlCmpt->GetCmptId() == ResaBktDom.Cmpt())
						{							
							// SCX
							YM_VnlSciIterator* SciIterator = pVnlCmpt->CreateVnlSciIterator();
							for (SciIterator->First(); ! SciIterator->Finished(); SciIterator->Next() )
							{
								pVnlScx = SciIterator->Current();
								if (pVnlScx->GetNestLevel() == ResaBktDom.Scx() && ResaBktDom.Scx() > 0)
								{
									int vva = pVnlScx->GetAuth();
									int bva = ResaBktDom.AuthCur();
									int vdi = pVnlScx->GetAvail();
									int ndi = vdi + bva - vva;
									if (ndi < 0)
										ndi = 0;
									pVnlScx->SetAvail(ndi);
									//SRE 79430 - Don't overwrite VNL authorization value
									pVnlScx->SetAuth(bva);
								}
							}
							delete SciIterator;
						}
					}
					delete CmptIterator;
				}
			}
			delete LegIterator;
		}
	}
	
	delete pSQL;
}


void CQuotasRhealysDlg::AnalyseVnlSI(CString sOrigine,YM_Vnl &pVnlSI)
{
	YM_VnlLeg* pVnlLeg;
	YM_VnlCmpt* pVnlCmpt;
	YM_VnlBkt* pVnlBkt;
	YM_VnlSci* pVnlSci;
	m_iSomme = 0;
	m_iscSomme = 0;
	m_iCapaA = 0;
	m_iCapaB = 0;
	CString sTroncon;
	int iCount = 0;


	m_quotas_view_si_sca._list_ctrl.clearData(m_nbScxVnlA==0);
	m_quotas_view_si_scb._list_ctrl.clearData(m_nbScxVnlB==0);		

	if(m_nbScxVnlA==0){
		m_SIA_SCG_MOI.SetWindowText("0");
		m_SIA_SCG_MOI.EnableWindow(false);
	}
	if(m_nbScxVnlB==0){
		m_SIB_SCG_MOI.SetWindowText("0");
		m_SIB_SCG_MOI.EnableWindow(false);
	}
	

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
		if (pVnlLeg->GetLegOrig() == sOrigine)
			m_ComboTroncon.SetCurSel(iCount-1);

		// Boucle sur les cmpt A et B
		// en vue de sommer les capacités et les buckets 0 de chaque tronçon
		YM_VnlCmptIterator* CmptIterator = pVnlLeg->CreateVnlCmptIterator();
		for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
		{
			pVnlCmpt = CmptIterator->Current();
			// Boucle sur les buckets 0 à N
			YM_VnlBktIterator* BktIterator = pVnlCmpt->CreateVnlBktIterator();

			for ( BktIterator->First(); ! BktIterator->Finished(); BktIterator->Next() )
			{
				pVnlBkt = BktIterator->Current();

				if (pVnlCmpt->GetCmptId() == "A")
				{
					// ANOMALIE 70134 : [SATC] 8.0.6 : Bascule de Quotas impossible sur tri-tranche
					// HRI, 03/08/2015
					m_iCapaA = pVnlCmpt->GetCapacity();  // Mémorisation de la capacite Cmpt A
					if (pVnlBkt->GetNestLevel() == 0)
					{
						SumBkt0A += pVnlBkt->GetAuth();
						SumCapA  += pVnlCmpt->GetCapacity();
						TabTronconSIA(iCount,pVnlLeg->GetLegOrig(),pVnlBkt->GetAuth(), pVnlBkt->GetAvail());
					}
					if (pVnlLeg->GetLegOrig() == sOrigine)
					{
						// DM7978-NCH
						m_pVnlCmpt = pVnlCmpt;
						m_pVnlBkt = pVnlBkt;
						SIRemplissageA();
					}
				}
				else // Cmpt B
				{  
					// ANOMALIE 70134 : [SATC] 8.0.6 : Bascule de Quotas impossible sur tri-tranche
					// HRI, 03/08/2015
					m_iCapaB = pVnlCmpt->GetCapacity();  // Mémorisation de la capacite Cmpt A

					if (pVnlBkt->GetNestLevel() == 0)
					{
						SumBkt0B += pVnlBkt->GetAuth();
						SumCapB  += pVnlCmpt->GetCapacity();
						TabTronconSIB(iCount,pVnlBkt->GetAuth(), pVnlBkt->GetAvail());
					}
					if (pVnlLeg->GetLegOrig()== sOrigine)
					{
						//DM7978-NCH
						m_pVnlCmpt = pVnlCmpt;
						m_pVnlBkt = pVnlBkt;
						SIRemplissageB();
					}
				}
			}
			delete BktIterator;
			if (pVnlLeg->GetLegOrig() == sOrigine)
			{
				// traitement de fin d'espace physique
				if (strcmp(pVnlCmpt->GetCmptId(),"A") == 0)
				{
					SIRemplissageFinA();
				}
				else // Cmpt B
				{
					SIRemplissageFinB();
				}
			}

			// NBN - Traitement SCX
			//if (!((CCTAApp*)APP)->GetRhealysFlag()){
				// Boucle sur les sous contingents
				YM_VnlSciIterator* ScxIterator = pVnlCmpt->CreateVnlSciIterator();

				for ( ScxIterator->First(); ! ScxIterator->Finished(); ScxIterator->Next() )
				{
					pVnlSci = ScxIterator->Current();

					if (pVnlCmpt->GetCmptId() == "A")
					{
						// ANOMALIE 70134 : [SATC] 8.0.6 : Bascule de Quotas impossible sur tri-tranche
						// HRI, 03/08/2015
						m_iCapaA = pVnlCmpt->GetCapacity();  // Mémorisation de la capacite Cmpt A
	
						if (pVnlLeg->GetLegOrig() == sOrigine)
						{
							// DM7978-NCH
							m_pVnlCmpt = pVnlCmpt;
							m_pVnlScx = pVnlSci;
							SISCRemplissageA();
						}
					}
					else // Cmpt B
					{  
						// ANOMALIE 70134 : [SATC] 8.0.6 : Bascule de Quotas impossible sur tri-tranche
						// HRI, 03/08/2015
						m_iCapaB = pVnlCmpt->GetCapacity();  // Mémorisation de la capacite Cmpt A

						if (pVnlLeg->GetLegOrig()== sOrigine)
						{
							//DM7978-NCH
							m_pVnlCmpt = pVnlCmpt;
							m_pVnlScx = pVnlSci;
							SISCRemplissageB();
						}
					}
				}
				delete ScxIterator;
				if (pVnlLeg->GetLegOrig() == sOrigine)
				{
					// traitement de fin d'espace physique
					if (strcmp(pVnlCmpt->GetCmptId(),"A") == 0)
					{
						SISCRemplissageFinA();
					}
					else // Cmpt B
					{
						SISCRemplissageFinB();
					}
				}
			//}
			// NBN - Traitement SCX fin


		}
		delete CmptIterator;
	}
	m_iTroncon = iCount;
	delete LegIterator;
	m_bFlag =TRUE;
}

void CQuotasRhealysDlg::AnalyseVnlSD(CString sOrigine,YM_Vnl &pVnlSD)
{
	YM_VnlLeg* pVnlLeg;
	YM_VnlCmpt* pVnlCmpt;
	YM_VnlBkt* pVnlBkt;
	YM_VnlSci* pVnlScx;
	m_iSomme = 0;
	m_iscSomme = 0;
	int iCount = 0;
	m_bSNDetranger = FALSE;


	m_quotas_view_sd_sca._list_ctrl.clearData(m_nbScxVnlA==0);
	m_quotas_view_sd_scb._list_ctrl.clearData(m_nbScxVnlB==0);	
	
	if(m_nbScxVnlA==0){
		m_SDA_SCG_MOI.SetWindowText("0");
		m_SDA_SCG_MOI.EnableWindow(false);
	}
	if(m_nbScxVnlB==0){
		m_SDB_SCG_MOI.SetWindowText("0");
		m_SDB_SCG_MOI.EnableWindow(false);
	}		
	

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


			for ( BktIterator->First(); ! BktIterator->Finished(); BktIterator->Next() )
			{
				pVnlBkt = BktIterator->Current();

				if(pVnlCmpt->GetCmptId() == "A")
				{
					if (pVnlBkt->GetNestLevel() == 0)
					{
						SumBkt0A +=  pVnlBkt->GetAuth();
						TabTronconSDA(iCount, pVnlBkt->GetAuth(),  pVnlBkt->GetAvail());
						if (( pVnlBkt->GetAuth() > 0) &&
							((pVnlLeg->GetLegOrig().Left(2) != "FR") || (pVnlLeg->GetLegDest().Left(2) != "FR")))
							m_bSNDetranger = TRUE;
					}
					if (pVnlLeg->GetLegOrig() == sOrigine)
					{
						//DM7978-NCH
						m_pVnlCmpt = pVnlCmpt;
						m_pVnlBkt = pVnlBkt;
						SDRemplissageA();
					}

				}
				else // Cmpt B
				{  
					if (pVnlBkt->GetNestLevel() == 0)
					{
						SumBkt0B += pVnlBkt->GetAuth();
						TabTronconSDB(iCount, pVnlBkt->GetAuth(),  pVnlBkt->GetAvail());
					}
					if (pVnlLeg->GetLegOrig() == sOrigine)
					{
						//NCH
						m_pVnlCmpt = pVnlCmpt;
						m_pVnlBkt = pVnlBkt;
						SDRemplissageB();
					}
				}
			}
			delete BktIterator;
			if (pVnlLeg->GetLegOrig() == sOrigine)
			{
				// traitement de fin d'espace physique
				if (pVnlCmpt->GetCmptId() == "A")
				{
					SDRemplissageFinA();
				}
				else // Cmpt B
				{
					SDRemplissageFinB();
				}
			}
			
			//if (!((CCTAApp*)APP)->GetRhealysFlag()){
				// NBN - Traitement SCX
				// Boucle sur les SCX
				YM_VnlSciIterator* ScxIterator = pVnlCmpt->CreateVnlSciIterator();

				for ( ScxIterator->First(); ! ScxIterator->Finished(); ScxIterator->Next() )
				{
					pVnlScx = ScxIterator->Current();

					if(pVnlCmpt->GetCmptId() == "A")
					{
						if (pVnlScx->GetNestLevel() == 0)
						{
							if (( pVnlScx->GetAuth() > 0) &&
								((pVnlLeg->GetLegOrig().Left(2) != "FR") || (pVnlLeg->GetLegDest().Left(2) != "FR")))
								m_bSNDetranger = TRUE;
						}
						if (pVnlLeg->GetLegOrig() == sOrigine)
						{
							//DM7978-NCH
							m_pVnlCmpt = pVnlCmpt;
							m_pVnlScx = pVnlScx;
							SDSCRemplissageA();
						}

					}
					else // Cmpt B
					{  
						if (pVnlLeg->GetLegOrig() == sOrigine)
						{
							//NCH
							m_pVnlCmpt = pVnlCmpt;
							m_pVnlScx = pVnlScx;
							SDSCRemplissageB();
						}
					}
				}
				delete ScxIterator;
				if (pVnlLeg->GetLegOrig() == sOrigine)
				{
					// traitement de fin d'espace physique
					if (pVnlCmpt->GetCmptId() == "A")
					{
						SDSCRemplissageFinA();
					}
					else // Cmpt B
					{
						SDSCRemplissageFinB();
					}
				}
			//}
			// Fin NBN - Traitement SCX


		}
		delete CmptIterator;
	}
	delete LegIterator;
}

void CQuotasRhealysDlg::AnalyseVnlDB(CString sOrigine,YM_Vnl &pVnlDB)
{
	YM_VnlLeg* pVnlLeg;
	YM_VnlCmpt* pVnlCmpt;
	YM_VnlBkt* pVnlBkt;
	YM_VnlSci* pVnlScx;
	m_iSomme = 0;
	m_iscSomme = 0;
	int iCount = 0;
	if (((CCTAApp*)APP)->GetRhealysFlag()){ //pour permettre de débloquer le controle en fin de VNL
		SetAuthFromBaseInVnl (&pVnlDB, 3);   //les VNAU sont prêts mais non envoyés, on fait comme si.
		SetAuthScxFromBaseInVnl(&pVnlDB, 3);
	}else{
		m_quotas_view_sdb_sca._list_ctrl.clearData(m_nbScxVnlA==0);
		m_quotas_view_sdb_scb._list_ctrl.clearData(m_nbScxVnlB==0);		
	}

	// Boucle sur le Leg
	YM_VnlLegIterator* LegIterator = pVnlDB.CreateVnlLegIterator();
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
			BktIterator->First();
			pVnlBkt = BktIterator->Current();
	 
			if(pVnlCmpt->GetCmptId() == "A")
			{
				SumBkt0A += pVnlBkt->GetAuth();
				TabTronconDBA(iCount,pVnlBkt->GetAuth(),pVnlBkt->GetAvail());
				if (pVnlLeg->GetLegOrig() == sOrigine)
				{
					m_pVnlCmpt = pVnlCmpt;
					m_pVnlBkt = pVnlBkt;	
					DBRemplissageA();


				}
			}
			else // Cmpt B
			{
				SumBkt0B +=  pVnlBkt->GetAuth();
				TabTronconDBB(iCount, pVnlBkt->GetAuth(),  pVnlBkt->GetAvail());
				if (pVnlLeg->GetLegOrig() == sOrigine)
				{
					m_pVnlCmpt = pVnlCmpt;
					m_pVnlBkt = pVnlBkt;
					DBRemplissageB();

				}
			}
			delete BktIterator;
            // nbn
            YM_VnlSciIterator* ScxIterator = pVnlCmpt->CreateVnlSciIterator();
            ScxIterator->First();
            pVnlScx = ScxIterator->Current();
     
            if(pVnlScx != NULL){
                if(pVnlCmpt->GetCmptId() == "A")
                {

                    //TabTronconDBA(iCount,pVnlBkt->GetAuth(),pVnlBkt->GetAvail());
                    if (pVnlLeg->GetLegOrig() == sOrigine)
                    {
                        m_pVnlCmpt = pVnlCmpt;
                        m_pVnlScx = pVnlScx;    
                        DBSCRemplissageA();

                    }
                }
                else // Cmpt B
                {
    
                    //TabTronconDBB(iCount, pVnlBkt->GetAuth(),  pVnlBkt->GetAvail());
                    if (pVnlLeg->GetLegOrig() == sOrigine)
                    {
                        m_pVnlCmpt = pVnlCmpt;
                        m_pVnlScx = pVnlScx;
                        DBSCRemplissageB();

                    }
                }
            }
            delete ScxIterator;
            // fin nbn

		}
		delete CmptIterator;
	}
	delete LegIterator;
}

 
void CQuotasRhealysDlg::OnMajTroncon()
{
	CString sSelTroncon;
	CString sOrigTroncon;
	int iIndex = 0;
	
	if ( bFlagModifManuelle && (bSISCAFlagModif || bSISCBFlagModif || bSDSCAFlagModif || bSDSCBFlagModif || bDBSCAFlagModif || bDBSCBFlagModif || bSIFlagModif || bSDFlagModif || bDBFlagModif))
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
	
	bSIFlagModif =0;
	bSDFlagModif =0;
	bDBFlagModif =0;
	bSISCAFlagModif =0;
	bSDSCAFlagModif =0;
	bDBSCAFlagModif =0;
	bSISCBFlagModif =0;
	bSDSCBFlagModif =0;
	bDBSCBFlagModif =0;
	bSICopieFlagModif = 0;
	bSDCopieFlagModif = 0;
	bDBCopieFlagModif = 0;
	bDBScaCopieFlagModif = 0;
	bDBScbCopieFlagModif = 0;
}

bool CQuotasRhealysDlg::TronconInter() // renvoi vrai si la tronçon courant est "international"
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

bool CQuotasRhealysDlg::TronconDE() // renvoi vrai si la tronçon courant est "international"
{
	CString sSelTroncon;
	CString ss1, ss2;
	m_ComboTroncon.GetWindowText(sSelTroncon);
	ss1 = sSelTroncon.Left(2);
	ss2 = sSelTroncon.Mid(8,2);
	if ((ss1 == "DE") && (ss2 == "DE"))
		return false;
	else
		return true;
}

bool CQuotasRhealysDlg::TronconDE(CString sSelTroncon) // renvoi vrai si la tronçon courant est "international"
{

	if ((sSelTroncon.Left(2) == "DE") && (sSelTroncon.Mid(8,2) == "DE"))
		return false;
	else
		return true;
}

void CQuotasRhealysDlg::StartSendVnl(CString sTronconVisu)
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


void CQuotasRhealysDlg::clearListControl()
{
	bool is_inter = this->TronconInter();
	bool is_de = this->TronconDE();

	// HRI, 12/08/2015 : cas spécial SATC Alléo
	if ( ((CCTAApp*)APP)->GetRhealysFlag())
	{
		//AfxMessageBox("CQuotasRhealysDlg::clearListControl avant", MB_OK);
		for (int i=0; i < 100; i++)
		{
			m_quotas_si_a_Svg_Alleo[i] = 0;
			m_quotas_sd_a_Svg_Alleo[i] = 0;
			m_quotas_si_b_Svg_Alleo[i] = 0;
			m_quotas_sd_b_Svg_Alleo[i] = 0;
			// NBN
			m_quotas_si_sca_Svg_Alleo[i] = 0;
			m_quotas_sd_sca_Svg_Alleo[i] = 0;
			m_quotas_si_scb_Svg_Alleo[i] = 0;
			m_quotas_sd_scb_Svg_Alleo[i] = 0;
		}
		//AfxMessageBox("CQuotasRhealysDlg::clearListControl apres", MB_OK);
	}
	
	m_quotas_view_sd_a._list_ctrl.clearData(is_inter);
	m_quotas_view_sd_b._list_ctrl.clearData(is_inter);
	
	m_quotas_view_sdb_sca._list_ctrl.clearData(is_de);
	m_quotas_view_sdb_scb._list_ctrl.clearData(is_de);

	m_quotas_view_si_a._list_ctrl.clearData(false);
	m_quotas_view_si_b._list_ctrl.clearData(false);
	
	m_quotas_view_sd_a._list_ctrl.RedrawItems(0, m_quotas_view_sd_a._list_ctrl.MaxIndexation());
	m_quotas_view_sd_b._list_ctrl.RedrawItems(0, m_quotas_view_sd_b._list_ctrl.MaxIndexation());
	
	m_quotas_view_si_a._list_ctrl.RedrawItems(0, m_quotas_view_si_a._list_ctrl.MaxIndexation());
	m_quotas_view_si_b._list_ctrl.RedrawItems(0, m_quotas_view_si_b._list_ctrl.MaxIndexation());


}

void CQuotasRhealysDlg::SendNextVnl()
{
	if (m_VnlCount > 3)
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
	case 3 :
		msg.Format ("VNL%d", m_trancheno3);
		break;
	default : return;
	}
	msg += '/';
	msg += sd;
	msg += "/SC";

	/// DM-7978 - CLE 
	CYMVnlBouchon::getInstance()->setTriTranche(m_trancheno1, m_trancheno2, m_trancheno3);
	((CCTAApp*)APP)->GetResComm()->SendVnl (this, msg);
	/// DM-7978 - CLE 
}

void CQuotasRhealysDlg::TransfoForInter()
{
	bool isInter = this->TronconInter();

	// NBN - Complement ANO 89531 - Ano 92996
	// L'utilisateur peut maintenant toujours modifier les scg de la tranche2
	//m_SDA_SCG_MOI.EnableWindow(!isInter);
	//m_SDB_SCG_MOI.EnableWindow(!isInter);

}

void CQuotasRhealysDlg::LireVnl(CString reponse)
{
	CString sTronconVisu;
	YM_Vnl pVnl;
	int iGrpA = 0;
	int iGrpB = 0;

/*	
	if(i == 0){
		reponse="0 5350/20OCT-NET   1  TRANCHE 005705 /2  TRANCHE 005706 /3\n\
				5350  FRPSTFRAEG              5350  FRAEGDEQKA\n\
				IX   AC   AU   LSA  BSS   PF  IX   AC   AU   LSA  BSS   PF\n\
				A00  111    111    111    0    0  A00  111   111   111    0    0\n\
				SCG       90   90    0        SCG       90   90    0\n\
				SC01      72   72    0        SC01      72   72    0\n\
				SC02      41   41    0        SC02      41   41    0\n\
				SC03      36   36    0        SC03      36   36    0\n\
				SC04      30   30    0        SC04      30   30    0\n\
				SC05      20   20    0        SC05      20   20    0\n\
				SC06      10   10    0        SC06      10   10    0\n\
				SC07       5    5    0        SC07       5    5    0\n\
				A01         20    20    0       A01         20    20    0\n\
				A02         1    1    0       A02         1    1    0\n\
				A03         1    1    0       A03         0    0    0\n\
				A04         1    1    0       A04         0    0    0\n\
				A05         1    1    0       A05         0    0    0\n\
				B00  50    50    50    0    0  B00  50    50   50    0    0\n\
				SCG       23  	23    0        SCG       23  23     0\n\
				SC01      20   20    0        SC01      20   20    0\n\
				SC02      18   18    0        SC02      18   18    0\n\
				SC03      16   16    0        SC03      16   16    0\n\
				SC04      10   10    0        SC04      10   10    0\n\
				SC05      10   10    0        SC05      10   10    0\n\
				SC06      10   10    0        SC06      10   10    0\n\
				SC07       5    5    0        SC07       5    5    0\n\
				B01         5    5    0       B01         5    5    0\n\
				B02         1    1    0       B02         1    1    0\n\
				B03         1    1    0       B03         0    0    0\n\
				B04         1    1    0       B04         0    0    0\n\
				B05         1    1    0       B05         0    0    0";

			
	}
	if(i == 1){
		reponse="0 5705/20OCT-NET   2  TRANCHE 005350 /1  TRANCHE 005706 /3\n\
				5705  FRPSTFRAEG              5705  FRAEGDEQKA\n\
				IX   AC   AU   LSA  BSS   PF  IX   AC   AU   LSA  BSS   PF\n\
				A00  111    111    111    0    0  A00  111   111   111    0    0\n\
				SCG       90   90    0        SCG       90   90    0\n\
				SC01      72   72    0        SC01      72   72    0\n\
				SC02      41   41    0        SC02      41   41    0\n\
				SC03      36   36    0        SC03      36   36    0\n\
				SC04      30   30    0        SC04      30   30    0\n\
				SC05      20   20    0        SC05      20   20    0\n\
				SC06      10   10    0        SC06      10   10    0\n\
				SC07       5    5    0        SC07       5    5    0\n\
				A01         20    20    0       A01         20    20    0\n\
				A02         1    1    0       A02         1    1    0\n\
				A03         1    1    0       A03         0    0    0\n\
				A04         1    1    0       A04         0    0    0\n\
				A05         1    1    0       A05         0    0    0\n\
				B00  150    150    150    0    0  B00  150    150   150    0    0\n\
				SCG       123  	123    0        SCG       123  123     0\n\
				SC01      20   20    0        SC01      20   20    0\n\
				SC02      18   18    0        SC02      18   18    0\n\
				SC03      16   16    0        SC03      16   16    0\n\
				SC04      10   10    0        SC04      10   10    0\n\
				SC05      10   10    0        SC05      10   10    0\n\
				SC06      10   10    0        SC06      10   10    0\n\
				SC07       5    5    0        SC07       5    5    0\n\
				B01         5    5    0       B01         5    5    0\n\
				B02         1    1    0       B02         1    1    0\n\
				B03         1    1    0       B03         0    0    0\n\
				B04         1    1    0       B04         0    0    0\n\
				B05         1    1    0       B05         0    0    0";

			
	}	
	if(i == 2){
		reponse="0 5706/20OCT-NET   3  TRANCHE 005350 /1  TRANCHE 005705 /2\n\
				5706  FRPSTFRAEG              5706  FRAEGDEQKA\n\
				IX   AC   AU   LSA  BSS   PF  IX   AC   AU   LSA  BSS   PF\n\
				A00  111    111    111    0    0  A00  111   111   111    0    0\n\
				SCG       90   90    0        SCG       90   90    0\n\
				SC01      72   72    0        SC01      72   72    0\n\
				SC02      41   41    0        SC02      41   41    0\n\
				SC03      36   36    0        SC03      36   36    0\n\
				SC04      30   30    0        SC04      30   30    0\n\
				SC05      20   20    0        SC05      20   20    0\n\
				SC06      10   10    0        SC06      10   10    0\n\
				SC07       5    5    0        SC07       5    5    0\n\
				A01         20    20    0       A01         20    20    0\n\
				A02         1    1    0       A02         1    1    0\n\
				A03         1    1    0       A03         0    0    0\n\
				A04         1    1    0       A04         0    0    0\n\
				A05         1    1    0       A05         0    0    0\n\
				B00  150    150    150    0    0  B00  150    150   150    0    0\n\
				SCG       123  	123    0        SCG       123  123     0\n\
				SC01      20   20    0        SC01      20   20    0\n\
				SC02      18   18    0        SC02      18   18    0\n\
				SC03      16   16    0        SC03      16   16    0\n\
				SC04      10   10    0        SC04      10   10    0\n\
				SC05      10   10    0        SC05      10   10    0\n\
				SC06      10   10    0        SC06      10   10    0\n\
				SC07       5    5    0        SC07       5    5    0\n\
				B01         5    5    0       B01         5    5    0\n\
				B02         1    1    0       B02         1    1    0\n\
				B03         1    1    0       B03         0    0    0\n\
				B04         1    1    0       B04         0    0    0\n\
				B05         1    1    0       B05         0    0    0";

			i=-1;
	}	
	i++;
*/	

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
	// HRI, 10/08/2015 : pour n'afficher que les CC conformes au VNL
	bool avantInitListe = false;
	if (m_nbBktVnlA == 0 && m_nbBktVnlB == 0)
	{
		avantInitListe = true;
	}
	// Détermination du nombre de CC selon le VNL
	// (si le VNL ne renvoie que les CC A0 à A6, m_nbBktVnlA sera égal à 7)
	if (pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(0)->GetCmptId() == "A")
		m_nbBktVnlA  = pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(0)->m_pVnlBktList.GetSize();
	if (pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(1)->GetCmptId() == "B")
		m_nbBktVnlB  = pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(1)->m_pVnlBktList.GetSize();
	if (pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(0)->GetCmptId() == "A")
		m_nbScxVnlA  = pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(0)->m_pVnlSciList.GetSize();
	if (pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(1)->GetCmptId() == "B")
		m_nbScxVnlB  = pVnl.m_pVnlLegList.GetAt(0)->GetCmpt(1)->m_pVnlSciList.GetSize();
	


	// Dès qu'on dispose du premier VNL et donc de son nombre de buckets
	// on peut afficher la liste en la taillant comme il faut
	if (avantInitListe == true)
	{
		InitCtrlQuotasList();  // HRI, afficher les tableaux une fois terminé les 3 VNL
	}
	// Fin modif HRI du 10/08/2015

	if (sLastTroncon == "")               // Si Troncon Peakleg vide
		sTronconVisu = pVnl.GetLegOrig(); // Affichage du premier troncon
	else 
		sTronconVisu = sLastTroncon;
	if (pVnl.getTrancheNo() == pVnl.GetNatTranche(YM_Vnl::SN_INTER)) // HRI, 10/08/2015
	{
		AnalyseVnlSI(sTronconVisu,pVnl);
		//		   ((CCTAApp*)APP)->GetGrpTranche(m_trancheno1,m_lDateDep,pVnl.GetLegOrig(),iGrpA,iGrpB);
		((CCTAApp*)APP)->GetGrpTranche(m_trancheno1,m_lDateDep,sTronconVisu,iGrpA,iGrpB);
		m_SI_AGrp = iGrpA;
		m_SI_BGrp = iGrpB;
		m_VnlCount++;
		SendNextVnl();
		return;
	}
	if (pVnl.getTrancheNo() == pVnl.GetNatTranche(YM_Vnl::SN_DOM)) // HRI, 10/08/2015
	{
		AnalyseVnlSD(sTronconVisu,pVnl);
		//           ((CCTAApp*)APP)->GetGrpTranche(m_trancheno2,m_lDateDep,pVnl.GetLegOrig(),iGrpA,iGrpB);
		((CCTAApp*)APP)->GetGrpTranche(m_trancheno2,m_lDateDep,sTronconVisu,iGrpA,iGrpB);
		m_SD_AGrp = iGrpA;
		m_SD_BGrp = iGrpB;
		m_VnlCount++;
		SendNextVnl();
		return;
	}

	if (pVnl.getTrancheNo() == pVnl.GetNatTranche(YM_Vnl::DB)) // HRI, 10/08/2015
	{
		AnalyseVnlDB(sTronconVisu,pVnl);
		m_VnlCount++;  // HRI, 10/08/2015
	}

	if (m_VnlCount>=3) // HRI, 10/08/2015
	{
		FinVnl();
	}
}

void CQuotasRhealysDlg::FinVnl()
{
	// ICI, on affiche les tableaux

	m_SI_A_DIFF.SetTextColor(RGB(0,0,0)); //NCH
	m_SI_B_DIFF.SetTextColor(RGB(0,0,0)); //NCH
	m_SD_A_DIFF.SetTextColor(RGB(0,0,0)); //NCH
	m_SD_B_DIFF.SetTextColor(RGB(0,0,0));	//NCH 
	m_DB_A_DIFF.SetTextColor(RGB(0,0,0));	//NCH
	m_DB_B_DIFF.SetTextColor(RGB(0,0,0)); //NCH
	RecapRemplissage();
	GrpRemplissage();
	//BlancFondStatic();
	RecapDispoVert();
	TabTroncon_A_Total();
	TabTroncon_B_Total();
	TabTronconHide();
	TransfoForInter();

	if ((SumBkt0A == SumCapA) && (SumBkt0B == SumCapB))
	{
		AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(m_bSNDetranger ? IDS_SND_ETRANGER : IDS_QUOTA_OK),
			AfxGetAppName(), MB_ICONINFORMATION | MB_APPLMODAL);
		((CCTAApp*)APP)->UpdRhealysCriticalFlag (FALSE, m_trancheno1, m_lDateDep);
		((CCTAApp*)APP)->UpdRhealysCriticalFlag (FALSE, m_trancheno2, m_lDateDep);
		((CCTAApp*)APP)->UpdRhealysCriticalFlag (FALSE, m_trancheno3, m_lDateDep);
		CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
		pTrancheView->UpdRhealysCriticalFlag (FALSE, m_NumeroTrain, m_lDateDep);
	}
	if (bVnlDeControle)
	{
		FinSendVnau();
		bVnlDeControle = FALSE;
	}
	else if ( ((CCTAApp*)APP)->GetRhealysFlag())
	{ //Positionnement du EDIT_STATUS_OKAY_RHEALYS sur les tranches chargées
		CTrancheView *pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
		pTrancheView->SetEditStatus (m_trancheno1, m_lDateDep, EDIT_STATUS_OKAY_RHEALYS);
		pTrancheView->SetEditStatus (m_trancheno2, m_lDateDep, EDIT_STATUS_OKAY_RHEALYS);
		pTrancheView->SetEditStatus (m_trancheno3, m_lDateDep, EDIT_STATUS_OKAY_RHEALYS);
		((CCTAApp*)APP)->SetTrancheEditStatus(m_trancheno1, m_lDateDep, EDIT_STATUS_OKAY_RHEALYS);
		((CCTAApp*)APP)->SetTrancheEditStatus(m_trancheno2, m_lDateDep, EDIT_STATUS_OKAY_RHEALYS);
		((CCTAApp*)APP)->SetTrancheEditStatus(m_trancheno3, m_lDateDep, EDIT_STATUS_OKAY_RHEALYS);
	}
}

void CQuotasRhealysDlg::TabTronconSIA(int icount,CString sLegOrig, long lAuth, long lDisp)
{
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

void CQuotasRhealysDlg::TabTronconSIB(int icount,long lAuth, long lDisp)
{
	
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

void CQuotasRhealysDlg::TabTronconSDA(int icount,long lAuth, long lDisp)
{
	
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

void CQuotasRhealysDlg::TabTronconSDB(int icount,long lAuth, long lDisp)
{
	
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

void CQuotasRhealysDlg::TabTronconDBA(int icount,long lAuth, long lDisp)
{
	
	CString sTemp;
	switch(icount)
	{
	case 1 :
		m_DB_A0_1.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_A0_1.SetWindowText(sTemp);
		m_DB_DA0_1.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DA0_1.SetWindowText(sTemp);
		break;
	case 2 :
		m_DB_A0_2.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_A0_2.SetWindowText(sTemp);
		m_DB_DA0_2.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DA0_2.SetWindowText(sTemp);
		break;
	case 3 :
		m_DB_A0_3.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_A0_3.SetWindowText(sTemp);
		m_DB_DA0_3.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DA0_3.SetWindowText(sTemp);
		break;
	case 4 :
		m_DB_A0_4.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_A0_4.SetWindowText(sTemp);
		m_DB_DA0_4.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DA0_4.SetWindowText(sTemp);
		break;
	case 5 :
		m_DB_A0_5.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_A0_5.SetWindowText(sTemp);
		m_DB_DA0_5.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DA0_5.SetWindowText(sTemp);
		break;
	case 6 :
		m_DB_A0_6.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_A0_6.SetWindowText(sTemp);
		m_DB_DA0_6.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DA0_6.SetWindowText(sTemp);
		break;
	case 7 :
		m_DB_A0_7.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_A0_7.SetWindowText(sTemp);
		m_DB_DA0_7.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DA0_7.SetWindowText(sTemp);
		break;
	case 8 :
		m_DB_A0_8.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_A0_8.SetWindowText(sTemp);
		m_DB_DA0_8.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DA0_8.SetWindowText(sTemp);
		break;
	case 9 :
		m_DB_A0_9.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_A0_9.SetWindowText(sTemp);
		m_DB_DA0_9.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DA0_9.SetWindowText(sTemp);
		break;
	case 10 :
		m_DB_A0_1.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_A0_10.SetWindowText(sTemp);
		m_DB_DA0_10.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DA0_10.SetWindowText(sTemp);
		break;
	case 11 :
		m_DB_A0_1.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_A0_11.SetWindowText(sTemp);
		m_DB_DA0_11.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DA0_11.SetWindowText(sTemp);
		break;
	case 12 :
		m_DB_A0_1.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_A0_12.SetWindowText(sTemp);
		m_DB_DA0_12.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DA0_12.SetWindowText(sTemp);
		break;
	}
}

void CQuotasRhealysDlg::TabTronconDBB(int icount,long lAuth, long lDisp)
{
	
	CString sTemp;
	switch(icount)
	{
	case 1 :
		m_DB_B0_1.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_B0_1.SetWindowText(sTemp);
		m_DB_DB0_1.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DB0_1.SetWindowText(sTemp);
		break;
	case 2 :
		m_DB_B0_2.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_B0_2.SetWindowText(sTemp);
		m_DB_DB0_2.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DB0_2.SetWindowText(sTemp);
		break;
	case 3 :
		m_DB_B0_3.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_B0_3.SetWindowText(sTemp);
		m_DB_DB0_3.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DB0_3.SetWindowText(sTemp);
		break;
	case 4 :
		m_DB_B0_4.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_B0_4.SetWindowText(sTemp);
		m_DB_DB0_4.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DB0_4.SetWindowText(sTemp);
		break;
	case 5 :
		m_DB_B0_5.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_B0_5.SetWindowText(sTemp);
		m_DB_DB0_5.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DB0_5.SetWindowText(sTemp);
		break;
	case 6 :
		m_DB_B0_6.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_B0_6.SetWindowText(sTemp);
		m_DB_DB0_6.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DB0_6.SetWindowText(sTemp);
		break;
	case 7 :
		m_DB_B0_7.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_B0_7.SetWindowText(sTemp);
		m_DB_DB0_7.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DB0_7.SetWindowText(sTemp);
		break;
	case 8 :
		m_DB_B0_8.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_B0_8.SetWindowText(sTemp);
		m_DB_DB0_8.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DB0_8.SetWindowText(sTemp);
		break;
	case 9 :
		m_DB_B0_9.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_B0_9.SetWindowText(sTemp);
		m_DB_DB0_9.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DB0_9.SetWindowText(sTemp);
		break;
	case 10 :
		m_DB_B0_10.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_B0_10.SetWindowText(sTemp);
		m_DB_DB0_10.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DB0_10.SetWindowText(sTemp);
		break;
	case 11 :
		m_DB_B0_11.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_B0_11.SetWindowText(sTemp);
		m_DB_DB0_11.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DB0_11.SetWindowText(sTemp);
		break;
	case 12 :
		m_DB_B0_12.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lAuth);
		m_DB_B0_12.SetWindowText(sTemp);
		m_DB_DB0_12.ShowWindow(SW_SHOW);
		sTemp.Format("%d",lDisp);
		m_DB_DB0_12.SetWindowText(sTemp);
		break;
	}
}

void CQuotasRhealysDlg::UnTotal (YM_Static &m_Total, int caparef, int Som)
{
	char tot[6];
	if (Som == caparef)
		m_Total.SetTextColor(RGB(0,0,0));
	else if (Som < caparef)
		m_Total.SetTextColor(RGB(255,0,0));
	else if (Som > caparef)
		m_Total.SetTextColor(COLOR_OVB_MTRANCHE);
	m_Total.ShowWindow(SW_SHOW);
	m_Total.SetWindowText(itoa(Som, tot, 10));
	m_Total.Invalidate();
}
void CQuotasRhealysDlg::TabTroncon_A_Total()
{
	int i;
	int iSom;
	CString sTemp;

	for (i=1;i<(m_iTroncon+1);i++)
		switch(i)
	{
		case 1 :
			m_SNI_A0_1.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_1.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_A0_1.GetWindowText(sTemp);
			iSom += atoi(sTemp);    
			UnTotal (m_Total_A0_1, m_iCapaA, iSom);

			break;
		case 2 :
			m_SNI_A0_2.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_2.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_A0_2.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_2, m_iCapaA, iSom);

			break;
		case 3 :
			m_SNI_A0_3.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_3.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_A0_3.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_3, m_iCapaA, iSom);

			break;
		case 4 :
			m_SNI_A0_4.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_4.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_A0_4.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_4, m_iCapaA, iSom);

			break;
		case 5 :
			m_SNI_A0_5.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_5.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_A0_5.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_5, m_iCapaA, iSom);

			break;
		case 6 :
			m_SNI_A0_6.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_6.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_A0_6.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_6, m_iCapaA, iSom);

			break;
		case 7 :
			m_SNI_A0_7.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_7.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_A0_7.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_7, m_iCapaA, iSom);

			break;
		case 8 :
			m_SNI_A0_8.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_8.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_A0_8.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_8, m_iCapaA, iSom);

			break;
		case 9 :
			m_SNI_A0_9.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_9.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_A0_9.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_9, m_iCapaA, iSom);

			break;
		case 10 :
			m_SNI_A0_10.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_10.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_A0_10.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_10, m_iCapaA, iSom);

			break;
		case 11 :
			m_SNI_A0_11.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_11.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_A0_11.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_11, m_iCapaA, iSom);

			break;
		case 12 :
			m_SNI_A0_12.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_A0_12.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_A0_12.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_A0_12, m_iCapaA, iSom);

			break;
	}
}

void CQuotasRhealysDlg::TabTroncon_B_Total()
{
	int i;
	int iSom;
	CString sTemp;

	for (i=1;i<(m_iTroncon+1);i++)
	{
		switch(i)
		{
		case 1 :
			m_SNI_B0_1.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_1.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_B0_1.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_1, m_iCapaB, iSom);
			break;
		case 2 :
			m_SNI_B0_2.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_2.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_B0_2.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_2, m_iCapaB, iSom);
			break;
		case 3 :
			m_SNI_B0_3.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_3.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_B0_3.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_3, m_iCapaB, iSom);
			break;
		case 4 :
			m_SNI_B0_4.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_4.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_B0_4.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_4, m_iCapaB, iSom);
			break;
		case 5 :
			m_SNI_B0_5.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_5.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_B0_5.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_5, m_iCapaB, iSom);
			break;
		case 6 :
			m_SNI_B0_6.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_6.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_B0_6.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_6, m_iCapaB, iSom);
			break;
		case 7 :
			m_SNI_B0_7.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_7.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_B0_7.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_7, m_iCapaB, iSom);
			break;
		case 8 :
			m_SNI_B0_8.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_8.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_B0_8.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_8, m_iCapaB, iSom);
			break;
		case 9 :
			m_SNI_B0_9.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_9.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_B0_9.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_9, m_iCapaB, iSom);
			break;
		case 10 :
			m_SNI_B0_10.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_10.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_B0_10.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_10, m_iCapaB, iSom);
			break;
		case 11 :
			m_SNI_B0_11.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_11.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_B0_11.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_11, m_iCapaB, iSom);
			break;
		case 12 :
			m_SNI_B0_12.GetWindowText(sTemp);
			iSom = atoi(sTemp);
			m_SND_B0_12.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			m_DB_B0_12.GetWindowText(sTemp);
			iSom += atoi(sTemp);
			UnTotal (m_Total_B0_12, m_iCapaB, iSom);
			break;
		}
	}
}

void CQuotasRhealysDlg::TabTronconHide()
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
			m_DB_A0_1.ShowWindow(SW_HIDE);
			m_Total_A0_1.ShowWindow(SW_HIDE);
			m_SNI_B0_1.ShowWindow(SW_HIDE);
			m_SND_B0_1.ShowWindow(SW_HIDE);
			m_DB_B0_1.ShowWindow(SW_HIDE);
			m_Total_B0_1.ShowWindow(SW_HIDE);
			m_SNI_DA0_1.ShowWindow(SW_HIDE);
			m_SND_DA0_1.ShowWindow(SW_HIDE);
			m_DB_DA0_1.ShowWindow(SW_HIDE);
			m_SNI_DB0_1.ShowWindow(SW_HIDE);
			m_SND_DB0_1.ShowWindow(SW_HIDE);
			m_DB_DB0_1.ShowWindow(SW_HIDE);
			break;
		case 2 :
			m_Troncon_Col_2.ShowWindow(SW_HIDE);
			m_SNI_A0_2.ShowWindow(SW_HIDE);
			m_SND_A0_2.ShowWindow(SW_HIDE);
			m_DB_A0_2.ShowWindow(SW_HIDE);
			m_Total_A0_2.ShowWindow(SW_HIDE);
			m_SNI_B0_2.ShowWindow(SW_HIDE);
			m_SND_B0_2.ShowWindow(SW_HIDE);
			m_DB_B0_2.ShowWindow(SW_HIDE);
			m_Total_B0_2.ShowWindow(SW_HIDE);
			m_SNI_DA0_2.ShowWindow(SW_HIDE);
			m_SND_DA0_2.ShowWindow(SW_HIDE);
			m_DB_DA0_2.ShowWindow(SW_HIDE);
			m_SNI_DB0_2.ShowWindow(SW_HIDE);
			m_SND_DB0_2.ShowWindow(SW_HIDE);
			m_DB_DB0_2.ShowWindow(SW_HIDE);
			break;
		case 3 :
			m_Troncon_Col_3.ShowWindow(SW_HIDE);
			m_SNI_A0_3.ShowWindow(SW_HIDE);
			m_SND_A0_3.ShowWindow(SW_HIDE);
			m_DB_A0_3.ShowWindow(SW_HIDE);
			m_Total_A0_3.ShowWindow(SW_HIDE);
			m_SNI_B0_3.ShowWindow(SW_HIDE);
			m_SND_B0_3.ShowWindow(SW_HIDE);
			m_DB_B0_3.ShowWindow(SW_HIDE);
			m_Total_B0_3.ShowWindow(SW_HIDE);
			m_SNI_DA0_3.ShowWindow(SW_HIDE);
			m_SND_DA0_3.ShowWindow(SW_HIDE);
			m_DB_DA0_3.ShowWindow(SW_HIDE);
			m_SNI_DB0_3.ShowWindow(SW_HIDE);
			m_SND_DB0_3.ShowWindow(SW_HIDE);
			m_DB_DB0_3.ShowWindow(SW_HIDE);
			break;
		case 4 :
			m_Troncon_Col_4.ShowWindow(SW_HIDE);
			m_SNI_A0_4.ShowWindow(SW_HIDE);
			m_SND_A0_4.ShowWindow(SW_HIDE);
			m_DB_A0_4.ShowWindow(SW_HIDE);
			m_Total_A0_4.ShowWindow(SW_HIDE);
			m_SNI_B0_4.ShowWindow(SW_HIDE);
			m_SND_B0_4.ShowWindow(SW_HIDE);
			m_DB_B0_4.ShowWindow(SW_HIDE);
			m_Total_B0_4.ShowWindow(SW_HIDE);
			m_SNI_DA0_4.ShowWindow(SW_HIDE);
			m_SND_DA0_4.ShowWindow(SW_HIDE);
			m_DB_DA0_4.ShowWindow(SW_HIDE);
			m_SNI_DB0_4.ShowWindow(SW_HIDE);
			m_SND_DB0_4.ShowWindow(SW_HIDE);
			m_DB_DB0_4.ShowWindow(SW_HIDE);
			break;
		case 5 :
			m_Troncon_Col_5.ShowWindow(SW_HIDE);
			m_SNI_A0_5.ShowWindow(SW_HIDE);
			m_SND_A0_5.ShowWindow(SW_HIDE);
			m_DB_A0_5.ShowWindow(SW_HIDE);
			m_Total_A0_5.ShowWindow(SW_HIDE);
			m_SNI_B0_5.ShowWindow(SW_HIDE);
			m_SND_B0_5.ShowWindow(SW_HIDE);
			m_DB_B0_5.ShowWindow(SW_HIDE);
			m_Total_B0_5.ShowWindow(SW_HIDE);
			m_SNI_DA0_5.ShowWindow(SW_HIDE);
			m_SND_DA0_5.ShowWindow(SW_HIDE);
			m_DB_DA0_5.ShowWindow(SW_HIDE);
			m_SNI_DB0_5.ShowWindow(SW_HIDE);
			m_SND_DB0_5.ShowWindow(SW_HIDE);
			m_DB_DB0_5.ShowWindow(SW_HIDE);
			break;
		case 6 :
			m_Troncon_Col_6.ShowWindow(SW_HIDE);
			m_SNI_A0_6.ShowWindow(SW_HIDE);
			m_SND_A0_6.ShowWindow(SW_HIDE);
			m_DB_A0_6.ShowWindow(SW_HIDE);
			m_Total_A0_6.ShowWindow(SW_HIDE);
			m_SNI_B0_6.ShowWindow(SW_HIDE);
			m_SND_B0_6.ShowWindow(SW_HIDE);
			m_DB_B0_6.ShowWindow(SW_HIDE);
			m_Total_B0_6.ShowWindow(SW_HIDE);
			m_SNI_DA0_6.ShowWindow(SW_HIDE);
			m_SND_DA0_6.ShowWindow(SW_HIDE);
			m_DB_DA0_6.ShowWindow(SW_HIDE);
			m_SNI_DB0_6.ShowWindow(SW_HIDE);
			m_SND_DB0_6.ShowWindow(SW_HIDE);
			m_DB_DB0_6.ShowWindow(SW_HIDE);
			break;
		case 7 :
			m_Troncon_Col_7.ShowWindow(SW_HIDE);
			m_SNI_A0_7.ShowWindow(SW_HIDE);
			m_SND_A0_7.ShowWindow(SW_HIDE);
			m_DB_A0_7.ShowWindow(SW_HIDE);
			m_Total_A0_7.ShowWindow(SW_HIDE);
			m_SNI_B0_7.ShowWindow(SW_HIDE);
			m_SND_B0_7.ShowWindow(SW_HIDE);
			m_DB_B0_7.ShowWindow(SW_HIDE);
			m_Total_B0_7.ShowWindow(SW_HIDE);
			m_SNI_DA0_7.ShowWindow(SW_HIDE);
			m_SND_DA0_7.ShowWindow(SW_HIDE);
			m_DB_DA0_7.ShowWindow(SW_HIDE);
			m_SNI_DB0_7.ShowWindow(SW_HIDE);
			m_SND_DB0_7.ShowWindow(SW_HIDE);
			m_DB_DB0_7.ShowWindow(SW_HIDE);
			break;
		case 8 :
			m_Troncon_Col_8.ShowWindow(SW_HIDE);
			m_SNI_A0_8.ShowWindow(SW_HIDE);
			m_SND_A0_8.ShowWindow(SW_HIDE);
			m_DB_A0_8.ShowWindow(SW_HIDE);
			m_Total_A0_8.ShowWindow(SW_HIDE);
			m_SNI_B0_8.ShowWindow(SW_HIDE);
			m_SND_B0_8.ShowWindow(SW_HIDE);
			m_DB_B0_8.ShowWindow(SW_HIDE);
			m_Total_B0_8.ShowWindow(SW_HIDE);
			m_SNI_DA0_8.ShowWindow(SW_HIDE);
			m_SND_DA0_8.ShowWindow(SW_HIDE);
			m_DB_DA0_8.ShowWindow(SW_HIDE);
			m_SNI_DB0_8.ShowWindow(SW_HIDE);
			m_SND_DB0_8.ShowWindow(SW_HIDE);
			m_DB_DB0_8.ShowWindow(SW_HIDE);
			break;
		case 9 :
			m_Troncon_Col_9.ShowWindow(SW_HIDE);
			m_SNI_A0_9.ShowWindow(SW_HIDE);
			m_SND_A0_9.ShowWindow(SW_HIDE);
			m_DB_A0_9.ShowWindow(SW_HIDE);
			m_Total_A0_9.ShowWindow(SW_HIDE);
			m_SNI_B0_9.ShowWindow(SW_HIDE);
			m_SND_B0_9.ShowWindow(SW_HIDE);
			m_DB_B0_9.ShowWindow(SW_HIDE);
			m_Total_B0_9.ShowWindow(SW_HIDE);
			m_SNI_DA0_9.ShowWindow(SW_HIDE);
			m_SND_DA0_9.ShowWindow(SW_HIDE);
			m_DB_DA0_9.ShowWindow(SW_HIDE);
			m_SNI_DB0_9.ShowWindow(SW_HIDE);
			m_SND_DB0_9.ShowWindow(SW_HIDE);
			m_DB_DB0_9.ShowWindow(SW_HIDE);
			break;
		case 10 :
			m_Troncon_Col_10.ShowWindow(SW_HIDE);
			m_SNI_A0_10.ShowWindow(SW_HIDE);
			m_SND_A0_10.ShowWindow(SW_HIDE);
			m_DB_A0_10.ShowWindow(SW_HIDE);
			m_Total_A0_10.ShowWindow(SW_HIDE);
			m_SNI_B0_10.ShowWindow(SW_HIDE);
			m_SND_B0_10.ShowWindow(SW_HIDE);
			m_DB_B0_10.ShowWindow(SW_HIDE);
			m_Total_B0_10.ShowWindow(SW_HIDE);
			m_SNI_DA0_10.ShowWindow(SW_HIDE);
			m_SND_DA0_10.ShowWindow(SW_HIDE);
			m_DB_DA0_10.ShowWindow(SW_HIDE);
			m_SNI_DB0_10.ShowWindow(SW_HIDE);
			m_SND_DB0_10.ShowWindow(SW_HIDE);
			m_DB_DB0_10.ShowWindow(SW_HIDE);
			break;
		case 11 :
			m_Troncon_Col_11.ShowWindow(SW_HIDE);
			m_SNI_A0_11.ShowWindow(SW_HIDE);
			m_SND_A0_11.ShowWindow(SW_HIDE);
			m_DB_A0_11.ShowWindow(SW_HIDE);
			m_Total_A0_11.ShowWindow(SW_HIDE);
			m_SNI_B0_11.ShowWindow(SW_HIDE);
			m_SND_B0_11.ShowWindow(SW_HIDE);
			m_DB_B0_11.ShowWindow(SW_HIDE);
			m_Total_B0_11.ShowWindow(SW_HIDE);
			m_SNI_DA0_11.ShowWindow(SW_HIDE);
			m_SND_DA0_11.ShowWindow(SW_HIDE);
			m_DB_DA0_11.ShowWindow(SW_HIDE);
			m_SNI_DB0_11.ShowWindow(SW_HIDE);
			m_SND_DB0_11.ShowWindow(SW_HIDE);
			m_DB_DB0_11.ShowWindow(SW_HIDE);
			break;
		case 12 :
			m_Troncon_Col_12.ShowWindow(SW_HIDE);
			m_SNI_A0_12.ShowWindow(SW_HIDE);
			m_SND_A0_12.ShowWindow(SW_HIDE);
			m_DB_A0_12.ShowWindow(SW_HIDE);
			m_Total_A0_12.ShowWindow(SW_HIDE);
			m_SNI_B0_12.ShowWindow(SW_HIDE);
			m_SND_B0_12.ShowWindow(SW_HIDE);
			m_DB_B0_12.ShowWindow(SW_HIDE);
			m_Total_B0_12.ShowWindow(SW_HIDE);
			m_SNI_DA0_12.ShowWindow(SW_HIDE);
			m_SND_DA0_12.ShowWindow(SW_HIDE);
			m_DB_DA0_12.ShowWindow(SW_HIDE);
			m_SNI_DB0_12.ShowWindow(SW_HIDE);
			m_SND_DB0_12.ShowWindow(SW_HIDE);
			m_DB_DB0_12.ShowWindow(SW_HIDE);
			break;
		}
	}
}

BOOL CQuotasRhealysDlg::GetTrancheNo (CString msg)
{
	CString line1,sTitre;
	//  CString sVisu = "";
	char    pstline[128];
	char    *pszt;
	int nb = 1;
	m_nbTrancheOK = 0;
	m_trancheno1  = 0;
	m_trancheno2  = 0;
	m_trancheno3  = 0;
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
					m_trancheno2 = atoi (st);
				else if (nb == 3)
				{
					m_trancheno3 = atoi (st);
					m_nbTrancheOK = 3;
				}
				nb++;
			}
		}	
	}

	if (m_nbTrancheOK != 3)
	{
		AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TRAIN_NON_RHEALYS), AfxGetAppName(), 
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
		sTitre.Format("DB  -  %d",m_trancheno3);
		m_DB.SetWindowText(sTitre);
		sTitre.Format("Domestique DB  -  %d",m_trancheno3);
		m_DO.SetWindowText(sTitre);
		StartSendVnl(m_sPeakLeg); // Affichage du troncon Peakleg
		return TRUE;
	}
}

// récupération des numéro de tranches en base de données.
// badtrain revient à true si train trouvé, mais pas bi-tranche
BOOL CQuotasRhealysDlg::LireTrancheNo (bool &badtrain) 
{
	CString sTitre;
	YmIcTrancheLisDom TrancheDom;
	TrancheDom.TrainNo (m_NumeroTrain);
	TrancheDom.DptDate (m_lDateDep);
	int nb = 0;
	int nl = 0;
	badtrain = false;
	m_nbTrancheOK = 0;
	m_trancheno1  = 0;
	m_trancheno2  = 0;
	m_trancheno3  = 0;
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
			if (TrancheDom.Nature() == 1)
			{
				m_trancheno1 = TrancheDom.TrancheNo();
				nb++;
			}
			else if (TrancheDom.Nature() == 2)
			{
				m_trancheno2 = TrancheDom.TrancheNo();
				nb++;
			}
			else if (TrancheDom.Nature() == 3)
			{
				m_trancheno3 = TrancheDom.TrancheNo();
				nb++;
			}
		}
	}
	delete pSQL;
	if (nb == 3)
	{
		m_nbTrancheOK = 3;
		if (m_lRrdIndex < 0)
			m_lRrdIndex = ((CCTAApp*)APP)->GetLastRrdIndexOnLastBatch (m_trancheno1, m_lDateDep);
		sTitre.Format("SNCF International  -  %d",m_trancheno1);
		m_SI.SetWindowText(sTitre);
		sTitre.Format("SNCF France  -  %d",m_trancheno2);
		m_SD.SetWindowText(sTitre);
		sTitre.Format("DB  -  %d",m_trancheno3);
		m_DB.SetWindowText(sTitre);
		sTitre.Format("Domestique DB  -  %d",m_trancheno3);
		m_DO.SetWindowText(sTitre);
		StartSendVnl(m_sPeakLeg); // Affichage du troncon Peakleg
		return TRUE;
	}
	else
	{
		if (nl)
		{ // on a trouvé le train, mais ce n'est pas un bi-tranche
			AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TRAIN_NON_RHEALYS), AfxGetAppName(), 
				MB_ICONERROR | MB_APPLMODAL);
			badtrain = true;
		}
		return FALSE;
	}
}



CString CQuotasRhealysDlg::MakeStrDate (RW4Byte date, BOOL bannee /*= FALSE*/)
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

void CQuotasRhealysDlg::OnNotify( YM_Observable* pObject )
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

			AfxGetMainWnd()->MessageBox(serr, AfxGetAppName(),MB_ICONERROR | MB_APPLMODAL);
			if ((pMsg->GetNat() == YM_Msg::MSG_DISPO) ||
				(pMsg->GetNat() == YM_Msg::MSG_VNL))
			{
				OnCancel();
			}
			else
			{
				SendNextVnau();  // On envoi quand même les VNAU suivants
			}
		} 
		else if (pMsg->GetNat() == YM_Msg::MSG_DISPO)
		{
			GetTrancheNo (pMsg->GetMsg());
		}
		else if (pMsg->GetNat() == YM_Msg::MSG_VNL)
		{
			LireVnl(pMsg->GetMsg());

			if(false){

				int maxA = App().GetDocument()->m_pGlobalSysParms->GetMaxCmptBkt('A');
				int maxB = App().GetDocument()->m_pGlobalSysParms->GetMaxCmptBkt('B');
				int maxScA = App().GetDocument()->m_pGlobalSysParms->GetMaxSciBkt('A');
				int maxScB = App().GetDocument()->m_pGlobalSysParms->GetMaxSciBkt('B');

				// A
				for (int i = 0;i<maxA;i++){
					m_quotas_view_si_a._list_ctrl.SetLine(i, 0,	maxA-(i), maxA-(i), maxA-(i));
				}
				for (int i = 0;i<maxA;i++){
					m_quotas_view_sd_a._list_ctrl.SetLine(i, 0,	maxA-(i), maxA-(i), maxA-(i));
				}
				for (int i = 0;i<maxScA;i++){
					m_quotas_view_si_sca._list_ctrl.SetLine(i, 0,maxScA-(i), maxScA-(i), maxScA-(i));
				}
				for (int i = 0;i<maxScA;i++){
					m_quotas_view_sd_sca._list_ctrl.SetLine(i, 0,maxScA-(i), maxScA-(i), maxScA-(i));
				}
	
			}


		}
		else
		{
			SendNextVnau();
		}
	}
}


HBRUSH CQuotasRhealysDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);


	return hbr;
}

void CQuotasRhealysDlg::OnOK()
{
	CWnd* pWnd = GetFocus();
	pWnd->PostMessage(WM_KEYDOWN, 0x09, 0);
	//CDialog::OnOK();
}

void CQuotasRhealysDlg::VerifFlagTroncon()
{
	CString sMOI,sRecap;
	int     iMoi,iRecap;
	int countOK = 0;
	m_FlagTroncon = FALSE;
	m_iCapaMoi_A.GetWindowText(sMOI);
	iMoi = atoi(sMOI);
	m_CapaA.GetWindowText(sRecap);
	iRecap = atoi(sRecap);
	if (iMoi > iRecap)
		m_iCapaMoi_A.SetTextColor (COLOR_OVB_MTRANCHE);
	else if (iMoi < iRecap)
		m_iCapaMoi_A.SetTextColor (RGB (255, 0, 0));
	else
	{
		countOK ++;
		m_iCapaMoi_A.SetTextColor (RGB (0, 0, 0));
	}

	m_iCapaMoi_B.GetWindowText(sMOI);
	iMoi = atoi(sMOI);
	m_CapaB.GetWindowText(sRecap);
	iRecap = atoi(sRecap);
	if (iMoi > iRecap)
		m_iCapaMoi_B.SetTextColor (COLOR_OVB_MTRANCHE);
	else if (iMoi < iRecap)
		m_iCapaMoi_B.SetTextColor (RGB (255, 0, 0));
	else
	{
		countOK ++;
		m_iCapaMoi_B.SetTextColor (RGB (0, 0, 0));
	}
	m_FlagTroncon = (countOK == 2);
}


void CQuotasRhealysDlg::MajAuthSD(int NoTranche, CString sLegOrig, CString sCmpt)
{
	if (m_lRrdIndex < 0)
		return;

	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);

	YmIcResaBktDom pResaBkt;
	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);

	QuotasAutorisationsListCtrl* list = NULL;
	
	if (sCmpt == "A")
	{
		list = &m_quotas_view_sd_a._list_ctrl;	
	}
	else if (sCmpt == "B")
	{
		list = &m_quotas_view_sd_b._list_ctrl;	
	}

	if (list == NULL)
	{
		delete pSQL;
		return ;
	}

	std::vector<QuotasValues>& data = list->Datas();
	std::vector<QuotasValues>::iterator it;
	for(it = data.begin(); it != data.end(); ++it)
	{
		QuotasValues& values = (*it);
		pResaBkt.AuthUser(values._moi._value);
		pResaBkt.AuthCur(values._moi._value);
		pResaBkt.NestLevel(values._nest_level._value);

		pSQL->SetDomain(&pResaBkt);
		WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH);
	}	

	delete pSQL;
}

// NBN ANO 94818 - Ajout de méthode pour maj base de données AUTH_USER pour SATC ALLEO
void CQuotasRhealysDlg::MajAuthSDRhealys(int NoTranche, CString sLegOrig, CString sCmpt)
{
	if (m_lRrdIndex < 0)
		return;

	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);

	YmIcResaBktDom pResaBkt;
	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);

	// NBN ANO 94818
	int nbMax;
	nbMax = m_nbBktVnlA;
	if (sCmpt == 'B')
	{
		nbMax = m_nbBktVnlB;
	}
	
	// NBN - ANO 94818 - Partir de 0 pour récupérer toutes les infos des CC
	for(int i = 0 ; i < nbMax; i++)
	{
		int moi = (sCmpt == 'A' ? m_quotas_sd_a_Svg_Alleo[i] : m_quotas_sd_b_Svg_Alleo[i]);

		pResaBkt.AuthUser(moi);
		pResaBkt.AuthCur(moi);
		pResaBkt.NestLevel(i);

		pSQL->SetDomain(&pResaBkt);
		WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH);
	}

	delete pSQL;
}

void CQuotasRhealysDlg::MajAuthSDSC(int NoTranche, CString sLegOrig, CString sCmpt)
{
	if (m_lRrdIndex < 0)
		return;

	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);

	YmIcResaBktDom pResaBkt;
	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);

	QuotasAutorisationsListCtrl* list = NULL;
	
	if (sCmpt == "A")
	{
		list = &m_quotas_view_sd_sca._list_ctrl;	
	}
	else if (sCmpt == "B")
	{
		list = &m_quotas_view_sd_scb._list_ctrl;	
	}

	if (list == NULL)
	{
		delete pSQL;
		return ;
	}

	std::vector<QuotasValues>& data = list->Datas();
	std::vector<QuotasValues>::iterator it;
	for(it = data.begin(); it != data.end(); ++it)
	{
		
		QuotasValues& values = (*it);
		if(values._nest_level._value > 0){
			pResaBkt.AuthUser(values._moi._value);
			pResaBkt.AuthCur(values._moi._value);
			pResaBkt.NestLevel(values._nest_level._value);

			pSQL->SetDomain(&pResaBkt);
			WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);
		}
	}	

	delete pSQL;
}


void CQuotasRhealysDlg::MajAuthSISC(int NoTranche, CString sLegOrig, CString sCmpt)
{
	if (m_lRrdIndex < 0)
		return;

	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);

	YmIcResaBktDom pResaBkt;
	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);

	QuotasAutorisationsListCtrl* list = NULL;
	
	if (sCmpt == "A")
	{
		list = &m_quotas_view_si_sca._list_ctrl;	
	}
	else if (sCmpt == "B")
	{
		list = &m_quotas_view_si_scb._list_ctrl;	
	}

	if (list == NULL)
	{
		delete pSQL;
		return ;
	}
	std::vector<QuotasValues>& data = list->Datas();
	std::vector<QuotasValues>::iterator it;
	for(it = data.begin(); it != data.end(); ++it)
	{
		QuotasValues& values = (*it);
		if(values._nest_level._value > 0){
			
			pResaBkt.AuthUser(values._moi._value);
			pResaBkt.AuthCur(values._moi._value);
			pResaBkt.NestLevel(values._nest_level._value);

			pSQL->SetDomain(&pResaBkt);
			WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);
		}
	}	


	delete pSQL;
}

void CQuotasRhealysDlg::MajAuthSI(int NoTranche, CString sLegOrig, CString sCmpt)
{
	if (m_lRrdIndex < 0)
		return;

	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);

	YmIcResaBktDom pResaBkt;
	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);

	QuotasAutorisationsListCtrl* list = NULL;
	
	if (sCmpt == "A")
	{
		list = &m_quotas_view_si_a._list_ctrl;	
	}
	else if (sCmpt == "B")
	{
		list = &m_quotas_view_si_b._list_ctrl;	
	}

	if (list == NULL)
	{
		delete pSQL;
		return ;
	}
	std::vector<QuotasValues>& data = list->Datas();
	std::vector<QuotasValues>::iterator it;
	for(it = data.begin(); it != data.end(); ++it)
	{
		QuotasValues& values = (*it);
		pResaBkt.AuthUser(values._moi._value);
		pResaBkt.AuthCur(values._moi._value);
		pResaBkt.NestLevel(values._nest_level._value);

		pSQL->SetDomain(&pResaBkt);
		WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH);
	}	


	delete pSQL;
}

// NBN ANO 94818 - Ajout de méthode pour maj base de données AUTH_USER pour SATC ALLEO
void CQuotasRhealysDlg::MajAuthSIRhealys(int NoTranche, CString sLegOrig, CString sCmpt)
{
	if (m_lRrdIndex < 0)
		return;

	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);

	YmIcResaBktDom pResaBkt;
	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);

	// NBN ANO 94818
	int nbMax;
	nbMax = m_nbBktVnlA;
	if (sCmpt == 'B')
	{
		nbMax = m_nbBktVnlB;
	}
	
	// NBN - ANO 94818 - Partir de 0 pour récupérer toutes les infos des CC
	for(int i = 0 ; i < nbMax; i++)
	{
		int moi = (sCmpt == 'A' ? m_quotas_si_a_Svg_Alleo[i] : m_quotas_si_b_Svg_Alleo[i]);

		pResaBkt.AuthUser(moi);
		pResaBkt.AuthCur(moi);
		pResaBkt.NestLevel(i);

		pSQL->SetDomain(&pResaBkt);
		WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH);
	}

	delete pSQL;
}

void CQuotasRhealysDlg::MajAuthDB(int NoTranche, CString sLegOrig, CString sCmpt)
{
	if (m_lRrdIndex < 0)
		return;

	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);

	YmIcResaBktDom pResaBkt;
	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);

	QuotasAutorisationsListCtrl* list = NULL;
	
	if (sCmpt == "A")
	{
		list = &m_quotas_view_sdb_a._list_ctrl;	
	}
	else if (sCmpt == "B")
	{
		list = &m_quotas_view_sdb_b._list_ctrl;	
	}

	if (list == NULL)
	{
		delete pSQL;
		return ;
	}
	std::vector<QuotasValues>& data = list->Datas();
	std::vector<QuotasValues>::iterator it;
	for(it = data.begin(); it != data.end(); ++it)
	{
		QuotasValues& values = (*it);
		pResaBkt.AuthUser(values._moi._value);
		pResaBkt.AuthCur(values._moi._value);
		pResaBkt.NestLevel(values._nest_level._value);

		pSQL->SetDomain(&pResaBkt);
		WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH);

		// NBN ANO 94818
		// Bien que l'on ne voit que la CC0 sur la tranche 3 
		// il faut modifier la CC0 en base CC0=CC1
		if (values._nest_level._value == 0 )
		{
			pResaBkt.AuthUser(values._moi._value);
			pResaBkt.AuthCur(values._moi._value);
			pResaBkt.NestLevel(1);
			pSQL->SetDomain(&pResaBkt);
			WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH);
		}
	}	

	delete pSQL;
}

void CQuotasRhealysDlg::MajAuthDBSC(int NoTranche, CString sLegOrig, CString sCmpt)
{
	if (m_lRrdIndex < 0)
		return;

	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);

	YmIcResaBktDom pResaBkt;
	int ilevel = 0;
	pResaBkt.TrancheNo(NoTranche);
	pResaBkt.DptDate(m_lDateDep);
	pResaBkt.LegOrig(sLegOrig);
	pResaBkt.Cmpt(sCmpt);
	pResaBkt.RrdIndex(m_lRrdIndex);

	QuotasAutorisationsListCtrl* list = NULL;
	
	if (sCmpt == "A")
	{
		list = &m_quotas_view_sdb_sca._list_ctrl;	
	}
	else if (sCmpt == "B")
	{
		list = &m_quotas_view_sdb_scb._list_ctrl;	
	}

	if (list == NULL)
	{
		delete pSQL;
		return ;
	}
	std::vector<QuotasValues>& data = list->Datas();
	std::vector<QuotasValues>::iterator it;
	for(it = data.begin(); it != data.end(); ++it)
	{
		QuotasValues& values = (*it);
		if(values._nest_level._value > 0){		
			
			pResaBkt.AuthUser(values._moi._value);
			pResaBkt.AuthCur(values._moi._value);
			pResaBkt.NestLevel(values._nest_level._value);

			pSQL->SetDomain(&pResaBkt);
			WinApp->m_pDatabase->Transact (pSQL, IDS_SQL_UPDMULT_RESA_BKT_AUTH_SCI);
		}
	}	

	delete pSQL;
}


void CQuotasRhealysDlg::GrisageTroncon()
{
	((CButton *)GetDlgItem(IDC_UN_TRONCON))->EnableWindow(m_FlagTroncon);
	//((CButton *)GetDlgItem(IDC_TOUS_TRONCONS))->EnableWindow(m_FlagTroncon);
	((CButton *)GetDlgItem(IDC_SEL_TRONCONS))->EnableWindow(m_FlagTroncon);
}

BOOL CQuotasRhealysDlg::GetSelTroncon(CString sChoix, CString &sListTroncon)
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

void CQuotasRhealysDlg::SetTitle(CString NewTitle)
{
	m_szTitle = NewTitle;
}

void CQuotasRhealysDlg::Chercher()
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

void CQuotasRhealysDlg::OnChercher()
{
	if ( bFlagModifManuelle && (bSISCAFlagModif || bSISCBFlagModif || bSDSCAFlagModif || bSDSCBFlagModif || bDBSCAFlagModif || bDBSCBFlagModif || bSIFlagModif || bSDFlagModif || bDBFlagModif))
	{
		if ( AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_MODIF_DONE),
			AfxGetAppName(), MB_OKCANCEL | MB_APPLMODAL | MB_ICONEXCLAMATION) == IDCANCEL )
			return;
	}
	bSIFlagModif =0;
	bSDFlagModif =0;
	bDBFlagModif =0;
	bSISCAFlagModif =0;
	bSISCBFlagModif =0;
	bSDSCAFlagModif =0;
	bSDSCBFlagModif =0;
	bDBSCAFlagModif =0;
	bDBSCBFlagModif =0;
	int iIndex;
	m_bFlag = FALSE;
	if (!UpdateData(TRUE)) // excute le doDataExchange pour recuperer 
		return;            // le numero de train et la date de départ
	int iCount = m_ListTroncon.GetCount();
	for (iIndex=iCount;iIndex>=0;iIndex--)
		m_ListTroncon.DeleteString(iIndex);

	iCount = m_ComboTroncon.GetCount();
	for (iIndex=iCount;iIndex>=0;iIndex--)
		m_ComboTroncon.DeleteString(iIndex);

	iCount = m_ListTroncon.GetCount();
	iCount = m_ComboTroncon.GetCount();
	// c'est un VNAV
	RWDate m_rwBegDate(m_startdate.GetDay(), m_startdate.GetMonth(), m_startdate.GetYear());
	CString sDate;
	sDate.Format( "%2.2u/%2.2u/%2.2u", m_rwBegDate.dayOfMonth(), m_rwBegDate.month(), m_rwBegDate.year() % 100 );
	m_lDateDep = m_rwBegDate.julian ();
	m_lRrdIndex = -1;
	m_sPeakLeg.Empty();
	Chercher();
	// Titre de la fenêtre Quotas Rhealys
	CString sTitle = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_QUOTA_TITLE);
	m_szTitle.Format("%s   -   %s   %s     %s  -  %s",
		sTitle,m_NumeroTrain,sDate,m_Origine,m_Destination);
	SetWindowText(m_szTitle);
}

// DM 7171. les utilisateur ALLEO n'envoi pas les VNAU à la COMM, ils se contentent de les poser en base
// et de façon non envoyables par la COMM.
void CQuotasRhealysDlg::PutRhealysVnau (CString svnau, CString sTroncon, int nature)
{
	YmIcVnauDom VnauDom;
	BOOL bCrossMidnight = sTroncon.Find ("-") < 0;
	// dans la liste des tronçons, ceux payant un départ après minuit sont présenté avec "..." entre les deux
	// codes gare, sinon le séparateur est " - ". Donc si on ne trouve pas "-" il s'agit d'un départ le lendemain 
	switch (nature)
	{
	case 1 : VnauDom.TrancheNo (m_trancheno1); break;
	case 2 : VnauDom.TrancheNo (m_trancheno2); break;
	case 3 : VnauDom.TrancheNo (m_trancheno3); break;
	default : return;
	}
	VnauDom.CommandSource ("A"); // On met "A" pour qu'il soit ignoré par un éventuel RESEND
	VnauDom.StatusSend (3); // On met met 3 pour les reconnaitre et éviter envoi par REEMET.
	VnauDom.DptDate (m_lDateDep);
	VnauDom.LegOrig (sTroncon.Left(5));
	VnauDom.LegDest (sTroncon.Right(5));
	VnauDom.CommandText (svnau);
	CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
	pSQL->SetDomain(&VnauDom);
	WinApp->m_pDatabase->Transact(pSQL, IDS_SQL_INSERT_VNAU_SYSDATE);
	delete pSQL;
	/** Pour voir le résultat des VNAU "émis" (en fait sauvé avec COMMAND_SOURCE='A'
	depuis SATC Rhealys
	CString toto;
	toto.Format("VNAU Rhealys nature=%d   Command_text=%s", 
		nature, svnau);
	AfxMessageBox(toto);
	**/
}

// Pour les VNAU du SATC Alleo des tranches internationales, il faut retrouver
// les CC A1 à An et B1 à Bn qui sont masquées, mais doivent apparaître dans le VNL.
// La méthode ci-dessous génère la partie du VNAU correspondant à ce qui a été
// récupéré du VNL, éventuellement ramené à la valeur de CC0 (si supérieure initialement)
// HRI : méthode rajoutée le 12/08/2015
CString CQuotasRhealysDlg::ComplementVNAUAlleo(char cmpt, int listSvg[], long authCC0)
{
	CString complement;
	CString ssw;
	int nestLevel = 0;
	int nbMax;
	nbMax = m_nbBktVnlA;
	if (cmpt == 'B')
	{
		nbMax = m_nbBktVnlB;
	}
	
	// NBN - ANO 94818 - Partir de 0 pour récupérer toutes les infos des CC
	for(int i = 0 ; i < nbMax; i++)
	{
		nestLevel = i;
		int moi = listSvg[i];

		// NBN ANO 82727 - CC0 = CC1
		if(i == 1 || i == 0){
			moi = authCC0;
		}
		// Si supérieur à la CC0, on égalise
		if (moi > authCC0)
		{
			moi = authCC0;
		}
		
		// NBN - ANO 94818
		// On part de 1 car la valeur de la CC0 est déjà dans le VNAU
		if (i == 1){
			ssw.Format("/%c%d-%d", cmpt, nestLevel, authCC0);
		}
		if (i > 1){
			ssw.Format("/%c%d-%d", cmpt, nestLevel, moi);
		}

		// NBN - Ano 94818. Mise à jour des auto envoyées dans le tableau de sauvegarde
		// Utilisé pour la mise à jour de AUTH_USER en base de données (YM_TDLPJNG)
		listSvg[i] = moi;

		complement += ssw;
	}
	
	return complement;
}

void CQuotasRhealysDlg::HideScSiDbForAlleo()
{
	m_quotas_view_si_sca.ShowWindow(SW_HIDE);
	m_quotas_view_sd_sca.ShowWindow(SW_HIDE);
	m_SI_SC.ShowWindow(SW_HIDE);
	m_SD_SC.ShowWindow(SW_HIDE);

	m_SIA_SCG_MOI.ShowWindow(SW_HIDE);	
	m_SDA_SCG_MOI.ShowWindow(SW_HIDE);	
	m_SIA_SCG_ACT.ShowWindow(SW_HIDE);	
	m_SDA_SCG_ACT.ShowWindow(SW_HIDE);

	m_SDB_SCG_ACT.ShowWindow(SW_HIDE);
	m_SIB_SCG_MOI.ShowWindow(SW_HIDE);
	m_SIB_SCG_ACT.ShowWindow(SW_HIDE);
	m_SDB_SCG_MOI.ShowWindow(SW_HIDE);
}

// ------------------------------------------------------------

// Construction des VNAU en fonction des changements effectués
// ------------------------------------------------------------
void CQuotasRhealysDlg::BuildVnau(CString sListTroncon,BOOL siModif, BOOL sdModif, BOOL dbModif, BOOL dbScaModif, BOOL dbScbModif, BOOL isACopie) 
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

	CString sVnau,sSI,sSD,sDB, ssw;
	CString sCmpt;
	char* pbuf;
	char* tok;
	CString stok;
	pbuf =(char*)malloc (sListTroncon.GetLength()+1);
	strcpy (pbuf, sListTroncon);
	tok = strtok (pbuf,delim);
	CQuotaVnau *pQuotaVnau;
	BOOL bt1, bt2, bt3;
	bt1 = bt2 = bt3 = TRUE;
	BOOL bFirst = TRUE;
	CTrancheView *pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
	CLegView* pLegView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetLegView();
	unsigned long datevnau; // date mise dans le texte du VNAU

	// HRI, 12/08/2015 : cas particulier ALLEO : on conserve la CC0 pour les tranches internationales et domestiques
	long authCC0Alleo = 0;
	int nestLevel = 0;
	while (tok)
	{
		stok = tok;
		datevnau = m_lDateDep;
		if (stok.Find ("-") < 0)
			datevnau += 1;
		if (siModif /*|| bAllFlagModif*/)
		{   
			// Lecture des valeurs MOI pour la CMPT A
			std::vector<QuotasValues>& listSiA = m_quotas_view_si_a._list_ctrl.Datas();
			std::vector<QuotasValues>::iterator it;
			for(it = listSiA.begin() ; it != listSiA.end(); ++it)
			{
				QuotasValues& value = (*it);
				nestLevel = value._nest_level._value;
				// HRI, 07/08/2015 : ne remplir le VNAU qu'à hauteur de ce qu'indique le VNL
				if (nestLevel >= m_nbBktVnlA)  
					break;
				int moi = value._moi._value;
				if (nestLevel == 0)
				{
					sSI.Format("A%d-%d", nestLevel, moi);
					authCC0Alleo = moi;
				}
				else
				{
					ssw.Format("/A%d-%d", nestLevel, moi);
					sSI += ssw;
				}
			}
			// Cas SATC Alleo, prendre les CC tranche internationale devenues invisibles à l'affichage
			// HRI, 12/08/2015
			if (nestLevel == 0 && ((CCTAApp*)APP)->GetRhealysFlag())
			{
				CString complementVnauAlleo;
				complementVnauAlleo = ComplementVNAUAlleo('A', m_quotas_si_a_Svg_Alleo, authCC0Alleo);
				sSI += complementVnauAlleo;
			}

			// Lecture des valeurs MOI pour la CMPT B
			std::vector<QuotasValues>& listSiB = m_quotas_view_si_b._list_ctrl.Datas();
			for(it = listSiB.begin() ; it != listSiB.end(); ++it)
			{
				QuotasValues& value = (*it);
				nestLevel = value._nest_level._value;
				// HRI, 07/08/2015 : ne remplir le VNAU qu'à hauteur de ce qu'indique le VNL
				if (nestLevel >= m_nbBktVnlB)
					break;
				int moi = value._moi._value;
				if (nestLevel == 0)
				{
					authCC0Alleo = moi;
				}
				ssw.Format("/B%d-%d", nestLevel, moi);
				sSI += ssw;
			}
			// Cas SATC Alleo, prendre les CC tranche internationale devenues invisibles à l'affichage
			// HRI, 12/08/2015
			if (nestLevel == 0 && ((CCTAApp*)APP)->GetRhealysFlag())
			{
				CString complementVnauAlleo;
				complementVnauAlleo = ComplementVNAUAlleo('B', m_quotas_si_b_Svg_Alleo, authCC0Alleo);
				sSI += complementVnauAlleo;
			}
			sVnau.Format("VNAU%d/%s%s%s/%s",m_trancheno1,MakeStrDate(datevnau,FALSE),
				stok.Left(5),stok.Right(5),sSI);
			
			// Changement DT ALLEO - Envoit en direct les tranches 1 et 2
			pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 1);
			m_VnauList.Add ((CObject*)pQuotaVnau);


			if (bt1)
			{
				// envoi en direct tout le temps
				pTrancheView->SetSentFlag (m_trancheno1, m_lDateDep, 1);
				bt1 = FALSE;
			}
			
			if (bFirst)
			{
				pLegView->SetSentFlag (m_trancheno1, stok.Left(5), m_lDateDep, 1);
				((CCTAApp*)APP)->SetLegSentFlag (false, m_trancheno1, sLegsOrig, m_lDateDep);
				
				// ANo 94818 - NBN - Mise à jour des autos en base
				if (((CCTAApp*)APP)->GetRhealysFlag()){
					MajAuthSIRhealys(m_trancheno1, sLegsOrig, "A");
					MajAuthSIRhealys(m_trancheno1, sLegsOrig, "B");
				}else{					
					sCmpt = "A";
					
					/// DM-7978 - CLE
					MajAuthSI(m_trancheno1, sLegsOrig, sCmpt);

					sCmpt = "B";

					MajAuthSI(m_trancheno1, sLegsOrig, sCmpt);
				}
			}
		}
		if (sdModif /*|| bAllFlagModif*/)
		{  
			// Lecture des valeurs MOI pour cmpt A
			std::vector<QuotasValues>& listSdA = m_quotas_view_sd_a._list_ctrl.Datas();
			std::vector<QuotasValues>::iterator it;
			for(it = listSdA.begin() ; it != listSdA.end(); ++it)
			{
				QuotasValues& value = (*it);
				nestLevel = value._nest_level._value;
				int moi = value._moi._value;
				if (nestLevel == 0)
				{
					sSD.Format("A%d-%d", nestLevel, moi);
					authCC0Alleo = moi;
				}
				else
				{
					ssw.Format("/A%d-%d", nestLevel, moi);
					sSD += ssw;
				}
			}
			// Cas SATC Alleo, prendre les CC tranche domesyique devenues invisibles à l'affichage
			// HRI, 12/08/2015
			if (nestLevel == 0 && ((CCTAApp*)APP)->GetRhealysFlag())
			{
				CString complementVnauAlleo;
				complementVnauAlleo = ComplementVNAUAlleo('A', m_quotas_sd_a_Svg_Alleo, authCC0Alleo);
				sSD += complementVnauAlleo;
			}
	
			// Lecture des valeurs MOI pour cmpt A
			std::vector<QuotasValues>& listSdB = m_quotas_view_sd_b._list_ctrl.Datas();
			for(it = listSdB.begin() ; it != listSdB.end(); ++it)
			{
				QuotasValues& value = (*it);
				nestLevel = value._nest_level._value;
				int moi = value._moi._value;
				if (nestLevel == 0)
				{
					authCC0Alleo = moi;
				}
				ssw.Format("/B%d-%d", nestLevel, moi);
				sSD += ssw;
			}
			// Cas SATC Alleo, prendre les CC tranche domestique devenues invisibles à l'affichage
			// HRI, 12/08/2015
			if (nestLevel == 0 && ((CCTAApp*)APP)->GetRhealysFlag())
			{
				CString complementVnauAlleo;
				complementVnauAlleo = ComplementVNAUAlleo('B', m_quotas_sd_b_Svg_Alleo, authCC0Alleo);
				sSD += complementVnauAlleo;
			}
			sVnau.Format("VNAU%d/%s%s%s/%s",m_trancheno2,MakeStrDate(datevnau,FALSE),
				stok.Left(5),stok.Right(5),sSD);

			// Envoi tout le temps en direct
			pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 2);
			m_VnauList.Add ((CObject*)pQuotaVnau);			

			if (bt2)
			{
				pTrancheView->SetSentFlag (m_trancheno2, m_lDateDep, 1);
				bt2 = FALSE;
			}

			pLegView->SetSentFlag (m_trancheno2, stok.Left(5), m_lDateDep, 1);

			if (bFirst)
			{
				pLegView->SetSentFlag (m_trancheno2, stok.Left(5), m_lDateDep, 1);
				((CCTAApp*)APP)->SetLegSentFlag (false, m_trancheno2, sLegsOrig, m_lDateDep);
				
				// ANo 94818 - NBN - Mise à jour des autos en base
				if (((CCTAApp*)APP)->GetRhealysFlag()){
					MajAuthSDRhealys(m_trancheno2, sLegsOrig, "A");
					MajAuthSDRhealys(m_trancheno2, sLegsOrig, "B");
				}else{				
					sCmpt = "A";
					/// DM-7978 - CLE
					MajAuthSD(m_trancheno2, sLegsOrig, sCmpt);

					sCmpt = "B";
					
					// TDR - Anomalie 79005
					MajAuthSD(m_trancheno2, sLegsOrig, sCmpt);
				}
			}
		}
		if (dbModif /*|| bAllFlagModif*/)
		{   
			/*
			// Lecture des valeurs MOI
			LectureMoiDB(iMoiA0, iMoiB0);
			// Constitution de la chaîne VNAU (VNAUN°Tranche/DateOD/A0-valeur/A1-valeur/...
			sDB.Format("A0-%d/A1-0/B0-%d/B1-0",iMoiA0,iMoiB0);
			*/

			// Lecture des valeurs MOI
			std::vector<QuotasValues>& listDdA = m_quotas_view_sdb_a._list_ctrl.Datas();
			std::vector<QuotasValues>::iterator it;
			nestLevel = 0;
			for(it = listDdA.begin() ; it != listDdA.end(); ++it)
			{
				QuotasValues& value = (*it);
				nestLevel = value._nest_level._value;
				int moi = value._moi._value;
				if (nestLevel == 0){
					// NBN Ano 82727
					sDB.Format("A%d-%d/A1-%d", nestLevel, moi,moi);
				}
				else
				{
					ssw.Format("/A%d-%d", nestLevel, moi);
					sDB += ssw;
				}
			}
			// Rajout pour les VNAU Alleo : on ajoute /A1-0
			// HRI, 12/08/2015
			//if (nestLevel == 0)
			//{
			//	sDB += "/A1-0";
			//}
	
			std::vector<QuotasValues>& listDbB = m_quotas_view_sdb_b._list_ctrl.Datas();
			for(it = listDbB.begin() ; it != listDbB.end(); ++it)
			{
				QuotasValues& value = (*it);
				nestLevel = value._nest_level._value;
				int moi = value._moi._value;
				if (nestLevel == 0){
					// NBN Ano 82727
					ssw.Format("/B%d-%d/B1-%d", nestLevel, moi,moi);
				}else{
					ssw.Format("/B%d-%d", nestLevel, moi);
				}
				sDB += ssw;
			}
			// Rajout pour les VNAU Alleo : on ajoute /A1-0
			// HRI, 12/08/2015
			//if (nestLevel == 0)
			//{
			//	sDB += "/B1-0";
			//}
			sVnau.Format("VNAU%d/%s%s%s/%s",m_trancheno3,MakeStrDate(datevnau,FALSE),
				stok.Left(5),stok.Right(5),sDB);

			if (((CCTAApp*)APP)->GetRhealysFlag())
				PutRhealysVnau (sVnau, stok, 3);
			else
			{
				pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 3);
				m_VnauList.Add ((CObject*)pQuotaVnau);			
			}

			if (bt3)
			{
				// si Rhealys, le SetTrancheSentFlag fait un "| 2" sur le edit_status (triangle noir)
				((CCTAApp*)APP)->SetTrancheSentFlag (false, m_trancheno3, m_lDateDep);
				if (((CCTAApp*)APP)->GetRhealysFlag())
				{
					pTrancheView->SetEditStatus (m_trancheno3, m_lDateDep, EDIT_STATUS_AUTHMODNOTSENT);
				}	
				else
				{
					pTrancheView->SetSentFlag (m_trancheno3, m_lDateDep, 1);
				}
					
				bt3 = FALSE;
			}				

			if (bFirst)
			{
				if (((CCTAApp*)APP)->GetRhealysFlag())
				{
					// En attente (triangle noir)
					((CCTAApp*)APP)->SetLegEditStatus(m_trancheno3, stok.Left(5), m_lDateDep, EDIT_STATUS_AUTHMODNOTSENT);
					pLegView->SetEditStatus (m_trancheno3, stok.Left(5), m_lDateDep, EDIT_STATUS_AUTHMODNOTSENT);
					
				}
				else
				{
					// Envoi direct
					pLegView->SetSentFlag (m_trancheno3, stok.Left(5), m_lDateDep, 1);
					((CCTAApp*)APP)->SetLegSentFlag (false, m_trancheno3, sLegsOrig, m_lDateDep);			
				}
				
				sCmpt = "A";
				/// DM-7978 - CLE
				MajAuthDB(m_trancheno3, sLegsOrig, sCmpt);

				sCmpt = "B";
				MajAuthDB(m_trancheno3, sLegsOrig, sCmpt);
			}
		}
		

		

		//*********************************************
		// Sous contingent SI compartiment A
		//*********************************************
		if(bSISCAFlagModif){
			// Constitution de la chaîne VNAS 
			sSI= "";

			std::vector<QuotasValues>& listSIScxA = m_quotas_view_si_sca._list_ctrl.Datas();
			std::vector<QuotasValues>::iterator it;

			CString sTemp;
			m_SIA_SCG_MOI.GetWindowText(sTemp); //NCH
			int value = atoi(sTemp);
			sSI.Format("AG-%d", value);

			for( it = listSIScxA.begin(); it != listSIScxA.end(); ++it)
			{
				QuotasValues& values = (*it);
				int nestLevel = values._nest_level._value;
				value = values._moi._value;

				if (nestLevel > 0)
				{
					ssw.Format("/A%d-%d", nestLevel, value);
					sSI += ssw;
				}
			}
			
			sVnau.Format("VNAS%d/%s%s%s/%s",m_trancheno1,MakeStrDate(datevnau,FALSE),
				stok.Left(5),stok.Right(5),sSI);

			pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 1);
			m_VnauList.Add ((CObject*)pQuotaVnau);
			


			sCmpt = "A";
			if (bFirst)
				MajAuthSISC(m_trancheno1, sLegsOrig, sCmpt);
		}
		
		//*********************************************
		// Sous contingent SI compartiment B
		//*********************************************
		

		if(bSISCBFlagModif){
			// Constitution de la chaîne VNAS 
			sSI= "";

			std::vector<QuotasValues>& listSIScxB = m_quotas_view_si_scb._list_ctrl.Datas();
			std::vector<QuotasValues>::iterator it;

			CString sTemp;
			m_SIB_SCG_MOI.GetWindowText(sTemp); //NCH
			int value = atoi(sTemp);
			// ANO 93000 - NBN
			sSI.Format("BG-%d", value);

			for( it = listSIScxB.begin(); it != listSIScxB.end(); ++it)
			{
				QuotasValues& values = (*it);
				int nestLevel = values._nest_level._value;
				value = values._moi._value;
		

				if (nestLevel > 0)
				{
					ssw.Format("/B%d-%d", nestLevel, value);
					sSI += ssw;
				}
			}
			
			sVnau.Format("VNAS%d/%s%s%s/%s",m_trancheno1,MakeStrDate(datevnau,FALSE),
				stok.Left(5),stok.Right(5),sSI);


			pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 1);
			m_VnauList.Add ((CObject*)pQuotaVnau);
			


			sCmpt = "B";
			if (bFirst)
				MajAuthSISC(m_trancheno1, sLegsOrig, sCmpt);
		}

		//*********************************************
		// Sous contingent SD compartiment A
		//*********************************************
		if(bSDSCAFlagModif){
			// Constitution de la chaîne VNAS 
			sSD = "";

			std::vector<QuotasValues>& listSDScxA = m_quotas_view_sd_sca._list_ctrl.Datas();
			std::vector<QuotasValues>::iterator it;

			CString sTemp;
			m_SDA_SCG_MOI.GetWindowText(sTemp); //NCH
			int value = atoi(sTemp);
			sSD.Format("AG-%d", value);

			for( it = listSDScxA.begin(); it != listSDScxA.end(); ++it)
			{
				QuotasValues& values = (*it);
				int nestLevel = values._nest_level._value;
				value = values._moi._value;
		

				if (nestLevel > 0)
				{
					ssw.Format("/A%d-%d", nestLevel, value);
					sSD += ssw;
				}
			}
			
			sVnau.Format("VNAS%d/%s%s%s/%s",m_trancheno2,MakeStrDate(datevnau,FALSE),
				stok.Left(5),stok.Right(5),sSD);


			pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 2);
			m_VnauList.Add ((CObject*)pQuotaVnau);
			


			sCmpt = "A";
			if (bFirst)
				MajAuthSDSC(m_trancheno2, sLegsOrig, sCmpt);
		}

		//*********************************************
		// Sous contingent SD compartiment B
		//*********************************************
		if(bSDSCBFlagModif){
			// Constitution de la chaîne VNAS 
			sSD = "";

			std::vector<QuotasValues>& listSDScxB = m_quotas_view_sd_scb._list_ctrl.Datas();
			std::vector<QuotasValues>::iterator it;

			CString sTemp;
			// ANO 93000 - NBN
			m_SDB_SCG_MOI.GetWindowText(sTemp); //NCH
			int value = atoi(sTemp);
			// ANO 93000 - NBN
			sSD.Format("BG-%d", value);

			for( it = listSDScxB.begin(); it != listSDScxB.end(); ++it)
			{
				QuotasValues& values = (*it);
				int nestLevel = values._nest_level._value;
				value = values._moi._value;
	
				if (nestLevel > 0)
				{
					ssw.Format("/B%d-%d", nestLevel, value);
					sSD += ssw;
				}
			}
			
			sVnau.Format("VNAS%d/%s%s%s/%s",m_trancheno2,MakeStrDate(datevnau,FALSE),
				stok.Left(5),stok.Right(5),sSD);

			pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 2);
			m_VnauList.Add ((CObject*)pQuotaVnau);
			


			sCmpt = "B";
			if (bFirst)
				MajAuthSDSC(m_trancheno2, sLegsOrig, sCmpt);
		}

		//*********************************************
		// Sous contingent DB compartiment A
		//*********************************************
		BOOL dbScaModification = isACopie ? (dbScaModif && !TronconDE() && !TronconDE(sListTroncon)) : (dbScaModif);
		if(dbScaModification){
			// Constitution de la chaîne VNAS 
			sDB = "";

			std::vector<QuotasValues>& listDBScxA = m_quotas_view_sdb_sca._list_ctrl.Datas();
			std::vector<QuotasValues>::iterator it;

			for( it = listDBScxA.begin(); it != listDBScxA.end(); ++it)
			{
				QuotasValues& values = (*it);
				int nestLevel = values._nest_level._value;
				int value = values._moi._value;
		
				if (nestLevel = 1)
				{
					sDB.Format("AG-%d/A1-%d", value,value);
				}
				/*
				if (nestLevel > 1)
				{
					ssw.Format("/A%d-%d", nestLevel, value);
					sDB += ssw;
				}
				*/
			}
			
			sVnau.Format("VNAS%d/%s%s%s/%s",m_trancheno3,MakeStrDate(datevnau,FALSE),
				stok.Left(5),stok.Right(5),sDB);

			if (((CCTAApp*)APP)->GetRhealysFlag()) // si Rhealys insertion directe en base sans COMM
				PutRhealysVnau (sVnau, stok, 3);
			else
			{
				pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 3);
				m_VnauList.Add ((CObject*)pQuotaVnau);
			}

			//CME Ano 92550
			if (bt3)
			{	
				if (((CCTAApp*)APP)->GetRhealysFlag())
				{
					// Mise en attente (triangle noir)
					pTrancheView->SetEditStatus (m_trancheno3, m_lDateDep, EDIT_STATUS_AUTHMODNOTSENT);
				}	
				else
				{
					pTrancheView->SetScxSentFlag (m_trancheno3, m_lDateDep, 1);
				}

				bt3 = FALSE;
			}

			sCmpt = "A";
			if (bFirst)
			{
				if (((CCTAApp*)APP)->GetRhealysFlag())
				{
					// Mise en attente (triangle noir)
					((CCTAApp*)APP)->SetLegEditStatus(m_trancheno3, stok.Left(5), m_lDateDep, EDIT_STATUS_AUTHMODNOTSENT);
					pLegView->SetEditStatus (m_trancheno3, stok.Left(5), m_lDateDep, EDIT_STATUS_AUTHMODNOTSENT);
				}	
				else
				{
					// Envoi direct
					pLegView->SetScxSentFlag (m_trancheno3, stok.Left(5), m_lDateDep, 1);
					((CCTAApp*)APP)->SetLegSentFlag (true, m_trancheno3, sLegsOrig, m_lDateDep);
					
				}	
				
				MajAuthDBSC(m_trancheno3, sLegsOrig, sCmpt);
			}
			//--CME Ano 92550	
		}

		//*********************************************
		// Sous contingent DB compartiment B
		//*********************************************
		BOOL dbScbModification = isACopie ? (dbScbModif && !TronconDE() && !TronconDE(sListTroncon)) : (dbScbModif);
		if(dbScbModification){
			// Constitution de la chaîne VNAS 
			sDB = "";

			std::vector<QuotasValues>& listDBScxB = m_quotas_view_sdb_scb._list_ctrl.Datas();
			std::vector<QuotasValues>::iterator it;

			for( it = listDBScxB.begin(); it != listDBScxB.end(); ++it)
			{
				QuotasValues& values = (*it);
				int nestLevel = values._nest_level._value;
				int value = values._moi._value;
		
				if (nestLevel = 1)
				{
					sDB.Format("BG-%d/B1-%d", value,value);
				}
				/*
				if (nestLevel > 0)
				{
					ssw.Format("/B%d-%d", nestLevel, value);
					sDB += ssw;
				}
				*/
			}
			
			sVnau.Format("VNAS%d/%s%s%s/%s",m_trancheno3,MakeStrDate(datevnau,FALSE),
				stok.Left(5),stok.Right(5),sDB);

			if (((CCTAApp*)APP)->GetRhealysFlag()) // si Rhealys insertion directe en base sans COMM
				PutRhealysVnau (sVnau, stok, 3);
			else
			{
				pQuotaVnau = new CQuotaVnau (sVnau, stok.Find ("-") < 0, 3);
				m_VnauList.Add ((CObject*)pQuotaVnau);
			}

			//CME Ano 92550
			if (bt3)
			{	
				if (((CCTAApp*)APP)->GetRhealysFlag())
				{
					// Mise en attente (triangle noir)
					pTrancheView->SetEditStatus (m_trancheno3, m_lDateDep, EDIT_STATUS_AUTHMODNOTSENT);
				}	
				else
				{
					pTrancheView->SetScxSentFlag (m_trancheno3, m_lDateDep, 1);
				}
					
				bt3 = FALSE;
			}
		
			sCmpt = "B";
			if (bFirst)
			{
				if (((CCTAApp*)APP)->GetRhealysFlag())
				{
					// Mise en attente (triangle noir)
					((CCTAApp*)APP)->SetLegEditStatus(m_trancheno3, stok.Left(5), m_lDateDep, EDIT_STATUS_AUTHMODNOTSENT);
					pLegView->SetEditStatus (m_trancheno3, stok.Left(5), m_lDateDep, EDIT_STATUS_AUTHMODNOTSENT);
					
				}	
				else
				{
					// Envoi direct
					pLegView->SetScxSentFlag (m_trancheno3, stok.Left(5), m_lDateDep, 1);
					((CCTAApp*)APP)->SetLegSentFlag (true, m_trancheno3, sLegsOrig, m_lDateDep);
					
				}

				MajAuthDBSC(m_trancheno3, sLegsOrig, sCmpt);
			}
			//--CME Ano 92550
		}

		tok = strtok (NULL, delim);
		bFirst = FALSE;
	}
	free (pbuf);
	// Ano 82483 - NBN
	bSICopieFlagModif = bSIFlagModif;
	bSDCopieFlagModif = bSDFlagModif;
	bDBCopieFlagModif = bDBFlagModif;
	bDBScaCopieFlagModif = bDBSCAFlagModif;
	bDBScbCopieFlagModif = bDBSCBFlagModif;

	bAllFlagModif = FALSE;
	bFlagModifManuelle = FALSE;
	bSIFlagModif = FALSE;
	bSDFlagModif = FALSE;;
	bDBFlagModif = FALSE;;
	bDBSCAFlagModif = FALSE;;
	bDBSCBFlagModif = FALSE;;	


}

void CQuotasRhealysDlg::PurgeVnauList()
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

void CQuotasRhealysDlg::InitSendVnau()
{
	((CButton *)GetDlgItem(IDC_QR_CHERCHER))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_QR_QUITTER))->EnableWindow(FALSE);
	CWnd* pWnd = GetDlgItem (IDC_NBVNAU);
	CString sMess;
	if (m_lRrdIndex >= 0)
		sMess = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_MAJ_AUTH);
	else
		sMess.Format ("VNAU 0 / %d", m_VnauList.GetSize());
	pWnd->SetWindowText (sMess);

}

void CQuotasRhealysDlg::FinSendVnau()
{
	((CButton *)GetDlgItem(IDC_QR_CHERCHER))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_QR_QUITTER))->EnableWindow(TRUE);
	CWnd* pWnd = GetDlgItem (IDC_NBVNAU);
	pWnd->SetWindowText ("");
	if ((SumBkt0A != SumCapA) || (SumBkt0B != SumCapB))
		AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_QUOTA_KO), AfxGetAppName(), 
		MB_ICONINFORMATION | MB_APPLMODAL);
}

void CQuotasRhealysDlg::SendNextVnau()
{
	int i;
	CQuotaVnau* pQuotaVnau;
	for (i = 0; i < m_VnauList.GetSize(); i++)
	{
		pQuotaVnau = (CQuotaVnau*)m_VnauList.GetAt(i);
		if (pQuotaVnau && !pQuotaVnau->m_bSent)
		{
			VNAU_MSG_TYPE tyVnau;
			if (pQuotaVnau->m_nature == 1)
				tyVnau = VNAU_NAT1_TYPE;
			else if (pQuotaVnau->m_nature == 2)
				tyVnau = VNAU_NAT2_TYPE;
			else
				tyVnau = VNAU_NAT3_TYPE;
			((CCTAApp*)APP)->GetResComm()->SendVnau (this, pQuotaVnau->m_mess, FALSE, pQuotaVnau->m_bCrossMidnight, tyVnau);
			pQuotaVnau->m_bSent = TRUE;
			CWnd* pWnd = GetDlgItem (IDC_NBVNAU);
			CString sMess;
			sMess.Format ("VNAU : %d / %d", i+1, m_VnauList.GetSize());
			pWnd->SetWindowText (sMess);
			// CLE - DM-7978 ne comprend pas pourquoi il faisait un return
			// NPI - Ano 70134
			// On décommente la ligne ci-dessous (commentaire mis à tort)

			return;

		}
	}
	// Si on est ici, c'est que tous les VNAU ont été envoyé, on fait donc le ménage.
	PurgeVnauList();
	bVnlDeControle = TRUE;
	StartSendVnl (sLastTroncon); 
}

BOOL CQuotasRhealysDlg::MessageNbVnau(CString sListTroncon) 
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
	int iVnau = 0;
	int iModif = 0;
	int iScModif = 0;

	while (tok)
	{
		iVnau++;
		stok = tok;
		tok = strtok (NULL, delim);
	}
	if ( bSIFlagModif)
		iModif++;
	if ( bSDFlagModif)
		iModif++;
	if ( bDBFlagModif)
		iModif++;

	if ( bSISCAFlagModif)
		iScModif++;
	if ( bSDSCAFlagModif)
		iScModif++;
	if ( bDBSCAFlagModif)
		iScModif++;

	if ( bSISCBFlagModif)
		iScModif++;
	if ( bSDSCBFlagModif)
		iScModif++;
	if ( bDBSCBFlagModif)
		iScModif++;


	iVnau = iVnau * (iModif);
	CString sVnauSend = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VNAU_SEND);
	sMess.Format("%d/%d %s",iVnau,iScModif, sVnauSend);
	free (pbuf);
	if (AfxGetMainWnd()->MessageBox(sMess, AfxGetAppName(),MB_OKCANCEL | MB_ICONQUESTION | MB_APPLMODAL) == IDCANCEL)
		return FALSE;
	return TRUE;
}

void CQuotasRhealysDlg::OnUnTroncon() 
{
	CString sVnau,sSI,sSD;
	CString sListTroncon;
	m_ComboTroncon.GetWindowText(sListTroncon);
	if (!MessageNbVnau(sListTroncon))
		return;
	InitSendVnau();
	BuildVnau(sListTroncon,bSIFlagModif,bSDFlagModif,bDBFlagModif,bDBSCAFlagModif,bDBSCBFlagModif,FALSE);
	SendNextVnau();
}

void CQuotasRhealysDlg::OnTousTroncon() 
{
	CString sListTroncon;
	BOOL bRep;
	// Lancement de la requete de mise a jour sur tous les troncons
	bRep = GetSelTroncon("TOUS", sListTroncon);
	if (bRep)
	{
		bAllFlagModif = TRUE;
		InitSendVnau();
		BuildVnau(sListTroncon,bSIFlagModif,bSDFlagModif,bDBFlagModif,bDBSCAFlagModif,bDBSCBFlagModif,FALSE);
		
		bAllFlagModif = FALSE;
		bSIFlagModif = FALSE;
		bSDFlagModif = FALSE;
		bDBFlagModif = FALSE;
		bDBSCAFlagModif = FALSE;
		bDBSCBFlagModif = FALSE;
		bFlagModifManuelle = FALSE;
		
		SendNextVnau();
	}
}

void CQuotasRhealysDlg::OnSelTroncon() 
{
	CString sListTroncon;
	BOOL bRep;
	// Lancement de la requete de mise a jour sur les troncons selectionnés
	bRep = GetSelTroncon("SEL", sListTroncon);
	CString sCurTroncon;
	m_ComboTroncon.GetWindowText(sCurTroncon);
	CString sTestTroncon = sListTroncon + "," + sCurTroncon;


    // NBN ANO 82483 - la copie d'un troncon inter vers frontalier n'est pas autorisée 
    // Verifier que la source est Internationale et faire le test de la destination
    if(sCurTroncon.Left(5).Left(2) == "DE" && sCurTroncon.Right(5).Left(2) == "DE"){

        if(!ControlCopieInterOnly(sTestTroncon)){
            // Verification que tous les troncons a copier sont des DE DE
            // la copie d'un troncon inter vers frontalier n'est pas autorisée
            AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NOT_MIX_COPY), 
                MB_OK | MB_APPLMODAL | MB_ICONSTOP );
			return;
        }    
	}

	if (!ControlCrossFrontier (sTestTroncon))
	{
		AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NOT_MIX_COPY), 
			MB_OK | MB_APPLMODAL | MB_ICONSTOP );
		return;
	}
	if (bRep)
	{
		bAllFlagModif = TRUE;
		InitSendVnau();

		BuildVnau(sListTroncon,bSICopieFlagModif,bSDCopieFlagModif,bDBCopieFlagModif,bDBScaCopieFlagModif,bDBScbCopieFlagModif,TRUE);

		// Les VNAU sont envoyé, il n'y a plus de modifs en cours
		bAllFlagModif = FALSE;
		bSIFlagModif = FALSE;
		bSDFlagModif = FALSE;
		bDBFlagModif = FALSE;
		bDBSCAFlagModif = FALSE;
		bDBSCBFlagModif = FALSE;

		bSICopieFlagModif = FALSE;
		bSDCopieFlagModif = FALSE;
		bDBCopieFlagModif = FALSE;
		bDBScaCopieFlagModif = FALSE;
		bDBScbCopieFlagModif = FALSE;
		
		SendNextVnau();
	}
}

bool CQuotasRhealysDlg::ControlCopieInterOnly(CString sListTroncon)
{
    // La copie d'un troncon inter vers frontalier n'est pas autorisée
    // Uniquement copie troncon international vers troncon international autorisée
    char* pbuf;
    char* tok;
    CString stok, orig, dest;
    char delim[4];
    strcpy (delim, ",");
    bool bDEOnly = true;
    pbuf =(char*)malloc (sListTroncon.GetLength()+1);
    strcpy (pbuf, sListTroncon);
    tok = strtok (pbuf,delim);

    while (tok)
    {
        stok = tok;
        orig = stok.Left(5);
        dest = stok.Right(5);
        if ((orig.Left(2) != "DE") || (dest.Left(2) != "DE"))
            bDEOnly = false;

        tok = strtok (NULL, delim);
    }
    free (pbuf);
    return (bDEOnly);

}


bool CQuotasRhealysDlg::ControlCrossFrontier (CString sListTroncon)
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

void CQuotasRhealysDlg::OnQuitter() 
{
	if (bFlagModifManuelle && (bSISCBFlagModif || bSISCAFlagModif || bSDSCAFlagModif || bSDSCBFlagModif || bDBSCAFlagModif || bDBSCBFlagModif || bSIFlagModif || bSDFlagModif || bDBFlagModif))
	{
		if ( AfxGetMainWnd()->MessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_MODIF_DONE),
			AfxGetAppName(), MB_OKCANCEL | MB_APPLMODAL | MB_ICONEXCLAMATION) == IDCANCEL )
			return;
	}
	//    OnCancel();
	CDialog::OnCancel();
}

void CQuotasRhealysDlg::OnKeepEcart() 
{
	m_bOvbAdjustFlag = (((CButton *)GetDlgItem(IDC_KEEP_ECART))->GetCheck());
}

void CQuotasRhealysDlg::OnCancel() 
{	
	OnQuitter();
}