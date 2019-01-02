// CTAChart.cpp : implementation file
//

#include "StdAfx.h"




#include "CTAChart.h"
#include "CTAChartHelpDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// constants

CCTACHART_DATA_STYLE_ARRAY CCTAChart::m_aDataStyles;

/////////////////////////////////////////////////////////////////////////////
// CCTAChart

CCTAChart::CCTAChart()
{
  m_pParentView = NULL;
  m_pCurChartData = NULL;
  m_pHelpDlg    = NULL;
  m_iCurFlashSet  = 0;
  m_iCurFlashPt   = 0;
  m_iCurSelectSet = 0;
  m_iCurSelectPt  = 0;
  m_FlashLabel = NULL;
  m_bAccumulateSets_DataSet1 = FALSE;
  m_bAccumulateSets_DataSet2 = FALSE;

  if (m_aDataStyles.GetSize())
    return; // déjà fait

  // set colors in Data Style Array
  XrtDataStyle* pDataStyle = NULL;
  for(int idx = eFirstStyleIdx; idx < eLastStyleIdx; idx++)  // for all styles (all values to be graphed)
  {
    XrtDataStyle* pXrtDataStyle = (XrtDataStyle*)malloc(sizeof XrtDataStyle);
    memset(pXrtDataStyle, '\0', sizeof(XrtDataStyle));

    // default styles
    pXrtDataStyle->width = 1;  // line width
    pXrtDataStyle->lpat = XRT_LPAT_SOLID;  // line pattern
    pXrtDataStyle->color =   // line color
    ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetGraphRGBValue((CtaDataStyleIdx)idx);
      pXrtDataStyle->pcolor =   // point color
    ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetGraphRGBValue((CtaDataStyleIdx)idx);
    pXrtDataStyle->point = XRT_POINT_DOT;  // point style
    pXrtDataStyle->psize = CTA_GPH_DEFAULT_POINT_SIZE;  // point size

  	// specific differences
    switch ( (CtaDataStyleIdx)idx )  // index of the particular style for value to be graphed
    {
	  case ePrevisionStyleIdx:
	    pXrtDataStyle->point = XRT_POINT_INVERT_TRI;
		break;
	  case eHoiTraStyleIdx:
	  case eHoiTraIdvStyleIdx:
	  case eHisto3HoiTraStyleIdx :
	  case eHisto2HoiTraStyleIdx :
	  case eHisto1HoiTraStyleIdx :
        pXrtDataStyle->psize = CTA_GPH_XXL_POINT_SIZE+1;  // point size
	    pXrtDataStyle->point = XRT_POINT_DIAG_CROSS;
		break;
      case eRevAvgFcstHighStyleIdx:
      case eResHoldStdvHighStyleIdx:
        pXrtDataStyle->fpat = XRT_FPAT_25_PERCENT;
        break;
      case eRevAvgFcstLowStyleIdx:
      case eResHoldStdvLowStyleIdx:
        pXrtDataStyle->fpat = XRT_FPAT_VERT_STRIPE;
        break;
      case eResHoldAvgStyleIdx:
        if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTRNFlag() )  // a bit thicker than the others
          pXrtDataStyle->width = 2;  // line width
        break;
        // customize the graph attributes for the "Comptages" points (TRN client)
      case eCmptComptageOfferStdvHighStyleIdx:
        pXrtDataStyle->point = XRT_POINT_INVERT_TRI;  // point style
        pXrtDataStyle->psize = CTA_GPH_DEFAULT_POINT_SIZE;  // point size
        break;
      case eCmptComptageOfferStyleIdx:
        pXrtDataStyle->point = XRT_POINT_DIAMOND;  // point style
        pXrtDataStyle->psize = CTA_GPH_XLARGE_POINT_SIZE;  // point size
        break;
      case eCmptComptageOfferStdvLowStyleIdx:
        pXrtDataStyle->point = XRT_POINT_TRI;  // point style
        pXrtDataStyle->psize = CTA_GPH_DEFAULT_POINT_SIZE;  // point size
        break;
      case eCmptComptageOccupStdvHighStyleIdx:
        pXrtDataStyle->point = XRT_POINT_INVERT_TRI;  // point style
        pXrtDataStyle->psize = CTA_GPH_DEFAULT_POINT_SIZE;  // point size
        break;
      case eCmptComptageOccupStyleIdx:
        pXrtDataStyle->point = XRT_POINT_DIAMOND;  // point style
        pXrtDataStyle->psize = CTA_GPH_XLARGE_POINT_SIZE;  // point size
        break;
      case eCmptComptageOccupStdvLowStyleIdx:
        pXrtDataStyle->point = XRT_POINT_TRI;  // point style
        pXrtDataStyle->psize = CTA_GPH_DEFAULT_POINT_SIZE;  // point size
        break;
	  case eHisto3OvbLevelStyleIdx:
		pXrtDataStyle->point = XRT_POINT_TRI;  // point style
        pXrtDataStyle->psize = CTA_GPH_XLARGE_POINT_SIZE;  // point size	
		pXrtDataStyle->lpat = XRT_LPAT_SHORT_DASH;
		break;
      case eHisto3ResHoldTotStyleIdx:
		pXrtDataStyle->point = XRT_POINT_BOX;  // point style
        pXrtDataStyle->psize = CTA_GPH_LARGE_POINT_SIZE;  // point size
		break;
      case eHisto2OvbLevelStyleIdx:
		pXrtDataStyle->point = XRT_POINT_TRI;  // point style
        pXrtDataStyle->psize = CTA_GPH_XLARGE_POINT_SIZE;  // point size	
		pXrtDataStyle->lpat = XRT_LPAT_SHORT_DASH;
		break;
      case eHisto2ResHoldTotStyleIdx:
		pXrtDataStyle->point = XRT_POINT_BOX;  // point style
        pXrtDataStyle->psize = CTA_GPH_LARGE_POINT_SIZE;  // point size
		break;
      case eHisto1OvbLevelStyleIdx:
		pXrtDataStyle->point = XRT_POINT_TRI;  // point style
        pXrtDataStyle->psize = CTA_GPH_XLARGE_POINT_SIZE;  // point size	
		pXrtDataStyle->lpat = XRT_LPAT_SHORT_DASH;
		break;
      case eHisto1ResHoldTotStyleIdx:
		pXrtDataStyle->point = XRT_POINT_BOX;  // point style
        pXrtDataStyle->psize = CTA_GPH_LARGE_POINT_SIZE;  // point size
		break;
	  case eCmptComptageHist3OfferStyleIdx:
      case eCmptComptageHist3OccupStyleIdx:
		pXrtDataStyle->point = XRT_POINT_BOX;  // point style
        pXrtDataStyle->psize = CTA_GPH_XLARGE_POINT_SIZE;  // point size
		break;
      case eCmptComptageHist2OfferStyleIdx:
      case eCmptComptageHist2OccupStyleIdx:
		pXrtDataStyle->point = XRT_POINT_BOX;  // point style
        pXrtDataStyle->psize = CTA_GPH_XLARGE_POINT_SIZE;  // point size
		break;
      case eCmptComptageHist1OfferStyleIdx:
      case eCmptComptageHist1OccupStyleIdx:
		pXrtDataStyle->point = XRT_POINT_BOX;  // point style
        pXrtDataStyle->psize = CTA_GPH_XLARGE_POINT_SIZE;  // point size
		break;
      default:
        break;
    }
	
    m_aDataStyles.Add(pXrtDataStyle);  // add this style to the array
  }
}

