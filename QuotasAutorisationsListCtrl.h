/*! \file		QuotasAutorisationsListCtrl.h
*   \brief	    Lot : 7978
*               List Control gérant une colonne d'autorisations de cc ou de scx dans les fenètres de bascule de quotas
*   \author     Frederic Manisse
*   \date       1 décembre 2014
*/
#ifndef _QUOTAS_AUTORISATIONS_LIST_CTRL_H_
#define _QUOTAS_AUTORISATIONS_LIST_CTRL_H_

/********************************************************************************/ 
/*			section include														*/ 
/********************************************************************************/ 

/********************************************************************************/ 
/*			déclaration des enums et types liés à la bascule des quotas			*/ 
/********************************************************************************/ 


class Quotas
{
public:

	/*!
	 * \enum TYPES_QUOTAS
     * \brief Types de classes ayant des autorisations gérées par ce contrôle
	 */
	enum TYPES_QUOTAS
	{
		CC	= 0,		
	    SCX	= 1,
	};


	/*!
	 * \enum   TYPES_VALUES
	 * \brief  Listing des différentes valeurs affichées / impliquées dans la bascule des quotas. 
	 *		   Chaque valeur correspond à une colonne dans les grilles des autorisations
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
 * \brief typedef pour représenter un enum TYPES_QUOTAS
 */
typedef Quotas::TYPES_QUOTAS quotas_type_t;

/*!
 * \brief typedef pour représenter un enum TYPES_VALUES
 */
typedef Quotas::TYPES_VALUES quotas_values_t;

/********************************************************************************/
/*			declaration des callback d'édition de valeurs						*/ 
/********************************************************************************/ 

/*!
 * \brief	typedef pour les fonctions de callback sur l'édition d'une valeur
 *          La callback a la possibilité de modifier la valeur pour correction
 */ 

//typedef void (*satc_quotas_edited_fn) (quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);

typedef void (*satc_quotas_edited_fn) (quotas_type_t type, char espace_physique, quotas_values_t type_value, long indexation, long& valeur);


/********************************************************************************/
/*				declaration du type QuotasValue									*/ 
/********************************************************************************/ 

 			
/*!
 * \struct QuotasValue
 * \brief  Stocke le format pour l'affichage d'une donnée ainsi que sa valeur et la callback à appeler en cas d'édition
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
		 *  \brief vrai si la valeur est éditable
		 */
		bool  _editable;


		/*!
		 * \brief vrai si la valeur doit être affichée en gras
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
		 * \brief callback à appeler lorsque la valeur est éditée par l'utilisateur
		 */
		satc_quotas_edited_fn		_callback_edition;

		/*!
		 * \brief constructeur
		 */
		QuotasValue();

		/*!
		 * \brief Fonction de débug pour construire une version texte de l'objet
		 */
		std::string ToString() const;

};


/********************************************************************************/
/*				declaration du type QuotasValues								*/ 
/********************************************************************************/ 

 			
/*!
 * \struct QuotasValues
 * \brief  Rassemble toutes les QuotasValue pour d'une classe de contrôle / sous contingent
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
		 *  \brief  Nombre d'autorisations temps réels
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
		 * \brief Différence
		 */ 
		QuotasValue _diff;



		/*!
		 * \brief Setter complet
		 */		
		void Set(long vendus_res, long act, long moi, long dispo, long etanche = -1);
		void Set(int res_total, int diff);

		/*!
		 * \brief Fonction de débug pour construire une version texte de l'objet 
		 */
		std::string ToString() const;

};


/********************************************************************************/
/*			declaration de la classe QuotasAutorisationsHeaderCtrl				*/ 
/********************************************************************************/ 

/*!
 * \class QuotasAutorisationsHeaderCtrl
 * \brief Controlleur dans les interfaces de bascule de quotas pour gérer un header de colonnes multilignes
 */

