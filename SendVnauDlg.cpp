// SendVnauDlg.cpp : implementation file
//
#include "StdAfx.h"

#include "VNAXwnd.h"
#include "ResTracer.h"
#include "SendVnauDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVnauAuthoLeg

CVnauAuthoLeg::CVnauAuthoLeg(CString orig, CString dest, BOOL dptnextday)
{
  m_legorig = orig;
  m_legdest = dest;
  m_dptnextday = dptnextday;
}

CVnauAuthoLeg::~CVnauAuthoLeg()
{
}

/////////////////////////////////////////////////////////////////////////////
// CVnasAuthoLeg

CVnasAuthoLeg::CVnasAuthoLeg(CString orig, CString dest, BOOL dptnextday)
{
  m_legorig = orig;
  m_legdest = dest;
  m_dptnextday = dptnextday;
}

CVnasAuthoLeg::~CVnasAuthoLeg()
{
}


////////////////////////////////////////////////////////////////////
// CVnauByLegAuthMap

CVnauByLegAuthMap::CVnauByLegAuthMap()
{
}

CVnauByLegAuthMap::~CVnauByLegAuthMap()
{
  Empty();
}

void CVnauByLegAuthMap::Empty()
{
  POSITION pos = GetStartPosition();
  while( pos != NULL )
  {
    CString sLeg;
    CVnauAuthoLeg* pAuthoLeg;

    GetNextAssoc( pos, sLeg, pAuthoLeg );
    if (pAuthoLeg)
	  delete pAuthoLeg;
    pAuthoLeg = NULL;
  }
  RemoveAll();
}

////////////////////////////////////////////////////////////////////
// CVnasByLegAuthMap

CVnasByLegAuthMap::CVnasByLegAuthMap()
{
}

CVnasByLegAuthMap::~CVnasByLegAuthMap()
{
  Empty();
}

void CVnasByLegAuthMap::Empty()
{
  POSITION pos = GetStartPosition();
  while( pos != NULL )
  {
    CString sLeg;
    CVnasAuthoLeg* pAuthoLeg;

    GetNextAssoc( pos, sLeg, pAuthoLeg );
    if (pAuthoLeg)
	  delete pAuthoLeg;
    pAuthoLeg = NULL;
  }
  RemoveAll();
}



/////////////////////////////////////////////////////////////////////////////
// CSendVnauDlg dialog

void CSendVnauDlg::Create()
{
  if ( ! YM_ETSLayoutDialog::Create(CSendVnauDlg::IDD, AfxGetMainWnd() ) )
    AfxGetMainWnd()->MessageBox("Unable to create status box", 
                        AfxGetAppName(), 
                        MB_ICONERROR | MB_APPLMODAL);
  CWnd* ResultWnd = GetDlgItem (IDC_EDIT_AUTHORIZATIONS);

  ResultWnd->SetFont (((CCTAApp*)APP)->GetDocument()->GetFixedFont(NULL));
  EnableVnau (FALSE);
  m_natureRhealys = 0;
}

void CSendVnauDlg::Changefont()
{
  if (!IsWindow(m_hWnd))
    return;
  CWnd* ResultWnd = GetDlgItem (IDC_EDIT_AUTHORIZATIONS);
  if (ResultWnd)
    ResultWnd->SetFont (((CCTAApp*)APP)->GetDocument()->GetFixedFont(NULL));
}

void CSendVnauDlg::Show()
{
	if( !GetSafeHwnd() )
		Create();

  ShowWindow(SW_SHOW);
}

void CSendVnauDlg::Hide()
{
  ShowWindow(SW_HIDE);
}

CSendVnauDlg::CSendVnauDlg()
	: YM_ETSLayoutDialog(CSendVnauDlg::IDD)
//	: YM_ETSLayoutDialog()
{
   	//{{AFX_DATA_INIT(CSendVnauDlg)
	m_month = _T("");
	m_vnlresult = _T("");
	m_day = 1;
	m_dest = _T("");
	m_orig = _T("");
	m_tranche = 0;
	m_editvnau = _T("");
	m_editOD = _T("");
	m_fixedvnau = _T("");
	m_listvnau = _T("");
	m_bModeVNAS = FALSE;
	//}}AFX_DATA_INIT

	m_fvlnrep = "";
	m_nbWaitedVnau = 0;
    m_strRegStore = "SendVnauDlg";
	m_bTrainAuto = FALSE;
	m_capaTrainAuto = 0;
}


void CSendVnauDlg::DoDataExchange(CDataExchange* pDX)
{
	YM_ETSLayoutDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendVnauDlg)
	DDX_CBString(pDX, IDC_COMBO_MONTH, m_month);
	DDV_MaxChars(pDX, m_month, 3);
	DDX_Text(pDX, IDC_EDIT_AUTHORIZATIONS, m_vnlresult);
	DDX_Text(pDX, IDC_EDIT_DEST, m_dest);
	DDV_MaxChars(pDX, m_dest, STATION_SIZE);
	DDX_Text(pDX, IDC_EDIT_ORIG, m_orig);
	DDV_MaxChars(pDX, m_orig, STATION_SIZE);
	DDX_Text(pDX, IDC_EDIT_TRANCHE, m_tranche);
	DDV_MinMaxInt(pDX, m_tranche, 1, 99999);
	DDX_Text(pDX, IDC_EDIT_VNAU, m_editvnau);
	DDX_Text(pDX, IDC_EDIT_OD, m_editOD);
	DDX_Text(pDX, IDC_FIXED_VNAU, m_fixedvnau);
	DDX_LBString(pDX, IDC_LIST_VNAU, m_listvnau);
	DDX_Text(pDX, IDC_EDIT_DAY, m_day);
	DDV_MaxChars(pDX, (char)m_day, 2);
	DDV_MinMaxInt(pDX, m_day, 1, 31);
	DDX_Check(pDX, IDC_MODE_VNAS, m_bModeVNAS);
	//}}AFX_DATA_MAP

	// Laisser le traitement du jour en dernier, pour garder IDC_EDIT_DAY
	// comme edit control courant pour le test suivant.

    if (!pDX->m_bSaveAndValidate)
	{
	  if (m_tranche == 0)
	  {
	    CWnd *pCtrl = GetDlgItem (IDC_EDIT_TRANCHE);
	    pCtrl->SetWindowText ("");
	    pCtrl = GetDlgItem (IDC_EDIT_DAY);
	    pCtrl->SetWindowText ("");
	  }
	  return;
	}
    RWDate today;
	int year = today.year();
	if (today.month() > 2)
	  year++;
	if (
	    ((m_day == 31) &&
	    ((m_month == "FEB") || (m_month == "APR") || (m_month == "JUN") ||
		 (m_month == "SEP") || (m_month == "NOV")))
		||
		((m_day == 30) && (m_month == "FEB"))
		||
		((m_day == 29) && (m_month == "FEB") && (year % 4 != 0))
		||
		(m_month == "")
	   )
	{ 
	  CString s;
      s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_WRONG_DATE);
	  MessageBox (s, MB_APPLMODAL);
	  if (m_month == "")
	    pDX->PrepareCtrl (IDC_COMBO_MONTH);
	  pDX->Fail();
	}
	int lo = m_orig.GetLength();
	int ld = m_dest.GetLength();
	if ((lo && (lo != STATION_SIZE)) || (ld && (ld != STATION_SIZE)) || (lo && !ld) || (ld && !lo))
	{ 
	  CString s;
      s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_SHORT_STATIONCODE);
	  MessageBox (s, MB_APPLMODAL);
	  if (lo && (lo != STATION_SIZE))
	    pDX->PrepareEditCtrl (IDC_EDIT_ORIG);
	  else
	    pDX->PrepareEditCtrl (IDC_EDIT_DEST);
	  pDX->Fail();
	}
}


