#ifndef CTACONSTANTS_H
#define CTACONSTANTS_H

//  FILE: CTAConstants.h
//
//  DESCRIPTION: 
//  Constants of the CTA Application.
//
//  REV: 1.0
//  CREATED: September 1997
//  AUTHOR:  Casey Jones
//

// CTA Record Set Keys
//
#define DCDATE_KEY    100
#define SYSDATE_KEY   101
#define SCSTATUS_KEY  102
#define MAX_ENTITY_RRD_KEY   103
#define CMPT_KEY      104
#define DEF_RRD_KEY   105
#define USERINFO_KEY  106
#define SYSPARM_KEY   107
#define HISTOSTATUS_KEY  108
#define BKTEVOL_AVG_KEY     109
#define SVC_KEY	            110
#define TRAIN_ENTITY_KEY	111		//VNL ENGINE
#define TRAIN_TRANCHE_KEY	112		//VNL ENGINE
#define SERVLEG_WENT_KEY	113		//VNL ENGINE
#define SERVLEG_KEY			114		//VNL ENGINE
#define SERVMARKET_WENT_KEY	115		//VNL ENGINE
#define SERVMARKET_KEY		116		//VNL ENGINE
#define LEGTIMES_KEY		117
#define BKTEVOL_AVG_ALT_KEY	118
#define TGVGAG_KEY	        119

#define ENTITY_KEY    120
#define TRANCHE_KEY   121
#define LEG_KEY       122
#define BKTEVOL_KEY   123
#define CMPTEVOL_KEY  124
#define BKTHIST_KEY   125
#define HOITRAI_KEY   126
#define HOITRAR_KEY   127
#define HISTDPT_KEY   128
#define OTHLEGS_KEY   129
#define RELTRANCHE_KEY  130
#define RESABKT_KEY   131
#define RESACMPT_KEY  132
#define VIRTNEST_KEY  133
#define TGVJUM_KEY	  134
#define STATION_KEY	  135
#define TDLJK_KEY	  136
#define TDPJE_KEY	  137
#define BRMS_KEY	  138

#define CALENDAR_TRAIN_KEY  139
#define CALENDAR_TDLJK_KEY  140
#define MINMAX_DPT_KEY    141
#define RESABKT_SCI_KEY   142
#define RESACMPT_SCI_KEY  143
#define SEASONS_KEY       144
#define CONGES_KEY        145
#define HISTDPT_ALT_KEY   146
#define BKTEVOL_ALT_KEY   147
#define CMPTEVOL_ALT_KEY  148
#define BKTHIST_ALT_KEY   149
#define TOP_KEY           150
#define SENSIBILITE_KEY   151
#define OTHLEGS_ALT_KEY   152
#define RELTRANCHE_ALT_KEY  153
#define CALENDAR_PAST_TRAIN_KEY  154
#define ALLTRAIN_WENT_KEY	155	  //VNL ENGINE
#define ALLTRAIN_KEY		156	  //VNL ENGINE
#define TGVSTATIONS_KEY     157
#define COMPTAGES_KEY		158	  // Comptages (TRN Client)
#define VNAU_LIST_KEY		159	  // Liste des VNAU 
//#define COACHCONFIGS_KEY  160
#define PREVISION_KEY       161
#define TRAINSUPPLEMENT_KEY		162	  // Tranche Supplement (TRN Client)
#define TRAINADAPTATION_KEY		163	  // Train Adaptation (TRN Client)
#define TRAINDELETED_KEY		164	  // Tranche Deleted (TRN Client)

#define OPTIMVNAUOPEN_KEY		165	  // VNAY optimisés
#define OPTIMVNAULAST_KEY		166	  // VNAY optimisés
#define ENTITY_GAG_KEY	        167
#define DATE_HISTO_KEY	        168
#define HISTOEVOL_KEY			169
#define HISTOEVOL_ALT_KEY		170



#define COMPTAGES_HIST_KEY		171	  // Historiques Comptages (TRN Client)
#define BKTEVOL_AVG_REC_KEY	    172	  
#define QUELCMPT_KEY			173	  // Pour savoir les compartiments exacts de la famille car les comptages F
#define BKTEVOL_AVG_REC_ALT_KEY	174	  

#define BKTHISTO_KEY			175
#define BKTHISTO_ALT_KEY	    176

#define CMPTEVOL_AVG_KEY		177
#define CMPTEVOL_AVG_ALT_KEY	178

