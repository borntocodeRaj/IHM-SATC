#if !defined(AFX_CALENDARWND_H__INCLUDED_)
#define AFX_CALENDARWND_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// couleur des niveaux de warning
#define color_l0 RGB(128, 128, 128);
#define color_l1 RGB(0, 0, 0);
#define color_l2 RGB(128, 64, 0);
#define color_l3 RGB(0, 128, 255);
#define color_l4 RGB(255, 0, 128);

// couleur des TOP
#define color_t0 RGB(255,255,255);
#define color_t1 RGB(153,204,0);
#define color_t2 RGB(0,255,0);
#define color_t3 RGB(0,204,255);
#define color_t4 RGB(255,153,204);
#define color_t5 RGB(153,51,102);
#define color_t6 RGB(255,153,0);
#define color_t7 RGB(255,0,0);

// couleur des curseurs
#define color_A1 RGB(255, 0, 0);	// color valeur 1 premiere
#define color_A2 RGB(180, 64, 64);  // color valeur 2 premiere
#define color_B1 RGB(0, 255, 0);	// color valeur 1 seconde
#define color_B2 RGB(0, 128, 0);    // color valeur 2 seconde
#define color_Full RGB (220, 220, 0); // couleur de fond des curseurs

class CCalendarWnd : public YMCalendar
{
  public:
    // Constructeur appelée depuis la sélection mono ou multidate
    // mm1,yy1 mois année début, mm2,yy2 mois année fin, yy sur 4 digits
	CCalendarWnd(int mm1, int yy1, int mm2, int yy2);
	~CCalendarWnd();

  enum choixAffichage 
	{
    UNDEFINED = -1,
    TOP = 0,
    COMMENT,
	VNAU,
	CRITIC,
	TGVJUM,
	AFFAIRE,
	DESSERTE,
	GESTIONOD,
	REVISION,
    NIVEAU,
    EQUIP,
	FORCEMENT,
	SUPPLEMENTAIRE,
	SERVICE,
	NOMOS,
	TAUX_PREMIERE,
	TAUX_SECONDE
	};

    void ChangeAffichage (choixAffichage chx1, choixAffichage chx2, bool bInvalidate = true);
	void SetMinDateTranche (unsigned long mindate) { m_minDateTranche = mindate; }
	void SetMaxDateTranche (unsigned long maxdate) { m_maxDateTranche = maxdate; }
	void SetMinDateOp (unsigned long mindate) { m_minDateOp = mindate; }
	void PositionOnToday(); // positionne le calendrier sur la date du jour.

	//gestion de AllResaTranchesMap qui permet de mémoriser les resa sur les tranches sur toutes les dates
	void PurgeTrancheResa();
	void ReadTrancheResa(int trancheNo, int familyNo, CString familyFlag);

  protected:
    // pure vitual de YMCalendar
	void SelDone();
	bool DblClickDone();
	COLORREF GetBkColor(YMDay* pDay);
	CString GetConges(YMDay* pDay);
	char GetSpecialChar(YMDay* pDay, CString& comment);
	bool GetCursor (YMDay* pDay, int& val1, int& val2, int& full, CString& sInfo,
		            COLORREF& color1, COLORREF& color2, COLORREF& bckColor);
	bool GetDisplayInfo (YMDay* pDay, int col, int& bmpId, CString& sBuffer, CString& sInfo,
		                 bool& bital, bool& bbold, COLORREF& color, COLORREF& bckcolor);
	bool IsUnusedDate (YMDay* pDay, COLORREF& color);

private:
    choixAffichage m_choix1;
	choixAffichage m_choix2;
	unsigned long m_minDateTranche; // date min de la liste de tranches lues
	unsigned long m_maxDateTranche; // date max de la liste de tranches lues
	unsigned long m_minDateOp;		// date minimale venant de la base opérationnelle 
	unsigned long m_minDate;		// date min de la sc_tranches
	unsigned long m_maxDate;        // date max de la sc_tranches
	CString m_szClient;

	// Analyse d'un PNDATA venant de YM_VNL_HIST, qcm indique le numéro d'ordre du compartiment que l'on recherche
	// si OK, sCmpt donne le compartiment, capa s capacité et res_hold le total des resa
	bool FindInfoPnData (LPCSTR pndata, int nbnest, CString& sCmpt,
		                 int& capa, int& reshold, int qcm);

	// tableau de CObArray, indexé sur le numero de tranches. Les CObArray contiennent une entrée par date de départ.
	// permet de lacner ces requêtes en mode synchrone (transact) et de mémorisé le résultat qui sera réutilisé si l'on revient sur le
	// même numéro de tranche
	CMapWordToOb AllResaTranchesMap;

};

#endif