BEGIN_MESSAGE_MAP(CSendVnauDlg, YM_ETSLayoutDialog)
	//{{AFX_MSG_MAP(CSendVnauDlg)
	ON_BN_CLICKED(IDC_GETVNL, OnGetvnl)
	ON_BN_CLICKED(IDC_BUILD_VNAU, OnBuildVnau)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendVnauDlg message handlers



void CSendVnauDlg::OnGetvnl() 
{
  if (!UpdateData(TRUE))
    return;

  CString msg = "VNL";
  char tamp[10];
  itoa (m_tranche, tamp, 10);
  msg += tamp;
  msg += '/';
  itoa (m_day, tamp, 10);
  if (m_day < 10)
    msg += '0';		// date sur 2 digits
  msg += tamp;
  msg += m_month;
  msg += m_orig;
  msg += m_dest;
  msg += "/SC";


  m_vnlresult = "";
  m_fvlnrep = "";
  UpdateData (FALSE);

	/// DM-7978 - NCH (Bouchon problem)
  EnableEdit (FALSE);
  EnableVnau (FALSE); 

  // attempt to send the vnl command
  

  /// DM-7978 - CLE
  CYMVnlBouchon::getInstance()->setMonoTranche();
  /// DM-7978 - CLE

  if (((CCTAApp*)APP)->GetResComm()->SendVnl (this, msg) == -1)
  { 
    // Communication socket not opened.
    CString s;
    s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_SOCKETOPEN);
    MessageBox (s, MB_APPLMODAL);
    return;
  }

  //EnableEdit (FALSE);
  //EnableVnau (FALSE);
	/// DM-7978 - NCH 
}

void CSendVnauDlg::EnableEdit (BOOL enabled)
{
  	CWnd* pwnd;
	pwnd = GetDlgItem (IDC_EDIT_TRANCHE);
	pwnd->EnableWindow (enabled);
	pwnd = GetDlgItem (IDC_EDIT_DAY);
	pwnd->EnableWindow (enabled);
	pwnd = GetDlgItem (IDC_COMBO_MONTH);
	pwnd->EnableWindow (enabled);
	pwnd = GetDlgItem (IDC_EDIT_ORIG);
	pwnd->EnableWindow (enabled);
	pwnd = GetDlgItem (IDC_EDIT_DEST);
	pwnd->EnableWindow (enabled);
	pwnd = GetDlgItem (IDC_GETVNL);
	pwnd->EnableWindow (enabled);

}

void CSendVnauDlg::EnableVnau (BOOL enabled)
{
  	CWnd* pwnd;
	m_fixedvnau = "";
	if (!enabled)
	{ m_goodtranche = 0;
	  m_goodday = 0;
	  m_goodmonth = "";
	  m_editvnau = "";
	  m_editOD = "";
	  m_jumdata = "";
	  m_NewAuth.Empty();
	  CListBox* plistb;
      plistb = (CListBox*) GetDlgItem (IDC_LIST_VNAU);
      plistb->ResetContent();
	  m_CrossMidnightTable.RemoveAll();
	}
	else
	{ m_goodtranche = m_tranche;
	  m_goodday = m_day;
	  m_goodmonth = m_month;
	  m_editOD = m_orig;
	  m_editOD += m_dest;
	  char tamp[10];
      itoa (m_goodtranche, tamp, 10);
      m_fixedvnau += tamp;
      m_fixedvnau += '/';
      itoa (m_goodday, tamp, 10);
      if (m_goodday < 10)
        m_fixedvnau += '0';		// date sur 2 digits
      m_fixedvnau += tamp;
      m_fixedvnau += m_goodmonth;
	  m_fixedvnau += '/';
	}
	UpdateData (FALSE);
	pwnd = GetDlgItem (IDC_EDIT_VNAU);
	pwnd->EnableWindow (enabled);
	pwnd = GetDlgItem (IDC_EDIT_OD);
	pwnd->EnableWindow (enabled && !m_bTrainAuto);
	pwnd = GetDlgItem (IDC_BUILD_VNAU);
	pwnd->EnableWindow (enabled);
	pwnd = GetDlgItem (IDOK); // Envoi des VNAU
	pwnd->EnableWindow (enabled);
}


void CSendVnauDlg::OnNotify( YM_Observable* pObject )
{
	if( pObject->IsKindOf( RUNTIME_CLASS(YM_Msg) ) )
	{
		YM_Msg* pMsg = (YM_Msg*)pObject;

		if (pMsg->GetNat() == YM_Msg::MSG_PROVNAU)
		{
			m_nbWaitedVnau--;
			if (!m_nbWaitedVnau)
			{ //CWnd* pwnd;
				//pwnd = GetDlgItem (IDCANCEL);
				//pwnd->EnableWindow (TRUE);
				EnableVnau (TRUE);
				EnableEdit (TRUE);
				OnGetvnl();
			}
		}
		else if (pMsg->GetNat() == YM_Msg::MSG_VNL)
		{
			EnableEdit (TRUE);
			if (pMsg->GetType() != YM_Msg::SOC_REP_OK)
			{
				MessageBox (pMsg->GetMsg(), MB_APPLMODAL);
				m_vnlresult = "";
				m_fvlnrep = "";
				UpdateData (FALSE);
				m_LegAuthList.Empty();
				return;
			}

			YM_Vnl vnl;
			//vnl.Parse(pMsg->GetMsg());
			int errorParsing = vnl.Parse(pMsg->GetMsg());  // parse the VNL response
  			// Anomalie 67837 : SATC 8.0.2 : consolidation du code
			if (errorParsing == VNL_PARSE_ERROR)
			{
				CString msgErrorParsing = 
					((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VNL_ERROR) + 
						": " + pMsg->GetMsg();
				AfxMessageBox(msgErrorParsing, MB_OK);
				return;
			}

			// JMG debug.
			/*if (vnl.GetNatTranche (YM_Vnl::NOT_RHEALYS) % 2 == 0)
			vnl.DummySci(5);*/	
			m_bTrainAuto = FALSE;
			m_capaTrainAuto = 0;
			m_ValidCmpt = "";
			bool bYaSci = false;

			if (vnl.GetNatTranche(YM_Vnl::DB) == vnl.GetNatTranche (YM_Vnl::NOT_RHEALYS))
			{
				CString s;
				s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_FORBIDEN_ON_DB);
				AfxGetMainWnd()->MessageBox(s, AfxGetAppName(), MB_ICONERROR | MB_APPLMODAL);
				m_fvlnrep = pMsg->GetMsg();
				m_vnlresult = pMsg->GetMsg();
				MakeGoodLine (m_vnlresult);
				EnableVnau (FALSE);
				return;
			}

			YM_VnlLegIterator* LegIterator = vnl.CreateVnlLegIterator();

			// Si tous les legs indiquent un DepartsNextDay, il faut supprimer cette information, car
			// le VNL a déjà été fait avec la date du lendemain. (cas de VBNL envoyé sur une OD partielle
			// Dans le cas VNAS, le VNL est envoyé sans OD.
			BOOL allnextday = true;
			for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
			{
				YM_VnlLeg* Leg = LegIterator->Current();
				if (!Leg->DepartsNextDay()) allnextday = false;
			}

			int iLeg = 0;
			for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
			{
				YM_VnlLeg* Leg = LegIterator->Current();
				//TRACE( "Orig: %s  Dest: %s  Departs Next Day?: %d\n", 
				//  Leg->GetLegOrig(), Leg->GetLegDest(), Leg->DepartsNextDay() );
				CVnauAuthoLeg* p_autholeg = new CVnauAuthoLeg (Leg->GetLegOrig(), Leg->GetLegDest(),
					allnextday ? FALSE : Leg->DepartsNextDay() );
				m_LegAuthList.SetAt (Leg->GetLegOrig(), p_autholeg);
				iLeg++;
				YM_VnlCmptIterator* CmptIterator = Leg->CreateVnlCmptIterator();
				for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
				{
					YM_VnlCmpt* Cmpt = CmptIterator->Current();
					if (Cmpt->YaSci())
						bYaSci = true;
					p_autholeg->AddCmpt (Cmpt->GetCmptId().GetAt(0), (WORD)Cmpt->GetCapacity());
					if (Cmpt->GetCmptId() == "O")		//DM 5480 train auto 
					{
						m_bTrainAuto = TRUE;
						m_capaTrainAuto = Cmpt->GetCapacity();
					}
					if (iLeg == 1)
						m_ValidCmpt += Cmpt->GetCmptId();	  

					YM_VnlBktIterator* BktIterator = Cmpt->CreateVnlBktIterator();
					for ( BktIterator->First(); ! BktIterator->Finished(); BktIterator->Next() )
					{
						YM_VnlBkt* Bkt = BktIterator->Current();
						p_autholeg->AddAutho (Cmpt->GetCmptId().GetAt(0),
							(WORD)Bkt->GetNestLevel(),
							Bkt->GetAuth());
					}
					delete BktIterator;
				}
				delete CmptIterator;
			}
			delete LegIterator;
			if ((pMsg->GetInitial().GetLength() > 17) && m_bTrainAuto)
			{// le VNL a été envoyé avec une OD, alors qu'il s'agit d'un train AUTO
				m_orig = "";
				m_dest = "";
				UpdateData (FALSE);
				EnableVnau(FALSE);
				OnGetvnl();
			}
			m_fvlnrep = pMsg->GetMsg();
			m_vnlresult = pMsg->GetMsg();
			MakeGoodLine (m_vnlresult);
			UpdateData (FALSE);
			EnableVnau (TRUE);

			((CButton*)GetDlgItem (IDC_MODE_VNAS))->EnableWindow(bYaSci);
			if (!bYaSci)
				((CButton*)GetDlgItem (IDC_MODE_VNAS))->SetCheck (FALSE);
		}
        // Les anomalies 75842 et 76034 sont identiques : plantage sur VNAU Mode Pro.
        // Enlever le delete (31/12/2015)
        //delete pMsg; // NPI - Memory leak
	}
}

