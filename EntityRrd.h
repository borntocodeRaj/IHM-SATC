#pragma once

#define MAX_NB_RRD 61

// CEntityRrd dialog

class CEntityRrd : public CDialog
{
	DECLARE_DYNAMIC(CEntityRrd)

public:
	CEntityRrd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEntityRrd();

// Dialog Data
	enum { IDD = IDD_ENTITY_RRD };
    CComboBox	m_comboCurEntity;
	CComboBox	m_comboFullEntity;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeCurEntityList();
	afx_msg void OnCbnSelchangeFullEntityList();
	afx_msg void OnBnClickedIdem();
	afx_msg void OnBnClickedInitiales();
	afx_msg void OnBnClickedDefault();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnBnClickedErrd1();
	afx_msg void OnBnClickedErrd2();
	afx_msg void OnBnClickedErrd3();
	afx_msg void OnBnClickedErrd4();
	afx_msg void OnBnClickedErrd5();
	afx_msg void OnBnClickedErrd6();
	afx_msg void OnBnClickedErrd7();
	afx_msg void OnBnClickedErrd8();
	afx_msg void OnBnClickedErrd9();
	afx_msg void OnBnClickedErrd10();
	afx_msg void OnBnClickedErrd11();
	afx_msg void OnBnClickedErrd12();
	afx_msg void OnBnClickedErrd13();
	afx_msg void OnBnClickedErrd14();
	afx_msg void OnBnClickedErrd15();
	afx_msg void OnBnClickedErrd16();
	afx_msg void OnBnClickedErrd17();
	afx_msg void OnBnClickedErrd18();
	afx_msg void OnBnClickedErrd19();
	afx_msg void OnBnClickedErrd20();
	afx_msg void OnBnClickedErrd21();
	afx_msg void OnBnClickedErrd22();
	afx_msg void OnBnClickedErrd23();
	afx_msg void OnBnClickedErrd24();
	afx_msg void OnBnClickedErrd25();
	afx_msg void OnBnClickedErrd26();
	afx_msg void OnBnClickedErrd27();
	afx_msg void OnBnClickedErrd28();
	afx_msg void OnBnClickedErrd29();
	afx_msg void OnBnClickedErrd30();
	afx_msg void OnBnClickedErrd31();
	afx_msg void OnBnClickedErrd32();
	afx_msg void OnBnClickedErrd33();
	afx_msg void OnBnClickedErrd34();
	afx_msg void OnBnClickedErrd35();
	afx_msg void OnBnClickedErrd36();
	afx_msg void OnBnClickedErrd37();
	afx_msg void OnBnClickedErrd38();
	afx_msg void OnBnClickedErrd39();
	afx_msg void OnBnClickedErrd40();
	afx_msg void OnBnClickedErrd41();
	afx_msg void OnBnClickedErrd42();
	afx_msg void OnBnClickedErrd43();
	afx_msg void OnBnClickedErrd44();
	afx_msg void OnBnClickedErrd45();
	afx_msg void OnBnClickedErrd46();
	afx_msg void OnBnClickedErrd47();
	afx_msg void OnBnClickedErrd48();
	afx_msg void OnBnClickedErrd49();
	afx_msg void OnBnClickedErrd50();
	afx_msg void OnBnClickedErrd51();
	afx_msg void OnBnClickedErrd52();
	afx_msg void OnBnClickedErrd53();
	afx_msg void OnBnClickedErrd54();
	afx_msg void OnBnClickedErrd55();
	afx_msg void OnBnClickedErrd56();
	afx_msg void OnBnClickedErrd57();
	afx_msg void OnBnClickedErrd58();
	afx_msg void OnBnClickedErrd59();
	afx_msg void OnBnClickedErrd60();
	afx_msg void OnBnClickedErrd0();

private:
	void InitFullEntity();
	void GetDefaultRrd();
	void GetEntityRrd(CString entity);
	bool CtrlIdemDefaut();
	int  CountNbChecked(); // renvoi le nombre de rrd coché
	void MemoInitialValue(); // popule m_entityRrd_Flag avant modification
	bool YaDesModif(); // revnvoi vrai si les aces checkées sont différente de m_entityRrd_Flag
	void OnAnyCheckRrd(); // appelée sur tous les check sur J-X

	// tableaux de 61 positions pour stocker les infos de YM_DEFAULT_RRD.
	// On suppose qu'il n'y a pas plus de 61 "RRD" avec flag_usage >= 1 dans ym_default_rrd (60 standards + le J+1)
	// Il y  61 cases à cocher dans la boite de dialogue.
    int m_defaultRrd_Idx[61];	// >= 1 : RRD défini dans ym_default_rrd avec flag_usage >= 1, 0 sinon
	int m_defaultRrd_Flag[61];  // 1 ou 2, valeur du flag usage si >=1 dans m_defaultRrd_Idx. 0 sinon
	int m_nbRrd;

	// tableaux de 61 positions pour stocker les infos de YM_ENTITY_RRD pour une entité.
	int m_entityRrd_Flag[61];  // 1 ou 2, valeur du flag usage si >=1 dans m_defaultRrd_Idx. 0 sinon
};
