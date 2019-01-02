#if !defined(AFX_QUOTASRHEALYSDLG_H__27FF118F_EAD2_46A7_9D77_6D5758BA53BB__INCLUDED_)
#define AFX_QUOTASRHEALYSDLG_H__27FF118F_EAD2_46A7_9D77_6D5758BA53BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CQuotasRhealysDlg;
class YM_Database;
class CVnax;

/////////////////////////////////////////////////////////////////////////////
// CQuotasRhealysDlg dialog
class CQuotasRhealysDlg : public CDialog, public YM_Observer
{
	// Construction
public:
	static CQuotasRhealysDlg* getInstance(YmIcTrancheLisDom* pTranche = NULL);
	static void deleteInstance();
	
	BOOL PreTranslateMessage(MSG* pMsg);//SRE 83311

	afx_msg LONG kbReachedEndList(UINT wParam, LONG lParam); //SRE 76064 Evol

	void InitCtrlQuotasList(); //NCH
	YmIcTrancheLisDom* pTranche;
	
	std::vector<QuotasAutorisationsListView*> orderedView;//SRE 83311

	QuotasAutorisationsListView	m_quotas_view_si_a;
	QuotasAutorisationsListView	m_quotas_view_si_sca;
	QuotasAutorisationsListView	m_quotas_view_sd_a;
	QuotasAutorisationsListView	m_quotas_view_sd_sca;
	QuotasAutorisationsListView	m_quotas_view_sdb_a;
	QuotasAutorisationsListView	m_quotas_view_sdb_sca;


	QuotasAutorisationsListView	m_quotas_view_si_b;
	QuotasAutorisationsListView	m_quotas_view_si_scb;
	QuotasAutorisationsListView	m_quotas_view_sd_b;
	QuotasAutorisationsListView	m_quotas_view_sd_scb;
	QuotasAutorisationsListView	m_quotas_view_sdb_b;
	QuotasAutorisationsListView	m_quotas_view_sdb_scb;

	// Pour le SATC ALLEO (où on n'affiche que la bucket 0), 
	// on a besoin de sauvegarder les buckets/autorisations après le VNL
	// Elles servent seulement au moment de générer le VNAU
	// afin de récupérer les valeurs d'autorisations pour les buckets >= A1/B1
	// HRI, 12/08/2015
	int	m_quotas_si_a_Svg_Alleo[100];
	int m_quotas_sd_a_Svg_Alleo[100];
	int	m_quotas_si_b_Svg_Alleo[100];
	int m_quotas_sd_b_Svg_Alleo[100];
	// NBN
	int	m_quotas_si_sca_Svg_Alleo[100];
	int m_quotas_sd_sca_Svg_Alleo[100];
	int	m_quotas_si_scb_Svg_Alleo[100];
	int m_quotas_sd_scb_Svg_Alleo[100];

	int m_valeurPrecSCA_SI;  // Pour sauver la valeur initiale du SCG ou celle précédemment saisie
	int m_valeurPrecSCB_SI;
	int m_valeurPrecSCA_SD;
	int m_valeurPrecSCB_SD;

	static void EditingMoiCallbackSI(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);
	static void EditingMoiCallbackSD(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);
	static void EditingMoiCallbackDB(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);

	void TransfoForInter();
	void checkScxDB();
	static BOOL CheckDBInput(QuotasAutorisationsListCtrl* list_ctrl_db,QuotasAutorisationsListCtrl* list_ctrl_dbScx, char espace_physique);

	void UpdateListDBScx(QuotasAutorisationsListCtrl* list_ctrl, long indexation, long& valeur);
	void UpdateListScx(QuotasAutorisationsListCtrl* list_ctrl, long indexation, long& valeur, bool bSi);
	void cumulEtanche(QuotasAutorisationsListCtrl* list, int& sumAuth, int& sumDisp);
	void cumulVente(QuotasAutorisationsListCtrl* list, int& sumVente);