//DM4802, ajout d'un contrôle sur la capacite
// coderr sert a savoir si cette erreur a déja ete signale
void CSendVnauDlg::ControlValue (CString& szErrAuthCap, int valeur, int capacity)
{
  CString szMsg;
  if (valeur > capacity * 1.12) 
    szMsg.Format( IDS_AUTH_CAP_RANGE_WARNING, (int)valeur, 112, (int)( capacity * 1.12 ) );
  else if ((valeur < capacity) && (!m_natureRhealys))
    szMsg = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_OVBLEVEL_RANGE_WARN);
				  
  if (!szMsg.IsEmpty())
  {
    if (szErrAuthCap.IsEmpty())
	  szErrAuthCap = szMsg;
	else if (szErrAuthCap.Find (szMsg, 0) == -1)
	{
	  szErrAuthCap += '\n';
	  szErrAuthCap += szMsg;
	}
  }
}

void CSendVnauDlg::BuildVnas()
{
  m_natureRhealys = 0;
  if (!CheckScgSci())
  {
    m_NewSci.Empty();
	MessageBeep (MB_ICONEXCLAMATION);
    return;
  }

  CListBox* plistb;
  plistb = (CListBox*) GetDlgItem (IDC_LIST_VNAU);
  //plistb->SetHorizontalExtent (800);
  plistb->ResetContent();
  m_CrossMidnightTable.RemoveAll();

  CString fullmess;
  CString basemsg = "VNAS";
  CString basedemain;
  CString jjMMM;
  RW4Byte jule;
  char tamp[10];
  itoa (m_goodtranche, tamp, 10);
  basemsg += tamp;
  basemsg += '/';
  jjMMM = "";
  itoa (m_goodday, tamp, 10);
  if (m_goodday < 10)
    jjMMM += '0';		// date sur 2 digits
  jjMMM += tamp;
  jjMMM += m_goodmonth;
  basedemain = basemsg;
  basemsg += jjMMM;
  jule = GiveJulian (jjMMM);
  jule++;
  jjMMM = MakeStrDate (jule);
  basedemain += jjMMM;

  CString lego, legd, msg, zMsg;
  lego = m_editOD.Left(STATION_SIZE);
  legd = m_editOD.Right(STATION_SIZE);

  YM_Vnl vnl;
  //vnl.Parse(m_fvlnrep);
  int errorParsing = vnl.Parse(m_fvlnrep);  // parse the VNL response
  // Anomalie 67837 : SATC 8.0.2 : consolidation du code
  if (errorParsing == VNL_PARSE_ERROR)
  {
		CString msgErrorParsing = 
			((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VNL_ERROR) + 
				": " + m_fvlnrep;
		AfxMessageBox(msgErrorParsing, MB_OK);
		return;
  }

  // JMG debug.
  /*if (vnl.GetNatTranche (YM_Vnl::NOT_RHEALYS) % 2 == 0)
     vnl.DummySci(5);*/

  BOOL LegOk = FALSE;
  YM_VnlLegIterator* LegIterator = vnl.CreateVnlLegIterator();
  for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
  {
    YM_VnlLeg* Leg = LegIterator->Current();
	if (Leg->GetLegOrig() == lego)
	  LegOk = TRUE;
	if (LegOk)
	{
	  // initialisation de m_NewSci avec les valeurs du tronçon
	  InitNewSci (&vnl, Leg->GetLegOrig());
	  // modification de m_NewSci avec les valeurs saisies.
	  AnalyzeFullScgi ();
	  // Vérification

	  if (!m_NewSci.VerifyScgAuth(msg))
	  {
		zMsg.Format ("%s - %s : %s", Leg->GetLegOrig(), Leg->GetLegDest(), msg);
		MessageBox (zMsg, MB_APPLMODAL);
		plistb->ResetContent();
		m_NewSci.Empty();
		delete LegIterator;
		return;
	  }
	  POSITION pos = m_NewSci.m_CmptList.GetStartPosition();
	  CVnasCmptValue *pCVuser;
	  char cmpt;
	  CString finmess, scimess;
	  int i;
		while (pos)
		{
			m_NewSci.m_CmptList.GetNextAssoc (pos, cmpt, pCVuser);
			scimess.Format ("/%cG-%d", cmpt, pCVuser->m_scg);
			finmess = scimess;
			for (i = 1; i <= m_NewSci.NbSci(cmpt); i++)
			{
				if (pCVuser->m_scivalue[i] != UNDEFVAL)
				{
					scimess.Format("/%c%d-%d", cmpt, i, pCVuser->m_scivalue[i]);
					finmess += scimess;
				}
			}
	    if (Leg->DepartsNextDay())
	      fullmess = basedemain;
	    else
	      fullmess = basemsg;
	    fullmess += Leg->GetLegOrig();
	    fullmess += Leg->GetLegDest();
	    fullmess += finmess;

			// DM7978-NCH
			CDC*					pDC = plistb->GetDC();
			CFont*				pFont = plistb->GetFont();
			CFont*				pOldFont = pDC->SelectObject(pFont);
			CSize					sz;
			TEXTMETRIC		tm;
			int						dx = 0;

			pDC->GetTextMetrics(&tm);
			sz = pDC->GetTextExtent(fullmess);
			sz.cx += tm.tmAveCharWidth;
			if (sz.cx > dx)
				dx = sz.cx;
			pDC->SelectObject(pOldFont);
			plistb->ReleaseDC(pDC);
			plistb->SetHorizontalExtent(dx);
			//END DM7978-NCH

      plistb->AddString (fullmess);
	    m_CrossMidnightTable.Add ((int)(Leg->DepartsNextDay()));
	  }
	  
	  if (Leg->GetLegDest() == legd)
	    LegOk = FALSE;
	  m_NewSci.Empty();
	} // if (LegOk)
  }
  delete LegIterator;
}
void CSendVnauDlg::OnBuildVnau() 
{ 
	if (!UpdateData(TRUE))
		return;

	if (m_bModeVNAS)
	{
		BuildVnas();
		return;
	}

	m_natureRhealys = 0;
	if (!CheckAuthorizations())
	{
		m_NewAuth.Empty();
		m_jumdata = "";
		MessageBeep (MB_ICONEXCLAMATION);
		return;
	}
	CListBox* plistb;
	plistb = (CListBox*) GetDlgItem (IDC_LIST_VNAU);
	plistb->ResetContent();
	m_CrossMidnightTable.RemoveAll();

	CString basemsg = "VNAU";
	CString basedemain;
	CString jjMMM;
	RW4Byte jule;
	char tamp[10];
	itoa (m_goodtranche, tamp, 10);
	basemsg += tamp;
	basemsg += '/';
	jjMMM = "";
	itoa (m_goodday, tamp, 10);
	if (m_goodday < 10)
		jjMMM += '0';		// date sur 2 digits
	jjMMM += tamp;
	jjMMM += m_goodmonth;
	basedemain = basemsg;
	basemsg += jjMMM;
	jule = GiveJulian (jjMMM);
	jule++;
	jjMMM = MakeStrDate (jule);
	basedemain += jjMMM;

	CString lego, legd, legw, msg;
	CVnauAuthoLeg *pAuthoLeg;

	lego = m_editOD.Left(STATION_SIZE);
	legd = m_editOD.Right(STATION_SIZE);
	BOOL   withcmpt;

	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	// DM 7978 : DT 13903 - avt 17
	// Ne controler CC0 == CC1 que si la date gang est atteinte
	bool controlerCC0EgalCC1 = pDoc->m_pGlobalSysParms->GetControlerCC0EgalCC1();

	legw = lego;
	// 4802; ajout d'un message si autorisation < capa ou > 112% de la capa 
	int coderr = 0;
	CString szErrAuthCap;
	int auth_cc0 = 0;								//DM7978 - NCH
	int auth_cc1 = 0;								//DM7978 - NCH
	int cc_idx = 0;									//DM7978 - NCH
	std::string name_cc0, name_cc1; //DM7978 - NCH

	CString szCC0NonEgaleCC1;
	CString szClient = ((CCTAApp*)AfxGetApp())->m_pDatabase->GetClientParameter();

	while (!legw.IsEmpty())
	{
		if (m_LegAuthList.Lookup (legw, pAuthoLeg))
		{
			if (pAuthoLeg->m_dptnextday)
				msg = basedemain;
			else
				msg = basemsg;
			msg += pAuthoLeg->m_legorig;
			legw = pAuthoLeg->m_legdest;
			msg += legw;
			withcmpt = FALSE;

			if (legw.CompareNoCase (legd) == 0)
				legw = "";
			// Ecriture des niveaux d'autorisations, mais en se basant sur le retour du VNL
			// Pour avoir la liste des buckets. Mais sur ce qu'a entre l'utilisateur pour
			// la liste des compartiments.
			CVnauCmptValue *pCVvnl;
			WORD ibkt;
			WORD valeur;
			if (!m_jumdata.IsEmpty())
			{ 
				// Traitement des jumeaux
				int i = m_jumdata.Find('/');
				int j;
				CString svaljum = m_jumdata.Left (i);
				msg += '/';
				msg += svaljum;
				for (j = i+1; j < m_jumdata.GetLength(); j++)
				{
					char cmpt;
					if (pAuthoLeg->m_CmptList.Lookup (m_jumdata.GetAt(j), pCVvnl))
					{ 
						// Le compartiment indique par l'utilisateur existe aussi dans le VNL 
						withcmpt = TRUE;
						cc_idx = 0;
						for (ibkt = 0; ibkt < pCVvnl->m_bktvalue.GetSize(); ibkt++)
						{ 
							cmpt = m_jumdata.GetAt(j);
							if (!m_NewAuth.GetValue (m_jumdata.GetAt(j), ibkt, valeur))
							{				
								pAuthoLeg->GetValue (m_jumdata.GetAt(j), ibkt, valeur);
								//DM4802, ajout d'un contrôle sur la capacite
								if (ibkt == 0)
									ControlValue (szErrAuthCap, valeur, pCVvnl->m_capacity);
								msg += '/';
								msg += m_jumdata.GetAt(j);
								itoa (ibkt, tamp, 10);
								msg += tamp;
								msg += '-';
								itoa (valeur, tamp, 10);
								msg += tamp;

								//DM7978 YI6617
								if (cc_idx == 0)
								{
									// name_cc0 += m_jumdata.GetAt(j);
									// HRI, 17/06/2015 dans le cadre de l'AVT 17 DT 13903
									name_cc0  = m_jumdata.GetAt(j);
									name_cc0 += '0';
									auth_cc0  = valeur;
								}
								if (cc_idx == 1)
								{
									// name_cc1 += m_jumdata.GetAt(j);
									// HRI, 17/06/2015 dans le cadre de l'AVT 17 DT 13903
									name_cc1  = m_jumdata.GetAt(j);
									name_cc1 += '1';
									auth_cc1 = valeur;
								}
								cc_idx++;
								//DM7978 YI6617
							}

							//END DM7978 YI6617
						} // End du For
						//DM7978 YI6617
						cc_idx = 0;
						if(szClient == SNCF || szClient == SNCF_TRN)
						{	
							// DM 7978 : DT 13903 - avt 17
							// Ne controler CC0 == CC1 que si l'indicateur de contrôle est à true
							if (cmpt != 'O' && controlerCC0EgalCC1 && auth_cc0 != auth_cc1)
							{
								if (szCC0NonEgaleCC1.IsEmpty())
								{
									szCC0NonEgaleCC1.Format ("Mise à jour impossible\n%s(%d) != %s(%d)",
										name_cc0.c_str(), auth_cc0, name_cc1.c_str(), auth_cc1);
								}
								else
								{
									CString sz = szCC0NonEgaleCC1;
									szCC0NonEgaleCC1.Format ("%s\n%s(%d) != %s(%d)", sz,
										name_cc0.c_str(), auth_cc0, name_cc1.c_str(), auth_cc1);
								}

							}
						}
					}     // End boucle sur les cmpt
					if (!szCC0NonEgaleCC1.IsEmpty())
					{
						AfxMessageBox (szCC0NonEgaleCC1, MB_ICONSTOP | MB_OK);
						return;
					}
				}
			}
			else
			{ 
				// Traitement des VNAU normaux (non jumeaux)
				CVnauCmptValue *pCVuser;
				char cmpt;
				cc_idx = 0;

				// Dans le but de forcer le VNAU a contenir tous les compartiments renvoyés par
				// le VNL, on ajoute de façon bidon dans m_NewAuth les compartiments non cités
				// par l'utilisateur.
				POSITION pos = pAuthoLeg->m_CmptList.GetStartPosition();
				CString listeCmptATraiter;
				while (pos)
				{
					pAuthoLeg->m_CmptList.GetNextAssoc (pos, cmpt, pCVvnl);
					if (! m_NewAuth.m_CmptList.Lookup (cmpt, pCVvnl))
					{
						m_NewAuth.AddCmpt (cmpt, 0);
					}
					else
					{
						listeCmptATraiter.AppendChar(cmpt);
					}
				}

				pos = m_NewAuth.m_CmptList.GetStartPosition();
				WORD lastval;
				while (pos)
				{
					m_NewAuth.m_CmptList.GetNextAssoc (pos, cmpt, pCVuser);
					if (pAuthoLeg->m_CmptList.Lookup (cmpt, pCVvnl))
					{
						// Le compartiment indique par l'utilisateur existe aussi dans le VNL (heureusement)
						withcmpt = TRUE;
						lastval = UNDEFVAL;
						for (ibkt = 0; ibkt < pCVvnl->m_bktvalue.GetSize(); ibkt++)
						{
							if (!m_NewAuth.GetValue (cmpt, ibkt, valeur))
							{
								pAuthoLeg->GetValue (cmpt, ibkt, valeur);
							}
							if (valeur == UNDEFVAL) // pas indique par l'utilisateur, on prend celle du VNL
							{
								pAuthoLeg->GetValue (cmpt, ibkt, valeur);
							}
							if (valeur > lastval)
								valeur = lastval;
							lastval = valeur;
							//DM4802, ajout d'un contrôle sur la capacite
							if (ibkt == 0)
								ControlValue (szErrAuthCap, valeur, pCVvnl->m_capacity);
							msg += '/';
							msg += cmpt;
							itoa (ibkt, tamp, 10);
							msg += tamp;
							msg += '-';
							itoa (valeur, tamp, 10);
							msg += tamp;

							//DM7978 YI6617
							if (cc_idx == 0)
							{
								// name_cc0 += cmpt;
								// HRI, 17/06/2015 dans le cadre de l'AVT 17 DT 13903
								name_cc0 = cmpt;
								name_cc0 += '0';
								auth_cc0 = valeur;
							}
							if (cc_idx == 1)
							{
								// name_cc1 += cmpt;
								// HRI, 17/06/2015 dans le cadre de l'AVT 17 DT 13903
								name_cc1 = cmpt;
								name_cc1 += '1';
								auth_cc1 = valeur;
							}
							cc_idx++;
							//END DM7978 YI6617
						}
						//END DM7978 YI6617
					}   // END du For
						//DM7978 YI6617
					cc_idx = 0;
					if(szClient == SNCF || szClient == SNCF_TRN)
					{	
						// DM 7978 : DT 13903 - avt 17
						// Ne controler CC0 == CC1 que si l'indicateur de contrôle est à true
						if (cmpt != 'O' &&
							listeCmptATraiter.Find(cmpt)>= 0 &&
							controlerCC0EgalCC1 && 
							auth_cc0 != auth_cc1)
						{
							if (szCC0NonEgaleCC1.IsEmpty())
							{
								szCC0NonEgaleCC1.Format ("Mise à jour impossible\n%s(%d) != %s(%d)",
									name_cc0.c_str(), auth_cc0, name_cc1.c_str(), auth_cc1);
							}
							else
							{
								CString sz = szCC0NonEgaleCC1;
								szCC0NonEgaleCC1.Format ("%s\n%s(%d) != %s(%d)", sz,
									name_cc0.c_str(), auth_cc0, name_cc1.c_str(), auth_cc1);
							}

						}
					}
				}       // End du Cmpt
				if (!szCC0NonEgaleCC1.IsEmpty())
				{
					AfxMessageBox (szCC0NonEgaleCC1, MB_ICONSTOP | MB_OK);
					return;
				}
			}
			if (withcmpt)
			{
				// DM7978-NCH
				CDC*					pDC = plistb->GetDC();
				CFont*				pFont = plistb->GetFont();
				CFont*				pOldFont = pDC->SelectObject(pFont);
				CSize					sz;
				TEXTMETRIC		tm;
				int						dx = 0;

				pDC->GetTextMetrics(&tm);
				sz = pDC->GetTextExtent(msg);
				sz.cx += tm.tmAveCharWidth;
				if (sz.cx > dx)
					dx = sz.cx;
				pDC->SelectObject(pOldFont);
				plistb->ReleaseDC(pDC);
				plistb->SetHorizontalExtent(dx);
				//END DM7978-NCH

				plistb->AddString (msg);
				m_CrossMidnightTable.Add ((int)(pAuthoLeg->m_dptnextday));
			}
		}
		else legw = "";
	}
	if (!szErrAuthCap.IsEmpty())
	{
		if (AfxMessageBox (szErrAuthCap, MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION) == IDNO)
		{
			plistb->ResetContent();
			m_CrossMidnightTable.RemoveAll();
		}
	}

	//plistb->SetHorizontalExtent(800);
}

void CSendVnauDlg::OnOK() 
{
  CListBox* plistb;
  plistb = (CListBox*) GetDlgItem (IDC_LIST_VNAU);
  int i, n;
  BOOL pasopen;

  n = plistb->GetCount();
  CString vnaumsg;
  if (!n)
  { MessageBeep (MB_ICONEXCLAMATION);
    return;
  }
  pasopen = FALSE;

  for (i = 0; i < n; i++)
  { 
    plistb->GetText ( i, vnaumsg);
	m_nbWaitedVnau ++;
	if (((CCTAApp*)APP)->GetResComm()->SendVnau (
		  this, vnaumsg, FALSE, (BOOL)(m_CrossMidnightTable.GetAt(i)), VNAU_PRO_TYPE) == -1)
	{ 
	  // Communication socket not opened.
	  pasopen = TRUE;
	  m_nbWaitedVnau --; // pas la peine de l'attendre
    }
  }
  if (pasopen)
  {
    CString s;
    s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VNAU_INBASE);
    MessageBox (s, MB_APPLMODAL);
  }
  EnableEdit (FALSE);
  EnableVnau (FALSE);
  //CWnd* pwnd;
  //pwnd = GetDlgItem (IDCANCEL);
  //pwnd->EnableWindow (FALSE);

  if ((!VNAUALLOWED(((CCTAApp*)APP)->m_UserRole)) &&
      (((CCTAApp*)APP)->m_SuperUser == 0))
    m_nbWaitedVnau = 0;
  //YM_ETSLayoutDialog::OnOK();
}

