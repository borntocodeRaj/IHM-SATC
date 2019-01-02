#if !defined(AFX_QUOTASBiTrancheDLG_H__27FF118F_EAD2_46A7_9D77_6D5758BA53BB__INCLUDED_)
#define AFX_QUOTASBiTrancheDLG_H__27FF118F_EAD2_46A7_9D77_6D5758BA53BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CQuotasBiTrancheDlg;
class YM_Database;
class CVnax;

/////////////////////////////////////////////////////////////////////////////
// CQuotasBiTrancheDlg dialog
class CQuotasBiTrancheDlg : public CDialog, public YM_Observer
{
protected:
	BOOL PreTranslateMessage(MSG* msg);
// Construction
public:
	//CQuotasBiTrancheDlg();   // standard constructor
	//CQuotasBiTrancheDlg(YmIcTrancheLisDom* pTranche);
	//~CQuotasBiTrancheDlg();

	static CQuotasBiTrancheDlg* getInstance(YmIcTrancheLisDom* pTranche = NULL);
	static void deleteInstance();

	YmIcTrancheLisDom* pTranche;
	
	std::vector<QuotasAutorisationsListView*> orderedView;//SRE 83311 - hold list order

	QuotasAutorisationsListView	m_quotas_view_si_cc_a;
	QuotasAutorisationsListView	m_quotas_view_sd_cc_a;

	QuotasAutorisationsListView	m_quotas_view_si_scx_a;
	QuotasAutorisationsListView	m_quotas_view_sd_scx_a;

	QuotasAutorisationsListView	m_quotas_view_si_cc_b;
	QuotasAutorisationsListView	m_quotas_view_sd_cc_b;

	QuotasAutorisationsListView	m_quotas_view_si_scx_b;
	QuotasAutorisationsListView	m_quotas_view_sd_scx_b;

	afx_msg LONG kbReachedEndList(UINT wParam, LONG lParam); //SRE 76064 Evol
	
	/// callback edition SI MOI CC pour nest level 0 
	static void EditingSIMoiCallbackCC0(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);
	/// callback edition SI MOI CC pour nest level different de 0
	static void EditingSIMoiCallbackCC(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);
	/// callback edition SD MOI CC pour nest level 0 
	static void EditingSDMoiCallbackCC0(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);
	/// callback edition SD MOI CC pour nest level different 0
	static void EditingSDMoiCallbackCC(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);

	/// callback edition SD MOI SCX pour nest level different de 0 (equivalent a ScDA ou ScDB 
	static void EditingSDMoiCallbackSCX(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);
	/// callback edition SD MOI SCX pour nest level 0 
	static void EditingSDMoiCallbackSCX0(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);
	/// callback edition SI MOI SCX pour nest level different de 0 
	static void EditingSIMoiCallbackSCX(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);
	/// callback edition SI MOI SCX pour nest level 0 
	static void EditingSIMoiCallbackSCX0(quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);


	void UpdateListCc(QuotasAutorisationsListCtrl* list_ctrl, long indexation, long& valeur);
	// NPI - Ano 70130
	/*void UpdateListSICc0(QuotasAutorisationsListCtrl* list_ctrl_cc_si, QuotasAutorisationsListCtrl* list_ctrl_cc_sd, QuotasAutorisationsListCtrl* list_ctrl_scx, long indexation, long& valeur);
	void UpdateListSDCc0(QuotasAutorisationsListCtrl* list_ctrl_cc_sd, QuotasAutorisationsListCtrl* list_ctrl_cc_si, QuotasAutorisationsListCtrl* list_ctrl_scx, long indexation, long& valeur);*/
	void UpdateListSICc0(QuotasAutorisationsListCtrl* list_ctrl_cc_si, QuotasAutorisationsListCtrl* list_ctrl_cc_sd, long indexation, long& valeur);
	void UpdateListSDCc0(QuotasAutorisationsListCtrl* list_ctrl_cc_sd, QuotasAutorisationsListCtrl* list_ctrl_cc_si, long indexation, long& valeur);
	///

	void UpdateListScx(QuotasAutorisationsListCtrl* list_ctrl, long indexation, long& valeur, bool bSi);
	void UpdateListSISc0(QuotasAutorisationsListCtrl* list_ctrl_si_scx, QuotasAutorisationsListCtrl* list_ctrl_si, long indexation, long& valeur);
	void UpdateListSDSc0(QuotasAutorisationsListCtrl* list_ctrl_sd_scx, QuotasAutorisationsListCtrl* list_ctrl_sd, long indexation, long& valeur);
	
