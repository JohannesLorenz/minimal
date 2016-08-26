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

#include <jack/jack.h>
#include "audio_instrument.h"
#include "io.h"

namespace mini
{

int audio_instrument_t::process (jack_nframes_t /*nframes*/)
{
#if 0
	// copy nframes samples to a memory area and set pointer
	float* mem0 = ports[0].get_buffer<float>(nframes);
	float* mem1 = ports[1].get_buffer<float>(nframes);

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
	std::size_t to_write = nframes * sizeof(jack_default_audio_sample_no_t);

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
#endif
	return 0;
}

void audio_instrument_t::init_2()
{
	init();
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
	audio_out((effect_t&)*this, rb_size)
{
	add_out_port(static_cast<audio_out*>(this));
}

template<class T>
void test(out_port_t<T>&) {}

void audio_instrument_t::init(/*jack_client_t &client*/)
{
#if 0
	no_rt::mlog << "initing with pid: " << pid << std::endl;
	client.init(("jack_client_" + os_pid_as_padded_string(pid)).c_str());
	no_rt::mlog << "init" << std::endl;

	client.activate(); // TODO: call this at end of init() ?

	// load ringbuffers into cache
	audio_out::data[0].touch();
	audio_out::data[1].touch();

	ports[0] = client.register_port("rb0", JACK_DEFAULT_AUDIO_TYPE,
			JackPortIsInput, 0);
	ports[1] = client.register_port("rb1", JACK_DEFAULT_AUDIO_TYPE,
			JackPortIsInput, 0);
	if(!ports[0] || !ports[1])
	 throw "can not register port";

	const std::string z_client_name = "zynaddsubfx_" + os_pid_as_padded_string(pid);
	std::string z_port_names[2] = { z_client_name + ":out_" + std::to_string(1),
		z_client_name + ":out_" + std::to_string(2) };

	no_rt::mlog << "available jack ports: " << std::endl;
	system("jack_lsp");
	no_rt::mlog << z_port_names[0] << " -> " << ports[0].name() << std::endl;
	no_rt::mlog << z_port_names[1] << " -> " << ports[1].name() << std::endl;

	if(!client.client)
	 throw "CLIENT";

	if (client.connect(z_port_names[0].c_str(), ports[0].name()) // TODO: out_1 from where?
		|| client.connect (z_port_names[1].c_str(), ports[1].name())) {

		int prob = client.connect(z_port_names[0].c_str(), ports[0].name());
		no_rt::mlog << "PROBLEM: " << prob << std::endl;
		perror("???");

		throw "cannot connect input port TODO to TODO";
	}

	jack_set_process_callback(client.client, _process, this);
	jack_on_shutdown (client.client, _shutdown, this);
#endif
}

}