#define ODTRAIN_WENT_KEY	179		//VNL ENGINE
#define ODTRAIN_KEY			180		//VNL ENGINE

#define FULL_JX_KEY		181

// revenus DP 5350
// fenêtre historique par depart
#define HISTDPT_YAV_KEY		182
#define HISTDPT_YAV_ALT_KEY	183

// fenêtre montee en charge recette
#define HISTOEVOL_REC_KEY			184
#define HISTOEVOL_REC_ALT_KEY		185
#define CMPTEVOL_REC_KEY            186
#define CMPTEVOL_REC_ALT_KEY        187
#define BKTEVOL_REC_KEY             188
#define BKTEVOL_REC_ALT_KEY         189
#define CMPTEVOL_AVG_REC_KEY        190
#define CMPTEVOL_AVG_REC_ALT_KEY    191

#define RELATEDTGV_KEY			192
#define RELATEDTGV_ALT_KEY	    193

//DM 7093, j-x incertains en historique.
#define HISTO_HOITRAI_KEY   194
#define HISTO_HOITRAR_KEY   195

//recup des valeurs avec remise a disposition dans histo par bucket DM 6496
#define PREVAUTH_KEY			196
#define TRAQUE_TARIF_KEY		197
#define VNAU_EXCLU_KEY			198
#define HISTOEVOL_CMPT_KEY		199
#define HISTOEVOL_CMPT_ALT_KEY	200
#define HISTOEVOL_RECCMPT_KEY			201
#define HISTOEVOL_RECCMPT_ALT_KEY		202

// DM 7790
#define AXE_KEY	203
#define ROUTE_AXE_KEY	204
#define ENTITY_ROUTE_KEY	205
#define ALLTRAIN_WAXES_KEY	206
#define ALLTRAIN_WROUTES_KEY	207


// DM 7978 
#define VNL_STUB_KEY	208
// DM 7978 - KGH-
#define SPHERES_PRIX_KEY 209
#define BKT_SPHERES_PRIX_KEY 210
#define SCI_SPHERES_PRIX_KEY 211
#define SPHERES_ENTITY_NATURE_ID_KEY 212
// DM 7978 - KGH- FIN
// DM 7978 - NCH
#define CC_USER_KEY 213
// DM 7978 - LME 
#define HISTO_DATA_RRD_KEY 214
// DM 7978 - LME - YI-6046 
#define ASC_TARIF_KEY 215
// DM-7978 - CLE
#define CC_MAX_CC_OUV_KEY 216
#define CC_MAX_CC_OUV_LEGS_KEY 217
// DM-7978 - CLE

// NPI - DM7870 : Dates de référence
#define ENTITY_NATURE_KEY	218

// TDR - Evol prevision
#define PREVISION_VENTE_KEY 219
#define PREVISION_REVENU_KEY 220

// ATTENTION dans BktHistView.h, les requêtes de récupération des infos TGV jumeaux sont identifiées à partir de 300.

#define USERALLOWED(s) ((s.CompareNoCase(TACTICAL) == 0) || (s.CompareNoCase(DVLPMT) == 0))
#define VNAXALLOWED(s) ((s.CompareNoCase(TACTICAL) == 0) || (s.CompareNoCase(DVLPMT) == 0) || (s.CompareNoCase(ANALYST) == 0))
#define VNAUALLOWED(s) ((s.CompareNoCase(JV) == 0) || (s.CompareNoCase(TACTICAL) == 0) || (s.CompareNoCase(ANALYST) == 0))
#define DEVELOPMENT(s) (s.CompareNoCase(DVLPMT) == 0)

// Convert French Francs to Euro
#define CONVERT_FRANC_TO_EURO( x ) ( (x == 0) ? 0 : (double)((double)x / (double)6.55957) )

// Delais d'attente de timer
#define TOO_OLD_PENDING 120 // time to consider that a VNAU is pending
							// since a too long time (in second).
#define PRESENCE_ELAPSE 10  // time in second to verify the presence test of the
							// communication server.
