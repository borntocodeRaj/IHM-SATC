// GlobalSysParms.cpp : implementation file
//

#include "StdAfx.h"


#include "CtaChart.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCTAGlobalSysParms

CCTAGlobalSysParms::CCTAGlobalSysParms()
{
  m_dUmLevel = 520.0;
  m_dPortNumber = 0.0;
  m_szHostName.Empty();
  m_bUpdateSynchrone = FALSE;
  m_bAvecPrixGaranti = FALSE;
  m_bVNTPactif = FALSE;
  m_defSurResaGlobBitr = 200;
  m_SeuilVnauVnax = 1000;
  m_nbjoursVisuModifTop = 5;
  m_bVNSPactif = FALSE; // NPI - DM7838 : souplesse tarifaire
  m_bVNSPdynamique = FALSE; // NPI - DM7838 : souplesse tarifaire
  m_bVnlCommBouchon = FALSE; // DM7978 - CLE - bouchon vln
	m_bKdisnCommBouchon = FALSE; // DM7978 - LME - bouchon kdisn
	m_nbccP = 0; // DM 7978 - YI-5841/YI-5842
	m_nbscP = 0; // DM 7978 - YI-5841/YI-5842

  // DM 7978 : DT 13903 - avt 17
  m_lDateGang = (long) 20991231;
  m_controlerCC0EgalCC1 = false; // remettre à false ensuite, HRI, 16/05/2015;
  // Fin DM 7978 : DT 13903 - avt 17
  DefineClientParms();
}

