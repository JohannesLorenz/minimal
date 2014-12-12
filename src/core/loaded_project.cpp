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

#include <iostream>
#include <cstdlib>
#include <cstdarg>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stack>

#include "loaded_project.h"

namespace mini
{


/*rtosc_con loaded_project::make_rtosc_con(
	const instrument_t& instrument)
{
// TODO: move to rtosc_con ctor?
	rtosc_con con;
	get_input(instrument.make_start_command().c_str(), &con.pid);
	con.fd = 0; // TODO
	con.port = instrument.get_port(con.pid, con.fd);
	return con;
}*/


std::vector<loaded_instrument_t> loaded_project_t::make_ins() const
{
	std::vector<loaded_instrument_t> result;
/*	for(const std::unique_ptr<instrument_t>& ins : project.instruments())
	{
		result.emplace_back(*ins);
	}*/
	return result;
}

loaded_project_t::loaded_project_t(project_t&& project) :
	project(std::move(project)),
	_ins(std::move(make_ins())),
	_global(daw_visit::visit(project.global()))
{
	for(effect_t* e : project.effects()) // TODO: -> initializer list
	{
		if(e->writers.empty())
		{
			_effect_root.readers.push_back(e);
			e->writers.push_back(&_effect_root);
		}
	}
}




loaded_project_t::~loaded_project_t()
{
	for(std::size_t i = 0; i < _ins.size(); ++i)
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

void player_t::update_effects()
{
	std::stack<effect_t*> ready_fx;
	ready_fx.push(&project.effect_root());
	do
	{
		effect_t* cur_effect = ready_fx.top();
		ready_fx.pop();

		cur_effect->proceed(pos);

		for(effect_t* next: cur_effect->readers)
		 ready_fx.push(next);

	} while(ready_fx.size());
}

void player_t::fill_commands()
{
	for(const auto& pr : project.global())
	for(const auto& pr2 : pr.second)
	{
		pr2.first->complete_buffer();
	}
}

void player_t::send_commands()
{

}

player_t::player_t(loaded_project_t &project)  : project(project)
{
	for(const auto& pr : project.global())
	for(const auto& pr2 : pr.second)
	if(pr2.second.empty()) // marks a poll
	{
		//pq.push(new task_poll());
	}
	else
	{
		pq.push(new task_events(&pr.first, pr2.first, pr2.second.begin()));
	//	std::cerr << "pushing: " <<  *pr2.second.begin() << std::endl;
	}
	pq.push(new task_events(nullptr, nullptr, end_set.begin())); // = sentinel
}

void player_t::play_until(float dest)
{
	for(; pos < dest; pos += step)
	{
		update_effects();
		fill_commands();
	//	send_commands();
		std::cerr << "pos:" << pos << std::endl;
		while(pq.top()->next_time() <= pos)
		{

			pq_entry top = std::move(pq.top());
			pq.pop();


			/*const bool reinsert = top->proceed(pos);
			if(reinsert)
			 pq.push(top);*/
			top->proceed(pos); // will update the next-time event
			pq.push(top);


#if 0
			pq_entry top = std::move(pq.top());
			pq.pop();
 // TODO !!! ??
/* auto x4 = dynamic_cast<const activator_events*>(top.activator);
auto x3 = dynamic_cast<const activator_events_itr*>(top.itr)->itr;

			if(x3 == x4->events.end())
			 throw "End";*/


//			std::cerr << pos << ": Next: " << *top.itr << std::endl;
//			std::cerr << pos << ": Would send: " << top.cmd->complete_buffer() << std::endl;
			//project.cons()[top.ins]

			project.cons().front().send_osc_str(top.cmd->buffer());


			top.itr->operator ++();

/*			if(!top.activator)
			 throw "TACTIV";
			if(!top.itr)
			 throw "TOPITR";*/

			pq.push(std::move(top));

#endif
/*
auto x2 = dynamic_cast<const activator_events*>(top.activator);
auto x1 = dynamic_cast<const activator_events_itr*>(top.itr)->itr;
			if(x1
			!= x2->events.end())
		//	if(top.itr != top.activator.end())
			{
				pq.push(std::move(top));
			}
			else
			{
				throw "found end itr, missing sentinel?";
			}*/


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


void player_t::task_events::proceed(float)
{

	// TODO !!! ??
	/* auto x4 = dynamic_cast<const activator_events*>(top.activator);
auto x3 = dynamic_cast<const activator_events_itr*>(top.itr)->itr;

			if(x3 == x4->events.end())
			 throw "End";*/


	//			std::cerr << pos << ": Next: " << *top.itr << std::endl;
	//			std::cerr << pos << ": Would send: " << top.cmd->complete_buffer() << std::endl;
	//project.cons()[top.ins]

	if(!cmd)
	 throw "CMD";
	ins->con.send_osc_str(cmd->buffer());
	update_next_time(*++itr);

}

}
