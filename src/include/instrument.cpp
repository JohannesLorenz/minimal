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

#include <sys/wait.h>
#include <iostream> // TODO
#include "lo_port.h"
#include "instrument.h"

namespace mini {

void send_single_command(lo_port_t& lo_port, const osc_string &str)
{
	lo_port.send_raw(str.raw(), str.size());
}

instrument_t::~instrument_t()
{
}

bool _get_input(const char* shell_command, pid_t* _childs_pid)
{
	int pipefd[2];
	pid_t childs_pid;

	if (pipe(pipefd) == -1) {
		std::cerr << "pipe() failed -> no zyn" << std::endl;
		return false;
	}

//	fcntl(pipefd[0], F_SETFL, O_NONBLOCK); // ?????

	// fork sh
	childs_pid=fork();
	if(childs_pid < 0) {
		std::cerr << "fork() failed -> no zyn" << std::endl;
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

pid_t instrument_t::make_fork()
{
	pid_t pid = 0; // TODO: use return value, make pid class with operator bool
	_get_input(make_start_command().c_str(), &pid);
	return pid;
}

void instrument_t::instantiate()
{
//	on_preinit();

	pid = make_fork();
	lo_port.init(get_port(pid, 0 /*TODO*/));

	// TODO: init in effect ctor? (probably not doable in every case)
	set_next_time(std::numeric_limits<float>::max());

	for(const command_base* cmd : const_commands)
	 send_single_command(lo_port, cmd->buffer());
}

instrument_t::instrument_t(const char *name, std::initializer_list<const command_base *> const_commands) :
	effect_t(name),
	const_commands(const_commands)
{
}

void instrument_t::clean_up()
{
	{
		command_base* close_command = make_close_command(); // TODO: auto ptr
		send_single_command(lo_port, close_command->buffer());
		delete close_command;
	}

	std::cerr << "zasf should be closed now... " << std::endl;
	// TODO: kill() if this did not work

	int status;
	while (-1 == waitpid(pid, &status, 0)) {
		puts("...");
		usleep(10000); // TODO: what is a good value here?
	}
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
		std::cerr << "Process (pid " << pid << ") failed" << std::endl;
		exit(1);
	}


	//std::cout << "destroying instrument: " << name() << std::endl;
	for(const command_base*& cb : const_commands)
	{
	//	std::cout << name() << ": deleting " << cb->path() << std::endl;
		delete cb;
	}
}

float instrument_t::_proceed(float time)
{
	if(work_queue_t::has_active_tasks(time))
	 throw "should not have active tasks yet";

	// read all changed ports to put new commands on queue
	for(std::size_t i = 0; i < cp->size(); ++i)
	if((*cp)[i])
	{
		std::cerr << "in port: " << i << std::endl;
		if(in_ports[i]->update())
		{
			in_ports[i]->on_recv(time);
		}
	}

	// execute new commands
	work_queue_t::run_tasks_keep(time);

	// all ports are triggers, so sleep
	return std::numeric_limits<float>::max();

	//return time + 0.1f; // TODO??????????????????????????????
//	return work_queue_t::run_tasks(time);
/*	for(in_port_base* ipb : get_in_ports())
	{
		if(ipb==nullptr)
		 throw "OUCH!";
		if(ipb->update())
		{
			std::cerr << "unread changes at: " << ipb << std::endl;
			ipb->on_recv(time);
			ipb->unread_changes = false;
		}
	}
	return std::numeric_limits<float>::max();
	//return work_queue_t::run_tasks(time);*/
}

/*instrument_t *instrument_t::clone() const
{
	instrument_t* result = new instrument_t();
	result->next_id = next_id;
	for(const command_base* cmd : commands)
	 result->commands.push_back(cmd->clone());
	return result;
}*/

}

