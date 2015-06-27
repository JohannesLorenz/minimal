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

#include "command.h"
#include "instrument.h"
#include "io.h"
#include "mports.h"

namespace mini {

instrument_t::~instrument_t()
{
}

bool _get_input(const char* shell_command, pid_t* _childs_pid)
{
	int pipefd[2];
	pid_t childs_pid;

	if (pipe(pipefd) == -1) {
		no_rt::mlog << "pipe() failed -> no zyn" << std::endl;
		return false;
	}

//	fcntl(pipefd[0], F_SETFL, O_NONBLOCK); // ?????

	// fork sh
	childs_pid=fork();
	if(childs_pid < 0) {
		no_rt::mlog << "fork() failed -> no zyn" << std::endl;
		return false;
	}
	else if(childs_pid == 0) {

		close(pipefd[0]); /* Close unused read end */

		dup2(pipefd[1], STDOUT_FILENO);

		execlp("/bin/sh", "sh"	, "-c", shell_command, NULL);

		close(pipefd[1]); /* Reader will see EOF */
		exit(0);
	}

	close(pipefd[1]); /* Close unused write end */
	dup2(pipefd[0], STDIN_FILENO);

	if(_childs_pid)
	 *_childs_pid = childs_pid;
	return true;
}

void instrument_t::instantiate()
{
//	on_preinit();
	instantiate_first();

	set_next_time(std::numeric_limits<sample_no_t>::max());

	for(const command_base* cmd : const_commands)
	 /*plugin->*/send_osc_cmd(cmd->buffer().raw());

	init_2();
}

instrument_t::instrument_t(const char *name, std::initializer_list<const command_base *> const_commands) :
	effect_t(name),
	const_commands(const_commands)
{
}

void instrument_t::add_const_command(const command_base &cmd) {
	const_commands.push_back(&cmd);
}

void instrument_t::clean_up()
{
	{
		command_base* close_command = make_close_command(); // TODO: auto ptr
		/*plugin->*/send_osc_cmd(close_command->buffer().raw());
		delete close_command;
	}

	no_rt::mlog << "zasf should be closed now... " << std::endl;

	for(const command_base*& cb : const_commands)
	 delete cb;
}

bool instrument_t::_proceed()
{
	if(work_queue_t::has_active_tasks(time()))
	 throw "should not have active tasks yet";

	// read all changed ports to put new commands on queue
	for(std::size_t i = 0; i < cp->size(); ++i)
	if((*cp)[i])
	{
		//io::mlog << "in port: " << i << io::endl;
		if(in_ports[i]->update())
		{
			in_ports[i]->on_read(time());
		}
	}

	// send changed osc parameters
	// e.g. run all tasks at this time
	work_queue_t::run_tasks_keep(time());

	// actually do the work
	//return plugin->proceed(samples);
	return advance();

//	// all ports are triggers, so sleep
//	return std::numeric_limits<sample_no_t>::max();

	//return time + 0.1f; // TODO??????????????????????????????
//	return work_queue_t::run_tasks(time);
/*	for(in_port_base* ipb : get_in_ports())
	{
		if(ipb==nullptr)
		 throw "OUCH!";
		if(ipb->update())
		{
			no_rt::mlog << "unread changes at: " << ipb << std::endl;
			ipb->on_read(time);
			ipb->unread_changes = false;
		}
	}
	return std::numeric_limits<sample_no_t>::max();
	//return work_queue_t::run_tasks(time);*/
}

void node_t_base::print_all_used(std::ostream &os) const
{
	os << name() << std::endl;
	for(const auto& pr : used_ch) {
			pr.second->print_all_used(os);
		}
}

void node_t_base::preinit() {
	on_preinit();
	for(const auto& pr : used_ch) {
			pr.second->on_preinit();
		}
}

#if 0
void plugin_instrument::instantiate_first()
{
	/*	const std::vector<const char*> cmds = build_start_args();
	typedef char* argv_t[];
	plugin.call<int, int, argv_t>("main", cmds.size(), cmds.data());
	*/
	set_path("TODO");
	call<minimal_plugin*, unsigned long>("instantiate", 1024);
	if(plugin == nullptr)
	 throw "plugin is null, could not be loaded";
	plugin->prepare();
	return 0;
}
#endif

/*instrument_t *instrument_t::clone() const
{
	instrument_t* result = new instrument_t();
	result->next_id = next_id;
	for(const command_base* cmd : commands)
	 result->commands.push_back(cmd->clone());
	return result;
}*/

}

