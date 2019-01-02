#if !defined(AFX_CHART_H__EABECC93_9E13_11D1_8381_400020302A31__INCLUDED_)
#define AFX_CHART_H__EABECC93_9E13_11D1_8381_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CCTAChart.h : header file
//

#define CAPACITY_BAR_THICKNESS  .04
#define FRONTBACKDIST	5

#include "och2dMFC.h"
#include "oc_color.h"

const DWORD CTA_TAB_STYLE = 
  TCS_BUTTONS | TCS_FOCUSONBUTTONDOWN | TCS_FIXEDWIDTH | WS_VISIBLE;


const LPCTSTR CTA_GPH_HEADER_FONT = "MS Sans Serif,9";
const LPCTSTR CTA_GPH_FOOTER_FONT = "MS Sans Serif,9";
const LPCTSTR CTA_GPH_AXIS_FONT = "MS Sans Serif,9";
const LPCTSTR CTA_GPH_AXIS_FONT_SMALL = "MS Sans Serif,7";
const LPCTSTR CTA_GPH_AXIS_FONT_MINISCULE = "MS Sans Serif,5";
const LPCTSTR CTA_GPH_LEGEND_FONT = "MS Sans Serif,9";

// define the chart data styles
const int CTA_GPH_XLARGE_POINT_SIZE = XRT_DEFAULT_POINT_SIZE-2;
const int CTA_GPH_LARGE_POINT_SIZE = XRT_DEFAULT_POINT_SIZE-3;
const int CTA_GPH_DEFAULT_POINT_SIZE = XRT_DEFAULT_POINT_SIZE-4;
const int CTA_GPH_SMALL_POINT_SIZE = XRT_DEFAULT_POINT_SIZE-5;
const int CTA_GPH_XXL_POINT_SIZE = XRT_DEFAULT_POINT_SIZE-1;

// define a constant which represents an invalid help dialog value
const double INVALID_GRAPH_VALUE = XRT_HUGE_VAL;

typedef CTypedPtrArray<CPtrArray, XrtDataStyle*> CCTACHART_DATA_STYLE_ARRAY;

/////////////////////////////////////////////////////////////////////////////
// CCTAChart object

class CCTAChart;

//
// CCTAChartData
//   Provides an indexing of (the tab control to) graph data objects.
//
class CCTAChartData : public CObject
{
public:
  // enumeration type for indexing of the graph types
  enum GraphType
  {
	  eCmpt = 0,  
    eBkt,
    eAvail,
    eResHold,
    eDmdDptFcst,
	eRecettes
  } ;

public:
  // Constructor
  CCTAChartData(CCTAChart* pParentChart,
                             enum GraphType eGraphType, 
                             CChart2DData* pGraphDataObj = NULL, 
                             CChart2DData* pGraphDataObj2 = NULL, 
                             CString szCmpt = "",
                             int  iCmptSeq = -1,
                             CString szBktLabel = "",
                             CString szBktId = "",
                             int  iNestLevel = -1);

	~CCTAChartData();

  void SetParentChart(CCTAChart* pChart) { m_pParentChart = pChart; }
  CCTAChart* GetParentChart() { return m_pParentChart; }

  enum GraphType  GetGraphType()  { return m_eGraphType; } ;
  void SetGraphType(enum GraphType eGraphType) { m_eGraphType = eGraphType; } ;

  CChart2DData* GetDataObj() { return m_pDataObj; } ;
  void SetDataObj(CChart2DData* pDataObj) { m_pDataObj = pDataObj; } ;

  CChart2DData* GetDataObj2() { return m_pDataObj2; } ;
  void SetDataObj2(CChart2DData* pDataObj2) { m_pDataObj2 = pDataObj2; } ;

  CString GetCmpt() { return m_szCmpt; } ;
  void SetCmpt(CString szCmpt) { m_szCmpt = szCmpt; } ;
  int GetCmptSeq() { return m_iCmptSeq; } ;
  void SetCmptSeq(int iCmptSeq) { m_iCmptSeq = iCmptSeq; } ;
  int GetNestLevel() { return m_iNestLevel; } ;
  void SetNestLevel(int iNestLevel) { m_iNestLevel = iNestLevel; } ;
  int GetNestLevelCount() { return m_iNestLevelCount; } ;
  void SetNestLevelCount(int iNestLevelCount) { m_iNestLevelCount = iNestLevelCount; } ;
  CString GetBktLabel() { return m_szBktLabel; } ;
  void SetBktLabel(CString szBktLabel) { m_szBktLabel = szBktLabel; } ;
  CString GetBktId() { return m_szBktId; } ;
  void SetBktId(CString szBktId) { m_szBktId = szBktId; } ;
  BOOL GetFlag() { return m_bFlag; }
  void SetFlag(BOOL bFlag) { m_bFlag = bFlag; }

  CStringArray* GetXLabels() { return &m_aszXLabels; } ;  // get the array of X-axis labels

