/*! \file		QuotasAutorisationsListCtrl.cpp
*   \Brief	    Lot : 7978
*               List Control gérant une colonne d'autorisations de cc ou de scx dans les fenètres de bascule de quotas
*   \author     Frederic Manisse
*   \date       1 décembre 2014
*/
#include "StdAfx.h"


static CString ltocs(long i)
{
	CString s;
	s.Format("%ld", i);
	return s;
}

QuotasValue::QuotasValue() 

	: _editable(false), _bold(false), _grey_background(false), _text_color(0), _value(0), _callback_edition(NULL), _bBucket(FALSE)

{}

std::string QuotasValue::ToString() const
{
	std::ostringstream ss;
	ss << "(" << _col << "," << _row << "editable=" << _editable << ", bold = " << _bold 
		<< ", grey bkg=" << _grey_background << ", textcolor=0x" << std::hex << _text_color << std::dec 
		<< ", value=" << _value << ", callback=" << (_callback_edition?"DEFINED":"NULL")
		<< std::endl;

	return ss.str();
}


/********************************************************************************/ 
/*																																							*/ 
/*			implémentation de la classe QuotasValues																*/ 
/*																																							*/ 
/********************************************************************************/ 
void QuotasValues::Set (long vendus_res, long act, long moi, long dispo, long etanche)
{
	this->_sold_res._value = vendus_res;
	this->_act._value = act;
	this->_moi._value = moi;
	this->_dispo._value = dispo;
	this->_etanche._value = etanche;
}

//NCH
void QuotasValues::Set(int res_total, int diff)
{
	this->_res_total._row = res_total;
	this->_diff._row = diff;
}

std::string QuotasValues::ToString() const
{
	std::ostringstream ss;

	ss << "NEST LEVEL " << _nest_level.ToString()
	   << "SOLD " << _sold_res.ToString()
	   << "ACT " << _act.ToString() 
	   << "MOI " << _moi.ToString()
	   << "DISPO " << _dispo.ToString()
	   ;

	return ss.str();
}
/********************************************************************************/ 
/*																																							*/ 
/*		implementation de la classe QuotasAutorisationsValueEdit									*/ 
/*																																							*/ 
/********************************************************************************/ 

BEGIN_MESSAGE_MAP(QuotasAutorisationsValueEdit, CEdit)
ON_WM_WINDOWPOSCHANGING()
ON_WM_KEYDOWN() // SRE 76064 Evol - Catch keys on CEdit
END_MESSAGE_MAP()

QuotasAutorisationsValueEdit::QuotasAutorisationsValueEdit() // SRE 76064 Evol - Add constructor
{
	this->parentList = NULL;
}

void QuotasAutorisationsValueEdit::setParentCListCtrl(QuotasAutorisationsListCtrl *listAddress) // SRE 76064 Evol
{
	this->parentList = listAddress;
}

void QuotasAutorisationsValueEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) // SRE 76064 Evol
{
	if (nChar != 38 && nChar != 40 && nChar != 9 && nChar != 16)
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	else
	{
		if (this->parentList != NULL && GetKeyState( VK_SHIFT ) < 0 && nChar == 9)
			this->parentList->JumpOnPrevList(nChar, nRepCnt, nFlags);
		else if (this->parentList != NULL && nChar == 9)
			this->parentList->JumpOnNextList(nChar, nRepCnt, nFlags);
		else if (this->parentList != NULL && nChar == 40)
			this->parentList->EditNextElement(nChar, nRepCnt, nFlags);
		else if (this->parentList != NULL && nChar == 38)
			this->parentList->EditPreviousElement(nChar, nRepCnt, nFlags);
	}
}

void QuotasAutorisationsValueEdit::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CEdit::OnWindowPosChanging(lpwndpos);
	lpwndpos->x = this->_x;
}

/********************************************************************************/ 
/*																				*/ 
/*			implémentation de la classe QuotasAutorisationsHeaderCtrl      		*/ 
/*																				*/ 
/********************************************************************************/ 

QuotasAutorisationsHeaderCtrl::QuotasAutorisationsHeaderCtrl()
{ 
	m_whiteBackgroundBrush.CreateSolidBrush(RGB(255, 255, 255));
}

QuotasAutorisationsHeaderCtrl::~QuotasAutorisationsHeaderCtrl()
{ }

BEGIN_MESSAGE_MAP(QuotasAutorisationsHeaderCtrl, CHeaderCtrl)
//{{AFX_MSG_MAP(QuotasAutorisationsHeaderCtrl)
ON_WM_SETCURSOR()
ON_WM_LBUTTONDBLCLK()
ON_WM_LBUTTONDOWN()
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Nécéssaire de redéfinir cette fonction comme vide.
void QuotasAutorisationsHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

}

void QuotasAutorisationsHeaderCtrl::OnPaint()
{
CPaintDC dc(this); // device context for painting
CRect rect;

    this->GetClientRect(&rect);
    CDC* pDC = CDC::FromHandle(dc);
    pDC->FillRect(&rect, &m_whiteBackgroundBrush); 
    
    int nItems = GetItemCount();

    for(int i = 0; i <nItems; i++)
    {
        
        TCHAR title[256];
        HD_ITEM hditem1;
        
        hditem1.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER;
        hditem1.pszText = title;
        hditem1.cchTextMax = 255;
        GetItem( i, &hditem1 );
        
        GetItemRect(i, &rect);

		//THIS FONT IS ONLY FOR DRAWING AS LONG AS WE DON'T DO A SetFont(...)

		// CME Ano 79307
		//pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));
		pDC->SelectObject(this->GetFont());

		pDC->SetBkColor(RGB(255,255,255)) ;


		UINT uFormat = DT_CENTER;

		// check if multiline 
		bool multiline = false;
		const char* pt = title;
		while (*pt)
			if ((*pt++) == '\n')
			{ multiline = true; break; }

		if (!multiline)
			uFormat |= DT_SINGLELINE | DT_VCENTER;

		//DRAW THE TEXT
		pDC->DrawText(title, strlen(title), &rect, uFormat);

		pDC->SelectStockObject(SYSTEM_FONT);
	}
}

 
// Nécéssaire de redéfinir ces callbacks de message comme vide pour supprimer le mécanisme de resize des colonnes

BOOL QuotasAutorisationsHeaderCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return TRUE;
}

void QuotasAutorisationsHeaderCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{}

void QuotasAutorisationsHeaderCtrl::OnLButtonDown(UINT, CPoint)
{}

/********************************************************************************/ 
/*																				*/ 
/*			implémentation de la classe QuotasAutorisationsListCtrl      		*/ 
/*																				*/ 
/********************************************************************************/ 
QuotasAutorisationsListCtrl::QuotasAutorisationsListCtrl() : _edited(NULL), _ctrl_edit()
{
	m_defaultBackgroundBrush.CreateSolidBrush(RGB(255, 255, 255));
	m_greyBackgroundBrush.CreateSolidBrush(RGB(192, 192, 192));
	// CME Ano 79307 - Deb
	COLORREF clrBk = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();
	COLORREF clrAlt= ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt();
	m_CCBackgroundBrush.CreateSolidBrush(clrBk);
	m_SCBackgroundBrush.CreateSolidBrush(clrAlt);
	// CME Ano 79307 - Fin
	this->editedItemIndex = 0; //SRE 76064 Evol - Add an index to keep track of our current row edit postition
	this->_ctrl_edit.setParentCListCtrl(this); // SRE 76064 Evol - set into edit ctrl address of this class for input management purposes
	this->_edited = NULL; // SRE 76064 Evol - Moved from OnLButtonDown - Otherwise we loose keyboard navigation if the user clicks on a non editable field
}

BEGIN_MESSAGE_MAP(QuotasAutorisationsListCtrl, CListCtrl)
//{{AFX_MSG(QuotasAutorisationsListCtrl)
ON_WM_LBUTTONDOWN()
ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnLvnBeginlabeledit)
ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnLvnEndlabeledit)
ON_WM_KEYDOWN()
ON_WM_PAINT()
//}}AFX_MSG
END_MESSAGE_MAP()

void QuotasAutorisationsListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//void :)
}

/*!
 *  \fn			long IndexationToItem (long indexation) const;
 *  \brief		Converti un indice d'indexation ( par exemple 3 pour scx a3 ) en un numéro d'item dans la grille
 */
 long QuotasAutorisationsListCtrl::IndexationToItem (long indexation) const
{
	switch (_type)
	{
		case Quotas::CC : return indexation;
		case Quotas::SCX: return indexation >= 1 ? indexation -1 : -1;
		default: return -1;
	}
}

/*!
 *  \fn			long ItemToIndexation (long item) const;
 *  \brief      Converti un numéro d'item dans la grille en une indexation de classe de controle ou de sous contingent
 */
 long QuotasAutorisationsListCtrl::ItemToIndexation (long item) const
 {
	switch (_type)
	{
		case Quotas::CC : return item;
		case Quotas::SCX: return item >= 0 ? item +1 : -1;
		default: return -1;
	}
 }


/*!
 *  \fn		void Configure (type_quotas_t type, long nest_levels)
 *  \brief  Configure la grille pour un espace physique et un nombre de classes de services / sous contingents
*/
void QuotasAutorisationsListCtrl::Configure(quotas_type_t type, char espace_physique, long max_indexation)
{

	_type = type;
	_espace_physique = espace_physique;
	_max_indexation = max_indexation;

	// Redimensionnement du tableau de données
	//_datas.resize(_max_indexation+1);
	_datas.resize(_max_indexation);


	// Réaffectation des indice d'indexation dans les lignes
	//for (size_t i = 0; i <= (size_t) this->_max_indexation; ++i)
	for (size_t i = 0; i < (size_t) this->_max_indexation; ++i)
		_datas[i]._nest_level._value = i;
}


void QuotasAutorisationsListCtrl::CreateColumns ()
{
	// Ajoute le flag FULLROWSELECT pour sélectionner toute la ligne si on clique dessus
	this->SetExtendedStyle( this->GetExtendedStyle() | LVS_EX_FULLROWSELECT );

	// Suppression des colonnes existantes
	int nColumnCount = this->GetHeaderCtrl()->GetItemCount();

	// Delete all of the columns.
	for (int i=0; i < nColumnCount; i++)
	{
		this->DeleteColumn(0);
	}

	// Premiere colonne : nom de l'espace physique , en minuscule si sous contingent
	CString s = "A" ;
	s.SetAt(0, this->_espace_physique);

	// converti en majuscules si type = CC , et en minuscules pour les SCX
	switch (this->_type)
	{
		case Quotas::CC :
			s.MakeUpper();
			break;

		case Quotas::SCX:
			s.MakeLower();
			break;
	}

	this->InsertColumn(Quotas::INDEXATION, s, LVCFMT_LEFT , 20);

	// Deuxième colonne : etanchéité 
	this->InsertColumn(Quotas::ETANCHE, "", LVCFMT_LEFT, this->_type == Quotas::SCX ? 10 : 0);

	// Deuxième colonne : vendus + réservations
	this->InsertColumn(Quotas::VENTES,"Vdu\n+ res", LVCFMT_RIGHT ,35);

	// Troisième colonne : autorisations en temps réel
	this->InsertColumn(Quotas::ACT, "Act", LVCFMT_RIGHT ,35);

	// Quatrième colonne : autorisations définies par l'utilisateur
	this->InsertColumn(Quotas::MOI, "Moi", LVCFMT_RIGHT ,35);

	// Cinquième colonne : autorisations encore disponibles
	this->InsertColumn(Quotas::DISPO, "Dispo", LVCFMT_RIGHT ,35);

}

