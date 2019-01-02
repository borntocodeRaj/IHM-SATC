// ResaRailView.cpp : implementation file
//
#include "StdAfx.h"


#include "CTAChartHelpDlg.h"
#include "ResTracer.h"
#include "TrancheView.h"
#include "ResaRailView.h"
#include "OthLegsView.h"
#include "Optimization.h"
#include "OptimizationSNCF.h"
#include "RRDHistView.h"
#include "RecDHistView.h"
#include "BktHistView.h"
#include "BktSciSphere.h"
#include "SpheresPrix.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// DM7978 - LME - YI5401
#define OFFSETX						340
#define OFFSETY						35
#define OFFSETY_SCI				10
// DM7978 - LME - FIN
#define OFFSET_UNDERLINE  2
#define STDV_FACTOR_RATE  0.59
#define ZERO_EPSILON      0.000001


/////////////////////////////////////////////////////////////////////////////
// CResaCmptBktLine

CResaCmptBktLine::CResaCmptBktLine( CWnd* pParentWnd, YmIcResaBktDom* pResaBkt, int nID )
{
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	// initially, we fill the member variables
	//   with the data from the domain record
	//
	m_pResaBkt = pResaBkt;  // hold the domain record

	// initialize the member variables
	double dAddDmdFcNew = ((CResaRailView*)pParentWnd)->GetAddDmdFcNew( pResaBkt );
	m_lAddDmdNew = ( dAddDmdFcNew >= 0) ? (long)(dAddDmdFcNew+0.5) : (long)(dAddDmdFcNew-0.5);
	m_lAuthNew = pResaBkt->AuthUser();
	m_lAuthPrev = pResaBkt->AuthUser();
	m_lAuthInit = pResaBkt->AuthUser();
	m_lAvailCur = 0;
	m_lAvailUser = 0;
	m_lAvailFcOrg = 0;
	m_lAuthNomos = 0;
	m_lAvailNomos = 0;
	m_lRecette = pResaBkt->Recette();

	SetAuthModify( FALSE );
	SetAuthRevMixFlag( FALSE );
	SetDmdRevMixFlag( FALSE );

	CString s;
	CRect rect(0, 0, 0, 0);
	// create the special edit windows
	m_pEditAuthNew = NULL;

	m_pEditAuthNew = new CResaRailViewEdit();
	// recuperation de la tranche current
	YmIcTrancheLisDom* pTranche = 
		YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);

	if ( (((pTranche->Nature() == 1) || (pTranche->Nature() == 2) ||(pTranche->Nature() == 3) ||
		(pTranche->Nature() == 4) || (pTranche->Nature() == 5))
		&& (pResaBkt->NestLevel() == 0)))
	{
		m_pEditAuthNew->Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_RIGHT|ES_MULTILINE|ES_READONLY, rect, pParentWnd, nID );
	}
	else if (((CCTAApp*)APP)->GetRhealysFlag())
	{
		if (m_pResaBkt->NestLevel())
			m_pEditAuthNew->Create( WS_CHILD|WS_TABSTOP|ES_RIGHT|ES_MULTILINE|ES_READONLY, rect, pParentWnd, nID );
		else
			m_pEditAuthNew->Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_RIGHT|ES_MULTILINE|ES_READONLY, rect, pParentWnd, nID );
	}
	else
	{
		RWDate today;
		if (pTranche->DptDate() < today.julian())
			m_pEditAuthNew->Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_RIGHT|ES_MULTILINE|ES_READONLY, rect, pParentWnd, nID );
		else
			m_pEditAuthNew->Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_RIGHT|ES_MULTILINE, rect, pParentWnd, nID );
	}

	RWDate today;
	((CResaRailView*)pParentWnd)->m_FlagReadOnly = FALSE;
	if ((pTranche->Nature() == 3) || (pTranche->DptDate() < today.julian()) )
		((CResaRailView*)pParentWnd)->m_FlagReadOnly = TRUE;

	m_pEditAuthNew->SetFont( pDoc->GetStdFont(NULL) );
	s.Format( "%d", m_lAuthNew );
	m_pEditAuthNew->SetWindowText( s );
	m_pEditAddDmdNew = NULL;
}

CResaCmptBktLine::~CResaCmptBktLine()
{
	if (m_pEditAuthNew)
	{
		delete m_pEditAuthNew;
		m_pEditAuthNew = NULL;
	}
	if (m_pEditAddDmdNew)
	{
		delete m_pEditAddDmdNew;
		m_pEditAddDmdNew = NULL;
	}
}

// Donne la recette temps réel + estimation des prix garantis.
// venteVNLbkt est le nombre de vente dans la bucket, lu dans le VNL. mettre 0 si VNL indisponible
// dans ce cas la fonction renvoi le revenu du temps reel (REV_REAL_AUTHORIZATION) + revenu prix garanti du RRD
int CResaCmptBktLine::GetRecetteTempsReel(CWnd* pParentWnd, int venteVNLbkt)
{

	BOOL bPrixGaranti = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag();

	if (!bPrixGaranti)
		return (m_pResaBkt->Recette() / 100);

	if (! venteVNLbkt) // pas de VNL temps réel, ou zéro ventes dans la bucket. On prend recette REAL + prix garanti
	{
		if (!((CCTAApp*)APP)->GetRealTimeMode())
		{ // temp réel inactif : On prend recette REAL + prix garanti
			long rec = m_pResaBkt->Recette();
			if (m_pResaBkt->NestLevel() == 0)
				rec += ((CResaRailView*)pParentWnd)->GetSciRecette (m_pResaBkt->Cmpt()[0], -1);
			rec += m_pResaBkt->RrdPrixGaranti();
			return (rec) / 100;
		}
		else // Pas de ventes dans la bucket , on renvoi 0
			return 0;
	}

	long pgtr = 0;
	double cdbl = 0;
	if (!m_pResaBkt->RrdPrixGaranti())
	{ // Pas de prix garanti déja calculé
		// On calcul uniquement a partir de REV_REAL et du VNL.
		// Simple règle de 3 entre le revenu et nb-voyageur temps réel et le nombre de voyageur du VNL
		int nbv = m_pResaBkt->RealNbVoyageurs();
		if (m_pResaBkt->NestLevel() == 0) // Nest Level 0, il faut ajouter les nb_voyageurs SCI au nb_voyageurs bkt0 simple
			nbv += ((CResaRailView*)pParentWnd)->GetSciNbVoyageurs (m_pResaBkt->Cmpt()[0], -1);
		if (nbv)
		{
			cdbl = m_pResaBkt->Recette();
			if (m_pResaBkt->NestLevel() == 0)
			{ // Nest level 0, il faut ajouter les recette des sous-contingent, car les ventes VNL inclu les sous-contingent
				cdbl += ((CResaRailView*)pParentWnd)->GetSciRecette (m_pResaBkt->Cmpt()[0], -1);
			}
			cdbl *= venteVNLbkt;
			cdbl = cdbl / nbv;
			cdbl = cdbl / 100;
			pgtr = static_cast<long>(cdbl);
		}
		else // On renvoi la recette telle quelle
		{
			pgtr = m_pResaBkt->Recette();
			if (m_pResaBkt->NestLevel() == 0)
				pgtr += ((CResaRailView*)pParentWnd)->GetSciRecette (m_pResaBkt->Cmpt()[0], -1);
			pgtr = pgtr / 100;
		}
		return pgtr;
	}

	int div = m_pResaBkt->ResHoldGrp() + m_pResaBkt->ResHoldIdv() - m_pResaBkt->RrdNbVoyageurs(); 
	// RrdNbVoyageurs inclu les SCX dans la BKT0, vient de YM_TDLPJNG
	if ( div == 0)
	{ // Aucune vente à prix garanti au reading day précédent, ou annulation égale au nombre de prix garanti
		// Dans le doute, on ignore les données revenu au reading day (ne sachant comment les prendre en compte)
		// eet on fait la simple règle de trois entre données temps réel et VNL
		if (m_pResaBkt->RealNbVoyageurs())
		{
			cdbl = m_pResaBkt->Recette() + m_pResaBkt->RrdPrixGaranti();
			if (m_pResaBkt->NestLevel() == 0) // bkt 0, ajout recette scx, mais pas prix garanti scx car deja inclu dans prix garanti bkt0
				cdbl += ((CResaRailView*)pParentWnd)->GetSciRecette (m_pResaBkt->Cmpt()[0], -1);
			cdbl *= venteVNLbkt;
			int nbv = m_pResaBkt->RealNbVoyageurs();
			if (m_pResaBkt->NestLevel() == 0) // Nest Level 0, il faut ajouter les nb_voyageurs SCI au nb_voyageurs bkt0 simple
				nbv += ((CResaRailView*)pParentWnd)->GetSciNbVoyageurs (m_pResaBkt->Cmpt()[0], -1);
			cdbl = cdbl / nbv;
			cdbl = cdbl / 100;
			pgtr = static_cast<long>(cdbl);
		}
		else // On renvoi la recette telle quelle
		{
			pgtr = m_pResaBkt->Recette() + m_pResaBkt->RrdPrixGaranti();
			if (m_pResaBkt->NestLevel() == 0)
				pgtr += ((CResaRailView*)pParentWnd)->GetSciRecette (m_pResaBkt->Cmpt()[0], -1);
			pgtr = pgtr / 100;
		}
		return pgtr;
	}

	long rec = m_pResaBkt->Recette();
	if (m_pResaBkt->NestLevel() == 0) // Nest Level 0, il faut ajouter les recette SCI 
		rec += ((CResaRailView*)pParentWnd)->GetSciRecette (m_pResaBkt->Cmpt()[0], -1);
	cdbl = m_pResaBkt->RrdPrixGaranti();
	cdbl /= div;
	//cdbl = abs (cdbl);
	int nbv = m_pResaBkt->RealNbVoyageurs();
	if (m_pResaBkt->NestLevel() == 0) // Nest Level 0, il faut ajouter les nb_voyageurs SCI
		nbv += ((CResaRailView*)pParentWnd)->GetSciNbVoyageurs (m_pResaBkt->Cmpt()[0], -1);
	cdbl *=  (venteVNLbkt - nbv) ;

	cdbl += rec;
	cdbl = cdbl / 100;
	if (cdbl < 0)
		rec = 0;
	else
		rec = static_cast<long>(cdbl);
	return rec;
}

int CResaCmptBktLine::GetPanierMoyen(CWnd* pParentWnd, int venteVNLbkt)
{
	//CString osMessage;
	//osMessage.Format("venteVNLbkt : %ld", venteVNLbkt);
	//AfxMessageBox(osMessage);
	double dwrec = GetRecetteTempsReel(pParentWnd, venteVNLbkt);
	//osMessage.Format("GetRecetteTempsReel : %ld", dwrec);
	//AfxMessageBox(osMessage);
	if (m_pResaBkt->NestLevel() == 0)
	{// bucket 0, il faut enlever le revenu du SCG
		int revscg = ((CResaRailView*)pParentWnd)->GetRealTimeSciRevenu (m_pResaBkt->Cmpt()[0], -1);
		if (revscg > 0)
			dwrec -= revscg;
		//osMessage.Format("GetRecetteTempsReel Bucket 0 : %ld", dwrec);
		//AfxMessageBox(osMessage);
	}
	double dpanier = 0;
	int panier = 0;
	double nbvoyag = venteVNLbkt;
	if (!nbvoyag)
		nbvoyag = __max( m_pResaBkt->RealNbVoyageurs(), m_pResaBkt->ResHoldGrp() + m_pResaBkt->ResHoldIdv() );
	//osMessage.Format("m_pResaBkt->RealNbVoyageurs() : %ld\r\nm_pResaBkt->ResHoldGrp() : %ld\r\nm_pResaBkt->ResHoldIdv() : %ld\r\nnbvoyag : %ld", 
	//	m_pResaBkt->RealNbVoyageurs(), m_pResaBkt->ResHoldGrp(), m_pResaBkt->ResHoldIdv(), nbvoyag);
	//AfxMessageBox(osMessage);
	if (m_pResaBkt->NestLevel() == 0)
	{// bucket 0, il faut enlever les resa du SCG
		int voyscg = ((CResaRailView*)pParentWnd)->GetRealTimeSciResa (m_pResaBkt->Cmpt()[0], -1);
		if (voyscg > 0)
			nbvoyag -= voyscg;
	}
	if (nbvoyag > 0)
	{
		dpanier = dwrec / nbvoyag;
		//panier = static_cast<int>(dpanier);
		panier = (int)dpanier;
		//osMessage.Format("dpanier : %ld\r\npanier : %ld", dpanier, panier);
		//AfxMessageBox(osMessage);
		if (dpanier - panier > 0.5)
			panier += 1;
	}
	//osMessage.Format("panier : %ld", panier);
	//AfxMessageBox(osMessage);
	return panier;
}

/////////////////////////////////////////////////////////////////////////////
// CResaCmptBkt

CResaCmptBkt::CResaCmptBkt()
{
	m_pResaCmpt = NULL;
	m_lResHoldTot = 0;
	m_bTGVJumPctChanged = FALSE;
	m_lAddDmdNewTot = 0;


}

CResaCmptBkt::~CResaCmptBkt()
{
	for( int iBktIdx=0; iBktIdx < m_CmptBktLineArray.GetSize(); iBktIdx++ )
	{
		delete m_CmptBktLineArray[ iBktIdx ];
		m_CmptBktLineArray[ iBktIdx ] = NULL;
	}
}

void CResaCmptBkt::AccumAddDmd()
{
	// Accumulate the additional demand values at the bucket level 
	//   for this compartment and set the total in the static window
	m_lAddDmdNewTot = 0;
	for( int iBktIdx=0; iBktIdx < m_CmptBktLineArray.GetSize(); iBktIdx++ )
		m_lAddDmdNewTot += m_CmptBktLineArray[ iBktIdx ]->m_lAddDmdNew; 
}

/////////////////////////////////////////////////////////////////////////////
// CCmptBkpMap

void CResaCmptBktMap::DeleteAll()
{
	POSITION pos = GetStartPosition();
	while( pos != NULL )
	{
		CString   sKey;
		CResaCmptBkt* pCmptBkt;

		GetNextAssoc( pos, sKey, pCmptBkt );
		delete pCmptBkt;
	}
}

CResaCmptBktLine* CResaCmptBktMap::AddResaBkt( CWnd* pParentWnd, YmIcResaBktDom* pResaBkt, int nID )
{
	CRect rect(0, 0, 0, 0);
	CString sKey( pResaBkt->Cmpt() );
	CString s;
	CResaCmptBkt* pCmptBkt;
	if( !Lookup( sKey, pCmptBkt ) )
	{
		pCmptBkt = new CResaCmptBkt();
		SetAt( sKey, pCmptBkt );
		m_aszKey.Add( sKey );
	}

	CResaCmptBktLine* pCmptBktLine = new CResaCmptBktLine( pParentWnd, pResaBkt, nID );
	pCmptBkt->m_CmptBktLineArray.Add( pCmptBktLine );
	pCmptBkt->m_lResHoldTot += ( pResaBkt->ResHoldIdv() + pResaBkt->ResHoldGrp() + pResaBkt->ResHoldNotYield() );

	// Ensure we have not added too many fields for the view
	if( nID > ID_ES_BASE_BKT + ID_ES_MAX_BKT )
	{
		TRACE( "Too many bucket level edit fields for ResaRailView!\n" );
		return pCmptBktLine;
	}

	// Insert an element in the map for the authorizations
	((CResaRailView*)pParentWnd)->GetEditMapArray()->SetAt( nID, pCmptBktLine );

	return pCmptBktLine;
}

CResaCmptBkt* CResaCmptBktMap::AddResaCmpt( YmIcResaCmptDom* pResaCmpt )
{
	CString sKey( pResaCmpt->Cmpt() );
	CResaCmptBkt* pCmptBkt;
	if( !Lookup( sKey, pCmptBkt ) )
	{
		pCmptBkt = new CResaCmptBkt();
		SetAt( sKey, pCmptBkt );
	}
	pCmptBkt->m_pResaCmpt = pResaCmpt;
	return pCmptBkt;
}

void CResaCmptBktMap::RemoveAllResaBkt()
{
	POSITION pos = GetStartPosition();
	while( pos != NULL )
	{
		CString   sKey;
		CResaCmptBkt* pCmptBkt;

		GetNextAssoc( pos, sKey, pCmptBkt );
		for( int iBktIdx=0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
		{
			delete pCmptBkt->m_CmptBktLineArray[ iBktIdx ];
			pCmptBkt->m_CmptBktLineArray[ iBktIdx ] = NULL;
		}
		pCmptBkt->m_CmptBktLineArray.RemoveAll();
		pCmptBkt->m_lResHoldTot = 0;
		if( pCmptBkt->m_pResaCmpt == NULL )
		{
			delete pCmptBkt;
			pCmptBkt = NULL;
			RemoveKey( sKey );
		}
	}
	m_aszKey.RemoveAll();  // remove all elements from the array which contains the cmpt order
}

void CResaCmptBktMap::RemoveAllResaCmpt()
{
	POSITION pos = GetStartPosition();
	while( pos != NULL )
	{
		CString   sKey;
		CResaCmptBkt* pCmptBkt;

		GetNextAssoc( pos, sKey, pCmptBkt );
		pCmptBkt->m_pResaCmpt = NULL;
		if( pCmptBkt->m_CmptBktLineArray.GetSize() == 0 )
		{
			delete pCmptBkt;
			pCmptBkt = NULL;
			RemoveKey( sKey );
		}
	}
}

void CResaCmptBktMap::AccumAddDmd()
{
	// iterate through the elements (buckets) in the map
	//   and perform an accumulation on additional demand
	POSITION pos = GetStartPosition();
	while( pos != NULL )
	{
		CString   sKey;
		CResaCmptBkt* pCmptBkt;

		GetNextAssoc( pos, sKey, pCmptBkt );
		pCmptBkt->AccumAddDmd();
	}
}

// structure SCI
/////////////////////////////////////////////////////////////////////////////
// CResaCmptBktSciLine

CResaCmptBktSciLine::CResaCmptBktSciLine( CWnd* pParentWnd, YmIcResaBktDom* pResaBkt, int nID )
{
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	// initially, we fill the member variables
	//   with the data from the domain record
	//
	m_pResaBkt = pResaBkt;  // hold the domain record

	// initialize the member variables
	m_lAuthNew = pResaBkt->AuthUser();
	m_lAuthPrev = pResaBkt->AuthUser();
	m_lAuthInit = pResaBkt->AuthUser();
	m_lAvailCur = 0;
	m_lAvailUser = 0;
	m_lRecette = pResaBkt->Recette();

	SetAuthModify( FALSE );

	CString s;
	CRect rect(0, 0, 0, 0);
	// create the special edit windows
	m_pEditAuthNew = NULL;
	BOOL bCreateIt = TRUE;  // create the auth edit window by default

	if ( bCreateIt )
	{
		m_pEditAuthNew = new CResaRailViewEdit(true);
		// recuperation de la tranche current
		YmIcTrancheLisDom* pTranche = 
			YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
		RWDate today;
		// NBN - ANO 89416 - Interdire la saisie de la SC1
		if ((pTranche->Nature() == 3) || pTranche->DptDate() < today.julian())
			m_pEditAuthNew->Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_RIGHT|ES_MULTILINE|ES_READONLY, rect, pParentWnd, nID );
		else
			m_pEditAuthNew->Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_RIGHT|ES_MULTILINE, rect, pParentWnd, nID );

		((CResaRailView*)pParentWnd)->m_FlagReadOnly = FALSE;

	    //if ((pTranche->Nature() == 3) || (pTranche->DptDate() < today.julian()) ) // NBN ANO 89569
		if ((pTranche->DptDate() < today.julian()) )
			((CResaRailView*)pParentWnd)->m_FlagReadOnly = TRUE;

		m_pEditAuthNew->SetFont( pDoc->GetStdFont(NULL) );
		s.Format( "%d", m_lAuthNew );
		m_pEditAuthNew->SetWindowText( s );
	}
}

CResaCmptBktSciLine::~CResaCmptBktSciLine()
{
	if (m_pEditAuthNew)
	{
		delete m_pEditAuthNew;
		m_pEditAuthNew = NULL;
	}
}

// Donne la recette temps réel + estimation des prix garantis.
// venteVNLbkt est le nombre de vente dans la bucket, lu dans le VNL. mettre 0 si VNL indisponible
// dans ce cas la fonction renvoi le revenu du temps reel (REV_REAL_AUTHORIZATION) + revenu prix garanti du RRD
int CResaCmptBktSciLine::GetRecetteTempsReel(int venteVNLbkt)
{
	BOOL bPrixGaranti = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetAvecPrixGarantiFlag();
	if (!bPrixGaranti)
		return (m_pResaBkt->Recette() / 100);

	if (! venteVNLbkt) // pas de VNL temps réel, ou zéro ventes dans la bucket. On prend recette REAL + prix garanti
	{
		if (!((CCTAApp*)APP)->GetRealTimeMode())
		{ // temp réel inactif : On prend recette REAL + prix garanti
			long rec = m_pResaBkt->Recette();
			rec += m_pResaBkt->RrdPrixGaranti();
			return (rec) / 100;
		}
		else // Pas de ventes dans la bucket , on renvoi 0
			return 0;
	}

	long pgtr = 0;
	double cdbl = 0;
	if (!m_pResaBkt->RrdPrixGaranti())
	{ // Pas de prix garanti déja calculé
		// On calcul uniquement a partir de REV_REAL et du VNL.
		// Simple règle de 3 entre le revenu et nb-voyageur temps réel et le nombre de voyageur du VNL
		if (m_pResaBkt->RealNbVoyageurs())
		{
			cdbl = m_pResaBkt->Recette();
			cdbl *= venteVNLbkt;
			cdbl = cdbl / m_pResaBkt->RealNbVoyageurs();
			cdbl = cdbl / 100;
			pgtr = static_cast<long>(cdbl);
		}
		else // On renvoi la recette telle quelle
		{ 
			pgtr = m_pResaBkt->Recette();
			pgtr = pgtr / 100;
		}
		return pgtr;
	}

	int div = m_pResaBkt->ResHoldGrp() + m_pResaBkt->ResHoldIdv() - m_pResaBkt->RrdNbVoyageurs();
	if ( div == 0)
	{ // Aucune vente à prix garanti au reading day précédent, ou annulation égale au nombre de prix garanti
		// Dans le doute, on ignore les données revenu au reading day (ne sachant comment les prendre en compte)
		// eet on fait la simple règle de trois entre données temps réel et VNL
		if (m_pResaBkt->RealNbVoyageurs())
		{
			cdbl = m_pResaBkt->Recette() + m_pResaBkt->RrdPrixGaranti();
			cdbl *= venteVNLbkt;
			cdbl = cdbl / m_pResaBkt->RealNbVoyageurs();
			cdbl = cdbl / 100;
			pgtr = static_cast<long>(cdbl);
		}
		else // On renvoi la recette telle quelle
		{ 
			pgtr = m_pResaBkt->Recette() + m_pResaBkt->RrdPrixGaranti();
			pgtr = pgtr / 100;
		}
		return pgtr;
	}

	long rec = m_pResaBkt->Recette();
	//ANCIEN CODE 
	//cdbl = m_pResaBkt->RrdPrixGaranti() * (venteVNLbkt - m_pResaBkt->RealNbVoyageurs());
	//cdbl = cdbl / div;
	// Nouveau code selon proposition Cécile du 6/7/7.
	// On veut cdbl = (venteVNLbkt - m_pResaBkt->RealNbVoyageurs()) * abs (m_pResaBkt->RrdPrixGaranti()  / div);
	// Mais on calcul en revenant toujours dans un double pour éviter les erreurs d'arrondi
	cdbl = m_pResaBkt->RrdPrixGaranti();
	cdbl /= div;
	cdbl *=  (venteVNLbkt - m_pResaBkt->RealNbVoyageurs()) ;
	cdbl = abs (cdbl);

	cdbl += rec;
	cdbl = cdbl / 100;
	rec = static_cast<long>(cdbl);
	return rec;
}

int CResaCmptBktSciLine::GetPanierMoyen(int venteVNLbkt)
{
	double dwrec = GetRecetteTempsReel(venteVNLbkt);
	double dpanier = 0;
	int panier = 0;
	double nbvoyag = venteVNLbkt;
	if (!nbvoyag)
		nbvoyag = __max( m_pResaBkt->RealNbVoyageurs(), m_pResaBkt->ResHoldGrp() + m_pResaBkt->ResHoldIdv() );
	if (nbvoyag)
	{
		dpanier = dwrec / nbvoyag;
		panier = static_cast<int>(dpanier);
		if (dpanier - panier > 0.5)
			panier += 1;
	}
	return panier;
}

/////////////////////////////////////////////////////////////////////////////
// CResaCmptBktSci

CResaCmptBktSci::CResaCmptBktSci(CWnd* pParentWnd, YmIcResaCmptDom* pResaCmpt, int nID)
{
	m_pResaCmpt = NULL;
	m_lResHoldTot = 0;
	CRect rect(0, 0, 0, 0);
	CString s;
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	m_pEditScgCapa = new CResaRailViewEdit(true);
	// recuperation de la tranche current
	YmIcTrancheLisDom* pTranche = 
		YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	RWDate today;
	if (pTranche->DptDate() < today.julian())
		m_pEditScgCapa->Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_RIGHT|ES_MULTILINE|ES_READONLY, rect, pParentWnd, nID );
	else
		m_pEditScgCapa->Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_RIGHT|ES_MULTILINE, rect, pParentWnd, nID );

	((CResaRailView*)pParentWnd)->m_FlagReadOnly = FALSE;
	if ((pTranche->DptDate() < today.julian()) )
		((CResaRailView*)pParentWnd)->m_FlagReadOnly = TRUE;

	m_pEditScgCapa->SetFont( pDoc->GetStdFont(NULL) );
	s.Format( "%d", pResaCmpt->Capacity() );
	m_pEditScgCapa->SetWindowText( s );
	m_lAuthNew = pResaCmpt->Capacity();
	m_lAuthPrev = pResaCmpt->Capacity();
	m_bAuthModified = FALSE;
}

CResaCmptBktSci::~CResaCmptBktSci()
{
	for( int iBktIdx=0; iBktIdx < m_CmptBktLineArray.GetSize(); iBktIdx++ )
	{
		delete m_CmptBktLineArray[ iBktIdx ];
		m_CmptBktLineArray[ iBktIdx ] = NULL;
	}
	delete m_pEditScgCapa;
}


/////////////////////////////////////////////////////////////////////////////
// CResaCmptBkpSciMap

void CResaCmptBktSciMap::DeleteAll()
{
	POSITION pos = GetStartPosition();
	while( pos != NULL )
	{
		CString   sKey;
		CResaCmptBktSci* pCmptBkt;

		GetNextAssoc( pos, sKey, pCmptBkt );
		delete pCmptBkt;
	}
}

CResaCmptBktSciLine* CResaCmptBktSciMap::AddResaBkt( CWnd* pParentWnd, YmIcResaBktDom* pResaBkt, int nID )
{
	CRect rect(0, 0, 0, 0);
	CString sKey( pResaBkt->Cmpt() );
	CString s;
	CResaCmptBktSci* pCmptBkt;
	if( !Lookup( sKey, pCmptBkt ) )
		return NULL; // le niveau cmpt DOIT avoir été créé avant.

	CResaCmptBktSciLine* pCmptBktLine = new CResaCmptBktSciLine( pParentWnd, pResaBkt, nID );
	pCmptBkt->m_CmptBktLineArray.Add( pCmptBktLine );
	pCmptBkt->m_lResHoldTot += ( pResaBkt->ResHoldIdv() + pResaBkt->ResHoldGrp() + pResaBkt->ResHoldNotYield() );

	// Ensure we have not added too many fields for the view
	if( nID > ID_ES_BASE_BKT + ID_ES_MAX_BKT )
	{
		TRACE( "Too many bucket level edit fields for ResaRailView!\n" );
		return pCmptBktLine;
	}

	// Insert an element in the map for the authorizations
	((CResaRailView*)pParentWnd)->GetEditSciMapArray()->SetAt( nID, pCmptBktLine );

	return pCmptBktLine;
}

CResaCmptBktSci* CResaCmptBktSciMap::AddResaCmpt( CWnd* pParentWnd, YmIcResaCmptDom* pResaCmpt, int nID )
{
	CString sKey( pResaCmpt->Cmpt() );
	CResaCmptBktSci* pCmptBkt;
	if( !Lookup( sKey, pCmptBkt ) )
	{
		pCmptBkt = new CResaCmptBktSci(pParentWnd, pResaCmpt, nID);
		SetAt( sKey, pCmptBkt );
		m_aszKey.Add( sKey );
	}
	pCmptBkt->m_pResaCmpt = pResaCmpt;
	return pCmptBkt;
}

void CResaCmptBktSciMap::RemoveAllResaBkt()
{
	POSITION pos = GetStartPosition();
	while( pos != NULL )
	{
		CString   sKey;
		CResaCmptBktSci* pCmptBkt;

		GetNextAssoc( pos, sKey, pCmptBkt );
		for( int iBktIdx=0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
		{
			delete pCmptBkt->m_CmptBktLineArray[ iBktIdx ];
			pCmptBkt->m_CmptBktLineArray[ iBktIdx ] = NULL;
		}
		pCmptBkt->m_CmptBktLineArray.RemoveAll();
		pCmptBkt->m_lResHoldTot = 0;
		if( pCmptBkt->m_pResaCmpt == NULL )
		{
			delete pCmptBkt;
			pCmptBkt = NULL;
			RemoveKey( sKey );
		}
	}
	m_aszKey.RemoveAll();  // remove all elements from the array which contains the cmpt order
}

void CResaCmptBktSciMap::RemoveAllResaCmpt()
{
	POSITION pos = GetStartPosition();
	while( pos != NULL )
	{
		CString   sKey;
		CResaCmptBktSci* pCmptBkt;

		GetNextAssoc( pos, sKey, pCmptBkt );
		pCmptBkt->m_pResaCmpt = NULL;
		if( pCmptBkt->m_CmptBktLineArray.GetSize() == 0 )
		{
			delete pCmptBkt;
			pCmptBkt = NULL;
			RemoveKey( sKey );
		}
	}
}

// fin structure SCI

/////////////////////////////////////////////////////////////////////////////
// CResaRailView

IMPLEMENT_DYNCREATE(CResaRailView, CScrollView)

CResaRailView::CResaRailView()
{
	m_pWndChildFocusSaved = NULL;
	m_bShiftKeyDown = FALSE;
	m_pVnl = NULL;
	m_pVnlLeg = NULL;
	m_iMsgId = 0;
	m_bVnlResent = FALSE;
	m_bIsValid = FALSE;
	m_bModificationCancelled = FALSE;
	m_bOkOnRhea = TRUE;
	m_bSciHeight = FALSE;
	m_bPrevisionOK = FALSE;
	m_bVnauExcluOK = FALSE;
	m_SurResaGlob = 200;
	m_capatotale = 0;
	m_lYSciNotDisplayed = 0L;
	m_bInitialUpdate = FALSE;
	m_ascTarifaire = 0;
	memset (m_SurResa, '\0', sizeof (m_SurResa));
	memset (m_SousResa, '\0', sizeof (m_SousResa));
	//TSE_Ano 79307 - BEGIN
	m_WinReduit = FALSE;//Variable pour vérifier si la fenêtre Résarail est gloale ou réduite
	m_ChangedWindowGlobal = TRUE;
	m_ChangedWindowReduit = TRUE;
	
	rectUpGlob.right = 0;
	rectUpGlob.left = 0;
	rectUpGlob.top = 0;
	rectUpGlob.bottom = 0;
	
	rectDoGlob.right = 0;//Réctangle Global CC
	rectDoGlob.left = 0;
	rectDoGlob.top = 0;
	rectDoGlob.bottom = 0;
	
	rectUpRed.right = 0;
	rectUpRed.left = 0;
	rectUpRed.top = 0;
	rectUpRed.bottom = 0;

	rectDoRed.right = 0;
	rectDoRed.left = 0;
	rectDoRed.top = 0;
	rectDoRed.bottom = 0;
	//TSE_Ano 79307 - END

}

CResaRailView::~CResaRailView()
{
	m_CmptBktMap.DeleteAll();
	m_CmptBktSciMap.DeleteAll();
	m_EditMap.RemoveAll();
	m_EditSciMap.RemoveAll();

	delete m_pVnl;
	m_pVnl = NULL;
	m_pVnlLeg = NULL;
	m_LegScg.RemoveAll();
}

CLegView* CResaRailView::GetLegView ()
{
	// Renvoie la LegView Alt ou non en fonction de GetAltFlag
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	CMainFrame* pMainFrame = ((CMainFrame*) ((CCTAApp*)APP)->GetMainWnd());
	POSITION pos = pDoc->GetFirstViewPosition();
	while( pos != NULL ) 
	{
		CView* pView = pDoc->GetNextView( pos );
		if (pMainFrame->IsLegView (pView))
			return (CLegView*)pView;
	}
	return NULL;
}

int CResaRailView::ChangeSentFlag (int change, int curval)
{ // DM4802, gestion du sentFlag
	// Mise à jour de la composante SATC du SentFlag, change vaut 0 ou 1.
	if (change == 1)
		curval = curval | 1;
	else if (curval % 2 == 1)
		curval -= 1;
	return curval;
}

// Iterate throught the Vnl object to find the given leg
//   and set the member variable which holds the leg level object
//
void CResaRailView::SetVnlLeg(CString szLegOrig, CString szLegDest)
{
	if ( m_pVnl == NULL )
		return;

	YM_VnlLeg* pVnlLegRet = NULL;  // this is our pointer we use to set the member variable
	YM_VnlLeg* pVnlLeg = NULL;  // this is our working pointer

	YM_VnlLegIterator* LegIterator = m_pVnl->CreateVnlLegIterator();
	LegIterator->First();

	for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
	{
		pVnlLeg = LegIterator->Current();
		if ( ( pVnlLeg->GetLegOrig() == szLegOrig ) && ( pVnlLeg->GetLegDest() == szLegDest ) )
		{
			pVnlLegRet = pVnlLeg;  // let's return this one
			break;  // and get out of this loop
		}
	}
	// clean up and return the right thing
	delete LegIterator;
	m_pVnlLeg = pVnlLegRet;
}

void CResaRailView::InitLegScg()
{
	if (m_LegScg.GetCount())
		return;  
	YM_VnlLeg* pVnlLeg = NULL;  
	YM_VnlCmpt* pVnlCmpt = NULL;
	CString skey;
	YM_VnlLegIterator* LegIterator = m_pVnl->CreateVnlLegIterator();
	LegIterator->First();

	for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
	{
		pVnlLeg = LegIterator->Current();
		YM_VnlCmptIterator* CmptIterator = pVnlLeg->CreateVnlCmptIterator();
		for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
		{
			pVnlCmpt = CmptIterator->Current();
			skey.Format("%s%s%s", pVnlLeg->GetLegOrig(), pVnlLeg->GetLegDest(), pVnlCmpt->GetCmptId());
			m_LegScg.SetAt(skey, (CObject*)(pVnlCmpt->GetScgCapa()));
		}
		delete CmptIterator;
	}
	delete LegIterator;
}

// verifie que les VNAS ne rentre pas dans un deux cas de figure suivants
// mise à 0 d'un SCG alors que les autres tronçons ont un SCG, et l'inverse.
// Le test se fait par rapport à la table m_LegScg.
void CResaRailView::CtrlScgVide(CString sVnas1, CString sVnas2)
{
	CString sl; // OD des VNAS
	CString sv; // chaine des autorisations SCI sur l'ensemble des espaces physiques
	CString sc; // un compartiment
	CString sg; // La chaine valeur de SCG dans le VNAS
	CString key; // concatenation de sl et sc pour comparaison avec :
	CString skey; // clef dans la map m_LegScg
	int vg;	  // valeur du SCG correspondant à sc.
	int k = sVnas1.Find('/');
	sl = sVnas1.Mid (k+6,10); // OD du VNAU
	sv = sVnas1.Right(sVnas1.GetLength() -k -16); 
	if (!sVnas2.IsEmpty())
		sv += sVnas2.Right(sVnas2.GetLength() -k -16);
	// sv contient toutes les autorisations SCI
	// on passe en revue les compartiments
	k = sv.Find("G-");
	while (k > 0)
	{
		sc = sv.Mid(k-1,1); // le compartiment
		int i = sv.Find('/', k);
		if (i == -1)
			i = sv.GetLength();
		sg = sv.Mid (k+2, i-k-2);
		vg = atoi ((LPCSTR)sg);
		skey = sl+sc;
		int val;
		POSITION pos = m_LegScg.GetStartPosition();
		while (pos)
		{
			m_LegScg.GetNextAssoc( pos, key, (CObject*&)val );
			if ((key != skey) && (key.Right(1) == skey.Right(1)) &&
				((!val && vg) || (val && !vg)))
			{
				CString mess;
				mess.Format("SCG nul sur une partie de la desserte pour espace physique %s", sc);
				AfxMessageBox (mess, MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION );
				return;
			}
		}
		k++;
		k= sv.Find ("G-", k);
	}
}


// Iterate throught the Vnl object to find the given bkt
//   and return the cmpt level object
//
YM_VnlCmpt* CResaRailView::FindVnlCmpt(CString szCmpt)
{
	if ( ( m_pVnl == NULL ) || ( m_pVnlLeg == NULL ) )
		return NULL;

	szCmpt.MakeUpper(); // remettre les éventuel sci en majuscules.

	YM_VnlCmptIterator* CmptIterator = m_pVnlLeg->CreateVnlCmptIterator();
	YM_VnlCmpt* pVnlCmptRet = NULL;  // this is our returned pointer
	YM_VnlCmpt* pVnlCmpt = NULL;  // this is our working pointer

	for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
	{
		pVnlCmpt = CmptIterator->Current();
		if ( pVnlCmpt->GetCmptId() == szCmpt )
		{
			pVnlCmptRet = pVnlCmpt;  // let's return this one
			break;  // and get out of this loop
		}
	}
	// clean up and return the right thing
	delete CmptIterator;
	return pVnlCmptRet;
}

// Iterate throught the Vnl object to find the given cmpt / bkt
//   and return the bucket level object
//
YM_VnlBkt* CResaRailView::FindVnlCmptBkt(CString szCmpt, int iNestLevel)
{
	if ( ( m_pVnl == NULL ) || ( m_pVnlLeg == NULL ) )
		return NULL;

	YM_VnlCmptIterator* CmptIterator = m_pVnlLeg->CreateVnlCmptIterator();
	YM_VnlCmpt* pVnlCmpt = NULL;  // this is our cmpt level object

	YM_VnlBktIterator* BktIterator = NULL;
	YM_VnlBkt* pVnlBktRet = NULL;  // this is our returned pointer
	YM_VnlBkt* pVnlBkt = NULL;  // this is our working pointer

	for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
	{
		pVnlCmpt = CmptIterator->Current();
		if ( pVnlCmpt->GetCmptId() == szCmpt )  // is it this cmpt ?
		{
			BktIterator = pVnlCmpt->CreateVnlBktIterator();
			for ( BktIterator->First(); ! BktIterator->Finished(); BktIterator->Next() )
			{
				pVnlBkt = BktIterator->Current();
				if ( pVnlBkt->GetNestLevel() == iNestLevel )  // is it this nest level ?
				{
					pVnlBktRet = pVnlBkt;
					break;
				}
			}
			delete BktIterator;
			break;
		}
	}
	// clean up and return the right thing
	delete CmptIterator;
	return pVnlBktRet;
}

// Iterate throught the Vnl object to find the given cmpt / bkt
//   and return the bucket level object
//
YM_VnlSci* CResaRailView::FindVnlCmptSci(CString szCmpt, int iSci)
{
	if ( ( m_pVnl == NULL ) || ( m_pVnlLeg == NULL ) )
		return NULL;

	szCmpt.MakeUpper();

	YM_VnlCmptIterator* CmptIterator = m_pVnlLeg->CreateVnlCmptIterator();
	YM_VnlCmpt* pVnlCmpt = NULL;  // this is our cmpt level object

	YM_VnlSciIterator* SciIterator = NULL;
	YM_VnlSci* pVnlSciRet = NULL;  // this is our returned pointer
	YM_VnlSci* pVnlSci = NULL;  // this is our working pointer

	for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
	{
		pVnlCmpt = CmptIterator->Current();
		if ( pVnlCmpt->GetCmptId() == szCmpt )  // is it this cmpt ?
		{
			SciIterator = pVnlCmpt->CreateVnlSciIterator();
			for ( SciIterator->First(); ! SciIterator->Finished(); SciIterator->Next() )
			{
				pVnlSci = SciIterator->Current();
				if ( pVnlSci->GetNestLevel() == iSci )  // is it this nest level ?
				{
					pVnlSciRet = pVnlSci;
					break;
				}
			}
			delete SciIterator;
			break;
		}
	}
	// clean up and return the right thing
	delete CmptIterator;
	return pVnlSciRet;
}

// Iterate through all the buckets of each compartment
//   and ensure that the authorization levels are descending
//   and determine if the Revenue Mix Model should be executed
// Note: we keep the cmpt/bkt data object and the domain record in sync
//
void CResaRailView::BucketCheck()
{
	// do we have some data ?
	if( GetCmptBktMap()->GetCount() != 0 )
	{
		CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
		CLegView* pLegView = GetLegView();

		if ( ! pLegView )
		{
			AfxMessageBox( 
				((CCTAApp*)APP)->GetResource()->LoadResString(IDS_LEGVIEW_NOT_PRESENT_WARN), 
				MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION );
		}


		// iterate through the ResaRail view's compartment map
		POSITION pos = GetCmptBktMap()->GetStartPosition();
		while( pos != NULL )
		{
			CString sKey;
			CResaCmptBkt* pCmptBkt;

			GetCmptBktMap()->GetNextAssoc( pos, sKey, pCmptBkt );
			YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;

			// how many buckets do we have ?
			int iArraySize = pCmptBkt->m_CmptBktLineArray.GetSize();

			// DO NOT RUN Revenue Mix Model if Adjustment is set ON
			if ( pLegView && (! pLegView->GetOvbAdjustFlag()) )
			{
				//   run Revenue Mix Model if:
				//     - Authorization value is modified for bucket 0 
				//     - FC Demand value is modified for any bucket
				BOOL bRunRevMix = FALSE;
				for( int iBktIdx = 0; iBktIdx < iArraySize; iBktIdx++ )  // if ANY dmd fcst value has changed
				{
					// if Rev Mix flag is set for this bucket
					if ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetDmdRevMixFlag() )  // set in method UpdateData 
					{
						bRunRevMix = TRUE;  // okay, let's get ready to run it

						// reset the flag so we don't run it again (until user changes something)
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->SetDmdRevMixFlag( FALSE );
					}
				}

				// if Rev Mix flag is set for bucket 0
				if ( pCmptBkt->m_CmptBktLineArray[ 0 ]->GetAuthRevMixFlag() )  // set in method UpdateData 
				{
					bRunRevMix = TRUE;  // okay, let's get ready to run it

					// reset the flag so we don't run it again (until user changes something)
					pCmptBkt->m_CmptBktLineArray[ 0 ]->SetAuthRevMixFlag( FALSE );
				}
			}

			// iterate through all the buckets for this compartment
			//   and ensure a descending or equivalent order

			int iAuthPrev = INT_MAX;
			iAuthPrev = pCmptBkt->m_CmptBktLineArray[ 0 ]->m_lAuthNew;
			BOOL bRecalc = pCmptBkt->m_CmptBktLineArray[ 0 ]->GetAuthModify();
			for( int iBktIdx = 1; iBktIdx < iArraySize; iBktIdx++ )
			{
				if ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetAuthModify() )
					bRecalc = TRUE;
				if ( bRecalc && ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew >= iAuthPrev ) )
				{
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew = __max( 0, iAuthPrev );

					// set the new value in the domain (db) record
					YmIcResaBktDom* pResaBkt = 
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
					pResaBkt->AuthUser( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew );
				}

				// hold the previous bucket authorization
				iAuthPrev = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;
			}
		}
	}
}




