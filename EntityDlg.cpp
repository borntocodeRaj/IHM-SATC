// EntityDlg.cpp : implementation file
//

#include "limits.h"
#include "stdafx.h"

//RW includes
#include <rw/db/db.h>













#include "EntityDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constants
const char 
CEntityDlg::szEntityConst[] = "Entity";
const char 
CEntityDlg::szBegDptDateConst[] = "Beg Dpt Date";
const char 
CEntityDlg::szEndDptDateConst[] = "End Dpt Date";
const char 
CEntityDlg::szDOWConst[] = "DOW";
const char 
CEntityDlg::szLastFcstDateOffsetConst[] = "Last Fcst Date Offset";
const char 
CEntityDlg::szUseEndDptDateConst[] = "Use End Dpt Date";
const char 
CEntityDlg::szConstrainLastFcDateConst[] = "Constrain Last Auth Fc Date";
const char 
CEntityDlg::szNoConstrainDptDateConst[] = "No Constrain Dpt Date";
const char
CEntityDlg::szTrainAutoOnly[] = "Auto Trains Only";
const char
CEntityDlg::szAllJMX[] = "All J-x";

/////////////////////////////////////////////////////////////////////////////
// CEntityDlg dialog

CEntityDlg::CEntityDlg(CCTADoc* pCTADoc, CWnd* pParent /*=NULL*/)
: YM_ETSLayoutDialog(CEntityDlg::IDD, pParent)
{
  m_pCTADoc = pCTADoc;
  m_strRegStore = "EntityDlg";
  m_bAutoTrain = false;
  
  //{{AFX_DATA_INIT(CEntityDlg)
  //}}AFX_DATA_INIT
}

void CEntityDlg::DoDataExchange(CDataExchange* pDX)
{
  YM_ETSLayoutDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CEntityDlg)
  DDX_Control(pDX, IDC_STARTDPTDATE, m_ctlStartDptDate);
  DDX_DateTimeCtrl(pDX, IDC_STARTDPTDATE, m_tmStartDptDate);
  DDX_Control(pDX, IDC_ENDDPTDATE, m_ctlEndDptDate);
  DDX_DateTimeCtrl(pDX, IDC_ENDDPTDATE, m_tmEndDptDate);
  DDX_Control(pDX, IDC_ENTITY_LISTVIEW, m_EntityView);
  DDX_Control(pDX, IDOK, m_btnOK);
  DDX_Control(pDX, IDCANCEL, m_btnCancel);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEntityDlg, YM_ETSLayoutDialog)
//{{AFX_MSG_MAP(CEntityDlg)
ON_BN_CLICKED(IDC_DPT_DATE_RADIO, OnDptDate)
ON_BN_CLICKED(IDC_FCST_DAY1, OnFcstDay1)
ON_BN_CLICKED(IDC_FCST_DAY2, OnFcstDay2)
ON_BN_CLICKED(IDC_FCST_DAY3, OnFcstDay3)
ON_BN_CLICKED(IDC_FCST_DAY4, OnFcstDay4)
ON_BN_CLICKED(IDC_FCST_DAY5, OnFcstDay5)
ON_BN_CLICKED(IDC_FCST_DAY6, OnFcstDay6)
ON_BN_CLICKED(IDC_FCST_DAY7, OnFcstDay7)
ON_BN_CLICKED(IDC_CHECK_MON, OnCheckDow)
ON_BN_CLICKED(IDC_CHECK_ALL, OnCheckDowAll)
ON_BN_CLICKED(IDC_CHECK_END_DPT_DATE, OnUseEndDptDate)
//ON_BN_CLICKED(IDC_CHECK_LAST_AUTH_FCST_DATE, OnConstrainLastAuthFcDate)
//ON_BN_CLICKED(IDC_CHECK_NO_DPT_DATE_CONSTRAINT, OnConstrainDptDate)
ON_BN_CLICKED(IDC_CHECK_TUE, OnCheckDow)
ON_BN_CLICKED(IDC_CHECK_WED, OnCheckDow)
ON_BN_CLICKED(IDC_CHECK_THU, OnCheckDow)
ON_BN_CLICKED(IDC_CHECK_FRI, OnCheckDow)
ON_BN_CLICKED(IDC_CHECK_SAT, OnCheckDow)
ON_BN_CLICKED(IDC_CHECK_SUN, OnCheckDow)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_ENTITY_LISTVIEW, OnItemchanged)
ON_NOTIFY(DTN_DATETIMECHANGE, IDC_STARTDPTDATE, OnStartDptDateChange)
ON_NOTIFY(DTN_DATETIMECHANGE, IDC_ENDDPTDATE, OnEndDptDateChange)
ON_NOTIFY(LVN_COLUMNCLICK, IDC_ENTITY_LISTVIEW, OnColumnclickEntityListview)
ON_NOTIFY(NM_DBLCLK, IDC_ENTITY_LISTVIEW, OnLButtonDblClk)
ON_NOTIFY(DTN_CLOSEUP, IDC_STARTDPTDATE, OnCloseupDatetimepickerStart)
ON_NOTIFY(DTN_CLOSEUP, IDC_ENDDPTDATE, OnCloseupDatetimepickerEnd)
ON_WM_LBUTTONUP()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CEntityDlg::SetColumnAttribute(int iIndex, LPARAM lParam)
{
  ASSERT(iIndex >= 0 && iIndex < MAX_COLUMNS_ENTITY);
  
  m_ViewAttrArray[iIndex] = lParam;
}

///////////////////////////////////////////////////////////////
// Build the key to the profile settings in the registry 
//  (this is to allow the entity dialog to write registry entries like the YmPersistenChildWnd objects)
//
void CEntityDlg::BuildProfileKey(CString& szProfileKey)
{
  // Set current profile key
  CString szRegistryKey = APP->GetPersistence()->GetCurrentConfiguration();
  szProfileKey = szRegistryKey + "\\ENTITY LIST Window";
}

//////////////////////////////////////////////////
// Static Compare callback function
//  for sorting columns on Columnclick method
//
int CALLBACK CEntityDlg::Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
  CString szCmp1, szCmp2;
  if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetEntityDescFlag() )
  {
    szCmp1 = ((YmIcEntitiesDom*)lParam1)->Description();
    szCmp2 = ((YmIcEntitiesDom*)lParam2)->Description();
  }
  else
  {
    szCmp1 = ((YmIcEntitiesDom*)lParam1)->Entity();
    szCmp2 = ((YmIcEntitiesDom*)lParam2)->Entity();
  }
  return szCmp1.Compare( szCmp2 ) *  ((lParamSort & REVERSE_ORDER_FLAG) ? -1 : 1 );
}

/////////////////////////////////////////////////////////////////////////////
// Determine if this entity is in the list 
BOOL CEntityDlg::FindEntity(CString szListP, const char* szEntity)
{
  CString szListBis = szListP;
  char *szList = szListBis.GetBuffer( szListBis.GetLength() ); 
  
  // Parse the list
  const char acDelimeters[] = ",'";
  char *szToken = strtok((LPSTR)szList, acDelimeters);
  BOOL bReturn = FALSE;
  while ( szToken )
  {
    if (!strcmp(szEntity, szToken))
    {
      bReturn = TRUE;
      break;
    }
    szToken = strtok(NULL, acDelimeters);  // get next token
  }
  szListBis.ReleaseBuffer();
  return(bReturn);
}