CCTAChart::~CCTAChart()
{
  // clean up the array containing the tabs and graph data objects
  for (int iIdx=0; iIdx < m_apChartData.GetSize(); iIdx++)
  {
    delete m_apChartData[iIdx];
    m_apChartData[iIdx] = NULL;
  }
  m_apChartData.RemoveAll();

  DestroyHelpDlg();
}

void CCTAChart::SetDataStyleColor(CtaDataStyleIdx eDataStyleIdx, int iRGBValue)
{
  XrtDataStyle* pXrtDataStyle = m_aDataStyles.GetAt(eDataStyleIdx);
  pXrtDataStyle->color = iRGBValue;
  pXrtDataStyle->pcolor = iRGBValue;
/*
  aCtaDataStyles[eDataStyleIdx].color = iRGBValue;
  aCtaDataStyles[eDataStyleIdx].pcolor = iRGBValue;
*/
}

// delete and remove all elements from the chart data array
void CCTAChart::ClearCurChartData()
{
  if ( m_pCurChartData == NULL )
    return;

  // clean up the array containing X-axis labels
  m_pCurChartData->GetXLabels()->RemoveAll();

  // now clean up the data element
  delete m_pCurChartData;      // delete and
  m_pCurChartData = NULL;
}

// delete and remove all elements from the chart data array
void CCTAChart::ClearChartDataArray()
{
  int iSize = m_apChartData.GetSize();
  for (int iIdx = 0; iIdx < iSize; iIdx++)
  {
    // clean up the array containing X-axis labels
    m_apChartData[0]->GetXLabels()->RemoveAll();

    // now clean up the data element
    delete m_apChartData[0];      // delete and
    m_apChartData[0] = NULL;
    m_apChartData.RemoveAt(0);    // remove the item from the array
  }
  m_pCurChartData = NULL;
}

