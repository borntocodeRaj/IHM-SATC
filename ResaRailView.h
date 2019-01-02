// ResaRailView.h : header file
#if !defined(AFX_RESARAILVIEW_H__58915283_709D_11D1_837A_400020302A31__INCLUDED_)
#define AFX_RESARAILVIEW_H__58915283_709D_11D1_837A_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#define ID_ES_BASE_BKT		100
#define ID_ES_OFFSET_BKT	2
#define ID_ES_MAX_BKT			160
#define TRANS_BACK				RGB(255, 255, 255)
#define WM_REFRESH_ALL	WM_USER + 20
#define TABCTRL_HEIGHT 23


class YmIcResaBktDom;
class YmIcResaCmptDom;
class CResaRailViewEdit;


// représente une ligne de classe de contrôle
class CResaCmptBktLine
{
public:
	CResaCmptBktLine( CWnd* pParentWnd, YmIcResaBktDom* pResaBkt, int nID );
	//CResaCmptBktLine( YmIcResaBktDom* pResaBkt );
	~CResaCmptBktLine();
	BOOL GetAuthModify() { return m_bAuthModified; };
	void SetAuthModify( BOOL bModified ) { m_bAuthModified = bModified; };
	BOOL GetAuthRevMixFlag() { return m_bAuthRevMixFlag; };
	void SetAuthRevMixFlag( BOOL bAuthRevMixFlag ) { m_bAuthRevMixFlag = bAuthRevMixFlag; };
	BOOL GetDmdRevMixFlag() { return m_bDmdRevMixFlag; };
	void SetDmdRevMixFlag( BOOL bDmdRevMixFlag ) { m_bDmdRevMixFlag = bDmdRevMixFlag; };

	// Donne la recette temps réel + estimation des prix garantis.
	// venteVNLbkt est le nombre de vente dans la bucket, lu dans le VNL. mettre 0 si VNL indisponible
	// dans ce cas la fonction renvoi le revenu du temps reel (REV_REAL_AUTHORIZATION) + revenu prix garanti du RRD
	int GetRecetteTempsReel(CWnd* pParentWnd, int venteVNLbkt);
	int GetPanierMoyen (CWnd* pParentWnd, int venteVNLbkt);

	YmIcResaBktDom* m_pResaBkt;
	CResaRailViewEdit* m_pEditAuthNew;    // the user-editable authorization fields
	CResaRailViewEdit* m_pEditAddDmdNew;  // the user-editable additional demand fields

	long m_lAuthNew;  // the current authorization value as entered by the user (or read from db)
	long m_lAuthPrev;  // the authorization value before the last change by the user (hold)
	long m_lAuthInit;  // the initial authorization at object creation (DM 6496)
	long m_lAddDmdNew;
	long m_lAvailCur;   // hold the 3 levels of availability for each line
	long m_lAvailUser;
	long m_lAvailFcOrg;
	long m_lRecette;	// DM 5350, données recette
	long m_lAuthNomos; // autorisation NOMOS ignorée
	long m_lAvailNomos; // dispo correspondante a autorisation NOMOS ignorée

protected:
	BOOL m_bAuthModified; // indicates whether the auth values were modified

	// the following booleans are defined to tell us when to run the revenue mix model
	// if authorizations for bkt 0 OR any dmd fcst values are modified, we must run revmix.
	BOOL m_bAuthRevMixFlag;
	BOOL m_bDmdRevMixFlag;

};

// typedef for the array which contains cmpt bkt line
typedef CTypedPtrMap<CMapWordToPtr, UINT, CResaCmptBktLine*> CEditMapArray;
typedef CTypedPtrArray<CPtrArray,CResaCmptBktLine*> CCmptBktLineArray;

// regroupe les CResaCmptBktLine d’un espace physique

class CResaCmptBkt
{
public:
	CResaCmptBkt();
	~CResaCmptBkt();
	void AccumAddDmd();

	long m_lAddDmdNewTot;
	YmIcResaCmptDom* m_pResaCmpt;
	CCmptBktLineArray m_CmptBktLineArray;
	long m_lResHoldTot;
	BOOL m_bTGVJumPctChanged;    // has the TGV Jum pctg value been changed ?
};

// Liste des CResaCmptBkt présents dans la fenetre Resarail, un par espace physique

