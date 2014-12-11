/*************************************************************************/
/* minimal - a minimal osc sequencer                                     */
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

#include <unistd.h> // TODO
#include <fstream>

#include "config.h"
#include "zynaddsubfx.h"

namespace mini {

std::string zynaddsubfx_t::make_start_command() const
{
	const std::string cmd = ZYN_BINARY
		" --no-gui -O alsa"; // TODO: read from options file
	return cmd;
}

cmd_vectors zynaddsubfx_t::make_note_commands(const std::multimap<daw::note_geom_t, daw::note_t> &mm) const
{
	// channel, note, velocity

	// note offset <-> command
	std::map<int, command_base*> cmd_of;

	cmd_vectors res;
	for(const std::pair<daw::note_geom_t, daw::note_t>& pr : mm)
	{
		//	res.emplace_back(new command<int_f, int_f, int_v>("/noteOn", 0, pr.first.offs, pr.second.velocity()), pr.first.start); // TODO: valgrind!

		auto itr1 = cmd_of.find(pr.first.offs);
		if(itr1 == cmd_of.end())
		{
			// TODO: valgrind
			command_base* cmd = new command<oint<>, oint<>, oint<>>("/noteOn", 0, pr.first.offs, pr.second.velocity());
			cmd_of.emplace_hint(itr1, pr.first.offs, cmd);

			// TODO: note_off

			res.emplace(cmd, std::set<float>{pr.first.start});
			std::cerr << "New note command: " << cmd << std::endl;

			cmd = new command<oint<>, oint<>>("/noteOff", 0, pr.first.offs);
			res.emplace(cmd, std::set<float>{pr.first.start + pr.second.length()});

			std::cerr << "Map content now: " << std::endl;
			for(const auto& p : res)
			{
				std::cerr << p.first->buffer() << std::endl;
			}
		}
		else
		{
			res.find(itr1->second)->second.insert(pr.first.start);
			std::cerr << "Found note command." << std::endl;
		}



	}
	std::cerr << "Added " << res.size() << " note commands to track." << std::endl;
	return res;
}

instrument_t::port_t zynaddsubfx_t::get_port(pid_t pid, int) const
{
	port_t port;
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
	instrument_t(name, { new command<>("/quit") }), // TODO! close-ui?
	node_t("/", "")
{}

}

