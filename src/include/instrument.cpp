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

#include <fstream>
#include "ports.h"
#include "instrument.h"

namespace mmms {

std::size_t instrument_t::next_id;

/*void instrument_t::set_param_fixed(const char *param, ...)
{
	ports::send_rtosc_msg(param, "?", "...");
}*/

std::string zynaddsubfx_t::make_start_command() const
{
	std::string cmd = binary;
	cmd += " ";
	cmd += default_args;
	return cmd;
}

instrument_t::port_t zynaddsubfx_t::get_port(pid_t pid, int) const
{
	port_t port;
	std::string tmp_filename = "/tmp/zynaddsubfx_"
		+ std::to_string(pid);
	std::ifstream stream(tmp_filename);
	if(!stream.good()) {
		throw "Error: Communication to zynaddsubfx failed.";
	}
	stream >> port;
	return port;
}

}