	// NBN
	static void EditingMoiCallbackSISC(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);
	static void EditingMoiCallbackSDSC(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);
	static void EditingMoiCallbackDBSC(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);
	//void UpdateListSISCcc0(QuotasAutorisationsListCtrl* list_ctrl_sd, QuotasAutorisationsListCtrl* list_ctrl_si, QuotasAutorisationsListCtrl* list_ctrl_sdb, long indexation, long& valeur);
	//void UpdateListSDSCcc0(QuotasAutorisationsListCtrl* list_ctrl_sd, QuotasAutorisationsListCtrl* list_ctrl_si, QuotasAutorisationsListCtrl* list_ctrl_sdb, long indexation, long& valeur);
	//void UpdateListSDBSC(QuotasAutorisationsListCtrl* list_ctrl_si, QuotasAutorisationsListCtrl* list_ctrl_sd, QuotasAutorisationsListCtrl* list_ctrl_sdb, long indexation, long& valeur);
	
	void UpdateListSIcc0(QuotasAutorisationsListCtrl* list_ctrl_sd, QuotasAutorisationsListCtrl* list_ctrl_si, QuotasAutorisationsListCtrl* list_ctrl_sdb, long indexation, long& valeur, CString espace_physique);
	void UpdateListSDcc0(QuotasAutorisationsListCtrl* list_ctrl_sd, QuotasAutorisationsListCtrl* list_ctrl_si, QuotasAutorisationsListCtrl* list_ctrl_sdb, long indexation, long& valeur, CString espace_physique);
	void UpdateListSDB(QuotasAutorisationsListCtrl* list_ctrl_si, QuotasAutorisationsListCtrl* list_ctrl_sd, QuotasAutorisationsListCtrl* list_ctrl_sdb, long indexation, long& valeur);
    void UpdateListccX(QuotasAutorisationsListCtrl* list_ctrl, long indexation, long& valeur, BOOL &bFlagModif,char espace_physique);
    void UpdateDispo(QuotasAutorisationsListCtrl* list,char espace_physique);

	int getMaxAuthScxControl(QuotasAutorisationsListCtrl* ctrl);
	void OnMajAuthoSIA();
	void OnMajAuthoSIB();
	void OnMajAuthoSDA();
	void OnMajAuthoSDB();
	void raboterSCX(QuotasAutorisationsListCtrl* ctrl , long maValue);

	int i;

	// NBN : différencier le cumul étanche sur un seul scx et sur l'ensemble des scx
	// Nouvelles méthodes : ci-dessous, elle traite l'ensemble des SCX
	void SommeGlobaleSIA(int &sumAuthEtanche, int &sumDispEtanche);
	void SommeGlobaleSIB(int &sumAuthEtanche, int &sumDispEtanche);
	void SommeGlobaleSDA(int &sumAuthEtanche, int &sumDispEtanche);
	void SommeGlobaleSDB(int &sumAuthEtanche, int &sumDispEtanche);
	void SommeGlobaleSCX(QuotasAutorisationsListCtrl* list, int& sumAuth, int& sumDisp);

	//  Variable
	CObArray m_VnauList;
	RWDate m_rwBegDate;
	unsigned long m_lDateDep;
	long m_lRrdIndex;
	CVnax*		vnaxref;
	int m_trancheno1,m_trancheno2,m_trancheno3;
	int m_nbTrancheOK;
	YM_VnlCmpt* m_pVnlCmpt;
	YM_VnlBkt*  m_pVnlBkt;
	YM_VnlSci*  m_pVnlScx;
	int m_iSomme;
	int m_iscSomme;
	int m_iDiffTotale;
	BOOL m_bFlagEcart;
	BOOL m_bOvbAdjustFlag;
	int m_iDiff;
	int m_iCapaA,m_iCapaB;
	BOOL m_FlagTroncon;
	int m_Un_Troncon;
	int m_Tous_Troncon;
	int m_Sel_Troncon;
	int m_iTroncon;
	int m_SI_AGrp;
	int m_SI_BGrp;
	int m_SD_AGrp;
	int m_SD_BGrp;
	int m_nbBktVnlA; // HRI, 07/08/2015
	int m_nbBktVnlB; // HRI, 07/08/2015
	int m_nbScxVnlA; // NBN
	int m_nbScxVnlB; // NBN
	BOOL bSICopieFlagModif;  // Modif sur tranche internationale pour copie
	BOOL bSDCopieFlagModif;  // Modif sur tranche domestique pour copie
	BOOL bDBCopieFlagModif;  // Modif sur tranche domestique pour copie
	BOOL bDBScaCopieFlagModif;  // Modif sur tranche domestique pour copie
	BOOL bDBScbCopieFlagModif;  // Modif sur tranche domestique pour copie
	BOOL bSIFlagModif;  // Modif sur tranche internationale
	BOOL bSDFlagModif;  // Modif sur tranche domestique
	BOOL bDBFlagModif;  // Modif sur tranche domestique
	BOOL bSISCAFlagModif;  // Modif SC sur tranche internationale
	BOOL bSISCBFlagModif;  // Modif SC sur tranche internationale
	BOOL bSDSCAFlagModif;  // Modif SC sur tranche domestique
	BOOL bSDSCBFlagModif;  // Modif SC sur tranche domestique
	BOOL bDBSCAFlagModif;  // Modif SC sur tranche domestique
	BOOL bDBSCBFlagModif;  // Modif SC sur tranche domestique
	BOOL bAllFlagModif;
	BOOL bFlagModifManuelle;  // Si modif manuelle demandée (détecter cas où les auto des domestiques sont forcés à 0 automatiquement)
	BOOL m_bFlag;
	CString m_sPeakLeg;
	CString m_szTitle;
	BOOL bFlagAppel;
	BOOL bBucket;
	int m_VnlCount;
	BOOL bVnlDeControle;
	// NBN ANO 83648 - Ajou de deux variables contenant la somme des vente en A et B
    // Utilise dans le cas d'ALLEo car visibilite uniquement du CC0
    int iSommeVentesA;
    int iSommeVentesB;

