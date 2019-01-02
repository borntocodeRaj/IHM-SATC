// QuickVnauDlg.cpp : implementation file
//
#include "StdAfx.h"

#include "ResTracer.h"
#include "VNAXwnd.h"
#include "SendVnauDlg.h"
#include "QuickVnauDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuickVnauDlg dialog


CQuickVnauDlg::CQuickVnauDlg()
	: CDialog()
{
	//{{AFX_DATA_INIT(CQuickVnauDlg)
	m_input = _T("");
	m_result = _T("");
	//}}AFX_DATA_INIT
	m_bTrainAuto = FALSE;
	m_capaTrainAuto = 0;
	m_LegAuthList = new CVnauByLegAuthMap();
	m_LegSciList = new CVnasByLegAuthMap();
}

CQuickVnauDlg::~CQuickVnauDlg()
{
	delete m_LegAuthList;
	delete m_LegSciList;
}

void CQuickVnauDlg::Create()
{
  if ( ! CDialog::Create(CQuickVnauDlg::IDD, AfxGetMainWnd() ) )
    AfxGetMainWnd()->MessageBox("Unable to create QuickVnau box", 
                        AfxGetAppName(), 
                        MB_ICONERROR | MB_APPLMODAL);
  CWnd* ResultWnd = GetDlgItem (IDC_RESULT);

  ResultWnd->SetFont (((CCTAApp*)APP)->GetDocument()->GetFixedFont(NULL));
}

void CQuickVnauDlg::Changefont()
{
  if (!IsWindow(m_hWnd))
    return;
  CWnd* ResultWnd = GetDlgItem (IDC_RESULT);
  if (ResultWnd)
    ResultWnd->SetFont (((CCTAApp*)APP)->GetDocument()->GetFixedFont(NULL));
}

void CQuickVnauDlg::Show()
{
	if( !GetSafeHwnd() )
		Create();

  ShowWindow(SW_SHOW);
}

void CQuickVnauDlg::Hide()
{
  ShowWindow(SW_HIDE);
}

void CQuickVnauDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuickVnauDlg)
	DDX_Text(pDX, IDC_INPUT, m_input);
	DDV_MaxChars(pDX, m_input, INT_MAX);
	DDX_Text(pDX, IDC_RESULT, m_result);
	//}}AFX_DATA_MAP

	if (!pDX->m_bSaveAndValidate)
	{
	  CEdit* pedit = (CEdit*)GetDlgItem(IDC_RESULT);
	  int kl = pedit->GetLineCount();
	  if (kl > 20)
		pedit->LineScroll (kl - 10);
	}
}


BEGIN_MESSAGE_MAP(CQuickVnauDlg, CDialog)
	//{{AFX_MSG_MAP(CQuickVnauDlg)
	ON_BN_CLICKED(IDC_PURGE, OnPurge)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CQuickVnauDlg::EnableInput (BOOL enabled)
{
  CWnd* pwnd;
  pwnd = GetDlgItem (IDC_INPUT);
  pwnd->EnableWindow (enabled);
}