#define PRESENCE_TIMER_ID	23 // timer id pour le test de presence
#define TIMER_TEST_VNAU     20 // timer id pour le test des vnau en attente.
#define RECONNECT_TIMER     21 // timer id pour les tentatives de reconnexion.
#define SOCKET_BLOCKING_TIMER 22 // timer id pour tuer un appel socket bloqué
// Bitmap offsets for tranche list and leg list
//
//  IDB_STATUS_BITMAP
const int BMP_COMMENT		           =  0;
const int BMP_EDIT_NOMOD			     =  1;
const int BMP_EDIT_AUTHMODNOTSENT	 =  2;
const int BMP_EDIT_NOTSEEN         =  3;
const int BMP_EDIT_TGVJUM_100PC		 =  4;	// Présence TGV jumeaux + 100% remplissage sur au moins un CMPT/LEG
const int BMP_EDIT_TGVMODSENT		   =  5;
const int BMP_FORCE_RECOMMEND		   =  6;
const int BMP_FORCE_ACCEPTED		   =  7;
const int BMP_FORCE_REFUSED		     =  8;
const int BMP_FORCE_MODIFIED		   =  9;
const int BMP_REVIEW_NOGO		       = 10;
const int BMP_SENT			           = 11;
const int BMP_SUP				           = 12;
const int BMP_REVIEW_REVIEWIT		   = 13;
const int BMP_LEGAUTH_DUPLICATE	   = 14; // DM 4802, recup de cette icone Virgin uniquement, c'était l'icone classique de copie 2 feuilles superposées
const int BMP_YIELDOD_YOD		   =14;
const int BMP_TRANCHE_INFLUENTE		= 15;
const int BMP_SUSPENDED		= 16;	// DM 4802, recup de cette icone Virgin uniquement, c'était un triangle "attention danger" sur fond jaune.
const int BMP_YIELDOD_YT	= 16;
const int BMP_FCST_ADJUSTED		= 17; // DM 4802, recup de cette icone Virgin uniquement, c'etait une main avec un iedex pointant vers le bas à gauche.
const int BMP_REVIEW_NEVER		= 17;
const int BMP_TO_BE_SENT = BMP_TRANCHE_INFLUENTE;  // clock
const int BMP_RECIEVED_AND_SENT	= 18; // DM 4802, recup de cette icone Virgin uniquement, c'était une page avec un point rouge en haut a droite et 3 petits traits sur le côté gauche
const int BMP_YIELDOD_OD = 18;
const int BMP_RHEALYS_NOTSENT	= 19; // triangle noir évidé
const int BMP_RHEALYS_OKAY			= 20; // smiley blanc
const int BMP_RHEALYS_SENT			= 21; // fléche noire évidée
const int BMP_SENT_COMMERCIAL	= 22;
const int BMP_EDIT_TGVJUM_80PC		= 23; // Présence TGV jumeaux + 80% remplissage sur au moins un CMPT/LEG
const int BMP_SENT_USER_ET_COMMERC  = 24; // Sent_flag = 3.
const int BMP_SENT_HRRD				= 25; // Sent_flag = 4
const int BMP_SENT_USER_ET_HRRD		= 26; // Sent_flag = 5
const int BMP_SENT_COMMERC_ET_HRRD	= 27; // Sent_flag = 6
const int BMP_SENT_U_C_H			= 28; // Sent_flag = 7
const int BMP_DESSERTE_FLAG			= 29; // Changement de desserte
const int BMP_SENT_NOMOS			= 30; // Sent_flag = 8,  envoi VNAU par modèle NOMOS
const int BMP_SENT_PLUS_NOMOS		= 31; // Sent_flag = 9
const int BMP_SENT_COMMER_NOMOS		= 32; // Sent_flag = 10
const int BMP_SENT_USER_COMMER_NOMOS	= 33; // Sent_flag = 11
const int BMP_SENT_HRRD_NOMOS		= 34; // Sent_flag = 12
const int BMP_SENT_USER_HRRD_NOMOS	= 35; // Sent_flag = 13
const int BMP_SENT_COMMER_HRRD_NOMOS	= 36; // Sent_flag = 14
const int BMP_SENT_U_C_H_N			= 37; // Sent_flag = 15
const int BMP_VNAU_M7J				= 38; // fleche kaki, DM 6881 VNAU commerciaux envoyés depuis 7 jours 
const int BMP_VNAU_M6J				= 39; // fleche verte, DM 6881 VNAU commerciaux envoyés depuis 6 jours
const int BMP_VNAU_M5J				= 40; // fleche bleu, DM 6881 VNAU commerciaux envoyés depuis 5 jours
const int BMP_VNAU_M4J				= 41; // fleche rose, DM 6881 VNAU commerciaux envoyés depuis 4 jours
const int BMP_VNAU_M3J				= 42; // fleche pourpre, DM 6881 VNAU commerciaux envoyés depuis 3 jours
const int BMP_VNAU_M2J				= 43; // fleche orange, DM 6881 VNAU commerciaux envoyés depuis 2 jours
const int BMP_VNAU_M1J				= 44; // fleche rouge, DM 6881 VNAU commerciaux envoyés hier
const int BMP_SUPP_RECOMMEND		= 45; // combinaison train supplémentaire + forcement demandé
const int BMP_SUPP_ACCEPTED		    = 46; // combinaison train supplémentaire + forcement accepté
const int BMP_SUPP_REFUSED		    = 47; // combinaison train supplémentaire + forcement refusé
const int BMP_SUPP_MODIFIED		    = 48; // combinaison train supplémentaire + forcement modifié
const int BMP_MDI					= 49; // il existe une ou des gares à montée ou descente interdite
const int BMP_ODI					= 50; // il existe une ou des OD isolées non vendable
const int BMP_PDI					= 51; // mixte des 2 (ODI + MDI)
const int BMP_EX_MDI					= 52; // il a existé une ou des gares à montée ou descente interdite
const int BMP_EX_ODI					= 53; // il a existé une ou des OD isolées non vendable
const int BMP_EX_PDI					= 54; // il a existé un mixte des 2 (ODI + MDI)