// Iterate through all the buckets of each compartment and scale the auth levels accordingly.
// Keep the difference of the modification (difference of before and after modification),
//    and use this difference to change the auth values of all other buckets.
// Note: we keep the cmpt/bkt data object and the domain record in sync
//
void CResaRailView::OvbAdjust()
{
	// do we have some data ?
	if( GetCmptBktMap()->GetCount() == 0 )
		return;

	// iterate through the ResaRail view's compartment map
	POSITION pos = m_CmptBktMap.GetStartPosition();
	while( pos != NULL )  // iterate through the compartments
	{
		CString sKey;
		CResaCmptBkt* pCmptBkt;
		m_CmptBktMap.GetNextAssoc( pos, sKey, pCmptBkt );

		// if the auth for Overbooking Level (bkt 0) was not modified, get out now,
		//    because we do not want to scale the lower valued buckets
		CResaCmptBktLine* p = pCmptBkt->m_CmptBktLineArray[ 0 ];

		YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;

		// iterate through all the buckets for this compartment
		//   and scale the authorization levels
		int iDiffFcCur = 0;
		int iPrevBktAuth    = 0;
		BOOL bFoundModified = FALSE;  // boolean which indicates we've found a modified edit box
		YmIcResaBktDom* pResaBkt = NULL;
		for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )  // iterate through buckets
		{
			pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;  // get record 

			// this is where we do the adjustment "ecart" to keep the difference between the value
			// entered by the user, and the value which existed prior to the modification.
			// we take this difference, and apply it to all "lower-valued" buckets.
			//
			//  we are looking for the first authorizations edit box which has adjusted flag set to FALSE.
			//  when we find the first one, we signal by setting a boolean variable, 
			//  then we adjust all remaining bucktes from that index to the end of the array.
			if ( ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew != NULL ) &&
				( ! pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew->GetEditAdjusted() || bFoundModified ) )
			{
				if ( ! bFoundModified )  // this is the first one (lowest index) mofidifed
				{
					// ensure current auth entered by user is a positive value (or zero)
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew =
						__max( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew, 0 );

					// calculate the difference between the value entered by the user 
					//   and the value stored in the record.
					iDiffFcCur = 
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew - 
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthPrev;

					// force the previous value to be equal to the new value
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthPrev =
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;

					// hold the previous bucket's value entered by the user
					// so that we ensure descending authorization values
					iPrevBktAuth = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;

					// set it in the domain (db) record
					pResaBkt->AuthUser( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew );

					// okay, we've adjusted this one, now turn off the flag
					if (pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew != NULL)
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew->SetEditAdjusted( TRUE );
					bFoundModified = TRUE;  // indicate we've found the modified edit box
				}
				else
				{
					// calculate auth user for the record
					int iAuthNew = __max( pResaBkt->AuthUser() + iDiffFcCur, 0 );

					// set it in the cmpt/bkt data object
					// we take the minimum of the newly calculated authorization, and
					//   the authorization of the bucket + 1 ( next higher-valued bucket )
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew = 
						iPrevBktAuth = __min( iAuthNew, iPrevBktAuth );

					// set it in the domain (db) record
					pResaBkt->AuthUser( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew );
				}
			}  // if adjusted flag is set
		}  // bkt loop
	}  // cmpt loop
}

// Get the availability for this cmpt and nest level
//
int CResaRailView::GetResaValue( const char* cCmpt, int iNestLevel, CResaRailView::AuthAvailInd eAuthAvailInd )
{
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	int iReturnVal = INT_MAX;
	if( m_CmptBktMap.GetCount() != 0 )  // if we have data 
	{
		// iterate through the ResaRail view's compartment map to find this cmpt
		POSITION pos = m_CmptBktMap.GetStartPosition();
		while( pos != NULL )  // iterate through the compartments
		{
			CString sKey;
			CResaCmptBkt* pCmptBkt;
			m_CmptBktMap.GetNextAssoc( pos, sKey, pCmptBkt );
			YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;
			if (pResaCmpt == NULL)
				continue;
			if (!strcmp(pResaCmpt->Cmpt(), cCmpt))
			{
				// iterate through the buckets to find this bucket
				YmIcResaBktDom* pResaBkt;
				for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
				{
					pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
					if (pResaBkt == NULL)
						continue;
					if (pResaBkt->NestLevel() == iNestLevel)
					{
						switch(eAuthAvailInd)  // what do they want ?
						{
						case CResaRailView::AVAIL_CURRENT :
							return pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailCur;
							break;
						case CResaRailView::AVAIL_FORECAST :  // CResaRailView::AVAIL_USER :
							return pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailUser;
							break;
						case CResaRailView::AVAIL_FORECAST_ORIG :
							return pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailFcOrg;
							break;
						case CResaRailView::AUTH_NEW :
							return pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;
							break;
						case CResaRailView::AUTH_PREV :
							return pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthPrev;
							break;
						case CResaRailView::ADD_DMD :
							return pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAddDmdNew;
							break;
						}
					}
				}
			}
		}
	}
	return iReturnVal;
}

// Get the res hold value either from VNL object from Resa Rail or from data structure from database
//
int CResaRailView::GetRealTimeResHold( const char* cCmpt, int iNestLevel, YmIcResaBktDom* m_pResaBkt )
{
	YM_VnlBkt* pVnlBkt = FindVnlCmptBkt( cCmpt, iNestLevel );
	if (pVnlBkt)  // is there a Vnl Object (i.e., have we received the results of the VNL from ResaRail) ?
		return pVnlBkt->GetBaseSold();  // get the bucket level data from the vnl for this cmpt / bkt

	return (m_pResaBkt->ResHoldIdv()+m_pResaBkt->ResHoldGrp()+m_pResaBkt->ResHoldNotYield() );
}

int CResaRailView::GetRealTimeSciResHold( const char* cCmpt, int iSci, YmIcResaBktDom* m_pResaBkt )
{
	YM_VnlSci* pVnlSci = FindVnlCmptSci( cCmpt, iSci );
	if (pVnlSci)  // is there a Vnl Object (i.e., have we received the results of the VNL from ResaRail) ?
		return pVnlSci->GetBaseSold();  // get the sci level data from the vnl for this cmpt / bkt

	return (m_pResaBkt->ResHoldIdv()+m_pResaBkt->ResHoldGrp());
}

int CResaRailView::GetRealTimeSciEtanche( const char* cCmpt, int iSci, YmIcResaBktDom* m_pResaBkt )
{
	YM_VnlSci* pVnlSci = FindVnlCmptSci( cCmpt, iSci );
	if (pVnlSci)  // is there a Vnl Object (i.e., have we received the results of the VNL from ResaRail) ?
		return pVnlSci->GetEtanche();  // get the sci level data from the vnl for this cmpt / bkt

	return (m_pResaBkt->ScxEtanche());
}

// Get the auth value either from VNL object from Resa Rail or from data structure from database
//
int CResaRailView::GetRealTimeAuth( YmIcResaCmptDom* pResaCmpt, int iNestLevel, YmIcResaBktDom* pResaBkt )
{
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	// we may need a ceiling...
	long lTotalCap = 0;

	if (m_pVnl && m_pVnlLeg)  // is there a Vnl Object (i.e., have we received the results of the VNL from ResaRail) ?
	{
		// get the bucket level data from the vnl for this cmpt / bkt
		YM_VnlBkt* pVnlBkt = FindVnlCmptBkt( pResaCmpt->Cmpt(), iNestLevel );
		if ( pVnlBkt )  // we found this cmpt/bkt in the Vnl results
			return pVnlBkt->GetAuth();
	}
	if ( ! pResaBkt->AuthCurIsNull() )
		return pResaBkt->AuthCur();
	else
		return lTotalCap;  // we return Auth User because we need a ceiling if AuthCur is NULL !
}

// Get the auth value either from VNL object from Resa Rail or from data structure from database
//
int CResaRailView::GetRealTimeSciAuth( const char* cCmpt, int iSci, YmIcResaBktDom* pResaBkt )
{
	// we may need a ceiling...
	long lTotalCap = 0;
	YM_VnlSci* pVnlSci = FindVnlCmptSci( cCmpt, iSci );
	if (pVnlSci)
		return pVnlSci->GetAuth();

	if ( ! pResaBkt->AuthCurIsNull() )
		return pResaBkt->AuthCur();
	else
		return lTotalCap;  // we return Auth User because we need a ceiling if AuthCur is NULL !
}

int CResaRailView::GetRealTimeScgCapa( const char* cCmpt, YmIcResaCmptDom* pResaCmpt)
{
	YM_VnlCmpt* pVnlCmpt = FindVnlCmpt( cCmpt );
	if (pVnlCmpt)
		return pVnlCmpt->GetScgCapa();
	else
		return pResaCmpt->Capacity();
}

void CResaRailView::GetCapValues(
																 YmIcResaCmptDom* pResaCmpt, 
																 long& lReservableCap, 
																 long& lStandingCap, 
																 long& lUnreservableCap)
{
	YM_VnlCmpt* pVnlCmpt = FindVnlCmpt( pResaCmpt->Cmpt() );
	lReservableCap = ( pVnlCmpt == NULL ) ? pResaCmpt->Capacity() : pVnlCmpt->GetCapacity() ;
	YmIcLegsDom* pLeg = 
		YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
	lStandingCap = 0;
	lUnreservableCap = 0;
}

double CResaRailView::GetAddDmdFc( YmIcResaBktDom* pResaBkt, BOOL& bIsNull)
{
	if (pResaBkt->AddDmdFcIsNull())
		bIsNull = TRUE;
	return (pResaBkt->AddDmdFc());
}

double CResaRailView::GetAddDmdFcNew( YmIcResaBktDom* pResaBkt )
{
	return (pResaBkt->AddDmdFcUser());
}

void CResaRailView::SetAddDmdFcUser( YmIcResaBktDom* pResaBkt, double dDmdFcValue )
{
	pResaBkt->AddDmdFcUser(dDmdFcValue);
}


// renvoi la dispo de la bucket
int CResaRailView::GetAvailBkt (char cmpt, int bkt)
{
	if( m_CmptBktMap.GetCount() == 0)
		return 0;
	if (bkt < 0)
		return 0;
	POSITION pos = m_CmptBktMap.GetStartPosition();
	while( pos != NULL )
	{
		CString sKey;
		CResaCmptBkt* pCmptBkt = NULL;
		m_CmptBktMap.GetNextAssoc( pos, sKey, pCmptBkt );
		if (pCmptBkt && (toupper(sKey[0]) == toupper(cmpt)))
		{
			if (bkt >= pCmptBkt->m_CmptBktLineArray.GetSize())
				return 0;
			return pCmptBkt->m_CmptBktLineArray[ bkt ]->m_lAvailCur;
		}
	}
	return 0;
}

// Calculate availability based upon either Net Nesting or Threshold Nesting Method
//
void CResaRailView::CalcAvail(BOOL bCalculate /* = TRUE */)
{
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	if( m_CmptBktMap.GetCount() != 0 )  // if we have data 
	{
		// iterate through the ResaRail view's compartment map
		POSITION pos = m_CmptBktMap.GetStartPosition();
		while( pos != NULL )  // iterate through the compartments
		{
			CString sKey;
			CResaCmptBkt* pCmptBkt;
			m_CmptBktMap.GetNextAssoc( pos, sKey, pCmptBkt );
			YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;
			if (pResaCmpt == NULL)
				return;

			int iAvailCmp;
			int iAvailPrevCur = INT_MAX;  // initialize prev avail with something big
			int iAvailPrevFcst = INT_MAX;  // initialize prev avail with something big
			int iAvailPrevUser = INT_MAX;  // initialize prev avail with something big
			int iAuthRealTime = 0;
			long lResHoldAccum;

			// récupération de la somme des dispo des sci étanches de l'espace physique
			int sumDispEtanche = GetSumDispEtanche(pResaCmpt->Cmpt()[0]);

			// iterate through the buckets
			YmIcResaBktDom* pResaBkt;
			for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
				lResHoldAccum = 0;
				if ( pDoc->m_pGlobalSysParms->GetInventoryNestingMethod() )
				{
					// NET NESTING: begin with current bkt, iterate through all lower valued buckets to accumulate ResHold
					for( int j = iBktIdx; j < pCmptBkt->m_CmptBktLineArray.GetSize(); j++ )
					{
						lResHoldAccum += GetRealTimeResHold( pResaCmpt->Cmpt(), 
							pCmptBkt->m_CmptBktLineArray[ j ]->m_pResaBkt->NestLevel(), 
							pCmptBkt->m_CmptBktLineArray[ j ]->m_pResaBkt );
					}
				}
				else
				{
					// THRESHOLD NESTING: 
					// is there a Vnl Object (i.e., have we received the results of the VNL from ResaRail) ?
					YM_VnlCmpt* pVnlCmpt = FindVnlCmpt( pResaCmpt->Cmpt() );
					if ( pVnlCmpt)
					{
						// no need to iterate through the buckets because we have seats sold only at the cmpt level for threshold nesting
						lResHoldAccum = pVnlCmpt->GetBaseSold();
					}
					else
					{
						// iterate through all buckets to calculate accumulated ResHold.  begin at bucket 0 for threshold nesting
						for( int j = 0; j < pCmptBkt->m_CmptBktLineArray.GetSize(); j++ )
						{
							lResHoldAccum += GetRealTimeResHold( pResaCmpt->Cmpt(), 
								pCmptBkt->m_CmptBktLineArray[ j ]->m_pResaBkt->NestLevel(), 
								pCmptBkt->m_CmptBktLineArray[ j ]->m_pResaBkt );
						}
					}
				}
				iAvailPrevCur = (iBktIdx <= 0) ? INT_MAX : pCmptBkt->m_CmptBktLineArray[ iBktIdx-1 ]->m_lAvailCur;
				iAvailPrevFcst = (iBktIdx <= 0) ? INT_MAX : pCmptBkt->m_CmptBktLineArray[ iBktIdx-1 ]->m_lAvailUser;
				iAvailPrevUser = (iBktIdx <= 0) ? INT_MAX : pCmptBkt->m_CmptBktLineArray[ iBktIdx-1 ]->m_lAvailFcOrg;

				if ( bCalculate )  // do we calculate availability ?
				{
					// Current Availability
					// get real time authorization (if available)
					iAuthRealTime = 
						GetRealTimeAuth( 
						pResaCmpt, 
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->NestLevel(), 
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt );
					// calculate the availability value which we will compare with the previous bucket (do not allow negative values)
					iAvailCmp = __max ( iAuthRealTime - lResHoldAccum, 0 );
					// set the availability as the minimum of this and the previous bucket 
					//  (ie, do not allow this bucket to be greater than the previous bucket)
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailCur = ( __min ( iAvailCmp, iAvailPrevCur ) ) ;

					// retrait éventuelle dispo des sci étanches
					if (iBktIdx == 0)
					{
						int avail = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailCur;
						avail -= sumDispEtanche;
						if (avail < 0)
							avail = 0;
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailCur = avail;
					}
				}
				else  // get availability value it directly from the database
				{
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailCur = 
						__max(pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->AvailabilityCur(), 0) ;
				}

				if ( bCalculate )  // do we calculate availability ?
				{
					// Forecasted Availability (User Modified)
					// get the availability for the previous bucket
					//        iAvailPrevFcst = (iBktIdx <= 0) ? INT_MAX : pCmptBkt->m_CmptBktLineArray[ iBktIdx-1 ]->m_lAvailUser;
					// calculate the availability value which we will compare with the previous bucket (do not allow negative values)
					iAvailCmp = __max ( 
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->AuthUser() - lResHoldAccum, 0 );
					// set the availability as the minimum of this and the previous bucket 
					//  (ie, do not allow this bucket to be greater than the previous bucket)
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailUser = ( __min ( iAvailCmp, iAvailPrevFcst ) ) ;
					// retrait éventuelle dispo des sci étanches
					if (iBktIdx == 0)
					{
						int avail = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailUser;
						avail -= sumDispEtanche;
						if (avail < 0)
							avail = 0;
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailUser = avail;
					}
				}
				else  // get availability value it directly from the database
				{
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailUser = 
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->AvailabilityFc() ;
				}

				if ( bCalculate )  // do we calculate availability ?
				{
					// Original Forecasted Availability
					// get the availability for the previous bucket
					//        iAvailPrevUser = (iBktIdx <= 0) ? INT_MAX : pCmptBkt->m_CmptBktLineArray[ iBktIdx-1 ]->m_lAvailFcOrg;
					// calculate the availability value which we will compare with the previous bucket (do not allow negative values)
					iAvailCmp = __max ( 
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->AuthFcOrg() - lResHoldAccum, 0 );
					// set the availability as the minimum of this and the previous bucket 
					//  (ie, do not allow this bucket to be greater than the previous bucket)
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailFcOrg =  ( __min ( iAvailCmp, iAvailPrevUser ) ) ;
				}
				else  // get availability value it directly from the database
				{
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailFcOrg = 
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->AvailabilityFc() ;
				}
			}
		}
	}
}

//Renvoi la somme des Dispo des SCI étanches
int CResaRailView::GetSumDispEtanche(char cmpt)//SRE BUG-i 101835 Ajout de la somme des dispo des SC Etanche depuis les données VNL si disponible.
{
	int sumDispEtanche = 0;

	if (m_pVnl && m_pVnlLeg)
	{
		YM_VnlCmptIterator* CmptIterator = m_pVnlLeg->CreateVnlCmptIterator();
		YM_VnlCmpt* pVnlCmpt = NULL;  // this is our cmpt level object

		YM_VnlSciIterator* SciIterator = NULL;
		YM_VnlSci* pVnlSciRet = NULL;  // this is our returned pointer
		YM_VnlSci* pVnlSci = NULL;  // this is our working pointer

		for ( CmptIterator->First(); ! CmptIterator->Finished(); CmptIterator->Next() )
		{
			pVnlCmpt = CmptIterator->Current();
			if ( pVnlCmpt->GetCmptId()[0] == cmpt )  // is it this cmpt ?
			{
				SciIterator = pVnlCmpt->CreateVnlSciIterator();
				for ( SciIterator->First(); ! SciIterator->Finished(); SciIterator->Next() )
				{
					if (SciIterator->Current()->GetEtanche())  // is it this nest level ?
					{
						sumDispEtanche += SciIterator->Current()->GetAvail();
					}
				}
				delete SciIterator;
				break;
			}
		}
		// clean up and return the right thing
		delete CmptIterator;
	}
	else
	{
		if( m_CmptBktSciMap.GetCount() != 0 )  // if we have data 
		{
			POSITION pos = m_CmptBktSciMap.GetStartPosition();
			while( pos != NULL )  // iterate through the compartments
			{
				CString sKey;
				CResaCmptBktSci* pCmptBkt;
				m_CmptBktSciMap.GetNextAssoc( pos, sKey, pCmptBkt );
				if (toupper(sKey[0]) == toupper(cmpt))
				{
					for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
					{
						YmIcResaBktDom* pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
						if (pResaBkt->ScxEtanche() && !pResaBkt->Clicked())
							sumDispEtanche += __max (pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailCur, 0);
					}
				}
			}
		}
	}

	return sumDispEtanche;
}


//calcul des disponibilité par SCI
void CResaRailView::CalcAvailSci(BOOL bVnl)
{
	BOOL yaEtanche = FALSE;
	if( m_CmptBktSciMap.GetCount() != 0 )  // if we have data 
	{
		// iterate through the ResaRail view's compartment map
		POSITION pos = m_CmptBktSciMap.GetStartPosition();
		while( pos != NULL )  // iterate through the compartments
		{
			CString sKey;
			CResaCmptBktSci* pCmptBkt;
			m_CmptBktSciMap.GetNextAssoc( pos, sKey, pCmptBkt );
			YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;
			if (pResaCmpt == NULL)
				return;

			long lResHoldAccum = 0;

			// iterate through the sci to calculate SUM of sells
			YmIcResaBktDom* pResaBkt;
			for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
				if (pResaBkt->ScxEtanche())
					yaEtanche = TRUE;
				lResHoldAccum += GetRealTimeSciResHold( pResaCmpt->Cmpt(), pResaBkt->NestLevel(), pResaBkt);
			}
			int ScgCapa = 0;
			if (bVnl)
				ScgCapa = GetRealTimeScgCapa (pResaCmpt->Cmpt(), pResaCmpt);
			else
				ScgCapa = pResaCmpt->Capacity();

			int ScgDisp = ScgCapa - lResHoldAccum;
			char cmpt = pCmptBkt->m_pResaCmpt->Cmpt()[0];
			if (cmpt > 'Z')
				cmpt -= 32;
			int Bkt0Disp = GetAvailBkt (cmpt, 0);
			// TDR - Anomalie 101835
			/*if (ScgDisp > Bkt0Disp)
				ScgDisp = Bkt0Disp;*/

			int sumDispEtanche = 0;
			//calcul de la somme des delta autorisations - ventes des SCI étanche, et mise à jour dispo des sci étanche
			for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
				if (pResaBkt->ScxEtanche() && !pResaBkt->Clicked())
				{
					int SciAuth = 0;
					if (bVnl)
						SciAuth = GetRealTimeSciAuth (pResaCmpt->Cmpt(), pResaBkt->NestLevel(), pResaBkt);
					else
						SciAuth =  pResaBkt->AuthUser();

					int SciResHold = GetRealTimeSciResHold( pResaCmpt->Cmpt(), pResaBkt->NestLevel(), pResaBkt);
					int SciDisp = SciAuth - SciResHold;
					if (SciDisp < 0)
						SciDisp = 0;
					sumDispEtanche += SciDisp;
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailCur = SciDisp;
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailUser = SciDisp;
				}
			}
			ScgDisp -= sumDispEtanche; // On retranche les dispo des sci étanches

			// Mise à jour dispo des sci non étanche
			for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
				if (!pResaBkt->ScxEtanche() || pResaBkt->Clicked())
				{
					int RealTimeSciResHold = GetRealTimeSciResHold( pResaCmpt->Cmpt(), pResaBkt->NestLevel(), pResaBkt);
					int SciAuth = 0;
					//this->GetRe
					if (bVnl)
						SciAuth = GetRealTimeSciAuth (pResaCmpt->Cmpt(), pResaBkt->NestLevel(), pResaBkt);
					else
						SciAuth =  pResaBkt->AuthUser();

					int SciDisp = SciAuth - RealTimeSciResHold;

					int ScgAuth = this->GetScgAuth(pResaCmpt->Cmpt()[0]);//Pour debug
					//int realtimereshold = this->GetRealTimeResHold(pResaCmpt->Cmpt(), pResaBkt->NestLevel(), pResaBkt);
					int ScgResHold = this->GetScgResHold(pResaCmpt->Cmpt()[0]);
					// TDR - Anomalie 101835
					/*if (SciDisp > this->GetScgAuth(pResaCmpt->Cmpt()[0]) - this->GetScgResHold(pResaCmpt->Cmpt()[0]) - sumDispEtanche)
						SciDisp = (this->GetScgAuth(pResaCmpt->Cmpt()[0]) - this->GetScgResHold(pResaCmpt->Cmpt()[0]) - sumDispEtanche);*/

					if (SciDisp > (ScgAuth - ScgResHold - sumDispEtanche))
						SciDisp = ScgAuth - ScgResHold - sumDispEtanche;

					if (SciDisp > Bkt0Disp)
						SciDisp = Bkt0Disp;
					if (SciDisp < 0)
						SciDisp = 0;
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailCur = SciDisp;
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailUser = SciDisp;
				}
			}
		}
		if (yaEtanche)
		{ // présence de SCI étanche et appel de la fonction suite à une modif d'autorisation de SCI ou SCG
			// On appel le calcul de dispo des buckets pour la prise en compte de l'étanchétité dans les disp des buckets 0
			CalcAvail (TRUE);
		}
	}
}

// Contrôle de l'étanchéité après reception du VNL
void CResaRailView::CtrlEtancheOnVnl()
{
	if( m_CmptBktSciMap.GetCount() != 0 )  // if we have data 
	{
		// iterate through the ResaRail view's compartment map
		POSITION pos = m_CmptBktSciMap.GetStartPosition();
		while( pos != NULL )  // iterate through the compartments
		{
			CString sKey;
			CResaCmptBktSci* pCmptBkt;
			m_CmptBktSciMap.GetNextAssoc( pos, sKey, pCmptBkt );
			YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;
			if (pResaCmpt == NULL)
				return;
			YmIcResaBktDom* pResaBkt;
			for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
				int etan = GetRealTimeSciEtanche( pResaCmpt->Cmpt(), pResaBkt->NestLevel(), pResaBkt);
				if ((pResaBkt->ScxEtanche() && !etan) || (!pResaBkt->ScxEtanche() && etan))
				{
					YM_Query* pSQL = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
					pResaBkt->ScxEtanche(etan ? 1 : 0);
					pSQL->SetDomain(pResaBkt);
					((CCTAApp*)APP)->m_pDatabase->Transact(pSQL, IDS_SQL_UPDATE_ETANCHE_SCI);
					delete pSQL;
				}
			}
		}
	}
}

// renvoi vrai si les autorisations des SCI sont correctes par rapport à la valeur de SCG
BOOL CResaRailView::CtrlAuthSci(char cmpt, int valscg)
{
	if( m_CmptBktSciMap.GetCount() != 0 )  // if we have data 
	{
		POSITION pos = m_CmptBktSciMap.GetStartPosition();
		while( pos != NULL )  // iterate through the compartments
		{
			int sumAuthEtanche = 0;
			CString sKey;
			CResaCmptBktSci* pCmptBkt;
			m_CmptBktSciMap.GetNextAssoc( pos, sKey, pCmptBkt );
			if (toupper(sKey[0]) == toupper(cmpt))
			{
				int Bkt0Disp = GetAvailBkt (pCmptBkt->m_pResaCmpt->Cmpt()[0], 0);
				for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
				{
					YmIcResaBktDom* pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
					if (pResaBkt->ScxEtanche() && !pResaBkt->Clicked())
					{
						sumAuthEtanche += pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;
						if (pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailUser > Bkt0Disp)
						{
							AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DISP_ETANCHE_TOO_HIGH), 
								MB_OK | MB_APPLMODAL | MB_ICONSTOP );
							return FALSE;
						}
					}
				}
				if (sumAuthEtanche > valscg)
				{
					AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_SUM_ETANCHE_SUP_SCG), 
						MB_OK | MB_APPLMODAL | MB_ICONSTOP );
					return FALSE;
				}
				for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
				{
					YmIcResaBktDom* pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
					if (!pResaBkt->ScxEtanche() || pResaBkt->Clicked())
					{
						int valsci = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailCur;
						if (valsci > valscg - sumAuthEtanche)
						{
							AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NON_ETANCHE_TROP_HAUT), 
								MB_OK | MB_APPLMODAL | MB_ICONSTOP );
							return FALSE;
						}
					}
				}
			}
		}
	}
	return TRUE;
}



// This method works just as the CWnd::UpdateData method
// Initialize data in an edit window (bSaveAndValidate = FALSE), 
// or to retrieve and validate data from an edit window (bSaveAndValidate = TRUE)
//
void CResaRailView::UpdateData(BOOL bSaveAndValidate)
{
	BOOL bUpdate = FALSE;
	BOOL bTrans;

	if( m_CmptBktMap.GetCount() != 0 )  // if we have data 
	{
		POSITION pos = m_CmptBktMap.GetStartPosition();
		while( pos != NULL )  // iterate through the compartments
		{
			CString sKey;
			CResaCmptBkt* pCmptBkt;
			m_CmptBktMap.GetNextAssoc( pos, sKey, pCmptBkt );
			YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;

			// iterate through the buckets
			for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				// was authorization modified ?
				// get the value from the demand edit window
				LONG lAuth = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;  // initialize the auth value as same from member var
				if (pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew != NULL)  // if we have an edit window, get the auth value
					lAuth = GetDlgItemInt( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew->GetDlgCtrlID(), &bTrans, FALSE );

				if( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew != lAuth )
				{
					// indicate that a value for this line changed
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->SetAuthModify( TRUE );
					/* FOR NOW, WE DISACTIVATE REVENUE MIX MODEL FOR AUTHORIZATION LEVEL !!!!
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->SetAuthRevMixFlag( TRUE );
					*/
					if ( bSaveAndValidate )  // from window to data
					{
						bUpdate = TRUE;  // update the data obj

						// first, set it in the "previous auth" member variable (hold the previous auth value)
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthPrev = 
							pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;

						// then, set it in the variable representing the edit window's value
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew = lAuth;

						// and, now set it in the domain (db) record
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->AuthUser( lAuth );
					}
					else  // from data to window
					{
						if (pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew != NULL)
						{
							// simply restore the edit window with the value in the database record
							CString s;
							s.Format( "%d", pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew );
							pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew->SetWindowText( s );
						}
					}
				}
			}
		}
	}
	if( bUpdate )
		m_CmptBktMap.AccumAddDmd();  // now update the totals

	UpdateDataSci (bSaveAndValidate);
}

void CResaRailView::UpdateDataSci(BOOL bSaveAndValidate)
{
	BOOL bTrans;

	if( m_CmptBktSciMap.GetCount() != 0 )  // if we have data 
	{
		POSITION pos = m_CmptBktSciMap.GetStartPosition();
		while( pos != NULL )  // iterate through the compartments
		{
			CString sKey;
			CResaCmptBktSci* pCmptBkt;
			m_CmptBktSciMap.GetNextAssoc( pos, sKey, pCmptBkt );
			YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;

			if (pCmptBkt->m_pEditScgCapa != NULL)
			{
				LONG lAuth = GetDlgItemInt( pCmptBkt->m_pEditScgCapa->GetDlgCtrlID(), &bTrans, FALSE );
				if (pCmptBkt->m_lAuthNew != lAuth)
				{
					pCmptBkt->SetAuthModify(TRUE);
					if ( bSaveAndValidate )  // from window to data
					{
						// first, set it in the "previous auth" member variable (hold the previous auth value)
						pCmptBkt->m_lAuthPrev = pCmptBkt->m_lAuthNew;

						// then, set it in the variable representing the edit window's value
						pCmptBkt->m_lAuthNew = lAuth;

						// and, now set it in the domain (db) record
						pCmptBkt->m_pResaCmpt->Capacity( lAuth );
					}
					else
					{
						// simply restore the edit window with the value in the database record
						CString s;
						s.Format( "%d", pCmptBkt->m_lAuthNew );
						pCmptBkt->m_pEditScgCapa->SetWindowText( s );
					}
				}
			}

			// iterate through the buckets
			for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				// was authorization modified ?
				// get the value from the demand edit window
				LONG lAuth = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;  // initialize the auth value as same from member var
				if (pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew != NULL)  // if we have an edit window, get the auth value
					lAuth = GetDlgItemInt( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew->GetDlgCtrlID(), &bTrans, FALSE );

				if( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew != lAuth )
				{
					// indicate that a value for this line changed
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->SetAuthModify( TRUE );
					if ( bSaveAndValidate )  // from window to data
					{
						// first, set it in the "previous auth" member variable (hold the previous auth value)
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthPrev = 
							pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;

						// then, set it in the variable representing the edit window's value
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew = lAuth;

						// and, now set it in the domain (db) record
						pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->AuthUser( lAuth );
					}
					else  // from data to window
					{
						if (pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew != NULL)
						{
							// simply restore the edit window with the value in the database record
							CString s;
							s.Format( "%d", pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew );
							pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew->SetWindowText( s );
						}
					}
				}
			}
		}
	}
}

// Iterate through all the buckets of each compartment and see if something was modified
BOOL CResaRailView::Modified()
{
	// do we have some data ?
	if( GetCmptBktMap()->GetCount() == 0 )
		return FALSE;

	// iterate through the compartment map
	POSITION pos = m_CmptBktMap.GetStartPosition();
	CString sKey;
	while( pos != NULL )
	{
		CResaCmptBkt* pCmptBkt;
		m_CmptBktMap.GetNextAssoc( pos, sKey, pCmptBkt );
		if ( pCmptBkt->m_bTGVJumPctChanged )
			return TRUE;    // a compartment was modified

		// iterate through all the buckets for this compartment
		YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;
		for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			if ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetAuthModify() )  // was an auth value modified ?
				return TRUE;  // something was modified
	}

	pos = m_CmptBktSciMap.GetStartPosition();
	while( pos != NULL )
	{
		CResaCmptBktSci* pCmptBkt;
		m_CmptBktSciMap.GetNextAssoc( pos, sKey, pCmptBkt );
		if (pCmptBkt->GetAuthModify())
			return TRUE;

		// iterate through all the buckets for this compartment
		for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			if ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetAuthModify() )  // was an auth value modified ?
				return TRUE;  // something was modified
	}
	return FALSE;
}

// Iterate through all compartment and see it's O
BOOL CResaRailView::TrainAuto()
{
	// do we have some data ?
	if( GetCmptBktMap()->GetCount() == 0 )
		return FALSE;

	// iterate through the compartment map
	POSITION pos = m_CmptBktMap.GetStartPosition();
	CString sKey;
	while( pos != NULL )
	{
		CResaCmptBkt* pCmptBkt;
		m_CmptBktMap.GetNextAssoc( pos, sKey, pCmptBkt );

		// iterate through all the buckets for this compartment
		YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;
		if (!strcmp(pResaCmpt->Cmpt(), "O"))
			return TRUE;
	}
	return FALSE;
}

// Iterate through all the buckets of each compartment and perform some edit checks 
//    (i.e., run the method ResaRailViewEditEntry on each auth edit window)
void CResaRailView::EditEntries()
{
	// do we have some data ?
	if( GetCmptBktMap()->GetCount() == 0 )
		return;

	BOOL bModified = FALSE;  // flag to indicate if we need to update the window ?

	// iterate through the compartment map
	POSITION pos = m_CmptBktMap.GetStartPosition();
	while( pos != NULL )
	{
		CString sKey;
		CResaCmptBkt* pCmptBkt;

		m_CmptBktMap.GetNextAssoc( pos, sKey, pCmptBkt );
		// iterate through all the buckets for this compartment
		for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			if ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetAuthModify() )  // was an auth value modified ?
			{
				bModified = TRUE;  // something was modified, set flag to update the window...
				if (pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew != NULL)
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew->ResaRailViewEditEntry( FALSE );
			}
	}

	pos = m_CmptBktSciMap.GetStartPosition();
	while( pos != NULL )
	{
		CString sKey;
		CResaCmptBktSci* pCmptBkt;

		m_CmptBktSciMap.GetNextAssoc( pos, sKey, pCmptBkt );
		if (pCmptBkt->GetAuthModify())
		{
			bModified = TRUE;  // something was modified, set flag to update the window...
			if (pCmptBkt->m_pEditScgCapa != NULL)
				pCmptBkt->m_pEditScgCapa->ResaRailViewEditEntry (FALSE);
		}

		// iterate through all the buckets for this compartment
		for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			if ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetAuthModify() )  // was an auth value modified ?
			{
				bModified = TRUE;  // something was modified, set flag to update the window...
				if (pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew != NULL)
					pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew->ResaRailViewEditEntry( FALSE );
			}
	}

	if ( bModified )  // do we need to update the window ?
	{
		UpdateData( FALSE ); // JMG ano 131098, si ResaRailViewEditEntry est appelé avec FALSE, je n'y fait plus
		// le resarailview->UpdateData (FAlSE). On le déporte donc ici.

		Invalidate(FALSE);
		UpdateWindow();
	}
}

