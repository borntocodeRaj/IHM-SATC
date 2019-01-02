// CTA.h : main header file for the CTA application
//

#if !defined(AFX_CTA_H__2017F0D5_3EE9_11D1_8369_400020302A31__INCLUDED_)
#define AFX_CTA_H__2017F0D5_3EE9_11D1_8369_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'StdAfx.h' before including this file for PCH
#endif


class CResComm;
class CBktHistDateMap;
class CVnlEngineData;

// User-defined messages
/////////////////////////////////////////////////////////////////////////////
#define WM_SWITCHFOCUS  (WM_USER + 119)
#define WM_VALUEUPDATED (WM_USER + 120)

#define CMPT_HIST  "COMPARTMENT HISTORY"
#define CMPT_HIST_ALT  "COMPARTMENT HISTORY ALT"

class YM_Database;
class CCTADoc;

extern CString KDICLI;

/////////////////////////////////////////////////////////////////////////////
// CCTAApp:
// See CTA.cpp for the implementation of this class
//
#define RRDDISPLAYALL	0	// moyenne, ecart-type et historique
#define RRDDISPLAYMEC	1	// moyenne et ecart-type, mais pas historiques
#define RRDDISPLAYECH	2   // Ecart-type et historique
#define RRDDISPLAYHIS	3	// historique mais pas moyenne ni ecart type

#define MAXNBJMX	    51  //nombre maximum de J-X dans une courbe de montée en charge.
#define MAXNBJMXPLUS5	58  // 56 à 58 : On met un peu de rab pour éviter certains dépassements (HRI, ano 49551) adXData 

class CCTAApp : public YM_WinApp
{
public:
  int _nbReconnection;
  char m_Client; // SRE 78599 - Add 'Client' to store Command Line 'client=' key
  CVnlEngineData*  m_pVnlEngineData;
  BOOL m_bDvltMode;			   // True if we are in dvlt mode (/dvlt on command line)
  BOOL m_AutoReconnect;		// If TRUE, autoreconnection system will work
  int m_SuperUser;
  CString m_UserRole;			   // Role of the user
  CString m_EntityList;			// liste d'entités utilisable pour selection
  BOOL m_CmptHistAllDays;		// Vrai si lecture cmpthist pour tous les jours de la semaine
  CCTADoc* m_pDoc; 
  int m_RRDdisplayMode;		 // mode d'affichage dans fenetre montee en charge
							 // vaut RRDDSISPLAYALL ou RRDDISPLAYMEC ou RRDDISPLAYHIS
  bool m_bRRDRotatelabel;    // mémorisation de l'état tourné ou non des labels dans la fenêtre RRDHIST
							 // en cas de split de cette dernière, pour initialisé correctement la nouvelle partie.
  CString m_RRDBoolSet;		 // paramétrage courant de l'affichage de la fenêtre RRD History
  CString m_RECDBoolSet;		 // paramétrage courant de l'affichage de la fenêtre RECD History
  enum CCTAChart::GraphScaleIndex m_ScaleRRD;	 // mémorisation du scale utilisé sur la fenêtre RRD history

  bool m_bHDptRotatelabel;   // mémorisation de l'état tourné ou non des labels dans la fenêtre historique par départ
  int  m_HDptInterval;		 // mémorisation de l'interval en X des labels dans la fenêtre historique par départ
  enum CCTAChart::GraphScaleIndex m_ScaleHDpt;	 // mémorisation du scale utilisé sur la fenêtre RRD history

  int m_lasttranchevnau;	// n° tranche du dernier vnau autorisation envoyé
  RWDate	m_lastdatevnau;	// date du dernier vnau envoyé
  RWDate    m_queryDate;	// sert a passer une date pour une requête.
  // NPI - DT12135
  long	m_queryEntityNatureID;		// sert à passer EntityNatureID pour une requête
  BOOL	m_queryDateRefDlg;
  ///
	
  ///

