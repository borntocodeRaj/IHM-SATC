// ResTracer.h : header file
#if !defined(AFX_VNLVNAUTRACER_H__73B3C601_D9EC_11D1_91D7_400020302A10__INCLUDED_)
#define AFX_VNLVNAUTRACER_H__73B3C601_D9EC_11D1_91D7_400020302A10__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class YM_RecordSet;

/////////////////////////////////////////////////////////////////////////////
// CResTracer window

class CResTracer : public CListBox
{
// Construction
public:
	CResTracer();

// Attributes
public:

// Operations
public:
	int m_avcharwidth;
	BOOL m_shortvnaudisplay;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResTracer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CResTracer();

	// Generated message map functions
protected:
	//{{AFX_MSG(CResTracer)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg void OnViewEmptyVnau();
	afx_msg void OnVnauShort();
	afx_msg void OnVnauReconnect();

	DECLARE_MESSAGE_MAP()
};


typedef enum
{
	MSG_VNL,
	MSG_DISPO,
	MSG_4GY,
	MSG_VNAU,
	MSG_INIT,
	MSG_TEST_PRESENCE
} VNMSG_TYPE;

typedef enum
{
	NOT_VNAU_TYPE,
	VNAU_CTA_TYPE,
	VNAU_VNAX_TYPE,
	VNAU_PRO_TYPE,
	VNAU_VNAY_TYPE,
	VNAU_NAT0_TYPE,
	VNAU_NAT1_TYPE,
	VNAU_NAT2_TYPE,
	VNAU_NAT3_TYPE,
	VNAU_VNAZ_TYPE
} VNAU_MSG_TYPE;


// The structure below is used to memorize the message sent via socket.
typedef struct SentMsgStruct
{
	VNMSG_TYPE	msgtype;
	VNAU_MSG_TYPE msgnature;
	CString		msgtext;	// Text of the initial message
	BOOL			pending;	// TRUE while we have'nt receive the response.
	time_t		sendtime;   // heure d'envoi.
	CString		errortext;	// contains error message, if any, else it is empty.
		// The text of the answer will be retrieve by the Observer via the YM_Msg sent
		// as notify parameter
	YM_Msg*		pymmsg;      // Memorisation du YM_Msg qui lui est lie.
	BOOL			crossmidnight; // indique si cas d'un VNAU passé minuit.

} TSentMsg;

 
/////////////////////////////////////////////////////////////////////////////
// CResComm class

class CResComm : public YM_Observer
{
public:
	CResComm(); // will start initialisation and socket opening
	~CResComm(); // Also close the socket communication

// Implementation
public:
	// Give a pointer on the VNAU trace window
	void SetTracer (CResTracer *wndTracer);
	void SetHostName( CString szSrvHostName ) { m_SrvHostName = szSrvHostName; }
	void SetPortNumber( int iPortNumber ) { m_iPortNumber = iPortNumber; }
	YM_MsgSocket* GetMsgSocket() { return m_pMsgSocket; }

	virtual void OpenComm(BOOL resetbadtry = FALSE);
	virtual void CloseComm();
	virtual BOOL CanExit();
	virtual void StopActivity();
  void KillIfBlocking();
	virtual BOOL GetAcceptedConnection() { return m_bAcceptedConnection; };

	// This function return the number of VNAU messages still pending older
	// than "diftime" second
	int  YaOldVnauPending (int diftime);

	// Function proposing to write Pending VNAU in SC_VNAU table
	void ProcessPending (BOOL WithWarning = FALSE);

	// Function proposing to resend Pending VNAU
	void ResendPending();

	// functions below return -1, if we are not able to sent message,
	// in fact because initialisation are not finished
	int SendVnl (YM_Observer *observer, CString& msg);
	int SendDispo (YM_Observer *observer, CString& msg);
	int Send4GY (YM_Observer *observer, CString& msg);

