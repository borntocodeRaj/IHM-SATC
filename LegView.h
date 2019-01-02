#if !defined(AFX_LEGVIEW_H__B9B17132_4ABA_11D1_836E_400020302A31__INCLUDED_)
#define AFX_LEGVIEW_H__B9B17132_4ABA_11D1_836E_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LegView.h : header file

class YmIcLegsDom;
class CTrancheView;
class CResaRailView;
class CCTAViewHelpDlg;

/////////////////////////////////////////////////////////////////////////////
// CLegView view

class CLegView : public YM_ListView
{
protected:

	CLegView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CLegView)

// Attributes
protected:

	enum ColumnId 
	{
    UNDEFINED = -1,
    FIRST_COLUMN = 0,
    STATUS = 0,
	LEG_ORIG,
	LEG_DEST,
	OD_MANAGED,
	CRITICALITY,
	CRITICALITYREC,
	DPT_TIME,
	ARV_TIME,
		/// DM-7978 DT8805 - CLE
	  CC_MAX_01,
	  CC_MAX_02,
	  CC_OUV_01,
	  CC_OUV_02,
	/// FIN DM-7978 DT8805 - CLE
	LAST_COLUMN,
    DEFAULT_ORDER,
    REVERSE_ORDERS
	};

// Operations
public:

  CString GetColumnTitle(int iResId);
  CUIntArray* GetSortOrderArray() { return &m_aiSortOrder; }
  CColAttrArray* GetColAttributeArray() { return &m_apColAttr; }
  FUNC_SORT* GetSortFuncArray() { return m_apSortFunctions; }

	enum LegUpdateMode
	{
		CURRENTLY_LOADED_LEG,
		CURRENTLY_SELECTED_LEG,
		PARAMETER_PASSED_LEG,
		ALL_LEGS
	};

	enum LegConfirmUpdateMode
	{
    CONFIRM_ALWAYS,
    CONFIRM_NEVER,
    CONFIRM_ONCE
	};

  void LegSend( BOOL bConfirm = TRUE );
  void LegPasteAuthSelected(int AUorAS);
  void LegPasteAuthList(int AUorAS);
  void LegPasteAuthAll(int AUorAS);
  BOOL LegOKActDiffMoi(int AUorAS); // DM 6496, controle si act diff moi non modifié fen resarail
						  // si oui pose question d'envoi de VNAU et renvoi vrai si user approuve et faux s'il refuse
						  // si non renvoi vrai.
  // DM 7978 (souplesse tarifaire BUGI 69672), controle CC0=CC1 (plus AVT 17)
  // si FALSE, empêcher la mise à jour des VNAU
  // si TRUE, on peut poursuivre.
  BOOL LegOKCC0EgalCC1(int AUorAS,
					   enum LegUpdateMode eUpdateMode); 
  void LegNext();
  void LegPrev();
  void LegDefaultSort();
  void LegReverseSort();
  void LegSelPeak();
  void LoadLeg( YmIcLegsDom* pLeg, int iIndex );
  void LegUndo();
  void LegCriticalMessages();
  void LegHelp();
  void LoadSelectedItem();
  YmIcLegsDom* FindLeg(CString szOrig, CString szDest, int& iIndex);
  YmIcLegsDom* FindFrontier(); // renvoi le tronçon frontière s'il existe. NULL sinon
  CTrancheView* GetTrancheView();

	// NBN ANO 89765
	static int SortCcMax1(LPARAM lParam1, LPARAM lParam2);	
	static int SortCcMax2(LPARAM lParam1, LPARAM lParam2);
	static int SortCcOuv1(LPARAM lParam1, LPARAM lParam2);
	static int SortCcOuv2(LPARAM lParam1, LPARAM lParam2);
	static int CompareCC(CString str1, CString str2);

  BOOL SetUpdateParm(
    enum LegUpdateMode eUpdateMode, 
    YmIcLegsDom** pLegIn,
    YmIcLegsDom** pLegOut, 
    char** pszLegOrigParm);
  void UpdateLeg(int AUorAS,
    enum LegUpdateMode eUpdateMode, 
    YmIcLegsDom* pLegIn = NULL, 
    enum LegConfirmUpdateMode eConfirmMode = CONFIRM_ALWAYS);
  BOOL IsSortDefault();
  BOOL IsSortReverse();
  BOOL IsSelPeak() { return m_bSelPeak; }

  void WriteViewConfig();
  void RestoreViewConfig();

  BOOL SaveAllModified();

  BOOL** GetSortContextPointer() {return &m_pbSortTimeContext; }
  BOOL DrawHeaderImage();  // do we draw header images or simply text ?

  static BOOL GetSortTimeContext() { return *m_pbSortTimeContext; }
  static BOOL* m_pbSortTimeContext;

  BOOL AutoCopyConfirm();
  BOOL Adaptation() { return m_bAdaptationFlag; }
  void SetAdaptationFlag(BOOL bAdaptationFlag) { m_bAdaptationFlag = bAdaptationFlag; }
  BOOL GetOvbAdjustFlag() { return m_bOvbAdjust; }
  void SetOvbAdjustFlag( BOOL bOvbAdjust ) { m_bOvbAdjust = bOvbAdjust; };
  BOOL IsSelectedLegLoaded();
  BOOL GetAutoCopyAuthFlag() { return m_bAutoCopyAuth; }
  void SetAutoCopyAuthFlag(BOOL bAutoCopyAuth) { m_bAutoCopyAuth = bAutoCopyAuth; }
  YmIcLegsDom* GetLoadedLeg() { return m_pLegLoaded; }
  void CtrlScgTranche();	// contrôle la tranche courante pour éventuellement changer son SCG_CAPA.
							// et provoquer un réaffichage si besoin.

  // DM 6977 Sur envoi de VNAU par la boîte de répartition des quotas, positionne en mémoire le bit de valeur valbit du sent_flag sur la tranche/date
  void SetEditStatus (int tranche_no, CString leg_orig, unsigned long ldate, int valbit);
  void SetSentFlag (int tranche_no, CString leg_orig, unsigned long ldate, int valbit);
  void SetScxSentFlag (int tranche_no, CString leg_orig, unsigned long ldate, int valbit);

  // DM 6977 passe en revue la liste des tranches en recherchant celles pour lequel edit_status|2 == 2
  // qui signifie VNAU non envoyé, pour désactiver ce flag et positionner le sent_flag à 1 (flèche noire) 
  void RhealysSendVnau();
  int CriticalityRev (YmIcLegsDom* pLeg); // renvoi la criticalité revenu du leg
  int FirstCriticality (YmIcLegsDom* pLeg);    // renvoi la crititicé principale du leg

  // OBA Ano 70573 recharge le même tronçon en cas de raffraichissement
  void SaveCurrentIndex();
  int m_index;

