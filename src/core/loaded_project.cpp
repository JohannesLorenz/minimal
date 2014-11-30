/*************************************************************************/
/* minimal - a minimal rtosc sequencer                                   */
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

#include <iostream>
#include <cstdlib>
#include <cstdarg>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stack>

#include "loaded_project.h"

// TODO: move this out?
bool get_input(const char* shell_command, pid_t* _childs_pid)
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



/*mini::rtosc_con mini::loaded_project::make_rtosc_con(
	const instrument_t& instrument)
{
// TODO: move to rtosc_con ctor?
	rtosc_con con;
	get_input(instrument.make_start_command().c_str(), &con.pid);
	con.fd = 0; // TODO
	con.port = instrument.get_port(con.pid, con.fd);
	return con;
}*/


std::vector<mini::rtosc_con> mini::loaded_project_t::make_cons() const
{
	std::vector<mini::rtosc_con> result;
	for(const std::unique_ptr<instrument_t>& ins : project.instruments())
	{
		result.emplace_back(*ins);
	}
	return result;
}

mini::loaded_project_t::loaded_project_t(mini::project_t&& project) :
	project(std::move(project)),
	_cons(std::move(make_cons())),
	_global(daw_visit::visit(project.global()))
{
	for(effect* e : project.effects()) // TODO: -> initializer list
	{
		if(e->writers.empty())
		{
			_effect_root.readers.push_back(e);
			e->writers.push_back(&_effect_root);
		}
	}
}




mini::loaded_project_t::~loaded_project_t()
{
	for(std::size_t i = 0; i < _cons.size(); ++i)
	{
		const auto& quit_commands = project.instruments()[i]->quit_commands();
		for(std::size_t j = 0; j < quit_commands.size(); ++j)
		{
			std::cout << "???" << std::endl;
		//	dump d;
		//	quit_commands[j]->execute(d);


		//	std::cout << quit_commands[j]->casted().arg(0) << std::endl;
		//	quit_commands[j]->call(cons[i].send_rtosc_msg); // TODO: without c_str()?
		}
	}
}


pid_t mini::rtosc_con::make_fork(const char* start_cmd)
{
	pid_t pid = 0; // TODO: use return value, make pid class with operator bool
	get_input(start_cmd, &pid);
	return pid;
}

mini::rtosc_con::~rtosc_con()
{
	//sleep(2); // TODO
//	kill(pid, SIGTERM);
//	lo_port.send_rtosc_msg("/noteOn", "ccc", 0, 54, 20);
	std::cerr << "Closing zasf now... " << std::endl;
	lo_port.send_rtosc_msg("/close-ui", "");
	sleep(2);
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
	sleep(3);
}

mini::rtosc_con::rtosc_con(const instrument_t &ins) :
	pid(make_fork(ins.make_start_command().c_str())),
	fd(0), // TODO
	port(ins.get_port(pid, fd)),
	lo_port(port_as_str().c_str())
{
}

void mini::rtosc_con::send_rtosc_msg(const char *path, const char *msg_args, ...)
const {
	va_list argptr;
	va_start(argptr, msg_args);
	bool ret = lo_port.send_rtosc_msg(path, msg_args, argptr);
	va_end(argptr);
	//return ret;
	(void)ret; // :-(
}

void mini::rtosc_con::send_rtosc_str(const rtosc_string& rt_str) const
{
	lo_port.send_raw(rt_str.raw(), rt_str.size());
}


void mini::player_t::update_effects()
{
	std::stack<effect*> ready_fx;
	ready_fx.push(&project.effect_root());
	do
	{
		effect* cur_effect = ready_fx.top();
		ready_fx.pop();

		cur_effect->proceed(pos);

		for(effect* next: cur_effect->readers)
		 ready_fx.push(next);

	} while(ready_fx.size());
}

void mini::player_t::fill_commands()
{
	for(const auto& pr : project.global())
	for(const auto& pr2 : pr.second)
	{
		pr2.first->complete_buffer();
	}
}

void mini::player_t::send_commands()
{

}

mini::player_t::player_t(mini::loaded_project_t &project)  : project(project)
{
	for(const auto& pr : project.global())
	for(const auto& pr2 : pr.second)
	{
		pq.push({pr.first, pr2.first, pr2.second, pr2.second.begin()});
		std::cerr << "pushing: " <<  *pr2.second.begin() << std::endl;
	}
	pq.push({nullptr, nullptr, end_set, end_set.begin()});
}

void mini::player_t::play_until(float dest)
{
	for(; pos < dest; pos += step)
	{
		update_effects();
		fill_commands();
	//	send_commands();
		std::cerr << "pos:" << pos << std::endl;
		while(*pq.top().itr <= pos)
		{


			pq_entry top = std::move(pq.top());
			pq.pop();

			if(top.itr == top.vals.end())
			 throw "End";

			std::cerr << pos << ": Next: " << *top.itr << std::endl;
			std::cerr << pos << ": Would send: " << top.cmd->complete_buffer() << std::endl;
			//project.cons()[top.ins]

			project.cons().front().send_rtosc_str(top.cmd->buffer());

			++top.itr;
			if(top.itr != top.vals.end())
			{
				pq.push(std::move(top));
			}
			else
			{
				throw "found end itr, missing sentinel?";
			}


		//	if(top.itr == top.vals.end())
		//	 throw "end";

			//pq.decrease(pq.top());

			/*if(top().itr == top().vals.end())
			 pq.insert
			pq.pop();*/
		}
		std::cerr << "done: " << pos << std::endl;
		usleep(1000000 * step);
	}
}