/*!
* \fn		void BuildRows ();
* \brief	Crée les lignes de la grilles selon le type et le nombre de classes configurés
*/
void QuotasAutorisationsListCtrl::CreateRows()
{
	size_t index = 0;		// Index dans la liste

	size_t start = this->ItemToIndexation(0);

	this->DeleteAllItems();

	//for (size_t i = start; i <= (size_t) _max_indexation; ++i)
	for (size_t i = start; i < (size_t) _max_indexation; ++i)
	{
		QuotasValues& data = this->_datas[i];
		data._moi._editable = true;
		data._moi._text_color = RGB(0,0,255);

		// Premiere colonne : numero du niveau de classe
		this->InsertItem(LVIF_TEXT, index,  ltocs(data._nest_level._value) ,0,0,0,0);
		data._nest_level._col = Quotas::INDEXATION;
		data._nest_level._row = index;

		// Deuxième colonne : etancheité 
		CString strEtanche = (data._etanche._value == 1 ? "E" : " ");
		this->SetItemText(index, 1, strEtanche);
		data._etanche._col = Quotas::ETANCHE;
		data._etanche._row = index;

		// Troisième colonne : vendus + réservés
		this->SetItemText(index, 2, ltocs (data._sold_res._value)) ; 
		data._sold_res._col = Quotas::VENTES;
		data._sold_res._row = index;

		// Quatrième colonne : autorisations en temps réels
		this->SetItemText(index, 3, ltocs (data._act._value)) ; 
		data._act._col = Quotas::ACT;
		data._act._row = index;

		// Cinquieme colonne : autorisations définies par l'utilisateur
		this->SetItemText(index, 4, ltocs (data._moi._value)) ; 
		data._moi._col = Quotas::MOI;
		data._moi._row = index;
		data._moi._bBucket = FALSE; // NPI - ano 70130

		// Sixieme colonne : autorisations restantes
		this->SetItemText(index, 5, ltocs (data._dispo._value)) ; 
		data._dispo._col = Quotas::DISPO;
		data._dispo._row = index;

		++index;
	}
}


void QuotasAutorisationsListCtrl::CreateBoldFont(CDC &pDC, bool isBold)
{
	// retourne si font déjà créée
	if (m_boldFont.GetSafeHandle() != NULL)
		return;

	// CME Ano 79307
	LOGFONT lfont;

	lfont.lfHeight = ((CCTAApp*)APP)->GetDocument()->GetFontHeight();
	lfont.lfWidth = 0;
	lfont.lfEscapement = 0;
	lfont.lfOrientation = 0;
	lfont.lfWeight = ((CCTAApp*)APP)->GetDocument()->GetFontWeight();
	lfont.lfItalic = ((CCTAApp*)APP)->GetDocument()->GetFontItalic();
	lfont.lfUnderline = FALSE;
	lfont.lfStrikeOut = 0;
	lfont.lfCharSet = DEFAULT_CHARSET;
	lfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lfont.lfQuality = DEFAULT_QUALITY;
	lfont.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
	strcpy (lfont.lfFaceName, ((CCTAApp*)APP)->GetDocument()->GetFontName());

	if(isBold){
		lfont.lfWeight = FW_BOLD;
	}

	m_boldFont.CreateFontIndirect(&lfont);
}


void QuotasAutorisationsListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if(lpDrawItemStruct == NULL) 
		return;  
		

    int item = lpDrawItemStruct->itemID; 
  
    CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC); 
    int nSavedDC = pDC->SaveDC(); 
  
    CRect rItem( lpDrawItemStruct->rcItem ); 

	// Colorisation du background (en blanc par défaut )
	CBrush* pOldBrush = pDC->SelectObject( &m_defaultBackgroundBrush ); 
	pDC->FillRect(&rItem, &m_defaultBackgroundBrush); 
	pDC->SelectObject( pOldBrush ); 

	

	// variables de boucle
	int width_column;
	int nb_columns = GetHeaderCtrl()->GetItemCount(); 
	LVITEM  lvitem; 
	LV_COLUMN lvc;
	char szItem[255];
	CString sValue;
	RECT r = lpDrawItemStruct->rcItem;

	// Remplissage de la valeur par colonnes
	for( int icol = 0; icol < nb_columns ; icol++ ) 
	{         

		width_column = GetColumnWidth( icol ); 
		lvc.mask = LVCF_FMT;         

		if( GetColumn( icol, &lvc ) ==0 ) 
			continue; 

		CRect rSubItem=r; 
		rSubItem.right = rSubItem.left + width_column;

		lvitem.mask=LVIF_TEXT |LVIF_IMAGE | LVIF_PARAM; 
		lvitem.cchTextMax=sizeof(szItem); 
		lvitem.pszText=szItem; 
		lvitem.iItem=item; 
		lvitem.iSubItem=icol; 
		lvitem.iImage=2; 

		GetItem( &lvitem ); 

		QuotasValue* lval = this->GetValue( (quotas_values_t) icol, this->ItemToIndexation(item));

		// Colorie le fond de la case si pas blanc
		if (lval->_grey_background)
		{
			pOldBrush = pDC->SelectObject( &m_greyBackgroundBrush ); 
			pDC->FillRect(&rSubItem, &m_greyBackgroundBrush); 
			pDC->SelectObject( pOldBrush ); 			
		}
		// CME Ano 79307 - Deb
		else
		{
			if(this->Type() == Quotas::CC){
				pOldBrush = pDC->SelectObject( &m_CCBackgroundBrush ); 
				pDC->FillRect(&rSubItem, &m_CCBackgroundBrush); 
				pDC->SelectObject( pOldBrush );
			}
			else{
				pOldBrush = pDC->SelectObject( &m_SCBackgroundBrush ); 
				pDC->FillRect(&rSubItem, &m_SCBackgroundBrush); 
				pDC->SelectObject( pOldBrush );

			}
		}// CME Ano 79307 - Fin

		if (lval != NULL)
		{
			if (lval->_col == Quotas::ETANCHE)
			{
				sValue = lval->_value == 1 ? "E" : " ";
			}
			else
			{
				sValue = ltocs(lval->_value);
			}
		}
		else sValue = CString("");

		

		bool is_text_bold = lval->_bold;

		// change pour la police en gras si besoin
		CFont *pOldFont = NULL;

		this->CreateBoldFont(*pDC,is_text_bold);
		pOldFont = pDC->SelectObject(&m_boldFont);


		pDC->SetTextColor(lval->_text_color);

		if( lvc.fmt & LVCFMT_RIGHT ) 
		{ 
			pDC->SetTextAlign( TA_RIGHT ); 
			pDC->ExtTextOut( r.left + width_column-10 , r.top, 0, &rSubItem, sValue, NULL ); 
		} 
		else 
		{ 
			pDC->SetTextAlign( TA_LEFT ); 
			pDC->ExtTextOut( r.left+5 ,r.top, 0, &rSubItem, sValue, NULL ); 
		} 

		// Retourne à la police normale si il y a eu permutation
		if (is_text_bold)
			pDC->SelectObject(pOldFont);

		r.left += width_column; 
	}
}

