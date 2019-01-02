#if !defined(AFX_GLOBALSYSPARMS_H__6B84F230_E344_11D1_83A7_400020302A31__INCLUDED_)
#define AFX_GLOBALSYSPARMS_H__6B84F230_E344_11D1_83A7_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GlobalSysParms.h : header file
//

class CCTAChart;

 //FIX # ano hpqc 1066 KGH
typedef struct 
{
	int ordre_affichage;
	CString cmpt;
} CmptOrdre;
 //FIX # ano hpqc 1066 KGH

/////////////////////////////////////////////////////////////////////////////
// CCTAGlobalSysParms window

class CCTAGlobalSysParms : virtual public YM_GlobalSysParms
{
	// Construction
public:
	CCTAGlobalSysParms();

	// Attributes
public:
	void InitGlobalSysParms();  // virtual

	double GetUmLevel()	 { return m_dUmLevel; }
	double GetPortNumber()	 { return m_dPortNumber; }
	CString GetHostName() { return m_szHostName; }
	CString GetClient() { return m_szClient; }
	CString GetHistoSelect() { return m_szHistoSelect; }
	CString GetTgvSelect() { return m_szTgvSelect; }
	CString GetTgvHistSelect() { return m_szTgvHistSelect; }
	int GetMaxCmptBkt (CString cmpt); // renvoi nb max bucket pour ce compartiment, -1 si inexistant
	/// DM-7978 - CLE - YI-6018 YI-6019
	int GetMaxSciBkt(CString cmpt);
	/// DM-7978 - CLE 
	//DM7978 FIX # ano HPQC 1088 KGH:reorganiser l'ordre (SYS_PARM_PLUS) de la map m_CmptShowOrder(oum_CmptScxShowOrder)  par rapport a "ordre_affichage"(FLOAT_VALUE)
	void reodonerCmptShowOrder(std::map <int, CmptOrdre> &cmptShowOrder);
	//DM7978 FIX # ano HPQC 1088 KGH

	int GetSeuilVnauVnax() { return m_SeuilVnauVnax; }
	int GetNbJoursVisuModifTop() { return m_nbjoursVisuModifTop; }
	BOOL GetAutoCopyAuthFlag() { return m_bAutoCopyAuthFlag; }
	BOOL GetCompositionFlag() { return m_bCompositionFlag; }
	BOOL GetComptagesFlag() { return m_bComptagesFlag; }
	BOOL GetEntityDescFlag() { return m_bEntityDescFlag; }
	BOOL GetForcementFlag() { return m_bForcementFlag; }
	BOOL GetYieldODFlag() { return m_bYieldODFlag; }
	BOOL GetNiveauInVNLengine() { return m_bNiveauInVNLengine;}
	BOOL GetMasquageCourbe() { return m_bMasquageCourbe;}
	BOOL GetGraphOutlineFlag() { return m_bGraphOutlineFlag; }
	BOOL GetHeaderImageFlag() { return m_bHeaderImageFlag; }
	BOOL GetInfluentFlag() { return m_bInfluentFlag; }
	BOOL GetInventoryNestingMethod() { return m_bInventoryNestingMethod; }
	BOOL GetMultipleCriticalRulesFlag() { return m_bMultipleCriticalRulesFlag; }
	BOOL GetRevenuFlag() { return m_bRevenuFlag; }
	void SetRevenuFlag(bool bRevenuFlag) { m_bRevenuFlag = bRevenuFlag; }
	BOOL GetSupplementTrainFlag() { return m_bSupplementTrainFlag; }
	BOOL GetSupplementLevelFlag() { return m_bSupplementLevelFlag; }
	BOOL GetTGVJumeauxFlag() { return m_bTGVJumeauxFlag; }
	BOOL GetTrainAdaptFlag() { return m_bTrainAdaptFlag; }
	BOOL GetTrancheNoFlag() { return m_bTrancheNoFlag; }
	BOOL GetTRNFlag() { return m_bTRNFlag; }
	BOOL GetSCXFlag() { return m_bSCXFlag; }
	void SetSCXFlag(bool bSCXFlag) {m_bSCXFlag = bSCXFlag;}
	BOOL GetRRDHistoryFlag() { return m_bRRDHistoryFlag; }
	BOOL GetControlRRDCurveFlag() { return m_bControlRRDCurveFlag; }
	BOOL GetComptagesHistoryFlag() { return m_bRRDHistoryFlag && m_bComptagesFlag; }
	BOOL GetRecalcHistoBktFlag() { return m_bRecalcHistoBkt; }
	void SetRecalcHistoBktFlag(BOOL bRecalcHistoBkt){m_bRecalcHistoBkt=bRecalcHistoBkt;}
	BOOL GetCompactRRDHistViewFlag() { return m_bCompactRRDHistView; }
	BOOL GetUseBuckEvolAvtRepFlag() { return m_bUseBuckEvolAvtRep; }
	BOOL GetRealTimeInRRDHistFlag() { return m_bRealTimeInRRDHist; }
	BOOL GetHistoDepLastYearFlag() { return m_bHistoDepLastYear; }
	BOOL GetUpdateSynchroneFlag() { /*return m_bUpdateSynchrone;*/ return TRUE; }
	BOOL GetAvecPrixGarantiFlag() { return m_bAvecPrixGaranti; }
	BOOL GetVNTPactifFlag() { return m_bVNTPactif; }
	BOOL GetMultiEntitySelFlag() { return m_bMultiEntitySelFlag; }
	BOOL GetAriDataFlag() { return m_bAriDataFlag; }
	BOOL GetNomosFlag() { return m_bNomosFlag; }
	BOOL GetTopFlag() { return m_bTopFlag; }
	BOOL GetHoiTraFlag() { return m_bHoiTraFlag; }
	BOOL GetTrPrevFlag() { return m_bTrPrevFlag; }
	int GetBitrSurResaGlob() { return m_defSurResaGlobBitr; }
	BOOL GetVNSPactifFlag() { return m_bVNSPactif; } // NPI - DM7838 : souplesse tarifaire
	BOOL GetVNSPdynamiqueFlag() { return m_bVNSPdynamique; } // NPI - DM7838 : souplesse tarifaire
	BOOL GetNiveauSocialFlag() { return m_bNiveauSocialFlag; } // NPI - DM7838 : souplesse tarifaire
	BOOL GetNiveauGammeFlag() { return m_bNiveauGammeFlag; } // NPI - DM7838 : souplesse tarifaire

