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

#ifndef ZYNADDSUBFX_H
#define ZYNADDSUBFX_H

#include <vector>
#include "lo_port.h"
#include "instrument.h"
#include "ports.h"
#include "impl.h"

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

struct prioritized_command_base //: public work_queue_t::task_base
{
	std::size_t priority;
	float next_time;
	prioritized_command_base(std::size_t priority, float next_time) :
		priority(priority),
		next_time(next_time)
	{

	}
};

template<class PortType>
struct prioritized_command : public prioritized_command_base
{
	command<PortType>* cmd; // TODO: does command_base suffice?
	prioritized_command(std::size_t priority, float next_time,
		command<PortType>* cmd) :
		prioritized_command_base(priority, next_time), cmd(cmd)
	{

	}
};

template<class Ins, class PortType>
struct rtosc_in_port : PortType
{
	prioritized_command<rtosc_in_port<Ins, PortType>>* cmd;
	Ins* ins;

	void on_recv() {
		send_single_command(ins->get_impl()->lo_port, cmd->cmd->complete_buffer());
	}

	using PortType::PortType;
};

// TODO: make this a subclass of rtosc_instr and then remove get_impl() ?
template<class InstClass, class PortType>
struct in_port_with_command : node_t<InstClass>
{ // TODO: instrument.h -> ?
	using rtosc_in_port = rtosc_in_port<InstClass, PortType>;

	prioritized_command<rtosc_in_port> cmd;
public:
	in_port_with_command(InstClass* ins, const std::string& base, const std::string& ext) :
		node_t<InstClass>(ins, base, ext),
		cmd(1, 0.0f, new command<rtosc_in_port>((base + ext).c_str(), (effect_t&)*ins))
	{
		cmd.cmd->template port_at<0>().set_trigger();
		cmd.cmd->template port_at<0>().ins = ins;
		cmd.cmd->template port_at<0>().cmd = &cmd;
	}

	rtosc_in_port& port() {
		return cmd.cmd->template port_at<0>();
	}
};



using znode_t = node_t<zynaddsubfx_t>;

template<class PortT>
using p_envsustain = in_port_with_command<zynaddsubfx_t, PortT>;

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

struct zyn_impl : is_impl_of_t<zynaddsubfx_t>, protected work_queue_t
{
	using is_impl_of_t<zynaddsubfx_t>::is_impl_of_t;
	const pid_t pid;
	lo_port_t lo_port;

	pid_t make_fork();

	zyn_impl(zynaddsubfx_t* ref);
	~zyn_impl();

	virtual command_base* make_close_command() const;

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

class zynaddsubfx_t : public zyn::znode_t, public effect_t, public instrument_t, public has_impl_t<zyn_impl, zynaddsubfx_t>
{
	using m_impl = has_impl_t<zyn_impl, zynaddsubfx_t>;
public:
	template<template<class , bool> class Port1 = use_no_port,
		template<class , bool> class Port2 = use_no_port,
		template<class , bool> class Port3 = use_no_port>
	class note_on : public command<port_type_of<Port1, int>, port_type_of<Port2, int>, port_type_of<Port3, int>>
	{
		using base = command<port_type_of<Port1, int>, port_type_of<Port2, int>, port_type_of<Port3, int>>;
	public:
		note_on(port_type_of<Port1, int> chan, port_type_of<Port2, int> note, port_type_of<Port3, int>&& velocity) // TODO: rvals
			: base("/noteOn", chan, note, std::move(velocity))
		{
		}
	};

	template<template<class , bool> class Port1 = use_no_port,
		template<class , bool> class Port2 = use_no_port,
		template<class , bool> class Port3 = use_no_port>
	class note_off : public command<port_type_of<Port1, int>, port_type_of<Port2, int>, port_type_of<Port3, int>>
	{
		using base = command<port_type_of<Port1, int>, port_type_of<Port2, int>, port_type_of<Port3, int>>;
	public:
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
		InstClass* ins;
		using m_note_on_t = note_on<use_no_port, use_no_port, self_port_templ>;
		using m_note_off_t = note_off<use_no_port, use_no_port, self_port_templ>;

		std::vector<m_note_on_t> note_ons;
		std::vector<m_note_off_t> note_offs;
	public:
		notes_t_port_t(InstClass* ins, const std::string& base, const std::string& ext) : // todo: base, ext does not make sense here?
			node_t<InstClass>(ins, base, ext),
			notes_in(*ins),
			ins(ins)
		{
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

		void on_recv() {
			send_all(&ins->impl->lo_port);
		}

		void send_all(lo_port_t* lo_port)
		{
			for(const std::pair<int, int>& p : notes_in::data->recently_changed)
			if(p.first < 0)
			 break;
			else
			{
				std::pair<int, int> p2 = notes_in::data->lines[p.first][p.second];
				if(p2.first < 0)
				{
					send_single_command(*lo_port, note_offs[p.first].buffer());
				}
				else
				{
					m_note_on_t& note_on_cmd = note_ons[p.first];
					// self_port_t must be completed manually:
					note_on_cmd.port_at<2>().set(p2.second);
					note_on_cmd.command::update();
					send_single_command(*lo_port, note_on_cmd.complete_buffer());
				}
			}
		}
	};

	notes_t_port_t<zynaddsubfx_t> notes_t_port; // TODO: inherit??


public:

	float _proceed(float time) {
		std::cerr << "proceeding with zyn" << std::endl;
		return impl->proceed(time); }
	void instantiate() { m_impl::instantiate(); }

	std::string make_start_command() const;

	udp_port_t get_port(pid_t pid, int ) const;
	zynaddsubfx_t(const char* name);
	virtual ~zynaddsubfx_t() {} //!< in case someone derives this class

	/*
	 *  ports
	 */

	zyn::adpars add0() const {
		return spawn<zyn::adpars>("part0/kit0/adpars/");
	}

	notes_t_port_t<zynaddsubfx_t>& note_input() {
		return notes_t_port;
	}

	class part : zyn::znode_t
	{
	public:
		using zyn::znode_t::znode_t;
		//zyn::amp_env amp_env() const {
		//	return spawn<zyn::amp_env>("AmpEnvelope/");
		//}
		template<class Port>
		zyn::p_envsustain<Port>* Ppanning() const { // TODO: panning must be int...
			return spawn_new<zyn::in_port_with_command<zynaddsubfx_t, Port>>("Ppanning");
		}
	};

	part part0() const { return spawn<part>("part0/"); } // TODO: large tuple for these

	template<class Port>
	zyn::p_envsustain<Port>* volume() const {
		return spawn_new<zyn::in_port_with_command<zynaddsubfx_t, Port>>("volume");
	}
};

}

#endif // ZYNADDSUBFX_H