//  IDB_STATUS_BITMAP_SUPPLEMENT
const int BMP_TRAIN_SUPPLEMENT_NOTSEEN  =  0;
const int BMP_TRAIN_SUPPLEMENT_ADDED		= 1;
const int BMP_TRAIN_SUPPLEMENT_DELETED		= 2;
const int BMP_TRAIN_SUPPLEMENT_ADDED_DB		= 3;
const int BMP_TRAIN_SUPPLEMENT_DELETED_DB	= 4;
const int BMP_TRAIN_SUPPLEMENT_BASEPLAN		= 5;
const int BMP_TRAIN_SUPPLEMENT_BASEPLAN_DB	= 6;
const int BMP_TRAIN_SUPPLEMENT_UNDETERMINED		= 7;
const int BMP_TRAIN_SUPPLEMENT_UNDETERMINED_DB	= 8;
const int BMP_TRAIN_SUPPLEMENT_TRAINDELETED		= 9;
const int BMP_TRAIN_SUPPLEMENT_TRAINDELETED_DB	= 10;
const int BMP_TRAIN_SUPPLEMENT_DYSFUNCTION		= 11;
const int BMP_TRAIN_SUPPLEMENT_DYSFUNCTION_DB	= 12;

//  IDB_VNAS_BITMAP
const int BMP_CCM1_SEUL	= 0;
const int BMP_VNAS_SENT = 1;
const int BMP_VNAS_SENT_COMMERCIAL = 2;
const int BMP_VNAS_SENT_USER_ET_COMMERC = 3;
const int BMP_VNAS_SENT_HRRD = 4;
const int BMP_VNAS_SENT_USER_ET_HRRD = 5;
const int BMP_VNAS_SENT_COMMERC_ET_HRRD = 6;
const int BMP_VNAS_SENT_U_C_H = 7;
// On met les icones de la DM 7320 avec celles des VNAS.
const int BMP_TRPREV_FAC = 32;		// tranche prévisionnelle facultative
const int BMP_TRPREV_SUS = 33;		// tranche prévisionnelle suspendue
const int BMP_TRPREV_CLOSEBIZ = 34; // not_open_flag + trprev biz
const int BMP_TRPREV_CLOSE = 35;    // not open_flag, idem BMP_REVIEW_NOGO (sens interdit)
const int BMP_TRPREV_BIZ = 36;      // tranche prévisionnelle autre que F ou S

// Edit status as stored in the database
//
const int EDIT_STATUS_NOTSEEN           = 0;
const int EDIT_STATUS_OKAY              = 1;
const int EDIT_STATUS_AUTHMODNOTSENT	  = 2;
const int EDIT_STATUS_TGVMODNOTSENT		  = 4;
const int EDIT_STATUS_TGVJUMPCTSET		= 8;
const int EDIT_STATUS_OKAY_RHEALYS		= 16;

// Sent status as stored in the database
//
const int SENT_STATUS_SENT              = 0;
const int SENT_STATUS_NOTSENT           = 1;

// Review status as stored in the database
//
const int REVIEW_STATUS_NOREVIEW        = 0;
const int REVIEW_STATUS_REVIEWIT        = 1;
const int REVIEW_STATUS_REVIEWNEVER     = 2;

