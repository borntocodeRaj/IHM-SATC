#if !defined(AFX_LEGLISTBOXDLG_H__9736FCF3_853D_11D2_8FF8_00104BE1C142__INCLUDED_)
#define AFX_LEGLISTBOXDLG_H__9736FCF3_853D_11D2_8FF8_00104BE1C142__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LegListboxDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLegListboxDlg dialog

typedef CTypedPtrArray<CPtrArray, YmIcLegsDom*> CLegPtrArray;

class CLegListboxDlg : public CDialog
{
public:
	enum LegListSortKey
	{
    eSequence,
    eLegOrig,
    eDefault
	};

// Construction
public:
  CLegListboxDlg(
    CWnd* pParent = NULL, 
    BOOL bSelectAll = FALSE, 
    BOOL bIncCurLeg = TRUE, 
    BOOL bAllowHoles = TRUE, 
    enum LegListSortKey eSortKey = eDefault  );   // standard constructor
	~CLegListboxDlg();

  int GetSelectedItemCount() { return m_iSelectedLegCount; }
  int* GetSelectedItemsIndexArray() { return m_aiSelBuf; };
  void* GetSelectedItem( int iIndex ) { return m_apLegs[ iIndex ]; }
  void SetAllowHoles( BOOL bAllowHoles ) { m_bAllowHoles = bAllowHoles; };

// Dialog Data
	//{{AFX_DATA(CLegListboxDlg)
	enum { IDD = IDD_DLG_LEG_LISTBOX };
	CListBox	m_LegListbox;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLegListboxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  int GetSortPos( int iIndex, YmIcLegsDom* pLeg );

  CWnd* m_pParent;
  int m_iSelectedLegCount;  // the count of legs selected by the user in the listbox
  int (*m_aiSelBuf);  // an array listing the indexes of the legs selected by the user
  CLegPtrArray m_apLegs;
  BOOL m_bSelectAll;  // do we select all legs on initialization of the listbox ?  Default is FALSE
  BOOL m_bIncCurLeg;  // do we include the current leg in the dialog listbox ? Default is TRUE
  BOOL m_bAllowHoles;  // do we force the user to select consecutive legs (i.e., holes or no holes) in the selection ? Default is TRUE
  enum LegListSortKey m_eLegSortKey;

	// Generated message map functions
	//{{AFX_MSG(CLegListboxDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSelctAllLegs();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEGLISTBOXDLG_H__9736FCF3_853D_11D2_8FF8_00104BE1C142__INCLUDED_)
