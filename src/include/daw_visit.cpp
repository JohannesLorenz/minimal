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

#include <cstdarg>
#include <unistd.h>
#include <sys/wait.h>

#include "daw_visit.h"

namespace mini {

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

pid_t rtosc_con::make_fork(const char* start_cmd)
{
	pid_t pid = 0; // TODO: use return value, make pid class with operator bool
	get_input(start_cmd, &pid);
	return pid;
}

rtosc_con::~rtosc_con()
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

rtosc_con::rtosc_con(const instrument_t &ins) :
	pid(make_fork(ins.make_start_command().c_str())),
	fd(0), // TODO
	port(ins.get_port(pid, fd)),
	lo_port(port_as_str().c_str())
{
}

void rtosc_con::send_osc_msg(const char *path, const char *msg_args, ...)
const {
	va_list argptr;
	va_start(argptr, msg_args);
	bool ret = lo_port.send_rtosc_msg(path, msg_args, argptr);
	va_end(argptr);
	//return ret;
	(void)ret; // :-(
}

void rtosc_con::send_osc_str(const osc_string& rt_str) const
{
	lo_port.send_raw(rt_str.raw(), rt_str.size());
}

int loaded_instrument_t::next_id = 0;

namespace daw_visit {

	std::pair<note_geom_t, note_t> visit(note_geom_t offset, const note_t &n)
	{
		std::cerr << "Adding note, offset: " << offset.offs << ", start: " << offset.start << std::endl;
		return std::make_pair(offset, n);
	}

	std::multimap<note_geom_t, note_t> visit(note_geom_t offset, const notes_t &ns)
	{
		// TODO: can we not use vectors? -> linear time
		note_geom_t abs_offs = ns.geom + offset;
		std::multimap<note_geom_t, note_t> res;
		for(const notes_t::pair_t<note_t>& p : ns.get<note_t>())
		{
			//	std::pair<note_geom_t, note_t> p = visit(n);
			res.insert(visit(abs_offs + p.first, *p.second));
		}
		for(const notes_t::pair_t<notes_t>& ns2 : ns.get<notes_t>())
		{
			std::multimap<note_geom_t, note_t> res2 = visit(abs_offs, *ns2.second);
			std::move(res2.begin(), res2.end(), std::inserter(res, res.begin()));
		}
		std::cerr << "Added " << res.size() << " notes." << std::endl;
		return res;
	}

#if 0
	cmd_vectors visit(const track_t &t)
	{
		cmd_vectors result;
		std::cerr << "sz: " << t.get<notes_t>().size() << std::endl;

		for(const auto& pr : t.get<notes_t>())
		{
			std::multimap<note_geom_t, note_t> mm = visit(t.geom + pr.first, *pr.second);
			cmd_vectors note_commands =
					t.instrument()->make_note_commands(mm);
			for(auto& pr : note_commands) {
				pr.second.insert(std::numeric_limits<float>::max()); // sentinel
			}
			std::cerr << "Added " << note_commands.size() << " note commands to track." << std::endl;

			for(auto& pr : note_commands)
			{
				auto itr = result.find(pr.first);
				if(itr == result.end())
					result.emplace(pr.first, pr.second);
				else
					std::move(pr.second.begin(), pr.second.end(), // TODO: use move!
						  std::inserter(itr->second, itr->second.end()));
				// dynamic_cast<activator_events*>(itr->second)->move_from(dynamic_cast<activator_events*>(pr.second));
			}

			//std::move(note_commands.begin(), note_commands.end(),
			//	std::inserter(result, result.end()));
		}
		//for(t.get<auto_t>) // automation tracks...

		for(const auto& pr : t.get<command_base>())
		{
			result.emplace(pr.second, std::set<float>{});
		}

		std::cerr << "Added track with " << result.size() << " note commands." << std::endl;
		return result;
	}

