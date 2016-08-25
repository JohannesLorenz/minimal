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

#include "io.h"
#include "instrument.h"
#include "command_tools.h"

namespace mini
{

void prioritized_command_cmd::proceed()
{
	proceed_base();

	io::mlog << "osc msg to: " << plugin->name() << ": " << io::endl
		 << "osc: " << cmd->complete_buffer() << io::endl;

	//no_rt::mlog << "osc msg to: " << plugin->name() << ": " << std::endl
	//	<< "osc: " << cmd->complete_buffer() << std::endl;

	if(!plugin) throw "plugin";
	if(!cmd) throw "not cmd";
	plugin->send_osc_cmd(cmd->complete_buffer().raw());

	// TODO: not sure, but max sounds correct:
	update_next_time(std::numeric_limits<sample_no_t>::max());
	w->update(get_handle());
}


}
