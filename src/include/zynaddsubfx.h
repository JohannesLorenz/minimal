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

#include <vector>
#include "lo_port.h"
#include "instrument.h"
#include "ports.h"

namespace mini
{

class lo_port_t;


class zynaddsubfx_t;

namespace zyn {

template<class InstClass>
class node_t : public named_t
{
public: // TODO
	InstClass* ins;
// the inheriting class must define the sub-nodes
protected:

	template<class NodeT>
	NodeT* spawn_new(const std::string& ext) const {
		return new NodeT(ins, name(), ext);
	}

	template<class NodeT>
	NodeT spawn(const std::string& ext) const {
		return NodeT(ins, name(), ext);
	}

	template<class NodeT>
	NodeT spawn(const std::string& ext, std::size_t id) const {
		return spawn<NodeT>(ext + std::to_string(id));
	}
public:
	node_t(InstClass* ins, const std::string& base, const std::string& ext)
		: named_t(base + ext), ins(ins) {}



	//node(std::string base, std::string ext, std::size_t id)
	//	: node(base, ext + std::to_string(id)) {}
};

template<class T>
class node_port_t : node_t<T>
{
	// todo: disallow spawn here
	template<class NodeT>
	NodeT spawn(const std::string& ext, std::size_t id) const {
		return spawn<NodeT>(ext + std::to_string(id));
	}

	using node_t<T>::node_t;
};


/*class p_envsustain : node_port_t
{
public:
	using node_port_t::node_port_t;
};*/



/*template<class Port1 = no_port<int>>
class p_envsustain : public command<oint<Port1>>
{
	using base = command<oint<Port1>>;
public:
	static const char* path() { return "Penvsustain"; } // TODO: noteOn string is code duplicate
	p_envsustain(oint<Port1> value) // TODO: "ref?"
		: base("/Penvsustain", value)
	{
	}
};*/

struct osc_string_sender
{
	virtual void send_all(lo_port_t* lo_port) = 0;
};

void send_single_command(lo_port_t& lp, const osc_string& str);

template<class PortType, class InstClass>
struct in_port_with_command : node_t<InstClass>//, PortType//, osc_string_sender
{ // TODO: instrument.h
	// a bit non-conform to store the command here, but working...
	//using base = command<oint<Port1>>;

	command_base* cmd;
public:
	in_port_with_command(InstClass* ins, const std::string& base, const std::string& ext, command_base* cmd) :
		node_t<InstClass>(ins, base, ext),
		//PortType(*ins),
		cmd(cmd)
	{
//		PortType::set_trigger(); // TODO: here?
//		static_cast<instrument_t*>(ins)->add_in_port(this);
		port().set_trigger();
		static_cast<instrument_t*>(ins)->add_in_port(&port()); // TODO: is this needed?
	}

/*out_port_with_command(oint<Port1> value) // TODO: "ref?"
		: base("/Penvsustain", value)
	{
	}*/

	PortType& port() {
		return (dynamic_cast<command<PortType>*>(cmd))->template port_at<0>();
	}

	void send_all(lo_port_t* lo_port)
	{
	//	cmd->update(); // TODO: check ret val?
		send_single_command(*lo_port, cmd->complete_buffer());
	}
};



using znode_t = node_t<zynaddsubfx_t>;

template<class PortT>
struct p_envsustain : public in_port_with_command<in_port_templ<int>, zynaddsubfx_t>
{
	using base = in_port_with_command<in_port_templ<int>, zynaddsubfx_t>;
	p_envsustain(zynaddsubfx_t* ins, const std::string& base, const std::string& ext) :
		in_port_with_command<in_port_templ<int>, zynaddsubfx_t>(ins, base, ext, new command<in_port_templ<int>>((base + ext).c_str(), (effect_t&)*ins)) {
	}
};

#if 0
struct p_note_input : public in_port_with_command<in_port_templ<note_signal_t>, zynaddsubfx_t>
{
	using base = in_port_with_command<in_port_templ<int>, zynaddsubfx_t>;
	p_note_input(zynaddsubfx_t* ins, const std::string& base, const std::string& ext) :
		in_port_with_command<in_port_templ<note_signal_t>, zynaddsubfx_t>(ins, base, ext, new command<vint<PortT>>(ext.c_str(), *this)) {
	}
/*	p_envsustain(zynaddsubfx_t* ins, const std::string& base, const std::string& ext) :
		in_port_with_command<in_port_templ<int>, zynaddsubfx_t>(ins, base, ext, new command<note_signal_t<PortT>>(ext.c_str(), *this)) {
	}*/

};
#endif


#if 0
ss znode_t : node_t
{
	/*zynaddsubfx_t& zyn;
	znode_t(const zynaddsubfx_t& zyn, const std::string& base, const std::string& ext)
		: node_t(base, ext), zyn(zyn) {}*/
}
#endif

class amp_env : znode_t
{
public:
	using znode_t::znode_t;
	/*template<class Port1>*/
/*	zyn::p_envsustain envsustain() const {
		ins->add_in_port(new );
		//return p_envsustain();
	}*/
	template<class Port>
	zyn::p_envsustain<Port>* envsustain() const {
		return spawn_new<zyn::p_envsustain<Port>>("Penvsustain");
	}

