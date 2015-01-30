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

template<class InstClass>
class node_t : public named_t
{
public: // TODO
	InstClass* ins;
// the inheriting class must define the sub-nodes
protected:

	template<class NodeT>
	NodeT* spawn_new(const std::string& ext) const {
		return new NodeT(ins, name(), ext);
	}

	template<class NodeT>
	NodeT spawn(const std::string& ext) const {
		return NodeT(ins, name(), ext);
	}

	template<class NodeT>
	NodeT spawn(const std::string& ext, std::size_t id) const {
		return spawn<NodeT>(ext + std::to_string(id));
	}
public:
	node_t(InstClass* ins, const std::string& base, const std::string& ext)
		: named_t(base + ext), ins(ins) {}



	//node(std::string base, std::string ext, std::size_t id)
	//	: node(base, ext + std::to_string(id)) {}
};

struct prioritized_command_base //: public work_queue_t::task_base
{
	std::size_t priority;
	float next_time;
	prioritized_command_base(std::size_t priority, float next_time) :
		priority(priority),
		next_time(next_time)
	{

	}
};

template<class PortType>
struct prioritized_command : public prioritized_command_base
{
	command<PortType>* cmd; // TODO: does command_base suffice?
	prioritized_command(std::size_t priority, float next_time,
		command<PortType>* cmd) :
		prioritized_command_base(priority, next_time), cmd(cmd)
	{

	}
};

template<class Ins, class PortType>
struct rtosc_in_port : PortType
{
	prioritized_command<rtosc_in_port<Ins, PortType>>* cmd;
	Ins* ins;

	void on_recv() {
		send_single_command(ins->lo_port, cmd->cmd->complete_buffer());
	}

	using PortType::PortType;
};

// TODO: make this a subclass of rtosc_instr and then remove get_impl() ?
template<class InstClass, class PortType>
struct in_port_with_command : node_t<InstClass>
{ // TODO: instrument.h -> ?
	using rtosc_in_port = rtosc_in_port<InstClass, PortType>;

	prioritized_command<rtosc_in_port> cmd;
public:
	in_port_with_command(InstClass* ins, const std::string& base, const std::string& ext) :
		node_t<InstClass>(ins, base, ext),
		cmd(1, 0.0f, new command<rtosc_in_port>((base + ext).c_str(), (effect_t&)*ins))
	{
		cmd.cmd->template port_at<0>().set_trigger();
		cmd.cmd->template port_at<0>().ins = ins;
		cmd.cmd->template port_at<0>().cmd = &cmd;
	}

	rtosc_in_port& port() {
		return cmd.cmd->template port_at<0>();
	}
};

class instrument_t : public effect_t //, protected work_queue_t
{
public:
	lo_port_t lo_port; // TODO: private?
protected:
	pid_t pid; // TODO: private?

	std::vector<command_base*> commands; // TODO: unique?
private:
	pid_t make_fork();
public:
	using udp_port_t = int;
	using effect_t::effect_t;

	void instantiate();

	virtual ~instrument_t();
//	virtual instrument_t* clone() const = 0; // TODO: generic clone class?

	virtual command_base *make_close_command() const = 0;

	virtual std::string make_start_command() const = 0;
	//! shall return the lo port (UDP) after the program was started
	virtual udp_port_t get_port(pid_t pid, int fd) const = 0;

	void clean_up();

	float _proceed(float);
};

template <char ...Letters> class fixed_str {
	static std::string make_str() { return std::string(Letters...); }
};


/*template<class Cmd, class ...Args>
make_cmd()*/


}

#endif // INSTRUMENT_H
