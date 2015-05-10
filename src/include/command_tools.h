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

#ifndef COMMAND_TOOLS_H
#define COMMAND_TOOLS_H

#include "command.h"

namespace mini {

struct prioritized_command_base : public work_queue_t::task_base_with_handle
{
	std::size_t priority;
	prioritized_command_base(std::size_t priority, sample_t ) :
		task_base_with_handle(std::numeric_limits<sample_t>::max()),
		priority(priority)
	{

	}
	bool cmp(const task_base& rhs) const {
		return priority < dynamic_cast<const prioritized_command_base&>(rhs).priority;
	}
};

class prioritized_command : public prioritized_command_base
{
	bool changed = false;
protected:
	minimal_plugin** plugin; // TODO: single pointer would be cool...
public:
	prioritized_command(std::size_t priority, sample_t next_time,
		minimal_plugin** plugin) :
		prioritized_command_base(priority, next_time),
		plugin(plugin)
	{

	}

	void proceed_base(sample_t) { // TODO: call virtual from here?
	//	io::mlog << "PROCEEDING: " << this << io::endl;
		if(!changed)
		 throw "proceeding with unchanged command...";
		changed = false;
	}

	bool set_changed() {
		bool had_effect = (changed == false);
	//	io::mlog << "SETCHANGED: " << this << io::endl;
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
		std::size_t priority, sample_t next_time, minimal_plugin** plugin,
		command_base* cmd) :
		prioritized_command(priority, next_time, plugin),
		w(w),
		cmd(cmd)
		{}

	void proceed(sample_t time)
	{
		proceed_base(time);

		//send_single_command(*lo_port, cmd->complete_buffer());
		(*plugin)->send_osc_cmd(cmd->complete_buffer().raw());

		// TODO: not sure, but max sounds correct:
		update_next_time(std::numeric_limits<sample_t>::max());
		w->update(get_handle());
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

	void on_recv(sample_t time); // see below
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
	static void exec(InsType& ins, CmdType& cmd, minimal_plugin** plugin)
	{
		cmd.cmd->template port_at<I>().set_trigger();
		cmd.cmd->template port_at<I>().ins = ins;
		cmd.cmd->template port_at<I>().cmd = &cmd;
		cmd.cmd->template port_at<I>().plugin = plugin; // TODO: redundant with ctor assignment?

		init_port<N, I+1>(p);
	}
};

template<std::size_t N>
struct init_port<N, N> {
	template<class InsType, class CmdType>
	static void exec(const InsType& , const CmdType& , const minimal_plugin** ) {}
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
struct _in_port_with_command : node_t<void>, util::non_copyable_t
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
		cmd(static_cast<work_queue_t*>(ins), 1, 0.0f, ins->get_plugin_ptr(), cmd_ptr)
	{
		/*cmd.cmd->template port_at<0>().set_trigger();
		cmd.cmd->template port_at<0>().ins = ins;
		cmd.cmd->template port_at<0>().cmd = &cmd;*/
		//init_port<sizeof...(PortTypes)>::exec(ins, cmd, &ins->lo_port); // TODO: ref instead of & ?

		functor_init_ports<InstClass, cmd_type> f{ins, cmd};
		cmd_ptr->for_all_variables(f);

		cmd.set_handle(ins->add_task(&cmd)); // TODO: do this in cmd's ctor? possible??
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
void rtosc_in_port_t<T>::on_recv(sample_t time)
{
	// mark as changed
	if(cmd->set_changed())
	{
		// update in pq
		cmd->update_next_time(time);
		ins->update(cmd->get_handle());
	}
//	send_single_command(ins->lo_port, cmd->cmd->complete_buffer());
}


/*template<class Cmd, class ...Args>
make_cmd()*/

}

#endif // COMMAND_TOOLS_H
