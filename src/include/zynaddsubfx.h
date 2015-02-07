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
#include "instrument.h"
#include "ports.h"
#include "impl.h"

namespace mini
{

class lo_port_t;


class zyn_tree_t;

namespace zyn {

using znode_t = node_t<zyn_tree_t>;

template<class PortT>
using p_envsustain = in_port_with_command<zyn_tree_t, PortT>;

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

template<class = void, bool = false>
class use_no_port {};

template<template<class, bool> class P, class T>
struct _port_type_of { using type = P<T, true>; };

template<class T>
struct _port_type_of<use_no_port, T> { using type = T; };

template<template<class , bool> class P, class T>
using port_type_of = typename _port_type_of<P, T>::type;

class zyn_tree_t : public zyn::znode_t, public instrument_t
{
public:
	template<template<class , bool> class Port1 = use_no_port,
		template<class , bool> class Port2 = use_no_port,
		template<class , bool> class Port3 = use_no_port>
	class note_on : public in_port_with_command<zyn_tree_t, port_type_of<Port1, int>, port_type_of<Port2, int>, port_type_of<Port3, int>>
	{
		using base = in_port_with_command<zyn_tree_t, port_type_of<Port1, int>, port_type_of<Port2, int>, port_type_of<Port3, int>>;
	public:
		note_on(zyn_tree_t* zyn, port_type_of<Port1, int> chan, port_type_of<Port2, int> note, port_type_of<Port3, int>&& velocity) // TODO: rvals
			: base(zyn, "/", "noteOn", chan, note, std::forward<port_type_of<Port3, int>>(velocity)) // TODO: forward instead of move?
		{
		}
	};

	template<template<class , bool> class Port1 = use_no_port,
		template<class , bool> class Port2 = use_no_port,
		template<class , bool> class Port3 = use_no_port>
	class note_off : public in_port_with_command<zyn_tree_t, port_type_of<Port1, int>, port_type_of<Port2, int>, port_type_of<Port3, int>>
	{
		using base = in_port_with_command<zyn_tree_t, port_type_of<Port1, int>, port_type_of<Port2, int>, port_type_of<Port3, int>>;
	public:
		note_off(zyn_tree_t* zyn, port_type_of<Port1, int> chan, port_type_of<Port2, int> note, port_type_of<Port3, int>&& id)
			: base(zyn, "/", "noteOff", chan, note, std::forward<port_type_of<Port3, int>>(id))
		{
		}
	};
private:
	using c_note_on = note_on<use_no_port, use_no_port, self_port_templ>;
	using c_note_off = note_off<use_no_port, use_no_port, self_port_templ>;

	template<class InstClass>
	struct notes_t_port_t : node_t<InstClass>, rtosc_in_port_t<notes_in>
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
			rtosc_in_port_t<notes_in>(*ins),
			ins(ins)
		{
			note_ons.reserve(NOTES_MAX);
			note_offs.reserve(NOTES_MAX);
			// TODO: one for loop suffices
			std::size_t idx = 0;
			for(; idx < NOTES_MAX; ++idx)
			{
				// TODO: leave the rtosc...
				//note_ons.emplace_back(0 /*chan*/, idx/*offs*/, self_port_templ<int, true>{});
				//using c_note_on = note_on<use_no_port, use_no_port, self_port_templ>;

				note_ons.emplace_back(ins, 0 /*chan*/, idx/*offs*/, self_port_templ<int, true>{});

			//	c_note_on* cmd_ptr = new c_note_on(0, idx, self_port_templ<int, true>{});
			//	note_ons.emplace_back(1, 0.0f, &ins->lo_port, cmd_ptr);
			}
			idx = 0;
			for(; idx < NOTES_MAX; ++idx)
			{
				// TODO: leave the rtosc...

				note_offs.emplace_back(ins, 0 /*chan*/, idx/*offs*/, self_port_templ<int, true>{});

				//note_offs.emplace_back(0 /*chan*/, idx/*offs*/, self_port_templ<int, true>{});

			//	c_note_off* cmd_ptr = new c_note_off(0, idx, self_port_templ<int, true>{});
			//	note_offs.emplace_back(1, 0.0f, &ins->lo_port, cmd_ptr);
			}

			set_trigger(); // TODO: here?
		}

		/*void proceed(float time) {
			proceed_base(time);
			send_all();
		}*/

		void on_recv(float pos) {
			//update_next_time(time);
			//ins->update(cmd->handle);
			send_all(pos);
		}

		void send_all(float pos)
		{
			std::cerr << "SENDING ALL..." << std::endl;
			for(const std::pair<int, int>& p : notes_in::data->recently_changed)
			if(p.first < 0)
			 break;
			else
			{
				// for self_port_t, on_recv is not virtual, so we call it manually...
				std::pair<int, int> p2 = notes_in::data->lines[p.first][p.second];				
				if(p2.first < 0)
				{
				#if 0
					send_single_command(*lo_port, note_offs[p.first].buffer());
				#endif
				// TODO!!
					// note_offs[p.first].on_recv();



					if(note_offs[p.first].cmd.set_changed())
					{
						note_offs[p.first].cmd.update_next_time(pos); // TODO: call on recv
						ins->update(note_offs[p.first].cmd.handle);
					}
					std::cerr << "SEND OFF" << std::endl;
				}
				else
				{
					m_note_on_t& note_on_cmd = note_ons[p.first];
					// self_port_t must be completed manually:
					note_on_cmd.cmd_ptr->port_at<2>().set(p2.second);
					note_on_cmd.cmd_ptr->command::update();
				#if 0
					send_single_command(*lo_port, note_on_cmd.complete_buffer());
				#endif
					note_on_cmd.cmd_ptr->complete_buffer(); // TODO: call in on_recv??
					if(note_on_cmd.cmd.set_changed())
					{
						note_on_cmd.cmd.update_next_time(pos); // TODO: call on recv
						ins->update(note_on_cmd.cmd.handle);
					}

					std::cerr << "SEND ON" << std::endl;
					// TODO!!
					// note_on_cmd.on_recv();
				}
			}
		}
	};

	notes_t_port_t<zyn_tree_t> notes_t_port; // TODO: inherit??


public:

	zyn_tree_t(const char* name);
	virtual ~zyn_tree_t() {}

	/*
	 *  ports
	 */

	zyn::adpars add0() const {
		return spawn<zyn::adpars>("part0/kit0/adpars/");
	}

	notes_t_port_t<zyn_tree_t>& note_input() {
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
			return spawn_new<in_port_with_command<zyn_tree_t, Port>>("Ppanning");
		}
	};

	part part0() const { return spawn<part>("part0/"); } // TODO: large tuple for these

	template<class Port>
	zyn::p_envsustain<Port>* volume() const {
		return spawn_new<in_port_with_command<zyn_tree_t, Port>>("volume");
	}
};

class zynaddsubfx_t : public zyn_tree_t//, protected work_queue_t
{
	std::string make_start_command() const;
	instrument_t::udp_port_t get_port(pid_t pid, int) const;
	command_base* make_close_command() const;

public:
	using zyn_tree_t::zyn_tree_t;
	~zynaddsubfx_t() = default;
};

}

#endif // ZYNADDSUBFX_H