void CCTAGlobalSysParms::InitGlobalSysParms()
{
  // Initialize the global system parameters from the SysParms RecordSet

  YmIcSysParmsDom* pGlobalSysParms = NULL;  // Set the values for the global system parameters

  YM_Iterator<YmIcSysParmsDom>* pIterator = 
		YM_Set<YmIcSysParmsDom>::FromKey(SYSPARM_KEY)->CreateIterator();

  if ( pIterator->GetCount() == 0 )  // Do we have data in the table ?
  {
    CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA_IN_TABLE);
    szText += " SC_SYS_PARMS";
    AfxMessageBox( szText, MB_ICONHAND | MB_APPLMODAL );
    delete pIterator;
    return;
  }

  for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
  {
    pGlobalSysParms = pIterator->Current();

    if ( !strcmp(pGlobalSysParms->SysParmName(), "UM_LEVEL") )
     m_dUmLevel  = pGlobalSysParms->FloatValue();
    else
    if ( !strcmp(pGlobalSysParms->SysParmName(), "HOST_NAME") )
     m_szHostName  = pGlobalSysParms->CharValue();
    else
    if ( !strcmp(pGlobalSysParms->SysParmName(), "PORT_NUMBER") )
     m_dPortNumber  = pGlobalSysParms->FloatValue();
    else
	if ( !strcmp(pGlobalSysParms->SysParmName(), "UPDATE_SYNCHRONE") )
     m_bUpdateSynchrone  = (pGlobalSysParms->FloatValue() > 0);
	else
	if ( !strcmp(pGlobalSysParms->SysParmName(), "RHEALYS_IDENT") )
     m_szRhealysIdent  = pGlobalSysParms->CharValue();
    else
	if( !strcmp(pGlobalSysParms->SysParmName(), "CMPT_SHOW_ORDER") )
	{
		//DM7978  FIX # ano hpqc 1066  KGH: recuperer en plus SysParmPlus (indispensable pour CORAIL car cmpt>2)
		int ordre_map = atoi(pGlobalSysParms->SysParmPlus());
		int ordre_affichage = static_cast<int>(pGlobalSysParms->FloatValue());
		//int order = static_cast<int>(pGlobalSysParms->FloatValue());
		CString cmpt = pGlobalSysParms->CharValue();
		CmptOrdre cmpt_ordre;
		cmpt_ordre.ordre_affichage = ordre_affichage;
		cmpt_ordre.cmpt = cmpt;
		//m_CmptShowOrder.SetAtGrow(order, cmpt);
		m_CmptShowOrder[ordre_map] = cmpt_ordre;
		//FIN DM7978  FIX # ano hpqc 1066  KGH
	}
	else if( !strcmp(pGlobalSysParms->SysParmName(), "CMPT_SCX_SHOW_ORDER") )
	{
		//DM7978  FIX # ano hpqc 1066  KGH: recuperer en plus SysParmPlus (indispensable pour CORAIL car cmpt>2)
		int ordre_map = atoi(pGlobalSysParms->SysParmPlus());
		int ordre_affichage = static_cast<int>(pGlobalSysParms->FloatValue());
		//int order = static_cast<int>(pGlobalSysParms->FloatValue());
		CString cmpt = pGlobalSysParms->CharValue();
		CmptOrdre cmpt_ordre;
		cmpt_ordre.ordre_affichage = ordre_affichage;
		cmpt_ordre.cmpt = cmpt;
		m_CmptScxShowOrder[ordre_map] = cmpt_ordre;
		//m_CmptScxShowOrder.SetAtGrow(order, cmpt);
		//Fin FIX # ano hpqc 1066  KGH
	}
	else if ( !strcmp(pGlobalSysParms->SysParmName(), "MAX_CMPT_BKT") )
	{
		char c = *pGlobalSysParms->CharValue();
	  int nbk = (int)pGlobalSysParms->FloatValue();
      m_MaxCmptBkt.SetAt (pGlobalSysParms->CharValue(), (CObject*)nbk);
			GagConfigMaxClasses::Instance()._maxCC[c] = nbk;
			GagConfigMaxClasses::Instance()._maxCCUser[c] = nbk;
	}
	else
	if (! strcmp(pGlobalSysParms->SysParmName(), "MAX_CMPT_SCX"))
	{
		char c = *pGlobalSysParms->CharValue();
	  int nSci = (int)pGlobalSysParms->FloatValue();
      m_MaxCmptSci.SetAt (pGlobalSysParms->CharValue(), (CObject*)nSci);
			GagConfigMaxClasses::Instance()._maxSCX[c] = nSci;
			GagConfigMaxClasses::Instance()._maxSCXUser[c] = nSci;
	}
	else
	if ( !strcmp(pGlobalSysParms->SysParmName(), "ADD_PRIX_GARANTI") )
      m_bAvecPrixGaranti  = (pGlobalSysParms->FloatValue() > 0);
	else
	if ( !strcmp(pGlobalSysParms->SysParmName(), "VNTP_ACTIF") )
      m_bVNTPactif  = (pGlobalSysParms->FloatValue() > 0);
	else
	if ( !strcmp(pGlobalSysParms->SysParmName(), "HISTOSELECT") )
	  m_szHistoSelect  = pGlobalSysParms->CharValue();		
	else
	if ( !strcmp(pGlobalSysParms->SysParmName(), "TGVSELECT") )
	  m_szTgvSelect  = pGlobalSysParms->CharValue();
	else
	if ( !strcmp(pGlobalSysParms->SysParmName(), "TGVHISTSELECT") )
	  m_szTgvHistSelect  = pGlobalSysParms->CharValue();
	else
	if ( !strcmp(pGlobalSysParms->SysParmName(), "CLIENT") )
	{
	  CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
	  WinApp->m_pDatabase->SetClientParameter(pGlobalSysParms->CharValue());
	}
	else 
	if ( !strcmp(pGlobalSysParms->SysParmName(), "BITR_SUR_RESA_GLOB") )
      m_defSurResaGlobBitr  = static_cast<long>(pGlobalSysParms->FloatValue());
	else 
	if ( !strcmp(pGlobalSysParms->SysParmName(), "SEUIL_VNAU_VNAX") )
      m_SeuilVnauVnax  = static_cast<int>(pGlobalSysParms->FloatValue());
	else 
	if ( !strcmp(pGlobalSysParms->SysParmName(), "NBJOURS_VISU_MODIF_TOP") )
      m_nbjoursVisuModifTop  = static_cast<int>(pGlobalSysParms->FloatValue());
	// NPI - DM7838 : souplesse tarifaire
	else
	if ( !strcmp(pGlobalSysParms->SysParmName(), "VNSP_ACTIF") )
      m_bVNSPactif  = (pGlobalSysParms->FloatValue() > 0);
	// NPI - DM7838 : souplesse tarifaire
	else
	if ( !strcmp(pGlobalSysParms->SysParmName(), "VNSP_DYNAMIQUE") )
      m_bVNSPdynamique  = (pGlobalSysParms->FloatValue() > 0);
	/// DM 7978 - CLE 
	else
		if (! strcmp(pGlobalSysParms->SysParmName(), "VNL_COMM_BOUCHON"))
			m_bVnlCommBouchon = pGlobalSysParms->FloatValue() == 1;
	//
	/// DM 7978 - LME
	else
		if (! strcmp(pGlobalSysParms->SysParmName(), "KDISN_COMM_BOUCHON"))
			m_bKdisnCommBouchon = pGlobalSysParms->FloatValue() == 1;
	//
	// DM 7978 - YI-5841/YI-5842
	else
		if (! strcmp(pGlobalSysParms->SysParmName(), "NB_CC_VISU_SATC"))
			m_nbccP = static_cast<int>(pGlobalSysParms->FloatValue());
	else
		if (! strcmp(pGlobalSysParms->SysParmName(), "NB_SC_VISU_SATC"))
			m_nbscP = static_cast<int>(pGlobalSysParms->FloatValue());
	// END DM 7978 - YI-5841/YI-5842
	//DM7978 - KGH - Sphere de prix: Base Py
	else
	if ( !strcmp(pGlobalSysParms->SysParmName(), "PYSELECT") )
	  m_szPySelect  = pGlobalSysParms->CharValue();
	else
	if ( !strcmp(pGlobalSysParms->SysParmName(), "CORAILMDP") )
	  m_szCorailMdp  = pGlobalSysParms->CharValue();
	else
	if ( !strcmp(pGlobalSysParms->SysParmName(), "CORAILHISTMDP") )
	  m_szCorailHistMdp  = pGlobalSysParms->CharValue();
	//DM7978 - KGH - FIN
	else 	// DM 7978 : DT 13903 - avt 17
	if (! strcmp(pGlobalSysParms->SysParmName(), "DATE_BANG"))
	{
		m_lDateGang = (long) pGlobalSysParms->FloatValue();
		// DM 7978 : DT 13903 - avt 17
		RWDate rwToday;
		// Comparer date du jou et dateGang pour valoriser ControlerCC0EgalCC1
		CString szDate = rwToday.asString("%Y%m%d");
		CString szDateGang;
		szDateGang.Format("%d", this->GetDateGang());
		if (szDateGang <= szDate)
		{
			this->SetControlerCC0EgalCC1(true);
		}
	}
  }

  delete pIterator;
  reodonerCmptShowOrder(m_CmptShowOrder);//DM7978 FIX # ano HPQC 1088 KGH
  reodonerCmptShowOrder(m_CmptScxShowOrder);//DM7978 FIX # ano HPQC 1088 KGH
  DefineClientParms();
}

