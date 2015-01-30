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

#include <unistd.h> // TODO
#include <fstream>

#include "lo_port.h"
#include "config.h"
#include "zynaddsubfx.h"

namespace mini {

std::string zyn_impl::make_start_command() const
{
	const std::string cmd = ZYN_BINARY
		" --no-gui -O alsa"; // TODO: read from options file
	return cmd;
}

instrument_t::udp_port_t zyn_impl::get_port(pid_t pid, int) const
{
	udp_port_t port;
	std::string tmp_filename = "/tmp/zynaddsubfx_"
			+ std::to_string(pid);
	std::cout << "Reading " << tmp_filename << std::endl;
	sleep(1); // wait for zyn to be started... (TODO)
	std::ifstream stream(tmp_filename);
	if(!stream.good()) {
			throw "Error: Communication to zynaddsubfx failed.";
		}
	stream >> port;
	return port;
}

zynaddsubfx_t::zynaddsubfx_t(const char *name) :
	zyn::znode_t(this, "/", ""),
	instrument_t(name),
	notes_t_port(this, "/", "/noteOn")
{
}

zyn_impl::zyn_impl(const char *name)
	: zynaddsubfx_t(name)
{
}

#if 0
zyn_impl::zyn_impl(zynaddsubfx_t *ref) :
	is_impl_of_t<zynaddsubfx_t>::is_impl_of_t(ref),
	pid(make_fork()),
	lo_port(ref->get_port(pid, 0 /*TODO*/))
{
}
#endif

zyn_impl::~zyn_impl()
{
}

command_base *zyn_impl::make_close_command() const
{
	return new command<>("/close-ui");
}

}

