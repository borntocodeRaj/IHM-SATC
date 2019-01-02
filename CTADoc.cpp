// CTADoc.cpp : implementation of the CCTADoc class
//
#include "StdAfx.h"


#include "CTAChartHelpDlg.h"
#include "BktHistView.h"
#include "HistDptView.h"

#include "OthLegsView.h"
#include "RRDHistView.h"
#include "ResaRailView.h"
#include "TrancheView.h"
#include "EntityDlg.h"
#include "ResTracer.h"
#include "JumeauxDlg.h"
#include "BktSciSphere.h"
#include "SpheresPrix.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCTADoc

IMPLEMENT_DYNCREATE(CCTADoc, CDocument)

BEGIN_MESSAGE_MAP(CCTADoc, CDocument)
	//{{AFX_MSG_MAP(CCTADoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCTADoc construction/destruction

CCTADoc::CCTADoc()
{
  m_bAutoDelete = FALSE;

  CreateRecordSets();  // create all record sets
  CreateRecordKeys();  // initialize the keys for all record sets
  
  m_pRefDataCtl = NULL;
  
  m_bRefDataError = FALSE;
  m_bRefDataDone = FALSE;
  m_bFirstTestBatchFinished = TRUE;
  m_bBatchFinished = TRUE;
  m_bNomosFinished = TRUE;
  m_bHideNomosNotFinish = FALSE;
  m_bFullInitCompleted = FALSE;
  m_bNotFullJX = FALSE;
  
  m_fontname = "";
  m_fontheight = 0;
  m_fontweight = 2; // valeur bidon, pour dire non initialisé
  m_fontitalic = 2; // valeur bidon, pour dire non initialisé
  
  m_pScreenStdFont = NULL;
  m_pScreenBoldFont = NULL;
  m_pScreenTitleFont = NULL;
  m_pScreenFixedFont = NULL;
  m_pPrintStdFont = NULL;
  m_pPrintTitleFont = NULL;
  m_pPrintFixedFont = NULL;
  m_ScreenResol = 0;
  m_PrintResol = 0;
  m_lDiffSysDate = 0;
  m_lLastAuthFcDate = 0;
  m_bConstrainLastAuthFcDate = TRUE;
  
  m_pGlobalSysParms = NULL;
  
  m_lMinRrdIndex = 0;
  m_lMaxRrdIndex = 0;
  m_pCurrentEntityGag = NULL;
  m_decalBatch = 0;

   m_pDlgTgvJumeaux = NULL;
   m_pTrainInfluentDlg = NULL;

  m_bTrainAutoOnly = FALSE; //DM5480
  m_bDbLinkCreated = FALSE;
  m_bAllJMX = FALSE;
  m_szEntities = "";

  
  m_sphereManager = new CSphereManager();// DM7978 KGH

  ReloadScreenFonts();

  m_lCurrentEntityNatureID = -1; // NPI - DM7870 : Dates de références
}

CCTADoc::~CCTADoc()
{
  if (m_pScreenStdFont)
  {
    delete m_pScreenStdFont;
    m_pScreenStdFont = NULL;
  }
  if (m_pScreenBoldFont)
  {
    delete m_pScreenBoldFont;
    m_pScreenBoldFont = NULL;
  }
  if (m_pScreenTitleFont)
  {
    delete m_pScreenTitleFont;
    m_pScreenTitleFont = NULL;
  }
  if (m_pScreenFixedFont)
  {
    delete m_pScreenFixedFont;
    m_pScreenFixedFont = NULL;
  }
  if (m_pPrintStdFont)
  {
    delete m_pPrintStdFont;
    m_pPrintStdFont = NULL;
  }
  if (m_pPrintTitleFont)
  {
    delete m_pPrintTitleFont;
    m_pPrintTitleFont = NULL;
  }
  if (m_pPrintFixedFont)
  {
    delete m_pPrintFixedFont;
    m_pPrintFixedFont = NULL;
  }
  
  // Reference Data
  delete m_pDCDateSet;
  m_pDCDateSet = NULL;
  delete m_pSysDateSet;
  m_pSysDateSet = NULL;
  delete m_pScStatusSet;
  m_pScStatusSet = NULL;
  delete m_pBrmsDateSet;
  m_pBrmsDateSet = NULL;
  delete m_pHistoStatusSet;
  m_pHistoStatusSet = NULL;
  delete m_pStationNotGrp;
  m_pStationNotGrp = NULL;
  delete m_pCmptSet;
  m_pCmptSet = NULL;
  delete m_pMaxEntityRrdSet;
  m_pMaxEntityRrdSet = NULL;
  delete m_pDefRrdSet;
  m_pDefRrdSet = NULL;
  delete m_pFullJXSet;
  m_pFullJXSet = NULL;
  delete m_pUserInfoSet;
  m_pUserInfoSet = NULL;
  delete m_pSysParmSet;
  m_pSysParmSet = NULL;
  delete m_pOrderSVCSet;
  m_pOrderSVCSet = NULL;
  delete m_pTgvStations;
  m_pTgvStations = NULL;
  delete m_pVnauListSet;
  m_pVnauListSet = NULL;
  delete m_pPrevisionSet;
  m_pPrevisionSet = NULL;
  delete m_pSeasonsSet;
  m_pSeasonsSet = NULL;
  delete m_pCongesSet;
  m_pCongesSet = NULL;
  delete m_pMinMaxDptDate;
  m_pMinMaxDptDate = NULL;
  delete m_pTraqueTarifSet;
  m_pTraqueTarifSet = NULL;
  delete m_pNomosVnauExclu;
  m_pNomosVnauExclu = NULL;
  delete m_pTopSet;
  m_pTopSet = NULL;
  delete m_pSensibiliteSet;
  m_pSensibiliteSet = NULL;

  // requêtes liées au calendier DM 6881
  delete m_pCalendarTrainSet;
  m_pCalendarTrainSet = NULL;
  delete m_pCalendarPastTrainSet;
  m_pCalendarPastTrainSet = NULL;
  delete m_pCalendarTDLJKSet;
  m_pCalendarTDLJKSet = NULL;
  
  // Entity, Tranche, and Leg Lists
  delete m_pEntitySet;
  m_pEntitySet = NULL;
  delete m_pEntityGagSet;
  m_pEntityGagSet = NULL;
  delete m_pTrancheSet;
  m_pTrancheSet = NULL;
  delete m_pTDLJKSet;
  m_pTDLJKSet = NULL;
  delete m_pTDPJESet;
  m_pTDPJESet = NULL;
  delete m_pHoiTraISet;
  m_pHoiTraISet = NULL;
  delete m_pHoiTraRSet;
  m_pHoiTraRSet = NULL;
  delete m_pLegSet;
  m_pLegSet = NULL;
  
  // All other Windows
  delete m_pResaBktSet;
  m_pResaBktSet = NULL;
  delete m_pResaCmptSet;
  m_pResaCmptSet = NULL;
  delete m_pResaBktSciSet;
  m_pResaBktSciSet = NULL;
  delete m_pResaCmptSciSet;
  m_pResaCmptSciSet = NULL;
  delete m_pBktEvolSet;
  m_pBktEvolSet = NULL;
  delete m_pBktEvolAltSet;
  m_pBktEvolAltSet = NULL;
  delete m_pBktEvolAvgSet;
  m_pBktEvolAvgSet = NULL;
  delete m_pBktEvolAvgAltSet;
  m_pBktEvolAvgAltSet = NULL;
  delete m_pCmptEvolSet;
  m_pCmptEvolSet = NULL;
  delete m_pCmptEvolAltSet;
  m_pCmptEvolAltSet = NULL;
  delete m_pCmptEvolAvgSet;
  m_pCmptEvolAvgSet = NULL;
  delete m_pCmptEvolAvgAltSet;
  m_pCmptEvolAvgAltSet = NULL;
  delete m_pHistoEvolSet;
  m_pHistoEvolSet = NULL;
  delete m_pHistoEvolAltSet;
  m_pHistoEvolAltSet = NULL;
  delete m_pHistoHoiTraISet;
  m_pHistoHoiTraISet = NULL;
  delete m_pHistoHoiTraRSet;
  m_pHistoHoiTraRSet = NULL;
  delete m_pHistoEvolCmptSet;
  m_pHistoEvolCmptSet = NULL;
  delete m_pHistoEvolCmptAltSet;
  m_pHistoEvolCmptAltSet = NULL;
  delete m_pBktHistSet;
  m_pBktHistSet = NULL;
  delete m_pBktHistAltSet;
  m_pBktHistAltSet = NULL;
  delete m_pBktHistoSet;
  m_pBktHistoSet = NULL;
  delete m_pBktHistoAltSet;
  m_pBktHistoAltSet = NULL;
  delete m_pPrevAuthSet;
  m_pPrevAuthSet = NULL;
  delete m_pHistDptSet;
  m_pHistDptSet = NULL;
  delete m_pHistDptAltSet;
  m_pHistDptAltSet = NULL;
  delete m_pOthLegsSet;
  m_pOthLegsSet = NULL;
  delete m_pOthLegsAltSet;
  m_pOthLegsAltSet = NULL;
  delete m_pVirtNestSet;
  m_pVirtNestSet = NULL;
  delete m_pRelTrancheSet;
  m_pRelTrancheSet = NULL;
  delete m_pRelTrancheAltSet;
  m_pRelTrancheAltSet = NULL;
  delete m_pRelTgvSet;
  m_pRelTgvSet = NULL;
  delete m_pRelTgvAltSet;
  m_pRelTgvAltSet = NULL;
  if (m_pQuelCmptSet)
  {
    delete m_pQuelCmptSet;
    m_pQuelCmptSet = NULL;
  }
  if (m_pComptagesSet)
  {
    delete m_pComptagesSet;
    m_pComptagesSet = NULL;
  }
  if (m_pComptagesHistSet)
  {
    delete m_pComptagesHistSet;
    m_pComptagesHistSet = NULL;
  }
  if (m_pTrainSupplementSet)
  {
    delete m_pTrainSupplementSet;
    m_pTrainSupplementSet = NULL;
  }
  if (m_pTrainDeletedSet)
  {
    delete m_pTrainDeletedSet;
    m_pTrainDeletedSet = NULL;
  }
  if (m_pTrainAdaptationSet)
  {
    delete m_pTrainAdaptationSet;
    m_pTrainAdaptationSet = NULL;
  }
  
  // VNL Engine
  delete m_pTrainEntitySet;
  m_pTrainEntitySet = NULL;
  delete m_pTrainTrancheSet;
  m_pTrainTrancheSet = NULL;
  delete m_pServedLegWEntSet;
  m_pServedLegWEntSet = NULL;
  delete m_pServedLegSet;
  m_pServedLegSet = NULL;
  delete m_pODTrainWEntSet;
  m_pODTrainWEntSet = NULL;
  delete m_pODTrainSet;
  m_pODTrainSet = NULL;
  delete m_pAllTrainWEntSet;
  m_pAllTrainWEntSet = NULL;
  delete m_pAllTrainSet;
  m_pAllTrainSet = NULL;
  delete m_pServedMarketWEntSet;
  m_pServedMarketWEntSet = NULL;
  delete m_pServedMarketSet;
  m_pServedMarketSet = NULL;
  delete m_pLegTimesSet;
  m_pLegTimesSet = NULL;
  //DM7790
  delete m_pAxeSet;
  m_pAxeSet = NULL;
  delete m_pRouteAxeSet;
  m_pRouteAxeSet = NULL;
  delete m_pEntityRouteSet;
  m_pEntityRouteSet = NULL;
  delete m_pAllTrainWAxesSet;
  m_pAllTrainWAxesSet = NULL;
  delete m_pAllTrainWRoutesSet;
  m_pAllTrainWRoutesSet = NULL;

  // NPI - DM7870 : Dates de référence
  delete m_pEntiteNatureSet;
  m_pEntiteNatureSet = NULL;
  
  // VNAU automatique
  delete m_pTgvGagSet;
  m_pTgvGagSet = NULL;
  delete m_pOptimOpenSet;
  m_pOptimOpenSet = NULL;
  delete m_pOptimLastSet;
  m_pOptimLastSet = NULL;

  delete m_pDateHistoSet;
  m_pDateHistoSet = NULL;
  
  if (m_pRefDataCtl)
  {
    delete m_pRefDataCtl;
    m_pRefDataCtl = NULL;
  }
   
  // miscellaneous
  if (m_pGlobalSysParms)
  {
    if (m_pDlgTgvJumeaux)
    {
      delete m_pDlgTgvJumeaux;
      m_pDlgTgvJumeaux = NULL;
    }
    if (m_pTrainInfluentDlg)
    {
      delete m_pTrainInfluentDlg;
      m_pTrainInfluentDlg = NULL;
    }
    delete m_pGlobalSysParms;
    m_pGlobalSysParms = NULL;
  }

  //DM 5350 recette
  delete m_pBktEvolRevSet;
  delete m_pBktEvolRevAltSet;
  delete m_pBktEvolAvgRevSet;
  delete m_pBktEvolAvgRevAltSet;
  delete m_pCmptEvolRevSet;
  delete m_pCmptEvolRevAltSet;
  delete m_pCmptEvolAvgRevSet;
  delete m_pCmptEvolAvgRevAltSet;
  delete m_pHistoEvolRevSet;
  delete m_pHistoEvolRevAltSet;
  delete m_pHistoEvolRevCmptSet;
  delete m_pHistoEvolRevCmptAltSet;

  //Année précedente pour historique par départ 
  delete m_pHistDptYavSet;
  m_pHistDptYavSet = NULL;
  delete m_pHistDptYavAltSet;
  m_pHistDptYavAltSet = NULL;


  // DM-7978 - KGH - Sphere prix	
  if ( m_sphereManager != NULL)
  {	
	  delete m_sphereManager;
	  m_sphereManager = NULL;
  }
  // DM-7978 - KGH - FIN

	// DM-7978 - LME - CCmax CCouv et J-x histo
  /*if ( m_pHistoDataRRDSet != NULL)
  {	
	  delete m_pHistoDataRRDSet;
	  m_pHistoDataRRDSet = NULL;
  }*/
  // DM-7978 - LME - FIN

	// DM-7978 - LME - YI-6046
  if ( m_pAscTarifSet != NULL)
  {	
	  delete m_pAscTarifSet;
	  m_pAscTarifSet = NULL;
  }
  // DM-7978 - LME - FIN
}

