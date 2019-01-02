#if !defined(AFX_VIEWHELPDLG_H__EABECC92_9E13_11D1_8381_400020302A31__INCLUDED_)
#define AFX_VIEWHELPDLG_H__EABECC92_9E13_11D1_8381_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CTAViewHelpDlg.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CCTAViewHelpDlg dialog

class CCTAViewHelpDlg : public CDialog
{
// Construction
public:
	CCTAViewHelpDlg(CView* pParent = NULL);   // standard constructor
  ~CCTAViewHelpDlg();
  void Create(CView* pParent);
  CView* GetParentChart() { return m_pParent; };
  CFont* GetHelpTextFont();

// Dialog Data
	//{{AFX_DATA(CCTAViewHelpDlg)
	enum { IDD = IDD_VIEW_HELP_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCTAViewHelpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCTAViewHelpDlg)
	afx_msg void OnCloseButton();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  CView* m_pParent;

	CFont*  m_pFont;  	  // dialog font
  CBrush* m_pBkBrush; // brush to paint the background

private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWHELPDLG_H__EABECC92_9E13_11D1_8381_400020302A31__INCLUDED_)
