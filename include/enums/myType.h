#pragma once

#include <map>
#include <string>

namespace mlnserver {
	struct MY_TYPE {
		using Ty = int;
		inline static std::map<Ty, std::string> DEFMAP;
	public:
		void setValue(const char* str, const Ty value) { DEFMAP[value] = str; }
#define _MLN_ENUM_DEFINE_MY_TYPE
#define DAT(a,b,c) const static Ty a=b; struct __register##a{__register##a(){DEFMAP[b]=c;};}; inline static __register##a __tempInstance##a;
#include "myType.dat"
#undef _MLN_ENUM_DEFINE_MY_TYPE
#undef DAT
	};
}
