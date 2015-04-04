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

#include "io.h"
#include "jack_player.h"

namespace mini {

jack_player_t::jack_player_t() :
	audio_in((effect_t&)*this, rb_size, rb_size)
{
	// TODO: error prone that programmer can forget this:
	set_next_time(std::numeric_limits<float>::max());
}

void jack_player_t::instantiate()
{
	client.init("jack_client_player");

	ports[0] = client.register_port("play_0", JACK_DEFAULT_AUDIO_TYPE,
			JackPortIsOutput, 0);
	ports[1] = client.register_port("play_1", JACK_DEFAULT_AUDIO_TYPE,
			JackPortIsOutput, 0);
	if(!ports[0] || !ports[1])
	 throw "can not register port";


	io::mlog_no_rt << "available jack ports: " << std::endl;
	system("jack_lsp");

	io::mlog_no_rt << "system:playback_1" << " <- " << jack_port_name(ports[0]) << std::endl;
	io::mlog_no_rt << "system:playback_2" << " <- " << jack_port_name(ports[1]) << std::endl;

	if (client.connect(jack_port_name(ports[0]), "system:playback_1") // TODO: out_1 from where?
		|| client.connect (jack_port_name(ports[1]), "system:playback_2")) {

		int prob = client.connect("system:playback_1", jack_port_name(ports[0]));
		io::mlog_no_rt << "PROBLEM: " << prob << std::endl;
		perror("???");

		throw "cannot connect input port TODO to TODO";
	}

	//jack_set_process_callback(client.client, _process, this);
	//jack_on_shutdown (client.client, _shutdown, this);
}

float jack_player_t::_proceed(float /*time*/)
{
	io::mlog << "JACKPLAYER" << io::endl;


	std::size_t read_space = std::min(
		audio_in::data[0].read_space(),
		audio_in::data[1].read_space());

	using jsample_t = jack_default_audio_sample_t;
	jsample_t* buf0 = (jsample_t *)jack_port_get_buffer(ports[0], read_space);
	jsample_t* buf1 = (jsample_t *)jack_port_get_buffer(ports[1], read_space);

	{
		auto rs = audio_in::data[0].read(read_space);

		for(std::size_t i = 0; i < read_space; ++i)
		 buf0[i] = rs[i];
		//std::copy_n(rs.???, read_space, buf0); // TODO
	}

	{
		auto rs = audio_in::data[1].read(read_space);

		for(std::size_t i = 0; i < read_space; ++i)
		 buf1[i] = rs[i];
		//std::copy_n(rs.???, read_space, buf0); // TODO
	}


	//multiplex<ringbuffer_t> rbs(rb_size, rb_size);
	//multiplex<ringbuffer_reader_t> rds(rb_size, rb_size);

	return std::numeric_limits<float>::max();
}

}

