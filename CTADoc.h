// CTADoc.h : interface of the CCTADoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTADOC_H__2017F0DD_3EE9_11D1_8369_400020302A31__INCLUDED_)
#define AFX_CTADOC_H__2017F0DD_3EE9_11D1_8369_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CSphereManager;	//DM7978 KGH
class YM_RecordSet;
class YM_RefDataCtl;
class CTgvJumDlg;
class CTrainInfluentDlg;
class YM_Observer;
class CCTAGlobalSysParms;

class CCTADoc : public CDocument, public YM_Observer
{
protected: // create from serialization only
	DECLARE_DYNCREATE(CCTADoc)

// Attributes
public:
	YmIcUserCCDom m_ccUserDom;//SRE 83664 - Optimization. Allocate it one time, not at every function exec.
	YM_RecordSet* m_pDCDateSet;
	YM_RecordSet* m_pSysDateSet;
	YM_RecordSet* m_pScStatusSet;
	YM_RecordSet* m_pHistoStatusSet;
	YM_RecordSet* m_pBrmsDateSet;
	YM_RecordSet* m_pCmptSet;
	YM_RecordSet* m_pMaxEntityRrdSet;
	YM_RecordSet* m_pDefRrdSet;
    YM_RecordSet* m_pFullJXSet;
	YM_RecordSet* m_pStationNotGrp;
	YM_RecordSet* m_pUserInfoSet;
	YM_RecordSet* m_pSysParmSet;
	YM_RecordSet* m_pTrainSupplementSet;
	YM_RecordSet* m_pTrainAdaptationSet;
	YM_RecordSet* m_pTrainDeletedSet;
	YM_RecordSet* m_pOrderSVCSet;		// liste ordonnée des buckets (pour moteur VNL)
	YM_RecordSet* m_pTgvStations;
	YM_RecordSet* m_pVnauListSet;		// DM 6850, visualisation de la liste des VNAU envoyés sur un TDL.
	YM_RecordSet* m_pPrevisionSet;
	YM_RecordSet* m_pSeasonsSet;		// DM 6881, coloration des saisons SNCF dans le calendrier
	YM_RecordSet* m_pCongesSet;			// DM 6881, conges scolaire
	YM_RecordSet* m_pMinMaxDptDate;		// DM 6881, les dates min et max de sc_tranches;
	YM_RecordSet* m_pTraqueTarifSet;	// DM 6715, traque tarifaire
	YM_RecordSet* m_pNomosVnauExclu;		// DM 6185.4 exclusion du modèle NOMOS
	YM_RecordSet* m_pTopSet;			// DM 7171, liste des TOP
	YM_RecordSet* m_pSensibiliteSet;	// DM 7171, liste des Sensibilité

	// requêtes liées au calendier DM 6881
	YM_RecordSet* m_pCalendarTrainSet;
	YM_RecordSet* m_pCalendarPastTrainSet;
	YM_RecordSet* m_pCalendarTDLJKSet;

	// RecordSet utilisées par le moteur VNL
	YM_RecordSet* m_pTrainEntitySet;
	YM_RecordSet* m_pTrainTrancheSet;
	YM_RecordSet* m_pServedLegWEntSet;
	YM_RecordSet* m_pServedLegSet;
	YM_RecordSet* m_pODTrainWEntSet;
	YM_RecordSet* m_pODTrainSet;
	YM_RecordSet* m_pAllTrainWEntSet;
	YM_RecordSet* m_pAllTrainSet;
	YM_RecordSet* m_pServedMarketWEntSet;
	YM_RecordSet* m_pServedMarketSet;
	YM_RecordSet* m_pLegTimesSet;
	//DM 7790
	YM_RecordSet* m_pAxeSet;
	YM_RecordSet* m_pRouteAxeSet;
	YM_RecordSet* m_pEntityRouteSet;
	YM_RecordSet* m_pAllTrainWAxesSet;
	YM_RecordSet* m_pAllTrainWRoutesSet;

	// RecordSet utilisé par les VNAU automatiques : SNCF uniquement
	YM_RecordSet* m_pTgvGagSet;
	YM_RecordSet* m_pOptimOpenSet;
	YM_RecordSet* m_pOptimLastSet;

