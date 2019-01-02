// YMChartHelpDlg.cpp : implementation file
//
#include "StdAfx.h"

#include "CTA_Resource.h"
#include "CTAChart.h"
#include "CTAChartHelpDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const CString LABEL_PREV_INDISPO = "Prévision indisponible";

/////////////////////////////////////////////////////////////////////////////
CDynaButton::CDynaButton()
{
}

BEGIN_MESSAGE_MAP(CDynaButton, CButton)
	//{{AFX_MSG_MAP(CCTAChart)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDynaButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
  CButton::OnLButtonDown(nFlags, point);
  int nId = GetDlgCtrlID();
  // correction de l'ano 159666, on gere le check nous même.
  int k = GetCheck();
  SetCheck(k ? 0 : 1);
  ((CCTAChartHelpDlg*)GetParent())->ClickOnDynaButton (nId, k);
}

void CDynaButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
}

/////////////////////////////////////////////////////////////////////////////
// CCTAChartHelpDlg dialog

#if 0
CCTAChartHelpDlg::CCTAChartHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCTAChartHelpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCTAChartHelpDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#endif

CCTAChartHelpDlg::CCTAChartHelpDlg(CCTAChart* pParent /*=NULL*/)
{
  ASSERT(pParent);

  m_pParent = pParent;
  m_pFont    = NULL;

  m_pBkBrush = new CBrush(GetSysColor(COLOR_WINDOW));

  m_iCurSelectPt = 0;
  m_sizeTextLabel.cx = 0;
  m_sizeTextLabel.cy = 0;
  m_sizeTextValue.cx = 0;
  m_sizeTextValue.cy = 0;

  m_hXrtData = NULL;
  m_hXrtData2 = NULL;
}

CCTAChartHelpDlg::~CCTAChartHelpDlg()
{
  if (m_pFont)
  {
  	delete m_pFont;
    m_pFont = NULL;
  }
  if ( m_pBkBrush )
  {
    delete m_pBkBrush;
    m_pBkBrush = NULL;
  }
}

void CCTAChartHelpDlg::Create(CCTAChart* pParentChart)
{
  // attempt to place it at the previous position
  CPoint* pPrevPoint = 
    ((CMainFrame*) AfxGetApp()->GetMainWnd())->GetHelpDlgPoint();

  // create it...
  if ( ! CDialog::Create(CCTAChartHelpDlg::IDD, m_pParent) )
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

  m_pParent = pParentChart;
  SetOwner( pParentChart );

  CString szWindowText;
  m_pParent->GetParentFrame()->GetWindowText( szWindowText );
  SetWindowText( szWindowText );

  if (m_pFont == NULL)
  {
	  m_pFont = new CFont();
    m_pFont->CreateFont(-12,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"MS Sans Serif,9");
  }
}

CFont* CCTAChartHelpDlg::GetHelpTextFont()
{
  return ( ( ((CCTAApp*)APP)->GetDocument()->IsDefaultFont() ) ? 
                m_pFont : 
                ((CCTAApp*)APP)->GetDocument()->GetStdFont(NULL) );
}

void CCTAChartHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCTAChartHelpDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


// Clear the help dialog window
void CCTAChartHelpDlg::ClearHelpWin()
{
  CRect rectWnd, rectClient;
  GetWindowRect( &rectWnd );
  GetClientRect( &rectClient );

  // set brush and mode
  CDC* pDC = GetDC();
  int OldMode = pDC->SetMapMode(MM_TEXT);  // set the new mode and hold the old mode
  CBrush brushBkgd( ::GetSysColor(COLOR_WINDOW) );
  CBrush* pOldBrush = pDC->SelectObject(&brushBkgd);   // select the new brush and hold the old one

  pDC->FillRect( &rectClient, &brushBkgd );

  pDC->SetMapMode(OldMode);         // restore the old mode
  pDC->SelectObject(pOldBrush);     // restore the old brush
}