	CString GetPySelect() { return m_szPySelect; } //DM7978 - KGH - Sphere de prix
	CString GetCorailMdp() { return m_szCorailMdp; } //Ano 110487
	CString GetCorailHistMdp() { return m_szCorailHistMdp; } //Ano 110487

	// DM 7978 - CLE - bouchon vln pour test 
	BOOL GetVnlBouchon()	{return m_bVnlCommBouchon;}
	// DM 7978
	// DM 7978 - LME - bouchon kdisn pour test 
	BOOL GetKdisnBouchon()	{return m_bKdisnCommBouchon;}
	// DM 7978
	// DM 7978 - YI-5841/YI-5842
	/// nombre cc visible dans onglet réduit 
	int GetNbCcVisu() { return m_nbccP; }
	/// nombre sc visible dans onglet réduit 
	int GetNbScVisu() { return m_nbscP; }
	// END DM 7978 - YI-5841/YI-5842
	//FIX # ano hpqc 1066 KGH
	inline std::map <int, CmptOrdre>& GetCmptShowOrder(){ return m_CmptShowOrder;}
	//inline const CArray <CString, CString&>& GetCmptShowOrder(){ return m_CmptShowOrder;}
	//inline const CArray <CString, CString&>& GetCmptScxShowOrder(){ return m_CmptScxShowOrder;}
	inline std::map <int, CmptOrdre>& GetCmptScxShowOrder(){ return m_CmptScxShowOrder;}
	//FIN FIX # ano hpqc 1066 KGH

	void SetRRDHistoryFlag(BOOL bFlag) { m_bRRDHistoryFlag = bFlag; }
	void SetHistoDepLastYearFlag(BOOL bFlag) {m_bHistoDepLastYear = bFlag; }

	int GetGraphRGBValue(CCTAChart::CtaDataStyleIdx eDataStyleIdx);

	BOOL GetYieldRevenuFlag();

	CString GetRhealysIdent() { return m_szRhealysIdent; }

	// DM 7978 : DT 13903 - avt 17
	// Ne controler CC0 == CC1 que si l'indicateur de contrôle est à true
	long GetDateGang() { return m_lDateGang; }
	void SetControlerCC0EgalCC1(bool controlerCC0EgalCC1) {m_controlerCC0EgalCC1 = controlerCC0EgalCC1;};
	bool GetControlerCC0EgalCC1() {return m_controlerCC0EgalCC1;};
	// Fin DM 7978 : DT 13903 - avt 17
	

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCTAGlobalSysParms)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CCTAGlobalSysParms();

