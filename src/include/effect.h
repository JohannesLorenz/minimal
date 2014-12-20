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

class effect_t
{
	float next_time;
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
};

class ef_con_base
{
	virtual effect_t* instantiate() = 0;
};

template<class T>
class ef_con_t
{
	T* instantiate() { return new T; } // TODO: smart ptr?
};

template<class OutType>
struct freq_lfo_out
{
	const effect_t& ef_ref;
	out_port<OutType> out;
	freq_lfo_out(const effect_t& ef_ref) : ef_ref(ef_ref) {}
};
/*
template<class T>
class has_lfo_out
{
	out_port<T> lfo_out;
};*/

class abstract_effect_t
{

};

constexpr unsigned char MAX_NOTES_PRESSED = 32;

using namespace daw; // TODO

class note_line_t : public effect_t, public work_queue_t
{

	std::multimap<note_geom_t, notes_t> note_events;


	int _notes_pressed[MAX_NOTES_PRESSED];
	using notes_pressed_ref = int*;
	out_port<notes_pressed_ref> notes_pressed;
public:
	note_line_t(/*std::multimap<note_geom_t, note_t>&& note_events*/) :
		/*note_events(note_events),*/
		notes_pressed(*this)
	{
		notes_pressed.set(_notes_pressed);
	}

	void add_notes(const notes_t& n, const note_geom_t& ng) {
		note_events.emplace(ng, n);
	}

	struct note_task_t : public task_base
	{
		//const loaded_instrument_t* ins;
		//const command_base* cmd;
		note_line_t* nl_ref;
		int* last_key;
		const int note_height;
		std::set<float>::const_iterator itr;

		void proceed(float /* time*/) {

			//ins->con.send_osc_str(cmd->buffer());

			*(last_key++) = note_height;

			update_next_time(*++itr);
		}
		note_task_t(note_line_t& nl_ref,
			const int& note_height,
			const std::set<float>& values,
			float first_event = 0.0f) :
			task_base(first_event),
			nl_ref(&nl_ref),
			last_key(nl_ref.notes_pressed.get()),
			note_height(note_height),
			itr(values.begin())
		{
		}
	};



	float _proceed(float time) {
		return run_tasks(time);
	}
};


#if 1
template<class OutType>
class lfo_out
{
public:
	out_port<OutType> port;
};

template<class OutType>
class lfo_in
{
public:
	out_port<OutType> port;
};


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
class port_chain : public _port_chain<0, void, Args...>
{
	using base = _port_chain<0, void, Args...>;
public:
	std::tuple<decltype(Args::port)&...> tp;
	port_chain(effect_t& ef_ref) : base(ef_ref),
		tp(Args::port...) {}
};

#elif 0
template<class PortType>
class has_port
{
public:
	using port_type = PortType;
protected:
	const port_type& ref;
public:
	has_port(const port_type& ref) : ref(ref) {}
};

template<class OutType>
class has_lfo_out : public has_port<out_port<OutType>>
{
public:
	using port_type = typename has_port<out_port<OutType>>::port_type;
	const port_type& port_lfo_out() const { return this->ref; }
};

template<std::size_t I, class Tp, class First, class ...Args>
class _port_chain : public First, _port_chain<I+1, Tp, Args...>
{
	_port_chain() : First(std::get<I>(this->tp)) {} // TODO: "this"
};

template<std::size_t I, class Tp, class First> class _port_chain<I, Tp, First> : public First
{
protected:
	Tp tp;
	_port_chain() : First(std::get<I>(tp)) {}
};

template<class ...Args>
class port_chain : public _port_chain<0, std::tuple<typename Args::port_type...>, Args...>
{
};

#else



template<const out_port<float>& Ref>
class has_lfo_out
{
public:
	using port_type = out_port<float>;
protected:
	//const port_type& ref;
public:
	//has_port(const port_type& ref) : ref(ref) {}
	const port_type& port_lfo_out() const { return Ref; }
};
/*
template<class OutType>
class has_lfo_out : public has_port<out_port<OutType>>
{
public:
	using port_type = typename has_port<out_port<OutType>>::port_type;
	const port_type& port_lfo_out() const { return this->ref; }
};*/

template<std::size_t I, class Tp, template<const out_port<float>& > class First, template<const out_port<float>& > class ...Args>
class _port_chain : public First<std::get<I>(_port_chain<I+1, Tp, Args...>::tp)>, _port_chain<I+1, Tp, Args...>
{
	//_port_chain() : First(std::get<I>(this->tp)) {} // TODO: "this"
};

template<class Tp>
struct _port_head
{
	Tp tp;
};

template<std::size_t I, class Tp, template<const out_port<float>& > class First>
class _port_chain<I, Tp, First> : public _port_head<Tp>, First<std::get<I>(_port_head<Tp>::tp)>
{
protected:
	//Tp tp;
	//_port_chain() : First(std::get<I>(tp)) {}
};

template<template<const out_port<float>& > class ...Args>
class port_chain : public _port_chain<0, std::tuple<out_port<float>, out_port<float>>, Args...>
{
};

#endif

struct _test : effect_t
{
	port_chain<lfo_out<float>, lfo_in<float>> ch;
	_test() : ch(*this) {
		auto p = ch.lfo_out<float>::port;
		(void)p;
	}
};

}

#endif // EFFECT_H