// Force status as stored in the database
//
const int FORCE_STATUS_NOACTION         = 0;
const int FORCE_STATUS_RECOMMENDED      = 1;
const int FORCE_STATUS_ACCEPTED         = 2;
const int FORCE_STATUS_REFUSED          = 3;
const int FORCE_STATUS_MODIFIED         = 4;

// Supplement and Adaptation status
//
// DB Status in Record (Field => DbStatus)
const int TRAINSUPPLEMENTVIEW_DB_UNMODIFIED  = 0;
const int TRAINSUPPLEMENTVIEW_DB_INSERTED  = 1;
const int TRAINSUPPLEMENTVIEW_DB_DELETED  = 2;
const int TRAINSUPPLEMENTVIEW_DB_MODIFIEDNOTUPDATED  = 4;
const int TRAINSUPPLEMENTVIEW_DB_UPDATED  = 8;

// Status updated in sc_adaptation.status
const int TRAIN_NON_SUPPLEMENTAIRE = 0;

const int TRAIN_SUPPLEMENT_UNDEFINED = -1;
const int TRAIN_SUPPLEMENT_UNDETERMINED = 0;
const int TRAIN_ADAPTATION = 1;
const int TRAIN_SUPPLEMENT_ADAPTED = 2;
const int TRAIN_DELETED_ADAPTED = 3;
const int TRAIN_SUPPLEMENT = 4;
const int TRAIN_DELETED = 5;
const int TRAIN_SUPPLEMENT_BASEPLAN = 6;
const int TRAIN_DELETED_UNDETERMINED = 7;
const int TRAIN_DELETED_BASEPLAN = 8;


// Positions for menu items inserted during Client Parameterization Configuration
const int SYSTEM_MENU_ITEM_OFFSET = 0;  // begin position for optional menu items - System menu
const int HELP_MENU_ITEM_OFFSET = 1;  // begin position for optional menu items - System menu
const int WINDOW_MENU_LEG_ITEM_OFFSET = 6;  // begin position for optional menu items - Leg List Alt Window menu
const int WINDOW_MENU_LEG_ALT_ITEM_OFFSET = 7;  // begin position for optional menu items - Leg List Alt Window menu
const int WINDOW_MENU_BKT_HIST_ITEM_OFFSET = 8;  // begin position for optional menu items - Bkt Hist Window menu
const int WINDOW_MENU_ITEM_OFFSET = 16;  // begin position for optional menu items - Window menu
const int TRANCHE_CONTEXT_MENU_ITEM_OFFSET = 4;  // begin position for optional menu items - tranche context menu
const int LEG_MENU_SEND_RESA_ITEM_OFFSET = 0;  // begin position for optional menu item - Leg List menu
const int LEG_MENU_RESTORE_MODEL_AUTH_OFFSET = 1;  // begin position for optional menu item - Leg List menu
const int LEG_MENU_AUTO_COPY_AUTH_OFFSET = 5;  // begin position for optional menu item - Leg List menu
const int LEG_MENU_TUNNEL_ORDER_ITEM_OFFSET = 7;  // begin position for optional menu item - Leg List menu
const int RELTRAN_TRNJOIN_MENU_ITEM_OFFSET = 1;  // begin position for optional menu item - RelTran menu
const int RELTRAN_TRANCHE_MENU_ITEM_OFFSET = 5;  // begin position for optional menu item - ReltTan menu
const int RELTRAN_SUPPLVL_MENU_ITEM_OFFSET = 8;  // begin position for optional menu item - RelTran menu
const int RRDHIST_REVWIN_MENU_ITEM_OFFSET = 14;  // begin position for optional menu item - RelTran menu

#define COLOR_PANEDATE1		RGB (100, 100, 255) // bleu gris   au lieu de    RGB(255, 128, 128) // rose
#define COLOR_PANEDATE2		RGB (0, 191, 128)   // vert bleuté   au lieu de    RGB(0,128, 0)	// vert moyen
#define COLOR_PANEDATE3		RGB(128,0,255)	  // violet 'flashi'
#define COLOR_RECETTE		RGB (128, 0, 0)	// marron pour affichage des recettes
#define COLOR_PREVISION		RGB (50, 200, 0) // vert moyen pour affichage des données de prévision
#define COLOR_RECTICKETE	RGB (255, 128, 0) // orange pour les revenu tickete
#define COLOR_OVB_MTRANCHE  RGB (80, 212, 80) // vert clair mais pas trop lumineux

#endif