//////////////////////////////////////////////////////////////////////
// Initialize and fill the Entity Listbox
void CEntityDlg::InitializeEntityList() 
{
  // Add a column to the List View
  SetColumnAttribute( 0, CEntityDlg::ENTITY );
  m_EntityView.InsertColumn( 0, 
    ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_ENTITY), 
    LVCFMT_LEFT, 160 );

  m_bAutoTrain = true;
  
  // Is the Entity Record Set valid ?
 if( m_pCTADoc->m_pEntitySet->IsValid() ) 
  {
    CString szProfileKey;
    BuildProfileKey(szProfileKey);
    CString szSavedEntity(APP->GetProfileString(szProfileKey, szEntityConst));
    
	// NPI - Ano 56494
	// Chargement de la liste des entités sauvegardées en base de registre
	CStringArray obListEntity;
	LPSTR lpEntity;
	// NPI - Ano 58169
	// Allocation dynamique de la mémoire affectée à la variable szTokEntity
	// pour meilleure gestion de l'initialisation de la sélection des entités
	//char szTokEntity[200];
	char* szTokEntity = NULL;
	szTokEntity = new char[szSavedEntity.GetLength() + 1];
	///

	strcpy(szTokEntity, szSavedEntity);
	lpEntity = strtok(szTokEntity, ",");
	while (lpEntity != NULL)
	{
		obListEntity.Add(lpEntity);
		lpEntity = strtok(NULL, ",");
	}

	delete szTokEntity; // NPI - Ano 58169
	///

    // depending upon system parm, do we display the Entity or the Entity Description ?
    BOOL bDispEntDesc = 
      (BOOL) ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetEntityDescFlag();
    
    LV_ITEM	Item;
    
    m_EntityView.DeleteAllItems();
    
    // iterate through the entity record set and insert into the list
    BOOL bSelected = FALSE;
    int iSelIndex = 0;
    int iIndex = 0;
	CString szEnti;
    YM_Iterator<YmIcEntitiesDom>* pIterator = YM_Set<YmIcEntitiesDom>::FromKey( ENTITY_KEY)->CreateIterator();
    for( pIterator->First(); !pIterator->Finished(); pIterator->Next() ) 
    {
      YmIcEntitiesDom* pEntity = (YmIcEntitiesDom*)pIterator->Current();
      
      // find the position of the last saved entity and insert it as 'selected'
	  if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetMultiEntitySelFlag())
	    m_EntityView.ModifyStyle(LVS_SINGLESEL,0);
      else
  	    m_EntityView.ModifyStyle(0,LVS_SINGLESEL);
	  // test autotrain seul
      szEnti = pEntity->Entity();
	  if (szEnti.Find("AUTO") == -1)
	    m_bAutoTrain = false;

	  CString szLabel = ( bDispEntDesc ) ? pEntity->Description() : pEntity->Entity(); 
      
	  // NPI - Ano 56494
	  // szSavedEntity peut contenir plusieurs entités ou un libellé d'entité peut-être en partie identique à un autre
	  // Récupérer la ou les différentes entités afin de faire une comparaison précise au lieu d'un find

	  /*if ( szSavedEntity.Find(szLabel) >= 0 )
      {
        bSelected = TRUE;
        Item.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
        Item.state = LVIS_SELECTED;
        Item.stateMask = LVIS_SELECTED ;
      }
      else
      {
        Item.mask = LVIF_TEXT | LVIF_PARAM;
      }*/

	  BOOL bEntityInList = FALSE;
	  for (int nEntity = 0; nEntity < obListEntity.GetSize(); nEntity++)
	  {
		  // En raison de la méthode de sauvegarde des entités, on ajoute des apostrophes (') à l'entité à comparer
		  CString osLabel;
		  osLabel.Format("'%s'", szLabel);

		  CString osEntity = obListEntity.ElementAt(nEntity);
		  if (osEntity == osLabel)
		  {
			  bEntityInList = TRUE;
			  break;
		  }
	  }

	  if (bEntityInList)
      {
        bSelected = TRUE;
        Item.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
        Item.state = LVIS_SELECTED;
        Item.stateMask = LVIS_SELECTED ;
      }
      else
      {
        Item.mask = LVIF_TEXT | LVIF_PARAM;
      }
	  ///
      
      // add an item to the list.  note we add either entity or entity description depending upon parm
      Item.iItem = 0;
      Item.iSubItem = 0;
      Item.pszText = ( bDispEntDesc ) ? (char*)pEntity->Description() : (char*)pEntity->Entity();
      Item.lParam = (LONG)pEntity;
      
      m_EntityView.InsertItem( &Item );
      iIndex++;
    }
    delete pIterator;

    
    // sort the list
    if( m_EntityView.GetItemCount() > 0 ) 
      m_EntityView.SortItems( Compare, m_ViewAttrArray[ 0 ] );
    
    m_EntityView.SetColumnWidth( 0, LVSCW_AUTOSIZE );

    // ensure at least one item was selected
    if (! bSelected)
    {
      // select the first item by default
      Item.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
      Item.state = LVIS_SELECTED;
      Item.stateMask = LVIS_SELECTED ;
      m_EntityView.SetItemState( 0, &Item );
    }
    
    // ensure the selected item is visible
    LV_FINDINFO fi;
    fi.flags = LVFI_STRING;
    fi.psz = ((LPCSTR)szSavedEntity);
    iSelIndex = (m_EntityView.FindItem( &fi, -1 ));
    m_EntityView.EnsureVisible( iSelIndex, FALSE );

	if (m_bAutoTrain)
	{
	  ((CButton *)GetDlgItem(IDC_TOUT_JMX))->ShowWindow (SW_HIDE);
      ((CButton *)GetDlgItem(IDC_SATC_JMX))->ShowWindow (SW_HIDE);
	  ((CButton *)GetDlgItem(IDC_CHECK_AUTO_ONLY))->ShowWindow (SW_HIDE);
	}
  }
  else 
  {
    m_EntityView.DeleteAllItems();
  }
}

BOOL CEntityDlg::SetLastAuthFcstDate()
{
  // Confirm that we have the Last Auth Fcst Date from SC_Dc_Date table (Collection Date)
  YM_Iterator<YmIcDcDateDom>* pDcDateIterator = 
    YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY)->CreateIterator();
  if ( pDcDateIterator->GetCount() == 0 )  // Do we have data in the table ?
  {
    CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_DATA_IN_TABLE);
    szText += " SC_COLLECTION_DATE";
    AfxMessageBox( szText, MB_ICONHAND | MB_APPLMODAL );
    delete pDcDateIterator;
    EndDialog( TRUE );
    return FALSE;
  }
  delete pDcDateIterator;
  
  long lLastAuthFcDate = 
    YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY)->DcDate();
  if (lLastAuthFcDate == 0)
  {
    CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_INVALID_DCDATE);
    AfxMessageBox( szText, MB_ICONHAND | MB_APPLMODAL );
    EndDialog( TRUE );
    return TRUE;
  }
  m_pCTADoc->SetLastAuthFcDate(lLastAuthFcDate);
  m_pCTADoc->SetConstrainLastAuthFcDate(1);

  // j'utilise le domaine conges (DATE_DEB, DATE_FIN) pour récupérer, dpt_date min et max de la sc_tranches
  // lue pendant la phase d'initialisation, la donnée doit être prête.
  YM_Iterator<YmIcCongesDom>* pIterator = YM_Set<YmIcCongesDom>::FromKey( MINMAX_DPT_KEY)->CreateIterator();
  pIterator->First();  // il n'y a qu'une ligne dans le recordset
  YmIcCongesDom* pConges = (YmIcCongesDom*)pIterator->Current();
  delete pIterator; // NPI - Correction Memory Leak
    
  // set min and max dpt dates
  m_lMinBegDptDate = lLastAuthFcDate -1;
  m_lMaxBegDptDate = pConges->DateFin();
  m_lMinEndDptDate = lLastAuthFcDate -1;
  m_lMaxEndDptDate = pConges->DateFin();
  
  return TRUE;
}