  CCTAApp();
  // NPI - Correction Memory Leak
  virtual ~CCTAApp();
  RWZoneSimple* m_RWMyZone;
  ///
  void ForceVnlViewer();	// force l'affichage de la fenêtre de trace des VNL/VNAU

  // Placement et suppression un timer sur la MainFrame, qui tentera
  // régulièrerement la reconnexion au service de Comm
  void StartReconnectTimer();
  void StopReconnectTimer();

  void CreateDocTemplates();
  void CreateNewView( const CString& str, CDocument* pDoc );
  CWnd* IsWindowCreated( const CString& sWindowTitle );
  //ano 133192, on force fermeture des fenêtre splitées. Elle seront réouverte si besoin
  //ano 145958, 145968, 145965, 145964; on ferme toutes les fenêtre à taquets
  // Mais si l'appel vient de FinalDocInitialization il ne faut pas fermer et réouvrir les fenêtres
  // car il peut y avoir des notifications dans les tuyaux, d'ou ajout du boolean m_bDoClose.
  void OpenPersistentChildWindows();

  void NotCloseOpenPersistentChildWindows();
  

  void ToggleLoggingSQL();
  void InitiateComm( CString szHostname, int iPortNumber );
  void SetRealTimeMode (BOOL tempreel);
  CResComm* GetResComm() { return m_pResComm; }
  BOOL GetRealTimeMode() { return m_bRealTime;}
  BOOL GetFlashLabelFlag()  { return m_bDisplayFlashLabels; }
  void SetFlashLabelFlag(BOOL bFlashLabel) { m_bDisplayFlashLabels = bFlashLabel; }
  BOOL GetTranMenuModFlag()  { return m_bTrancheMenuModified; }
  void SetTranMenuModFlag(BOOL bTrancheMenuModified) { m_bTrancheMenuModified = bTrancheMenuModified; }
  BOOL GetRelTranMenuModFlag(BOOL bAlt = FALSE)  { return bAlt ? m_bAltRelTrancheMenuModified : m_bRelTrancheMenuModified; }
  void SetRelTranMenuModFlag(BOOL bRelTrancheMenuModified, BOOL bAlt = FALSE)
  { if (bAlt)
      m_bAltRelTrancheMenuModified = bRelTrancheMenuModified;
    else
	  m_bRelTrancheMenuModified = bRelTrancheMenuModified; }
  BOOL GetRrdHistMenuModFlag()  { return m_bRrdHistMenuModified; }
  void SetRrdHistMenuModFlag(BOOL bRrdHistMenuModified) { m_bRrdHistMenuModified = bRrdHistMenuModified; }
  BOOL GetRecdHistMenuModFlag()  { return m_bRecdHistMenuModified; }
  void SetRecdHistMenuModFlag(BOOL bRecdHistMenuModified) { m_bRecdHistMenuModified = bRecdHistMenuModified; }

  void RestoreAppConfig();  // restore application specific data from the registry
  void WriteAppConfig();  // write application specific data to the registry

  // Sunday is beginning of the week in U.S./England, Monday is beginning of the week in Europe
  BOOL GetSundayIsDOW1()  { return m_bSundayIsDOW1; }
  void SetSundayIsDOW1(BOOL bSundayIsDOW1) { m_bSundayIsDOW1 = bSundayIsDOW1; }
  int ConvertRWDayOfWeek(int iRwWeekday);
  BOOL SaveModifications();
  
  // Ano 116770 et modif de requête montee en charge historique
  YmIcDateHistoDom m_CurHistoDates; // Dates historiques courantes.
  // NPI - DM7870 : Dates de référence
  //void LoadCurHistoDates(unsigned long dptdate,CString szCurrentEntity); // mise à jour de m_CurHistoDates
  void LoadCurHistoDates(unsigned long dptdate); // mise à jour de m_CurHistoDates
  ///
  void SetCurrentEntityNatureID(CString szCurrentEntity, long lNature); // NPI - DM7870 : Dates de référence
  void SetQueryEntityNatureID(CString szCurrentEntity, long lNature); // NPI - DT12135
  
