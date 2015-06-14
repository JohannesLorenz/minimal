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

#ifndef JACK_ENGINE_H
#define JACK_ENGINE_H

#include "jack.h"
#include "engine.h"

namespace mini {

class jack_engine_t : public engine_t, public jack::client_t
{
	virtual int process(jack::frames_t samples)
	{
		engine_t::proceed(samples);
		for(int side = 0; side < 1; ++side)
		{
			ringbuffer_reader_t& reader = player.sink.get().data[side];
			if(reader.read_space() < samples)
			 throw "not enough read space";
			else
			{
				float* buffer = out[0].get_buffer(samples);
				if(buffer)
				{
					auto rs = reader.read_max(samples);
					if(rs.size() < samples)
					 throw "not enough space in rs";
					for(int i = 0; i < rs.size(); ++i)
					 buffer[i] = rs[i];
				}
				else
				 throw "could not get buffer";
			}
		}

		return 0; // 0 = no error, 1 = error
	}
	virtual void shutdown() {
		throw "shutdown not implemented :P";
	}

	virtual void get_sample_rate() { return sample_rate(); }

	jack::port_t out[2];

	void vrun();
public:
	jack_engine_t();
	const char* type_hint() { return "audio"; }
};

}

#endif // JACK_ENGINE_H