// Update the database with all compartment values which have changed
//
BOOL CResaRailView::UpdateCompartments( YmIcLegsDom* pLeg /* = NULL */ )
{
	if ( pLeg == NULL )  // Set the Leg Origin from the Leg record
		pLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);

	BOOL bModified = FALSE;

	// do we have some data ?
	if( GetCmptBktMap()->GetCount() != 0 )
	{
		// iterate through the compartments
		POSITION pos = GetCmptBktMap()->GetStartPosition();
		while( pos != NULL )
		{
			CString sKey;
			CResaCmptBkt* pCmptBkt;

			GetCmptBktMap()->GetNextAssoc( pos, sKey, pCmptBkt );

			if ( pCmptBkt->m_bTGVJumPctChanged )
				bModified = TRUE;   // a compartment was modified

			if (bModified)
			{  
				// create new cmpt level record and copy from record set
				YmIcResaCmptDom* pResaCmptBis = new YmIcResaCmptDom(*(pCmptBkt->m_pResaCmpt));
				pResaCmptBis->LegId( pLeg->LegId() );

				YM_Query* pUpdateQuery = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), TRUE);
				pUpdateQuery->Start( pResaCmptBis, IDS_SQL_UPDATE_RESA_CMPT );
			}

			// Now, send the vnau command

			// Did user change at least one value OR does this leg need to be sent
			if ( pCmptBkt->m_bTGVJumPctChanged /*|| ( pLeg->EditStatus() & EDIT_STATUS_TGVMODNOTSENT )*/ )
			{
				CString szMsg;
				SendVnau( szMsg, pLeg, pCmptBkt );
			}
		}
	}
	return bModified;   // was at least one compartment modified ?
}

// ------------------------------------------------------------------------------------
// Pour DM 7978 : permet de gérer le cas CC0=CC1
// méthode appelée à l'intérieur de UpdateBuckets
// qu'on pourra aussi utiliser depuis CLegView pour tester le même cas (CC0=CC1)
// BUGI 69972 : lors du report d'autorisations par copier-collé d'un leg à l'autre,
// il faut vérifier que CC0= cC1 (sous couvert AVT 17 - date bang)
// ------------------------------------------------------------------------------------
BOOL CResaRailView::ControleEgaliteCC0CC1(
	YmIcLegsDom* pLeg, 
	enum CLegView::LegUpdateMode eUpdateMode,
	BOOL bForceAuthMod,
	BOOL &bModified)
{
	BOOL bErreur   = FALSE;  // a priori, pas d'erreur
	if (GetCmptBktMap()->GetCount() == 0)
	{
		return FALSE;
	}

	CCTADoc *pDoc = ((CCTAApp*)APP)->GetDocument();
	// DM 7978 : DT 13903 - avt 17
	// Ne controler CC0 == CC1 que si la date gang est atteinte
	bool controlerCC0EgalCC1 = pDoc->m_pGlobalSysParms->GetControlerCC0EgalCC1();

	// First, we determine if there were any modifications made by the user
	POSITION pos = GetCmptBktMap()->GetStartPosition();
	CString zMsg;
	while( pos != NULL) // &&  ! bModified ) // loop through all compartments until we find mods
	{
		CString sKey;
		CResaCmptBkt* pCmptBkt;
		GetCmptBktMap()->GetNextAssoc( pos, sKey, pCmptBkt );
		YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;

		// iterate through all the buckets for this compartment
		for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
		{
			// was a value in this line changed (auth or dmd) ?
			// if bForceAuthMod (copier coller option) is TRUE, force authorizations to be modified for this leg
			//  That is, pay no attention to the state of the modified flags.
			if ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetAuthModify() || bForceAuthMod )
			{
				bModified = TRUE;
			}
		}

		// DM7978 - LME - YI-6617 - controle CC0 == CC1
		CString szClient = ((CCTAApp*)AfxGetApp())->m_pDatabase->GetClientParameter();
		// Si TGV ou Corail
		if(szClient == SNCF || szClient == SNCF_TRN)
		{
			// si CC0 != CC1 on affiche un message de controle et on annule les modifs
			// DM 7978 : DT 13903 - avt 17
			// Ne controler CC0 == CC1 que si l'indicateur de contrôle est à true
			char cmpt = *(pCmptBkt->m_pResaCmpt->Cmpt());
			if (controlerCC0EgalCC1 &&
				cmpt != 'O' &&  // Auto-train exclu de ce contrôle
				pCmptBkt->m_CmptBktLineArray[0]->m_lAuthNew != pCmptBkt->m_CmptBktLineArray[1]->m_lAuthNew)
			{
				CString longToString;
				if (zMsg.GetLength() == 0)
				{
					zMsg = ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_RESA_BLOCK_MESSAGE );
					zMsg += '\n';
				}
				else
				{
					zMsg += '\n';
				}

				zMsg += pCmptBkt->m_pResaCmpt->Cmpt();
				zMsg += "0(";
				longToString.Format("%ld", pCmptBkt->m_CmptBktLineArray[0]->m_lAuthNew);
				zMsg += longToString;
				zMsg += ") !=  ";
				zMsg += pCmptBkt->m_pResaCmpt->Cmpt();
				zMsg += "1(";
				longToString.Format("%ld", pCmptBkt->m_CmptBktLineArray[1]->m_lAuthNew);
				zMsg += longToString;
				zMsg += ")";
			}

		}
		// DM7978 - LME - FIN
	}

	// Au terme des travaux, un message CC0 != CC1 est à afficher
	// avec un ou deux CMPT en cause
	if (!zMsg.IsEmpty() && bModified==TRUE)
	{
		AfxMessageBox(zMsg, MB_OK);
		bErreur = TRUE;
	}
	return bErreur;
}
// Update the database with all authorization (and demand) values which have changed
// BUGI 69972 : lors du report d'autorisations par copier-collé d'un leg à l'autre,
// il faut vérifier que CC0= cC1 (sous couvert AVT 17 - date bang)
// extraction code pour en faire CResaRailView::ControleEgaliteCC0CC1
//
BOOL CResaRailView::UpdateBuckets( int AUorAS, YmIcLegsDom* pLeg /* = NULL */, 
																	enum CLegView::LegUpdateMode eUpdateMode,
																	BOOL bForceAuthMod /* = FALSE */, 
																	BOOL bConfirm /* = TRUE */ )
{
	//  Note that the bForceAuthMod parameter is used to "force" an update of the authorizations.
	//  During a normal update, the currently loaded leg is the leg which is being updated.
	//  But, if the bForceAuthMod parameter is set TRUE, this means that the values for the currently
	//  loaded leg are being used to update ANOTHER leg (for example, the currently selected leg).
	//  If this is the case, we must be careful about the "modified" flags, because if we change these flags,
	//  we are changing them for the currently loaded leg, and when it comes time to update the currently
	//  loaded leg, these flags will not be in a proper state.

	if (((CCTAApp*)APP)->GetRhealysFlag())
		return FALSE;

	CCTADoc *pDoc = ((CCTAApp*)APP)->GetDocument();
	// DM 7978 : DT 13903 - avt 17
	// Ne controler CC0 == CC1 que si la date gang est atteinte
	bool controlerCC0EgalCC1 = pDoc->m_pGlobalSysParms->GetControlerCC0EgalCC1();

	if ( pLeg == NULL )  // was a leg record passed in ?
		pLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);  // no, set the Leg Origin from the Leg record
	int iVnauSentStatus = 0; 

	BOOL bModified = FALSE;  // was at least one bucket value modified?
	// do we have some data ?BOOL bModified = FALSE;  // was at least one bucket value modified?
	if( (AUorAS %2 == 1) && (GetCmptBktMap()->GetCount() != 0) )
	{
		BOOL bErreur = ControleEgaliteCC0CC1( pLeg, 
									   eUpdateMode,
									   bForceAuthMod,
									   bModified);
		// Erreur bloquante rencontrée (CC0<>CC1), on sort
		if (bErreur == TRUE)
		{
			return FALSE;
		}
	}

	BOOL bSciModified = FALSE;  // was at least one bucket value modified?
	// do we have some SCI or SCG modified
	if( (AUorAS >= 2) && (GetCmptBktSciMap()->GetCount() != 0 ))
	{
		// First, we determine if there were any modifications made by the user
		POSITION pos = GetCmptBktSciMap()->GetStartPosition();
		while( pos != NULL &&  ! bSciModified ) // loop through all compartments until we find mods
		{
			CString sKey;
			CResaCmptBktSci* pCmptBkt;
			GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCmptBkt );
			YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;
			if (pCmptBkt->GetAuthModify())
				bSciModified = TRUE;

			// iterate through all the buckets for this compartment
			for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				if ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetAuthModify() || bForceAuthMod )
				{
					bSciModified = TRUE;
					break;
				}
			}
		}
	}

	CString szMsg;
	CString szMsgVnas;
	CString SuiteVnas;
	// Did user change at least one value OR does this leg need to be sent
	if ( bModified || bSciModified ||( pLeg->EditStatus() & EDIT_STATUS_AUTHMODNOTSENT ) )
	{
		if (!OverBookingTotalOK())
			return FALSE;
		// Build the VNAU command...
		YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
		BOOL bCML = IsCrossMidnightLeg (pTranche, pLeg);
		if ( bModified ||( pLeg->EditStatus() & EDIT_STATUS_AUTHMODNOTSENT ) )
			szMsg = BuildVnau( pLeg, NULL, bCML );
		if (bSciModified)
		{
			szMsgVnas = BuildVnas (pLeg, bCML, SuiteVnas);

			writeLog("UpdateBuckets(before):"+szMsgVnas);

			CtrlScgVide(szMsgVnas, SuiteVnas);

			writeLog("UpdateBuckets(after):"+szMsgVnas);
		}
		iVnauSentStatus = -2;
		if (bConfirm)
		{
			CString zMsg, ss;
			if (ControlActDiffMoi(AUorAS))
			{
				zMsg = ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_ACT_DIFF_MOI );
				zMsg += '\n';
				zMsg += szMsg;
			}
			else
			{
				ss = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_CONFIRM_VNAU);
				zMsg.Format (ss, szMsg);
			}
			if (!szMsgVnas.IsEmpty())
				zMsg += '\n' + szMsgVnas;
			if (!SuiteVnas.IsEmpty())
				zMsg += '\n' + SuiteVnas;
			if (AfxMessageBox (zMsg, MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL) == IDNO)
			{
				iVnauSentStatus = 0;
				// Authorizations were modified but not sent to Resa Rail
				// NBN ANO 92387 - EDIT_STATUS_OKAY en ca ou les VNAU/VNAS ne sont pas envoyés	
				pLeg->EditStatus( pLeg->EditStatus() | EDIT_STATUS_OKAY);
				pTranche->EditStatus( pTranche->EditStatus() | EDIT_STATUS_OKAY);
				pLeg->SentFlag(ChangeSentFlag(0, pLeg->SentFlag()));  // user did not send to ResaRail
				pTranche->SentFlag(ChangeSentFlag(0, pTranche->SentFlag()));
				((CCTAApp*)APP)->UpdateOneLeg (pLeg);
			}
		}
	}

	BOOL bVnauSent = FALSE;
	BOOL bVnasSent = FALSE;
	if (iVnauSentStatus == -2)
	{
		if ( bModified ||( pLeg->EditStatus() & EDIT_STATUS_AUTHMODNOTSENT ) )
			bVnauSent = (SendVnau ( szMsg, pLeg, NULL, FALSE ) > 0);
		if (bSciModified)
		{
			writeLog("UpdateBuckets : " +szMsgVnas);
			bVnasSent = (SendVnas ( szMsgVnas, SuiteVnas, pLeg, FALSE ) > 0);
		}
	}

	if (YaSciClicked())
	{
		if (SendVnae ()) // construit les VNAE, demande confirmation et envoi si OK
			UpdateSciEtanche(); // met a 0 l'indicateur d'étanchéité pour tous les sc_scx_authorisations de la desserte.
	}

	// Now, we update the database with the auth and dmd values at bucket level
	CString sLegOrig = pLeg->LegOrig();
	// NBN - Anomalie 92387
	if (bModified && iVnauSentStatus !=0)
		UpdateBkt (bVnauSent, bForceAuthMod, sLegOrig);
	// NBN - Anomalie 92387
	if (bSciModified && iVnauSentStatus !=0)
		UpdateSci (bVnasSent, sLegOrig);
	return bModified || bSciModified;
}

void CResaRailView::UpdateBkt(BOOL bVnauSent, BOOL bForceAuthMod, CString sLegOrig)
{
	// do we have some data ?
	if( GetCmptBktMap()->GetCount() != 0 )
	{
		POSITION pos = GetCmptBktMap()->GetStartPosition();  // start at the beginning of the cmpt map
		while( pos != NULL )
		{
			CString sKey;
			CResaCmptBkt* pCmptBkt;
			GetCmptBktMap()->GetNextAssoc( pos, sKey, pCmptBkt );

			// iterate through all the buckets for this compartment
			YmIcResaBktDom* pResaBkt;
			for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
				YmIcResaBktDom* pResaBktBis = new YmIcResaBktDom(*pResaBkt);
				pResaBktBis->LegOrig( sLegOrig );
				pResaBktBis->AuthUser( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew );

				if ( bVnauSent)  // Vnau was sent to ResaRail, so keep AuthCur "current" with ResaRail
					pResaBktBis->AuthCur( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew );

				if ( ! bForceAuthMod )  // do not set the dmd fcst levels if bForceAuthMod (copier coller parm) is TRUE
					SetAddDmdFcUser( pResaBktBis, pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAddDmdNew );

				// was an auth value modified by the user OR do we FORCE the modification ?
				// if ( ! ( eUpdateMode == CLegView::ALL_LEGS_OVERBOOKING_LEVEL_ONLY && iBktIdx != 0 ) )
				if ( bForceAuthMod || pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetAuthModify() )
				{
					YmIcResaBktDom* pResaBktBis2 = new YmIcResaBktDom(*pResaBktBis);
					YM_Query* pUpdateQuery = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), TRUE);
					pUpdateQuery->Start( pResaBktBis2, IDS_SQL_UPDATE_RESA_BKT_AUTH );
				}
				delete pResaBktBis;

				// reset flags which indicate changed values for this line
				//
				// Note that we do not reset the "auth values were modified" flag if the eUpdateMode is CURRENTLY_LOADED_LEG,
				//    because in that case, we are NOT updating the auth values for the currently loaded leg, rather we are updating
				//    the auth values for a leg OTHER THAN the currently loaded leg.  This means that, if we reset the auth modified
				//    flag to TRUE here, we never know whether the currently loaded leg's auth values were actually modified.
				// if (eUpdateMode == CLegView::CURRENTLY_LOADED_LEG || eUpdateMode == CLegView::ALL_LEGS)
				pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->SetAuthModify( FALSE );
			}
		}
	}
}

// Update the database with all SCI and SCG values which have changed
//
void CResaRailView::UpdateSci(BOOL bVnasSent, CString sLegOrig)
{
	// do we have some data ?
	if( GetCmptBktSciMap()->GetCount() != 0 )
	{
		// Now, we update the database with thes auth and dmd values at bucket level
		POSITION pos = GetCmptBktSciMap()->GetStartPosition();  // start at the beginning of the cmpt map
		while( pos != NULL )
		{
			CString sKey;
			CResaCmptBktSci* pCmptBkt;

			GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCmptBkt );
			YmIcResaCmptDom* pResaCmptBis = new YmIcResaCmptDom (*(pCmptBkt->m_pResaCmpt));
			pResaCmptBis->LegOrig(sLegOrig);
			CString sCmpt = pResaCmptBis->Cmpt();
			sCmpt.MakeUpper();
			pResaCmptBis->Cmpt((LPCSTR)sCmpt);
			YM_Query* pUpdateQueryCmpt = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), TRUE);
			pUpdateQueryCmpt->Start( pResaCmptBis, IDS_SQL_UPDATE_RESA_CMPT_SCI );
			pCmptBkt->SetAuthModify(FALSE);

			// iterate through all the buckets for this compartment
			YmIcResaBktDom* pResaBkt;

			for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
				YmIcResaBktDom* pResaBktBis = new YmIcResaBktDom(*pResaBkt);
				pResaBktBis->Cmpt((LPCSTR)sCmpt);
				pResaBktBis->NestLevel(pResaBktBis->NestLevel());

				// fill "where" columns for the update statement
				pResaBktBis->LegOrig( sLegOrig );
				pResaBktBis->AuthUser( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew );
				pResaBktBis->AuthCur( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew );

				// create and start the query to update the database
				YmIcResaBktDom* pResaBktBis2 = new YmIcResaBktDom(*pResaBktBis);
				YM_Query* pUpdateQuery = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), TRUE);
				pUpdateQuery->Start( pResaBktBis2, IDS_SQL_UPDATE_RESA_BKT_AUTH_SCI );
				pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->SetAuthModify( FALSE );
			} 
		}
	}  // if count != 0
}

// Update the database to remove etanche flag where needed, 
void CResaRailView::UpdateSciEtanche()
{
	// do we have some data ?
	if( GetCmptBktSciMap()->GetCount() != 0 )
	{
		int iKeyIdx = 0;
		BOOL yenaeu = FALSE;
		BOOL yaetanche = FALSE;
		CString sKey;
		YM_Query* pSQL = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
		while( iKeyIdx < m_CmptBktSciMap.m_aszKey.GetSize() )
		{
			CResaCmptBktSci* pCmptBkt = NULL;
			sKey = m_CmptBktSciMap.m_aszKey.GetAt(iKeyIdx++);
			m_CmptBktSciMap.Lookup( sKey, pCmptBkt );
			if (pCmptBkt)
			{
				YmIcResaBktDom* pResaBkt;
				for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
				{
					pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
					if (pResaBkt->Clicked())
					{
						pResaBkt->ScxEtanche(0);
						pSQL->SetDomain(pResaBkt);
						((CCTAApp*)APP)->m_pDatabase->Transact(pSQL, IDS_SQL_UPDATE_ETANCHE_SCI);
						pResaBkt->Clicked(FALSE);
						yenaeu = TRUE;
					}
					if (pResaBkt->ScxEtanche())
						yaetanche = TRUE;
				}
			}
		}
		delete pSQL;

		if (yenaeu && !yaetanche)
		{ // plus de scx etanche, il faut lever l'indicateur au niveau de la tranche.
			YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
			pTranche->ScxEtanche(0);
		}
	} // if count != 0
}


BEGIN_MESSAGE_MAP(CResaRailView, CScrollView)
	//{{AFX_MSG_MAP(CResaRailView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TCN_SELCHANGE, IDC_RESARAIL_TABCTRL, OnChartSelchange)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SWITCHFOCUS, OnSwitchFocus)
	ON_COMMAND(ID_PRINT, CView::OnFilePrint) 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResaRailView drawing

