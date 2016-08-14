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

#ifndef SIMPLE_H
#define SIMPLE_H

#include <iostream> // TODO: ugly!
#include <string>

namespace mini
{

class named_t
{
	const std::string _name;
public:
	const std::string& name() const { return _name; }
	named_t(const char* _name) : _name(_name) {}
	named_t(const std::string& _name) : _name(_name) {}
};

template<class T, T default_value>
class value_t
{
	T t = default_value;
public:
	const T& value() const { return t; }
	T& value() { return t; }
	// TODO: std::forward
	void set(const T& new_value) { t = new_value; }
	// TODO: std::forward
	value_t(const T& t) : t(t) {}
	value_t() = default;

	template<class T2, T2 default_value2>
	friend std::ostream& operator<<(std::ostream& os, const value_t<T2, default_value2>& val);
};

template<class T, T default_value>
std::ostream& operator<<(std::ostream& os, const value_t<T, default_value>& val)
{
	return os << val.value();
}

}

#endif // SIMPLE_H
