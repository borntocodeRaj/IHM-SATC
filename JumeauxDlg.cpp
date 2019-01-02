// DlgJumeaux.cpp : implementation file
//
#include "StdAfx.h"


#include "JumeauxDlg.h"

#include "ResaRailView.h"
#include "ResTracer.h"
#include "TrancheView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CTgvJumDlg::CTgvJumDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTgvJumDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTgvJumDlg)
	m_pct1 = 0;
	m_pct2 = 0;
	m_pct3 = 0;
	m_pct4 = 0;
	m_pct5 = 0;
	m_pct6 = 0;
	//}}AFX_DATA_INIT

    m_curTwinTgv = 0;
	m_LastRRDindex = 0;
	m_notified = TRUE;
	pSelectedTranche = NULL;
    m_resacmptkey = 0;
}

CTgvJumDlg::~CTgvJumDlg()
{
}

void CTgvJumDlg::OnNotify( YM_Observable* pObject )
{
  if (!::IsWindow(m_hWnd)) return;

  if( pObject->IsKindOf( RUNTIME_CLASS(YM_Msg) ) )
  {
	 ActivatePctEdit();
	 YM_Msg* pMsg = (YM_Msg*)pObject;
	 if (pMsg->GetNat() == YM_Msg::MSG_VNL)
	 {
	   CWnd* pwnd = GetDlgItem (IDOK);
	   pwnd->EnableWindow (TRUE);
	   if (pMsg->GetType() != YM_Msg::SOC_REP_OK)
	   {
		 m_vnlresult = "";
		 MessageBox (pMsg->GetMsg(), MB_APPLMODAL);
		 EndDialog (IDCANCEL); 
	   }
	   else
	   {
		 m_vnlresult = pMsg->GetMsg(); 

		 /* Analyse to put % tgv jum defined in the VNL */
		 YM_Vnl vnl;
		 int errorParsing = vnl.Parse(m_vnlresult);  // parse the VNL response
  	     // Anomalie 67837 : SATC 8.0.2 : consolidation du code
	     if (errorParsing == VNL_PARSE_ERROR)
		 {
			m_vnlresult = "";
			CString msgErrorParsing = 
				((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VNL_ERROR) + 
					": " + pMsg->GetMsg();
			AfxMessageBox(msgErrorParsing, MB_OK);
			pwnd = GetDlgItem (IDCANCEL);
		    pwnd->EnableWindow (TRUE);
			return;
		 }
		 YM_VnlLegIterator* LegIterator = vnl.CreateVnlLegIterator();
		 // ano 144727, 09/06/05 on recherche le % tgv jumeaux sur l'ensemble des tronçons
		 BOOL firstiter = TRUE;
		 for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
		 {
		   YM_VnlLeg* Leg = LegIterator->Current();
		   YM_VnlCmptIterator* CmptIterator = Leg->CreateVnlCmptIterator();
		   for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
		   {
		     YM_VnlCmpt* Cmpt = CmptIterator->Current();
		     SetPctVal (Cmpt->GetCmptId().GetAt(0), Cmpt->GetTgvJumPctg(), firstiter);
		   }
		   delete CmptIterator;
		   firstiter = FALSE;
		 }
		 delete LegIterator;
		 pwnd = GetDlgItem (IDCANCEL);
		 pwnd->EnableWindow (TRUE);
		 m_notified = TRUE;
	   }
	 }
	 ((CEdit *)GetDlgItem(IDC_EDIT_PC1))->SetFocus();
	 ((CEdit *)GetDlgItem(IDC_EDIT_PC1))->SetSel(0,-1);
  }
}

void CTgvJumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTgvJumDlg)
	DDX_Text(pDX, IDC_EDIT_PC1, m_pct1);
	DDV_MinMaxUInt(pDX, m_pct1, 0, 100);
	DDX_Text(pDX, IDC_EDIT_PC2, m_pct2);
	DDV_MinMaxUInt(pDX, m_pct2, 0, 100);
	DDX_Text(pDX, IDC_EDIT_PC3, m_pct3);
	DDV_MinMaxUInt(pDX, m_pct3, 0, 100);
	DDX_Text(pDX, IDC_EDIT_PC4, m_pct4);
	DDV_MinMaxUInt(pDX, m_pct4, 0, 100);
	DDX_Text(pDX, IDC_EDIT_PC5, m_pct5);
	DDV_MinMaxUInt(pDX, m_pct5, 0, 100);
	DDX_Text(pDX, IDC_EDIT_PC6, m_pct6);
	DDV_MinMaxUInt(pDX, m_pct6, 0, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTgvJumDlg, CDialog)
	//{{AFX_MSG_MAP(CTgvJumDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTgvJumDlg message handlers