void CCTADoc::CreateRecordSets() 
{
  // Reference Data
  m_pDCDateSet = new YM_ArraySet<YmIcDcDateDom>(App().m_pDatabase, IDS_SQL_SELECT_DC_DATE, NULL, YM_RecordSet::AUTO_SELECT);
  m_pDCDateSet->SetKey(DCDATE_KEY);
  m_pSysDateSet = new YM_ArraySet<YmIcSysDateDom>(App().m_pDatabase, IDS_SQL_SELECT_SYS_DATE, NULL, YM_RecordSet::AUTO_SELECT);
  m_pSysDateSet->SetKey(SYSDATE_KEY);
  m_pScStatusSet = new YM_ArraySet<YmIcSysParmsDom>(App().m_pDatabase,IDS_SQL_SELECT_STATUS,NULL,YM_RecordSet::AUTO_SELECT);
  m_pScStatusSet->SetKey(SCSTATUS_KEY);
  m_pHistoStatusSet = new YM_ArraySet<YmIcSysParmsDom>(App().m_pDatabase,IDS_SQL_SELECT_HISTO_STATUS,NULL,YM_RecordSet::AUTO_SELECT);
  m_pHistoStatusSet->SetKey(HISTOSTATUS_KEY);
  m_pBrmsDateSet = new YM_ArraySet<YmIcVnauDom>(App().m_pDatabase,IDS_SQL_GET_MAX_DATE_BRMS,NULL,YM_RecordSet::AUTO_SELECT);
  m_pBrmsDateSet->SetKey(BRMS_KEY);
  m_pStationNotGrp = new YM_ArraySet<YmBaTgvStationDom>(App().m_pDatabase, IDS_SQL_SELECT_STATIONS_NOGROUP, NULL, YM_RecordSet::AUTO_SELECT);
  m_pStationNotGrp->SetKey(STATION_KEY);
  
  m_pCmptSet = new YM_ArraySet<YmIcCmptDom>(App().m_pDatabase, IDS_SQL_SELECT_CMPT_LIST, NULL, YM_RecordSet::AUTO_SELECT);
  m_pCmptSet->SetKey(CMPT_KEY);
  m_pMaxEntityRrdSet = new YM_ArraySet<YmIcRdDaysDom>(App().m_pDatabase, IDS_SQL_SELECT_MAX_ENTITY_RRD, NULL, YM_RecordSet::AUTO_SELECT);
  m_pMaxEntityRrdSet->SetKey(MAX_ENTITY_RRD_KEY);
  m_pDefRrdSet = new YM_ArraySet<YmIcRdDaysDom>(App().m_pDatabase, IDS_SQL_SELECT_DEF_RRD, NULL, YM_RecordSet::AUTO_SELECT);
  m_pDefRrdSet->SetKey(DEF_RRD_KEY);
  m_pFullJXSet = new YM_ArraySet<YmIcRdDaysDom>(App().m_pDatabase, IDS_SQL_SELECT_FULL_JX, NULL, YM_RecordSet::AUTO_SELECT);
  m_pFullJXSet->SetKey(FULL_JX_KEY);
  m_pUserInfoSet = new CCUserInfo(App().m_pDatabase);
  m_pUserInfoSet->SetKey(USERINFO_KEY);
  m_pSysParmSet = new YM_ArraySet<YmIcSysParmsDom>(App().m_pDatabase, IDS_SQL_SELECT_SYS_PARMS, NULL, YM_RecordSet::AUTO_SELECT);
  m_pSysParmSet->SetKey(SYSPARM_KEY);
  m_pOrderSVCSet = new YM_ArraySet<YmBaCosGenDom>(App().m_pDatabase, IDS_SQL_SELECT_ORDER_SVC, NULL, YM_RecordSet::AUTO_SELECT);
  m_pOrderSVCSet->SetKey(SVC_KEY);
  m_pTgvStations = new YM_ArraySet<YmBaTgvStationDom>(App().m_pDatabase, IDS_SQL_SELECT_TGV_STATIONS, NULL, YM_RecordSet::AUTO_SELECT);
  m_pTgvStations->SetKey(TGVSTATIONS_KEY);
  m_pVnauListSet = new CVnauListSet(App().m_pDatabase);
  m_pVnauListSet->SetKey(VNAU_LIST_KEY);
  m_pTraqueTarifSet = new CTraqueTarifSet(App().m_pDatabase);
  m_pTraqueTarifSet->SetKey(TRAQUE_TARIF_KEY);
  m_pPrevisionSet = new CPrevisionSet(App().m_pDatabase);
  m_pPrevisionSet->SetKey(PREVISION_KEY);
  m_pSeasonsSet = new YM_ArraySet<YmIcSeasonDom>(App().m_pDatabase, IDS_SQL_GET_SEASONS, NULL, YM_RecordSet::AUTO_SELECT);
  m_pSeasonsSet->SetKey(SEASONS_KEY);
  m_pCongesSet = new YM_ArraySet<YmIcCongesDom>(App().m_pDatabase, IDS_SQL_GET_CONGES, NULL, YM_RecordSet::AUTO_SELECT);
  m_pCongesSet->SetKey(CONGES_KEY);
  m_pMinMaxDptDate = new YM_ArraySet<YmIcCongesDom>(App().m_pDatabase, IDS_SQL_GET_MINMAX_DPTDATE, NULL, YM_RecordSet::AUTO_SELECT);
  m_pMinMaxDptDate->SetKey(MINMAX_DPT_KEY);
  m_pNomosVnauExclu = new CVnauExcluSet (App().m_pDatabase);
  m_pNomosVnauExclu->SetKey(VNAU_EXCLU_KEY);
  m_pTopSet = new YM_ArraySet<YmIcTypeTrainDom>(App().m_pDatabase, IDS_SQL_SELECT_TYPE_TRAIN, NULL, YM_RecordSet::AUTO_SELECT);
  m_pTopSet->SetKey(TOP_KEY);
  m_pSensibiliteSet = new YM_ArraySet<YmIcTypeTrainDom>(App().m_pDatabase, IDS_SQL_SELECT_SENSIBILITE, NULL, YM_RecordSet::AUTO_SELECT);
  m_pSensibiliteSet->SetKey(SENSIBILITE_KEY);

  //DM 6881, calendrier
  m_pCalendarTrainSet = new CCalendarTrainSet(App().m_pDatabase);
  m_pCalendarTrainSet->SetKey(CALENDAR_TRAIN_KEY);
  m_pCalendarPastTrainSet = new CCalendarTrainSet(App().m_pDatabase);
  m_pCalendarPastTrainSet->SetKey(CALENDAR_PAST_TRAIN_KEY);
  m_pCalendarPastTrainSet->SetTransIdSelect(IDS_SQL_CALENDAR_TRAIN_PAST);
  m_pCalendarTDLJKSet = new CCalendarTDLJKSet(App().m_pDatabase);
  m_pCalendarTDLJKSet->SetKey(CALENDAR_TDLJK_KEY);
  
  // Entity, Tranche, and Leg Lists
  m_pEntitySet = new YM_ArraySet<YmIcEntitiesDom>(App().m_pDatabase, IDS_SQL_SELECT_ENTITY_LIST);
  m_pEntitySet->SetKey(ENTITY_KEY);
  m_pEntityGagSet = new YM_ArraySet<YmIcEntitiesDom>(App().m_pDatabase, IDS_SQL_SELECT_ENTITY_GAG);
  m_pEntityGagSet->SetKey(ENTITY_GAG_KEY);
  m_pTrancheSet = new CTrancheSet(App().m_pDatabase);
  m_pTrancheSet->SetKey(TRANCHE_KEY);
  if (App().m_pDatabaseAlleo != NULL)
  {
	TRACE("Connexion alternative initialisée\n");
	m_pTDLJKSet = new CTDLJKSet(App().m_pDatabaseAlleo);
	m_pTDLJKSet->SetKey(TDLJK_KEY);
	TRACE("User = %s, Pwd = %s\n", App().m_pDatabaseAlleo->UserName(), App().m_pDatabaseAlleo->HostString());//TODO - a virer
  }
  else
  {
	m_pTDLJKSet = new CTDLJKSet(App().m_pDatabase);
	m_pTDLJKSet->SetKey(TDLJK_KEY);
  }
  m_pTDPJESet = new CTDPJESet(App().m_pDatabase, ' ');
  m_pTDPJESet->SetKey(TDPJE_KEY);
  m_pHoiTraISet = new CTDPJESet(App().m_pDatabase, 'I');
  m_pHoiTraISet->SetKey(HOITRAI_KEY);
  m_pHoiTraISet->SetTransIdSelect (IDS_SELECT_MAX_HOI_TRA);
  m_pHoiTraRSet = new CTDPJESet(App().m_pDatabase, 'R');
  m_pHoiTraRSet->SetKey(HOITRAR_KEY);
  m_pHoiTraRSet->SetTransIdSelect (IDS_SELECT_MAX_HOI_TRA);
  m_pLegSet = new CLegSet(App().m_pDatabase);
  m_pLegSet->SetKey(LEG_KEY);
  m_pDateHistoSet = new CDateHistoSet(App().m_pDatabase);
  m_pDateHistoSet->SetKey(DATE_HISTO_KEY);

  // All other Windows
  m_pResaBktSet = new CResaBktSet(App().m_pDatabase);
  m_pResaBktSet->SetKey(RESABKT_KEY);
  m_pResaCmptSet = new CResaCmptSet(App().m_pDatabase);
  m_pResaCmptSet->SetKey(RESACMPT_KEY);
  m_pResaBktSciSet = new CResaBktSet(App().m_pDatabase);
  m_pResaBktSciSet->SetKey(RESABKT_SCI_KEY);
  m_pResaBktSciSet->SetTransIdSelect (IDS_SQL_SELECT_RESA_BKT_SCI);
  m_pResaBktSciSet->SetTransIdUpdate (IDS_SQL_UPDATE_RESA_BKT_AUTH_SCI);
  m_pResaCmptSciSet = new CResaCmptSet(App().m_pDatabase);
  m_pResaCmptSciSet->SetKey(RESACMPT_SCI_KEY);
  m_pResaCmptSciSet->SetTransIdSelect (IDS_SQL_SELECT_RESA_CMPT_SCI);
  m_pResaCmptSciSet->SetTransIdUpdate (NULL);
  m_pBktEvolSet = new CRRDHistBktEvolSet(App().m_pDatabase);
  m_pBktEvolSet->SetKey(BKTEVOL_KEY);
  m_pBktEvolAltSet = new CRRDHistBktEvolSet(App().m_pDatabase);
  m_pBktEvolAltSet->SetKey(BKTEVOL_ALT_KEY);
  m_pBktEvolAltSet->SetTransIdSelect(IDS_SQL_SELECT_RRD_HIST_BKT_SCI);
  m_pBktEvolAvgSet = new CRRDHistBktEvolSet(App().m_pDatabase);
  m_pBktEvolAvgSet->SetKey(BKTEVOL_AVG_KEY);
  m_pBktEvolAvgSet->SetTransIdSelect(IDS_SQL_SELECT_RRD_HIST_BKT_AVG);
  m_pBktEvolAvgAltSet = new CRRDHistBktEvolSet(App().m_pDatabase);
  m_pBktEvolAvgAltSet->SetKey(BKTEVOL_AVG_ALT_KEY);
  m_pBktEvolAvgAltSet->SetTransIdSelect(IDS_SQL_SELECT_RRD_HIST_BKT_SCI_AVG);
  m_pCmptEvolSet = new CRRDHistCmptEvolSet(App().m_pDatabase);
  m_pCmptEvolSet->SetKey(CMPTEVOL_KEY);
  m_pCmptEvolAltSet = new CRRDHistCmptEvolSet(App().m_pDatabase);
  m_pCmptEvolAltSet->SetKey(CMPTEVOL_ALT_KEY);			// requete IDS_SQL_SELECT_RRD_HIST_CMPT
  m_pCmptEvolAltSet->SetTransIdSelect(IDS_SQL_SELECT_RRD_HIST_CMPT_SCI);
  m_pCmptEvolAvgSet = new CRRDHistCmptEvolSet(App().m_pDatabase);
  m_pCmptEvolAvgSet->SetKey(CMPTEVOL_AVG_KEY);
  m_pCmptEvolAvgSet->SetTransIdSelect(IDS_SQL_SELECT_RRD_HIST_CMPT_AVG);
  m_pCmptEvolAvgAltSet = new CRRDHistCmptEvolSet(App().m_pDatabase);
  m_pCmptEvolAvgAltSet->SetKey(CMPTEVOL_AVG_ALT_KEY);
  m_pCmptEvolAvgAltSet->SetTransIdSelect(IDS_SQL_SELECT_RRD_HIST_CMPT_SCI_AVG);
  m_pHistoEvolSet = new CRRDHistoEvolSet(App().m_pDatabase, ' ');
  m_pHistoEvolSet->SetKey(HISTOEVOL_KEY);
  m_pHistoEvolAltSet = new CRRDHistoEvolSet(App().m_pDatabase, ' ');
  m_pHistoEvolAltSet->SetKey(HISTOEVOL_ALT_KEY);
  m_pHistoEvolAltSet->SetTransIdSelect (IDS_SQL_SELECT_RRD_DATES_HISTO_SCI);
  m_pHistoHoiTraISet = new CRRDHistoEvolSet(App().m_pDatabase, 'I');
  m_pHistoHoiTraISet->SetKey(HISTO_HOITRAI_KEY);
  m_pHistoHoiTraISet->SetTransIdSelect (IDS_SELECT_MAX_HISTO_HOI_TRA);
  m_pHistoHoiTraRSet = new CRRDHistoEvolSet(App().m_pDatabase, 'R');
  m_pHistoHoiTraRSet->SetKey(HISTO_HOITRAR_KEY);
  m_pHistoHoiTraRSet->SetTransIdSelect (IDS_SELECT_MAX_HISTO_HOI_TRA);
  m_pHistoEvolCmptSet = new CRRDHistoEvolSet(App().m_pDatabase, ' ');
  m_pHistoEvolCmptSet->SetKey(HISTOEVOL_CMPT_KEY);
  m_pHistoEvolCmptSet->SetTransIdSelect (IDS_SQL_SELECT_RRDCMPT_DATES_HISTO);
  m_pHistoEvolCmptAltSet = new CRRDHistoEvolSet(App().m_pDatabase, ' ');
  m_pHistoEvolCmptAltSet->SetKey(HISTOEVOL_CMPT_ALT_KEY);
  m_pHistoEvolCmptAltSet->SetTransIdSelect (IDS_SQL_SELECT_RRDCMPT_DATES_HISTO_SCI);
  m_pBktHistSet = new CBktHistSet(App().m_pDatabase);
  m_pBktHistSet->SetKey(BKTHIST_KEY);
  m_pBktHistAltSet = new CBktHistAltSet(App().m_pDatabase);
  m_pBktHistAltSet->SetKey(BKTHIST_ALT_KEY);
  m_pBktHistoSet = new CBktHistoSet(App().m_pDatabase);
  m_pBktHistoSet->SetKey(BKTHISTO_KEY);
  m_pBktHistoAltSet = new CBktHistoSet(App().m_pDatabase);
  m_pBktHistoAltSet->SetKey(BKTHISTO_ALT_KEY);
  m_pPrevAuthSet = new CPrevAuthSet(App().m_pDatabase);
  m_pPrevAuthSet->SetKey(PREVAUTH_KEY);
  m_pHistDptSet  = new CHistDptSet(App().m_pDatabase);
  m_pHistDptSet->SetKey(HISTDPT_KEY);
  m_pHistDptAltSet  = new CHistDptSet(App().m_pDatabase);
  m_pHistDptAltSet->SetKey(HISTDPT_ALT_KEY);
  m_pOthLegsSet  = new COthLegsSet(App().m_pDatabase);
  m_pOthLegsSet->SetKey(OTHLEGS_KEY);
  m_pOthLegsAltSet  = new COthLegsSet(App().m_pDatabase);
  m_pOthLegsAltSet->SetKey(OTHLEGS_ALT_KEY);
  m_pVirtNestSet = new CVirtNestSet(App().m_pDatabase);
  m_pVirtNestSet->SetKey(VIRTNEST_KEY);
  m_pRelTrancheSet  = new CRelatedSet(App().m_pDatabase);
  m_pRelTrancheSet->SetKey(RELTRANCHE_KEY);
  m_pRelTrancheAltSet  = new CRelatedAltSet(App().m_pDatabase);
  m_pRelTrancheAltSet->SetKey(RELTRANCHE_ALT_KEY);
  m_pRelTgvSet  = new CRelatedSet(App().m_pDatabase);
  m_pRelTgvSet->SetKey(RELATEDTGV_KEY);
  m_pRelTgvSet->SetTransIdSelect(IDS_SQL_SELECT_REL_TGV_TRN);
  m_pRelTgvAltSet  = new CRelatedAltSet(App().m_pDatabase);
  m_pRelTgvAltSet->SetKey(RELATEDTGV_ALT_KEY);
  m_pRelTgvAltSet->SetTransIdSelect(IDS_SQL_SELECT_REL_TGV_SCI_TRN);
  m_pQuelCmptSet = new CQuelCmptSet(App().m_pDatabase);
  m_pQuelCmptSet->SetKey(QUELCMPT_KEY);
  m_pComptagesSet = new CComptagesSet(App().m_pDatabase);
  m_pComptagesSet->SetKey(COMPTAGES_KEY);
  m_pComptagesHistSet = new CComptagesHistSet(App().m_pDatabase);
  m_pComptagesHistSet->SetKey(COMPTAGES_HIST_KEY);
  m_pTrainSupplementSet = new CTrainSupplementSet(App().m_pDatabase);
  m_pTrainSupplementSet->SetKey(TRAINSUPPLEMENT_KEY);
  m_pTrainDeletedSet = new CTrainDeletedSet(App().m_pDatabase);
  m_pTrainDeletedSet->SetKey(TRAINDELETED_KEY);
  m_pTrainAdaptationSet = new CTrainAdaptationSet(App().m_pDatabase);
  m_pTrainAdaptationSet->SetKey(TRAINADAPTATION_KEY);
  
  // VNL Engine
  m_pTrainEntitySet = new CTrainEntitySet(App().m_pDatabase);
  m_pTrainEntitySet->SetKey(TRAIN_ENTITY_KEY);
  m_pTrainTrancheSet = new CTrainTrancheSet(App().m_pDatabase);
  m_pTrainTrancheSet->SetKey(TRAIN_TRANCHE_KEY);
  m_pServedLegWEntSet = new CServedLegWEntSet(App().m_pDatabase);
  m_pServedLegWEntSet->SetKey(SERVLEG_WENT_KEY);
  m_pServedLegSet = new CServedLegSet(App().m_pDatabase);
  m_pServedLegSet->SetKey(SERVLEG_KEY);
  m_pODTrainWEntSet = new CODTrainWEntSet(App().m_pDatabase);
  m_pODTrainWEntSet->SetKey(ODTRAIN_WENT_KEY);
  m_pODTrainSet = new CODTrainSet(App().m_pDatabase);
  m_pODTrainSet->SetKey(ODTRAIN_KEY);
  m_pAllTrainWEntSet = new CAllTrainWEntSet(App().m_pDatabase);
  m_pAllTrainWEntSet->SetKey(ALLTRAIN_WENT_KEY);
  m_pAllTrainSet = new CAllTrainSet(App().m_pDatabase);
  m_pAllTrainSet->SetKey(ALLTRAIN_KEY);
  m_pServedMarketWEntSet = new CServedMarketWEntSet(App().m_pDatabase);
  m_pServedMarketWEntSet->SetKey(SERVMARKET_WENT_KEY);
  m_pServedMarketSet = new CServedMarketSet(App().m_pDatabase);
  m_pServedMarketSet->SetKey(SERVMARKET_KEY);
  m_pLegTimesSet = new CLegTimesSet(App().m_pDatabase);
  m_pLegTimesSet->SetKey(LEGTIMES_KEY);
  // DM7790
  m_pAxeSet = new YM_ArraySet<YmIcAxesDom>(App().m_pDatabase, IDS_SQL_SELECT_AXE_LIST);
  m_pAxeSet->SetKey(AXE_KEY);
  m_pRouteAxeSet = new CRouteAxeSet(App().m_pDatabase);
  m_pRouteAxeSet->SetKey(ROUTE_AXE_KEY);
  m_pEntityRouteSet = new CEntityRouteSet(App().m_pDatabase);
  m_pEntityRouteSet->SetKey(ENTITY_ROUTE_KEY);
  m_pAllTrainWAxesSet = new CAllTrainWAxesSet(App().m_pDatabase);
  m_pAllTrainWAxesSet->SetKey(ALLTRAIN_WAXES_KEY);
  m_pAllTrainWRoutesSet = new CAllTrainWRoutesSet(App().m_pDatabase);
  m_pAllTrainWRoutesSet->SetKey(ALLTRAIN_WROUTES_KEY);
  
  // NPI - DM7870 : Dates de référence
  m_pEntiteNatureSet = new CEntityNatureSet(App().m_pDatabase);
  m_pEntiteNatureSet->SetKey(ENTITY_NATURE_KEY);

  // VNAU automatiques (SNCF only)
  m_pTgvGagSet = new CTgvGagSet(App().m_pDatabase);
  m_pTgvGagSet->SetKey(TGVGAG_KEY);
  m_pOptimOpenSet = new YM_ArraySet<YmIcOptimVnauDom>(App().m_pDatabase, IDS_SQL_SELECT_OPEN_OPTIM_VNAU, NULL, YM_RecordSet::AUTO_SELECT);
  m_pOptimOpenSet->SetKey(OPTIMVNAUOPEN_KEY);
  m_pOptimLastSet = new YM_ArraySet<YmIcOptimVnauDom>(App().m_pDatabase, IDS_SQL_SELECT_LAST_OPTIM_VNAU, NULL, YM_RecordSet::AUTO_SELECT);
  m_pOptimLastSet->SetKey(OPTIMVNAULAST_KEY);
  
  // DM5350 revenus

	//Montee en charge recette
	m_pBktEvolRevSet = new CRRDHistBktEvolSet(App().m_pDatabase);
    m_pBktEvolRevSet->SetKey(BKTEVOL_REC_KEY);
	m_pBktEvolRevSet->SetTransIdSelect(IDS_SQL_SELECT_REV_HIST_BKT);
    m_pBktEvolRevAltSet = new CRRDHistBktEvolSet(App().m_pDatabase);
    m_pBktEvolRevAltSet->SetKey(BKTEVOL_REC_ALT_KEY);
	m_pBktEvolRevAltSet->SetTransIdSelect(IDS_SQL_SELECT_REV_HIST_BKT_SCI);
	m_pBktEvolAvgRevSet = new CRRDHistBktEvolSet(App().m_pDatabase);
    m_pBktEvolAvgRevSet->SetKey(BKTEVOL_AVG_REC_KEY);
	m_pBktEvolAvgRevSet->SetTransIdSelect(IDS_SQL_SELECT_REV_HIST_BKT_AVG);
    m_pBktEvolAvgRevAltSet = new CRRDHistBktEvolSet(App().m_pDatabase);
    m_pBktEvolAvgRevAltSet->SetKey(BKTEVOL_AVG_REC_ALT_KEY);
	m_pBktEvolAvgRevAltSet->SetTransIdSelect(IDS_SQL_SELECT_REV_HIST_BKT_SCI_AVG);
    m_pCmptEvolRevSet = new CRRDHistCmptEvolSet(App().m_pDatabase);		
    m_pCmptEvolRevSet->SetKey(CMPTEVOL_REC_KEY);
	m_pCmptEvolRevSet->SetTransIdSelect(IDS_SQL_SELECT_REV_HIST_CMPT);
    m_pCmptEvolRevAltSet = new CRRDHistCmptEvolSet(App().m_pDatabase); 
    m_pCmptEvolRevAltSet->SetKey(CMPTEVOL_REC_ALT_KEY);
	m_pCmptEvolRevAltSet->SetTransIdSelect(IDS_SQL_SELECT_REV_HIST_CMPT_SCI);
	m_pCmptEvolAvgRevSet = new CRRDHistCmptEvolSet(App().m_pDatabase);		
    m_pCmptEvolAvgRevSet->SetKey(CMPTEVOL_AVG_REC_KEY);
	m_pCmptEvolAvgRevSet->SetTransIdSelect(IDS_SQL_SELECT_REV_HIST_CMPT_AVG);
    m_pCmptEvolAvgRevAltSet = new CRRDHistCmptEvolSet(App().m_pDatabase); 
    m_pCmptEvolAvgRevAltSet->SetKey(CMPTEVOL_AVG_REC_ALT_KEY);
	m_pCmptEvolAvgRevAltSet->SetTransIdSelect(IDS_SQL_SELECT_REV_HIST_CMPT_SCI_AVG);
	m_pHistoEvolRevSet = new CRRDHistoEvolSet(App().m_pDatabase, ' ');
    m_pHistoEvolRevSet->SetKey(HISTOEVOL_REC_KEY);
	m_pHistoEvolRevSet->SetTransIdSelect(IDS_SQL_SELECT_REV_DATES_HISTO);
    m_pHistoEvolRevAltSet = new CRRDHistoEvolSet(App().m_pDatabase, ' ');
    m_pHistoEvolRevAltSet->SetKey(HISTOEVOL_REC_ALT_KEY);
	m_pHistoEvolRevAltSet->SetTransIdSelect(IDS_SQL_SELECT_REV_DATES_HISTO_SCI);
	m_pHistoEvolRevCmptSet = new CRRDHistoEvolSet(App().m_pDatabase, ' ');
    m_pHistoEvolRevCmptSet->SetKey(HISTOEVOL_RECCMPT_KEY);
	m_pHistoEvolRevCmptSet->SetTransIdSelect(IDS_SQL_SELECT_REVCMPT_DATES_HISTO);
    m_pHistoEvolRevCmptAltSet = new CRRDHistoEvolSet(App().m_pDatabase, ' ');
    m_pHistoEvolRevCmptAltSet->SetKey(HISTOEVOL_RECCMPT_ALT_KEY);
	m_pHistoEvolRevCmptAltSet->SetTransIdSelect(IDS_SQL_SELECT_REVCMPT_DATES_HISTO_SCI);

	TRACE("Client = %c\n", ((CCTAApp*)APP)->m_Client);

	if (((CCTAApp*)APP)->m_Client == 'S')
	{
		m_pBktPrevisionVenteSet = new CBktPrevisionVenteSet(App().m_pDatabase);
		m_pBktPrevisionVenteSet->SetKey(PREVISION_VENTE_KEY);
		m_pBktPrevisionRevenuSet = new CBktPrevisionRevenuSet(App().m_pDatabase);
		m_pBktPrevisionRevenuSet->SetKey(PREVISION_REVENU_KEY);
	}

	// année précédente pour historique par départ
	m_pHistDptYavSet  = new CHistDptSet(App().m_pDatabase);
    m_pHistDptYavSet->SetKey(HISTDPT_YAV_KEY);
	m_pHistDptYavSet->SetTransIdSelect(IDS_SQL_SELECT_HIST_DPT_YAV);
    m_pHistDptYavAltSet  = new CHistDptSet(App().m_pDatabase);
    m_pHistDptYavAltSet->SetKey(HISTDPT_YAV_ALT_KEY);
	m_pHistDptYavAltSet->SetTransIdSelect(IDS_SQL_SELECT_HIST_DPT_YAV_SCI);
    
	m_pRelTrancheAltSet->SetTransIdSelect(IDS_SQL_SELECT_REL_TRANCHES_SCI);
	m_pOthLegsAltSet->SetTransIdSelect(IDS_SQL_SELECT_OTH_LEGS_SCI);
	m_pBktHistAltSet->SetTransIdSelect(IDS_SQL_SELECT_BKT_HIST_SCI);
	m_pBktHistoAltSet->SetTransIdSelect(IDS_SQL_SELECT_BKT_HISTO_SCI);
	m_pHistDptAltSet->SetTransIdSelect (IDS_SQL_SELECT_HIST_DPT_SCI);



	// DM7978 - LME - CCmax CCouv et j-x histo pour montee en charge
	/*m_pHistoDataRRDSet  = new CHistoDataRRDSet(App().m_pDatabase);
	m_pHistoDataRRDSet->SetKey(HISTO_DATA_RRD_KEY);*/
	// DM7978 - LME - FIN

	// DM7978 - LME - YI-6046
	m_pAscTarifSet = new CAscTarifSet(App().m_pDatabase);
	m_pAscTarifSet->SetKey(ASC_TARIF_KEY);
	// DM7978 - LME - FIN

}