void CResaRailView::OnDraw(CDC* pDC)
{
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();//DM 8029
	

	BOOL Flag_BuildOK = pDoc->m_pGlobalSysParms->GetYieldRevenuFlag();

	CFont* pFontSaved = pDC->SelectObject( pDoc->GetStdFont(pDC) );
	COLORREF clrFtCC, clrFtSci, clrPrevision;
	clrFtCC = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkMain();
	clrFtSci = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorFontRecBkAlt();
	clrPrevision = pDoc->m_pGlobalSysParms->GetGraphRGBValue(CCTAChart::ePrevisionStyleIdx);

	//DM7978 - LME - YI-5401 - ajout des onglets global/reduit
	m_TabCtrl.SetFont( pDoc->GetTitleFont(pDC) );
	LONG offsetY = 0L;
	LONG lYSciDisplayed = 0L;
	int nbCc_Reduit = pDoc->m_pGlobalSysParms->GetNbCcVisu();
	int nbSci_Reduit = pDoc->m_pGlobalSysParms->GetNbScVisu();
	//DM7978 - LME - FIN

	// DM7978 - LME - coloration sphère de prix
	COLORREF curColor = RGB(255, 255, 255);

	CResaCmptBktLine* pCmptBktLine;
	UINT nID;

	// Associe le font a la zone d'edition
	// TDR - Anomalie 79307
	bool bModifFont = false;
	POSITION pos = m_EditMap.GetStartPosition();
	while (pos)
	{
		m_EditMap.GetNextAssoc( pos, nID, pCmptBktLine );
		if (pCmptBktLine->m_pEditAuthNew != NULL)
		{
			if (pCmptBktLine->m_pEditAuthNew->GetFont() != pDoc->GetStdFont(pDC))
			{
				bModifFont = true;
			}

			pCmptBktLine->m_pEditAuthNew->SetFont( pDoc->GetStdFont(pDC) );
		}
		if (pCmptBktLine->m_pEditAddDmdNew != NULL)
		{
			if (pCmptBktLine->m_pEditAddDmdNew->GetFont() != pDoc->GetStdFont(pDC))
			{
				bModifFont = true;
			}
			pCmptBktLine->m_pEditAddDmdNew->SetFont( pDoc->GetStdFont(pDC) );
		}
	}

	pos = m_CmptBktMap.GetStartPosition();
	CString sKey;
	CResaCmptBkt* pCmptBkt;
	while (pos)
	{
		m_CmptBktMap.GetNextAssoc( pos, sKey, pCmptBkt );
	}

	TEXTMETRIC  tm;
	pDC->GetTextMetrics( &tm );


	// CME - HPQC 3864 - Gestion des couleurs de fond
	// Prise en compte du nombre de classes de controles et du nombre de sous contingents
	int sizeCC = 1;
	if( m_CmptBktMap.GetCount() != 0 )
	{
		std::map <int, CmptOrdre>& showOrder = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetCmptShowOrder();
		for(unsigned int i = 0; i < showOrder.size(); i++)
		{
			CResaCmptBkt* pCmptBkt = NULL;
			sKey = showOrder[i].cmpt;
			m_CmptBktMap.Lookup( sKey, pCmptBkt );
			if(pCmptBkt)
			{
				int vCCSize = pCmptBkt->m_CmptBktLineArray.GetSize();
				if(sizeCC < vCCSize)
					sizeCC = vCCSize;
			}		
		}
	}

	int sizeSCX = 1;
	if (m_CmptBktSciMap.GetCount())
	{
		std::map<int, CmptOrdre>& showOrderScx = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetCmptScxShowOrder();
		for(unsigned int i = 0; i < showOrderScx.size(); i++)
		{
			CResaCmptBktSci* pCmptBkt = NULL;
			sKey = showOrderScx[i].cmpt;
			m_CmptBktSciMap.Lookup( sKey, pCmptBkt );
			if(pCmptBkt)
			{
				int vSCXSize = pCmptBkt->m_CmptBktLineArray.GetSize();
				if(sizeSCX < vSCXSize)
					sizeSCX = vSCXSize;		
			}
		}
	}	


	// if record set is valid and we have data 
	if( m_CmptBktMap.GetCount() != 0 )
	{
		//DM7978 - LME - YI-6046 - recuperation de l'association B0/AX (resp. D0/CX) pour l'ascenseur tarifaire
		long ccPremiere = m_ascTarifaire;
		//DM7978 - LME - FIN

		CRect rect;
		GetClientRect(rect);
		COLORREF clrBk = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();
		COLORREF clrAlt= ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt();
		// DM7978 - LME - FIN
		//TSE_Ano 79307 - BEGIN 
		//pDC->SetBkColor(clrBk);
		//pDC->SetBkColor(clrAlt);	
		if (!GetEtatWindow())
		{		
			rectDoGlob.left = rect.left;
			rectDoGlob.right = rect.right;

			rectUpGlob.left = rect.left;
			rectUpGlob.right = rect.right;

			
			if (m_ChangedWindowGlobal)
			{
				// CME - HPQC 3864 - Gestion des couleurs de fond
				rectUpGlob.bottom  = sizeCC * (tm.tmHeight + tm.tmExternalLeading) + (4*OFFSETY);
				rectDoGlob.top = rectUpGlob.bottom;
				rectDoGlob.bottom = rectDoGlob.top + ( sizeSCX * (tm.tmHeight + tm.tmExternalLeading)) + (4*OFFSETY);
				if( rectDoGlob.bottom < rect.bottom)
					rectDoGlob.bottom = rect.bottom;

				// CME Ano 79307 Passage à false supprimé pour permettre le raffraichissement
				//m_ChangedWindowGlobal = FALSE;
			}
			//CPoint ptz = GetScrollPosition();
			//rectUp += ptz;
			pDC->FillSolidRect(rectUpGlob, clrBk);
			pDC->FillSolidRect(rectDoGlob, clrAlt);
		}
		else
		{
			rectUpRed.left = rect.left;
			rectUpRed.right = rect.right;

			rectDoRed.left = rect.left;
			rectDoRed.right = rect.right;
			
			
			if (m_ChangedWindowReduit)
			{   
				rectUpRed = rect;

				// CME - HPQC 3864 - Gestion des couleurs de fond
				if( nbCc_Reduit < sizeCC)
					sizeCC = nbCc_Reduit;
				rectUpRed.bottom  = sizeCC * (tm.tmHeight + tm.tmExternalLeading) + (3*OFFSETY);
				rectDoRed.top = rectUpRed.bottom;

				if( nbSci_Reduit < sizeSCX)
					sizeSCX = nbSci_Reduit;
				rectDoRed.bottom = rectDoRed.top + ( sizeSCX * (tm.tmHeight + tm.tmExternalLeading)) + (3*OFFSETY);
                 
				 // CME Ano 79307 Passage à false supprimé pour permettre le raffraichissement
				//m_ChangedWindowReduit = FALSE;
			}
			

			pDC->FillSolidRect(rectUpRed, clrBk);
			pDC->FillSolidRect(rectDoRed, clrAlt);
		}
		//TSE_Ano 79307 - END
		if (m_CmptBktSciMap.GetCount())
		{ // Il y a des données Sci
			CSize sz = GetTotalSize();
			//TSE - Ano 79307
			CRect rcAlt (sz.cx , 0, sz.cx, sz.cy);//Ligne modifiée par TSE
			COLORREF clrAlt= ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt();
			pDC->FillSolidRect(rcAlt, clrAlt);
			pDC->SetBkColor(clrBk);
		}
		else { //CME Ano 79307 
			
			pDC->SetBkColor(clrBk);
		}

		CPen* pOldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);

		CPoint pt(0,0 + TABCTRL_HEIGHT); // point permettant de positionner le "crayon"
		CString s;
		int iTotalDiffRes = 0;
		int iTotalResHold = 0;
		long iTotalRecette = 0;
		long iTotalRevTick = 0;

		BOOL bConditionAffichageReduitCc = TRUE;
		BOOL bConditionAffichageReduitSc = TRUE;

		//
		// Affichage des informations CC
		//

		//DM7978  FIX # ano hpqc 1066  KGH
		std::map <int, CmptOrdre>& showOrder = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetCmptShowOrder();
		//const CArray <CString, CString&>& showOrder = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetCmptShowOrder();
		CString sKey;

		// Boucle sur les cmpts
		//for(int i = 0; i < showOrder.GetSize(); i++) //DM7978 - LME - YI-5401
		for(unsigned int i = 0; i < showOrder.size(); i++) //DM7978 - LME - YI-5401
		{
			CResaCmptBkt* pCmptBkt = NULL;
			iTotalRecette = 0;
			iTotalRevTick = 0;
			//sKey = showOrder.GetAt(i); //DM7978 - LME - YI-5401
			sKey = showOrder[i].cmpt; //DM7978 - LME - YI-5401
			//FIN DM7978  FIX # ano hpqc 1066  KGH
			m_CmptBktMap.Lookup( sKey, pCmptBkt );

			//DM7978 - LME - YI-6404 - ascenseur tarifaire 
			if(IsModeGlobal() && i == 0)
			{
				pt.y += ccPremiere* tm.tmHeight;
			}
			//DM7978 - LME - FIN

			if ( pCmptBkt)//DM7978  FIX # ano hpqc 1066  KGH
			{
				CSize sizeField( tm.tmAveCharWidth * 6, tm.tmHeight + tm.tmExternalLeading );
				CPoint ptOffset( tm.tmAveCharWidth * 7, 0 );
				YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;
				if ( pResaCmpt )
				{
					pt.y +=  tm.tmHeight + tm.tmExternalLeading; 
					
					CRect rect( pt, sizeField );
					// Affichage des labels
					int kl = ( m_pVnl && m_pVnlLeg && ! m_bVnlResent ) ? 3 : 2;  // additional column for res sys difference
					// For the TRN and Virgin clients' ResaRail Window, we display only 2 columns of Authorizations, else, we display 3..
					int iColOffset = 4;

					// Cmpt
					s = pResaCmpt->Cmpt();
					rect = CRect( pt, sizeField );
					pDC->DrawText( s, rect, DT_RIGHT|DT_SINGLELINE ); //Numéro de cmpt

					// Labels
					rect += ptOffset;
					DrawLabel( pDC, IDS_SEATS_SOLD, rect, TRUE ); // Label Vdu
					if ( m_pVnl && m_pVnlLeg && ! m_bVnlResent )
					{
						rect += ptOffset;
						DrawLabel( pDC, IDS_RES, rect, TRUE ); // Label Res
					}

					if (m_bPrevisionOK)
					{
						rect += ptOffset;
						COLORREF crSaved = pDC->SetTextColor( clrPrevision ); 
						DrawLabel( pDC, IDS_PREV, rect, TRUE ); // Label Prev (pour prevision)
						pDC->SetTextColor( crSaved );
					}

					rect += ptOffset;
					DrawLabel( pDC, IDS_CUR, rect, TRUE );  // Label Act

					rect += ptOffset;
					DrawLabel( pDC, IDS_NEW, rect, TRUE ); // Label colonne Moi
					rect += ptOffset;
					DrawLabel( pDC, IDS_AVAIL, rect, TRUE ); // Label colonne Dispo
					/* DM 5350 Rajout du label de la colonne € */
					if ( Flag_BuildOK )
					{
						COLORREF crSaved = pDC->SetTextColor( clrFtCC ); 
						rect += ptOffset;
						DrawLabel( pDC, m_bPanierMoyen ? IDS_EURO_PM : IDS_EURO, rect, TRUE ); // IDS_EURO Colonne Recette
						pDC->SetTextColor( crSaved );
					}
					/* DM 5350 */

					if (m_bPrevisionOK)
					{
						rect += ptOffset;
						COLORREF crSaved = pDC->SetTextColor( clrPrevision ); 
						DrawLabel( pDC, IDS_EURO, rect, TRUE ); // Label €
						pDC->SetTextColor( crSaved );
					}

					if (m_bVnauExcluOK)
					{
						rect += ptOffset;
						COLORREF crSaved = pDC->SetTextColor( clrPrevision ); 
						CFont* pFont = pDC->SelectObject(pDoc->GetTitleFont(pDC));
						DrawLabel( pDC, IDS_AUTO_NOMOS, rect, TRUE ); // Label "Auto N"
						rect += ptOffset;
						DrawLabel( pDC, IDS_DISPO_NOMOS, rect, TRUE ); // Label "Dispo N"
						pDC->SetTextColor( crSaved );
						pDC->SelectObject(pFont);
					}

					pt.y +=  tm.tmHeight + tm.tmExternalLeading + OFFSET_UNDERLINE;

					rect += CPoint( 0, tm.tmHeight + tm.tmExternalLeading + OFFSET_UNDERLINE );

					double dAddDmdFcTot = 0;
					double dAddDmdNewTot = 0;
					long PrevisionResa = -1;
					long PrevisionRecette = -1;
					long lResHoldTot = pCmptBkt->m_lResHoldTot;

					// Affichage de l'indicateur de sous-contingent
					YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
					if ( pTranche->ScxCount() > 0)
					{
						int scxcount, scxetanche;
						GetSciCountEtanche (pResaCmpt->Cmpt()[0], scxcount, scxetanche);
						if (scxcount)
						{
							CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
							if (pTrancheView)
							{
								CImageList* pImageList = pTrancheView->GetVnasImageList();
								int bmpid = BMP_CCM1_SEUL;
								if (scxetanche)
									bmpid += 8;
								if (GetScgAuth (pResaCmpt->Cmpt()[0]) == 0)
									bmpid += 16;
								DrawIconEx (pDC->m_hDC, 5 + pt.x, pt.y, pImageList->ExtractIcon(bmpid),
									iBitMapSizeConst, iBitMapSizeConst, 0, NULL, DI_NORMAL );
							}
						}
					}

					//DM7978 - LME - YI-5405 - filtrage des CC a afficher en onglet reduit
					// Stockage des indices des CC a afficher en onglet reduit
					CCmptFilteredArray indexLoop;
					indexLoop.RemoveAll();

					// On masque toutes les CEdit par bucket pour les afficher ensuite selon le filtrage...
					for(int j = 0; j < pCmptBkt->m_CmptBktLineArray.GetSize(); j++)
					{
						CResaRailViewEdit* pEdit = pCmptBkt->m_CmptBktLineArray[j]->m_pEditAuthNew;
						// NPI - Ano 70141
						//pEdit->ShowWindow(SW_HIDE);
						if (!((CCTAApp*)APP)->GetRhealysFlag())
						{
							pEdit->ShowWindow(SW_HIDE);
						}
						///
					}

					// Initilisation du tableau d'indice a parcourir pour l'onglet reduit
					if(IsModeGlobal())
					{
						for( int j = 0; j < pCmptBkt->m_CmptBktLineArray.GetSize(); j++ )
						{
							indexLoop.Push(j);
						}
					}
					else if(IsModeReduit())
					{
						CmptFilteringRule(showOrder[i].cmpt, indexLoop);//KGH ano 1066
						//CmptFilteringRule(showOrder.GetAt(i), indexLoop);
					}
					//DM7978 - LME - FIN

					// Bucket Level Data
					//
					YmIcResaBktDom* pResaBkt = NULL;
					for( int k = 0; k < indexLoop.GetSize(); k++ )
					{
						int iBktIdx = indexLoop.GetAt(k);
						pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
						BOOL bUnderline = k == ( indexLoop.GetSize() - 1 );
						CRect rectCtrl;

						// DM7978 - LME - YI-5415 - recuperation des spheres de prix
						// NPI - Ano 70141
						/*CSpherePrix spheresPrixCc = pDoc->m_sphereManager->findSpherePrixCC(pResaBkt->NestLevel(), pResaBkt->Cmpt(),pTranche->Nature(),pTranche->Entity()); //chercher la sphere qui correspond a NestLevel et Cmpt

						curColor = spheresPrixCc.getColor(true);//appliquer la couleur pastel*/

						if (!((CCTAApp*)APP)->GetRhealysFlag())
						{
							CSpherePrix spheresPrixCc = pDoc->m_sphereManager->findSpherePrixCC(pResaBkt->NestLevel(), pResaBkt->Cmpt(),pTranche->Nature(),pTranche->Entity()); //chercher la sphere qui correspond a NestLevel et Cmpt
							curColor = spheresPrixCc.getColor(true);//appliquer la couleur pastel
						}
						///

						// create and select a solid blue brush
						CBrush brush(curColor);
						CBrush* pOldBrush = NULL;
						// create and select a thick, black pen
						CPen pen;
						pen.CreatePen(PS_SOLID, 1, curColor);
						CPen* pOldPen = NULL;
						// DM7978 - LME - FIN

						// NestLevel (Bucket)
						rect = CRect( pt, sizeField );
						// DM7978 - LME - YI-5415 
						// On decale le cote gauche du rectangle sur la droite pour ne pas masquer l'indicateur de sous-contingent
						rect.left += 18;
						DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
						// On enleve le decalage du rectangle
						rect.left -= 18;
						// DM7978 - LME - FIN

						// Numéro de la classe de controle
						pDC->DrawText( pResaBkt->BucketLabel(), rect, DT_RIGHT|DT_SINGLELINE );
						rect += ptOffset;

						// ResHoldTot (SS)
						if ((!pResaBkt->ResHoldIdvIsNull()) && (!pResaBkt->ResHoldGrpIsNull()))  // draw this field if the database value is not NULL
						{
							// DM7978 - LME - YI-5415 
							// dessine un rectangle de taille et position rect
							rect.left -= 5;
							DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
							rect.left += 5;
							// DM7978 - LME - FIN
							COLORREF crSaved;
							if (pDoc->GetNotFullJX())
								crSaved = pDC->SetTextColor( RGB(0,0,0) ); // DM7978 - LME - YI5401 - affichage en noir
							if (((CCTAApp*)APP)->GetRhealysFlag() && pResaBkt->NestLevel())
								pDC->DrawText("--", rect, DT_RIGHT|DT_SINGLELINE);
							else
								DrawLongValue(pDC, pResaBkt->ResHoldIdv()+pResaBkt->ResHoldGrp()+pResaBkt->ResHoldNotYield(), rect, bUnderline ); // colonne Vdu 
							if (pDoc->GetNotFullJX())
								pDC->SetTextColor( crSaved );
						}

						// Difference in ResHoldTot from ResaRail (Res)
						// Do not draw this COLUMN if the VNL object is NULL
						if ( m_pVnl && m_pVnlLeg && ! m_bVnlResent )
						{
							CString szText;
							if ( m_pVnl->IsNetNesting() )  // For Net Nesting Inventory Method, we display Seats Sold at the Bucket level
							{
								YM_VnlBkt* pVnlBkt = FindVnlCmptBkt( pResaCmpt->Cmpt(), pResaBkt->NestLevel() );
								if ( ! pVnlBkt )  // couldn't find this cmpt/bkt in the Vnl results
								{
									szText.Format("+?");
								}
								else
								{
									// calculate the difference between ResaRail and Database ResHold values
									if ((!pResaBkt->ResHoldIdvIsNull()) && (!pResaBkt->ResHoldGrpIsNull()))  // draw this field if the database value is not NULL
									{
										int iDiff = pVnlBkt->GetBaseSold() - (pResaBkt->ResHoldIdv()+pResaBkt->ResHoldGrp());//104151
										szText.Format("%+d", iDiff);
										iTotalDiffRes += iDiff;
									}
									else
									{
										szText.Empty();
									}
								}
							}
							else    // For Threshold Nesting Inventory Method, we display Seats Sold at the Compartment Level
							{
								// accumulate ResHold
								iTotalResHold += (pResaBkt->ResHoldIdv()+pResaBkt->ResHoldGrp()+pResaBkt->ResHoldNotYield());
							}
							rect += ptOffset;
							// DM7978 - LME - YI-5415 
							// dessine un rectangle de taille et position rect
							rect.left -= 5;
							DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
							rect.left += 5;
							// DM7978 - LME - FIN
							if (((CCTAApp*)APP)->GetRhealysFlag() && pResaBkt->NestLevel())
								pDC->DrawText("--", rect, DT_RIGHT|DT_SINGLELINE);
							else
								pDC->DrawText( szText, rect, DT_RIGHT|DT_SINGLELINE ); // colonne Res
							if( bUnderline )
								Underline( pDC, rect );
						}

						CResaRailViewEdit* pEdit = NULL;

						if (m_bPrevisionOK)
						{
							rect += ptOffset;
							// Modif 26/11/2013 suite aux test NOMOS sur demande JM Grugeaux
							// fait par HRI (premier paramètre à -1 au lieu de 0)
							int lprevresa = GetRrdPrevisionResa (-1, pResaCmpt->Cmpt(), pResaBkt->NestLevel());
							COLORREF crSaved = pDC->SetTextColor( clrPrevision ); 
							if (lprevresa >= 0)
							{
								// DM7978 - LME - YI-5415 
								// dessine un rectangle de taille et position rect
								rect.left -= 5;
								DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
								rect.left += 5;
								// DM7978 - LME - FIN
								DrawLongValue( pDC, lprevresa, rect, bUnderline );
								if (PrevisionResa == -1)
									PrevisionResa = lprevresa;
								else
									PrevisionResa += lprevresa;
							}
							else
							{
								pDC->DrawText( "--", rect, DT_RIGHT|DT_SINGLELINE );
								if( bUnderline )
									Underline( pDC, rect );
							}
							pDC->SetTextColor( crSaved );
						}

						// AuthCur (Cur)
						YM_VnlBkt* pVnlBkt = FindVnlCmptBkt( pResaCmpt->Cmpt(), pResaBkt->NestLevel() );
						long lAuthCur = ( pVnlBkt == NULL ) ? pResaBkt->AuthCur() : pVnlBkt->GetAuth() ;
						//DM 6496 test sur difference entre AuthCur du VNL et AuthCur lu en base
						if (pVnlBkt && (pVnlBkt->GetAuth() != pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthInit))
						{ // ya une différence, on memorise la lettre de l'espace physique
							if (m_CActMoi.Find (pResaCmpt->Cmpt()) < 0)
								m_CActMoi += pResaCmpt->Cmpt();
						}
						rect += ptOffset;
						if (!pResaBkt->AuthCurIsNull())  // draw this field if the database value is not NULL
						{
							// DM7978 - LME - YI-5415 
							// dessine un rectangle de taille et position rect
							rect.left -= 5;
							DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
							rect.left += 5;
							// DM7978 - LME - FIN
							if (((CCTAApp*)APP)->GetRhealysFlag() && pResaBkt->NestLevel())
								pDC->DrawText("--", rect, DT_RIGHT|DT_SINGLELINE);
							else
								DrawLongValue( pDC, lAuthCur, rect, FALSE ); // colonne Act 4
						}

						// AuthUser(New) -> Edit (New)
						rect += ptOffset;
						if (pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew != NULL)  // ensure this edit control is not NULL
						{
							// DM7978 - LME - YI-5415 
							// dessine un rectangle de taille et position rect
							rect.left -= 5;
							DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
							rect.left += 5;
							// DM7978 - LME - FIN
							pEdit = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew;
							// DM7978 - LME - coloration du CEdit
							pEdit->SetBackColor(curColor);
							// DM7978 - LME - correction du bug de la colonne Moi grisée lorsque le dptDate < date 
							// si le CEDit n'est pas en lecture seule on l'affiche
							if(!m_FlagReadOnly)
							{
								// NPI - Ano 70141
								//pEdit->ShowWindow(SW_SHOW);
								if (!((CCTAApp*)APP)->GetRhealysFlag())
								{
									pEdit->ShowWindow(SW_SHOW);
								}
								///
							}
							// sinon on affiche un label
							else
							{
								// OBA - Ano 70141 - problem was still existing for nature = 3
								if (((CCTAApp*)APP)->GetRhealysFlag() && iBktIdx > 0)
								{
									pEdit->ShowWindow(SW_HIDE);
								}
								else
								{
									pDC->SetTextColor(RGB(0,0,255));
									DrawLongValue( pDC, pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthInit , rect, bUnderline );
									pDC->SetTextColor(RGB(0,0,0));
								}
							}
							// DM7978 - LME - FIN
							rectCtrl = rect;
							pDC->LPtoDP( rectCtrl );
							pEdit->MoveWindow( rectCtrl );
							if( m_pWndChildFocusSaved == NULL )
							{
								m_pWndChildFocusSaved = pEdit;
								pEdit->SetFocus();
							}
							if (pDC->IsPrinting())
							{
								CString toto;
								pEdit->GetWindowText(toto);
								COLORREF crSaved = pDC->SetTextColor( RGB(0,0,255) );
								pDC->DrawText( toto, rect, DT_RIGHT|DT_SINGLELINE );
								pDC->SetTextColor( crSaved );
							}
						}
						// AuthFcAvail (Avail) (Calculated because AuthUserNew is editable)
						rect += ptOffset;

						// DM 5350, une seule colonne dispo, initialisé comme celle qui disparait, mais sur modification
						// on affiche les valeurs calculée a partir des autorisations, comme avant la DM 5350.
						if ( !Modified())
						{
							if (!pResaBkt->AvailabilityCurIsNull())  // draw this field if the database value is not NULL
							{
								// DM7978 - LME - YI-5415 
								// dessine un rectangle de taille et position rect
								rect.left -= 5;
								DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
								rect.left += 5;
								// DM7978 - LME - FIN
								if (((CCTAApp*)APP)->GetRhealysFlag() && pResaBkt->NestLevel())
									pDC->DrawText("--", rect, DT_RIGHT|DT_SINGLELINE);
								else
									DrawLongValue( pDC, pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailCur, rect, FALSE ); //Dispo
							}
						}
						else if (!pResaBkt->AvailabilityFcIsNull())  // draw this field if the database value is not NULL
						{ // colonne DISPO
							// DM7978 - LME - YI-5415 
							// dessine un rectangle de taille et position rect
							rect.left -= 5;
							DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
							rect.left += 5;
							// DM7978 - LME - FIN
							if (((CCTAApp*)APP)->GetRhealysFlag() && pResaBkt->NestLevel())
								pDC->DrawText("--", rect, DT_RIGHT|DT_SINGLELINE);
							else
								DrawLongValue( pDC, pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailUser, rect, FALSE );
						}

						rect += ptOffset;
						if (Flag_BuildOK)  // draw this field if the database value is not NULL
						{ // colonne Recette
							// DM7978 - LME - YI-5415 
							// dessine un rectangle de taille et position rect
							rect.left -= 5;
							DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
							rect.left += 5;
							// DM7978 - LME - FIN
							COLORREF crSaved = pDC->SetTextColor( clrFtCC );
							long ptgr = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetRecetteTempsReel(this, pVnlBkt ? pVnlBkt->GetBaseSold() : 0);
							if (ptgr < 0)
							{
								pDC->DrawText( "--", rect, DT_CENTER|DT_SINGLELINE);
							}
							else
							{
								long pp = ptgr;
								if (m_bPanierMoyen)
									pp = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetPanierMoyen(this, pVnlBkt ? pVnlBkt->GetBaseSold() : 0);
								if (((CCTAApp*)APP)->GetRhealysFlag() && pResaBkt->NestLevel())
									pDC->DrawText("--", rect, DT_RIGHT|DT_SINGLELINE);
								else
									DrawLongValue( pDC, pp, rect, bUnderline );
								iTotalRecette += ptgr;
							}
							long rece = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->Recette();
							if (rece > 0)
								iTotalRevTick += rece;
							pDC->SetTextColor( crSaved );
						}

						if (m_bPrevisionOK)
						{
							rect += ptOffset;
							// DM7978 - LME - YI-5415 
							// dessine un rectangle de taille et position rect
							rect.left -= 5;
							DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
							rect.left += 5;
							// DM7978 - LME - FIN
							int lprevrev = GetRrdPrevisionRevenu (0, pResaCmpt->Cmpt(), pResaBkt->NestLevel());
							COLORREF crSaved = pDC->SetTextColor( clrPrevision ); 
							if (lprevrev >= 0)
							{
								if (((CCTAApp*)APP)->GetRhealysFlag() && pResaBkt->NestLevel())
									pDC->DrawText("--", rect, DT_RIGHT|DT_SINGLELINE);
								else
									DrawLongValue( pDC, lprevrev, rect, bUnderline );
								if (PrevisionRecette == -1)
									PrevisionRecette = lprevrev;
								else
									PrevisionRecette += lprevrev;
							}
							else
							{
								pDC->DrawText( "--", rect, DT_RIGHT|DT_SINGLELINE );
								if( bUnderline )
									Underline( pDC, rect );
							}
							pDC->SetTextColor( crSaved );
						}

						if (m_bVnauExcluOK)
						{
							rect += ptOffset; 
							// DM7978 - LME - YI-5415 
							// dessine un rectangle de taille et position rect
							rect.left -= 5;
							DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
							rect.left += 5;
							// DM7978 - LME - FIN
							COLORREF crSaved = pDC->SetTextColor( clrPrevision ); 
							CFont* pFont = pDC->SelectObject(pDoc->GetTitleFont(pDC));
							if (((CCTAApp*)APP)->GetRhealysFlag() && pResaBkt->NestLevel())
								pDC->DrawText("--", rect, DT_RIGHT|DT_SINGLELINE);
							else
								DrawLongValue( pDC, pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNomos, rect, false );
							rect += ptOffset;
							if (((CCTAApp*)APP)->GetRhealysFlag() && pResaBkt->NestLevel())
								pDC->DrawText("--", rect, DT_RIGHT|DT_SINGLELINE);
							else
								DrawLongValue( pDC, pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailNomos, rect, false );
							pDC->SetTextColor( crSaved );
							pDC->SelectObject(pFont);
						}

						if (!iBktIdx && (pResaBkt->AuthUser() != lAuthCur))
						{
							YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
							if ( pTranche->Nature() && m_bOkOnRhea )
							{
								m_bOkOnRhea = FALSE;
								RWDate today;
								if (pResaBkt->DptDate() >= today)
								{
									CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RHEALYS_ACT_NE_MOI);
									AfxMessageBox( szText, MB_ICONEXCLAMATION | MB_APPLMODAL );
								}
							}
						}

						lResHoldTot -= (pResaBkt->ResHoldIdv()+pResaBkt->ResHoldGrp()+pResaBkt->ResHoldNotYield());

						pt.y +=  tm.tmHeight + tm.tmExternalLeading;
					}
					pt.y += OFFSET_UNDERLINE;

					for(int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++)
					{
						CResaRailViewEdit* pEdit = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew;
						pEdit->UpdateWindow();
					}

					// ResHoldTot (Total)
					rect = CRect( pt + ptOffset, sizeField );
					DrawLongValue( pDC, pCmptBkt->m_lResHoldTot, rect, FALSE ); 

					int sumventecmpt = pCmptBkt->m_lResHoldTot;
					if ( m_pVnl && m_pVnlLeg && ! m_bVnlResent )
					{
						if ( ! m_pVnl->IsNetNesting() ) // for Threshold Nesting, we must calculate Real-Time Res Hold difference at the cmpt level
						{
							YM_VnlCmpt* pVnlCmpt = FindVnlCmpt( pCmptBkt->m_pResaCmpt->Cmpt() );
							iTotalDiffRes = pVnlCmpt->GetBaseSold() - iTotalResHold;
						}
						rect += ptOffset;
						CString szText;
						szText.Format("%+d", iTotalDiffRes);
						pDC->DrawText( szText, rect, DT_RIGHT|DT_SINGLELINE );
						sumventecmpt += iTotalDiffRes;
						iTotalDiffRes = iTotalResHold = 0;
					}

					// calculate the column offset for Non-Demand Forecasting clients and Virgin
					iColOffset = 5;

					if (m_bPrevisionOK)
					{ // total prévision resa
						rect += ptOffset;
						COLORREF crSaved = pDC->SetTextColor( clrPrevision ); 
						if (PrevisionResa >= 0)
							DrawLongValue( pDC, PrevisionResa, rect, FALSE );
						else
							pDC->DrawText( "--", rect, DT_RIGHT|DT_SINGLELINE );
						pDC->SetTextColor( crSaved );
					}		

					CRect TotRevRect;  // rectangle contenant le revenu total
					if (Flag_BuildOK)  // draw this field if the database value is not NULL
					{ 
						// totale colonne Recette
						if ( iTotalRecette < 0)
						{
							pDC->DrawText( "--", rect, DT_CENTER|DT_SINGLELINE);
						}
						else
						{
							rect += ptOffset;
							rect += ptOffset;
							rect += ptOffset;
							rect += ptOffset;	
							CString szText;
							COLORREF crSaved = pDC->SetTextColor( clrFtCC );
							long totaff = iTotalRecette;
							if (m_bPanierMoyen)
							{
								int revscg = GetRealTimeSciRevenu (pResaCmpt->Cmpt()[0], -1);
								if (revscg < 0)
									revscg = 0;
								int resascg = GetRealTimeSciResa (pResaCmpt->Cmpt()[0], -1);
								if (resascg < 0)
									resascg = 0;
								double dd = 0;
								if (sumventecmpt - resascg > 0 )
									dd = (iTotalRecette - revscg) / (sumventecmpt - resascg);
								totaff = (int)dd;
								if (dd - totaff > 0.5)
									totaff ++;
							}
							szText.Format("%d", totaff);
							if (((CCTAApp*)APP)->GetRhealysFlag() && pResaBkt->NestLevel())
								pDC->DrawText("--", rect, DT_RIGHT|DT_SINGLELINE);
							else
								pDC->DrawText( szText, rect, DT_RIGHT|DT_SINGLELINE );
							pDC->SetTextColor( crSaved );
							sumventecmpt = 0;
							TotRevRect = rect;
						}
					}

					if (m_bPrevisionOK)
					{ // total prévision recette
						rect += ptOffset;
						COLORREF crSaved = pDC->SetTextColor( clrPrevision ); 
						if (PrevisionRecette >= 0)
						{
							if (((CCTAApp*)APP)->GetRhealysFlag() && pResaBkt->NestLevel())
								pDC->DrawText("--", rect, DT_RIGHT|DT_SINGLELINE);
							else
								DrawLongValue( pDC, PrevisionRecette, rect, FALSE );
						}
						else
							pDC->DrawText( "--", rect, DT_RIGHT|DT_SINGLELINE );
						pDC->SetTextColor( crSaved );
					}	

					pt.y +=  tm.tmHeight + tm.tmExternalLeading;
					pt.y +=  (tm.tmHeight + tm.tmExternalLeading) / 2;

					CSize sizeTextLabel(0, 0);
					CString szTextLabel;
					CPoint ptLabel( pt );
					CPoint ptSpacing( 20, 0 );
					ptLabel -= ptSpacing;  // adjust backwards just a little
					int iSpacing = 20;

					// Compartemt Level Data       
					// Capacity
					YM_VnlCmpt* pVnlCmpt = FindVnlCmpt( pCmptBkt->m_pResaCmpt->Cmpt() );
					long lCapacity = ( pVnlCmpt == NULL ) ? pCmptBkt->m_pResaCmpt->Capacity() : pVnlCmpt->GetCapacity() ;

					s.LoadString( IDS_CAP );
					s += " :";
					szTextLabel.Format( "%s %d", s, lCapacity );
					sizeTextLabel = pDC->GetOutputTextExtent( szTextLabel );  // get the text extent

					rect = CRect( ptLabel, CSize( sizeTextLabel.cx, sizeTextLabel.cy ) );
					rect += ptOffset;
					pDC->DrawText( szTextLabel, rect, DT_RIGHT|DT_SINGLELINE );
					ptLabel.x += sizeTextLabel.cx += iSpacing/2;

					// ResHoldGrp
					s.LoadString( IDS_GRP );
					s += " :";
					if (pDoc->GetNotFullJX())
						szTextLabel = s + " ?";
					else
						szTextLabel.Format( "%s %d", s, pCmptBkt->m_pResaCmpt->ResHoldGrp() );
					sizeTextLabel = pDC->GetOutputTextExtent( szTextLabel );  // get the text extent

					rect = CRect( ptLabel, CSize( sizeTextLabel.cx, sizeTextLabel.cy ) );
					rect += ptOffset;
					pDC->DrawText( szTextLabel, rect, DT_RIGHT|DT_SINGLELINE );

					// % prix garanti
					if (Flag_BuildOK && iTotalRecette)  
					{ 
						rect = TotRevRect;
						int yoff = tm.tmHeight + tm.tmExternalLeading;
						yoff += (tm.tmHeight + tm.tmExternalLeading) / 2;
						rect.top += yoff;
						rect.bottom += yoff;
						rect.left -= 60;
						iTotalRecette *= 100; // iTotalRevTick est en centimes d'euros
						double dd = ((iTotalRecette - iTotalRevTick)*100) / iTotalRecette;
						long pctpg = (int)dd;
						if (dd - pctpg > 0.5)
							pctpg ++;
						szTextLabel.Format ("PG: %d%%", pctpg);
						COLORREF crSaved = pDC->SetTextColor( clrFtCC );
						pDC->DrawText( szTextLabel, rect, DT_RIGHT|DT_SINGLELINE );
						pDC->SetTextColor( crSaved );
					}

					pt.y +=  tm.tmHeight + tm.tmExternalLeading;
					pt.y +=  (tm.tmHeight + tm.tmExternalLeading) / 2;
				} // if (pResaCmpt)

				offsetY = max(offsetY, pt.y); 
				pt.y = TABCTRL_HEIGHT;
				// CME Ano 79307
				int xPos = pDC->GetCurrentPosition().x;
				// CME ano hpqc 3835
				if(((CCTAApp*)APP)->GetRhealysFlag())
				{	
					if ( m_pVnl && m_pVnlLeg && ! m_bVnlResent )
					{
						pt.x =	xPos + (ptOffset.x)*4;
					}
					else
					{
						pt.x = xPos ;
					}		
				}
				else {
					pt.x = xPos;
				}
				
			}//DM7978  FIX # ano hpqc 1066  KGH: fin "if ( pCmptBkt)"
		} // boucle sur les cmpt

		YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
		if ( pTranche->ScxCount() > 0)
			CtrlScgLegs(); // Si besoin met à gris ou jaune le carré indicateur SCG dans fenêtre tronçon
		// et en cascade dans la fenêtre tranche.

		CSize sz = GetTotalSize();
		// DM7978 - LME - YI-5401 - inversion x et y
		m_SizeView.cx = pt.x;
		pt.x = 0;
		pt.y = offsetY + OFFSETY;
		// DM7978 - LME - YI-5401 - masquage des sous-contingents
		m_lYSciNotDisplayed = pt.y;
		//FIX #963 La hauteur de la fenetre n'était pas init si on avait pas de SC
		m_SizeView.cy = pt.y;
		// DM7978 - LME - FIN
		clrBk = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt();
		pDC->SetBkColor(clrBk);

		//
		// Affichage des informations SCI
		//
		//DM7978  FIX # ano hpqc 1066  KGH
		//const CArray <CString, CString&>& showOrderScx = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetCmptScxShowOrder();
		std::map<int, CmptOrdre>& showOrderScx = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetCmptScxShowOrder();
		bool bLastScx;

		if (m_CmptBktSciMap.GetCount()) //&& (((CCTAApp*)APP)->GetRhealysFlag() ||
			//(pTrancheView->getCurrentTrancheNature() == 1 || pTrancheView->getCurrentTrancheNature() == 3)))
			for(unsigned int i = 0; i < showOrderScx.size(); i++)// DM7978 - LME - YI-5401
			//for(int i = 0; i < showOrderScx.GetSize(); i++)// DM7978 - LME - YI-5401
			{
				//bLastScx = i == showOrder.GetSize() - 1;
				bLastScx = i == showOrder.size() - 1; 
				CResaCmptBktSci* pCmptBkt = NULL;
				iTotalRecette = 0;
				iTotalRevTick = 0;
				sKey = showOrderScx[i].cmpt; // DM7978 - LME - YI-5401
				//sKey = showOrderScx.GetAt(i); // DM7978 - LME - YI-5401
				m_CmptBktSciMap.Lookup( sKey, pCmptBkt );
				if ( pCmptBkt)//test si!=NULL
				{

					CPoint ptOffset( tm.tmAveCharWidth * 7, 0 );
					//FIN DM7978  FIX # ano hpqc 1066  KGH
					YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;
					if ( pResaCmpt )
					{
						const CSize sizeField( tm.tmAveCharWidth * 6, tm.tmHeight + tm.tmExternalLeading );
						const CSize bigSizeField( tm.tmAveCharWidth * 13, tm.tmHeight + tm.tmExternalLeading ); // sizeField + un ptOffset

						pt.y +=  tm.tmHeight + tm.tmExternalLeading;
						CRect rect( pt, sizeField );

						// Labels
						int kl = ( m_pVnl && m_pVnlLeg && ! m_bVnlResent ) ? 3 : 2;  // additional column for res sys difference
						int iColOffset = 4;


						// Cmpt
						s = pResaCmpt->Cmpt();
						rect = CRect( pt, sizeField );
						
						if (m_CmptBktSciMap.GetCount())// && (((CCTAApp*)APP)->GetRhealysFlag() ||
							//(pTrancheView->getCurrentTrancheNature() == 1 || pTrancheView->getCurrentTrancheNature() == 3)))
							pDC->DrawText( s, rect, DT_RIGHT|DT_SINGLELINE ); //Numéro de cmpt

						// Labels
						rect += ptOffset;
						DrawLabel( pDC, IDS_SEATS_SOLD, rect, TRUE ); // Label Vdu
						if ( m_pVnl && m_pVnlLeg && ! m_bVnlResent )
						{
							rect += ptOffset;
							DrawLabel( pDC, IDS_RES, rect, TRUE ); // Label Res
						}

						if (m_bPrevisionOK)
						{
							rect += ptOffset;
							COLORREF crSaved = pDC->SetTextColor( clrPrevision ); 
							DrawLabel( pDC, IDS_PREV, rect, TRUE ); // Label Prev (pour prevision)
							pDC->SetTextColor( crSaved );
						}

						rect += ptOffset;
						DrawLabel( pDC, IDS_CUR, rect, TRUE );  // Label Act

						rect += ptOffset;
						DrawLabel( pDC, IDS_NEW, rect, TRUE ); // Label colonne Moi
						rect += ptOffset;
						DrawLabel( pDC, IDS_AVAIL, rect, TRUE ); // Label colonne Dispo
						/* DM 5350 Rajout du label de la colonne € */
						if ( Flag_BuildOK )
						{
							COLORREF crSaved = pDC->SetTextColor( clrFtSci ); 
							rect += ptOffset;
							DrawLabel( pDC, m_bPanierMoyen ? IDS_EURO_PM : IDS_EURO, rect, TRUE ); // IDS_EURO Colonne Recette
							pDC->SetTextColor( crSaved );
						}
						/* DM 5350 */

						if (m_bPrevisionOK)
						{
							rect += ptOffset;
							COLORREF crSaved = pDC->SetTextColor( clrPrevision ); 
							DrawLabel( pDC, IDS_EURO, rect, TRUE ); // Label €
							pDC->SetTextColor( crSaved );
						}

						pt.y +=  tm.tmHeight + tm.tmExternalLeading + OFFSET_UNDERLINE;
						rect += CPoint( 0, tm.tmHeight + tm.tmExternalLeading + OFFSET_UNDERLINE );

						double dAddDmdFcTot = 0;
						double dAddDmdNewTot = 0;
						long PrevisionResa = -1;
						long PrevisionRecette = -1;
						long lResHoldTot = pCmptBkt->m_lResHoldTot;


						// Bucket Level Data
						//
						YmIcResaBktDom* pResaBkt = NULL;

						// DM7978 - LME - YI-5405 - condition d'affichage reduit des sci
						// determination du dernier sous-contingent affiche
						int iBktIdxMax = pCmptBkt->m_CmptBktLineArray.GetSize();
						if(IsModeReduit())
						{
							iBktIdxMax = min(iBktIdxMax, nbSci_Reduit);
						}

						for(int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++)
						{
							CResaRailViewEdit* pEdit = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew;
							pEdit->ShowWindow(SW_HIDE);
						}

						for( int iBktIdx = 0; iBktIdx < iBktIdxMax; iBktIdx++ )
						{
							pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
							BOOL bUnderline = iBktIdx == ( iBktIdxMax - 1 );
							CRect rectCtrl;
							YM_VnlSci* pVnlSci = NULL; // DM7978 - LME - YI-5405 - declaration de pVnlSci en dehors du if

							// DM7978 - LME - YI-5415 - recuperation des spheres de prix
							CSpherePrix spheresPrixSc = pDoc->m_sphereManager->findSpherePrixSC(pResaBkt->NestLevel(), pResaBkt->Cmpt(),pTranche->Nature(),pTranche->Entity()); //chercher la sphere qui correspond a NestLevel et Cmpt
							curColor = spheresPrixSc.getColor(true);//appliquer la couleur pastel
							// create and select a solid blue brush
							CBrush brush(curColor);
							CBrush* pOldBrush = NULL;
							// create and select a thick, black pen
							CPen pen;
							pen.CreatePen(PS_SOLID, 1, curColor);
							CPen* pOldPen = NULL;
							// DM7978 - LME - FIN

							// NestLevel (Bucket)
							rect = CRect( pt, sizeField );
							// DM7978 - LME - YI-5415 
							// dessine un rectangle de taille et position rect
							rect.left += 18;
							DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
							rect.left -= 18;
							// DM7978 - LME - FIN

							if (pResaBkt->ScxEtanche())
							{
								rect.left += 8;
								pResaBkt->XPos(rect.left);
								pResaBkt->YPos(rect.top);
								COLORREF crSaved;
								if (pResaBkt->Clicked())
									crSaved = pDC->SetTextColor( RGB(128,128,128) );
								pDC->DrawText( "(E)", rect, DT_LEFT|DT_SINGLELINE );
								if (pResaBkt->Clicked())
									pDC->SetTextColor (crSaved);
								rect.left -= 8;
							}
							// Numéro du sous-contingent
							pDC->DrawText( pResaBkt->BucketLabel(), rect, DT_RIGHT|DT_SINGLELINE );

							// ResHoldTot (SS)
							rect += ptOffset;
							if ((!pResaBkt->ResHoldIdvIsNull()) && (!pResaBkt->ResHoldGrpIsNull()))  // draw this field if the database value is not NULL
							{
								// DM7978 - LME - YI-5415 
								// dessine un rectangle de taille et position rect
								rect.left -= 5;
								DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
								rect.left += 5;
								// DM7978 - LME - FIN
								COLORREF crSaved;
								if (pDoc->GetNotFullJX())
									crSaved = pDC->SetTextColor( RGB(0,0,0) ); // DM7978 - LME - YI5401 - affichage en noir
								DrawLongValue( pDC, (pResaBkt->ResHoldIdv()+pResaBkt->ResHoldGrp()+pResaBkt->ResHoldNotYield()),
									rect, bUnderline ); // colonne Vdu  
								if (pDoc->GetNotFullJX())
									pDC->SetTextColor( crSaved );
							}

							// Difference in ResHoldTot from ResaRail (Res)
							// Do not draw this COLUMN if the VNL object is NULL
							if ( m_pVnl && m_pVnlLeg && ! m_bVnlResent )
							{
								CString szText;
								if ( m_pVnl->IsNetNesting() )  // For Net Nesting Inventory Method, we display Seats Sold at the Bucket level
								{
									YM_VnlSci* pVnlSci = FindVnlCmptSci( pResaCmpt->Cmpt(), pResaBkt->NestLevel() );
									if ( ! pVnlSci )  // couldn't find this cmpt/bkt in the Vnl results
										szText.Format("+?");
									else
									{
										// calculate the difference between ResaRail and Database ResHold values
										if ((!pResaBkt->ResHoldIdvIsNull()) && (!pResaBkt->ResHoldGrpIsNull()))  // draw this field if the database value is not NULL
										{
											int iDiff = pVnlSci->GetBaseSold() - (pResaBkt->ResHoldIdv()+pResaBkt->ResHoldGrp());
											szText.Format("%+d", iDiff);
											iTotalDiffRes += iDiff;
										}
										else
										{
											szText.Empty();
										}
									}
								}
								else   
								{
									// accumulate ResHold
									iTotalResHold += (pResaBkt->ResHoldIdv()+pResaBkt->ResHoldGrp()+pResaBkt->ResHoldNotYield());
								}
								rect += ptOffset;
								// DM7978 - LME - YI-5415 
								// dessine un rectangle de taille et position rect
								rect.left -= 5;
								DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
								rect.left += 5;
								// DM7978 - LME - FIN
								pDC->DrawText( szText, rect, DT_RIGHT|DT_SINGLELINE ); // colonne Res
								if( bUnderline )
									Underline( pDC, rect );
							}

							CResaRailViewEdit* pEdit = NULL;

							if (m_bPrevisionOK)
							{
								rect += ptOffset;
								// Modif 26/11/2013 suite aux test NOMOS sur demande JM Grugeaux
								// fait par HRI (premier paramètre à -1 au lieu de 0)
								// DM7978 - LME - YI-5415 
								// dessine un rectangle de taille et position rect
								rect.left -= 5;
								DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
								rect.left += 5;
								// DM7978 - LME - FIN
								int lprevresa = GetRrdPrevisionResa (-1, pResaCmpt->Cmpt(), 0, pResaBkt->NestLevel()); // Ici NestLevel contient le SCX !!
								COLORREF crSaved = pDC->SetTextColor( clrPrevision );
								if (lprevresa >= 0)
								{
									DrawLongValue( pDC, lprevresa, rect, bUnderline );
									if (PrevisionResa == -1)
										PrevisionResa = lprevresa;
									else
										PrevisionResa += lprevresa;
								}
								else
								{
									pDC->DrawText( "--", rect, DT_RIGHT|DT_SINGLELINE );
									if( bUnderline )
										Underline( pDC, rect );
								}
								pDC->SetTextColor( crSaved );
							}

							// AuthCur (Cur)
							pVnlSci = FindVnlCmptSci( pResaCmpt->Cmpt(), pResaBkt->NestLevel() );
							long lAuthCur = ( pVnlSci == NULL ) ? pResaBkt->AuthCur() : pVnlSci->GetAuth() ;

							if (pVnlSci && (pVnlSci->GetAuth() != pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthInit))
							{ // ya une différence, on mémorise la lettre de l'espace physique.
								if (m_SciCActMoi.Find (pResaCmpt->Cmpt()) < 0)
									m_SciCActMoi += pResaCmpt->Cmpt();
							}
							rect += ptOffset;
							if (!pResaBkt->AuthCurIsNull())  // draw this field if the database value is not NULL
							{
								// DM7978 - LME - YI-5415 
								// dessine un rectangle de taille et position rect
								rect.left -= 5;
								DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
								rect.left += 5;
								// DM7978 - LME - FIN
								DrawLongValue( pDC, lAuthCur, rect, FALSE ); // colonne Act 4
							}

							// AuthUser(New) -> Edit (New)
							rect += ptOffset;
							if (pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew != NULL)  // ensure this edit control is not NULL
							{
								// DM7978 - LME - YI-5415 
								// dessine un rectangle de taille et position rect
								rect.left -= 5;
								DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
								rect.left += 5;
								// DM7978 - LME - FIN
								pEdit = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew;
								pEdit->SetBackColor(curColor);
								// DM7978 - LME - correction du bug de la colonne Moi grisée lorsque le dptDate < date 
								// si le CEDit n'est pas en lecture seule on l'affiche
								if(!m_FlagReadOnly)
								{
									pEdit->ShowWindow(SW_SHOW);
								}
								// sinon on affiche un label
								else
								{
									pDC->SetTextColor(RGB(0,0,255));
									DrawLongValue( pDC, pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthInit , rect, bUnderline );
									pDC->SetTextColor(RGB(0,0,0));
								}
								// DM7978 - LME - FIN
								rectCtrl = rect;
								pDC->LPtoDP( rectCtrl );
								pEdit->MoveWindow( rectCtrl );
								if( m_pWndChildFocusSaved == NULL )
								{
									m_pWndChildFocusSaved = pEdit;
									pEdit->SetFocus();
								}
							}
							// AuthFcAvail (Avail) (Calculated because AuthUserNew is editable)
							rect += ptOffset;

							// DM 5350, une seule colonne dispo, initialisé comme celle qui disparait, mais sur modification
							// on affiche les valeurs calculée a partir des autorisations, comme avant la DM 5350.
							// DM7978 - LME - YI-5415 
							// dessine un rectangle de taille et position rect
							rect.left -= 5;
							DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
							rect.left += 5;
							// DM7978 - LME - FIN
							if ( !Modified())
							{
								if (!pResaBkt->AvailabilityCurIsNull())  // draw this field if the database value is not NULL                
									DrawLongValue( pDC, pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailCur, rect, FALSE ); //Dispo
							}
							else if (!pResaBkt->AvailabilityFcIsNull())  // draw this field if the database value is not NULL
							{ // colonne DISPO	
								DrawLongValue( pDC, pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAvailUser, rect, FALSE );
							}
							rect += ptOffset;
							if (Flag_BuildOK)  // draw this field if the database value is not NULL
							{ // colonne Recette
								// DM7978 - LME - YI-5415 
								// dessine un rectangle de taille et position rect
								rect.left -= 5;
								DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
								rect.left += 5;
								// DM7978 - LME - FIN
								COLORREF crSaved = pDC->SetTextColor( clrFtSci );
								long ptgr = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetRecetteTempsReel(pVnlSci ? pVnlSci->GetBaseSold() : 0);
								if (ptgr < 0)
								{
									pDC->DrawText( "--", rect, DT_CENTER|DT_SINGLELINE);
								}
								else
								{
									long pp = ptgr;
									if (m_bPanierMoyen)
										pp = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetPanierMoyen(pVnlSci ? pVnlSci->GetBaseSold() : 0);
									DrawLongValue( pDC, pp, rect, bUnderline );
									iTotalRecette += ptgr;
								}
								long rece = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->Recette();
								if (rece > 0)
									iTotalRevTick += rece;
								pDC->SetTextColor( crSaved );
							}

							if (m_bPrevisionOK)
							{
								rect += ptOffset;
								// DM7978 - LME - YI-5415 
								// dessine un rectangle de taille et position rect
								rect.left -= 5;
								DrawRect(pDC, rect, pOldBrush, &brush, pOldPen, &pen);
								rect.left += 5;
								// DM7978 - LME - FIN
								int lprevrev = GetRrdPrevisionRevenu (0, pResaCmpt->Cmpt(), 0, pResaBkt->NestLevel()); // ici le NestLevel contient le SCX
								COLORREF crSaved = pDC->SetTextColor( clrPrevision ); 
								if (lprevrev >= 0)
								{
									DrawLongValue( pDC, lprevrev, rect, bUnderline );
									if (PrevisionRecette == -1)
										PrevisionRecette = lprevrev;
									else
										PrevisionRecette += lprevrev;
								}
								else
								{
									pDC->DrawText( "--", rect, DT_RIGHT|DT_SINGLELINE );
									if( bUnderline )
										Underline( pDC, rect );
								}
								pDC->SetTextColor( crSaved );
							}

							lResHoldTot -= (pResaBkt->ResHoldIdv()+pResaBkt->ResHoldGrp()+pResaBkt->ResHoldNotYield());
							pt.y +=  tm.tmHeight + tm.tmExternalLeading;

						}
						pt.y += OFFSET_UNDERLINE;

						for(int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++)
						{
							CResaRailViewEdit* pEdit = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew;
							pEdit->UpdateWindow();
						}

						// ResHoldTot (Total)
						rect = CRect( pt + ptOffset, sizeField );
						DrawLongValue( pDC, pCmptBkt->m_lResHoldTot, rect, FALSE ); 

						int sumventescg = pCmptBkt->m_lResHoldTot;
						if ( m_pVnl && m_pVnlLeg && ! m_bVnlResent )
						{
							if ( ! m_pVnl->IsNetNesting() ) // for Threshold Nesting, we must calculate Real-Time Res Hold difference at the cmpt level
							{
								YM_VnlCmpt* pVnlCmpt = FindVnlCmpt( pCmptBkt->m_pResaCmpt->Cmpt() );
								iTotalDiffRes = pVnlCmpt->GetBaseSold() - iTotalResHold;
							}

							rect += ptOffset;
							CString szText;
							szText.Format("%+d", iTotalDiffRes);
							pDC->DrawText( szText, rect, DT_RIGHT|DT_SINGLELINE );
							sumventescg += iTotalDiffRes;
							iTotalDiffRes = iTotalResHold = 0;
						}

						if (m_bPrevisionOK)
						{ // total prévision resa
							rect += ptOffset;
							COLORREF crSaved = pDC->SetTextColor( clrPrevision ); 
							if (PrevisionResa >= 0)
								DrawLongValue( pDC, PrevisionResa, rect, FALSE );
							else
								pDC->DrawText( "--", rect, DT_RIGHT|DT_SINGLELINE );
							pDC->SetTextColor( crSaved );
						}	

						CRect TotRevRect;
						if (Flag_BuildOK)  // draw this field if the database value is not NULL
						{ // totale colonne Recette
							if ( iTotalRecette < 0)
							{
								pDC->DrawText( "--", rect, DT_CENTER|DT_SINGLELINE);
							}
							else
							{
								rect += ptOffset;
								rect += ptOffset;
								rect += ptOffset;
								rect += ptOffset;	
								CString szText;
								COLORREF crSaved = pDC->SetTextColor( clrFtSci );
								long totaff = iTotalRecette;
								if (m_bPanierMoyen)
								{
									double dd = 0;
									if (sumventescg > 0)
										dd = iTotalRecette / sumventescg;
									totaff = (int)dd;
									if (dd - totaff > 0.5)
										totaff ++;
								}
								szText.Format("%d", totaff);
								pDC->DrawText( szText, rect, DT_RIGHT|DT_SINGLELINE );
								pDC->SetTextColor( crSaved );
								sumventescg = 0;
								TotRevRect = rect;
							}
						}

						if (m_bPrevisionOK)
						{ // total prévision recette
							rect += ptOffset;
							COLORREF crSaved = pDC->SetTextColor( clrPrevision ); 
							if (PrevisionRecette >= 0)
								DrawLongValue( pDC, PrevisionRecette, rect, FALSE );
							else
								pDC->DrawText( "--", rect, DT_RIGHT|DT_SINGLELINE );
							pDC->SetTextColor( crSaved );
						}	

						pt.y +=  tm.tmHeight + tm.tmExternalLeading;
						pt.y +=  (tm.tmHeight + tm.tmExternalLeading) / 2;

						CSize sizeTextLabel(0, 0);
						CString szTextLabel;
						CPoint ptLabel( pt );
						CPoint ptSpacing( 20, 0 );
						ptLabel -= ptSpacing;  // adjust backwards just a little

						// Compartemt Level Data
						//

						// ResHoldGrp
						s.LoadString( IDS_GRP );
						s += ":";
						if (pDoc->GetNotFullJX())
							szTextLabel = s + " ?";
						else
							szTextLabel.Format( "%s %d", s, pCmptBkt->m_pResaCmpt->ResHoldGrp() );
						sizeTextLabel = pDC->GetOutputTextExtent( szTextLabel );  // get the text extent

						rect = CRect( ptLabel, CSize( sizeTextLabel.cx, sizeTextLabel.cy ) );
						rect += ptOffset;
						pDC->DrawText( szTextLabel, rect, DT_RIGHT|DT_SINGLELINE );

						ptLabel.x += (sizeTextLabel.cx + 10 + sizeField.cx);


						// CC -1 // JEJEJEJE
						YM_VnlCmpt* pVnlCmpt = FindVnlCmpt( pCmptBkt->m_pResaCmpt->Cmpt() );	
						s.LoadString( IDS_CCM1 );
						s += ":";
						szTextLabel.Format( "%s %d", s, pVnlCmpt ? pVnlCmpt->GetScgCapa() : pCmptBkt->m_lAuthPrev );
						sizeTextLabel = pDC->GetOutputTextExtent( szTextLabel );  // get the text extent

						CResaRailViewEdit* pEdit = pCmptBkt->m_pEditScgCapa;
						rect = CRect( pt + ptOffset, bigSizeField );
						if (pVnlCmpt)
							rect += ptOffset;
						if (m_bPrevisionOK)
							rect += ptOffset;
						pDC->DrawText( szTextLabel, rect, DT_RIGHT|DT_SINGLELINE );

						rect = CRect (pt + ptOffset + ptOffset + ptOffset, sizeField);
						if (pVnlCmpt)
							rect += ptOffset;
						if (m_bPrevisionOK)
							rect += ptOffset;
						// DM7978 - LME - correction du bug colonne Moi grisée lorsque dptDate < date
						if(m_FlagReadOnly)
						{
							pEdit->ShowWindow(SW_HIDE);
							pDC->SetTextColor(RGB(0,0,255));
							DrawLongValue(pDC, pVnlCmpt ? pVnlCmpt->GetScgCapa() : pCmptBkt->m_lAuthPrev , rect, false);
							pDC->SetTextColor(RGB(0,0,0));
						}
						// DM7978 - LME - FIN

						CRect rectCtrl = rect;
						pDC->LPtoDP( rectCtrl );
						if (pEdit)
							pEdit->MoveWindow( rectCtrl ); //JEJEJJEJE


						// % prix garanti //DM8029
						if (m_CmptBktSciMap.GetCount())// && (((CCTAApp*)APP)->GetRhealysFlag() ||
							//(pTrancheView->getCurrentTrancheNature() == 1 || pTrancheView->getCurrentTrancheNature() == 3)))
							if (Flag_BuildOK && iTotalRecette)  
							{ 
								rect = TotRevRect;
								int yoff = tm.tmHeight + tm.tmExternalLeading;
								yoff += (tm.tmHeight + tm.tmExternalLeading) / 2;
								rect.top += yoff;
								rect.bottom += yoff;
								rect.left -= 60;
								iTotalRecette *= 100; // iTotalRevTick est en centimes d'euros
								double dd = ((iTotalRecette - iTotalRevTick)*100) / iTotalRecette;
								long pctpg = (int)dd;
								if (dd - pctpg > 0.5)
									pctpg ++;
								szTextLabel.Format ("PG: %d%%", pctpg);
								COLORREF crSaved = pDC->SetTextColor( clrFtCC );
								pDC->DrawText( szTextLabel, rect, DT_RIGHT|DT_SINGLELINE );
								pDC->SetTextColor( crSaved );
							}

						pt.y +=  tm.tmHeight + tm.tmExternalLeading;
						pt.y +=  (tm.tmHeight + tm.tmExternalLeading) / 2;
					} // if (pResaCmpt)

					// DM7978 - LME - YI5401 - masquage des sous-contingents
					lYSciDisplayed = max(pt.y, lYSciDisplayed);
					m_SizeView.cy = pt.y;
					// DM7978 - LME - YI5401 
					pt.y = offsetY + OFFSETY;
					// CME Ano 79307
					pt.x = pDC->GetCurrentPosition().x;
					

				}//DM7978  FIX # ano hpqc 1066  KGH: fin "if ( pCmptBkt)"
			} // boucle sur les cmpt SCI
			// fin affichage SCI
			pDC->SelectObject(pOldPen);

			// TDR Anomalie 79307
			if (m_bInitialUpdate || bModifFont)
			{
				CalculateSize();
				m_bInitialUpdate = false;
				Resize();
			}
	}
	else
	{
		// Gray the background
		CRect rect;
		GetClientRect(rect);
		::FillRect( pDC->m_hDC, rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH) );
	}
	pDC->SelectObject( pFontSaved );

}

