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

#include <fstream>

#include "config.h"
#include "zynaddsubfx.h"

namespace mini {

zyn_tree_t::zyn_tree_t(const char *name) :
	zyn::znode_t(this, "", ""),
	audio_instrument_t(name),
	notes_t_port(this, "", "noteOn")
{
}

std::string zynaddsubfx_t::make_start_command() const
{
	const std::string cmd = ZYN_BINARY
		" --no-gui -p -O alsa"; // TODO: read from options file
	return cmd;
}

const char* zynaddsubfx_t::library_path() const
{
	return ZYN_BINARY;
}

command_base *zynaddsubfx_t::make_close_command() const
{
	return new command<>("/close-ui");
}

}

