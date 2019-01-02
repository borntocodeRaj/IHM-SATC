// QuotaEdit.cpp : implementation file
//

#include "StdAfx.h"

#include "QuotaEdit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//classe servant a ajouter des infos aux VNAU envoyés par les boîtes de bascule de quotas
CQuotaVnau::CQuotaVnau (CString sMess, BOOL bCrossMidnight, int nature)
{
  m_mess = sMess;
  m_bCrossMidnight = bCrossMidnight;
  m_bSent = FALSE;
  m_nature = nature;
}

/////////////////////////////////////////////////////////////////////////////
// CQuotaEdit

CQuotaEdit::CQuotaEdit()
{
  m_bBucket0 = FALSE;
}

CQuotaEdit::~CQuotaEdit()
{
}


BEGIN_MESSAGE_MAP(CQuotaEdit, CEdit)
	//{{AFX_MSG_MAP(CQuotaEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuotaEdit message handlers

void CQuotaEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if ((nChar < '0') || (nChar >'9'))
	  return;
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CQuotaEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
    if (nChar == 0x26)
	{
	  INPUT input[4];
	  ::ZeroMemory (input, sizeof(input));
      input[0].type = input[1].type = input[2].type = input[3].type = INPUT_KEYBOARD;
      input[0].ki.wVk = input[3].ki.wVk = VK_SHIFT;
	  input[1].ki.wVk = input[2].ki.wVk = VK_TAB;
	  input[2].ki.dwFlags = input[3].ki.dwFlags = KEYEVENTF_KEYUP;
	  SendInput(4, input, sizeof(INPUT));
	  return;
	}

	if (nChar == 0x28)
	{
	  nChar = 0x09;
	  PostMessage(WM_KEYDOWN, nChar, 0);
	  return;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CQuotaEdit::OnPaint()
{
  if (m_bBucket0)
	  this->SetFont (((CCTAApp*)APP)->GetDocument()->GetTitleFont(NULL));
  CEdit::OnPaint();
}