void CCTADoc::AddCCsInTrancheDom(YmIcTrancheLisDom* tranche)//SRE 83664 - split UpdateCCMaxCCOuv (easier to read)
{
	CString szClient = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient();

	if (this->m_ccUserDom.getCmpt() == 'A')
		if (tranche->get('A', this->m_ccUserDom))
		{
			std::ostringstream ossCcMax;
			ossCcMax << 'A' << this->m_ccUserDom.getCCMaxUser();
			tranche->ccMax1(ossCcMax.str());
			tranche->ccMaxUser1(this->m_ccUserDom.getCCMaxUser());
			tranche->ccMaxInd1(this->m_ccUserDom.getCCMaxInd());
			
			std::ostringstream ossCcOuv;
			ossCcOuv << 'A' << this->m_ccUserDom.getLastOpenCCUser();
			tranche->ccOuv1(ossCcOuv.str());
			tranche->ccOuvInd1(this->m_ccUserDom.getLastOpenCCInd());
			tranche->ccOuvUser1(this->m_ccUserDom.getLastOpenCCUser());
		}

	if (this->m_ccUserDom.getCmpt() == 'B')
		if (tranche->get('B', this->m_ccUserDom))
		{
			std::ostringstream ossCcMax;
			ossCcMax << 'B' << this->m_ccUserDom.getCCMaxUser();
			tranche->ccMax2(ossCcMax.str());
			tranche->ccMaxUser2(this->m_ccUserDom.getCCMaxUser());
			tranche->ccMaxInd2(this->m_ccUserDom.getCCMaxInd());

			std::ostringstream ossCcOuv;
			ossCcOuv << 'B' << this->m_ccUserDom.getLastOpenCCUser();
			tranche->ccOuv2(ossCcOuv.str());
			tranche->ccOuvInd2(this->m_ccUserDom.getLastOpenCCInd());
			tranche->ccOuvUser2(this->m_ccUserDom.getLastOpenCCUser());
		}
		if (szClient == SNCF_TRN)
		{
			/// CORAIL 
			if (this->m_ccUserDom.getCmpt() == 'C')
				if (tranche->get('C', this->m_ccUserDom))
				{
					std::ostringstream ossCcMax;
					ossCcMax << 'C' << this->m_ccUserDom.getCCMaxUser();
					tranche->ccMax1(ossCcMax.str());
					tranche->ccMaxUser1(this->m_ccUserDom.getCCMaxUser());
					tranche->ccMaxInd1(this->m_ccUserDom.getCCMaxInd());

					std::ostringstream ossCcOuv;
					ossCcOuv << 'C' << this->m_ccUserDom.getLastOpenCCUser();
					tranche->ccOuv1(ossCcOuv.str());
					tranche->ccOuvInd1(this->m_ccUserDom.getLastOpenCCInd());
					tranche->ccOuvUser1(this->m_ccUserDom.getLastOpenCCUser());
				}
			if (this->m_ccUserDom.getCmpt() == 'D')
				if (tranche->get('D', this->m_ccUserDom))
				{
					std::ostringstream ossCcMax;
					ossCcMax << 'D' << this->m_ccUserDom.getCCMaxUser();
					tranche->ccMax2(ossCcMax.str());
					tranche->ccMaxUser2(this->m_ccUserDom.getCCMaxUser());
					tranche->ccMaxInd2(this->m_ccUserDom.getCCMaxInd());
					
					std::ostringstream ossCcOuv;
					ossCcOuv << 'D' << this->m_ccUserDom.getLastOpenCCUser();
					tranche->ccOuv2(ossCcOuv.str());
					tranche->ccOuvInd2(this->m_ccUserDom.getLastOpenCCInd());
					tranche->ccOuvUser2(this->m_ccUserDom.getLastOpenCCUser());
				}
			if (this->m_ccUserDom.getCmpt() == 'F')
				if (tranche->get('F', this->m_ccUserDom))
				{
					std::ostringstream ossCcMax;
					ossCcMax << 'F' << this->m_ccUserDom.getCCMaxUser();
					tranche->ccMax2(ossCcMax.str());
					tranche->ccMaxUser2(this->m_ccUserDom.getCCMaxUser());
					tranche->ccMaxInd2(this->m_ccUserDom.getCCMaxInd());

					std::ostringstream ossCcOuv;
					ossCcOuv << 'F' << this->m_ccUserDom.getLastOpenCCUser();
					tranche->ccOuv2(ossCcOuv.str());
					tranche->ccOuvInd2(this->m_ccUserDom.getLastOpenCCInd());
					tranche->ccOuvUser2(this->m_ccUserDom.getLastOpenCCUser());
				}
			if (this->m_ccUserDom.getCmpt() == 'O')
				if (tranche->get('O', this->m_ccUserDom))
				{
					std::ostringstream ossCcMax;
					ossCcMax << 'O' << this->m_ccUserDom.getCCMaxUser();
					tranche->ccMax2(ossCcMax.str());
					tranche->ccMaxUser2(this->m_ccUserDom.getCCMaxUser());
					tranche->ccMaxInd2(this->m_ccUserDom.getCCMaxInd());

					std::ostringstream ossCcOuv;
					ossCcOuv << 'O' << this->m_ccUserDom.getLastOpenCCUser();
					tranche->ccOuv2(ossCcOuv.str());
					tranche->ccOuvInd2(this->m_ccUserDom.getLastOpenCCInd());
					tranche->ccOuvUser2(this->m_ccUserDom.getLastOpenCCUser());
				}
		}
}

/// DM-7978 - CLE
/*
 *	SRE 83664 - rewritten method. Optimization.
 *	Modified the function to query "nbr_elements_to_query" number of CCMaxCCouv.
 *	Speed gain ~x4 to ~x6 (avoid 100+ items to query, it is slower, probably due to RogueWave's way to manage the query string)
 *	The method also keeps the behaviour of the previous version. Needed when we have one tranche to query.
 *	Permits to non recall the method to get our result.
 *	-- force parameter purpose: When we have tranchesNumEvaluated <"nbr_elements_to_query"
 *	-- we do not launch the query. So when we have less tranche to query than the trigger, we need to
 *	-- to force it to be launched to get our remaining results.
 */
void CCTADoc::UpdateCCMaxCCOuv(YmIcTrancheLisDom* tranche, YM_Iterator<YmIcTrancheLisDom>* pIterator, int index, int nbr_elements_to_query, bool force)
{
	static int tranchesNumEvaluated = 0;

	if (tranche)
		tranchesNumEvaluated++;

	if (tranche != NULL)
		this->m_ccUserDom.addTrDptJX(tranche->TrancheNo(), tranche->DptDate(), tranche->LastJX());

	if (!force || !tranchesNumEvaluated) //SRE 88480 - Vérifier qu'on a au moins un élément à requêter, si faux, sortir...
		if (tranchesNumEvaluated < nbr_elements_to_query)
			return;

	// NBN - ANO 74720
	YM_Query* pSQL = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
	pSQL->SetDomain (&(this->m_ccUserDom));
	RWDBReader Reader(((CCTAApp*)APP)->m_pDatabase->Transact(pSQL, IDS_SELECT_CCMAX_CCOUV));

	if (nbr_elements_to_query == 1)
	{
		tranche->clearCcMaxCcOuv();
	}
	int matched = 0;
	int readed = 0;
	while (Reader())
	{
		readed++;
		if (Reader.isValid())
		{
			Reader >> this->m_ccUserDom;

			if (tranche != NULL && nbr_elements_to_query == 1)
			{
				tranche->add(this->m_ccUserDom);
				AddCCsInTrancheDom(tranche);
			}
			else
			{
				int x = 0;
				pIterator->First();
				while (x++ < (index - tranchesNumEvaluated))
					pIterator->Next();

				for (int range_it = 0; range_it < tranchesNumEvaluated; range_it++)
				{
					if (pIterator->Current()->TrancheNo() == this->m_ccUserDom.getTrancheNo() &&
						pIterator->Current()->LastJX() == this->m_ccUserDom.getJx() &&
						pIterator->Current()->DptDate() == this->m_ccUserDom.getDptDate())
					{
						tranche = pIterator->Current();
						tranche->add(this->m_ccUserDom);
						AddCCsInTrancheDom(tranche);
					}
					pIterator->Next();
				}
			}
		}
	}
	// NBN - ANO 74720
	//SRE 83664 - Return back to our "to be queried" point in iterator.
	//tried to avoid that, but those iterators are not convenient...
	//should be possible by saving the address of our last position
	if (pIterator != NULL)
	{
		int x = 0;
		pIterator->First();
		while (x++ < (index -1))
			pIterator->Next();
	}
	this->m_ccUserDom.resetTrDptJX();
	tranchesNumEvaluated = 0;
	delete pSQL;
}