QuotasValue* QuotasAutorisationsListCtrl::GetValue(quotas_values_t col, size_t indexation)
{

	/// a enlever 
	int size = _datas.size();

	//if (col < 0 || col > (quotas_values_t) this->_datas.size())
	if (col < Quotas::TYPE_VALUES_START || col >= Quotas::TYPE_VALUES_END)
	{
		return NULL;
	}

	QuotasValues& line = _datas[indexation];

	switch (col)
	{
		case Quotas::INDEXATION		: return &line._nest_level;
		case Quotas::ETANCHE		: return &line._etanche;
		case Quotas::VENTES			: return &line._sold_res;
		case Quotas::ACT			: return &line._act;
		case Quotas::MOI			: return &line._moi;
		case Quotas::DISPO			: return &line._dispo;
		default: return NULL;
	}
}

void QuotasAutorisationsListCtrl::SetValue(quotas_values_t col, size_t indexation, long value)
{
	QuotasValue* lval = this->GetValue(col, indexation);
	if (lval)
		lval->_value = value;
}

const QuotasValue* QuotasAutorisationsListCtrl::GetValue(quotas_values_t col, size_t indexation) const
{
	//if (col < 0 || col > (quotas_values_t) this->_datas.size())
	if (col < Quotas::TYPE_VALUES_START || col >= Quotas::TYPE_VALUES_END)
	{
		return NULL;
	}

	const QuotasValues& line = _datas[indexation];

	switch (col)
	{
		case Quotas::INDEXATION		: return &line._nest_level;
		case Quotas::ETANCHE		: return &line._etanche;
		case Quotas::VENTES			: return &line._sold_res;
		case Quotas::ACT			: return &line._act;
		case Quotas::MOI			: return &line._moi;
		case Quotas::DISPO			: return &line._dispo;
		default: return NULL;
	}	
}
void QuotasAutorisationsListCtrl::SetFontWeight(quotas_values_t col, size_t indexation, bool bold)
{
	QuotasValue* lval = this->GetValue(col, indexation);
	if (lval)
		lval->_bold = bold;
}

void QuotasAutorisationsListCtrl::SetTextColor(quotas_values_t col, size_t indexation, COLORREF color)
{
	QuotasValue* lval = this->GetValue(col, indexation);
	if (lval)
		lval->_text_color = color;
}


/*!
* \fn   void SetGreyBackground(quotas_values_t col, size_t indexation, bool is_grey);
* \brief Précise la couleur de fond d'une case
*/
void QuotasAutorisationsListCtrl::SetGreyBackground(quotas_values_t col, size_t indexation, bool is_grey)
{
	QuotasValue* lval = this->GetValue(col, indexation);
	if (lval)
		lval->_grey_background = is_grey;
}

/*!
* \fn   void SetWhiteBackground(quotas_values_t col, size_t indexation);
* \brief Retourne au fond blanc pour le background pour une case donnée
*/
void QuotasAutorisationsListCtrl::SetWhiteBackground(quotas_values_t col, size_t indexation)
{
	QuotasValue* lval = this->GetValue(col, indexation);
	if (lval)
		lval->_grey_background = false;
}

/*!
 * \fn   void SetWhiteBackground(size_t indexation);
 * \brief Retourne au fond blanc pour le background pour une ligne donnée
*/
void QuotasAutorisationsListCtrl::SetWhiteBackground(size_t indexation)
{
	for (size_t col = Quotas::TYPE_VALUES_START; col < Quotas::TYPE_VALUES_END; ++col)
		this->SetWhiteBackground((quotas_values_t) col, indexation);
}

/*!
 * \fn   void SetWhiteBackground();
 * \brief Retourne au fond blanc pour le background sur toute la grille
*/
void QuotasAutorisationsListCtrl::SetWhiteBackground()
{
	for (size_t i = 0; i < _datas.size(); ++i)
	{
		this->SetWhiteBackground(i);
	}
}