// Initialize the help dialog window
void CCTAChartHelpDlg::InitHelpWin( int iNumberLabels )
{
#define NUMBER_BUTTONS 3

  iNumberLabels = 10;

  CDC* pDC = GetDC();
  CFont* pOldFont = pDC->SelectObject( GetHelpTextFont() );

  // how much space for the drawing region ?
  CSize m_sizeTextLabel = pDC->GetOutputTextExtent( "W" );  // get height of the text
  CSize m_sizeTextValue = pDC->GetOutputTextExtent( "9" );  // get height of the text
  long lDrawingRegion = (m_sizeTextLabel.cy * 2) * (2 + iNumberLabels);

  // set the dimensions of the dialog (adjust the height)
  //
  // determine the difference between the height of client and frame
  CRect rectWnd, rectClient;
  GetWindowRect( &rectWnd );
  GetClientRect( &rectClient );
  long lHeightDiff = rectWnd.Height() - rectClient.Height();

  CSize sizeText = pDC->GetOutputTextExtent( "W" );  // get height of the text

  // adjust the bottom
  CRect rectButton;
  GetDlgItem( IDC_PREVIOUS_BUTTON )->GetWindowRect(&rectButton);
  GetWindowRect( &rectWnd );
  rectWnd.bottom =  rectWnd.top + lHeightDiff + lDrawingRegion + rectButton.Height();
  
  // resize the dialog window
  SetWindowPos( NULL,rectWnd.left, rectWnd.top, rectWnd.Width(), rectWnd.Height(), SWP_NOZORDER | SWP_SHOWWINDOW );
  GetClientRect( &rectClient );  // get the client rect again...


  // adjust the buttons
  long lButtonSpacing = (rectClient.Width() - (rectButton.Width()*NUMBER_BUTTONS)) / (NUMBER_BUTTONS+1);
  long lButtonTop     = (rectClient.bottom - rectButton.Height() - sizeText.cy/2);
  GetDlgItem( IDC_PREVIOUS_BUTTON )->SetWindowPos( NULL, lButtonSpacing, lButtonTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW );
  GetDlgItem( IDC_CLOSE_BUTTON )->GetWindowRect(&rectButton);
  GetDlgItem( IDC_CLOSE_BUTTON )->SetWindowPos( NULL, (lButtonSpacing*2)+rectButton.Width(), lButtonTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW );
  GetDlgItem( IDC_NEXT_BUTTON )->GetWindowRect(&rectButton);
  GetDlgItem( IDC_NEXT_BUTTON )->SetWindowPos( NULL, (lButtonSpacing*3)+(rectButton.Width()*2), lButtonTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW );

  // get the chart data values
  XrtGetValues( m_pParent->m_hChart, XRT_DATA, &m_hXrtData, NULL);
  XrtGetValues( m_pParent->m_hChart, XRT_DATA2, &m_hXrtData2, NULL);

  // get the index to the currently selected data point (make sure it's not too far)
  m_iCurSelectPt = __min( m_pParent->GetCurSelectPt(), XrtDataGetNPoints( m_hXrtData, m_pParent->GetCurSelectSet() ) - 1 );
}

