
#ifndef _BKTSCISPHERESPRIX_H_
#define _BKTSCISPHERESPRIX_H_
//DM7978 - KGH - Sphere de prix 

class CBktSciSphere
{
public:
	CBktSciSphere(void);
	CBktSciSphere(const CBktSciSphere&);
	CBktSciSphere&operator=(const CBktSciSphere&);
	virtual BOOL	operator==(const CBktSciSphere&) const;
	virtual ~CBktSciSphere() {;}
	virtual ostream &Print( ostream&) const;
	void setBktSciSphere(YmIcBktSciSpherePrixDom&);
//Setter	
	
	inline void setSphereId(long sphereId)					{m_sphereId = sphereId;}
	inline void setEspacePhysique(RWCString espacePhysique)	{m_espacePhysique = espacePhysique;}
	inline void setNestLevel(long nestLevel)				{m_nestLevel = nestLevel;}
	inline void setScx(long scx)							{m_scx = scx;}
	inline void setDptDateDebut(unsigned long dptDateDebut)	{m_dptDateDebut = dptDateDebut;}
	inline void setDptDateFin(unsigned long dptDateFin)		{m_dptDateFin = dptDateFin;}

//Getter
	inline long				getSphereId()			{return m_sphereId ;}
	inline const char*		getEspacePhysique()		{return m_espacePhysique ;}
	inline long				getNestLevel()			{return m_nestLevel ;}
	inline unsigned long	getDptDateDebut()		{return m_dptDateDebut ;}
	inline unsigned long	getDptDateFin()			{return m_dptDateFin ;}
	inline long				getScx()				{return m_scx ;}
	
	
	


protected:
	
	long			m_sphereId;
	RWCString		m_espacePhysique;
	long			m_nestLevel;
	long			m_scx;			// SC
	unsigned long	m_dptDateDebut;
	unsigned long	m_dptDateFin;

};
//DM7978 - KGH - Fin
#endif