BOOL CTgvJumDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWnd* pwnd;
	pwnd = GetDlgItem (IDC_EDIT_PC1);
	pwnd->EnableWindow (FALSE);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_EDIT_PC2);
	pwnd->EnableWindow (FALSE);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_EDIT_PC3);
	pwnd->EnableWindow (FALSE);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_EDIT_PC4);
	pwnd->EnableWindow (FALSE);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_EDIT_PC5);
	pwnd->EnableWindow (FALSE);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_EDIT_PC6);
	pwnd->EnableWindow (FALSE);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_PC1);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_PC2);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_PC3);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_PC4);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_PC5);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_PC6);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_CMPT1);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_CMPT2);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_CMPT3);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_CMPT4);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_CMPT5);
	pwnd->ShowWindow (SW_HIDE);
	pwnd = GetDlgItem (IDC_CMPT6);
	pwnd->ShowWindow (SW_HIDE);

  //YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);  
  YmIcTrancheLisDom* pTranche = pSelectedTranche;
  m_vnlmsg = "VNL";
  char buf[20];
  m_curTrancheNo = pTranche->TrancheNo();
  itoa (pTranche->TrancheNo(), buf, 10);
  pwnd = GetDlgItem (IDC_TRANCHE_NO);
  pwnd->SetWindowText (buf);
  pwnd->EnableWindow (FALSE);
  m_vnlmsg += buf;
  m_vnlmsg += '/';

  m_CurDptDate = pTranche->DptDate();
  RWDate d(pTranche->DptDateTr());
  CString s;
  s.Format( "%2.2u/%2.2u/%2.2u",  d.dayOfMonth(), d.month(), d.year() );
  pwnd = GetDlgItem (IDC_DPT_DATE);
  pwnd->SetWindowText (s);
  pwnd->EnableWindow (FALSE);
  m_vnlmsg += s.Left (2);
  m_vnlmsg += MONTH_LIST_ENGLISH.Mid( (d.month()-1)*3, 3 );

    m_LastRRDindex = pTranche->LastJX();
	itoa (pTranche->LastJX(), buf, 10);
	pwnd = GetDlgItem (IDC_RRD);
	pwnd->SetWindowText (buf);
	pwnd->EnableWindow (FALSE);

    m_curTwinTgv = pTranche->TgvJumeauxNo();
    if (!m_curTwinTgv)
	  buf[0] = 0;
	else
	  itoa (pTranche->TgvJumeauxNo(), buf, 10);
	/* La zone n° de TGV jumeaux n'est plus affichée.
	pwnd = GetDlgItem (IDC_CUR_TWIN);
	pwnd->SetWindowText (buf);
	pwnd->EnableWindow (FALSE);
	*/


  /// DM-7978 - CLE
  CYMVnlBouchon::getInstance()->setMonoTranche();
  /// DM-7978 - CLE

  // attempt to send the vnl command
  if (((CCTAApp*)APP)->GetResComm()->SendVnl (this, m_vnlmsg) == -1)
  { 
    // Communication socket not opened.
    CString ss;
    ss = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_SOCKETOPEN);
    MessageBox (ss, MB_APPLMODAL);
    EndDialog (IDCANCEL);
  }
  else
  {
    pwnd = GetDlgItem (IDOK);
    pwnd->EnableWindow (FALSE);
    m_notified = FALSE;
    pwnd = GetDlgItem (IDCANCEL);
    //pwnd->EnableWindow (FALSE);
  }

  return FALSE;
 // return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// Activation des zones d'édition de % TGV jumeaux, en fonction des compartiments
