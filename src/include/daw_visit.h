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

#ifndef DAW_VISIT_H
#define DAW_VISIT_H

#include <string>

#include "utils.h"
#include "lo_port.h"
#include "instrument.h"

namespace mini {

// TODO: own header
class rtosc_con : non_copyable_t
{
private:
	static pid_t make_fork(const char *start_cmd);
	rtosc_con(const rtosc_con& other) = delete;
public:
	const pid_t pid;
	const int fd;
	const int port; // TODO: port_t
	const lo_port_t lo_port;

	std::string port_as_str() { return std::to_string(port); }
	~rtosc_con();
	rtosc_con(const instrument_t& ins);
	rtosc_con(rtosc_con&) = delete;
	rtosc_con(rtosc_con&&) = default;
	void send_osc_msg(const char *path, const char *msg_args, ...)
		const;
	void send_osc_str(const osc_string &rt_str)
		const;
};

class loaded_instrument_t : non_copyable_t, public effect_t, protected work_queue_t
{
	static int next_id;
public:
	const instrument_t& ins;
	rtosc_con con;
	int id;
	loaded_instrument_t(const instrument_t& ins)
		: ins(ins), con(ins), id(++next_id)
	{
	}
	bool operator<(const loaded_instrument_t& rhs) const
	{
		return id < rhs.id;
	}

	template<class Cmd>
	class command_task_t : public task_base
	{
		const loaded_instrument_t* ins; // TODO: ref?
		command_base* cmd;

		command_task_t(const loaded_instrument_t* ins,
			command_base* cmd) :
			// TODO: 0 is wrong if we don't start playback at 0
			task_base(0.0f),
			ins(ins),
			cmd(cmd)
		{
		}

		void proceed(float ) {
			if(cmd->update()) // TODO: only fetch values if they changed?
			 ins->con.send_osc_str(cmd->buffer());
			float next_time = cmd->get_next_time();
			update_next_time(next_time);
		}
	};


	virtual float _proceed(float time)
	{
		return run_tasks(time);
	}

};

namespace daw_visit {

	using namespace daw;

	std::pair<note_geom_t, note_t> visit(note_geom_t offset, const note_t& n);

	std::multimap<note_geom_t, note_t> visit(note_geom_t offset, const notes_t& ns);

//	cmd_vectors visit(const track_t& t);

	// rtosc port (via instrument), commands, times
//	using global_map = std::map<const loaded_instrument_t, cmd_vectors>;

//	global_map visit(global_t& g);

}

}

#endif // DAW_VISIT_H