void CQuickVnauDlg::OnNotify( YM_Observable* pObject )
{

 if( pObject->IsKindOf( RUNTIME_CLASS(YM_Msg) ) )
 {
   YM_Msg* pMsg = (YM_Msg*)pObject;

   /// DM-7978 - NCH
   if ((pMsg->GetNat() == YM_Msg::MSG_VNL)  &&  (m_bwaitingVnl))
   {
	if (pMsg->GetType() == YM_Msg::SOC_REP_OK)
	{
      YM_Vnl vnl;
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
	  // BUGi 69452 : [THALES][QUA][DM7978][SATC8.0.6] : L’envoie de deux message VNAU sur la même classe de contrôle bloque la fenetre VNAU
	  // HRI, 29/06/2015 : Dès lors qu'on a réussi à récupérer le VNL
	  // on peut remettre m_bwaitingVnl à FALSE
	  // NPI - 70039
	  // Mise en commentaire de la correction --> Effet de bord Auto-Train
	  // La description de l'ano 69452 indique une correction via la COMM
	  //m_bwaitingVnl = FALSE; 
	  ///
	  // Fin modification HRI
	  m_bTrainAuto = FALSE;
	  m_capaTrainAuto = 0;
	  m_LegAuthList->Empty();
	  m_LegSciList->Empty();

	  if (vnl.GetNatTranche(YM_Vnl::DB) == vnl.GetNatTranche (YM_Vnl::NOT_RHEALYS))
	  {
	    CString mess = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_FORBIDEN_ON_DB);
		AfxGetMainWnd()->MessageBox((LPCSTR)mess,AfxGetAppName(), MB_ICONERROR | MB_APPLMODAL);
		EnableInput (TRUE);
		return;
	  }
	  if ((vnl.GetNatTranche(YM_Vnl::SN_INTER) > 0) || (vnl.GetNatTranche(YM_Vnl::SN_BI_INTER) > 0))
	  {
		CString ws = m_input;
	    ws.MakeUpper();
		if ((ws.Find( "A0" ) >= 0) || (ws.Find( "B0" ) >= 0))
		{
		  CString mess = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RHEALYS_BKT0_FORBID);
		  AfxGetMainWnd()->MessageBox((LPCSTR)mess, AfxGetAppName(), MB_ICONERROR | MB_APPLMODAL);
		  EnableInput (TRUE);
		  return;
		}
	  }

      YM_VnlLegIterator* LegIterator = vnl.CreateVnlLegIterator();

	  // Si tous les legs indiquent un DepartsNextDay, il faut supprimer cette information, car
	  // le VNL a déjà été fait avec la date du lendemain.
	  BOOL allnextday = true;
	  BOOL bYaSci = false;

	  for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
      {
        YM_VnlLeg* Leg = LegIterator->Current();
		if (!Leg->DepartsNextDay()) allnextday = false;

		//DM 5480 vérification de l'état train Auto
		 // Le VNL envoyé comportait une OD, c'est donc celui construit a partir du VNAU saisi
		  YM_VnlCmptIterator* CmptIterator = Leg->CreateVnlCmptIterator();
          for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
          {
            YM_VnlCmpt* Cmpt = CmptIterator->Current();
	
			if (Cmpt->YaSci())
			  bYaSci = true;
		    if ((Cmpt->GetCmptId() == "O") && (pMsg->GetInitial().GetLength() > 17))		//DM 5480 train auto
		    {
					// Par sécurité on renvoi le VNL sans OD pour être sûr de travailler sur l'ensemble de la desserte.
					delete CmptIterator;
							delete LegIterator;
					CString finmess = pMsg->GetInitial().Right(2);
					int sl = 10;
					if (finmess == "SC") // le VNL comportait la demande de sous-contingent.
						sl = 13;
					CString VnlMess = pMsg->GetInitial().Left (pMsg->GetInitial().GetLength() - sl);
					if (sl==13)
						VnlMess += "/SC";
					((CCTAApp*)APP)->GetResComm()->SendVnl (this, VnlMess);
					return;
		    }
			if (m_NewSci.m_CmptList.GetCount())
			{ // envoi de VNAS, on complete info capa et si besoin scg
				m_NewSci.SetCapa (Cmpt->GetCmptId().GetAt(0), Cmpt->GetCapacity());
			  WORD scg = 0;
			  if (m_NewSci.GetScg (Cmpt->GetCmptId().GetAt(0), scg) && (scg == UNDEFVAL))
				m_NewSci.SetScg (Cmpt->GetCmptId().GetAt(0), Cmpt->GetScgCapa());
			}
		  }
	  }

	  if (m_NewSci.m_CmptList.GetCount()) // cas VNAS
		{
			if (!bYaSci)
			{
				CString mess = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NOSCI_TRANCHE);
				CString sz ;
				sz.Format (mess, vnl.GetNatTranche (YM_Vnl::NOT_RHEALYS));
				AfxMessageBox (sz, MB_ICONSTOP | MB_OK);
				EnableInput (TRUE);
				return;
			}
			BOOL bFirstLeg = TRUE;
			for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
			{
				YM_VnlLeg* Leg = LegIterator->Current();
				CVnasAuthoLeg* p_autholeg = new CVnasAuthoLeg (Leg->GetLegOrig(), Leg->GetLegDest(),
					allnextday ? FALSE : Leg->DepartsNextDay() );
				m_LegSciList->SetAt (Leg->GetLegOrig(), p_autholeg);
				YM_VnlCmptIterator* CmptIterator = Leg->CreateVnlCmptIterator();
				for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
				{
					YM_VnlCmpt* Cmpt = CmptIterator->Current();
					//if (bFirstLeg) // forcer l'ensemble des compartiments à être présents dans le VNAS. Ne fait rien si déjà présent
					//m_NewSci.AddCmpt (Cmpt->GetCmptId().GetAt(0), (WORD)Cmpt->GetCapacity(), (WORD)Cmpt->GetScgCapa()); 
					p_autholeg->AddCmpt (Cmpt->GetCmptId().GetAt(0), (WORD)Cmpt->GetCapacity(),
						(WORD)Cmpt->GetScgCapa(), (WORD)Cmpt->GetAuthBktZero());
					if (Cmpt->GetCmptId() == "O")		//DM 5480 train auto
					{
						m_bTrainAuto = TRUE;
						m_capaTrainAuto = Cmpt->GetCapacity();
					}

					YM_VnlSciIterator* SciIterator = Cmpt->CreateVnlSciIterator();
					for ( SciIterator->First(); ! SciIterator->Finished(); SciIterator->Next() )
					{
						YM_VnlSci* Sci = SciIterator->Current();
						p_autholeg->AddAutho (Cmpt->GetCmptId().GetAt(0), (WORD)Sci->GetNestLevel(),
																	Sci->GetAuth(), Sci->GetEtanche());
					}
					delete SciIterator;
				}
				delete CmptIterator;
				bFirstLeg = FALSE;
			}
	  }
	  else // cas VNAU classiques
      for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
      {
        YM_VnlLeg* Leg = LegIterator->Current();
		CVnauAuthoLeg* p_autholeg = new CVnauAuthoLeg (Leg->GetLegOrig(), Leg->GetLegDest(),
			allnextday ? FALSE : Leg->DepartsNextDay() );
        m_LegAuthList->SetAt (Leg->GetLegOrig(), p_autholeg);

        YM_VnlCmptIterator* CmptIterator = Leg->CreateVnlCmptIterator();
        for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
        {
          YM_VnlCmpt* Cmpt = CmptIterator->Current();
		  p_autholeg->AddCmpt (Cmpt->GetCmptId().GetAt(0), (WORD)Cmpt->GetCapacity());
		  if (Cmpt->GetCmptId() == "O")		//DM 5480 train auto
		  {
			m_bTrainAuto = TRUE;
			m_capaTrainAuto = Cmpt->GetCapacity();
		  }

          YM_VnlBktIterator* BktIterator = Cmpt->CreateVnlBktIterator();
          for ( BktIterator->First(); ! BktIterator->Finished(); BktIterator->Next() )
          {
            YM_VnlBkt* Bkt = BktIterator->Current();
			p_autholeg->AddAutho (Cmpt->GetCmptId().GetAt(0), (WORD)Bkt->GetNestLevel(), Bkt->GetAuth());
	      }
		  delete BktIterator;
        }
		delete CmptIterator;
      }
	  delete LegIterator;

	  if (m_NewSci.m_CmptList.GetCount())
	    BuildAndSendVnas();
	  else
	    BuildAndSendVnau();
	  UpdateData (FALSE); // HRI, 30/06/2015
	}
   }
   if (pMsg->GetType() == YM_Msg::SOC_REP_ERROR)
   {
	  CString svnlrep = pMsg->GetMsg();
	  MakeGoodLine (svnlrep);
	  m_result += "\r\n";
	  m_result += svnlrep;
	  // BUGi 69452 : [THALES][QUA][DM7978][SATC8.0.6] : L’envoie de deux message VNAU sur la même classe de contrôle bloque la fenetre VNAU
	  // HRI, 29/06/2015
	  // Penser à débloquer l'édit box d'input
	  // NPI - 70039
	  // Mise en commentaire de la correction --> Effet de bord Auto-Train
	  // La description de l'ano 69452 indique une correction via la COMM
	  //EnableInput (TRUE);
	  //m_bwaitingVnl = FALSE;
	  ///
	  // Fin modification
	  UpdateData (FALSE);
   }
   

   if ((!m_bwaitingVnl) || (pMsg->GetType() != YM_Msg::SOC_REP_ERROR))
   {
	  CString svnlrep = pMsg->GetMsg();
	  MakeGoodLine (svnlrep);
	  m_result += "\r\n";
	  m_result += svnlrep;
	  UpdateData (FALSE);
   }
 }
 
 EnableInput (TRUE);
}

