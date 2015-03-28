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
#include <memory>
#include <iostream> // TODO
#include <map>
#include <set>
#include <initializer_list>
// ...
// when is it going to end?

#include "types.h"
#include "utils.h"
#include "daw.h"
#include "command.h"
#include "effect.h"
#include "work_queue.h"
#include "lo_port.h"


namespace mini
{

void send_single_command(lo_port_t& lp, const osc_string& str);

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
	void print_all_used(std::ostream& os = std::cerr) const
	{
		os << name() << std::endl;
		for(const auto& pr : used_ch) {
			pr.second->print_all_used(os);
		}
	}

	virtual void on_preinit() {} // TODO: priv
	void preinit() {
		on_preinit();
		for(const auto& pr : used_ch) {
			pr.second->on_preinit();
		}
	}
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
		std::cerr << "ADDING? " << ext << (used_ch.find(ext) == used_ch.end()) << std::endl;
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
public:
	lo_port_t lo_port; // TODO: private?
protected:
	pid_t pid; // TODO: private?
private:
	std::vector<const command_base*> const_commands;

	const std::vector<bool>* cp;
	pid_t make_fork();
public:
	using udp_port_t = int;
	using effect_t::effect_t;

	void instantiate();

	instrument_t(const char* name, std::initializer_list<const command_base*> const_commands);
	void add_const_command(const command_base& cmd) {
		const_commands.push_back(&cmd);
	}

	virtual ~instrument_t();

	void pass_changed_ports(const std::vector<bool>& _cp)
	{ // TODO: no vector bool -> stack of ints
		cp = &_cp;
	}

//	virtual instrument_t* clone() const = 0; // TODO: generic clone class?

	virtual command_base *make_close_command() const = 0;

	virtual std::string make_start_command() const = 0;
	//! shall return the lo port (UDP) after the program was started
	virtual udp_port_t get_port(pid_t pid, int fd) const = 0;

	void clean_up();

	float _proceed(float time);
};






struct prioritized_command_base : public work_queue_t::task_base
{
	std::size_t priority;
	work_queue_t::handle_type handle;
	prioritized_command_base(std::size_t priority, float ) : // TODO: float
		task_base(std::numeric_limits<float>::max()),
		priority(priority)
	{

	}
	bool cmp(const task_base& rhs) const {
		return priority < dynamic_cast<const prioritized_command_base&>(rhs).priority;
	}
	work_queue_t::handle_type& get_handle() { return handle; }
};

class prioritized_command : public prioritized_command_base
{
	bool changed = false;
protected:
	lo_port_t* lo_port;
public:
	prioritized_command(std::size_t priority, float next_time,
		lo_port_t* lo_port) :
		prioritized_command_base(priority, next_time),
		lo_port(lo_port)
	{

	}

	void proceed_base(float) { // TODO: call virtual from here?
	//	std::cerr << "PROCEEDING: " << this << std::endl;
		if(!changed)
		 throw "proceeding with unchanged command...";
		changed = false;
	}

	bool set_changed() {
		bool had_effect = (changed == false);
	//	std::cerr << "SETCHANGED: " << this << std::endl;
		changed = true;
		return had_effect;
	}
};

//! for single command stuff
class prioritized_command_cmd : public prioritized_command
{
	work_queue_t* w;
public:
	command_base* cmd; // TODO: does command_base suffice?
	prioritized_command_cmd(work_queue_t* w,
		std::size_t priority, float next_time, lo_port_t* lo_port,
		command_base* cmd) :
		prioritized_command(priority, next_time, lo_port),
		w(w),
		cmd(cmd)
		{}

	void proceed(float time)
	{
		proceed_base(time);
		send_single_command(*lo_port, cmd->complete_buffer());

		// TODO: not sure, but max sounds correct:
		update_next_time(std::numeric_limits<float>::max());
		w->update(handle);
	}
};

template<class PortType>
struct rtosc_in_port_t : PortType
{
	prioritized_command_cmd* cmd;
	instrument_t* ins;

	using PortType::PortType;
	rtosc_in_port_t(PortType&& arg)
		: PortType(std::move(arg))
	{
	}

	void on_recv(float time); // see below
//	rtosc_in_port_t(effect_t& ef) : PortType(ef) {}
};

/*
template<class Inst, class ...OtherTypes>
struct rtosc_in_port_t<Inst, int, OtherTypes...> // TODO: int
{
	int val;
	operator int() { return val; }
};*/

