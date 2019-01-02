// LegListboxDlg.cpp : implementation file
//
#include "StdAfx.h"



#include "LegListboxDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLegListboxDlg dialog

CLegListboxDlg::CLegListboxDlg(
                               CWnd* pParent /*=NULL*/, 
                               BOOL bSelectAll /*= FALSE*/, 
                               BOOL bIncCurLeg /*=TRUE*/, 
                               BOOL bAllowHoles /*=TRUE*/, 
                               enum CLegListboxDlg::LegListSortKey eSortKey /* = eDefault */ )
	: CDialog(CLegListboxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLegListboxDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  m_pParent = pParent;
  m_aiSelBuf = NULL;
  m_bSelectAll = bSelectAll;  // select all legs by default
  m_bIncCurLeg = bIncCurLeg;  // include the current leg in the list
  m_bAllowHoles = bAllowHoles;  // allow the user to select non-consecutive legs
  m_eLegSortKey = eSortKey;  // key to sort the list of legs
}

CLegListboxDlg::~CLegListboxDlg()
{
}

void CLegListboxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLegListboxDlg)
	DDX_Control(pDX, IDC_LISTBOX_LEGS, m_LegListbox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLegListboxDlg, CDialog)
	//{{AFX_MSG_MAP(CLegListboxDlg)
	ON_BN_CLICKED(IDC_BUTTON_SELCT_ALL_LEGS, OnButtonSelctAllLegs)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLegListboxDlg message handlers

BOOL CLegListboxDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  // Get the currently loaded leg
  int iLegKey = LEG_KEY;

	YM_RecordSet* pLegSet = YM_Set<YmIcLegsDom>::FromKey(iLegKey);
  YmIcLegsDom* pLegCurrent = (YmIcLegsDom*)pLegSet->GetCurrent();

  // Iterate through the leg list adding all but the currently loaded leg
  int iIndex=0;
  int iPosition = 0;
  CString szOrigDest;
	YM_Iterator<YmIcLegsDom>* pLegIterator = 
		YM_Set<YmIcLegsDom>::FromKey(iLegKey)->CreateIterator();
  YmIcLegsDom* pLeg;
  for( pLegIterator->First(); !pLegIterator->Finished(); pLegIterator->Next() ) 
  {
    pLeg = pLegIterator->Current();
    if ( (pLeg != pLegCurrent) || m_bIncCurLeg )  // do we include the current leg ?
    {
      szOrigDest.Format( "%s\t%s", pLeg->LegOrig(), pLeg->LegDest() );
      iPosition = GetSortPos( iIndex, pLeg );
      m_LegListbox.InsertString( iPosition, szOrigDest );
      m_LegListbox.SetItemDataPtr( iPosition, pLeg );
//      pLeg = (YmIcLegsDom*)m_LegListbox.GetItemDataPtr( iIndex );
      iIndex++;
    }
  }
  delete pLegIterator;

  if ( m_bSelectAll )
  {
    if (m_LegListbox.GetCount() == 1)
      m_LegListbox.SetSel( 0, TRUE );
    else
      m_LegListbox.SelItemRange( TRUE, 0, m_LegListbox.GetCount()-1 );	
  }

  CenterWindow( GetOwner() );  // Center the dialog box in the Leg List
  m_LegListbox.SetFocus();

  return FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLegListboxDlg::OnOK() 
{
  // ensure the user has selected at least one item in the listbox
  if ( m_LegListbox.GetSelCount( ) == 0 )
  {
    MessageBeep(MB_ICONEXCLAMATION);
    return;
  }

  // put the index of all selected items in the integer array
  // create and initialize an integer array to hold the indeces of all selected items
  int iListCount =  m_LegListbox.GetCount();  // how many items in the listbox ?
  m_aiSelBuf = new int [iListCount];
  m_iSelectedLegCount  = m_LegListbox.GetSelItems( iListCount, m_aiSelBuf ) ;

  // determine if the user is forced to select consecutive legs
  if ( ! m_bAllowHoles )
  {
    // ensure a consecutive
    for ( int i=0; i< (m_iSelectedLegCount-1); i++ )
    {
     if ( (m_aiSelBuf[ i ] + 1) != m_aiSelBuf [ i+1 ] )
      {
        CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CONSECUTIVE_LEG_SELECTION);
        AfxMessageBox( szText, MB_ICONHAND | MB_APPLMODAL );
        return;
      }
    }
  }

  m_apLegs.SetSize( m_iSelectedLegCount );

  // put each selected leg's pointer into the leg array
  YmIcLegsDom* pLeg = NULL;
  for (int iIndex = 0; iIndex < m_iSelectedLegCount ; iIndex++)
  {
    pLeg = (YmIcLegsDom*)m_LegListbox.GetItemDataPtr( m_aiSelBuf[ iIndex ] );
    m_apLegs[ iIndex ] = pLeg ;
  }

  CDialog::OnOK();
}

// Return the position in which to put this leg based upon sort order
//
int CLegListboxDlg::GetSortPos( int iIndex, YmIcLegsDom* pLeg )
{
  // for default sort order, do nothing
  if ( m_eLegSortKey == eDefault )
    return iIndex;
  YmIcLegsDom* pLegComp;
  int iPos;
  for ( iPos = 0; iPos < iIndex; iPos++ )
  {
    pLegComp = (YmIcLegsDom*)m_LegListbox.GetItemDataPtr( iPos );
    switch( m_eLegSortKey )
    {
    case eSequence:
      if ( pLeg->LegSeq() < pLegComp->LegSeq() )
        return iPos;
      break;
    case eLegOrig:
      if ( strcmp( pLeg->LegOrig(), pLegComp->LegOrig() ) < 0 )
        return iPos;
      break;
    case eDefault:
    default:
      break;
    }
  }
  return iPos;
}

void CLegListboxDlg::OnButtonSelctAllLegs() 
{
  if (m_LegListbox.GetCount() == 1)
    m_LegListbox.SetSel( 0, TRUE );
  else
    m_LegListbox.SelItemRange( TRUE, 0, m_LegListbox.GetCount()-1 );	
}

void CLegListboxDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
  // clean up the arrays...
  delete [ ] m_aiSelBuf;
  m_aiSelBuf = NULL;
}
