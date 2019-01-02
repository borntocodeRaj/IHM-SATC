// ResTracer.cpp : implementation file
//
#include "StdAfx.h"

#include "CTA_Resource.h"
#include "ResTracer.h"
#include "BouchonKdisn.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*static*/ int CResComm::negindice = 0;

/////////////////////////////////////////////////////////////////////////////
// CResComm
CResComm::CResComm()
{
	m_pMsgSocket = new YM_MsgSocket();
	m_pMsgSocket->AddObserver(this);
	m_bAcceptedConnection = FALSE;
	m_iPortNumber = 0;
	m_SrvHostName = "";
	m_killbusy = TRUE;
	m_stopactivity = FALSE;
	m_pwndTracer = NULL; 
  m_ctaclosecomm = FALSE;
}

CResComm::~CResComm()
{
	if (m_pMsgSocket->IsOpen())
	m_pMsgSocket->Close();

	delete m_pMsgSocket;
	m_pMsgSocket = NULL;

	POSITION pos = m_PendingMap.GetStartPosition();
	TSentMsg* pSentMsg;
	int num;
	while (pos)
	{
		m_PendingMap.GetNextAssoc (pos, num, pSentMsg);
		if (pSentMsg)
			delete pSentMsg;
	}

	// Unesuful to delete the messages, as they are built as AutoDelete observable
	// in YMMsgsocket.cpp
	m_PendingMap.RemoveAll();
}

void CResComm::SetTracer (CResTracer *wndTracer)
{
	m_pwndTracer = wndTracer;
}

int CResComm::SendInit()
{
   CString	strUserName = ((CCTAApp*)APP)->m_pDatabase->UserName();
   if (m_killbusy)
     strUserName += " +";
   YM_Msg* pMsg = new YM_Msg(strUserName);
   pMsg->AddObserver(this);
   m_pMsgSocket->SendMsg( pMsg );
   TSentMsg* pSentMsg = new TSentMsg;
   pSentMsg->msgtype = MSG_INIT;
   pSentMsg->msgnature = NOT_VNAU_TYPE;
   pSentMsg->msgtext = strUserName;
   pSentMsg->pending = TRUE;
   pSentMsg->errortext = "";
   pSentMsg->pymmsg = NULL;
   pSentMsg->crossmidnight = FALSE;
   CTime titi = CTime::GetCurrentTime();
   pSentMsg->sendtime = static_cast<time_t>(titi.GetTime());
   m_PendingMap.SetAt (pMsg->GetNum(), pSentMsg);
   MajTracer();
   return pMsg->GetNum();
}

void CResComm::SendTest(BOOL manuel)
{
   CString s;
   if (manuel) s = "VERIF PRESENCE";
   else s = "TEST PRESENCE";
    YM_Msg* pMsg = new YM_Msg(s, YM_Msg::MSG_TEST);
   pMsg->AddObserver(this);
   m_pMsgSocket->SendMsg( pMsg );
   TSentMsg* pSentMsg = new TSentMsg;
   pSentMsg->msgtype = MSG_TEST_PRESENCE;
   pSentMsg->msgnature = NOT_VNAU_TYPE;
   pSentMsg->msgtext = s;
   pSentMsg->pending = TRUE;
   pSentMsg->errortext = "";
   pSentMsg->pymmsg = NULL;
   pSentMsg->crossmidnight = FALSE;
   CTime titi = CTime::GetCurrentTime();
   pSentMsg->sendtime = static_cast<time_t>(titi.GetTime());
   m_PendingMap.SetAt (pMsg->GetNum(), pSentMsg);
   MajTracer();
}

void CResComm::VerifPresence()
{
  POSITION pos = m_PendingMap.GetStartPosition();
  TSentMsg* pSentMsg = NULL;
  int num, n;
  CTime titi = CTime::GetCurrentTime();
  n = 0;
  CString zs = "";
  while (pos)
  {
    m_PendingMap.GetNextAssoc (pos, num, pSentMsg);
    if (pSentMsg && pSentMsg->pending &&
	    (pSentMsg->msgtype == MSG_TEST_PRESENCE))
	{
	  n++;
	  pSentMsg->pending = FALSE;     
	}
  }
  if (pSentMsg->msgtext == "VERIF PRESENCE")
  {
    if (n)
	  zs = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMMUNICATION_OUT);
	else
	  zs = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMMUNICATION_OK);
	AfxMessageBox (zs, MB_ICONINFORMATION | MB_OK | MB_APPLMODAL);
  }
  if (n)
  { // Le serveur de comm est dans les choux, on va prendre des mesures.
    CloseComm();
	ProcessPending(TRUE);
  }
}

int CResComm::SendVnl (YM_Observer *observer, CString& msg)
{
	/// DM-7978 - CLE add vnl stub
	if (App().GetDocument()->m_pGlobalSysParms->GetVnlBouchon())
	{
		/// TODO
		return (static_cast<int>(CYMVnlBouchon::getInstance()->sendVnl(observer, msg)));
	}
	else return SendQueryMessage (observer, msg, MSG_VNL);
}

