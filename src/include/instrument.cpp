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

#include <iostream> // TODO
#include "lo_port.h"
#include "instrument.h"

namespace mini {

std::size_t instrument_t::next_id;

/*void instrument_t::set_param_fixed(const char *param, ...)
{
	ports::send_rtosc_msg(param, "?", "...");
}*/

instrument_t::~instrument_t()
{
	std::cout << "destroying instrument: " << name() << std::endl;
	for(command_base*& cb : commands)
	{
		std::cout << name() << ": deleting " << cb->path() << std::endl;
		delete cb;
	}
}

#if 0
activator_events_itr::activator_events_itr(const activator_events &ab) :
	itr(ab.events.begin())
{
}
#endif

/*instrument_t *instrument_t::clone() const
{
	instrument_t* result = new instrument_t();
	result->next_id = next_id;
	for(const command_base* cmd : commands)
	 result->commands.push_back(cmd->clone());
	return result;
}*/

}