void CResaRailView::DrawLabel( CDC* pDC, UINT nID, LPRECT lpRect, BOOL bUnderline, UINT nFormat /* = DT_CENTER|DT_SINGLELINE */ )
{
	CString s;

	s.LoadString( nID );
	pDC->DrawText( s, lpRect, nFormat);
	if( bUnderline )
		Underline( pDC, lpRect );
}

void CResaRailView::DrawLongValue( CDC* pDC, long lValue, LPRECT lpRect, BOOL bUnderline )
{
	CString s;

	s.Format( "%5d", lValue );
	pDC->DrawText( s, lpRect, DT_RIGHT|DT_SINGLELINE);
	if( bUnderline )
		Underline( pDC, lpRect );
}

void CResaRailView::DrawRect(CDC *pDC, CRect rect, CBrush* pOldBrush, CBrush* pBrush, CPen* pOldPen, CPen* pPen)
{
	// DM7978 - LME - YI-5415 
	// dessine un rectangle de taille et position rect
	pOldBrush = pDC->SelectObject(pBrush);
	pOldPen = pDC->SelectObject(pPen);
	pDC->Rectangle(rect);
	// put back the old objects
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	pDC->SetBkMode(TRANSPARENT);
	// DM7978 - LME - FIN
}

void CResaRailView::Underline( CDC* pDC, LPRECT lpRect )
{
	pDC->MoveTo( lpRect->left, lpRect->bottom );
	pDC->LineTo( lpRect->right, lpRect->bottom );
}


int CResaRailView::GetRealTimeRevenu (char cmpt, int bkt)
{
	CResaCmptBkt* pCmptBkt = NULL;
	CString sKey = cmpt;
	if (!m_CmptBktMap.Lookup( sKey, pCmptBkt ))
		return -1;
	int valeur = 0;
	int ptgr;
	if (bkt == -1)
	{ // on fait la somme au niveau compartiment  
		for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
		{
			YM_VnlBkt* pVnlBkt = FindVnlCmptBkt( sKey, iBktIdx );
			ptgr = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetRecetteTempsReel(this, pVnlBkt ? pVnlBkt->GetBaseSold() : 0);
			if (ptgr > 0)
				valeur += ptgr;
		}
		return valeur;
	}
	else if (bkt < pCmptBkt->m_CmptBktLineArray.GetSize())
	{
		YM_VnlBkt* pVnlBkt = FindVnlCmptBkt( sKey, bkt );
		ptgr = pCmptBkt->m_CmptBktLineArray[ bkt ]->GetRecetteTempsReel(this, pVnlBkt ? pVnlBkt->GetBaseSold() : 0);
		return ptgr;
	}
	return -1;
}

int CResaRailView::GetRealTimeSciRevenu (char cmpt, int sci)
{
	CResaCmptBktSci* pCmptBkt = NULL;
	CString sKey = cmpt;
	if (!m_CmptBktSciMap.Lookup( sKey.MakeLower(), pCmptBkt ))
		return -1;
	int valeur = 0;
	int ptgr;
	if (sci <= 0)
	{ // on fait la somme au niveau compartiment  
		for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
		{
			YM_VnlSci* pVnlSci = FindVnlCmptSci( sKey, iBktIdx+1 );
			ptgr = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetRecetteTempsReel(pVnlSci ? pVnlSci->GetBaseSold() : 0);
			if (ptgr > 0)
				valeur += ptgr;
		}
		return valeur;
	}
	else if (sci <= pCmptBkt->m_CmptBktLineArray.GetSize())
	{
		YM_VnlSci* pVnlSci = FindVnlCmptSci( sKey, sci );
		ptgr = pCmptBkt->m_CmptBktLineArray[ sci-1 ]->GetRecetteTempsReel(pVnlSci ? pVnlSci->GetBaseSold() : 0);
		return ptgr;
	}
	return -1;
}

int CResaRailView::GetRealTimeSciResa (char cmpt, int sci)
{
	CResaCmptBktSci* pCmptBkt = NULL;
	CString sKey = cmpt;
	if (!m_CmptBktSciMap.Lookup( sKey.MakeLower(), pCmptBkt ))
		return -1;
	int valeur = 0;
	if (sci <= 0)
	{ // on fait la somme au niveau compartiment  
		for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
		{
			YmIcResaBktDom* pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
			YM_VnlSci* pVnlSci = FindVnlCmptSci( sKey, iBktIdx+1 );
			if (pVnlSci)
				valeur += pVnlSci->GetBaseSold();
			else
				valeur += (pResaBkt->ResHoldIdv()+pResaBkt->ResHoldGrp()+pResaBkt->ResHoldNotYield());
		}
		return valeur;
	}
	else if (sci <= pCmptBkt->m_CmptBktLineArray.GetSize())
	{
		YM_VnlSci* pVnlSci = FindVnlCmptSci( sKey, sci );
		if (pVnlSci)
			valeur = pVnlSci->GetBaseSold();
		else
			valeur = pCmptBkt->m_lResHoldTot;
		return valeur;
	}
	return -1;
}

int CResaRailView::GetSciNbVoyageurs (char cmpt, int sci)
{
	CResaCmptBktSci* pCmptBkt = NULL;
	CString sKey = cmpt;
	if (!m_CmptBktSciMap.Lookup( sKey.MakeLower(), pCmptBkt ))
		return -1;
	int valeur = 0;
	CResaCmptBktSciLine *pCmptBktSciLine;
	if (sci <= 0)
	{ // on fait la somme au niveau compartiment  
		for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
		{
			pCmptBktSciLine = pCmptBkt->m_CmptBktLineArray[ iBktIdx ];
			valeur += pCmptBktSciLine->m_pResaBkt->RealNbVoyageurs();
		}
		return valeur;
	}
	else if (sci <= pCmptBkt->m_CmptBktLineArray.GetSize())
	{
		pCmptBktSciLine = pCmptBkt->m_CmptBktLineArray[ sci-1 ];
		valeur = pCmptBktSciLine->m_pResaBkt->RealNbVoyageurs();
		return valeur;
	}
	return -1;
}

int CResaRailView::GetSciRecette (char cmpt, int sci)
{
	CResaCmptBktSci* pCmptBkt = NULL;
	CString sKey = cmpt;
	if (!m_CmptBktSciMap.Lookup( sKey.MakeLower(), pCmptBkt ))
		return -1;
	int valeur = 0;

	CResaCmptBktSciLine *pCmptBktSciLine;
	if (sci <= 0)
	{ // on fait la somme au niveau compartiment  
		for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
		{
			pCmptBktSciLine = pCmptBkt->m_CmptBktLineArray[ iBktIdx ];
			valeur += pCmptBktSciLine->m_pResaBkt->Recette();
		}
		return valeur;
	}
	else if (sci <= pCmptBkt->m_CmptBktLineArray.GetSize())
	{
		pCmptBktSciLine = pCmptBkt->m_CmptBktLineArray[ sci-1 ];
		valeur = pCmptBktSciLine->m_pResaBkt->Recette();
		return valeur;
	}
	return -1;
}


void CResaRailView::GetSciCountEtanche(char cmpt, int& scxcount, int& scxetanche)
{
	scxcount = 0;
	scxetanche = 0;
	CResaCmptBktSci* pCmptBkt = NULL;
	CString sKey = cmpt;
	if (!m_CmptBktSciMap.Lookup( sKey.MakeLower(), pCmptBkt ))
		return;
	YmIcResaBktDom* pResaBkt;
	for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
	{
		pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
		scxcount++;
		if (pResaBkt->ScxEtanche())
			scxetanche++;
	}
}

// Return TRUE if this Leg is a CroosMidnight One
BOOL CResaRailView::IsCrossMidnightLeg( YmIcTrancheLisDom* pTranche, YmIcLegsDom* pLeg )
{
	if (m_pVnl)  // do we have real time ?
	{
		// due to an inconsistency in the database and real-time VNL, we now search the VNL to see if this train crosses midnight.
		YM_VnlLeg* pVnlLeg = NULL;  // this is our working pointer
		YM_VnlLegIterator* LegIterator = m_pVnl->CreateVnlLegIterator();
		for ( LegIterator->First(); ! LegIterator->Finished(); LegIterator->Next() )
		{
			pVnlLeg = LegIterator->Current();
			if ( (pVnlLeg->GetLegOrig() == pLeg->LegOrig()) && (pVnlLeg->GetLegDest() == pLeg->LegDest()) )
			{
				delete LegIterator;
				return pVnlLeg->GetDepartsNextDay();
			}
		}
		delete LegIterator;
	}
	// If not found, let's try the second algorithm
	/* else  // no real time
	{*/
	if (!pTranche->CrossesMidnight())
		return FALSE;

	YM_Iterator<YmIcLegsDom>* pLegIterator = 
		YM_Set<YmIcLegsDom>::FromKey(LEG_KEY)->CreateIterator();
	YmIcLegsDom* pwLeg;
	for( pLegIterator->First(); !pLegIterator->Finished(); pLegIterator->Next() ) 
	{
		pwLeg = pLegIterator->Current();
		if ((pwLeg->LegSeq() < pLeg->LegSeq()) && (pwLeg->DptTime() > pLeg->DptTime()))
		{ 
			delete pLegIterator;
			return TRUE;
		}
	}
	delete pLegIterator;
	return FALSE; 
	//  }
}
CString CResaRailView::BuildDebVnaMess (CString debVna, BOOL bCrossMidnight)
{
	char szItoa[10];
	CString szMsg = debVna;

	// Tranche number from currently loaded tranche
	YmIcTrancheLisDom* pTranche = 
		YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	itoa (pTranche->TrancheNo(), szItoa, 10);
	szMsg += szItoa;
	szMsg += '/';

	// Date from currently loaded tranche
	//
	// Day of month
	RWDate rwDate;
	if (bCrossMidnight)
		rwDate = pTranche->DptDateTr() + 1;
	else
		rwDate = pTranche->DptDateTr();
	int iDay = rwDate.dayOfMonth();
	itoa ( iDay, szItoa, 10 );
	if (iDay < 10)
		szMsg += '0';
	szMsg += szItoa;

	// Convert into a month name
	szMsg += MONTH_LIST_ENGLISH.Mid( (rwDate.month()-1)*3, 3 );
	return szMsg;
}

// Build a VNAU
// The input parameter is a Leg record.  The reason for this is the copier/coller
//   functionality.  It is possible to build a VNAU for ALL legs in the current
//   leg list (currently loaded Tranche/Date/RRD Index), and therefore we use
//   the current authorization levels, no matter the leg record passed in.
//
CString CResaRailView::BuildVnau( YmIcLegsDom* pLeg, CResaCmptBkt* pCmptBktJum,
																 BOOL bCrossMidnight )
{
	// Build the VNAU command...
	char szItoa[10];
	CString szMsg = BuildDebVnaMess ("VNAU", bCrossMidnight);
	// Leg Origin / Destination from parameter
	szMsg += pLeg->LegOrig();
	szMsg += pLeg->LegDest();

	if ( pCmptBktJum != NULL)  // are we building a Tgv Jumeaux VNAU command ?
	{
		szMsg += "/";
		itoa ( pCmptBktJum->m_pResaCmpt->TgvJumeauxPct(), szItoa, 10 );
		szMsg += szItoa;

		YmIcResaCmptDom* pResaCmpt = pCmptBktJum->m_pResaCmpt;

		for( int iBktIdx = 0; iBktIdx < pCmptBktJum->m_CmptBktLineArray.GetSize(); iBktIdx++ )
		{
			// add the cmpt/bkt authorizations to the vnau command
			szMsg += "/";
			szMsg += pResaCmpt->Cmpt();
			itoa ( pCmptBktJum->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->NestLevel(), szItoa, 10 );
			szMsg += szItoa;
			szMsg += "-";
			itoa ( pCmptBktJum->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew, szItoa, 10 );
			szMsg += szItoa;
		}  // for
	}
	else  // a particular compartment was not passed, so loop through all cmpts, and build the Vnau
	{
		// Iterate through the compartments for currently loaded TDL
		CResaCmptBktMap* pCmptBktMap = GetCmptBktMap();
		if( pCmptBktMap->GetCount() == 0 )  // ensure we have some cmpts and bkts
		{
			szMsg.Empty();
		}
		else
		{
			CString sKey;
			int iKeyIdx = 0;
			while( iKeyIdx < m_CmptBktMap.m_aszKey.GetSize() )
			{
				CResaCmptBkt* pCmptBkt;
				sKey = m_CmptBktMap.m_aszKey.GetAt(iKeyIdx++);
				GetCmptBktMap()->Lookup( sKey, pCmptBkt );

				YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;

				// iterate through all the buckets for this compartment
				YmIcResaBktDom* pResaBkt;
				for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
				{
					pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
					YM_VnlCmpt* pVnlCmpt = FindVnlCmpt( pCmptBkt->m_pResaCmpt->Cmpt() );
					long lCapacity = ( pVnlCmpt == NULL ) ? pResaCmpt->Capacity() : pVnlCmpt->GetCapacity() ;
					if (!strcmp(pResaCmpt->Cmpt(), "O") &&
						(pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew > lCapacity))
					{
						szMsg.Empty();
						CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AUTOTRAIN_AUTH_INF_CAP);
						CString mess;
						mess.Format (szText, lCapacity);
						AfxMessageBox( mess, MB_ICONEXCLAMATION | MB_APPLMODAL );
						return ( szMsg );
					}

					// add the cmpt/bkt authorizations to the vnau command
					szMsg += "/";
					szMsg += pResaCmpt->Cmpt();
					itoa ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->NestLevel(), szItoa, 10 );
					szMsg += szItoa;
					szMsg += "-";
					itoa ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew, szItoa, 10 );
					szMsg += szItoa;
				}  // for
			}  // while
		} 
	}
	if (szMsg.IsEmpty())
	{
		CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_RESARAIL_DATA);
		AfxMessageBox( szText, MB_ICONEXCLAMATION | MB_APPLMODAL );
	}
	return ( szMsg );
}

//DM7978 - LME - YI-5405 - implementation de la regle de filtrage des CC (onglet reduit)
void CResaRailView::CmptFilteringRule(const CString& cmpt, CCmptFilteredArray& filtered)
{
	CResaCmptBkt* pCmptBkt = NULL;
	GetCmptBktMap()->Lookup(cmpt, pCmptBkt );
	if(pCmptBkt)//DM7978  FIX # ano hpqc 1066  KGH
	{
		long lastOpenCc = pCmptBkt->m_pResaCmpt->LastOpenCc();
		long ccMax = pCmptBkt->m_pResaCmpt->CcMax();
		long nbCcMax = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNbCcVisu();

		// on verifie que 
		if(pCmptBkt->m_CmptBktLineArray.GetSize() < nbCcMax)
		{
			nbCcMax = pCmptBkt->m_CmptBktLineArray.GetSize();
		}
		// si la derniere cc ouverte est inferieur au nombre de cc a afficher
		if(lastOpenCc < nbCcMax)
		{
			// on affiche toutes les cc de 0 a nbCCmax-1
			for(int i = 0; i < nbCcMax; i++)
			{
				filtered.Push(i);
			}
		}
		// sinon
		else
		{
			filtered.Push(0);
			filtered.Push(1);
			if(ccMax <= lastOpenCc - (nbCcMax - 3))
			{

				filtered.Push(ccMax);
				for(int i = lastOpenCc - nbCcMax + 4; i <= lastOpenCc ; i++)
				{
					filtered.Push(i);
				}
			}
			// sinon
			else if(ccMax > lastOpenCc - (nbCcMax - 3))
			{
				for(int i = lastOpenCc - nbCcMax + 3; i <= lastOpenCc ; i++)
				{
					filtered.Push(i);
				}
			}
		}
	}//DM7978  FIX # ano hpqc 1066  KGH: fin du "if(pCmptBkt)"
}

void CResaRailView::ReorganizeTabOrder()
{
	CResaRailViewEdit * pLastEdit = NULL;
	bool bIsFirstEdit = true;
	//DM7978  FIX # ano hpqc 1066  KGH
	std::map <int, CmptOrdre>& showOrder = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetCmptShowOrder();
	//const CArray <CString, CString&>& showOrder = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetCmptShowOrder();
	//const CArray <CString, CString&>& showOrderScx = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetCmptScxShowOrder();
	std::map <int, CmptOrdre>& showOrderScx = ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetCmptScxShowOrder();
	//FIN DM7978  FIX # ano hpqc 1066  KGH
	CString sKey;
	CCmptFilteredArray indexLoop;
	CResaCmptBkt* pCmptBkt = NULL;
	CResaCmptBktSci* pCmptBktSci = NULL;
	CResaRailViewEdit* pEdit = NULL;
	int nbSci_Reduit = App().GetDocument()->m_pGlobalSysParms->GetNbScVisu();

	// Reorganisation CC
	// Boucle sur les cmpts
	//DM7978  FIX # ano hpqc 1066  KGH
	for(unsigned int i = 0; i < showOrder.size(); i++) //DM7978 - LME - YI-5401
	//for(int i = 0; i < showOrder.GetSize(); i++) //DM7978 - LME - YI-5401
	{
		CmptOrdre cmpt_ordre = showOrder[i];
		sKey = cmpt_ordre.cmpt; //DM7978 - LME - YI-5401
		//sKey = showOrder.GetAt(i); //DM7978 - LME - YI-5401
		//FIN DM7978  FIX # ano hpqc 1066  KGH
		m_CmptBktMap.Lookup( sKey, pCmptBkt );

		if (pCmptBkt == NULL)
			continue;

		// Stockage des indices des CC a afficher en onglet reduit
		indexLoop.RemoveAll();

		// Initilisation du tableau d'indice a parcourir pour l'onglet reduit
		if(IsModeGlobal())
		{
			for( int j = 0; j < pCmptBkt->m_CmptBktLineArray.GetSize(); j++ )
			{
				indexLoop.Push(j);
			}
		}
		else if(IsModeReduit())
		{
			CmptFilteringRule(showOrder[i].cmpt, indexLoop);//DM7978  FIX # ano hpqc 1066  KGH
			//CmptFilteringRule(showOrder.GetAt(i), indexLoop);
		}
		//DM7978 - LME - FIN

		// Bucket Level Data
		//
		YmIcResaBktDom* pResaBkt = NULL;
		for( int k = 0; k < indexLoop.GetSize(); k++ )
		{
			int iBktIdx = indexLoop.GetAt(k);
			pEdit = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew;
			pEdit->SetWindowPos(pLastEdit, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			pLastEdit = pEdit;
		}
	}

	// Reorganisation SCx
	// Boucle sur les cmpts
	//DM7978  FIX # ano hpqc 1066  KGH
	for(unsigned int i = 0; i < showOrderScx.size(); i++) //DM7978 - LME - YI-5401 
	//for(int i = 0; i < showOrderScx.GetSize(); i++) //DM7978 - LME - YI-5401
	{
		sKey = showOrderScx[i].cmpt; //DM7978 - LME - YI-5401
		//sKey = showOrderScx.GetAt(i); //DM7978 - LME - YI-5401
		//FIN DM7978  FIX # ano hpqc 1066  KGH
		m_CmptBktSciMap.Lookup( sKey, pCmptBktSci );

		if (pCmptBktSci == NULL)
			continue;

		// NPI - Ano 70036
		pEdit = pCmptBktSci->m_pEditScgCapa;
		pEdit->SetWindowPos(pLastEdit, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		///

		// Stockage des indices des CC a afficher en onglet reduit
		indexLoop.RemoveAll();

		int iBktIdxMax = pCmptBktSci->m_CmptBktLineArray.GetSize();
		if(IsModeReduit())
		{
			iBktIdxMax = min(iBktIdxMax, nbSci_Reduit);
		}

		for( int iBktIdx = 0; iBktIdx < iBktIdxMax; iBktIdx++ )
		{
			pEdit = pCmptBktSci->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew;
			pEdit->SetWindowPos(pLastEdit, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			pLastEdit = pEdit;
		}
	}
}

void CResaRailView::ReorganizeEditVisibility()
{

}

//DM7978 - LME - FIN

int CResaRailView::TotAuthBkt0()
{
	CString sKey;
	int iKeyIdx = 0;
	int tot0 = 0;
	while( iKeyIdx < m_CmptBktMap.m_aszKey.GetSize() )
	{
		CResaCmptBkt* pCmptBkt;
		sKey = m_CmptBktMap.m_aszKey.GetAt(iKeyIdx++);
		GetCmptBktMap()->Lookup( sKey, pCmptBkt );
		tot0 += pCmptBkt->m_CmptBktLineArray[0]->m_lAuthNew;
	}
	return tot0;
}

BOOL CResaRailView::OverBookingTotalOK()
{
	CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
	if (!pTrancheView)
		return TRUE;
	YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();
	if (!pLoadedTranche || pLoadedTranche->Nature())
		return TRUE;
	int tot = TotAuthBkt0();
	if (tot > (m_capatotale * m_SurResaGlob) / 100)
	{
		CString ss = ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_OVB_GLOBAL ); 
		CString mess;
		mess.Format (ss, tot, m_capatotale, m_SurResaGlob);
		AfxMessageBox (mess, MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}

CString CResaRailView::BuildVnae( CString& VnaeSuite )
{
	CString szMsg = BuildDebVnaMess ("VNAE", FALSE);
	szMsg += "ALL";
	int iKeyIdx = 0;
	CString sKey;
	VnaeSuite = szMsg;
	int nbc = 0;
	int yena = 0;
	int yenabase = 0;
	int yenasuite = 0;
	if (m_CmptBktSciMap.GetCount())
		while( iKeyIdx < m_CmptBktSciMap.m_aszKey.GetSize() )
		{
			CResaCmptBktSci* pCmptBkt = NULL;
			sKey = m_CmptBktSciMap.m_aszKey.GetAt(iKeyIdx++);
			m_CmptBktSciMap.Lookup( sKey, pCmptBkt );
			if (pCmptBkt)
			{
				nbc++;
				YmIcResaBktDom* pResaBkt;
				for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
				{
					pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
					if (pResaBkt->Clicked())
					{
						CString rab;
						yena ++;
						rab.Format ("/%c%d", toupper(pResaBkt->Cmpt()[0]), pResaBkt->NestLevel());
						if (nbc == 1)
						{
							yenabase++;
							szMsg += rab;
						}
						else
						{
							yenasuite++;
							VnaeSuite += rab;
						}
					}
				}
			}
		}
		if (!yena)
		{
			szMsg.Empty();
			VnaeSuite.Empty();
		}
		if (!yenasuite)
			VnaeSuite.Empty();
		if (!yenabase)
		{
			szMsg = VnaeSuite;
			VnaeSuite.Empty();
		}
		return szMsg;
}

// Build a VNAS
// The input parameter is a Leg record.  The reason for this is the copier/coller
//   functionality.  It is possible to build a VNAS for ALL legs in the current
//   leg list (currently loaded Tranche/Date/RRD Index), and therefore we use
//   the current authorization levels, no matter the leg record passed in.
//	 The return of the function is the VNAS for the first compartment.
//   The VnasSuite parameter will contains the VNAS for the folowing compartments
//
CString CResaRailView::BuildVnas( YmIcLegsDom* pLeg, BOOL bCrossMidnight, CString& VnasSuite)
{
	CString szMsg;
	if (!GetCmptBktSciMap()->GetCount())
		return szMsg;

	szMsg = BuildDebVnaMess ("VNAS", bCrossMidnight);
	// Leg Origin / Destination from parameter
	szMsg += pLeg->LegOrig();
	szMsg += pLeg->LegDest();

	POSITION pos = GetCmptBktSciMap()->GetStartPosition();
	CString sKey;
	CString sCmpt;
	CString sSci;
	CString sScg;
	int nbc = 0;
	VnasSuite = szMsg;
	while( pos != NULL ) // loop through all compartments
	{
		CResaCmptBktSci* pCB = NULL;
		GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCB );
		if (pCB)
		{
			nbc++;
			sCmpt = pCB->m_pResaCmpt->Cmpt();
			sCmpt.MakeUpper();
			sScg.Format ("/%sG-%d/", sCmpt, pCB->m_lAuthNew);
			if (nbc == 1)
				szMsg += sScg;
			else
				VnasSuite += sScg;
			for( int iBktIdx = 0; iBktIdx < pCB->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				if (iBktIdx == 0)
					sSci.Format ("%s%d-%d", sCmpt, iBktIdx+1, pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew);
				else 
					sSci.Format ("/%s%d-%d", sCmpt, iBktIdx+1, pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew);
				if (nbc==1)
					szMsg += sSci;
				else
					VnasSuite += sSci;
			}
		}
	}
	if (nbc==1)
		VnasSuite.Empty();
	return szMsg;
}

void CResaRailView::MajTrancheSentFlag ()
{
	YM_Iterator<YmIcLegsDom>* pLegIterator = 
		YM_Set<YmIcLegsDom>::FromKey(LEG_KEY)->CreateIterator();
	int sent = 0;   //DM4802, modification on met a jour le SentFlag de la tranche dès qu'un seul VNAU a
	// été envoyé sur au moins un tronçon.
	YmIcLegsDom* pLeg;
	for( pLegIterator->First(); !pLegIterator->Finished(); pLegIterator->Next() ) 
	{
		pLeg = pLegIterator->Current();
		if (pLeg->SentFlag())
		{
			sent = 1;
			break;
		}  
	}
	delete pLegIterator;
	pLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY); 
	if (pLeg->SentFlag())
		sent = 1;
	YmIcTrancheLisDom* pTranche = 
		YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	pTranche->SentFlag(ChangeSentFlag(sent, pTranche->SentFlag()));
}

void CResaRailView::MajTrancheScxSentFlag ()
{
	YM_Iterator<YmIcLegsDom>* pLegIterator = 
		YM_Set<YmIcLegsDom>::FromKey(LEG_KEY)->CreateIterator();
	int sent = 0;   //DM4802, modification on met a jour le SentFlag de la tranche dès qu'un seul VNAS a
	// été envoyé sur au moins un tronçon.
	YmIcLegsDom* pLeg;
	long SumScg = 0;
	for( pLegIterator->First(); !pLegIterator->Finished(); pLegIterator->Next() ) 
	{
		pLeg = pLegIterator->Current();
		if (pLeg->ScxSentFlag())
			sent = 1;
		SumScg += pLeg->ScxCapa();
	}
	delete pLegIterator;
	YmIcTrancheLisDom* pTranche = 
		YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	pTranche->ScxSentFlag(ChangeSentFlag(sent, pTranche->ScxSentFlag()));
	pTranche->ScxCapa(SumScg);
}

// Send a VNAU
// The input parameter is a Leg record.  The reason for this is the copier/coller
//   functionality.  It is possible to send a VNAU for ALL legs in the current
//   leg list (currently loaded Tranche/Date/RRD Index), and therefore we use
//   the current authorization levels, no matter the leg record passed in.
//
// This function will build the VNAU command (if not given), Send it, then Update the Edit Status
//   for the currently loaded Tranche and the Leg record passed in as parameter.
//
int CResaRailView::SendVnau( CString szMsg, YmIcLegsDom* pLeg, CResaCmptBkt* pCmptBktJum, BOOL bConfirm /* = TRUE */ )
{
	if (!m_bOkOnRhea)
		return 0;

	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	// Get currently loaded tranche
	YmIcTrancheLisDom* pTranche = 
		YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);

	// Send the VNAU command...
	int iVnauSentStatus = 0;

	// Build the VNAU command...
	BOOL bCML = IsCrossMidnightLeg (pTranche, pLeg);
	if (szMsg.IsEmpty())
	{
		if (!OverBookingTotalOK())
			return 0;
		szMsg = BuildVnau( pLeg, pCmptBktJum, bCML );
		if ( szMsg.IsEmpty() )
			return 0;
	}

	if (pCmptBktJum)
		iVnauSentStatus = 1;
	else 
	{
		if (bConfirm && ControlActDiffMoi(1))
		{
			CString s;
			s = ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_ACT_DIFF_MOI );
			s += '\n';
			s += szMsg;
			if (MessageBox (s, NULL, MB_YESNO) == IDYES)
				iVnauSentStatus = ((CCTAApp*)APP)->GetResComm()->SendVnau( NULL, szMsg, FALSE, bCML );
		}
		else
			iVnauSentStatus = ((CCTAApp*)APP)->GetResComm()->SendVnau( NULL, szMsg, bConfirm, bCML );
	}

	// Update the database (Tranche and Leg) with the Edit Status...

	switch ( iVnauSentStatus )
	{
	case 0 :  // user chose NOT to send vnau to ResaRail
		{
			// VNAU was not sent
			if ( pCmptBktJum != NULL)
			{ // Cas impossible à partir du 10/12/98, car les VNAU % TGV jumeaux sont
				// envoyés sans demander de confirmation à l'utilisateur.
				// Tgv Jum pctg was modified but not sent to Resa Rail
				//pLeg->EditStatus( pLeg->EditStatus() | EDIT_STATUS_TGVMODNOTSENT);
				pTranche->EditStatus( pTranche->EditStatus() | EDIT_STATUS_TGVMODNOTSENT);
			}
			else
			{
				// Authorizations were modified but not sent to Resa Rail
				pLeg->EditStatus( pLeg->EditStatus() | EDIT_STATUS_AUTHMODNOTSENT);
				pTranche->EditStatus( pTranche->EditStatus() | EDIT_STATUS_AUTHMODNOTSENT);
			}

			pLeg->SentFlag(ChangeSentFlag(0, pLeg->SentFlag()));  // user did not send to ResaRail
			pTranche->SentFlag(ChangeSentFlag(0, pTranche->SentFlag()));
			break;
		}
	case -1 :  // Communication socket not opened, the vnau is directly inserted in the database.
		{
			((CCTAApp*)APP)->SetRealTimeMode(FALSE);  // Turn off Real Time menu option
			if (pCmptBktJum == NULL)
			{
				pLeg->SentFlag(ChangeSentFlag(1, pLeg->SentFlag()));  // even though the socket was not opened, the VNAU was sent to the database
				MajTrancheSentFlag ();
			}
			break;
		}
	default :  // the message number was returned, which means vnau successfully sent to ResaRail
		{
			if ( pCmptBktJum != NULL)
			{
				// Don't change the status if TGV Jum pctg were not sent
				// otherwise, everything is okay
				// pLeg->EditStatus (pLeg->EditStatus() &  ~EDIT_STATUS_TGVMODNOTSENT);
				pTranche->EditStatus (pTranche->EditStatus() &  ~EDIT_STATUS_TGVMODNOTSENT);

				// Reset the tgv jum changed boolean indicator
				pCmptBktJum->m_bTGVJumPctChanged = FALSE;
			}
			else
			{
				YmIcLegsDom* pCurLeg = GetLegView()->GetLoadedLeg();
				// Don't change the status if Auths were not sent
				// otherwise, everything is okay
				pLeg->EditStatus (pLeg->EditStatus() & ~EDIT_STATUS_AUTHMODNOTSENT);
				if (pCurLeg && !strcmp (pLeg->LegOrig(), pCurLeg->LegOrig())
					&& (pLeg->TrancheNo() == pCurLeg->TrancheNo())
					&& (pLeg->DptDate() == pCurLeg->DptDate()))
					pLeg->EditStatus (pLeg->EditStatus() | EDIT_STATUS_OKAY);
				pTranche->EditStatus (pTranche->EditStatus() &  ~EDIT_STATUS_AUTHMODNOTSENT);
				pTranche->EditStatus (pTranche->EditStatus() | EDIT_STATUS_OKAY);
				pLeg->SentFlag(ChangeSentFlag(1, pLeg->SentFlag()));
				MajTrancheSentFlag ();
			}
			break;
		}
	}

	if ( !((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetUpdateSynchroneFlag() )
	{ // 17/05/06, suite a plainte des utilisateurs sur le temps de réponse globale, on remeet en place
		// l'update désynchronisé. Mais par sécurité je rend ceci paramétrable.

		// Create a new leg domain and copy this leg (for database update)
		YmIcLegsDom* pLegBis = new YmIcLegsDom(*pLeg);
		// Update this leg in the database with the new status
		YM_Query* pUpdateQuery = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), TRUE);  // AutoDelete is TRUE
		// pUpdateQuery->AddObserver(this);	JMG le 07/01/02, notification non utilisée. limite le risque
		// de renvoi d'une notification par YM_OBSERVABLE::Notify (ymthread.cpp) si le timeout a expiré.
		pUpdateQuery->Start( pLegBis, IDS_SQL_UPDATE_LEG_LIST );

		// Create a new tranche domain and copy this tranche (for database update)
		YmIcTrancheLisDom* pTrancheBis = new YmIcTrancheLisDom(*pTranche);
		// Update the currently loaded tranche in the database with the new status
		pUpdateQuery = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), TRUE);  // AutoDelete is TRUE
		// pUpdateQuery->AddObserver(this);		JMG le 07/01/02, notification non utilisée. limite le risque
		// de renvoi d'une notification par YM_OBSERVABLE::Notify (ymthread.cpp) si le timeout a expire
		pUpdateQuery->Start( pTrancheBis, IDS_SQL_UPDATE_TRANCHE_LIST );
	}
	else
	{
		// Utilisation d'une requete synchrone de mise à jour de sc_legs pour eviter le problème
		// s'étant produiy le 25/10/2005, lorsque des deletes massifs étaient fait en base bpsn001 sur sc_legs
		// PB de desynchronisation, la query était détruite avant de passer dans le OnWorkFinished
		((CCTAApp*)APP)->UpdateOneLeg (pLeg);

		YM_Query* pUpdateQuery = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
		pUpdateQuery->SetDomain (pTranche);
		((CCTAApp*)APP)->m_pDatabase->Transact (pUpdateQuery, IDS_SQL_UPDATE_TRANCHE_LIST);
		delete pUpdateQuery;
	}
	return iVnauSentStatus;
}

// NBN ANO 79119
void CResaRailView::writeLog (CString input) {

	CTime t = CTime::GetCurrentTime();
	CString s = t.Format("%d/%m/%Y %H:%M:%S");

	ofstream file;  

    TCHAR appData[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL,
                                  CSIDL_DESKTOPDIRECTORY | CSIDL_FLAG_CREATE,
                                  NULL,
                                  SHGFP_TYPE_CURRENT,
                                  appData)))

    wcout << appData << endl;
    std::basic_ostringstream<TCHAR> file_path;
    file_path << appData << _TEXT("\\SATC.log");

	file.open(file_path.str().c_str(),std::ios_base::app);
    file<<"[RESARAIL]"+s+"-"+input+"\n";
    file.close();
}