// Overrides
protected:

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLegView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:

	virtual ~CLegView();
  CFont* GetStdFont(CDC* pDC);
  CFont* GetBoldFont(CDC* pDC);
  void CreateItemImageList();
  void DrawImageList(LPDRAWITEMSTRUCT lpDrawItemStruct, 
                                 UINT uiFlags, 
                                 COLORREF clrTextSave, 
                                 COLORREF clrBkSave, 
                                 COLORREF clrImage);
  CString GetViewTitle();
  CString GetDispInfo(int iSubItem, LPARAM lParam, int iItem);
  int GetPropertyLevel(LPARAM lParam);
  CString GetDefaultSortOrder();
  void InsertColumns();
  BOOL IsLoadedItem(LPARAM lParam) { return ( m_pLegLoaded == (YmIcLegsDom*)lParam ); }
  YM_RecordSet* GetRecordSet() const { return YM_Set<YmIcLegsDom>::FromKey(LEG_KEY); }
  int GetMessageNo(LPARAM plParam) const { return ((YmIcLegsDom*)plParam)->MessageNo(); }
  int GetMessageNoRec(LPARAM plParam) const { return ((YmIcLegsDom*)plParam)->MessageNoRec(); }
  void EnableToolTips();

  void DisplayHelpDlg();
  CCTAViewHelpDlg* CreateHelpDlg();

  CString GetToolTipText(LVITEM* pItem) const;

  static int SortLegEditStatus(LPARAM lParam1, LPARAM lParam2);
  static int SortLegOrig(LPARAM lParam1, LPARAM lParam2);
  static int SortLegCmpt(LPARAM lParam1, LPARAM lParam2);
  static int SortLegODManaged(LPARAM lParam1, LPARAM lParam2);
  static int SortLegCriticality(LPARAM lParam1, LPARAM lParam2);
  static int SortLegCriticalityRec(LPARAM lParam1, LPARAM lParam2);
  static int SortDummy(LPARAM lParam1, LPARAM lParam2);

