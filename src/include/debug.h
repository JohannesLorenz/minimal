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

//! @file debug.h This file contains a project for thread testing

#ifndef DEBUG_H
#define DEBUG_H

#include <set>
#include <thread>
#include "atomic.h"
#include "effect.h"
#include "mports.h"

namespace mini
{

struct int_out : out_port_t<int>
{
	using base::out_port_t;
};

struct int_in_1 : in_port_t<int>
{
	void on_read(sample_no_t ) {}
	using base::in_port_t;
};

struct int_in_2 : in_port_t<int>
{
	void on_read(sample_no_t ) {}
	using base::in_port_t;
};

struct debug_effect_base : public effect_t
{
	void instantiate() {}
	void clean_up() {}
	using effect_t::effect_t;
};

struct start_t : public debug_effect_base, public int_out
{
	start_t();

	void instantiate() {}
	void clean_up() {}

	// this will only be called on startup
	bool _proceed();
};

class pipe_t : public debug_effect_base, public int_in_1, public int_out
{
	std::set<std::thread::id> threads_used;
	const int expected;
public:
	std::size_t n_threads_used() const { return threads_used.size(); }
		
	pipe_t(std::size_t n_tasks, int expected);
	~pipe_t();

	void instantiate() {}
	void clean_up() {}

	static atomic_def<int, 0> counter;
	std::atomic<int> sum_this;

	// this will be only called on startup
	bool _proceed();
};

struct in2_t : public debug_effect_base, public int_in_1, public int_in_2
{
	in2_t();

	void instantiate() {}
	void clean_up() {}

	// this will be only called on startup // -> ??
	bool _proceed();
};

}

#endif // DEBUG_H