// returns the X and Y Offsets
CPoint CCTAChartHelpDlg::DrawTitles( CPaintDC& dc )
{
  CString szTitle1;
  GetTitle1( szTitle1 );

  CString szTitle2;
  GetTitle2( szTitle2 );

  if(!IsRecetteDataSet())
  {
     dc.SetTextColor(RGB(0,0,0));	//Noir
  }
  else
  {
     dc.SetTextColor(COLOR_RECETTE);
  }

  // Le client demande a ce que le titre de la boite d'aide contienne "Comptage" lorsque qu'elle affiche des données comptage
  // Codage cochon, vu que je teste sur le contenu des titres ou des libellés
  CString szWndTitle;
  if (szTitle1 == "J: C")
  {
	GetWindowText (szWndTitle);
    bool bAlt = (szWndTitle.Find ("(ALT)") >= 0);
    int idTitle = bAlt ? IDS_RRDHIST_ALT_CPT_TITLE : IDS_RRDHIST_CPT_TITLE;
	szWndTitle = ((CCTAApp*)APP)->GetResource()->LoadResString(idTitle);
  }
  else
	m_pParent->GetParentFrame()->GetWindowText( szWndTitle );
  SetWindowText( szWndTitle );

  // calculate the X offset of the 1st title
  CSize sizeText = dc.GetOutputTextExtent( szTitle1 );
  CRect rectClient;
	GetClientRect(&rectClient);
  int iXOffset = (rectClient.Width() - sizeText.cx) / 2;  // center it in the dlg
  // calculate the Y offset of the 1st title...
  int iYOffset = (sizeText.cy/2);
  // create a point object
  CPoint pointDraw(iXOffset, iYOffset);

  // set the default system background color for the drawing
  dc.SetBkColor( ::GetSysColor(COLOR_WINDOW) );

  // set brush and mode
  int OldMode = dc.SetMapMode(MM_TEXT);  // set the new mode and hold the old mode
  CBrush brushBkgd( ::GetSysColor(COLOR_WINDOW) );
  CBrush* pOldBrush = dc.SelectObject(&brushBkgd);   // select the new brush and hold the old one

  // make sure that we clear the area from the previous drawing's title
  CSize sizeClear = dc.GetOutputTextExtent( szTitle1 );
  sizeClear.cx = rectClient.Width();  // change the width of the rectangle to stretch across the dialog
  CPoint pointClear(0, iYOffset);
 	CRect rectTextTitleClear(pointClear, sizeClear);    // create a rect object
  dc.FillRect( &rectTextTitleClear, &brushBkgd );

  // draw the 1st title of the help dialog
 	CRect rectTextTitle(pointDraw, sizeText);    // create a rect object
  dc.FillRect( &rectTextTitle, &brushBkgd );
  dc.DrawText( szTitle1, rectTextTitle, DT_RIGHT | DT_SINGLELINE );

  // calculate the X offset of the 2nd title
  sizeText = dc.GetOutputTextExtent( szTitle2 );
  iXOffset = (rectClient.Width() - sizeText.cx) / 2;  // center it in the dlg
  // calculate the Y offset of the 2nd title...
  iYOffset += sizeText.cy;

  // draw the 2nd title of the help dialog
  pointDraw.x = iXOffset;
  pointDraw.y = iYOffset;

  // make sure that we clear the previous title
  sizeClear = dc.GetOutputTextExtent( szTitle2 );
  sizeClear.cx = rectClient.Width();  // change the width of the rectangle to stretch across the dialog
  pointClear.y = iYOffset;
 	CRect rectTextTitle2Clear(pointClear, sizeClear);    // create a rect object
  dc.FillRect( &rectTextTitle2Clear, &brushBkgd );

  if ( ! szTitle2.IsEmpty() )
  {
  rectTextTitle.SetRect(pointDraw.x, pointDraw.y, pointDraw.x + sizeText.cx, pointDraw.y + sizeText.cy);
  dc.FillRect( &rectTextTitle, &brushBkgd );
  dc.DrawText( szTitle2, rectTextTitle, DT_RIGHT | DT_SINGLELINE );  // draw the 2nd title
  }
  else
  {
  iYOffset -= sizeText.cy;
  }

  dc.SetMapMode(OldMode);         // restore the old mode
  dc.SelectObject(pOldBrush);     // restore the old brush

  return CPoint( iXOffset, iYOffset );
}

//Virtual
CString CCTAChartHelpDlg::GetValue( int iDataSet, int iCurSelectSet, int iCurSelectPt )
{
  double dValue;
  if ( iDataSet == XRT_DATA )
  {
    dValue = XrtDataGetYElement( m_hXrtData, iCurSelectSet, m_iCurSelectPt );

  }
  else  // iDataSet == XRT_DATA2
  {
    dValue = XrtDataGetYElement( m_hXrtData2, iCurSelectSet, m_iCurSelectPt );
  }
  CString szValue;
  if (dValue == INVALID_GRAPH_VALUE)
    szValue = "N/A";
  else
    szValue.Format( "%5.0f", dValue );

  return szValue;
}

//Virtual
int CCTAChartHelpDlg::GetCheckBox( int iDataSet, int iCurSelectSet, int iCurSelectPt)
{
  return 0;
}

//Virtual
void CCTAChartHelpDlg::ClickDynaButton (int nId, int checked) 
{
}

// Appelé directment par le CDynaButton qui ne connait que CCTAChartHelpDlg, et non la parent.
void CCTAChartHelpDlg::ClickOnDynaButton (int nId, int checked)
{
  ClickDynaButton(nId, checked);
}

