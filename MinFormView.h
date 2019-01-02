#if !defined(AFX_CALENDAR_H__INCLUDED_)
#define AFX_CALENDAR_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxcview.h>

#include "YMConstants.h"
#include "ChildFrm.h"

/////////////////////////////////////////////////////////////////////////////
// CComptagesView view

class CCalendarView : public CFormView
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
 
	void FrameSizeChange (UINT nType, int cx, int cy);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CView)
	protected:
	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL


// Operations
public:


// Implementation
protected:
	virtual ~CCalendarView();


  
 
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CComptagesView)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};


  
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALENDAR_H__INCLUDED_)