int CQuickVnauDlg::AnalyseVnau()
{
	// Analyse
	CString ws = m_input;
	ws.MakeUpper();
	CString ms = ws.Left (4);
	CString mess, ods, VnlMess, toks;
	int num, dd, trancheNo, nbkt;
	bool bVnas = false;
	bool bVnae = false;

	char *tok;

	if ((ms != "VNAU") && (ms != "VNAS") && (ms != "VNAE"))
	  return IDS_VNAU_BAD_SYNTAX;
	ws = ws.Right (ws.GetLength() - 4); // on vire VNAU

	bVnas = (ms == "VNAS");
	bVnae = (ms == "VNAE");

	char delim[4];
	delim[0] = '/';
	delim[1] = 0;
	char sline[512];

	if (ws.GetLength() > 511)
	  return IDS_VNAU_BAD_SYNTAX;

	strcpy (sline, (LPCSTR)ws);
	tok = strtok (sline, delim); // tok doit contenir le numero de tranche.
	if (tok == NULL) return IDS_VNAU_BAD_SYNTAX;
	toks = tok;
	if (!StrongAtoi(trancheNo, toks))
	  return IDS_VNAU_BAD_SYNTAX;

	// Analyse date et OD, construction du message VNL et des bases de messages
	// VNAU pour départ au jour indiqué et au lendemain.
	tok = strtok (NULL, delim); // tok doit contenir date origine destination.
	if (tok == NULL) return IDS_VNAU_BAD_SYNTAX;
	int ml = bVnae ? 7 : 0; // longeur en moins de la zone dateOD dans le cas VNAE
	if ((strlen(tok) == 14-ml) || (strlen(tok) == 15-ml))
	{ // si == 14, le jour du mois doit être sur un digit.
	  toks = tok;
	  if (strlen(tok) == 14-ml)
	  {
		ws = toks.Left(1);
		ms = toks.Mid (1, 3);
	  }
	  else
	  {
		ws = toks.Left(2);
		ms = toks.Mid (2, 3);
	  }
	  if ((MONTH_LIST_ENGLISH.Find( ms ) < 0) || 
		  (!StrongAtoi (dd, ws)))
        return IDS_WRONG_DATE;
	  if (!bVnae)
	  {
	    ods = toks.Right(STATION_SIZE * 2);
	    for (num = 0; num < STATION_SIZE * 2; num++)
	    { // controle qu'il n'y a que des caractère alphabetique dans l'OD.
		  if ((ods.GetAt(num) < 'A') || (ods.GetAt(num) > 'Z'))
		    return IDS_BAD_OD;
	    }
	  }
	  CString jjMMM;
	  if (ws.GetLength() == 1)
		jjMMM = '0' + ws;
	  else
		jjMMM = ws;
	  jjMMM += ms;
	  RW4Byte jule = GiveJulian (jjMMM);
	  if (!jule)
	    return IDS_WRONG_DATE;
	  if (bVnae)
	  {
		((CCTAApp*)APP)->GetResComm()->SendVnau (this, m_input, FALSE, FALSE, VNAU_PRO_TYPE);
		return 0;
	  }
	  m_basemsg.Format ("VNAU%d", trancheNo);
	  m_basemsg += '/';
	  m_basedemain = m_basemsg;
	  VnlMess.Format ("VNL%d", trancheNo);
	  VnlMess += '/';
	  m_basemsg += jjMMM;
	  VnlMess += jjMMM;
	  VnlMess += ods;
	  jule++;
	  jjMMM = MakeStrDate (jule);
      m_basedemain += jjMMM;
	}
	else
	  return IDS_VNAU_BAD_SYNTAX;

	if (bVnas)
	{ // VNAS
	  VnlMess += "/SC";
	  tok = strtok (NULL, delim);
	  if (tok == NULL) return IDS_VNAU_BAD_SYNTAX;
	  char curcmpt = ' ';
	  CString sVal;
	  int val;
	  while (tok != NULL)
		{ // lecture d'une autorisation
			toks = tok;

			// DM7978 - NCH
			// Augmentation du nombre de CC
			if ((toks.GetLength() < 4) || (toks.GetLength() > 7))
				return IDS_VNAU_BAD_SYNTAX;
			if ((toks.GetAt(0) < 'A') || (toks.GetAt(0) > 'Z'))
				return IDS_BAD_SYNTAX_AUTO;
			if ((toks.GetAt(2) != '-') && (toks.GetAt(3) != '-'))
				return IDS_BAD_SYNTAX_AUTO;
			curcmpt = toks.GetAt(0);
			m_NewSci.AddCmpt (curcmpt, 0, 0, 0);
			sVal = toks.Right(toks.GetLength() - toks.Find("-") - 1);
			if (!StrongAtoi (val, sVal))
				return IDS_BAD_SYNTAX_AUTO;
			if (toks.GetAt(1) == 'G')
				m_NewSci.SetScg (curcmpt, (WORD)val);
			else
			{
				// HRI, 15/05/2015 : le format du VNAU/VNAS souplesse ne passait pas 
				// Il faut accepter 0 en toks.GetAt(1)
				//if ((toks.GetAt(1) < '1') || (toks.GetAt(1) > '7'))
				if ((toks.GetAt(1) < '0') || (toks.GetAt(1) > '9'))
					return IDS_BAD_SYNTAX_AUTO;
				ws = toks.Left(toks.Find("-"));
				ws.Delete(0, 1);
				if (!StrongAtoi (nbkt, ws))
					return IDS_BAD_SYNTAX_AUTO;
				m_NewSci.AddAutho (curcmpt, (WORD)nbkt, (WORD)val, 0);
			// END DM7978 - NCH
			}
			tok = strtok (NULL, delim);
		}
	}
	else
	{ // VNAU
	  // On regarde Maintenant les autorisations.
	  tok = strtok (NULL, delim);
	  if (tok == NULL) return IDS_VNAU_BAD_SYNTAX;
	  if (isdigit (tok[0]))
	  { // ce doit être un % tgv jumeaux
	    toks = tok;
	    if ( ! ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTGVJumeauxFlag() )
	      // TGV Jumeaux format is not allowed for this client - produce an error message
	      return IDS_VNAU_BAD_SYNTAX;
	    if (!StrongAtoi(dd, toks))
		  return IDS_VNAU_BAD_SYNTAX;
	    m_jumdata = toks;
	    tok = strtok (NULL, delim); // positionne tok sur une autorisation
	    if (tok == NULL) return IDS_VNAU_BAD_SYNTAX;
	  }
	  while (tok != NULL)
	  { // lecture d'une autorisation
	    toks = tok;

			// DM7978 - NCH
			// Augmentation du nombre de CC
			if ((toks.GetLength() < 4) || (toks.GetLength() > 7))
	      return IDS_BAD_SYNTAX_AUTO;
	    if ((toks.GetAt(0) < 'A') || (toks.GetAt(0) > 'Z'))
	      return IDS_BAD_SYNTAX_AUTO;
		  if (!isdigit (toks.GetAt(1)))
				return IDS_BAD_SYNTAX_AUTO;		
			if ((toks.GetAt(2) != '-') && (toks.GetAt(3) != '-'))
				return IDS_BAD_SYNTAX_AUTO;
			ws = toks.Right(toks.GetLength() - toks.Find("-") - 1);
			if (!StrongAtoi (num, ws))
				return IDS_BAD_SYNTAX_AUTO;
			ws = toks.Left(toks.Find("-"));
			ws.Delete(0, 1);
			if (!StrongAtoi (nbkt, ws))
				return IDS_BAD_SYNTAX_AUTO;
			m_NewAuth.AddAutho(toks.GetAt(0), (WORD)nbkt, num);
			// END DM7978 - NCH

	    tok = strtok (NULL, delim);
	  }
	}

	if (!m_NewAuth.VerifyNesting())
	  return IDS_BAD_NESTING;


	
	/// DM-7978 - CLE
	CYMVnlBouchon::getInstance()->setMonoTranche();
	/// DM-7978 - CLE

	///////
	if (App().GetDocument()->m_pGlobalSysParms->GetVnlBouchon())
	{
		EnableInput (FALSE);
		m_bwaitingVnl = TRUE;
	}
	///////
	// On envoi le VNL
    if (((CCTAApp*)APP)->GetResComm()->SendVnl (this, VnlMess) == -1)
	{
			return IDS_NO_SOCKETOPEN;
	}
	else
	{
		if (!App().GetDocument()->m_pGlobalSysParms->GetVnlBouchon())
		{
			EnableInput (FALSE);
			m_bwaitingVnl = TRUE;
		}
	}

	return 0;
}