void CCTAChart::ClearChartDataArray(enum CCTAChartData::GraphType eGraphType)
{
  int iSize = m_apChartData.GetSize();
  for (int iIdx = iSize-1; iIdx >= 0; iIdx--)
  {
    if ( m_apChartData[iIdx]->GetGraphType() == eGraphType )
    {
      // clean up the array containing X-axis labels
      m_apChartData[iIdx]->GetXLabels()->RemoveAll();

      // now clean up the data element
      delete m_apChartData[iIdx];      // delete and
      m_apChartData[iIdx] = NULL;
      m_apChartData.RemoveAt(iIdx);    // remove the item from the array
    }
  }
  m_pCurChartData = NULL;
}


BEGIN_MESSAGE_MAP(CCTAChart, CWnd)
	//{{AFX_MSG_MAP(CCTAChart)
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCTAChart::DestroyHelpDlg()
{
  if ( m_pHelpDlg && m_pHelpDlg->GetSafeHwnd( ) )
  {
    m_pHelpDlg->DestroyWindow();
    delete m_pHelpDlg;
    m_pHelpDlg = NULL;
  }
}

void CCTAChart::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
  // on double click we display the help dialog and destroy the old one
  //
  DestroyHelpDlg();
	
  // If we're now in the data ... 
  int iDistance = 0;
  if ( GetType() == XRT_TYPE_PLOT )
    iDistance = FRONTBACKDIST;

  XrtPickResult pick;
  if ( Pick ( XRT_DATASET1, point.x, point.y, 
              &pick, XRT_XFOCUS|XRT_YFOCUS ) == XRT_RGN_IN_GRAPH && 
       pick.distance <= iDistance)
  {
    // set the current set and point as member variables
    SetCurSelectSet(pick.set);
    SetCurSelectPt(pick.point);

    // set the help dlg window as member variable here and in CMainFrame
    CreateHelpDlg();
    DisplayHelpDlg( point );
  }

	CChart2D::OnLButtonDblClk(nFlags, point);
}

