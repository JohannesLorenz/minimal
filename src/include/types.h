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

#ifndef TYPES_H
#define TYPES_H

#include <string>

namespace mmms
{

//! note: if binary gets too large, we might need to not use templates...


template<class T, char _sign>
class par_base
{
public:
	using value_type = char;
	static constexpr char sign() { return _sign; }
};

class input_fixed {};
template<class T> struct no_port { using type = T; };

template</*class Input, */class InputPort, char _sign>
class variable : public par_base<typename InputPort::type, _sign>
{
	const InputPort& _input;
public:
	constexpr static bool is_const() { return false; }
	using type = typename InputPort::type;
	variable(const InputPort& input) : _input(input) {}
	const type& value() const { return _input.get(); }
};

template<class T, char _sign> // TODO: should be constexpr
class variable<no_port<T>, _sign> : public par_base<T, _sign>
{
	const T _value;
public:
	constexpr static bool is_const() { return true; }
	using type = T;
	const T& value() const { return _value; }
	variable(const T& value) : _value(value) {}
};


/*
template<class Input, class InputId, char _sign>
class variable : public par_base<decltype(Input().get(InputId())), _sign>
{
	const Input& _input;
	using T = decltype(_input.get(InputId()));
public:
	variable(const Input& input) : _input(input) {}
	const T& value() const { return _input.get(InputId()); }
};

template<class T, char _sign>
class fixed : public par_base<T, _sign>
{
	const T _value;
public:
	const T& value() const { return _value; }
	fixed(const T& value) : _value(value) {}
};

template<bool Fixed>
class oint : public fixed<int, 'i'>
{
	using fixed::fixed;
};

template<>
class oint<false> : public variable<int, 'i'>
{
	using variable::variable;
};*/

inline std::vector<char> store_int32_t(const int32_t i) {
	constexpr int32_t ff = 0xff;
	return {
		static_cast<char>((i>>24) & ff),
		static_cast<char>((i>>16) & ff),
		static_cast<char>((i>>8) & ff),
		static_cast<char>(i & ff) };
}

template<class InputPort = no_port<int>>
class oint : public variable<InputPort, 'i'>
{
	using base = variable<InputPort, 'i'>;
public:
	using variable<InputPort, 'i'>::variable;
	constexpr static bool size_fix() { return true; }
	constexpr static std::size_t size() { return 4; }

	std::vector<char> to_osc_string() const {
		return store_int32_t(base::value());
	}
};

template<class InputPort = no_port<float>>
class ofloat : public variable<InputPort, 'f'>
{
	using base = variable<InputPort, 'f'>;
public:
	using variable<InputPort, 'f'>::variable;
	constexpr static bool size_fix() { return true; }
	constexpr static std::size_t size() { return 4; }

	std::vector<char> to_osc_string() const {
		return store_int32_t(base::value());
	}




};

}

#endif // TYPES_H