class CResaCmptBktMap : public CTypedPtrMap<CMapStringToPtr,CString,CResaCmptBkt*>
{
public:
	void DeleteAll();
	CResaCmptBktLine* AddResaBkt( CWnd* pParentWnd, YmIcResaBktDom* pResaBkt, int nID );
	CResaCmptBkt* AddResaCmpt( YmIcResaCmptDom* pResaCmpt );
	void RemoveAllResaBkt();
	void RemoveAllResaCmpt();
	void AccumAddDmd();
	CStringArray& GetKeyArray() { return m_aszKey; }

	// the array which contains keys to the cmpt order (to display cmpts in the correct order)
	CStringArray m_aszKey;  
};

////////////////////// SCI
class CResaCmptBktSciLine
{
public:
	CResaCmptBktSciLine( CWnd* pParentWnd, YmIcResaBktDom* pResaBkt, int nID );
	//CResaCmptBktLine( YmIcResaBktDom* pResaBkt );
	~CResaCmptBktSciLine();
	BOOL GetAuthModify() { return m_bAuthModified; };
	void SetAuthModify( BOOL bModified ) { m_bAuthModified = bModified; };


	// Donne la recette temps reel + estimation des prix garantis.
	// venteVNLbkt est le nombre de vente dans la bucket, lu dans le VNL. mettre 0 si VNL indisponible
	// dans ce cas la fonction renvoi le revenu du temps reel (REV_REAL_AUTHORIZATION) + revenu prix garanti du RRD
	int GetRecetteTempsReel(int venteVNLbkt);
	int GetPanierMoyen (int venteVNLbkt);

	YmIcResaBktDom* m_pResaBkt;
	CResaRailViewEdit* m_pEditAuthNew;    // the user-editable authorization fields

	long m_lAuthNew;  // the current authorization value as entered by the user (or read from db)
	long m_lAuthPrev;  // the authorization value before the last change by the user (hold)
	long m_lAuthInit;  // the initial authorization at object creation
	long m_lAvailCur;
	long m_lAvailUser;
	long m_lRecette;	// DM 5350, données recette

protected:
	BOOL m_bAuthModified; // indicates whether the auth values were modified
};

// typedef for the array which contains cmpt bkt line
typedef CTypedPtrMap<CMapWordToPtr, UINT, CResaCmptBktSciLine*> CEditSciMapArray;
typedef CTypedPtrArray<CPtrArray,CResaCmptBktSciLine*> CCmptBktSciLineArray;

class CResaCmptBktSci
{
public:
	CResaCmptBktSci(CWnd* pParentWnd, YmIcResaCmptDom* pResaCmpt, int nID);
	~CResaCmptBktSci();
	BOOL GetAuthModify() { return m_bAuthModified; };
	void SetAuthModify( BOOL bModified ) { m_bAuthModified = bModified; };

	CResaRailViewEdit* m_pEditScgCapa;    // valeur de la CC-1
	YmIcResaCmptDom* m_pResaCmpt;
	CCmptBktSciLineArray m_CmptBktLineArray;
	long m_lResHoldTot;
	long m_lAuthNew;
	long m_lAuthPrev;
protected:
	BOOL m_bAuthModified; // indicates whether the auth values were modified
};

class CResaCmptBktSciMap : public CTypedPtrMap<CMapStringToPtr,CString,CResaCmptBktSci*>
{
public:
	void DeleteAll();
	CResaCmptBktSciLine* AddResaBkt( CWnd* pParentWnd, YmIcResaBktDom* pResaBkt, int nID );
	CResaCmptBktSci* AddResaCmpt( CWnd* pParentWnd, YmIcResaCmptDom* pResaCmpt, int nID );
	void RemoveAllResaBkt();
	void RemoveAllResaCmpt();
	CStringArray& GetKeyArray() { return m_aszKey; }

	// the array which contains keys to the cmpt order (to display cmpts in the correct order)
	CStringArray m_aszKey;  
};

// DM7978 - LME - YI-5405 - ajout 

class CCmptFilteredArray : public CArray <long, long&>
{
public:
	int IndexOf(long value) const;
	void Push(long value);
};

///////////////////fin SCI

typedef struct 
{
	UINT uiKeyID;
	CResaRailViewEdit* pEditWin;
} WUEditKeyStruct;