void CSendVnauDlg::OnCancel() 
{
  	//Copie de l'initialisation faite dans le constructeur
	m_month = _T("");
	m_vnlresult = _T("");
	m_day = 1;
	m_dest = _T("");
	m_orig = _T("");
	m_tranche = 0;
	m_editvnau = _T("");
	m_editOD = _T("");
	m_fixedvnau = _T("");
	m_listvnau = _T("");


	m_fvlnrep = "";
	m_nbWaitedVnau = 0;

	UpdateData (FALSE);
	m_LegAuthList.Empty();
	EnableVnau (FALSE);
        EnableEdit (TRUE);
	
        YM_ETSLayoutDialog::OnCancel();
}


#define RCMPT	0
#define RBKT	1
#define RAUTHO  2
BOOL CSendVnauDlg::CheckAuthorizations()
{
  // The UpdateData(TRUE) must have just been done, in order to have
  // m_authorizations updated.
  m_NewAuth.Empty();
  m_jumdata = "";

  if (m_editvnau.IsEmpty())
  { MessageBeep(MB_ICONEXCLAMATION);
    return FALSE;
  }
  YM_Vnl vnl;
  //vnl.Parse(m_fvlnrep);
  int errorParsing = vnl.Parse(m_fvlnrep);  // parse the VNL response
  // Anomalie 67837 : SATC 8.0.2 : consolidation du code
  if (errorParsing == VNL_PARSE_ERROR)
  {
		CString msgErrorParsing = 
			((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VNL_ERROR) + 
				": " + m_fvlnrep;
		AfxMessageBox(msgErrorParsing, MB_OK);
		return FALSE;
  }
  if ((vnl.GetNatTranche(YM_Vnl::SN_INTER) > 0) || (vnl.GetNatTranche(YM_Vnl::SN_BI_INTER) > 0))
  {
	  m_natureRhealys = vnl.GetNatTranche(YM_Vnl::SN_INTER);
	  if (vnl.GetNatTranche(YM_Vnl::DB) == vnl.GetNatTranche (YM_Vnl::NOT_RHEALYS))
	  {
	    CString s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_FORBIDEN_ON_DB);
		AfxGetMainWnd()->MessageBox(s, AfxGetAppName(), MB_ICONERROR | MB_APPLMODAL);
		return FALSE;
	  }
	  m_editvnau.MakeUpper();
	  if ((m_editvnau.Find( "A0" ) >=0 ) || (m_editvnau.Find( "B0" ) >=0 ))
	  {
	     CString s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RHEALYS_BKT0_FORBID);
		 AfxGetMainWnd()->MessageBox(s, AfxGetAppName(), MB_ICONERROR | MB_APPLMODAL);
	     return FALSE;
	  }
  }

  char cj = m_editvnau.GetAt(0);