	//  Méthode
	//void BlancFondStatic();
	void RecapDispoVert();
	CString     MakeStrDate (RW4Byte date, BOOL bannee = FALSE);
	CString     GetFirstLine(CString& msg);
	BOOL GetTrancheNo (CString msg); // recupere le numéro de tranche dans réponse KDISN et envoi VNL
	BOOL LireTrancheNo (bool &badtrain); // récupération des numéro de tranches en base de données.
	// badtrain revient à true si train trouvé, mais pas bi-tranche
	void StartSendVnl(CString sTronconVisu);
	void SendNextVnl();
	void LireVnl(CString reponse);
	void FinVnl();
	void AnalyseVnlSI(CString sOrigine,YM_Vnl &pVnlSI);
	void AnalyseVnlSD(CString sOrigine,YM_Vnl &pVnlSD);
	void AnalyseVnlDB(CString sOrigine,YM_Vnl &pVnlDB);

	void ShowOld(int mode);
	void SwitchA();
	void SwitchB();
	void ShowStaticLabel(int mode);
	void ShowDynamicA(int mode);
	void ShowDynamicB(int mode);
	void ShowHideForAlleo(int mode);

	void SIRemplissageA();
	void SIRemplissageFinA();
	void SIRemplissageB();
	void SIRemplissageFinB();
	void SDRemplissageA();
	void SDRemplissageFinA();
	void SDRemplissageB();
	void SDRemplissageFinB();
	void DBRemplissageA();
	void DBRemplissageB();
	// NBN
	void SISCRemplissageA();
	void SISCRemplissageFinA();
	void SISCRemplissageB();
	void SISCRemplissageFinB();
	void SDSCRemplissageA();
	void SDSCRemplissageFinA();
	void SDSCRemplissageB();
	void SDSCRemplissageFinB();
	void DBSCRemplissageA();
	void DBSCRemplissageB();

	// Recap
	void RecapRemplissage();
	void GrpRemplissage();
	void clearListControl();

	// Troncon
	void VerifFlagTroncon();
	void GrisageTroncon();
	BOOL GetSelTroncon(CString sChoix, CString &sListTroncon);
	void OnMajTroncon();

	//	VNAU
	void BuildVnau(CString sListTroncon,BOOL bSIFlagModif,BOOL bSDFlagModif,BOOL bDBFlagModif, BOOL dbScaModif, BOOL dbScbModif, BOOL isACopie);
	BOOL MessageNbVnau(CString sListTroncon);
	void Chercher();
	void SetTitle(CString NewTitle);

