#if !defined(AFX_QUOTAEDIT_H__64DE898C_E3BA_4BA2_94D6_0DD1BBF121C9__INCLUDED_)
#define AFX_QUOTAEDIT_H__64DE898C_E3BA_4BA2_94D6_0DD1BBF121C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QuotaEdit.h : header file
//

//classe servant a ajouter des infos aux VNAU envoyés par les boîtes de bascule de quotas
class CQuotaVnau
{
public:
  CQuotaVnau (CString sMess, BOOL bCrossMidnight, int nature);
  CString m_mess;
  BOOL    m_bCrossMidnight;
  BOOL    m_bSent;
  int     m_nature;
};

/////////////////////////////////////////////////////////////////////////////
// CQuotaEdit window

class CQuotaEdit : public CEdit
{
// Construction
public:
	CQuotaEdit();

// Attributes
public:
	BOOL m_bBucket0;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuotaEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQuotaEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CQuotaEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUOTAEDIT_H__64DE898C_E3BA_4BA2_94D6_0DD1BBF121C9__INCLUDED_)
