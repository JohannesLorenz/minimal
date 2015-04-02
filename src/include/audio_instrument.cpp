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

#include <jack/jack.h>
#include "audio_instrument.h"

constexpr std::size_t buffer_size = 1 << 14;
constexpr std::size_t sample_size = sizeof(jack_default_audio_sample_t);
constexpr std::size_t rb_size = buffer_size * sample_size;

namespace mini
{

int audio_instrument_t::process (jack_nframes_t nframes)
{
	// copy nframes samples to a memory area and set pointer
	float* mem0 = (float*)jack_port_get_buffer (ports[0], nframes);
	float* mem1 = (float*)jack_port_get_buffer (ports[1], nframes);

#if 0
	/* Sndfile requires interleaved data. It is simpler here to
	* just queue interleaved samples to a single ringbuffer. */
	for (i = 0; i < nframes; i++) {
	for (chn = 0; chn < nports; chn++) {
	if (jack_ringbuffer_write (rb, (void *) (in[chn]+i),
	sample_size)
	< sample_size)
	overruns++;
	}
	}
#endif
	std::size_t to_write = nframes * sizeof(jack_default_audio_sample_t);

	if(data[0].write_space() < to_write
		|| data[1].write_space() < to_write)
	{
		throw "panic";
	}
	else
	{
		data[0].write((char*)mem0, to_write);
		data[1].write((char*)mem1, to_write);
	}

	return 0;
}

void audio_instrument_t::init_2()
{
//	init_3();
}

int
_process (jack_nframes_t nframes, void *arg)
{
	return static_cast<audio_instrument_t*>(arg)->process(nframes);
}

void
_shutdown (void *arg)
{
	static_cast<audio_instrument_t*>(arg)->shutdown();
}

audio_instrument_t::audio_instrument_t(const char *name) :
	instrument_t(name),
	audio_out((effect_t&)*this, rb_size, rb_size),
	client(nullptr) // TODO
{
}

void audio_instrument_t::init(/*jack_client_t &client*/)
{
	// load ringbuffers into cache
	audio_out::data[0].touch();
	audio_out::data[1].touch();

	ports[0] = jack_port_register(client, "rb0", JACK_DEFAULT_AUDIO_TYPE,
			JackPortIsInput, 0);
	ports[1] = jack_port_register(client, "rb1", JACK_DEFAULT_AUDIO_TYPE,
			JackPortIsInput, 0);
	if(!ports[0] || !ports[1])
	 throw "can not register port";

	if (jack_connect (client, "out_1", "rb0") // TODO: out_1 from where?
		|| jack_connect (client, "out_2", "rb1")) {
		throw "cannot connect input port TODO to TODO";
	}

	jack_set_process_callback(client, _process, this);
	jack_on_shutdown (client, _shutdown, this);
}

}
