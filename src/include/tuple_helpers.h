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

#ifndef TUPLE_HELPERS_H
#define TUPLE_HELPERS_H

#include <tuple>

// adapted from [1]

// TODO: check what is unused

namespace tuple_helpers
{

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
constexpr bool is_in() {
	return get_internal<0,T,Types...>::type::found;
};

template<class T, class... Types>
inline constexpr bool contains(std::tuple<Types...>& tuple)
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

#endif // TUPLE_HELPERS_H