	YM_RecordSet* m_pDateHistoSet;
	YM_RecordSet* m_pEntitySet;
	YM_RecordSet* m_pEntityGagSet;
	YM_RecordSet* m_pTrancheSet;
	YM_RecordSet* m_pTDLJKSet;
	YM_RecordSet* m_pTDPJESet;
	YM_RecordSet* m_pHoiTraISet;
	YM_RecordSet* m_pHoiTraRSet;
	YM_RecordSet* m_pLegSet;

	YM_RecordSet* m_pResaBktSet;
	YM_RecordSet* m_pResaCmptSet;
	YM_RecordSet* m_pResaBktSciSet;
	YM_RecordSet* m_pResaCmptSciSet;
	YM_RecordSet* m_pBktEvolSet;
	YM_RecordSet* m_pBktEvolAltSet;
	YM_RecordSet* m_pBktEvolAvgSet;
	YM_RecordSet* m_pBktEvolAvgAltSet;
	YM_RecordSet* m_pHistoHoiTraISet;
	YM_RecordSet* m_pHistoHoiTraRSet;
	YM_RecordSet* m_pCmptEvolSet;
	YM_RecordSet* m_pCmptEvolAltSet;
	YM_RecordSet* m_pCmptEvolAvgSet;
	YM_RecordSet* m_pCmptEvolAvgAltSet;
	YM_RecordSet* m_pHistoEvolSet;
	YM_RecordSet* m_pHistoEvolAltSet;
	YM_RecordSet* m_pHistoEvolCmptSet;
	YM_RecordSet* m_pHistoEvolCmptAltSet;
	YM_RecordSet* m_pBktHistSet;
	YM_RecordSet* m_pBktHistAltSet;
	YM_RecordSet* m_pBktHistoSet;
	YM_RecordSet* m_pBktHistoAltSet;
	YM_RecordSet* m_pPrevAuthSet;
	YM_RecordSet* m_pHistDptSet;
	YM_RecordSet* m_pHistDptAltSet;
	YM_RecordSet* m_pOthLegsSet;
	YM_RecordSet* m_pOthLegsAltSet;
	YM_RecordSet* m_pRelTrancheSet;
	YM_RecordSet* m_pRelTrancheAltSet;
	YM_RecordSet* m_pRelTgvSet;			// pour aller rechercher les TGV encadrants depuis corail
	YM_RecordSet* m_pRelTgvAltSet;		// idem
	YM_RecordSet* m_pVirtNestSet;
	YM_RecordSet* m_pQuelCmptSet;
	YM_RecordSet* m_pComptagesSet;
	YM_RecordSet* m_pComptagesHistSet;

	// RecordSets des revenus
	YM_RecordSet* m_pHistDptRevSet;
	YM_RecordSet* m_pHistDptRevAltSet;
	YM_RecordSet* m_pHistDptYavRevSet;		// Histo depart revenu année précédente
	YM_RecordSet* m_pHistDptYavRevAltSet;	// Histo depart revenu année précédente

	// Fenêtre de montée en charge des revenus
	YM_RecordSet* m_pBktEvolRevSet;
    YM_RecordSet* m_pBktEvolRevAltSet;
	YM_RecordSet* m_pBktEvolAvgRevSet;
    YM_RecordSet* m_pBktEvolAvgRevAltSet;
    YM_RecordSet* m_pCmptEvolRevSet;
    YM_RecordSet* m_pCmptEvolRevAltSet;
	YM_RecordSet* m_pCmptEvolAvgRevSet;
    YM_RecordSet* m_pCmptEvolAvgRevAltSet;
	YM_RecordSet* m_pHistoEvolRevSet;
    YM_RecordSet* m_pHistoEvolRevAltSet;
	YM_RecordSet* m_pHistoEvolRevCmptSet;
    YM_RecordSet* m_pHistoEvolRevCmptAltSet;

	// TDR Evol Prevision
	YM_RecordSet* m_pBktPrevisionVenteSet;
	YM_RecordSet* m_pBktPrevisionRevenuSet;

	// resa de l'année précédente pour l'historique par départ
	YM_RecordSet* m_pHistDptYavSet;
	YM_RecordSet* m_pHistDptYavAltSet;

	// NPI - DM7870 : Dates de référence
	YM_RecordSet* m_pEntiteNatureSet;
	
	// DM7978 - KGH - sheres de prix 
	CSphereManager* m_sphereManager;
	// DM7978 - KGH - FIN

