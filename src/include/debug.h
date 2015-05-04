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

#ifndef DEBUG_H
#define DEBUG_H

#include <limits>
#include "effect.h"
#include "ports.h"
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
	void on_recv(sample_t ) {}
	using base::in_port_templ;
};

struct int_in_2 : in_port_templ<int>
{
	void on_recv(sample_t ) {}
	using base::in_port_templ;
};

struct debug_effect_base : public effect_t
{
	void instantiate() {}
	void clean_up() {}
};

struct start_t : public debug_effect_base, public int_out
{
	start_t() : int_out((effect_t&)*this)
	{
		set_next_time(0.0f);
	}

	void instantiate() {}
	void clean_up() {}

	// this will be only called on startup
	bool _proceed(sample_t ) {
		io::mlog << "proceed: start_t" << io::endl;
		return true;
	}
};

struct pipe_t : public debug_effect_base, public int_in_1, public int_out
{
	pipe_t() : int_in_1((effect_t&)*this),
		int_out((effect_t&)*this)
	{
		set_next_time(std::numeric_limits<sample_t>::max());
	}

	void instantiate() {}
	void clean_up() {}

	// this will be only called on startup
	bool _proceed(sample_t ) {
		io::mlog << "proceed: pipe_t" << io::endl;
		return true;
	}
};

struct in2_t : public debug_effect_base, public int_in_1, public int_in_2
{
	in2_t() : int_in_1((effect_t&)*this),
		 int_in_2((effect_t&)*this)
	{
		set_next_time(std::numeric_limits<sample_t>::max());
	}

	void instantiate() {}
	void clean_up() {}

	// this will be only called on startup
	bool _proceed(sample_t ) {
		io::mlog << "proceed: in2_t" << io::endl;
		return true;
	}
};

}

#endif // DEBUG_H
