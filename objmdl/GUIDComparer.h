#pragma once

struct GUIDComparer
{
	inline bool const operator() (const GUID & lhs, const GUID & rhs)
	{
		return ( memcmp( &lhs, &rhs, sizeof(GUID) ) > 0 ? true : false );
	}
};