BOOL CCTAChartHelpDlg::DrawThisLabel( int iDataSet, CPaintDC& dc, int iCurSelectSet, CPoint& pointDraw,
									 int iLeftMargin, int iRightMargin, BOOL bWithCheckBox)
{
  //  get the index which points to the style for which we draw the color-coded rectangle...
  //  note that if this method returns -1, we get out immediately because we don't want to deal with it
  int iCtaDataStyleIdx = GetCTADataStyleIndex( iDataSet, iCurSelectSet );  // virtual method
  if ( iCtaDataStyleIdx < 0 )
    return FALSE;  // we report that we didn't draw anything

  // get the text label to be displayed
  CString szTextLabel;
  if ( ! GetTextLabel( iDataSet, szTextLabel, iCurSelectSet ) )  // virtual method
    return FALSE;  

  // get the numeric value (as a CString) to be displayed
  CString szValue = GetValue( iDataSet, iCurSelectSet, m_iCurSelectPt  );  // virtual method
  if ( szValue.IsEmpty() )  // if GetValue returns an empty string, we don't draw anything...
    return FALSE;

  int checkidc = GetCheckBox ( iDataSet, iCurSelectSet, m_iCurSelectPt );

  m_sizeTextLabel = dc.GetOutputTextExtent( szTextLabel );

  // create rect objects to draw the label and the value
  CRect rectTextLabel(pointDraw, m_sizeTextLabel);    // create a rect object for the text label
  CRect rectTextValue(pointDraw, m_sizeTextValue);    // create a rect object for the value

  // set a rectangle for the text label to be drawn
  rectTextLabel.SetRect( pointDraw.x, pointDraw.y, 
    pointDraw.x + m_sizeTextLabel.cx, pointDraw.y + m_sizeTextLabel.cy );

  // set the default system background color for the drawing
  dc.SetBkColor( ::GetSysColor(COLOR_WINDOW) );
  CBrush brushBkgd( ::GetSysColor(COLOR_WINDOW) );

  // now draw the text label
  dc.FillRect( &rectTextLabel, &brushBkgd );
  dc.DrawText( szTextLabel, rectTextLabel, DT_RIGHT | DT_SINGLELINE );

  // set a rectangle for the value to be drawn
  rectTextValue.SetRect( iRightMargin - m_sizeTextValue.cx, pointDraw.y,
    iRightMargin, pointDraw.y + m_sizeTextValue.cy );

  // now draw the value
  dc.FillRect( &rectTextValue, &brushBkgd );
  dc.DrawText( szValue, rectTextValue, DT_RIGHT | DT_SINGLELINE );

  // now draw the color-coded rectangle...
/*
  int iCtaDataStyleIdx = GetCTADataStyleIndex( iDataSet, iCurSelectSet );  // virtual method
  if ( iCtaDataStyleIdx < 0 )
    return;
*/
  CBitmap bmpBrush;
  CBrush brush;
  COLORREF clrHoldTextColor = dc.GetTextColor( );  // hold the text color
  switch ( m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->fpat )
  {
  case XRT_FPAT_25_PERCENT :
    dc.SetTextColor( m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->color );
    bmpBrush.LoadBitmap( IDB_PATTERN25 );
    brush.CreatePatternBrush( &bmpBrush );
    break;
  case XRT_FPAT_VERT_STRIPE :
    brush.CreateHatchBrush( HS_CROSS, m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->color );
    break;
  case  XRT_FPAT_HORIZ_STRIPE :
    brush.CreateHatchBrush( HS_HORIZONTAL, m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->color );
    break;
  case XRT_FPAT_DIAG_HATCHED :
    brush.CreateHatchBrush( HS_FDIAGONAL, m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->color );
    break;
  case XRT_FPAT_CROSS_HATCHED :
    brush.CreateHatchBrush( HS_DIAGCROSS, m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->color );
    break;
  case XRT_FPAT_SOLID :
  default :
    brush.CreateSolidBrush( m_pParent->m_aDataStyles.GetAt(iCtaDataStyleIdx)->color );
    break;
  }

  // position the color-coded rectangle (legend box) just to the left of the text
  CRect rectBox( iLeftMargin + (bWithCheckBox ? CHECKBOXWIDTH : 0), rectTextLabel.top, 
    (rectTextLabel.left - SPACING), rectTextLabel.bottom );

  dc.SelectObject(&brush);
  dc.Rectangle(&rectBox);

  // restore the text color
  dc.SetTextColor( clrHoldTextColor );

  // eventuelle creation checkBox -- DT Prevision
  if (checkidc)
  { // checkidc negatif signifie que la case n'est pas checké.
	int idcbox = (checkidc < 0) ? -checkidc : checkidc;
	CDynaButton *pButton = (CDynaButton*)GetDlgItem (idcbox);
	if (!pButton)
	{ // La checkbox n'existe pas il faut la creer
	  CRect rectButton (iLeftMargin - 30, rectTextLabel.top, (iLeftMargin - 30)+CHECKBOXWIDTH, rectTextLabel.bottom);
	  pButton = new CDynaButton();
	  BOOL bb = pButton->Create ("", BS_CHECKBOX | WS_CHILD | WS_VISIBLE, rectButton, this, idcbox);
	  // correction de l'ano 159666, utilisation de BS_CHECKBOX au lieu de BS_AUTOCHECKBOX
	  // EN mode autocheck on passait parfois sur un double clic qui faisait que le check n'était plus en 
	  // phase avec l'affichage de la courbe.
	  if (bb)
		m_ListCheckIdc.Add(idcbox);
	}
	pButton->SetCheck (checkidc > 0);

	// Ano 106681
	pButton->EnableWindow(LABEL_PREV_INDISPO.Compare(szTextLabel));
  }

  return TRUE;
}

