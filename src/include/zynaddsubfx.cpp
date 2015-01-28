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

#include <unistd.h> // TODO
#include <fstream>
#include <sys/wait.h>

#include "lo_port.h"
#include "config.h"
#include "zynaddsubfx.h"

namespace mini {

std::string zynaddsubfx_t::make_start_command() const
{
	const std::string cmd = ZYN_BINARY
		" --no-gui -O alsa"; // TODO: read from options file
	return cmd;
}

instrument_t::udp_port_t zynaddsubfx_t::get_port(pid_t pid, int) const
{
	udp_port_t port;
	std::string tmp_filename = "/tmp/zynaddsubfx_"
			+ std::to_string(pid);
	std::cout << "Reading " << tmp_filename << std::endl;
	sleep(1); // wait for zyn to be started... (TODO)
	std::ifstream stream(tmp_filename);
	if(!stream.good()) {
			throw "Error: Communication to zynaddsubfx failed.";
		}
	stream >> port;
	return port;
}

zynaddsubfx_t::zynaddsubfx_t(const char *name) :
	zyn::znode_t(this, "/", ""),
	effect_t(name),
	m_impl(this),
	notes_t_port(this, "/", "/noteOn")
{
	set_next_time(std::numeric_limits<float>::max());
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

pid_t zyn_impl::make_fork()
{
	pid_t pid = 0; // TODO: use return value, make pid class with operator bool
	_get_input(ref->make_start_command().c_str(), &pid);
	return pid;
}

zyn_impl::zyn_impl(zynaddsubfx_t *ref) :
	is_impl_of_t<zynaddsubfx_t>::is_impl_of_t(ref),
	pid(make_fork()),
	lo_port(ref->get_port(pid, 0 /*TODO*/))
{
}

zyn_impl::~zyn_impl()
{
	command_base* close_command = make_close_command(); // TODO: auto ptr
	send_single_command(lo_port, close_command->buffer());
	delete close_command;

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
}

command_base *zyn_impl::make_close_command() const
{
	return new command<>("/close-ui");
}

float zyn_impl::proceed(float )
{
	for(in_port_base* ipb : ref->get_in_ports())
	{
		if(ipb==nullptr)
		 throw "OUCH!";
		if(ipb->update())
		{
			std::cerr << "unread changes at: " << ipb << std::endl;
			ipb->on_recv();
			ipb->unread_changes = false;
		}
	}
	return std::numeric_limits<float>::max();
	//return work_queue_t::run_tasks(time);
}

}