void CQuickVnauDlg::EnvoiVnl()
{
	if (App().GetDocument()->m_pGlobalSysParms->GetVnlBouchon())
	{
		EnableInput (FALSE);
	}
	
	if (((CCTAApp*)APP)->GetResComm()->SendVnl (this, m_input) == -1)
	{
		CString mess = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_SOCKETOPEN);
		MessageBox (mess, MB_APPLMODAL);
	}
	else
	{
		//EnableInput (FALSE);
		if (!App().GetDocument()->m_pGlobalSysParms->GetVnlBouchon())
		{
			EnableInput (FALSE);
		}
	}
}

void CQuickVnauDlg::EnvoiDispo()
{
	if (App().GetDocument()->m_pGlobalSysParms->GetVnlBouchon())
	{
		EnableInput (FALSE);
	}

	if (((CCTAApp*)APP)->GetResComm()->SendDispo (this, m_input) == -1)
	{
		CString mess = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_SOCKETOPEN);
		MessageBox (mess, MB_APPLMODAL);
	}
	else
	{
		//EnableInput (FALSE);
		if (!App().GetDocument()->m_pGlobalSysParms->GetVnlBouchon())
		{
			EnableInput (FALSE);
		}
	}
}

void CQuickVnauDlg::Envoi4GY()
{
	if (App().GetDocument()->m_pGlobalSysParms->GetVnlBouchon())
	{
		EnableInput (FALSE);
	}

	if (((CCTAApp*)APP)->GetResComm()->Send4GY (this, m_input) == -1)
	{
		CString mess = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_SOCKETOPEN);
		MessageBox (mess, MB_APPLMODAL);
	}
	else
	{
        //EnableInput (FALSE);
		if (!App().GetDocument()->m_pGlobalSysParms->GetVnlBouchon())
		{
			EnableInput (FALSE);
		}
	}
}