//DM7978 FIX # ano HPQC 1088 KGH
//cette methode permet de reorganiser l'ordre (SYS_PARM_PLUS) de la map m_CmptShowOrder(oum_CmptScxShowOrder)  par rapport a "ordre_affichage"(FLOAT_VALUE) 
//pour les Cmpt (A et B) et (C et D) (a et b) et (c et d)
void CCTAGlobalSysParms::reodonerCmptShowOrder(std::map <int, CmptOrdre> &cmptShowOrder)
{
	int sizeCmptShowOrder = cmptShowOrder.size();
	for(int i = 0; i < sizeCmptShowOrder; i++)
	{
		CString sCmpt1 = cmptShowOrder[i].cmpt;
		int tmpOrdre_affichage1 = cmptShowOrder[i].ordre_affichage;
		CString sCmpt2 = "";
		int tmpOrdre_affichage2 = -1;
		if (i<sizeCmptShowOrder -1)// pour ne pas depasser la taille de la map si on fait i+1
		{
			sCmpt2 = cmptShowOrder[i+1].cmpt;
			tmpOrdre_affichage2 = cmptShowOrder[i+1].ordre_affichage;
		}
		//A et B / a et b
		if((!sCmpt1.Compare("A") || !sCmpt1.Compare("a")) && i < sizeCmptShowOrder -1)
			if(!sCmpt2.Compare("B") || !sCmpt2.Compare("b"))
				if(tmpOrdre_affichage1 > tmpOrdre_affichage2)
				{
					//mettre les info de 'B/b' dans 'A/a'
					cmptShowOrder[i].cmpt = sCmpt2;
					cmptShowOrder[i].ordre_affichage = tmpOrdre_affichage2;
					//mettre les info de 'A/a' dans 'B/b'
					cmptShowOrder[i+1].cmpt = sCmpt1;
					cmptShowOrder[i+1].ordre_affichage = tmpOrdre_affichage1;
				}

				//B et A / b et a
		if((!sCmpt1.Compare("B") || !sCmpt1.Compare("b")) && i < sizeCmptShowOrder -1)
			if(!sCmpt2.Compare("A") || !sCmpt2.Compare("a"))
				if(tmpOrdre_affichage1 > tmpOrdre_affichage2)
				{
					//mettre les info de 'A/a' dans 'B/b'
					cmptShowOrder[i].cmpt = sCmpt2;
					cmptShowOrder[i].ordre_affichage = tmpOrdre_affichage2;
					//mettre les info de 'B/b' dans 'A/a'
					cmptShowOrder[i+1].cmpt = sCmpt1;
					cmptShowOrder[i+1].ordre_affichage = tmpOrdre_affichage1;
				}	
				//C et D / c et d
				if((!sCmpt1.Compare("C") || !sCmpt1.Compare("c"))  && i < sizeCmptShowOrder -1)
					if(!sCmpt2.Compare("D") || !sCmpt2.Compare("d"))
						if(tmpOrdre_affichage1 > tmpOrdre_affichage2)
						{
							//mettre les info de 'D/d' dans 'C/c'
							cmptShowOrder[i].cmpt = sCmpt2;
							cmptShowOrder[i].ordre_affichage = tmpOrdre_affichage2 ;
							//mettre les info de 'C/c' dans 'D/d'
							cmptShowOrder[i+1].cmpt = sCmpt1;
							cmptShowOrder[i+1].ordre_affichage = tmpOrdre_affichage1;
						}
		//D et C / d et c
				if((!sCmpt1.Compare("D") || !sCmpt1.Compare("d"))  && i < sizeCmptShowOrder -1)
					if(!sCmpt2.Compare("C") || !sCmpt2.Compare("c"))
						if(tmpOrdre_affichage1 > tmpOrdre_affichage2)
						{
							//mettre les info de 'C/c' dans 'D/d'
							cmptShowOrder[i].cmpt = sCmpt2;
							cmptShowOrder[i].ordre_affichage = tmpOrdre_affichage2 ;
							//mettre les info de 'D/d' dans 'C/c'
							cmptShowOrder[i+1].cmpt = sCmpt1;
							cmptShowOrder[i+1].ordre_affichage = tmpOrdre_affichage1;
						}
	}
}
//FIN DM7978 FIX # ano HPQC 1088 KGH