int CResComm::SendDispo (YM_Observer *observer, CString& msg)
{
	// DM7978 - LME - bouchon KDISN
	if(App().GetDocument()->m_pGlobalSysParms->GetKdisnBouchon()) // DM7978 - LME - bouchon KDISN
	{
		return (static_cast<int>(BouchonKdisn::getInstance()->SendKdisn(observer, msg)));
	}
	else return SendQueryMessage (observer, msg, MSG_DISPO);
}

int CResComm::Send4GY (YM_Observer *observer, CString& msg)
{
	return SendQueryMessage (observer, msg, MSG_4GY);
}

int CResComm::SendQueryMessage (YM_Observer *observer, CString& msg, VNMSG_TYPE tymess)
{
   if (!m_bAcceptedConnection || !m_pMsgSocket->IsOpen())
   {
     CString s;
     s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_SOCKETOPEN);
	 negindice --;
	 TSentMsg* pSentMsg = new TSentMsg;
     pSentMsg->msgtype = tymess;
	 pSentMsg->msgnature = NOT_VNAU_TYPE;
     pSentMsg->msgtext = msg;
     pSentMsg->pending = FALSE;
     pSentMsg->sendtime = 0;
     pSentMsg->errortext = s;
	 pSentMsg->pymmsg = NULL;
	 pSentMsg->crossmidnight = FALSE;
     m_PendingMap.SetAt (negindice, pSentMsg);
     MajTracer();
     return -1;
   }
   YM_Msg* pMsg = new YM_Msg(msg, (tymess == MSG_VNL) ? YM_Msg::MSG_VNL :
                                  (tymess == MSG_DISPO) ? YM_Msg::MSG_DISPO : YM_Msg::MSG_OTHER);
   pMsg->AddObserver(this);
   if (observer)
     pMsg->AddObserver(observer);
   m_pMsgSocket->SendMsg( pMsg );
   TSentMsg* pSentMsg = new TSentMsg;
   pSentMsg->msgtype = tymess;
   pSentMsg->msgnature = NOT_VNAU_TYPE;
   pSentMsg->msgtext = msg;
   pSentMsg->pending = TRUE;
   CTime titi = CTime::GetCurrentTime();
   pSentMsg->sendtime = static_cast<time_t>(titi.GetTime());
   pSentMsg->errortext = "";
   pSentMsg->pymmsg = NULL;
   pSentMsg->crossmidnight = FALSE;
   m_PendingMap.SetAt (pMsg->GetNum(), pSentMsg);
   MajTracer();
   return pMsg->GetNum();
}

void CResComm::OpenComm(BOOL resetbadtry)
{
  if ( m_bAcceptedConnection )
    return;
  if ( m_pMsgSocket->IsOpen() )
    m_pMsgSocket->Close();
  if (resetbadtry)
	m_pMsgSocket->nbbadtry = 0;
  m_pMsgSocket->Open (m_SrvHostName, m_iPortNumber);
/*
  if ( m_pMsgSocket->IsOpen() )
	 SendInit();
  else
	 m_pMsgSocket->Open (m_SrvHostName, m_iPortNumber);
*/
  return;
}

void CResComm::CloseComm()
{
  m_ctaclosecomm = TRUE;
  if ( m_pMsgSocket->IsOpen() )
    m_pMsgSocket->Close();
  SetConnectionFlag (FALSE);
}

void CResComm::KillIfBlocking()
{
   if ( m_pMsgSocket->IsOpen() )
     m_pMsgSocket->KillIfBlocking();
}

int CResComm::SendVnau (YM_Observer *observer, CString& msg, BOOL confirm /*TRUE*/,
					    BOOL crossMidnigthLeg /*FALSE*/,
						VNAU_MSG_TYPE tyvnax, /*VNAU_CTA_TYPE*/
						BOOL enbasedirect, /*FALSE*/
						BOOL notifyoninsert /*FALSE*/)
{
  return SendVnauBis (observer, msg, "", confirm, crossMidnigthLeg, tyvnax, enbasedirect, notifyoninsert);
}