void CTgvJumDlg::ActivatePctEdit()
{
  YM_Iterator<YmIcResaCmptDom>* pResaCmptIterator =
    YM_Set<YmIcResaCmptDom>::FromKey(m_resacmptkey)->CreateIterator();
	YmIcResaCmptDom* pResaCmpt;

    int count = 0;
	int id_edit, id_cmpt, id_pc;
	id_edit = id_cmpt = id_pc = 0;
	if (pResaCmptIterator)
	{
	  for (pResaCmptIterator->First(); !pResaCmptIterator->Finished(); pResaCmptIterator->Next() )
	  {
	     if (pResaCmpt = pResaCmptIterator->Current())
       {
	       count++;
		     switch (count)
         {
		     case 1 : { id_edit = IDC_EDIT_PC1; 
				   id_cmpt = IDC_CMPT1; id_pc = IDC_PC1; break; }
		     case 2 : { id_edit = IDC_EDIT_PC2;
				   id_cmpt = IDC_CMPT2; id_pc = IDC_PC2; break; }
		     case 3 : { id_edit = IDC_EDIT_PC3;
				   id_cmpt = IDC_CMPT3; id_pc = IDC_PC3; break; }
		     case 4 : { id_edit = IDC_EDIT_PC4;
			       id_cmpt = IDC_CMPT4; id_pc = IDC_PC4; break; }
		     case 5 : { id_edit = IDC_EDIT_PC5;
			       id_cmpt = IDC_CMPT5; id_pc = IDC_PC5; break; }
		     case 6 : { id_edit = IDC_EDIT_PC6; 
			       id_cmpt = IDC_CMPT6; id_pc = IDC_PC6; break; }
		     default : break;
         }

		 CWnd* pwnd;
		 char buf[20];
		 pwnd = GetDlgItem (id_pc);
	     pwnd->ShowWindow (SW_SHOW);
		 pwnd = GetDlgItem (id_cmpt);
		 pwnd->SetWindowText (pResaCmpt->Cmpt());
		 pwnd->ShowWindow (SW_SHOW);
		 itoa ( pResaCmpt->TgvJumeauxPct(), buf, 10);
		 pwnd = GetDlgItem (id_edit);
		 pwnd->SetWindowText (buf);
		 pwnd->ShowWindow (SW_SHOW);
		 pwnd->EnableWindow (TRUE);
       }
    }
	delete pResaCmptIterator;
  }
  ((CEdit *)GetDlgItem(IDC_EDIT_PC1))->SetFocus();
  ((CEdit *)GetDlgItem(IDC_EDIT_PC1))->SetSel(0,-1);
}

// Retourne la string contenant la valeur de pourcentage TGV jumeaux
// choisi par l'utilisateur pour l'espace physique cmpt.
// Retourne une chaine vide, si cmpt n'est pas affiche.
CString CTgvJumDlg::PctVal (TCHAR cmpt)
{
  CString zzz, szcmpt;
  CWnd* pwnd;
  szcmpt = cmpt;

  pwnd = GetDlgItem (IDC_CMPT1);
  pwnd->GetWindowText (zzz);
  if (szcmpt.CompareNoCase (zzz) == 0)
  { 
    pwnd = GetDlgItem (IDC_EDIT_PC1);
	pwnd->GetWindowText (zzz);
	return zzz;
  }

  pwnd = GetDlgItem (IDC_CMPT2);
  pwnd->GetWindowText (zzz);
  if (szcmpt.CompareNoCase (zzz) == 0)
  { 
    pwnd = GetDlgItem (IDC_EDIT_PC2);
	pwnd->GetWindowText (zzz);
	return zzz;
  }

  pwnd = GetDlgItem (IDC_CMPT3);
  pwnd->GetWindowText (zzz);
  if (szcmpt.CompareNoCase (zzz) == 0)
  { 
    pwnd = GetDlgItem (IDC_EDIT_PC3);
	pwnd->GetWindowText (zzz);
	return zzz;
  }

  pwnd = GetDlgItem (IDC_CMPT4);
  pwnd->GetWindowText (zzz);
  if (szcmpt.CompareNoCase (zzz) == 0)
  { 
    pwnd = GetDlgItem (IDC_EDIT_PC4);
	pwnd->GetWindowText (zzz);
	return zzz;
  }

  pwnd = GetDlgItem (IDC_CMPT5);
  pwnd->GetWindowText (zzz);
  if (szcmpt.CompareNoCase (zzz) == 0)
  { 
    pwnd = GetDlgItem (IDC_EDIT_PC5);
	pwnd->GetWindowText (zzz);
	return zzz;
  }

  pwnd = GetDlgItem (IDC_CMPT6);
  pwnd->GetWindowText (zzz);
  if (szcmpt.CompareNoCase (zzz) == 0)
  { 
    pwnd = GetDlgItem (IDC_EDIT_PC6);
	pwnd->GetWindowText (zzz);
	return zzz;
  }

  zzz = "";
  return zzz;
}