CCTAGlobalSysParms::~CCTAGlobalSysParms()
{
}

int CCTAGlobalSysParms::GetMaxCmptBkt (CString cmpt)
{
  int nbk;
  if (m_MaxCmptBkt.Lookup (cmpt, (CObject*&)nbk))
	return nbk;
  return -1;
}


int CCTAGlobalSysParms::GetMaxSciBkt(CString cmpt)
{
	int nbSci;
	if (m_MaxCmptSci.Lookup(cmpt, (CObject*&)nbSci))
		return nbSci;
	else return -1;
}

void CCTAGlobalSysParms::DefineClientParms()
{
	// Get the Client Sys Parm from the Database, because we've already retrieved it via a stored procedure (see YM_Database)
    m_szClient = App().m_pDatabase->GetClientParameter();
#ifdef MOTEURVNL_APPLICATION
  DefineMoteurVNLParms();
#else
  ASSERT(! m_szClient.IsEmpty());
  if (m_szClient == SNCF)  // Determine which client, and define the parameter settings
    DefineSNCFParms();
  else
  if (m_szClient == SNCF_TRN)
    DefineSNCFTRNParms();
  else
  if (m_szClient == THALYS)
    DefineThalysParms();
#endif
}

void CCTAGlobalSysParms::DefineSNCFParms()
{
  // Define default settings for SNCF TGV
  m_bAutoCopyAuthFlag = FALSE;
  m_bCompositionFlag = TRUE;
  m_bComptagesFlag = FALSE;
  m_bEntityDescFlag = TRUE;
  m_bForcementFlag = TRUE;
  m_bYieldODFlag = TRUE;
  m_bNiveauInVNLengine = TRUE;
  m_bMasquageCourbe = TRUE;
  m_bGraphOutlineFlag = TRUE;
  m_bHeaderImageFlag = TRUE;
  m_bInfluentFlag = TRUE;
  m_bInventoryNestingMethod = TRUE;
  m_bMultipleCriticalRulesFlag = TRUE;
  m_bRevenuFlag = TRUE;
  m_bSupplementTrainFlag = TRUE;
  m_bSupplementLevelFlag = TRUE;
  m_bNiveauSocialFlag = TRUE; // NPI - DM7838 : souplesse tarifaire
  m_bNiveauGammeFlag = TRUE; // NPI - DM7838 : souplesse tarifaire
  m_bTGVJumeauxFlag = TRUE;
  m_bTrainAdaptFlag = FALSE;
  m_bTrancheNoFlag = TRUE;
  m_bTRNFlag = FALSE;
  m_bSCXFlag = TRUE;
  m_bRRDHistoryFlag = TRUE;
  m_bControlRRDCurveFlag = TRUE;
  m_bRecalcHistoBkt = TRUE;
  m_bCompactRRDHistView = TRUE;
  m_bUseBuckEvolAvtRep = TRUE;
  m_bRealTimeInRRDHist = TRUE;
  m_bHistoDepLastYear = FALSE;
  m_bMultiEntitySelFlag = TRUE;
  m_bAriDataFlag = TRUE;
  m_bNomosFlag = FALSE;
  m_bTopFlag = TRUE;
  m_bHoiTraFlag = TRUE;
  m_bTrPrevFlag = TRUE;

  DefineSNCFColors();  // set the client specific colors
}