void CCTADoc::UpdateCCMaxCCOuvLegs(YmIcLegsDom* leg)
{
	YmIcUserCCDom ccUserDom;
	ccUserDom.setTrancheNo(leg->TrancheNo());
	ccUserDom.setDptDate(leg->DptDate());
	ccUserDom.setJx(leg->J_X());
	ccUserDom.setLegOrig(leg->LegOrig());
	ccUserDom.setLegDest(leg->LegDest());

	// Ano 74720
	YM_Query* pSQL = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
	pSQL->SetDomain (&ccUserDom);
	RWDBReader Reader( ((CCTAApp*)APP)->m_pDatabase->Transact(pSQL, IDS_SELECT_CCMAX_CCOUV_LEGS));
	
	leg->clearCcMaxCcOuv();

	while(Reader())
	{
		Reader >> ccUserDom;
		leg->add(ccUserDom);
	}

	CString szClient = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetClient();
	//LSO FIX 1089 - Corail doit aussi avoir les cmpt A et B
	/*if (szClient == SNCF)
	{*/
		/// TGV 
		if (leg->get('A', ccUserDom))
		{
			std::ostringstream ossCcMax;
			ossCcMax << 'A' << ccUserDom.getCCMaxUser();
			leg->ccMax1(ossCcMax.str());
			leg->ccMaxUser1(ccUserDom.getCCMaxUser());
			leg->ccMaxInd1(ccUserDom.getCCMaxInd());
			
			std::ostringstream ossCcOuv;
			ossCcOuv << 'A' << ccUserDom.getLastOpenCCUser();
			leg->ccOuv1(ossCcOuv.str());
			leg->ccOuvInd1(ccUserDom.getLastOpenCCInd());
			leg->ccOuvUser1(ccUserDom.getLastOpenCCUser());
		}

		if (leg->get('B', ccUserDom))
		{
			std::ostringstream ossCcMax;
			ossCcMax << 'B' << ccUserDom.getCCMaxUser();
			leg->ccMax2(ossCcMax.str());
			leg->ccMaxUser2(ccUserDom.getCCMaxUser());
			leg->ccMaxInd2(ccUserDom.getCCMaxInd());

			std::ostringstream ossCcOuv;
			ossCcOuv << 'B' << ccUserDom.getLastOpenCCUser();
			leg->ccOuv2(ossCcOuv.str());
			leg->ccOuvInd2(ccUserDom.getLastOpenCCInd());
			leg->ccOuvUser2(ccUserDom.getLastOpenCCUser());
		}
	/*}
	else */
  if (szClient == SNCF_TRN)
	{
		/// CORAIL 
		if (leg->get('C', ccUserDom))
		{
			std::ostringstream ossCcMax;
			ossCcMax << 'C' << ccUserDom.getCCMaxUser();
			leg->ccMax1(ossCcMax.str());
			leg->ccMaxUser1(ccUserDom.getCCMaxUser());
			leg->ccMaxInd1(ccUserDom.getCCMaxInd());

			std::ostringstream ossCcOuv;
			ossCcOuv << 'C' << ccUserDom.getLastOpenCCUser();
			leg->ccOuv1(ossCcOuv.str());
			leg->ccOuvInd1(ccUserDom.getLastOpenCCInd());
			leg->ccOuvUser1(ccUserDom.getLastOpenCCUser());
		}

		if (leg->get('D', ccUserDom))
		{
			std::ostringstream ossCcMax;
			ossCcMax << 'D' << ccUserDom.getCCMaxUser();
			leg->ccMax2(ossCcMax.str());
			leg->ccMaxUser2(ccUserDom.getCCMaxUser());
			leg->ccMaxInd2(ccUserDom.getCCMaxInd());
			
			std::ostringstream ossCcOuv;
			ossCcOuv << 'D' << ccUserDom.getLastOpenCCUser();
			leg->ccOuv2(ossCcOuv.str());
			leg->ccOuvInd2(ccUserDom.getLastOpenCCInd());
			leg->ccOuvUser2(ccUserDom.getLastOpenCCUser());
		}

		if (leg->get('F', ccUserDom))
		{
			std::ostringstream ossCcMax;
			ossCcMax << 'F' << ccUserDom.getCCMaxUser();
			leg->ccMax2(ossCcMax.str());
			leg->ccMaxUser2(ccUserDom.getCCMaxUser());
			leg->ccMaxInd2(ccUserDom.getCCMaxInd());

			std::ostringstream ossCcOuv;
			ossCcOuv << 'F' << ccUserDom.getLastOpenCCUser();
			leg->ccOuv2(ossCcOuv.str());
			leg->ccOuvInd2(ccUserDom.getLastOpenCCInd());
			leg->ccOuvUser2(ccUserDom.getLastOpenCCUser());
		}

		if (leg->get('O', ccUserDom))
		{
			std::ostringstream ossCcMax;
			ossCcMax << 'O' << ccUserDom.getCCMaxUser();
			leg->ccMax2(ossCcMax.str());
			leg->ccMaxUser2(ccUserDom.getCCMaxUser());
			leg->ccMaxInd2(ccUserDom.getCCMaxInd());

			std::ostringstream ossCcOuv;
			ossCcOuv << 'O' << ccUserDom.getLastOpenCCUser();
			leg->ccOuv2(ossCcOuv.str());
			leg->ccOuvInd2(ccUserDom.getLastOpenCCInd());
			leg->ccOuvUser2(ccUserDom.getLastOpenCCUser());
		}
	}
	// NBN - ANO 74720		
	delete pSQL;
}

bool CCTADoc::recalculCCMaxCCOuv(long trancheNo, unsigned long dptDate, int jx, char cmpt)
{
	int queryLegs = IDS_SELECT_LEGS;
	int queryUpdateCcMaxCcOuv = IDS_UPDATE_CCMAX_CCOUV;
	bool bLegs = false;
	std::list<YmIcLegsDom> listLegs;

	YmIcLegsDom domLeg;
	domLeg.TrancheNo(trancheNo);
	domLeg.DptDate(dptDate);
	domLeg.J_X(jx);

	YM_Query pQueryLegs(*(((CCTAApp*)APP)->m_pDatabase), FALSE);
	pQueryLegs.SetDomain(&domLeg);
	RWDBReader ReaderLegs( ((CCTAApp*)APP)->m_pDatabase->Transact(&pQueryLegs, queryLegs));

	while(ReaderLegs())
	{
		YmIcLegsDom leg;
		ReaderLegs >> leg;
		listLegs.push_back(leg);
	}


	int iCcMax = 0;
	int iCcOuv = 0;
	int iCcMaxInd = 0;
	int iCcOuvInd = 0;

	std::list<int> listCcMax;
	std::list<int> listCcOuv;
	std::list<int> listCcMaxInd;
	std::list<int> listCcOuvInd;

	// YI-5006 
	// Les modalités d'évaluation de la CC max agrégée au niveau tranche / date / j-x / espace physique sont les suivantes :
	// • la valeur de la CC Max agrégée au niveau espace physique correspond à la plus petite des valeurs positives de "CC Max utilisateur" parmi celles calculées pour tous les tronçons de l'espace physique de la tranche / date.
	// • Si pour tous les tronçons de l'espace physique, les CC max utilisateurs valent -1 alors la valeur agrégée pour l'espace physique vaut également -1.
	// YI-5025
	// Les modalités d'évaluation de la dernière CC Ouverte agrégée utilisateur au niveau tranche / date / j-x / espace physique sont les suivantes :
	// • la valeur de la dernière CC Ouverte agrégée utilisateur au niveau espace physique correspond à la plus petite des valeurs positives de "dernière CC Ouverte utilisateur" (c'est à dire de bucket de taille la plus grande) parmi celles calculées pour tous les tronçons de l'espace physique de la tranche / date / j-x.
	// • Si pour tous les tronçons de l'espace physique, les dernières CC Ouvertes utilisateurs valent -1 alors la valeur agrégée pour l'espace physique vaut également -1
	// 

	if (listLegs.size() > 0)
	{
		YmIcLegsDom domLeg;
		int defaultCcMax = -1;
		int defaultCcOuv = -1;
		int defaultCcMaxInd = -1;
		int defaultCcOuvInd = -1;

		while(listLegs.size() > 0)
		{
			domLeg = listLegs.front();
			listLegs.pop_front();

			CString strLegOrig = domLeg.LegOrig();
			CString strLegDest = domLeg.LegDest();

			if (reclaculCCMaxCCOuvLegs(trancheNo, dptDate, jx, cmpt, strLegOrig, strLegDest, iCcMax, iCcOuv, iCcMaxInd, iCcMaxInd))
			{
				listCcMax.push_back(iCcMax);
				listCcOuv.push_back(iCcOuv);
				listCcMaxInd.push_back(iCcMaxInd);
				listCcOuvInd.push_back(iCcOuvInd);
			}

		}

		/// calcul du ccMax et ccOuv pour 
		while(listCcMax.size() > 0 && listCcOuv.size() > 0 && listCcMax.size() == listCcOuv.size())
		{
			int ccMax = listCcMax.front();
			int ccOuv = listCcOuv.front();
			int ccMaxInd = listCcMaxInd.front();
			int ccOuvInd = listCcOuvInd.front();

			if (ccMax > 0 && ((defaultCcMax == -1) || (defaultCcMax > ccMax)))
			{
				defaultCcMax = ccMax;
			}

			if (ccOuv > 0 && (defaultCcOuv == -1) || (defaultCcOuv > ccOuv))
			{
				defaultCcOuv = ccOuv;
			}

			if (ccOuvInd > 0 && (defaultCcOuvInd == -1) || (defaultCcOuvInd > ccOuvInd))
			{
				defaultCcOuvInd = ccOuvInd;
			}

			if (ccMaxInd > 0 && (defaultCcMaxInd == -1) || (defaultCcMaxInd > ccMaxInd))
			{
				defaultCcMaxInd = ccMaxInd;
			}

			/// 
			listCcMax.pop_front();
			listCcOuv.pop_front();
			listCcMaxInd.pop_front();
			listCcOuvInd.pop_front();
		}


		/// update ccmax pour ce tronçon
		YmIcUserCCDom dom;
		dom.setTrancheNo(trancheNo);
		dom.setDptDate(dptDate);
		dom.setJx(jx);
		dom.setCmpt(cmpt);

		YM_Query queryCcMaxCcOuv(*(((CCTAApp*)APP)->m_pDatabase), FALSE);
		dom.setCCMaxUser(defaultCcMax);
		dom.setLastOpenCCUser(defaultCcOuv);
		dom.setCCMaxInd(defaultCcMaxInd);
		dom.setLastOpenCCInd(defaultCcOuvInd);

		queryCcMaxCcOuv.SetDomain(&dom);

		RWDBReader reader(((CCTAApp*)APP)->m_pDatabase->Transact(&queryCcMaxCcOuv, queryUpdateCcMaxCcOuv));
	}
	else
	{
		std::ostringstream oss;
		oss << " impossible to find legs for Tranche " << trancheNo << " Dpt Date " << RWDate(dptDate).asString("%Y/%m/%d")  << " Jx " << jx << std::endl;  
		TRACE(oss.str().c_str());
		return false;
	}

	return true;
}


bool CCTADoc::reclaculCCMaxCCOuvLegs(long trancheNo, unsigned long dptDate, int jx, char cmpt, CString strLegOrig, CString strLegDest, int& ccMax, int& ccOuv, int& ccMaxInd, int& ccOuvInd)
{
	int querySelectCcMaxCcOuvLegs = IDS_SELECT_CCMAX_CCOUV_LEGS;
	int queryUpdateCcMaxCcOuvLegs = IDS_UPDATE_CCMAX_CCOUV_LEGS;
	std::list<YmIcUserCCDom> listCcDom;

	YmIcUserCCDom dom;
	dom.setDptDate(dptDate);
	dom.setJx(jx);
	dom.setTrancheNo(trancheNo);
	dom.setCmpt(cmpt);
	dom.setLegOrig((LPCSTR)strLegOrig);
	dom.setLegDest((LPCSTR)strLegDest);

	YM_Query pQueryCcMaxCcOuv(*(((CCTAApp*)APP)->m_pDatabase), FALSE);
	pQueryCcMaxCcOuv.SetDomain(&dom);
	RWDBReader Reader( ((CCTAApp*)APP)->m_pDatabase->Transact(&pQueryCcMaxCcOuv, querySelectCcMaxCcOuvLegs));

	while(Reader())
	{
		YmIcUserCCDom userDom;
		Reader >> userDom;
		listCcDom.push_back(userDom);
	}


	if (listCcDom.size() > 0)
	{
		listCcDom.sort(SortYmICUserCCDomNestLevel());

		//
		// YI-5005 
		// à déterminer quel est l'indice de la classe de contrôle le plus élevé pour lequel les niveaux d'autorisations positionnés correspondent à ceux de la classe de contrôle de niveau 0 (décontrainte)
		// - si le niveau d'autorisation de la CC0 est égal à 0 pour le tronçon considéré alors la valeur de CC Max utilisateur prend -1 pour le tronçon
		// - à enregistrer cette information dans la zone de stockage dédiée de la base de données (donnée CC Max utilisateur au niveau tranche / date / espace physique / tronçon / j-x)
		// YI-5006 
		// Les modalités d'évaluation de la CC max agrégée au niveau tranche / date / j-x / espace physique sont les suivantes :
		// • la valeur de la CC Max agrégée au niveau espace physique correspond à la plus petite des valeurs positives de "CC Max utilisateur" parmi celles calculées pour tous les tronçons de l'espace physique de la tranche / date.
		// • Si pour tous les tronçons de l'espace physique, les CC max utilisateurs valent -1 alors la valeur agrégée pour l'espace physique vaut également -1.
		// YI-5018 
		// Le traitement vise :
		// - à déterminer quel est l'indice de la classe de contrôle le plus élevé pour lequel les niveaux d'autorisations positionnés sont strictement supérieurs à 0.
		// - si les niveaux d'autorisation sont égaux à 0 pour toutes les CC du tronçon, alors la valeur de la dernière CC ouverte vaut -1.
		// - à enregistrer cette information dans la zone de stockage dédiée de la base de données (donnée dernière CC ouverte utilisateur au niveau tranche / j-x / date / espace physique / tronçon)
		// YI-5025
		// Les modalités d'évaluation de la dernière CC Ouverte agrégée utilisateur au niveau tranche / date / j-x / espace physique sont les suivantes :
		// • la valeur de la dernière CC Ouverte agrégée utilisateur au niveau espace physique correspond à la plus petite des valeurs positives de "dernière CC Ouverte utilisateur" (c'est à dire de bucket de taille la plus grande) parmi celles calculées pour tous les tronçons de l'espace physique de la tranche / date / j-x.
		// • Si pour tous les tronçons de l'espace physique, les dernières CC Ouvertes utilisateurs valent -1 alors la valeur agrégée pour l'espace physique vaut également -1
		// 

		std::ostringstream oss;
		YmIcUserCCDom domCc;

		// YI-5005 
		// à déterminer quel est l'indice de la classe de contrôle le plus élevé pour lequel les niveaux d'autorisations positionnés correspondent à ceux de la classe de contrôle de niveau 0 (décontrainte)
		// - si le niveau d'autorisation de la CC0 est égal à 0 pour le tronçon considéré alors la valeur de CC Max utilisateur prend -1 pour le tronçon
		// - à enregistrer cette information dans la zone de stockage dédiée de la base de données (donnée CC Max utilisateur au niveau tranche / date / espace physique / tronçon / j-x)
		// YI-5018 
		// Le traitement vise :
		// - à déterminer quel est l'indice de la classe de contrôle le plus élevé pour lequel les niveaux d'autorisations positionnés sont strictement supérieurs à 0.
		// - si les niveaux d'autorisation sont égaux à 0 pour toutes les CC du tronçon, alors la valeur de la dernière CC ouverte vaut -1.
		// - à enregistrer cette information dans la zone de stockage dédiée de la base de données (donnée dernière CC ouverte utilisateur au niveau tranche / j-x / date / espace physique / tronçon)

		domCc = listCcDom.front();
		if (domCc.getNestLevel() != 0)
		{
			std::ostringstream oss;
			oss << " problème pour récupérer la CC0 sur la tranche " << trancheNo << " pour la date de départ " << RWDate(dptDate).asString("%Y/%m/%d");
			oss << " jx " << jx << " Leg Origin " << strLegOrig << " Leg Dest " << strLegDest; 
			TRACE(oss.str().c_str());
			return false;
		}
		listCcDom.pop_front();
		
		int cc0AuthInd = domCc.getAuthInd();
		ccMax = -1;
		ccOuv = -1;
		ccOuvInd = -1;
		ccMaxInd = -1;

		if (cc0AuthInd != 0)
		{
			while(listCcDom.size() > 0)
			{
				domCc = listCcDom.front();
				listCcDom.pop_front();
				int authInd = domCc.getAuthInd();
				if (authInd == cc0AuthInd)
					ccMax = domCc.getNestLevel();

				if (authInd > 0)
					ccOuv = domCc.getNestLevel();

				/// on recupère les ccmax et ccouv collecté par trancon  
				int iCcMaxInd = domCc.getCCMaxInd();
				if (iCcMaxInd > 0 && ((ccMaxInd == -1) || ( iCcMaxInd < ccMaxInd)))
					ccMaxInd = iCcMaxInd;

				int iCcOuvInd = domCc.getLastOpenCCInd();
				if (iCcOuvInd > 0 && ((ccOuvInd == -1) || ( iCcOuvInd < ccOuvInd)))
					ccOuvInd = iCcOuvInd;
			}
		}

		/// update ccmax pour ce tronçon
		YM_Query queryCcMaxCcOuvLegs(*(((CCTAApp*)APP)->m_pDatabase), FALSE);
		dom.setCCMaxUser(ccMax);
		dom.setLastOpenCCUser(ccOuv);

		queryCcMaxCcOuvLegs.SetDomain(&dom);

		RWDBReader reader(((CCTAApp*)APP)->m_pDatabase->Transact(&queryCcMaxCcOuvLegs, queryUpdateCcMaxCcOuvLegs));
	}
	else
	{
		/// no data
		return false;
	}

	return true;
}