// Generated message map functions
protected:
	//{{AFX_MSG(CLegView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg void OnLegSend();
	afx_msg void OnLegSendNoQuery();
	afx_msg void OnUpdateLegSend( CCmdUI* pCmdUI );
	afx_msg void OnLegPasteAuthSelectedVnau();
	afx_msg void OnLegPasteAuthSelectedVnas();
	afx_msg void OnLegPasteAuthSelectedLes2();
	afx_msg void OnUpdateLegPasteAuthSelected( CCmdUI* pCmdUI );
	afx_msg void OnUpdateLegPasteAuthSelectedVnas( CCmdUI* pCmdUI );
	afx_msg void OnLegPasteAuthListVnau();
	afx_msg void OnLegPasteAuthListVnas();
	afx_msg void OnLegPasteAuthListLes2();
	afx_msg void OnUpdateLegPasteAuthList( CCmdUI* pCmdUI );
	afx_msg void OnUpdateLegPasteAuthListVnas( CCmdUI* pCmdUI );
	afx_msg void OnLegPasteAuthAllVnau();
	afx_msg void OnLegPasteAuthAllVnas();
	afx_msg void OnLegPasteAuthAllLes2();
	afx_msg void OnUpdateLegPasteAuthAll( CCmdUI* pCmdUI );
	afx_msg void OnUpdateLegPasteAuthAllVnas( CCmdUI* pCmdUI );
    afx_msg void OnLegLoad();
	afx_msg void OnUpdateLegLoad( CCmdUI* pCmdUI );
	afx_msg void OnLegNext();
	afx_msg void OnUpdateLegNext( CCmdUI* pCmdUI );
	afx_msg void OnLegPrev();
	afx_msg void OnUpdateLegPrev( CCmdUI* pCmdUI );
	afx_msg void OnLegUndo();
	afx_msg void OnUpdateLegUndo( CCmdUI* pCmdUI );
    afx_msg void OnLegAdjust();
	afx_msg void OnUpdateLegAdjust( CCmdUI* pCmdUI );
    afx_msg void OnLegAutoCopyAuth();
	afx_msg void OnUpdateLegAutoCopyAuth( CCmdUI* pCmdUI );
    afx_msg void OnLegCriticalMessages();
    afx_msg void OnUpdateLegCriticalMessages( CCmdUI* pCmdUI );
	afx_msg void OnLegDefaultSort();
	afx_msg void OnUpdateLegDefaultSort( CCmdUI* pCmdUI );
	afx_msg void OnLegReverseSort();
	afx_msg void OnUpdateLegReverseSort( CCmdUI* pCmdUI );
	afx_msg void OnLegSelPeak();
	afx_msg void OnUpdateLegSelPeak( CCmdUI* pCmdUI );
	DECLARE_MESSAGE_MAP()

private:
  // these member variables are used to store the index of each column
  int   m_iStatus;
  int   m_iLegOrig;
  int   m_iLegDest;

  /// DM-7978 DT8805 - CLE 
  int m_iCcMax01;
  int m_iCcMax02;
  int m_iCcOuv01;
  int m_iCcOuv02;
  /// FIN DM-7978 DT8805 - CLE 

  int   m_iDptTime;
  int	m_iArvTime;
  int   m_iCmpt;
  int   m_iODManaged;
  int   m_iCriticality;
  int   m_iCriticalityRec;
  int   m_iLegCritCount;
  int   m_iCritMessages;
  int   m_iCapacity;
  int   m_iResHoldTot;
  int   m_iResHoldGrp;
  int   m_iResNonYield;
  int   m_iUnreservableCap;
  int   m_iStandingCap;
  int   m_iDefaultOrder;
  int   m_iReverseOrder;

  YmIcLegsDom* m_pLegLoaded;
  YmIcLegsDom* m_pLegPrevious;
  CCTAViewHelpDlg* m_pHelpDlg;
  BOOL m_bOvbAdjust;
  BOOL m_bAutoCopyAuth;
  BOOL m_bAdaptationFlag;
  BOOL m_bSelPeak;

  CResaRailView* GetResaRailView();
  void SetFlagTruc (int truc, int tranche_no, CString leg_orig, unsigned long ldate, int valbit);

  CColAttrArray m_apColAttr;
  CUIntArray m_aiSortOrder;
  FUNC_SORT m_apSortFunctions[MAX_COLUMNS];
//  int (*m_apSortFunctions[MAX_COLUMNS])(LPARAM pRecord1, LPARAM pRecord2);

  static const char szAutoCopyAuthConst[];
  static const char szPreserveDiffConst[];
  static const char szLoadPeakConst[];
};

/////////////////////////////////////////////////////////////////////////////
// CLegAltView view

class CLegAltView : public CLegView
{
friend class CLegView;
	CLegAltView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CLegAltView)

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLegAltView)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

};

/////////////////////////////////////////////////////////////////////////////
// CLegViewHelpDlg dialog

class CLegViewHelpDlg;

class CLegViewHelpDlg : public CCTAViewHelpDlg
{
// Construction
public:
  CLegViewHelpDlg(CLegView* pParent = NULL);
  CLegView* GetParent() { return ((CLegView*)CWnd::GetParent()); };

// Dialog Data
	//{{AFX_DATA(CLegViewHelpDlg)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLegViewHelpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

void DrawLine( CPaintDC& dc, CImageList* pImageList, 
                            int iIconId, CPoint& ptDrawIcon, 
                            int iLabelId, CPoint& ptDrawTxt );

	// Generated message map functions
	//{{AFX_MSG(CLegViewHelpDlg)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEGVIEW_H__B9B17132_4ABA_11D1_836E_400020302A31__INCLUDED_)
