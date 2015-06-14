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
#include <limits>

#include "sample.h"
#include "types.h"
#include "utils.h"

#include "port_fwd.h"

namespace mini
{

class effect_t;
class in_port_base;
class out_port_base;

void add_out_port(effect_t& e, out_port_base* opb);
void add_in_port(effect_t& e, in_port_base* opb);

class port_base : public is_variable
{
};

class out_port_base : public port_base
{
public: // TODO!! protected
	effect_t* e;
	std::vector<in_port_base*> readers;
public:
//	bool changed = true; // TODO: stamp?
	sample_t change_stamp = -1.0f;
	out_port_base(effect_t& ef) :
		e(&ef)
	{
	}

	virtual const void* get_value() const = 0; // void* is not good...
	sample_t next_time = std::numeric_limits<sample_t>::max();

	//virtual void connect(const in_port_base& ) ;
};

template<class T, bool IsDep>
class in_port_templ;

template<class T>
class out_port_templ : public out_port_base
{
protected:
	//! identifies us as a base for children
	using base = out_port_templ<T>;
public:
	T data;
protected:
	bool start = true;

	//operator const T&() { return data; }
public:
	//void connect(const in_port_templ& ip) { target = &ip; }

	using type = T;
	using data_type = T;

	template <class ...Args>
	out_port_templ(effect_t& e, Args... args) : // TODO: forward?
		out_port_base(e),
		data(args...)
	{
		add_out_port(e, this);
	}

	const T& get() const { return data; }
	void set(const T& new_val, sample_t now)
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

// TODO: abstract port base
class in_port_base : public util::non_copyable_t, public port_base
{
public: // TODO!! protected
	effect_t* e;
private:
	bool _is_trigger = false;
	bool _is_precomputable;
//protected:
public: // TODO
	sample_t change_stamp = -1;
	std::size_t id;

	//bool unread_changes = false; // initally send values - TODO??
protected:
	const out_port_base* source = nullptr;

/*
	friends
*/
	template<class T1, class T2, bool IsDep>
	friend void internal_connect(in_port_templ<T1, IsDep>& ipt, out_port_templ<T2>& opt);

	template<class T, bool IsDep>
	friend void operator<<(in_port_templ<const T*, IsDep>& ipt, out_port_templ<T>& opt);
public:

	in_port_base(in_port_base&& ) noexcept = default;

	in_port_base(effect_t& ef) :
		e(&ef)
	{
		add_in_port(ef, this);
		//no_rt::mlog << "NEW IN PORT AT: " << this << std::endl;
	}
/*
	in_port_base(effect_t& ef, const out_port_base& source) :
		e(&ef),
		source(&source)
	{
		add_in_port(ef, this);
		no_rt::mlog << "NEW IN PORT AT: " << this << std::endl;
	}*/

	template<class P>
	in_port_base(port_ctor<P> pc) : in_port_base(*pc.effect()) {}

	sample_t get_outs_next_time() const {
		return source->next_time;
	}

	virtual bool update() = 0;

	bool is_trigger() const { return _is_trigger; }
	void set_trigger(bool is_trigger = true) { _is_trigger = is_trigger; } // TODO: ctor?

	bool is_precomputable() const { return _is_precomputable; } // TODO: can those be removed?
	void set_precomputable(bool is_precomputable = true) { _is_precomputable = is_precomputable; }

	virtual void on_read(sample_t time) = 0;

	virtual const void* get_value() const = 0;

	virtual ~in_port_base() {}
};

template<class T, bool IsDep = true>
class in_port_templ_base : public in_port_base
{
public: // TODO! (protected)
	T data;
	using data_type = T;
	const void* get_value() const { return reinterpret_cast<const void*>(&data); }

	using in_port_base::in_port_base;
	template <class ...Args>
	in_port_templ_base(effect_t& e, Args... args) : // TODO: forward?
		in_port_base(e),
		data(args...)
	{
	}

	bool is_dependency() const { return IsDep; }

