/*! \file		AutoSql.h
*   \brief	    Lot : 7978
*               Classe de r�cup�ration des autorisations via requ�te SQL
*               
*   \author     Julien Lavigne
*   \date       26 Novembre 2014
*/
class GagAutorizDom;

BOOL findMatchingTrain(const long trainRef, YM_Iterator<GagAutorizDom>* it);

/*!
 * \fn		void CGetAutoSql()
 * \brief	Va r�cup�rer toute les autorisations pour une liste de train donn� et les ajoute directement dans la classe GagAutorizDom
 */
void CGetAutoSql(std::vector<GagAutorizDom>& gagAuto);


