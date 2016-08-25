/*************************************************************************/
/* minimal - a minimal osc sequencer                                     */
/* Copyright (C) 2014-2016                                               */
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

#include <cstdlib>
#include <chrono>
#include <map>

#include "bars.h"
#include "io.h"
#include "player.h"
#include "mports.h"

namespace mini
{

void _player_t::init()
{
	no_rt::mlog << "Player for " << project->title() << std::endl;

	project->emplace<sentinel_effect>(1 + project->effects().size());
	std::map<const effect_t*, handle_type> handles;

	no_rt::mlog << "FOUND " << project->effects().size() << " FX..." << std::endl;
	for(effect_t*& e : project->get_effects_noconst())
	{
		no_rt::mlog << "pushing effect " << e->name() << " (" << e->id() << "), next time: " << e->get_next_time() << std::endl;
		task_effect* new_task = new task_effect(e);
		handles[e] = add_task(new_task);
		no_rt::mlog << e << std::endl;
		new_task->set_handle(handles[e]);
	}

	for(auto& pr : handles)
	{
		task_effect* te = static_cast<task_effect*>(*pr.second);
		effect_t* e = te->effect;
		for(const out_port_base* op  : e->get_out_ports())
		for(in_port_base* target_ip : op->readers())
		{
			const effect_t* target_ef = target_ip->get_effect();
			//changed_ports[target_ef->id()][target_ip->id] = true;

			//handle_type h = handles.at(target_ef);
			handle_type h = handles[target_ef];
			/*(*h)->update_next_time(cur_next_time);
			update(h);*/
			te->out_efcs.push_back(dynamic_cast<task_effect*>(*h)); // TODO: cast correct/needed?


		}

		// TODO: use default atomic
		e->cur_threads.store(/*e->max_threads*/0); // marks this task as "done"
		no_rt::mlog << "Effect " << e->name() << ": threads: " << e->cur_threads << e->max_threads << std::endl;
	}

//	ready_fx.reserve(project.effects().size());

	changed_ports.resize(1 + project->effects().size());

/*	for(std::size_t i = 0; i < project.effects().size(); ++i)
	{
		//no_rt::mlog << project.effects()[i]->name() << " -> " << project.effects()[i]->get_in_ports().size()
		//	<< std::endl;
		changed_ports

		changed_ports[i].resize(project.effects()[i]->get_in_ports().size());
	}*/

	for(effect_t* e : project->effects())
	{
		changed_ports[e->id()].resize(e->get_in_ports().size());
	}
}

void _player_t::set_project(loaded_project_t &_project)
{
	project = &_project;
	init();
}

_player_t::_player_t(loaded_project_t &_project) :
	project(&_project)//,
{
	init();
}

#ifdef __clang__
	#define REALTIME __attribute__((annotate("realtime")))
#else
	#define REALTIME // replace with "nothing"
#endif

#if 0
// TODO: deprecated!?!?!?
void _player_t::play_until(sample_no_t dest)
{
// function probably deprecated

	sample_no_t final_pos = dest; //pos + work;
	io::mlog << "starting playback: " << pos << io::endl;

	auto start_time = std::chrono::system_clock::now().time_since_epoch();

	for(; next_task_time() < final_pos; pos = next_task_time()) // TODO: <= ?
	{
		{
			auto duration = std::chrono::system_clock::now().time_since_epoch() - start_time;
			auto useconds_right_now = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
			sample_no_t samples_right_now = useconds_right_now / info.usecs_per_sample;

			// do not sleep longer than dest, but at least 0
			sample_no_t sleep_time = std::max(std::min(next_task_time() - samples_right_now,
					dest - samples_right_now) * info.usecs_per_sample, 0_smps);
			usleep((useconds_t)sleep_time);
		}

		process(pos); // TODO: not 0
		io::mlog << "done: " << pos << io::endl;
		io::mlog << "next: " << next_task_time() << io::endl;
		io::mlog << "next name / id: " << ((task_effect*)peek_next_task())->effect->name() << " / "
			<< ((task_effect*)peek_next_task())->effect->id() << io::endl;
	}

	io::mlog << "Finishing, since nothing to do at " << pos << io::endl;

	// no more tasks possible, so:
	// pos = final_pos; // dangerous for subsequent calls
}
#endif

//! the "heart" of minimal
//! @note the thread stuff here is all incomplete and buggy. just ignore it,
//!   as minimal currently only runs single threaded...
void REALTIME _player_t::process(sample_no_t work)
{
	if(work == 0) {
		throw "Error: work = 0";
	 std::cerr << "WARNING: work = 0" << std::endl;
	 work = 1024;
	}
//	const auto& has_active_tasks = []() -> bool {
//
//	}

	// TODO: thread repeater

	// TODO: always spinlock the work queue!!!!

	io::mlog << dynamic_cast<task_effect*>(peek_next_task())->effect->cur_threads
		<< " vs "
		<< dynamic_cast<task_effect*>(peek_next_task())->effect->max_threads << io::endl;

	// only used in while loop's condition
	auto try_get_task = [this](sample_no_t limit) -> task_effect*
	{
		task_effect* ret = nullptr;
		work_queue_lock.lock();
		task_base* next_task = peek_next_task();

		task_effect* te = dynamic_cast<task_effect*>(next_task);

		io::mlog << "next task: " << te->effect->id() << io::endl
			<< "at time: " << next_task->next_time() << io::endl;
		io::mlog << "limit: " << limit << io::endl;

		io::mlog << "cur threads: " << te->effect->cur_threads
			<< "of " << te->effect->max_threads << io::endl;
		if(next_task->next_time() <= limit
			&& te->effect->cur_threads < te->effect->max_threads)
		 ret = te;
		work_queue_lock.unlock();
		io::mlog << "acceptable? "
			<< ret << io::endl;
		return ret;
	};


	task_effect* task_e;
	/*while(peek_next_task()->next_time() <= pos &&
		dynamic_cast<task_effect*>(peek_next_task())->effect->cur_threads
		< dynamic_cast<task_effect*>(peek_next_task())->effect->max_threads )*/
	while((task_e = try_get_task(pos + work)))
	{
		// TODO: is this correct? we start an effect *before* the limit is reached?

		io::mlog << "pos, work: "
			<< pos << ", " << work << io::endl;

		// TODO: simple reinsert??

		//task_base* top = peek_next_task();
//			task_effect* task_e = dynamic_cast<task_effect*>(top);
		effect_t* this_ef = task_e->effect;

		/*const bool reinsert = top->proceed(pos <- time!!!);
		if(reinsert)
		 pq.push(top);*/
		const sample_no_t cur_next_time = task_e->next_time();

		/*int cur_threads_afterwards = ++this_ef->cur_threads;
		if(cur_threads_afterwards > this_ef->max_threads)
		{
			// => argh, should not happen

			// if this happens,
			//  1. we are exclusively in this if
			//  2. all other threads have left this effect
		}*/
		
		if(++this_ef->cur_threads == 1)
		 this_ef->pass_changed_ports(changed_ports[this_ef->id()]); // TODO: unsafe!!!



		io::mlog << "next effect threads now: " << this_ef->cur_threads << io::endl;
		io::mlog << "next effect work now: " << work << io::endl;

		task_e->proceed(); // will also update the next-time event

		//handles.at(this_ef) = add_task(top);

		// update pq, since #finished_threads has changed
		work_queue_lock.lock();
		io::mlog << "update start -> " << io::endl;
		update(task_e->get_handle());
		io::mlog << "<- update end" << io::endl;
		work_queue_lock.unlock();


		if(++this_ef->finished_threads == this_ef->max_threads) // TODO: wrong
		{
			io::mlog << "REACHED MAX TASKS!" << io::endl;
			// if this happens,
			//  * we are exclusively in this if block
			//  * all threads (including ours) are finished
			//  * no other task has access to this_ef

			/*for(const effect_t* dep : reinterpret_cast<task_effect*>(top)->effect->deps)
			{
				handle_type h = handles.at(dep);
				(*h)->update_next_time(cur_next_time);
				update(h);
			}*/


			std::size_t count = 0;
			// TODO: effect should give us this array...
			for(const out_port_base* op  : this_ef->get_out_ports())
			{
			// TODO! only dependencys...
			if(op->change_stamp >= pos)
			{
				for(in_port_base* target_ip : op->readers())
				{
				//	TODO: check this!!!
					const effect_t* target_ef = target_ip->get_effect();

					changed_ports[target_ef->id()][target_ip->get_id()] = true;
					// TODO: use a vector in task_effect, too? like in_efcs, out_efcs?

					//handle_type h = handles.at(target_ef);
					handle_type h = task_e->out_efcs[count++]->get_handle(); //target_ef
					task_base* te = *h;

				//	te->effect
					dynamic_cast<task_effect*>(te)->effect->init_next_time(cur_next_time);
					te->update_next_time(cur_next_time);
					io::mlog << "next time: " << cur_next_time << io::endl;

					work_queue_lock.lock();
					update(h);
					work_queue_lock.unlock();
				}
			}
			else
			 count +=op->readers().size();
			}

			this_ef->finished_threads.store(0);
			this_ef->cur_threads.store(0);
		}
		else
		 io::mlog << "NOT REACHED MAX TASKS!" << io::endl;
	} // while has active tasks at this time

	io::mlog << "Nothing to do at " << pos << io::endl;
}

void _player_t::task_effect::proceed()
{
	io::mlog << as_bars(effect->get_next_time(), info.samples_per_bar)
		<< ": next effect: " << effect->name() << " (id: " << effect->id() << ')'
		<< io::endl;
	if(effect->proceed() && true)
	{
		update_next_time(effect->get_next_time());
	}
	//else

	// the effect might have been finished or not
	// depending on which thread exits



	//update_next_time(effect->proceed(amnt));
}

bool _player_t::task_effect::cmp(const work_queue_t::task_base& other) const {
	// ugly cast, but probably not avoidable?
	//return effect->id() < dynamic_cast<const task_effect&>(other).effect->id();
	const effect_t* const o_effect = dynamic_cast<const task_effect&>(other).effect;

	if(&other == this)
		io::mlog << "equal fx compared..." << io::endl;

	bars_t b_self(effect->cur_threads, effect->max_threads),
			b_other(o_effect->cur_threads, o_effect->max_threads);

	io::mlog << "bars: " << b_self << " vs " << b_other << ": " <<
		    (b_self == b_other) << ", " << (b_self > b_other) << io::endl;

	// strict ordering is guaranteed (!)
	return (b_self == b_other) // don't bother about b_other?
			? effect->id() > o_effect->id() // take smaller id
			: (b_self > b_other); // take smaller bar

}

}

