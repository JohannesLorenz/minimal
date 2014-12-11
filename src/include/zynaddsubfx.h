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

#ifndef ZYNADDSUBFX_H
#define ZYNADDSUBFX_H

#include "instrument.h"

namespace mini
{

namespace zyn {

class node_t : public named_t
{
// the inheriting class must define the sub-nodes
protected:
	template<class NodeT>
	NodeT spawn(const std::string& ext) const {
		return NodeT(name(), ext);
	}

	template<class NodeT>
	NodeT spawn(const std::string& ext, std::size_t id) const {
		return spawn<NodeT>(ext + std::to_string(id));
	}
public:
	node_t(const std::string& base, const std::string& ext)
		: named_t(base + ext) {}

	//node(std::string base, std::string ext, std::size_t id)
	//	: node(base, ext + std::to_string(id)) {}
};

class node_port_t : node_t
{
	// todo: disallow spawn here
	template<class NodeT>
	NodeT spawn(const std::string& ext, std::size_t id) const {
		return spawn<NodeT>(ext + std::to_string(id));
	}

	using node_t::node_t;
};


/*class p_envsustain : node_port_t
{
public:
	using node_port_t::node_port_t;
};*/



template<class Port1 = no_port<int>>
class p_envsustain : public command<oint<Port1>>
{
	using base = command<oint<Port1>>;
public:
	static const char* path() { return "PEnvsustain"; } // TODO: noteOn string is code duplicate
	p_envsustain(oint<Port1> value) // TODO: "ref?"
		: base("/PEnvsustain", value)
	{
	}
};





class amp_env : node_t
{
public:
	using node_t::node_t;
	template<class Port1>
	zyn::p_envsustain<Port1> envsustain(Port1& con) const {
		return envsustain(oint<Port1>(con));
	}
	template<class Port1>
	zyn::p_envsustain<Port1> envsustain(oint<Port1> con) const {
		return p_envsustain<Port1>(con);
	}
};

class global : node_t
{
public:
	using node_t::node_t;
	zyn::amp_env amp_env() const {
		return spawn<zyn::amp_env>("AmpEnvelope/");
	}
};


class voice0 : node_t
{
public:
	using node_t::node_t;
};

class adpars : node_t
{
public:
	using node_t::node_t;
	zyn::voice0 voice0() const {
		return spawn<zyn::voice0>("voice0/");
	}
	zyn::global global() const {
		return spawn<zyn::global>("global/");
	}
};

}

class zynaddsubfx_t : public instrument_t, zyn::node_t
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
	cmd_vectors make_note_commands(const std::multimap<daw::note_geom_t, daw::note_t>& mm) const;

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

	zyn::adpars add0() const {
		return spawn<zyn::adpars>("part0/kit0/adpars/");
	}
};

}

#endif // ZYNADDSUBFX_H
