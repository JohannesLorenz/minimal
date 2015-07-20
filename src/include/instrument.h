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

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <string>
#include <vector>
#include <map>
#include <initializer_list>

#include "plugin.h"
#include "utils.h"
#include "effect.h"
#include "work_queue.h"
//#include "minimal_plugin.h"
#include "port_fwd.h"

namespace mini
{

class command_base;

#if 0

template<class Functor, class Tar, class ...Src>
class func
{
	using id_t = std::size_t;
	static id_t next_id;
	Functor ftor;
public:
	id_t id;
	func(Functor& ftor) : ftor(ftor), id(next_id++) {}
	Tar operator()(Src... input) { return ftor(input...); }
};


template<class F, class T, class ...S>
typename func<F, T, S...>::id_t func<F, T, S...>::next_id;

template<class T>
class con
{
	using id_t = std::size_t;
	static id_t no_id() { return std::numeric_limits<id_t>::max(); }
public:
	id_t id;
	T value;
	bool is_fixed() const { return id == no_id(); }
	con(const T& fixed_value) :
		id(no_id()),
		value(fixed_value)
	{
	}
	con(const typename T::value_type& fixed_value) :
		id(no_id()),
		value(fixed_value)
	{
	}

	template<class F, class ...S>
	con(const func<F, T, S...>& func) :
		id(func.id),
		value(0)
	{
	}


/*	con(const con<T>& other) = default;
	con(const con<typename T::base_type>& other) :
		id(other.id),
		value(other.value)
	{
	}*/
};
#endif

class instrument_t : public effect_t, public work_queue_t
{
	multi_plugin_t plugin_creator;
protected:
	//minimal_plugin* plugin = nullptr; // TODO: nullptr... auto_ptr?
private:
	std::vector<const command_base*> const_commands;

	const std::vector<bool>* cp;
	pid_t make_fork();

	//! should advance the instrument s.t. at least @a samples samples
	//! are computed in all out ports
	virtual bool advance() = 0;
public:
	using effect_t::effect_t;

	//minimal_plugin** get_plugin_ptr() { return &plugin; }

	virtual void init_2() = 0;
	void instantiate();

	instrument_t(const char* name, std::initializer_list<const command_base*> const_commands);
	void add_const_command(const command_base& cmd);

	virtual ~instrument_t();

	void pass_changed_ports(const std::vector<bool>& _cp)
	{ // TODO: no vector bool -> stack of ints
		cp = &_cp;
	}

//	virtual instrument_t* clone() const = 0; // TODO: generic clone class?
	//! should create OSC commands which cause the plugin to clean up as
	//! much as it requires
	virtual command_base *make_close_command() const = 0;

	//! should create an OSC command which will cause the plugin to
	//! initialize, as much as the plugin requires
	virtual std::string make_start_command() const = 0;
	
	//! shall send the OSC string @a cmd to the plugin
	virtual void send_osc_cmd(const char* cmd) = 0;

	void clean_up();

	bool _proceed() final;
	
	// this will be instantiated before the instrument will be
	virtual void instantiate_first() = 0;
};

class plugin_instrument : public instrument_t, public multi_plugin_t
{
	void instantiate_first();
};

}

#endif // INSTRUMENT_H