class CResaRailView : public CScrollView, public YM_ObsView
{
//DM7978 - LME - YI-5401 - ajout des onglets global/reduit
#define IDC_RESARAIL_TABCTRL 103
//DM7978 - LME - FIN

protected:
	CResaRailView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CResaRailView)

public:
	enum AuthAvailInd
	{
		AVAIL_CURRENT,
		AVAIL_FORECAST,
		AVAIL_USER = AVAIL_FORECAST,
		AVAIL_FORECAST_ORIG,
		AUTH_NEW,
		AUTH_PREV,
		ADD_DMD
	};

	enum SwitchFocusOrigin
	{
		RIGHT_ARROW_KEY = 0,
		LEFT_ARROW_KEY,
		DOWN_ARROW_KEY,
		UP_ARROW_KEY,
		MOUSE_CLICK
	};

	//TSE_Ano 79307 - BEGIN
	BOOL GetEtatWindow(){return m_WinReduit;};//GETTER la Variable pour vérifier si la fenêtre Résarail est gloale ou réduite
	void SetEtatWindow(BOOL WinReduit){m_WinReduit = WinReduit;};//SETTER la Variable pour vérifier si la fenêtre Résarail est gloale ou réduite
	//TSE_Ano 79307 - END
	CCTADoc* GetDocument();
	CResaCmptBktMap* GetCmptBktMap() { return &m_CmptBktMap; };
	CResaCmptBktSciMap* GetCmptBktSciMap() { return &m_CmptBktSciMap; };
	CEditMapArray* GetEditMapArray() { return &m_EditMap; };
	CEditSciMapArray* GetEditSciMapArray() { return &m_EditSciMap; };
	CLegView* GetLegView();
	YM_Vnl* GetVnl() { return m_pVnl; };
	void ChangeSelTC(const int & idx);
	void CtReload();
	CResaRailViewEdit* GetFocusEditWnd() { return m_pWndChildFocusSaved; };
	void SetFocusEditWnd( CResaRailViewEdit* pWndChildFocus ) 
	{ m_pWndChildFocusSaved = pWndChildFocus; };
	int GetResaValue( const char* cCmpt, int iNestLevel, CResaRailView::AuthAvailInd eAuthAvailInd );
	int GetRealTimeResHold( const char* cCmpt, int iNestLevel, YmIcResaBktDom* m_pResaBkt );
	int GetRealTimeSciResHold( const char* cCmpt, int iNestLevel, YmIcResaBktDom* m_pResaBkt );
	int GetRealTimeSciEtanche( const char* cCmpt, int iSci, YmIcResaBktDom* m_pResaBkt );
	int GetRealTimeAuth( YmIcResaCmptDom* pResaCmpt, int iNestLevel, YmIcResaBktDom* m_pResaBkt );
	int GetRealTimeSciAuth( const char* cCmpt, int iSci, YmIcResaBktDom* pResaBkt );
	int GetRealTimeScgCapa( const char* cCmpt, YmIcResaCmptDom* pResaCmpt);
	void SetShiftKeyDown( BOOL bShiftKeyDown ) { m_bShiftKeyDown = bShiftKeyDown; };
	BOOL GetShiftKeyDown() { return m_bShiftKeyDown; };
	void SetCtrlKeyDown( BOOL bCtrlKeyDown ) { m_bCtrlKeyDown = bCtrlKeyDown; };
	BOOL GetCtrlKeyDown() { return m_bCtrlKeyDown; };
	double GetAddDmdFc( YmIcResaBktDom* pResaBkt );
	double GetAddDmdFc( YmIcResaBktDom* pResaBkt, BOOL& bIsNull);
	double GetAddDmdFcNew( YmIcResaBktDom* pResaBkt );
	void SetAddDmdFcUser( YmIcResaBktDom* pResaBkt, double dAddDmdFc );
	int ChangeSentFlag (int change, int curval);
	int GetRealTimeRevenu (char cmpt, int bkt); // renvoi le revenu temps reel du bucket ou du cmpt si bkt = -1. renvoi -1 si pb
	int GetRealTimeSciRevenu (char cmpt, int sci); // renvoi le revenu temps reel du SCI ou du SCG si sci=-1. renvoi -1 si pb
	int GetRealTimeSciResa (char cmpt, int sci); // renvoi les resa temps reel du SCI ou du SCG si sci=-1. renvoi -1 si pb
	int GetSciNbVoyageurs (char cmpt, int sci); // renvoi le nb_voyageur du SCI ou du SCG si sci=-1. renvoi -1 si pb
	int GetSciRecette (char cmpt, int sci); // renvoi la recette (ym_rev_real_tdlpng) du SCI ou du SCG si sci=-1. renvoi -1 si pb
	void GetSciCountEtanche(char cmpt, int& scxcount, int& scxetanche); // calcul nombre de sci et le nombre d'étanche du compartiment
	void CtrlEtancheOnVnl(); // contrôle de l'étanchéité indiquée par le VNL et éventuelle mise à jour en mémoire et en base
	void SetMaxSci (char cmpt, int valmax); // rabote si besoin la valeur des SCI à valmax pour compartiment cmpt
	void ForceSci (char cmpt, int val); // NBN ANO 89569 - Pour la tranche 3 SCG = SC1
	void SetMaxScg (char cmpt, int valmax); // rabote si besoin la valeur des Scg à valmax pour compartiment cmpt
	void CtrlDiffCC0CC1 (); // verifie que CC0-CC1 >= SCG
	void CtrlLessCapaAndCC0 ();   // verifie que SGC <= Capa et SCG <= CC0
	long GetScgAuth (char cmpt); // renvoi l'autorisation du sous-contingent global CC-1 pour le compartiment cmpt (colonne MOI)
	long GetScgResHold(char cmpt);
	long GetSumScgAuth();		  // renvoi la somme des autorisations des sous-contingents pour tous les compartiments (colonne MOI)
	void CtrlScgLegs();  // Contrôle le leg courant et la tranche courante pour éventuellement changer leur SCX_CAPA.
	// et provoquer le changement d'affichage du carré sous-contingent de jaune a gris et inversement.
	BOOL m_FlagReadOnly;

	void UpdateData(BOOL bSaveAndValidate);
	void UpdateDataSci(BOOL bSaveAndValidate);
	BOOL UpdateCompartments(YmIcLegsDom* pLeg = NULL );

	// Pour DM 7978 : permet de gérer le cas CC0=CC1
	// méthode appelée à l'intérieur de UpdateBuckets
	// qu'on pourra aussi utiliser depuis CLegView pour tester le même cas (CC0=CC1)
	// BUGI 69972 : lors du report d'autorisations par copier-collé d'un leg à l'autre,
	// il faut vérifier que CC0= cC1 (sous couvert AVT 17 - date bang)
	BOOL ControleEgaliteCC0CC1( YmIcLegsDom* pLeg, 
							enum CLegView::LegUpdateMode eUpdateMode,
							BOOL bForceAuthMod,
							BOOL &bModified);
	BOOL UpdateBuckets(int AUorAS,
		YmIcLegsDom* pLeg, 
		enum CLegView::LegUpdateMode eUpdateMode,
		BOOL bForceAuthMod, 
		BOOL bConfirm);
	void UpdateBkt(BOOL bVnauSent, BOOL bForceAuthMod, CString sLegOrig);
	void UpdateSci(BOOL bVnassent, CString sLegOrig);
	void UpdateSciEtanche();
	BOOL Modified();  // was a value modified ?
	BOOL TrainAuto(); // Is it a train auto (compartment O)
	BOOL GetModificationCancelled() { return m_bModificationCancelled; }
	void SetModificationCancelled(BOOL bFlag) { m_bModificationCancelled = bFlag; }
	void EditEntries();
	void OvbAdjust();
	void BucketCheck();
	int GetSumDispEtanche(char cmpt); //Renvoi la somme des Dispo des SCI étanches du compartiment
	void CalcAvail(BOOL bCalculate);
	void CalcAvailSci(BOOL bVnl); // Si bVnl est vrai on prend les autorisations du VNL, sinon celle de la colonne MOI
	BOOL CtrlAuthSci(char cmpt, int valscg); // renvoi vrai si les autorisations des SCI sont correctes par rapport à la valeur de SCG
	int  SendVnl( BOOL bDepartsNextDay = FALSE );
	int SendVnau( CString szMsg, YmIcLegsDom* pLeg, CResaCmptBkt* pCmptBkt, BOOL bConfirm = TRUE );
	int SendVnas( CString szMsg, CString SuiteVnas, YmIcLegsDom* pLeg, BOOL bConfirm = TRUE );
	int SendVnae(); //contruit et envoi éventuels VNAE, confirmation obligatoire. renvoi 0 si rien envoyé
	void WriteViewConfig();
	void RestoreViewConfig();
	void writeLog(CString input);
	BOOL GetBoolPMoyen() { return m_bPanierMoyen; }
	void SetBoolPMoyen (BOOL bpm) { m_bPanierMoyen = bpm; }

	//  void LoadResa();

	BOOL SaveAllModified(int AUorAS,
		YmIcLegsDom* pLeg = NULL, 
		enum CLegView::LegUpdateMode eUpdateMode = CLegView::CURRENTLY_LOADED_LEG,
		BOOL bForceAuthMod = FALSE, 
		BOOL bConfirm = TRUE);
	BOOL SaveOnAllLegs (int AUorAS);
	void UpdateOthLegsView( int iEditWinId );
	void FrameSizeChange (UINT nType, int cx, int cy) {};

	BOOL IsCrossMidnightLeg( YmIcTrancheLisDom* pTranche, YmIcLegsDom* pLeg );
	void SetVnlLeg(CString szLegOrig, CString szLegDest);
	void InitLegScg (); // initialise la table m_LegScg a partir du résultat du VNL.
	void CtrlScgVide(CString sVnas1, CString sVnas2); // verifie que les VNAS ne rentre pas dans un deux cas de figure suivants
	// mise à 0 d'un SCG alors que les autres tronçons ont un SCG, et l'inverse.
	// Le test se fait par rapport à la table m_LegScg.
	YM_VnlCmpt* FindVnlCmpt(CString szCmpt );
	YM_VnlBkt* FindVnlCmptBkt( CString szCmpt, int iNestLevel );
	YM_VnlSci* FindVnlCmptSci( CString szCmpt, int iSci );

	// controle si des espaces physique pour le quel ACT était différent de MOI pour au moins un bucket
	// sont resté inchangé sur tout leur bucket. Renvoi vrai si c'est le cas.
	// ajouté en profitant de la DM 6496
	// DM 5882.1, on etend le contrôle aux VNAS, AUorAS permet de savoir si l'on controle:
	// les buckets (AUorAS=1), les SCI (AUorAS=2), les deux (AUorAS = 3)
	BOOL ControlActDiffMoi(int AUorAS);

	// DM 6185.2 Exploitation des données de NM_PREV_TDLPRN (Thalys Nomos)
	// lue avec la requête IDS_SQL_SELECT_THALYS_PREVISION. Fonctions aussi utilisée pour les montée en charge
	long GetRrdPrevisionResa (long rrd, const char* cmpt, long nestlevel);
	long GetRrdPrevisionResa (long rrd, const char* cmpt, long nestlevel, long scx);
	long GetRrdPrevisionRevenu (long rrd, const char* cmpt, long nestlevel);
	long GetRrdPrevisionRevenu (long rrd, const char* cmpt, long nestlevel, long scx);

	//DM 6884.2, sur-resa bi-tranche
	int m_SurResa[26];       // utilisé comme tableau de int (valeur entre 0 et 255), un par espace physique possible
	int m_SousResa[26];       // utilisé comme tableau de int (valeur entre 0 et 255), un par espace physique possible
	int m_SurResaGlob;		// sur resa globale admissible
	int m_capatotale;        // capacité totale de la tranche.
	int TotAuthBkt0();	   // somme des autorisations en bkt 0.
	BOOL OverBookingTotalOK(); // renvoi faux et affiche un message si on depasse l'overbooking global.



	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResaRailView)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