class QuotasAutorisationsHeaderCtrl : public CHeaderCtrl
{
	/****************************************************************************/ 
	/*				constructeurs, destructeur									*/ 
	/****************************************************************************/ 

	public:

	/*!
	 * \brief Constructeur par défaut. Initialise m_whiteBackgroundBrush
	 */
	QuotasAutorisationsHeaderCtrl();

	/*!
	 * \brief Destructeur virtuel. Ne fait rien
	 */
	virtual ~QuotasAutorisationsHeaderCtrl();

	/****************************************************************************/ 
	/*				implémentation des fonctions virtuelles MFC					*/ 
	/****************************************************************************/ 

	/*!
	 * \brief fonction vide
	 *
	 * Cette fonction a été dérivée afin que la méthode par défaut ne soit pas appelé.
	 * Elle est vide car le rendu des colonnes est fait dans la fonction OnPaint
	 * En effet la fonction DrawItem ne permet pas de redessinner les limites de l'objets qui permettent le redimensionnement.
	 */
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	/****************************************************************************/ 
	/*				implémentation des messages MFC								*/ 
	/****************************************************************************/ 

	protected:
	//{{AFX_MSG(QuotasAutorisationsHeaderCtrl)

	
	/*!
     * \brief Implémentation de la callback pour le message SetCursor
	 * 
	 * La fonction est vide afin d'empêcher le comportement par défaut,
	 * c'est à dire le changement de curseur si la souris est sur un marqueur de redimensionnement.
	 */ 
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	/*!
	 * \brief Implémentation de la callback pour le message LButtonDblClick
	 *
	 * La fonction est vide afin d'empêcher le comportement par défaut,
     * c'est à dire le drap and drop pour redimensionner une des colonnes.
	 */
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	/*!
	 * \brief Implémentation de la callback pour le message LButtonDown
	 *
	 * La fonction est vide afin d'empêcher le comportement par défaut,
     * c'est à dire le drap and drop pour redimensionner une des colonnes.
	 */
	afx_msg void OnLButtonDown (UINT, CPoint) ;

	/*!
	 * \brief Implémentation de la callback pour le message Paint
	 *
	 * La fonction dessine un fond blanc sur l'ensemble de l'espace d'en tête, puis affiche les noms de colonnes
	 */
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	/****************************************************************************/ 
	/*				protected data member										*/ 
	/****************************************************************************/ 
	/*!
	 * \brief brush pour le remplissage du background - blanc par défaut
	 */
	CBrush	m_whiteBackgroundBrush;
};

/********************************************************************************/
/*			declaration de la classe QuotasAutorisationsValueEdit				*/ 
/********************************************************************************/ 

class QuotasAutorisationsListCtrl; //Forward declaration of parent class

