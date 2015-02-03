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

#ifndef IO_PORTS_H
#define IO_PORTS_H

#include <array>
#include <iostream> // TODO!
#include <limits>

#include "types.h"
#include "utils.h"

namespace mini
{

class effect_t;
class in_port_base;
class out_port_base;

void add_out_port(effect_t& e, out_port_base* opb);
void add_in_port(effect_t& e, in_port_base* opb);
/*
template<class Port>
class port_ctor
{
	effect_t* e;
public:
	port_ctor(effect_t& e) : e(e) {}
	effect_t* effect() { return e; }
};*/

class port_base : public is_variable
{
};

class out_port_base : public port_base
{
public: // TODO!! protected
	effect_t* e;
public:
//	bool changed = true; // TODO: stamp?
	float change_stamp = -1.0f;
	out_port_base(effect_t& ef) :
		e(&ef)
	{
	}

	virtual const void* get_value() const = 0; // void* is not good...
	float next_time = std::numeric_limits<float>::max();

	//virtual void connect(const in_port_base& ) ;
};

template<class T, bool IsDep>
class in_port_templ;

template<class T>
class out_port_templ : public out_port_base
{
protected:
public:
	T data;
	bool start = true;

	//operator const T&() { return data; }
public:
	//void connect(const in_port_templ& ip) { target = &ip; }

	using type = T;
	using data_type = T;

	out_port_templ(effect_t& e) : out_port_base(e)
		//: data(e)
	{
		add_out_port(e, this);
	}

	const T& get() const { return data; }
	void set(const T& new_val, float now)
	{ /*return base::ref*/
		if(start || data != new_val)
		{
			data = new_val;
			change_stamp = now;
			start = false;
		}
	//	changed = true;
	}

	const void* get_value() const { return reinterpret_cast<const void*>(&data); }

//	friend
//	void operator<<(in_port_templ<T>& ipt, const out_port_templ<T>& opt);
};

class lo_port_t;

// TODO: abstract port base
class in_port_base : public non_copyable_t, public port_base
{
public: // TODO!! protected
	effect_t* e;
private:
	bool _is_trigger = false;
protected:
public: // TODO
	float change_stamp = -1.0f;
public:
	bool unread_changes = false; // initally send values - TODO??
	const out_port_base* source = nullptr;
	in_port_base(effect_t& ef) :
		e(&ef)
	{
		add_in_port(ef, this);
	}

	in_port_base(effect_t& ef, const out_port_base& source) :
		e(&ef),
		source(&source)
	{
		add_in_port(ef, this);
	}

	float get_outs_next_time() const {
		return source->next_time;
	}

	virtual bool update() = 0;

	// TODO: const? probably not...
	//virtual void send_all(lo_port_t* ) {} // TODO: not sure if this fits here always

	bool is_trigger() const { return _is_trigger; }
	void set_trigger(bool is_trigger = true) { _is_trigger = is_trigger; } // TODO: ctor?

	virtual void on_recv(float time) = 0;

	virtual const void* get_value() const = 0;
};

template<class T, bool IsDep = true>
class in_port_templ_base : public in_port_base
{
public: // TODO! (protected)
	T data;
	using data_type = T;
	const void* get_value() const { return reinterpret_cast<const void*>(&data); }

	using in_port_base::in_port_base;

	bool is_dependency() const { return IsDep; }

	const T& get() const { return data; }

	using type = T;
protected:
	void update_stamp() {
		if(unread_changes)
		 throw "omitting a value now!";
		unread_changes = true;
		change_stamp = source->change_stamp;
	}

};

template<class T, bool IsDep = true>
class in_port_templ : public in_port_templ_base<T, IsDep>
{
protected:
	//! identifies us as a base for children
	using base = in_port_templ<T, IsDep>;
	using templ_base = in_port_templ_base<T, IsDep>;

public:
	using in_port_templ_base<T, IsDep>::in_port_templ_base;

protected:
	bool set(const T& new_value)
	{
		templ_base::update_stamp();
		templ_base::data = new_value;
		return true;
	}
public:

	bool update() {
		bool out_port_changed = templ_base::change_stamp != templ_base::source->change_stamp;
		std::cerr << "OUT PORT CHANGED? " << out_port_changed << std::endl;
		return (out_port_changed) && set(*(static_cast<const T*>(templ_base::source->get_value())));
	}

};

template<class T, bool IsDep>
class in_port_templ<T*, IsDep> : public in_port_templ_base<T*, IsDep>
{
protected:
	//! identifies us as a base for children
	using base = in_port_templ<T*, IsDep>;
	using templ_base = in_port_templ_base<T*, IsDep>;
public:
	using in_port_templ_base<T*, IsDep>::in_port_templ_base;
protected:
	bool set(const T* )
	{
		templ_base::update_stamp();
		return true;
	}

public:
	bool update() {
		bool out_port_changed = templ_base::change_stamp != templ_base::source->change_stamp;
		return (out_port_changed) && set((static_cast<const T*>(templ_base::source->get_value())));
	}
};




//! copy-value based connection
template<class T, bool IsDep>
void operator<<(in_port_templ<T, IsDep>& ipt, const out_port_templ<T>& opt)
{
	if(ipt.source != nullptr)
	 throw "double connect to in port";
	ipt.source = &opt;
	if(ipt.is_dependency()) // TODO: via template matching
	 opt.e->deps.push_back(ipt.e);
	else
	 opt.e->readers.push_back(ipt.e);
	ipt.e->writers.push_back(opt.e);
}

//! pointer based connection
template<class T, bool IsDep>
void operator<<(in_port_templ<const T*, IsDep>& ipt, const out_port_templ<T>& opt)
{
	if(ipt.source != nullptr)
	 throw "double connect to in port";
	ipt.source = &opt;
	ipt.data = &opt.data;
	if(ipt.is_dependency()) // TODO: via template matching
	 opt.e->deps.push_back(ipt.e);
	else
	 opt.e->readers.push_back(ipt.e);
	ipt.e->writers.push_back(opt.e);
}

class self_port_base : public port_base
{
public:
	bool unread_changes = true; // initally send values
	float get_outs_next_time() const {
		// who knows :-)) (bad validation of protocol...)
		return std::numeric_limits<float>::max();
	}
	virtual const void* get_value() const = 0;
};

template<class T, bool = false>
class self_port_templ : public self_port_base
{
protected:
	T data;
	float next_time;

protected:
/*	bool set(const T& new_value) {
		if(!unread_changes)
		 throw "omitted a vlue";
		unread_changes = (data == new_value);
		if(unread_changes)
		{
			data = new_value;
		}
		return unread_changes;
	}*/

public:
	using data_type = T;

	const T& get() const { return data; }

	void set(const T& _data) { data = _data; }

	//! this is actually a validation of the protocol...
	constexpr bool update() const { return true; }

	const void* get_value() const {
		std::cerr << "GET: " << *reinterpret_cast<const T*>(&data) << std::endl;
		return reinterpret_cast<const void*>(&data); }

	//! this has no effect
	void set_trigger(bool = true) const { }

	using type = T;
};

template<class T>
struct freq_lfo_out : out_port_templ<T>
{
	using out_port_templ<T>::out_port_templ;
};

constexpr std::size_t NOTES_MAX = 12 * 10;
constexpr std::size_t POLY_MAX = 16;

//
// a few example ports
//

struct note_signal_t
{
	//! whether a note at height <int> is on or off
	std::pair<int, int> lines[NOTES_MAX][POLY_MAX];

	int changed_stamp = 0;

	//! the recently switched lines
	std::array<std::pair<int, int>, POLY_MAX> recently_changed;

	note_signal_t() {
		recently_changed[0].first = -1;
	}
};

struct notes_out : out_port_templ<note_signal_t>
{
	using out_port_templ<note_signal_t>::out_port_templ;
};

struct notes_in : in_port_templ<const note_signal_t*>
{
	using base::in_port_templ;
};

}

#endif // IO_PORTS_H