void CEntityDlg::InitializeBatchDayRadioButtons()
{
  // get local time
  byte byDOW;
  time_t lTime ;
  time ( &lTime ) ;
  struct tm *m_aTime = localtime ( &lTime ) ;
  
  // find the correct offset for current day of week
  if (m_aTime->tm_wday == 0)  // Sunday is tm_wday == 0
    byDOW = 6;
  else
    byDOW = (BYTE) (m_aTime->tm_wday - 1) ;
  
  // load the day of week and set for each radio button
  // note: !!! Identifiers for IDC_FCST_DAY1 through IDC_FCST_DAY7 MUST BE CONSECUTIVE !!!
  unsigned short usID;
  for (usID = IDC_FCST_DAY1; usID <= IDC_FCST_DAY7; usID++)
  {
    int iId = 0;
    switch (byDOW)
    {
    case 0:  iId = IDS_MONDAY;  break;
    case 1:  iId = IDS_TUESDAY;  break;
    case 2:  iId = IDS_WEDNESDAY;  break;
    case 3:  iId = IDS_THURSDAY;  break;
    case 4:  iId = IDS_FRIDAY;  break;
    case 5:  iId = IDS_SATURDAY;  break;
    case 6:  iId = IDS_SUNDAY;  break;
    }
    ((CButton *)GetDlgItem(usID))->
      SetWindowText(APP->GetResource()->LoadResString( iId ));
    
    byDOW = (byDOW > 0) ? byDOW - 1 : 6;
  }
}

void CEntityDlg::InitializeDptDateRange()
{
  CString szProfileKey;
  BuildProfileKey(szProfileKey);
  RWDate rwDateToday;
  unsigned long lSavedNoConstrainDptDate = APP->GetProfileInt(szProfileKey, szNoConstrainDptDateConst, 0);
  unsigned long lSavedBegDptDate = APP->GetProfileInt(szProfileKey, szBegDptDateConst, JAN_1_1900);
  unsigned long lSavedEndDptDate = APP->GetProfileInt(szProfileKey, szEndDptDateConst, rwDateToday.julian());
  unsigned long lSavedUseEndDate = APP->GetProfileInt(szProfileKey, szUseEndDptDateConst, -1);
  unsigned long lSavedFcstDateOffset = APP->GetProfileInt(szProfileKey, szLastFcstDateOffsetConst, 0);
  unsigned long lSavedConstrainLastAuthFcDate = APP->GetProfileInt(szProfileKey, szConstrainLastFcDateConst, 1);
  unsigned long lSavedAutoTrainOnly = APP->GetProfileInt(szProfileKey, szTrainAutoOnly, 0);
  CString szSavedDOW(APP->GetProfileString(szProfileKey, szDOWConst));
  unsigned long lSavedAllJMX = APP->GetProfileInt(szProfileKey, szAllJMX, 0);

  if (m_bAutoTrain)
    lSavedAllJMX = 0;
  
  // set arbitrary min max range
  COleDateTime* pMinRange = new COleDateTime;
  COleDateTime* pMaxRange = new COleDateTime;
  pMinRange->SetDate( 1980, 1, 1 );
  pMaxRange->SetDate( 2030, 12, 31 );
  m_ctlStartDptDate.SetRange( pMinRange, pMaxRange );
  m_ctlEndDptDate.SetRange( pMinRange, pMaxRange );
  delete pMinRange;
  delete pMaxRange;
  
  // if the user has last chosen dpt date, and the dpt date is still within the allowed date range, set it as the default
  RWDate rwBegDate;
  RWDate rwEndDate;
  // if the user last checked "No Constrain Dpt Date" checkbox, we do not care about min and max date checking
  if (
    ( (lSavedNoConstrainDptDate) &&
    (lSavedBegDptDate != JAN_1_1900) ) ||
    ( (lSavedBegDptDate >= m_lMinBegDptDate) && 
    (lSavedBegDptDate <= m_lMaxBegDptDate) && 
    (lSavedBegDptDate != JAN_1_1900) )
    )
  {
    // use dpt date...
    m_lBegDptDate = lSavedBegDptDate;
    m_lEndDptDate = lSavedEndDptDate;
    
    OnDptDate();  // select the dpt date radio button and deselect all the other radio buttons
    
    // set the check box on by default
    if ( lSavedUseEndDate < 0 )  // -1 means it wasn't found in the registry
    {
      ((CButton *)GetDlgItem(IDC_CHECK_END_DPT_DATE))->SetCheck(0);
      ((CEdit*)GetDlgItem(IDC_ENDDPTDATE))->EnableWindow( FALSE );
    }
    else  // found it in the registry...was it 0 or 1 ?
    {
      ((CButton *)GetDlgItem(IDC_CHECK_END_DPT_DATE))->SetCheck(lSavedUseEndDate);
      ((CEdit*)GetDlgItem(IDC_ENDDPTDATE))->EnableWindow( lSavedUseEndDate);
    }
  }
  else
  {
    // determine which radio button (fcst date) was last checked
    SetFcstDaySelectionButtons( 0 );
    ((CButton *)GetDlgItem(IDC_DPT_DATE_RADIO))->SetCheck( 0 );
    EnableDptDateEdit( 0 );
    
    CButton *pButtonFcstDay = (CButton *)(GetDlgItem(IDC_FCST_DAY1+lSavedFcstDateOffset));
    if ( pButtonFcstDay )
      pButtonFcstDay->SetCheck( 1 );
    else
      ((CButton *)GetDlgItem(IDC_FCST_DAY1))->SetCheck( 1 );
    
    // fill the edit controls with default today's date (if between min and max)
    if ( rwBegDate.julian() < m_lMinBegDptDate )
    {
      m_lBegDptDate = m_lMinBegDptDate;
      m_lEndDptDate = m_lMinEndDptDate;
    }
    else
    {
      if ( rwBegDate.julian() > m_lMaxBegDptDate )
      {
        m_lBegDptDate = m_lMaxBegDptDate;
        m_lEndDptDate = m_lMaxEndDptDate;
      }
      else
      {
        m_lBegDptDate = rwBegDate.julian();
        m_lEndDptDate = rwEndDate.julian();
      }
    }
  }
  
  // initialize the date time picker controls (start and end dpt date range)
  rwBegDate.julian( m_lBegDptDate );
  COleDateTime t1(rwBegDate.year(), rwBegDate.month(), rwBegDate.dayOfMonth(), 0, 0, 0);
  m_tmStartDptDate = t1;
  m_ctlStartDptDate.SetTime(m_tmStartDptDate);
  
  rwEndDate.julian( m_lEndDptDate );
  COleDateTime t2(rwEndDate.year(), rwEndDate.month(), rwEndDate.dayOfMonth(), 0, 0, 0);
  m_tmEndDptDate = t2;
  m_ctlEndDptDate.SetTime(m_tmEndDptDate);
    
  // check the appropriate day of week buttons
  if ( szSavedDOW.IsEmpty() )
  {
    SetDOWSelectionButtons( 1 );
  }
  else
  {
    BOOL bAllChecked = TRUE;
    // must determine the start day of the week (U.S. is Sunday / Europe is Monday)
    ( szSavedDOW.Find('1') >= 0 ) ? ((CButton *)GetDlgItem(IDC_CHECK_MON))->SetCheck(1) :
                                     bAllChecked = FALSE;
    ( szSavedDOW.Find('2') >= 0 ) ? ((CButton *)GetDlgItem(IDC_CHECK_TUE))->SetCheck(1) :
                                     bAllChecked = FALSE;
    ( szSavedDOW.Find('3') >= 0 ) ? ((CButton *)GetDlgItem(IDC_CHECK_WED))->SetCheck(1) :
                                     bAllChecked = FALSE;
    ( szSavedDOW.Find('4') >= 0 ) ? ((CButton *)GetDlgItem(IDC_CHECK_THU))->SetCheck(1) :
                                     bAllChecked = FALSE;
    ( szSavedDOW.Find('5') >= 0 ) ? ((CButton *)GetDlgItem(IDC_CHECK_FRI))->SetCheck(1) :
                                     bAllChecked = FALSE;
    ( szSavedDOW.Find('6') >= 0 ) ? ((CButton *)GetDlgItem(IDC_CHECK_SAT))->SetCheck(1) :
                                     bAllChecked = FALSE;
    ( szSavedDOW.Find('7') >= 0 ) ? ((CButton *)GetDlgItem(IDC_CHECK_SUN))->SetCheck(1) :
                                     bAllChecked = FALSE;
    if ( bAllChecked )
      ((CButton *)GetDlgItem(IDC_CHECK_ALL))->SetCheck(1);
  }
  
  ((CButton *)GetDlgItem(IDC_TOUT_JMX))->SetCheck(lSavedAllJMX);
  ((CButton *)GetDlgItem(IDC_SATC_JMX))->SetCheck(!lSavedAllJMX);
  ((CButton *)GetDlgItem(IDC_CHECK_AUTO_ONLY))->SetCheck(lSavedAutoTrainOnly);
}

