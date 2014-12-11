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

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <string>
#include <vector>
#include <memory>
#include <iostream> // TODO
#include <map>
#include <set>
// ...
// when is it going to end?

#include "types.h"
#include "utils.h"
#include "daw.h"
#include "command.h"
#include "effect.h"

namespace mini
{


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

class named_t
{
	const std::string _name;
public:
	const std::string& name() const { return _name; }
	named_t(const char* _name) : _name(_name) {}
	named_t(const std::string& _name) : _name(_name) {}
};


struct map_cmp
{
	bool operator()(const command_base* c1, const command_base* c2)
	{
		return *c1 < *c2;
	}
};

#if 0
class activator_events;
class activator_poll;

class activator_base_itr
{
public:
	virtual float operator*() const = 0;
	virtual activator_base_itr& operator++() = 0;
};

class activator_events_itr : public activator_base_itr
{
 public: // TODO
	std::set<float>::const_iterator itr;
public:
	activator_events_itr(const activator_events& ab);
	virtual float operator*() const { return *itr; }
	virtual activator_base_itr& operator++() {
		return ++itr, *this;
	}
};

class activator_poll_itr : public activator_base_itr
{
public:
	activator_poll_itr(const activator_poll& )
		//itr(ab.begin())
	{
	}
	virtual float operator*() const { return 0.0f; }
	virtual activator_base_itr& operator++() { return *this; }
};



class activator_base
{
public:
	virtual activator_base_itr* make_itr() const = 0;
};

class activator_events : public activator_base
{ public: // TODO
	std::set<float> events;
	//std::set<float>::const_iterator itr = events.begin();
	friend class activator_events_itr;
public:
	activator_events(const std::set<float>& events) : events(events) {}
	void insert(const float& val) { events.insert(val); }
	void move_from(activator_events* src) { // TODO: should pass rvalue?
		std::move(src->events.begin(), src->events.end(), // TODO: use move!
			std::inserter(events, events.end()));
	}
	activator_events_itr* make_itr() const { return new activator_events_itr(*this); }
};

class activator_poll : public activator_base
{
public:
	activator_poll_itr* make_itr() const { return new activator_poll_itr(*this); }
};

// height, command + times
using cmd_vectors = std::map<const command_base*, activator_base*, map_cmp>; // TODO: prefer vector?
#endif

using cmd_vectors = std::map<const command_base*, std::set<float>, map_cmp>; // TODO: prefer vector?

class instrument_t : public named_t, non_copyable_t, public effect_t
{
public:
	using id_t = std::size_t;
private:
	static std::size_t next_id;
	const std::size_t _id;
	std::vector<command_base*> commands; // TODO: unique?
	const std::vector<command_base*> _quit_commands;
public:
	const std::vector<command_base*>& quit_commands() const {
		return _quit_commands;
	}
	using port_t = int;
	instrument_t(const char* name,
		const std::vector<command_base*>&& _quit_commands) :
		named_t(name),
		_id(next_id++),
		_quit_commands(_quit_commands)
		{ std::cout << "instrument: constructed" << std::endl; }
	virtual ~instrument_t();
//	virtual instrument_t* clone() const = 0; // TODO: generic clone class?

	//instrument_t(const instrument_t& other);


	const id_t& id() const { return _id; }
	enum class type
	{
		zyn
	};
/*	template<class ...Args>
	void add_param_fixed(const char* param, Args ...args) {
		using command_t = command<Args...>;
		commands.push_back(std::unique_ptr<command_t>(new command_t(param, args...)));
	}*/


/*	template<class C, class ...Args>
	void add_command_fixed(Args ...args) {
		commands.push_back(new C(args...));
	}*/


	//template<class Command>
	//void add_port(const Command& cb) { add<Command, command_base>(cb, note_geom_t(0, 0)); }

	void set_param_fixed(const char* param, ...);
	virtual std::string make_start_command() const = 0;
	//! shall return the lo port (UDP) after the program was started
	virtual port_t get_port(pid_t pid, int fd) const = 0;
	//instrument_t(instrument_t&& other) = default;

	virtual cmd_vectors make_note_commands(const std::multimap<daw::note_geom_t, daw::note_t>& ) const = 0;





	virtual float proceed(float time)
	{
		(void)time;
		return 0.0;
		// TODO
	}
};

template <char ...Letters> class fixed_str {
	static std::string make_str() { return std::string(Letters...); }
};


/*template<class Cmd, class ...Args>
make_cmd()*/


}

#endif // INSTRUMENT_H
