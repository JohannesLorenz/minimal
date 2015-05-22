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
#include "minimal_plugin.h"
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

class node_t_base : public named_t
{
protected:
	std::map<std::string, node_t_base*> used_ch;
	using named_t::named_t;

public:
	void print_all_used(std::ostream& os) const;

	virtual void on_preinit() {} // TODO: priv
	void preinit();
};

class instrument_t;

template<class >
class node_t : public node_t_base
{
	using InstClass = instrument_t;
public: // TODO
	InstClass* ins;
// the inheriting class must define the sub-nodes

	//! if no node with ext exists, adds it. always returns the ref
	template<class NodeT> NodeT& add_if_new(const std::string& ext)
	{
		return static_cast<NodeT&>(
			*used_ch.emplace(ext, new NodeT(ins, name(), ext)).
			first->second);
	}

protected:

	/*template<class NodeT>
	NodeT* spawn_new(const std::string& ext) {
		return new NodeT(ins, name(), ext);
	}*/

	/*template<class NodeT, char ...Lttrs>
	NodeT* spawn() const {
		return NodeT(ins, name(), std::string{Lttrs...});
	}*/

	template<class NodeT>
	NodeT& spawn(const std::string& ext) {
		return add_if_new<NodeT>(ext);
	}

	template<class NodeT>
	NodeT& spawn(const std::string& ext, std::size_t id) {
		return spawn<NodeT>(ext + std::to_string(id));
	}

	template<class NodeT, std::size_t Id>
	NodeT& spawn(const std::string& ext) {
		return spawn<NodeT>(ext + std::to_string(Id));
	}
public:
	node_t(InstClass* ins, const std::string& base, const std::string& ext)
		// base is assumed to already end on '/'
		: node_t_base(base + ext + "/"), ins(ins) {}



	//node(std::string base, std::string ext, std::size_t id)
	//	: node(base, ext + std::to_string(id)) {}
};




class instrument_t : public effect_t, public work_queue_t
{
	multi_plugin_t plugin_creator;

	virtual const char* library_path() const = 0;
	virtual const std::vector<const char *> start_args() const = 0;
	std::vector<const char *> build_start_args() const;
protected:
	minimal_plugin* plugin = nullptr; // TODO: nullptr... auto_ptr?
private:
	std::vector<const command_base*> const_commands;

	const std::vector<bool>* cp;
	pid_t make_fork();
public:
	using effect_t::effect_t;

	minimal_plugin** get_plugin_ptr() { return &plugin; }

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
	//! much as it requireds
	virtual command_base *make_close_command() const = 0;

	//! should create an OSC command which will cause the plugin to
	//! initialize, as much as the plugin requires
	virtual std::string make_start_command() const = 0;

	void clean_up();

	bool _proceed(sample_t samples);
};

}

#endif // INSTRUMENT_H
