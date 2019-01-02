// EntityRrd.cpp : implementation file
//

#include "stdafx.h"







#include "EntityRrd.h"


// CEntityRrd dialog

IMPLEMENT_DYNAMIC(CEntityRrd, CDialog)

CEntityRrd::CEntityRrd(CWnd* pParent /*=NULL*/)
	: CDialog(CEntityRrd::IDD, pParent)
{
 int i = 0;
 for (i = 0; i < MAX_NB_RRD; i++)
 {
   m_defaultRrd_Idx[i] = 0;
   m_defaultRrd_Flag[i] = 0;
 }
 m_nbRrd = 0;
}

CEntityRrd::~CEntityRrd()
{
}

void CEntityRrd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUR_ENTITY_LIST, m_comboCurEntity);
	DDX_Control(pDX, IDC_FULL_ENTITY_LIST, m_comboFullEntity);
}


BEGIN_MESSAGE_MAP(CEntityRrd, CDialog)
	ON_CBN_SELCHANGE(IDC_CUR_ENTITY_LIST, &CEntityRrd::OnCbnSelchangeCurEntityList)
	ON_CBN_SELCHANGE(IDC_FULL_ENTITY_LIST, &CEntityRrd::OnCbnSelchangeFullEntityList)
	ON_BN_CLICKED(IDIDEM, &CEntityRrd::OnBnClickedIdem)
	ON_BN_CLICKED(IDINITIALES, &CEntityRrd::OnBnClickedInitiales)
	ON_BN_CLICKED(IDDEFAULT, &CEntityRrd::OnBnClickedDefault)
	ON_BN_CLICKED(IDOK, &CEntityRrd::OnBnClickedSave)
	ON_BN_CLICKED(IDCANCEL, &CEntityRrd::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_ERRD0, &CEntityRrd::OnBnClickedErrd0)
	ON_BN_CLICKED(IDC_ERRD1, &CEntityRrd::OnBnClickedErrd1)
	ON_BN_CLICKED(IDC_ERRD2, &CEntityRrd::OnBnClickedErrd2)
	ON_BN_CLICKED(IDC_ERRD3, &CEntityRrd::OnBnClickedErrd3)
	ON_BN_CLICKED(IDC_ERRD4, &CEntityRrd::OnBnClickedErrd4)
	ON_BN_CLICKED(IDC_ERRD5, &CEntityRrd::OnBnClickedErrd5)
	ON_BN_CLICKED(IDC_ERRD6, &CEntityRrd::OnBnClickedErrd6)
	ON_BN_CLICKED(IDC_ERRD7, &CEntityRrd::OnBnClickedErrd7)
	ON_BN_CLICKED(IDC_ERRD8, &CEntityRrd::OnBnClickedErrd8)
	ON_BN_CLICKED(IDC_ERRD9, &CEntityRrd::OnBnClickedErrd9)
	ON_BN_CLICKED(IDC_ERRD10, &CEntityRrd::OnBnClickedErrd10)
	ON_BN_CLICKED(IDC_ERRD11, &CEntityRrd::OnBnClickedErrd11)
	ON_BN_CLICKED(IDC_ERRD12, &CEntityRrd::OnBnClickedErrd12)
	ON_BN_CLICKED(IDC_ERRD13, &CEntityRrd::OnBnClickedErrd13)
	ON_BN_CLICKED(IDC_ERRD14, &CEntityRrd::OnBnClickedErrd14)
	ON_BN_CLICKED(IDC_ERRD15, &CEntityRrd::OnBnClickedErrd15)
	ON_BN_CLICKED(IDC_ERRD16, &CEntityRrd::OnBnClickedErrd16)
	ON_BN_CLICKED(IDC_ERRD17, &CEntityRrd::OnBnClickedErrd17)
	ON_BN_CLICKED(IDC_ERRD18, &CEntityRrd::OnBnClickedErrd18)
	ON_BN_CLICKED(IDC_ERRD19, &CEntityRrd::OnBnClickedErrd19)
	ON_BN_CLICKED(IDC_ERRD20, &CEntityRrd::OnBnClickedErrd20)
	ON_BN_CLICKED(IDC_ERRD21, &CEntityRrd::OnBnClickedErrd21)
	ON_BN_CLICKED(IDC_ERRD22, &CEntityRrd::OnBnClickedErrd22)
	ON_BN_CLICKED(IDC_ERRD23, &CEntityRrd::OnBnClickedErrd23)
	ON_BN_CLICKED(IDC_ERRD24, &CEntityRrd::OnBnClickedErrd24)
	ON_BN_CLICKED(IDC_ERRD25, &CEntityRrd::OnBnClickedErrd25)
	ON_BN_CLICKED(IDC_ERRD26, &CEntityRrd::OnBnClickedErrd26)
	ON_BN_CLICKED(IDC_ERRD27, &CEntityRrd::OnBnClickedErrd27)
	ON_BN_CLICKED(IDC_ERRD28, &CEntityRrd::OnBnClickedErrd28)
	ON_BN_CLICKED(IDC_ERRD29, &CEntityRrd::OnBnClickedErrd29)
	ON_BN_CLICKED(IDC_ERRD30, &CEntityRrd::OnBnClickedErrd30)
	ON_BN_CLICKED(IDC_ERRD31, &CEntityRrd::OnBnClickedErrd31)
	ON_BN_CLICKED(IDC_ERRD32, &CEntityRrd::OnBnClickedErrd32)
	ON_BN_CLICKED(IDC_ERRD33, &CEntityRrd::OnBnClickedErrd33)
	ON_BN_CLICKED(IDC_ERRD34, &CEntityRrd::OnBnClickedErrd34)
	ON_BN_CLICKED(IDC_ERRD35, &CEntityRrd::OnBnClickedErrd35)
	ON_BN_CLICKED(IDC_ERRD36, &CEntityRrd::OnBnClickedErrd36)
	ON_BN_CLICKED(IDC_ERRD37, &CEntityRrd::OnBnClickedErrd37)
	ON_BN_CLICKED(IDC_ERRD38, &CEntityRrd::OnBnClickedErrd38)
	ON_BN_CLICKED(IDC_ERRD39, &CEntityRrd::OnBnClickedErrd39)
	ON_BN_CLICKED(IDC_ERRD40, &CEntityRrd::OnBnClickedErrd40)
	ON_BN_CLICKED(IDC_ERRD41, &CEntityRrd::OnBnClickedErrd41)
	ON_BN_CLICKED(IDC_ERRD42, &CEntityRrd::OnBnClickedErrd42)
	ON_BN_CLICKED(IDC_ERRD43, &CEntityRrd::OnBnClickedErrd43)
	ON_BN_CLICKED(IDC_ERRD44, &CEntityRrd::OnBnClickedErrd44)
	ON_BN_CLICKED(IDC_ERRD45, &CEntityRrd::OnBnClickedErrd45)
	ON_BN_CLICKED(IDC_ERRD46, &CEntityRrd::OnBnClickedErrd46)
	ON_BN_CLICKED(IDC_ERRD47, &CEntityRrd::OnBnClickedErrd47)
	ON_BN_CLICKED(IDC_ERRD48, &CEntityRrd::OnBnClickedErrd48)
	ON_BN_CLICKED(IDC_ERRD49, &CEntityRrd::OnBnClickedErrd49)
	ON_BN_CLICKED(IDC_ERRD50, &CEntityRrd::OnBnClickedErrd50)
	ON_BN_CLICKED(IDC_ERRD51, &CEntityRrd::OnBnClickedErrd51)
	ON_BN_CLICKED(IDC_ERRD52, &CEntityRrd::OnBnClickedErrd52)
	ON_BN_CLICKED(IDC_ERRD53, &CEntityRrd::OnBnClickedErrd53)
	ON_BN_CLICKED(IDC_ERRD54, &CEntityRrd::OnBnClickedErrd54)
	ON_BN_CLICKED(IDC_ERRD55, &CEntityRrd::OnBnClickedErrd55)
	ON_BN_CLICKED(IDC_ERRD56, &CEntityRrd::OnBnClickedErrd56)
	ON_BN_CLICKED(IDC_ERRD57, &CEntityRrd::OnBnClickedErrd57)
	ON_BN_CLICKED(IDC_ERRD58, &CEntityRrd::OnBnClickedErrd58)
	ON_BN_CLICKED(IDC_ERRD59, &CEntityRrd::OnBnClickedErrd59)
	ON_BN_CLICKED(IDC_ERRD60, &CEntityRrd::OnBnClickedErrd60)