	//void clearList();
	void clearListControl();

	void InitCtrlQuotasList();
	//void ShowOld(int mode);
	void SwitchA();
	void SwitchB();
	//void SwitchOld();
	
	void ShowStaticLabel(int mode);
	void ShowDynamicA(int mode);
	void ShowDynamicB(int mode);

	void SIRemplissageA();
	void SIRemplissageB();

	/// DM-7978 - CLE
	void SIRecalculSommeA();
	void SIRecalculSommeB();

	// NPI - Ano 70130
	//void cumulVenteEtEtanche(QuotasAutorisationsListCtrl* list, int& sumVente, int& sumAuth, int& sumDisp);
	void cumulEtanche(QuotasAutorisationsListCtrl* list, int& sumAuth, int& sumDisp);
	void cumulVente(QuotasAutorisationsListCtrl* list, int& sumVente);
	///
	bool ctrlMaxAuthSci(QuotasAutorisationsListCtrl* list, int indexation, int maxVal);

	void OnUpdateDispoCC(QuotasAutorisationsListCtrl* list, bool bSi);

	/// DM-7978 - CLE

	void SDRemplissage(QuotasAutorisationsListCtrl& ctrl);
	void SDRemplissageA();
	void SDRemplissageB();

	void SCIRemplissageA();
	void SCIRemplissageB();

	void SCDRemplissage(QuotasAutorisationsListCtrl& ctrl);
	void SCDRemplissageA();
	void SCDRemplissageB();

	// Rajout HRI (13/08/2015) sur le contrôle des SCG
	void OnMajSCIAG();
	void OnMajSCIBG();
	void OnMajSCDAG();
	void OnMajSCDBG();
	bool ControleScIAG();
	bool ControleScIBG();
	bool ControleScDAG();
	bool ControleScDBG();
	int m_valeurPrecAG_I;  // Pour sauver la valeur initiale du SCG ou celle précédemment saisie
	int m_valeurPrecBG_I;
	int m_valeurPrecAG_D;
	int m_valeurPrecBG_D;
	// Fin modif HRI pour gestion SCG


//  Variable
	CObArray m_VnauList;
    RWDate m_rwBegDate;
	unsigned long m_lDateDep;
	long m_lRrdIndex;
    CVnax*		vnaxref;
    int m_trancheno1,m_trancheno2;
    int m_nbTrancheOK;
	YM_VnlCmpt* m_pVnlCmpt;
  YM_VnlBkt*  m_pVnlBkt;
	YM_VnlSci*  m_pVnlSci;
    int m_iSomme;
    int m_iDiffTotale;
    BOOL m_bFlagEcart;
    BOOL m_bOvbAdjustFlag;
	int m_iDiff;
	int m_iCapaA,m_iCapaB;
    BOOL m_FlagTroncon;
	BOOL m_FlagSousResa;
	int m_Un_Troncon;
	int m_Tous_Troncon;
	int m_Sel_Troncon;
	int m_iTroncon;
	int m_SI_AGrp;
	int m_SI_BGrp;
	int m_SD_AGrp;
	int m_SD_BGrp;
	int m_nbBktVnlCC_A; // HRI, 11/08/2015
	int m_nbBktVnlCC_B; // HRI, 11/08/2015
	int m_nbBktVnlSC_A; // HRI, 11/08/2015
	int m_nbBktVnlSC_B; // HRI, 11/08/2015
	BOOL bSIFlagModif;   // Si modification demandée CC A ou B tranche internationale
	BOOL bSDFlagModif;   // Si modification demandée CC A ou B tranche domestique
	BOOL bSCIAFlagModif; // Si modification demandée SC A tranche internationale
	BOOL bSCDAFlagModif; // Si modification demandée SC A tranche domestique
	BOOL bSCIBFlagModif; // Si modification demandée SC B tranche internationale
	BOOL bSCDBFlagModif; // Si modification demandée SC B tranche domestique
	BOOL bAllFlagModif;
	BOOL bFlagModifManuelle;  // Si modif manuellle demandée (détecter cas où les auto des domestiques sont forcés à 0 automatiquement)
    BOOL m_bFlag;
    CString m_sPeakLeg;
    CString m_szTitle;
	BOOL bFlagAppel;
	BOOL bBucket;
	int m_VnlCount;
	BOOL bVnlDeControle;
	CString	    m_NumeroTrain;
	CString	    m_Origine;
    CString	    m_Destination;