/*!
 * \brief Class customisée d'édition de texte pour gérer la saisie d'une valeur
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
 * \brief Controlleur dans les interfaces de bascule de quotas pour gérer une grille d'autorisation
 *  pour un espace physique donnée, un type (classe de controle ou sous contingent), et un ensemble de niveau
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
	 * \brief constructeur par défaut
	 */
	QuotasAutorisationsListCtrl();
	
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags); // SRE 76064 Evol

	/****************************************************************************/ 
	/*				initialisation du contrôle									*/ 
	/****************************************************************************/ 
	/*!
	 *  \fn		void Configure (type_quotas_t type, long nest_levels)
	 *  \brief  Configure la grille pour un espace physique et un nombre de classes de services / sous contingents
	 */
	void Configure(quotas_type_t type, char espace_physique, long max_indexation);

	/*!
	 * \fn		void CreateColumns ()
	 * \brief	Crée les colonnes nécéssaires
	 * 
	 * Cette fonction crée les colonnes nécéssaires:( Espace physique |Vdus + Res | Act | Moi |Dispo) 
	 * conformément à l'exigence YI- 6134
	 */
	void CreateColumns ();

	/*!
	 * \fn		void BuildRows ();
	 * \brief	Crée les lignes de la grilles selon le type et le nombre de classes configurés
	 */
	void CreateRows();

	/*!
	 * \fn		void CreateBoldFont(CDC& pDC, bool bold)
	 * \brief	Initialise la police caractère gras
	 */
	void CreateBoldFont(CDC& pDC, bool bold);


	/*!
	 * \fn    void SetFontWeight(quotas_values_t col, size_t indexation, bool bold);
	 * \brief Précise si une case est écrit en font gras ou pas
	 */
	void SetFontWeight(quotas_values_t col, size_t indexation, bool bold);


	/*!
	 * \fn   void SetTextColor(quotas_values_t col, size_t indexation, COLORREF color);
	 * \brief Précise la couleur d'affichage d'une case
	 */
	void SetTextColor(quotas_values_t col, size_t indexation, COLORREF color);

	/*!
	 * \fn   void SetGreyBackground(quotas_values_t col, size_t indexation, bool is_grey);
	 * \brief Précise la couleur de fond d'une case
	 */
	void SetGreyBackground(quotas_values_t col, size_t indexation, bool is_grey);

	/*!
	 * \fn   void SetWhiteBackground(quotas_values_t col, size_t indexation);
	 * \brief Retourne au fond blanc pour le background pour une case donnée
	 */
	void SetWhiteBackground(quotas_values_t col, size_t indexation);

	/*!
	 * \fn   void SetWhiteBackground(size_t indexation);
	 * \brief Retourne au fond blanc pour le background pour une ligne donnée
	 */
	void SetWhiteBackground(size_t indexation);

	/*!
	 * \fn   void SetWhiteBackground();
	 * \brief Retourne au fond blanc pour le background sur toute la grille
	 */
	void SetWhiteBackground();


	/*!
	 *  \fn     void SetEditable (quotas_values_t col, size_t indexation, bool is_editable);
	 *  \brief  precise si une case est éditable
	 */
	void SetEditable (quotas_values_t col, size_t indexation, bool is_editable);

	/*!
	 *  \fn			long IndexationToItem (long indexation) const;
	 *  \brief		Converti un indice d'indexation ( par exemple 3 pour scx a3 ) en un numéro d'item dans la grille
	 */
	long IndexationToItem (long indexation) const;

	/*!
	 *  \fn			long ItemToIndexation (long item) const;
	 *  \brief      Converti un numéro d'item dans la grille en une indexation de classe de controle ou de sous contingent
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
	 * \brief retourne le type de classe ( cc ou scx ) géré par le contrôle. Voir enum TYPE_CLASSES pour la liste
	 */
	quotas_type_t Type() const;


	/*!
	 * \fn char EspacePhysique() const;
	 * \brief  retourne l'espace physique géré par le contrôle
	 */ 
	char EspacePhysique() const;


	/*!
	 * \fn long MaxIndexation() const;
	 * \brief retourne le niveau maximum d'indexation  des classes gérées par le contrôle
	 */
	long MaxIndexation() const;


	/*!
	 * \fn std::vector<QuotasValues> Datas();
	 * \brief retourne une référence sur le tableau de données
	 */
	std::vector<QuotasValues>& Datas();

	/*!
	 * \fn QuotasValue* GetValue(quotas_values_t col, size_t indexation);
	 * \brief Retourne un pointeur vers le descripteur de la valeur correspondant à la colonne col et à la ligne row, lue à partir du tableau _data. 
	 * Retourne NULL si la valeur n'existe pas
	 */
	QuotasValue* GetValue(quotas_values_t col, size_t indexation);
	const QuotasValue* GetValue(quotas_values_t col, size_t indexation) const;

	/*!
	 * \fn void SetValue(quotas_values_t col, size_t indexation, long value);
	 * \brief Précise la valeur pour une colonne et une indexation de classe / sous contingent
	 */
	void SetValue(quotas_values_t col, size_t indexation, long value);

	/*!
	 * \brief Met à jour les valeurs d'autorisations et réaffiche la ligne de la grille
	 */
	void SetLine (size_t indexation, long vendus_res, long act, long moi, long dispo, long etanche = -1);

	/*!
	 * \brief Affichage des champs res et diff
	 */
	void SetLine (size_t indexation, int res_total, int diff);

	/*!
	* \brief reinitialize les données d'origine  
	*/ 
	void clearData(bool is_inter);

	/****************************************************************************/ 
	/*				implémentation des fonctions virtuelles MFC					*/ 
	/****************************************************************************/ 

	/*!
	 * \fn void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct); 
	 * \brief   Customisation de l'affichage pour supporter les colorisations demandées
	 */
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct); 

	/****************************************************************************/ 
	/*				message map													*/ 
	/****************************************************************************/ 
	
	DECLARE_MESSAGE_MAP()
	//{{AFX_MSG(QuotasAutorisationsListCtrl)

	/*!
	 * \brief Implémentation de la callback pour le message LButtonDown
	 *
	 * Surcharge le fonctionnement par défaut.
	 * Si la case cliquée est flaggée éditable, crée une fenêtre de saisie de type QuotasAutorisationsValueEdit 
	 * par dessus afin de saisir la nouvelle valeur.
	 */
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	/*!
	 *  \brief Implementation de la callback pour le message LvnBeginLabelEdit
	 *
	 *  Gère l'ouverture de la fenêtre de saisie de type QuotasAutorisationsValueEdit
	 */
	afx_msg void OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT    *pResult);

	/*!
	 * \brief Implémentation de la callback pour le message LvnEndLabelEdit
	 *
	 * Si la case éditée possède une callback, celle fonction est appelée
	 * Puis met à jour la donnée dans la grille et détruit la fenêtre d'édition
	 */
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);

	/*!
	* \brief Implémentation de la callback pour le message Paint
	*
	* Force l'affichage par dessus la liste de la fenêtre d'édition si elle existe.
	*/
	afx_msg void OnPaint();
	//}}AFX_MSG

	/****************************************************************************/ 
	/*				initialisation du contrôle									*/ 
	/****************************************************************************/ 
	protected:

	/*!
	 * \brief   Type de classe géré par la fenêtre, voir enum TYPE_QUOTAS
	 */
	quotas_type_t	_type;		


	/*!
	 * \brief Espace physique
	 */
	char			_espace_physique;


	/*!
	 * \brief Nombre de classes à afficher pour le type configuré
	 */
	long			_max_indexation;


	/*!
	 *  \classe indiquant l'index et le déplacement envoyé par les EditBox à la bascule.
	 */
	Bascules::DirectionInfo infos;
	
	/*!
	 *  \brief tableau des données à afficher
	 */
	std::vector<QuotasValues>	_datas;


	/*!
	 * \brief brush pour le remplissage du background - blanc par défaut
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
	 *  \brief pointeur vers la valeur en cours d'édition
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
/*			déclaration de la classe QuotasAutorisationsListView				*/ 
/*																				*/ 
/********************************************************************************/ 

/*!
 * \class QuotasAutorisationsListView
 * \brief Classe synthétisant un listctrl et le headctrl associé avec le maximum de code générique pour éviter les répétitions dans le code des interfaces
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
	 * \brief Met à jour les valeurs d'autorisations et réaffiche la ligne de la grille
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
	 * \brief Police de caractère utilisé pour le rendu texte
	 */
	CFont								_head_font;

	/*!
	 * \brief Controlleur pour gérer la grille des quotas par classe de contrôle ou sous contingent.
	 */
	QuotasAutorisationsListCtrl			_list_ctrl;

	/*!
	 * \brief Controlleur pour gérer l'affichage des entêtes de colonnes de _list_ctrl pour coller au style graphique demandé.
	 */
	QuotasAutorisationsHeaderCtrl		_head_ctrl;
};


/********************************************************************************/
/*																				*/ 
/*			implémentation des fonctions inlines								*/ 
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
