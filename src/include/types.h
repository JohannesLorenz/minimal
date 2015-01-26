/*************************************************************************/
/* minimal - a minimal osc sequencer                                     */
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

#include <limits>
#include <vector>
#include <string>

#include "utils.h"

namespace mini
{

//! note: if binary gets too large, we might need to not use templates...


template</*class Input, */class T>
class variable
{
	T data;
public:
	const T& get() const { return data; }
	T& get() { return data; }
	void set(const T& new_data) { data = new_data; } // TODO: && alternative

	using type = T;

	/*variable(InputPort& input) : _input(&input) {}
	variable(InputPort* input) : _input(input) {}*/

	variable(const T& data) : data(data) {}
	variable() {}



//	const type& value() const { return _input->get(); }
//	bool update() { return _input->update(); }
//	float get_next_time() const { return _input->get_outs_next_time(); }
};

/*
template<class T, char _sign> // TODO: should be constexpr
class variable<no_port<T>, _sign> : public par_base<T, _sign>, util::dont_instantiate_me<T>
{
	const T _value;
public:
//	constexpr static bool is_const() { return true; }
//	using type = T;
//	const T& value() const { return _value; }
	variable(const T& value) : _value(value) {
	}
};*/

template<class T>
struct pad_size : util::dont_instantiate_me<T>
{
	 // TODO: don't instantiate me!
	 constexpr static std::size_t value() { return 0; }
};

template<>
struct pad_size<int>
{
	constexpr static std::size_t value() { return 4; }
};

template<>
struct pad_size<float> : public pad_size<int> {};

//! inherits from base type of variable
//! OOP can be really interesting sometimes...
template<class T> // TODO: clean up structs where you can use funcs now
struct pad_size<variable<T>> : public pad_size<T>
{
};

template<class T>
bool value(const variable<T>& v) { return v.get(); }

template<class T>
bool value(const T& elem) { return elem; }

template<class T>
struct is_const
{
	constexpr static bool value() { return true; }
};

template<class T>
struct is_const<variable<T>>
{
	constexpr static bool value() { return false; }
};



template<class T>
constexpr bool _size_fix() { return false; } // TODO: don't instantiate me!

template<>
constexpr bool _size_fix<int>() { return true; }

template<>
constexpr bool _size_fix<float>() { return true; }


template<class T>
struct size_fix
{
	constexpr static bool value() { return _size_fix<T>(); }
};

template<class T>
struct size_fix<variable<T>>
{
	constexpr static bool value() {
		return _size_fix<typename variable<T>::type>(); }
};


template<class T>
struct get_type
{
	using type = T; // TODO: without struct?
};

template<class T>
struct get_type<variable<T>>
{
	using type = typename variable<T>::type; // TODO: without struct?
};

template<class T>
constexpr inline char sign() { return sign<typename T::type>(); }

template<>
constexpr inline char sign<int>() { return 'i'; }

template<>
constexpr inline char sign<float>() { return 'f'; }

inline std::vector<char> store_int32_t(const int32_t* i) {
	constexpr int32_t ff = 0xff;
	return {
		static_cast<char>(((*i)>>24) & ff),
		static_cast<char>(((*i)>>16) & ff),
		static_cast<char>(((*i)>>8) & ff),
		static_cast<char>((*i) & ff) };
}

template<class T>
std::vector<char> to_osc_string(const T& elem) {
	return store_int32_t((int32_t*)(&elem));
}

template<class T>
std::vector<char> to_osc_string(const variable<T>& v) {
	return to_osc_string(v.get());
}

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


using vint = variable<int>;
using vfloat = variable<float>;

#if 0
template<class InputPort>
class oint : public variable<InputPort, 'i'>
{
	using base = variable<InputPort, 'i'>;
public:
	using variable<InputPort, 'i'>::variable;
	//constexpr static bool size_fix() { return true; }

	std::vector<char> to_osc_string() const {
		return store_int32_t(base::value());
	}
};

template<class InputPort>
class ofloat : public variable<InputPort, 'f'>
{
	using base = variable<InputPort, 'f'>;
public:
	using variable<InputPort, 'f'>::variable;
	//constexpr static bool size_fix() { return true; }

	std::vector<char> to_osc_string() const {
		return store_int32_t(base::value());
	}




};
#endif


using osc_int = int;
using osc_float = float;


namespace variable_detail
{

#if 0
template<class T>
bool update(T& ) {
	return false;
}

template<class T>
bool update(variable<T>& v) {
	return v.update();
}

template<class T>
float get_next_time(const T& ) {
	return std::numeric_limits<float>::max();
}

template<class T>
float get_next_time(const variable<T>& v) {
	return v.get_next_time();
}
#endif

}


}

#endif // TYPES_H
