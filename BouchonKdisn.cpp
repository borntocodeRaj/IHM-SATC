#include "StdAfx.h"







#include "BouchonKdisn.h"


#include <vector>

BouchonKdisn* BouchonKdisn::m_pInstance = NULL;

BouchonKdisn::BouchonKdisn(void)
{
}

BouchonKdisn::~BouchonKdisn(void)
{
}

BouchonKdisn* BouchonKdisn::getInstance()
{
	if (m_pInstance == NULL)
		m_pInstance = new BouchonKdisn();

	return m_pInstance;
}

// DM7978 - LME - sendKdisn (inspire du OnNotify qui recupere la reponse KDISN)
// Cette methode simule une reponse de la COMM suite a un envoi de KDISN et remplit la map m_TrancheList 
bool BouchonKdisn::SendKdisn(YM_Observer *observer, CString& msg)
{
	ParseKdisn(msg);
	KdisnQuery();
	CString kdisnRep = GenerateKdisnRep();
	YM_Msg* msg2 = new YM_Msg(kdisnRep, YM_Msg::MSG_DISPO);  
	observer->OnNotify(msg2);
	return true;
}

// 
void BouchonKdisn::ParseKdisn(CString& kdisn)
{
	std::ostringstream oss;
	// Parsage du KDISN
	// NumTrain
	int numTrainIdxDeb = kdisn.Find('N') + 1;
	int numTrainIdxFin = kdisn.Find('/') - 1;
	CString szNumTrain = kdisn.Mid(numTrainIdxDeb, numTrainIdxFin - numTrainIdxDeb + 1);
	TrainNumber(atoi(szNumTrain));
	// DptDate
	kdisn.Delete(0, numTrainIdxFin + 2);
	CString szDay = kdisn.Left(2);
	int day = atoi(szDay);
	kdisn.Delete(0, 2);
	CString szMonth = kdisn.Left(3);
	int month = MONTH_LIST_ENGLISH.Find(szMonth)/3 + 1;
	kdisn.Delete(0, 3);
	CString szYear = kdisn.Left(2);
	int year = atoi(szYear);
	kdisn.Delete(0, 2);
	RWDate rwDate(day, month, year+2000);
	DptDate(rwDate.julian());
	// LegOrig
	Orig(kdisn.Left(5));
	kdisn.Delete(0, 5);
	// LegDest
	Dest(kdisn.Left(5));
}

// Envoie la requete donnant les num de tranches
// Remplit la vector m_KdisnQueryResult
void BouchonKdisn::KdisnQuery()
{
	// Envoi de la requete
	YmIcBouchonKdisnDom dom;
	dom.TrainNo(TrainNumber());
	dom.DptDate(DptDate());
	dom.TrancheOrig(Orig());
	dom.TrancheDest(Dest());

	CCTAApp* WinApp = (CCTAApp*)AfxGetApp();
	YM_Query* pSQL = new YM_Query(* (WinApp->m_pDatabase), FALSE);
	pSQL->SetDomain(&dom);
	RWDBReader reader(WinApp->m_pDatabase->Transact(pSQL, IDS_SQL_SELECT_BOUCHON_KDISN));
	
	m_KdisnQueryResult.clear();
	while(reader())
	{
		if (reader.isValid())
		{
			reader >> dom;
			m_KdisnQueryResult.push_back(dom);
		}
	}

	delete pSQL;
}

// Genere une reponse KDISN en parsant m_KdisnQueryResult sous forme de CString
CString BouchonKdisn::GenerateKdisnRep()
{
  CString str;

  // retourne un train multi tranche qq soit le train
  #ifdef DEBUG_DT_4417

  if (TrainNumber() == 6101)
  {
  m_KdisnQueryResult.resize(4);
  m_KdisnQueryResult[0].LegOrig("FRPLY");
  m_KdisnQueryResult[0].LegDest("FRLPD");
  m_KdisnQueryResult[0].TrancheNo(6101);
  m_KdisnQueryResult[1].LegOrig("FRPLY");
  m_KdisnQueryResult[1].LegDest("FRLPD");
  m_KdisnQueryResult[1].TrancheNo(6102);
  m_KdisnQueryResult[2].LegOrig("FRLPD");
  m_KdisnQueryResult[2].LegDest("FRMSC");
  m_KdisnQueryResult[2].TrancheNo(6101);
  m_KdisnQueryResult[3].LegOrig("FRLPD");
  m_KdisnQueryResult[3].LegDest("FRMSC");
  m_KdisnQueryResult[3].TrancheNo(6102); 
  }

  #endif

	if(m_KdisnQueryResult.size())
	{
		CString date;
		CString numtrain;
		str += "TRAIN-"; // TRAIN-XXXX
		numtrain.Format("%d",TrainNumber());
		str += numtrain;
		str += " DATE-"; // DATE-DDMMMYY
		RWDate rwDate(DptDate());
		date.Format ( "%2.2u", rwDate.dayOfMonth());
		date += MONTH_LIST_ENGLISH.Mid( (rwDate.month()-1)*3, 3 );
		str += date;
		date.Format ( "%2.2u", rwDate.year() % 100); 
		str += date;
		str += "\t";
		str += Orig();
		str += " ";
		str += Dest();
		str += "\n";

    YmIcBouchonKdisnDom domPrec;

		// Parcours de m_KdisnQueryResult
		// Attention la reponse de la requete IDS_SQL_SELECT_BOUCHON_KDISN doit etre triee par LEG_SEQ
		for(std::vector<YmIcBouchonKdisnDom>::size_type i = 0; i != m_KdisnQueryResult.size(); i++) 
		{
			CString numTranche;
			
			YmIcBouchonKdisnDom domCourant = m_KdisnQueryResult[i];

			if(!YmIcBouchonKdisnDom::IsSameLeg(domPrec, domCourant))
			{
				str += "\n";
				str += domCourant.LegOrig();
				str += " ";
				str += domCourant.LegDest();
				str += " - SN";
				numTranche.Format("%06d", domCourant.TrancheNo()); 
				str += numTranche;
			}
			else
			{
				str += " SN";
				numTranche.Format("%d", domCourant.TrancheNo()); 
				str += numTranche;
			}
			domPrec = domCourant;
		}
	}
	else
	{
		str += "00004352 TRAIN INEXISTANT";
	}
	return str;
}

void BouchonKdisn::OnNotify( YM_Observable* pObject )
{
	std::cout << " OnNotify " << std::endl;

	if (pObject->IsKindOf(RUNTIME_CLASS(YM_Msg)))
	{
		YM_Msg* msg = (YM_Msg*)pObject;
		std::cout << " Message " << std::endl;
	}

	/*if( pObject->IsKindOf( RUNTIME_CLASS(YM_RecordSet) ) )
    {
		YM_RecordSet* pRecordSet = (YM_RecordSet*)pObject;
		if (pRecordSet == m_pVnlStub) 
		{
			/// not available use query 
		}
		std::cout << " RecordSet " << std::endl;
	}

	else if (pObject->IsKindOf(RUNTIME_CLASS(YM_Query)))
	{
		std::cout << " Query " << std::endl;
	}*/
}
// DM7978 - LME - FIN
