#include <Engine_pch.h>
#include "ID.h"

namespace Insight {

	size_t ID::ms_uniqueID = 0;

	ID::ID()
	{
		m_Id = GetUniqueID();
	}

	std::string ID::GetUniqueID()
	{
		return ("uid_" + std::to_string(ms_uniqueID++));
	}

}