int CResaRailView::SendVnas( CString szMsg, CString SuiteVnas, YmIcLegsDom* pLeg, BOOL bConfirm /* = TRUE */ )
{
	if (!m_bOkOnRhea)
		return 0;

	YmIcLegsDom* pCurLeg = GetLegView()->GetLoadedLeg();
	BOOL bIsCur = pCurLeg && (!strcmp(pCurLeg->LegOrig(), pLeg->LegOrig())
		&& (pLeg->TrancheNo() == pCurLeg->TrancheNo())
		&& (pLeg->DptDate() == pCurLeg->DptDate()));

	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	// Get currently loaded tranche
	YmIcTrancheLisDom* pTranche = 
		YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);

	// Send the VNAU command...
	int iVnauSentStatus = 0;

	// Build the VNAU command...
	BOOL bCML = IsCrossMidnightLeg (pTranche, pLeg);
	if (szMsg.IsEmpty())
	{
		szMsg = BuildVnas( pLeg, bCML, SuiteVnas );
		if ( szMsg.IsEmpty() )
			return 0;
	}

	if (bConfirm && ControlActDiffMoi(2))
	{
		CString s;
		s = ((CCTAApp*)APP)->GetResource()->LoadResString( IDS_ACT_DIFF_MOI );
		s += '\n';
		s += szMsg;
		if (MessageBox (s, NULL, MB_YESNO) == IDYES)
			iVnauSentStatus = ((CCTAApp*)APP)->GetResComm()->SendVnau( NULL, szMsg, FALSE, bCML );
	}
	else{
		iVnauSentStatus = ((CCTAApp*)APP)->GetResComm()->SendVnauBis( NULL, szMsg, SuiteVnas, bConfirm, bCML );
		if(szMsg.Find("/A0-0/")){
			writeLog("SendVnas:"+szMsg);
		}
		
	}
	if (iVnauSentStatus && !SuiteVnas.IsEmpty()) // si OK pour le premier VNAS, on envoi automatiquement le suivant
		((CCTAApp*)APP)->GetResComm()->SendVnau( NULL, SuiteVnas, false, bCML );

	int SumScg = GetSumScgAuth();
	switch ( iVnauSentStatus )
	{
	case 0 :  // user chose NOT to send vnau to ResaRail
		{
			pLeg->ScxSentFlag(ChangeSentFlag(0, pLeg->ScxSentFlag()));  // user did not send to ResaRail
			if (bIsCur)
				pCurLeg->ScxSentFlag(ChangeSentFlag(0, pCurLeg->ScxSentFlag()));
			pTranche->ScxSentFlag(ChangeSentFlag(0, pTranche->ScxSentFlag()));
			break;
		}
	case -1 :  // Communication socket not opened, the vnau is directly inserted in the database.
		{
			((CCTAApp*)APP)->SetRealTimeMode(FALSE);  // Turn off Real Time menu option

			pLeg->ScxSentFlag(ChangeSentFlag(1, pLeg->ScxSentFlag()));  // even though the socket was not opened, the VNAU was sent to the database
			pLeg->ScxCapa(SumScg);
			if (bIsCur)
			{
				pCurLeg->ScxSentFlag(ChangeSentFlag(1, pCurLeg->ScxSentFlag()));  
				pCurLeg->ScxCapa(SumScg);
			}
			MajTrancheScxSentFlag ();
			break;
		}
	default :  // the message number was returned, which means vnau successfully sent to ResaRail
		{
			// Don't change the status if Auths were not sent
			// otherwise, everything is okay
			if (bIsCur)
				pLeg->EditStatus (pLeg->EditStatus() | EDIT_STATUS_OKAY);
			pTranche->EditStatus (pTranche->EditStatus() | EDIT_STATUS_OKAY);
			pLeg->ScxSentFlag(ChangeSentFlag(1, pLeg->ScxSentFlag()));
			pLeg->ScxCapa(SumScg);
			if (bIsCur)
			{
				pCurLeg->ScxSentFlag(ChangeSentFlag(1, pCurLeg->ScxSentFlag()));  
				pCurLeg->ScxCapa(SumScg);
			}
			MajTrancheScxSentFlag ();
			break;
		}
	}

	((CCTAApp*)APP)->UpdateOneLeg (pLeg);
	YM_Query* pUpdateQuery = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
	pUpdateQuery->SetDomain (pTranche);
	((CCTAApp*)APP)->m_pDatabase->Transact (pUpdateQuery, IDS_SQL_UPDATE_TRANCHE_LIST);
	delete pUpdateQuery;

	return iVnauSentStatus;
}

int CResaRailView::SendVnae()
{
	if (!m_bOkOnRhea)
		return 0;

	// Send the VNAU command...
	int iVnaeSentStatus = 0;

	CString szMsg, SuiteVnae;
	szMsg = BuildVnae (SuiteVnae);
	if (szMsg.IsEmpty())
		return 0;

	if(szMsg.Find("/A0-0/")){
		writeLog("SendVnae:"+szMsg);
	}

	iVnaeSentStatus = ((CCTAApp*)APP)->GetResComm()->SendVnauBis( NULL, szMsg, SuiteVnae, TRUE);

	if (iVnaeSentStatus && !SuiteVnae.IsEmpty()) // si OK pour le premier VNAE, on envoi automatiquement le suivant
		((CCTAApp*)APP)->GetResComm()->SendVnau( NULL, SuiteVnae, FALSE );

	return iVnaeSentStatus;
}


int CResaRailView::SendVnl( BOOL bDepartsNextDay /* = FALSE */ )
{
	// Build the VNL command
	if (!bDepartsNextDay) // VNL sur toute la desserte, on vide la table m_LegScg. DM 5882.1
		m_LegScg.RemoveAll();

	// NPI - Correction formatage jour du départ
	/*char szItoa[10];
	CString szMsg = "VNL";

	// Tranche number
	YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	itoa (pTranche->TrancheNo(), szItoa, 10);
	szMsg += szItoa;
	szMsg += '/';

	// Day of month
	RWDate rwDate = pTranche->DptDateTr();
	if ( bDepartsNextDay )
		rwDate++;  // if the departs next day flag is set, we must increment the dpt date
	int iDay = rwDate.dayOfMonth();
	itoa ( iDay, szItoa, 10 );
	szMsg += szItoa;

	// Month name
	szMsg += MONTH_LIST_ENGLISH.Mid( (rwDate.month()-1)*3, 3 );

	// if the departs next day flag is set, we increment the date, and add the leg orig/dest to the vnl
	//  this is done because the tranche may not actually depart on the following day, but the leg does
	if ( bDepartsNextDay )
	{
		// Leg Origin / Destination
		YmIcLegsDom* pLeg = YM_Set<YmIcLegsDom>::FromKey( LEG_KEY);
		szMsg += pLeg->LegOrig();
		szMsg += pLeg->LegDest();
	}
	szMsg += "/SC"; // On demande systématiquement un éventuel sous-contingent, sans effet si absent.*/

	YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	RWDate rwDate = pTranche->DptDateTr();
	if ( bDepartsNextDay )
		rwDate++;  // if the departs next day flag is set, we must increment the dpt date

	CString szMsg = "";

	// if the departs next day flag is set, we increment the date, and add the leg orig/dest to the vnl
	//  this is done because the tranche may not actually depart on the following day, but the leg does
	if (bDepartsNextDay)
	{
		// Leg Origin / Destination
		YmIcLegsDom* pLeg = YM_Set<YmIcLegsDom>::FromKey( LEG_KEY);
		szMsg.Format("VNL%ld/%02d%s%s%s/SC", pTranche->TrancheNo(), rwDate.dayOfMonth(),  MONTH_LIST_ENGLISH.Mid((rwDate.month() - 1) * 3, 3), pLeg->LegOrig(), pLeg->LegDest());
	}
	else
	{
		szMsg.Format("VNL%ld/%02d%s/SC", pTranche->TrancheNo(), rwDate.dayOfMonth(),  MONTH_LIST_ENGLISH.Mid((rwDate.month() - 1) * 3, 3));
	}
	///


	// Send the VNL command


  /// DM-7978 - CLE
  CYMVnlBouchon::getInstance()->setMonoTranche();
  /// DM-7978 - CLE

	int iVnlSentStatus = 0;
	iVnlSentStatus = ((CCTAApp*)APP)->GetResComm()->SendVnl( this, szMsg );
	if ( iVnlSentStatus == -1)  // Check the status
	{ 
		// Communication socket not opened.
		CString s;
		s = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NO_SOCKETOPEN);
		MessageBox( s,  MB_APPLMODAL );

		// Turn off Real Time menu option
		((CCTAApp*)APP)->SetRealTimeMode(FALSE);
	}
	m_iMsgId = iVnlSentStatus;
	return iVnlSentStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CResaRailView diagnostics

#ifdef _DEBUG
void CResaRailView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CResaRailView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CCTADoc* CResaRailView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCTADoc)));
	return (CCTADoc*)m_pDocument;
}

#endif //_DEBUG

//DM7978 - LME - YI-5401 - ajout des onglets global/réduit

int CResaRailView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//int ret = CView::OnCreate(lpCreateStruct);
	int ret = CScrollView::OnCreate(lpCreateStruct);
	if (ret == -1)
		return ret;

	CRect rect(0,0,0,0);
	//GetClientRect(&rect);
	if (! m_TabCtrl.Create( TCS_BUTTONS | TCS_FOCUSONBUTTONDOWN | TCS_FIXEDWIDTH | WS_VISIBLE, rect, this, IDC_RESARAIL_TABCTRL ))
		return -1;

	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;

	// Creation de l'onglet Global
	CString  szGlobal = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESA_TAB_GLOBAL);
	TabCtrlItem.pszText = (LPSTR)(LPCSTR)szGlobal;
	TabCtrlItem.cchTextMax = szGlobal.GetLength();
	m_TabCtrl.InsertItem( 0, &TabCtrlItem );

	// Creation de l'onglet Reduit
	CString  szReduit = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESA_TAB_REDUIT);
	TabCtrlItem.pszText = (LPSTR)(LPCSTR)szReduit;
	TabCtrlItem.cchTextMax = szReduit.GetLength();
	m_TabCtrl.InsertItem( 1, &TabCtrlItem );

	// Selectionne l'onglet Global par défaut
	m_TabCtrl.SetCurSel(0);  
	m_eButtonClicked = eOverallClicked;

	return ret;
}

//DM7978 - LME - FIN

void CResaRailView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();

	//DM7978 - LME - YI-5401 - ajout des onglets global/réduit
	m_TabCtrl.SetFont( ((CCTAApp*)APP)->GetDocument()->GetTitleFont(NULL) );

	CRect rect;
	GetClientRect(&rect);
	rect.bottom = TABCTRL_HEIGHT;
	m_TabCtrl.MoveWindow(rect);
	m_TabCtrl.AdjustRect( FALSE, rect );
	m_TabCtrl.Invalidate(TRUE);
	//DM7978 - LME - FIN

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	// Get the 2 record sets
	YM_RecordSet* pResaBktSet = YM_Set<YmIcResaBktDom>::FromKey(RESABKT_KEY);
	YM_RecordSet* pResaCmptSet = YM_Set<YmIcResaCmptDom>::FromKey(RESACMPT_KEY);
	YM_RecordSet* pAscTarifSet = YM_Set<YmIcAscTarifDom>::FromKey(ASC_TARIF_KEY);
	
	// NBN 79001 - 83246
	CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
	if (pTrancheView)
	{
		//Begin Duplicated from MainFrm
		// TDR - Anomalie 78938
		pTrancheView->SetIsRefresh(true);
		//SRE 72153 Save the Top Index position, before the list is cleared
		pTrancheView->SaveSelectionPosition();
		//-- End Duplicated from MainFrm

		pTrancheView->TrancheReload(true);
	}

	ASSERT( pResaBktSet != NULL );
	ASSERT( pResaCmptSet != NULL );


	// Ensure the 2 record sets are open
	if( ! pResaBktSet->IsOpen() )
		pResaBktSet->Open();
	if( ! pResaCmptSet->IsOpen() )
		pResaCmptSet->Open();

	// DM-7978 - LME - YI-6046
	if(!pAscTarifSet->IsOpen())
		pAscTarifSet->Open();

	pAscTarifSet->AddObserver( this );
	// DM-7978 - LME - FIN

	// Ask for notification from these RecordSets
	pResaBktSet->AddObserver( this );
	pResaCmptSet->AddObserver( this );


	CString szClient = App().m_pDatabase->GetClientParameter();

	CTrancheView *tranche = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();

	TRACE("ScxFlag = %d\n", ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSCXFlag());

	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSCXFlag() || tranche->getCurrentTrancheNature() == 3)
	{ // les SCI
		YM_RecordSet* pResaBktSciSet = YM_Set<YmIcResaBktDom>::FromKey(RESABKT_SCI_KEY);
		YM_RecordSet* pResaCmptSciSet = YM_Set<YmIcResaCmptDom>::FromKey(RESACMPT_SCI_KEY);

		ASSERT( pResaBktSciSet != NULL );
		ASSERT( pResaCmptSciSet != NULL );

		if( ! pResaBktSciSet->IsOpen() )
			pResaBktSciSet->Open();
		if( ! pResaCmptSciSet->IsOpen() )
			pResaCmptSciSet->Open();
		// Ask for notification from these RecordSets
		pResaBktSciSet->AddObserver( this );
		pResaCmptSciSet->AddObserver( this );
	}

	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
	{ // Les previsions
		YM_RecordSet* pPrevisionSet = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY);
		ASSERT( pPrevisionSet != NULL );
		if( ! pPrevisionSet->IsOpen() )
			pPrevisionSet->Open();
		pPrevisionSet->AddObserver( this );

		YM_RecordSet* pVnauExcluSet = YM_Set<YmIcVnauDom>::FromKey(VNAU_EXCLU_KEY);
		ASSERT( pVnauExcluSet != NULL );
		if( ! pVnauExcluSet->IsOpen() )
			pVnauExcluSet->Open();
		pVnauExcluSet->AddObserver( this );
	}

	CString titi, toto;
	titi = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_RESARAIL_TITLE);
	GetParentFrame()->GetWindowText(toto);
	GetParentFrame()->SetWindowText(titi);
	((YM_PersistentChildWnd*)GetParentFrame())->SetChildKey (toto);
	((CCTAApp*)APP)->SetRealTimeMode (((CCTAApp*)APP)->GetRealTimeMode());

	SetStatusBarText(IDS_READ_DATA);  // set text as finished processing data
	RestoreViewConfig();

	m_bInitialUpdate = TRUE;
}

void CResaRailView::OnUpdateFromResaMsg(CView* pSender, LPARAM lHint, CObject* pHint)
{
	YM_Msg* pMsg = (YM_Msg*)pHint;
	m_bOkOnRhea = TRUE;

	if ( pMsg->GetNat() == YM_Msg::MSG_VNL )  // is this a nofification for VNL results
	{
		// must check to ensure that this is the results of the last VNL which was sent
		if ( pMsg->GetNum() == m_iMsgId  || m_bVnlResent )  // is this the results of the last one sent ?
		{
			// Vnl object is old, so ensure it's deleted
			if (m_pVnl)
			{
				delete m_pVnl;
				m_pVnl = NULL;
				m_pVnlLeg = NULL;
			}

			// parse the vnl object
			m_pVnl = new YM_Vnl;
			if ((pMsg->GetType() != YM_Msg::SOC_REP_OK) ||
				( m_pVnl->Parse(pMsg->GetMsg()) == VNL_PARSE_ERROR ))
			{
				// display the vnl results (error) on the status bar
				SetStatusBarText(
					((CCTAApp*)APP)->GetResource()->LoadResString(IDS_VNL_ERROR) + 
					": " + pMsg->GetMsg());

				if ( m_pVnl )  // delete the vnl object
				{
					delete m_pVnl;
					m_pVnl = NULL;
					m_pVnlLeg = NULL;
				}
				//          AfxMessageBox( ("Unable to parse VNL results:\n" + pMsg->GetMsg()) );
			}
			else  // VNL results look good...
			{
				// if this leg departs next day (tranche crosses midnight) we must re-send the VNL with dpt date+1
				// the vnl contains all legs for this tranche.  we must find the currently selected leg.
				YmIcLegsDom* pLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
				SetVnlLeg( pLeg->LegOrig(), pLeg->LegDest() );
				InitLegScg(); // initialisation de m_LegScg si vide.
				if ( m_pVnlLeg == NULL )
					return;  // we didn't find this orig/dest, get the hell out

				if ( m_pVnlLeg->DepartsNextDay() && ! m_bVnlResent )
				{
					SendVnl( TRUE );  // send it again
					m_bVnlResent = TRUE;  // signal that we don't send the vnl again (avoid infinite looping)
				}
				else
				{
					m_bVnlResent = FALSE;  // reset, don't send it again

					// re-calculate the current, forecast, and original forecast availabilities 
					//  based upon reshold values from ResaRail
					CtrlEtancheOnVnl();
					CalcAvail(TRUE);
					CalcAvailSci(TRUE);
					if (m_bVnauExcluOK)
						CalcVnauExcluAvail();
					if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetRealTimeInRRDHistFlag())
					{ // DM 5350 : Recuperation de la fenetre RRGHistView pour relancer les builds
						CRRDHistView* pCRRDHistView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetRRDHistView(FALSE);
						if (pCRRDHistView)
							pCRRDHistView->PostMessage (WM_REFRESH_ALL);
						//pCRRDHistView->RefreshAll(); 

						if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetYieldRevenuFlag())
						{
							CRecDHistView* pCRecDHistView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetRecDHistView(FALSE);
							if (pCRecDHistView)
								pCRecDHistView->PostMessage (WM_REFRESH_ALL);
							//pCRecDHistView->RefreshAll();
						}
					}

					SetStatusBarText(IDS_DONE);  // set text as finished processing data
				}
			}
		}
		else
		{
			// don't invalidate and update the window...
			return;
		}
	}
}

void CResaRailView::OnUpdateFromRecordSet(CView* pSender, LPARAM lHint, CObject* pHint)
{
	m_bOkOnRhea = TRUE;
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	CTrancheView *tranche = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();

	//    m_bConfirmUnderbooking = TRUE;  // reset the boolean to ask confirmation

	// Get the 2 record sets
	YM_RecordSet* pResaBktSet = YM_Set<YmIcResaBktDom>::FromKey(RESABKT_KEY);
	YM_RecordSet* pResaCmptSet = YM_Set<YmIcResaCmptDom>::FromKey(RESACMPT_KEY);
	YM_RecordSet* pResaBktSciSet = YM_Set<YmIcResaBktDom>::FromKey(RESABKT_SCI_KEY);
	YM_RecordSet* pResaCmptSciSet = YM_Set<YmIcResaCmptDom>::FromKey(RESACMPT_SCI_KEY);
	YM_RecordSet* pPrevisionSet = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY);
	YM_RecordSet* pVnauExcluSet = YM_Set<YmIcVnauDom>::FromKey(VNAU_EXCLU_KEY);

	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSCXFlag() || tranche->getCurrentTrancheNature() == 3)//SRE - DM8029 - 89383 - force la lecture des SC si besoin est
	{ // les SCI
		if( ! pResaBktSciSet->IsOpen() )
		{
			pResaBktSciSet->Open();
			pResaBktSciSet->AddObserver( this );

		}
		if( ! pResaCmptSciSet->IsOpen() )
		{
			pResaCmptSciSet->Open();
			pResaCmptSciSet->AddObserver( this );
		}
	}
	else//SRE - DM8029 - 89383 - Force la fermeture des SC si pas de SC ou si tranche (mode ALLEO) != nature 3 (SCXFlag = false en mode ALLEO)
	{
		pResaBktSciSet->Close();
		pResaCmptSciSet->Close();
	}

	// DM7978 - LME - YI-6404
	m_ascTarifaire = 0;	//KGH
	YM_Iterator<YmIcAscTarifDom>* pAscTarif = YM_Set<YmIcAscTarifDom>::FromKey(ASC_TARIF_KEY)->CreateIterator();
	for(pAscTarif->First(); !pAscTarif->Finished(); pAscTarif->Next())
	{
		YmIcAscTarifDom* dom = pAscTarif->Current();//KGH
		m_ascTarifaire = dom->getCcPremiere();//KGH
	}
	// DM7978 - LME - FIN

	YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
	CString szClient = App().m_pDatabase->GetClientParameter();

	
	// CME - Ano 89383
	bool YaSci = ((((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSCXFlag() ||tranche->getCurrentTrancheNature() == 3) 
		&& pTranche->ScxCount() > 0);
	TRACE("tranche nature = %d, scx = %d\n", tranche->getCurrentTrancheNature(), pTranche->ScxCount());

	bool YaPrevision = ((((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag()) != 0);

	if (!YaSci && ((pHint == (CObject*)pResaBktSciSet) || (pHint == (CObject*)pResaCmptSciSet)))
		return;

	// Populate or empty internal structure
	if( (pResaBktSet->IsValid() && pResaBktSet->GetStatus() == YM_RecordSet::SUCCESS) && 
		(pResaCmptSet->IsValid() && pResaCmptSet->GetStatus() == YM_RecordSet::SUCCESS) &&
		(!YaSci || (pResaCmptSciSet->IsValid() && pResaCmptSciSet->GetStatus() == YM_RecordSet::SUCCESS)) &&
		(!YaSci || (pResaBktSciSet->IsValid() && pResaBktSciSet->GetStatus() == YM_RecordSet::SUCCESS)) &&
		(!YaPrevision || (pPrevisionSet->IsValid() && pVnauExcluSet->IsValid()) ))
	{
		if (YaPrevision)
		{
			YM_Iterator<YmIcResaBktDom>* pIteratorPrevision = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY)->CreateIterator();
			m_bPrevisionOK = (pIteratorPrevision->GetCount() > 0);
			delete pIteratorPrevision;

			YM_Iterator<YmIcVnauDom>* pIteratorVnauExclu = YM_Set<YmIcVnauDom>::FromKey(VNAU_EXCLU_KEY)->CreateIterator();
			m_bVnauExcluOK = (pIteratorVnauExclu->GetCount() > 0);
			delete pIteratorVnauExclu;
		}

		// Declare the iterators for each record set
		YM_Iterator<YmIcResaBktDom>* pIteratorBkt = YM_Set<YmIcResaBktDom>::FromKey(RESABKT_KEY)->CreateIterator();
		YM_Iterator<YmIcResaCmptDom>* pIteratorCmpt = YM_Set<YmIcResaCmptDom>::FromKey(RESACMPT_KEY)->CreateIterator();
		UINT nID = ID_ES_BASE_BKT;
		m_CActMoi.Empty();
		m_SciCActMoi.Empty();

		// Do we have some data from the queries ?
		if( (!pIteratorBkt->GetCount()) || (!pIteratorCmpt->GetCount()) )
		{
			// Set text in status bar control
			SetStatusBarText(IDS_NO_DATA);
			// Empty ResaBkt part of internal structure used for drawing
			m_CmptBktMap.RemoveAllResaBkt();
			m_CmptBktSciMap.RemoveAllResaBkt();
			m_pWndChildFocusSaved = NULL;
			m_EditMap.RemoveAll();
			m_EditSciMap.RemoveAll();
			// Empty ResaCmpt part of internal structure used for drawing
			m_CmptBktMap.RemoveAllResaCmpt();
			m_CmptBktSciMap.RemoveAllResaCmpt();

			// Vnl object is no good, so make sure it's deleted
			if (m_pVnl)
			{
				delete m_pVnl;
				m_pVnl = NULL;
				m_pVnlLeg = NULL;
				m_iMsgId = 0;
			}
		}
		else  // We have data !
		{
			// DM 5480 Train auto, vérification du compartiment
			YmIcResaCmptDom * pResaCmpt;
			int maxValAuth = 99999;
			BOOL bRabote = FALSE;
			int ovb, unb, ovg;
			m_capatotale = 0;
			CTrancheView * pTrancheView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
			if (!pTrancheView)
				return;
			YmIcTrancheLisDom* pLoadedTranche = pTrancheView->GetLoadedTranche();

			for( pIteratorCmpt->First(); !pIteratorCmpt->Finished(); pIteratorCmpt->Next() )
			{
				pResaCmpt= pIteratorCmpt->Current();
				if (!strcmp(pResaCmpt->Cmpt(), "O"))
					maxValAuth = pResaCmpt->Capacity();
				m_capatotale += pResaCmpt->Capacity();
				if (!pDoc->GetEntitySurSousResa (pLoadedTranche->Entity(), pResaCmpt->Cmpt(), ovb, unb, ovg, true))
				{
					ovb = 112;
					unb = 100;
					ovg = 200;
				}
				m_SurResa[(int)(pResaCmpt->Cmpt()[0] - 'A')] = ovb;
				m_SousResa[(int)(pResaCmpt->Cmpt()[0] - 'A')] = unb;
				m_SurResaGlob = ovg;
			}     

			// Iterate through the ResaBkt (bucket level data)
			BOOL bNewCmpt = FALSE;
			char szCmptHold[20];
			szCmptHold[0] = '\0';
			YmIcResaBktDom* pResaBkt = NULL;
			for( pIteratorBkt->First(); !pIteratorBkt->Finished(); pIteratorBkt->Next() )
			{
				pResaBkt = pIteratorBkt->Current();		

				//DM 5480 Train Auto,
				BOOL bModifRabot = FALSE;
				if ((!strcmp(pResaBkt->Cmpt(), "O")) && (pResaBkt->AuthCur() > maxValAuth))
				{
					if (pResaBkt->NestLevel() == 0)
					{
						CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_REDUCE_AUTH_AUTOTRAIN);
						if (AfxMessageBox (szText, MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION ) == IDYES )
							bRabote = TRUE;
					}
					if (bRabote)
					{
						pResaBkt->AuthUser(maxValAuth);
						bModifRabot = TRUE;
					}
				}

				// If AuthUser is NULL, set it to the default value of AuthFcOrg
				if (pResaBkt->AuthUserIsNull())
					pResaBkt->AuthUser(pResaBkt->AuthFcOrg());

				// is this a new compartment ?
				if ( strcmp( pResaBkt->Cmpt(), szCmptHold ) )  // new compartment ?
				{
					bNewCmpt = TRUE;
					strcpy( szCmptHold, pResaBkt->Cmpt() );  // reset the cmpt hold variable
				}
				else
					bNewCmpt = FALSE;

				// Create a new entry in the cmpt/bkt map (bucket level)
				CResaCmptBktLine* pCmptBktLine = m_CmptBktMap.AddResaBkt( this, pResaBkt, nID );

				//DM 5480 train auto
				if (bModifRabot)
					pCmptBktLine->SetAuthModify (TRUE);

				nID += ID_ES_OFFSET_BKT;
			}

			// Fill internal structure used for drawing
			// Iterate throught the compartment level data
			for( pIteratorCmpt->First(); !pIteratorCmpt->Finished(); pIteratorCmpt->Next() )
			{
				// Create a new entry in the cmpt/bkt map (compartment level)
				CResaCmptBkt* pCmptBkt = m_CmptBktMap.AddResaCmpt( pIteratorCmpt->Current() );
				pCmptBkt->AccumAddDmd();
			}
			// accumulate additional demand
			m_CmptBktMap.AccumAddDmd();

			// calcul dispo buckets
			CalcAvail(TRUE);
		}
		delete pIteratorBkt;
		delete pIteratorCmpt;

		//Maintenant on charge les SCi.
		// Declare the iterators for each record set
		YM_Iterator<YmIcResaBktDom>* pIteratorBktSci = YM_Set<YmIcResaBktDom>::FromKey(RESABKT_SCI_KEY)->CreateIterator();
		YM_Iterator<YmIcResaCmptDom>* pIteratorCmptSci = YM_Set<YmIcResaCmptDom>::FromKey(RESACMPT_SCI_KEY)->CreateIterator();

		if (YaSci && pIteratorBktSci->GetCount() && pIteratorCmptSci->GetCount())
		{ // Il nous faut des données sur les deux requêtes SCI
			// Iterate through the ResaBkt (bucket level data)
			BOOL bNewCmpt = FALSE;
			char szCmptHold[20];
			szCmptHold[0] = '\0';
			// Iterate throught the compartment level data
			//if (tranche->getCurrentTrancheNature() == 2 && ((CCTAApp*)APP)->GetRhealysFlag()) 
				for( pIteratorCmptSci->First(); !pIteratorCmptSci->Finished(); pIteratorCmptSci->Next() )
				{
					// Create a new entry in the cmpt/bkt map (compartment level)
					CResaCmptBktSci* pCmptBkt = m_CmptBktSciMap.AddResaCmpt( this, pIteratorCmptSci->Current(), nID );
					nID += ID_ES_OFFSET_BKT;
				}

			YmIcResaBktDom* pResaBkt = NULL;
			CTrancheView * tranche = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
			if (!((CCTAApp*)APP)->GetRhealysFlag())//DM8029 !!
			{				
				for( pIteratorBktSci->First(); !pIteratorBktSci->Finished(); pIteratorBktSci->Next() )
				{
					pResaBkt = pIteratorBktSci->Current();
					// is this a new compartment ?
					if ( strcmp( pResaBkt->Cmpt(), szCmptHold ) )  // new compartment ?
					{
						bNewCmpt = TRUE;
						strcpy( szCmptHold, pResaBkt->Cmpt() );  // reset the cmpt hold variable
					}
					else
						bNewCmpt = FALSE;

					// Create a new entry in the cmpt/bkt map (bucket level)
					TRACE("RhealysFlag = %d\n", (((CCTAApp*)APP)->GetRhealysFlag()));
					
					// NBN - YI-11632 - Ano 89569
					// Visibilité du SCG uniquement sur la nature 3 (Revient à afficher pour sc1 la visibilité des informations standards de la fenêtre : Vdu, Res, Act, Moi, Dispo).		
					if (tranche->getCurrentTrancheNature() != 3 || pResaBkt->NestLevel() == 1){
						CResaCmptBktSciLine* pCmptBktLine = m_CmptBktSciMap.AddResaBkt( this, pResaBkt, nID );
						nID += ID_ES_OFFSET_BKT;
					}


				}
				CalcAvailSci(TRUE);
			}
		}
		delete pIteratorBktSci;
		delete pIteratorCmptSci;

		CalculateSize();

		// NOMOS ignored info update
		if (m_bVnauExcluOK)
		{
			GetVnauExcluAuth();		// lecture des autorisations ignorées
			CalcVnauExcluAvail();		// calcul des dispo correspondantes, sans le VNL
		}

		// Now it's time to issue the vnl command  
		if ( ((CCTAApp*)APP)->GetRealTimeMode())
		{
			SendVnl();
			if ( m_iMsgId == -1)
				// Set text in status bar control
				SetStatusBarText(IDS_DONE);
			else
				// Set text in status bar control
				SetStatusBarText(IDS_SENDING_VNL);
		}
		else
		{
			// Set text in status bar control
			SetStatusBarText(IDS_DONE);
		}   
	}
	else
	{
		// Set text in status bar control
		SetStatusBarText(IDS_READ_DATA);

		// Empty ResaBkt part of internal structure used for drawing
		m_CmptBktMap.RemoveAllResaBkt();
		m_CmptBktSciMap.RemoveAllResaBkt();
		m_pWndChildFocusSaved = NULL;
		m_EditMap.RemoveAll();
		m_EditSciMap.RemoveAll();

		// Empty ResaCmpt part of internal structure used for drawing
		m_CmptBktMap.RemoveAllResaCmpt();
		m_CmptBktSciMap.RemoveAllResaCmpt();
		m_bPrevisionOK = FALSE;

		// Vnl object is no good, so make sure it's deleted
		if (m_pVnl)
		{
			delete m_pVnl;
			m_pVnl = NULL;
			m_pVnlLeg = NULL;
			m_iMsgId = 0;
		}
	}

	ReorganizeTabOrder();
}
void CResaRailView::CalculateSize()
{
	// NPI - Ano 71815
	/*// Calculate size
	long nLigns = 0; 
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	long nCols = 7;

	if (m_bPrevisionOK)
		nCols += 2; // prevision resa et revenu

	if (m_bVnauExcluOK)
		nCols += 2; // autorisation et dispo nomos ignorées

	if (m_pVnl)
		nCols++;

	if (m_CmptBktSciMap.GetCount())
	{
		nCols += 8; // il y a SCI. et on reserve la place du temps réel
		if (m_bPrevisionOK)
			nCols += 2; // prevision resa et revenu
		if (m_bVnauExcluOK)
			nCols += 2; // décalage
	}

	// Compute font size for Scrollbar initialization
	TEXTMETRIC  tm;
	CClientDC dc(this);
	CFont* pFontSaved = dc.SelectObject( ((CCTAApp*)APP)->GetDocument()->GetStdFont(NULL) );
	dc.GetTextMetrics( &tm );
	dc.SelectObject( pFontSaved );

	POSITION pos = m_CmptBktMap.GetStartPosition();
	int nblhf;
	nblhf = 5;

	while( pos != NULL )
	{
		CString   sKey;
		CResaCmptBkt* pCmptBkt;
		m_CmptBktMap.GetNextAssoc( pos, sKey, pCmptBkt );
		if (pCmptBkt)
		{
			CResaCmptBktSci* pCmptBktSci = NULL;
			m_CmptBktSciMap.Lookup(sKey.MakeLower(), pCmptBktSci);
			int nbl = 0;
			if (pCmptBktSci)
				nbl = pCmptBktSci->m_CmptBktLineArray.GetSize();
			nLigns += nblhf * (tm.tmHeight + tm.tmExternalLeading);  // nblhf ligns to draw Header and footer for a cmpt
			nLigns += (3 * OFFSET_UNDERLINE);
			if (pCmptBkt->m_CmptBktLineArray.GetSize() > nbl)
				nbl = pCmptBkt->m_CmptBktLineArray.GetSize();
			nLigns += ( nbl * (tm.tmHeight + tm.tmExternalLeading)); // 1 lign by bucket 
		}
	}
	CSize szTotal( tm.tmAveCharWidth * 7 * nCols - (nCols*2), nLigns  );

	// Adjust Scrollbars
	SetScrollSizes( MM_TEXT, szTotal );*/

	// Calculate size

	// Nombre de colonnes
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	long nCols = 7;

	if (m_bPrevisionOK)
		nCols += 2; // prevision resa et revenu

	if (m_bVnauExcluOK)
		nCols += 2; // autorisation et dispo nomos ignorées

	if (m_pVnl)
		nCols++;

	nCols = nCols * m_CmptBktMap.GetCount();

	// Nombre de lignes
	// Compute font size for Scrollbar initialization
	TEXTMETRIC  tm;
	CClientDC dc(this);
	CFont* pFontSaved = dc.SelectObject( ((CCTAApp*)APP)->GetDocument()->GetStdFont(NULL) );
	dc.GetTextMetrics( &tm );
	dc.SelectObject( pFontSaved );

	POSITION pos = m_CmptBktMap.GetStartPosition();
	int nblhf = 5;

	long nLigns = 0;
	while( pos != NULL )
	{
		long nLocalLigns = 0;
		CString sKey;
		CResaCmptBkt* pCmptBkt;
		m_CmptBktMap.GetNextAssoc( pos, sKey, pCmptBkt );
		if (pCmptBkt)
		{
			// Nombre de lignes de la bucket
			nLocalLigns += ( pCmptBkt->m_CmptBktLineArray.GetSize() * (tm.tmHeight + tm.tmExternalLeading)); // 1 lign by bucket 
			nLocalLigns += nblhf * (tm.tmHeight + tm.tmExternalLeading);  // nblhf ligns to draw Header and footer for a cmpt
			nLocalLigns += (3 * OFFSET_UNDERLINE);

			nLocalLigns += (OFFSETY * 3);

			// Nombre de scx
			CResaCmptBktSci* pCmptBktSci = NULL;
			m_CmptBktSciMap.Lookup(sKey.MakeLower(), pCmptBktSci);
			if (pCmptBktSci)
				nLocalLigns += (pCmptBktSci->m_CmptBktLineArray.GetSize() * (tm.tmHeight + tm.tmExternalLeading)); // 1 lign by SCx 
			nLocalLigns += nblhf * (tm.tmHeight + tm.tmExternalLeading);  // nblhf ligns to draw Header and footer for a scx
			nLocalLigns += (3 * OFFSET_UNDERLINE);
		}

		if (nLigns < nLocalLigns)
		{
			nLigns = nLocalLigns;
		}
	}
	CSize szTotal(tm.tmAveCharWidth * 7 * nCols, nLigns);

	// Adjust Scrollbars
	SetScrollSizes( MM_TEXT, szTotal );

	Resize();
	///
}

void CResaRailView::ChangeSizeForSci(BOOL bForce)
{
	// DM7978 - LME - YI-5401 - masquaque du bloc sous-contingent par clic-droit
	if (!bForce && (m_bSciHeight == (m_CmptBktSciMap.GetCount() > 0)))
		return;

	if (!m_bSciHeight && !m_CmptBktSciMap.GetCount())
		return;

	CRect resarect(0, 0, 0, 0);
	GetParentFrame()->GetWindowRect(&resarect);
	CPoint pt(0,0);
	((CCTAApp*)APP)->GetMainWnd()->ClientToScreen (&pt);
	CRect memorect (resarect.left - pt.x, resarect.top - pt.y,
		resarect.right - pt.x, resarect.bottom - pt.y);
	// NPI - Ano 71815
	//CSize szTotal = m_SizeView;
	CSize szTotal = GetTotalSize();

	memorect.left -= 2;
	memorect.top -= 2;
	///

	// Si masquage active et affichage des sci
	if (bForce && m_bSciHeight)
	{
		// NPI - Ano 71815
		/*memorect.left -= 2;
		memorect.top -= 2;
		memorect.right = memorect.left + szTotal.cx;
		memorect.bottom = memorect.top + szTotal.cy + 100;
		GetParentFrame()->MoveWindow (&memorect);
		m_bSciHeight = FALSE;*/

		memorect.right = memorect.left + szTotal.cx + 50;
		memorect.bottom = memorect.top + szTotal.cy + 50;
		GetParentFrame()->MoveWindow (&memorect);
		m_bSciHeight = FALSE;
		///
	}
	// Si masquage active et masquage des sci
	//DM7978 - LME - YI-5401 
	else if (bForce && !m_bSciHeight)
	{
		// NPI - Ano 71815
		/*memorect.left -= 2;
		memorect.top -= 2;
		memorect.right = memorect.left + szTotal.cx;
		memorect.bottom = memorect.top + m_lYSciNotDisplayed + 50;
		GetParentFrame()->MoveWindow (&memorect);
		m_bSciHeight = TRUE;*/

		memorect.right = memorect.left + szTotal.cx + 50;
		memorect.bottom = memorect.top + (szTotal.cy / 2) + 50;
		GetParentFrame()->MoveWindow (&memorect);
		m_bSciHeight = TRUE;
		///
	}
	//DM7978 - LME - FIN
}

bool CResaRailView::IsValidCmptBktSet(YM_RecordSet* pResaBktSet,YM_RecordSet* pResaCmptSet, YM_RecordSet* pResaBktSciSet, YM_RecordSet* pResaCmptSciSet, YM_RecordSet* pPrevisionSet, YM_RecordSet* pVnauExcluSet, bool YaSci, bool YaPrevision) 
{
	return (pResaBktSet->IsValid() && pResaBktSet->GetStatus() == YM_RecordSet::SUCCESS) && 
			(pResaCmptSet->IsValid() && pResaCmptSet->GetStatus() == YM_RecordSet::SUCCESS) &&
			(!YaSci || (pResaCmptSciSet->IsValid() && pResaCmptSciSet->GetStatus() == YM_RecordSet::SUCCESS)) &&
			(!YaSci || (pResaBktSciSet->IsValid() && pResaBktSciSet->GetStatus() == YM_RecordSet::SUCCESS)) &&
			(!YaPrevision || (pPrevisionSet->IsValid() && pVnauExcluSet->IsValid()) );
}

void CResaRailView::NettoyerBktAndBktSciMap() 
{
	m_CmptBktMap.RemoveAll();
	m_CmptBktSciMap.RemoveAll(); // suppresion de toutes les lignes
}

void CResaRailView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	//pour eviter double affichage
	NettoyerBktAndBktSciMap();

	BOOL bInvalidate = FALSE;
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	// We are expecting a notification from either YM_RecordSet
	//   or YM_Msg (response of the VNL command issued to ResaRail)
	//
	if( pHint == NULL)
		return;

	// don't respond to any OnUpdate messages except for the following...
	if  ( ! pHint->IsKindOf( RUNTIME_CLASS(YM_RecordSet) )  &&
		! pHint->IsKindOf( RUNTIME_CLASS(YM_Msg) ) )
		return;

	// Get all the record sets
	YM_RecordSet* pResaBktSet = YM_Set<YmIcResaBktDom>::FromKey(RESABKT_KEY);
	YM_RecordSet* pResaCmptSet = YM_Set<YmIcResaCmptDom>::FromKey(RESACMPT_KEY);
	YM_RecordSet* pResaBktSciSet = YM_Set<YmIcResaBktDom>::FromKey(RESABKT_SCI_KEY);
	YM_RecordSet* pResaCmptSciSet = YM_Set<YmIcResaCmptDom>::FromKey(RESACMPT_SCI_KEY);
	YM_RecordSet* pPrevisionSet = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY);
	YM_RecordSet* pVnauExcluSet = YM_Set<YmIcVnauDom>::FromKey(VNAU_EXCLU_KEY);
	// DM7978 - LME - YI-6404
	YM_RecordSet* pAscTarifSet = YM_Set<YmIcAscTarifDom>::FromKey(ASC_TARIF_KEY);

	if( pHint->IsKindOf( RUNTIME_CLASS(YM_RecordSet) ) )
	{
		YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);

		CTrancheView *tranche = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();

		bool YaSci = ((((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSCXFlag() || tranche->getCurrentTrancheNature() == 1 ||
		tranche->getCurrentTrancheNature() == 2) && pTranche->ScxCount() > 0);

		bool YaPrevision = ((((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag()) != 0);
		// Populate or empty internal structure

		if(IsValidCmptBktSet( pResaBktSet,
							  pResaCmptSet,
							  pResaBktSciSet,
							  pResaCmptSciSet,
							  pPrevisionSet,
							  pVnauExcluSet, 
							  YaSci,
							  YaPrevision) != m_bIsValid)
		{
		m_bIsValid = !m_bIsValid;
		bInvalidate = TRUE;
		}	
		OnUpdateFromRecordSet(pSender, lHint, pHint);
	}
	else  // it wasn't a record set which sent the notification
	{
		if( pHint->IsKindOf( RUNTIME_CLASS(YM_Msg) ) )  // VNL results ?
		{
			OnUpdateFromResaMsg(pSender, lHint, pHint);
			bInvalidate = TRUE;
		}
	}
	if ( ! m_bVnlResent )
	{
		// 27/04/01 remplacement de FALSE par TRUE pour éviter pb de réaffichage
		// sur reception d'un VNL si la fenêtre est scrollée horizontalement
		TRACE("Invalidate\n");
		if (bInvalidate)
			Invalidate(TRUE);
		//    UpdateWindow();
	}
}

void CResaRailView::SetStatusBarText( UINT nId )
{
	((CChildFrame*)GetParentFrame())->
		GetStatusBar()->SetPaneOneAndLock(
		((CCTAApp*)APP)->GetResource()->LoadResString(nId) );
}

void CResaRailView::SetStatusBarText( CString szText )
{
	((CChildFrame*)GetParentFrame())->
		GetStatusBar()->SetPaneOneAndLock( szText );
}

HBRUSH CResaRailView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CScrollView::OnCtlColor(pDC, pWnd, nCtlColor);

	COLORREF clrBk = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();
	COLORREF clrBkAlt = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkAlt();

	CResaRailViewEdit *pEdit = (CResaRailViewEdit*)pWnd;
	if( nCtlColor == CTLCOLOR_EDIT )
	{  
		if (pEdit->m_bSci)
			pDC->SetBkColor (clrBkAlt);
		else
			pDC->SetBkColor( clrBk );
		pDC->SetTextColor( RGB( 0, 0, 255 ) );
	}
	else if( nCtlColor == CTLCOLOR_STATIC )
	{
		if(m_FlagReadOnly == TRUE)
		{
			pDC->SetTextColor( RGB( 128, 128, 128 ) );
		}
		if (pEdit->m_bSci)
			pDC->SetBkColor (clrBkAlt);
		else
			pDC->SetBkColor( clrBk );
		hbr = GetSysColorBrush( clrBk );
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

/////////////////////////////////////////////////////////////////////////////
// CVirtNestView message handlers

void CResaRailView::OnChartSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	BOOL Reduit = GetEtatWindow();
	Reduit = !Reduit;
	SetEtatWindow(Reduit);
	this->CtReload();
}
void CResaRailView::ChangeSelTC(const int & idx)
{ 
	int oldIdx = m_TabCtrl.GetCurSel();
	if (oldIdx != idx)
	{
		m_TabCtrl.SetCurSel(idx);
		CtReload();
	}
}
void CResaRailView::CtReload()
{
	if(m_TabCtrl.GetCurSel() == 0)
	{
		m_eButtonClicked = eOverallClicked;
	}
	else if(m_TabCtrl.GetCurSel() == 1)
	{
		m_eButtonClicked = eReducedClicked;
	}

	// DM7978 - LME - YI6056 - redimensionnement de la fenetre resarail lors d'un clic sur onglet
	if(m_CmptBktMap.GetCount() > 0)
	{ 
		RedrawWindow();
		ReorganizeTabOrder();
		Resize();
	}
	// DM7978 - LME - YI6056 

	// DM7978 - NCH
	CBktHistView *pCBktHistView				= ((CMainFrame*) AfxGetApp()->GetMainWnd())->GetCBktHistView();
	CBktHistAltView *pCBktHistAltView = ((CMainFrame*) AfxGetApp()->GetMainWnd())->GetCBktHistAltView();
	if (pCBktHistView != NULL)
		pCBktHistView->ChangeSelTC(m_TabCtrl.GetCurSel());
	if (pCBktHistAltView != NULL)
		pCBktHistAltView->ChangeSelTC(m_TabCtrl.GetCurSel());
	// END DM7978 - NCH
}

LRESULT CResaRailView::OnSwitchFocus(WPARAM wParam, LPARAM lParam)
{
	// when the focus is switched from a CResaRailViewEdit control,
	//   we tab to the next, and if it is the last edit control in
	//   the window, we must put the focus back to the first one
	CWnd* pFocusWnd = (CWnd*)wParam;
	SwitchFocusOrigin eSource = (SwitchFocusOrigin)lParam;

	// Which window was the window we are coming from ???
	CWnd* pWndOrigin;
	if ( eSource == MOUSE_CLICK )  // 
		pWndOrigin = pFocusWnd;
	else
		pWndOrigin = (CWnd*)m_pWndChildFocusSaved;

	// was the text in the window we are tabbing FROM modified ?
	if ( ((CResaRailViewEdit*)pWndOrigin)->GetModify() )
	{
		((CResaRailViewEdit*)pWndOrigin)->ResaRailViewEditEntry( TRUE );
		((CResaRailViewEdit*)pWndOrigin)->SetModify( FALSE );  // we've acted on this modification
	}

	if ( eSource == MOUSE_CLICK )
		return 0L;  // if this was caused by a mouse click, let's get the hell out now...

	UINT nFlag = GW_HWNDNEXT ;  // by default we go to the next...
	if ( eSource == RIGHT_ARROW_KEY || eSource == DOWN_ARROW_KEY )  // which direction is the user going ?
		nFlag = GW_HWNDNEXT;  // in the case of RIGHT_ARROW_KEY or DOWN_ARROW_KEY, we set the direction NEXT
	else
		nFlag = GW_HWNDPREV;  // in the case of LEFT_ARROW_KEY or UP_ARROW_KEY, we set the direction PREVIOUS

	// get the next window in the list in the direction of the user's keying
	CWnd* pNextWnd = pFocusWnd->GetNextWindow( nFlag );

	if (! pNextWnd->GetSafeHwnd())
		return 0L;

	if( pNextWnd )
	{
		// ensure that the next window (window we've tabbed to) is
		//   of type CResaRailViewEdit (because it could be a CStatic)
		if (! pNextWnd->IsKindOf(RUNTIME_CLASS(CResaRailViewEdit)) )
		{
			pNextWnd = GetNextWindow(GW_CHILD);  // next window in list
			if ( eSource == DOWN_ARROW_KEY )
				pNextWnd = pNextWnd->GetNextWindow( GW_HWNDNEXT );
			if ( eSource == UP_ARROW_KEY )
				pNextWnd = pNextWnd->GetNextWindow( GW_HWNDPREV );
		}
	}
	else
	{
		return 0L;
	}

	// set the member variable to hold the window which is in focus

	if(pNextWnd != NULL)
	{
		m_pWndChildFocusSaved = (CResaRailViewEdit*) pNextWnd;
		pNextWnd->SetFocus();
	}
	return 0L;

	AfxMessageBox("Tab received but no control has focus", MB_ICONEXCLAMATION | MB_APPLMODAL);
	return (LRESULT)0;
}

void CResaRailView::OnSetFocus(CWnd* pOldWnd) 
{
	//  CScrollView::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
	if( m_pWndChildFocusSaved )
		m_pWndChildFocusSaved->SetFocus();
	else
	{
		CWnd* pChildWnd = GetWindow( GW_CHILD );
		if( pChildWnd )
			pChildWnd->SetFocus();
	}
}

//DM7978 - LME - YI-5401
/*void CResaRailView::OnSize(UINT nType, int cx, int cy)
{

}*/
//DM7978 - LME - FIN

void CResaRailView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ChangeSizeForSci(TRUE);
}

void CResaRailView::OnLButtonUp(UINT nFlags, CPoint point)
{
	int iKeyIdx = 0;
	CString sKey;
	if (m_CmptBktSciMap.GetCount())
		while( iKeyIdx < m_CmptBktSciMap.m_aszKey.GetSize() )
		{
			CResaCmptBktSci* pCmptBkt = NULL;
			sKey = m_CmptBktSciMap.m_aszKey.GetAt(iKeyIdx++);
			m_CmptBktSciMap.Lookup( sKey, pCmptBkt );
			if (pCmptBkt)
			{
				YmIcResaBktDom* pResaBkt;
				for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
				{
					pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
					if (pResaBkt->ScxEtanche() && pResaBkt->XPos() && (point.x > pResaBkt->XPos()) &&
						(point.x <= pResaBkt->XPos() + 16) && (point.y > pResaBkt->YPos()) && (point.y <= pResaBkt->YPos() + 16))
					{
						pResaBkt->Clicked(!pResaBkt->Clicked());
						if (CtrlAuthSci(sKey[0], pCmptBkt->m_lAuthNew))
						{ // C'est OK on laisse
							CalcAvailSci(FALSE);
							Invalidate();
						}
						else // pas OK on restore
							pResaBkt->Clicked(!pResaBkt->Clicked());
						return;
					}
				}
			}
		}
}

BOOL CResaRailView::YaSciClicked() // Renvoi vrai si besoin de faire un VNAE
{
	int iKeyIdx = 0;
	CString sKey;
	if (m_CmptBktSciMap.GetCount())
		while( iKeyIdx < m_CmptBktSciMap.m_aszKey.GetSize() )
		{
			CResaCmptBktSci* pCmptBkt = NULL;
			sKey = m_CmptBktSciMap.m_aszKey.GetAt(iKeyIdx++);
			m_CmptBktSciMap.Lookup( sKey, pCmptBkt );
			if (pCmptBkt)
			{
				YmIcResaBktDom* pResaBkt;
				for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
				{
					pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
					if (pResaBkt->Clicked())
						return TRUE;
				}
			}
		}
		return FALSE;
}

/*
void CResaRailView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
}
*/

void CResaRailView::OnDestroy() 
{
	WriteViewConfig();
	CScrollView::OnDestroy();

	// TODO: Add your message handler code here
	YM_RecordSet* pResaBktSet = YM_Set<YmIcResaBktDom>::FromKey(RESABKT_KEY);
	YM_RecordSet* pResaCmptSet = YM_Set<YmIcResaCmptDom>::FromKey(RESACMPT_KEY);

	ASSERT( pResaBktSet != NULL );
	ASSERT( pResaCmptSet != NULL );

	pResaBktSet->RemoveObserver(this);
	pResaBktSet->Close();
	pResaCmptSet->RemoveObserver(this);
	pResaCmptSet->Close();

	CString szClient = App().m_pDatabase->GetClientParameter();
	CTrancheView *tranche = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();
	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetSCXFlag() || tranche->getCurrentTrancheNature() == 3)//SRE 89383 - Fermeture et destruction des threads si besoin
	{ // les SCI
		YM_RecordSet* pResaBktSciSet = YM_Set<YmIcResaBktDom>::FromKey(RESABKT_SCI_KEY);
		YM_RecordSet* pResaCmptSciSet = YM_Set<YmIcResaCmptDom>::FromKey(RESACMPT_SCI_KEY);

		ASSERT( pResaBktSciSet != NULL );
		ASSERT( pResaCmptSciSet != NULL );

		pResaBktSciSet->RemoveObserver(this);
		pResaBktSciSet->Close();
		pResaCmptSciSet->RemoveObserver(this);
		pResaCmptSciSet->Close();
	}

	if (((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetNomosFlag())
	{ // Les previsions
		YM_RecordSet* pPrevisionSet = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY);
		ASSERT( pPrevisionSet != NULL );
		pPrevisionSet->RemoveObserver(this);
		pPrevisionSet->Close();

		YM_RecordSet* pVnauExcluSet = YM_Set<YmIcVnauDom>::FromKey(VNAU_EXCLU_KEY);
		ASSERT( pVnauExcluSet != NULL );
		pVnauExcluSet->RemoveObserver(this);
		pVnauExcluSet->Close();
	}

}

void CResaRailView::OnResaRailPrint()

{
	CPrintDialog printDlg( TRUE );
	int iPrintIt = printDlg.DoModal();
	if ( iPrintIt == IDOK )
	{
		CDC dc;
		HDC hDC= printDlg.CreatePrinterDC();
		ASSERT(hDC != NULL);
		dc.Attach(hDC);
		OnDraw( &dc );
	}
}

void CResaRailView::OnUpdateResaRailPrint( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( TRUE );
}

BOOL CResaRailView::SaveOnAllLegs(int AUorAS)
{
	BOOL bModified = FALSE;
	if (!m_bOkOnRhea)
		return FALSE;

	if (!OverBookingTotalOK())
		return FALSE;

	YmIcLegsDom* pLeg = NULL;
	UpdateData( TRUE );  // update the auth or dmd value in the cmpt/bkt data object from the focus edit wnd

	YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);

	YM_Iterator<YmIcLegsDom>* pLegIterator = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY)->CreateIterator();
	YmIcLegsDom* pCurLeg = GetLegView()->GetLoadedLeg();
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	BOOL bCurLeg = FALSE;
	BOOL bVnasSent = FALSE;
	long SumScg = GetSumScgAuth();
	for( pLegIterator->First(); !pLegIterator->Finished(); pLegIterator->Next() ) 
	{
		pLeg = pLegIterator->Current();
		UpdateCompartments( pLeg );
		BOOL bCML = IsCrossMidnightLeg (pTranche, pLeg);
		if (AUorAS %2 == 1)
		{
			CString szMsg = BuildVnau( pLeg, NULL, bCML );
			if ( szMsg.IsEmpty() )
				return FALSE;
			((CCTAApp*)APP)->GetResComm()->SendVnau( NULL, szMsg, FALSE, bCML );
		}
		CString SuiteVnas;
		CString szMsgVnas;
		if (AUorAS >= 2)
		{
			szMsgVnas = BuildVnas( pLeg, bCML, SuiteVnas );
			if (!szMsgVnas.IsEmpty())
			{
				int rtn = ((CCTAApp*)APP)->GetResComm()->SendVnau( NULL, szMsgVnas, FALSE, bCML );
				if (!SuiteVnas.IsEmpty())
					((CCTAApp*)APP)->GetResComm()->SendVnau( NULL, SuiteVnas, FALSE, bCML );
				bVnasSent = (rtn > 0) || bVnasSent;
			}
		}

		pLeg->EditStatus (pLeg->EditStatus() & ~EDIT_STATUS_AUTHMODNOTSENT);
		if (pCurLeg && !strcmp (pLeg->LegOrig(), pCurLeg->LegOrig())
			&& (pLeg->TrancheNo() == pCurLeg->TrancheNo())
			&& (pLeg->DptDate() == pCurLeg->DptDate()))
		{
			pLeg->EditStatus (pLeg->EditStatus() | EDIT_STATUS_OKAY);
			bCurLeg = TRUE;
		}
		else
			bCurLeg = FALSE;
		pTranche->EditStatus (pTranche->EditStatus() &  ~EDIT_STATUS_AUTHMODNOTSENT);
		pTranche->EditStatus (pTranche->EditStatus() | EDIT_STATUS_OKAY);
		if (AUorAS %2 == 1)
		{
			pLeg->SentFlag(ChangeSentFlag(1, pLeg->SentFlag()));
			MajTrancheSentFlag ();
		}
		if (AUorAS >= 2)
		{
			if (!szMsgVnas.IsEmpty())
			{
				pLeg->ScxSentFlag(ChangeSentFlag(1, pLeg->ScxSentFlag()));
				pLeg->ScxCapa (SumScg);
				MajTrancheScxSentFlag ();
			}
		}

		if (bCurLeg)
		{
			((CCTAApp*)APP)->UpdateOneLeg (pLeg); // Pour mettre à jour le EditStatus du leg courant
			if (AUorAS %2 == 1)
				((CCTAApp*)APP)->UpdateAllLegs (pLeg); // Pour mettre à jour le SentFlag de tous les legs
			if (AUorAS >= 2)
			{
				if (!szMsgVnas.IsEmpty())
					((CCTAApp*)APP)->UpdateSciAllLegs (pLeg); // Pour mettre à jour le SentFlag de tous les legs
			}

			// Mise à jour de la tranche.
			YM_Query* pUpdateQuery = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), FALSE);
			pUpdateQuery->SetDomain (pTranche);
			((CCTAApp*)APP)->m_pDatabase->Transact (pUpdateQuery, IDS_SQL_UPDATE_TRANCHE_LIST);
			delete pUpdateQuery;
		}
	}
	delete pLegIterator;

	// Now, we update the database with the auth values at bucket level
	if (AUorAS %2 == 1)
	{
		POSITION pos = GetCmptBktMap()->GetStartPosition();  // start at the beginning of the cmpt map
		while( pos != NULL )
		{
			CString sKey;
			CResaCmptBkt* pCmptBkt;
			GetCmptBktMap()->GetNextAssoc( pos, sKey, pCmptBkt );
			YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;

			// iterate through all the buckets for this compartment
			YmIcResaBktDom* pResaBkt;
			for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				pResaBkt = pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt;
				YmIcResaBktDom* pResaBktBis = new YmIcResaBktDom(*pResaBkt);
				pResaBktBis->LegOrig("%"); //update all legs in one query
				pResaBktBis->AuthUser( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew );
				pResaBktBis->AuthCur( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew ); 
				YM_Query* pUpdateQuery = new YM_Query (*(((CCTAApp*)APP)->m_pDatabase), TRUE);
				pUpdateQuery->Start( pResaBktBis, IDS_SQL_UPDATE_RESA_BKT_AUTH );
				pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->SetAuthModify( FALSE );
			} 
		}
	}
	if (AUorAS >= 2)
		UpdateSci (bVnasSent, "%");

	if (YaSciClicked())
	{
		if (SendVnae ()) // construit les VNAE, demande confirmation et envoi si OK
			UpdateSciEtanche(); // met a 0 l'indicateur d'étanchéité pour tous les sc_scx_authorisations de la desserte.
	}
	return TRUE;
}