	void MajAuthSD(int NoTranche, CString sLegOrig, CString sCmpt);
	void MajAuthSI(int NoTranche, CString sLegOrig, CString sCmpt);
	void MajAuthDB(int NoTranche, CString sLegOrig, CString sCmpt);
	void MajAuthSDSC(int NoTranche, CString sLegOrig, CString sCmpt);
	void MajAuthSISC(int NoTranche, CString sLegOrig, CString sCmpt);
	void MajAuthDBSC(int NoTranche, CString sLegOrig, CString sCmpt);
	// NBN ANO 94818 - Ajout de méthode pour maj base de données AUTH_USER pour SATC ALLEO
	void MajAuthSDRhealys(int NoTranche, CString sLegOrig, CString sCmpt);
	void MajAuthSIRhealys(int NoTranche, CString sLegOrig, CString sCmpt);
	void MajAuthSCRhealys(int lesAuthos[], int NoTranche, CString sLegOrig, CString sCmpt);

	void TabTronconSIA(int iCount,CString sLegOrig,long lAuth, long lDisp);
	void TabTronconSIB(int iCount,long lAuth, long lDisp);
	void TabTronconSDA(int iCount,long lAuth, long lDisp);
	void TabTronconSDB(int iCount,long lAuth, long lDisp);
	void TabTronconDBA(int iCount,long lAuth, long lDisp);
	void TabTronconDBB(int iCount,long lAuth, long lDisp);
	void UnTotal (YM_Static &m_Total, int caparef, int Som);
	void TabTroncon_A_Total();
	void TabTroncon_B_Total();
	void TabTronconHide();
	void HideScSiDbForAlleo();
	// Dialog Data
	//{{AFX_DATA(CQuotasRhealysDlg)
	enum { IDD = IDD_Quotas_RHEALYS };
	//--SNCF INTERNATIONAL
	YM_Static m_SI_A_RES_TOT;
	YM_Static m_SI_SCA_RES_TOT;
	YM_Static m_SI_A_DIFF;
	YM_Static m_SI_A_GRP;
	YM_Static m_SI_B_RES_TOT;
	YM_Static m_SI_SCB_RES_TOT;
	YM_Static m_SI_B_DIFF;
	YM_Static m_SI_B_GRP;

	YM_Static m_SIA_SCG_ACT;
    YM_Static m_SIB_SCG_ACT;
    YM_Static m_SIA_SCG_MOI;
    YM_Static m_SIB_SCG_MOI;
    YM_Static m_SDA_SCG_ACT;
    YM_Static m_SDB_SCG_ACT;
    YM_Static m_SDA_SCG_MOI;
    YM_Static m_SDB_SCG_MOI;

	//NCH CONTROLES
	YM_Static m_SD_A_RES_TOT;
	YM_Static m_SD_SCA_RES_TOT;
	YM_Static m_SD_A_DIFF;
	YM_Static m_SD_A_GRP;
	YM_Static m_SD_B_RES_TOT;
	YM_Static m_SD_SCB_RES_TOT;
	YM_Static m_SD_B_DIFF;
	YM_Static m_SD_B_GRP;

	//NCH DB
	YM_Static m_DB_A_DIFF;
	YM_Static m_DB_B_DIFF;
	//NCH END

	//--RECAP
	YM_Static	m_Zone_Recap;
	YM_Static	m_RECAP_OFFRE;
	YM_Static	m_RECAP_ACT;
	YM_Static	m_RECAP_ECART;
	YM_Static	m_RECAP_MOI;
	YM_Static	m_RECAP_CAPA;
	YM_Static	m_RECAP_L1;
	YM_Static	m_RECAP_ACT1;
	YM_Static	m_RECAP_ECART1;
	YM_Static	m_RECAP_MOI1;
	YM_Static	m_RECAP_CAPA1;
	YM_Static	m_RECAP_L2;
	YM_Static	m_RECAP_ACT2;
	YM_Static	m_RECAP_ECART2;
	YM_Static	m_RECAP_MOI2;
	YM_Static	m_RECAP_CAPA2;
	YM_Static	m_RECAP_L3;
	YM_Static	m_RECAP_ACT3;
	YM_Static	m_RECAP_ECART3;
	YM_Static	m_RECAP_MOI3;
	YM_Static	m_RECAP_CAPA3;
	YM_Static	m_CapaA;
	YM_Static	m_CapaB;
	YM_Static	m_iCapaMoi_A;
	YM_Static	m_iCapaMoi_B;