	// DM-7978 - LME - CCmax, CCouv et j-x histo 
	//YM_RecordSet* m_pHistoDataRRDSet;		//liste des ccmax ccouv et j-x histo

	// DM-7978 - LME - YI-6046
	YM_RecordSet* m_pAscTarifSet;		//liste des ascenseurs tarifaires
	
		
	// DM 6884.2 sur-resa bi-tranche, recup des entity des tranches
	CStringArray m_AllEntityTranche;

	CTgvJumDlg*  m_pDlgTgvJumeaux;
	CTrainInfluentDlg* m_pTrainInfluentDlg;

	CCTAGlobalSysParms* m_pGlobalSysParms;  // global system parameters

// Operations
public:
	CCTADoc();

	enum ReferenceData
	{
    COMPARTMENTS,
    CRITICALITY_MESSAGES,
    DC_DATE,
    READING_DAYS,
    SYS_PARMS,
    USERS
	};

  // Last Authorization Forecast Date
  //  Called from Entity Dialog
  CString GetEntities() { return m_szEntities; }
  void SetEntities(CString szEntities) { m_szEntities = szEntities; };
  CString GetSelectedEntities() { return m_szSelectedEntities; }
  void SetSelectedEntities(CString szEntities) { m_szSelectedEntities = szEntities; };
  CString GetEntitiesDesc() { return m_szEntitiesDesc; }
  void SetEntitiesDesc(CString szEntitiesDesc) { m_szEntitiesDesc = szEntitiesDesc; };
  CString GetDOWs() { return m_szDOWs; }
  void SetDOWs(CString szDOWs) { m_szDOWs = szDOWs; };
  long GetLastAuthFcDate() { return m_lLastAuthFcDate; };
  void SetLastAuthFcDate(long lLastAuthFcDate) { m_lLastAuthFcDate= lLastAuthFcDate; };
  long GetDiffSysDate() { return m_lDiffSysDate; };
  void SetDiffSysDate(long lDiffSysDate) { m_lDiffSysDate= lDiffSysDate; };
  long GetBegDptDate() { return m_lBegDptDate; };
  void SetBegDptDate( long lBegDptDate ) { m_lBegDptDate = lBegDptDate; };
  long GetEndDptDate() { return m_lEndDptDate; };
  void SetEndDptDate( long lEndDptDate ) { m_lEndDptDate = lEndDptDate; };
  long GetMinRrdIndex() { return m_lMinRrdIndex; };
  void SetMinRrdIndex( long lMinRrdIndex ) { m_lMinRrdIndex = lMinRrdIndex; };
  long GetMaxRrdIndex() { return m_lMaxRrdIndex; };
  void SetMaxRrdIndex( long lMaxRrdIndex ) { m_lMaxRrdIndex = lMaxRrdIndex; };
  CString GetCurRrdList() { return m_szEntityRrdList; };
  CString GetDefRrdList() { return m_szDefaultRrdList; };
  void SetAllJMX ( BOOL bAllJMX) { m_bAllJMX = bAllJMX; };
  long GetAllJMX () { return m_bAllJMX; }
  void SetDecalBatch( int decal) {m_decalBatch = decal; }
  int  GetDecalBatch () { return m_decalBatch; }
  void SetNotFullJX (int jx);
  BOOL GetNotFullJX () { return m_bNotFullJX; }
  long GetConstrainLastAuthFcDate() { return m_bConstrainLastAuthFcDate; };
  void SetConstrainLastAuthFcDate(long _bConstrainLastAuthFcDate)
    { m_bConstrainLastAuthFcDate = _bConstrainLastAuthFcDate; };
  CStringArray* GetBktNoModArray() { return &m_aszBktNoMod; };
  BOOL IsNoModBucket( CString BucketId );
  BOOL IsValidCmpt (char cmpt);
  CString GetStationDescription(CString szStationCode);
  BOOL IsTgvStation(LPCSTR stationname);
  CMapStringToOb& GetTgvStationMap() {return m_TgvStationMap;}; // list de YmBaTgvStationDom indexe par nom de station
  YmIcComptagesDom* GetComptagesRecord(CPtrArray* pComptagesArray, int iFamilyNo, int iDptDate, CString szLegOrig, CString szCmpt);
  long GetCurrentEntityNatureID() {return m_lCurrentEntityNatureID;}; // NPI - DM7870 : Dates de référence
  void SetCurrentEntityNatureID(long lEntityNatureID) { m_lCurrentEntityNatureID = lEntityNatureID; }; // NPI - DM7870 : Dates de référence

