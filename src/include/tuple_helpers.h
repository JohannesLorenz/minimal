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

#ifndef TUPLE_HELPERS_H
#define TUPLE_HELPERS_H

#include <tuple>
#include <utility>

// adapted from [1] and [2]

// TODO: check what is unused

namespace tuple_helpers
{


template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
for_each(const std::tuple<Tp...> &, FuncT)
{ }

template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I < sizeof...(Tp), void>::type
for_each(const std::tuple<Tp...>& t, FuncT f)
{
	f(std::get<I>(t));
	for_each<I + 1, FuncT, Tp...>(t, f);
}

// matches for recursion
template<int Index, class Search, class First = void, class... Types>
struct get_internal
{
	typedef typename get_internal<Index + 1, Search, Types...>::type type;
	static constexpr int index = Index;
	static constexpr bool found = false;
};

// matches if nothing was found
template<int Index, class Search>
struct get_internal<Index, Search, void>
{
	typedef get_internal type;
	static constexpr int index = Index;
	static constexpr bool found = false;
};

// matches if type matched
template<int Index, class Search, class... Types>
struct get_internal<Index, Search, Search, Types...>
{
	typedef get_internal type;
	static constexpr int index = Index;
	static constexpr bool found = true;
};

template<class T, class... Types>
inline T& get(std::tuple<Types...>& tuple)
{
	static_assert(get_internal<0,T,Types...>::type::found,
		"You looked up a type which could not be found in the respective tuple.");
	T& ret = std::get<get_internal<0,T,Types...>::type::index>(tuple);
	return ret;
}

template<class T, class... Types>
inline const T& get(const std::tuple<Types...>& tuple)
{
	static_assert(get_internal<0,T,Types...>::type::found,
		"You looked up a type which could not be found in the respective tuple.");
	const T& ret = std::get<get_internal<0,T,Types...>::type::index>(tuple);
	return ret;
}

template<class T, class... Types>
constexpr bool is_in() {
	return get_internal<0,T,Types...>::type::found;
};

template<class T, class... Types>
inline constexpr bool contains(std::tuple<Types...>& )
{
	return is_in<T, Types...>();
}

static_assert(get_internal<0,int,int,char>::type::index == 0, "...");
static_assert(get_internal<0,int,char,int>::type::index == 1, "...");
static_assert(get_internal<0,int,char,bool>::type::index == 2, "...");

} // namespace tuple_helpers

// sources:
// [1] http://stackoverflow.com/questions/16594002/
//   for-stdtuple-how-to-get-data-by-type-and-how-to-get-type-by-index
// [2] http://stackoverflow.com/questions/1198260/iterate-over-tuple

#endif // TUPLE_HELPERS_H