  //  Méthode
	void BlancFondStatic();
	void RecapDispoVert();
	void SciFondStatic();
    CString     MakeStrDate (RW4Byte date, BOOL bannee = FALSE);
	CString     GetFirstLine(CString& msg);
    BOOL GetTrancheNo (CString msg); // recupere le numéro de tranche dans réponse KDISN et envoi VNL
	BOOL LireTrancheNo (bool &badtrain); // récupération des numéro de tranches en base de données.
										 // badtrain revient à true si train trouvé, mais pas bi-tranche
	void StartSendVnl(const CString& sTronconVisu);
	void SendNextVnl();
	void LireVnl(const CString& reponse);
	void FinVnl();
    void AnalyseVnlSI(const CString& sOrigine,YM_Vnl &pVnlSI);
    void AnalyseVnlSD(const CString& sOrigine,YM_Vnl &pVnlSD);

// SI Cmpt A 
	void SIRemplissageFinA();

// SI Cmpt B
	void SIRemplissageFinB();
	
// SD Cmpt A
    void SDRemplissageFinA();
	
// SD Cmpt B
    void SDRemplissageFinB();

// Recap
	void RecapRemplissage();
	void GrpRemplissage();

// Methode de mise à jour des valeurs de bucket suivantes
	void VerifFlagTroncon();
	void GrisageTroncon();
	void GrisageVnauVnas();
    BOOL GetSelTroncon(const CString& sChoix, CString &sListTroncon);
    void OnMajTroncon();
    
	void BuildVnau(const CString& sListTroncon);
	//void BuildVnas(CString sListTroncon);
	BOOL MessageNbVnauVnas(const CString& sListTroncon);
	BOOL CtrlSousResaOK();
	void Chercher();
	void SetTitle(const CString& NewTitle);
	
	void MajAuthSD(int NoTranche, const CString& sLegOrig, const CString& sCmpt);
	void MajAuthSI(int NoTranche, const CString& sLegOrig, const CString& sCmpt);

	void MajAuthSciSD(int NoTranche, const CString& sLegOrig, const CString& sCmpt);
	void MajAuthSciSI(int NoTranche, const CString& sLegOrig, const CString& sCmpt);
	/// DM-7978 - CLE
	void raboterSCX(QuotasAutorisationsListCtrl* ctrl , long maValue);
    void TabTronconSIA(int iCount,const CString& sLegOrig,long lAuth, long lDisp);
	void TabTronconSIB(int iCount,long lAuth, long lDisp);
	void TabTronconSDA(int iCount,long lAuth, long lDisp);
	void TabTronconSDB(int iCount,long lAuth, long lDisp);
	void UnTotal (YM_Static &m_Total, int caparef, int maxref, int Som);
	void GlobTotal (YM_Static &m_TotalA, YM_Static &m_TotalB);
	void TabTroncon_A_Total();
	void TabTroncon_B_Total();
	void TabTroncon_Glob_Total();
    void TabTronconHide();
	// Dialog Data
	//{{AFX_DATA(CQuotasBiTrancheDlg)
    enum { IDD = IDD_QUOTAS_BITRANCHE };
//--SNCF INTERNATIONAL
	YM_Static m_SD_A_CC_RES_TOT;
	YM_Static m_SI_A_CC_RES_TOT;
	YM_Static m_SD_B_CC_RES_TOT;
	YM_Static m_SI_B_CC_RES_TOT;
	YM_Static m_SI_A_SCX_RES_TOT;
	YM_Static m_SD_A_SCX_RES_TOT;
	YM_Static m_SI_B_SCX_RES_TOT;
	YM_Static m_SD_B_SCX_RES_TOT;
	YM_Static m_IDC_LABEL_SD_A_GRP;
	YM_Static m_IDC_LABEL_SI_A_GRP;
	YM_Static m_IDC_LABEL_SD_B_GRP;
	YM_Static m_IDC_LABEL_SI_B_GRP;
	YM_Static m_IDC_LABEL_SD_A_DIFF;
	YM_Static m_IDC_LABEL_SI_A_DIFF;
	YM_Static m_IDC_LABEL_SD_B_DIFF;
	YM_Static m_IDC_LABEL_SI_B_DIFF;
	YM_Static m_IDC_LABEL_SD_A_TOTAL_ACT;
	YM_Static m_IDC_LABEL_SI_A_TOTAL_ACT;
	YM_Static m_IDC_LABEL_SD_B_TOTAL_ACT;
	YM_Static m_IDC_LABEL_SI_B_TOTAL_ACT;
	YM_Static m_IDC_LABEL_SD_A_TOTAL_MOI;
	YM_Static m_IDC_LABEL_SI_A_TOTAL_MOI;
	YM_Static m_IDC_LABEL_SD_B_TOTAL_MOI;
	YM_Static m_IDC_LABEL_SI_B_TOTAL_MOI;