// Affecte un % tgv jumeaux pour le compartiement correspondant
void CTgvJumDlg::SetPctVal (TCHAR cmpt, int pct, BOOL eraseprev)
{
  CString zzz, szcmpt;
  CWnd* pwnd;
  szcmpt = cmpt;
  char buf[5];
  itoa ( pct, buf, 10);

  pwnd = GetDlgItem (IDC_CMPT1);
  pwnd->GetWindowText (zzz);
  if (szcmpt.CompareNoCase (zzz) == 0)
  { 
    pwnd = GetDlgItem (IDC_EDIT_PC1);
	pwnd->GetWindowText (zzz);
	if ((atoi(zzz) < pct) || eraseprev)
	  pwnd->SetWindowText (buf);
	return;
  }

  pwnd = GetDlgItem (IDC_CMPT2);
  pwnd->GetWindowText (zzz);
  if (szcmpt.CompareNoCase (zzz) == 0)
  { 
    pwnd = GetDlgItem (IDC_EDIT_PC2);
	pwnd->GetWindowText (zzz);
	if ((atoi(zzz) < pct) || eraseprev)
	  pwnd->SetWindowText (buf);
	return;
  }

  pwnd = GetDlgItem (IDC_CMPT3);
  pwnd->GetWindowText (zzz);
  if (szcmpt.CompareNoCase (zzz) == 0)
  { 
    pwnd = GetDlgItem (IDC_EDIT_PC3);
	pwnd->GetWindowText (zzz);
	if ((atoi(zzz) < pct) || eraseprev)
	  pwnd->SetWindowText (buf);
	return;
  }

  pwnd = GetDlgItem (IDC_CMPT4);
  pwnd->GetWindowText (zzz);
  if (szcmpt.CompareNoCase (zzz) == 0)
  { 
    pwnd = GetDlgItem (IDC_EDIT_PC4);
	pwnd->GetWindowText (zzz);
	if ((atoi(zzz) < pct) || eraseprev)
	  pwnd->SetWindowText (buf);
	return;
  }

  pwnd = GetDlgItem (IDC_CMPT5);
  pwnd->GetWindowText (zzz);
  if (szcmpt.CompareNoCase (zzz) == 0)
  { 
    pwnd = GetDlgItem (IDC_EDIT_PC5);
	pwnd->GetWindowText (zzz);
	if ((atoi(zzz) < pct) || eraseprev)
	  pwnd->SetWindowText (buf);
	return;
  }

  pwnd = GetDlgItem (IDC_CMPT6);
  pwnd->GetWindowText (zzz);
  if (szcmpt.CompareNoCase (zzz) == 0)
  { 
    pwnd = GetDlgItem (IDC_EDIT_PC6);
	pwnd->GetWindowText (zzz);
	if ((atoi(zzz) < pct) || eraseprev)
	  pwnd->SetWindowText (buf);
	return;
  }
}