/////////////////////////////////////////////////////////////////////////////
// CEntityDlg message handlers

BOOL CEntityDlg::OnInitDialog() 
{
  YM_ETSLayoutDialog::OnInitDialog();
  
  m_btnOK.SetDefaultShade();
  m_btnCancel.SetDefaultShade();
  
  CPane batchPane = pane( VERTICAL )
    << item( IDC_FCST_DAY1, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_FCST_DAY2, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_FCST_DAY3, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_FCST_DAY4, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_FCST_DAY5, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_FCST_DAY6, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_FCST_DAY7, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_DPT_DATE_RADIO, ABSOLUTE_HORZ);
  
  CPane groupBatchPane = paneCtrl( IDC_ENTITY_FCSTDATE_GROUPBOX, VERTICAL, GREEDY, nDefaultBorder, 15, 15 )
    << batchPane;
  
  CPane dowPane = pane( VERTICAL )
    << item( IDC_CHECK_MON, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_CHECK_TUE, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_CHECK_WED, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_CHECK_THU, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_CHECK_FRI, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_CHECK_SAT, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_CHECK_SUN, ABSOLUTE_HORZ)
    << itemGrowing(VERTICAL)
    << item( IDC_CHECK_ALL, ABSOLUTE_HORZ);
  
  CPane groupDOWPane = paneCtrl( IDC_ENTITY_DOW_GROUPBOX, VERTICAL, GREEDY, nDefaultBorder, 15, 15 )
    << dowPane;
  
  CPane dateH1Pane = pane( HORIZONTAL )
    << item( IDC_CHECK_NULL, NORESIZE | ALIGN_CENTER )
    << item( IDC_BEG_DPTDATE_STATIC, NORESIZE | ALIGN_CENTER )
    << item( IDC_STARTDPTDATE, NORESIZE | ALIGN_CENTER )
   << item( IDC_CHECK_AUTO_ONLY, NORESIZE | ALIGN_CENTER );
  
  CPane dateH2Pane = pane( HORIZONTAL )
    << item( IDC_CHECK_END_DPT_DATE, NORESIZE | ALIGN_CENTER )
    << item( IDC_END_DPTDATE_STATIC, NORESIZE | ALIGN_CENTER )
    << item( IDC_ENDDPTDATE, NORESIZE | ALIGN_CENTER );
  
  CPane dateH3Pane = pane( HORIZONTAL )
    << item( IDC_CHECK_NO_DPT_DATE_CONSTRAINT, NORESIZE | ALIGN_CENTER )
    << item( IDC_SELECT_PREVIOUSLY_DEPARTED_TRAINS, NORESIZE | ALIGN_CENTER );
  
  CPane dateH4Pane = pane( VERTICAL )
    //<< item( IDC_CHECK_LAST_AUTH_FCST_DATE, NORESIZE | ALIGN_CENTER )
    //<< item( IDC_LAST_AUTH_FCST_DATE_STATIC, NORESIZE | ALIGN_CENTER );
    << item( IDC_TOUT_JMX, NORESIZE | ALIGN_LEFT )
    << item( IDC_SATC_JMX, NORESIZE | ALIGN_LEFT );
  
  CPane dateV1Pane = pane( VERTICAL )
    << dateH1Pane << dateH2Pane << dateH3Pane << dateH4Pane;
  
  CPane groupDatePane = paneCtrl( IDC_ENTITY_DPTDATE_GROUPBOX, VERTICAL, NORESIZE, nDefaultBorder, 15, 15 )
    << dateV1Pane;
  
  CPane datePane = pane( HORIZONTAL, NORESIZE )
    << groupDatePane;
  
  CPane paneOpt = pane( HORIZONTAL )
    << groupBatchPane
    << groupDOWPane;
  
  CPane btnPane = pane( HORIZONTAL, GREEDY, 20 )
    << item( GetPaneNull(), ALIGN_FILL_HORZ | NORESIZE )
    << item( IDOK, NORESIZE )
    << item( IDCANCEL, NORESIZE )
    << item( GetPaneNull(), ALIGN_FILL_HORZ | NORESIZE );
  
  CPane btnFillerPane = pane( HORIZONTAL )
    << item( GetPaneNull(), ALIGN_FILL_HORZ | NORESIZE );
  
  CPane rightPane = pane( VERTICAL )
    << paneOpt
    << datePane;
  
  CPane groupEntityPane = paneCtrl( IDC_STATIC_ENTITY, VERTICAL, GREEDY, nDefaultBorder, 15, 15 )
    << item( IDC_ENTITY_LISTVIEW );
  
  CPane entityPane = pane( HORIZONTAL )
    << groupEntityPane;
  
  CPane leftPane = pane( VERTICAL )
    << entityPane;
  
  CPane mainPane = pane( HORIZONTAL )
    << leftPane
    << rightPane;
  
  CreateRoot( VERTICAL )
    << mainPane
    << btnFillerPane
    << btnPane
    << btnFillerPane;
  
  UpdateLayout();
  
  if ( ! SetLastAuthFcstDate() )  // Confirm that we have the Last Auth Fcst Date from SC_Dc_Date table (Collection Date)
    return TRUE;
  
  // restore the saved entity from the registry to select for the user
  CString szProfileKey;
  BuildProfileKey(szProfileKey);
  
  m_wCurrentItem = 0;
  
  InitializeEntityList();  // Initialize and fill the Entity listbox
  InitializeBatchDayRadioButtons();  // Initialize the radio buttons for selection of batch run
  InitializeDptDateRange();
  
  m_EntityView.SetFocus();
  // DM 5524 LRO le 28-08-06
  if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetMultiEntitySelFlag())
	m_EntityView.ModifyStyle(LVS_SINGLESEL,0);
  else
	m_EntityView.ModifyStyle(0,LVS_SINGLESEL);

  ((CCTAApp*)APP)->AllEntityRhealys();

  // DM5480 train auto
  CString szClient = App().m_pDatabase->GetClientParameter();
  if ( szClient != SNCF_TRN )
    ((CButton *)GetDlgItem(IDC_CHECK_AUTO_ONLY))->ShowWindow (SW_HIDE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CEntityDlg::SetFcstDaySelectionButtons( BOOL bState )
{
  ((CButton *)GetDlgItem(IDC_FCST_DAY1))->SetCheck( bState );
  ((CButton *)GetDlgItem(IDC_FCST_DAY2))->SetCheck( bState );
  ((CButton *)GetDlgItem(IDC_FCST_DAY3))->SetCheck( bState );
  ((CButton *)GetDlgItem(IDC_FCST_DAY4))->SetCheck( bState );
  ((CButton *)GetDlgItem(IDC_FCST_DAY5))->SetCheck( bState );
  ((CButton *)GetDlgItem(IDC_FCST_DAY6))->SetCheck( bState );
  ((CButton *)GetDlgItem(IDC_FCST_DAY7))->SetCheck( bState );
}

void CEntityDlg::SetDOWSelectionButtons(BOOL bState) 
{
  ((CButton *)GetDlgItem(IDC_CHECK_MON))->SetCheck( bState );
  ((CButton *)GetDlgItem(IDC_CHECK_TUE))->SetCheck( bState );
  ((CButton *)GetDlgItem(IDC_CHECK_WED))->SetCheck( bState );
  ((CButton *)GetDlgItem(IDC_CHECK_THU))->SetCheck( bState );
  ((CButton *)GetDlgItem(IDC_CHECK_FRI))->SetCheck( bState );
  ((CButton *)GetDlgItem(IDC_CHECK_SAT))->SetCheck( bState );
  ((CButton *)GetDlgItem(IDC_CHECK_SUN))->SetCheck( bState );
  ((CButton *)GetDlgItem(IDC_CHECK_ALL))->SetCheck( bState );
}

void CEntityDlg::EnableDptDateEdit( BOOL bEnable )
{
  ((CEdit*)GetDlgItem(IDC_STARTDPTDATE))->EnableWindow( bEnable );
  ((CButton *)GetDlgItem(IDC_CHECK_MON))->EnableWindow( bEnable );
  ((CButton *)GetDlgItem(IDC_CHECK_TUE))->EnableWindow( bEnable );
  ((CButton *)GetDlgItem(IDC_CHECK_WED))->EnableWindow( bEnable );
  ((CButton *)GetDlgItem(IDC_CHECK_THU))->EnableWindow( bEnable );
  ((CButton *)GetDlgItem(IDC_CHECK_FRI))->EnableWindow( bEnable );
  ((CButton *)GetDlgItem(IDC_CHECK_SAT))->EnableWindow( bEnable );
  ((CButton *)GetDlgItem(IDC_CHECK_SUN))->EnableWindow( bEnable );
  ((CButton *)GetDlgItem(IDC_CHECK_ALL))->EnableWindow( bEnable );
  ((CEdit*)GetDlgItem(IDC_ENDDPTDATE))->EnableWindow( bEnable );
  ((CButton *)GetDlgItem(IDC_CHECK_END_DPT_DATE))->EnableWindow( bEnable );
}

void CEntityDlg::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_LISTVIEW *pNm = (NM_LISTVIEW *)pNMHDR;
  
  if( pNm->uChanged & LVIF_STATE ) {
    if( pNm->uNewState & LVIS_SELECTED ) 
    {
      m_wCurrentItem = ( m_EntityView.GetItemData(pNm->iItem) );
    }
  }
}