//  if ((cj >= '0') && (cj <= '9'))
  if (isdigit( cj ))
  { // l'utilisateur specifie un % TGV jumeaux, on va donc ignorer 
    // l'origine destination indiquée
	m_editOD.Empty();
	UpdateData (FALSE);
  }

  if (!CheckOD (&vnl))
    return FALSE;

  int i, etat, lautho, rbkt, rautho;
  i = etat = lautho = rbkt = rautho = 0;
  WORD value;
  char c, rcmpt;
  etat = RCMPT;
  CString s, zMsg;
  rcmpt = '0';
  lautho = m_editvnau.GetLength();
  CEdit *editautho = (CEdit*)GetDlgItem (IDC_EDIT_VNAU);
  if (!lautho)
  {
    MessageBeep(MB_ICONEXCLAMATION);
    editautho->SetFocus();
    return FALSE;
  }

  c = m_editvnau.GetAt(0);
//  if ((c >= '0') && (c <= '9'))
  if (isdigit( c ))
  { // TGV Jumeaux -- Is it allowed for this client ?
	BOOL bjumok = TRUE;
	if ( ! ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTGVJumeauxFlag() )
	{
	  // TGV Jumeaux format is not allowed for this client - produce an error message
	  s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VNAU_BAD_SYNTAX);
	  MessageBox (s, MB_APPLMODAL);
	  bjumok = FALSE;
	}
	else
	{
	  int i = m_editvnau.Find ('/');
	  if ( i < 1) 
	  {
		bjumok = FALSE;
	  }
	  else
	  { 
		char k;
		int j;
		for (j = 0; j < i; j++)
		{ k = m_editvnau.GetAt(j);
//		  if ((k < '0') || (k > '9'))
		  if (! isdigit(k))
		  { bjumok = FALSE;
			break;
		  }
		}
		if (bjumok)
		{
		  if (m_editvnau.GetLength() == i+1)
			bjumok = FALSE;
		  else for (j = i+1; j < m_editvnau.GetLength(); j++)
		  {
			c = m_editvnau.GetAt(j);
			if ((c <'A') || (c > 'Z'))
			  bjumok = FALSE;
		  }
		}
	  }
	  if (bjumok) 
		m_jumdata = m_editvnau;
	  else
	  {
    int iResID = ( ! ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTRNFlag() ) ?
      IDS_FORMAT_JUM : IDS_FORMAT_JUM_TRN;

		s = ((CCTAApp*)APP)->GetResource()->LoadResString(iResID);
		MessageBox (s, MB_APPLMODAL);
	  }
	}
	return bjumok;
  }


  for (i = 0; i < lautho; i++)
  {
    c = m_editvnau.GetAt(i);
	switch (etat)
	{
	  case RCMPT :
	  { 
	    if ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'))
		  break;

		if (m_ValidCmpt.Find (c) < 0)
	    {
		  s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NOTA_CMPT);
		  zMsg.Format (s, c);
		  editautho->SetSel (i, i+1);
		  MessageBox (zMsg, MB_APPLMODAL);
	      return FALSE;
		}
		etat = RBKT;
		rcmpt = c;
		rbkt = 0;
		rautho = 0;
		break;
	  }

	  case RBKT :
	  {
	    if ( (c != '-') && (! isdigit(c)) )
		{
		  s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NOTA_BKT);
		  zMsg.Format (s, c);
		  editautho->SetSel (i, i+1);
		  MessageBox (zMsg, MB_APPLMODAL);
	      return FALSE;
		}
		if (c != '-')
		  rbkt = rbkt * 10 + (int)(c - '0');
		else
		{
		  if (rbkt > 99)
		  {
		    s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NOTA_BKT);
		    zMsg.Format (s, c);
		    editautho->SetSel (i, i+1);
		    MessageBox (zMsg, MB_APPLMODAL);
	        return FALSE;
		  }
		  etat = RAUTHO;
		}
		break;
	  }

	  case RAUTHO :
	  {
	    if ( (c != '/') && (! isdigit(c)) )
		{
		  s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NOTA_AUTHO);
		  zMsg.Format (s, c);
		  editautho->SetSel (i, i+1);
		  MessageBox (zMsg, MB_APPLMODAL);
	      return FALSE;
		}
		if (c != '/')
		  rautho = rautho * 10 + (int)(c - '0');
		else
		{
		  if (rautho > 9999)
		  {
		    s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NOTA_AUTHO);
		    zMsg.Format (s, c);
		    editautho->SetSel (i, i+1);
		    MessageBox (zMsg, MB_APPLMODAL);
	        return FALSE;
		  }

		  if (m_NewAuth.GetValue (rcmpt, rbkt, value) && (value != UNDEFVAL))
		  {
		    s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_ERR_DBLBKT);
			zMsg.Format (s, rcmpt, rbkt);
			MessageBox (zMsg, MB_APPLMODAL);
			return FALSE;
		  }
		  else
		    m_NewAuth.AddAutho (rcmpt, rbkt, rautho);
		  etat = RCMPT;
		  rcmpt = ' ';
		}
		break;
	  }
	} // switch (etat)
  } // for loop of character reading.
  if ((etat == RAUTHO) ||
      ((etat == RCMPT) && (rcmpt == ' ')) )
  { // Ok ending
    if (m_NewAuth.GetValue (rcmpt, rbkt, value) && (value != UNDEFVAL))
	{
	  s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_ERR_DBLBKT);
	  zMsg.Format (s, rcmpt, rbkt);
	  MessageBox (zMsg, MB_APPLMODAL);
	  return FALSE;
    }
	else m_NewAuth.AddAutho (rcmpt, rbkt, rautho);
  }
  else
  {
     s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VNAU_BAD_SYNTAX);
	 MessageBox (s, MB_APPLMODAL);
	 return FALSE;
  }

  if (!m_NewAuth.VerifyNesting())
  {
	s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BAD_NESTING);
	MessageBox (s, MB_APPLMODAL);
	return FALSE;
  }

  //DM5480 Train auto
  if (m_bTrainAuto)
  { 
    WORD valO0;
	if ( m_NewAuth.GetValue('O', 0, valO0) && (valO0 > m_capaTrainAuto) && (valO0 != UNDEFVAL))
	{
	  s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AUTOTRAIN_AUTH_INF_CAP);
	  MessageBox (s, MB_APPLMODAL);
	  return FALSE;
	}
  }

  return TRUE;
}