END_MESSAGE_MAP()

BOOL CEntityRrd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InitFullEntity();
	GetDefaultRrd();

	CString sEntities = ((CCTAApp*)APP)->GetDocument()->GetSelectedEntities();
	//Liste des entitées avec quotes et virgule
	int curpos = 0;
	int index = 0;
	CString stok = sEntities.Tokenize ("\',", curpos); 
	while (!stok.IsEmpty())
	{
	  stok.MakeUpper();
	  if (stok.Find("AUTO") == -1) // ce n'est pas une entité autotrain
	  {
	    m_comboCurEntity.InsertString(index, stok);
	    index ++;
	  }
	  stok = sEntities.Tokenize ("\',", curpos); 
	}
	m_comboCurEntity.SetCurSel(0);

	CString sEntity;
	m_comboCurEntity.GetWindowTextA(sEntity);
	GetEntityRrd(sEntity);
	MemoInitialValue();
    ((CButton*)GetDlgItem (IDINITIALES))->EnableWindow(false);
	((CButton*)GetDlgItem (IDC_ERRD_P1))->SetCheck(1);
	((CButton*)GetDlgItem (IDC_ERRD_P1))->EnableWindow(false);
	((CButton*)GetDlgItem (IDC_ERRD0))->SetCheck(1);
	((CButton*)GetDlgItem (IDC_ERRD0))->EnableWindow(false);
	CString ss;
	ss.Format ("%d", MAXNBJMX);
	((CWnd*)GetDlgItem(IDC_MAX_FLAG2))->SetWindowTextA(ss);

	return TRUE;
}