void CCTAChartHelpDlg::DrawLabels( CPaintDC& dc, CPoint& pointDraw, BOOL bReverseLabels, /* = FALSE */
								  BOOL bWithCheckBox /*= FALSE*/)
{
  // get the text extents of the labels and values to be drawn
  //  Note: we choose the longest string to accomodate the others
  int iWidthCheckBox = 0;
  if (bWithCheckBox)
	iWidthCheckBox = CHECKBOXWIDTH;
  m_sizeTextLabel = GetExtentLongestLabel( dc );
  CString szValue;
//  szValue.Format( "%3.0f", 999.999 );
  szValue.Format( "%5.0f - %5.0f", 99999., 99999. );
  m_sizeTextValue = dc.GetOutputTextExtent( szValue );
  
  // recalculate the X offset
  // note that we center the lable in the middle of the dialog box
  CRect rectClient;
  GetClientRect(&rectClient);
  pointDraw.x = (rectClient.Width() - m_sizeTextLabel.cx) / 2;
  // DM7978 KGH adaptation pour l'affichage des libelles longs (spheres)
  //pointDraw.x = (int)((double)pointDraw.x * .75);  // move it over to the left by 25%
  pointDraw.x = (int)((double)pointDraw.x * .5);  // move it over to the left by 50%: sert a adapter l'affichage des libelles longs (spheres)
	// DM7978 KGH FIN

  // recalculate the Y offset
  // (i.e., skip down to where the group of lines will be centered in the dlg)
  CRect rectButton;
  GetDlgItem( IDC_PREVIOUS_BUTTON )->GetWindowRect(&rectButton);
  pointDraw.y += (m_sizeTextLabel.cy * 2);

  // prepare to paint the rectangles
  CBrush brush( HS_VERTICAL, m_pParent->m_aDataStyles.GetAt(0)->color );
  CBrush* pOldBrush = dc.SelectObject(&brush);   // select the new brush and hold the old one
  int OldMode = dc.SetMapMode(MM_TEXT);  // set the new mode and hold the old mode

  if(!IsRecetteDataSet())
  {
     dc.SetTextColor(RGB(0,0,0));	//Noir
  }
  else
  {
     dc.SetTextColor(COLOR_RECETTE);	
  }
  // set the right and left-hand margins
  int iLeftMargin = (pointDraw.x - (RECTANGLE_WIDTH + SPACING + iWidthCheckBox));
  if (iLeftMargin <= 0)
  iLeftMargin = 1;
  int iRightMargin = rectClient.Width() - iLeftMargin;

  // loop through (both) data set(s) to draw the values
  if ( m_hXrtData )  // do we have data from the chart's principal data set ?
  {
    int iLastSet = XrtDataGetLastSet( m_hXrtData );
    if ( ! bReverseLabels )
    {
      for (int iCurSelectSet = 0; iCurSelectSet <= iLastSet; iCurSelectSet++)
      {
        if ( DrawThisLabel( XRT_DATA, dc, iCurSelectSet, pointDraw, iLeftMargin, iRightMargin, bWithCheckBox) )
          pointDraw.Offset(0, (m_sizeTextLabel.cy * 2));  // skip down 2 lines
      }
    }
    else
    {
      for (int iCurSelectSet = iLastSet; iCurSelectSet >= 0; iCurSelectSet--)
      {
        if ( DrawThisLabel( XRT_DATA, dc, iCurSelectSet, pointDraw, iLeftMargin, iRightMargin, bWithCheckBox) )
          pointDraw.Offset(0, (m_sizeTextLabel.cy * 2));  // skip down 2 lines
      }
    }
  }

  if(!IsRecetteDataSet())
  {
	if ( m_hXrtData2 )  // do we have data from the chart's secondary data set ?
	{
	  int iLastSet = XrtDataGetLastSet( m_hXrtData2 );
	  for (int iCurSelectSet = 0; iCurSelectSet <= iLastSet; iCurSelectSet++)
		{
	    if ( DrawThisLabel( XRT_DATA2, dc, iCurSelectSet, pointDraw, iLeftMargin, iRightMargin, bWithCheckBox) )
			pointDraw.Offset(0, (m_sizeTextLabel.cy * 2));  // skip down 2 lines
		}
	}
  }

  dc.SetMapMode(OldMode);         // restore the old mode
  dc.SelectObject(pOldBrush);     // restore the old brush
}