BOOL CSendVnauDlg::CheckOD(YM_Vnl* pVnl)
{
  // verification de l'origine destination
  BOOL bgoodOD = TRUE;
  CString lego, legd;
  if (m_editOD.IsEmpty())
  {
    lego = "";
	legd = "";
    YM_VnlLegIterator* LegIterator = pVnl->CreateVnlLegIterator();
    for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
	{  
	   YM_VnlLeg* Leg = LegIterator->Current();
	   if (lego.IsEmpty())
	     lego = Leg->GetLegOrig();
	   legd = Leg->GetLegDest();
	}
	delete LegIterator;
    m_editOD = lego;
	m_editOD += legd;
	UpdateData (FALSE);
  }
  else
  { 
    if (m_editOD.GetLength() != STATION_SIZE * 2)
      bgoodOD = FALSE;
    else
    { 
      int no, nd, nn;
	  no = nd = -1;
      lego = m_editOD.Left(STATION_SIZE);
	  legd = m_editOD.Right(STATION_SIZE);
      nn = 0;
      YM_VnlLegIterator* LegIterator = pVnl->CreateVnlLegIterator();
      for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
      {
	    nn++;
        YM_VnlLeg* Leg = LegIterator->Current();
	    if (lego == Leg->GetLegOrig()) no = nn;
	    if (legd == Leg->GetLegDest()) nd = nn+1;
	  }
	  delete LegIterator;
	  if ((no < 0) || (nd < 0) || (nd <= no))
	    bgoodOD = FALSE;
	}
  }
  if (!bgoodOD)
  {	  
      CString s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BAD_OD);
	  MessageBox (s, MB_APPLMODAL);
	  return FALSE;
  }
  return TRUE;
}

