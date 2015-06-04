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

//! @file debug.h This file contains a project for thread testing

#ifndef DEBUG_H
#define DEBUG_H

#include <limits>
#include <set>
#include <thread>
#include "effect.h"
#include "mports.h"
#include "io.h"

namespace mini
{

/*template<class OutType = float>
struct lfo_t;

template<class OutType = float>
struct lfo_con : ef_con_t<lfo_t<OutType>>, public port_chain<freq_lfo_out<OutType>>
{
};*/

struct int_out : out_port_templ<int>
{
	using base::out_port_templ;
};

struct int_in_1 : in_port_templ<int>
{
	void on_read(sample_t ) {}
	using base::in_port_templ;
};

struct int_in_2 : in_port_templ<int>
{
	void on_read(sample_t ) {}
	using base::in_port_templ;
};

struct debug_effect_base : public effect_t
{
	void instantiate() {}
	void clean_up() {}
	using effect_t::effect_t;
};

struct start_t : public debug_effect_base, public int_out
{
	start_t() : debug_effect_base("start"), int_out((effect_t&)*this)
	{
		set_next_time(0);
	}

	void instantiate() {}
	void clean_up() {}

	// this will be only called on startup
	bool _proceed(sample_t ) {
		io::mlog << "proceed: start_t" << io::endl;
		set_next_time(std::numeric_limits<sample_t>::max());
		return true;
	}
};

class pipe_t : public debug_effect_base, public int_in_1, public int_out
{
	std::set<std::thread::id> threads_used;
public:
	std::size_t n_threads_used() const { return threads_used.size(); }
		
	pipe_t(std::size_t n_tasks) : debug_effect_base("pipe"),
		int_in_1((effect_t&)*this),
		int_out((effect_t&)*this)
	{
		max_threads.store(n_tasks);
		set_next_time(std::numeric_limits<sample_t>::max());
	}

	void instantiate() {}
	void clean_up() {}

	// this will be only called on startup
	bool _proceed(sample_t ) {
		io::mlog << "proceed: pipe_t" << io::endl;
		threads_used.insert(std::this_thread::get_id());

		//set_next_time(t + 1); // TODO: assertion if next time was not updated
		set_next_time(std::numeric_limits<sample_t>::max());
		return true;
	}
};

struct in2_t : public debug_effect_base, public int_in_1, public int_in_2
{
	in2_t() : debug_effect_base("sink"),
		int_in_1((effect_t&)*this),
		int_in_2((effect_t&)*this)
	{
		set_next_time(std::numeric_limits<sample_t>::max());
	}

	void instantiate() {}
	void clean_up() {}

	// this will be only called on startup
	bool _proceed(sample_t ) {
		io::mlog << "proceed: in2_t" << io::endl;
		//set_next_time(t + 1);
		set_next_time(std::numeric_limits<sample_t>::max());
		return true;
	}
};

}

#endif // DEBUG_H