private:
	void NettoyerBktAndBktSciMap();
	bool IsValidCmptBktSet(YM_RecordSet* pResaBktSet,YM_RecordSet* pResaCmptSet, YM_RecordSet* pResaBktSciSet, YM_RecordSet* pResaCmptSciSet, YM_RecordSet* pPrevisionSet, YM_RecordSet* pVnauExcluSet, bool YaSci, bool YaPrevision);
	// Implementation
protected:
	virtual ~CResaRailView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void OnUpdateFromRecordSet(CView* pSender, LPARAM lHint, CObject* pHint);
	void OnUpdateFromResaMsg(CView* pSender, LPARAM lHint, CObject* pHint);
	void GetCapValues(YmIcResaCmptDom* pResaCmpt, 
		long& lReservableCap, 
		long& lStandingCap, 
		long& lUnreservableCap);

	// Generated message map functions
protected:
	//{{AFX_MSG(CResaRailView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);	
	afx_msg void OnDestroy();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChartSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnSwitchFocus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnResaRailPrint();
	afx_msg void OnUpdateResaRailPrint( CCmdUI* pCmdUI );
	//	afx_msg LRESULT OnValueUpdated(WPARAM wParam, LPARAM lParam);
	//	afx_msg void OnEditKillFocus(UINT nId);
	DECLARE_MESSAGE_MAP()