void CCTAGlobalSysParms::DefineSNCFTRNParms()
{
  // Define default settings for SNCF TRN/TRD
  m_bAutoCopyAuthFlag = FALSE;
  m_bCompositionFlag = FALSE;
  m_bComptagesFlag = FALSE;
  m_bEntityDescFlag = TRUE;
  m_bForcementFlag = TRUE;
  m_bYieldODFlag = TRUE;
  m_bNiveauInVNLengine = TRUE;
  m_bMasquageCourbe = TRUE;
  m_bGraphOutlineFlag = TRUE;
  m_bHeaderImageFlag = TRUE;
  m_bInfluentFlag = TRUE;
  m_bInventoryNestingMethod = TRUE;
  m_bMultipleCriticalRulesFlag = TRUE;
  m_bRevenuFlag = TRUE;
  m_bSupplementTrainFlag = TRUE;
  m_bSupplementLevelFlag = TRUE;
  m_bNiveauSocialFlag = TRUE; // NPI - DM7838 : souplesse tarifaire
  m_bNiveauGammeFlag = TRUE; // NPI - DM7838 : souplesse tarifaire
  m_bTGVJumeauxFlag = TRUE;
  m_bTrainAdaptFlag = TRUE;
  m_bTrancheNoFlag = TRUE;
  m_bTRNFlag = TRUE;
  m_bSCXFlag = TRUE;
  m_bRRDHistoryFlag = TRUE;
  m_bControlRRDCurveFlag = TRUE;
  m_bRecalcHistoBkt = TRUE;
  m_bCompactRRDHistView = TRUE;
  m_bUseBuckEvolAvtRep = TRUE;
  m_bRealTimeInRRDHist = TRUE;
  m_bHistoDepLastYear = FALSE;
  m_bMultiEntitySelFlag = TRUE;
  m_bAriDataFlag = TRUE;
  m_bNomosFlag = FALSE;
  m_bTopFlag = TRUE;
  m_bHoiTraFlag = TRUE;
  m_bTrPrevFlag = TRUE;

  DefineSNCFTRNColors();  // set the client specific colors
}

void CCTAGlobalSysParms::DefineThalysParms()
{
  // Define default settings for Thalys
  m_bAutoCopyAuthFlag = TRUE;
  m_bCompositionFlag = FALSE;
  m_bComptagesFlag = FALSE;
  m_bEntityDescFlag = TRUE;
  m_bForcementFlag = TRUE;
  m_bYieldODFlag = FALSE;
  m_bNiveauInVNLengine = FALSE;
  m_bMasquageCourbe = FALSE;
  m_bGraphOutlineFlag = TRUE;
  m_bHeaderImageFlag = TRUE;
  m_bInfluentFlag = TRUE;
  m_bInventoryNestingMethod = TRUE;
  m_bMultipleCriticalRulesFlag = TRUE;
  m_bRevenuFlag = TRUE;
  m_bSupplementTrainFlag = FALSE;
  m_bSupplementLevelFlag = FALSE;
  m_bNiveauSocialFlag = FALSE; // NPI - DM7838 : souplesse tarifaire
  m_bNiveauGammeFlag = FALSE; // NPI - DM7838 : souplesse tarifaire
  m_bTGVJumeauxFlag = TRUE;
  m_bTrainAdaptFlag = FALSE;
  m_bTrancheNoFlag = TRUE;
  m_bTRNFlag = FALSE;
  m_bSCXFlag = TRUE;
  m_bRRDHistoryFlag = TRUE;
  m_bControlRRDCurveFlag = TRUE;
  m_bRecalcHistoBkt = TRUE;
  m_bCompactRRDHistView = TRUE;
  m_bUseBuckEvolAvtRep = FALSE;
  m_bRealTimeInRRDHist = TRUE;
  m_bHistoDepLastYear = FALSE;
  m_bMultiEntitySelFlag = TRUE;
  m_bAriDataFlag = FALSE;
  m_bNomosFlag = TRUE;
  m_bTopFlag = FALSE;
  m_bHoiTraFlag = FALSE;
  m_bTrPrevFlag = FALSE;

  DefineThalysColors();  // set the client specific colors (Thalys same as SNCF)
}

