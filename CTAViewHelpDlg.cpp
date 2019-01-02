// CTAViewHelpDlg.cpp : implementation file
//

#include "StdAfx.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCTAViewHelpDlg dialog

CCTAViewHelpDlg::CCTAViewHelpDlg(CView* pParent /*=NULL*/)
{
  ASSERT(pParent);

  m_pParent = pParent;

  m_pFont    = NULL;
  m_pBkBrush = new CBrush(GetSysColor(COLOR_WINDOW));
}

CCTAViewHelpDlg::~CCTAViewHelpDlg()
{
  if (m_pFont)
  	delete m_pFont;

  delete m_pBkBrush;
}

void CCTAViewHelpDlg::Create(CView* pParent)
{
  // attempt to place it at the previous position
  CPoint* pPrevPoint = 
    ((CMainFrame*) AfxGetApp()->GetMainWnd())->GetHelpDlgPoint();

  // create it...
  if ( ! CDialog::Create(CCTAViewHelpDlg::IDD, m_pParent) )
  {
    m_pParent->MessageBox("Unable to create help window", 
                      AfxGetAppName(), MB_ICONERROR | MB_APPLMODAL);
    return;
  }

  // do we reposition the dialog box to the place it was, previously ?
  if ( pPrevPoint->x != -1 || pPrevPoint->y != -1 )  // x and y are set to default of -1 in CMainFrame constructor
  {
    SetWindowPos( NULL, pPrevPoint->x, pPrevPoint->y, 0, 0, 
                  SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW );
  }
  else
  {
    // Center the help dialog
    CenterWindow( CWnd::GetDesktopWindow() );

    // Get its position and convert it to a point
    CRect rectDlg;
    CPoint pt;
    GetWindowRect( rectDlg );
    pt.x = rectDlg.left;
    pt.y = rectDlg.top;

    // Set this point in the Mainframe
    ((CMainFrame*) AfxGetApp()->GetMainWnd())->SetHelpDlgPoint( pt );
  }

  m_pParent = pParent;
  SetOwner( pParent );

  CString szWindowText;
  CString szHelp = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_HELP);
  m_pParent->GetParentFrame()->GetWindowText( szWindowText );
  szHelp += szWindowText;
  SetWindowText( szHelp );

  if (m_pFont == NULL)
  {
	  m_pFont = new CFont();
    m_pFont->CreateFont(-12,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"MS Sans Serif,7");
  }
}

CFont* CCTAViewHelpDlg::GetHelpTextFont()
{
  return ( ( ((CCTAApp*)APP)->GetDocument()->IsDefaultFont() ) ? 
                m_pFont : 
                ((CCTAApp*)APP)->GetDocument()->GetStdFont(NULL) );
}

void CCTAViewHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCTAViewHelpDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCTAViewHelpDlg, CDialog)
	//{{AFX_MSG_MAP(CCTAViewHelpDlg)
	ON_BN_CLICKED(IDC_CLOSE_BUTTON, OnCloseButton)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCTAViewHelpDlg message handlers

void CCTAViewHelpDlg::OnCloseButton() 
{
  ((CMainFrame*) AfxGetApp()->GetMainWnd())->DestroyHelpDlg();
}

void CCTAViewHelpDlg::OnDestroy() 
{
  // when the dialog is destroyed, save its current dimensions
  CRect rectDlg;
  GetWindowRect(&rectDlg);  // get the dialog position
  CPoint point(rectDlg.left, rectDlg.top);
  ((CMainFrame*) AfxGetApp()->GetMainWnd())->SetHelpDlgPoint(point);

	CDialog::OnDestroy();
}

HBRUSH CCTAViewHelpDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
  if (nCtlColor == CTLCOLOR_DLG)
  {
    pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
    return HBRUSH(*m_pBkBrush);
  }
	
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	return hbr;
}