int CResComm::SendVnauBis (YM_Observer *observer, CString& msg, CString SuiteMess, BOOL confirm /*TRUE*/,
					    BOOL crossMidnigthLeg /*FALSE*/,
						VNAU_MSG_TYPE tyvnax, /*VNAU_CTA_TYPE*/
						BOOL enbasedirect, /*FALSE*/
						BOOL notifyoninsert /*FALSE*/)
{

	/* Supprimé dans cas de la DT ALLEO - On peut maintenant envoyé des VNAU/VNAS en direct avec SATC ALLEO
	if (((CCTAApp*)APP)->GetRhealysFlag())
    return 0;
	*/

  if (confirm)
  {
    CString zMsg, ss, sss;
	sss = msg;
	if (!SuiteMess.IsEmpty())
	{
	  sss += '\n';
	  sss += SuiteMess;
	}
    ss = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CONFIRM_VNAU);
    zMsg.Format (ss, sss);
    if (AfxMessageBox (zMsg, MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL) == IDNO) 
	  return 0;
  }

  if (((!VNAUALLOWED(((CCTAApp*)APP)->m_UserRole)) &&
      (((CCTAApp*)APP)->m_SuperUser == 0)))
  {
    CString msgAff = msg;
	msgAff.Format("Not authorized to send VNAU (%s), sorry", msg);
    AfxMessageBox (msgAff, MB_OK);
    return 1;
  }
  
  CString fullmess, szDay, szMonth, szYear;
  int wday, wmonth, wyear;
  COleDateTime date;
  char conv[5];

  // Analyze of the VNAU command, to complete the parameters needed by communication
  // protocol. The format of the VNAU command follow this exemple :
  // VNAU2300/10DECFRJDQFRPLY/A0-100
  // We must complete it in the following way:
  // VNAU2300/10DECFRJDQFRPLY/A0-100 2300 FRJDQ FRPLY 12/10/1998 >
  // The year is set by the next future date for the given day /month 
  // But for safety purpose, we don't accept dpt_date more than 10 months in advance. 
  date = COleDateTime::GetCurrentTime();
  wyear = date.GetYear();
  int pfs = msg.Find ('/');
  if (pfs < 5) return 0; 
  fullmess = msg;
  fullmess += ' ';
  fullmess += msg.Mid (4, pfs-4);  // ajout de numero de tranche
  ((CCTAApp*)APP)->m_lasttranchevnau = atoi (msg.Mid (4, pfs-4)); 
  fullmess += ' ';
  if ((msg.GetAt(pfs+2) >= '0') && (msg.GetAt(pfs+2) <= '9'))
  { szDay = msg.Mid (pfs+1,2); // date sur 2 digits
    pfs += 3;
  }
  else
  { szDay = msg.Mid (pfs+1,1);
    pfs += 2;
  }
  wday = atoi ((LPCSTR)szDay);
  szMonth = msg.Mid (pfs, 3);

  // Convert into a month number
  wmonth = MONTH_LIST_ENGLISH.Find( szMonth );
  wmonth = (wmonth / 3) + 1;

  pfs += 3; // on passe les 3 caractères du mois

  // NPI - DM7838 : souplesse tarifaire
  //if ((msg.Left(4) != "VNAE") && (msg.Left(4) != "VNTP"))
  if ((msg.Left(4) != "VNAE") && (msg.Left(4) != "VNTP") && (msg.Left(4) != "VNSP"))
  {
    fullmess += msg.Mid (pfs, STATION_SIZE);
    fullmess += ' ';
    fullmess += msg.Mid (pfs+STATION_SIZE, STATION_SIZE);
    if ((msg[pfs+STATION_SIZE+STATION_SIZE+1] >= '0') &&
	    (msg[pfs+STATION_SIZE+STATION_SIZE+1] <= '9'))
	  ((CCTAApp*)APP)->m_lasttranchevnau = 0;
  }
  else
	fullmess += "XXXXX XXXXX";
  fullmess += ' ';
  if ( wmonth < date.GetMonth() )
  {
    if (date.GetMonth() - wmonth < 3)
    {
      CString ss;
      ss = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TOOFAR_VNAU);
      AfxMessageBox (ss);
      return 1;
    }
    else
      wyear += 1;
  }
  else 
  {
    if ( wmonth == date.GetMonth() )
    {
      if ( wday < date.GetDay() )
      { // Il s'agit d'une erreur de manipulation de l'utilisateur, refus
        // refus.
        CString ss;
        ss = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_YESTERDAY_VNAU);
        AfxMessageBox (ss);
        return 1;   
      }
    }
  }

  if (enbasedirect || !m_bAcceptedConnection || !m_pMsgSocket->IsOpen())
  {
    TSentMsg* pSentMsg = new TSentMsg;
    pSentMsg->msgtype = MSG_VNAU;
	pSentMsg->msgnature = tyvnax;
    pSentMsg->msgtext = msg;
    pSentMsg->pending = TRUE;
    pSentMsg->errortext = "";
	pSentMsg->pymmsg = NULL;
	pSentMsg->crossmidnight = crossMidnigthLeg;
    pSentMsg->sendtime = 0;
	PutaVnauInTable (pSentMsg, 0, notifyoninsert ? observer : NULL );
	delete pSentMsg;
    return -1;
  }

  
  RWDate dada (wday, wmonth, wyear);
  ((CCTAApp*)APP)->m_lastdatevnau = dada;
  if (crossMidnigthLeg) 
  { // Le VNAU contient la date du lendemain, mais la dpt_date dans sc_vnau reste la meme
    dada.julian (dada.julian() -1);
    wday = dada.dayOfMonth();
    wmonth = dada.month();
    wyear = dada.year();
    itoa (wday, conv, 10);
    if (wday >= 10)
      szDay = conv;
    else
    { 
      szDay = "0";
      szDay += conv;
    }
  }

  itoa (wmonth, conv, 10);
  if (wmonth < 10)
    fullmess += '0';	// le mois doit etre sur 2 digits.
  fullmess += conv;
  fullmess += '/';
  fullmess += szDay;
  fullmess += '/';
  itoa (wyear, conv, 10);
  fullmess += conv;
  fullmess += ' ';

  // Insertion du posted time
  itoa (date.GetYear(), conv, 10);
  fullmess += conv;
  fullmess += '/';
  itoa (date.GetMonth(), conv, 10);
  if (conv[1] == 0)
    fullmess += '0';     // le mois doit etre sur 2 digits.
  fullmess += conv;
  fullmess += '/';
  itoa (date.GetDay(), conv, 10);
  if (conv[1] == 0)
    fullmess += '0';     // la date doit etre sur 2 digits.
  fullmess += conv;
  fullmess += ' ';
  
  RWTime now;
  long diff = (((CCTAApp*)APP)->GetDocument()->GetDiffSysDate());
  now += diff;
  itoa (now.hour(), conv, 10);
  if (conv[1] == 0)
    fullmess += '0';     // heure sur deux digits
  fullmess += conv;
  fullmess += ':';
  itoa (now.minute(), conv, 10);
  if (conv[1] == 0)
    fullmess += '0';     // minute sur deux digits
  fullmess += conv;
  fullmess += ':';
  itoa (now.second(), conv, 10);
  if (conv[1] == 0)
    fullmess += '0';     // seconde sur deux digits
  fullmess += conv;
  fullmess += ' ';

  YM_Msg::MsgNat nat;
  if (tyvnax == VNAU_PRO_TYPE) nat = YM_Msg::MSG_PROVNAU;
  else if (tyvnax == VNAU_VNAX_TYPE) nat = YM_Msg::MSG_VNAX;
  else if (tyvnax == VNAU_VNAY_TYPE) nat = YM_Msg::MSG_VNAY;
  else if (tyvnax == VNAU_VNAZ_TYPE) nat = YM_Msg::MSG_VNAZ;
  else if (tyvnax == VNAU_NAT0_TYPE) nat = YM_Msg::MSG_VNAU_NAT0_Q;
  else if (tyvnax == VNAU_NAT1_TYPE) nat = YM_Msg::MSG_VNAU_NAT1_R;
  else if (tyvnax == VNAU_NAT2_TYPE) nat = YM_Msg::MSG_VNAU_NAT2_S;
  else if (tyvnax == VNAU_NAT3_TYPE) nat = YM_Msg::MSG_VNAU_NAT3_T;
  else nat = YM_Msg::MSG_VNAU;
  YM_Msg* pMsg = new YM_Msg(fullmess, nat);
  pMsg->AddObserver(this);
  if (observer)
    pMsg->AddObserver (observer);
	
  TSentMsg* pSentMsg = new TSentMsg;
  pSentMsg->msgtype = MSG_VNAU;
  pSentMsg->msgnature = tyvnax;
  pSentMsg->msgtext = msg;
  pSentMsg->pending = TRUE;
  pSentMsg->errortext = "";
  pSentMsg->pymmsg = pMsg;
  pSentMsg->crossmidnight = crossMidnigthLeg;
  CTime titi = CTime::GetCurrentTime();
  pSentMsg->sendtime = static_cast<time_t>(titi.GetTime());
  PutaVnauInTable (pSentMsg, now.seconds(), notifyoninsert ? observer : NULL);
  // NBN ANO 79119
  //delete pSentMsg;
  return 1;
}

