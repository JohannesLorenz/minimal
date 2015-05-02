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
	set_next_time(std::numeric_limits<sample_t>::max());
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


	no_rt::mlog << "available jack ports: " << std::endl;
	system("jack_lsp");

	no_rt::mlog << "system:playback_1" << " <- " << ports[0].name() << std::endl;
	no_rt::mlog << "system:playback_2" << " <- " << ports[1].name() << std::endl;

	if (client.connect(ports[0].name(), "system:playback_1") // TODO: out_1 from where?
		|| client.connect (ports[1].name(), "system:playback_2")) {

		int prob = client.connect("system:playback_1", ports[0].name());
		no_rt::mlog << "PROBLEM: " << prob << std::endl;
		perror("???");

		throw "cannot connect input port TODO to TODO";
	}

	//jack_set_process_callback(client.client, _process, this);
	//jack_on_shutdown (client.client, _shutdown, this);
}

bool jack_player_t::_proceed(sample_t /*time*/)
{
	io::mlog << "JACKPLAYER" << io::endl;


	std::size_t read_space = std::min(
		audio_in::data[0].read_space(),
		audio_in::data[1].read_space());

	using jsample_t = jack_default_audio_sample_t; // TODO: incompatible?
	jsample_t* buf0 = ports[0].get_buffer<jsample_t>(read_space);
	jsample_t* buf1 = ports[1].get_buffer<jsample_t>(read_space);

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

	set_next_time(std::numeric_limits<sample_t>::max());
	return true;
}

}

