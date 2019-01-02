#if !defined(AFX_CHARTHELPDLG_H__EABECC92_9E13_11D1_8381_400020302A31__INCLUDED_)
#define AFX_CHARTHELPDLG_H__EABECC92_9E13_11D1_8381_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// YMChartHelpDlg.h : header file
//

#include "och2dMFC.h"

// typedef for storing the X-axis labels for help dialogs (OthLegsView and RelTrancheView)
typedef struct 
{
  CString szCmpt;
//  char  cCmpt;
//  CDWordArray alBktArray;
  CStringArray aszBktArray;
} CmptBktLabels;

typedef CTypedPtrArray<CPtrArray, CmptBktLabels*> CCmptBktLabelArray;




/////////////////////////////////////////////////////////////////////////////
// CCTAChartHelpDlg dialog

class CDynaButton : public CButton
{
public:
  CDynaButton(); //Standard Constructor

protected:
	//{{AFX_MSG(CCTAChart)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CCTAChart;

class CCTAChartHelpDlg : public CDialog
{
#define RECTANGLE_WIDTH  8   // this is the width of the color-coded rectangle
#define SPACING 7          // this is the amount of space between the color-coded rectangle and the label
#define CHECKBOXWIDTH 12   // size of check box
	

// Construction
public:
	CCTAChartHelpDlg(CCTAChart* pParent = NULL);   // standard constructor
  ~CCTAChartHelpDlg();
  void Create(CCTAChart* pParent);
  CCTAChart* GetParentChart() { return m_pParent; };
  void ClearHelpWin();
  CFont* GetHelpTextFont();
  CObArray m_CheckBoxArray;
  void ClickOnDynaButton (int nId, int checked); // Appeler sur click sur CDynaButton


// Dialog Data
	//{{AFX_DATA(CCTAChartHelpDlg)
	enum { IDD = IDD_GRAPH_HELP_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCTAChartHelpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  XrtDataHandle m_hXrtData;  // handle to the chart data
  XrtDataHandle m_hXrtData2;  // handle to the chart data 2
  int m_iCurSelectPt;  // current point selected for which to display help
  CSize m_sizeTextLabel;  // size of the text
  CSize m_sizeTextValue;  // size of the values

  void InitHelpWin( int iNumberLabels );
  virtual void GetTitle1( CString& szTitle ) = 0;
  virtual void GetTitle2( CString& szTitle ) = 0;
  virtual CSize GetExtentLongestLabel( CPaintDC& dc ) = 0;
  virtual BOOL GetTextLabel( int iDataSet, CString& szTextLabel, int iCurSelectSet ) = 0;
  virtual BOOL IsRecetteDataSet () { return FALSE; }
  virtual int GetCTADataStyleIndex( int iDataSet, int iCurSelectSet ) = 0;
  virtual CString GetValue( int iDataSet, int iCurSelectSet, int iCurSelectPt );
  virtual int GetCheckBox ( int iDataSet, int iCurSelectSet, int iCurSelectPt );
  virtual void ClickDynaButton (int nId, int checked);
  CPoint DrawTitles( CPaintDC& dc );
  void DrawLabels( CPaintDC& dc, CPoint& pointDraw, BOOL bReverseLabels = FALSE, BOOL bWithCheckBox = FALSE );

 // DM7879	KGH reimplementation de cette methode
 // BOOL DrawThisLabel( int iDataSet, CPaintDC& dc, int iCurSelectSet, CPoint& pointDraw, int iLeftMargin, int iRightMargin, BOOL bWithCheckBox);
   virtual BOOL DrawThisLabel( int iDataSet, CPaintDC& dc, int iCurSelectSet, CPoint& pointDraw, int iLeftMargin, int iRightMargin, BOOL bWithCheckBox);
   //DM7879	KGH FIN


	// Generated message map functions
	//{{AFX_MSG(CCTAChartHelpDlg)
	afx_msg void OnCloseButton();
	afx_msg void OnNextButton();
	afx_msg void OnPreviousButton();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  CCTAChart* m_pParent;

	CFont*  m_pFont;  	  // dialog font
  CBrush* m_pBkBrush; // brush to paint the background
  CUIntArray m_ListCheckIdc; // Liste des identifiants de checkbox.


private:
  void PurgeCheckBoxes();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHARTHELPDLG_H__EABECC92_9E13_11D1_8381_400020302A31__INCLUDED_)