private:

	// Focus Management
	CResaRailViewEdit* m_pWndChildFocusSaved;

	// Drawing methods
	void SetStatusBarText( UINT nId );
	void SetStatusBarText( CString szText );
	void DrawLabel( CDC* pDC, UINT nID, LPRECT lpRect, BOOL bUnderline, UINT nFormat = DT_CENTER|DT_SINGLELINE );
	void DrawLongValue( CDC* pDC, long lValue, LPRECT lpRect, BOOL bUnderline );
	void Underline( CDC* pDC, LPRECT lpRect );
	void ChangeSizeForSci(BOOL bForce);
	void CalculateSize();
	void GetVnauExcluAuth(); //DM 6185.4
	void CalcVnauExcluAvail(); // DM 6185.4
	// DM7978 - LME - YI-5405 - implementation de la regle de filtrage pour les CC (onglet reduit)
	void Resize();
	void CmptFilteringRule(const CString& cmpt, CCmptFilteredArray& filtered);
	void ReorganizeTabOrder();
	void ReorganizeEditVisibility();
	inline bool IsModeGlobal() const {return (m_eButtonClicked == eOverallClicked);}
	inline bool IsModeReduit() const {return (m_eButtonClicked == eReducedClicked);}
	
	// Internal structure
	CResaCmptBktMap m_CmptBktMap;
	CResaCmptBktSciMap m_CmptBktSciMap;
	CEditMapArray m_EditMap;
	CEditSciMapArray m_EditSciMap;
	CStringArray m_aszCmpt;
	BOOL m_bSciHeight; // vrai si hauteur SCI (masquage des SCI)
	BOOL m_bPanierMoyen; // vrai si affichage de panier moyen
	BOOL m_bPrevisionOK; // vrai s'il y a des previsions DM 6185.2
	BOOL m_bVnauExcluOK; // vrai s'il y a un VNAU exclu DM 6185.4

	//TSE_Ano 79307 - BEGIN
	BOOL m_WinReduit;//Variable est à 0 si la fenêtre est globale, 1 si réduite
	BOOL m_ChangedWindowGlobal;//Variable pour ne pas changer le Réctangle Global
	BOOL m_ChangedWindowReduit;//Variable pour ne pas changer le Réctangle Réduit
	CRect rectUpGlob;//Réctangle Global CC Up
	CRect rectDoGlob;//Réctangle Global CC Down
	CRect rectUpRed;//Réctangle Réduit SCi Up
	CRect rectDoRed;//Réctangle Réduit SCi Down
	//TSE_Ano 79307 - END
	//DM7978 - LME - YI-5401 - ajout d'un CTabCtrl pour les onglets global/réduit
  CTabCtrl m_TabCtrl; // the tab control
	typedef enum BUTTON_E {eNoButtonClicked = -1, eOverallClicked, eReducedClicked} BUTTON_E;
	BUTTON_E m_eButtonClicked;

	//DM7978 - LME - YI-5415
	void SetColorSpherePrix(); // fonction permettant d'associer a chaque CC/SCx une couleur de sphere de prix
	void DrawRect(CDC *pDC, CRect rect, 
		CBrush* pOldBrush, CBrush* pBrush, 
		CPen* pOldPen, CPen* pPen); // affiche un rectangle plein

	// VNL management
	YM_Vnl* m_pVnl;  // the vnl object containing parsed vnl info
	YM_VnlLeg* m_pVnlLeg;  // the leg level object for the currently selected leg selected from the vnl
	int m_iMsgId;    // the latest id of the vnl msg sent to resarail
	BOOL m_bVnlResent;  // boolean used to indicate a vnl was resent (due to leg departing next day)
	BOOL m_bIsValid;
	BOOL m_bOkOnRhea;	// VRAI si on peut envoyer des VNAU sur tranche Rhealys (ACT = MOI)

	CString BuildVnau( YmIcLegsDom* pLeg, CResaCmptBkt* pCmptBktJum, BOOL bCrossMidnight );
	CString BuildVnas( YmIcLegsDom* pLeg, BOOL bCrossMidnight, CString& VnasSuite );
	CString BuildVnae( CString& VnaeSuite );
	CString BuildDebVnaMess (CString debVna, BOOL bCrossMidnight);
	int GetAvailBkt (char cmpt, int bkt); // renvoi la dispo de la bucket
	BOOL YaSciClicked(); // Renvoi vrai si besoin de faire un VNAE

	// SentFlag management
	void MajTrancheSentFlag ();
	void MajTrancheScxSentFlag ();

	BOOL m_bShiftKeyDown;  // determine if the user has pressed or released the Shift Key
	BOOL m_bCtrlKeyDown;  // determine if the user has pressed or released the Alt Key
	BOOL m_bModificationCancelled;
	//DM7978 - LME - YI-6056 - dimensionnement automatique
	CSize m_SizeView;
	bool m_bInitialUpdate;
	//DM7978 - LME - YI-5401 - masquage des sous-contingents
	LONG m_lYSciNotDisplayed;
	//DM7978 - LME - YI-6046
	long m_ascTarifaire;//KGH

	//DM7978 - LME - YI-5414
	

	CString m_CActMoi;		// liste des compartiments où au moins un bucket a ACT(VNL) <> MOI.
	// pour contrôle lors envoi VNAU. Ajouté le 30/10/08 lors de la DM6496
	CString m_SciCActMoi;		// liste des compartiments où au moins un SCI a ACT(VNL) <> MOI.
	CMapStringToOb m_LegScg;	// mémorise pour chaque leg/espace physique la présence intiale d'un sous-contingent (CC-1)
	// iniitialisé sur reception du premier VNL (sans OD) envoyé par resarail.
	
	friend class CResaRailViewEdit;
};