BOOL CResComm::IsStillPending( int num)
{
  TSentMsg* pSentMsg;
  m_PendingMap.Lookup (num, pSentMsg);
  return ((pSentMsg && pSentMsg->pending));
}

void CResComm::PurgeErrorTrace ()
{
   POSITION pos = m_PendingMap.GetStartPosition();
   TSentMsg* pSentMsg;
   int num;

   while (pos)
   {
     m_PendingMap.GetNextAssoc (pos, num, pSentMsg);
	 if (pSentMsg)
	 { 
	   if (!pSentMsg->errortext.IsEmpty())
	   { 
	     m_PendingMap.RemoveKey (num);
		 delete pSentMsg;
	   }
	 }
   }
   MajTracer();
}

void CResComm::OnNotify( YM_Observable* pObject )
{
  if (m_stopactivity) 
    return;
  if( pObject->IsKindOf( RUNTIME_CLASS(YM_Query) ) )
  { // Notification after insertion in the database of a VNAU
    // We have now to send the message to the communication process
    YM_Query* pQuery = (YM_Query*)pObject;
	YmIcVnauDom* InsVnauDom = (YmIcVnauDom*)pQuery->PDomain();
	TSentMsg* pSentMsg = (TSentMsg*)InsVnauDom->PtToStruct;
	pSentMsg->pymmsg->SetNum (m_pMsgSocket->GetNum());
    m_PendingMap.SetAt (pSentMsg->pymmsg->GetNum(), pSentMsg);
	m_pMsgSocket->SendYmMsgLock( pSentMsg->pymmsg );
    //MajTracer();
	pQuery->SetAutoDelete (TRUE);
  }
  else 
  {
    // Did the notification come from a YM_MsgSocket ?
    if( pObject == m_pMsgSocket )
    {
      if (m_pMsgSocket->IsOpen() && !m_bAcceptedConnection )
        SendInit();
      if (!m_pMsgSocket->IsOpen() )
      {
        if (!m_ctaclosecomm)
          ((CCTAApp*)APP)->StartReconnectTimer();
        SetConnectionFlag(FALSE);
      }
	  m_ctaclosecomm = FALSE;
    }
    else 
    {
      // Did the notification come from a YM_Msg ?
      if( pObject->IsKindOf( RUNTIME_CLASS(YM_Msg) ) )
      {
        YM_Msg* pMsg = (YM_Msg*)pObject;
        TSentMsg *pSentMsg;
        if (! m_PendingMap.Lookup (pMsg->GetNum(), pSentMsg))
          return;
        pSentMsg->pending = FALSE;

        if (pMsg->GetType() == YM_Msg::SOC_REP_ERROR)
        { 
          pSentMsg->errortext = pMsg->GetMsg();
          if (pSentMsg->errortext.GetLength() == 0)
            pSentMsg->errortext = "UNDEFINED ERROR !!!";
		  if (pSentMsg->msgtype == MSG_VNAU)
		  {
		    CMainFrame* pMainFrm = (CMainFrame*) ((CCTAApp*)APP)->GetMainWnd();
			pMainFrm->WarnOnVNAU(pSentMsg->errortext);
		  }
        }

        switch (pSentMsg->msgtype)
        {
          case MSG_TEST_PRESENCE :
          {
            pSentMsg->errortext = "";
            MajTracer();
            break;
          }
          case MSG_INIT :
          {
            switch (pMsg->GetType())
            {
              case YM_Msg::SOC_REP_BUSY :
              {
                CString s;
        				((CCTAApp*)APP)->StopReconnectTimer();
                s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_BUSY_USER);
                // The socket comm is automaticaly closed by the server.
                if (AfxMessageBox (s, MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL) == IDYES)
                {
                  m_killbusy = TRUE;
                  m_pMsgSocket->Open (m_SrvHostName, m_iPortNumber);
                }
                break;
              }
              case YM_Msg::SOC_REP_ERROR :
              {
                CString s, zMsg;
                s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RES_BAD_USER);
                zMsg.Format (s, pSentMsg->msgtext);
                AfxMessageBox (zMsg, MB_ICONSTOP | MB_OK | MB_APPLMODAL);
                // The socket comm is automaticaly closed by the server.
                break;
              }
              case YM_Msg::SOC_REP_OK :
              {
                 ((CCTAApp*)APP)->StopReconnectTimer();
                SetConnectionFlag (TRUE);
                MajTracer();
                break;
              }
              break;
            }
          }
          case MSG_VNL :
		  case MSG_DISPO :
		  case MSG_4GY :
          {
            MajTracer();
            break;
          }
		  case MSG_VNAU :
		  {
            MajTracer();
			CMainFrame* pMainFrm = (CMainFrame*) ((CCTAApp*)APP)->GetMainWnd();
			if (pMsg->GetType() != YM_Msg::SOC_REP_ERROR)
			  pMainFrm->WarnOnVNAU("");
            break;
          }
        }
      }
    }
  }
}


