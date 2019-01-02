// VnauListView.cpp : implementation file
//

#include "stdafx.h"
#include <afxmt.h>
#include <assert.h>

#include "YMApp.h"
#include "YMDtDomSet.h"

#include "CTA.h"
#include "CTADoc.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "CalendarView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CCalendarView

IMPLEMENT_DYNCREATE(CCalendarView, CFormView)

CCalendarView::CCalendarView()
   : CFormView(CCalendarView::IDD)
{
}

CCalendarView::~CCalendarView()
{
}

BEGIN_MESSAGE_MAP(CCalendarView, CFormView)
	//{{AFX_MSG_MAP(CCalendarView)
  ON_WM_SIZE()
  ON_WM_CLOSE()
  ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendarView diagnostics

#ifdef _DEBUG
void CCalendarView::AssertValid() const
{
	CView::AssertValid();
}

void CCalendarView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCalendarView message handlers


void CCalendarView::OnDraw(CDC* pDC)
{
}


void CCalendarView::FrameSizeChange (UINT nType, int cx, int cy)
{
  
}

void CCalendarView::OnDestroy() 
{
  CView::OnDestroy();
 
}

void CCalendarView::OnInitialUpdate() 
{

  CView::OnInitialUpdate();  // base class method
}


void CCalendarView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
 
}





  
