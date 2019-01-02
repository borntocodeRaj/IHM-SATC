/*! \file		QuotasAutorisationsListCtrl.h
*   \brief	    Lot : 7978
*               List Control g�rant une colonne d'autorisations de cc ou de scx dans les fen�tres de bascule de quotas
*   \author     Frederic Manisse
*   \date       1 d�cembre 2014
*/
#ifndef _QUOTAS_AUTORISATIONS_LIST_CTRL_H_
#define _QUOTAS_AUTORISATIONS_LIST_CTRL_H_

/********************************************************************************/ 
/*			section include														*/ 
/********************************************************************************/ 

/********************************************************************************/ 
/*			d�claration des enums et types li�s � la bascule des quotas			*/ 
/********************************************************************************/ 


class Quotas
{
public:

	/*!
	 * \enum TYPES_QUOTAS
     * \brief Types de classes ayant des autorisations g�r�es par ce contr�le
	 */
	enum TYPES_QUOTAS
	{
		CC	= 0,		
	    SCX	= 1,
	};


	/*!
	 * \enum   TYPES_VALUES
	 * \brief  Listing des diff�rentes valeurs affich�es / impliqu�es dans la bascule des quotas. 
	 *		   Chaque valeur correspond � une colonne dans les grilles des autorisations
	 */
	enum TYPES_VALUES
	{
		TYPE_VALUES_START			= 0,

		INDEXATION		= 0,
		ETANCHE			= 1,
		VENTES			= 2,
		ACT				= 3,
		MOI				= 4,
		DISPO			= 5,

		TYPE_VALUES_END				= 6
	};
};

/*!
 * \brief typedef pour repr�senter un enum TYPES_QUOTAS
 */
typedef Quotas::TYPES_QUOTAS quotas_type_t;

/*!
 * \brief typedef pour repr�senter un enum TYPES_VALUES
 */
typedef Quotas::TYPES_VALUES quotas_values_t;

/********************************************************************************/
/*			declaration des callback d'�dition de valeurs						*/ 
/********************************************************************************/ 

/*!
 * \brief	typedef pour les fonctions de callback sur l'�dition d'une valeur
 *          La callback a la possibilit� de modifier la valeur pour correction
 */ 

//typedef void (*satc_quotas_edited_fn) (quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);

typedef void (*satc_quotas_edited_fn) (quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);


/********************************************************************************/
/*				declaration du type QuotasValue									*/ 
/********************************************************************************/ 

 			
/*!
 * \struct QuotasValue
 * \brief  Stocke le format pour l'affichage d'une donn�e ainsi que sa valeur et la callback � appeler en cas d'�dition
 */
struct QuotasValue
{

		/*!
		 * \brief index de ligne de la valeur dans la grille
		 */
		int			   _row;

		/*!
		 * \brief identifiant de la colonne de la valeur dans la grille
		 */ 
		quotas_values_t _col;

		/*!
		 *  \brief vrai si la valeur est �ditable
		 */
		bool  _editable;


		/*!
		 * \brief vrai si la valeur doit �tre affich�e en gras
		 */
		bool	_bold;	


		/*!
		 *  \brief vrai si le background de la case est en gris
		 */
		bool	_grey_background;

		/*!
		 * \brief couleur du text
		 */
		COLORREF		_text_color;

		/*!
		 * \brief valeur
		 */
		long			_value;

		// NPI - Ano 70130
		/*!
		 * \brief bucket
		 */
		BOOL	_bBucket;
		///

		/*!
		 * \brief callback � appeler lorsque la valeur est �dit�e par l'utilisateur
		 */
		satc_quotas_edited_fn		_callback_edition;

		/*!
		 * \brief constructeur
		 */
		QuotasValue();

		/*!
		 * \brief Fonction de d�bug pour construire une version texte de l'objet
		 */
		std::string ToString() const;

};


/********************************************************************************/
/*				declaration du type QuotasValues								*/ 
/********************************************************************************/ 

 			
/*!
 * \struct QuotasValues
 * \brief  Rassemble toutes les QuotasValue pour d'une classe de contr�le / sous contingent
 */
struct QuotasValues
{
		/*!
		 * \brief niveau de la classe : 3 pour A3, 4 pour a4, etc ...
		 */
		QuotasValue	_nest_level;

		/*!
		* \brief etanche ou non (valable pour les sous contingent)
		*/

		QuotasValue _etanche;

		/*!
		 * \brief  Nombre d'autorisations vendues ou reservees
		 */ 
		QuotasValue	_sold_res;


