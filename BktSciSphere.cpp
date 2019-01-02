#include "StdAfx.h"

#include "BktSciSphere.h"
//DM7978 - KGH - Sphere de prix 


//SMD 
// Name: CBktSciSphere
// Desc: Default Constructor
// Signature: CBktSciSphere()
//EMD 
CBktSciSphere::CBktSciSphere()
{
	m_sphereId			= 0;
	m_espacePhysique	= "";
	m_nestLevel			= -1;
	m_scx				= -1;
	m_dptDateDebut		= 0L;
	m_dptDateFin		= 0L;
}

//SMD 
// Name: CBktSciSphere
// Desc: Copy Constructor
// Signature: CBktSciSphere( const CBktSciSphere &)
//EMD 
CBktSciSphere::CBktSciSphere(const CBktSciSphere & aCmpt)
{
	m_sphereId			= aCmpt.m_sphereId;
	m_espacePhysique	= aCmpt.m_espacePhysique;
	m_nestLevel			= aCmpt.m_nestLevel;
	m_scx				= aCmpt.m_scx;
	m_dptDateDebut		= aCmpt.m_dptDateDebut;
	m_dptDateFin		= aCmpt.m_dptDateFin;
}

//SMD 
// Name: operator=
// Desc: Assignment Constructor.
// Signature: CBktSciSphere &CBktSciSphere::operator=( const CBktSciSphere &)
//EMD 
CBktSciSphere &CBktSciSphere::operator=( const CBktSciSphere &aCmpt)
{
	m_sphereId			= aCmpt.m_sphereId;
	m_espacePhysique	= aCmpt.m_espacePhysique;
	m_nestLevel			= aCmpt.m_nestLevel;
	m_scx				= aCmpt.m_scx;
	m_dptDateDebut		= aCmpt.m_dptDateDebut;
	m_dptDateFin		= aCmpt.m_dptDateFin;

	return *this;
}

//SMD 
// Name: operator==
// Desc: Equality operator overloading.
// Signature: BOOL CBktSciSphere::operator==( const CBktSciSphere &)
//EMD 
BOOL CBktSciSphere::operator==( const CBktSciSphere &aCmpt) const
{
	return (
	 ( !strcmp(m_espacePhysique, aCmpt.m_espacePhysique))  &&
	 (m_sphereId		== aCmpt.m_sphereId)  &&
	 (m_nestLevel		== aCmpt.m_nestLevel)  &&
	 (m_scx				== aCmpt.m_scx)  &&
	 (m_dptDateDebut	== aCmpt.m_dptDateDebut) &&
	 (m_dptDateFin		== aCmpt.m_dptDateFin)
		);
}


//SMD 
// Name: Print
// Desc: Method to print the class attributes.
// Signature: ostream &Print( ostream &) const
//EMD 
ostream &CBktSciSphere::Print( ostream &os) const
{
	os					<< 
	"Sphere ID:"		<< m_sphereId									<< endl <<
	"Espace physique:"	<< m_espacePhysique								<< endl <<
	"NestLevel:"		<< m_nestLevel									<< endl <<
	"m_scx:"			<< m_scx										<< endl <<
	"DptDateDebut:"		<< RWDate(m_dptDateDebut).asString("%Y/%m/%d")	<< endl <<
	"DptDateFin:"		<< RWDate(m_dptDateFin).asString("%Y/%m/%d")	<< endl;

	return os;
}

void CBktSciSphere::setBktSciSphere(YmIcBktSciSpherePrixDom &dom)
{
	m_sphereId			= dom.getSphereId();
	m_espacePhysique	= dom.getEspacePhysique();
	m_nestLevel			= dom.getNestLevel();
	m_scx				= dom.getScx();
	m_dptDateDebut		= dom.getDptDateDebut();
	m_dptDateFin		= dom.getDptDateFin();
}
//DM7978 - KGH - FIN