//////////////////////////////////////////
// Determines if the cursor is in a "hot" area
// Invoked when the mouse is moved
void CCTAChart::OnMouseInDataArea( CPoint point ) 
{
  if ( ! ((CCTAApp*)AfxGetApp())->GetFlashLabelFlag() )
  {
    RemoveFlashLabel();  // remove a flash label if it exists
    return;
  }

#if 0
  // on a mouse move, flash the value when the cursor passes over a bar
  // If we're not active, ensure everything's off and do nothing.
  CRect rect;
  GetClientRect(&rect);
  CWnd* pActiveWnd = GetForegroundWindow();

  // if we are no longer the active window, and we've captured the mouse, let it go !
  if ( pActiveWnd != ((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd()) )
  {
    CWnd* pCaptureWnd = GetCapture();
    if ( pCaptureWnd == this )
    {
      ReleaseCapture();
      RemoveFlashLabel();  // If there's a flash label up, turn it off
    }
    return;
  }

  // We're active and in the window
  // Set capture on so we know when mouse cursor leaves window
  SetCapture();
#endif

  // if we are no longer the active window, remove the flash label if exists
  if ( GetForegroundWindow() != AfxGetApp()->GetMainWnd() )
  {
    RemoveFlashLabel();  // If there's a flash label up, turn it off
  }
  else
  {
    // If we're now in the data ... 

    // let's give priority in the flashing of labels to the dataset which is in front...
    int iFrontDataSet = GetFrontDataset();
    int iBackDataSet = (iFrontDataSet == XRT_DATASET1) ? XRT_DATASET2 : XRT_DATASET1;

    // set the distance for the Front and Back DataSets
    int iFrontDistance = 0;
    int iBackDistance = 0;
    if (iFrontDataSet == XRT_DATASET1)
    {
      if ( GetType() == XRT_TYPE_PLOT )  // GetType() is for XRT_DATASET1
        iFrontDistance = FRONTBACKDIST;
      if ( GetType2() == XRT_TYPE_PLOT )  // GetType2() is for XRT_DATASET2
        iBackDistance = FRONTBACKDIST;
    }
    else
    {
      if ( GetType() == XRT_TYPE_PLOT )  // GetType() is for XRT_DATASET1
        iBackDistance = FRONTBACKDIST;
      if ( GetType2() == XRT_TYPE_PLOT )  // GetType2() is for XRT_DATASET2
        iFrontDistance = FRONTBACKDIST;
    }

    XrtPickResult pick;
    // check if the mouse has been moved over the "front" dataset
    if ( Pick ( iFrontDataSet, point.x, point.y, &pick, 
                XRT_XFOCUS|XRT_YFOCUS ) == XRT_RGN_IN_GRAPH && 
         pick.distance <= iFrontDistance)
      DisplayFlashLabel( pick.set, pick.point, iFrontDataSet );
    else
      // check if the mouse has been moved over the "back" dataset
      if ( Pick ( iBackDataSet, point.x, point.y, &pick, 
                  XRT_XFOCUS|XRT_YFOCUS ) == XRT_RGN_IN_GRAPH && 
           pick.distance <= iBackDistance)
        DisplayFlashLabel( pick.set, pick.point, iBackDataSet );
      else
        RemoveFlashLabel();  // If there's a flash label up, turn it off
  }
}

void CCTAChart::OnMouseMove(UINT nFlags, CPoint point) 
{
  OnMouseInDataArea( point );
 	CChart2D::OnMouseMove(nFlags, point);
}

void CCTAChart::OnRButtonDown(UINT nFlags, CPoint point) 
{
  RemoveFlashLabel();  // If there's a flash label up, turn it off
  DestroyHelpDlg();

  // If we're now in the data ... 
  int iDistance = 0;
  if ( GetType() == XRT_TYPE_PLOT )
    iDistance = FRONTBACKDIST;

  // determine if the the right single-click was within the determined distance of a data point
  XrtPickResult pick;
  BOOL bInRange = 
    ( Pick ( XRT_DATASET1, point.x, point.y, &pick, XRT_XFOCUS|XRT_YFOCUS ) == XRT_RGN_IN_GRAPH && 
       pick.distance <= iDistance);
  if (bInRange)  // if the click was within range, set the member variables
  {
    // set the current set and point as member variables
    SetCurSelectSet(pick.set);
    SetCurSelectPt(pick.point);
  }
  // Invoke the virtual Contextual Display method
  ContextualDisplay( point, bInRange );  // virtual method

	CWnd::OnRButtonDown(nFlags, point);
}

void CCTAChart::OnLButtonDown(UINT nFlags, CPoint point) 
{
  // on double click we display the help dialog and destroy the old one
  //
  DestroyHelpDlg();
	
  // If we're now in the data ... 
  int iFrontDataSet = GetFrontDataset();
  int iBackDataSet = (iFrontDataSet == XRT_DATASET1) ? XRT_DATASET2 : XRT_DATASET1;

  // set the distance for the Front and Back DataSets
  int iFrontDistance = 0;
  int iBackDistance = 0;
  if (iFrontDataSet == XRT_DATASET1)
  {
    if ( GetType() == XRT_TYPE_PLOT )  // GetType() is for XRT_DATASET1
      iFrontDistance = FRONTBACKDIST;
    if ( GetType2() == XRT_TYPE_PLOT )  // GetType2() is for XRT_DATASET2
      iBackDistance = FRONTBACKDIST;
  }
  else
  {
    if ( GetType() == XRT_TYPE_PLOT )  // GetType() is for XRT_DATASET1
      iBackDistance = FRONTBACKDIST;
    if ( GetType2() == XRT_TYPE_PLOT )  // GetType2() is for XRT_DATASET2
      iFrontDistance = FRONTBACKDIST;
  }

  XrtPickResult pick;
  if ( Pick ( iFrontDataSet, point.x, point.y, 
              &pick, XRT_XFOCUS|XRT_YFOCUS ) == XRT_RGN_IN_GRAPH && 
       pick.distance <= iFrontDistance)
  {
    // set the current set and point as member variables
    SetCurSelectSet(pick.set);
    SetCurSelectPt(pick.point);

    // set the help dlg window as member variable here and in CMainFrame
    CreateHelpDlg();
    DisplayHelpDlg( point );
  }
  else
  if ( Pick ( iBackDataSet, point.x, point.y, 
              &pick, XRT_XFOCUS|XRT_YFOCUS ) == XRT_RGN_IN_GRAPH && 
       pick.distance <= iBackDistance)
  {
    // set the current set and point as member variables
    SetCurSelectSet(pick.set);
    SetCurSelectPt(pick.point);

    // set the help dlg window as member variable here and in CMainFrame
    CreateHelpDlg();
    DisplayHelpDlg( point );
  }

	CWnd::OnLButtonDown(nFlags, point);
}

//////////////////////////////////////////
// Get the CTA system-wide selected font  as an HFONT
//
HFONT CCTAChart::GetCtaHFont()
{
  CString szFontName = ((CCTAApp*)APP)->GetDocument()->GetFontName();
  long lFontHeight = ((CCTAApp*)APP)->GetDocument()->GetFontHeight();
  long lFontWeight = ((CCTAApp*)APP)->GetDocument()->GetFontWeight();
  BOOL bFontItalic = ((CCTAApp*)APP)->GetDocument()->GetFontItalic();
  return CreateFont( lFontHeight, 0, 0, 0, lFontWeight, bFontItalic, 0, 0, 0, 0, 0, 0, 0, (LPCSTR)szFontName );
}

//////////////////////////////////////////
// Set the graph properties 
//
void CCTAChart::SetGraphFont()
{
  if ( ((CCTAApp*)APP)->GetDocument()->IsDefaultFont() )
  {
    SetPropString(XRT_HEADER_FONT, (LPSTR) CTA_GPH_HEADER_FONT);
    SetPropString(XRT_FOOTER_FONT, (LPSTR) CTA_GPH_FOOTER_FONT);
    SetPropString(XRT_AXIS_FONT,   (LPSTR) CTA_GPH_AXIS_FONT);
    SetPropString(XRT_LEGEND_FONT, (LPSTR) CTA_GPH_LEGEND_FONT);
  }
  else
  {
    HFONT hFont = GetCtaHFont();

    SetHeaderFont( hFont );
    SetFooterFont( hFont );
    SetAxisFont( hFont );
    SetLegendFont( hFont );

    DeleteObject( hFont );
  }
}

////////////////////////////
// Get the value to display in the flash label
//
double CCTAChart::GetFlashLabelValue(XrtDataHandle& hXrtData, int iCurFlashSet, int iCurFlashPt, BOOL bAccumulate)
{
  if ( bAccumulate )  // do we accumulate values all in this set to be displayed ?
  {
    double dFlashValue = 0;
    // accumulate all sets for this point...
    int iLastSet = XrtDataGetLastSet( hXrtData );
    for( int i = 0; i <= iLastSet; i++ )
      dFlashValue += XrtDataGetYElement( hXrtData, i, iCurFlashPt );
    return dFlashValue;
  }
  else
  {
    return XrtDataGetYElement( hXrtData, iCurFlashSet, iCurFlashPt );
  }
}

////////////////////////////
// Display the flash label
//
void CCTAChart::DisplayFlashLabel(int iCurFlashSet, int iCurFlashPt, int iDataSet)
{
  BOOL bAccumulate = FALSE;
  if ( ! ((CCTAApp*)AfxGetApp())->GetFlashLabelFlag() )
  {
    RemoveFlashLabel();  // remove a flash label if it exists
    return;
  }

  // If we have a flash label up, but it's not for this point turn it off
  if ( m_FlashLabel && (iCurFlashPt != m_iCurFlashPt || iCurFlashSet != m_iCurFlashSet) )
    RemoveFlashLabel();

  // If we have no flash label up for this point, put one up
  if ( !m_FlashLabel )
  {
    // Get the appropriate chart data set
    XrtDataHandle hXrtData;
    switch ( iDataSet )
    {
    case XRT_DATASET1:
      XrtGetValues( m_hChart, XRT_DATA, &hXrtData, NULL);
      bAccumulate = m_bAccumulateSets_DataSet1;
      break;
    case XRT_DATASET2:
      XrtGetValues( m_hChart, XRT_DATA2, &hXrtData, NULL);
      bAccumulate = m_bAccumulateSets_DataSet2;
      break;
    default:
      return;  // invalid dataset
    }

    // Attach by pixel so not clipped on graph area
    // Find the pixel coords of the data point
    XrtPickResult pick;
    XrtUnpick( m_hChart, iDataSet, iCurFlashSet, iCurFlashPt, &pick );
    if ( pick.pix_x == -1 || pick.pix_y == -1 )
      return;
    
    // Return if we have no data or we are out of the data range
    if ( !hXrtData || 
         iCurFlashSet > XrtDataGetLastSet( hXrtData ) ||
         iCurFlashPt > XrtDataGetLastPoint( hXrtData, iCurFlashSet ) ) 
      return;

    // Get the value and construct the label
    char szLabelText[15];
    char* strs[] = {szLabelText,NULL}; // only one line for a flash label
    double dFlashValue = GetFlashLabelValue( hXrtData, iCurFlashSet, iCurFlashPt, bAccumulate );
    if ( dFlashValue == INVALID_GRAPH_VALUE )
      return;
    sprintf( szLabelText,"%.0f", dFlashValue );

    /* Create (and show) the text area. */
    if ( !(m_FlashLabel = XrtTextAreaCreate(m_hChart)) )
        return;

    // Get the style of the data set which is currently being "flashed"
    XrtDataStyle* DataStyle = 
      ( iDataSet == XRT_DATASET1 ) ? GetNthDataStyle(iCurFlashSet) : GetNthDataStyle2(iCurFlashSet);

    // Set the foreground text color to white if the background color is:
    //    BLUE or BLACK
    XrtColor cColor = DataStyle->color;
/*
    BYTE bRcolor = GetRValue(cColor);
    BYTE bGcolor = GetGValue(cColor);
    BYTE bBcolor = GetBValue(cColor);
*/
    XrtColor cForeground = 
    (
      ( GetRValue(cColor) == 0xFF && GetGValue(cColor) == 0x00 && GetBValue(cColor) == 0x00 ) ||
      ( GetRValue(cColor) == 0x00 && GetGValue(cColor) == 0x00 && GetBValue(cColor) == 0xFF ) ||
      ( GetRValue(cColor) == 0x00 && GetGValue(cColor) == 0x00 && GetBValue(cColor) == 0x00 ) ||
      ( GetRValue(cColor) == 0x80 || GetGValue(cColor) == 0x80 || GetBValue(cColor) == 0x80 ) ||
      ( GetRValue(cColor) == 0x00 && GetGValue(cColor) == 0x80 && GetBValue(cColor) == 0x80 )
    )  ?  XrtColorWhite : XrtColorBlack ;

    XrtTextAreaSetValues(m_FlashLabel,
                         XRT_TEXT_ANCHOR, XRT_ANCHOR_NORTHWEST,
                         XRT_TEXT_ATTACH_PIXEL_X, pick.pix_x,
                         XRT_TEXT_ATTACH_PIXEL_Y, pick.pix_y,
                         XRT_TEXT_OFFSET, 0,
                         XRT_TEXT_IS_CONNECTED, FALSE,
                         XRT_TEXT_ATTACH_TYPE, XRT_TEXT_ATTACH_PIXEL,
                         XRT_TEXT_ADJUST, XRT_ADJUST_CENTER,
                         XRT_TEXT_BACKGROUND_COLOR, DataStyle->color,
                         XRT_TEXT_FOREGROUND_COLOR, cForeground,
                         XRT_TEXT_BORDER, XRT_BORDER_SHADOW, 
                         XRT_TEXT_BORDER_WIDTH, 4,
                         XRT_TEXT_STRINGS, strs,
                         XRT_TEXT_IS_SHOWING, TRUE,
                         NULL);

    /* Save new state. */
    m_iCurFlashSet = iCurFlashSet;
    m_iCurFlashPt  = iCurFlashPt;
  }
}

////////////////////////////
// Remove the flash label
//
void CCTAChart::RemoveFlashLabel()
{
  if (m_FlashLabel)
  {
    XrtTextAreaDestroy(m_FlashLabel);
    m_FlashLabel = NULL;
  }
}

void CCTAChart::DisplayHelpDlg(CPoint point )
{
#if 0
  XrtMapResult map;
  XrtRegion rc = Map(1, point.x, point.y, &map);
  double x1 = map.x;
  double y1 = map.y;
#endif

  ASSERT(m_pHelpDlg != NULL);

  // try to position it where the user last left it
  CPoint* pPrevPoint = 
    ((CMainFrame*) AfxGetApp()->GetMainWnd())->GetHelpDlgPoint();

  m_pHelpDlg->SetWindowPos( NULL, pPrevPoint->x, pPrevPoint->y, 0, 0, 
                            SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW );
}

///////////////////////////////
// Scale the graph to the specified value
void CCTAChart::ScaleGraph(enum GraphScaleIndex eGraphScale )
{
  SetRepaint( FALSE );

  SetCurScaling(eGraphScale);

  CCTAChartData* pCurData = GetCurChartData();
  if ( pCurData == NULL )
    return;

  double dVal1, dVal2;

  switch ( eGraphScale )
  {
  case CCTAChart::eAuthScaleIdx:
    SetYAxisMax(__max(pCurData->GetAuthMax(),1));
    SetY2AxisMax(__max(pCurData->GetAuthMax(),1));
    SetY2AxisMin(GetYAxisMin());
    break;
  case CCTAChart::eCapScaleIdx:
    SetYAxisMax(__max(pCurData->GetCapMax(),1));
    SetY2AxisMax(__max(pCurData->GetCapMax(),1));
    SetY2AxisMin(GetYAxisMin());
    break;
  case CCTAChart::eResHoldScaleIdx:
    SetYAxisMax(__max(pCurData->GetResHoldMax(),1));
    SetY2AxisMax(__max(pCurData->GetResHoldMax(),1));
    SetY2AxisMin(GetYAxisMin());
    break;
  case CCTAChart::eDmdDptFcstScaleIdx:
    SetYAxisMax(__max(pCurData->GetDmdDptFcstMax(),1));
    SetY2AxisMax(__max(pCurData->GetDmdDptFcstMax(),1));
    SetY2AxisMin(GetYAxisMin());
    break;
  case CCTAChart::eTrafficScaleIdx:
    SetYAxisMax(__max(pCurData->GetTrafficMax(),1));
    SetY2AxisMax(__max(pCurData->GetTrafficMax(),1));
    SetY2AxisMin(GetYAxisMin());
    break;
  case CCTAChart::eMaxScaleIdx:
  default:
    SetYAxisMaxUseDefault(TRUE);
    dVal1 = GetYAxisMax();
    if ( GetCurChartData()->GetDataObj2() )  // do we have a second data set ?
    {
      SetY2AxisMaxUseDefault(TRUE);
      dVal2 = GetY2AxisMax();

      if ( dVal1 > dVal2)
      {
        SetYAxisMaxUseDefault(TRUE);
        SetY2AxisMaxUseDefault(FALSE);
        SetYAxisMax(__max(GetYAxisMax(),1));
        SetY2AxisMax(__max(GetYAxisMax(),1));
        SetY2AxisMin(GetYAxisMin());
      }
      else
      {
        SetY2AxisMaxUseDefault(TRUE);
        SetYAxisMaxUseDefault(FALSE);
        SetY2AxisMax(__max(GetY2AxisMax(),1));
        SetYAxisMax(__max(GetY2AxisMax(),1));
        SetYAxisMin(GetY2AxisMin());
      }
    }
    break;
  }

  SetY2AxisShow(FALSE);  // hide the 2nd graph axis

    SetRepaint( TRUE );

	/*double nValue = GetXAxisMax();
	nValue = GetXAxisMin();
	nValue = GetXOrigin();
	nValue = GetXOriginBase();
	XrtOriginPlacement obTest = GetXOriginPlacement();

	SetXAxisMax(32);
	SetXAxisMin(1);*/
	//SetXOrigin(1);
} 

void CCTAChart::OnSize(UINT nType, int cx, int cy) 
{
    CChart2D::OnSize(nType, cx, cy);
	if (m_hChart)
	  ScaleGraph(m_eCurScale);
}

////////////////////////
// CCTAChartData
//

// constructor
CCTAChartData::CCTAChartData(CCTAChart* pParentChart,
                             enum GraphType eGraphType, 
                             CChart2DData* pGraphDataObj /* = NULL */, 
                             CChart2DData* pGraphDataObj2 /* = NULL */, 
                             CString szCmpt /* = "" */,
                             int  iCmptSeq /* = -1 */,
                             CString szBktLabel /* = "" */,
                             CString szBktId /* = "" */,
                             int  iNestLevel /* = -1 */)
{
  m_pParentChart = pParentChart;
  m_eGraphType = eGraphType;
  m_pDataObj   = pGraphDataObj;
  m_pDataObj2  = pGraphDataObj2;
  m_szCmpt  = szCmpt;
  m_iCmptSeq = iCmptSeq;
  m_szBktLabel = szBktLabel;
  m_szBktId = szBktId;
  m_iNestLevel = iNestLevel;
  m_iNestLevelCount = 0;

  m_lNumPts = 0;  // number of data points (ticks along X axis)

  // graphed values which can be scaled to the max graph value
  m_dMax = 0;  // maximum value on the graph
  m_dAuthMax = 0;  // authorizations
  m_dCapMax = 0;  // capacity
  m_dResHoldMax = 0;  // total reservations holding
  m_dDmdDptFcstMax = 0;  // demand forecast at departure
  m_dTrafficMax = 0;  // traffic
  this->m_scxEtanche = 0;
}

CCTAChartData::~CCTAChartData()
{
	delete m_pDataObj;
  m_pDataObj = NULL;
	delete m_pDataObj2;
  m_pDataObj2 = NULL;
  m_aszXLabels.RemoveAll();
}

void CCTAChartData::SetGraphMax(double dValue) 
{ 
  m_dMax = __max( m_dMax, dValue );
}

void CCTAChartData::SetAuthMax(double dValue) 
{ 
  m_dAuthMax = __max( m_dAuthMax, dValue );
  m_dMax = __max( m_dMax, dValue );
}

void CCTAChartData::SetResHoldMax(double dValue) 
{ 
  m_dResHoldMax = __max( m_dResHoldMax, dValue );
  m_dMax = __max( m_dMax, dValue );
}

void CCTAChartData::SetCapMax(double dValue) 
{ 
  m_dCapMax = __max( m_dCapMax, dValue );
  m_dMax = __max( m_dMax, dValue );
}

void CCTAChartData::SetDmdDptFcstMax(double dValue) 
{ 
  m_dDmdDptFcstMax = __max( m_dDmdDptFcstMax, dValue );
  m_dMax = __max( m_dMax, dValue );
}

void CCTAChartData::SetTrafficMax(double dValue) 
{ 
  m_dTrafficMax = __max( m_dTrafficMax, dValue );
  m_dMax = __max( m_dMax, dValue );
}

////////////////////////////////////////////////////////////////////////////////////////
// Add an element (perhaps in the correct order)
//
void CCTAChart::AddChartData( CCTAChartData *pNewChartData /* = NULL */, BOOL bSort /* = TRUE */ )
{
  // if there is a cmpt and/or nest level, we must sort the data objects in the order
  // that they should appear on the graph.  so, find the place for this element in the array;
  // order by compartment and bucket.  (e.g., "A,A0,A1...,B,B0,B1...");

  if ( pNewChartData == NULL )
    return;

  if ( bSort )  // do we sort the array in order of cmpt / nest level ?
  {
    CCTAChartData *pExistChartData;

    int iSortKeyNew;
    int iSortKeyOld;
    int iIndex;
    for (iIndex=0; iIndex < m_apChartData.GetSize(); iIndex++)
    {
      pExistChartData = m_apChartData.GetAt(iIndex);

      // do we use cmpt or cmpt sequence for the sorting here?
      //  This is only useful for EYMS/TYMS version being used with SNCF database before SNCF database is converted !!!
      //  ALSO, THIS WILL ONLY WORK IF THE CONSTRUCTOR FOR THE CLASS YmIcCmptEvolDom
      //    HAS A NEGATIVE ONE (-1) AS THE DEFAULT VALUE FOR THE FIELD _cmptSeq !!!
      iSortKeyNew = 
        CHART_DATA_SORT_KEY(pNewChartData->GetCmptSeq(), pNewChartData->GetCmpt()[0], pNewChartData->GetNestLevel());
      iSortKeyOld = 
        CHART_DATA_SORT_KEY(pExistChartData->GetCmptSeq(), pExistChartData->GetCmpt()[0], pExistChartData->GetNestLevel());

      if ( iSortKeyNew <= iSortKeyOld )
        break;
    }
    m_apChartData.InsertAt( iIndex, pNewChartData );
  }
  else  // don't ask questions, just add the new member to the array
  {
    m_apChartData.Add( pNewChartData );
  }
}

///////////////////////////////////////////////////////////////////////
// set the currently selected data object
//
void CCTAChart::SetCurChartData(int iIdx)
{ 
  if ( iIdx >= m_apChartData.GetSize() )
    m_pCurChartData = NULL; 
  else
    m_pCurChartData = m_apChartData.GetAt(iIdx); 
}