void CQuickVnauDlg::BuildAndSendVnau()
{
	POSITION pos = m_LegAuthList->GetStartPosition();
	CString keyleg, VnauMsg;
	CVnauAuthoLeg *pAuthoLeg;
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	// DM 7978 : DT 13903 - avt 17
	// Ne controler CC0 == CC1 que si l'indicateur de contrôle est égal à true
	bool controlerCC0EgalCC1 = pDoc->m_pGlobalSysParms->GetControlerCC0EgalCC1();

	while (pos)
	{
	  m_LegAuthList->GetNextAssoc (pos, keyleg, pAuthoLeg);
	  if (pAuthoLeg->m_dptnextday)
	    VnauMsg = m_basedemain;
	  else
	    VnauMsg = m_basemsg;
	  VnauMsg += pAuthoLeg->m_legorig;
	  VnauMsg += pAuthoLeg->m_legdest;

	  // construction de la chaine d'autorisation
	  POSITION poscmpt = m_NewAuth.m_CmptList.GetStartPosition();
	  WORD lastval;
	  CVnauCmptValue *pCVuser;
	  CVnauCmptValue *pCVvnl;
	  char cmpt;
	  WORD ibkt;
	  WORD valeur;
	  char tamp[10];
	  CString msgauth;
	  msgauth.Empty();
	  while (poscmpt)
	  {
	    m_NewAuth.m_CmptList.GetNextAssoc (poscmpt, cmpt, pCVuser);
	    if (pAuthoLeg->m_CmptList.Lookup (cmpt, pCVvnl))
		{ // Le compartiment indique par l'utilisateur existe aussi dans le VNL (heureusement)
		  lastval = UNDEFVAL;
		  for (ibkt = 0; ibkt < pCVvnl->m_bktvalue.GetSize(); ibkt++)
		  { if (!m_NewAuth.GetValue (cmpt, ibkt, valeur))
			   pAuthoLeg->GetValue (cmpt, ibkt, valeur);
			if (valeur == UNDEFVAL) // pas indique par l'utilisateur, on prend celle du VNL
			    pAuthoLeg->GetValue (cmpt, ibkt, valeur);
			if (valeur > lastval)
			    valeur = lastval;
			lastval = valeur;

			//DM5480 train auto
			if (m_bTrainAuto && (cmpt == 'O') && valeur > m_capaTrainAuto)
			{  
			  CString mess = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AUTOTRAIN_AUTH_INF_CAP);
		      AfxMessageBox (mess, MB_ICONSTOP | MB_OK);
		      return;
			}
			msgauth += '/';
			msgauth += cmpt;
            itoa (ibkt, tamp, 10);
	        msgauth += tamp;
		    msgauth += '-';
			itoa (valeur, tamp, 10);
			msgauth += tamp;
		  }
		}
		else
		{
		  CString mess = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_UNKNOWN_CMPT);
		  CString sz ;
		  sz.Format (mess, cmpt);
		  AfxMessageBox (sz, MB_ICONSTOP | MB_OK);
		  return;
		}
	  }

	  if (!m_jumdata.IsEmpty())
	  { // VNAU % tgv jumeaux
		VnauMsg += '/';
		VnauMsg += m_jumdata;
	  }
	  
	  VnauMsg += msgauth;
	  m_result += "\r\n";
	  m_result += VnauMsg;
	  UpdateData (FALSE);

		// DM7978 - NCH
		CString szClient = ((CCTAApp*)AfxGetApp())->m_pDatabase->GetClientParameter();
		// Si TGV ou Corail
		if(szClient == SNCF || szClient == SNCF_TRN)
		{			
			std::string checkmsg = CStringA(msgauth);
			std::istringstream iss(checkmsg);
			std::string token, name_cc0, name_cc1;
			int auth_cc0 = 0;  // HRI, 05/04/2015 (initialiser à zéro)
			int auth_cc1 = 0;  // HRI, idem
			while(getline(iss, token, '/'))
			{
				if (token.length() > 3)
				{
					if (token.at(2) == '-')
					{
						if ((token.at(0) >= 'A' && token.at(0) <= 'Z') &&
							(token.at(1) == '0'))
						{
							name_cc0 = token.at(0);
							name_cc0 += token.at(1);
							token.erase(token.begin(), token.begin() + 3);
							auth_cc0 = atoi(token.c_str());
						}
						if ((token.at(0) >= 'A' && token.at(0) <= 'Z') &&
							(token.at(1) == '1'))
						{
							name_cc1 = token.at(0);
							name_cc1 += token.at(1);
							token.erase(token.begin(), token.begin() + 3);
							auth_cc1 = atoi(token.c_str());
							break;
						}
					}
				}
			}
			// DM 7978 : DT 13903 - avt 17
			// Ne controler CC0 == CC1 que si l'indicateur de contrôle est à true
			if (*name_cc0.c_str()!= 'O' && controlerCC0EgalCC1 && auth_cc0 != auth_cc1)
			{
				CString sz;
				sz.Format ("Mise à jour impossible\n%s(%d) != %s(%d)",
					name_cc0.c_str(), auth_cc0, name_cc1.c_str(), auth_cc1);
				AfxMessageBox (sz, MB_ICONSTOP | MB_OK);
				return;
			}
		}
		// END DM7978 - NCH
		((CCTAApp*)APP)->GetResComm()->SendVnau (
		  this, VnauMsg, FALSE, pAuthoLeg->m_dptnextday, VNAU_PRO_TYPE);
	}
}