		/*!
		 *  \brief  Nombre d'autorisations temps r�els
		 */
		QuotasValue	_act;


		/*!
		 * \brief Nombre d'autorisations selon l'utilisateur
		 */
		QuotasValue	_moi;


		/*!
		 *  \brief Nombre d'autorisations restantes
		 */
		QuotasValue	_dispo;

		/*!
		 * \brief Total des autorisations
		 */
		QuotasValue _res_total;

		/*!
		 * \brief Diff�rence
		 */ 
		QuotasValue _diff;



		/*!
		 * \brief Setter complet
		 */		
		void Set(long vendus_res, long act, long moi, long dispo, long etanche = -1);
		void Set(int res_total, int diff);

		/*!
		 * \brief Fonction de d�bug pour construire une version texte de l'objet 
		 */
		std::string ToString() const;

};


/********************************************************************************/
/*			declaration de la classe QuotasAutorisationsHeaderCtrl				*/ 
/********************************************************************************/ 

/*!
 * \class QuotasAutorisationsHeaderCtrl
 * \brief Controlleur dans les interfaces de bascule de quotas pour g�rer un header de colonnes multilignes
 */

class QuotasAutorisationsHeaderCtrl : public CHeaderCtrl
{
	/****************************************************************************/ 
	/*				constructeurs, destructeur									*/ 
	/****************************************************************************/ 

	public:

	/*!
	 * \brief Constructeur par d�faut. Initialise m_whiteBackgroundBrush
	 */
	QuotasAutorisationsHeaderCtrl();

	/*!
	 * \brief Destructeur virtuel. Ne fait rien
	 */
	virtual ~QuotasAutorisationsHeaderCtrl();

	/****************************************************************************/ 
	/*				impl�mentation des fonctions virtuelles MFC					*/ 
	/****************************************************************************/ 

	/*!
	 * \brief fonction vide
	 *
	 * Cette fonction a �t� d�riv�e afin que la m�thode par d�faut ne soit pas appel�.
	 * Elle est vide car le rendu des colonnes est fait dans la fonction OnPaint
	 * En effet la fonction DrawItem ne permet pas de redessinner les limites de l'objets qui permettent le redimensionnement.
	 */
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	/****************************************************************************/ 
	/*				impl�mentation des messages MFC								*/ 
	/****************************************************************************/ 

	protected:
	//{{AFX_MSG(QuotasAutorisationsHeaderCtrl)

	
	/*!
     * \brief Impl�mentation de la callback pour le message SetCursor
	 * 
	 * La fonction est vide afin d'emp�cher le comportement par d�faut,
	 * c'est � dire le changement de curseur si la souris est sur un marqueur de redimensionnement.
	 */ 
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	/*!
	 * \brief Impl�mentation de la callback pour le message LButtonDblClick
	 *
	 * La fonction est vide afin d'emp�cher le comportement par d�faut,
     * c'est � dire le drap and drop pour redimensionner une des colonnes.
	 */
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	/*!
	 * \brief Impl�mentation de la callback pour le message LButtonDown
	 *
	 * La fonction est vide afin d'emp�cher le comportement par d�faut,
     * c'est � dire le drap and drop pour redimensionner une des colonnes.
	 */
	afx_msg void OnLButtonDown (UINT, CPoint) ;

	/*!
	 * \brief Impl�mentation de la callback pour le message Paint
	 *
	 * La fonction dessine un fond blanc sur l'ensemble de l'espace d'en t�te, puis affiche les noms de colonnes
	 */
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	/****************************************************************************/ 
	/*				protected data member										*/ 
	/****************************************************************************/ 
	/*!
	 * \brief brush pour le remplissage du background - blanc par d�faut
	 */
	CBrush	m_whiteBackgroundBrush;
};

/********************************************************************************/
/*			declaration de la classe QuotasAutorisationsValueEdit				*/ 
/********************************************************************************/ 

class QuotasAutorisationsListCtrl; //Forward declaration of parent class

/*!
 * \brief Class customis�e d'�dition de texte pour g�rer la saisie d'une valeur
 */
class QuotasAutorisationsValueEdit: public CEdit
{
	public:
		QuotasAutorisationsValueEdit();
		int _x;
		int _y;
		QuotasAutorisationsListCtrl *parentList; // SRE 76064 Evol  - Add a pointer to ListCtrl to manage catched kb inputs

	public:
		void setParentCListCtrl(QuotasAutorisationsListCtrl *listAddress);
DECLARE_MESSAGE_MAP()
afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags); // SRE 76064 Evol
afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
 
};
 

