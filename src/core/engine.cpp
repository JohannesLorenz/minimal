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

#include <thread>
#include <threadpool/src/include/thread.h>

#include "engine.h"

namespace mini {

#if 0
other_tp::other_tp()
	: threads(/*std::thread::hardware_concurrency() - 1*/ 0) // TODO!!!
	 // TODO: allow custom number of threads
{
	for(threadpool::thread_t& t : threads)
	{
		t = threadpool::thread_t(*this);
	}
}
#endif

engine_t::~engine_t()
{
}

// TODO: && or & ? can multiple player access one project?
void engine_t::load_project(project_t& pro)
{
	player.set_project(pro);
}

void engine_t::proceed(sample_no_t samples)
{
	player.callback(samples);
}

/*void engine_t::play_until(bars_t end)
{
	player.play_until(as_samples_floor(end, info.samples_per_bar));
}*/

}