	/*template<class Port1>
	zyn::p_envsustain<Port1> envsustain(oint<Port1> con) const {
		return p_envsustain<Port1>(con);
	}*/
};

class global : znode_t
{
public:
	using znode_t::znode_t;
	zyn::amp_env amp_env() const {
		return spawn<zyn::amp_env>("AmpEnvelope/");
	}
};


class voice0 : znode_t
{
public:
	using znode_t::znode_t;
};

class adpars : znode_t
{
public:
	using znode_t::znode_t;
	//! shortcut, since voice0 is popular
	zyn::voice0 voice0() const {
		return spawn<zyn::voice0>("voice0/");
	}
	zyn::global global() const {
		return spawn<zyn::global>("global/");
	}
};

}

struct zyn_impl : is_impl_of_t<zynaddsubfx_t>//, protected work_queue_t
{
	using is_impl_of_t<zynaddsubfx_t>::is_impl_of_t;
	const pid_t pid;
	lo_port_t lo_port;

/*	class port_work : task_base
	{
		in_port_base* ip;
	public:
		port_work(in_port_base* ip) : task_base(0.0f), ip(ip) {}
		void proceed(float time) {
			(void)time;
			(void)ip;
// TODO
		}
	};*/

	pid_t make_fork();

	zyn_impl(zynaddsubfx_t* ref);
	~zyn_impl();

	float proceed(float);
};

template<class = void, bool = false>
class use_no_port {};

template<template<class, bool> class P, class T>
struct _port_type_of { using type = P<T, true>; };

template<class T>
struct _port_type_of<use_no_port, T> { using type = T; };

template<template<class , bool> class P, class T>
using port_type_of = typename _port_type_of<P, T>::type;

template<template<class , bool> class P, class T>
using port_arg = data_type_if_port<port_type_of<P, T>>;

class zynaddsubfx_t : public zyn::znode_t, public instrument_t, has_impl_t<zyn_impl, zynaddsubfx_t>
{
	// TODO: read from options file
/*	const char* binary
		= "/tmp/cprogs/fl_abs/gcc/src/zynaddsubfx";
	const char* default_args = "--no-gui -O alsa";*/

	using m_impl = has_impl_t<zyn_impl, zynaddsubfx_t>;
public:
	//class note_off : public command<p_char, p_char> { static const char* path() { return "/noteOff"; } };
	template<template<class , bool> class Port1 = use_no_port,
		template<class , bool> class Port2 = use_no_port,
		template<class , bool> class Port3 = use_no_port>
	class note_on : public command<port_type_of<Port1, int>, port_type_of<Port2, int>, port_type_of<Port3, int>>
	{
		using base = command<port_type_of<Port1, int>, port_type_of<Port2, int>, port_type_of<Port3, int>>;
	public:
		static const char* path() { return "/noteOn"; } // TODO: noteOn string is code duplicate
		note_on(port_type_of<Port1, int> chan, port_type_of<Port2, int> note, port_type_of<Port3, int>&& velocity) // TODO: rvals
			: base("/noteOn", chan, note, std::move(velocity))
		{
		}
	};

