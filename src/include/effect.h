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

#ifndef EFFECT_H
#define EFFECT_H

#include <set>
#include <vector>
#include <array>

#include "daw.h"
#include "types.h"
#include "work_queue.h"
#include "ports.h" // TODO!!! not needed!
#include "simple.h"
#include "io.h"

const float default_step = 0.1f; //0.001seconds; // suggested by fundamental

namespace mini
{

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

class has_id
{
	std::size_t _id;
public:
	has_id(std::size_t id = no_id()) : _id(id) {}
	std::size_t id() const { return _id; }
	void set_id(std::size_t id) { _id = id; }
	static constexpr std::size_t no_id() { return -1; }
};

class effect_t : util::non_copyable_t, public has_id, public named_t //: public port_chain
{
protected:
	std::vector<in_port_base*> in_ports;
	std::vector<out_port_base*> out_ports; // TODO: not public
private:
	float next_time;
	template<class T, class Tpl, int ...Is>
	//! helper
	static std::vector<T> make_vector(const Tpl& tpl, util::seq<Is...> ) {
		return std::vector<T>{ &std::get<Is>(tpl)... };
	}
	//! converts tuple to vector
	template<class T, class ...Args>
	static std::vector<T> make_vector(const std::tuple<Args...>& tpl) {
		return make_vector<T>(tpl, util::gen_seq<sizeof...(Args)>());
	}
protected:
	virtual float _proceed(float time) = 0;
public:
	std::vector<in_port_base*>& get_in_ports() { return in_ports; }
	std::vector<out_port_base*>& get_out_ports() { return out_ports; }

	void add_in_port(in_port_base* ipb);
	void add_out_port(out_port_base* opb);

	virtual void instantiate() = 0;
	virtual void clean_up() = 0;
	virtual void pass_changed_ports(const std::vector<bool>& ) {}

	std::vector<effect_t*> readers, deps, writers;
	// returns the next time when the effect must be started
	float proceed(float time) {
		io::mlog << "proceeding with effect " << id() << io::endl;
		return next_time = _proceed(time);
	}

	float get_next_time() const { return next_time; }
	void set_next_time(float next) { next_time = next; }

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
	effect_t(const std::tuple<Args2&...>& out_ports, const char* name = "unnamed")
		: named_t(name),
		out_ports(make_vector<out_port_base*>(out_ports))
	{
	}

	effect_t(const char* name = "unnamed") : named_t(name) {}
	virtual ~effect_t() {}
};

//! this is a dummy which will always be the last effect in the queue.
class sentinel_effect : public effect_t
{
public:
	sentinel_effect() {
		//set_id(std::numeric_limits<std::size_t>::max());
		set_next_time(std::numeric_limits<float>::max());
	}
	float _proceed(float ) { throw "impossible"; }
	void instantiate() {}
	void clean_up() {}
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

#if 0
// this may be needed when the Qt creator bug is being fixed
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


}

#endif // EFFECT_H