void CCTAGlobalSysParms::DefineMoteurVNLParms()
{
  // Define default settings for CORAIL MoteurVNL application
  m_bAutoCopyAuthFlag = FALSE;
  m_bCompositionFlag = TRUE;
  m_bComptagesFlag = FALSE;
  m_bEntityDescFlag = TRUE;
  m_bForcementFlag = TRUE;
  m_bYieldODFlag = FALSE;
  m_bNiveauInVNLengine = FALSE;
  m_bMasquageCourbe = FALSE;
  m_bGraphOutlineFlag = TRUE;
  m_bHeaderImageFlag = TRUE;
  m_bInfluentFlag = TRUE;
  m_bInventoryNestingMethod = TRUE;
  m_bMultipleCriticalRulesFlag = FALSE;
  m_bRevenuFlag = FALSE;
  m_bSupplementTrainFlag = TRUE;
  m_bSupplementLevelFlag = TRUE;
  m_bNiveauSocialFlag = FALSE; // NPI - DM7838 : souplesse tarifaire
  m_bNiveauGammeFlag = FALSE; // NPI - DM7838 : souplesse tarifaire
  m_bTGVJumeauxFlag = TRUE;
  m_bTrainAdaptFlag = FALSE;
  m_bTrancheNoFlag = TRUE;
  m_bTRNFlag = (m_szClient == SNCF_TRN);
  m_bSCXFlag = FALSE;
  m_bRRDHistoryFlag = FALSE;
  m_bControlRRDCurveFlag = FALSE;
  m_bRecalcHistoBkt = FALSE;
  m_bCompactRRDHistView = FALSE;
  m_bUseBuckEvolAvtRep = FALSE;
  m_bRealTimeInRRDHist = FALSE;
  m_bHistoDepLastYear = FALSE;
  m_bMultiEntitySelFlag = FALSE;
  m_bAriDataFlag = FALSE;
  m_bNomosFlag = FALSE;
  m_bTopFlag = FALSE;
  m_bHoiTraFlag = FALSE;
  m_bTrPrevFlag = FALSE;

  DefineSNCFColors();  // set the client specific colors
}