/// DM-7978 - CLE 

void CCTADoc::ReplaceSQL()
{
  ASSERT(((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms != NULL);

  // for TRN Client, adjust the SQL queries in the appropriate RecordSets ...
  if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTRNFlag())
  {
	m_pTgvGagSet->SetTransIdSelect(IDS_SQL_SELECT_CORAIL_GAG);
    m_pTgvStations->SetTransIdSelect(IDS_SQL_SELECT_ALL_STATIONS);
	m_pTraqueTarifSet->SetTransIdSelect(IDS_SQL_TRAQUE_TARIF_TRN);
	m_pRelTgvSet->AttachKey(LEG_KEY);
    m_pRelTgvAltSet->AttachKey(LEG_KEY);
  }
}

BOOL CCTADoc::RefDataValid() 
{
  return (
    (!((CCTAApp*)APP)->GetDocument()->RefDataError()) && 
    (((CCTAApp*)APP)->GetDocument()->RefDataComplete()) 
    ); 
}

BOOL CCTADoc::IsValidCmpt(char cmpt)
{
  YM_Iterator<YmIcCmptDom>* pIterator =
    YM_Set<YmIcCmptDom>::FromKey(CMPT_KEY)->CreateIterator();
  YmIcCmptDom* pCmpt;
  
  if (pIterator->GetCount() == 0)
  {
    CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA_IN_TABLE);
    szText += " SC_COMPARTMENTS";
    AfxMessageBox(szText, MB_ICONHAND | MB_APPLMODAL);
    delete pIterator;
    return FALSE;
  }
  
  for (pIterator->First(); !pIterator->Finished(); pIterator->Next())
  { 
    pCmpt = pIterator->Current();
    if (* (pCmpt->Cmpt()) == cmpt)
	{
	  delete pIterator;
      return TRUE;
	}
  }
  delete pIterator;
  return FALSE;
}

CString CCTADoc::GetStationDescription(CString szStationCode)
{
  CString szDescription;
  YM_Iterator<YmBaTgvStationDom>* pStaIterator = 
    YM_Set<YmBaTgvStationDom>::FromKey(STATION_KEY)->CreateIterator();
  for (pStaIterator->First(); !pStaIterator->Finished(); pStaIterator->Next()) 
  {
    YmBaTgvStationDom* pSta = pStaIterator->Current();
    if (pSta->StationCode() == szStationCode)
    {
      szDescription = pSta->Description();
      break;
    }
  }
  delete pStaIterator;
  return szDescription;
}

void CCTADoc::CreateRecordKeys()
{
  ////////////////
  // ENTITY Key  Owner is ENTITY, User is TRANCHE
  //
  m_pTrancheSet->AttachKey(ENTITY_KEY);
  m_pTDLJKSet->AttachKey(ENTITY_KEY);
  m_pTDPJESet->AttachKey(ENTITY_KEY);
  m_pHoiTraISet->AttachKey(ENTITY_KEY);
  m_pHoiTraRSet->AttachKey(ENTITY_KEY);
  m_pTrainSupplementSet->AttachKey(ENTITY_KEY);
  m_pTrainDeletedSet->AttachKey(ENTITY_KEY);
  m_pTrainAdaptationSet->AttachKey(ENTITY_KEY);
  
  ////////////////
  // TRANCHE Key  Owner is TRANCHE, Users are LEG and OTHER LEGS
  //
  m_pLegSet->AttachKey(TRANCHE_KEY);
  m_pOthLegsSet->AttachKey(TRANCHE_KEY);
  m_pOthLegsAltSet->AttachKey(TRANCHE_KEY);
  m_pVirtNestSet->AttachKey(TRANCHE_KEY);
  m_pQuelCmptSet->AttachKey(TRANCHE_KEY);
  m_pComptagesSet->AttachKey(TRANCHE_KEY);
  m_pComptagesHistSet->AttachKey(TRANCHE_KEY);
  //DM7978 - LME - YI-6046
	m_pAscTarifSet->AttachKey(TRANCHE_KEY);	
  
  ////////////////
  // LEG Key  Owner LEG, Users are all remaining windows
  //
  m_pResaBktSet->AttachKey(LEG_KEY);
  m_pResaCmptSet->AttachKey(LEG_KEY);
  m_pBktEvolSet->AttachKey(LEG_KEY);
  m_pBktEvolAltSet->AttachKey(LEG_KEY);
  m_pBktEvolAvgSet->AttachKey(LEG_KEY);
  m_pBktEvolAvgAltSet->AttachKey(LEG_KEY);
  m_pCmptEvolSet->AttachKey(LEG_KEY);
  m_pCmptEvolAltSet->AttachKey(LEG_KEY);
  m_pCmptEvolAvgSet->AttachKey(LEG_KEY);
  m_pCmptEvolAvgAltSet->AttachKey(LEG_KEY);
  m_pHistoEvolSet->AttachKey(LEG_KEY);
  m_pHistoEvolAltSet->AttachKey(LEG_KEY);
  m_pHistoHoiTraISet->AttachKey(LEG_KEY);
  m_pHistoHoiTraRSet->AttachKey(LEG_KEY);
  m_pHistoEvolCmptSet->AttachKey(LEG_KEY);
  m_pHistoEvolCmptAltSet->AttachKey(LEG_KEY);
  m_pBktHistSet->AttachKey(LEG_KEY);
  m_pBktHistAltSet->AttachKey(LEG_KEY);
  m_pBktHistoSet->AttachKey(LEG_KEY);
  m_pBktHistoAltSet->AttachKey(LEG_KEY);
  m_pHistDptSet->AttachKey(LEG_KEY);
  m_pHistDptAltSet->AttachKey(LEG_KEY);
  m_pRelTrancheSet->AttachKey(LEG_KEY);
  m_pRelTrancheAltSet->AttachKey(LEG_KEY);
  m_pVnauListSet->AttachKey(LEG_KEY);
  m_pPrevisionSet->AttachKey(LEG_KEY);
  m_pNomosVnauExclu->AttachKey(LEG_KEY);
  if (((CCTAApp*)APP)->m_Client == 'S')
  {
	m_pBktPrevisionVenteSet->AttachKey(TRANCHE_KEY);
	m_pBktPrevisionRevenuSet->AttachKey(TRANCHE_KEY);
  }
  //DM5350 revenus
  m_pBktEvolRevSet->AttachKey(LEG_KEY);
  m_pBktEvolRevAltSet->AttachKey(LEG_KEY);
  m_pBktEvolAvgRevSet->AttachKey(LEG_KEY);
  m_pBktEvolAvgRevAltSet->AttachKey(LEG_KEY);
  m_pCmptEvolRevSet->AttachKey(LEG_KEY);
  m_pCmptEvolRevAltSet->AttachKey(LEG_KEY);
  m_pCmptEvolAvgRevSet->AttachKey(LEG_KEY);
  m_pCmptEvolAvgRevAltSet->AttachKey(LEG_KEY);
  m_pHistoEvolRevSet->AttachKey(LEG_KEY);
  m_pHistoEvolRevAltSet->AttachKey(LEG_KEY);
  m_pHistoEvolRevCmptSet->AttachKey(LEG_KEY);
  m_pHistoEvolRevCmptAltSet->AttachKey(LEG_KEY);
  
  m_pHistDptYavSet->AttachKey(LEG_KEY);
  m_pHistDptYavAltSet->AttachKey(LEG_KEY);

  m_pTraqueTarifSet->AttachKey(LEG_KEY);

	//DM7978 - LME - CCmax CCouv et j-x histo
	//m_pHistoDataRRDSet->AttachKey(LEG_KEY);
}

BOOL CCTADoc::OnNewDocument()
{
  if (!CDocument::OnNewDocument())
    return FALSE;
  
  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)
  
  App().m_pDatabase->AddObserver(this);
  App().m_pDatabase->Open();

  if(((CCTAApp*)APP)->GetRhealysFlag()){
	App().m_pDatabaseAlleo->AddObserver(this);
	App().m_pDatabaseAlleo->Open(true, true);
  }
  
  m_pSysParmSet->AddObserver(this);
  m_pEntitySet->AddObserver(this);
  m_pEntityGagSet->AddObserver(this);
  
  m_pDCDateSet->AddObserver(this);
  m_pSysDateSet->AddObserver(this);
  m_pScStatusSet->AddObserver(this);
  m_pHistoStatusSet->AddObserver(this);
  m_pBrmsDateSet->AddObserver(this);
  m_pCmptSet->AddObserver(this);
  m_pMaxEntityRrdSet->AddObserver(this);
  m_pDefRrdSet->AddObserver(this);
  m_pFullJXSet->AddObserver(this);
  m_pStationNotGrp->AddObserver(this);
  m_pUserInfoSet->AddObserver(this);
  m_pOrderSVCSet->AddObserver(this);
  m_pSeasonsSet->AddObserver(this);
  m_pCongesSet->AddObserver(this);
  m_pMinMaxDptDate->AddObserver(this);


  // Create and Add Record Sets to the Reference Data Control Object (Launch is in OnNotify())
  m_pRefDataCtl = new YM_RefDataCtl;
#ifndef MOTEURVNL_APPLICATION
  m_pRefDataCtl->AddRecordSet(m_pDCDateSet, 0, "DCDate", FALSE, FALSE);
  m_pRefDataCtl->AddRecordSet(m_pCmptSet, 0, "Cmpt", FALSE, FALSE);
  m_pRefDataCtl->AddRecordSet(m_pMaxEntityRrdSet, 0, "MaxEntityRrd", TRUE, TRUE);
  m_pRefDataCtl->AddRecordSet(m_pDefRrdSet, 0, "DefRrdList", FALSE, FALSE);
  m_pRefDataCtl->AddRecordSet(m_pFullJXSet, 0, "FullJXList", FALSE, FALSE);
  m_pRefDataCtl->AddRecordSet(m_pStationNotGrp, 0, "Stations", TRUE, FALSE); 
#endif
  m_pRefDataCtl->AddRecordSet(m_pSysParmSet, 0, "SysParms", FALSE, FALSE);

#ifndef MOTEURVNL_APPLICATION
  m_pRefDataCtl->AddRecordSet(m_pSysDateSet, 1, "SysDate", FALSE, FALSE);
  m_pRefDataCtl->AddRecordSet(m_pScStatusSet, 1, "ScStatus", FALSE, FALSE);
  m_pRefDataCtl->AddRecordSet(m_pBrmsDateSet, 1, "BRMS date", TRUE, FALSE);
  m_pRefDataCtl->AddRecordSet(m_pCongesSet, 1, "Conges", TRUE, TRUE);
  m_pRefDataCtl->AddRecordSet(m_pSeasonsSet, 1, "Seasons", FALSE, FALSE); 
#endif
  m_pRefDataCtl->AddRecordSet(m_pEntitySet, 1, "Entity", FALSE, FALSE);
  m_pRefDataCtl->AddRecordSet(m_pOrderSVCSet, 1, "SVC", TRUE, FALSE);
  m_pRefDataCtl->AddRecordSet(m_pMinMaxDptDate, 1, "MinMax DptDate", FALSE, FALSE);
  
#ifndef MOTEURVNL_APPLICATION
  m_pRefDataCtl->AddRecordSet(m_pUserInfoSet, 2, "UserInfo", FALSE, FALSE);
  m_pRefDataCtl->AddRecordSet(m_pTopSet, 2, "TOP", TRUE, TRUE);
  m_pRefDataCtl->AddRecordSet(m_pSensibiliteSet, 2, "SENSIBILITE", TRUE, TRUE);
#endif
  //m_pRefDataCtl->AddRecordSet(m_pMinMaxDptDate, 2, "MinMax DptDate", FALSE, FALSE);

// NPI - Ano 75229
#ifndef MOTEURVNL_APPLICATION
///
  // DM7790
  m_pAxeSet->AddObserver(this);
  m_pRefDataCtl->AddRecordSet(m_pAxeSet, 1, "Axe", TRUE, FALSE);
#endif

  m_pRefDataCtl->SetReadingDataText(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_READING_REFERENCE));
  m_pRefDataCtl->SetNoDataText(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_RESULT_ON));
  
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCTADoc serialization

void CCTADoc::Serialize(CArchive& ar)
{
  if (ar.IsStoring())
  {
    // TODO: add storing code here
  }
  else
  {
    // TODO: add loading code here
  }
}

/////////////////////////////////////////////////////////////////////////////
// CCTADoc diagnostics

#ifdef _DEBUG
void CCTADoc::AssertValid() const
{
  CDocument::AssertValid();
}

void CCTADoc::Dump(CDumpContext& dc) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCTADoc miscellaneous

// Return TRUE if view is already created, FALSE if not
BOOL CCTADoc::IsViewCreated(const CRuntimeClass* cRuntimeClass)
{
  POSITION pos = GetFirstViewPosition();
  CView* pView = NULL; 
  while (pos != NULL) 
  {
    pView = GetNextView(pos); 
    if (pView->IsKindOf(cRuntimeClass)) 
      return TRUE;  // View already exists, return TRUE
  }
  return FALSE;
}

