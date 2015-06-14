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

#include "jack_engine.h"
#include "io.h"

namespace mini {

jack_engine_t::jack_engine_t() :
	jack::client_t("minimal"),
	out{ register_port("outl", JACK_DEFAULT_AUDIO_TYPE,
		JackPortIsOutput, 0),
		register_port("outr", JACK_DEFAULT_AUDIO_TYPE,
		JackPortIsOutput, 0)}
{
}

void jack_engine_t::vrun()
{
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
}

}