BOOL CResComm::CanExit()
{ // Return TRUE if there is no more pending message in the m_PendingMap
  POSITION pos = m_PendingMap.GetStartPosition();
  TSentMsg* pSentMsg;
  int num;
  while (pos)
  {
    m_PendingMap.GetNextAssoc (pos, num, pSentMsg);
    if (pSentMsg && pSentMsg->pending &&
		(pSentMsg->msgtype == MSG_VNAU))
	{
	  ((CCTAApp*)APP)->ForceVnlViewer ();
	  return FALSE;
	}
  }
  return TRUE;
}

void CResComm::SetConnectionFlag (BOOL etat)
{
  m_bAcceptedConnection = etat;
  ((CCTAApp*)APP)->SetRealTimeMode (etat);

  if (m_pwndTracer && IsWindow (m_pwndTracer->m_hWnd))
  {
    CString s1 = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TRACER_TITLE);
    CString s2;
    if (etat)
      s2 = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMMUNICATION_OK);
    else s2 = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMMUNICATION_OUT);
    s1 += "  ";
    s1 += s2;
    m_pwndTracer->GetParentFrame()->SetWindowText (s1);
  }
  CString sz;
  if (etat)
    sz = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMM_OK);
  else sz = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMM_OUT);
  CMainFrame* pMainFrm = (CMainFrame*) ((CCTAApp*)APP)->GetMainWnd();
  pMainFrm->GetStatusBar()->SetPaneStyle(1, SBPS_NORMAL);
  CStatusBar* stb = (CStatusBar*)pMainFrm->GetStatusBar();
  int k = stb->CommandToIndex (ID_INDICATOR_COMM);
  if (k != -1)
    BOOL ok = pMainFrm->GetStatusBar()->SetPaneText(k, sz);
}

int  CResComm::YaOldVnauPending (int diftime)
{ 
  POSITION pos = m_PendingMap.GetStartPosition();
  TSentMsg* pSentMsg;
  int num, n;
  CTime titi = CTime::GetCurrentTime();
  n = 0;
  while (pos)
  {
    m_PendingMap.GetNextAssoc (pos, num, pSentMsg);
    if (pSentMsg && pSentMsg->pending &&
	    (pSentMsg->msgtype == MSG_VNAU) &&
		(pSentMsg->msgnature != VNAU_VNAX_TYPE) &&
		(pSentMsg->msgnature != VNAU_VNAY_TYPE) &&
		(pSentMsg->msgnature != VNAU_VNAZ_TYPE) &&
	    (pSentMsg->sendtime + diftime < titi.GetTime()))
	 n++;
  }
  return n;
}