// Return TRUE if the batch is ended
BOOL CCTADoc::TestBatchFinished()
{ 
  if (!m_pScStatusSet->IsValid() || !m_pHistoStatusSet->IsValid() || !m_pBrmsDateSet->IsValid())
    return TRUE;

  if (!USERALLOWED(((CCTAApp*)APP)->m_UserRole) )
    return TRUE;

  RWDate completeBatch, currentBatch, purgeOpe, prixGaranti, histo, currentMoyenne, lastMoyenne, lastBrmsDate;
  RWDate today;
  YM_Iterator<YmIcSysParmsDom>* pSysParmsDomIterator =
    YM_Set<YmIcSysParmsDom>::FromKey(SCSTATUS_KEY)->CreateIterator();
  YmIcSysParmsDom* pScStep;
  
  for (pSysParmsDomIterator->First(); !pSysParmsDomIterator->Finished(); pSysParmsDomIterator->Next())
  {
    pScStep = (YmIcSysParmsDom*)pSysParmsDomIterator->Current();
	if (!strcmp (pScStep->SysParmName(), "COMPLETE_DATE_PNIIND"))
	  completeBatch.julian (pScStep->EditDt());
	if (!strcmp (pScStep->SysParmName(), "CURRENT_DATE_PNIIND"))
	  currentBatch.julian (pScStep->EditDt());
	if (!strcmp (pScStep->SysParmName(), "LAST_PURGE_DATE"))
	  purgeOpe.julian (pScStep->EditDt());
	if (!strcmp (pScStep->SysParmName(), "LAST_REV_GARANTI_DATE"))
	  prixGaranti.julian (pScStep->EditDt());
  }
  delete pSysParmsDomIterator;

  pSysParmsDomIterator = YM_Set<YmIcSysParmsDom>::FromKey(HISTOSTATUS_KEY)->CreateIterator();
  for (pSysParmsDomIterator->First(); !pSysParmsDomIterator->Finished(); pSysParmsDomIterator->Next())
  {
    pScStep = (YmIcSysParmsDom*)pSysParmsDomIterator->Current();
	if (!strcmp (pScStep->SysParmName(), "LAST_HISTO_DATE"))
	  histo.julian (pScStep->EditDt());
	if (!strcmp (pScStep->SysParmName(), "LAST_MOYENNES_DATE"))
	  lastMoyenne.julian (pScStep->EditDt());
	if (!strcmp (pScStep->SysParmName(), "CURRENT_MOYENNES_DATE"))
	  currentMoyenne.julian (pScStep->EditDt());
  }
  delete pSysParmsDomIterator;

  CString mess, szText, szDate;
  if (completeBatch != currentBatch)
  {
    szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RRD_NOT_FINISH);
	mess += szText;
  }
  if (currentBatch < today -1)
  {
    szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BATCH_LATE);
	szDate = currentBatch.asString("%d/%m/%Y");
	szText += szDate;
	if (!mess.IsEmpty())
	  mess += '\n';
	mess += szText;  
  }
  if (prixGaranti < completeBatch)
  {
    szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_PRIX_GARANTI_LATE);
	szDate = prixGaranti.asString("%d/%m/%Y");
	szText += szDate;
	if (!mess.IsEmpty())
	  mess += '\n';
	mess += szText;  
  }
  if (histo < currentBatch -1)
  {
    szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_HISTO_LATE);
	szDate = histo.asString("%d/%m/%Y");
	szText += szDate;
	if (!mess.IsEmpty())
	  mess += '\n';
	mess += szText;  
  }
  if (currentMoyenne != lastMoyenne)
  {
    szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BATCH_LATE);
	if (!mess.IsEmpty())
	  mess += '\n';
	mess += szText;  
  }
  if (currentMoyenne < histo)
  {
    szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_MOYENNE_LATE);
	szDate = currentMoyenne.asString("%d/%m/%Y");
	szText += szDate;
	if (!mess.IsEmpty())
	  mess += '\n';
	mess += szText;  
  }
  if (purgeOpe < completeBatch - 7)
  {
    szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_PURGE_LATE);
	szDate = purgeOpe.asString("%d/%m/%Y");
	szText += szDate;
	if (!mess.IsEmpty())
	  mess += '\n';
	mess += szText;  
  }

  YM_Iterator<YmIcVnauDom>* pBRMSDateDomIterator =
    YM_Set<YmIcVnauDom>::FromKey(BRMS_KEY)->CreateIterator();
  YmIcVnauDom* pBrmsDate;
  pBRMSDateDomIterator->First();
  // Il n'y a qu'une ligne en retour de la requête
  pBrmsDate = (YmIcVnauDom*)pBRMSDateDomIterator->Current();
  lastBrmsDate.julian(pBrmsDate->DptDate());
  if (lastBrmsDate < today +30)
	szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_YIELD_INTERDIT);
  else if (lastBrmsDate < today +60) 
    szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_YIELD_MAX_30);
  else
	szText = "";
  if (szText.GetLength())
  {
	CString sss;
	if (pBrmsDate->DptDate())
	{
      sss = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_LAST_BRMS_DATE);
	  szDate = lastBrmsDate.asString("%d/%m/%Y");
	  sss += " ";
	  sss += szDate;
	}
	else
	  sss = "Les règles critiques et commerciales n'ont pas tournées";
	szText += '\n';
	szText += sss;
	

	if (!mess.IsEmpty())
	  mess += '\n';
	mess += szText;  
  }
  delete pBRMSDateDomIterator; // NPI - Correction Memory Leak
 

  if (mess.GetLength())
  	m_bBatchFinished = FALSE;

  if (!m_bBatchFinished  && mess.IsEmpty())
  {
	CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BATCH_FINISHED);
	AfxMessageBox(szText);
	m_bBatchFinished = TRUE;
  }
  else if (!m_bFirstTestBatchFinished || mess.GetLength())
  { // affichage recap
    if (!mess.IsEmpty())
	  mess += "\n\n";
	mess += "RECAP:\n";
	mess += "Batch en cours : ";
	szDate = currentBatch.asString("%d/%m/%Y");
	mess += szDate;
	mess += '\n';
	mess += "Batch terminé : ";
	szDate = completeBatch.asString("%d/%m/%Y");
	mess += szDate;
	mess += '\n';
	mess += "Prix garantis : ";
	szDate = prixGaranti.asString("%d/%m/%Y");
	mess += szDate;
	mess += '\n';
	mess += "Purge : ";
	szDate = purgeOpe.asString("%d/%m/%Y");
	mess += szDate;
	mess += '\n';
	mess += "Historisation : ";
	szDate = histo.asString("%d/%m/%Y");
	mess += szDate;
	mess += '\n';
	mess += "Moyenne en cours : ";
	szDate = currentMoyenne.asString("%d/%m/%Y");
	mess += szDate;
	mess += '\n';
	mess += "Moyenne terminée : ";
	szDate = lastMoyenne.asString("%d/%m/%Y");
	mess += szDate;
	AfxMessageBox(mess, MB_ICONEXCLAMATION | MB_APPLMODAL);
  } 
  m_bFirstTestBatchFinished = FALSE;
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CheckSysTime - Validate the system time and 
//    calculate difference between PC and Database time
BOOL CCTADoc::CheckSysTime()
{
  // Ensure that we have a System Date
  YmIcSysDateDom* pSysDate = YM_Set<YmIcSysDateDom>::FromKey(SYSDATE_KEY);
  YM_Iterator<YmIcSysDateDom>* pSysDateIterator = 
    YM_Set<YmIcSysDateDom>::FromKey(SYSDATE_KEY)->CreateIterator();
  if (pSysDateIterator->GetCount() == 0)  // Do we have data in the table ?
  {
    CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_SYSTEM_DATE);
    AfxMessageBox(szText);
    m_bRefDataError = TRUE;  // error
	delete pSysDateIterator;
    return FALSE;
  }
  else  // calculate the difference between PC and Database Time
  {
    RWTime now; // heure actuelle sur le PC number of sec since 1/1/1901
    RWDate today; // date actuelle sur le PC
    unsigned long tpc = now.seconds();
    CString Oratime = YM_Set<YmIcSysDateDom>::FromKey(SYSDATE_KEY)->CurrDtTm();
    int sys_year = atoi(Oratime.Mid(0, 4));
    int sys_month = atoi(Oratime.Mid(5, 2));
    int sys_day = atoi(Oratime.Mid(8, 2));
    int sys_hour = atoi(Oratime.Mid(11, 2));
    int sys_min = atoi(Oratime.Mid(14, 2));
    int sys_sec = atoi(Oratime.Mid(17, 2));
    int pc_year = today.year();
    int pc_month = today.month();
    int pc_day = today.dayOfMonth();
    int pc_hour = now.hour();
    int pc_min = now.minute();
    int pc_sec = now.second();
    
    if ((sys_year != pc_year) || (sys_month != pc_month) || (sys_day != pc_day))
    {
      CString szText;
      szText.Format("%s\nPC = %d-%d-%d, %d:%d:%d\nDB = %d-%d-%d, %d:%d:%d",
        ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_PC_BAD_DATE),
        pc_year, pc_month, pc_day, pc_hour, pc_min, pc_sec,
        sys_year, sys_month, sys_day, sys_hour, sys_min, sys_sec);
      AfxMessageBox(szText);
      m_bRefDataError = TRUE;
	  delete pSysDateIterator; // NPI - Correction Memory Leak
      return FALSE;
    }
    long lDiffTime = (long)(sys_hour - pc_hour) * 3600 +
      (long)(sys_min - pc_min) * 60 + (long)(sys_sec - pc_sec);  
    
    SetDiffSysDate(lDiffTime);
  }
  delete pSysDateIterator; // NPI - Correction Memory Leak
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SetUserRole - Set the user's CTA role from the SC_User table
//
BOOL CCTADoc::SetUserRole()
{
  // Set the Role of the User (Tactical, Critical, ...)
  YM_Iterator<CmOpUserDom>* pUserDomIterator =
    YM_Set<CmOpUserDom>::FromKey(USERINFO_KEY)->CreateIterator();
  
  pUserDomIterator->First();
  CmOpUserDom* pUserInfo = pUserDomIterator->Current();
  ((CCTAApp*)APP)->m_UserRole = pUserInfo->Role();
  delete pUserDomIterator;
  
  // This is where we tell the database to filter certain messages depending upon type of user
  // if we are in development mode, let's not report certain Oracle error messages
  if (DEVELOPMENT(((CCTAApp*)APP)->m_UserRole))
  {
    App().m_pDatabase->AddMsgIgnore(1031);  // Oracle msg no for "Insufficient Privilege"
	if (App().m_pDatabaseAlleo != NULL)
		App().m_pDatabaseAlleo->AddMsgIgnore(1031);
  }
  
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BldRrdArray - Build rrd list
//
BOOL CCTADoc::BldRrdArray()
{
  // Build the list of reading days
  YmIcRdDaysDom* pRrd;
  int iMaxRrdIndex = 0;
  CString sRrdList;
  CString sFullJXList;

  YM_Iterator<YmIcRdDaysDom>* pRrdIterator =
    YM_Set<YmIcRdDaysDom>::FromKey(MAX_ENTITY_RRD_KEY)->CreateIterator();
  
  if (pRrdIterator->GetCount())  // Do we have data in the table
  { // une seule ligne revient
    for (pRrdIterator->First(); !pRrdIterator->Finished(); pRrdIterator->Next())
    {
      pRrd = (YmIcRdDaysDom*)pRrdIterator->Current();
      iMaxRrdIndex = __max(iMaxRrdIndex, pRrd->RrdIndex());
    }
  }
  delete pRrdIterator;

  CString ss;
  pRrdIterator = YM_Set<YmIcRdDaysDom>::FromKey(DEF_RRD_KEY)->CreateIterator();
  int nj = 0;
  if (pRrdIterator->GetCount())  // Do we have data in the table
  { 
    for (pRrdIterator->First(); !pRrdIterator->Finished(); pRrdIterator->Next())
    {
      pRrd = (YmIcRdDaysDom*)pRrdIterator->Current();
      iMaxRrdIndex = __max(iMaxRrdIndex, pRrd->RrdIndex());
	  if (!sRrdList.IsEmpty())
	    sRrdList += "','";
	  ss.Format ("%d", pRrd->RrdIndex());
	  sRrdList += ss;
	  if (pRrd->RrdIndex() >= 1)
		nj++;
	  if (nj > MAXNBJMX+1)
	  {
	    CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TOO_MUCH_DEF_RRD);
		CString szMess;
		szMess.Format (szText, MAXNBJMX, sRrdList);
	    AfxMessageBox (szMess);
        break;
	  }
    }
  }
  delete pRrdIterator;
  if (sRrdList.IsEmpty())
    return FALSE;
  SetMaxRrdIndex( iMaxRrdIndex );
  m_szDefaultRrdList = sRrdList;

  
  pRrdIterator = YM_Set<YmIcRdDaysDom>::FromKey(FULL_JX_KEY)->CreateIterator();
  nj = 0;
  if (pRrdIterator->GetCount())  // Do we have data in the table
  { 
    for (pRrdIterator->First(); !pRrdIterator->Finished(); pRrdIterator->Next())
    {
      pRrd = (YmIcRdDaysDom*)pRrdIterator->Current();
	  ss.Format (",%d", pRrd->RrdIndex());
	  sFullJXList += ss;
    }
  }
  delete pRrdIterator;
  if (sFullJXList.IsEmpty())
    return FALSE;
  sFullJXList += ",";
  m_szFullCollecteJX = sFullJXList;

  return TRUE;
}

void CCTADoc::SetNotFullJX(int jx)
{
  CString ss;
  ss.Format (",%d,", jx);
  if (m_szFullCollecteJX.Find(ss) >= 0)
    m_bNotFullJX = FALSE;
  else
    m_bNotFullJX = TRUE;
}

void CCTADoc::LoadCurEntityRrd(CString szCurrentEntity)
{
  if (szCurrentEntity == m_szCurEntity)
	return; // les données en mémoire sont bonnes. pas la peine de relire.

  YmIcRdDaysDom RrdDom;
  CString sRrdList;
  CString ss;
  RrdDom.UserId(szCurrentEntity); // magouille, j'utilise le user_id pour passer le nom de l'entité
  YM_Query* pSQL = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
  pSQL->SetDomain(&RrdDom);
  int nj = 0;
  RWDBReader Reader( ((CCTAApp*)APP)->m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_ENTITY_RRD) );
  while (Reader())
  {
    if (Reader.isValid())
	{
      Reader >> RrdDom;
	  if (!sRrdList.IsEmpty())
	    sRrdList += "','";
	  ss.Format ("%d", RrdDom.RrdIndex());
	  sRrdList += ss;
	  if (RrdDom.RrdIndex() >= 1)
		nj++;
	  if (nj > MAXNBJMX+1)
	  {
	    CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TOO_MUCH_ENTITY_RRD);
		CString szMess;
		szMess.Format (szText, MAXNBJMX, szCurrentEntity, sRrdList);
	    AfxMessageBox (szMess);
        break;
	  }
	}
  }
  delete pSQL;
  m_szCurEntity = szCurrentEntity;
  if (sRrdList.IsEmpty())
    m_szEntityRrdList = m_szDefaultRrdList;
  else
    m_szEntityRrdList = sRrdList;
}

BOOL CCTADoc::BldTgvStations()
{

  // Build the list of TGV Stations
  YM_Iterator<YmBaTgvStationDom>* pStaIterator =
    YM_Set<YmBaTgvStationDom>::FromKey(TGVSTATIONS_KEY)->CreateIterator();
  if (pStaIterator->GetCount() == 0)  // Do we have data in the table ?
  {
    CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA_IN_TABLE);
    szText += " SC_STATIONS, TGV";
    AfxMessageBox(szText, MB_ICONHAND | MB_APPLMODAL);
    delete pStaIterator;
	if (!((CCTAApp*)APP)->m_bDvltMode)
	{
		m_bRefDataError = TRUE;
	}
    return FALSE;
  }
  YmBaTgvStationDom* pSta;
  for (pStaIterator->First(); !pStaIterator->Finished(); pStaIterator->Next())
  {
    pSta = (YmBaTgvStationDom*)pStaIterator->Current();
    ASSERT(pSta != NULL);
    m_TgvStationMap.SetAt(pSta->StationCode(), (CObject*)pSta);
  }
  delete pStaIterator;
  ((CMainFrame*)((CCTAApp*)APP)->GetMainWnd())->GetStatusBar()->SetPaneOneAndLock(
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DONE));
  return TRUE;
}

BOOL CCTADoc::IsTgvStation(LPCSTR stationname)
{
  YmBaTgvStationDom* pSta;
  return m_TgvStationMap.Lookup(stationname, (CObject*&)pSta);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CheckBatchComplete - OBSOLETE
//
BOOL CCTADoc::CheckBatchComplete()
{
  BOOL bIsBatchFini = TestBatchFinished();
  if (!bIsBatchFini)
    return FALSE;
  
  // Check of the DC_DATE versus today.
  YmIcDcDateDom* pDCDate = YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY);
  RWDate today; // date actuelle sur le PC
  RWDate dcdate(pDCDate->DcDate());  // collection date.
  int diffdate = today.julian() - dcdate.julian();
  if (diffdate >= 2)  
  {
    CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TEST_OR_DOUBLE);
    AfxMessageBox(szText, MB_ICONEXCLAMATION | MB_APPLMODAL);
  }
  return TRUE;
}

void CCTADoc::PostDatabaseInitialization()
{
  // perform any initialization immediately after login to database, and prior to launch any queries
  
  ///////////////
  // Registry manipulation after reading Client variable from Database
  //
  CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
  CMainFrame* MainFrame = (CMainFrame*)(WinApp->GetMainWnd());
  CString szKey = WinApp->GetResource()->LoadResString(IDS_YMREGISTRY_RAILREV_PROJECT_KEY);
  CString szClient = WinApp->m_pDatabase->GetClientParameter();
  
  szKey += "_";
#ifdef MOTEURVNL_APPLICATION  // We must re-compile the application with the Compiler Directive MOTEURVNL for this client
  szKey += MOTEURVNL;
#else
  szKey += szClient;
  // Si SNCF, on ajoute la lecture des entites GAG
  if ( (szClient == SNCF) || (szClient == SNCF_TRN) )
    m_pRefDataCtl->AddRecordSet(m_pEntityGagSet, 0, "Entity GAG", TRUE, TRUE);
#endif

  // AttachKey sur les requetes uniquement SCI doit être fait après connaissance du client
  m_pResaBktSciSet->AttachKey(LEG_KEY);
  m_pResaCmptSciSet->AttachKey(LEG_KEY);
  

  WinApp->SetRegistryKey( LPCSTR(szKey) );
  WinApp->m_pszProfileName = _tcsdup(_T("CTA"));

	// Restore values from registry for application
  WinApp->RestoreAppConfig();
  ReloadScreenFonts();
  MainFrame->SetFirstTimeFlag(TRUE);  // force it
  MainFrame->ShowWindow(WinApp->m_nCmdShow);

  // Set text in status bar control
  MainFrame->GetStatusBar()->SetPaneOneAndLock(
    WinApp->GetResource()->LoadResString(IDS_READING_REFERENCE));
  
  // Also display the status box
  APP->GetStatusBox()->Show();
  APP->GetStatusBox()->SetText(IDS_READING_REFERENCE);
  
  // Add the Database SID to the Mainframe title...
  CString szFrameTitle;
  MainFrame->GetWindowText(szFrameTitle);
  szFrameTitle += " - ";
  szFrameTitle += App().m_pDatabase->UserName();
  szFrameTitle += "@";
  szFrameTitle += App().m_pDatabase->HostString();
  MainFrame->SetWindowText(szFrameTitle);
      
  m_pGlobalSysParms = new CCTAGlobalSysParms;  // create the object containing global system parameters
}

