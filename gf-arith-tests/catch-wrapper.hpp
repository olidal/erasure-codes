#ifndef CATCH_WRAPPER_HPP
#define CATCH_WRAPPER_HPP

#define CATCH_CONFIG_MAIN 

#ifdef _MSC_VER
#pragma warning (disable : 4244)
#endif

#include "catch.hpp"

#ifdef _MSC_VER
#pragma warning (default : 4244)
#endif

#include <iostream>
#include "matrix.h"

namespace gfarith
{
	std::ostream& operator<<(std::ostream& os, const matrix& m)
	{
		os << "{ ";
		bool rstart = true, cstart;
		for (size_t r = 0; r < m.size1(); ++r)
		{
			if (!rstart)
				os << ", ";
			else
				rstart = false;

			cstart = true;
			os << "{ ";

			for (size_t c = 0; c < m.size2(); ++c)
			{
				if (!cstart)
					os << ", ";
				else
					cstart = false;

				os << (int)m(r, c).value;
			}

			os << " }";
		}

		os << " }";

		return os;
	}
}

#endif
