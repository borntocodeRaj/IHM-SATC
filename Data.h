#ifndef DATA_H
#define DATA_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

class YM_Database;

//
// CPointReelEvol
//
class CPointReelEvol : public YmIcBktEvolDom
{
public:
	inline long Key() { return MAKELONG( *Cmpt(), NestLevel() ); }
};

//
// CRRDHistBktEvol
//
class CRRDHistBktEvol : public YmIcBktEvolDom
{
public:
	inline long Key() { return MAKELONG( *Cmpt(), NestLevel() ); }
};


//
// CRRDHistBktEvolSet
//
class CRRDHistBktEvolSet : public YM_MapArraySet<CRRDHistBktEvol,long,long>
{
public:
	CRRDHistBktEvolSet(YM_Database* pDatabase)
		: YM_MapArraySet<CRRDHistBktEvol,long,long>(pDatabase,IDS_SQL_SELECT_RRD_HIST_BKT) {}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CRRDHistoEvolSet
//
class CRRDHistoEvolSet : public YM_ArraySet<YmIcHistoEvolDom>
{
public:
	CRRDHistoEvolSet(YM_Database* pDatabase, char HoiTra);
protected:
	YmDtDom* OnSelectQuery();
	char m_hoitra; // le CRRDHistoEvolSet est aussi utilisé pour récupérer les info HOITRA sur dates historiques. 
	               // (IDS_SELECT_MAX_HISTO_HOI_TRA). m_hoitra sert dans ce cas à indiquer la lettre recherchée dans YH_TDJ.HOI_TRA
};

//
// CRRDHistCmptEvol
//
class CRRDHistCmptEvol : public YmIcCmptEvolDom
{
public:
	inline char Key() { return *Cmpt(); }
};

//
// CRRDHistCmptEvolSet
//
class CRRDHistCmptEvolSet : public YM_MapArraySet<CRRDHistCmptEvol,char,char>
{
public:
	CRRDHistCmptEvolSet(YM_Database* pDatabase)
		: YM_MapArraySet<CRRDHistCmptEvol,char,char>(pDatabase,IDS_SQL_SELECT_RRD_HIST_CMPT) {}
protected:
	YmDtDom* OnSelectQuery();
};


//
// CQuelCmptSet
//
class CQuelCmptSet : public YM_ArraySet<YmIcComptagesDom>
{
  public:
	CQuelCmptSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcComptagesDom>(pDatabase,IDS_SQL_SELECT_QUEL_CMPT_5100){}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CComptagesSet
//
class CComptagesSet : public YM_ArraySet<YmIcComptagesDom>
{
public:
	CComptagesSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcComptagesDom>(pDatabase,IDS_SQL_SELECT_COMPTAGES_TRN){}
protected:
	YmDtDom* OnSelectQuery();
};


//
// CComptagesHistSet
//
class CComptagesHistSet : public YM_ArraySet<YmIcComptagesDom>
{
public:
	CComptagesHistSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcComptagesDom>(pDatabase,IDS_SQL_SELECT_COMPTAGES_HIST_TRN){}
protected:
	YmDtDom* OnSelectQuery();
};


//
// CHistDpt
//
class CHistDpt : public YmIcHistDptDom
{
public:
	inline char Key() { return *Cmpt(); }
};

//
// CHistDptSet
//
class CHistDptSet : public YM_MapArraySet<CHistDpt,char,char>
{
public:
	CHistDptSet(YM_Database* pDatabase)
		: YM_MapArraySet<CHistDpt,char,char>(pDatabase,IDS_SQL_SELECT_HIST_DPT) {}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CBktHistSet
//
class CBktHistSet : public YM_ArraySet<YmIcBktHistDom>
{
public:
	CBktHistSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcBktHistDom>(pDatabase,IDS_SQL_SELECT_BKT_HIST,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
	YmDtDom* OnSelectQueryX(BOOL bAlt = FALSE);
};


class CBktHistAltSet : public CBktHistSet
{
public:
	CBktHistAltSet(YM_Database* pDatabase)
		: CBktHistSet(pDatabase){}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CBktHistoSet
//
class CBktHistoSet : public YM_ArraySet<YmIcBktHistDom>
{
public:
	CBktHistoSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcBktHistDom>(pDatabase,IDS_SQL_SELECT_BKT_HISTO,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CBktPrevisionVenteSet
//
class CBktPrevisionVenteSet : public YM_ArraySet<YmIcPrevisionDom>
{
public:
	CBktPrevisionVenteSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcPrevisionDom>(pDatabase,IDS_SQL_SELECT_PREVISION_VENTE,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CBktPrevisionRevenuSet
//
class CBktPrevisionRevenuSet : public YM_ArraySet<YmIcPrevisionDom>
{
public:
	CBktPrevisionRevenuSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcPrevisionDom>(pDatabase,IDS_SQL_SELECT_PREVISION_REVENU,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CBktHistJumSet
//
class CBktHistJumSet : public YM_ArraySet<YmIcBktHistDom>
{
public:
	CBktHistJumSet(YM_Database* pDatabase, long m_jumno, unsigned long m_dptdate, CString m_legorig);
protected:
	YmDtDom* OnSelectQuery();
	YmDtDom* OnSelectQueryX(BOOL bAlt = FALSE);
public :
   long  m_jumno;
   unsigned long  m_dptdate;
   CString m_legorig;
};


//
// CPrevAuthSet
//
class CPrevAuthSet : public YM_ArraySet<YmIcBktHistDom>
{
public:
	CPrevAuthSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcBktHistDom>(pDatabase,IDS_SQL_PREVAUTH,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
	YmDtDom* OnSelectQueryX(BOOL bAlt = FALSE);
};

//
// CCmptHist
//
class CCmptHist : public YmIcCmptHistDpDom
{
public:
	inline long Spoil()
		{ return (ResHoldDptTot()>=AuthDpt()) ? (Capacity()-TrafficRes()) : 0L; }
	inline long Spill() {	return 0L; }
	inline long ResHoldDptTot() { return ResHoldDptIdv() + ResHoldDptGrp(); }
	inline long TrafficGrp() { return 0L; }
	inline long TrafficGoShows() { return 0L; }
};

//
// CCUserInfo
//
class CCUserInfo : public YM_ArraySet<CmOpUserDom>
{
public:
	CCUserInfo(YM_Database* pDatabase)
		: YM_ArraySet<CmOpUserDom>(pDatabase,IDS_SQL_SELECT_USERS,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CLegSet
//
class CLegSet : public YM_ArraySet<YmIcLegsDom>
{
  // note that AUTO_UPDATE is specified
public:
	CLegSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcLegsDom>(pDatabase,IDS_SQL_SELECT_LEG_LIST,IDS_SQL_UPDATE_LEG_LIST,YM_RecordSet::AUTO_UPDATE){}
protected:
	YmDtDom* OnSelectQuery();
	YmDtDom* OnSelectQueryX(BOOL bAlt = FALSE);
  YmDtDom* OnUpdateQuery();
  YmDtDom* OnUpdateQueryX(BOOL bAlt = FALSE);
};


//
// COthLegsSet
//
class COthLegsSet : public YM_ArraySet<YmIcOthLegsDom>
{
public:
	COthLegsSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcOthLegsDom>(pDatabase,IDS_SQL_SELECT_OTH_LEGS,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CRelatedSet
//
class CRelatedSet : public YM_ArraySet<YmIcRelatedDom>
{
public:
	CRelatedSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcRelatedDom>(pDatabase,IDS_SQL_SELECT_REL_TRANCHES,NULL,YM_RecordSet::AUTO_SELECT){}

	YmDtDom* OnSelectQuery();
	YmDtDom* OnSelectQueryX(BOOL bAlt = FALSE);

  YmIcLegsDom* m_pMkt;  // the market (leg) for which the analyst is currently studying
  CString m_szMktOrig;
  CString m_szMktDest;
  BOOL m_bMktSelect;
};

class CRelatedAltSet : public CRelatedSet
{
public:
	CRelatedAltSet(YM_Database* pDatabase)
		: CRelatedSet(pDatabase){}

	YmDtDom* OnSelectQuery();
};

//
// CResaBktSet
//
class CResaBktSet : public YM_ArraySet<YmIcResaBktDom>
{
public:
	CResaBktSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcResaBktDom>(pDatabase,IDS_SQL_SELECT_RESA_BKT,IDS_SQL_UPDATE_RESA_BKT_AUTH,YM_RecordSet::AUTO_UPDATE){}
protected:
	YmDtDom* OnSelectQuery();
	YmDtDom* OnSelectQueryX(BOOL bAlt = FALSE);
  YmDtDom* OnUpdateQuery();
  YmDtDom* OnUpdateQueryX(BOOL bAlt = FALSE);
};


//
// CResaCmptSet
//
class CResaCmptSet : public YM_ArraySet<YmIcResaCmptDom>
{
public:
	CResaCmptSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcResaCmptDom>(pDatabase,IDS_SQL_SELECT_RESA_CMPT,IDS_SQL_UPDATE_RESA_CMPT,YM_RecordSet::AUTO_UPDATE){}
protected:
	YmDtDom* OnSelectQuery();
	YmDtDom* OnSelectQueryX(BOOL bAlt = FALSE);
  YmDtDom* OnUpdateQuery();
	YmDtDom* OnUpdateQueryX(BOOL bAlt = FALSE);
};


//
// CTranche
//
class CTrancheSet : public YM_ArraySet<YmIcTrancheLisDom>
{
  // note that AUTO_UPDATE is specified
public:
	CTrancheSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_TRANCHE_LIST,IDS_SQL_UPDATE_TRANCHE_LIST,YM_RecordSet::AUTO_UPDATE){}
protected:
	YmDtDom* OnSelectQuery();
	YmDtDom* OnSelectQueryX(BOOL bAlt = FALSE);
	YmDtDom* OnUpdateQuery();
	YmDtDom* OnUpdateQueryX(BOOL bAlt = FALSE);
};

//
// CTDLJK
//
class CTDLJKSet : public YM_ArraySet<YmIcTDLJKDom>
{
  // note that AUTO_UPDATE is specified
public:
	CTDLJKSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTDLJKDom>(pDatabase,IDS_SQL_SELECT_TDLJK,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CTDPJE
//
class CTDPJESet : public YM_ArraySet<YmIcTDPJEDom>
{
  // note that AUTO_UPDATE is specified
public:
	CTDPJESet(YM_Database* pDatabase, char HoiTra);
protected:
	YmDtDom* OnSelectQuery();
	char m_hoitra; // le CTDLPJESet est aussi utilisé pour récupérer les info HOITRA. (IDS_SELECT_MAX_HOI_TRA)
	               // m_hoitra sert dans ce cas à indiquer la lettre recherchée dans TDJ.HOI_TRA
};


//
// CVirtNestSet
//
class CVirtNestSet : public YM_ArraySet<YmIcVirtNestDom>
{
public:
	CVirtNestSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcVirtNestDom>(pDatabase,IDS_SQL_SELECT_VIRT_NEST,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
	YmDtDom* OnSelectQueryX(BOOL bAlt = FALSE);
};



//
//CTrainDateSet
//
class CTrainDateSet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CTrainDateSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_TGV_JUM_POSSIBLE,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};


//
//CTrainSupplementSet
//
class CTrainSupplementSet : public YM_ArraySet<YmIcTrainSupplementDom>
{
public:
	CTrainSupplementSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrainSupplementDom>(pDatabase,IDS_SQL_SELECT_TRAIN_SUPPLEMENT,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

//
//CTrainAdaptationSet
//
class CTrainAdaptationSet : public YM_ArraySet<YmIcTrainSupplementDom>
{
public:
	CTrainAdaptationSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrainSupplementDom>(pDatabase,IDS_SQL_SELECT_TRAIN_ADAPTATION,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

//
//CTrainDeletedSet
//
class CTrainDeletedSet : public YM_ArraySet<YmIcTrainSupplementDom>
{
public:
	CTrainDeletedSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrainSupplementDom>(pDatabase,IDS_SQL_SELECT_TRAIN_DELETED,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

//
//CTrainEntitySet
//
class CTrainEntitySet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CTrainEntitySet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_TRAIN_ENTITY,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

//
//CTrainTrancheSet
//
class CTrainTrancheSet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CTrainTrancheSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_TRAIN_TRANCHE,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

//
//CAllTrainSet
//
class CAllTrainSet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CAllTrainSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_ALL_TRAINS,NULL,YM_RecordSet::AUTO_SELECT){}
private:
	YmDtDom* OnSelectQuery();
  
};

//
//CAllTrainWEntSet
//
class CAllTrainWEntSet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CAllTrainWEntSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_ALL_TRAINS_WITH_ENTITY,NULL,YM_RecordSet::AUTO_SELECT){}
private:
	YmDtDom* OnSelectQuery();
  
};

//
//CServedLegSet
//
class CServedLegSet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CServedLegSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_SERVED_LEG,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

//
//CServedLegWEntSet
//
class CServedLegWEntSet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CServedLegWEntSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_SERVED_LEG_WITH_ENTITY,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

//
//CODTrainSet
//
class CODTrainSet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CODTrainSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_ODTRAIN,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

//
//CODTrainWEntSet
//
class CODTrainWEntSet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CODTrainWEntSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_ODTRAIN_WITH_ENTITY,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

//
//CServedMarketWEntSet
//
class CServedMarketWEntSet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CServedMarketWEntSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_SERVED_MARKET_WITH_ENTITY,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

//
//CServedMarketSet
//
class CServedMarketSet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CServedMarketSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_SERVED_MARKET,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

//
//CLegTimesSet
//
class CLegTimesSet : public YM_ArraySet<YmIcLegsDom>
{
public:
	CLegTimesSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcLegsDom>(pDatabase,IDS_SQL_SELECT_LEG_TIMES,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

//
//CTgvGagSet
//
class CTgvGagSet : public YM_ArraySet<GagAutorizDom>
{
public:
	CTgvGagSet(YM_Database* pDatabase)
		: YM_ArraySet<GagAutorizDom>(pDatabase,IDS_SQL_SELECT_TGV_GAG,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};

//
//CDateHistoSet
//
class CDateHistoSet : public YM_ArraySet<YmIcDateHistoDom>
{
public:
	CDateHistoSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcDateHistoDom>(pDatabase,IDS_SQL_SELECT_DATE_HISTO,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CVnauListSet
//
class CVnauListSet : public YM_ArraySet<YmIcVnauDom>
{
public:
	CVnauListSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcVnauDom>(pDatabase,IDS_SQL_SELECT_VNAU_LIST,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};



//
// CVnlListSet
//


class CVnlListStubSet : public YM_ArraySet<YmIcVnlDom>
{
	public :
		CVnlListStubSet(YM_Database* pDatabase) :
		   YM_ArraySet<YmIcVnlDom>(pDatabase, IDS_SQL_SELECT_VNL_BOUCHON, NULL, YM_RecordSet::AUTO_SELECT){}

protected :
	YmDtDom* OnSelectQuery();
	YmDtDom* OnUpdateQuery();
};


// DM-7978-NCH
// CUserCCSet
//
class CUserCCSet : public YM_ArraySet<YmIcUserCCDom>
{
	public :
		CUserCCSet(YM_Database* pDatabase) :
				 YM_ArraySet<YmIcUserCCDom>(pDatabase, IDS_SQL_SELECT_CC_USER, NULL, YM_RecordSet::AUTO_SELECT){}
  protected:
		YmDtDom* OnSelectQuery();
		YmDtDom* OnUpdateQuery();
};



//
// CTraqueTarifSet
//
class CTraqueTarifSet : public YM_ArraySet<YmIcTraqueTarifDom>
{
public:
	CTraqueTarifSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTraqueTarifDom>(pDatabase,IDS_SQL_TRAQUE_TARIF,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CPrevisionSet
//
class CPrevisionSet : public YM_ArraySet<YmIcResaBktDom>
{
public:
	CPrevisionSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcResaBktDom>(pDatabase,IDS_SQL_SELECT_THALYS_PREVISION,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CCalendarTrainSet
//
class CCalendarTrainSet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CCalendarTrainSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_CALENDAR_TRAIN,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CTDLJK
//
class CCalendarTDLJKSet : public YM_ArraySet<YmIcTDLJKDom>
{
  // note that AUTO_UPDATE is specified
public:
	CCalendarTDLJKSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTDLJKDom>(pDatabase,IDS_SQL_CALENDAR_TDLJK,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};

//
// CVnauExcluSet
//
class CVnauExcluSet : public YM_ArraySet<YmIcVnauDom>
{
public:
	CVnauExcluSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcVnauDom>(pDatabase,IDS_SQL_VNAU_EXCLU,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
};

// DM7790
//
//CRouteAxeSet
//
class CRouteAxeSet : public YM_ArraySet<YmIcRoutesDom>
{
public:
	CRouteAxeSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcRoutesDom>(pDatabase,IDS_SQL_SELECT_ROUTE_AXE,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

// DM7790
//
//CEntityRouteSet
//
class CEntityRouteSet : public YM_ArraySet<YmIcEntitiesDom>
{
public:
	CEntityRouteSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcEntitiesDom>(pDatabase,IDS_SQL_SELECT_ENTITY_ROUTE,NULL,YM_RecordSet::AUTO_SELECT){}
protected:
	YmDtDom* OnSelectQuery();
  
};

#endif

//DM7790
//
//CAllTrainWAxesSet
//
class CAllTrainWAxesSet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CAllTrainWAxesSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_ALL_TRAINS_WITH_AXES,NULL,YM_RecordSet::AUTO_SELECT){}
private:
	YmDtDom* OnSelectQuery();
  
};

//DM7790
//
//CAllTrainWAxesSet
//
class CAllTrainWRoutesSet : public YM_ArraySet<YmIcTrancheLisDom>
{
public:
	CAllTrainWRoutesSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcTrancheLisDom>(pDatabase,IDS_SQL_SELECT_ALL_TRAINS_WITH_ROUTES,NULL,YM_RecordSet::AUTO_SELECT){}
private:
	YmDtDom* OnSelectQuery();
  
};

//DM7870
//
//CEntityNatureSet
//
class CEntityNatureSet : public YM_ArraySet<YmIcEntiteNatureDom>
{
public:
	CEntityNatureSet(YM_Database* pDatabase)
		: YM_ArraySet<YmIcEntiteNatureDom>(pDatabase,IDS_SQL_SELECT_ENTITES_NATURE){}
private:
	YmDtDom* OnSelectQuery();
};

//DM7978 - LME - CCmax CCouv et j-x histo pour montee en charge
// CHistoDataRRDSet
//

class CHistoDataRRDSet : public YM_ArraySet<YmIcHistoDataRRDDom>
{
	public :
	CHistoDataRRDSet(YM_Database* pDatabase) :
		YM_ArraySet<YmIcHistoDataRRDDom>(pDatabase, IDS_SQL_SELECT_RRD_HIST_CMPT_HISTO, NULL, YM_RecordSet::AUTO_SELECT){}

protected :
	YmDtDom* OnSelectQuery();
};

//DM7978 - LME - FIN

class CAscTarifSet : public YM_ArraySet<YmIcAscTarifDom>
{
public :
	CAscTarifSet(YM_Database* pDatabase) :
		YM_ArraySet<YmIcAscTarifDom>(pDatabase, IDS_SQL_SELECT_ASCENSEUR_TARIFAIRE, NULL, YM_RecordSet::AUTO_SELECT){}

protected :
	YmDtDom* OnSelectQuery();
};
//DM7978 - LME - FIN

// DM-7978 - CLE
class CCcMaxCcOuvSet : public YM_ArraySet<YmIcUserCCDom>
{
public :
	CCcMaxCcOuvSet(YM_Database* pDatabase) :
		   YM_ArraySet<YmIcUserCCDom>(pDatabase, IDS_SELECT_CCMAX_CCOUV, NULL, YM_RecordSet::AUTO_SELECT){}

protected :
	YmDtDom* OnSelectQuery();
};

class CCcMaxCcOuvLegSet : public YM_ArraySet<YmIcUserCCDom>
{
public :
	CCcMaxCcOuvLegSet(YM_Database* pDatabase) :
		   YM_ArraySet<YmIcUserCCDom>(pDatabase, IDS_SELECT_CCMAX_CCOUV_LEGS, NULL, YM_RecordSet::AUTO_SELECT){}

protected :
	YmDtDom* OnSelectQuery();
};


// DM-7978 
