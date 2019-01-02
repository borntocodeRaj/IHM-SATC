/*! \file		AutoSql.h
*   \brief	    Lot : 7978
*               Classe de récupération des autorisations via requête SQL
*               
*   \author     Julien Lavigne
*   \date       26 Novembre 2014
*/

#include "StdAfx.h"


#include "GagAutoCc.h"



#define		MAX_TRAINS_IN_QUERY		998


BOOL findMatchingTrain(const long trainRef, YM_Iterator<GagAutorizDom>* it)
{
	it->First();
	for (it->First(); !it->Finished(); it->Next())
	{
		if (it->Current()->IdAuto() == trainRef)
			return (true);
	}
	return (false);
}


/*!
* \fn		void CGetAutoSql(std::vector<GagAutorizDom>& gagAuto)
* \brief	Va récupérer toute les autorisations pour une liste de train donné et les ajoute directement dans la classe GagAutorizDom
* \param   autokey correspond à la key du tableau <GagAutorizDom*> où sont stocké tous les trains.
*/
void CGetAutoSql(std::vector<GagAutorizDom>& gagAuto)//SRE 80766 - Partially rewritten function, to gain some speed
{
	int number_train = 0;
	int max_train = 0;
	int currentTrain = -1;
	CString ep;
	
	YM_Iterator<GagAutorizDom>* pIterator = YM_Set<GagAutorizDom>::FromKey (TGVGAG_KEY)->CreateIterator();
	
	if(pIterator->GetCount() !=0)
	{
		GagAutoCcDom* gautocc = new GagAutoCcDom();
		YM_Query* pQuery = new YM_Query (*APP->m_pDatabase, FALSE);
		CString train, condition;
		int extracted = 0;

		//SRE 80766 - Calculation of the required loops number in order to do our queries, limited to MAX_TRAINS_IN_QUERY
		int loops = (pIterator->GetCount() / MAX_TRAINS_IN_QUERY) + ((pIterator->GetCount() % MAX_TRAINS_IN_QUERY) >= 1);

		TRACE("DBG: Train number: %d, %d loops\n", train, loops);
		pIterator->First();
		while (loops != 0)
		{
			if (extracted != 0)
			{
				pIterator->First();
				for (int progression = 0; progression < extracted; progression++)
					pIterator->Next();
			}
			train.Format("%d", ((GagAutorizDom*)pIterator->Current())->IdAuto());
			condition = "AND REF_ID_AUTO in ('" +  train + "'";
			extracted = extracted + 1;
			for (pIterator->Next(); !pIterator->Finished() && (extracted % MAX_TRAINS_IN_QUERY); pIterator->Next(), extracted++)
			{
				condition += ","; //SRE 80766 - Autorisations query, now composed of 998 trains instead of just 1 (limited to be < 1000 by ORACLE dll)
				train.Format("%d", pIterator->Current()->IdAuto());
				condition += "'" + train + "'";
			}
			condition += ")";
			gautocc->WhereClause(condition);
			pQuery->SetDomain(gautocc);
			bool bRecalcCurTrain = true;
			RWDBReader Reader( APP->m_pDatabase->Transact(pQuery, IDS_SQL_SELECT_AUTO_CC_GAG) );
			
			while (Reader())//SRE 80766 - Read our rows (X to X + 0 >= X < X + 998)
			{
				if (Reader.isValid())
				{
					Reader >> *gautocc;
					if (gautocc->IdAuto() != currentTrain || bRecalcCurTrain)
					{
						findMatchingTrain(gautocc->IdAuto(), pIterator);//SRE 80766 - Find train corresponding to the one we are currently reading (it's autorisations) in DB 
						currentTrain = gautocc->IdAuto();
						bRecalcCurTrain = false;
					}
					ep = gautocc->EP();
					if(gautocc->TypeClasse() == "CC" /*"SC"*/)//SRE 80766 - Corrected mistake contained in the original code
					{
						if (GagConfigMaxClasses::Instance()._maxCC[ep.GetAt(0)] >= gautocc->NestLevel() )
						{
							//pIterator->Current()->SetSCXAutorisation(ep.GetAt(0),gautocc->NestLevel(),gautocc->Autoriz());
							pIterator->Current()->SetCCAutorisation(ep.GetAt(0),gautocc->NestLevel(),gautocc->Autoriz());//SRE 80766 - Corrected mistake contained in the original code
						}
					}
					else	
					{
						if (GagConfigMaxClasses::Instance()._maxSCX[ep.GetAt(0)] >= gautocc->NestLevel() )
						{
							//pIterator->Current()->SetCCAutorisation(ep.GetAt(0),gautocc->NestLevel(),gautocc->Autoriz());
							pIterator->Current()->SetSCXAutorisation(ep.GetAt(0),gautocc->NestLevel(),gautocc->Autoriz());//SRE 80766 - Corrected mistake contained in the original code
						}
					}
				}

			}
			loops--;
		}

		for (pIterator->First(); !pIterator->Finished(); pIterator->Next())
		{
			gagAuto.push_back(*(pIterator->Current()));//SRE 80766 - Push entire YM_Set (original code behavior)
		}
		
		delete pQuery;
		delete gautocc;

		/*
		// Boucle sur les trains
		for ( pIterator->First(); !pIterator->Finished(); pIterator->Next() )
		{
			GagAutorizDom autoriz =  *(pIterator->Current());
			train.Format("%d", autoriz.IdAuto());
			condition+=" '"+ train +"')";
			GagAutoCcDom* gautocc = new GagAutoCcDom();
			gautocc->WhereClause(condition);
			YM_Query* pQuery = new YM_Query (*APP->m_pDatabase, FALSE);
			pQuery->SetDomain(gautocc);
			RWDBReader Reader( APP->m_pDatabase->Transact(pQuery, IDS_SQL_SELECT_AUTO_CC_GAG) );

			// Boucle sur les cmpt/bkt
			while (Reader())
			{
				if (Reader.isValid())
				{
					Reader >> *gautocc;

					ep = gautocc->EP();
					if(gautocc->TypeClasse() == "SC")
					{
						if (GagConfigMaxClasses::Instance()._maxCC[ep.GetAt(0)] >= gautocc->NestLevel() )
						{
							autoriz.SetSCXAutorisation(ep.GetAt(0),gautocc->NestLevel(),gautocc->Autoriz());
						}
					}
					else
					{
						if (GagConfigMaxClasses::Instance()._maxSCX[ep.GetAt(0)] >= gautocc->NestLevel() )
						{
							autoriz.SetCCAutorisation(ep.GetAt(0),gautocc->NestLevel(),gautocc->Autoriz());
						}
					}
				}// isValid()
			}// while
			delete pQuery;
			delete gautocc;
			condition = "AND REF_ID_AUTO in ("; 
			gagAuto.push_back(autoriz);
		}*/
	}
	delete pIterator;
}