BOOL CSendVnauDlg::CheckScgSci()
{
  // The UpdateData(TRUE) must have just been done, in order to have
  // m_authorizations updated.
  m_NewSci.Empty();

  if (m_editvnau.IsEmpty())
  { MessageBeep(MB_ICONEXCLAMATION);
    return FALSE;
  }
  YM_Vnl vnl;
  //vnl.Parse(m_fvlnrep); 
  // Anomalie 67837 : SATC 8.0.2 : consolidation du code
  int errorParsing = vnl.Parse(m_fvlnrep);  // parse the VNL response
  if (errorParsing == VNL_PARSE_ERROR)
  {
		CString msgErrorParsing = 
			((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VNL_ERROR) + 
				": " + m_fvlnrep;
		AfxMessageBox(msgErrorParsing, MB_OK);
		return FALSE;
  }
  // JMG debug.
  /*if (vnl.GetNatTranche (YM_Vnl::NOT_RHEALYS) % 2 == 0)
    vnl.DummySci(5);*/
  if (vnl.GetNatTranche(YM_Vnl::SN_INTER) > 0)
  {
	m_natureRhealys = vnl.GetNatTranche(YM_Vnl::SN_INTER);
	if (vnl.GetNatTranche(YM_Vnl::DB) == vnl.GetNatTranche (YM_Vnl::NOT_RHEALYS))
	{
	  CString s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_FORBIDEN_ON_DB);
	  AfxGetMainWnd()->MessageBox(s, AfxGetAppName(), MB_ICONERROR | MB_APPLMODAL);
	  return FALSE;
	}
  }

  if (!CheckOD (&vnl))
    return FALSE;

  CEdit *editautho = (CEdit*)GetDlgItem (IDC_EDIT_VNAU);
  if (!m_editvnau.GetLength())
  {
    MessageBeep(MB_ICONEXCLAMATION);
    editautho->SetFocus();
    return FALSE;
  }

  // Initialisation de m_NewSci avec les données du VNL pour le premier leg
  InitNewSci (&vnl, "");

  return AnalyzeFullScgi();
}

// Analyse de m_editvnau (la chaine complete saisie par l'utilisateur) dans le cas SCG, SCI
// Si OK, en sortie la structure m_NewSci contiendra les valeurs lues.
// m_NewSci devra avoir été initialisée avec les valeurs lues dans le VNL pour le tronçon cible.
// Cette fonction servira donc d'une part pour l'analyse syntaxique initiale de la chaine saisie,
// et d'autre part pour modifier m_NewSci pour chaque leg avant construction du VNAS de ce leg.
BOOL CSendVnauDlg::AnalyzeFullScgi ()
{
  // Analyse de la chaine saisie par bloc compris entre une lettre d'espace physique et
  // un slash ou la fin de la chaine
  int j = 0;
  int p = 0;
  int k = 0;
  CString subedit;
  do
  {
    p = j;
    k = m_editvnau.Find('/', j);
    if (k == -1)
    {
	  subedit = m_editvnau.Right(m_editvnau.GetLength()-j);
      j = m_editvnau.GetLength();
	}
    else
    {
	  subedit = m_editvnau.Mid(j,k-j); 
	  j = k+1;
    }
    if (!AnalyzeScgi (subedit, p))
     return FALSE;
  }
  while (j < m_editvnau.GetLength());

  return TRUE;
}

// Analyse d'une sous-chaine SCG et SCI pour un compartiment dans ce qui a été saisi
// p est la position de subedit dans la chaine complete m_editvnau
BOOL CSendVnauDlg::AnalyzeScgi (CString subedit, int p)
{
  char rcmpt = subedit[0];
  CString s, zMsg;
  CEdit *editautho = (CEdit*)GetDlgItem (IDC_EDIT_VNAU);
  if (m_ValidCmpt.Find (rcmpt) < 0)
  {
    s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NOTA_CMPT);
	zMsg.Format (s, rcmpt);
	editautho->SetSel (p, p+1);
	MessageBox (zMsg, MB_APPLMODAL);
	return FALSE;
  }
  int j, k, q;
  j = 0;
  CString sSci;
  do
  {
    q = j;
	k = subedit.Find('/',j);
	if (k == -1)
	{
	  sSci = subedit.Right (subedit.GetLength()-j);
	  j = subedit.GetLength();
	}
	else
	{
	  sSci = subedit.Mid (j, k-j);
	  j = k+1;
	}
	if (!AnalyseOneScgi (sSci, rcmpt))
	{
	  editautho->SetSel (p+q, p+q+sSci.GetLength()-1);
	  zMsg.Format ("Erreur : %s", sSci);
	  MessageBox (zMsg, MB_APPLMODAL);
	  return FALSE;
	}
  }
  while (j < subedit.GetLength());
  return TRUE;
}

