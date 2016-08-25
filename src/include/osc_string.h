/*************************************************************************/
/* minimal - a minimal osc sequencer                                     */
/* Copyright (C) 2014-2016                                               */
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

#ifndef OSC_STRING_H
#define OSC_STRING_H

#include <vector>
#include <cstring>

namespace mini {

template<std::size_t PadSize>
constexpr inline std::size_t pad_next(std::size_t pos) {
	return ((PadSize - pos % PadSize) % PadSize);
}

class osc_string
{
	std::vector<char> _data;
public:
	//! returns the full OSC string, including 0-bytes, as vector
	std::vector<char>& data() { return _data; }
	const std::vector<char>& data() const { return _data; }
	//! like data(), but returns the string as a C string
	const char* raw() const { return _data.data(); }
	//! returns the size of the full OSC string, including 0-bytes
	std::size_t size() const { return _data.size(); }

	std::vector<char>::iterator get_itr_type_str() {
		std::vector<char>::iterator itr = _data.begin() + strlen(_data.data());
		++itr;
		itr += pad_next<4>(std::distance(_data.begin(), itr));
		if(*(itr++)!=',')
		 throw "osc string invalid: type string does not start with `,'";
		return itr;
	}

	std::vector<char>::iterator get_itr_first_arg() {
		std::vector<char>::iterator itr = get_itr_type_str();
		itr += strlen(&*itr);
		++itr;
		itr += pad_next<4>(std::distance(_data.begin(), itr));
		return itr;
	}
	
	//! prints the OSC string verbosely, including a newline
	std::ostream& inspect(std::ostream& stream) const;
	//! prints the OSC string human readable
	friend std::ostream& operator<<(std::ostream& stream, const osc_string&);

	bool operator==(const osc_string& other) const {
		return _data == other._data; }
	//! lexicographical comparison of the full OSC strings with 0-bytes
	//! included
	bool operator<(const osc_string& other) const {
		return _data < other._data; }
	
	//! constructor from raw data
	osc_string(std::vector<char>& _data) : _data(_data) {}
	osc_string(std::vector<char>&& _data) : _data(std::move(_data)) {}
	osc_string() = default;
};



}

#endif // OSC_STRING_H