BOOL CCTADoc::FinalDocumentInitialization()
{
  // Validate that all reference data record sets have data
  if (m_pRefDataCtl->ValidateReferenceData() == YM_RefDataCtl::eFailure)
  {
    m_bRefDataError = TRUE;  // report the error
    return FALSE;  // return on error
  }
  BOOL bVerifVnau = FALSE;

  if (! m_bRefDataDone)  // do this only once (i.e., when the reference data has been read)
  {
    m_bRefDataDone = TRUE;  // set the boolean so we don't get here again
    bVerifVnau = TRUE;

    m_pGlobalSysParms->InitGlobalSysParms();  // initialize global system parameters
	ReplaceSQL();
  
    // initialte the communications to ResaRail
    App().InitiateComm(m_pGlobalSysParms->GetHostName(), (int)(m_pGlobalSysParms->GetPortNumber()));
    
#ifndef MOTEURVNL_APPLICATION   
    if (! CheckSysTime())  // compare PC time against Database time
      return FALSE;
    if (! SetUserRole())  // set the user role from the SC_User table
      return FALSE;
	if (! BldRrdArray())  // build the array of relative reading days
      return FALSE;
    if (! TestBatchFinished())  // determine the completion status of the batch processing
      return FALSE;
    //  if (! BldTgvStations())  // build the list of TGV Stations
	//	return FALSE;
#endif
    
    // Create TGV Jumeaux and Train Influential dialogs
    if (m_pGlobalSysParms->GetTGVJumeauxFlag())
      m_pDlgTgvJumeaux = new CTgvJumDlg();
    if (m_pGlobalSysParms->GetInfluentFlag())
      m_pTrainInfluentDlg = new CTrainInfluentDlg();
    
	  
    YM_Iterator<YmIcSysParmsDom>* pIterator = 
      YM_Set<YmIcSysParmsDom>::FromKey(SYSPARM_KEY)->CreateIterator();
    YmIcSysParmsDom* pSysParms = NULL;
    if (pIterator->GetCount() != 0)  // Do we have data in the table ?
    {
      for (pIterator->First(); !pIterator->Finished(); pIterator->Next()) 
      {
        pSysParms = pIterator->Current();
        if (!strcmp(pSysParms->SysParmName(), "UI_BUCKET_NOMOD"))
          m_aszBktNoMod.Add(pSysParms->CharValue());
      }
    }
	delete pIterator;
#ifndef MOTEURVNL_APPLICATION  
    // for Comptages client (TRN), clean up the table SC_TRAIN_DATE_COMPTAGES
    if ( m_pGlobalSysParms && m_pGlobalSysParms->GetComptagesFlag() )
    {
      ((CMainFrame*)((CCTAApp*)APP)->GetMainWnd())->GetStatusBar()->SetPaneOneAndLock(
        ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_PURGE_TRAIN_DATE_COMPTAGES_TEXT));

      // purge all rows from SC_TRAIN_DATE_COMPTAGES table with dpt date less than DC (Collection) Date
      YmIcComptagesDom ComptagesRecord;
      ComptagesRecord.DptDate(YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY)->DcDate());
      YM_Query* pSQL = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
      pSQL->SetDomain(&ComptagesRecord);
      ((CCTAApp*)APP)->m_pDatabase->Transact(pSQL, IDS_SQL_PURGE_TRAIN_DATE_COMPTAGES);
      delete pSQL;
    }
#endif

  
    ((CMainFrame*)((CCTAApp*)APP)->GetMainWnd())->GetStatusBar()->SetPaneOneAndLock(
      ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DONE));
    
    APP->GetStatusBox()->Hide();

	// HRI : SATC 7.4.8 pour possible résolution anomalies
	// 47991 	SATC 7.4.7: Sauvegarde non réalisée de la fenêtre entité sur nouvelle configuration
	// 47803 	SATC 7.4.7: La sauvegarde des configurations est partielle.

	// Set current profile key
	CString szRegistryKey = APP->GetPersistence()->GetCurrentConfiguration();

	if (szRegistryKey.IsEmpty()) 
	{
		AfxMessageBox("Vous arrivez sur l'application SATC pour la première fois.\n"
					"Veuillez d'entrée saisir votre répertoire de sauvegarde de la configuration.");

		// Appel du choix de la clé de sauvegarde de registre
		APP->GetPersistence()->ConfigurationDialog();
	}
    // Fin modif HRI
#ifndef MOTEURVNL_APPLICATION  // We must re-compile the application with the Compiler Directive MOTEURVNL for this client

    CEntityDlg dlg(this);  // Entity Dialog
    dlg.DoModal();
	((CCTAApp*)APP)->VerifyResaven(); 
#else
    App().NotCloseOpenPersistentChildWindows();
    m_bFullInitCompleted = TRUE;
#endif
	
  }

  if (!m_bDbLinkCreated)
  {
#ifndef MOTEURVNL_APPLICATION
    // mise en place des DBlink pour préparer DM 6609
    CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
    CString szClient = WinApp->m_pDatabase->GetClientParameter();
    CString szHistoselect = m_pGlobalSysParms->GetHistoSelect();
	if (szHistoselect.GetLength())
		((CCTAApp*)APP)->m_pDatabase->AddDbLink("HISTOSELECT", szHistoselect, ((CCTAApp*)APP)->GetNbConnHisto());
//DM7978 - KGH - Sphere de prix
	 CString szPyselect = m_pGlobalSysParms->GetPySelect();
    if (szPyselect.GetLength())
      ((CCTAApp*)APP)->m_pDatabase->AddDbLink("PYSELECT", szPyselect);
//Ano 110487
	CString szCorailMdp = m_pGlobalSysParms->GetCorailMdp();
	if (szCorailMdp.GetLength())
      ((CCTAApp*)APP)->m_pDatabase->AddDbLink("CORAILMDP", szCorailMdp);
	CString szCorailHistMdp = m_pGlobalSysParms->GetCorailHistMdp();
	if (szCorailHistMdp.GetLength() && (szClient == SNCF))
      ((CCTAApp*)APP)->m_pDatabase->AddDbLink("CORAILHISTMDP", szCorailHistMdp);
//DM7978 - KGH - FIN
    CString szTgvselect = m_pGlobalSysParms->GetTgvSelect();
    if (szTgvselect.GetLength() && (szClient == SNCF_TRN))
      ((CCTAApp*)APP)->m_pDatabase->AddDbLink("TGVSELECT", szTgvselect, 2);
	  // que 2 requêtes avec TGVSELECT, pas la peine de plus de deux connexions.
	CString szTgvHistselect = m_pGlobalSysParms->GetTgvHistSelect();
    if (szTgvHistselect.GetLength() && (szClient == SNCF_TRN))
      ((CCTAApp*)APP)->m_pDatabase->AddDbLink("TGVHISTSELECT", szTgvHistselect, 1);
	// que 1 requête avec TGVHISTSELECT, pas la peine de plus de une connexions.
#endif
	m_bDbLinkCreated = TRUE;
	((CCTAApp*)APP)->m_pDatabase->CtrlChangePwd();

#ifndef MOTEURVNL_APPLICATION
	m_pHistoStatusSet->Close(); // Il faut attendre création de l'histoselect pour lire cette info.
    m_pHistoStatusSet->Open();
#endif
  }

  if (m_pEntitySet->GetCurrent() && !m_bFullInitCompleted)
  {
    // everything's complete and looks good, let's go...
    App().NotCloseOpenPersistentChildWindows();
    m_bFullInitCompleted = TRUE;
	((CCTAApp*)APP)->AllEntityRhealys();
	((CCTAApp*)APP)->AllEntityAutoTrain();
  }

  if (((CCTAApp*)APP)->GetRhealysFlag() && bVerifVnau)
	((CCTAApp*)APP)->VerifUnsentVnau(false);

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCTADoc commands
//
void CCTADoc::OnNotify(YM_Observable* pObject)
{
  // Ignore any further notifications if there was an error reading reference data
  if (m_bRefDataError)
    return;
  
  if (pObject->IsKindOf(RUNTIME_CLASS(YM_Database)))
  {
	if (((YM_Database*)pObject)->bSecondary)
		return;
    if (((YM_Database*)pObject)->IsOpen())
    {
      PostDatabaseInitialization();
      
      // Launch the initial stage of the Reference Data Control Object
      CString szStatusMsg = m_pRefDataCtl->LaunchStage();
      ((CMainFrame*)((CCTAApp*)APP)->GetMainWnd())->GetStatusBar()->SetPaneOneAndLock(szStatusMsg);
    }
  }
  else 
  {
    if (pObject->IsKindOf(RUNTIME_CLASS(YM_RecordSet)))
    {
      YM_RecordSet* pRecordSet = (YM_RecordSet*)pObject;
	  if ((pRecordSet == m_pScStatusSet) || (pRecordSet == m_pHistoStatusSet) || (pRecordSet == m_pBrmsDateSet))
	  { 
	    TestBatchFinished();
	  }
	  
      // First, test if all stages of the Reference Data Ctl Object are valid.
      // If so, we are ready to go
      // Else, test if the current stage is valid.
      // if so, let's launch the next stage...
      if (! m_pRefDataCtl->IsValid(YM_RefDataCtl::eAllStages))
      {
        if (m_pRefDataCtl->IsValid(YM_RefDataCtl::eCurrentStage))
        {
          CString szStatusMsg = m_pRefDataCtl->LaunchStage();
          ((CMainFrame*)((CCTAApp*)APP)->GetMainWnd())->GetStatusBar()->SetPaneOneAndLock(szStatusMsg);
        }
        return;
      }
      else  // all stages of reference data are valid...
      {
        if (!FinalDocumentInitialization())
		{
		  CMainFrame* pMainFrame = ((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd());
		  pMainFrame->PostMessage (WM_CLOSE);
		}
      }
    }
  }
}

BOOL CCTADoc::CanCloseFrame(CFrameWnd* pFrame) 
{
  ASSERT_VALID(pFrame);
  UINT iCount = 0;
  
  POSITION pos = GetFirstViewPosition();
  while (pos != NULL)
  {
    CView* pView = GetNextView(pos);
    ASSERT_VALID(pView);
    CFrameWnd* pFrame = pView->GetParentFrame();
    
    // Assume frameless views are ok to close
    if (pFrame != NULL)
    {
      // Assume 1 document per frame
      ASSERT_VALID(pFrame);
      iCount++;
    }
  }
  if (iCount > 1)
    return TRUE;
  
  return SaveModified();
}

BOOL CCTADoc::GetRrd(int index, int& value)
{ 
  return index;
}

// Calculate the reference date with the same Day of Week with iYearOffset years difference
//   for example, if iYearOffset = -1, will calculate the closest date with same DOW 1 year ago
long CCTADoc::GetReferenceDate(long lDate, int iYearOffset)
{ 
  // we calculate the reference date here ... iYearOffset years difference, same day of week
  RWDate d1(lDate);
  RWDate d2(lDate+ (365*iYearOffset));
  int iWeekDay = d1.weekDay();   //  monday = 1  ...   sunday = 7
  int iWeekDayOffsetYear = d2.weekDay();   //  monday = 1  ...   sunday = 7
  int iDiff = iWeekDay - iWeekDayOffsetYear;
  int iAbsDiff = abs(iDiff);
  int i7Diff = 7 - iAbsDiff;
  if (iAbsDiff > 3)
  {
    RWDate rwDateRef(lDate+ (365*iYearOffset) + i7Diff);
    return rwDateRef.julian();
  }
  else
  {
    RWDate rwDateRef(lDate+ (365*iYearOffset) + iDiff);
    return rwDateRef.julian();
  }
}

// If this  bucket ID is not in sys parms "UI_BUCKET_NOMOD",
BOOL CCTADoc::IsNoModBucket(CString BucketId)
{
  CStringArray* paszTmp = GetBktNoModArray();  // from sys parms "UI_BUCKET_NOMOD"
  int iSize = paszTmp->GetSize();
  for (int i = 0; i < iSize; i++)
  {
    if (paszTmp->GetAt(i) == BucketId)
      return (TRUE);
  }
  return (FALSE);
}

///////////////////////////////////////////////////////
// Get the "Comptage" value for the currently selected: Family No and Leg
//  the Alt flag is passed in to determine which Tranche and Leg selection to use
//  the Dpt Date and Cmpt are passed in to allow a specific dpt date and cmpt to be retrieved
//
YmIcComptagesDom* CCTADoc::GetComptagesRecord(CPtrArray* pComptagesArray, int iFamilyNo, int iDptDate, CString szLegOrig, CString szCmpt)
{
  if (pComptagesArray->GetSize() == 0)
    return NULL;
  YmIcComptagesDom* pComptagesRecord = NULL;

  // Les données comptage ne contiennent pas le compartiment F. Il y a B à la place.
  // Donc si on demande du F on cherche du B
  if (szCmpt == "F")
	szCmpt = "B";
  
  // first, search for an exact match ...
  int iIndex;
  for (iIndex = 0; iIndex < pComptagesArray->GetSize(); iIndex++)
  {
    pComptagesRecord = (YmIcComptagesDom*)pComptagesArray->GetAt(iIndex);
    if (pComptagesRecord &&
	  pComptagesRecord->FamilyNo() == iFamilyNo &&
      pComptagesRecord->DptDate() ==(unsigned)iDptDate &&
      !strcmp(pComptagesRecord->LegOrig(), szLegOrig) &&
      !strcmp(pComptagesRecord->Cmpt(), szCmpt))
      return pComptagesRecord;  // found it !
  }
  
  // didn't find exact match, find closest by leg sequence ...
  YmIcLegsDom* pLegSelected = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
  YM_Iterator<YmIcLegsDom>* pLegIterator = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY)->CreateIterator();
  YmIcLegsDom* pLegIterate = NULL;
  
  // start with Leg Sequence difference of 1 (we've already tried the currently selected leg with difference of 0), then progress until we find a match ...
  for (int iMaxDiff = 1; iMaxDiff < pLegIterator->GetCount(); iMaxDiff++)
  {
    // find a leg in the Leg List RecordSet whose leg sequence is (iMaxDiff) less than or greater than current leg sequence
    for (pLegIterator->First(); ! pLegIterator->Finished(); pLegIterator->Next()) 
    {
      // calculate difference between this record and the currently selected record's leg sequence
      pLegIterate = pLegIterator->Current();
      if (abs(pLegSelected->LegSeq() - pLegIterate->LegSeq()) == iMaxDiff)
      {
        // now, try to find this pLegIterate in the Comptages RecordSet ...
        for (iIndex = 0; iIndex < pComptagesArray->GetSize(); iIndex++)
        {
          pComptagesRecord = (YmIcComptagesDom*)pComptagesArray->GetAt(iIndex);
          if (pComptagesRecord &&
			pComptagesRecord->FamilyNo() == iFamilyNo &&
            pComptagesRecord->DptDate() ==(unsigned)iDptDate &&
            !strcmp(pComptagesRecord->LegOrig(), pLegIterate->LegOrig()) &&
            !strcmp(pComptagesRecord->Cmpt(), szCmpt))
            return pComptagesRecord;  // found it !
        }
      }
    }
  }
  delete pLegIterator;
  return NULL;
}

// FONT Manipulation
//
BOOL CCTADoc::IsDefaultFont()
{
  if (
    GetFontName() == "MS Sans Serif" &&
    GetFontHeight() == 0xfffffff5 &&
    GetFontWeight() == FW_DONTCARE &&
    GetFontItalic() == 0 
    )
    return TRUE;
  else
    return FALSE;
}

CString CCTADoc::GetFontName()
{
  if (m_fontname.IsEmpty())
    m_fontname = ((CCTAApp*)APP)->GetProfileString("Fonts", "FontName", "MS Sans Serif");
  return m_fontname;
}
void CCTADoc::SetFontName(CString fname)
{ 
  m_fontname = fname;
  ((CCTAApp*)APP)->WriteProfileString("Fonts", "FontName", m_fontname);
}

long CCTADoc::GetFontWeight()
{ 
  if (m_fontweight == 2)
    m_fontweight = ((CCTAApp*)APP)->GetProfileInt("Fonts", "FontWeight", FW_DONTCARE);
  return m_fontweight;
}

void CCTADoc::SetFontWeight(long fweight)
{
  m_fontweight = fweight;
  ((CCTAApp*)APP)->WriteProfileInt("Fonts", "FontWeight", m_fontweight);
}

long CCTADoc::GetFontHeight()
{ 
  if (m_fontheight == 0)
    m_fontheight = ((CCTAApp*)APP)->GetProfileInt("Fonts", "FontHeight", 0xfffffff5);
  return m_fontheight;
}

void CCTADoc::SetFontHeight(long fheight)
{
  m_fontheight = fheight;
  ((CCTAApp*)APP)->WriteProfileInt("Fonts", "FontHeight", m_fontheight);
}

BYTE CCTADoc::GetFontItalic()
{ 
  if (m_fontitalic == 2)
    m_fontitalic = ((CCTAApp*)APP)->GetProfileInt("Fonts", "FontItalic", 0);
  return m_fontitalic;
}

void CCTADoc::SetFontItalic(BYTE fitalic)
{ 
  if (fitalic > 1)
    fitalic = 1; // On accepte uniquement 0 ou 1
  m_fontitalic = fitalic;
  ((CCTAApp*)APP)->WriteProfileInt("Fonts", "FontItalic", m_fontitalic);
}

