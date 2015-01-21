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

#ifndef IO_PORTS_H
#define IO_PORTS_H

#include <limits>
#include "effect.h"

namespace mini
{

class out_port_base
{
protected:
	effect_t* e;
public:
	bool changed = true;
	out_port_base(effect_t& ef) :
		e(&ef)
	{
	}

	virtual const void* get_value() const = 0; // void* is not good...
	float next_time = std::numeric_limits<float>::max();

	//virtual void connect(const in_port_base& ) ;
};

template<class T>
class out_port_templ : public out_port_base
{
protected:
public:
	T data;

	//operator const T&() { return data; }
public:
	//void connect(const in_port_templ& ip) { target = &ip; }

	using type = T;
	using data_type = T;

	out_port_templ(effect_t& e) : out_port_base(e)
		//: data(e)
	{
		e.out_ports.push_back(this);
	}

	const T& get() const { return data; }
	void set(const T& new_val) { /*return base::ref*/ data = new_val; }

	const void* get_value() const { return reinterpret_cast<const void*>(&data); }

};

// TODO: abstract port base
class in_port_base
{
protected:
	effect_t* e;
public:
	bool unread_changes = true; // initally send values
	const out_port_base* source = nullptr;
	in_port_base(effect_t& ef) :
		e(&ef)
	{
	}

	in_port_base(effect_t& ef, const out_port_base& source) :
		e(&ef),
		source(&source)
	{
	}

	float get_outs_next_time() const {
		return source->next_time;
	}
};

template<class T>
class in_port_templ : public in_port_base
{
protected:
	using base = in_port_templ<T>;

	T data;

public:
	using data_type = T;

	in_port_templ(effect_t& ef) :
		in_port_base(ef)
	//	: data(e)
	{
		ef.in_ports.push_back(this);
	}

	//operator const T&() { return data; }
	in_port_templ(effect_t& ef, const out_port_base& source) :
		in_port_base(ef, source)
	//	: data(e)
	{
		ef.in_ports.push_back(this);
	}
protected:
	bool set(const T& new_value) {
		if(!unread_changes)
		 throw "omitted a vlue";
		unread_changes = (data == new_value);
		if(unread_changes)
		{
			data = new_value;
		}
		return unread_changes;
	}

public:

	const T& get() const { return data; }

	bool update() {
		bool out_port_changed = source->changed;
		return (out_port_changed) && set(*(static_cast<const T*>(source->get_value())));
	}

	using type = T;
};

//! copy-value based connection
template<class T>
void operator<<(in_port_templ<T>& ipt, const out_port_templ<T>& opt)
{
	if(ipt.source != nullptr)
	 throw "double connect to in port";
	ipt.source = &opt;
}

//! pointer based connection
template<class T>
void operator<<(in_port_templ<T*>& ipt, const out_port_templ<T>& opt)
{
	if(ipt.source != nullptr)
	 throw "double connect to in port";
	ipt.source = &opt;
}

class self_port_base
{
public:
	bool unread_changes = true; // initally send values
	float get_outs_next_time() const {
		// who knows :-)) (bad validation of protocol...)
		return std::numeric_limits<float>::max();
	}
};

template<class T>
class self_port_templ : public self_port_base
{
protected:
	T data;
	float next_time;

protected:
	bool set(const T& new_value) {
		if(!unread_changes)
		 throw "omitted a vlue";
		unread_changes = (data == new_value);
		if(unread_changes)
		{
			data = new_value;
		}
		return unread_changes;
	}

public:
	using data_type = T;

	const T& get() const { return data; }

	//! this is actually a validation of the protocol...
	constexpr bool update() const { return true; }

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
	std::pair<int, int> lines[POLY_MAX][NOTES_MAX];
	//! the recently switched lines
//	std::size_t changed_hint[NOTES_MAX];

//	std::size_t* last_changed_hint = changed_hint;

	int changed_stamp = 0;
	std::array<std::pair<int, int>, POLY_MAX> recently_changed;

	note_signal_t() {
		recently_changed[0].first = -1;
	}
};

struct notes_out : out_port_templ<note_signal_t>
{
	using out_port_templ<note_signal_t>::out_port_templ;
};

struct notes_in : in_port_templ<note_signal_t*>
{
	using base::in_port_templ;
};

}

#endif // IO_PORTS_H