void CEntityDlg::OnFcstDay1() 
{
  SetDOWSelectionButtons( 0 );
  if (!m_bAutoTrain)
  {
    ((CButton *)GetDlgItem(IDC_TOUT_JMX))->ShowWindow (SW_SHOW);
    ((CButton *)GetDlgItem(IDC_SATC_JMX))->ShowWindow (SW_SHOW); 
  }
  EnableDptDateEdit( 0 );
  ((CButton *)GetDlgItem(IDC_DPT_DATE_RADIO))->SetCheck( 0 );
}

void CEntityDlg::OnFcstDay2() 
{
  SetDOWSelectionButtons( 0 );
  if (!m_bAutoTrain)
  {
    ((CButton *)GetDlgItem(IDC_TOUT_JMX))->ShowWindow (SW_SHOW);
    ((CButton *)GetDlgItem(IDC_SATC_JMX))->ShowWindow (SW_SHOW); 
  }
  EnableDptDateEdit( 0 );
  ((CButton *)GetDlgItem(IDC_DPT_DATE_RADIO))->SetCheck( 0 );
}

void CEntityDlg::OnFcstDay3() 
{
  SetDOWSelectionButtons( 0 );
  if (!m_bAutoTrain)
  {
    ((CButton *)GetDlgItem(IDC_TOUT_JMX))->ShowWindow (SW_SHOW);
    ((CButton *)GetDlgItem(IDC_SATC_JMX))->ShowWindow (SW_SHOW);
  }
  EnableDptDateEdit( 0 );
  ((CButton *)GetDlgItem(IDC_DPT_DATE_RADIO))->SetCheck( 0 );
}

void CEntityDlg::OnFcstDay4() 
{
  SetDOWSelectionButtons( 0 );
  if (!m_bAutoTrain)
  {
    ((CButton *)GetDlgItem(IDC_TOUT_JMX))->ShowWindow (SW_SHOW);
    ((CButton *)GetDlgItem(IDC_SATC_JMX))->ShowWindow (SW_SHOW);
  }
  EnableDptDateEdit( 0 );
  ((CButton *)GetDlgItem(IDC_DPT_DATE_RADIO))->SetCheck( 0 );
}