  //DM4965.3; Récupération du nombre de bucket par compartiment,
  // reutilisé par DM 6716 8eme CC en seconde
  CMapStringToOb m_nbBktByCmpt;
  void GetNbBktByCmpt  (YmIcTrancheLisDom* pTranche);

  //DM Rhealys
  void GetGrpTranche   (int iTrancheno,unsigned long DptDate, CString sLegOrig,int &iGrpA,int &iGrpB);

  //DM 4948: ne pas visualiser les recettes des tranches dont la date de circulation est >= max (dpt_date) from sc_daily_traffic
  unsigned long GetMaxDptDateDailyTraffic();

  CBktHistDateMap* GetTempBktJumHist(BOOL bAlt) { return bAlt ? m_pTempBktJumHistAlt : m_pTempBktJumHist; }

  //Antibug de ce qui c'est passé le 25/10/2005
  //Suite a des delete massifs sur sc_legs, il y avait un problème sur la mise à jour de sc_legs
  // Il semble que la query de mise à jour d'un tronçon activé en autodelete soit détruite avant
  // D'ou mise en place de cette fonction pour remplacer les query crée en autodelete sur sc_legs.
  void UpdateOneLeg (YmIcLegsDom* pRecord);

  // DM 5480, train auto. Les autorisations de train auto doivent être systématiquement affectée à l'ensemble
  // des tronçons. On profite de cette DM pour créer un fonction plus efficasse pour la commande "Coller autorisation partout"
  // cette requête ajoute 1 au sent_flag des legs de la tranche si mod (sent_flag, 2) = 0.
  void UpdateAllLegs (YmIcLegsDom* pRecord);
  void UpdateSciAllLegs (YmIcLegsDom* pRecord);

  // DM 5524 Rhealys. Met a jour le CriticalFlag sur la tranche/date indiquée. Si bSet=TRUE on positionne, sinon on retire
  // Le positionnement porte sur le deuxième bit du CRITICAL_FLAG
  void UpdRhealysCriticalFlag (BOOL bSet, int tranche_no, unsigned long ldate);

  //DM 6977 amélioration ALLEO
   // Sur envoi de VNAU par la boîte de répartition des quotas, positionne en base le bit 1 du sent_flag sur la tranche/date
   void SetTrancheSentFlag (bool bscx, int tranche_no, unsigned long ldate);
  //idem sur leg
  void SetLegSentFlag (bool bscx, int tranche_no, CString sLegOrig, unsigned long ldate);
  // positionne en base le bit 'valbit' de l'edit_status

  void SetTrancheEditStatus (int tranche_no, unsigned long ldate, int valbit);

  void SetLegEditStatus (int tranche_no, CString sLegOrig, unsigned long ldate, int valbit);

  BOOL IsEntityRhealys (CString sEntity); // Renvoi vrai si la description

  // donne le last_rrd_index si la tranche a traiter par le dernier batch, -1 sinon
  long GetLastRrdIndexOnLastBatch (long tranche, unsigned long ldate);
  // enumeration for method QueryRegistryEntry
  enum RegResult
  {
	  eValueMatched = 0,    // matched the string value
    eValueNotMatched,  // string value did not match 
    eValueNameNotFound,  // did not find the string value name
    eKeyNotFound  //did not find the registry (sub)key
  } ;
  enum RegResult QueryRegistryEntry(CString szRegSubKey, CString szValueName, CString szValue);

  void AllEntityRhealys();// Regarde si toutes les entity sont Rhealys m_bRhealys à TRUE
  char GetClientType()   { return m_Client; } //Renvoi le paramètre du client servant à définir la version du SATC -> 'S' correspond au
											  //SATC Classique (ou T-Resa : la différence entre classique et T-Resa est faite par le flag
											  //pré-processeur MOTEURVNL_APPLICATION présent sur la version T-Resa)
  BOOL GetRhealysFlag()  { return m_bRhealys; }
  int  GetNbConnHisto()	 { return this->m_NbConnHisto; }
  void SetNbConnHisto(int nbconn) { this->m_NbConnHisto = nbconn; }