void CQuickVnauDlg::BuildAndSendVnas()
{
	POSITION pos = m_LegSciList->GetStartPosition();
	CString keyleg, VnauMsg;
	CVnasAuthoLeg *pAuthoLeg;
	while (pos)
	{
	  m_LegSciList->GetNextAssoc (pos, keyleg, pAuthoLeg);
	  if (pAuthoLeg->m_dptnextday)
	    VnauMsg = m_basedemain;
	  else
	    VnauMsg = m_basemsg;
	  VnauMsg += pAuthoLeg->m_legorig;
	  VnauMsg += pAuthoLeg->m_legdest;
	  VnauMsg.SetAt(3,'S');

	  // construction de la chaine d'autorisation
	  POSITION poscmpt = m_NewSci.m_CmptList.GetStartPosition();
	  CVnasCmptValue *pCVuser;
	  CVnasCmptValue *pCVvnl;
	  char cmpt;
	  WORD ibkt;
	  WORD valeur, valscg;
	  char tamp[10];
	  CString msgauth;
	  msgauth.Empty();
	  while (poscmpt)
	  {
	    m_NewSci.m_CmptList.GetNextAssoc (poscmpt, cmpt, pCVuser);
	    if (pAuthoLeg->m_CmptList.Lookup (cmpt, pCVvnl))
		{ 
			// Le compartiment indique par l'utilisateur existe aussi dans le VNL (heureusement)
		  msgauth = '/';
		  msgauth += cmpt;
		  msgauth += "G-";
		  if (!m_NewSci.GetScg (cmpt, valscg))
			pAuthoLeg->GetScg (cmpt, valscg);
		  if (valscg == UNDEFVAL)
		    pAuthoLeg->GetScg (cmpt, valscg);
		  itoa (valscg, tamp, 10);
		  pAuthoLeg->GetBkt0 (cmpt, valeur);
		  if (valscg > valeur)
		  {
		    CString sz;
			sz.Format ("%s-%s : Compartiment %c : CC-1 (%d) > bucket 0 (%d)",
				       pAuthoLeg->m_legorig, pAuthoLeg->m_legdest, cmpt, valscg, valeur);
			AfxMessageBox (sz, MB_ICONSTOP | MB_OK);
		    return;
		  }
		  msgauth += tamp;
		  if (m_NewSci.NbSci(cmpt) > pCVvnl->NbSci())
		  {
		    CString sz;
			sz.Format ("%s-%s : Compartiment %c : pas de sous contingent %d dans le VNL",
					 pAuthoLeg->m_legorig, pAuthoLeg->m_legdest, cmpt, pCVvnl->NbSci());
			AfxMessageBox (sz, MB_ICONSTOP | MB_OK);
		    return;
		  }
		  int sumsci = 0;
		  for (ibkt = 1; ibkt <= pCVvnl->NbSci(); ibkt++)
		  { if (!m_NewSci.GetValue (cmpt, ibkt, valeur))
			   pAuthoLeg->GetValue (cmpt, ibkt, valeur);
			if (valeur == UNDEFVAL) // pas indique par l'utilisateur, on prend celle du VNL
			   pAuthoLeg->GetValue (cmpt, ibkt, valeur);
			sumsci += valeur;
			if (valeur > valscg)
			{
			  CString sz;
			  sz.Format ("%s-%s : Compartiment %c : sous contingent %d (%d) > CC-1 (%d)",
					  pAuthoLeg->m_legorig, pAuthoLeg->m_legdest, cmpt, ibkt, valeur, valscg);
			  AfxMessageBox (sz, MB_ICONSTOP | MB_OK);
		      return;
		  }
			msgauth += '/';
			msgauth += cmpt;
            itoa (ibkt, tamp, 10);
	        msgauth += tamp;
		    msgauth += '-';
			itoa (valeur, tamp, 10);
			msgauth += tamp;
		  }
		  /* Alerte supprimée sur demande du COC par mail du 24/03/09
		  if (sumsci < valscg)
		  {
		    CString sz;
			sz.Format ("%s-%s : Compartiment %c : somme des sous contingent (%d) < CC-1 (%d)",
					  pAuthoLeg->m_legorig, pAuthoLeg->m_legdest, cmpt, sumsci, valscg);
			AfxMessageBox (sz, MB_ICONSTOP | MB_OK);
		    return;
		  }
		  */
		}
		else
		{
		  CString mess = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_UNKNOWN_CMPT);
		  CString sz ;
		  sz.Format (mess, cmpt);
		  AfxMessageBox (sz, MB_ICONSTOP | MB_OK);
		  return;
		}
		CString sss = VnauMsg;
		sss += msgauth;
		m_result += "\r\n";
	    m_result += sss;
	    UpdateData (FALSE);

		((CCTAApp*)APP)->GetResComm()->SendVnau (
		  this, sss, FALSE, pAuthoLeg->m_dptnextday, VNAU_PRO_TYPE);
	  } 
	}
}

