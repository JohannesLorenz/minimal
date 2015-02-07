/*************************************************************************/
/* minimal - a minimal osc sequencer                                     */
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

#include <cstring>
#include <climits>

#include "osc_string.h"

template <typename T>
T swap_endian(T u)
{
	static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

	union
	{
		T u;
		unsigned char u8[sizeof(T)];
	} source, dest;

	source.u = u;

	for(std::size_t k = 0; k < sizeof(T); k++)
	 dest.u8[k] = source.u8[sizeof(T) - k - 1];

	return dest.u;
}

namespace mini {

std::ostream& operator<<(std::ostream& stream,
	const osc_string& r_str)
{
	const std::vector<char>& str = r_str._data;

#ifdef DUMP_RAW
	{
	bool first_word = true;
	stream << "RAW OSC string:" << std::endl;
	for(const char& x : str)
	{
		stream << +x;
		first_word = first_word && (bool)x;
		if(first_word && isprint(x))
		 stream << " ('" << x << "')";
		stream << std::endl;
	}
	}
#endif
	stream << std::endl;

	if(str[0] != '/')
	 throw "rtosc string invalid: does not start with `/'";
	stream << "rtosc msg: \"" << str.data() << "\"" << std::endl;
	std::vector<char>::const_iterator itr = str.begin() + strlen(str.data());
	++itr;
	itr += pad_next<4>(std::distance(str.begin(), itr));
	//for(; !*itr; ++itr) ;
	if(*(itr++)!=',')
	 throw "rtosc string invalid: type string does not start with `,'";
	const char* args = &*itr;
	itr += strlen(&*itr);
//	for(; !*itr; ++itr) ;
	++itr;
	itr += pad_next<4>(std::distance(str.begin(), itr));
	for(; *args && (itr != str.end()); ++args)
	{
		const std::size_t pos = std::distance(str.begin(), itr);
		stream << " * ";

		const char* c = &*itr;
		switch(*args)
		{
			case 's':
				stream << "string: " << c << std::endl;
				itr += strlen(c) + 1 + pad_next<4>(strlen(c) + 1);
				break;
			case 'i':
				stream << "int: " << swap_endian(*(int32_t*)c);
				itr += 4;
				break;
			case 'f':
				stream << "float: " << swap_endian(*(float*)c);
				itr += 4;
				break;
			default:
				stream << "(unknown type: " << *args << ")";
		}
		stream << " (pos: " << pos << ")" << std::endl;
		//std::cerr << "c - str.data(): " << c - str.data() << std::endl;
	}

	if(*args) // i.e. itr reached end too early
	{
		stream << " -> rtosc string not terminated here." << std::endl;
	}
	else if(itr != str.end())
	{
		stream << " -> rtosc string has overfluent bytes." << std::endl;
	}

	// TODO

	return stream;
}

}