// welcome to teplate-hell...

template<class T, bool, class Ins> // true
struct _type_of_rtosc_port {
	using type = rtosc_in_port_t<T>;
};

template<class T, class Ins>
struct _type_of_rtosc_port<T, false, Ins> {
	using type = T;
};

template<class T, class Ins, class ...Other>
using type_of_rtosc_port = typename _type_of_rtosc_port<T, _is_variable<T>(), Ins>::type;

template<std::size_t N, std::size_t I = 0>
struct init_port {
	template<class InsType, class CmdType>
	static void exec(InsType& ins, CmdType& cmd, lo_port_t* lo_port)
	{
		cmd.cmd->template port_at<I>().set_trigger();
		cmd.cmd->template port_at<I>().ins = ins;
		cmd.cmd->template port_at<I>().cmd = &cmd;
		cmd.cmd->template port_at<I>().lo_port = lo_port;

		init_port<N, I+1>(p);
	}
};

template<std::size_t N>
struct init_port<N, N> {
	template<class InsType, class CmdType>
	static void exec(const InsType& , const CmdType& , const lo_port_t* ) {}
};

template<class Ins, class Cmd>
struct functor_init_ports
{
	Ins* ins;
	Cmd& cmd;
	template<class ...Args> void operator()(rtosc_in_port_t<Args...>& p)
	{
		p.set_trigger();
		p.ins = ins;
		p.cmd = &cmd;
	}
};

// TODO: make this a subclass of rtosc_instr and then remove get_impl() ?
// TODO: make InstClass = effect_t? ???????????????????????????????????????????
template<class /*InstClass*/, class... PortTypes>
struct _in_port_with_command : node_t<void>, non_copyable_t
{ // TODO: instrument.h -> ?

	//using rtosc_in_ports = rtosc_in_port<PortTypes>;

	using cmd_type = prioritized_command_cmd;
	command<PortTypes...>* cmd_ptr;
	prioritized_command_cmd cmd;

	using InstClass = instrument_t;

public:
	template<class ...Args2>
	_in_port_with_command(InstClass* ins, const std::string& base, const std::string& ext, Args2&&... args) :
		node_t<void>(ins, base, ext),
		cmd_ptr(new command<PortTypes...>((base + ext).c_str(), std::forward<Args2>(args)...)),
		cmd(static_cast<work_queue_t*>(ins), 1, 0.0f, &ins->lo_port, cmd_ptr)
	{
		/*cmd.cmd->template port_at<0>().set_trigger();
		cmd.cmd->template port_at<0>().ins = ins;
		cmd.cmd->template port_at<0>().cmd = &cmd;*/
		//init_port<sizeof...(PortTypes)>::exec(ins, cmd, &ins->lo_port); // TODO: ref instead of & ?

		functor_init_ports<InstClass, cmd_type> f{ins, cmd};
		cmd_ptr->for_all_variables(f);

		cmd.handle = ins->add_task(&cmd);
	}

	//! should only be called if all args are ports
	//! otherwise, one has to initialize the fixed args on one's own
	_in_port_with_command(InstClass* ins, const std::string& base, const std::string& ext) :
		_in_port_with_command(ins, base, ext, port_ctor<PortTypes>(ins)...)
	{
	}

	/*template<std::size_t I>
	auto port_at() -> decltype(cmd.cmd->port_at<I>()) {
		return cmd.cmd->port_at<I>();
	}*/
};

template<class InstClass, class... PortTypes>
struct in_port_with_command : _in_port_with_command<InstClass, type_of_rtosc_port<PortTypes, InstClass>...>
{
//	using base = _in_port_with_command<InstClass, type_of_rtosc_port<PortTypes, InstClass>...>;
//	using base::_in_port_with_command;
	using _in_port_with_command<InstClass, type_of_rtosc_port<PortTypes, InstClass>...>::
		_in_port_with_command;
};

template <char ...Letters> class fixed_str {
	static std::string make_str() { return std::string(Letters...); }
};

template<class T>
void rtosc_in_port_t<T>::on_recv(float time)
{
	// mark as changed
	if(cmd->set_changed())
	{
		// update in pq
		cmd->update_next_time(time);
		ins->update(cmd->handle);
	}
//	send_single_command(ins->lo_port, cmd->cmd->complete_buffer());
}


/*template<class Cmd, class ...Args>
make_cmd()*/


}

#endif // INSTRUMENT_H
