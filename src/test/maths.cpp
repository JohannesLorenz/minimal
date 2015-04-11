/*************************************************************************/
/* test.cpp - test files for minimal                                     */
/* Copyright (C) 2014-2015                                               */
/* Johannes Lorenz (jlsf2013 @ sourceforge)                              */
/*                                                                       */
/* This program is free software; you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation; either version 3 of the License, or (at */
/* your option) any later version.                                       */
/* This program is distributed in the hope that it will be useful, but   */
/* WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      */
/* General Public License for more details.                              */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program; if not, write to the Free Software           */
/* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA  */
/*************************************************************************/

#include "bars.h"
#include "io.h"

using namespace mini;

template<class T1, class T2>
void throw_if_neq(const T1& x, const T2& y)
{
	if(!(x == y))
	{
		no_rt::mlog << "Error - not equal: " << x << " != " << y
			<< std::endl;
		throw "Elements not equal as expected";
	}
}

int main()
{
	try {
		throw_if_neq(lcm(42, 105), 210u);
		throw_if_neq(gcd(42, 105), 21u);
		
		throw_if_neq(bars_t(42,105), bars_t(2,5));
		throw_if_neq(bars_t(1,2) + bars_t(1, 3), bars_t(5,6));
		
		throw_if_neq(bars_t(42,105).floor(), 0);
		throw_if_neq(bars_t(21,4).floor(), 5);
		throw_if_neq(bars_t(23,4).rest(), return bars_t(3,4));

	} catch (const char* s)
	{
		no_rt::mlog << s << std::endl;
		return 1;
	}

	no_rt::mlog << "SUCCESS" << std::endl;
	return 0;
}


