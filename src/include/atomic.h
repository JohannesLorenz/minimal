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

#ifndef ATOMIC_H
#define ATOMIC_H

#include <atomic>

namespace mini
{

// TODO: check if relaxed is always acceptable
//! minimal atomic: an std atomic with relaxed memory order
template<class T>
class atomic
{
	std::atomic<T> at;
public:
	T load() const { return at.load(std::memory_order_acquire); }
	T load() const volatile { return at.load(std::memory_order_acquire); }

	void store(T x) { at.store(x, std::memory_order_release); }
	void store(T x) volatile { at.store(x, std::memory_order_release); }

	operator T() const { return at.operator T(); }
	operator T() const volatile { return at.operator T(); }

	T operator++() { return ++at; } // TODO: fetch add
	T operator++() volatile { return ++at; } // TODO: fetch add

	atomic() = default;
	constexpr atomic(T desired) : at(desired) {}
	atomic(const atomic&) = delete;

	std::atomic<T>& impl() { return at; }
	const std::atomic<T>& impl() const { return at; }
};

//! minimal atomic with default construction value
template<class T, T DefaultValue>
class atomic_def : public atomic<T>
{
public:
	atomic_def() : atomic<T>(DefaultValue) {}
	atomic_def(const atomic_def&) = delete;
};

}

#endif // ATOMIC_H