void CEntityDlg::OnFcstDay5() 
{
  SetDOWSelectionButtons( 0 );
  if (!m_bAutoTrain)
  {
    ((CButton *)GetDlgItem(IDC_TOUT_JMX))->ShowWindow (SW_SHOW);
    ((CButton *)GetDlgItem(IDC_SATC_JMX))->ShowWindow (SW_SHOW);
  }
  EnableDptDateEdit( 0 );
  ((CButton *)GetDlgItem(IDC_DPT_DATE_RADIO))->SetCheck( 0 );
}

void CEntityDlg::OnFcstDay6() 
{
  SetDOWSelectionButtons( 0 );
  if (!m_bAutoTrain)
  {
    ((CButton *)GetDlgItem(IDC_TOUT_JMX))->ShowWindow (SW_SHOW);
    ((CButton *)GetDlgItem(IDC_SATC_JMX))->ShowWindow (SW_SHOW);
  }
  EnableDptDateEdit( 0 );
  ((CButton *)GetDlgItem(IDC_DPT_DATE_RADIO))->SetCheck( 0 );
}

void CEntityDlg::OnFcstDay7() 
{
  SetDOWSelectionButtons( 0 );
  if (!m_bAutoTrain)
  {
    ((CButton *)GetDlgItem(IDC_TOUT_JMX))->ShowWindow (SW_SHOW);
    ((CButton *)GetDlgItem(IDC_SATC_JMX))->ShowWindow (SW_SHOW); 
  }
  EnableDptDateEdit( 0 );
  ((CButton *)GetDlgItem(IDC_DPT_DATE_RADIO))->SetCheck( 0 );
}

void CEntityDlg::OnDptDate() 
{
  // set the dpt date radio button...
  ((CButton *)GetDlgItem(IDC_DPT_DATE_RADIO))->SetCheck( 1 );
  SetFcstDaySelectionButtons( 0 );  // and, ensure all other radio buttons are de-selected
  
  // enable all of the dpt date edit windows, then interrogate the 'use end dpt date' checkbox
  EnableDptDateEdit( 1 );
  BOOL bEnable = ((CButton *)GetDlgItem(IDC_CHECK_END_DPT_DATE))->GetCheck();
  ((CEdit*)GetDlgItem(IDC_ENDDPTDATE))->EnableWindow( bEnable );
  ((CButton *)GetDlgItem(IDC_TOUT_JMX))->ShowWindow (SW_HIDE);
  ((CButton *)GetDlgItem(IDC_SATC_JMX))->ShowWindow (SW_HIDE);   
}

void CEntityDlg::OnCheckDow() 
{
  ((CButton *)GetDlgItem(IDC_CHECK_ALL))->SetCheck( 0 );
}

void CEntityDlg::OnCheckDowAll() 
{
  SetDOWSelectionButtons( ((CButton *)GetDlgItem(IDC_CHECK_ALL))->GetCheck() );
}

void CEntityDlg::ConstrainStartDptDate() 
{
  RWDate rwMinDptDate( m_lMinBegDptDate );  // date bounds check
  RWDate rwMaxDptDate( m_lMaxBegDptDate );
  COleDateTime tMin(rwMinDptDate.year(), rwMinDptDate.month(), rwMinDptDate.dayOfMonth(), 0, 0, 0);
  COleDateTime tMax(rwMaxDptDate.year(), rwMaxDptDate.month(), rwMaxDptDate.dayOfMonth(), 0, 0, 0);

  if (((CCTAApp*)APP)->GetRhealysFlag())
  {
    //tMin est = t jour - 3 mois
      CTime	        m_startdate;
      m_startdate = CTime::GetCurrentTime();
      RWDate m_rwBegDate(m_startdate.GetDay(), m_startdate.GetMonth(), m_startdate.GetYear());
	  unsigned long m_lDateDep;
      m_lDateDep = m_rwBegDate.julian ();
	  RWDate rwMinDptDateRhealys( m_lDateDep - 91 );
	  COleDateTime tMinRhealys(rwMinDptDateRhealys.year(), rwMinDptDateRhealys.month(), rwMinDptDateRhealys.dayOfMonth(), 0, 0, 0);
     if (m_tmStartDptDate < tMinRhealys)  // cannot be less than current year
          m_tmStartDptDate = tMinRhealys;
  }
  else
  {
	 if (m_tmStartDptDate < tMin)  // cannot be less than current year
       m_tmStartDptDate = tMin;
  }

     if (m_tmStartDptDate > tMax)  // cannot be highter than current year
       m_tmStartDptDate = tMax;
     
     m_ctlStartDptDate.SetTime(m_tmStartDptDate);  // just in case...set date in control
                                                /*
                                                CString szDate = m_tmStartDptDate.Format("%d/%m/%Y");
                                                CEdit* pEditDate = m_ctlStartDptDate.GetEditControl();
                                                pEditDate->SetWindowText(szDate);
												*/
}

void CEntityDlg::OnStartDptDateChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
  m_ctlStartDptDate.GetTime(m_tmStartDptDate);  // get the date from the control
  
  BOOL bNoConstrainDptDate = FALSE;
  if (! bNoConstrainDptDate)  // should we constrain the date to last auth fcst date (default is, we DO constrain to last auth fcst date) ?
  {
    ConstrainStartDptDate();  // NOT NO constrain, so constrain
  }
  if (m_tmStartDptDate > m_tmEndDptDate)
  {
    m_tmEndDptDate = m_tmStartDptDate;
    m_ctlEndDptDate.SetTime(m_tmEndDptDate);
  }
  *pResult = 0;
}

void CEntityDlg::ConstrainEndDptDate() 
{
  RWDate rwMinDptDate( m_lMinEndDptDate );  // date bounds check
  RWDate rwMaxDptDate( m_lMaxEndDptDate );
  COleDateTime tMin(rwMinDptDate.year(), rwMinDptDate.month(), rwMinDptDate.dayOfMonth(), 0, 0, 0);
  COleDateTime tMax(rwMaxDptDate.year(), rwMaxDptDate.month(), rwMaxDptDate.dayOfMonth(), 0, 0, 0);
  
  if (((CCTAApp*)APP)->GetRhealysFlag())
  {
    //tMin est = t jour - 3 mois
      CTime	        m_enddate;
      m_enddate = CTime::GetCurrentTime();
      RWDate m_rwEndDate(m_enddate.GetDay(), m_enddate.GetMonth(), m_enddate.GetYear());
	  unsigned long m_lDateEnd;
      m_lDateEnd = m_rwEndDate.julian ();
	  RWDate rwMinDptDateRhealys( m_lDateEnd - 91 );
	  COleDateTime tMinRhealys(rwMinDptDateRhealys.year(), rwMinDptDateRhealys.month(), rwMinDptDateRhealys.dayOfMonth(), 0, 0, 0);
     if (m_tmEndDptDate < tMinRhealys)  // cannot be less than current year
         m_tmEndDptDate = tMinRhealys;
  }
  else
  {
     if (m_tmEndDptDate < tMin)  // cannot be less than current year
         m_tmEndDptDate = tMin;
  }

  if (m_tmEndDptDate > tMax)  // cannot be highter than current year
      m_tmEndDptDate = tMax;
  
  m_ctlEndDptDate.SetTime(m_tmEndDptDate);  // just in case...set date in control
                                            /*
                                            CString szDate = m_tmEndDptDate.Format("%d/%m/%Y");
                                            CEdit* pEditDate = m_ctlEndDptDate.GetEditControl();
                                            pEditDate->SetWindowText(szDate);
  */
}

