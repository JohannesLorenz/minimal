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

#ifndef PORTS_H
#define PORTS_H

#include <lo/lo_types.h>
#include <cstddef>

namespace mini {

class lo_port_t
{
	lo_address dest;
public:
	lo_port_t(const char* udp_port);
	lo_port_t(std::size_t udp_port); // TODO: udp port is which type?
	~lo_port_t();
	// TODO: does const make sense
	bool send_rtosc_msg(const char *path, const char *msg_args, ...) const;
	bool send_raw(const char *buffer, std::size_t len) const;
	//bool send_rtosc_msg(const char *path) const;
};

class lo_server_t
{
	lo_server srv;
	void handle_events();
public:
	lo_server_t();
	~lo_server_t();
};

}

#endif // PORTS_H