// Put a VNAU in SC_VNAU table, and consider it as sent.
void CResComm::PutaVnauInTable (TSentMsg* pSentMsg, RW4Byte sec /*=0*/,
								YM_Observer *QueryObs /*=NULL*/)
{
  if ((pSentMsg && pSentMsg->pending) && (pSentMsg->msgtype == MSG_VNAU))
  { 
    YM_Query* pQueryInsVnau = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
    YmIcVnauDom* pInsVnauDom = new YmIcVnauDom();
    // NB, the AUTODELETE mechanism will delete the objects created above.
    switch (pSentMsg->msgnature)
	{
	  case VNAU_CTA_TYPE : pInsVnauDom->CommandSource ("U"); break;
	  case VNAU_VNAX_TYPE : pInsVnauDom->CommandSource ("X"); break;
	  case VNAU_VNAY_TYPE : pInsVnauDom->CommandSource ("Y"); break;
	  case VNAU_VNAZ_TYPE : pInsVnauDom->CommandSource ("Z"); break;
	  case VNAU_PRO_TYPE : pInsVnauDom->CommandSource ("P"); break;
	  case VNAU_NAT0_TYPE : pInsVnauDom->CommandSource ("Q"); break;
	  case VNAU_NAT1_TYPE : pInsVnauDom->CommandSource ("R"); break;
	  case VNAU_NAT2_TYPE : pInsVnauDom->CommandSource ("S"); break;
	  case VNAU_NAT3_TYPE : pInsVnauDom->CommandSource ("T"); break;
	  default : return;
	}
	pInsVnauDom->CommandText (pSentMsg->msgtext);
	pInsVnauDom->UserId( ((CCTAApp*)APP)->m_pDatabase->UserName());
	BOOL bVNTPactif = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetVNTPactifFlag();
	CString sDebVnau = pSentMsg->msgtext.Left(4);
	if (bVNTPactif || (sDebVnau != "VNTP"))
	  pInsVnauDom->StatusSend (1);
	else
	  pInsVnauDom->StatusSend (3);

    CString szDay, szMonth, szYear, szTranche;
    int wday, wmonth, wyear, wtranche;
    COleDateTime date;

    // Analyze of the VNAU command, to complete the parameters needed by communication
    // protocol. The format of the VNAU command follow this exemple :
    // VNAU2300/10DECFRJDQFRPLY/A0-100
    // We must complete it in the following way:
    // VNAU2300/10DECFRJDQFRPLY/A0-100 2300 FRJDQ FRPLY 12/10/1998 >
    // The year is set by the next future date for the given day /month  

    int pfs = pSentMsg->msgtext.Find ('/');
	szTranche = pSentMsg->msgtext.Mid (4, pfs-4);
	wtranche = atoi ((LPCSTR)szTranche);
	pInsVnauDom->TrancheNo (wtranche);

	date = COleDateTime::GetCurrentTime();
    wyear = date.GetYear();
    if ((pSentMsg->msgtext.GetAt(pfs+2) >= '0') && (pSentMsg->msgtext.GetAt(pfs+2) <= '9'))
    { szDay = pSentMsg->msgtext.Mid (pfs+1,2); // date sur 2 digits
      pfs += 3;
    }
    else
    { szDay = pSentMsg->msgtext.Mid (pfs+1,1);
      pfs += 2;
    }
    wday = atoi ((LPCSTR)szDay);
    szMonth = pSentMsg->msgtext.Mid (pfs, 3);

    // Convert into a month number
    wmonth = MONTH_LIST_ENGLISH.Find( szMonth );
    wmonth = (wmonth / 3) + 1;
    if ( wmonth < date.GetMonth() )
      wyear += 1;
    else 
    {
      if ( wmonth == date.GetMonth() )
      {
        if ( wday < date.GetDay() ) 
          wyear += 1; // Strange! Setting VNAU quite 1 year in advance !
      }
    }
    RWDate dada (wday, wmonth, wyear);
	if (pSentMsg->crossmidnight)
      pInsVnauDom->DptDate (dada.julian() -1);
	else
	  pInsVnauDom->DptDate (dada.julian());
	pfs += 3; // on passe les 3 caractères du mois
	// NPI - DM7838 : souplesse tarifaire
	//if (pSentMsg->msgtext.Left(4) != "VNAE")
	if ((pSentMsg->msgtext.Left(4) != "VNAE") && (pSentMsg->msgtext.Left(4) != "VNSP"))
    {
	  pInsVnauDom->LegOrig (pSentMsg->msgtext.Mid (pfs,STATION_SIZE));
	  pInsVnauDom->LegDest (pSentMsg->msgtext.Mid (pfs+STATION_SIZE,STATION_SIZE));
	}
	else
	{
	  pInsVnauDom->LegOrig ("XXXXX");
      pInsVnauDom->LegDest ("XXXXX");
	}
    if (sec) // dans ce cas on veut envoyer le message à la Comm
	{
	  pInsVnauDom->PostedTime (sec);
	  pInsVnauDom->PtToStruct = (int*)pSentMsg;
      // Not for a DVLPMT role, to avoid the annoying "Premission denied" message from Oracle
      if ( (!DEVELOPMENT(((CCTAApp*)APP)->m_UserRole)) ||
           (((CCTAApp*)APP)->m_SuperUser != 0) )
      {
	    pQueryInsVnau->AddObserver(this);
		if (QueryObs)
		  pQueryInsVnau->AddObserver (QueryObs);
	    pQueryInsVnau->Start (pInsVnauDom, IDS_SQL_INSERT_VNAU_SPECIFY_DATE);
      }
	}
	else 
	{
	  if (QueryObs)
		  pQueryInsVnau->AddObserver (QueryObs);
	  pQueryInsVnau->Start (pInsVnauDom, IDS_SQL_INSERT_VNAU_SYSDATE);
      pSentMsg->pending = FALSE;
	}
  }
}