void CEntityRrd::InitFullEntity()
{
  m_comboFullEntity.ResetContent();
  YmIcEntitiesDom EntityDom;
  CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
  YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
  pSQL->SetDomain(&EntityDom);
  int index = 0;
  RWDBReader Reader( WinApp->m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_ENTITY_WITH_RRD) );
  while (Reader())
  {
    if (Reader.isValid())
	{
	  Reader >> EntityDom;
	  m_comboFullEntity.InsertString (index, EntityDom.Entity());
	  index++;
	}
  }
  delete pSQL;

  if (index)
    m_comboFullEntity.SetCurSel(0);
}

void CEntityRrd::GetDefaultRrd()
{
  YmIcRdDaysDom RrdDom;
  CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
  YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
  pSQL->SetDomain(&RrdDom);
  int j = 0;
  CString sss;
  RWDBReader Reader( WinApp->m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_DEF_FLAGUSAGE) );
  // la requête ne récupere que les J-X >= 1. L'entrée 0 des tableaux ne sert pas a grand chose, car bloquée
  m_defaultRrd_Idx[0] = -1;
  m_defaultRrd_Flag[0] = 2;
  while (Reader())
  {
    if (Reader.isValid())
	{
	  Reader >> RrdDom;
	  sss.Format ("J-%d", RrdDom.Rrd());
	  ((CButton*)GetDlgItem (IDC_ERRD0+j))->SetWindowTextA (sss);
	  m_defaultRrd_Idx[j] = RrdDom.Rrd();
	  m_defaultRrd_Flag[j] = RrdDom.FlagUsage();
	  if (RrdDom.FlagUsage() == 2)
	   ((CButton*)GetDlgItem (IDC_ERRD0+j))->SetCheck(1); 
	  j++;
	  if (j == 61)
	    break;
	}
  }
  m_nbRrd = j-1;
  while (j <= 60)
  {
	((CButton*)GetDlgItem (IDC_ERRD0+j))->ShowWindow(SW_HIDE);
	j++;
  }
  delete pSQL;
  CountNbChecked();
}