void CCTAChartHelpDlg::PurgeCheckBoxes()
{
  int i;
  for (i = 0; i < m_ListCheckIdc.GetSize(); i++)
  {
	CDynaButton *pButton = (CDynaButton*)GetDlgItem (m_ListCheckIdc.GetAt(i));
	if (pButton)
	  delete pButton;
  }
  m_ListCheckIdc.RemoveAll();
}

BEGIN_MESSAGE_MAP(CCTAChartHelpDlg, CDialog)
	//{{AFX_MSG_MAP(CCTAChartHelpDlg)
	ON_BN_CLICKED(IDC_CLOSE_BUTTON, OnCloseButton)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, OnNextButton)
	ON_BN_CLICKED(IDC_PREVIOUS_BUTTON, OnPreviousButton)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCTAChartHelpDlg message handlers

void CCTAChartHelpDlg::OnCloseButton() 
{
  m_pParent->DestroyHelpDlg();
}

void CCTAChartHelpDlg::OnNextButton() 
{
  XrtDataHandle hXrtData;
  XrtGetValues( m_pParent->m_hChart, XRT_DATA, &hXrtData, NULL);
  if ( (m_pParent->GetCurSelectPt()+1) >=
         XrtDataGetNPoints(hXrtData, m_pParent->GetCurSelectSet()) )
  {
    MessageBeep(MB_ICONEXCLAMATION);
    return;
  }

  PurgeCheckBoxes();
  m_pParent->SetCurSelectPt(m_pParent->GetCurSelectPt()+1);
  Invalidate(TRUE);
	UpdateWindow();
}

void CCTAChartHelpDlg::OnPreviousButton() 
{
  if ( m_pParent->GetCurSelectPt() == 0 )
  {
    MessageBeep(MB_ICONEXCLAMATION);
    return;
  }

  PurgeCheckBoxes();
  m_pParent->SetCurSelectPt(m_pParent->GetCurSelectPt()-1);
  Invalidate(TRUE);
	UpdateWindow();
}

void CCTAChartHelpDlg::OnDestroy() 
{
  // when the dialog is destroyed, save its current dimensions
  CRect rectDlg;
  GetWindowRect(&rectDlg);  // get the dialog position
  CPoint point(rectDlg.left, rectDlg.top);
  ((CMainFrame*) AfxGetApp()->GetMainWnd())->SetHelpDlgPoint(point);

	CDialog::OnDestroy();
}

void CCTAChartHelpDlg::OnClose()
{
  m_pParent->DestroyHelpDlg();
}


HBRUSH CCTAChartHelpDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
  if (nCtlColor == CTLCOLOR_DLG)
  {
    pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
    return HBRUSH(*m_pBkBrush);
  }

	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

void CCTAChartHelpDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
#if 0
  CPoint point(x,y);
  ((CMainFrame*) AfxGetApp()->GetMainWnd())->SetHelpDlgPoint(point);
#endif
}