// Resend a pending VNAU
void CResComm::ResendVnau (TSentMsg* pSentMsg)
{
  if (pSentMsg && pSentMsg->pending && (pSentMsg->msgtype == MSG_VNAU))
  {
    pSentMsg->pending = FALSE;
	SendVnau (NULL, pSentMsg->msgtext, FALSE, pSentMsg->crossmidnight,
				 pSentMsg->msgnature );
  }
}

void CResComm::ProcessPending (BOOL WithWarning /*= FALSE*/)
{
	// Cet affichage systématique est invivable avec le système de reconnexion automatique
    //((CCTAApp*)APP)->ForceVnlViewer ();
	POSITION pos = m_PendingMap.GetStartPosition();
    
	TSentMsg* pSentMsg;
	int num, nbv;
	nbv = 0;
    while (pos)
    {
      m_PendingMap.GetNextAssoc (pos, num, pSentMsg);
	  if (pSentMsg->pending)
	    nbv ++;
      /*if (pSentMsg && pSentMsg->pending && (pSentMsg->msgtype == MSG_VNAU))
	    PutaVnauInTable (pSentMsg);*/
	  if (pSentMsg) pSentMsg->pending = FALSE; //already inserted in the database
											   // since 8/01/99
    }
	MajTracer();

	if (WithWarning && nbv)
	{
	  CString s, msg;
      s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_ALERT_VNAU);
	  msg.Format (s, nbv);
	  AfxMessageBox (msg, MB_ICONSTOP | MB_OK);
	}
}

void CResComm::ResendPending()
{
  POSITION pos = m_PendingMap.GetStartPosition();
  CString s;
  int num;
  TSentMsg* pSentMsg;
  s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AGAIN_VNAU);
  if (AfxMessageBox (s, MB_ICONEXCLAMATION | MB_YESNO | MB_APPLMODAL) == IDYES)
  {
	while (pos)
    {
      m_PendingMap.GetNextAssoc (pos, num, pSentMsg);
      if (pSentMsg && pSentMsg->pending && (pSentMsg->msgtype == MSG_VNAU))
		ResendVnau (pSentMsg);
    }
  }
}

void CResComm::StopActivity()
{
  ProcessPending();
  m_stopactivity = TRUE;
}

void CResComm::MajTracer()
{
   if (!m_pwndTracer)
     return;
   if (!m_pwndTracer->GetSafeHwnd())
     return;
   if (!IsWindow (m_pwndTracer->m_hWnd))
     return;
   if (m_stopactivity) return;

   CDC DC;
   int fvl = m_pwndTracer->GetTopIndex();
   m_pwndTracer->ResetContent();
   POSITION pos = m_PendingMap.GetStartPosition();
   TSentMsg* pSentMsg;
   int num, i, l;
   CString zMsg, sPending, sError, negError, sshortMsg;
   sPending = "%4d %s";
   sError = "%4d ERROR %s : %s";
   negError = "     ERROR %s : %s";
   i = -1;
   int maxlength = 0;
   while (pos)
   {
     m_PendingMap.GetNextAssoc (pos, num, pSentMsg);
	 if (pSentMsg)
	 {
	   sshortMsg = pSentMsg->msgtext;
	   if (m_pwndTracer->m_shortvnaudisplay)
	     sshortMsg = sshortMsg.Left(24);
	   if (pSentMsg->pending)
	   {
	     zMsg.Format (sPending, num, sshortMsg);
		 if (m_stopactivity) return;
	     m_pwndTracer->AddString ((LPCSTR)zMsg);
		 l = zMsg.GetLength();
		 if (l > maxlength) maxlength = l;
		 i++;
	   }
	   else if (!pSentMsg->errortext.IsEmpty())
	   {
	     if (num < 0)
		   zMsg.Format (negError, sshortMsg, pSentMsg->errortext); 
		 else
		   zMsg.Format (sError, num, pSentMsg->msgtext, pSentMsg->errortext);
	     if (m_stopactivity) return;
		 m_pwndTracer->AddString ((LPCSTR)zMsg);
		 l = zMsg.GetLength();
		 if (l > maxlength) maxlength = l;
		 i++;
       }
	   else
	   { 
	     // Unesuful to delete the message, as is has been built as AutoDelete observable
         // in YMMsgsocket.cpp
	     m_PendingMap.RemoveKey (num);
		 delete pSentMsg;
	   }
	 }
   }
   if (m_stopactivity) return;
   if (i>= 0)
     m_pwndTracer->SetTopIndex (fvl); // avant : SetTopIndex(i)
   maxlength = maxlength * m_pwndTracer->m_avcharwidth;
   if (!m_pwndTracer->GetSafeHwnd())
     return;
   if (!IsWindow (m_pwndTracer->m_hWnd))
     return;
   m_pwndTracer->SetHorizontalExtent(maxlength);
   m_pwndTracer->Invalidate (TRUE);
   m_pwndTracer->UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CResTracer

CResTracer::CResTracer()
{
   m_avcharwidth = 4;		// pourquoi pas !
   m_shortvnaudisplay = FALSE;
}

CResTracer::~CResTracer()
{
}


BEGIN_MESSAGE_MAP(CResTracer, CListBox)
	//{{AFX_MSG_MAP(CResTracer)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_COMMAND (ID_VIEW_EMPTYVNAU, OnViewEmptyVnau)
	ON_COMMAND (ID_VNAU_SHORT, OnVnauShort)
	ON_COMMAND (ID_RESA_RECONNECT, OnVnauReconnect)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResTracer message handlers

int CResTracer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
    
	
	CDC* pDC = GetDC();
	pDC->SelectObject (((CCTAApp*)APP)->GetDocument()->GetFixedFont(pDC));
	TEXTMETRIC tm;
	pDC->GetTextMetrics (&tm);
	ReleaseDC (pDC);
	m_avcharwidth = tm.tmAveCharWidth + 2;	// +2 pour etre tranquille
	return 0;
}

