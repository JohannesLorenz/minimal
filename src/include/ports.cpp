/*************************************************************************/
/* mmms - minimal multimedia studio                                      */
/* Copyright (C) 2014-2014                                               */
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

#include <cstdarg>
#include <cstring>
#include <lo/lo.h>
#include <iostream>

#include "rtosc_string.h"
#include "rtosc/rtosc.h"
#include "ports.h"

namespace mmms {

int on_receive(const char *path, const char *, lo_arg **, int, lo_message msg, void *)
{
#if 0
	static char buffer[1024*20];
	memset(buffer, 0, sizeof(buffer));
	size_t size = sizeof(buffer);
	lo_message_serialise(msg, path, buffer, &size);
	if(!strcmp("/paths", buffer)) // /paths:sbsbsbsbsb...
	update_paths(buffer, NULL);
	else if(!strcmp("/exit", buffer))
	die_nicely(buffer, NULL);
	else if(status_url == path)
	update_status_info(buffer);
	else if(!strcmp("undo_change", buffer))
	;//ignore undo messages
	else
	display(buffer, NULL);

	return 0;
#else
	(void)path;
	(void)msg;
	return 0;
#endif
}

void error_cb(int i, const char *m, const char *loc)
{
#if 0
	wprintw(log, "liblo :-( %d-%s@%s\n",i,m,loc);
#else
	(void)i;
	(void)m;
	(void)loc;
#endif
}

void lo_server_t::handle_events()
{
	lo_server_recv_noblock(srv, 0);
}

lo_port_t::lo_port_t(const char *udp_port) :
	dest(lo_address_new(nullptr, udp_port))
{
	if(!dest)
		throw "Could not connect to lo dest port.";
}

lo_port_t::~lo_port_t()
{
	lo_address_free(dest);
}

bool lo_port_t::send_raw(const char *buffer, std::size_t len) const
{
	std::vector<char> v(buffer, buffer + len);
	rtosc_string rt(v);
	std::cerr << "sending raw: " << std::endl;
	rt.inspect();

	int result;
	lo_message msg = lo_message_deserialise((void*)buffer, len, &result);
	if(!msg)
	{
		std::cerr << "lo error: " << result << std::endl;
		throw "lo_message_deserialise";
	}
	if(dest)
	{
		int res = lo_send_message(dest, buffer, msg);
		std::cerr << "lo_send: " << res << std::endl;
	}
	else
	 throw "dest";
	return true;
}

bool lo_port_t::send_rtosc_msg(const char *path, const char *msg_args, ...) const
{
	va_list va;
	va_start(va, msg_args);
	char buffer[2048];
	size_t len = rtosc_vmessage(buffer, 2048, path, msg_args, va);
	if(!len)
	{
		throw "rtosc_vmessage";
	}
	va_end(va);

	return send_raw(buffer, len);
}


/*bool lo_port_t::send_rtosc_msg(const char *path) const
{
	char buffer[2048];
	strncpy(buffer, path, 2048);
	std::cerr << "buffer" << std::endl;
	lo_message msg = lo_message_deserialise(buffer, strlen(buffer), nullptr);
	if(!msg)
	 std::cerr << "NO MESSAGE!" << std::endl;
	if(dest)
	 lo_send_message(dest, buffer, msg);
	return true; // TODO
}*/

lo_server_t::lo_server_t()
	: srv(lo_server_new_with_proto(NULL, LO_UDP, error_cb))
{
	lo_server_add_method(srv, NULL, NULL, on_receive, NULL);
}

lo_server_t::~lo_server_t()
{
	lo_server_free(srv);
}

}

