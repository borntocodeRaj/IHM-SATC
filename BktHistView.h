#if !defined(AFX_BKTHISTVIEW_H__58915282_709D_11D1_837A_400020302A31__INCLUDED_)
#define AFX_BKTHISTVIEW_H__58915282_709D_11D1_837A_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BktHistView.h : header file
//

class CBktHistView;
class CChildFrame;
class CCTADoc;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CBktHistCommentIcon window

class CBktHistCommentIcon : public CWnd
{
  // Construction
public:
  CBktHistCommentIcon(HICON hicon, CString comment);
  
  // Attributes
public:
  HICON m_hIcon;
  CString m_Commentaire;
  
  // Operations
public:
  
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CBktHistCommentIcon)
  //}}AFX_VIRTUAL
  
  // Implementation
public:
  virtual ~CBktHistCommentIcon();
  
  // Generated message map functions
protected:
  //{{AFX_MSG(CBktHistCommentIcon)
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnPaint();
  //}}AFX_MSG
  
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CBktHistJumLink window (représentée par une simple icone chronometre)
// On utilise une fenêtre pour reperer les clic qui y sont fait.

class CBktHistJumLink : public CWnd, public YM_Observer
{
#define BKT_JUM_QUERY_KEY  300
  
  // Construction
public:
  CBktHistJumLink(HICON hicon, long jumno, unsigned long dptdate,
    const char* legorig, CBktHistView* papa);
  void OnNotify(YM_Observable* pObject);
  
  // Attributes
public:
  HICON m_hIcon;
  long  m_jumno;
  unsigned long  m_dptdate;
  CString m_legorig;
  CBktHistView* m_papa;
  static int g_bktjumkey;
  
private:
  YM_RecordSet* m_pBktJumHistSet;
  int m_rKey; // key for m_pBktJumHistSet
  
  // Operations
public:
  
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CBktHistJumLink)
  //}}AFX_VIRTUAL
  
  // Implementation
public:
  virtual ~CBktHistJumLink();
  void ForceDisplay(BOOL display);
  BOOL CanDisplay(BOOL display);
  
  // Generated message map functions
protected:
  //{{AFX_MSG(CBktHistJumLink)
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnPaint();
  //}}AFX_MSG
  
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// Internal structures

// The Array below will contains all the BktHistDom for ONE compartment, ordered by bucket
typedef CTypedPtrArray<CPtrArray, YmIcBktHistDom*> CBktHistBktArray;


// Contains one CBktHistBktArray per compartment. Contains all the information
// displayed for a departure date.
class CBktHistCmptMap : public CTypedPtrMap<CMapStringToPtr, CString, CBktHistBktArray*>
{
public:
  CBktHistCmptMap(BOOL bJumMap);
  ~CBktHistCmptMap();
  void Add(YmIcBktHistDom* pBktHist);
  int GetNbrLines();
  void CalculAvecMatriceCC8();
  void CalculAvecMatrice(CMapStringToOb* pMatrice, BOOL bForce);
						// Si bForce, le recalcul est fait même si même nombre de bucket
  void MajOnRad(YmIcBktHistDom* pBktHist);

  // members containing info on the displayed tranche
  long m_lTrancheNo; 
  CString m_szTrainNo;   
  CBktHistCommentIcon* m_pCommentIcon;
  CBktHistJumLink* m_pJumeauxLink;
  long m_sup_flag;
  long m_force_status;
  long m_supplement_level;
  int m_historef;
  BOOL bCoeffMatriceCC8;
  BOOL m_bJumMap;
};


// Contains one CBktHistCmptMap per date.
class CBktHistDateMap : public CMap<long, long, CBktHistCmptMap*, CBktHistCmptMap*&>
{
public:
  CBktHistDateMap(BOOL bJumMap = FALSE);
  ~CBktHistDateMap();
  void Add(YmIcBktHistDom* pBktHist);
  void Empty();
  int GetNbrLines();
  void DisplayAllJum(BOOL display);
  BOOL CanDisplayAll(BOOL display);
  void TakeCoeffHisto();
  void MajOnRad(YmIcBktHistDom* pBktHist);
  CImageList m_ImageList;
  CBktHistView* m_papa;
  BOOL m_bJumMap;
  
};

class CBktHistTcDate
{
public:
  CBktHistTcDate( LPCTSTR txtdt, long lgdt, LPCTSTR pszKey );

  CString   _szText;
  long	    _lgdate;
  CString   _sKey;
};

class CBktHistDateTable : public CArray<CBktHistTcDate*, CBktHistTcDate*&>
{
public:
  CBktHistDateTable();
  ~CBktHistDateTable();
  void Empty();
  void Insert(CBktHistTcDate* ptcDate);
  void Renverse();
  
private:
  BOOL m_reversed;
};

/////////////////////////////////////////////////////////////////////////////
// CBktHistView view
class CBktHistView : public CScrollView, public YM_ObsView 
{
  friend class CBktHistWnd;
  
#define IDC_TC_DATE	100
//DM-7978-YI-5849
#define IDC_TC_GLO_RED	101
#define IDC_TC_SIZE	23
//End DM-7978-YI-5849

protected:
  CBktHistView();           // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CBktHistView)
    