void QuotasAutorisationsListCtrl::SetEditable (quotas_values_t col, size_t indexation, bool is_editable)
{
	QuotasValue* lval = this->GetValue(col, indexation);
	if (lval)
		lval->_editable = is_editable;
}

void QuotasAutorisationsListCtrl::Clip(size_t& index_start, size_t& index_end) const
{
	if (index_start < 0)
		index_start = 0;

	if (index_end >= _datas.size())
		index_end = _datas.size() -1;
}


long QuotasAutorisationsListCtrl::Sum(quotas_values_t colonne, size_t index_start, size_t index_end) const
{
	this->Clip(index_start, index_end);

	if (index_start >= _datas.size() ||index_end < 0)
		return 0;

	long res = 0;
	const QuotasValue* lval;
	for (size_t i = index_start; i < index_end; ++i)
	{
		 lval = this->GetValue(colonne, i);
		 if (lval != NULL)
		 {
			 res += lval->_value;
		 }
	}
	return res;
}

void QuotasAutorisationsListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	LVHITTESTINFO lvhit;
	lvhit.pt = point;
	int item = SubItemHitTest(&lvhit);

	
	
	QuotasValue* hitted = NULL;

	if (lvhit.flags &	LVHT_ONITEM)
	if (item >= 0 && item < (int) this->_datas.size() && lvhit.iSubItem >= 0 && lvhit.iSubItem <= Quotas::DISPO)

	{
		int indexation = this->ItemToIndexation(item);
		hitted = this->GetValue((quotas_values_t) lvhit.iSubItem, indexation);
		
		if (hitted->_editable)
		{
			CListCtrl::OnLButtonDown(nFlags, point);
			this->_edited = hitted;
			
			this->editedItemIndex = item;
			this->EditLabel(item);
		}
	}
}

void QuotasAutorisationsListCtrl::JumpOnPrevList(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	TRACE("Running JumpOnPrevList\n");
	CWnd *parent = this->GetParent(); //We need the parent to send our messages
	//Because this class is not an MFC class and can't send Messages
	QuotasValue* hitted = NULL;
	//this->infos.index = this->editedItemIndex;
	this->infos.move = Bascules::PREVIOUSLIST;
	
	if (parent == NULL)	//Should not happen, the parent has not set it's address in this class
		return;			//We cannot send message when we get to an end of the list
	if (nChar != 0)
	{
		this->infos.index = this->ItemToIndexation(this->editedItemIndex);//this->editedItemIndex;
		parent->SendMessage(WM_KB_LIST_END_REACHED, (WPARAM)this->parentView->getListId(),
			(LPARAM)&(this->infos)); //We tell the Bascule that we want the previous list
		return;
	}
	if (nChar == 0)//The function has been called by the Bascule and not triggered by keyboard key press
	{
		TRACE("Called by parent\n");
		this->editedItemIndex = (int)nRepCnt;
		if (_type == Quotas::SCX && this->editedItemIndex == -1)
			this->editedItemIndex = 0;
		if (_type == Quotas::SCX && (int)nRepCnt > ((int)this->_datas.size() - 2))
			this->editedItemIndex = this->_datas.size() - 2;
		if (_type == Quotas::CC && this->editedItemIndex > this->_datas.size() - 1)
			this->editedItemIndex = this->_datas.size() - 1;//Select the last element of the list
		int first_indexation = this->ItemToIndexation(this->editedItemIndex);//Translation between grid units and index
		hitted = this->GetValue((quotas_values_t) Quotas::MOI, first_indexation);//Always select the MOI column, the others must not be edited
		
		if (hitted != NULL)//Index->Element obtained ?
		{
			TRACE("We have our target value\n");
			if (!hitted->_editable)//If we jumped on a list with non editable 'Moi' Values
			{
				TRACE("uh oh, not editable\n");
				this->infos.index = (UINT)nRepCnt;
				this->infos.move = Bascules::PREVIOUSLIST;
				parent->SendMessage(WM_KB_LIST_END_REACHED, (WPARAM)this->parentView->getListId(),
					(LPARAM)&(this->infos)); //Jump to the previous one, using ID
				return;
			}
			this->_edited = hitted;
			this->EditLabel(editedItemIndex);
		}
		TRACE("check if not NULL and editable\n");
		if (this->_edited != NULL && this->_edited->_editable)
		{
			TRACE("We are on a valid box\n");
			if (this->editedItemIndex >= 0)//if we have not reached the first element - 1 (still inside this list)
			{
				//Close edit box, the goal is to force the execution of "OnLvnBeginlabeledit" event
				this->_ctrl_edit.CloseWindow();
			}

			//Still in range, edit our new index
			int indexation = this->ItemToIndexation(this->editedItemIndex);
			hitted = this->GetValue((quotas_values_t) Quotas::MOI, indexation);
			this->_edited = hitted;
			TRACE("Launch edition\n");
			this->EditLabel(editedItemIndex);
		}
		return;
	}
}

