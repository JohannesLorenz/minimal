/*************************************************************************/
/* mmms - minimal multimedia studio                                      */
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

#ifndef LOADED_PROJECT_H
#define LOADED_PROJECT_H

#include <unistd.h>
//#include <lo/lo_types.h>
#include <boost/heap/fibonacci_heap.hpp>

#include "instrument.h"
#include "project.h"
#include "ports.h"
#include "types.h"
#include "lfo.h"

namespace mmms {

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
	void send_rtosc_msg(const char *path, const char *msg_args, ...)
		const;
	void send_rtosc_str(const rtosc_string &rt_str)
		const;
};

/*class loaded_instrument
{
	rtosc_con make_rtosc_con() const;
public:
	const instrument_t& instrument;
	rtosc_con con;
	loaded_instrument(const instrument_t& instrument) :
		instrument(instrument),
		con(make_rtosc_con())
	{
	}
};*/

class loaded_project_t;

class player_t // TODO: own header
{
	//!< maximum seconds to sleep until wakeup forced
	//!< @deprecated deprecated?
	static constexpr const float max_sleep_time = 0.1;

	float step = 0.1f; //0.001seconds; // suggested by fundamental
	float pos = 0.0f;
	loaded_project_t& project; // TODO! must be const

	std::set<float> end_set = { std::numeric_limits<float>::max() };
	struct pq_entry
	{
		//float next;
		const instrument_t* ins;
		const command_base* cmd;
		const std::set<float>& vals;
		std::set<float>::const_iterator itr;
	};

	struct cmp_func
	{
		bool operator() (const pq_entry& lhs, const pq_entry& rhs) const
		{
			return *lhs.itr > *rhs.itr; // should be <, but we start with small values

		/*	bool left_end = lhs.itr != lhs.vals.end();
			bool right_end = rhs.itr != rhs.vals.end();

			return (right_end && !left_end) ||*/
		}
	};

	typedef boost::heap::fibonacci_heap<pq_entry, boost::heap::compare<cmp_func>> pq_type;
	pq_type pq;

	void update_effects();
	void fill_commands();
	void send_commands();

public:
	player_t(loaded_project_t& project);

	void play_until(float dest);
};

class command_table
{


};

class effect_root_t : public effect {
	void proceed(float ) {}
};

//! this class takes a project and then does some things to handle it
class loaded_project_t : non_copyable_t
{
	// project
	project_t project;

	// connections
	const std::vector<rtosc_con> _cons;
	std::vector<rtosc_con> make_cons() const;

	effect_root_t _effect_root;

//	static mmms::rtosc_con make_rtosc_con(const instrument_t &instrument);

	// commands
//	command_table commands;

	// player
	// player_t player;

	daw_visit::global_map _global;
public:
	const std::vector<rtosc_con>& cons() const { return _cons; }

	daw_visit::global_map& global() { return _global; }
	effect_root_t& effect_root() { return _effect_root; }
	loaded_project_t(project_t&& project);
	~loaded_project_t();
};

}

#endif // LOADED_PROJECT_H