BOOL CSendVnauDlg::AnalyseOneScgi (CString sSci, char cmpt)
{
	// DM7978 - NCH
	// Augmentation de 1 pour les nouvelles cc soit > 7
	if ((sSci.GetLength() < 4) || (sSci.GetLength() > 7))
	  return FALSE;

	// Rappel : La structure m_NewSci a déjà été initialisée à partir de la lecture du VNL.
	// par la fonction InitNewSci() appelée en début de CheckScgSci()
	// Toutes les SCG et SCI possibles et acceptables y sont donc déjà insérées.
	// On ne fait donc que des mises à jour.
	// La validité du compartiment a déjà été vérifiée en début de AnalyzeScgi.

	// DM7978 - NCH 
	// Augmentation auth
	int nsi = m_NewSci.NbSci(cmpt);
	CString sNum = sSci.Right(sSci.GetLength() - sSci.Find("-") - 1);
	int val;
	if (!StrongAtoi (val, sNum))
	  return FALSE;
	if (sSci[1] == 'G')
	  m_NewSci.SetScg (cmpt, (WORD)val);
	else
	{
		// DM7978 - NCH
		// Nouveau parsing des SCi
		CString csWs;
		int iNbkt;
		csWs = sSci.Left(sSci.Find("-"));
		csWs.Delete(0, 1);
		if (!StrongAtoi (iNbkt, csWs))
			return FALSE;
		if ((iNbkt > 0) && (iNbkt <= nsi))
			m_NewSci.SetAutho (cmpt, iNbkt, (WORD)val);
	  else
	    return FALSE;
	}
	return TRUE;
}

void CSendVnauDlg::InitNewSci (YM_Vnl* pVnl, CString legorig)
{
	m_NewSci.Empty();
	YM_VnlLegIterator* LegIterator = pVnl->CreateVnlLegIterator();
	for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
	{
		YM_VnlLeg* Leg = LegIterator->Current();
		if (legorig.IsEmpty() || (legorig == Leg->GetLegOrig()))
		{
			YM_VnlCmptIterator* CmptIterator = Leg->CreateVnlCmptIterator();
			for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
			{
				YM_VnlCmpt* Cmpt = CmptIterator->Current();
				m_NewSci.AddCmpt (Cmpt->GetCmptId().GetAt(0), (WORD)Cmpt->GetCapacity(),
					(WORD)Cmpt->GetScgCapa(), (WORD)Cmpt->GetAuthBktZero());
				YM_VnlSciIterator* SciIterator = Cmpt->CreateVnlSciIterator();
				for ( SciIterator->First(); ! SciIterator->Finished(); SciIterator->Next() )
				{
					YM_VnlSci* Sci = SciIterator->Current();
					m_NewSci.AddAutho (Cmpt->GetCmptId().GetAt(0), (WORD)Sci->GetNestLevel(),
						(WORD)Sci->GetAuth(), (WORD)Sci->GetEtanche());
				}
				delete SciIterator;
			}
			delete CmptIterator;
			delete LegIterator;
			return;
		}
	}
	delete LegIterator;
}

void CSendVnauDlg::OnClose() 
{
	//Copie de l'initialisation faite dans le constructeur
	m_month = _T("");
	m_vnlresult = _T("");
	//m_day = 1;
	m_dest = _T("");
	m_orig = _T("");
	//m_tranche = 0;
	m_editvnau = _T("");
	m_editOD = _T("");
	m_fixedvnau = _T("");
	m_listvnau = _T("");
	m_LegAuthList.Empty();

	m_fvlnrep = "";
	m_nbWaitedVnau = 0;

	YM_ETSLayoutDialog::OnClose();
}


BOOL CSendVnauDlg::OnInitDialog() 
{
	YM_ETSLayoutDialog::OnInitDialog();

  CPane tranchePane = pane( VERTICAL )
    << item( IDC_STATIC_TRANCHE, NORESIZE )
	  << item( IDC_EDIT_TRANCHE, NORESIZE );

  CPane dayPane = pane( VERTICAL )
    << item( IDC_STATIC_DAY, NORESIZE )
	  << item( IDC_EDIT_DAY, NORESIZE );

  CPane monthPane = pane( VERTICAL )
    << item( IDC_STATIC_MONTH, NORESIZE )
	  << item( IDC_COMBO_MONTH, NORESIZE );

  CPane origPane = pane( VERTICAL )
    << item( IDC_STATIC_ORIG, NORESIZE )
	  << item( IDC_EDIT_ORIG, NORESIZE );

  CPane destPane = pane( VERTICAL )
    << item( IDC_STATIC_DEST, NORESIZE )
	  << item( IDC_EDIT_DEST, NORESIZE );

  CPane parmPane = pane( HORIZONTAL, GREEDY, nDefaultBorder, 10, 10 )
    << tranchePane
    << dayPane
    << monthPane
    << origPane
    << destPane;

  CPane vnlPaneCtrl = paneCtrl( IDC_STATIC_VNL, VERTICAL, GREEDY, nDefaultBorder, 10, 10 )
    << parmPane
    << item( IDC_EDIT_AUTHORIZATIONS );

  CPane origDestPane = pane( VERTICAL )
    << item( IDC_STATIC_ORIGDEST, NORESIZE )
	  << item( IDC_EDIT_OD, NORESIZE );

  CPane authVnauPane = pane( VERTICAL )
    << item( IDC_STATIC_AUTH, NORESIZE )
	  << item( IDC_EDIT_VNAU, ABSOLUTE_VERT );

  CPane vnauPane = pane( HORIZONTAL )
    << item( IDC_FIXED_VNAU, NORESIZE | ALIGN_BOTTOM )
    << origDestPane
    << item( IDC_STATIC_DIV, NORESIZE | ALIGN_BOTTOM )
    << authVnauPane;

  CPane btnPane = pane( HORIZONTAL )
    << item( IDC_BUILD_VNAU, NORESIZE )
    << item( IDOK, NORESIZE )
    << itemFixed(HORIZONTAL, 50)
    << item( IDCANCEL, NORESIZE );

  CPane vnauPaneCtrl = paneCtrl( IDC_STATIC_VNAU, VERTICAL, GREEDY, nDefaultBorder, 10, 10 )
    << vnauPane
    << item( IDC_LIST_VNAU )
    << btnPane;

  CreateRoot(VERTICAL)
	  << vnlPaneCtrl
	  << vnauPaneCtrl;

  UpdateLayout();
  m_bTrainAuto = FALSE;

	CComboBox* pMonthList = (CComboBox*)GetDlgItem (IDC_COMBO_MONTH);
	pMonthList->ResetContent();
	pMonthList->AddString("JAN");
	pMonthList->AddString("FEB");
	pMonthList->AddString("MAR");
	pMonthList->AddString("APR");
	pMonthList->AddString("MAY");
	pMonthList->AddString("JUN");
	pMonthList->AddString("JUL");
	pMonthList->AddString("AUG");
	pMonthList->AddString("SEP");
	pMonthList->AddString("OCT");
	pMonthList->AddString("NOV");
	pMonthList->AddString("DEC");

  if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSCXFlag())
  {
    ((CButton*)GetDlgItem (IDC_MODE_VNAS))->ShowWindow(SW_SHOW);
	((CButton*)GetDlgItem (IDC_MODE_VNAS))->EnableWindow(TRUE);
  }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
