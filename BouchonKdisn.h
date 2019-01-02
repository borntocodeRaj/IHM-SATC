#ifndef BOUCHONKDISN_H
#define BOUCHONKDISN_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class YmIcBouchonKdisnDom;
class YM_RecordSet;
class YM_Observer;

class BouchonKdisn : public YM_Observer
{
	public :
		static BouchonKdisn* getInstance();
		virtual ~BouchonKdisn();

		virtual void OnNotify( YM_Observable* pObject );
		bool SendKdisn(YM_Observer *observer, CString& msg);

		void Orig(const CString& orig) {m_orig = orig;}
		void Dest(const CString& dest) {m_dest = dest;}
		void TrainNumber(const long& trainNumber) {m_trainNumber = trainNumber;}
		void DptDate(const unsigned long& dptDate) {m_dptDate = dptDate;}

		const CString& Orig() const {return m_orig;}
		const CString& Dest() const {return m_dest;}
		const int TrainNumber() const {return m_trainNumber;}
		const unsigned long DptDate() const {return m_dptDate;}

	protected :
		BouchonKdisn();

	private :
		static BouchonKdisn* m_pInstance;
		// DM7978 - LME - Bouchon KDISN
		void ParseKdisn(CString& kdisn);
		void KdisnQuery();
		CString GenerateKdisnRep();
		std::vector<YmIcBouchonKdisnDom> m_KdisnQueryResult;

		CString m_orig;
		CString m_dest;
		int m_trainNumber;
		unsigned long m_dptDate;
};


#endif 