  void AllEntityAutoTrain(); // regarde si toutes les entity sont autotrain, si oui met m_bAutoTrain à TRUE
  BOOL GetAutoTrainFlag() { return m_bAutoTrain; }
 
  // DM 6075 fenetre historique par classe de controle
  CMapStringToOb m_MapMatriceCC8;
  void GetCoeffMatriceCC8();
  BOOL TransformeMatrice (CString sEntity, CString sCmpt, CMapStringToOb* m_pMapOldResa,
	                      CArray<int, int>& NewResa, CMapStringToOb* pMatrice);

  // Ajout d'un contrôle sur dernier resaven
  void VerifyResaven();

  // DM 6977, vérification de VNAU ALLEO non envoyé
  int YaUnsentRhealysVNAU();
  int CheckRhealysVNASSent();//DM8029 - Ajout d'une méthode pour vérifier les envois de VNAS sur ALLEO
  void VerifUnsentVnau(bool bMessIfNot);

  //DM 7171 récup d'une sensibilité // renvoi la description du sous-type, lue dans YM_SENSIBILITE, vide si non trouvée
  CString SensiDesc(CString sstype);
  CString TopDesc(CString stop, unsigned long ddate); // idem pour type_train, si date = 0 on l'ignore
													  // et on prend la desc du premier TOP correspondant

  // DM7790
  CString m_AxeList;			// liste d'axes utilisable pour selection
  CString m_RouteList;			// liste de routes utilisable pour selection

  BOOL GetAttendEcranSelectionVnl() { return m_attendEcranSelectionVnl;}
  void SetAttendEcranSelectionVnl(BOOL attendEcranSelection) { m_attendEcranSelectionVnl = attendEcranSelection;}

  // NPI - DM7838 : souplesse tarifaire
  BOOL GetSouplesseTarifaire(BOOL bGestionAffichageItemMenu = FALSE, YmIcTrancheLisDom* pTrancheLoaded = NULL);


  CCTADoc* GetDocument();

  // Return the module handle
  HMODULE GetModule() { return m_hModule; };

  // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCTAApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL SaveAllModified();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCTAApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
  BOOL m_bRealTime;              // ResaRail switch - On or Off
  BOOL m_bDisplayFlashLabels;  // Flag indicating whether to display graph flash labels

  // flag indicating that the menu has been modified for client parameterization
  BOOL m_bTrancheMenuModified;  // has tranche menu been configured ?
  BOOL m_bRelTrancheMenuModified;  // has related tranche menu been configured ?
  BOOL m_bAltRelTrancheMenuModified;  // has related tranche menu been configured ?
  BOOL m_bRrdHistMenuModified;  // has rrd history menu been configured ?
  BOOL m_bRecdHistMenuModified;  // has recd history menu been configured ?
  BOOL m_bSundayIsDOW1;  // has rrd history menu been configured ?
  BOOL m_bRhealys;
  BOOL m_bAutoTrain;
private:
	int m_NbConnHisto;
  CResComm* m_pResComm;  // ResaRail communications handle
  HMODULE m_hModule;     // Instance of the module handle
  bool m_bDoClose; // Le OpenPersistentChildWindows ne fermera les fenêtre sus-cités que si m_bDoClose est positionné.
  CBktHistDateMap* m_pTempBktJumHist;
  CBktHistDateMap* m_pTempBktJumHistAlt;
  unsigned long    m_maxDateDailyTraffic;
  BOOL m_attendEcranSelectionVnl;	// L'écran de sélection Moteur VNL est requis à l'ouverture de la View

public:
	BOOL IsTrainDeNuit(YmIcTrancheLisDom* pTranche);
};

#define App() ( *( (CCTAApp*)AfxGetApp() ) )

#ifndef _DEBUG  // debug version in CTAApp.cpp
inline CCTADoc* CCTAApp::GetDocument()
   { return (CCTADoc*)m_pDoc; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTA_H__2017F0D5_3EE9_11D1_8369_400020302A31__INCLUDED_)