BOOL CResaRailView::SaveAllModified(int AUorAS,			// 1= VNAU, 2 = VNAS, 3 = VNAU et VNAS
																		YmIcLegsDom* pLeg /* = NULL */, 
																		enum CLegView::LegUpdateMode eUpdateMode /* = CURRENTLY_LOADED_LEG */,
																		BOOL bForceAuthMod /* = FALSE */, 
																		BOOL bConfirm /* = TRUE */ ) 
{
	BOOL bModified = FALSE;
	BOOL bSciModified = FALSE;
	BOOL bAutoTrain = FALSE;
	if (!m_bOkOnRhea)
		return FALSE;

	// DM 5480, on verifie s'il y a eu une autorisation modifié et si c'est sur un cmpt = "O"
	if( GetCmptBktMap()->GetCount() != 0 )
	{
		// First, we determine if there were any modifications made by the user
		POSITION pos = GetCmptBktMap()->GetStartPosition();
		while( pos != NULL &&  ! bModified ) // loop through all compartments until we find mods
		{
			CString sKey;
			CResaCmptBkt* pCmptBkt;
			GetCmptBktMap()->GetNextAssoc( pos, sKey, pCmptBkt );
			YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;
			if (pResaCmpt && !strcmp (pResaCmpt->Cmpt(), "O"))
				bAutoTrain = TRUE;

			// iterate through all the buckets for this compartment
			for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{     
				if ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetAuthModify() )
				{
					bModified = TRUE;
					break;
				}
			}
		}
	}
	if( GetCmptBktSciMap()->GetCount() != 0 )
	{
		// First, we determine if there were any modifications made by the user
		POSITION pos = GetCmptBktSciMap()->GetStartPosition();
		// NPI - Ano 72256
		//while( pos != NULL &&  ! bModified ) // loop through all compartments until we find mods
		while( pos != NULL &&  ! bSciModified ) // loop through all compartments until we find mods
		///
		{
			CString sKey;
			CResaCmptBktSci* pCmptBkt;
			GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCmptBkt );
			if (pCmptBkt->GetAuthModify())
				bSciModified = TRUE;
			else
				for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
				{     
					if ( pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetAuthModify() )
					{
						bSciModified = TRUE;
						break;
					}
				}
		}
	}
	EditEntries();
	if ((bModified || bSciModified) && bAutoTrain)
	{
		CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AUTOTRAIN_FULLOD);
		if (bModified)
			if (bSciModified)
				AUorAS = 3;
			else
				AUorAS = 1;
		else
			AUorAS = 2;
		if (AfxMessageBox (szText, MB_OKCANCEL) == IDOK)
		{
			SaveOnAllLegs(AUorAS);
			return TRUE;
		}
		else
			return FALSE;
	}

	if ( pLeg == NULL )
	{
		// Set the Leg Origin from the Leg record and update the authorizations
		pLeg = YM_Set<YmIcLegsDom>::FromKey(LEG_KEY);
	}

	//EditEntries();
	UpdateData( TRUE );  // update the auth or dmd value in the cmpt/bkt data object from the focus edit wnd
	BOOL bModif = FALSE;
	bModif = UpdateCompartments( pLeg );
	bModif = UpdateBuckets( AUorAS, pLeg, eUpdateMode, bForceAuthMod, bConfirm ) || bModif;

	return bModif;
}

void CResaRailView::UpdateOthLegsView( int iEditWinId )
{
	// Update the Other Legs View
	// 
	long lValue;
	CResaCmptBktLine* pCmptBktLine;
	YmIcResaBktDom* pResaBkt;
	if( GetEditMapArray()->Lookup( iEditWinId, pCmptBktLine ) )
	{
		BOOL bTrans;
		pResaBkt = pCmptBktLine->m_pResaBkt;
		// Was an Authorizations edit box modified ?
		if (pCmptBktLine->m_pEditAuthNew == NULL)
			return;
		if (pCmptBktLine->m_pEditAuthNew->GetDlgCtrlID() == (int)iEditWinId)
			lValue = GetDlgItemInt( iEditWinId, &bTrans, FALSE );
		else  // Demand Fcst edit box was modified
			return;  // so, do nothing
	}
	else
	{
		return;
	}

	// find the views and update them...
	//
	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	while( pos != NULL ) 
	{
		CView* pView = pDoc->GetNextView( pos ); 
		if( ((CMainFrame*) AfxGetApp()->GetMainWnd())->IsOthLegsView(pView, FALSE) )
		{
			if ( pView->GetSafeHwnd() )
				((COthLegsView*)pView)->UpdateAuthValue( pResaBkt->LegOrig(), (pResaBkt->Cmpt()), pResaBkt->NestLevel(), lValue );
			break;
		}
	}
}

void CResaRailView::SetMaxSci (char cmpt, int valmax)
{
	POSITION pos = GetCmptBktSciMap()->GetStartPosition();
	CString sKey;
	CString sCmpt = cmpt;
	BOOL bModif = FALSE;
	while( pos != NULL ) // loop through all compartments
	{
		CResaCmptBktSci* pCB = NULL;
		GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCB );
		if (pCB && !stricmp (pCB->m_pResaCmpt->Cmpt(), (LPCSTR)sCmpt))
		{
			pos = NULL;
			for( int iBktIdx = 0; iBktIdx < pCB->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				if (pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew > valmax)
				{
					CString s;
					s.Format ("%d", valmax);
					pCB->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew->SetWindowText(s);
					pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAuthPrev = pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;
					pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew = valmax;
					pCB->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->AuthUser( valmax );
					pCB->m_CmptBktLineArray[ iBktIdx ]->SetAuthModify(TRUE);
					bModif = TRUE;
				}
			}
		}
	}
	if (bModif)
		CalcAvailSci(FALSE);
}


void CResaRailView::SetMaxScg (char cmpt, int valmax)
{
	POSITION pos = GetCmptBktSciMap()->GetStartPosition();
	CString sKey;
	CString sCmpt = cmpt;
	while( pos != NULL ) // loop through all compartments
	{
		CResaCmptBktSci* pCB = NULL;
		GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCB );
		if (pCB && !stricmp (pCB->m_pResaCmpt->Cmpt(), (LPCSTR)sCmpt))
		{
			pos = NULL;
			if (pCB->m_lAuthNew > valmax)
			{
				CString s;
				s.Format ("%d", valmax);
				pCB->m_pEditScgCapa->SetWindowText (s);
				pCB->m_lAuthPrev = pCB->m_lAuthNew;
				pCB->m_lAuthNew = valmax;
				pCB->m_pResaCmpt->Capacity (valmax);
				pCB->SetAuthModify(TRUE);
				SetMaxSci (cmpt, valmax);
			}
		}
	}
}

void CResaRailView::CtrlLessCapaAndCC0() // vérification que SCG < min (capa, CC0)
{
	POSITION pos = GetCmptBktSciMap()->GetStartPosition();
	CString sCmpt, sKey;
	CResaCmptBktSci* pCB = NULL;
	while( pos != NULL ) // loop through all SCG
	{
		GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCB );
		if (pCB)
		{ // Il y a une CC-1 définie pour le compartiment.
			sCmpt = pCB->m_pResaCmpt->Cmpt();
			POSITION pos2 = GetCmptBktMap()->GetStartPosition();
			CResaCmptBkt* pCBcc = NULL;
			while( pos2 != NULL ) // loop through all compartments
			{
				GetCmptBktMap()->GetNextAssoc( pos2, sKey, pCBcc );
				if (pCBcc && !stricmp (pCBcc->m_pResaCmpt->Cmpt(), (LPCSTR)sCmpt))
				{
					int valmax = pCBcc->m_pResaCmpt->Capacity();
					if (pCBcc->m_CmptBktLineArray[0]->m_lAuthNew < valmax)
						valmax = pCBcc->m_CmptBktLineArray[0]->m_lAuthNew;
					SetMaxScg (sCmpt[0], valmax);
				}
			}
		}
	}
}

void CResaRailView::CtrlDiffCC0CC1() // verification que SCG < CC0-CC1, désactivé
{
	POSITION pos = GetCmptBktSciMap()->GetStartPosition();
	CString sCmpt, sKey;
	CResaCmptBktSci* pCB = NULL;
	while( pos != NULL ) // loop through all SCG
	{
		GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCB );
		if (pCB)
		{ // Il y a une CC-1 définie pour le compartiment.
			sCmpt = pCB->m_pResaCmpt->Cmpt();
			POSITION pos2 = GetCmptBktMap()->GetStartPosition();
			CResaCmptBkt* pCBcc = NULL;
			while( pos2 != NULL ) // loop through all compartments
			{
				GetCmptBktMap()->GetNextAssoc( pos2, sKey, pCBcc );
				if (pCBcc && !stricmp (pCBcc->m_pResaCmpt->Cmpt(), (LPCSTR)sCmpt))
				{
					if (pCBcc->m_CmptBktLineArray.GetSize() >= 2)
					{
						long diff01 = pCBcc->m_CmptBktLineArray[0]->m_lAuthNew - pCBcc->m_CmptBktLineArray[1]->m_lAuthNew;
						SetMaxScg (sCmpt[0], diff01);
					}
				}
			}
		}
	}
}

// controle si des espaces physique pour le quel ACT était différent de MOI pour au moins un bucket
// sont resté inchangé sur tout leur bucket. renvoi vrai si c'est le cas.
// ajouté en profitant de la DM 6496
BOOL CResaRailView::ControlActDiffMoi(int AUorAS)
{
	if (m_CActMoi.IsEmpty() && m_SciCActMoi.IsEmpty())
		return FALSE;
	int iKeyIdx = 0;
	CString sKey;
	if (!m_CActMoi.IsEmpty() && (AUorAS %2 == 1))
	{
		while( iKeyIdx < m_CmptBktMap.m_aszKey.GetSize() )
		{
			CResaCmptBkt* pCmptBkt;
			sKey = m_CmptBktMap.m_aszKey.GetAt(iKeyIdx++);
			if (m_CActMoi.Find(sKey) >= 0)
			{ // Le compartiment a été repéré comme ayant ACT <> MOI 
				m_CmptBktMap.Lookup( sKey, pCmptBkt );
				BOOL bModif = FALSE;
				for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
				{
					if (pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetAuthModify())
						bModif = TRUE;
				}
				if (!bModif)
					return TRUE; // Il n'a pas été modifié. il faut donc prevenir l'utilisateur
			}
		}
	}

	iKeyIdx = 0;
	if (!m_SciCActMoi.IsEmpty() && (AUorAS >= 2))
	{
		while( iKeyIdx < m_CmptBktSciMap.m_aszKey.GetSize() )
		{
			CResaCmptBktSci* pCmptBkt;
			sKey = m_CmptBktSciMap.m_aszKey.GetAt(iKeyIdx++);
			if (m_SciCActMoi.Find(sKey) >= 0)
			{ // Le compartiment a été repéré avec un SCI ayant ACT <> MOI 
				m_CmptBktSciMap.Lookup( sKey, pCmptBkt );
				BOOL bModif = FALSE;
				for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
				{
					if (pCmptBkt->m_CmptBktLineArray[ iBktIdx ]->GetAuthModify())
						bModif = TRUE;
				}
				if (!bModif)
					return TRUE; // Il n'a pas été modifié. il faut donc prevenir l'utilisateur
			}
		}
	}
	return FALSE;
}

// renvoi l'autorisation du sous-contingent global CC-1 pour le compartiment cmpt, colonne MOI
long CResaRailView::GetScgAuth (char cmpt)
{
	if (cmpt < 'a')
		cmpt += 32;
	POSITION pos = GetCmptBktSciMap()->GetStartPosition();
	CString sKey;
	CString sCmpt = cmpt;
	while( pos != NULL ) // loop through all compartments
	{
		CResaCmptBktSci* pCB = NULL;
		GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCB );
		if (pCB && !stricmp (pCB->m_pResaCmpt->Cmpt(), (LPCSTR)sCmpt)) 
			return pCB->m_lAuthNew;
	}
	return 0;
}

long CResaRailView::GetScgResHold(char cmpt)
{
	if (cmpt < 'a')
		cmpt += 32;
	POSITION pos = GetCmptBktSciMap()->GetStartPosition();
	CString sKey;
	CString sCmpt = cmpt;
	while( pos != NULL ) // loop through all compartments
	{
		CResaCmptBktSci* pCB = NULL;
		GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCB );
		if (pCB && !stricmp (pCB->m_pResaCmpt->Cmpt(), (LPCSTR)sCmpt)) 
			return pCB->m_lResHoldTot;
	}
	return 0;
}

// renvoi la somme des autorisations des sous-contingents pour tous les compartiments, colonne MOI
long CResaRailView::GetSumScgAuth()
{
	POSITION pos = GetCmptBktSciMap()->GetStartPosition();
	CString sKey;
	long Sum = 0;
	while( pos != NULL ) // loop through all compartments
	{
		CResaCmptBktSci* pCB = NULL;
		GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCB );
		if (pCB) 
			Sum += pCB->m_lAuthNew;
	}
	return Sum;
}


// Contrôle le leg courant et la tranche courante pour éventuellement changer leur SCX_CAPA.
// et provoquer le changement d'affichage du carré sous-contingent de jaune à gris et inversement.
void CResaRailView::CtrlScgLegs()
{
	CLegView * pLegView = GetLegView();
	YmIcLegsDom* pCurLeg = pLegView->GetLoadedLeg();
	int SumScg = GetSumScgAuth();
	if (((SumScg == 0) && (pCurLeg->ScxCapa() != 0)) ||
		((SumScg != 0) && (pCurLeg->ScxCapa() == 0)))
	{
		pCurLeg->ScxCapa(SumScg);
		pLegView->Invalidate(FALSE);
		YmIcLegsDom* pLeg = YM_Set<YmIcLegsDom>::FromKey( LEG_KEY);
		pLeg->ScxCapa(SumScg); // toujours cette merde entre item chargé et courant. Je fais les deux pour être sûr.
		pLegView->CtrlScgTranche();
	}
}

void CResaRailView::WriteViewConfig()
{
	CString szProfileKey;
	((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);
	APP->WriteProfileInt( szProfileKey, "AFFPMOYEN", m_bPanierMoyen );
}

//DM7978 - LME - YI-5401 - fonction de redimensionnement de la fenetre
void CResaRailView::Resize()
{
	// NPI - Ano 71815
	/*CRect resarect(0, 0, 0, 0);
	GetParentFrame()->GetWindowRect(&resarect);
	CPoint pt(0,0);
	((CCTAApp*)APP)->GetMainWnd()->ClientToScreen (&pt);
	CRect memorect (resarect.left - pt.x, resarect.top - pt.y,
		resarect.right - pt.x, resarect.bottom - pt.y);
	CSize szTotal = m_SizeView;

	if(memorect.right != memorect.left + szTotal.cx || memorect.bottom != memorect.top + szTotal.cy + 100)
	{
		memorect.left -= 2;
		memorect.top -= 2;
		memorect.right = memorect.left + szTotal.cx;
		// si le masquage du sous-contingent est actif
		if(m_bSciHeight)
		{
			memorect.bottom = memorect.top + m_lYSciNotDisplayed + 50;
		}
		else
		{
			memorect.bottom = memorect.top + szTotal.cy + 100;
		}
		GetParentFrame()->MoveWindow (&memorect);
	}*/

	CRect resarect(0, 0, 0, 0);
	GetParentFrame()->GetWindowRect(&resarect);
	CPoint pt(0,0);
	((CCTAApp*)APP)->GetMainWnd()->ClientToScreen (&pt);
	CRect memorect (resarect.left - pt.x, resarect.top - pt.y,
		resarect.right - pt.x, resarect.bottom - pt.y);
	CSize szTotal = GetTotalSize();

	// Si masquage active et affichage des sci
	if ((memorect.right != (memorect.left + szTotal.cx + 50)) || (memorect.bottom != (memorect.top + szTotal.cy + 50)))
	{
		memorect.left -= 2;
		memorect.top -= 2;
		memorect.right = memorect.left + szTotal.cx + 50;
		if (!m_bSciHeight)
		{
			memorect.bottom = memorect.top + szTotal.cy + 50;
		}
		else
		{
			memorect.bottom = memorect.top + (szTotal.cy / 2) + 50;
		}
		GetParentFrame()->MoveWindow (&memorect);
	}
	///
}
//DM7978 - LME - FIN

void CResaRailView::RestoreViewConfig()
{
	CString szProfileKey;
	((YM_PersistentChildWnd*)GetParentFrame())->BuildProfileKey(szProfileKey);
	m_bPanierMoyen = APP->GetProfileInt(szProfileKey, "AFFPMOYEN", 1);
}

// DM 6185.2 Exploitation des données de NM_PREV_TDLPRN (Thalys Nomos)
// lue avec la requête IDS_SQL_SELECT_THALYS_PREVISION. Fonctions aussi utilisée pour les montée en charge
long CResaRailView::GetRrdPrevisionResa (long rrd, const char* cmpt, long nestlevel)
{
	long res = -1;
	long sum = 0;
	YM_Iterator<YmIcResaBktDom>* pIteratorBkt = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY)->CreateIterator();
	YmIcResaBktDom* pResaBkt = NULL;
	for( pIteratorBkt->First(); !pIteratorBkt->Finished(); pIteratorBkt->Next() )
	{
		pResaBkt = pIteratorBkt->Current();
		if ((pResaBkt->RrdIndex() == rrd) && !strcmp (pResaBkt->Cmpt(), cmpt) &&
			(pResaBkt->NestLevel() == nestlevel))
		{
			sum += pResaBkt->ResHoldInd();
			res = sum;
			if (nestlevel > 0)
				break; // pas de sous-contingent, une seule valeur est à lire
		}
	}
	delete pIteratorBkt;
	return res;
}

long CResaRailView::GetRrdPrevisionResa (long rrd, const char* cmpt, long nestlevel, long scx)
{
	long res = -1;
	YM_Iterator<YmIcResaBktDom>* pIteratorBkt = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY)->CreateIterator();
	YmIcResaBktDom* pResaBkt = NULL;
	for( pIteratorBkt->First(); !pIteratorBkt->Finished(); pIteratorBkt->Next() )
	{
		pResaBkt = pIteratorBkt->Current();
		if ((pResaBkt->RrdIndex() == rrd) && !stricmp (pResaBkt->Cmpt(), cmpt) &&
			(pResaBkt->NestLevel() == nestlevel) && (pResaBkt->Scx() == scx))
		{
			res = pResaBkt->ResHoldInd();
			break;
		}
	}
	delete pIteratorBkt;
	return res;
}

long CResaRailView::GetRrdPrevisionRevenu (long rrd, const char* cmpt, long nestlevel)
{
	long res = -1;
	long sum = 0;
	YM_Iterator<YmIcResaBktDom>* pIteratorBkt = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY)->CreateIterator();
	YmIcResaBktDom* pResaBkt = NULL;
	for( pIteratorBkt->First(); !pIteratorBkt->Finished(); pIteratorBkt->Next() )
	{
		pResaBkt = pIteratorBkt->Current();
		if ((pResaBkt->RrdIndex() == rrd) && !strcmp (pResaBkt->Cmpt(), cmpt) &&
			(pResaBkt->NestLevel() == nestlevel))
		{
			sum += pResaBkt->RrdRecette();
			res = sum;
			if (nestlevel > 0)
				break; // pas de sous-contingent, une seule valeur est à lire
		}
	}
	delete pIteratorBkt;
	return res;
}

long CResaRailView::GetRrdPrevisionRevenu (long rrd, const char* cmpt, long nestlevel, long scx)
{
	long res = -1;
	YM_Iterator<YmIcResaBktDom>* pIteratorBkt = YM_Set<YmIcResaBktDom>::FromKey(PREVISION_KEY)->CreateIterator();
	YmIcResaBktDom* pResaBkt = NULL;
	for( pIteratorBkt->First(); !pIteratorBkt->Finished(); pIteratorBkt->Next() )
	{
		pResaBkt = pIteratorBkt->Current();
		if ((pResaBkt->RrdIndex() == rrd) && !stricmp (pResaBkt->Cmpt(), cmpt) &&
			(pResaBkt->NestLevel() == nestlevel) && (pResaBkt->Scx() == scx))
		{
			res = pResaBkt->RrdRecette();
			break;
		}
	}
	delete pIteratorBkt;
	return res;
}

