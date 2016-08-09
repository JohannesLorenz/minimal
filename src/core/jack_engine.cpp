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

#include <algorithm>
#include "jack_engine.h"
#include "audio.h"
#include "io.h"
#include "audio_sink.h"

namespace mini {

jack_engine_t::jack_engine_t() :
	jack::client_t("minimal"),
	out{ register_port("outl", JACK_DEFAULT_AUDIO_TYPE,
		JackPortIsOutput, 0),
		register_port("outr", JACK_DEFAULT_AUDIO_TYPE,
		JackPortIsOutput, 0)}
{
}

int jack_engine_t::process(jack::frames_t samples)
{
	engine_t::proceed(samples);

	m_reader_t& reader = player.sink()->get();

	if(reader.read_space() < samples)
		throw "not enough read space";

	auto rs = reader.read_max(samples);
	if(rs.size() < samples)
	 throw "not enough space in rs";

	for(int side = 0; side < 2; ++side)
	{
		sample_t* buffer =
			out[side].get_buffer<sample_t>(samples);
		if(buffer)
		{



			/*std::size_t first_sz = rs.first_half_size();
			std::copy_n(rs.first_half_ptr(),
				first_sz, buffer);
			std::copy_n(rs.second_half_ptr(),
				rs.second_half_size(),
				buffer + first_sz);*/
			// std::copy does not work because we have
			// ringbuffer<Stereo<float>>, and must output
			// buffer<float>
			float avg = 0.0f;
			for(std::size_t i = 0; i < rs.size(); ++i)
			{
				avg += buffer[i] = rs[i][side];
			}
			//std::cerr << "average: " << (avg/=rs.size()) << std::endl;
		}
		else
		 throw "could not get buffer";

	} // TODO: avoid throw here?

	if((samples_until_now += samples) > limit)
	 stop();

	// jack's return philosophy: 0 = no error, 1 = error
	return 0;
}

void jack_engine_t::vrun(bars_t _limit)
{
	limit = as_samples_floor(_limit, info.samples_per_bar);

	io::mlog << "Activating jack now..." << io::endl;
	activate();

	const char **outPorts = jack_get_ports(
				client,
				nullptr,
				nullptr,
				JackPortIsPhysical | JackPortIsInput);

	if(!outPorts || !outPorts[0] || !outPorts[1])
		throw "Could not connect to stereo output";

	// connect must be done after activate...
	connect(out[0].name(), outPorts[0]);
	connect(out[1].name(), outPorts[1]);

	io::mlog << "jack samplerate: " << sample_rate() << io::endl;
}

}

