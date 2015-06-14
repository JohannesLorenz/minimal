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

#include <limits>
#include "debug.h"
#include "io.h"

namespace mini {

atomic_def<int, 0> pipe_t::counter;

start_t::start_t() : debug_effect_base("start"), int_out((effect_t&)*this)
{
	init_next_time(0);
}

bool start_t::_proceed(sample_no_t ) {
	io::mlog << "proceed: start_t" << io::endl;
	set_next_time(std::numeric_limits<sample_no_t>::max());
	return true;
}

pipe_t::pipe_t(std::size_t n_tasks, int expected)
	: debug_effect_base("pipe"),
	  int_in_1((effect_t&)*this),
	  int_out((effect_t&)*this),
	  expected(expected)
{
	max_threads.store(n_tasks);
	init_next_time(std::numeric_limits<sample_no_t>::max());

	sum_this.store(0);
}

pipe_t::~pipe_t()
{
	if(sum_this != expected)
		{
			io::mlog << "Pipe with effect id " << effect_t::id()
				 << ": sum = " << sum_this << ", expected = "
				 << expected << io::endl;
			throw "Test failed for pipe.";
		}
}

bool pipe_t::_proceed(sample_no_t ) {

	sum_this += (++counter);

	io::mlog << "proceed: pipe_t" << io::endl;
	threads_used.insert(std::this_thread::get_id());

	//set_next_time(t + 1); // TODO: assertion if next time was not updated

	// TODO: find a way that this is only allowed when all threads are finished
	set_next_time(std::numeric_limits<sample_no_t>::max());
	return true;
}

in2_t::in2_t() : debug_effect_base("sink"),
	int_in_1((effect_t&)*this),
	int_in_2((effect_t&)*this)
{
	init_next_time(std::numeric_limits<sample_no_t>::max());
}

bool in2_t::_proceed(sample_no_t ) {
	io::mlog << "proceed: in2_t" << io::endl;
	//set_next_time(t + 1);
	set_next_time(std::numeric_limits<sample_no_t>::max());
	return true;
}

}