void QuotasAutorisationsListCtrl::JumpOnNextList(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	TRACE("Running JumpOnNextList\n");
	CWnd *parent = this->GetParent(); //We need the parent to send our messages
	//Because this class is not an MFC class and can't send Messages
	QuotasValue* hitted = NULL;
	//this->infos.index = this->editedItemIndex;
	this->infos.move = Bascules::NEXTLIST;
	
	if (parent == NULL)	//Should not happen, the parent has not set it's address in this class
		return;			//We cannot send message when we get to an end of the list
	if (nChar != 0)
	{
		this->infos.index = this->ItemToIndexation(this->editedItemIndex);//this->editedItemIndex;
		parent->SendMessage(WM_KB_LIST_END_REACHED, (WPARAM)this->parentView->getListId(),
			(LPARAM)&(this->infos)); //We tell the Bascule that we want the previous list
		return;
	}
	if (nChar == 0)//The function has been called by the Bascule and not triggered by keyboard key press
	{
		TRACE("Called by parent\n");
		
		this->editedItemIndex = (int)nRepCnt;
		if (_type == Quotas::SCX && this->editedItemIndex == -1)
			this->editedItemIndex = 0;
		TRACE("nRepCnt value = %d\n", this->editedItemIndex);
		TRACE("value of this->_datas.size() = %u\n", this->_datas.size());
		if (_type == Quotas::SCX && this->editedItemIndex > ((int)this->_datas.size() - 2))
			this->editedItemIndex = this->_datas.size() - 2;
		if (_type == Quotas::CC && this->editedItemIndex > this->_datas.size() - 1)
			this->editedItemIndex = this->_datas.size() - 1;//Select the last element of the list
		int first_indexation = this->ItemToIndexation(this->editedItemIndex);//Translation between grid units and index
		hitted = this->GetValue((quotas_values_t) Quotas::MOI, first_indexation);//Always select the MOI column, the others must not be edited
		
		if (hitted != NULL)//Index->Element obtained ?
		{
			TRACE("We have our target value\n");
			if (!hitted->_editable)//If we jumped on a list with non editable 'Moi' Values
			{
				TRACE("uh oh, not editable\n");
				this->infos.index = (UINT)nRepCnt;
				this->infos.move = Bascules::NEXTLIST;
				parent->SendMessage(WM_KB_LIST_END_REACHED, (WPARAM)this->parentView->getListId(),
					(LPARAM)&(this->infos)); //Jump to the previous one, using ID
				return;
			}
			this->_edited = hitted;
			this->EditLabel(editedItemIndex);
		}
		TRACE("check if not NULL and editable\n");
		if (this->_edited != NULL && this->_edited->_editable)
		{
			TRACE("We are on a valid box\n");
			if (this->editedItemIndex >= 0)//if we have not reached the first element - 1 (still inside this list)
			{
				//Close edit box, the goal is to force the execution of "OnLvnBeginlabeledit" event
				this->_ctrl_edit.CloseWindow();
			}

			//Still in range, edit our new index
			int indexation = this->ItemToIndexation(this->editedItemIndex);
			hitted = this->GetValue((quotas_values_t) Quotas::MOI, indexation);
			this->_edited = hitted;
			TRACE("Launch edition\n");
			this->EditLabel(editedItemIndex);
		}
		return;
	}
}

void QuotasAutorisationsListCtrl::EditPreviousElement(UINT nChar, UINT nRepCnt, UINT nFlags) //SRE 76064 Evol
{
	CWnd *parent = this->GetParent(); //We need the parent to send our messages
	//Because this class is not an MFC class and can't send Messages
	QuotasValue* hitted = NULL;
	
	this->infos.index = this->editedItemIndex;
	this->infos.move = Bascules::BEGINNING;

	if (parent == NULL)	//Should not happen, the parent has not set it's address in this class
		return;			//We cannot send message when we get to an end of the list
	if (this->_datas.size() == 0)//List has no data
	{
		parent->SendMessage(WM_KB_LIST_END_REACHED, (WPARAM)this->parentView->getListId(),
			(LPARAM)&(this->infos)); //We tell the Bascule that we want the previous list
		return;
	}
	if (nChar == 0)//The function has been called by the Bascule and not triggered by keyboard key press
	{
		if (_type == Quotas::SCX)
			this->editedItemIndex = this->_datas.size() - 2;
		else
			this->editedItemIndex = this->_datas.size() - 1;//Select the last element of the list
		int first_indexation = this->ItemToIndexation(this->editedItemIndex);//Translation between grid units and index
		hitted = this->GetValue((quotas_values_t) Quotas::MOI,
			first_indexation);//Always select the MOI column, the others must not be edited
		
		if (hitted != NULL)//Index->Element obtained ?
		{
			if (!hitted->_editable)//If we jumped on a list with non editable 'Moi' Values
			{
				parent->SendMessage(WM_KB_LIST_END_REACHED, (WPARAM)this->parentView->getListId(),
					(LPARAM)&(this->infos)); //Jump to the previous one, using ID
				return;
			}
			//Otherwise, edit the last item of this list (because the jump occured from the beggining of the next list)
			this->_edited = hitted;
			this->EditLabel(editedItemIndex);
		}
		return;
	}

	if (this->_edited != NULL && this->_edited->_editable)
	{
		if (this->editedItemIndex - 1 >= 0)//if we have not reached the first element - 1 (still inside this list)
		{
			//Close edit box, the goal is to force the execution of "OnLvnBeginlabeledit" event
			this->_ctrl_edit.CloseWindow();
			--(this->editedItemIndex);//Decrement keyboard index
		}
		else //We are out of range of the list
		{
			this->_ctrl_edit.CloseWindow(); //Close the edit window on this list... we will switch
			parent->SendMessage(WM_KB_LIST_END_REACHED, (WPARAM)this->parentView->getListId(),
				(LPARAM)&(this->infos));//Jump to the previous one, using ID
			return;
		}

		//Still in range, edit our new index
		int indexation = this->ItemToIndexation(this->editedItemIndex);
		hitted = this->GetValue((quotas_values_t) Quotas::MOI, indexation);
		this->_edited = hitted;
		this->EditLabel(editedItemIndex);
	}
}