void CTgvJumDlg::OnOK() 
{
	if (!UpdateData(TRUE))
	  return;

	// Now, updating of the TGV_JUMEAUX_PCT in SC_CMPT_RRD (SC_TDLPR)
	YM_Iterator<YmIcResaCmptDom>* pResaCmptIterator =
	  YM_Set<YmIcResaCmptDom>::FromKey(m_resacmptkey)->CreateIterator();
	YmIcResaCmptDom* pResaCmpt;

    int count = 0;
	int valpct = 0;

	//ano 144723, 09/06/05, ajout d'un controle pour signaler qu'il y a à la fois des 0 et !0
	int nbzero = 0;
	int nbnonzero = 0;
	for (pResaCmptIterator->First(); !pResaCmptIterator->Finished();
		 pResaCmptIterator->Next() )
	{
      count++;
	  switch (count)
	  {
		case 1 : valpct = m_pct1; break;
		case 2 : valpct = m_pct2; break;
		case 3 : valpct = m_pct3; break;
		case 4 : valpct = m_pct4; break;
		case 5 : valpct = m_pct5; break;
		case 6 : valpct = m_pct6; break;
	  }
	  if (valpct)
		nbnonzero++;
	  else
		nbzero++;
	}
    if (nbnonzero && nbzero)
	{
	  if (AfxMessageBox(((CCTAApp*)APP)->GetResource()->LoadResString(IDS_JUM_AND_NOTJUM), 
                          MB_OKCANCEL) != IDOK)
	  {
		delete pResaCmptIterator;
		CDialog::OnCancel();
		return;
	  }
	} // fin correction ano 144723

	count = 0;
	for (pResaCmptIterator->First(); !pResaCmptIterator->Finished();
		 pResaCmptIterator->Next() )
	{
	   if (pResaCmpt = pResaCmptIterator->Current())
	   {
	       count++;
		   switch (count)
		   {
		     case 1 : valpct = m_pct1; break;
			 case 2 : valpct = m_pct2; break;
			 case 3 : valpct = m_pct3; break;
			 case 4 : valpct = m_pct4; break;
			 case 5 : valpct = m_pct5; break;
			 case 6 : valpct = m_pct6; break;
		   }
		   pResaCmpt->TgvJumeauxPct (valpct);
	   }
	}
	delete pResaCmptIterator;

	

    // Get the ResaRail View , in order to be able to set the
	// m_bTGVJumPctChanged for all the bucket
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	CResaRailView* pResaRailView = NULL;
	while (pos != NULL )
	{
	  CView* pView = pDoc->GetNextView (pos);
	  if ( pView->IsKindOf (RUNTIME_CLASS (CResaRailView) ) )
	  { 
		if (m_resacmptkey == RESACMPT_KEY)
		{
		  pResaRailView = (CResaRailView*)pView;
	      pos = NULL;
		}
	  }
	}

    // We set to TRUE the m_bTGVJumPctChanged boolean for all the bucket
	// (No set to verify if the values has changed or not !)
	// The real database updating, will be done in :
	// CResaRailView::UpdateCompartments()
	if (pResaRailView)
	{	
	  if( pResaRailView->GetCmptBktMap()->GetCount() != 0 )
	  {
	    CString sKey;
	    CResaCmptBkt* pCmptBkt;
        // iterate through the compartments
        pos = pResaRailView->GetCmptBktMap()->GetStartPosition();
        while( pos != NULL )
        {
		  pResaRailView->GetCmptBktMap()->GetNextAssoc( pos, sKey, pCmptBkt );
          pCmptBkt->m_bTGVJumPctChanged = TRUE;
		}
	  }
    }


	// Envoi des VNAU
	YM_Vnl vnl;
    //vnl.Parse(m_vnlresult);  // parse the VNL response
	int errorParsing = vnl.Parse(m_vnlresult);  // parse the VNL response
  	// Anomalie 67837 : SATC 8.0.2 : consolidation du code
	if (errorParsing == VNL_PARSE_ERROR)
	{
		CString msgErrorParsing = 
			((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VNL_ERROR) + 
				": " + 	m_vnlresult;
		m_vnlresult = "";
		AfxMessageBox(msgErrorParsing, MB_OK);
		return;
	}
    CString basevnau;
	BOOL YaUnPct = FALSE;
	//YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	YmIcTrancheLisDom* pTranche = pSelectedTranche;
	char bufntranche[20];
    itoa (pTranche->TrancheNo(), bufntranche, 10);
	RWDate d(pTranche->DptDateTr());
	RWDate demain (pTranche->DptDateTr() +1);
	CString sAujour, sDemain;
	sAujour.Format( "%2.2u", d.dayOfMonth());
	sDemain.Format( "%2.2u", demain.dayOfMonth());
	sAujour += MONTH_LIST_ENGLISH.Mid( (d.month()-1)*3, 3 );
	sDemain += MONTH_LIST_ENGLISH.Mid( (demain.month()-1)*3, 3 );

    YM_VnlLegIterator* LegIterator = vnl.CreateVnlLegIterator();
    for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
    {
      basevnau = "VNAU";
	  basevnau += bufntranche;
	  basevnau += '/';
	  YM_VnlLeg* Leg = LegIterator->Current();
      if (Leg->DepartsNextDay())
	    basevnau += sDemain;
	  else
	    basevnau += sAujour;
	  basevnau += Leg->GetLegOrig();
	  basevnau += Leg->GetLegDest();
	  basevnau += '/';

      YM_VnlCmptIterator* CmptIterator = Leg->CreateVnlCmptIterator();
      for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
      {
        CString vnaumsg = basevnau;
		YM_VnlCmpt* Cmpt = CmptIterator->Current();
		CString kkk = PctVal (Cmpt->GetCmptId().GetAt(0));
        kkk.TrimLeft();
        kkk.TrimRight();
		int ik = atoi (kkk);
		if (!ik)
		  kkk = '0';
		else
		{
		  kkk.Format ("%d",ik);
		  YaUnPct = TRUE;
		}
		if (kkk.GetLength())
		{
          //TRACE( "Cmpt: %s  Cap: %d  Bss: %d  Abs: %d\n", 
          //  Cmpt->GetCmptId(), Cmpt->GetCapacity(), 
          //  Cmpt->GetBaseSold(), Cmpt->GetAbsSold() );
		  vnaumsg +=kkk;
          YM_VnlBktIterator* BktIterator = Cmpt->CreateVnlBktIterator();
          for ( BktIterator->First(); ! BktIterator->Finished(); BktIterator->Next() )
          {
            YM_VnlBkt* Bkt = BktIterator->Current();
            // TRACE( "Bkt: %d  Auth: %d  Avail: %d  Bss: %d\n",
            //  Bkt->GetNestLevel(), Bkt->GetAuth(), 
            //  Bkt->GetAvail(), Bkt->GetBaseSold() );
			vnaumsg += '/';
			vnaumsg += Cmpt->GetCmptId().GetAt(0);
			CString sf;
			sf.Format ("%d-%d", Bkt->GetNestLevel(), Bkt->GetAuth());
			vnaumsg += sf;
	      }
		  delete BktIterator;
        }
		((CCTAApp*)APP)->GetResComm()->SendVnau (NULL, vnaumsg, FALSE, Leg->DepartsNextDay());
      }
	  delete CmptIterator;
	}
	delete LegIterator;
	
	// Mise a jour du status au niveau tranche indiquant l'envoi de % tgv jum  
	if (YaUnPct)
	{
	  if (pTranche->TgvJumPleinFlag() == 0)
		pTranche->TgvJumPleinFlag (1);
	  pTranche->EditStatus (pTranche->EditStatus() | EDIT_STATUS_TGVJUMPCTSET);
	}
	else
	{
	  pTranche->TgvJumPleinFlag (0);
	  pTranche->EditStatus (pTranche->EditStatus() & ~EDIT_STATUS_TGVJUMPCTSET);
	}

   	// Mise à jour de l'affichage
	// mis en commentaire par JMG le 17/10/01, car provoque l'envoi d'un VNAU
	//autorisations, si la fenetre Resarail a ete modifiee

    pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  	  CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) )
	  {
	    pView->Invalidate();
		break;
	  }
	} 

	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CTrainInfluentDlg dialog