void CCTADoc::ReloadScreenFonts()
{
  // First we delete the Print fonts if they are loaded.
  // The two line below do this operation.
  m_PrintResol = 0;
  ReloadPrintFonts();
  
  if (m_pScreenStdFont)
  {
    delete m_pScreenStdFont;
    m_pScreenStdFont = NULL;
  }
  if (m_pScreenBoldFont)
  {
    delete m_pScreenBoldFont;
    m_pScreenBoldFont = NULL;
  }
  if (m_pScreenTitleFont)
  {
    delete m_pScreenTitleFont;
    m_pScreenTitleFont = NULL;
  }
  if (m_pScreenFixedFont)
  {
    delete m_pScreenFixedFont;
    m_pScreenFixedFont = NULL;
  }
  
  m_pScreenStdFont = new CFont();
  m_pScreenBoldFont = new CFont();
  m_pScreenTitleFont = new CFont();
  m_pScreenFixedFont = new CFont();

  m_fontname = "";
  m_fontheight = 0;
  m_fontweight = 2; // valeur bidon, pour dire non initialisé
  m_fontitalic = 2; // valeur bidon, pour dire non initialisé
  
  m_pScreenStdFont->CreateFont(
    GetFontHeight(), // int nHeight
    0, // int nWidth
    0, // int nEscapement
    0, // int nOrientation
    GetFontWeight(), // int nWeight
    GetFontItalic(), // BYTE bItalic
    FALSE, // BYTE bUnderline
    0, // BYTE cStrikeOut
    ANSI_CHARSET, // BYTE nCharSet
    OUT_DEFAULT_PRECIS, // BYTE nOutPrecision
    CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision
    DEFAULT_QUALITY, // BYTE nQuality
    DEFAULT_PITCH | FF_SWISS, // BYTE nPitchAndFamily
    GetFontName());	// LPCTSTR lpszFacename

  // Si la font standard est déjà BOLD, on inverse.
  int weight = GetFontWeight();
  if (weight <= FW_NORMAL)
    weight = FW_BOLD;
  else
    weight = FW_NORMAL;

  m_pScreenBoldFont->CreateFont(
    GetFontHeight(), // int nHeight
    0, // int nWidth
    0, // int nEscapement
    0, // int nOrientation
    weight, // int nWeight
    GetFontItalic(), // BYTE bItalic
    FALSE, // BYTE bUnderline
    0, // BYTE cStrikeOut
    ANSI_CHARSET, // BYTE nCharSet
    OUT_DEFAULT_PRECIS, // BYTE nOutPrecision
    CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision
    DEFAULT_QUALITY, // BYTE nQuality
    DEFAULT_PITCH | FF_SWISS, // BYTE nPitchAndFamily
    GetFontName());	// LPCTSTR lpszFacename
  
  m_pScreenTitleFont->CreateFont(
    GetFontHeight() + 1, // int nHeight
    0, // int nWidth
    0, // int nEscapement
    0, // int nOrientation
    FW_BOLD, // int nWeight
    GetFontItalic(), // BYTE bItalic
    FALSE, // BYTE bUnderline
    0, // BYTE cStrikeOut
    ANSI_CHARSET, // BYTE nCharSet
    OUT_DEFAULT_PRECIS, // BYTE nOutPrecision
    CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision
    DEFAULT_QUALITY, // BYTE nQuality
    DEFAULT_PITCH | FF_SWISS, // BYTE nPitchAndFamily
    GetFontName());	// LPCTSTR lpszFacename
  
  m_pScreenFixedFont->CreateFont(
    GetFontHeight(), // int nHeight
    0, // int nWidth
    0, // int nEscapement
    0, // int nOrientation
    GetFontWeight(), // int nWeight
    GetFontItalic(), // BYTE bItalic
    FALSE, // BYTE bUnderline
    0, // BYTE cStrikeOut
    ANSI_CHARSET, // BYTE nCharSet
    OUT_DEFAULT_PRECIS, // BYTE nOutPrecision
    CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision
    DEFAULT_QUALITY, // BYTE nQuality
    DEFAULT_PITCH | FF_SWISS, // BYTE nPitchAndFamily
    "Courier New");	// LPCTSTR lpszFacename
}

void CCTADoc::ReloadPrintFonts()
{
  if (m_pPrintStdFont)
  {
    delete m_pPrintStdFont;
    m_pPrintStdFont = NULL;
  }
  if (m_pPrintTitleFont)
  {
    delete m_pPrintTitleFont;
    m_pPrintTitleFont = NULL;
  }
  if (m_pPrintFixedFont)
  {
    delete m_pPrintFixedFont;
    m_pPrintFixedFont = NULL;
  }
  
  if (!m_PrintResol)
    return;
  
  if (!m_ScreenResol)
    m_ScreenResol = 100;
  
  m_pPrintStdFont = new CFont();
  m_pPrintTitleFont = new CFont();
  m_pPrintFixedFont = new CFont();
  
  m_pPrintStdFont->CreateFont(
    (GetFontHeight() * m_PrintResol) / m_ScreenResol, // int nHeight
    0, // int nWidth
    0, // int nEscapement
    0, // int nOrientation
    GetFontWeight(), // int nWeight
    GetFontItalic(), // BYTE bItalic
    FALSE, // BYTE bUnderline
    0, // BYTE cStrikeOut
    ANSI_CHARSET, // BYTE nCharSet
    OUT_DEFAULT_PRECIS, // BYTE nOutPrecision
    CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision
    DEFAULT_QUALITY, // BYTE nQuality
    DEFAULT_PITCH | FF_SWISS, // BYTE nPitchAndFamily
    GetFontName());	// LPCTSTR lpszFacename
  
  m_pPrintTitleFont->CreateFont(
    ((GetFontHeight() + 1) * m_PrintResol) / m_ScreenResol, // int nHeight
    0, // int nWidth
    0, // int nEscapement
    0, // int nOrientation
    FW_BOLD, // int nWeight
    GetFontItalic(), // BYTE bItalic
    FALSE, // BYTE bUnderline
    0, // BYTE cStrikeOut
    ANSI_CHARSET, // BYTE nCharSet
    OUT_DEFAULT_PRECIS, // BYTE nOutPrecision
    CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision
    DEFAULT_QUALITY, // BYTE nQuality
    DEFAULT_PITCH | FF_SWISS, // BYTE nPitchAndFamily
    GetFontName());	// LPCTSTR lpszFacename
  
  m_pPrintFixedFont->CreateFont(
    (GetFontHeight() * m_PrintResol) / m_ScreenResol, // int nHeight
    0, // int nWidth
    0, // int nEscapement
    0, // int nOrientation
    GetFontWeight(), // int nWeight
    GetFontItalic(), // BYTE bItalic
    FALSE, // BYTE bUnderline
    0, // BYTE cStrikeOut
    ANSI_CHARSET, // BYTE nCharSet
    OUT_DEFAULT_PRECIS, // BYTE nOutPrecision
    CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision
    DEFAULT_QUALITY, // BYTE nQuality
    DEFAULT_PITCH | FF_SWISS, // BYTE nPitchAndFamily
    "Courier New");	// LPCTSTR lpszFacename
}

CFont* CCTADoc::GetStdFont(CDC* pDC)
{
  if (pDC && pDC->IsPrinting())
  {
    if (m_PrintResol != pDC->GetDeviceCaps(LOGPIXELSX))
    {
      m_PrintResol = pDC->GetDeviceCaps(LOGPIXELSX);
      ReloadPrintFonts();
    }
    return m_pPrintStdFont;
  }
  if (pDC)
    m_ScreenResol = pDC->GetDeviceCaps(LOGPIXELSX);
  return m_pScreenStdFont;
}

CFont* CCTADoc::GetBoldFont(CDC* pDC)
{
  if (pDC && pDC->IsPrinting())
  {
    if (m_PrintResol != pDC->GetDeviceCaps(LOGPIXELSX))
    {
      m_PrintResol = pDC->GetDeviceCaps(LOGPIXELSX);
      ReloadPrintFonts();
    }
    return m_pPrintStdFont;
  }
  if (pDC)
    m_ScreenResol = pDC->GetDeviceCaps(LOGPIXELSX);
  return m_pScreenBoldFont;
}

CFont* CCTADoc::GetTitleFont(CDC* pDC)
{
  if (pDC && pDC->IsPrinting())
  {
    if (m_PrintResol != pDC->GetDeviceCaps(LOGPIXELSX))
    {
      m_PrintResol = pDC->GetDeviceCaps(LOGPIXELSX);
      ReloadPrintFonts();
    }
    return m_pPrintTitleFont;
  }
  if (pDC)
    m_ScreenResol = pDC->GetDeviceCaps(LOGPIXELSX);
  return m_pScreenTitleFont;
}

CFont* CCTADoc::GetFixedFont(CDC* pDC)
{
  if (pDC && pDC->IsPrinting())
  {
    if (m_PrintResol != pDC->GetDeviceCaps(LOGPIXELSX))
    {
      m_PrintResol = pDC->GetDeviceCaps(LOGPIXELSX);
      ReloadPrintFonts();
    }
    return m_pPrintFixedFont;
  }
  if (pDC)
    m_ScreenResol = pDC->GetDeviceCaps(LOGPIXELSX);
  return m_pScreenFixedFont;
}

bool CCTADoc::GetEntitySurSousResa (CString curentity, CString scmpt, int &ovb, int &unb, int &ovg, bool def112)
{
	if ( !m_pCurrentEntityGag || (curentity != m_pCurrentEntityGag->Entity()))
	{
	  bool b=false;
	  m_pCurrentEntityGag = NULL;
      YM_Iterator<YmIcEntitiesDom>* pIterator = 
      YM_Set<YmIcEntitiesDom>::FromKey( ENTITY_GAG_KEY)->CreateIterator();
	  YmIcEntitiesDom* pEntityDom;
      for( pIterator->First(); !pIterator->Finished() && !b; pIterator->Next() ) 
	  {  
         pEntityDom = (YmIcEntitiesDom*)pIterator->Current();
		 if (curentity == pEntityDom->Entity())
		 {
           m_pCurrentEntityGag = pEntityDom;
		   b = true;
		 }
	  }
	  delete pIterator;
	}

	if (def112)
	  ovb = 112;
	else if ((scmpt == "A") || (scmpt == "C"))
	  ovb = 120;
	else
	  ovb = 115;
	unb = 100;
	ovg = m_pGlobalSysParms->GetBitrSurResaGlob();
	bool bok = false;
	if (m_pCurrentEntityGag)
	{
	    if (scmpt == "A")      {ovb = m_pCurrentEntityGag->SurResaA(); unb = m_pCurrentEntityGag->SousResaA(); bok = true;}
  	    else if (scmpt == "B") {ovb = m_pCurrentEntityGag->SurResaB(); unb = m_pCurrentEntityGag->SousResaB(); bok = true;}
  	    else if (scmpt == "C") {ovb = m_pCurrentEntityGag->SurResaC(); unb = m_pCurrentEntityGag->SousResaC(); bok = true;}
  	    else if (scmpt == "D") {ovb = m_pCurrentEntityGag->SurResaD(); unb = m_pCurrentEntityGag->SousResaD(); bok = true;}
	    else if (scmpt == "F") {ovb = m_pCurrentEntityGag->SurResaF(); unb = m_pCurrentEntityGag->SousResaF(); bok = true;}
	    else { ovb = 115; unb = 100; }
	    // AntiBug
	    if (!ovb)
		{
		  if (def112)
	        ovb = 112;
	      else if ((scmpt == "A") || (scmpt == "C"))
		    ovb = 120;
		  else
		    ovb = 115;
		}
	    if (!unb)
		  unb = 100;
		ovg = m_pCurrentEntityGag->SurResaTot();
	}
	return bok;
}

bool CCTADoc::IsEntityGag (CString curentity)
{
  YM_Iterator<YmIcEntitiesDom>* pIterator = 
     YM_Set<YmIcEntitiesDom>::FromKey( ENTITY_GAG_KEY)->CreateIterator();
  YmIcEntitiesDom* pEntityDom;
  for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
  {  
    pEntityDom = (YmIcEntitiesDom*)pIterator->Current();
    if (curentity == pEntityDom->Entity())
	{
      delete pIterator;
      return true;
	}
  }
  delete pIterator;
  return false;
}

CString CCTADoc::GetGagEntityTranche(int trancheno)
{
  CString sEntity;
  if (m_AllEntityTranche.GetSize() == 0)
  { // il faut d'abord récupérer les entités de chaque tranche
	YmIcTrancheLisDom ETDom;
    YM_Query* pSQL = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
	m_AllEntityTranche.SetSize (10000);
    RWDBReader Reader( ((CCTAApp*)APP)->m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_GAG_ENTITY_TRANCHES));
    while (Reader())
    {
      if (Reader.isValid())
	  {
        Reader >> ETDom;
		sEntity = ETDom.Entity();
	    // BUG_I SATC 7.4.9 / 7.5.9 : moteur d'envoi VNAX apparition fenêtre DEBUG C++ si nature erronée
		// bien vérifier que le numéro de tranche est > 0
		if (ETDom.TrancheNo() < 10000 && ETDom.TrancheNo() > 0)
		  m_AllEntityTranche.SetAt (ETDom.TrancheNo(), sEntity);
	  }
	}
	delete pSQL;
  }
  sEntity.Empty();
  // BUG_I SATC 7.4.9 / 7.5.9 : moteur d'envoi VNAX apparition fenêtre DEBUG C++ si nature erronée
  // bien vérifier que le numéro de tranche est > 0
  if (trancheno < 10000 && trancheno > 0)
    sEntity = m_AllEntityTranche.GetAt (trancheno);
  return sEntity;
}

//DM 7093 Donne pour la tranche le premier j-x avec HOI_TRA = 'I', et le premier = 'R'.
// renvoie faux, si tranche non concernée, renvoi -2 dans confirme, si la tranche n'est pas encore confirmée.
BOOL CCTADoc::GetHoiTra (YmIcTrancheLisDom* pTranche, int& incertain, int& confirme)
{
  YmIcTDPJEDom* pRecord = NULL;
  incertain = -2;
  confirme = -2;
  if (!strcmp (pTranche->HoiTra(), "I") || !strcmp (pTranche->HoiTra(), "R"))
  {
	YM_Iterator<YmIcTDPJEDom>* pIterator2 = YM_Set<YmIcTDPJEDom>::FromKey( HOITRAI_KEY)->CreateIterator();
	for( pIterator2->First(); !pIterator2->Finished(); pIterator2->Next() ) 
    {
      pRecord = (YmIcTDPJEDom*)pIterator2->Current();
	  if ((pRecord->TrancheNo() == pTranche->TrancheNo()) && (pRecord->DptDate() == pTranche->DptDate()) )
	  {
	    incertain = pRecord->J_x();
		break;
	  }
	}
	delete pIterator2;
  }
  if (!strcmp (pTranche->HoiTra(), "R"))
  {
	YM_Iterator<YmIcTDPJEDom>* pIterator2 = YM_Set<YmIcTDPJEDom>::FromKey( HOITRAR_KEY)->CreateIterator();
	for( pIterator2->First(); !pIterator2->Finished(); pIterator2->Next() ) 
    {
      pRecord = (YmIcTDPJEDom*)pIterator2->Current();
	  if ((pRecord->TrancheNo() == pTranche->TrancheNo()) && (pRecord->DptDate() == pTranche->DptDate()) )
	  {
	    confirme = pRecord->J_x();
		break;
	  }
    }
    delete pIterator2;
  }
  return ((incertain > -2) || (confirme > -2));
}

//DM 7093, recherche dans les date historiques les j-x incertain et confirme.
// attention, peuvent être NULL, dans ce cas on renvoi false
// renvoi -2 dans confirme, si la tranche n'a jamais été confirmée.
BOOL CCTADoc::GetHistoHoiTra (int iHisto, int& incertain, int& confirme)
{
  YmIcHistoEvolDom* pHistoEvol = NULL;
  incertain = -2;
  confirme = -2;
  
	YM_Iterator<YmIcHistoEvolDom>* pIterator1 = YM_Set<YmIcHistoEvolDom>::FromKey( HISTO_HOITRAI_KEY)->CreateIterator();
	for( pIterator1->First(); !pIterator1->Finished(); pIterator1->Next() ) 
    {
      pHistoEvol = (YmIcHistoEvolDom*)pIterator1->Current();
      if ( ((iHisto == 1) && (pHistoEvol->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto1())) ||
		   ((iHisto == 2) && (pHistoEvol->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto2())) ||
		   ((iHisto == 3) && (pHistoEvol->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto3())) )
	  {
		 incertain = pHistoEvol->RrdIndex();
		 break;
	  }
	}
	delete pIterator1;

	YM_Iterator<YmIcHistoEvolDom>* pIterator2 = YM_Set<YmIcHistoEvolDom>::FromKey( HISTO_HOITRAI_KEY)->CreateIterator();
	for( pIterator2->First(); !pIterator2->Finished(); pIterator2->Next() ) 
    {
      pHistoEvol = (YmIcHistoEvolDom*)pIterator2->Current();
      if ( ((iHisto == 1) && (pHistoEvol->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto1())) ||
		   ((iHisto == 2) && (pHistoEvol->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto2())) ||
		   ((iHisto == 3) && (pHistoEvol->DptDate() == ((CCTAApp*)APP)->m_CurHistoDates.DateHisto3())) )
	  {
		 confirme = pHistoEvol->RrdIndex();
		 break;
	  }
	}
	delete pIterator2;

  return ((incertain > -2) || (confirme > -2));
}