	//class note_off : public command<p_char, p_char> { static const char* path() { return "/noteOff"; } };
	template<template<class , bool> class Port1 = use_no_port,
		template<class , bool> class Port2 = use_no_port,
		template<class , bool> class Port3 = use_no_port>
	class note_off : public command<port_type_of<Port1, int>, port_type_of<Port2, int>, port_type_of<Port3, int>>
	{
		using base = command<port_type_of<Port1, int>, port_type_of<Port2, int>, port_type_of<Port3, int>>;
	public:
		static const char* path() { return "/noteOff"; } // TODO: noteOn string is code duplicate
		note_off(port_type_of<Port1, int> chan, port_type_of<Port2, int> note, port_type_of<Port3, int>&& id)
			: base("/noteOff", chan, note, std::move(id))
		{
		}
	};
private:
	template<class InstClass>
	struct notes_t_port_t : node_t<InstClass>, notes_in
	{
		command_base* cmd;
		using m_note_on_t = note_on<use_no_port, use_no_port, self_port_templ>;
		using m_note_off_t = note_off<use_no_port, use_no_port, self_port_templ>;

		std::vector<m_note_on_t> note_ons;
		std::vector<m_note_off_t> note_offs;
	public:
		notes_t_port_t(InstClass* ins, const std::string& base, const std::string& ext) : // todo: base, ext does not make sense here?
			node_t<InstClass>(ins, base, ext),
			notes_in(*ins)
		{
			//static_cast<instrument_t*>(ins)->add_in_port(this);

			note_ons.reserve(NOTES_MAX);
			note_offs.reserve(NOTES_MAX);
			std::size_t idx = 0;
			for(; idx < NOTES_MAX; ++idx)
			{
				note_ons.emplace_back(0 /*chan*/, idx/*offs*/, self_port_templ<int, true>{});
			}
			idx = 0;
			for(; idx < NOTES_MAX; ++idx)
			{
				note_offs.emplace_back(0 /*chan*/, idx/*offs*/, self_port_templ<int, true>{});
			}

			set_trigger(); // TODO: here?
		}

		void send_all(lo_port_t* lo_port)
		{
			std::cerr << "SENDALL: " << std::endl;
			std::cerr << "NL2:" << &notes_in::data->changed_stamp << std::endl;
			std::cerr << "STAMP: " << notes_in::data->changed_stamp << std::endl;
			for(const std::pair<int, int>& p : notes_in::data->recently_changed)
			if(p.first < 0)
			 break;
			else
			{
				std::cerr << "SENDALL 2: " << std::endl;
				std::pair<int, int> p2 = notes_in::data->lines[p.first][p.second];
				if(p2.first < 0)
				{
					zyn::send_single_command(*lo_port, note_offs[p.first].buffer());
					//lo_port->send_raw(note_offs[p.first].buffer().raw(), note_offs[p.first].buffer().size());
				}
				else
				{
					m_note_on_t& note_on_cmd = note_ons[p.first];
					// self_port_t must be completed manually:
					std::cerr << "SETTING VOL: " << p2.second << std::endl;
					note_on_cmd.port_at<2>().set(p2.second);
					std::cerr << "GETTING VOL: " << note_on_cmd.port_at<2>().get() << std::endl;
					note_on_cmd.command::update();
					zyn::send_single_command(*lo_port, note_on_cmd.complete_buffer());
				/*	command_base* cmd = note_ons[p.first];
					cmd->complete_buffer();
					lo_port->send_raw(cmd->buffer().raw(), cmd->buffer().size());*/
				}
			}
		}
	};




	notes_t_port_t<zynaddsubfx_t> notes_t_port; // TODO: inherit??


public:

	/*template<template<class> C1, template<class> C2>
	class note_on : public command<int_f, int_f, int_f> { //using command::command;
	public:
		note_on(con<p_char> x, con<p_char> y, con<p_char> z) : command("/noteOn", x, y, z) {} // TODO: a bit much work?
	};*/

	float _proceed(float time) {
		std::cerr << "proceeding with zyn" << std::endl;
		return impl->proceed(time); }
	void instantiate() { m_impl::instantiate(); }


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



	zyn::adpars add0() const {
		return spawn<zyn::adpars>("part0/kit0/adpars/");
	}

//	in_port_templ<note_signal_t> note_input;
	notes_t_port_t<zynaddsubfx_t>& note_input() {
		return notes_t_port;
	}
};

}

#endif // ZYNADDSUBFX_H