/////////////////////////////////////////////////////////////////////////////
// CQuickVnauDlg message handlers

BOOL CQuickVnauDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CQuickVnauDlg::OnOK() 
{
	if (!UpdateData(TRUE)) // do force the doDataExchange.
      return;
	// RAZ

	m_basemsg.Empty();
	m_basedemain.Empty();
	m_NewAuth.Empty();
	m_NewSci.Empty();
	m_LegAuthList->Empty();
	m_jumdata.Empty();
	m_bwaitingVnl = FALSE;

	CString ws = m_input;
	ws.MakeUpper();

	if (ws.GetLength() < 4)
	  return;
	ws = ws.Left(3);
	CString sK8 = ws.Left(2);

	if (ws == "VNL")
	  EnvoiVnl();
	else if ((ws.GetAt(0) == '1') || (ws == "KDI") || (ws == "VIF"))
	  EnvoiDispo();
	else if (ws == "4GY")
	  Envoi4GY();
	else if (ws == "KA*")
	  EnvoiDispo();
	else if ((((CCTAApp*)APP)->m_SuperUser != 0) && (sK8 == "K8") )
	  EnvoiDispo();
	else
	{
	  int rtn = AnalyseVnau();
	  if (rtn)
	  {
	    if (((CCTAApp*)APP)->m_SuperUser != 0)
		  EnvoiDispo();
		else
		{
	      CString mess = ((CCTAApp*)APP)->GetResource()->LoadResString(rtn);
	      MessageBox (mess, MB_APPLMODAL);
		}
	  }
	}
}

void CQuickVnauDlg::OnCancel() 
{
	// BUGi 69452 : [THALES][QUA][DM7978][SATC8.0.6] : L’envoie de deux message VNAU sur la même classe de contrôle bloque la fenetre VNAU
	// HRI, 29/06/2015
	// Penser à débloquer l'édit box d'input
	EnableInput (TRUE);
	// Fin modification
	CDialog::OnCancel();
}


void CQuickVnauDlg::OnPurge() 
{
	m_result.Empty();
	UpdateData(FALSE);
}
