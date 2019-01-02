/*
 *		SRE 76064 - File created to implement keyboard management in Bascules.
 *		Avoid cyclic inclusions
 */

#ifndef		BASCULES_TYPES_H_
# define	BASCULES_TYPES_H_

# define	WM_KB_LIST_END_REACHED	WM_USER + 50

namespace Bascules
{
	enum basculesTypes { BITRANCHE, ALLEO };
	//SRE 83311 - Add NEXTLIST and PREVIOUSLIST. Used to move between zones (SIA, SIa, SDA, SDa, etc...)
	enum endReached	{ BEGINNING, END, NEXTLIST, PREVIOUSLIST };
	class DirectionInfo
	{
	public:
		int index;
		endReached move;
	};
}

#endif		/* BACULES_TYPES_H_ */