	//--CHERCHER
	CString	    m_NumeroTrain;
	CString	    m_Origine;
	CString	    m_Destination;
	CTime	        m_startdate;
	CDateTimeCtrl	m_ctlStartDate;
	CListBox    m_ListTroncon;
	CComboBox   m_ComboTroncon;

	//--TITRE
	YM_Static   m_SI;
	YM_Static   m_SD;
	YM_Static   m_DB;

	YM_Static   m_DO;
	YM_Static   m_SI_SC;
	YM_Static   m_SD_SC;

	YM_Static   m_DO_CC;

	//--Tableau des Troncons
	YM_Static	m_Troncon_Col_1;
	YM_Static   m_Troncon_Col_2;
	YM_Static   m_Troncon_Col_3;               
	YM_Static   m_Troncon_Col_4;
	YM_Static   m_Troncon_Col_5;               
	YM_Static   m_Troncon_Col_6;               
	YM_Static   m_Troncon_Col_7;               
	YM_Static   m_Troncon_Col_8;               
	YM_Static   m_Troncon_Col_9;               
	YM_Static   m_Troncon_Col_10;              
	YM_Static   m_Troncon_Col_11;              
	YM_Static   m_Troncon_Col_12;                       
	YM_Static   m_SNI_A0_1;                    
	YM_Static   m_SNI_A0_2;                     
	YM_Static   m_SNI_A0_3;                    
	YM_Static   m_SNI_A0_4;                    
	YM_Static   m_SNI_A0_5;                    
	YM_Static   m_SNI_A0_6;                    
	YM_Static   m_SNI_A0_7;                    
	YM_Static   m_SNI_A0_8;                    
	YM_Static   m_SNI_A0_9;                    
	YM_Static   m_SNI_A0_10;                   
	YM_Static   m_SNI_A0_11;                   
	YM_Static   m_SNI_A0_12;                                   
	YM_Static   m_SND_A0_1;                   
	YM_Static   m_SND_A0_2;                     
	YM_Static   m_SND_A0_3;                    
	YM_Static   m_SND_A0_4;                    
	YM_Static   m_SND_A0_5;                    
	YM_Static   m_SND_A0_6;                    
	YM_Static   m_SND_A0_7;                    
	YM_Static   m_SND_A0_8;                    
	YM_Static   m_SND_A0_9;                    
	YM_Static   m_SND_A0_10;                   
	YM_Static   m_SND_A0_11;                   
	YM_Static   m_SND_A0_12;                                  
	YM_Static   m_DB_A0_1;                     
	YM_Static   m_DB_A0_2;                     
	YM_Static   m_DB_A0_3;                     
	YM_Static   m_DB_A0_4;                     
	YM_Static   m_DB_A0_5;                     
	YM_Static   m_DB_A0_6;                     
	YM_Static   m_DB_A0_7;                     
	YM_Static   m_DB_A0_8;                     
	YM_Static   m_DB_A0_9;                     
	YM_Static   m_DB_A0_10;                    
	YM_Static   m_DB_A0_11;                    
	YM_Static   m_DB_A0_12;                                        

	YM_Static   m_SNI_DA0_1;                    
	YM_Static   m_SNI_DA0_2;                     
	YM_Static   m_SNI_DA0_3;                    
	YM_Static   m_SNI_DA0_4;                    
	YM_Static   m_SNI_DA0_5;                    
	YM_Static   m_SNI_DA0_6;                    
	YM_Static   m_SNI_DA0_7;                    
	YM_Static   m_SNI_DA0_8;                    
	YM_Static   m_SNI_DA0_9;                    
	YM_Static   m_SNI_DA0_10;                   
	YM_Static   m_SNI_DA0_11;                   
	YM_Static   m_SNI_DA0_12;                                   
	YM_Static   m_SND_DA0_1;                   
	YM_Static   m_SND_DA0_2;                     
	YM_Static   m_SND_DA0_3;                    
	YM_Static   m_SND_DA0_4;                    
	YM_Static   m_SND_DA0_5;                    
	YM_Static   m_SND_DA0_6;                    
	YM_Static   m_SND_DA0_7;                    
	YM_Static   m_SND_DA0_8;                    
	YM_Static   m_SND_DA0_9;                    
	YM_Static   m_SND_DA0_10;                   
	YM_Static   m_SND_DA0_11;                   
	YM_Static   m_SND_DA0_12;                                  
	YM_Static   m_DB_DA0_1;                     
	YM_Static   m_DB_DA0_2;                     
	YM_Static   m_DB_DA0_3;                     
	YM_Static   m_DB_DA0_4;                     
	YM_Static   m_DB_DA0_5;                     
	YM_Static   m_DB_DA0_6;                     
	YM_Static   m_DB_DA0_7;                     
	YM_Static   m_DB_DA0_8;                     
	YM_Static   m_DB_DA0_9;                     
	YM_Static   m_DB_DA0_10;                    
	YM_Static   m_DB_DA0_11;                    
	YM_Static   m_DB_DA0_12;                                        

