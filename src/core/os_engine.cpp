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

#include <unistd.h>
#include "os_engine.h"
#include "audio.h"
#include "io.h"
#include "audio_sink.h"

namespace mini {

bool os_engine_t::process(sample_no_t samples)
{
	engine_t::proceed(samples);

#if 0
	if(! player.sink() )
	 throw "no sink allocated?";
	m_reader_t& reader = player.sink()->get();

	// test reading buffers:
	// TODO : remove casts if sample typedefs get unsigned
	if(reader.read_space() < (std::size_t)samples)
		throw "not enough read space";

	auto rs = reader.read_max(samples);
	if(rs.size() < (std::size_t)samples) // TODO: remove casts soon?
	 throw "not enough space in rs";
#endif

/*	for(int side = 0; side < 2; ++side)
	{
		sample_t* buffer =
			out[side].get_buffer<sample_t>(samples);
		if(buffer)
		{



			for(std::size_t i = 0; i < rs.size(); ++i)
			 buffer[i] = rs[i][side];
		}
		else
		 throw "could not get buffer";

	}*/

	if((samples_until_now += samples) > limit)
	 stop();

	return true;
}

void os_engine_t::vrun(bars_t _limit)
{
	limit = as_samples_floor(_limit, info.samples_per_bar);

	run_thread = std::thread(os_engine_t::run_loop_static, this); // TODO: any nice way without static member function?
}

void os_engine_t::run_loop()
{
	// while ... if (process) stop();
	bool smps_left = true;
	do
	{
		smps_left = process(1024); // TODO!
		usleep(1000); // TODO!
	} while(smps_left);
}

}

