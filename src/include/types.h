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

#ifndef TYPES_H
#define TYPES_H

#include <limits>
#include <vector>
#include <string>
#include <type_traits>
#include <cstring>

#include "utils.h"

namespace mini
{

//! note: if binary gets too large, we might need to not use templates...

class is_variable {};

template</*class Input, */class T>
class variable : is_variable
{
	T data;
public:
	const T& get() const { return data; }
	T& get() { return data; }
	void set(const T& new_data) { data = new_data; } // TODO: && alternative

	using type = T;

	/*variable(InputPort& input) : _input(&input) {}
	variable(InputPort* input) : _input(input) {}*/

	variable(const T& data) : data(data) { }
	variable() = default;



//	const type& value() const { return _input->get(); }
//	bool update() { return _input->update(); }
//	float get_next_time() const { return _input->get_outs_next_time(); }
};

#if 0
// an SFINAE classic:
/*template<class T>
class has_is_variable_func
{
	typedef char false_type[1];
	typedef char true_type[2];

	template<typename U>
	static true_type& tester(typename U::data_type*);

	template<typename>
	static false_type& tester(...);

public:
	static const bool value = sizeof(tester<T>(0)) == sizeof(true_type);
};*/

template<typename T, typename = void>
struct has_is_variable_func : std::false_type { };

template<typename T>
struct has_is_variable_func<T, decltype(std::declval<T>().is_variable, void())> : std::true_type { };

template<class T, bool> // true
struct _is_variable {
	static constexpr bool exec() {
		return T::is_variable;
	}
};

template<class T>
struct _is_variable<T, false> {
	static constexpr bool exec() {
		return false;
	}
};

template<class T>
static constexpr bool _is_variable() {
	return _is_variable<T, has_is_variable_func<T>::value>::exec();
}
#endif

template<class T>
static constexpr bool _is_variable() {
	return std::is_base_of<is_variable, T>::value;
}

template<class T, bool> // true
struct _type_of_variable {
	using type = typename T::type;
};

template<class T>
struct _type_of_variable<T, false> {
	using type = T;
};

template<class T>
using type_of_variable = typename _type_of_variable<T, _is_variable<T>()>::type;

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


/*template<class T>
struct pad_size// : util::dont_instantiate_me<T>
{
	constexpr static std::size_t value() {

	//	static_assert(has_is_variable_func<T>::value, "...");

		static_assert(_is_variable<T>(),
			"T must be either an OSC primitive or a variable.");
		return pad_size<type_of_variable<T>>::value();
	}
};*/

constexpr std::size_t c_strlen(const char* ptr) { return (*ptr) ? (c_strlen(ptr + 1) + 1) : 0; }

template<std::size_t PadSize>
constexpr std::size_t pad(std::size_t pos) {
	return pos + ((PadSize - pos % PadSize) % PadSize);
}

template<class T>
struct _pad_size : _pad_size<type_of_variable<T>> {};

template<>
struct _pad_size<char*> : std::integral_constant<std::size_t, 4> {};

template<>
struct _pad_size<int> : std::integral_constant<std::size_t, 4> {};

template<>
struct _pad_size<float> : std::integral_constant<std::size_t, 4> {};

constexpr std::size_t pad_size(const char* str) {
	return pad<4>(c_strlen(str) + 1);
}

template<class T>
constexpr std::size_t pad_size(const T& ) {
	return _pad_size<T>::value;
}

//! inherits from base type of variable
//! OOP can be really interesting sometimes...
/*template<class T> // TODO: clean up structs where you can use funcs now
struct pad_size<variable<T>> : public pad_size<T>
{
};*/

template<class T, bool>
struct _type_and_bool
{
	using type = T;
};

template<class T>
using type_and_bool = _type_and_bool<T, _is_variable<T>()>;


#if 0
template<class T>
bool _value(const typename T::type& elem) { return elem.get(); }

template<class T, bool b>
bool _value(const typename _type_and_bool<T, b>::type& elem) { return elem; }

template<class T>
bool value(const T& elem) { return _value<type_and_bool<T>>(elem); }
#endif




/*

template<class T, bool>
bool value(const variable<T>& v) { return v.get(); }

template<class T>
bool value(const T& elem) { return elem; }*/

/*template<class T>
struct detail_is_const
{
	constexpr static bool exec() { return !_is_variable<T>(); }
};

template<class T>
struct detail_is_const<const T>
{
	constexpr static bool exec() { return true; }
};

template<class T>
constexpr bool is_const() { return detail_is_const<T>::exec(); }
*/

template<class T>
struct is_const {
	constexpr static bool value = !_is_variable<T>();
};

template<class T>
struct is_const <const T> : std::true_type {};


/*
template<class T>
struct is_const
{
	constexpr static bool value() { return true; }
};

template<class T>
struct is_const<variable<T>>
{
	constexpr static bool value() { return false; }
};*/

template<class T>
struct size_fix;

template<class T, bool IsVar> // = true
struct _size_fix_2
{
	static constexpr bool value = size_fix<typename T::type>::value;
};

template<class T> // = false -> type unknown -> not fix
struct _size_fix_2<T, false> : std::false_type {};

template<class T>
struct size_fix
{
	static constexpr bool value = _size_fix_2<T, _is_variable<T>()>::value;
};

template<class T>
struct size_fix<const T> : std::true_type {};

template<>
struct size_fix<int> : std::true_type {};

template<>
struct size_fix<float> : std::true_type {};

#if 0

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
#endif

/*
template<class T>
struct get_type
{
	using type = T; // TODO: without struct?
};

template<class T>
struct get_type<variable<T>>
{
	using type = typename variable<T>::type; // TODO: without struct?
};*/

//template<class T>
//constexpr inline char sign() { return sign<typename T::type>(); }

template<char Sgn>
struct has_sign {
	static constexpr char value = Sgn;
};

/*template<class T>
constexpr inline char _sign() {
	static_assert(_is_variable<T>(), "T must be a variable or primitive.");
	return sign<typename T::type>();
}

template<>
constexpr inline char _sign<std::string>() { return 's'; }

template<>
constexpr inline char _sign<int>() { return 'i'; }

template<>
constexpr inline char _sign<float>() { return 'f'; }

template<>
constexpr inline char sign<float>() { return 'f'; }*/

template<class T>
struct sign : sign<typename T::type> {}; // TODO: notify that T must be a variable

template<class T>
struct sign<T const> : sign<T> {};

template<>
struct sign<const char*> : has_sign<'s'> {};

template<>
struct sign<int> : has_sign<'i'> {};

template<>
struct sign<float> : has_sign<'f'> {};

inline std::vector<char> store_int32_t(const int32_t* i) {
	constexpr int32_t ff = 0xff;
	return {
		static_cast<char>(((*i)>>24) & ff),
		static_cast<char>(((*i)>>16) & ff),
		static_cast<char>(((*i)>>8) & ff),
		static_cast<char>((*i) & ff) };
}

template<class T, bool b> // true
struct _get_value
{
	static const typename T::type& exec(const T& elem) {
		return elem.get();
	}
};

template<class T>
struct _get_value<T, false>
{
	static const T& exec(const T& elem) {
		return elem;
	}
};

template<class T>
using get_value = _get_value<T, _is_variable<T>()>;


template<class T>
std::vector<char> to_osc_string(const T& elem) {
	return store_int32_t((int32_t*)(&get_value<T>::exec(elem)));
}

inline std::vector<char> to_osc_string(const char* const& elem) {
	std::vector<char> res(pad_size(elem), 0);
	std::copy(elem, elem + strlen(elem) + 1, res.begin());
	return res;
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