void CEntityDlg::OnEndDptDateChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
  m_ctlEndDptDate.GetTime(m_tmEndDptDate);  // get the date from the control
  
  BOOL bNoConstrainDptDate = FALSE;
  /*
  if ( m_bConstrainDptDateDlg )  // Did the user check the box which indicates we should not constrain the dpt date ?
    bNoConstrainDptDate = (((CButton *)GetDlgItem(IDC_CHECK_NO_DPT_DATE_CONSTRAINT))->GetCheck()) ? TRUE : FALSE;
  */
  if (! bNoConstrainDptDate)  // should we constrain the date to last auth fcst date (default is, we DO constrain to last auth fcst date) ?
  {
    ConstrainEndDptDate();  // NOT NO constrain, so constrain
  }
  if (m_tmEndDptDate < m_tmStartDptDate)
  {
    m_tmStartDptDate = m_tmEndDptDate;
    m_ctlStartDptDate.SetTime(m_tmStartDptDate);
  }
  *pResult = 0;
}

void CEntityDlg::OnCloseupDatetimepickerStart(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnStartDptDateChange(pNMHDR, pResult);
  
  *pResult = 0;
}

void CEntityDlg::OnCloseupDatetimepickerEnd(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnEndDptDateChange(pNMHDR, pResult);
  
  *pResult = 0;
}

void CEntityDlg::OnUseEndDptDate()
{
  if (((CButton *)GetDlgItem(IDC_CHECK_END_DPT_DATE))->GetCheck())
    OnDptDate();
  else
    ((CEdit*)GetDlgItem(IDC_ENDDPTDATE))->EnableWindow( FALSE );
}

