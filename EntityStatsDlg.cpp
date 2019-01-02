// EntityStatsDlg.cpp : implementation file
//

#include "StdAfx.h"



#include "EntityStatsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEntityStatsDlg dialog


CEntityStatsDlg::CEntityStatsDlg(CWnd* pParent /*=NULL*/)
	: YM_ETSLayoutDialog(CEntityStatsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEntityStatsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEntityStatsDlg::DoDataExchange(CDataExchange* pDX)
{
	YM_ETSLayoutDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEntityStatsDlg)
	DDX_Control(pDX, IDC_STATIC_VIEWED_VALUE, m_StaticViewedValue);
	DDX_Control(pDX, IDC_STATIC_UNSENT_VALUE, m_StaticUnsentValue);
	DDX_Control(pDX, IDC_STATIC_TOTAL_VALUE, m_StaticTotalValue);
	DDX_Control(pDX, IDC_STATIC_SENT_VALUE, m_StaticSentValue);
	DDX_Control(pDX, IDC_STATIC_SENTBATCH_VALUE, m_StaticSentBatchValue);
	DDX_Control(pDX, IDC_STATIC_SENTHRRD_VALUE, m_StaticSentHRRDValue);
	DDX_Control(pDX, IDC_STATIC_CRITICAL_VALUE, m_StaticCriticalValue);
	DDX_Control(pDX, IDOK, m_btnOK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEntityStatsDlg, YM_ETSLayoutDialog)
	//{{AFX_MSG_MAP(CEntityStatsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEntityStatsDlg message handlers

BOOL CEntityStatsDlg::OnInitDialog() 
{
	YM_ETSLayoutDialog::OnInitDialog();
	
	m_btnOK.SetDefaultShade();

  CPane staticPane1 = pane( HORIZONTAL, ABSOLUTE_VERT | ABSOLUTE_HORZ );
  staticPane1
	  << itemGrowing( HORIZONTAL )
	  << item( IDC_STATIC_TOTAL )
	  << item( IDC_STATIC_TOTAL_VALUE )
	  << itemGrowing( HORIZONTAL );

  CPane staticPane2 = pane( HORIZONTAL, ABSOLUTE_VERT | ABSOLUTE_HORZ );
  staticPane2
	  << itemGrowing( HORIZONTAL )
	  << item( IDC_STATIC_CRITICAL )
	  << item( IDC_STATIC_CRITICAL_VALUE )
	  << itemGrowing( HORIZONTAL );

  CPane staticPane3 = pane( HORIZONTAL, ABSOLUTE_VERT | ABSOLUTE_HORZ );
  staticPane3
	  << itemGrowing( HORIZONTAL )
	  << item( IDC_STATIC_VIEWED )
	  << item( IDC_STATIC_VIEWED_VALUE )
	  << itemGrowing( HORIZONTAL );

  CPane staticPane4 = pane( HORIZONTAL, ABSOLUTE_VERT | ABSOLUTE_HORZ );
  staticPane4
	  << itemGrowing( HORIZONTAL )
	  << item( IDC_STATIC_SENT )
	  << item( IDC_STATIC_SENT_VALUE )
	  << itemGrowing( HORIZONTAL );

  CPane staticPane5 = pane( HORIZONTAL, ABSOLUTE_VERT | ABSOLUTE_HORZ );
  staticPane5
	  << itemGrowing( HORIZONTAL )
	  << item( IDC_STATIC_SENT_BATCH )
	  << item( IDC_STATIC_SENTBATCH_VALUE )
	  << itemGrowing( HORIZONTAL );

  CPane staticPane6 = pane( HORIZONTAL, ABSOLUTE_VERT | ABSOLUTE_HORZ );
  staticPane6
	  << itemGrowing( HORIZONTAL )
	  << item( IDC_STATIC_SENT_HRRD )
	  << item( IDC_STATIC_SENTHRRD_VALUE )
	  << itemGrowing( HORIZONTAL );

  CPane staticPane7 = pane( HORIZONTAL, ABSOLUTE_VERT | ABSOLUTE_HORZ );
  staticPane7
	  << itemGrowing( HORIZONTAL )
	  << item( IDC_STATIC_UNSENT )
	  << item( IDC_STATIC_UNSENT_VALUE )
	  << itemGrowing( HORIZONTAL );

  CPane btnPane = pane ( HORIZONTAL )
	  << itemGrowing( HORIZONTAL )
	  << item( IDOK, NORESIZE )
	  << itemGrowing( HORIZONTAL );

  CreateRoot(VERTICAL)
	  << itemGrowing( VERTICAL )
    << staticPane1
	  << itemGrowing( VERTICAL )
	  << itemGrowing( VERTICAL )
    << staticPane2
	  << itemGrowing( VERTICAL )
    << staticPane3
	  << itemGrowing( VERTICAL )
    << staticPane4
	  << itemGrowing( VERTICAL )
    << staticPane5
	  << itemGrowing( VERTICAL )
	<< staticPane6
	  << itemGrowing( VERTICAL )
    << staticPane7
	  << itemGrowing( VERTICAL )
	  << btnPane;

  UpdateLayout();

	YM_RecordSet* pTrancheSet = 
    YM_Set<YmIcTrancheLisDom>::FromKey( TRANCHE_KEY);

  int iTotal = 0 ;
  int iViewed = 0;
  int iNotViewed = 0;
  int iSent = 0;
  int iSentBatch = 0;
  int iSentHRRD = 0;
  int iNotSent = 0;
  int iCritical = 0;

	if( pTrancheSet->IsValid() ) 
	{
    // iterate through the tranche record set and gather statistics
    //
		YM_Iterator<YmIcTrancheLisDom>* pIterator = 
			YM_Set<YmIcTrancheLisDom>::FromKey( TRANCHE_KEY)->CreateIterator();
		for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
		{
		  YmIcTrancheLisDom* pTranche = (YmIcTrancheLisDom*)pIterator->Current();

          // note we count only those trains which are OPEN !
          if (pTranche->NotOpenFlag() != 1)
          {
            iTotal++;

            if (pTranche->CriticalFlag())
              iCritical++;

            if ( (pTranche->EditStatus() & ~EDIT_STATUS_TGVJUMPCTSET) == EDIT_STATUS_NOTSEEN)
              iNotViewed++;
            else
              iViewed++;

            if (pTranche->SentFlag()& 1)
              iSent++;
            if (pTranche->SentFlag()& 2)
		      iSentBatch++;
			if (pTranche->SentFlag()& 4)
		      iSentHRRD++;
		    if (pTranche->SentFlag() == 0)
              iNotSent++;
          }
        }
		delete pIterator;
    }

	m_szCriticalTranches.Format("%d", iCritical);
	m_szSentTranches.Format("%d", iSent);
	m_szSentBatchTranches.Format("%d", iSentBatch);
	m_szSentHRRDTranches.Format("%d", iSentHRRD);
	m_szUnsentTranches.Format("%d", iNotSent);
	m_szViewedTranches.Format("%d", iViewed);
	m_szTotalTranches.Format("%d", iTotal);

	m_StaticViewedValue.SetWindowText(m_szViewedTranches);
	m_StaticUnsentValue.SetWindowText(m_szUnsentTranches);
	m_StaticTotalValue.SetWindowText(m_szTotalTranches);
	m_StaticSentValue.SetWindowText(m_szSentTranches);
	m_StaticSentBatchValue.SetWindowText(m_szSentBatchTranches);
	m_StaticSentHRRDValue.SetWindowText(m_szSentHRRDTranches);
	m_StaticCriticalValue.SetWindowText(m_szCriticalTranches);

    UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
