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
#include <jack/jack.h>

#include <stdlib.h>
#include <string.h>
#include <limits>

#include "jack.h"
#include "io.h"


namespace mini {
namespace jack {

frames_t client_t::sample_rate() const
{
	return jack_get_sample_rate(client);
}

port_t client_t::register_port(const char* port_name,
	const char* port_type, unsigned long flags,
	unsigned long buffer_size)
{
	return jack_port_register(client, port_name,
				  port_type, flags, buffer_size);
}

int client_t::connect(const char *source_port, const char *destination_port)
{
	return jack_connect(client, source_port, destination_port);
}

void error_cb(const char* err) {
	io::mlog << "Jack Error: " << err << io::endl;
}

void info_cb(const char* info) {
	io::mlog << "Jack Info: " << info << io::endl;
}

client_t::client_t(const char* clientname) :
	client(jack_client_open (clientname, JackNullOption, nullptr))
{
	jack_set_process_callback(client, p_process, this);
	jack_on_shutdown(client, p_shutdown, this);
	jack_set_error_function(error_cb);
	jack_set_info_function(info_cb);
}

void client_t::init(const char *clientname)
{
	if(client)
	 throw "Client already open";
	client = jack_client_open (clientname, JackNullOption, nullptr);
	if(!client)
	 throw "Could not open jack client";
}

void client_t::activate()
{
	if(!client)
		throw "Can not activate non-opened client";
	if( jack_activate(client) )
		throw "Could not activate jack client";
}

client_t::~client_t()
{
	if(client)
	{
		jack_deactivate(client);
		jack_client_close(client);
	}
}

const char *port_t::name() const { return jack_port_name(port); }

void* port_t::_get_buffer(frames_t nframes)
{
	return jack_port_get_buffer(port, nframes);
}

const void* port_t::_get_buffer(frames_t nframes) const
{
	return jack_port_get_buffer(port, nframes);
}

}
}

