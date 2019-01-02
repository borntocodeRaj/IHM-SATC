#if !defined(AFX_CTA_CALENDARVIEW_H__INCLUDED_)
#define AFX_CTA_CALENDARVIEW_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

#include "CalendarWnd.h" //keep it because enum can't be forwarded in the current version of C++

/////////////////////////////////////////////////////////////////////////////
// CCalendarView form view
class CCalendarView : public CFormView, public YM_ObsView 
{
protected:
	CCalendarView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCalendarView)

// Form Data
public:
	//{{AFX_DATA(CCalendarView)
	enum { IDD = IDD_CALENDAR_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
  void SetStatusBarText(UINT nId);
  void SetStatusBarText( CString szText );
  // Virtual de YM_ObsView
  void RestoreViewConfig();
  void WriteViewConfig();
  void FrameSizeChange (UINT nType, int cx, int cy);
  void EraseResa();
 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalendarView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	afx_msg void OnDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCalendarView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCalendarView)
	afx_msg void OnCheckTop1();
	afx_msg void OnCheckTop2();
	afx_msg void OnCheckComment1();
	afx_msg void OnCheckComment2();
	afx_msg void OnCheckVnau1();
	afx_msg void OnCheckVnau2();
	afx_msg void OnCheckCritic1();
	afx_msg void OnCheckCritic2();
	afx_msg void OnCheckTgvJum1();
	afx_msg void OnCheckTgvJum2();
	afx_msg void OnCheckAffaire1();
	afx_msg void OnCheckAffaire2();
	afx_msg void OnCheckDesserte1();
	afx_msg void OnCheckDesserte2();
	afx_msg void OnCheckGestionOD1();
	afx_msg void OnCheckGestionOD2();
	afx_msg void OnCheckRevision1();
	afx_msg void OnCheckRevision2();
	afx_msg void OnCheckNiveau1();
	afx_msg void OnCheckNiveau2();
	afx_msg void OnCheckEquip1();
	afx_msg void OnCheckEquip2();
	afx_msg void OnCheckForce1();
	afx_msg void OnCheckForce2();
	afx_msg void OnCheckSupp1();
	afx_msg void OnCheckSupp2();
	afx_msg void OnCheckService1();
	afx_msg void OnCheckService2();
	afx_msg void OnCheckNomos1();
	afx_msg void OnCheckNomos2();
	afx_msg void OnCheckTaux1ere();
	afx_msg void OnCheckTaux2eme();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	
	CCalendarWnd*  m_pCalendar;
	void UnCheckColumn1();
	void UnCheckColumn2();

	CCalendarWnd::choixAffichage GetCheck1();
	CCalendarWnd::choixAffichage GetCheck2();
	CCalendarWnd::choixAffichage GetChoix (CString sChoix);
	CString GetStrChoix (CCalendarWnd::choixAffichage ch);
	void SetCheck1 (CCalendarWnd::choixAffichage ch);
	void SetCheck2 (CCalendarWnd::choixAffichage ch);
	void LaunchReadTrancheResa();
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTA_CALENDARVIEW_H__INCLUDED_)