void QuotasAutorisationsListCtrl::EditNextElement(UINT nChar, UINT nRepCnt, UINT nFlags) //SRE 76064 Evol
{
	CWnd *parent = this->GetParent();
	QuotasValue* hitted = NULL;
	
	this->infos.index = this->editedItemIndex;
	this->infos.move = Bascules::END;

	if (parent == NULL)
		return;
	if (this->_datas.size() == 0)
	{
		parent->SendMessage(WM_KB_LIST_END_REACHED, (WPARAM)this->parentView->getListId(),
			(LPARAM)&(this->infos));
		return;
	}
	if (nChar == 0)
	{
		this->editedItemIndex = 0;
		int first_indexation = this->ItemToIndexation(this->editedItemIndex);
		hitted = this->GetValue((quotas_values_t) Quotas::MOI, first_indexation);
		
		if (hitted != NULL)
		{
			if (!hitted->_editable)
			{
				parent->SendMessage(WM_KB_LIST_END_REACHED, (WPARAM)this->parentView->getListId(),
					(LPARAM)&(this->infos));
				return;
			}
			this->_edited = hitted;
			this->EditLabel(editedItemIndex);
		}
		return;
	}

	if (this->_edited != NULL && this->_edited->_editable)
	{
		if (this->ItemToIndexation(this->editedItemIndex + 1) < (int) this->_datas.size())
		{
			//Close edit box, the goal is to force the execution of "OnLvnBeginlabeledit" event
			this->_ctrl_edit.CloseWindow();
			++(this->editedItemIndex);//Increment keyboard index
		}
		else
		{
			if (parent != NULL)
			{
				this->_ctrl_edit.CloseWindow();
				parent->SendMessage(WM_KB_LIST_END_REACHED, (WPARAM)this->parentView->getListId(),
					(LPARAM)&(this->infos));
				return;
			}
		}
		int indexation = this->ItemToIndexation(this->editedItemIndex);
		hitted = this->GetValue((quotas_values_t) Quotas::MOI, indexation);
		this->_edited = hitted;
		this->EditLabel(editedItemIndex);
	}
}

void QuotasAutorisationsListCtrl::clearData(bool is_inter)
{
	for(int i = 0 ; i < _max_indexation ; i++)
	{
		SetLine(i, 0, 0, 0, 0);
		SetEditable(Quotas::MOI, i, ! is_inter);
		SetGreyBackground(Quotas::MOI, i, is_inter);
		SetTextColor(Quotas::MOI, i, is_inter ? RGB(128, 128, 128) : RGB(0, 0, 255));
	}
}

void QuotasAutorisationsListCtrl::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	if (_edited)
	if (_ctrl_edit.GetSafeHwnd() != NULL)
	{
		CRect rect;
		CRect editrect;
		
		this->GetSubItemRect(_edited->_row, _edited->_col,LVIR_LABEL, rect);

		_ctrl_edit.GetWindowRect(editrect);
		ScreenToClient(editrect);

		if (editrect.right < rect.right)
		{
			rect.left = editrect.right ;
			ValidateRect(rect);
		}
		//block filling redraw of leftmost item (caused by FillRect)
		GetItemRect(_edited->_row, rect,LVIR_LABEL );
		ValidateRect(rect);
	}
	// Do not call CListCtrl::OnPaint() for painting messages
	CListCtrl::OnPaint();
}

void QuotasAutorisationsListCtrl::OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	//if (subitem selected for editing)
	if (_edited)
	{
		this->beingEdited = true;
		ASSERT(_edited->_row == pDispInfo->item.iItem);
		CRect subrect;
		
		GetSubItemRect( pDispInfo->item.iItem, _edited->_col, LVIR_BOUNDS , subrect );

		//get edit control and subclass
		HWND hWnd=(HWND)SendMessage(LVM_GETEDITCONTROL);
		ASSERT(hWnd!=NULL);
		VERIFY(_ctrl_edit.SubclassWindow(hWnd));

		//move edit control text 1 pixel to the right of org label,
		//as Windows does it...
		_ctrl_edit._x=subrect.left ;
		_ctrl_edit.SetWindowText(GetItemText(pDispInfo->item.iItem,_edited->_col));

		//hide subitem text so it don't show if we delete some
		//text in the edit control
		//OnPaint handles other issues also regarding this

		CRect rect;
		GetSubItemRect(pDispInfo->item.iItem, _edited->_col,	LVIR_LABEL ,rect);
		CDC* hDc = GetDC();
		hDc->FillRect(rect,&CBrush(::GetSysColor(COLOR_WINDOW)));
		ReleaseDC(hDc);
	}
	*pResult = 0;
}
//Saving the Changes
void QuotasAutorisationsListCtrl::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LV_ITEM *plvItem = &pDispInfo->item;

	// traitement uniquement si on a édité des autorisations
	if (_edited)
	{
		this->beingEdited = false;
		if (plvItem->pszText != NULL )
		{
			long ivalue = atoi(plvItem->pszText);
			if (ivalue <= 999 && ivalue >= 0 )
			{
				SetItemText(plvItem->iItem, _edited->_col,plvItem->pszText);
				_edited->_value = ivalue;
				if (_edited->_callback_edition != NULL)
					_edited->_callback_edition (this->_type, this->_espace_physique, _edited->_col, this->ItemToIndexation(_edited->_row), _edited->_value);
			}
			else
				AfxMessageBox("Merci d'entrer une valeur entre 0 et 999.");
		}
		VERIFY(_ctrl_edit.UnsubclassWindow()!=NULL);
		//_edited = NULL; // SRE 76064 Evol - Why ? Complicated to manage if we set this to null
		//We also loose the keyboard if we set it to null here
		*pResult = 0;
	}
}

