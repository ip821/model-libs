#pragma once

struct GUIDComparer
{
	inline bool operator() (const GUID & lhs, const GUID & rhs) const
	{
		return ( memcmp( &lhs, &rhs, sizeof(GUID) ) > 0 ? true : false );
	}
};