/////////////////////////////////////////////////////////////////////////////
// CResaRailViewEdit window

class CResaRailViewEdit : public CEdit
{
protected: // create from serialization only
	DECLARE_DYNCREATE(CResaRailViewEdit)

	// Construction
public:
	CResaRailViewEdit(bool bSci = FALSE);

	// Attributes
public:
	bool m_bSci;	// vrai si zone d'édition SCI (pour gestion de la couleur de fond)

	// Operations
public:
	BOOL RangeCheck();
	BOOL RangeCheckSci();
	void ResaRailViewEditEntry( BOOL bUpdateWindow = TRUE );
	void UpdateData(BOOL bSaveAndValidate);
	void UpdateDataSci(BOOL bSaveAndValidate);
	void ProcessTrainAdaptation(CResaCmptBkt* pCmptBkt, double dValOrg, double dValNew);
	int VnauConfirmationMessage();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResaRailViewEdit)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CResaRailViewEdit();
	inline BOOL GetEditChecked() { return m_bEditChecked; };
	inline BOOL GetEditAdjusted() { return m_bAdjusted; };
	inline void SetEditChecked( BOOL bRangeChecked ) { m_bEditChecked = bRangeChecked; } ;
	inline void SetEditAdjusted( BOOL bAdjusted ) { m_bAdjusted = bAdjusted; } ;
	inline void SetBktLevelFlag( BOOL bBktLevel ) { m_bBktLevel = bBktLevel; }
	// DM7978 - LME - YI-5415 - coloration sphere de prix
  inline void SetBackColor(COLORREF col) {m_BackColor = col;}

	// Generated message map functions
protected:
	//{{AFX_MSG(CResaRailViewEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetfocus();
	afx_msg void OnChange();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//DM7978 - LME - YI-5415 - handler pour changer la couleur de fond
	afx_msg HBRUSH CtlColor(CDC *pDC, UINT nCtlColor);
	afx_msg HBRUSH OnSetBkOpaque(CDC *pDC, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bEditChecked;
	BOOL m_bAdjusted;
	BOOL m_bRevMixExecuted;
	BOOL m_bBktLevel;
	// DM7978 - LME - YI-5415 - coloration sphere de prix
	COLORREF m_BackColor;
	CBrush m_Brush;
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESARAILVIEW_H__58915283_709D_11D1_837A_400020302A31__INCLUDED_)
