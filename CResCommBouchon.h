/*! 
* \file		ResCommBouchon.h
* \brief	Classe permettant de simuler un bouchon avec la COMM
*               
* \author  Frederic Manisse
* \date    23/02/2015
* \since   DM7978
* \version DM7978
*/

#ifndef _RES_COMM_BOUCHON_H_
#define _RES_COMM_BOUCHON_H_

/********************************************************************************/ 
/*		section include															*/ 
/********************************************************************************/

#include "ResTracer.h"

/********************************************************************************/ 
/*		Déclaration de la classe CResCommBouchon								*/ 
/********************************************************************************/

class CResCommBouchon : public CResComm
{
public:

	/****************************************************************************/ 
	/*			re-implementation des fonctions virtuelles de la classe CResComm*/ 
	/****************************************************************************/

	//! Simule l'ouverture d'unne connection 
	virtual void OpenComm(BOOL resetbadtry = FALSE) 
	{ m_bAcceptedConnection  = TRUE; }

	//! Simule la fermeture d'une connection
	virtual void CloseComm() 
	{m_bAcceptedConnection  = FALSE; }

	//! Simule le check de l'état de fermeture
	virtual BOOL CanExit()
	{ return TRUE; }

	//! Simule la cessation d'activité
	virtual void StopActivity()
	{ }
		
};


#endif
//End of file