/********************************************************************************/
/*			declaration de la classe QuotasAutorisationsListCtrl				*/ 
/********************************************************************************/ 

/*!
 * \class QuotasAutorisationsListCtrl
 * \brief Controlleur dans les interfaces de bascule de quotas pour g�rer une grille d'autorisation
 *  pour un espace physique donn�e, un type (classe de controle ou sous contingent), et un ensemble de niveau
 */

class QuotasAutorisationsListView;

class QuotasAutorisationsListCtrl : public CListCtrl
{
public:
	BOOL beingEdited;
	/****************************************************************************/ 
	/*				constructeurs, destructeur									*/ 
	/****************************************************************************/ 

	/*!
	 * \brief constructeur par d�faut
	 */
	QuotasAutorisationsListCtrl();
	
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags); // SRE 76064 Evol

	/****************************************************************************/ 
	/*				initialisation du contr�le									*/ 
	/****************************************************************************/ 
	/*!
	 *  \fn		void Configure (type_quotas_t type, long nest_levels)
	 *  \brief  Configure la grille pour un espace physique et un nombre de classes de services / sous contingents
	 */
	void Configure(quotas_type_t type, char espace_physique, long max_indexation);

	/*!
	 * \fn		void CreateColumns ()
	 * \brief	Cr�e les colonnes n�c�ssaires
	 * 
	 * Cette fonction cr�e les colonnes n�c�ssaires:( Espace physique |Vdus + Res | Act | Moi |Dispo) 
	 * conform�ment � l'exigence YI- 6134
	 */
	void CreateColumns ();

	/*!
	 * \fn		void BuildRows ();
	 * \brief	Cr�e les lignes de la grilles selon le type et le nombre de classes configur�s
	 */
	void CreateRows();

	/*!
	 * \fn		void CreateBoldFont(CDC& pDC, bool bold)
	 * \brief	Initialise la police caract�re gras
	 */
	void CreateBoldFont(CDC& pDC, bool bold);


	/*!
	 * \fn    void SetFontWeight(quotas_values_t col, size_t indexation, bool bold);
	 * \brief Pr�cise si une case est �crit en font gras ou pas
	 */
	void SetFontWeight(quotas_values_t col, size_t indexation, bool bold);


	/*!
	 * \fn   void SetTextColor(quotas_values_t col, size_t indexation, COLORREF color);
	 * \brief Pr�cise la couleur d'affichage d'une case
	 */
	void SetTextColor(quotas_values_t col, size_t indexation, COLORREF color);

	/*!
	 * \fn   void SetGreyBackground(quotas_values_t col, size_t indexation, bool is_grey);
	 * \brief Pr�cise la couleur de fond d'une case
	 */
	void SetGreyBackground(quotas_values_t col, size_t indexation, bool is_grey);

	/*!
	 * \fn   void SetWhiteBackground(quotas_values_t col, size_t indexation);
	 * \brief Retourne au fond blanc pour le background pour une case donn�e
	 */
	void SetWhiteBackground(quotas_values_t col, size_t indexation);

	/*!
	 * \fn   void SetWhiteBackground(size_t indexation);
	 * \brief Retourne au fond blanc pour le background pour une ligne donn�e
	 */
	void SetWhiteBackground(size_t indexation);

	/*!
	 * \fn   void SetWhiteBackground();
	 * \brief Retourne au fond blanc pour le background sur toute la grille
	 */
	void SetWhiteBackground();


	/*!
	 *  \fn     void SetEditable (quotas_values_t col, size_t indexation, bool is_editable);
	 *  \brief  precise si une case est �ditable
	 */
	void SetEditable (quotas_values_t col, size_t indexation, bool is_editable);

	/*!
	 *  \fn			long IndexationToItem (long indexation) const;
	 *  \brief		Converti un indice d'indexation ( par exemple 3 pour scx a3 ) en un num�ro d'item dans la grille
	 */
	long IndexationToItem (long indexation) const;

	/*!
	 *  \fn			long ItemToIndexation (long item) const;
	 *  \brief      Converti un num�ro d'item dans la grille en une indexation de classe de controle ou de sous contingent
	 */
	long ItemToIndexation (long item) const;

	/*!
	 *  \fn long Sum(quotas_values_t colonne, size_t index_start, size_t index_end) const
	 *  \brief  Calcule la somme dans une colonne de toutes les valeurs entre l'index de classe start et l'index de classe end compris
	 */
	long Sum(quotas_values_t colonne, size_t index_start, size_t index_end) const;

	/*!
	 *   \fn void Clip(size_t& index_start, size_t& index_end) const
	 *   \brief Modifie index_start et index_end pour qu'ils respectent le segment de valeur possible 0 ..._datas.size()
	 */
	void Clip(size_t& index_start, size_t& index_end) const;

	void JumpOnNextList(UINT nChar, UINT nRepCnt, UINT nFlags); // SRE 76064 Evol
	void JumpOnPrevList(UINT nChar, UINT nRepCnt, UINT nFlags); // SRE 76064 Evol

	void EditNextElement(UINT nChar, UINT nRepCnt, UINT nFlags); // SRE 76064 Evol
	void EditPreviousElement(UINT nChar, UINT nRepCnt, UINT nFlags); // SRE 76064 Evol

	/****************************************************************************/
	/*				accesseurs													*/ 
	/****************************************************************************/ 

	void setParentView(QuotasAutorisationsListView *address) //SRE 76064 Evol
	{
		this->parentView = address;
	}

	/*!
	 * \fn quotas_type_t Type() const;
	 * \brief retourne le type de classe ( cc ou scx ) g�r� par le contr�le. Voir enum TYPE_CLASSES pour la liste
	 */
	quotas_type_t Type() const;


	/*!
	 * \fn char EspacePhysique() const;
	 * \brief  retourne l'espace physique g�r� par le contr�le
	 */ 
	char EspacePhysique() const;


	/*!
	 * \fn long MaxIndexation() const;
	 * \brief retourne le niveau maximum d'indexation  des classes g�r�es par le contr�le
	 */
	long MaxIndexation() const;


	/*!
	 * \fn std::vector<QuotasValues> Datas();
	 * \brief retourne une r�f�rence sur le tableau de donn�es
	 */
	std::vector<QuotasValues>& Datas();

	/*!
	 * \fn QuotasValue* GetValue(quotas_values_t col, size_t indexation);
	 * \brief Retourne un pointeur vers le descripteur de la valeur correspondant � la colonne col et � la ligne row, lue � partir du tableau _data. 
	 * Retourne NULL si la valeur n'existe pas
	 */
	QuotasValue* GetValue(quotas_values_t col, size_t indexation);
	const QuotasValue* GetValue(quotas_values_t col, size_t indexation) const;

	/*!
	 * \fn void SetValue(quotas_values_t col, size_t indexation, long value);
	 * \brief Pr�cise la valeur pour une colonne et une indexation de classe / sous contingent
	 */
	void SetValue(quotas_values_t col, size_t indexation, long value);

	/*!
	 * \brief Met � jour les valeurs d'autorisations et r�affiche la ligne de la grille
	 */
	void SetLine (size_t indexation, long vendus_res, long act, long moi, long dispo, long etanche = -1);

	/*!
	 * \brief Affichage des champs res et diff
	 */
	void SetLine (size_t indexation, int res_total, int diff);

	/*!
	* \brief reinitialize les donn�es d'origine  
	*/ 
	void clearData(bool is_inter);

	/****************************************************************************/ 
	/*				impl�mentation des fonctions virtuelles MFC					*/ 
	/****************************************************************************/ 

	/*!
	 * \fn void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct); 
	 * \brief   Customisation de l'affichage pour supporter les colorisations demand�es
	 */
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct); 

	/****************************************************************************/ 
	/*				message map													*/ 
	/****************************************************************************/ 
	
	DECLARE_MESSAGE_MAP()
	//{{AFX_MSG(QuotasAutorisationsListCtrl)

	/*!
	 * \brief Impl�mentation de la callback pour le message LButtonDown
	 *
	 * Surcharge le fonctionnement par d�faut.
	 * Si la case cliqu�e est flagg�e �ditable, cr�e une fen�tre de saisie de type QuotasAutorisationsValueEdit 
	 * par dessus afin de saisir la nouvelle valeur.
	 */
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	/*!
	 *  \brief Implementation de la callback pour le message LvnBeginLabelEdit
	 *
	 *  G�re l'ouverture de la fen�tre de saisie de type QuotasAutorisationsValueEdit
	 */
	afx_msg void OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT    *pResult);

	/*!
	 * \brief Impl�mentation de la callback pour le message LvnEndLabelEdit
	 *
	 * Si la case �dit�e poss�de une callback, celle fonction est appel�e
	 * Puis met � jour la donn�e dans la grille et d�truit la fen�tre d'�dition
	 */
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);

	/*!
	* \brief Impl�mentation de la callback pour le message Paint
	*
	* Force l'affichage par dessus la liste de la fen�tre d'�dition si elle existe.
	*/
	afx_msg void OnPaint();
	//}}AFX_MSG

	/****************************************************************************/ 
	/*				initialisation du contr�le									*/ 
	/****************************************************************************/ 
	protected:

	/*!
	 * \brief   Type de classe g�r� par la fen�tre, voir enum TYPE_QUOTAS
	 */
	quotas_type_t	_type;		


	/*!
	 * \brief Espace physique
	 */
	char			_espace_physique;


	/*!
	 * \brief Nombre de classes � afficher pour le type configur�
	 */
	long			_max_indexation;


	/*!
	 *  \classe indiquant l'index et le d�placement envoy� par les EditBox � la bascule.
	 */
	Bascules::DirectionInfo infos;
	
	/*!
	 *  \brief tableau des donn�es � afficher
	 */
	std::vector<QuotasValues>	_datas;


	/*!
	 * \brief brush pour le remplissage du background - blanc par d�faut
	 */
	CBrush	m_defaultBackgroundBrush;


	/*!
	 *  \brief brush pour le remplissage du background - gris
	 */
	CBrush m_greyBackgroundBrush;

	/*!
	 *  \brief brush pour le remplissage du background - gris
	 */
	CBrush m_CCBackgroundBrush;

	/*!
	 *  \brief brush pour le remplissage du background - gris
	 */
	CBrush m_SCBackgroundBrush;


	/*!
	 * \brief Font en gras
	 */
	CFont m_boldFont;

	/*!
	 *  \brief pointeur vers la valeur en cours d'�dition
	 */
	QuotasValue*	_edited;


	/*!
	 * \brief controleur pour l'edition de valeur
	 */
	QuotasAutorisationsValueEdit _ctrl_edit;
	private:
		int editedItemIndex; //SRE 76064 Evol - Edit box index tracking
		QuotasAutorisationsListView *parentView;
};


