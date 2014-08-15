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
	rtosc_con(rtosc_con&&) = default;
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

//! this class takes a project and then does some things to handle it
class loaded_project_t : non_copyable_t
{
	project_t project;
	const std::vector<rtosc_con> cons;
	std::vector<rtosc_con> make_cons() const;
//	static mmms::rtosc_con make_rtosc_con(const instrument_t &instrument);
public:
	loaded_project_t(project_t&& project);
};

class player // TODO: own header
{
	//!< maximum seconds to sleep until wakeup forced
	static constexpr const float max_sleep_time = 0.1;
	float pos = 0.0f;
	const loaded_project_t& project;

	/*struct pq_entry
	{

	};

	struct cmp_func
	{
		bool operator() (const pq_entry* lhs, const pq_entry* rhs) const
		{
			return lhs->area < rhs->area;
		}
	};

	typedef boost::heap::fibonacci_heap<pq_entry, boost::heap::compare<cmp_func>> pq_type;*/

public:
	player(const loaded_project_t& project) : project(project)
	{

	}

	void play_until(float dest);
};

}

#endif // LOADED_PROJECT_H