void CResaRailView::GetVnauExcluAuth()
{
	YM_Iterator<YmIcVnauDom>* pIteratorVnau = YM_Set<YmIcVnauDom>::FromKey(VNAU_EXCLU_KEY)->CreateIterator();
	YmIcVnauDom* pVnau = NULL;
	pVnau = pIteratorVnau->Current();
	CString scVnau;
	if (pVnau)
		scVnau = pVnau->CommandText();
	else
		return;

	CString sKey, sBuck;
	int iKeyIdx = 0;
	while( iKeyIdx < m_CmptBktMap.m_aszKey.GetSize() )
	{
		CResaCmptBkt* pCmptBkt;
		sKey = m_CmptBktMap.m_aszKey.GetAt(iKeyIdx++);
		m_CmptBktMap.Lookup( sKey, pCmptBkt );
		if (pCmptBkt == NULL)
			return;
		for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
		{
			CResaCmptBktLine* pBktLine = pCmptBkt->m_CmptBktLineArray[ iBktIdx ];
			YmIcResaBktDom* pResaBkt = pBktLine->m_pResaBkt;
			CString sBuck;
			int k, l;
			sBuck.Format("/%c%d", pResaBkt->Cmpt()[0], pResaBkt->NestLevel());
			k = scVnau.Find(sBuck);
			if (k > -1)
			{
				l = scVnau.Find ('/', k+1);
				if (l < 0)
					l = scVnau.GetLength(); //le dernier bucket.
				CString sVal = scVnau.Mid (k+4, l-k-4);
				int ival = atoi (sVal);
				pBktLine->m_lAuthNomos = ival;
			}
		}
	}
}

void CResaRailView::CalcVnauExcluAvail()
{
	int iKeyIdx = 0;
	while( iKeyIdx < m_CmptBktMap.m_aszKey.GetSize() )
	{
		CResaCmptBkt* pCmptBkt;
		CString sKey = m_CmptBktMap.m_aszKey.GetAt(iKeyIdx++);
		m_CmptBktMap.Lookup( sKey, pCmptBkt );
		if (pCmptBkt == NULL)
			return;
		YmIcResaCmptDom* pResaCmpt = pCmptBkt->m_pResaCmpt;
		if (pResaCmpt == NULL)
			return;

		// récupération de la somme des dispo des sci étanches de l'espace physique
		int sumDispEtanche = GetSumDispEtanche(pResaCmpt->Cmpt()[0]);
		long lResHoldAccum;
		int iAvailPrevNomos = INT_MAX;  // initialize prev avail with something big
		for( int iBktIdx = 0; iBktIdx < pCmptBkt->m_CmptBktLineArray.GetSize(); iBktIdx++ )
		{
			CResaCmptBktLine* pBktLine = pCmptBkt->m_CmptBktLineArray[ iBktIdx ];
			lResHoldAccum = 0;
			for( int j = iBktIdx; j < pCmptBkt->m_CmptBktLineArray.GetSize(); j++ )
				lResHoldAccum += GetRealTimeResHold( pResaCmpt->Cmpt(), 
				pCmptBkt->m_CmptBktLineArray[ j ]->m_pResaBkt->NestLevel(), 
				pCmptBkt->m_CmptBktLineArray[ j ]->m_pResaBkt );
			iAvailPrevNomos = (iBktIdx <= 0) ? INT_MAX : pCmptBkt->m_CmptBktLineArray[ iBktIdx-1 ]->m_lAvailNomos;
			int iAvailNomos = pBktLine->m_lAuthNomos - lResHoldAccum;
			if (iBktIdx == 0)
			{
				iAvailNomos -= sumDispEtanche;
				if (iAvailNomos < 0)
					iAvailNomos = 0;
			}
			if (iAvailNomos > iAvailPrevNomos)
				iAvailNomos = iAvailPrevNomos;
			pBktLine->m_lAvailNomos = iAvailNomos;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CResaRailViewEdit

IMPLEMENT_DYNCREATE(CResaRailViewEdit, CEdit)

CResaRailViewEdit::CResaRailViewEdit(bool bSci /*=FALSE*/)
{
	SetEditChecked( FALSE );
	SetEditAdjusted( TRUE );
	m_bBktLevel = TRUE;  // authorization value edit box by default
	m_bSci = bSci;
	// DM7978 - LME - YI5415 - initialisation des couleurs
	m_BackColor = TRANS_BACK;
}

CResaRailViewEdit::~CResaRailViewEdit()
{
}

// Perform an Edit Check on an edit window.
// First, we determine if the edit window is for Authorization or Dmd Forecast value
// Then, we apply the percentage allowed, and warn the user if exceeded.
// Return TRUE if modification is acceptable, else FALSE
// The following rules apply:a
//    Authorization value for bucket = 0 cannot be less than Real Capacity
//    Authorization value for bucket = 0 cannot be greater than 112% of Real Capacity
//    Authorization value for bucket > 0 cannot exceed the range specified in Sc_Sys_Parms
BOOL CResaRailViewEdit::RangeCheck()
{
	if (m_bSci)
		return RangeCheckSci();

	if ( !GetEditChecked() )
	{
		SetEditChecked( TRUE );  // don't do it again until the value changes

		double dValOrg = 0;
		double dValNew = 0;
		BOOL bTrans;
		BOOL bAuthLevelModified = FALSE;
		CWnd* pParentWnd = GetParent();

		// lookup this line in the edit map array by its ID
		CResaCmptBktLine* pCmptBktLine;
		UINT nId = GetDlgCtrlID();
		if( ! ((CResaRailView*)pParentWnd)->GetEditMapArray()->Lookup( nId, pCmptBktLine ) )
			return FALSE;  // didn't find it...not normal

		// find this edit control in the map and determine what kind it is (Auth or Dmd Fcst)
		// What kind of edit box was modified?  Authorizations or Demand Fcst
		// get the value edited by the user and the value from the record for comparison
		if ( (pCmptBktLine->m_pEditAuthNew != NULL) && 
			(pCmptBktLine->m_pEditAuthNew->GetDlgCtrlID() == (int)nId) )  // Authorizations
		{
			bAuthLevelModified = TRUE;  // an authorization value was modified
			dValNew = ((CResaRailView*)pParentWnd)->GetDlgItemInt( nId, &bTrans, FALSE );
			dValOrg = (double)pCmptBktLine->m_pResaBkt->AuthUser();
		}

		if ( (int)dValNew == (int)dValOrg)  // did the value change ?
			return TRUE;  // no, get out now !

		if ( bAuthLevelModified )  // was an authorization value modified ?
		{
			if ( pCmptBktLine->m_pResaBkt->NestLevel() == 0 )  // Nest Level 0 ?
			{
				// Get real capacity either from the VNL or from the Cmpt data object
				CString sKey( pCmptBktLine->m_pResaBkt->Cmpt() );
				CResaCmptBkt* pCmptBkt;
				if( ! ((CResaRailView*)pParentWnd)->GetCmptBktMap()->Lookup( sKey, pCmptBkt ) )
					return FALSE;
				YM_VnlCmpt* pVnlCmpt = ((CResaRailView*)pParentWnd)->FindVnlCmpt( pCmptBktLine->m_pResaBkt->Cmpt() );
				long lCapacity = ( pVnlCmpt == NULL ) ? pCmptBkt->m_pResaCmpt->Capacity() : pVnlCmpt->GetCapacity() ;

				if ( dValNew < lCapacity)  // auth cannot be below capacity
				{
					// produce a warning beep and ask the user for confirmation of the change
					/*
					if ( ((CResaRailView*)pParentWnd)->m_bConfirmUnderbooking )  // do we ask again ?
					{
					((CResaRailView*)pParentWnd)->m_bConfirmUnderbooking = FALSE;  // don't ask again for this leg (see OnUpdate)
					*/
					if ( AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_OVBLEVEL_RANGE_WARN), 
						MB_OKCANCEL | MB_APPLMODAL | MB_ICONEXCLAMATION ) == IDCANCEL )
						return FALSE;  // user didn't accept it, so get out now, and don't change anything !
					/*
					}
					*/
				}
				// DM 5480 train auto
				if ((sKey == "O") && ( dValNew > lCapacity  ))
				{
					CString szText = ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_AUTOTRAIN_AUTH_INF_CAP);
					AfxMessageBox (szText, MB_OK | MB_ICONSTOP);
					((CResaRailView*)pParentWnd)->SetDlgItemInt( nId, lCapacity);
					dValNew = lCapacity;
				} 

				int ovb = 112;
				if (sKey != "O")
					ovb = ((CResaRailView*)pParentWnd)->m_SurResa[(int)(sKey[0] - 'A')];

				// get confirmation if auth is 112% above real capacity
				if ( dValNew > (( lCapacity * ovb ) / 100) )
				{
					// produce a warning beep and ask the user for confirmation of the change
					CString szMsg;
					szMsg.Format( IDS_AUTH_CAP_RANGE_WARNING, (int)dValNew, ovb, (int)((lCapacity * ovb) / 100) );
					if ( AfxMessageBox( szMsg, MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION ) == IDNO )
					{
						((CResaRailView*)pParentWnd)->SetModificationCancelled(TRUE);
						return FALSE;  // user didn't accept it, so get out now, and don't change anything !
					}
				}
				ProcessTrainAdaptation(pCmptBkt, dValOrg, dValNew);
			}
		}
	}
	return TRUE;
}

BOOL CResaRailViewEdit::RangeCheckSci()
{
	if ( !GetEditChecked() )
	{

		CTrancheView *tranche = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetTrancheView();

		SetEditChecked( TRUE );  // don't do it again until the value changes
		CString sKey;
		double dValNew = 0;
		BOOL bTrans;
		CWnd* pParentWnd = GetParent();

		// lookup this line in the edit map array by its ID
		CResaCmptBktSciLine* pCmptBktLine = NULL;
		CResaCmptBktSci*	 pCmptBkt = NULL;
		UINT nId = GetDlgCtrlID();
		dValNew = ((CResaRailView*)pParentWnd)->GetDlgItemInt( nId, &bTrans, FALSE );
		if (!bTrans)
			return FALSE;
		if( ((CResaRailView*)pParentWnd)->GetEditSciMapArray()->Lookup( nId, pCmptBktLine ) )
		{
			
			// NBN ANO 89569
			// Pour la tranche 3 - Force SC1 = SCG, donc pas de modification possible
			if(tranche->getCurrentTrancheNature() == 3){
				return FALSE;
			}
			// C'est une Sci
			YmIcResaBktDom* pResaBkt = pCmptBktLine->m_pResaBkt;
			POSITION pos = ((CResaRailView*)pParentWnd)->GetCmptBktSciMap()->GetStartPosition();
			CResaCmptBktSci* pCB = NULL;
			while( pos != NULL ) // loop through all compartments
			{  
				((CResaRailView*)pParentWnd)->GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCB );
				if (!strcmp (pCB->m_pResaCmpt->Cmpt(), pCmptBktLine->m_pResaBkt->Cmpt()))
				{
					pos = NULL;
					pCmptBkt = pCB;
				}
			}
			if (!pCmptBkt)
				return FALSE;
			// l'autorisation doit être inférieur au SCG
			if (dValNew > pCmptBkt->m_lAuthNew)
				return FALSE;

			int Bkt0Disp = ((CResaRailView*)pParentWnd)->GetAvailBkt (pCmptBkt->m_pResaCmpt->Cmpt()[0], 0);

			// Controle que la somme des SCI doit être supérieure au SCG
			if (pCB)
			{
				double SumSci = 0;
				double SumSciEtanche = 0;
				for( int iBktIdx = 0; iBktIdx < pCB->m_CmptBktLineArray.GetSize(); iBktIdx++ )
				{
					if (pCB->m_CmptBktLineArray[ iBktIdx ] == pCmptBktLine)
					{
						SumSci += dValNew;
						if (pCB->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->ScxEtanche() &&
							!(pCB->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->Clicked()))
						{
							SumSciEtanche += dValNew;
							int variation = static_cast<int>(dValNew - pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew);
							if (variation > 0)
							{ // augmentation d'une autorisation SCI Etanche

								if (pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAvailUser + variation > Bkt0Disp)
								{
								
									AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_DISP_ETANCHE_TOO_HIGH), 
										MB_OK | MB_APPLMODAL | MB_ICONSTOP );
									return FALSE;
								}
							}
						}
					}
					else
					{
						SumSci += pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;
						if (pCB->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->ScxEtanche() &&
							!(pCB->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->Clicked()))
							SumSciEtanche += pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;
					}
				}

				if (SumSciEtanche > pCmptBkt->m_lAuthNew)
				{
					AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_SUM_ETANCHE_SUP_SCG), 
						MB_OK | MB_APPLMODAL | MB_ICONSTOP );
					return FALSE;
				}
				if (SumSciEtanche)
				{
					// Il y a des étanches, vérifions que chaque SCI non étanche reste inférieur en autorisation à la
					// différence entre CC-1 et somme des étanches
					for( int iBktIdx = 0; iBktIdx < pCB->m_CmptBktLineArray.GetSize(); iBktIdx++ )
					{
						double valtest = 0;
						if (pCB->m_CmptBktLineArray[ iBktIdx ] == pCmptBktLine)
							valtest = dValNew;
						else
							valtest = pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;
						if (!(pCB->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->ScxEtanche()) ||
							pCB->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->Clicked())
						{ // c'est un non étanche ou un désentanchétifié
							if (valtest > pCmptBkt->m_lAuthNew - SumSciEtanche)
							{
								AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_NON_ETANCHE_TROP_HAUT), 
									MB_OK | MB_APPLMODAL | MB_ICONSTOP );
								return FALSE;
							}
						}
					}
				}
			}
		}
		else
		{ // il doit s'agir d'une SCG
			POSITION pos = ((CResaRailView*)pParentWnd)->GetCmptBktSciMap()->GetStartPosition();
			while( pos != NULL ) // loop through all compartments
			{
				CResaCmptBktSci* pCB = NULL;
				((CResaRailView*)pParentWnd)->GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCB );
				if (pCB->m_pEditScgCapa == this)
				{
					pos = NULL;
					pCmptBkt = pCB;
				}
			}
			if (!pCmptBkt)
				return FALSE;

			// il faut aussi vérifier que SCG <= CC0-CC1
			CResaCmptBkt* pCBcc = NULL;
			pos = ((CResaRailView*)pParentWnd)->GetCmptBktMap()->GetStartPosition();
			while (pos != NULL)
			{
				((CResaRailView*)pParentWnd)->GetCmptBktMap()->GetNextAssoc( pos, sKey, pCBcc );
				if (!stricmp (pCBcc->m_pResaCmpt->Cmpt(), pCmptBkt->m_pResaCmpt->Cmpt()))
				{
					pos = NULL;
					// verification que SCG <= CC0-CC1, désactivé
					/*
					if (pCBcc->m_CmptBktLineArray.GetSize() >= 2)
					{
					long diff01 = pCBcc->m_CmptBktLineArray[0]->m_lAuthNew - pCBcc->m_CmptBktLineArray[1]->m_lAuthNew;
					if (dValNew > diff01)
					return FALSE;
					}
					*/
					// remplacé par vérification que SCG < min (capa, CC0)
					CString sCmpt = pCBcc->m_pResaCmpt->Cmpt();
					int valmax = pCBcc->m_pResaCmpt->Capacity();
					if (pCBcc->m_CmptBktLineArray[0]->m_lAuthNew < valmax)
						valmax = pCBcc->m_CmptBktLineArray[0]->m_lAuthNew;
					if (dValNew > valmax)
						return FALSE;
					// Pour la tranche 3 ne pas faire de controle car on associe à la SC1 la SCG
					if (tranche->getCurrentTrancheNature() != 3 && !((CResaRailView*)pParentWnd)->CtrlAuthSci(pCmptBkt->m_pResaCmpt->Cmpt()[0], static_cast<int>(dValNew)))
						return FALSE;
				}
			}

			//On force les SCI a être inférieur
			((CResaRailView*)pParentWnd)->SetMaxSci(pCmptBkt->m_pResaCmpt->Cmpt()[0], static_cast<int>(dValNew));

			// NBN ANO 89569
			// Pour la tranche 3 - Force SC1 = SCG
			if(tranche->getCurrentTrancheNature() == 3){
				((CResaRailView*)pParentWnd)->ForceSci(pCmptBkt->m_pResaCmpt->Cmpt()[0], static_cast<int>(dValNew));
			}

		}
		return TRUE;
	}
	return TRUE;
}

void CResaRailView::ForceSci (char cmpt, int val)
{
	POSITION pos = GetCmptBktSciMap()->GetStartPosition();
	CString sKey;
	CString sCmpt = cmpt;
	BOOL bModif = FALSE;
	while( pos != NULL ) // loop through all compartments
	{
		CResaCmptBktSci* pCB = NULL;
		GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCB );
		if (pCB && !stricmp (pCB->m_pResaCmpt->Cmpt(), (LPCSTR)sCmpt))
		{
			pos = NULL;
			for( int iBktIdx = 0; iBktIdx < pCB->m_CmptBktLineArray.GetSize(); iBktIdx++ )
			{
				CString s;
				s.Format ("%d", val);
				pCB->m_CmptBktLineArray[ iBktIdx ]->m_pEditAuthNew->SetWindowText(s);
				pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAuthPrev = pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew;
				pCB->m_CmptBktLineArray[ iBktIdx ]->m_lAuthNew = val;
				pCB->m_CmptBktLineArray[ iBktIdx ]->m_pResaBkt->AuthUser( val );
				pCB->m_CmptBktLineArray[ iBktIdx ]->SetAuthModify(TRUE);
				//bModif = TRUE;
		
			}
		}
	}
	if (bModif)
		CalcAvailSci(FALSE);
}

void CResaRailViewEdit::ProcessTrainAdaptation(CResaCmptBkt* pCmptBkt, double dValOrg, double dValNew)
{
	// Train Adaptation
	if ( ((CCTAApp*)APP)->GetDocument()->m_pGlobalSysParms->GetTrainAdaptFlag() )  // Train Adaptation ?
	{
		if ( fabs(dValNew - dValOrg) >= 36 )  // Only ask for Adaptation if the difference in modification is +/- 36
		{
			if ( AfxMessageBox( ((CCTAApp*)APP)->GetResource()->LoadResString(IDS_ADAPTATION_CONFIRM), 
				MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION ) == IDYES )
			{
				YmIcTrancheLisDom* pTranche = YM_Set<YmIcTrancheLisDom>::FromKey(TRANCHE_KEY);
				CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
				YmIcTrainSupplementDom pRecord;
				pRecord.TrancheNo(pTranche->TrancheNo());
				pRecord.DptDate(pTranche->DptDate());
				pRecord.Cmpt(pCmptBkt->m_pResaCmpt->Cmpt());
				pRecord.Capacity(pCmptBkt->m_pResaCmpt->Capacity());
				pRecord.SeatBefore((long)dValOrg);
				pRecord.SeatAfter((long)dValNew);
				pRecord.setAdaptTypeIsNull(FALSE);
				pRecord.AdaptType(TRAIN_ADAPTATION);

				YM_Query* pSql = new YM_Query(* (WinApp->m_pDatabase), FALSE);
				pSql->SetDomain(&pRecord);
				WinApp->m_pDatabase->Transact(pSql, IDS_SQL_INSERT_TRAIN_SUPPLEMENT);
				delete pSql;

				// close, re-open the Adaptation RecordSet forcing an SQL to be sent
				YM_RecordSet* pTrainSupplementSet = 
					YM_Set<YmIcTrainSupplementDom>::FromKey(TRAINADAPTATION_KEY);
				ASSERT( pTrainSupplementSet != NULL );
				pTrainSupplementSet->Close();
				pTrainSupplementSet->Open();

				CLegView* pLegView = ((CResaRailView*)GetParent())->GetLegView();
				// Set the Adaptation Flag in the LegView so it will know to set the overbooking level in all legs for this train
				if (pLegView)
					pLegView->SetAdaptationFlag(TRUE);
			}
		}
	}
}

// This method is called when the user has:
// - pressed the Enter with focus set to a ResaRailViewEditEntry object
// - focus is switched from a ResaRailViewEditEntry object, and its value has been changed
// - an iteration through all the edit windows occurs (in EditEntries)
void CResaRailViewEdit::ResaRailViewEditEntry( BOOL bUpdateWindow /* = TRUE */ ) 
{
	if ( ! m_bBktLevel )
		return;
	BOOL bGetModify = GetModify();

	/*int yesOrNo = IDYES;
	{
	yesOrNo = VnauConfirmationMessage();
	}*/

	if (m_bSci)
	{
		/*if(yesOrNo == IDYES)
		{*/
		if (RangeCheckSci())
		{
			UpdateData( TRUE );
			((CResaRailView*)GetParent())->CalcAvailSci(FALSE);
			if (bUpdateWindow)
				((CResaRailView*)GetParent())->UpdateData( FALSE ); // Pour prendre en compte éventuelle modif de dispo SCI.
		}
		else
		{
			UpdateData( FALSE );
		}
		/*}
		else if(yesOrNo == IDNO)
		{
		UpdateData( FALSE );
		}*/
		if ( bUpdateWindow && bGetModify )
		{
			GetParent()->Invalidate(FALSE);
			GetParent()->UpdateWindow();
			SetModify(FALSE);
		}
		return;
	}

	CCTADoc* pDoc = ((CCTAApp*)APP)->GetDocument();

	/*if(yesOrNo == IDYES)
	{*/
	if ( RangeCheck() )  // perform a range check on the value in this edit window
	{
		UpdateData( TRUE );  // update the auth or dmd value in the cmpt/bkt data object from this edit wnd

		CLegView *pLegView = ((CResaRailView*)GetParent())->GetLegView();
		// perform all checks and modifications to the other buckets
		if ( pLegView && pLegView->GetOvbAdjustFlag())  // is the ovb adjust flag set in the leg list ?
			((CResaRailView*)GetParent())->OvbAdjust();    // overbooking adjustment flag was set, so perform overbook adjustment

		((CResaRailView*)GetParent())->BucketCheck();  // ensure correct order of auth levels

		// calculate the current, forecast, and original forecast availabilities
		((CResaRailView*)GetParent())->CalcAvail(TRUE);

		((CResaRailView*)GetParent())->CtrlLessCapaAndCC0(); // forcer SCG <= capacité sur tous les compartiments

		// because we might have changed some of the authorizations in the 
		//   OvbAdjust or BucketCheck methods, we must update all edit windows
		//   from the data in the cmpt/bkt data object
		if (bUpdateWindow) // Ano 131098 JMG
			((CResaRailView*)GetParent())->UpdateData( FALSE );
	}
	else
	{
		UpdateData( FALSE );  // restore the edit window value from the data in the cmpt/bkt data object
	}
	/*}
	else if(yesOrNo == IDNO)
	{
	UpdateData( FALSE );
	}*/
	if ( bUpdateWindow && bGetModify )
	{
		GetParent()->Invalidate(FALSE);
		GetParent()->UpdateWindow();
		SetModify(FALSE);

		// Update other views displaying authorization values
		UINT iEditWinId = GetDlgCtrlID();
		((CResaRailView*)GetParent())->UpdateOthLegsView( iEditWinId );
	}
	if (bUpdateWindow) {
		// PMe 041221: Change to next line when Enter is pressed (but do not change compartment)
		CResaCmptBktLine *pCmptBktLine1, *pCmptBktLine2;
		UINT nId = GetDlgCtrlID();

		// find this CmptBktLine objects in the map
		if (((CResaRailView*)GetParent())->GetEditMapArray()->Lookup( nId,   pCmptBktLine1 ) &&
			((CResaRailView*)GetParent())->GetEditMapArray()->Lookup( nId+2, pCmptBktLine2 )) {
				// Check if we will not change the compartment
				if (!strcmp( pCmptBktLine1->m_pResaBkt->Cmpt(), pCmptBktLine2->m_pResaBkt->Cmpt())) {
					WPARAM wParam = (WPARAM)((CWnd*)this);
					LPARAM lParam = 2; //CResaRailView::DOWN_ARROW_KEY:
					LRESULT r = ((CResaRailView*)GetParent())->OnSwitchFocus( wParam, lParam );
				}
			}
	}
}

// This method works just as the CWnd::UpdateData method
// Initialize data in an edit window (bSaveAndValidate = FALSE), 
// or to retrieve and validate data from an edit window (bSaveAndValidate = TRUE)
//
void CResaRailViewEdit::UpdateData(BOOL bSaveAndValidate)
{
	if (m_bSci)
		return UpdateDataSci(bSaveAndValidate);

	CWnd* pParentWnd = GetParent();
	CResaCmptBktLine* pCmptBktLine;
	BOOL bUpdate = FALSE;
	UINT nId = GetDlgCtrlID();

	// find this CmptBktLine object in the map
	if( ((CResaRailView*)pParentWnd)->GetEditMapArray()->Lookup( nId, pCmptBktLine ) )
	{
		// get the id for this control (resa rail edit window)

		//   or, is this edit window for authorizations ?
		if ( (pCmptBktLine->m_pEditAuthNew != NULL) && 
			(pCmptBktLine->m_pEditAuthNew->GetDlgCtrlID() == (int)nId) )
		{
			// get the value from the authorization edit window
			BOOL bTrans;
			LONG lAuth = ((CResaRailView*)pParentWnd)->GetDlgItemInt( nId, &bTrans, FALSE );

			// was it modified ?
			if( pCmptBktLine->m_lAuthNew != lAuth )
			{
				if (((CResaRailView*)pParentWnd)->GetModificationCancelled())
				{
					pCmptBktLine->SetAuthModify( FALSE );  // indicate that a value in this line changed
					((CResaRailView*)pParentWnd)->SetModificationCancelled(FALSE);  // reset
				}
				else
				{
					pCmptBktLine->SetAuthModify( TRUE );  // indicate that a value in this line changed
				}
				/* FOR NOW, WE DISACTIVATE REVENUE MIX MODEL FOR AUTHORIZATION LEVEL !!!!
				pCmptBktLine->SetAuthRevMixFlag( TRUE );  // reset revenue mix indicator
				*/
				if ( bSaveAndValidate )
				{
					// update the data obj
					bUpdate = TRUE;

					// hold the previous authorization value
					pCmptBktLine->m_lAuthPrev = pCmptBktLine->m_lAuthNew;

					// set the new value in the data object
					pCmptBktLine->m_lAuthNew = lAuth;

					// set it in the domain (db) record
					pCmptBktLine->m_pResaBkt->AuthUser( lAuth );
				}
				else
				{
					if (pCmptBktLine->m_pEditAuthNew != NULL)
					{
						// restore the edit window
						CString s;
						s.Format( "%d", pCmptBktLine->m_lAuthNew );
						pCmptBktLine->m_pEditAuthNew->SetWindowText( s );
					}
				}
			}
			if( !bTrans )
				SetDlgItemInt( nId, 0, FALSE );
		}
		else
		{
			TRACE( "Edit Control Notification without link!\n" );
		}
	}
	if( bUpdate )
		((CResaRailView*)pParentWnd)->GetCmptBktMap()->AccumAddDmd();  // now update the totals
}

// This method works just as the CWnd::UpdateData method
// Initialize data in an edit window (bSaveAndValidate = FALSE), 
// or to retrieve and validate data from an edit window (bSaveAndValidate = TRUE)
//
void CResaRailViewEdit::UpdateDataSci(BOOL bSaveAndValidate)
{
	CWnd* pParentWnd = GetParent();
	CResaCmptBktSciLine* pCmptBktSciLine;
	UINT nId = GetDlgCtrlID();

	BOOL bTrans;
	LONG lAuth = ((CResaRailView*)pParentWnd)->GetDlgItemInt( nId, &bTrans, FALSE );
	if ( !bTrans )
		SetDlgItemInt( nId, 0, FALSE );

	if( ((CResaRailView*)pParentWnd)->GetEditSciMapArray()->Lookup( nId, pCmptBktSciLine ) )
	{
		if( pCmptBktSciLine->m_lAuthNew != lAuth )
		{
			if (((CResaRailView*)pParentWnd)->GetModificationCancelled())
			{
				pCmptBktSciLine->SetAuthModify( FALSE );  // indicate that a value in this line changed
				((CResaRailView*)pParentWnd)->SetModificationCancelled(FALSE);  // reset
			}
			else
				pCmptBktSciLine->SetAuthModify( TRUE );  // indicate that a value in this line changed

			if ( bSaveAndValidate )
			{
				// hold the previous authorization value
				pCmptBktSciLine->m_lAuthPrev = pCmptBktSciLine->m_lAuthNew;

				// set the new value in the data object
				pCmptBktSciLine->m_lAuthNew = lAuth;

				// set it in the domain (db) record
				pCmptBktSciLine->m_pResaBkt->AuthUser( lAuth );
			}
			else
			{
				if (pCmptBktSciLine->m_pEditAuthNew != NULL)
				{
					// restore the edit window
					CString s;
					s.Format( "%d", pCmptBktSciLine->m_lAuthNew );
					pCmptBktSciLine->m_pEditAuthNew->SetWindowText( s );
				}
			}
		}
	}
	else
	{ // Ce doit être une valeur de SCG
		CResaCmptBktSci* pCmptBkt;
		POSITION pos = ((CResaRailView*)pParentWnd)->GetCmptBktSciMap()->GetStartPosition();
		while( pos != NULL ) // loop through all compartments
		{
			CString sKey;
			CResaCmptBktSci* pCB = NULL;
			((CResaRailView*)pParentWnd)->GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCB );
			if (pCB->m_pEditScgCapa == this)
			{
				pos = NULL;
				pCmptBkt = pCB;
			}
		}
		if (!pCmptBkt)
			return;
		if (((CResaRailView*)pParentWnd)->GetModificationCancelled())
		{
			pCmptBkt->SetAuthModify( FALSE );  // indicate that a value in this line changed
			((CResaRailView*)pParentWnd)->SetModificationCancelled(FALSE);  // reset
		}
		else
			pCmptBkt->SetAuthModify( TRUE );  // indicate that a value in this line changed

		if ( bSaveAndValidate )
		{
			// hold the previous authorization value
			pCmptBkt->m_lAuthPrev = pCmptBkt->m_lAuthNew;

			// set the new value in the data object
			pCmptBkt->m_lAuthNew = lAuth;

			// set it in the domain (db) record
			pCmptBkt->m_pResaCmpt->Capacity( lAuth );
		}
		else
		{
			if (pCmptBkt->m_pEditScgCapa != NULL)
			{
				// restore the edit window
				CString s;
				s.Format( "%d", pCmptBkt->m_lAuthNew );
				pCmptBkt->m_pEditScgCapa->SetWindowText( s );
			}
		}
	}
}

BEGIN_MESSAGE_MAP(CResaRailViewEdit, CEdit)
	//{{AFX_MSG_MAP(CResaRailViewEdit)
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CREATE()
	// DM7978 - LME - YI-5415
	ON_WM_CTLCOLOR_REFLECT()
	//ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetBkOpaque)
	ON_WM_LBUTTONDOWN()
	//ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResaRailViewEdit message handlers

void CResaRailViewEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	// DM7978 - LME - YI-5415 -- corrige le bug d'affichage lorsque l'on ecrit
	Invalidate(true);
	// DM7978 - LME - FIN

	switch (nChar)
	{
	case 0x10 :  // shift key - user pressed it, turn boolean on
		{
			((CResaRailView*)GetParent())->SetShiftKeyDown( TRUE );
			//      TRACE( "Shift key pressed\n");
			break;
		}
	case 0x28 :  // cursor down
	case 0x26 :  // cursor up
		{
			LPARAM lpTmp = ( nChar == 0x28 ) ? 
				(LPARAM)CResaRailView::DOWN_ARROW_KEY :
			(LPARAM)CResaRailView::UP_ARROW_KEY;
			GetParent()->PostMessage(WM_SWITCHFOCUS, (WPARAM)((CWnd*)this), lpTmp);
			break;
		}
	case 0x27 : // right cursor arrow
	case 0x25 : // left cursor arrow
		{
			int iStartChar, iEndChar;
			GetSel( iStartChar, iEndChar ) ;
			int iLen = LineLength();
			LPARAM lpTmp = ( nChar == 0x27 ) ? 
				(LPARAM)CResaRailView::RIGHT_ARROW_KEY :
			(LPARAM)CResaRailView::LEFT_ARROW_KEY;
			// are we at the beginning or the end of the text...
			if ( (iStartChar == iEndChar) && 
				( (iEndChar == iLen && nChar == 0x27) || (iEndChar == 0 && nChar == 0x25) ) )
			{
				GetParent()->PostMessage(WM_SWITCHFOCUS, (WPARAM)((CWnd*)this), lpTmp);
				break;
			}
			// default behavior for right and left cursor arrows...
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			break;
		}
	case 0x23 : // end
	case 0x24 : // home
	case 0x2d : // insert
	case 0x2e : // delete key
	case 0x33 : // page up
	case 0x34 : // page down
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			break;
		}
	case 0x11 : // ctrl key
		{
			((CResaRailView*)GetParent())->SetCtrlKeyDown( TRUE );
			//      TRACE( "Ctrl key pressed\n");
			break;
		}
	default:    // all other non-system char keys
		{
			//      CResaRailViewEdit::OnChar(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

void CResaRailViewEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case 0x10 :  // shift key - user released it, turn boolean off
		{
			((CResaRailView*)GetParent())->SetShiftKeyDown( FALSE );
			//      TRACE( "Shift key released\n");
			break;
		}
	case 0x11 : // ctrl key
		{
			((CResaRailView*)GetParent())->SetCtrlKeyDown( FALSE );
			//      TRACE( "Ctrl key released\n");
			break;
		}
	default:    // all other non-system char keys
		{
			CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

void CResaRailViewEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// DM7978 - LME - YI-5415 -- corrige le bug d'affichage lorsque l'on ecrit
	Invalidate(true);
	// DM7978 - LME - FIN

	switch (nChar)
	{
	case VK_TAB :  // tab
		{
			// perform a bit of checking before we set focus elsewhere
			CString szEditTxt;
			GetWindowText( szEditTxt );
			if ( szEditTxt.IsEmpty() )  // did the user completely remove all text from the edit window ?
				SetWindowText( "0" );
			LPARAM lpTmp = ( ! ((CResaRailView*)GetParent())->GetShiftKeyDown() ) ?
				(LPARAM)CResaRailView::RIGHT_ARROW_KEY :
			(LPARAM)CResaRailView::LEFT_ARROW_KEY;
			GetParent()->PostMessage(WM_SWITCHFOCUS, (WPARAM)((CWnd*)this), lpTmp);
			break;
		}
	case VK_BACK:  // backspace
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			SetEditChecked( FALSE );
			SetEditAdjusted( FALSE );
			break;
		}
	case VK_RETURN:  // enter (carriage return)
		{
			CString szEditTxt;
			GetWindowText( szEditTxt );
			if ( szEditTxt.IsEmpty() )
				SetWindowText( "0" );

			ResaRailViewEditEntry( TRUE );  // perform an entry of this auth or dmd value
			SetSel( 0, -1 );  // select al text

			break;
		}
	case 0xc:  // letter 'l'
		{
			if ( ((CResaRailView*)GetParent())->GetCtrlKeyDown() &&
				((CResaRailView*)GetParent())->GetCtrlKeyDown() )
			{
				//        TRACE("Ctrl-Shift-L pressed\n");
				((CCTAApp*)APP)->ToggleLoggingSQL();
			}
		}
	default:    // all other non-system char keys
		{
			if (IsCharAlphaNumeric((TCHAR)nChar) && !IsCharAlpha((TCHAR)nChar))
				CEdit::OnChar(nChar, nRepCnt, nFlags);
			else
				MessageBeep((UINT)-1);
			break;
		}
	}
}

void CResaRailViewEdit::OnSetfocus() 
{
	// set the member variable in the view to hold the edit window in focus
	((CResaRailView*)GetParent())->SetFocusEditWnd( this );
	SetSel( 0, -1 );
}


void CResaRailViewEdit::OnChange() 
{
	SetEditChecked( FALSE );
	SetEditAdjusted( FALSE );

	// find this edit control in the map and notify the change
	UINT nId = GetDlgCtrlID();
	CResaCmptBktLine* pCmptBktLine;
	if( ((CResaRailView*)GetParent())->GetEditMapArray()->Lookup( nId, pCmptBktLine ) )
	{
		pCmptBktLine->SetAuthModify( TRUE );
		return;
	}

	CResaCmptBktSciLine* pCmptBktSciLine;
	if( ((CResaRailView*)GetParent())->GetEditSciMapArray()->Lookup( nId, pCmptBktSciLine ) )
	{
		pCmptBktSciLine->SetAuthModify( TRUE );
		return;
	}

	// il doit s'agir d'une SCG
	POSITION pos = ((CResaRailView*)GetParent())->GetCmptBktSciMap()->GetStartPosition();
	CString sKey;
	while( pos != NULL ) // loop through all compartments
	{
		CResaCmptBktSci* pCB = NULL;
		((CResaRailView*)GetParent())->GetCmptBktSciMap()->GetNextAssoc( pos, sKey, pCB );
		if (pCB->m_pEditScgCapa == this)
		{
			pCB->SetAuthModify (TRUE);
			return;
		}
	}
}

HBRUSH CResaRailViewEdit::CtlColor(CDC *pDC, UINT nCtlColor)
{
	if(!(HBRUSH)m_Brush) 
	{
		// m_Brush.DeleteObject();
		m_Brush.CreateSolidBrush(m_BackColor);
	}

	//pDC->SetBkColor(m_BackColor);
	pDC->SetBkMode(TRANSPARENT);
	//Invalidate(true);

	return (HBRUSH)m_Brush;
}

BOOL CResaRailView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CResaRailView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CPrintDialog dlg(FALSE);
	CScrollView::OnBeginPrinting(pDC, pInfo);
}

void CResaRailView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class

	CScrollView::OnEndPrinting(pDC, pInfo);
}


int CResaRailViewEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetLimitText( 3 );

	return 0;
}

void CResaRailViewEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GetParent()->PostMessage(
		WM_SWITCHFOCUS, 
		(WPARAM)((CWnd*)((CResaRailView*)GetParent())->GetFocusEditWnd()), 
		(LPARAM)CResaRailView::MOUSE_CLICK);

	CEdit::OnLButtonDown(nFlags, point);
}

/*void CResaRailViewStatic::OnPaint() 
{
CPaintDC dc(this); // device context for painting

COLORREF clr = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetColorTextBkMain();
dc.SetBkColor(clr);

// Do not call CStatic::OnPaint() for painting messages
}*/

// DM7978 - LME - YI-5405 - implementation des methodes de la classe CCmptFilteredArray utilisees pour le filtrage (onglet reduit)
int CCmptFilteredArray::IndexOf(long value) const
{
	for(int i = 0; i < GetSize(); i++)
	{
		if(value == GetAt(i))
		{
			return i;
		}
	}
	return -1;
}

void CCmptFilteredArray::Push(long value)
{
	if(IndexOf(value) == -1)
	{
		Add(value);
	}
}
// DM7978 - LME - FIN