	int SendVnauBis (YM_Observer *observer, CString& msg, CString SuiteMess, BOOL confirm = TRUE,
		   BOOL crossMidnigthLeg = FALSE,
		   VNAU_MSG_TYPE tyvnax = VNAU_CTA_TYPE,
		   BOOL enbasedirect = FALSE,
		   BOOL notifyoninsert = FALSE);
	// Même chose que SendVnau, mais concatene '\n' + SuiteMess à msg pour poser la question de confirmation.

	int SendVnau (YM_Observer *observer, CString& msg, BOOL confirm = TRUE,
		   BOOL crossMidnigthLeg = FALSE,
		   VNAU_MSG_TYPE tyvnax = VNAU_CTA_TYPE,
		   BOOL enbasedirect = FALSE,
		   BOOL notifyoninsert = FALSE);
	// Return 1 if OK
	// Return 0, if confirm is TRUE, and the user decide to not send the VNAU
	// Return -1, if there is a problem with the communication server.
	//
	// Then the VNAU has been simply inserted in the SC_VNAU table.
	// tyvnax must be set to VNAU_VNAX_TYPE if SendVnau used by VNAX tool
	// tyvnax must be set to VNAU_VNAY_TYPE if SendVnau used by VNAY tool
	// tyvnax must be set to VNAU_PRO_TYPE if SendVnau used by manual VNAU
	// For SendVnl and SendVnau, when we will receive the response, we will notify
	// the observer indicated in parameter : observer->OnNotify (YM_MSG*)
	// The pObject type in parameter of this OnNotify will be
	//		IsKindOf( RUNTIME_CLASS(YM_Msg))
	// 
	// You can retreive the answer with the YM_Msg::GetMsg() method.
	// YM_Msg::GetNum() is the number sent back by the above functions
	// If you don't want to be notify, send NULL as *observer parameter
	//
	// For SendVnau, if tyvnax is not VNAU_VNAX_TYPE or VNAU_VNAY_TYPE, we wait the response of
	// communication server, before going out of the function. If after a "laps
	// of time", the message is still pending, we consider that there is a 
	// communication problem, and we return -1.
	//
	// If enbasedirect = TRUE :the VNAU is inserted in the database, but not sent to COMM
	// If notifyoninsert is TRUE, the "observer" is notify on VNAU insertion in database

	void PurgeErrorTrace ();
	// PurgeErrorTrace, remove the Error trace from the VNAU tracer window.

	void OnNotify( YM_Observable* pObject );

	BOOL IsStillPending( int num);
	// Return TRUE if the message number 'num' is still pending.

	void SendTest (BOOL manuel);
	void VerifPresence();
	void MajTracer();

private:
	static int negindice;

protected:	// DM 7978 - moved from private to protected so we can implement a bouchon
    BOOL m_bAcceptedConnection;
	int  m_iPortNumber;
	CString m_SrvHostName;
	CMap <int, int, TSentMsg*, TSentMsg*&> m_PendingMap;
	YM_MsgSocket* m_pMsgSocket;
	CResTracer* m_pwndTracer;
	YM_RecordSet* m_pSysParmSet;
	CString		  m_sKeySysParm;
	bool m_killbusy;
	bool m_stopactivity;
	bool m_ctaclosecomm;

protected:
	int SendInit();

    // Put a VNAU in SC_VNAU table, and consider it as sent.
    void PutaVnauInTable (TSentMsg* pSentMsg, RW4Byte sec = 0, YM_Observer *QueryObs = NULL);
    void ResendVnau (TSentMsg* pSentMsg);
	void SetConnectionFlag(BOOL etat);
	int SendQueryMessage (YM_Observer *observer, CString& msg, VNMSG_TYPE tymess);
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CResTracerView view

class CResTracerView : public CView
{
protected:
	CResTracerView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CResTracerView)

// Attributes
public:
    CResTracer	m_tracerlist;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResTracerView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CResTracerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CResTracerView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VNLVNAUTRACER_H__73B3C601_D9EC_11D1_91D7_400020302A10__INCLUDED_)
