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


#if 0
std::vector<loaded_instrument_t> loaded_project_t::make_ins() const
{
	std::vector<loaded_instrument_t> result;
/*	for(const std::unique_ptr<instrument_t>& ins : project.instruments())
	{
		result.emplace_back(*ins);
	}*/
	return result;
}
#endif

loaded_project_t::loaded_project_t(project_t&& _project) :
	project(std::move(_project))
	//_ins(std::move(make_ins()))
//	_global(daw_visit::visit(project.global()))
{
	std::cerr << "Loading project: " << this->project.title() << std::endl;
	// instantiate and connect all fx
	for(effect_t* e : project.effects()) // TODO: -> initializer list
	{
		e->instantiate();
		if(e->writers.empty())
		{
			_effect_root.readers.push_back(e);
			e->writers.push_back(&_effect_root);
		}
	}

	// identify all fx
	std::size_t next_id = 0;

	std::stack<effect_t*> ready_fx;
	ready_fx.push(&effect_root());
	do
	{
		effect_t* cur_effect = ready_fx.top();
		ready_fx.pop();

		if(cur_effect->id() != has_id::no_id())
		 throw "Id given twice";
		cur_effect->set_id(next_id++);

		std::cerr << "set id: " << next_id - 1 << " for " << cur_effect << std::endl;

		const auto cb = [&](const std::vector<effect_t*>& next_vector)
		{
			for(effect_t* next: next_vector)
			{
				bool parents_done = true;
				for(const effect_t* par: next->writers)
				 parents_done = parents_done && (par->id() != has_id::no_id());
				if(parents_done)
				 ready_fx.push(next);
			}
		};

		cb(cur_effect->readers);
		cb(cur_effect->deps);

	} while(ready_fx.size());


#if 0
	for(effect_t* e : project.effects()) // TODO: -> initializer list
	{
		e->instantiate();
		/*if(e->writers.empty())
		{
			_effect_root.readers.push_back(e);
			e->writers.push_back(&_effect_root);
		}*/
	}
#endif
}




loaded_project_t::~loaded_project_t()
{
	for(const effect_t* e : project.effects())
	if(e != &effect_root())
	 delete e;

#if 0
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
#endif
}

void player_t::update_effects()
{
	// TODO: use player's copies

	std::stack<effect_t*> ready_fx;
	ready_fx.push(&project.effect_root());
	do
	{
		effect_t* cur_effect = ready_fx.top();
		ready_fx.pop();

		cur_effect->proceed(pos);

		for(effect_t* next: cur_effect->deps)
		 ready_fx.push(next);

	} while(ready_fx.size());
}

void player_t::fill_commands()
{
/*	for(const auto& pr : project.global())
	for(const auto& pr2 : pr.second)
	{
		pr2.first->complete_buffer();
	}*/
}

void player_t::send_commands()
{

}

player_t::player_t(loaded_project_t &_project)  : project(_project)
{
/*	for(const auto& pr : project.global())
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
	pq.push(new task_events(nullptr, nullptr, end_set.begin())); // = sentinel*/
	std::cerr << "Player for " << _project.project.title() << std::endl;
	add_task(new sentinel);
	std::cerr << "FOUND " << _project.project.effects().size() << " FX..." << std::endl;
	for(effect_t*& e : _project.project.effects())
	{
		std::cerr << "pushing effect, next time: " << e->get_next_time() << std::endl;
		task_effect* new_task = new task_effect(e);
		handles[e] = add_task(new_task);
	}

}

void player_t::play_until(float dest)
{
	for(; pos < dest; pos += step)
	{
//		update_effects();
//		fill_commands();
	//	send_commands();
		std::cerr << "pos:" << pos << std::endl;
		while(has_active_tasks(pos))
		{
			std::cerr << "active task!" << std::endl;
			// TODO: simple reinsert??
			task_base* top = pop_next_task();


			/*const bool reinsert = top->proceed(pos);
			if(reinsert)
			 pq.push(top);*/
			const float cur_next_time = top->next_time();
			top->proceed(pos); // will update the next-time event

			handles.at(reinterpret_cast<task_effect*>(top)->effect) = add_task(top);

			for(const effect_t* dep : reinterpret_cast<task_effect*>(top)->effect->deps)
			{
				handle_type h = handles.at(dep);
				(*h)->update_next_time(cur_next_time);
				update(h);
			}


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

#if 0
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
#endif

}