	YM_Static   m_Total_A0_1;                  
	YM_Static   m_Total_A0_2;                  
	YM_Static   m_Total_A0_3;                  
	YM_Static   m_Total_A0_4;                  
	YM_Static   m_Total_A0_5;                  
	YM_Static   m_Total_A0_6;                  
	YM_Static   m_Total_A0_7;                  
	YM_Static   m_Total_A0_8;                  
	YM_Static   m_Total_A0_9;                  
	YM_Static   m_Total_A0_10;                 
	YM_Static   m_Total_A0_11;                 
	YM_Static   m_Total_A0_12;                               
	YM_Static   m_SNI_B0_1;                    
	YM_Static   m_SNI_B0_2;                    
	YM_Static   m_SNI_B0_3;                    
	YM_Static   m_SNI_B0_4;                    
	YM_Static   m_SNI_B0_5;                    
	YM_Static   m_SNI_B0_6;                    
	YM_Static   m_SNI_B0_7;                    
	YM_Static   m_SNI_B0_8;                    
	YM_Static   m_SNI_B0_9;                    
	YM_Static   m_SNI_B0_10;                   
	YM_Static   m_SNI_B0_11;                   
	YM_Static   m_SNI_B0_12;                              
	YM_Static   m_SND_B0_1;                    
	YM_Static   m_SND_B0_2;                    
	YM_Static   m_SND_B0_3;                    
	YM_Static   m_SND_B0_4;                    
	YM_Static   m_SND_B0_5;                    
	YM_Static   m_SND_B0_6;                    
	YM_Static   m_SND_B0_7;                    
	YM_Static   m_SND_B0_8;                    
	YM_Static   m_SND_B0_9;                    
	YM_Static   m_SND_B0_10;                   
	YM_Static   m_SND_B0_11;                   
	YM_Static   m_SND_B0_12;                                     
	YM_Static   m_DB_B0_1;                     
	YM_Static   m_DB_B0_2;                     
	YM_Static   m_DB_B0_3;                     
	YM_Static   m_DB_B0_4;                     
	YM_Static   m_DB_B0_5;                     
	YM_Static   m_DB_B0_6;                     
	YM_Static   m_DB_B0_7;                     
	YM_Static   m_DB_B0_8;                     
	YM_Static   m_DB_B0_9;                     
	YM_Static   m_DB_B0_10;                    
	YM_Static   m_DB_B0_11;                    
	YM_Static   m_DB_B0_12; 

	YM_Static   m_SNI_DB0_1;                    
	YM_Static   m_SNI_DB0_2;                    
	YM_Static   m_SNI_DB0_3;                    
	YM_Static   m_SNI_DB0_4;                    
	YM_Static   m_SNI_DB0_5;                    
	YM_Static   m_SNI_DB0_6;                    
	YM_Static   m_SNI_DB0_7;                    
	YM_Static   m_SNI_DB0_8;                    
	YM_Static   m_SNI_DB0_9;                    
	YM_Static   m_SNI_DB0_10;                   
	YM_Static   m_SNI_DB0_11;                   
	YM_Static   m_SNI_DB0_12;                              
	YM_Static   m_SND_DB0_1;                    
	YM_Static   m_SND_DB0_2;                    
	YM_Static   m_SND_DB0_3;                    
	YM_Static   m_SND_DB0_4;                    
	YM_Static   m_SND_DB0_5;                    
	YM_Static   m_SND_DB0_6;                    
	YM_Static   m_SND_DB0_7;                    
	YM_Static   m_SND_DB0_8;                    
	YM_Static   m_SND_DB0_9;                    
	YM_Static   m_SND_DB0_10;                   
	YM_Static   m_SND_DB0_11;                   
	YM_Static   m_SND_DB0_12;                                     
	YM_Static   m_DB_DB0_1;                     
	YM_Static   m_DB_DB0_2;                     
	YM_Static   m_DB_DB0_3;                     
	YM_Static   m_DB_DB0_4;                     
	YM_Static   m_DB_DB0_5;                     
	YM_Static   m_DB_DB0_6;                     
	YM_Static   m_DB_DB0_7;                     
	YM_Static   m_DB_DB0_8;                     
	YM_Static   m_DB_DB0_9;                     
	YM_Static   m_DB_DB0_10;                    
	YM_Static   m_DB_DB0_11;                    
	YM_Static   m_DB_DB0_12; 