void CEntityRrd::GetEntityRrd(CString entity)
{
  YmIcRdDaysDom RrdDom;
  int entityRrd_Flag[61];
  CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
  YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
  pSQL->SetDomain(&RrdDom);
  RrdDom.UserId(entity); // utilisation du champ user_id pour passer l'entité à la requete
  int j = 0;
  for (j = 0; j < MAX_NB_RRD; j++)
  { // initialisation avec les indices de ym_default_rrd, mais en forcant les flag_usage à 1
	entityRrd_Flag[j] = 1;
  }
  RWDBReader Reader( WinApp->m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_ENT_FLAGUSAGE) );
  j = 0;
  while (Reader())
  { // la requête ne donne que les flag_usage à 2.
    if (Reader.isValid())
	{
	  Reader >> RrdDom;
	  int k = 0;
	  int z = 0;
	  while (k < MAX_NB_RRD)
	  {
	    z = m_defaultRrd_Idx[k];
		if (z == RrdDom.Rrd())
		{
		  entityRrd_Flag[k] = 2;
		  k = MAX_NB_RRD;
		}
		else k++;
	  }
	  j++;
	}
  }
  delete pSQL;

  if (!j)
  { // on a rien lu dans ym_entity_rrd, on revient au flag par defaut
    for (j = 0; j < MAX_NB_RRD; j++)
	 entityRrd_Flag[j] = m_defaultRrd_Flag[j];
  }
  else
  { // il faut changer les Check
	for (j = 0; j <= m_nbRrd; j++)
	{
	  if (entityRrd_Flag[j] == 2)
	    ((CButton*)GetDlgItem (IDC_ERRD0+j))->SetCheck(1);
	  else
	    ((CButton*)GetDlgItem (IDC_ERRD0+j))->SetCheck(0);
	}
  }
  CountNbChecked();
  bool bidef = CtrlIdemDefaut();
  ((CButton*)GetDlgItem (IDDEFAULT))->EnableWindow(!bidef);
}

bool CEntityRrd::CtrlIdemDefaut()
{
  int j;
  bool bdef, bcur;
  for (j=0; j <= m_nbRrd; j++)
  {
    bcur = (((CButton*)GetDlgItem (IDC_ERRD0+j))->GetCheck() != 0);
		bdef = (m_defaultRrd_Flag[j] == 2);
		if (bdef != bcur)
			return false;
  }
  return true;
}

int CEntityRrd::CountNbChecked()
{
  int nb = 0;
  int j;
  for (j=0; j <= m_nbRrd; j++)
  {
    if ( ((CButton*)GetDlgItem (IDC_ERRD0+j))->GetCheck())
	  nb++;
  }
  CString ss;
  ss.Format("%d", nb);
  ((CWnd*)GetDlgItem(IDC_NB_JMX))->SetWindowTextA(ss);
  return nb;
}

void CEntityRrd::MemoInitialValue()
{
  int j;
  for (j = 0; j < MAX_NB_RRD; j++)
  { // initialisation avec les indices de ym_default_rrd, mais en forcant les flag_usage à 1
	m_entityRrd_Flag[j] = 1;
  }

  for (j=0; j <= m_nbRrd; j++)
  {
    if ( ((CButton*)GetDlgItem (IDC_ERRD0+j))->GetCheck())
	  m_entityRrd_Flag[j] = 2;
  }
  ((CButton*)GetDlgItem (IDOK))->EnableWindow(false);
}

bool CEntityRrd::YaDesModif()
{
  int j;
  bool bdef, bcur;
  for (j=0; j <= m_nbRrd; j++)
  {
    bcur = (((CButton*)GetDlgItem (IDC_ERRD0+j))->GetCheck() != 0);
		bdef = (m_entityRrd_Flag[j] == 2);
		if (bdef != bcur)
			return true;
  }
  return false;
}

void CEntityRrd::OnAnyCheckRrd()
{
   bool bmodif = YaDesModif(); 
  ((CButton*)GetDlgItem (IDOK))->EnableWindow(bmodif);
  ((CButton*)GetDlgItem (IDINITIALES))->EnableWindow(bmodif);
  CountNbChecked();
   bool bidef = CtrlIdemDefaut();
  ((CButton*)GetDlgItem (IDDEFAULT))->EnableWindow(!bidef);
}


// CEntityRrd message handlers

void CEntityRrd::OnCbnSelchangeCurEntityList()
{
	OnBnClickedDefault();
	CString sEntity;
	m_comboCurEntity.GetWindowTextA(sEntity);
	GetEntityRrd(sEntity);
	MemoInitialValue();
	((CButton*)GetDlgItem (IDINITIALES))->EnableWindow(false);
}

void CEntityRrd::OnCbnSelchangeFullEntityList()
{
	// TODO: Add your control notification handler code here
}

