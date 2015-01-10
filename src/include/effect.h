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

#ifndef EFFECT_H
#define EFFECT_H

#include <set>
#include <vector>
#include "daw.h"
#include "types.h"
#include "work_queue.h"

const float default_step = 0.1f; //0.001seconds; // suggested by fundamental

namespace mini
{
#if 0
class effect_t;

template<class T, class RefT>
class base_port
{
protected:
	effect_t* ef_ref;
	RefT ref;
	//RefT last_value;
	bool _changed;
	void reset_value(const T& new_value) {
		_changed = (ref != new_value);
		ref = new_value;
	}

public:
	const effect_t* get_ef_ref() const { return ef_ref; }
	effect_t* get_ef_ref() { return ef_ref; }
	bool changed() { return changed; }
	using type = T;
	const RefT& get() const { return ref; }
	base_port(effect_t& ef_ref) :
		ef_ref(&ef_ref) {}
	base_port() {} // TODO: remove?
};


template<class T>
class out_port : public base_port<T, T>
{
	using base = base_port<T, T>;
public:
	const T& set(const T& new_val) { return base::ref = new_val; }
	using base_port<T, T>::base_port;
};


template<class T>
class in_port : public base_port<T, T>
{
	using base = base_port<T, T>;
	const out_port<T>* out_p;
public:
	void connect(out_port<T>& op) {
		out_p = &op;
		op.get_ef_ref()->readers.push_back(base::ef_ref);
		base::ef_ref->writers.push_back(op.get_ef_ref());
	}

	void update() { reset_value(out_p->get()); }

	using base_port<T, T>::base_port;
};
#endif

class in_port_base;
class out_port_base;
/*
struct port_chain
{
	std::vector<out_port_base*> in_ports, out_ports;
};*/
/*
template<class ...Args>
struct arg_ptrs
{

};*/

class effect_t //: public port_chain
{
public:
	std::vector<in_port_base*> in_ports;
	std::vector<out_port_base*> out_ports; // TODO: not public
private:
	float next_time;
	template<class T, class Tpl, int ...Is>
	static std::vector<T> make_vector(const Tpl& tpl, util::seq<Is...> ) {
		return std::vector<T>{ &std::get<Is>(tpl)... };
	}
	template<class T, class ...Args>
	static std::vector<T> make_vector(const std::tuple<Args...>& tpl) {
		return make_vector<T>(tpl, util::gen_seq<sizeof...(Args)>());
	}
protected:
	virtual float _proceed(float time) = 0;
public:
	std::vector<effect_t*> readers, writers;
	// returns the next time when the effect must be started
	float proceed(float time) {
		return next_time = _proceed(time);
	}

	float get_next_time() const { return next_time; }
	float get_childrens_next_time() const {
		float result = std::numeric_limits<float>::max();
		for(const effect_t* e : writers)
		 result = std::min(result, e->get_next_time());
		return result;
	}

	/*template<class ...Args1, class ...Args2>
	effect_t(const std::tuple<Args1&...>& in_ports, const std::tuple<Args2&...>& out_ports) // TODO: fwd?
		: in_ports(in_ports), out_ports(out_ports)
	{
	}*/

	template<class ...Args2>
	effect_t(const std::tuple<Args2&...>& out_ports)
		: out_ports(make_vector<out_port_base*>(out_ports))
	{
	}

	effect_t() {}
};

template<class Impl>
class has_impl_t
{
protected:
	Impl* impl;
public:
	//has_impl_t() : impl(new Impl) {}
	template<class T>
	has_impl_t(T* ref) : impl(new Impl(ref)) {}
};

template<class Ref>
class is_impl_of_t
{
protected:
	Ref* ref;
public:
	template<class T>
	is_impl_of_t(T* ref) : ref(ref) {}
};


/*template<class Self, class Impl>
class effect_with_impl_t : has_impl_t<Impl>, effect_t
{
	template<class ...Args2>
	effect_with_impl_t(const std::tuple<Args2&...>& out_ports) :
		effect_t(out_ports),
		has_impl_t()
	{
	}

	effect_with_impl_t() {}
	//using effect_t::effect_t;
};*/

class ef_con_base
{
	virtual effect_t* instantiate() = 0;
};

template<class T>
class ef_con_t
{
	T* instantiate() { return new T; } // TODO: smart ptr?
};

#if 0
template<class OutType>
struct freq_lfo_out
{
	const effect_t& ef_ref;
	out_port<OutType> out;
	freq_lfo_out(const effect_t& ef_ref) : ef_ref(ef_ref) {}
};
#endif
/*
template<class T>
class has_lfo_out
{
	out_port<T> lfo_out;
};*/

class abstract_effect_t
{

};

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
	T data;

public:
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

template<class T>
void operator<<(in_port_templ<T>& ipt, const out_port_templ<T>& opt)
{
	if(ipt.source != nullptr)
	 throw "double connect to in port";
	ipt.source = &opt;
}


template<class T>
struct freq_lfo_out : out_port_templ<T>
{
	using out_port_templ<T>::out_port_templ;
};

constexpr std::size_t NOTES_MAX = 12 * 10;
constexpr std::size_t POLY_MAX = 16;

struct note_signal_t
{
	//! whether a note at height <int> is on or off
	int lines[POLY_MAX][NOTES_MAX];
	//! the recently switched lines
//	std::size_t changed_hint[NOTES_MAX];

//	std::size_t* last_changed_hint = changed_hint;

