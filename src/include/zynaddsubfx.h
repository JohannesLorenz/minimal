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
using port = in_port_with_command<zyn_tree_t, PortT>;

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
	zyn::port<Port>* envsustain() const {
		return spawn_new<zyn::port<Port>>("Penvsustain");
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
		return spawn<zyn::amp_env>("AmpEnvelope");
	}

};


class voice0 : znode_t
{
public:
	using znode_t::znode_t;
};

class padpars : znode_t
{
public:
	using znode_t::znode_t;
/*	zyn::voice0 voice0() const {
		return spawn<zyn::voice0>("voice0");
	}
	zyn::global global() const {
		return spawn<zyn::global>("global");
	}*/ // TODO
	padpars() {}
};

class adpars : znode_t
{
public:
	using znode_t::znode_t;
	//! shortcut, since voice0 is popular
	zyn::voice0 voice0() const {
		return spawn<zyn::voice0>("voice0");
	}
	zyn::global global() const {
		return spawn<zyn::global>("global");
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

#ifdef NEW_PORT_TYPES
template<char ...Chars>
class cstr
{
};

template<class Port, char ...Chars>
class named_port : cstr<Chars...>
{
};

template<int I>
struct lookup_name;

template<int I, class Search, class Content>
struct lookup_name_dtl : util::dont_instantiate_me<Content>
{
};

template<int I, class Search, class Port, char ...Chars>
struct lookup_name_dtl<I, Search, std::pair<cstr<Chars...>, Port>>
{
	template<class Tpl>
	static const char* exec() {
		return lookup_name<I-1>::template exec<Tpl, Search>();
	}
};

template<int I, class Search, char ...Chars>
struct lookup_name_dtl<I, Search, std::pair<cstr<Chars...>, Search>>
{
	template<class >
	static std::array<char, sizeof...(Chars)> exec() {
		return { Chars... };
	}
};


template<int I>
struct lookup_name
{
	template<class Tpl, class Search>
	static std::array<char, 0> exec()
	{
		return lookup_name_dtl<I, Search, typename std::tuple_element<I, Tpl>::type>::template exec<Tpl>();
	}
};

template<>
struct lookup_name<-1>
{
	template<class , class Search>
	static const char* exec()
	{
		util::dont_instantiate_me_func<Search>();
	}
};

namespace z
{
	template<class Parent>
	struct next
	{

	};


	template<class Parent>
	struct insefx
	{
		using self = insefx<Parent>;
		using next = next<self>;

		using ports = std::tuple<std::pair<cstr<'n', 'e', 'x', 't'>, next>>;
	};

	template<class Self>
	struct path_to_string
	{
		static std::string exec() { return "/"; }
	};

	template<class Parent, template<class > class Self>
	struct path_to_string<Self<Parent>>
	{
		static std::string exec() {
			// TODO: replace Self<Parent> by simply Self?
			return lookup_name<std::tuple_size<typename Parent::ports>::value - 1>::template exec<typename Parent::ports, Self<Parent>>().data();
		}
	};

}
#endif



class zyn_tree_t : public zyn::znode_t, public instrument_t
{
	// todo: only send some params on new note?
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
			for(std::size_t idx = 0; idx < NOTES_MAX; ++idx)
			{
				note_ons.emplace_back(ins, 0 /*chan*/, idx/*offs*/, self_port_templ<int, true>{});
				note_offs.emplace_back(ins, 0 /*chan*/, idx/*offs*/, self_port_templ<int, true>{});
			}

			set_trigger(); // TODO: here?
		}

		void on_recv(float pos)
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
				// TODO!!
					// note_offs[p.first].on_recv();



					if(note_offs[p.first].cmd.set_changed())
					{
						note_offs[p.first].cmd.update_next_time(pos); // TODO: call on recv
						ins->update(note_offs[p.first].cmd.handle);
					}
				}
				else
				{
					m_note_on_t& note_on_cmd = note_ons[p.first];
					// self_port_t must be completed manually:
					note_on_cmd.cmd_ptr->port_at<2>().set(p2.second);
					note_on_cmd.cmd_ptr->command::update();

					note_on_cmd.cmd_ptr->complete_buffer(); // TODO: call in on_recv??
					if(note_on_cmd.cmd.set_changed())
					{
						note_on_cmd.cmd.update_next_time(pos); // TODO: call on recv
						ins->update(note_on_cmd.cmd.handle);
					}

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
		return spawn<zyn::adpars>("part0/kit0/adpars");
	}

	zyn::padpars pad0() const {
		return spawn<zyn::padpars>("part0/kit0/padpars");
	}

	notes_t_port_t<zyn_tree_t>& note_input() {
		return notes_t_port;
	}


	struct fx_t : zyn::znode_t
	{
		using zyn::znode_t::znode_t;
		template<class Port>
		zyn::port<Port>* efftype() const { // TODO: panning must be int...
			return spawn_new<zyn::port<Port>>("efftype");
		}

		// TODO: template is useless
		template<class Port>
		zyn::port<Port>* eff0_part_id() const { // TODO: panning must be int...
			return new zyn::port<Port>(ins, "/", "Pinsparts0");
			//return spawn_new<zyn::port<Port>>("efftype");
		}
	};

	class part_t : zyn::znode_t
	{
	public:
		using zyn::znode_t::znode_t;
		//zyn::amp_env amp_env() const {
		//	return spawn<zyn::amp_env>("AmpEnvelope/");
		//}
		template<class Port>
		zyn::port<Port>* Ppanning() const { // TODO: panning must be int...
			return spawn_new<zyn::port<Port>>("Ppanning");
		}

		template<std::size_t Id = 0>
		fx_t partefx() const { // TODO: panning must be int...
			return spawn<fx_t, Id>("partefx");
		}

	//	using T = fx_t (zyn_tree_t::*)(const std::string& ext) const;
	//	const T partefx2 = &zyn_tree_t::spawn<fx_t, 0>;


		//using partefx2 = zyn_tree_t::spawn<fx_t, 0>; // TODO: instead, return struct which has operator()?
	};

	//void f(){

	/*	(void)ptr;
		//const T* const insefx2 = spawn<fx_t, 0>;
	}*/
	template<std::size_t Id = 0>
	fx_t insefx() const { return spawn<fx_t, Id>("insefx"); }


	template<std::size_t Id>
	part_t part() const { return spawn<part_t, Id>("part"); } // TODO: large tuple for these
	part_t part0() const { return part<0>(); }


/*	// TODO???
	using volume_ptr_t = zyn::port<int>*(*)();
	//spawn_new<zyn::port<Port>>;
	volume_ptr_t volume = &spawn_new<zyn::port<int>>;
*/
	template<class Port>
	zyn::port<Port>* volume() const {
		return spawn_new<zyn::port<Port>>("volume");
	}

#ifdef NEW_PORT_TYPES
	using insefx = z::insefx<zyn_tree_t>; // todo: automatic, via inheriting?
	std::tuple<std::pair<cstr<'i', 'n', 's', 'e', 'f', 'x'>, insefx>> ports;
#endif

	// /insefx0/efftype:b , :i
};

class zynaddsubfx_t : public zyn_tree_t
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