CTrainInfluentDlg::CTrainInfluentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrainInfluentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrainInfluentDlg)
	m_NewTwinTrain.Empty();
//	m_NewTwinTrain = 0;
	m_CurTrancheNo = 0;
	m_szDptDate = _T("");
	//}}AFX_DATA_INIT

	m_NewTwinTranche = 0;

	m_pTrainList = new CTrainDateSet (App().m_pDatabase);
	m_pTrainList->SetKey (TGVJUM_KEY);
	m_pTrainList->AddObserver (this);
	m_notified = TRUE;

}

CTrainInfluentDlg::~CTrainInfluentDlg()
{
  delete m_pTrainList;
  m_pTrainList = NULL;
}

void CTrainInfluentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrainInfluentDlg)
	DDX_Text(pDX, IDC_CUR_TWIN, m_NewTwinTrain);
//	DDV_MinMaxUInt(pDX, m_NewTwinTrain, 0, 999999);
	DDX_Text(pDX, IDC_TRANCHE_NO, m_CurTrainNo);
	DDX_Text(pDX, IDC_DPT_DATE, m_szDptDate);
	//}}AFX_DATA_MAP

    if (pDX->m_bSaveAndValidate)
    {
	  if (m_CurTrainNo == m_NewTwinTrain)
	  {
	    pDX->PrepareEditCtrl (IDC_CUR_TWIN);
	    pDX->Fail();
	    return;
	  }
	}
	// On a affiche le numero de train au lieu du numero de tranche,
	// mais je n'ai pas changer le libelle du champ
}