	/// CLE deplacement controle 
	YM_Static	m_SD_ARES_TOT;
	YM_Static	m_SD_ADIFF;
	YM_Static	m_SD_BRES_TOT;
	YM_Static	m_SD_BDIFF;

	YM_Static	m_SI_ARES_TOT;
	YM_Static	m_SI_ADIFF;
	YM_Static	m_SI_BRES_TOT;
	YM_Static	m_SI_BDIFF;
	/// CLE


	YM_Static	m_CapaA;
	YM_Static	m_CapaB;
	YM_Static	m_CapaTOT;
	YM_Static	m_iCapaMoi_A;
	YM_Static	m_iCapaMoi_B;
	YM_Static	m_iCapaMoi_TOT;

	CListBox    m_ListTroncon;
	CComboBox   m_ComboTroncon;
	
//--TITRE
    YM_Static   m_SI;
    YM_Static   m_SD;
//--GRP
	YM_Static	m_SI_AGRP;
	YM_Static	m_SI_BGRP;
	YM_Static	m_SD_AGRP;
	YM_Static	m_SD_BGRP;

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
	//{{AFX_VIRTUAL(CQuotasBiTrancheDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
 
	// Generated message map functions
	//{{AFX_MSG(CQuotasBiTrancheDlg)
//	afx_msg void OnSelchangeComboTroncon();
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

	// OHA - Ano 76062 
	// indicateur de controle : si CC0 doit etre egal a CC1
	BOOL controlerCC0EgalCC1; 
	//void CumulEtanche(CQuotaEdit* pSCI_MOI, YM_Static* pSCI_RES, int &SumAuth, int &SumDisp);

	// HRI, 14/08/2015 : différencier le cumul étanche sur un seul scx et sur l'ensemble des scx
	// chgt des méthodes ci-dessous où on ne traite que le scx (inspiration ancien code SATC)
	void CumulEtancheSIA(int &sumAuthEtanche, int &sumDispEtanche );
	void CumulEtancheSIB(int &sumAuthEtanche, int &sumDispEtanche );
	void CumulEtancheSDA(int &sumAuthEtanche, int &sumDispEtanche );
	void CumulEtancheSDB(int &sumAuthEtanche, int &sumDispEtanche );

	// HRI, 14/08/2015 : différencier le cumul étanche sur un seul scx et sur l'ensemble des scx
	// Nouvelles méthodes : ci-dessous, elle traite l'ensemble des SCX
	void SommeGlobaleSIA(int &sumAuthEtanche, int &sumDispEtanche);
	void SommeGlobaleSIB(int &sumAuthEtanche, int &sumDispEtanche);
	void SommeGlobaleSDA(int &sumAuthEtanche, int &sumDispEtanche);
	void SommeGlobaleSDB(int &sumAuthEtanche, int &sumDispEtanche);
	void SommeGlobaleSCX(QuotasAutorisationsListCtrl* list, int& sumAuth, int& sumDisp);


	bool TronconInter(); // renvoi vrai si la tronçon courant est "international"

	void SansSci(); // Pas de sous-contingent, on grise tout ce qui est SCI.
	bool m_YaSci;

	bool ControlCrossFrontier (const CString& sListTroncon); // Controle que la liste de tronçon est soit totalement francaise,
													  // soit totalement etrangère (tronçon fontière inclu)
	int SumBkt0A;
	int SumBkt0B;
	int SumCapA;
	int SumCapB;

	int m_ovbA, m_ovbB, m_unbA, m_unbB, m_ovg;
	BOOL m_bFlagOkGlobal;
	BOOL m_bSNDetranger;
    BOOL m_bYaSousResa;

	CBrush* m_pAltBrush;
	CBrush* m_pStdBrush;

	CString sLastTroncon; // dernier troncon passé en parametre de la méthode SendVnlLock


	CQuotasBiTrancheDlg();
	CQuotasBiTrancheDlg(YmIcTrancheLisDom* pTranche);
	virtual ~CQuotasBiTrancheDlg();


	static CQuotasBiTrancheDlg* m_pInstance;   
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