void CEntityDlg::OnOK() 
{
  // Transfer values from controls to member variables
  UpdateData(TRUE);
  
  RWDate rwBegDptDate(m_tmStartDptDate.GetDay(), m_tmStartDptDate.GetMonth(), m_tmStartDptDate.GetYear());
  m_lBegDptDate = rwBegDptDate.julian();
  RWDate rwEndDptDate(m_tmEndDptDate.GetDay(), m_tmEndDptDate.GetMonth(), m_tmEndDptDate.GetYear());
  m_lEndDptDate = rwEndDptDate.julian();
  // ensure a valid dpt date was entered OR a fcst date radio button was checked
  // which button was checked?
  CString szDOWs;
  short sOffset = -1;
  m_pCTADoc->SetDecalBatch(0);

  //DM 5480
  if ( ((CButton *)GetDlgItem(IDC_CHECK_AUTO_ONLY))->GetCheck() == 1 )
	m_pCTADoc->SetTrainAutoOnly (TRUE);
  else
    m_pCTADoc->SetTrainAutoOnly (FALSE);

  if ( ((CButton *)GetDlgItem(IDC_DPT_DATE_RADIO))->GetCheck() == 1 )
  {
    CString szBuffer;
    GetWindowText(szBuffer);
    
    m_pCTADoc->SetBegDptDate(m_lBegDptDate);  // set the dpt date in the document
    
    // if we're not using the extended entity dialog (i.e., there is only dpt date specified), use beg date for both.
    if ( ((CButton *)GetDlgItem(IDC_CHECK_END_DPT_DATE))->GetCheck() )
      m_pCTADoc->SetEndDptDate(m_lEndDptDate);
    else
      m_pCTADoc->SetEndDptDate(m_lBegDptDate);
    
    // set the last auth fcst date to zero because we don't use it in the query
    // if the Constrain on Last Auth Fc Date checkbox was checked, we must constrain on Collect Date
    BOOL bUseLastAuthFcstDate = FALSE;

    if (bUseLastAuthFcstDate)
    {
      long lLastAuthFcDate = YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY)->DcDate();
      m_pCTADoc->SetLastAuthFcDate(lLastAuthFcDate);
      m_pCTADoc->SetConstrainLastAuthFcDate(1);
    }
    else
    {
      m_pCTADoc->SetLastAuthFcDate(JAN_1_2100);
      m_pCTADoc->SetConstrainLastAuthFcDate(0);
    }
    
    // set the wanted minimalRrdValue
    m_pCTADoc->SetMinRrdIndex (0);
 
    szDOWs.Empty();
    if ( ((CButton *)GetDlgItem(IDC_CHECK_SUN))->GetCheck() == 1 )
    {
      ( ((CCTAApp*)APP)->GetSundayIsDOW1() ) ? szDOWs += "1" : szDOWs += "7";
    }
    if ( ((CButton *)GetDlgItem(IDC_CHECK_MON))->GetCheck() == 1 )
    {
      if (! szDOWs.IsEmpty())
        szDOWs += "','";
      ( ((CCTAApp*)APP)->GetSundayIsDOW1() ) ? szDOWs += "2" : szDOWs += "1";
    }
    if ( ((CButton *)GetDlgItem(IDC_CHECK_TUE))->GetCheck() == 1 )
    {
      if (! szDOWs.IsEmpty())
        szDOWs += "','";
      ( ((CCTAApp*)APP)->GetSundayIsDOW1() ) ? szDOWs += "3" : szDOWs += "2";
    }
    if ( ((CButton *)GetDlgItem(IDC_CHECK_WED))->GetCheck() == 1 )
    {
      if (! szDOWs.IsEmpty())
        szDOWs += "','";
      ( ((CCTAApp*)APP)->GetSundayIsDOW1() ) ? szDOWs += "4" : szDOWs += "3";
    }
    if ( ((CButton *)GetDlgItem(IDC_CHECK_THU))->GetCheck() == 1 )
    {
      if (! szDOWs.IsEmpty())
        szDOWs += "','";
      ( ((CCTAApp*)APP)->GetSundayIsDOW1() ) ? szDOWs += "5" : szDOWs += "4";
    }
    if ( ((CButton *)GetDlgItem(IDC_CHECK_FRI))->GetCheck() == 1 )
    {
      if (! szDOWs.IsEmpty())
        szDOWs += "','";
      ( ((CCTAApp*)APP)->GetSundayIsDOW1() ) ? szDOWs += "6" : szDOWs += "5";
    }
    if ( ((CButton *)GetDlgItem(IDC_CHECK_SAT))->GetCheck() == 1 )
    {
      if (! szDOWs.IsEmpty())
        szDOWs += "','";
      ( ((CCTAApp*)APP)->GetSundayIsDOW1() ) ? szDOWs += "7" : szDOWs += "6";
    }
      
    if (szDOWs.IsEmpty())
    {
      CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DEPARTURE_DATE_ERROR);
      szText = szText + ": ";
      szText = szText + ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DAY_OF_WEEK);
      AfxMessageBox( szText, MB_ICONHAND | MB_APPLMODAL );
      return;
    }
    m_pCTADoc->SetDOWs(szDOWs);
  }
  else
  {
    // which button was checked?  set offset
    sOffset = -1;
    if ( ((CButton *)GetDlgItem(IDC_FCST_DAY1))->GetCheck() == 1 )
      sOffset = 0;
    else if ( ((CButton *)GetDlgItem(IDC_FCST_DAY2))->GetCheck() == 1 )
      sOffset = 1;
    else if ( ((CButton *)GetDlgItem(IDC_FCST_DAY3))->GetCheck() == 1 )
      sOffset = 2;
    else if ( ((CButton *)GetDlgItem(IDC_FCST_DAY4))->GetCheck() == 1 )
      sOffset = 3;
    else if ( ((CButton *)GetDlgItem(IDC_FCST_DAY5))->GetCheck() == 1 )
      sOffset = 4;
    else if ( ((CButton *)GetDlgItem(IDC_FCST_DAY6))->GetCheck() == 1 )
      sOffset = 5;
    else if ( ((CButton *)GetDlgItem(IDC_FCST_DAY7))->GetCheck() == 1 )
      sOffset = 6;
    
    if (sOffset < 0)  /* this should not be possible ... */
    {
      sOffset = 0;
      ((CButton *)GetDlgItem(IDC_FCST_DAY1))->SetCheck( 1 );
    }
	m_pCTADoc->SetDecalBatch(sOffset);
    
    // calculate the last auth fcst date from the dc date record set
    long lLastAuthFcDate = 
		YM_Set<YmIcDcDateDom>::FromKey(DCDATE_KEY)->DcDate(); //- sOffset;

	// ano 502061, on veut voir le j+1 du jour de collecte suppression du décalage
	// qui est géré par la requête via DecalBacth
    
    // and set it in the document
    m_pCTADoc->SetLastAuthFcDate(lLastAuthFcDate);
    m_pCTADoc->SetConstrainLastAuthFcDate(0);
    
    // set the dpt date to default date because we don't use it in the query
    m_pCTADoc->SetBegDptDate(JAN_1_1900);
    m_pCTADoc->SetEndDptDate(JAN_1_1900);
    
    // set the wanted minimalRrdValue
    m_pCTADoc->SetMinRrdIndex (1);
  }

  //  Note: for the following modifications, it was necessary to change YmIcTranList.h
  //  because we no longer get the current entity, rather build the list of entities here,
  //  then store them in the document (CtaDoc), 
  //  then copy them to the parameter in OnSelectQuery() in data.cpp
  //
  //  we now do this later, because we have to build the list of entities first.
  //	m_pCTADoc->m_pEntitySet->SetCurrent( m_wCurrentItem );
  
  // depending upon system parm, do we display the Entity or the Entity Description ?
  BOOL bDispEntDesc = 
    (BOOL) ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetEntityDescFlag();
  CString szClient = App().m_pDatabase->GetClientParameter();
  int iIndex = m_EntityView.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
  
  CString szText;
  CString szSaveText;
  CString szNonGagEntity;
  YmIcEntitiesDom* pEntity = NULL;
  iIndex = -1;
  do
  {
    iIndex = m_EntityView.GetNextItem(iIndex, LVNI_ALL | LVNI_SELECTED);
    if ( iIndex >= 0 )
    {
      pEntity = (YmIcEntitiesDom*)m_EntityView.GetItemData(iIndex);
      if ( ! szText.IsEmpty() )
      {
        szText += "','";
        szSaveText += "','";
      }
	  else
	  {
        szSaveText = "'";
	  }
      ( bDispEntDesc ) ? szSaveText += pEntity->Description() : szSaveText += pEntity->Entity();
      szText += pEntity->Entity();
	  if ( szClient != THALYS )
	  {
	    if (!m_pCTADoc->IsEntityGag(pEntity->Entity()))
	    {
	      szNonGagEntity += pEntity->Entity();
          szNonGagEntity += ' ';
	    }
	  }
    }
  } while ( iIndex >= 0 );
  szSaveText += "'";
  m_pCTADoc->SetEntities( szText );
  m_pCTADoc->SetSelectedEntities ( szText );
  m_pCTADoc->SetEntitiesDesc (szSaveText);
  if (( szClient != THALYS ) && !((CCTAApp*)APP)->GetRhealysFlag() && szNonGagEntity.GetLength())
  {
    CString ss = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_ENTITY_NONGAG);
	CString mess;
	mess.Format (ss, szNonGagEntity);
	AfxMessageBox( mess, MB_ICONHAND | MB_APPLMODAL );
  }
  if ( ((CButton *)GetDlgItem(IDC_TOUT_JMX))->GetCheck() == 1 )
	m_pCTADoc->SetAllJMX(TRUE);
  else
    m_pCTADoc->SetAllJMX(FALSE);

  if ( m_wCurrentItem == 0 )
    return;
  
  //m_pCTADoc->m_pEntitySet->SetCurrent( m_wCurrentItem );

  CString szProfileKey;
  BuildProfileKey(szProfileKey);
  
  APP->WriteProfileString(szProfileKey, szEntityConst, szSaveText);
  APP->WriteProfileString(szProfileKey, szDOWConst, m_pCTADoc->GetDOWs());
  APP->WriteProfileInt(szProfileKey, szBegDptDateConst, m_pCTADoc->GetBegDptDate());
  APP->WriteProfileInt(szProfileKey, szEndDptDateConst, m_pCTADoc->GetEndDptDate());
  APP->WriteProfileInt(szProfileKey, szUseEndDptDateConst, 
    ((CButton *)GetDlgItem(IDC_CHECK_END_DPT_DATE))->GetCheck());
  APP->WriteProfileInt(szProfileKey, szTrainAutoOnly, 
    ((CButton *)GetDlgItem(IDC_CHECK_AUTO_ONLY))->GetCheck());
  APP->WriteProfileInt(szProfileKey, szAllJMX, 
	((CButton *)GetDlgItem(IDC_TOUT_JMX))->GetCheck());
  APP->WriteProfileInt(szProfileKey, szLastFcstDateOffsetConst, sOffset);
  
  m_pCTADoc->m_bProcessTrainSupplement = TRUE;
  m_pCTADoc->m_bProcessTrainDeleted = TRUE;

  ((CMainFrame*)APP->GetMainWnd())->PostMessage(WM_LOAD_TRANCHES, 0, (LPARAM)m_wCurrentItem);

  YM_ETSLayoutDialog::OnOK();
  
  //m_pCTADoc->m_pEntitySet->SetCurrent( m_wCurrentItem ); 

  // Chargement de la Matrice CC8
  //((CCTAApp*)APP)->GetCoeffMatriceCC8();
}

void CEntityDlg::OnColumnclickEntityListview(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
  
  if( m_ViewAttrArray[ pNMListView->iSubItem ] & REVERSE_ORDER_FLAG )
    m_ViewAttrArray[ pNMListView->iSubItem ] &= ~REVERSE_ORDER_FLAG;
  else
    m_ViewAttrArray[ pNMListView->iSubItem ] |= REVERSE_ORDER_FLAG;
  
  m_EntityView.SortItems( Compare, m_ViewAttrArray[ pNMListView->iSubItem ] );

}

//void CEntityDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
void CEntityDlg::OnLButtonDblClk(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnOK();
}