BEGIN_MESSAGE_MAP(CTrainInfluentDlg, CDialog)
	//{{AFX_MSG_MAP(CTrainInfluentDlg)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_SUPPR_TWIN, OnSupprTwin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrainInfluentDlg message handlers

BOOL CTrainInfluentDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWnd* pwnd;

    //YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	YmIcTrancheLisDom* pTranche = pSelectedTranche;
    m_CurTrancheNo = pTranche->TrancheNo();
	m_NewTwinTrain = pTranche->TgvJumTrain();
	RWDate d(pTranche->DptDate());
	m_szDptDate.Format( "%2.2u/%2.2u/%2.2u", 
					 d.dayOfMonth(), d.month(), d.year() );
    
	m_NewTwinTranche = 0;
	m_CurTwinTranche = pTranche->TgvJumeauxNo();
	m_CurTrainNo = pTranche->TrainNo();
//	m_CurTrainNo = atoi(pTranche->TrainNo());
	m_CurDptDate = pTranche->DptDate();
    UpdateData (FALSE);

	pwnd = GetDlgItem (IDC_TRANCHE_NO);
	pwnd->EnableWindow (FALSE);

	pwnd = GetDlgItem (IDC_DPT_DATE);
	pwnd->EnableWindow (FALSE);

	pwnd = GetDlgItem (IDOK);
	pwnd->EnableWindow (FALSE);

    pwnd = GetDlgItem (IDCANCEL);
    pwnd->EnableWindow (FALSE);
	m_notified = FALSE;

    pwnd = GetDlgItem (IDC_SUPPR_TWIN);
	pwnd->EnableWindow (FALSE);
	
	m_pTrainList->Open();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTrainInfluentDlg::OnNotify( YM_Observable* pObject )
{
  if (!GetSafeHwnd())
    return;

  if( pObject->IsKindOf( RUNTIME_CLASS(YM_RecordSet) ) )
  {
	 YM_RecordSet* pRecordSet = (YM_RecordSet*)pObject;
	 YM_RecordSet* pTrainList =  m_pTrainList;
		  
	 if( (pRecordSet == pTrainList) && (pTrainList->IsValid()) )
	 {
	   YM_Iterator<YmIcTrancheLisDom>* pIterator =
		   YM_Set<YmIcTrancheLisDom>::FromKey (TGVJUM_KEY)->CreateIterator();
	   if ( pIterator->GetCount() )
	   {
	     CWnd* pwnd = GetDlgItem (IDOK);
	     pwnd->EnableWindow (TRUE);
	   }
	   delete pIterator;
	   pTrainList->Close();
	   
	   CWnd* pwnd = GetDlgItem (IDCANCEL);
       pwnd->EnableWindow (TRUE);

	   if (m_CurTwinTranche)
	   {
	     pwnd = GetDlgItem (IDC_SUPPR_TWIN);
	     pwnd->EnableWindow (TRUE);
	   }
	   m_notified = TRUE;		 
     } 
  }
}

void CTrainInfluentDlg::OnOK() 
{
	if (!UpdateData(TRUE))
	  return;
	//YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);  
    YmIcTrancheLisDom* pTranche = pSelectedTranche;
//	if (!m_NewTwinTrain)
	if (m_NewTwinTrain.IsEmpty())
	{ 
	  UpdateTwinTranche(FALSE);
      pTranche->TgvJumeauxNo(0);
	  pTranche->DmdFcPct(100);
	}

    // Looking for the given train number in the list of the train having the
	// same departure date.
    YmIcTrancheLisDom* pTwins;
    YM_Iterator<YmIcTrancheLisDom>* pIterator =
		YM_Set<YmIcTrancheLisDom>::FromKey (TGVJUM_KEY)->CreateIterator();
    for (pIterator->First(); !pIterator->Finished(); pIterator->Next())
    {	
      pTwins = pIterator->Current();
//	  if ( pTwins->TrainNo() == (long)m_NewTwinTrain)
	  if ( pTwins->TrainNo() == m_NewTwinTrain)
	  {
	    m_NewTwinTranche =  pTwins->TrancheNo();
		break;
	  }    
    }
    delete pIterator;

    if (m_NewTwinTranche == 0)
	{
	  CString s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BAD_JUMTRAIN);
      MessageBox (s);
	  return;
	}

	if (m_CurTwinTranche != m_NewTwinTranche)
	{ 
	  if (m_CurTwinTranche)
	  { // there is already a TGV JUMEAUX defined for the current tranche
	    UpdateTwinTranche(FALSE);
	  }
	  UpdateTwinTranche(TRUE);
	}
   
	pTranche->TgvJumeauxNo(m_NewTwinTranche);
	pTranche->TgvJumTrain(m_NewTwinTrain);
	pTranche->DmdFcPct(50);

    CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
    POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  	  CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) )
	  {
	    pView->Invalidate();
		break;
	  }
	} 

	CDialog::OnOK();
}

