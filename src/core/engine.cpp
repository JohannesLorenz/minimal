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

#include <thread>

#include "threadpool/src/include/thread.h"
#include "engine.h"

namespace mini {

other_tp::other_tp()
	: threads(std::thread::hardware_concurrency() - 1)
	 // TODO: allow custom number of threads
{
	for(threadpool::thread_t& t : threads)
	{
		t = threadpool::thread_t(*this);
	}
}

engine_t::~engine_t()
{
}

void engine_t::load_project(project_t&& pro)
{
	lpro = std::move(pro);
}

void engine_t::play_until(float end)
{
	player_t<int> pl(lpro);
	pl.play_until(end);
}

}