  // methods to manipulate graph scaling
  void SetGraphMax(double dValue);
  double GetGraphMax() { return m_dMax; }
  void SetAuthMax(double dValue);
  double GetAuthMax() { return m_dAuthMax; }
  void SetResHoldMax(double dValue);
  double GetResHoldMax() { return m_dResHoldMax; }
  void SetCapMax(double dValue);
  double GetCapMax() { return m_dCapMax; }
  void SetDmdDptFcstMax(double dValue);
  double GetDmdDptFcstMax() { return m_dDmdDptFcstMax; }
  void SetTrafficMax(double dValue);
  double GetTrafficMax() { return m_dTrafficMax; }

  void setScxEtanche(char scxEtanche)	{ this->m_scxEtanche = scxEtanche; }
  char getScxEtanche()					{ return (this->m_scxEtanche); }

  BOOL AuthIsMax() { return m_dAuthMax == m_dMax; }
  BOOL CapIsMax() { return m_dCapMax == m_dMax; }
  BOOL ResHoldIsMax() { return m_dResHoldMax == m_dMax; }
  BOOL DmdDptFcstIsMax() { return m_dDmdDptFcstMax == m_dMax; }
  BOOL TrafficIsMax() { return m_dTrafficMax == m_dMax; }

  void SetNumPts(long lNumPts) { m_lNumPts = lNumPts; }
  long GetNumPts() { return m_lNumPts; }

  CStringArray m_aszXLabels;  // array of labels along the X-axis

private:
  CCTAChart* m_pParentChart;
  enum GraphType m_eGraphType;  // the type of graph (cmpt, bkt, etc.)
  CChart2DData* m_pDataObj;     // the graph data object
  CChart2DData* m_pDataObj2;    // the 2nd graph data object
  CString m_szCmpt;  // the compartment
  int m_iCmptSeq;
  int m_iNestLevel;  // the bucket
  int m_iNestLevelCount;  // the number of buckets for this compartment
  CString  m_szBktLabel;  // the bucket
  CString m_szBktId;  // bucket Id
  BOOL m_bFlag;  // miscellaneous flag used for general purpose
  char m_scxEtanche;//DT35037 - Lyria lot 3

  long m_lNumPts;  // number of data points (ticks along X axis)

  // graphed values which can be scaled to the max graph value
  double m_dMax;  // maximum value on the graph
  double m_dAuthMax;  // authorizations
  double m_dCapMax;  // capacity
  double m_dResHoldMax;  // total reservations holding
  double m_dDmdDptFcstMax;  // demand forecast at departure
  double m_dTrafficMax;  // traffic

};

// typedef for the array which contains chart data objects
typedef CTypedPtrArray<CPtrArray, CCTAChartData*> CCTAChartDataArray;

class CCTAChartHelpDlg;

class CCTAChart : public CChart2D
{
#define GRAPH_SCALE_FACTOR  "GRAPH_SCALE_FACTOR"
#define ROTATE_X_LABEL "ROTATE_X_LABEL"
#define LABEL_INTERVAL "LABEL_INTERVAL"

// this defines the key sequence for sorting the chart data
// the keys are (first primary - cmpt seq; sencond primary - cmpt; secondary - nest level)
// if cmpt seq does not exist (-1), then cmpt is used as primary.  if nest level exists, it is used as secondary.
#define CHART_DATA_SORT_KEY(CmptSeq, Cmpt, NestLevel)  \
  ( (10000 * (CmptSeq+1)) + ( (( (int)(Cmpt) ) - 64)*100 ) + NestLevel )

// Attributes
public:

  // Enumeration type for indexing of the graph lines
  // NOTE: Scaling is at the graph level, but scaling max values are at the graph data level
  enum GraphScaleIndex
  {
    eMaxScaleIdx = 0,
    eAuthScaleIdx,
    eCapScaleIdx,
    eResHoldScaleIdx,
    eDmdDptFcstScaleIdx,
    eTrafficScaleIdx
  } ;