    // Attributes
public:
  CCTADoc* GetDocument();
  BOOL GetAltFlag() const;
  
  void UpdateScrollSize();
  void UpdateSisterAlso();

  
  // Operations
public:
 	BOOL CanCloseFrame() 
  {
    return TRUE;
  };
  void SetStatusBarText(UINT nId);
  void FrameSizeChange(UINT nType, int cx, int cy);
  void WriteViewConfig();
  void RestoreViewConfig();
	void ChangeSelTC(const int & idx);
	void CtReload();
  CString GetTDforRAD(); // construit une string avec liste de cmpt/tranche/nest_level/date
						 // devant être verifier dans sc_prevauth.     DM 6496 remise à disposition
		// exemple A',1234,5,TO_DATE('2008/06/24','YYYY/MM/DD')),('B',1234,6,TO_DATE('2008/06/24','YYYY/MM/DD
        // Cette string sert d'entree à la requete IDS_SQL_PREVAUTH. Il y aura ajout de quote en début et fin par le dte
		// Les doubles parentheses de début et fin sont dans la requete afin que le premier et dernier caractère de
        // la string insérée dans la requête soient des simple quote
        // Ne renvoi pas de string vide si rien, mais une série de valeurs bidon pour passer la requête
  
  CPtrArray m_ComptagesArray;

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CBktHistView)
public:
  virtual void OnInitialUpdate();
protected:
  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  //}}AFX_VIRTUAL
  
  // Implementation
protected:
  virtual ~CBktHistView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif
  
  // Generated message map functions
protected:
  //{{AFX_MSG(CBktHistView)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnShowWindow (BOOL bShow, UINT nStatus);
  afx_msg void OnDestroy();
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  //}}AFX_MSG
  afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnShowAllJum();
  afx_msg void OnMaskAllJum();
  afx_msg void OnUpdatePrint(CCmdUI* pCmdUI); 
  DECLARE_MESSAGE_MAP()
    
private:
  // Current Tranche, DptDate and Train number
  long m_lCurTrancheNo, _lCurDptDate;
  CString m_szCurTrainNo;
  long m_lCurSupplement;
  BOOL m_bBktHistIsValid;
  BOOL m_bBktHistoIsValid;
  RWDate m_rwJanFirst2001;
  BOOL m_bRadDone;

  BOOL m_bColCount;  // for splitter window
  BOOL	_bSimpleSortByDate;	// False for SNCF
  
  // Ordering compartment
  // return the compartment character following c, according to CMPT_SEQ in SC_COMPARTMENT
  // return 0 if c is the last cmpt. Set c = 0 to get the first cmpt
  char GetNextCmpt(char c);

  LPCTSTR   buildKey( long lDate, CBktHistCmptMap* pBktHistCmptMap, CString & sKey );
  CBktHistView* getOtherView();
  
  // Drawing methods
  void FormatRWDate(CString& s, long lDate);
  void PaintIt(CDC* pDC);
  void DrawLabel(CDC* pDC, UINT nID, LPRECT lpRect, BOOL bUnderline);
  void DrawLongValue(CDC* pDC, long lValue, LPRECT lpRect, BOOL bUnderline);
  void DrawTextValue(CDC* pDC, CString s, LPRECT lpRect, BOOL bUnderline);
  void Underline(CDC* pDC, LPRECT lpRect);
	//DM7978 - NCH
	void CCUpdate(CDC*& pDC, YmIcBktHistDom*& pBktHist, const CBktHistBktArray*& pBktHistBktArray,
								CRect &rectb, CPoint &pt, const CSize &szField, const CPoint &ptOffset, CPen &revPen,
								long &lResHoldTotal, long &lResHoldGrpTotal, int &iSomRec, BOOL &bPrixGaranti,
								BOOL &Flag_BuildOK, COLORREF &clrFt, TEXTMETRIC &tm, const int &bucketIndex, const int &bucketItemsSize);

   //DM 6496 remise à disposition
  void MajFromRad(int reqKey);
  
  // Internal structure
  CBktHistDateMap m_BktHistDateMap;
  CBktHistDateTable m_DateTable;

  CFont* m_pItalFont;

  //DM7978 - NCH
  CTabCtrl		m_tcGlobRed;
  int					m_nLastTab;	
	CRect				m_rectb;
	BOOL				m_flagUpdated;
  //End DM7978-NCH
};

/////////////////////////////////////////////////////////////////////////////
// CBktHistAltView view

class CBktHistAltView : public CBktHistView
{
  friend class CBktHistView;
  CBktHistAltView();           // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CBktHistAltView)
    
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CBktHistAltView)
public:
  virtual void OnInitialUpdate();
  //}}AFX_VIRTUAL
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BKTHISTVIEW_H__58915282_709D_11D1_837A_400020302A31__INCLUDED_)