void CEntityRrd::OnBnClickedIdem()
{
	CString sEntity;
	m_comboFullEntity.GetWindowTextA(sEntity);

	if (!sEntity.IsEmpty())
	  GetEntityRrd(sEntity);
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedInitiales()
{
  OnBnClickedDefault(); // remise au valeur par defaut
  OnCbnSelchangeCurEntityList(); // rechargement des info de l'entité courante
  ((CButton*)GetDlgItem (IDINITIALES))->EnableWindow(false);
}

void CEntityRrd::OnBnClickedDefault()
{
  int j;
  for (j = 0; j <= m_nbRrd; j++)
  {
	if (m_defaultRrd_Flag[j] == 2)
	  ((CButton*)GetDlgItem (IDC_ERRD0+j))->SetCheck(1);
	else
	  ((CButton*)GetDlgItem (IDC_ERRD0+j))->SetCheck(0);
  }
  OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedSave()
{
  int kk = CountNbChecked();
  if (kk > MAXNBJMX)
  {
    CString ss = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_TOOMUCH_RRD);
	CString mess;
	mess.Format (ss, kk, MAXNBJMX); 
	AfxMessageBox(mess,  MB_ICONEXCLAMATION | MB_APPLMODAL);
	return;
  }
  ((CButton*)GetDlgItem (IDOK))->ShowWindow(SW_HIDE);
  ((CButton*)GetDlgItem (IDCANCEL))->ShowWindow(SW_HIDE);
  int j;
  CString sEntity;
  m_comboCurEntity.GetWindowTextA(sEntity);
  YmIcRdDaysDom RrdDom;
  CCTAApp *WinApp = (CCTAApp*)AfxGetApp();
  YM_Query* pSQL = new YM_Query (*(WinApp->m_pDatabase), FALSE);
  pSQL->SetDomain(&RrdDom);
  RrdDom.UserId(sEntity); // utilisation du champ user_id pour passer l'entité à la requete

  // On vide la table YM_ENTITY_RRD pour cette entité
  WinApp->m_pDatabase->Transact(pSQL, IDS_SQL_DELETE_ENTITY_RRD);
  CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
  pDoc->ResetCurEntityRrd();
  if (CtrlIdemDefaut())
  { // identique au default, on se contente de la suppression de YM_ENTITY_RRD
    delete pSQL;
	InitFullEntity(); // remise à jour de la liste d'entité copiable
	((CButton*)GetDlgItem (IDOK))->ShowWindow(SW_SHOW);
    ((CButton*)GetDlgItem (IDCANCEL))->ShowWindow(SW_SHOW);
	return;
  }

  // on recopie pour l'entité la totalité de la ym_defaut_rdd, mais avec les flag_usage=2 remis à 1;
  WinApp->m_pDatabase->Transact(pSQL, IDS_SQL_INIT_ENTITY_RRD); 

  RrdDom.Rrd(-1);
  WinApp->m_pDatabase->Transact(pSQL, IDS_SQL_FLAG_ENTITY_RRD); 
  // On positionne les flag_usage à 2
  for (j=0; j <= m_nbRrd; j++)
  {
    if (((CButton*)GetDlgItem (IDC_ERRD0+j))->GetCheck())
	{
	  RrdDom.Rrd(m_defaultRrd_Idx[j]);
	  WinApp->m_pDatabase->Transact(pSQL, IDS_SQL_FLAG_ENTITY_RRD); 
	}
  }
  delete pSQL;
  InitFullEntity(); // remise à jour de la liste d'entité copiable
  ((CButton*)GetDlgItem (IDOK))->ShowWindow(SW_SHOW);
  ((CButton*)GetDlgItem (IDCANCEL))->ShowWindow(SW_SHOW);
  MemoInitialValue();
  ((CButton*)GetDlgItem (IDINITIALES))->EnableWindow(false);
}

void CEntityRrd::OnBnClickedCancel()
{
	OnCancel();
}

void CEntityRrd::OnBnClickedErrd0()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd1()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd2()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd3()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd4()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd5()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd6()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd7()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd8()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd9()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd10()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd11()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd12()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd13()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd14()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd15()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd16()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd17()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd18()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd19()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd20()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd21()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd22()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd23()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd24()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd25()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd26()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd27()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd28()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd29()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd30()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd31()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd32()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd33()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd34()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd35()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd36()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd37()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd38()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd39()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd40()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd41()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd42()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd43()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd44()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd45()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd46()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd47()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd48()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd49()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd50()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd51()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd52()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd53()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd54()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd55()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd56()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd57()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd58()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd59()
{
	OnAnyCheckRrd();
}

void CEntityRrd::OnBnClickedErrd60()
{
	OnAnyCheckRrd();
}