  BOOL IsTrainSupplementViewCreated();
  BOOL IsTrainDeletedViewCreated();
  void TrainSupplement(YM_Observable* pObject);
  void TrainDeleted(YM_Observable* pObject);

  BOOL RefDataValid();
  BOOL RefDataError() { return m_bRefDataError; }
  BOOL RefDataComplete() { return m_bRefDataDone; }
  BOOL IsBatchFinished() { return m_bBatchFinished; }
  BOOL IsNomosFinished() { return m_bNomosFinished; }
  BOOL HideNomosNotFinish() { return m_bHideNomosNotFinish; }
  BOOL TestBatchFinished();
  BOOL CheckSysTime();
  BOOL CheckBatchComplete();
  void PostDatabaseInitialization();
  BOOL FinalDocumentInitialization();
  BOOL BldTgvStations();
  BOOL BldRrdArray();
  void LoadCurEntityRrd(CString szCurrentEntity);
  void ResetCurEntityRrd() {m_szCurEntity.Empty(); }

  BOOL IsViewCreated(const CRuntimeClass* cRuntimeClass);  // Is view already created ?
  BOOL GetRrd (int index, int& value);
  long GetReferenceDate (long lDate, int iYearOffset);

  // font's information Get and Set
  CString GetFontName();
  void SetFontName (CString fname);
  long GetFontWeight ();
  void SetFontWeight (long fweight);
  long GetFontHeight ();
  void SetFontHeight (long fheight);
  BYTE GetFontItalic ();
  void SetFontItalic (BYTE fitalic);
  BOOL IsDefaultFont();

  // To get fonts, pDC can be null, and then we send back a screen size font
  CFont* GetStdFont (CDC* pDC);
  CFont* GetBoldFont (CDC* pDC);
  CFont* GetTitleFont (CDC* pDC);
  CFont* GetFixedFont (CDC* pDC);

  // ReloadScreenFonts, a appeler après avoir mis à jour de nouvelles
  // caractéristiques de police (voir fct ci-dessus), et avant de
  // forcer la repeinture de toute les fenêtres.
  void ReloadScreenFonts();
  void ReloadPrintFonts();

  BOOL m_bProcessTrainSupplement;  // Set TRUE when an entity is selelcted
  BOOL m_bProcessTrainDeleted;  // Set TRUE when an entity is selelcted

  // DM5480 train auto
  BOOL IsTrainAutoOnly() { return m_bTrainAutoOnly; }
  void SetTrainAutoOnly( BOOL bTrainAutoOnly) { m_bTrainAutoOnly = bTrainAutoOnly; }

  //DM 6884.2 sur-resa bi-tranche
  YmIcEntitiesDom* m_pCurrentEntityGag;
  bool GetEntitySurSousResa (CString curentity, CString scmpt, int &ovb, int &unb, int &ovg, bool def112);
  bool IsEntityGag (CString curentity);
  CString GetGagEntityTranche(int trancheno);

   //DM 7093 Donne pour la tranche le premier j-x avec HOI_TRA = 'I', et le premier = 'R'.
  // renvoie faux, si tranche non concernée, renvoi -1 dans confirme, si la tranche n'est pas encore confirmée.
  BOOL GetHoiTra (YmIcTrancheLisDom* pTranche, int& incertain, int& confirme);
  BOOL CCTADoc::GetHistoHoiTra (int iHisto, int& incertain, int& confirme);


	  /// DM-7978 - CLE - YI-5005 YI-5006 - YI-5018 YI-5025

   /// DM-7978 - CLE
  //SRE 83664 - added the last 3 paramaters
	void UpdateCCMaxCCOuv(YmIcTrancheLisDom* tranche, YM_Iterator<YmIcTrancheLisDom>* pIterator, int index, int nbr_elements_to_query, bool force);
  //SRE 83664 - Refactoring of UpdateCCMaxCCOuv -> AddCCsInTrancheDom
	void AddCCsInTrancheDom(YmIcTrancheLisDom* tranche);
	void UpdateCCMaxCCOuvLegs(YmIcLegsDom* leg);
  /// DM-7978 - CLE