void CCTAGlobalSysParms::DefineSNCFColors()
{
  m_aGraphColors.SetAtGrow( CCTAChart::eAuthStyleIdx, RGB( 0, 0, 128 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eCapStyleIdx, RGB(  255, 0, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eDmdDptFcstStyleIdx, RGB(  0, 128, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eBktResHoldAvailSumStyleIdx, RGB(  0, 128, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldTotStyleIdx, RGB(  128, 128, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldAvgStyleIdx, RGB(  255, 0, 255 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldIdvStyleIdx, RGB(128, 0, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eTrafficResStyleIdx, RGB(0, 0, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eRevAvgFcstTotStyleIdx, RGB(0, 255, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eRevAvgFcstIdvStyleIdx, RGB(255, 255, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eRevAvgFcstHighStyleIdx, RGB(255, 0, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eRevAvgFcstLowStyleIdx, ::GetSysColor(COLOR_WINDOW));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldStdvHighStyleIdx, RGB(255, 200, 200));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldStdvLowStyleIdx, ::GetSysColor(COLOR_WINDOW));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket0StyleIdx, RGB(255,255, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket1StyleIdx, RGB(0, 255, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket2StyleIdx, RGB(0, 255, 255));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket3StyleIdx, RGB(255, 0, 255));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket4StyleIdx, RGB(128, 128, 128));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket5StyleIdx, RGB(128, 0, 128));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket6StyleIdx, RGB(128, 0, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket7StyleIdx, RGB(0, 0, 128));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket8StyleIdx, RGB(0, 128, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket9StyleIdx, RGB(128, 128, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eVoidStyleIdx, ::GetSysColor(COLOR_WINDOW));
  m_aGraphColors.SetAtGrow( CCTAChart::eUnreservableCapStyleIdx, RGB( 0, 128, 128 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eStandingCapStyleIdx, RGB(  255, 0, 255 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto3OvbLevelStyleIdx, COLOR_PANEDATE3);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto3ResHoldTotStyleIdx, COLOR_PANEDATE3);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto2OvbLevelStyleIdx, COLOR_PANEDATE2);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto2ResHoldTotStyleIdx, COLOR_PANEDATE2);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto1OvbLevelStyleIdx, COLOR_PANEDATE1);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto1ResHoldTotStyleIdx, COLOR_PANEDATE1);
  m_aGraphColors.SetAtGrow( CCTAChart::ePrevisionStyleIdx, COLOR_PREVISION);
  m_aGraphColors.SetAtGrow( CCTAChart::eRevTicketeStyleIdx, COLOR_RECTICKETE);
  m_aGraphColors.SetAtGrow( CCTAChart::eHoiTraStyleIdx, RGB(  128, 128, 0 )); //idem eResHoldTotStyleIdx
  m_aGraphColors.SetAtGrow( CCTAChart::eHoiTraIdvStyleIdx, RGB(  128, 0, 0 )); //idem eResHoldIdvStyleIdx
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto3HoiTraStyleIdx, COLOR_PANEDATE3);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto2HoiTraStyleIdx, COLOR_PANEDATE2);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto1HoiTraStyleIdx, COLOR_PANEDATE1);
}

void CCTAGlobalSysParms::DefineSNCFTRNColors()
{
  m_aGraphColors.SetAtGrow( CCTAChart::eAuthStyleIdx, RGB( 0, 0, 128 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eCapStyleIdx, RGB(  255, 0, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eDmdDptFcstStyleIdx, RGB(  0, 128, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eBktResHoldAvailSumStyleIdx, RGB(  245, 124, 3 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldTotStyleIdx, RGB(  128, 128, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldAvgStyleIdx, RGB(  255, 0, 255 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldIdvStyleIdx, RGB(128, 0, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eTrafficResStyleIdx, RGB(0, 0, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eRevAvgFcstTotStyleIdx, RGB(0, 255, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eRevAvgFcstIdvStyleIdx, RGB(255, 255, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eRevAvgFcstHighStyleIdx, RGB(255, 0, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eRevAvgFcstLowStyleIdx, ::GetSysColor(COLOR_WINDOW));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldStdvHighStyleIdx, RGB(255, 200, 200));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldStdvLowStyleIdx, ::GetSysColor(COLOR_WINDOW));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket0StyleIdx, RGB(252, 90, 50));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket1StyleIdx, RGB(0, 255, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket2StyleIdx, RGB(0, 255, 255));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket3StyleIdx, RGB(255, 0, 255));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket4StyleIdx, RGB(128, 128, 128));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket5StyleIdx, RGB(128, 0, 128));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket6StyleIdx, RGB(128, 0, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket7StyleIdx, RGB(0, 0, 128));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket8StyleIdx, RGB(0, 128, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket9StyleIdx, RGB(128, 128, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eVoidStyleIdx, ::GetSysColor(COLOR_WINDOW));
  m_aGraphColors.SetAtGrow( CCTAChart::eUnreservableCapStyleIdx, RGB( 0, 128, 128 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eStandingCapStyleIdx, RGB(  255, 0, 255 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto3OvbLevelStyleIdx, COLOR_PANEDATE3);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto3ResHoldTotStyleIdx, COLOR_PANEDATE3);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto2OvbLevelStyleIdx, COLOR_PANEDATE2);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto2ResHoldTotStyleIdx, COLOR_PANEDATE2);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto1OvbLevelStyleIdx, COLOR_PANEDATE1);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto1ResHoldTotStyleIdx, COLOR_PANEDATE1);
  m_aGraphColors.SetAtGrow( CCTAChart::eCmptComptageOfferStdvHighStyleIdx, RGB(  255, 0, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eCmptComptageOfferStyleIdx, RGB(  255, 0, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eCmptComptageOfferStdvLowStyleIdx, RGB(  255, 0, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eCmptComptageOccupStdvHighStyleIdx, RGB(  128, 128, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eCmptComptageOccupStyleIdx, RGB(  128, 128, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eCmptComptageOccupStdvLowStyleIdx, RGB(  128, 128, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eCmptComptageHist3OfferStyleIdx, COLOR_PANEDATE3);
  m_aGraphColors.SetAtGrow( CCTAChart::eCmptComptageHist3OccupStyleIdx, COLOR_PANEDATE3);
  m_aGraphColors.SetAtGrow( CCTAChart::eCmptComptageHist2OfferStyleIdx, COLOR_PANEDATE2);
  m_aGraphColors.SetAtGrow( CCTAChart::eCmptComptageHist2OccupStyleIdx, COLOR_PANEDATE2);
  m_aGraphColors.SetAtGrow( CCTAChart::eCmptComptageHist1OfferStyleIdx, COLOR_PANEDATE1);
  m_aGraphColors.SetAtGrow( CCTAChart::eCmptComptageHist1OccupStyleIdx, COLOR_PANEDATE1);
  m_aGraphColors.SetAtGrow( CCTAChart::ePrevisionStyleIdx, COLOR_PREVISION);
  m_aGraphColors.SetAtGrow( CCTAChart::eRevTicketeStyleIdx, COLOR_RECTICKETE);
  m_aGraphColors.SetAtGrow( CCTAChart::eHoiTraStyleIdx, RGB(  128, 128, 0 )); //idem eResHoldTotStyleIdx
  m_aGraphColors.SetAtGrow( CCTAChart::eHoiTraIdvStyleIdx, RGB(  128, 0, 0 )); //idem eResHoldIdvStyleIdx
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto3HoiTraStyleIdx, COLOR_PANEDATE3);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto2HoiTraStyleIdx, COLOR_PANEDATE2);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto1HoiTraStyleIdx, COLOR_PANEDATE1);
}

void CCTAGlobalSysParms::DefineThalysColors()
{
  m_aGraphColors.SetAtGrow( CCTAChart::eAuthStyleIdx, RGB( 0, 0, 128 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eCapStyleIdx, RGB(  255, 0, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eDmdDptFcstStyleIdx, RGB(  0, 128, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eBktResHoldAvailSumStyleIdx, RGB(  0, 128, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldTotStyleIdx, RGB(  128, 128, 0 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldAvgStyleIdx, RGB(  255, 0, 255 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldIdvStyleIdx, RGB(128, 0, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eTrafficResStyleIdx, RGB(0, 0, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eRevAvgFcstTotStyleIdx, RGB(0, 255, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eRevAvgFcstIdvStyleIdx, RGB(255, 255, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eRevAvgFcstHighStyleIdx, RGB(255, 0, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eRevAvgFcstLowStyleIdx, ::GetSysColor(COLOR_WINDOW));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldStdvHighStyleIdx, RGB(255, 200, 200));
  m_aGraphColors.SetAtGrow( CCTAChart::eResHoldStdvLowStyleIdx, ::GetSysColor(COLOR_WINDOW));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket0StyleIdx, RGB(255,255, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket1StyleIdx, RGB(0, 255, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket2StyleIdx, RGB(0, 255, 255));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket3StyleIdx, RGB(255, 0, 255));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket4StyleIdx, RGB(128, 128, 128));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket5StyleIdx, RGB(128, 0, 128));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket6StyleIdx, RGB(128, 0, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket7StyleIdx, RGB(0, 0, 128));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket8StyleIdx, RGB(0, 128, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eBucket9StyleIdx, RGB(128, 128, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eVoidStyleIdx, ::GetSysColor(COLOR_WINDOW));
  m_aGraphColors.SetAtGrow( CCTAChart::eUnreservableCapStyleIdx, RGB( 0, 128, 128 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eStandingCapStyleIdx, RGB(  255, 0, 255 ));
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto3OvbLevelStyleIdx, RGB (255, 128, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto3ResHoldTotStyleIdx, RGB (255, 128, 0));
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto2OvbLevelStyleIdx, COLOR_PANEDATE3);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto2ResHoldTotStyleIdx, COLOR_PANEDATE3);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto1OvbLevelStyleIdx, COLOR_PANEDATE1);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto1ResHoldTotStyleIdx, COLOR_PANEDATE1);
  m_aGraphColors.SetAtGrow( CCTAChart::ePrevisionStyleIdx, COLOR_PREVISION);
  m_aGraphColors.SetAtGrow( CCTAChart::eRevTicketeStyleIdx, RGB (220, 128, 128));
  m_aGraphColors.SetAtGrow( CCTAChart::eHoiTraStyleIdx, RGB(  128, 128, 0 )); //idem eResHoldTotStyleIdx
  m_aGraphColors.SetAtGrow( CCTAChart::eHoiTraIdvStyleIdx, RGB(  128, 0, 0 )); //idem eResHoldIdvStyleIdx
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto3HoiTraStyleIdx, COLOR_PANEDATE3);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto2HoiTraStyleIdx, COLOR_PANEDATE2);
  m_aGraphColors.SetAtGrow( CCTAChart::eHisto1HoiTraStyleIdx, COLOR_PANEDATE1);
}

// return the RGB (color) value for a given enumerated index (client-specific...see Define*Colors())
int CCTAGlobalSysParms::GetGraphRGBValue(CCTAChart::CtaDataStyleIdx eDataStyleIdx)
{ 
  if (eDataStyleIdx >= m_aGraphColors.GetSize())
    return 0;
  if (eDataStyleIdx >= CCTAChart::eLastStyleIdx)
    return 0;
  return  m_aGraphColors.GetAt(eDataStyleIdx);
}

BOOL CCTAGlobalSysParms::GetYieldRevenuFlag()
{
	if (!GetRevenuFlag())
		  return TRUE; // DT23805 JRO
		  
    else if ( GetTRNFlag() )
	{
	   YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
		
	   if ((pTranche) && (pTranche->YieldManaged() == 0))
		  
		   return TRUE; // DT23805 JRO
	   
	}
	
    return TRUE;// C'est un client "revenu"
	
}

/////////////////////////////////////////////////////////////////////////////
// CCTAGlobalSysParms message handlers
