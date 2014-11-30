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

#ifndef RTOSC_STRING_H
#define RTOSC_STRING_H

#include <vector>
#include <iostream>

namespace mmms {

class rtosc_string
{
	std::vector<char> _data;
public:
	const char* raw() const { return _data.data(); }
	std::size_t size() const { return _data.size(); }

	std::ostream& inspect(std::ostream& stream = std::cerr) const {
		return stream << *this << std::endl;
	}

	friend std::ostream& operator<<(std::ostream& stream, const rtosc_string&);

	bool operator==(const rtosc_string& other) const {
		return _data == other._data; }
	bool operator<(const rtosc_string& other) const {
		return _data < other._data; }

	rtosc_string(std::vector<char>& _data) : _data(_data) {}
	rtosc_string(std::vector<char>&& _data) : _data(std::move(_data)) {}
	rtosc_string() {}
};



}

#endif // RTOSC_STRING_H