	int changed_stamp = 0;
	std::pair<int, int> recently_changed[POLY_MAX];

	note_signal_t() {
		recently_changed[0].first = -1;
	}
};

struct notes_out : out_port_templ<note_signal_t>
{
};


/*
template<class OutType>
struct lfo_out
{
	out_port<OutType> port;
};

template<class OutType>
struct lfo_in
{
	in_port<OutType> port;
};

struct notes_out
{
	out_port<int*> port;
};

struct notes_in
{
	in_port<int*> port;
};*/



#if 0
template<std::size_t I, class Tp, class First, class ...Args>
class _port_chain : public First, public _port_chain<I+1, Tp, Args...>
{
	using base = _port_chain<I+1, Tp, Args...>;
public:
	_port_chain(effect_t& ef_ref) : First { ef_ref }, base(ef_ref) {}
	//_port_chain() : First(std::get<I>(this->tp)) {} // TODO: "this"
};

template<std::size_t I, class Tp, class First> class _port_chain<I, Tp, First> : public First
{
protected:
	_port_chain(effect_t& ef_ref) : First { ef_ref } {}
};

template<class ...Args>
class port_chain : public Args...
{
	using base = _port_chain<0, void, Args...>;
public:
	std::tuple<decltype(Args::port)&...> tp;
	port_chain(effect_t& ef_ref):
		tp(Args::port...) {}
};

struct _test : effect_t
{
	port_chain<lfo_in<float>, lfo_out<float>> ch;
	_test() : ch(*this) {
		auto p = ch.lfo_in<float>::port;
		(void)p;
	}
};
#endif



constexpr unsigned char MAX_NOTES_PRESSED = 32;

using namespace daw; // TODO

class note_line_t;

class note_line_impl : public is_impl_of_t<note_line_t>, public work_queue_t
{
	//std::map<int, std::map<float, note_t>> note_lines;
	float last_time = -1.0f;
	struct notes_impl_t
	{
		std::map<note_geom_t, note_t>::const_iterator itr;
		//std::vector<note_t_impl> children;
		typedef boost::heap::fibonacci_heap<notes_impl_t> pq_type;
		std::pair<note_geom_t, note_t> next_elem; // todo: ptr?


		pq_type pq;


		std::pair<note_geom_t, note_t> next_note() {
			next_elem =
			return std::min(*itr, pq.top());
		}
	};

	struct note_task_t : public task_base
	{
		//const loaded_instrument_t* ins;
		//const command_base* cmd;
		note_line_impl* nl_ref;
		//int* last_key;
		const int note_height;
		bool is_on = false;
		std::map<float, note_t>::const_iterator itr;


		void proceed(float time);

		note_task_t(note_line_impl& nl_ref,
			const int& note_height,
			const std::map<float, note_t>& values,
			float first_event = 0.0f) :
			task_base(first_event),
			nl_ref(&nl_ref),
			//last_key(nl_ref.notes_pressed.get()),
			note_height(note_height),
			itr(values.begin())
		{
			if(note_height < 0 || note_height >= (int)NOTES_MAX)
			 throw "invalid note height";
		}
	};

	notes_impl_t root;
public:
	note_line_impl(note_line_t *nl);

	float _proceed(float time) {
		return run_tasks(time);
	}
};

class note_line_t : public notes_out, effect_t, has_impl_t<note_line_impl> // TODO: which header?
{
	friend class note_line_impl;


	//std::multimap<note_geom_t, notes_t> note_events;
	 //! @note: one might need to store the notes_t blocks seperated for muting etc
	notes_t notes;

	//int _notes_pressed[MAX_NOTES_PRESSED];
//	out_port_templ<note_signal_t> notes_pressed; // TODO: pointer?
public:
	note_line_t(/*std::multimap<note_geom_t, note_t>&& note_events*/) :
		/*note_events(note_events),*/
//		port_chain<notes_out>((effect_t&)*this),
		notes_out(*this),
		effect_t(*this),
		has_impl_t(this)
//		notes_pressed((effect_t&)*this)
	{
		//notes_pressed.set(_notes_pressed);
	}

	void add_notes(const notes_t& n, const note_geom_t& ng) {
		//note_events.emplace(ng, n);
		notes.add_notes(n, ng);
	}


	float _proceed(float time) {
		return impl->_proceed(time);
	}
};

}

#endif // EFFECT_H