protected:

	// Methods to define Client Parameters
	void DefineClientParms();
	void DefineSNCFParms();
	void DefineSNCFTRNParms();
	void DefineThalysParms();
	void DefineMoteurVNLParms();

	// Methods to define Client Colors
	void DefineSNCFColors();
	void DefineSNCFTRNColors();
	void DefineThalysColors();

	// Global System Parameters (exist at only global level)
	double m_dUmLevel;
	double m_dPortNumber;
	BOOL   m_bUpdateSynchrone;
	BOOL   m_bAvecPrixGaranti;
	BOOL   m_bVNTPactif;
	BOOL m_bVNSPactif; // NPI - DM7838 : souplesse tarifaire
	BOOL m_bVNSPdynamique; // NPI - DM7838 : souplesse tarifaire
	CString m_szHostName;
	CString m_szClient;
	CString m_szRhealysIdent;
	CString m_szHistoSelect;		//16/09/10, mis en place pour tester multiconnexion aux bases de données
	CString m_szTgvSelect;        //16/09/10, mis en place pour tester multiconnexion aux bases de données
	CString m_szTgvHistSelect;    //16/09/10, mis en place pour tester multiconnexion aux bases de données
	long  m_defSurResaGlobBitr;		//09/11/11, surresa globale par defaut en bi-tranche
	CString m_szPySelect;			//DM7978 - KGH - Sphere de prix: pour la connexion a la base Py
	CString m_szCorailMdp;			//Ano 110487
	CString m_szCorailHistMdp;			//Ano 110487

	CMapStringToOb m_MaxCmptBkt;
	/// DM-7978 - CLE - nombre de sous contingent par bucket 
	CMapStringToOb m_MaxCmptSci;

	// DM7978 - LME - YI-6038 - ordonnancement de l'affichage des esp phys
	//FIX # ano hpqc 1066  KGH: ajout de la structure CmptOrdre  
	/*CArray <CString, CString&> m_CmptShowOrder;
	CArray <CString, CString&> m_CmptScxShowOrder;*/
	std::map <int, CmptOrdre> m_CmptShowOrder;
	std::map <int, CmptOrdre> m_CmptScxShowOrder;
	//FIX # ano hpqc 1066  KGH

	// Client Parameterization Flags
	BOOL m_bAutoCopyAuthFlag;
	BOOL m_bCompositionFlag;
	BOOL m_bComptagesFlag;
	BOOL m_bEntityDescFlag;
	BOOL m_bForcementFlag;
	BOOL m_bYieldODFlag;		   // Indication de yield à l'OD
	BOOL m_bNiveauInVNLengine;	// Vrai si on met les niveaux dans la fenêtre moteur VNL
	BOOL m_bMasquageCourbe;	    // Vrai si le masquage de courbe est autorisé
	BOOL m_bGraphOutlineFlag;
	BOOL m_bHeaderImageFlag;
	BOOL m_bInfluentFlag;
	BOOL m_bInventoryNestingMethod;
	BOOL m_bMultipleCriticalRulesFlag;
	BOOL m_bRevenuFlag;
	BOOL m_bSupplementTrainFlag;
	BOOL m_bSupplementLevelFlag;
	BOOL m_bNiveauSocialFlag; // NPI - DM7838 : souplesse tarifaire
	BOOL m_bNiveauGammeFlag; // NPI - DM7838 : souplesse tarifaire
	BOOL m_bTGVJumeauxFlag;
	BOOL m_bTrainAdaptFlag;
	BOOL m_bTrancheNoFlag;
	BOOL m_bTRNFlag;
	BOOL m_bSCXFlag;
	BOOL m_bRRDHistoryFlag;
	BOOL m_bControlRRDCurveFlag;
	BOOL m_bRecalcHistoBkt;
	BOOL m_bCompactRRDHistView;
	BOOL m_bUseBuckEvolAvtRep;
	BOOL m_bRealTimeInRRDHist;
	BOOL m_bHistoDepLastYear; // Ajout juste pour delier la courbe de l'année prededente dans historique par depart
	// de l'affichage des historique en général.
	BOOL m_bMultiEntitySelFlag;
	BOOL m_bAriDataFlag;	//présence de données Aristote
	BOOL m_bNomosFlag;    //pour modèle Canadien
	BOOL m_bTopFlag;		//notion de type de train
	BOOL m_bHoiTraFlag;	// notion de train à réservation anticipé (ou a horaire indicatif) : DM 7093
	BOOL m_bTrPrevFlag;   // notion de tranche prévisionnelle : DM 7320

	// DM 7978 - CLE - bouchon comm vnl pour dev
	BOOL m_bVnlCommBouchon;
	// DM 7979 fin
	// DM 7978 - LME - bouchon comm kdisn pour dev
	BOOL m_bKdisnCommBouchon;
	// DM 7979 fin

	int m_SeuilVnauVnax; // nombre de VNAU/VNAS générés par un lot de VNAX, au dela duquel le chargement de
	// la liste des VNAU/VNAS dans la fenêtre de suivi des lots VNAX n'est plus implicite
	int m_nbjoursVisuModifTop; // nombre de jours pendant lequel un TOP modifié s'affiche en vert.
	// DM 7978 - YI-5841/YI-5842
	int m_nbccP;				 // nombre de Classe de Controle par Espace Physique
	int m_nbscP;				 // nombre de Sous Contigent par Espace Physique
	// END DM 7978 - YI-5841/YI-5842
	CUIntArray m_aGraphColors;

	// DM 7978 : DT 13903 - avt 17
	long m_lDateGang;
	bool m_controlerCC0EgalCC1;
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLOBALSYSPARMS_H__6B84F230_E344_11D1_83A7_400020302A31__INCLUDED_)