	global_map visit(global_t &g)
	{
		global_map res;
		std::cerr << "sz0: " << g.get<track_t>().size() << std::endl;

		for(const auto& pr : g.get<track_t>())
		{
			const track_t& t = *pr.second;
			const instrument_t* ins = t.instrument(); // TODO: should t.instrument ret ref?

			//cmd_vectors v = std::make_pair(&t, visit(t));
			cmd_vectors _v = visit(t);

			using cmd_pair = std::pair<const command_base*, std::set<float>>;

			res.emplace/*_hint*/(/*res.end(),*/ *ins, _v);

			for(cmd_pair pr : _v)
			{
				//	auto ins_itr = res.find(ins);
				//	if(ins_itr == res.end())
				{
					// simply insert it
				//	cmd_vectors new_map { std::make_pair(pr.first, /*std::move*/(pr.second)) };
					//res.emplace_hint(/*res.end(),*/ *ins, new_map);
				//	itr->second
				}
#if 0
				else
				{
					// TODO : deprecated
					throw "deprecated: use two notes_t objects instead";

					auto vt = pr;
					//for(const cmd_vectors::value_type vt : v)
					{
						const command_base& cmd = *vt.first;
						std::set<float> & vals = vt.second;

						// if instrument *and* command are equal,
						// add the set to the known command

						auto ins_cmd = ins_itr->second.find(&cmd);
						if(ins_cmd == ins_itr->second.end())
							{
								ins_itr->second.emplace(&cmd, vals);
							}
						else
							{
								std::set<float>& vals_existing = ins_cmd->second;
								vals_existing.insert(vals.begin(), vals.end());
								/*	activator_events* vals_existing = dynamic_cast<activator_events*>(ins_cmd->second);
					//vals_existing.insert(vals.begin(), vals.end());
					vals_existing->move_from(dynamic_cast<activator_events*>(vals)); // TODO: is move ok here?*/
							}
					}
				}
#endif
			}

				// TODO: geometry of t is unused here?
		}
		/*for(const global_t& g : g.get<global_t>())
		{
			res.insert(std::make_pair(t.id(), visit(t)));
		}*/ // TODO


		/*	for(const auto& pr : res)
		{
			std::cerr << "Summary: contents of instrument " << pr.first->name() << ": " << std::endl;
			for(const auto& pr2 : pr.second)
			{
				std::cerr << " - track: " << pr2.first->buffer() << std::endl;
				for(const float& f : pr2.second)
				 std::cerr << "  * note at: " << f << std::endl;
			}
		}*/

		return res;
	}
#endif


#if 0

	cmd_vectors visit(const track_t &t)
	{
		cmd_vectors result;
		std::cerr << "sz: " << t.get<notes_t>().size() << std::endl;

		if(t.get<notes_t>().size() > 1)
		 throw "depreceated: 2 notes_t in one track. add them to 1 notes_t.";

		for(const auto& pr : t.get<notes_t>())
		{
		//	std::multimap<note_geom_t, note_t> mm = visit(t.geom + pr.first, *pr.second);
			note_line_t nlt(visit(t.geom + pr.first, *pr.second));
			// TODO: create loaded_instrument, connect it, make note_commands in there

			cmd_vectors note_commands =
					t.instrument()->make_note_commands(mm);
			for(auto& pr : note_commands) {
				pr.second.insert(std::numeric_limits<float>::max()); // sentinel
			}
			std::cerr << "Added " << note_commands.size() << " note commands to track." << std::endl;

			for(auto& pr : note_commands)
			{
				auto itr = result.find(pr.first);
				if(itr == result.end())
					result.emplace(pr.first, pr.second);
				else
					std::move(pr.second.begin(), pr.second.end(), // TODO: use move!
						  std::inserter(itr->second, itr->second.end()));
			}

		}

	/*	for(const auto& pr : t.get<command_base>())
		{
			result.emplace(pr.second, std::set<float>{});
		}
*/
		std::cerr << "Added track with " << result.size() << " note commands." << std::endl;
		return result;
	}

	global_map visit(global_t &g)
	{
		global_map res;
		std::cerr << "sz0: " << g.get<track_t>().size() << std::endl;

		for(const auto& pr : g.get<track_t>())
		{
			const track_t& t = *pr.second;
			const instrument_t* ins = t.instrument(); // TODO: should t.instrument ret ref?

			//cmd_vectors v = std::make_pair(&t, visit(t));
			cmd_vectors _v = visit(t);

			using cmd_pair = std::pair<const command_base*, std::set<float>>;

			// creation of loaded_istrument_t
			res.emplace/*_hint*/(/*res.end(),*/ *ins, _v);

				// TODO: geometry of t is unused here?
		}

		return res;
	}


#endif






}

}