	const T& get() const { return data; }
	T& get() { return data; } // TODO? needed currently for ringbuffer_reader_t

	using type = T;
protected:
	void update_stamp() {
		//if(unread_changes)
		// throw "omitting a value now!";
		//unread_changes = true;
		// TODO: check for unread changes via stamp?
		change_stamp = source->change_stamp;
	}

};

template<class T, bool IsDep>
class in_port_templ_noassign : public in_port_templ_base<T, IsDep>
{
protected:
	//! identifies us as a base for children
	using base = in_port_templ<T, IsDep>;
	using templ_base = in_port_templ_base<T, IsDep>;
public:
	using in_port_templ_base<T, IsDep>::in_port_templ_base;
protected:
	bool set()
	{
		templ_base::update_stamp();
		return true;
	}

public:
	bool update() {
		bool out_port_changed = templ_base::change_stamp != templ_base::source->change_stamp;
		return (out_port_changed) && set();
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
	//	io::mlog << "OUT PORT CHANGED? " << out_port_changed << io::endl;
		return (out_port_changed) && set(*(static_cast<const T*>(templ_base::source->get_value())));
	}

};

template<class T, bool IsDep>
struct in_port_templ<T*, IsDep> : public in_port_templ_noassign<T*, IsDep>
{
public:
	using in_port_templ_noassign<T*, IsDep>::in_port_templ_noassign;
};

template<class T1, class T2, bool IsDep>
void internal_connect(in_port_templ<T1, IsDep>& ipt, out_port_templ<T2>& opt)
{
	if(ipt.source != nullptr)
	 throw "double connect to in port";
	ipt.source = &opt;
	opt.readers.push_back(&ipt);
	if(ipt.is_dependency()) // TODO: via template matching
	 opt.e->deps.push_back(ipt.e);
	else
	 opt.e->readers.push_back(ipt.e);
	ipt.e->writers.push_back(opt.e);
}


//! copy-value based connection
template<class T1, class T2, bool IsDep> // TODO: make traits such that T1 matches T2
void operator<<(in_port_templ<T1, IsDep>& ipt, out_port_templ<T2>& opt)
{
	internal_connect(ipt, opt);
}

//! pointer based connection
template<class T, bool IsDep>
void operator<<(in_port_templ<const T*, IsDep>& ipt, out_port_templ<T>& opt)
{
	internal_connect(ipt, opt);
	ipt.data = &opt.data;
}

template<class T, T Value>
struct _constant
{
	constexpr static T value = Value;
};

//! constant value connection // TODO: also for pointer?
template<class T, T V, bool IsDep>
void operator<<(in_port_templ<T, IsDep>& ipt, const _constant<T, V>&) // TODO: forward
{
	ipt.data = V;
}

class self_port_base : public port_base
{
public:
	bool unread_changes = false; // initally send values - TODO
	sample_t get_outs_next_time() const {
		// who knows :-)) (bad validation of protocol...)
		return std::numeric_limits<sample_t>::max();
	}
	virtual const void* get_value() const = 0;
};

template<class T, bool = false>
class self_port_templ : public self_port_base
{
protected:
	T data;
	sample_t next_time;

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
	//	io::mlog << "GET: " << *reinterpret_cast<const T*>(&data) << io::endl;
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
	//! lines[x][y] : pair at height x, polymorphy y
	//! first: a unique id for this note, or -1 for note off
	//! second: volume
	std::pair<int, int> lines[NOTES_MAX][POLY_MAX];

	int changed_stamp = 0;

	//! the recently switched lines
	//! pairs geometry <-> polymorphy id
	//! if the pair is <x,y>, it marks lines[x][y]
	std::array<std::pair<int, int>, POLY_MAX> recently_changed;

	note_signal_t() {
		recently_changed[0].first = -1;
	}
};

struct notes_out : out_port_templ<note_signal_t>
{
	using base::out_port_templ;
};

struct notes_in : in_port_templ<const note_signal_t*>
{
	using base::in_port_templ;
};

}

#endif // IO_PORTS_H
