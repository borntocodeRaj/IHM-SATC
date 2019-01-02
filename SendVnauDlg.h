#if !defined(AFX_SENDVNAUDLG_H__B8320503_E97B_11D1_91DA_400020302A10__INCLUDED_)
#define AFX_SENDVNAUDLG_H__B8320503_E97B_11D1_91DA_400020302A10__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CVnauAutho;

class CVnauAuthoLeg : public CVnauAutho
{
  public:
  
  CVnauAuthoLeg (CString Orig, CString dest, BOOL dptnextday);
  ~CVnauAuthoLeg ();

  CString m_legorig;
  CString m_legdest;
  BOOL    m_dptnextday;
};

// CVnasAuthoLeg : non utilisé par SendVnau Dlg, mais par QuickVnauDlg.
// Mis ici par cohérence avec CVnauAuthoLeg dont il est issu.
class CVnasAuthoLeg : public CVnasAutho
{
  public:
  
  CVnasAuthoLeg (CString Orig, CString dest, BOOL dptnextday);
  ~CVnasAuthoLeg ();

  CString m_legorig;
  CString m_legdest;
  BOOL    m_dptnextday;
};


class CVnauByLegAuthMap : public CTypedPtrMap <CMapStringToPtr, CString, CVnauAuthoLeg*>
{
  public:
    CVnauByLegAuthMap();
	~CVnauByLegAuthMap();
	void Empty();
};

// CVnasAuthoLeg : non utilisé par SendVnau Dlg, mais par QuickVnauDlg.
// Mis ici par cohérence avec CVnauByLegAuthMap dont il est issu.
class CVnasByLegAuthMap : public CTypedPtrMap <CMapStringToPtr, CString, CVnasAuthoLeg*>
{
  public:
    CVnasByLegAuthMap();
	~CVnasByLegAuthMap();
	void Empty();
};


/////////////////////////////////////////////////////////////////////////////
// CSendVnauDlg dialog

class CSendVnauDlg : public YM_ETSLayoutDialog, public YM_Observer
{
// Construction
public:
	CSendVnauDlg();
	
	void Show();
	void Hide();

	void OnNotify( YM_Observable* pObject );
	void Changefont();

// Dialog Data
	//{{AFX_DATA(CSendVnauDlg)
	enum { IDD = IDD_SENDVNAU };
	CString	m_month;
	CString	m_vnlresult;
	short	m_day;
	CString	m_dest;
	CString	m_orig;
	int		m_tranche;
	CString	m_editvnau;
	CString	m_fixedvnau;
	CString	m_listvnau;
	CString m_editOD;
	BOOL	m_bModeVNAS;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendVnauDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Create();

	// CheckAuthorizations verify if the m_authorization member is
	// valid, respect the format (i.e A0-200/A1-180/A2-150/B0-300)
	// that a cmpt/bucket authorization for a bucket n is lower than
	// for bucket n-1.
	// If there is an error, it make selected the error area in the
	// IDC_EDIT_AUTHORIZATION box, and display an explanation message box.
	BOOL CheckAuthorizations();

	// CheckScgSci verify if the m_authorization memeber is valid
	// respect the format (ie A*SCG-80!SC1-40!SC2-20/B*SCG-100!SC1-60!SC3-30)
	// that any SCI in lower than the corresponding SCG
	// that SCG is lower than compartment capacity.
	// SCG is optional;
	// display an explanation message if any error.
	BOOL CheckScgSci();

	// Verification de l'OD indiqué, ou initialisation si vide.
	// Code extrait de CheckAuthorizations() car aussi utilisé par CheckScgSci()
	BOOL CheckOD(YM_Vnl* pVnl);

	// Analyse de m_editvnau (la chaine complete saisie par l'utilisateur) dans le cas SCG, SCI
	// Si OK, en sortie la structure m_NewSci contiendra les valeurs lues.
	// m_NewSci devra avoir été initialisée avec les valeurs lues dans le VNL pour le tronçon cible.
	// Cette fonction servira donc d'une part pour l'analyse syntaxique initiale de la chaine saisie,
	// et d'autre part pour modifier m_NewSci pour chaque leg avant construction du VNAS de ce leg.
	BOOL AnalyzeFullScgi ();

	// Analyse d'une sous-chaine SCG et SCI pour un compartiment dans ce qui a été saisi
	// p est la position de subedit dans la chaine complete m_editvnau
	BOOL AnalyzeScgi (CString subedit, int p);

	// Analyse d'une sous partie SCG ou SCI
	BOOL AnalyseOneScgi (CString sSci, char cmpt);

	// initialisation de m_NewSci avec le contenu du VNL pour le leg indiqué
	// On prend le premier si legorig est vide.
	void InitNewSci (YM_Vnl* pVnl, CString legorig);

	CVnauByLegAuthMap m_LegAuthList; // To store the result of the VNL.
    CVnauAutho m_NewAuth;   // Authorizations entered by the user.
	CVnasAutho m_NewSci;	// SCG and SCI entered by the user.
	int m_goodtranche;
	short m_goodday;
	CString m_goodmonth;
	CString m_jumdata;
	int m_natureRhealys;
	CString m_ValidCmpt;
	// DM 5480 train auto
	BOOL m_bTrainAuto;
	int m_capaTrainAuto;
	

private:
    void EnableEdit (BOOL enabled);
	void EnableVnau (BOOL enabled);
	void BuildVnas();

	//DM4802, ajout d'un contrôle sur la capacite
	void ControlValue (CString& szErrAuthCap, int valeur, int capacity);

	//BOOL VerifyNesting();
	CString m_fvlnrep; // COpy of the initial response of the VNL.
	int m_nbWaitedVnau; // nombre de VNAU sur lequel on attend une notification;
	CArray <int, int> m_CrossMidnightTable;

	// Generated message map functions
	//{{AFX_MSG(CSendVnauDlg)
	afx_msg void OnGetvnl();
	afx_msg void OnBuildVnau();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDVNAUDLG_H__B8320503_E97B_11D1_91DA_400020302A10__INCLUDED_)
