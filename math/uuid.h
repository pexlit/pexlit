#include "globalFunctions.h"
#include <sstream>
#include "array/arrayFunctions/arrayFunctions.h"
#pragma once
struct uuid
{
	uint idInts[4]{};

	//returns true if this uuid is initialized with a reference
	constexpr operator bool() const
	{
		return idInts[0] || idInts[1] || idInts[2] || idInts[3];
	}
	constexpr uuid()
	{
	}
	constexpr uuid(cuint(&idInts)[4])
	{
		copyAllElements(idInts, &this->idInts[0]);
	}
	constexpr bool operator == (const uuid& other) const
	{
		return idInts[0] == other.idInts[0] &&
			idInts[1] == other.idInts[1] &&
			idInts[2] == other.idInts[2] &&
			idInts[3] == other.idInts[3];
	}
	constexpr bool operator != (const uuid& other) const
	{
		return idInts[0] != other.idInts[0] ||
			idInts[1] != other.idInts[1] ||
			idInts[2] != other.idInts[2] ||
			idInts[3] != other.idInts[3];
	}
	//not necessary
	/*inline uuid operator++(int)
	{
		uuid old = *this;
		(*this)++;
		return old;
	}*/

	//prefix, use this to get the next uuid
	constexpr uuid& operator++()
	{
		idInts[0]++;
		if (idInts[0] == 0)
		{
			idInts[1]++;
			if (idInts[1] == 0)
			{
				idInts[2]++;
				if (idInts[2] == 0)
				{
					idInts[3]++;
				}
			}
		}
		return *this;
	}
	inline operator std::wstring() const {
		std::wstringstream ss;
		ss << std::hex <<
			idInts[0] << L"-" <<
			idInts[1] << L"-" <<
			idInts[2] << L"-" <<
			idInts[3];
		return ss.str();
	}
};
// 1) Provide a std::hash<uuid> specialization:
namespace std {
	template<>
	struct hash<uuid> {
		size_t operator()(const uuid& u) const noexcept {
			//just combine the first two integers. when a UUID is truly random, this shouldn't matter.
			return u.idInts[0] + u.idInts[1] * 0x100000000;
		}
	};
}