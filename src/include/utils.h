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

#ifndef UTILS_H
#define UTILS_H

#include <vector>

//! This class is in no way copyable, but movable
class non_copyable_t
{
public:
	non_copyable_t() = default;

	non_copyable_t(const non_copyable_t&) = delete;
	non_copyable_t & operator=(const non_copyable_t&) = delete;

	non_copyable_t(const non_copyable_t&&) noexcept {}
};

class non_movable_t
{
public:
	non_movable_t() = default;

	non_movable_t(const non_copyable_t&&) noexcept = delete;
	non_movable_t(non_copyable_t&&) noexcept = delete;
	//non_movable_t(non_copyable_t&&) = delete;
};

//! counts elements of template parameter list. not thread safe.
template<class ...>
class counted_t
{
public:
	using id_t = std::size_t;
private:
	id_t _id;
	static id_t next_id;
public:
	const id_t& id() const noexcept { return _id; }
	counted_t() noexcept : _id(next_id++) {}
};

template<class ...Args>
typename counted_t<Args...>::id_t counted_t<Args...>::next_id = 0;

template<class Store, class ...Args>
class stored_t : public counted_t<Store, Args...>
{
private:
	static std::vector<Store> elems;
public:
	template<class ...Constr>
	Store& create(Constr... c)
	{
		return *elems.emplace(c...);
	}
	static const Store& at(const id_t& id) { return elems[id]; }
};

namespace util {

/*
	templates for structs that you don't want to instantiate
*/

template<typename ...> struct falsify : public std::false_type { };
template<typename T, T Arg> class falsify_id : public std::false_type { };
template<typename ...Args>
class dont_instantiate_me {
	static_assert(falsify<Args...>::value, "This should not be instantiated.");
	constexpr static std::size_t value = 0;
};
template<typename T, T Arg>
class dont_instantiate_me_id {
	static_assert(falsify_id<T, Arg>::value, "This should not be instantiated.");
	constexpr static std::size_t value = 0;
};

template<class T, class ...ConstrArgs>
constexpr T dont_instantiate_me_func(ConstrArgs... args) {
	return (void)dont_instantiate_me<T>::value, T(args...);
}

template<class T, T Arg, class ...ConstrArgs>
constexpr T dont_instantiate_me_id_func(ConstrArgs... args) {
	return (void)dont_instantiate_me_id<T, Arg>::value, T(args...);
}

template<int... Is>
struct seq { };

template<int N, int... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

template<int... Is>
struct gen_seq<0, Is...> : seq<Is...> { };

}

#endif // UTILS_H
