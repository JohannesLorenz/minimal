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

#include "effect.h"
#include "io.h"
#include "mports.h"

namespace mini {

void effect_t::set_next_time(sample_no_t next)
{
	if(finished_threads >= max_threads - 1) { // TODO: check atomacity...
		io::mlog << "OK: only finished threads " << finished_threads
			<< " of " << max_threads << io::endl;
		next_time = next;
	}
	else
	 io::mlog << "WARNING: only finished threads " << finished_threads
		<< " of " << max_threads << io::endl;
}

void effect_t::init_next_time(sample_no_t next)
{
	io::mlog << "NOTE: init_next_time" << io::endl;
	if(!finished_threads) // TODO: check atomacity...
		next_time = next;
}

void effect_t::add_in_port(mini::in_port_base *ipb) {
	ipb->id = in_ports.size();
	io::mlog << "Adding in port: " << ipb
		 << ", stamp: " << ipb->change_stamp
		 << ", id: " << ipb->id << io::endl;
	in_ports.push_back(ipb);
}

void effect_t::add_out_port(mini::out_port_base *opb) {
	out_ports.push_back(opb);
}

void effect_t::proceed_message() {
	io::mlog << "proceeding with effect " << id() << io::endl; }

}
