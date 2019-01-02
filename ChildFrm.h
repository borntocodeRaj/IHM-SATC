// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__2017F0DB_3EE9_11D1_8369_400020302A31__INCLUDED_)
#define AFX_CHILDFRM_H__2017F0DB_3EE9_11D1_8369_400020302A31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000




class CChildFrame : public YM_PersistentChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)

#define SPLITTER_ROW_COUNT  "SPLITTER_ROW_COUNT"
#define SPLITTER_ROW_0_SIZE  "SPLITTER_ROW_0_SIZE"
#define SPLITTER_ROW_1_SIZE  "SPLITTER_ROW_1_SIZE"

#define SPLITTER_COL_COUNT  "SPLITTER_COL_COUNT"
#define SPLITTER_COL_0_SIZE  "SPLITTER_COL_0_SIZE"
#define SPLITTER_COL_1_SIZE  "SPLITTER_COL_1_SIZE"

protected:
	CChildFrame();

public:
  YM_StatusBar *GetStatusBar() { return &m_wndStatusBar; }
  BOOL GetAltFlag() { return m_bAlt; }
  void SetAltFlag(BOOL bAlt) 
  {
	  m_bAlt = bAlt; 
  }

// Attributes
public:

protected:
	YM_StatusBar  m_wndStatusBar;
  BOOL m_bAlt;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

 	BOOL CanCloseFrame() { return TRUE; };

// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnMDIActivate( BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd );
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void OnUpdateComm (CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()
};


class CSplitChildFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CSplitChildFrame)
public:
	CSplitChildFrame();

  CSplitterWnd* GetSplitterWnd() { return &m_wndSplitter; }

// Attributes
public:

protected:
	CSplitterWnd  m_wndSplitter;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitChildFrame)
	public:
	virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSplitChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

  virtual void WriteSplitterConfig(CString szProfileKey) { return; }
  virtual void RestoreSplitterConfig(CString szProfileKey) { return; }

// Generated message map functions
protected:
	//{{AFX_MSG(CSplitChildFrame)
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

class CSplitChildFrameV : public CSplitChildFrame
{
	DECLARE_DYNCREATE(CSplitChildFrameV)
public:
	CSplitChildFrameV();
protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitChildFrameV)
	virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSplitChildFrameV();

  void WriteSplitterConfig(CString szProfileKey);
  void RestoreSplitterConfig(CString szProfileKey);

// Generated message map functions
protected:
	//{{AFX_MSG(CSplitChildFrameV)
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

class CSplitChildFrameH : public CSplitChildFrame
{
	DECLARE_DYNCREATE(CSplitChildFrameH)
public:
	CSplitChildFrameH();
protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitChildFrameH)
	virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSplitChildFrameH();

  void WriteSplitterConfig(CString szProfileKey);
  void RestoreSplitterConfig(CString szProfileKey);

// Generated message map functions
protected:
	//{{AFX_MSG(CSplitChildFrameH)
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__2017F0DB_3EE9_11D1_8369_400020302A31__INCLUDED_)
