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

#ifndef UTILS_H
#define UTILS_H

//! This class is in no way copyable, but movable
class non_copyable_t
{
public:
	non_copyable_t() = default;

	non_copyable_t(const non_copyable_t&) = delete;
	non_copyable_t & operator=(const non_copyable_t&) = delete;

	non_copyable_t(const non_copyable_t&&) noexcept {}
	const non_copyable_t & operator=(const non_copyable_t&&)
		const noexcept {
		return *this;
	}
};

#endif // UTILS_H