  // enumerations for the graph data styles
  typedef enum
  {
   eFirstStyleIdx = 0,
   eAuthStyleIdx = eFirstStyleIdx,
   eCapStyleIdx,
   eDmdDptFcstStyleIdx,
   eBktResHoldAvailSumStyleIdx,
   eResHoldTotStyleIdx,
   eResHoldAvgStyleIdx,
   eResHoldIdvStyleIdx,
   eTrafficResStyleIdx,
   eRevAvgFcstTotStyleIdx,
   eRevAvgFcstIdvStyleIdx,
   eRevAvgFcstHighStyleIdx,
   eRevAvgFcstLowStyleIdx,
   eResHoldStdvHighStyleIdx,
   eResHoldStdvLowStyleIdx,
   eBucket0StyleIdx,
   eBucket1StyleIdx,
   eBucket2StyleIdx,
   eBucket3StyleIdx,
   eBucket4StyleIdx,
   eBucket5StyleIdx,
   eBucket6StyleIdx,
   eBucket7StyleIdx,
   eBucket8StyleIdx,
   eBucket9StyleIdx,
   eVoidStyleIdx,
   eUnreservableCapStyleIdx,
   eStandingCapStyleIdx, 
   eHisto3OvbLevelStyleIdx,
   eHisto3ResHoldTotStyleIdx,
   eHisto2OvbLevelStyleIdx,
   eHisto2ResHoldTotStyleIdx,
   eHisto1OvbLevelStyleIdx,
   eHisto1ResHoldTotStyleIdx,
   eCmptComptageOfferStdvHighStyleIdx,
   eCmptComptageOfferStyleIdx,
   eCmptComptageOfferStdvLowStyleIdx,
   eCmptComptageOccupStdvHighStyleIdx,
   eCmptComptageOccupStyleIdx,
   eCmptComptageOccupStdvLowStyleIdx,
   eCmptComptageHist3OfferStyleIdx,
   eCmptComptageHist3OccupStyleIdx,
   eCmptComptageHist2OfferStyleIdx,
   eCmptComptageHist2OccupStyleIdx,
   eCmptComptageHist1OfferStyleIdx,
   eCmptComptageHist1OccupStyleIdx,
   ePrevisionStyleIdx,
   eRevTicketeStyleIdx,
   eHoiTraStyleIdx,
   eHoiTraIdvStyleIdx,
   eHisto3HoiTraStyleIdx,
   eHisto2HoiTraStyleIdx,
   eHisto1HoiTraStyleIdx,
   eLastStyleIdx
  }  CtaDataStyleIdx;

// Construction
public:
  CCTAChart();
	~CCTAChart();
  void SetParentView(CView* pParentView) { m_pParentView = pParentView; }
  CView* GetParentView() { return m_pParentView; }
  CCTAChartDataArray* GetChartDataArray() { return &m_apChartData; }
  CCTAChartData* GetChartData(int i) { return m_apChartData.GetAt(i); }
  CCTAChartData* GetCurChartData() { return m_pCurChartData; }
  void AddChartData(CCTAChartData* pNewChartData = NULL, BOOL bSort = TRUE );
  void SetCurChartData(int iIdx);
  void SetCurChartData(CCTAChartData* pChartData) { m_pCurChartData = pChartData; }
  void ClearCurChartData();
  void ClearChartDataArray();  // delete and remove all elements from the chart data array
  void ClearChartDataArray(enum CCTAChartData::GraphType eGraphType);  // delete and remove certain graph element from array

  void SetGraphFont();
  HFONT GetCtaHFont();

  virtual double GetFlashLabelValue(XrtDataHandle& hXrtData, int iCurFlashSet, int iCurFlashPt, BOOL bAccumulate);
  void DisplayFlashLabel(int iCurFlashSet, int iCurFlashPt, int iDataSet);
  void RemoveFlashLabel();

  CCTAChartHelpDlg* GetHelpDlg() { return m_pHelpDlg; }
  void SetHelpDlg(CCTAChartHelpDlg* pHelpDlg) { m_pHelpDlg = pHelpDlg; }
  void DisplayHelpDlg(CPoint point);
  void DestroyHelpDlg();
  virtual CCTAChartHelpDlg* CreateHelpDlg() = 0;
  virtual void ContextualDisplay(CPoint point, BOOL bInRange) = 0;

  int GetCurSelectSet() { return m_iCurSelectSet; }
  void SetCurSelectSet(int iCurSet) { m_iCurSelectSet = iCurSet; }
  int GetCurSelectPt()  { return m_iCurSelectPt; }
  void SetCurSelectPt(int iCurPt) { m_iCurSelectPt = iCurPt; }

  void SetCurScaling (enum GraphScaleIndex eScale) { m_eCurScale = eScale; }
  enum GraphScaleIndex GetCurScaling() { return m_eCurScale; }
  void ScaleGraph( enum GraphScaleIndex eGraphScale);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCTAChart)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
  void CCTAChart::OnMouseInDataArea(CPoint point);
	//{{AFX_MSG(CCTAChart)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

  CView* m_pParentView;             // parent view
  CCTAChartData* m_pCurChartData;  // current chart data object 
  CCTAChartDataArray m_apChartData;
  CCTAChartHelpDlg* m_pHelpDlg;  // help dialog

  XrtTextHandle m_FlashLabel;

  enum GraphScaleIndex m_eCurScale;  // current scaling value

  // for flash labels
  int m_iCurFlashSet;
  int m_iCurFlashPt;

  BOOL m_bAccumulateSets_DataSet1;  // do we accumulate the values for XRT_DATASET1 ?
  BOOL m_bAccumulateSets_DataSet2;  // do we accumulate the values for XRT_DATASET2 ?

  // for help window
  int m_iCurSelectSet;
  int m_iCurSelectPt;

public:
//  static XrtDataStyle aCtaDataStyles[];

  static CCTACHART_DATA_STYLE_ARRAY m_aDataStyles;

  static void SetDataStyleColor(CCTAChart::CtaDataStyleIdx eDataStyleIdx, int iRGBValue);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHART_H__EABECC93_9E13_11D1_8381_400020302A31__INCLUDED_)