/********************************************************************************/
/*																				*/ 
/*			d�claration de la classe QuotasAutorisationsListView				*/ 
/*																				*/ 
/********************************************************************************/ 

/*!
 * \class QuotasAutorisationsListView
 * \brief Classe synth�tisant un listctrl et le headctrl associ� avec le maximum de code g�n�rique pour �viter les r�p�titions dans le code des interfaces
 */

class QuotasAutorisationsListView
{
public:
	void setListId(int _ID);
	int getListId();
	int ID;
	//SRE 76064 Evol /

	/*!
	 * \brief Reconstruit la vue
	 */
	void Build (quotas_type_t type, char espace_physique, size_t max_indexation);

	/*!
	 *  \brief Cache / affiche la grille
	 */
	BOOL ShowWindow(int nCmdShow);

	/*!
	 * \brief Met � jour les valeurs d'autorisations et r�affiche la ligne de la grille
	 */
	void SetLine (size_t indexation, long vendus_res, long act, long moi, long dispo, long etanche = -1);

	void ClearData();

	void ConfigureChildList() // SRE 76064 Evol - The list needs this class address to be able to send Msgs
	{
		this->_list_ctrl.setParentView(this);
	}

	/****************************************************************************/
	/*			public datas													*/ 
	/****************************************************************************/ 
	public:

	/*!
	 * \brief Police de caract�re utilis� pour le rendu texte
	 */
	CFont								_head_font;

	/*!
	 * \brief Controlleur pour g�rer la grille des quotas par classe de contr�le ou sous contingent.
	 */
	QuotasAutorisationsListCtrl			_list_ctrl;

	/*!
	 * \brief Controlleur pour g�rer l'affichage des ent�tes de colonnes de _list_ctrl pour coller au style graphique demand�.
	 */
	QuotasAutorisationsHeaderCtrl		_head_ctrl;
};


/********************************************************************************/
/*																				*/ 
/*			impl�mentation des fonctions inlines								*/ 
/*																				*/ 
/********************************************************************************/ 
inline
quotas_type_t QuotasAutorisationsListCtrl::Type() const
{ return this->_type; }

inline
char QuotasAutorisationsListCtrl::EspacePhysique() const
{ return this->_espace_physique; }

inline
long QuotasAutorisationsListCtrl::MaxIndexation() const
{ return _max_indexation; }

inline
std::vector<QuotasValues>& QuotasAutorisationsListCtrl::Datas()
{ return _datas; }

#endif
//End of file