void CTrainInfluentDlg::UpdateTwinTranche(BOOL SetIt)
{ // Set values of TGV_JUMEAUX_NO and DMD_FC_PCT in SC_TRANCHES,
  // If SetIt is false, we remove Twin Tgv info from m_CurTwinTranche
  // Else we set them in m_NewTwinTranche
  // for TRANCHE_NO = "WorkingTranche" and DPT_DATE = m_CurDptDate;
  // If SetIt is FALSE, we set to 0 TGV_JUMEAUX_NO and DMD_FC_PCT.
  // else we set it to m_CurTrancheNo and 50;
  // This is the miror effect of the action done on the current tranche record.
  
    UINT WorkingTranche = (SetIt) ? m_NewTwinTranche : m_CurTwinTranche;

    // First we check in the Tranche List we have in the Record Set if this
    // twin TGV is already loaded.
    // If yes, we will update its values in memory.
    YM_Iterator<YmIcTrancheLisDom>* pTrancheIterator =
	    YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY)->CreateIterator();
    YmIcTrancheLisDom* pTranche;

    if (pTrancheIterator)
    {
      for (pTrancheIterator->First(); !pTrancheIterator->Finished();
           pTrancheIterator->Next() )
      {
        if ( (pTranche = pTrancheIterator->Current()) &&
		     (pTranche->TrancheNo() == (long)WorkingTranche) &&
             (pTranche->DptDate() == m_CurDptDate) )
	    { 
		  pTranche->TgvJumeauxNo(SetIt ? m_CurTrancheNo : 0);
		  pTranche->TgvJumTrain(SetIt ? m_CurTrainNo : "");
//		  pTranche->TgvJumTrain(SetIt ? m_CurTrainNo : 0);
		  pTranche->DmdFcPct(SetIt ? 50 : 100);
		  break;
	    }
	  }
	  delete pTrancheIterator;
    }

    // Now we send an update query on SC_TD
    YM_Query* pQueryUpdTwin = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
    YmIcTrancheLisDom TgvJumDom ;
    // NB, the AUTODELETE mechanism will delete the objects created above.
    TgvJumDom.TrancheNo(WorkingTranche);
    TgvJumDom.DptDateBeg(m_CurDptDate);
	TgvJumDom.DptDateEnd(m_CurDptDate);
	TgvJumDom.DayOfWeek("1','2','3','4','5','6','7");
    TgvJumDom.TgvJumeauxNo (SetIt ? m_CurTrancheNo : 0);
	YM_Database* pDataBase = ((CCTAApp*)APP)->m_pDatabase;
	pQueryUpdTwin->SetDomain (&TgvJumDom);
	pDataBase->Transact(pQueryUpdTwin, IDS_SQL_UPDATE_TGV_JUM);
	delete pQueryUpdTwin;
}

void CTrainInfluentDlg::OnSupprTwin() 
{
	UpdateTwinTranche(FALSE);
	//YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY); 
	YmIcTrancheLisDom* pTranche = pSelectedTranche;
    pTranche->TgvJumeauxNo(0);
	pTranche->TgvJumTrain("");
//	pTranche->TgvJumTrain(0);
	pTranche->DmdFcPct(100);
    
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
    POSITION pos = pDoc->GetFirstViewPosition();
  	while( pos != NULL ) 
    {
  	  CView* pView = pDoc->GetNextView( pos ); 
	  if( pView->IsKindOf( RUNTIME_CLASS(CTrancheView) ) )
	  {
	    pView->Invalidate();
		break;
	  }
	} 

    OnCancel();
}