void CResTracer::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;
	menu.LoadMenu(IDR_CTA_VNAU_POP);
	menu.EnableMenuItem( ID_VIEW_EMPTYVNAU, MF_ENABLED );
	menu.EnableMenuItem( ID_VNAU_SHORT, MF_ENABLED );
	menu.EnableMenuItem( ID_RESA_RECONNECT, MF_ENABLED );
	menu.CheckMenuItem( ID_VNAU_SHORT, m_shortvnaudisplay ? MF_CHECKED : MF_UNCHECKED  );

	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
}

void CResTracer::OnPaint()
{
  SetFont( ((CCTAApp*)APP)->GetDocument()->GetFixedFont(NULL) );
  if (GetCount() == 0)
  {
    ((CCTAApp*)APP)->GetResComm()->MajTracer();
	if (GetCount() == 0)
	  CListBox::OnPaint();
  }
  else CListBox::OnPaint();
}

void CResTracer::OnViewEmptyVnau()
{
  ((CCTAApp*)APP)->GetResComm()->PurgeErrorTrace();
}

void CResTracer::OnVnauShort()
{
  m_shortvnaudisplay = !m_shortvnaudisplay;
  Invalidate (TRUE);
  UpdateWindow();
}

void CResTracer::OnVnauReconnect()
{
    ((CCTAApp*)APP)->GetResComm()->PurgeErrorTrace();
	((CCTAApp*)APP)->GetResComm()->ProcessPending();
	((CCTAApp*)APP)->GetResComm()->CloseComm();
    ((CCTAApp*)APP)->GetResComm()->OpenComm(TRUE);
    //((CCTAApp*)APP)->SetRealTimeMode (TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CResTracerView

IMPLEMENT_DYNCREATE(CResTracerView, CView)

CResTracerView::CResTracerView()
{
}

CResTracerView::~CResTracerView()
{
}


BEGIN_MESSAGE_MAP(CResTracerView, CView)
	//{{AFX_MSG_MAP(CResTracerView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResTracerView drawing

void CResTracerView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CResTracerView diagnostics

#ifdef _DEBUG
void CResTracerView::AssertValid() const
{
	CView::AssertValid();
}

void CResTracerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CResTracerView message handlers

void CResTracerView::OnInitialUpdate() 
{
	CString titi, toto;
    titi = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TRACER_TITLE);
	CString s2;
    if (((CCTAApp*)APP)->GetResComm()->GetAcceptedConnection())
	  s2 = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMMUNICATION_OK);
    else 
	  s2 = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_COMMUNICATION_OUT);
    titi += "  ";
    titi += s2;
    GetParentFrame()->GetWindowText(toto);
	GetParentFrame()->SetWindowText(titi);
    ((YM_PersistentChildWnd*)GetParentFrame())->SetChildKey (toto);

	CRect rect (0, 0, 0, 0);
    m_tracerlist.Create( WS_CHILD|WS_VSCROLL|WS_HSCROLL|LBS_SORT, rect, this , IDR_CTA_RES_TRACER );
	((CCTAApp*)APP)->GetResComm()->SetTracer (&m_tracerlist);

	CControlBar* statusbar = (CControlBar*)((CChildFrame*)GetParentFrame())->GetStatusBar();
    GetParentFrame()->ShowControlBar (statusbar, FALSE, FALSE);
    CView::OnInitialUpdate();
}

void CResTracerView::OnClose()
{
  ((CCTAApp*)APP)->GetResComm()->SetTracer (NULL);
  CView::OnClose();
}

void CResTracerView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	CRect rect;
	CView::OnSize(nType, cx, cy);
	GetClientRect(rect);
	if (::IsWindow(m_tracerlist))
	  m_tracerlist.SetWindowPos(&wndTop, 0, 0, rect.right - rect.left,
						rect.bottom - rect.top, SWP_SHOWWINDOW);	
}