	YM_Static   m_Total_B0_1;                  
	YM_Static   m_Total_B0_2;                  
	YM_Static   m_Total_B0_3;                  
	YM_Static   m_Total_B0_4;                  
	YM_Static   m_Total_B0_5;                  
	YM_Static   m_Total_B0_6;                  
	YM_Static   m_Total_B0_7;                  
	YM_Static   m_Total_B0_8;                  
	YM_Static   m_Total_B0_9;                  
	YM_Static   m_Total_B0_10;                 
	YM_Static   m_Total_B0_11;                 
	YM_Static   m_Total_B0_12;                 
	YM_Static   m_A0_Capa;
	YM_Static   m_B0_Capa;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuotasRhealysDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQuotasRhealysDlg)
	//	afx_msg void OnSelchangeComboTroncon();
	afx_msg void OnChercher();
	afx_msg void OnQuitter();
	afx_msg void OnUnTroncon();
	afx_msg void OnTousTroncon();
	afx_msg void OnSelTroncon();
	afx_msg void OnKeepEcart();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSetFocus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void OnNotify( YM_Observable* pObject );
	void PurgeVnauList();
	void SendNextVnau();
	void InitSendVnau();
	void FinSendVnau();
	bool TronconInter(); // renvoi vrai si la tronçon courant est "international"
	bool TronconDE(); // renvoi vrai si la tronçon courant est "international"
	bool TronconDE(CString sSelTroncon);
	void masquerBucketForRhealys (); // masquer info bucket >= 1 si client Rhealys
	// Evolution de la méthode ci-dessous : HRI, 12/08/2015 (cas particulier du SATC ALLEO)
	void PutRhealysVnau (CString svnau, CString sTroncon, int nature);
	// Pour les VNAU du SATC Alleo des tranches internationales, il faut retrouver
	// les CC A1 à An et B1 à Bn qui sont masquées, mais doivent apparaître dans le VNL.
	CString ComplementVNAUAlleo(char cmpt, int listAutoSvg[], long authCC0);
	CString rabotageScx(char cmpt, int listAutoSvg[], long authCC0);

	void SetAuthFromBaseInVnl (YM_Vnl* pVnl, int nature);
	void SetAuthScxFromBaseInVnl (YM_Vnl* pVnl, int nature);

	// OHA - Ano 77962 
	// indicateur de controle : si CC0 doit etre egal a CC1
	BOOL controlerCC0EgalCC1;
	

    // NBN ANO 82483 : la copie d'un troncon inter vers frontalier n'est pas autorisée 
    bool ControlCopieInterOnly (CString sListTroncon);
	bool ControlCrossFrontier (CString sListTroncon); // Controle que la liste de tronçon est soit totalement francaise,
	// Permet de savoir pour ALLEO si la CCO est inferieure à la SCG
	bool CheckRhealysCC0(int valueCC0, int valueSCG);

	int SumBkt0A;
	int SumBkt0B;
	int SumCapA;
	int SumCapB;

	CString sLastTroncon; // dernier troncon passé en parametre de la méthode SendVnlLock
	BOOL m_bSNDetranger;

	CQuotasRhealysDlg();   // standard constructor
	CQuotasRhealysDlg(YmIcTrancheLisDom* pTranche);
	~CQuotasRhealysDlg();

		static CQuotasRhealysDlg* m_pInstance;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
