/*************************************************************************/
/* mmms - minimal multimedia studio                                      */
/* Copyright (C) 2014-2014                                               */
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

#include <cstring>
#include "rtosc_string.h"

namespace mmms {

void inspect_rtosc_string(const std::vector<char>& str,
	std::ostream& stream) {

	for(const char& x : str)
	{
		std::cerr << +x << std::endl;
	}
	std::cerr << std::endl;

	if(str[0] != '/')
	 throw "rtosc string invalid: does not start with `/'";
	stream << "rtosc msg: \"" << str.data() << "\"" << std::endl;
	const char* c = str.data() + strlen(str.data());
	for(; !*c; ++c) ;
	if(*(c++)!=',')
	 throw "rtosc string invalid: type string does not start with `,'";
	std::string args = c;
	c = c + strlen(c);
	for(; !*c; ++c) ;
	for(const char& tp : args)
	{
		stream << " * ";
		switch(tp)
		{
			case 'i':
				stream << (int)(*(int32_t*)c);
				c += 4;
				break;
			case 'f':
				stream << *(float*)c;
				c += 4;
				break;
			default:
				stream << "(unknown type: " << tp << ")";
		}
		stream << std::endl;
		//std::cerr << "c - str.data(): " << c - str.data() << std::endl;
	}

	// TODO
}

}