void QuotasAutorisationsListCtrl::SetLine (size_t indexation, long vendus_res, long act, long moi, long dispo, long etanche)
{
	if (indexation < (unsigned)_max_indexation && indexation >= 0)
	{
		QuotasValues& data = this->_datas[indexation];
		data.Set (vendus_res, act, moi, dispo, etanche);

		size_t item = this->IndexationToItem(indexation);
		
		// Deuxième colonne : etancheité
		CString strEtanche = (data._etanche._value == 1 ? "E" : " ");

		this->SetItemText(item, Quotas::ETANCHE, strEtanche);

		// Troisième colonne : vendus + réservés
		this->SetItemText(item, Quotas::VENTES , ltocs (data._sold_res._value)) ; 

		// Quatrième colonne : autorisations en temps réels
		this->SetItemText(item, Quotas::ACT, ltocs (data._act._value)) ; 

		// Cinquieme colonne : autorisations définies par l'utilisateur
		this->SetItemText(item, Quotas::MOI, ltocs (data._moi._value)) ; 

		// Sixieme colonne : autorisations restantes
		this->SetItemText(item, Quotas::DISPO, ltocs (data._dispo._value)) ; 
	}
}

void QuotasAutorisationsListCtrl::SetLine (size_t indexation, int res_total, int diff)
{
	QuotasValues& data = this->_datas[indexation];
	data.Set (res_total, diff);

	size_t item = this->IndexationToItem(indexation);

	// Deuxième colonne : vendus + réservés
	this->SetItemText(item, Quotas::VENTES, ltocs (data._res_total._row));
}
/********************************************************************************/ 
/*																																							*/ 
/*		implémentation de la classe QuotasAutorisationsListView										*/ 
/*																																							*/ 
/********************************************************************************/ 

void QuotasAutorisationsListView::setListId(int _ID)
{
	this->ID = _ID;
	this->_list_ctrl.setParentView(this);
}

int QuotasAutorisationsListView::getListId()
{
	return (this->ID);
}

void QuotasAutorisationsListView::Build (quotas_type_t type, char espace_physique, size_t max_indexation)
{
	bool already_init_done = (this->_head_ctrl.GetSafeHwnd() != NULL);

	if (already_init_done == false)
	{
		// subclass le controleur des headers de colonnes
		CHeaderCtrl* pHeader = this->_list_ctrl.GetHeaderCtrl();

		if(pHeader==NULL)
			return;
		
		VERIFY(this->_head_ctrl.SubclassWindow(pHeader->m_hWnd));	
	}

	// configure la liste, construit les colonnes et les lignes
	this->_list_ctrl.Configure(type, espace_physique, max_indexation);
	this->_list_ctrl.CreateColumns();

	// Si type == CC, alors ligne 0 colonne MOI en gras
	if (type == Quotas::CC)
	{
		this->_list_ctrl.GetValue(Quotas::MOI, 0)->_bold = true;
	}
	this->_list_ctrl.CreateRows();	

	if (already_init_done == false)
	{
		// crée une font très grande pour tromper windows et avoir une hauteur dans le header pour 2 lignes
		//_head_font.CreatePointFont(190,"MS Serif");
		// CME Ano 79307
		LOGFONT lfont;

		lfont.lfHeight = ((CCTAApp*)APP)->GetDocument()->GetFontHeight();
		lfont.lfWidth = 0;
		lfont.lfEscapement = 0;
		lfont.lfOrientation = 0;
		lfont.lfWeight = ((CCTAApp*)APP)->GetDocument()->GetFontWeight();
		lfont.lfItalic = ((CCTAApp*)APP)->GetDocument()->GetFontItalic();
		lfont.lfUnderline = FALSE;
		lfont.lfStrikeOut = 0;
		lfont.lfCharSet = DEFAULT_CHARSET;
		lfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lfont.lfQuality = DEFAULT_QUALITY;
		lfont.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
		strcpy (lfont.lfFaceName, ((CCTAApp*)APP)->GetDocument()->GetFontName());

		_head_font.CreateFontIndirect(&lfont);
	}
	_head_ctrl.SetFont(&_head_font);

	// configure les colonnes pour le flag OWNERDRAW ( dessiné par la classe )
	HDITEM hdItem;
	memset(&hdItem, 0, sizeof(HDITEM));
	hdItem.mask = HDI_FORMAT;

	for(int i=0; i<_head_ctrl.GetItemCount(); i++)
	{
		_head_ctrl.GetItem(i,&hdItem);
		hdItem.fmt|= HDF_OWNERDRAW;		
		_head_ctrl.SetItem(i,&hdItem);
	}
	
}

void QuotasAutorisationsListView::ClearData()
{
	for(int i = 0; i < this->_list_ctrl.MaxIndexation(); i++)
	{
		QuotasValue* value = this->_list_ctrl.GetValue(Quotas::DISPO, i);
		if (value != NULL)
			value->_value = 0;
	}

/*
	std::vector<QuotasValues>& data = this->_list_ctrl.Datas();
	std::vector<QuotasValues>::iterator it;
	for(it = data.begin(); it != data.begin(); ++it)
	{
		QuotasValues& values = (*it);
		values.Set(0,0,0,0,0);
		values._dispo._value = 0;
	}
*/

}

BOOL QuotasAutorisationsListView::ShowWindow(int nCmdShow)
{
	return this->_list_ctrl.ShowWindow(nCmdShow);
}

void QuotasAutorisationsListView::SetLine (size_t indexation, long vendus_res, long act, long moi, long dispo, long etanche)
{
	this->_list_ctrl.SetLine(indexation, vendus_res, act, moi, dispo, etanche);
}

//End of file