	bool recalculCCMaxCCOuv(long trancheNo, unsigned long dptDate, int jx, char cmpt);
	bool reclaculCCMaxCCOuvLegs(long trancheNo, unsigned long dptDate, int jx, char cmpt, CString strLegOrig, CString strLegDest, int& ccMax, int& ccOuv, int& ccMaxInd, int& ccOuvInd);
  /// DM-7978 - CLE


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCTADoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnNotify( YM_Observable* pObject );
	virtual ~CCTADoc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CCTADoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  void CreateRecordSets();
  void CreateRecordKeys();
  void ReplaceSQL();

private:

  // Notification used if the user has lauch the check Rrd ending command

  BOOL SetUserRole();

  //////////////////////
  // Font data
  //
  CString m_fontname;
  long m_fontweight;
  long m_fontheight;
  BYTE m_fontitalic;

  CFont* m_pScreenStdFont;
  CFont* m_pScreenBoldFont;
  CFont* m_pScreenTitleFont;
  CFont* m_pScreenFixedFont;
  CFont* m_pPrintStdFont;
  CFont* m_pPrintTitleFont;
  CFont* m_pPrintFixedFont;

  int m_ScreenResol;
  int m_PrintResol;

  //////////////////////
  // Reference data
  //
  CString m_szEntities;  // provide first a list of entities to select tranche list, and after LoadTranche the current entity
  CString m_szSelectedEntities; // liste des entités choisies par EntityDlg. NE BOUGE PAS...
  CString m_szEntitiesDesc; // description of selected entities
  CString m_szDefaultRrdList; // Liste des jours d'analyse par default, sous forme d'une suite de chaines de caractères
  CString m_szEntityRrdList; // Liste des jours d'analyse spécifique à l'entité de la tranche courante
							 // = m_szDefaultRrdList, si rien défini pour cette entité
  CString m_szFullCollecteJX; // liste des j-x avec flag_usage >= 1 dans YM_DEFAULT_RRD, sour forme: "1,2,3,4,  ,65,87,119,150,"
                              // sert a repérer si le j-x est une collecte totale dans fen resarail eet decompo par CC
  CString m_szCurEntity;     // Nom de l'entité correspondant à m_szEntityRrdList
  CString m_szDOWs;  // provide a list of days of the week to select tranche list
  long m_lLastAuthFcDate;	// to fill the get_cta_tranche_list query from entity dlg
  long m_lBegDptDate;			//  idem
  long m_lEndDptDate;			//  idem
  long m_lMinRrdIndex;	    //  idem
  long m_lMaxRrdIndex;	    //  idem
  long m_lDiffSysDate; // difference between system date and PC date.
  BOOL m_bAllJMX;	   // vrai si entity dlg indique de ramasser tous les J-x collectés.
  BOOL m_bConstrainLastAuthFcDate;
  BOOL m_bRefDataError;
  BOOL m_bRefDataDone;	// Set TRUE after reading of all reference date
  BOOL m_bBatchFinished; // indication de fin du batch Thales
  BOOL m_bNomosFinished; // indication de fin du batch Nomos
  BOOL m_bHideNomosNotFinish;	// indication de masquage des rrd nomos non terminé
  BOOL m_bFullInitCompleted; // Set TRUE after first window opening at the CTA start
  BOOL m_bFirstTestBatchFinished; // vrai, puis mis a faux par le premier appel à TestBatchFinished
  CStringArray m_aszBktNoMod;  // contains a list of those buckets where auth values are not modifiable
  YM_RefDataCtl* m_pRefDataCtl; // Reference Data Control Object
  CMapStringToOb m_TgvStationMap; // list de YmBaTgvStationDom indexe par nom de station

  BOOL m_bTrainAutoOnly; // DM 5480 train auto, possibilité en Corail de ne récuperer que les trains Auto
  BOOL m_bDbLinkCreated; // mise en place multiconnexions aux base de données (en vue DM 6609 sans DBlink réel)
  int m_decalBatch;      // nombre de jour de décalage du batch choisi dans la fenêtre entité
  BOOL m_bNotFullJX;      // vrai si le j-x n'est pas un jour de collecte totale

  long m_lCurrentEntityNatureID; // DM7870 ID courant de l'association EntityNature depuis YM_ENTITES_NATURE
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTADOC_H__2017F0DD_3EE9_11D1_8369_400020302A31__INCLUDED_)
