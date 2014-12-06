/*************************************************************************/
/* minimal - a minimal rtosc sequencer                                   */
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

#ifndef ZYNADDSUBFX_H
#define ZYNADDSUBFX_H

#include "instrument.h"

namespace mini
{

class node_t : public named_t
{
public:
	node_t(const std::string& base, const std::string& ext)
		: named_t(base + ext) {}

	//node(std::string base, std::string ext, std::size_t id)
	//	: node(base, ext + std::to_string(id)) {}

	node_t spawn(const std::string& ext) const {
		return node_t(name(), ext);
	}

	node_t spawn(const std::string& ext, std::size_t id) const {
		return spawn(ext + std::to_string(id));
	}
};

class zynaddsubfx_t : public instrument_t, node_t
{
	// TODO: read from options file
/*	const char* binary
		= "/tmp/cprogs/fl_abs/gcc/src/zynaddsubfx";
	const char* default_args = "--no-gui -O alsa";*/
public:

	/*template<template<class> C1, template<class> C2>
	class note_on : public command<int_f, int_f, int_f> { //using command::command;
	public:
		note_on(con<p_char> x, con<p_char> y, con<p_char> z) : command("/noteOn", x, y, z) {} // TODO: a bit much work?
	};*/




	std::string make_start_command() const;
	cmd_vectors make_note_commands(const std::multimap<daw::note_geom_t, daw::note_t>& mm) const
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

				res.emplace(cmd, new activator_events(std::set<float>{pr.first.start}));
				std::cerr << "New note command: " << cmd << std::endl;

				cmd = new command<oint<>, oint<>>("/noteOff", 0, pr.first.offs);
				res.emplace(cmd, new activator_events(std::set<float>{pr.first.start + pr.second.length()}));

				std::cerr << "Map content now: " << std::endl;
				for(const auto& p : res)
				{
					std::cerr << p.first->buffer() << std::endl;
				}
			}
			else
			{
				dynamic_cast<activator_events*>(res.find(itr1->second)->second)->insert(pr.first.start);
				std::cerr << "Found note command." << std::endl;
			}



		}
		std::cerr << "Added " << res.size() << " note commands to track." << std::endl;
		return res;
	}

	port_t get_port(pid_t pid, int ) const;
	zynaddsubfx_t(const char* name);
	virtual ~zynaddsubfx_t() {} //!< in case someone derives this class

	// TODO: string as template param?
/*	class note_on : public command<p_char, p_char, p_char>
	{
		static const char* path() { return "/noteOn"; }
		template<class ...Args>
		note_on(const char* _path, Args ...args) : command(_path, ...args) {}
	};*/

	//class note_off : public command<p_char, p_char> { static const char* path() { return "/noteOff"; } };
	template<class Port1 = no_port<int>, class Port2 = no_port<int>, class Port3 = no_port<int>>
	class note_on : public command<oint<Port1>, oint<Port2>, oint<Port3>>
	{
		using base = command<oint<Port1>, oint<Port2>, oint<Port3>>;
	public:
		static const char* path() { return "/noteOn"; } // TODO: noteOn string is code duplicate
		note_on(oint<Port1> chan, oint<Port2> note, oint<Port3> velocity)
			: base("/noteOn", chan, note, velocity)
		{
		}
	};

	node_t add0() const {
		return spawn("part0/kit0/adpars/");
	}
};

}

#endif // ZYNADDSUBFX_H
