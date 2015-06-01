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

#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <iostream> // TODO
#include <boost/heap/fibonacci_heap.hpp>
#include "sample.h"

namespace mini {

class work_queue_t
{
public:
	class task_base;
	struct cmp_func
	{
		bool operator() (const task_base* const& lhs, const task_base* const& rhs) const; // see below
	};
	typedef boost::heap::fibonacci_heap<task_base*, boost::heap::compare<cmp_func>> pq_type;
	using handle_type = pq_type::handle_type;

	class task_base // TODO: make this crtp when possible?
	{
//		const command_base* cmd;
		sample_t _next_time;
	public:
		void update_next_time(sample_t new_value) {
			_next_time = new_value;
		}
		virtual void proceed(sample_t time) = 0;
		sample_t next_time() const { return _next_time; }
		task_base(sample_t next_time) : _next_time(next_time) {}
		virtual bool cmp(const task_base& other) const { return this < &other; }
		virtual handle_type& get_handle() { throw "not implemented"; }

//		virtual sample_t next() = 0;
	};

	class task_base_with_handle : public task_base
	{
		handle_type handle;
	public:
		using task_base::task_base;
		handle_type& get_handle() final { return handle; }
		void set_handle(handle_type h) { handle = h; }
	};

private:
	pq_type pq;
public:
	sample_t run_tasks(sample_t pos)
	{
		while(pq.top()->next_time() <= pos)
		{

			task_base* top = std::move(pq.top());
			pq.pop();


			/*const bool reinsert = top->proceed(pos);
			if(reinsert)
			 pq.push(top);*/
			top->proceed(pos); // will update the next-time event
			pq.push(top);

		}
		return pq.top()->next_time();
	}

	sample_t run_tasks_keep(sample_t pos)
	{
		while(pq.top()->next_time() <= pos)
		{

			task_base* top = std::move(pq.top());
			top->proceed(pos);
			update(top->get_handle());

			/*const bool reinsert = top->proceed(pos);
			if(reinsert)
			 pq.push(top);*/

		}
		return pq.top()->next_time();
	}

	sample_t next_task_time() const {
		return pq.top()->next_time();
	}

	handle_type add_task(task_base* new_task) {
		return pq.push(new_task);
	}

	task_base* peek_next_task() {
		return pq.top();
	}

	task_base* pop_next_task() {
		task_base* ret_val = peek_next_task();
		pq.pop();
		return ret_val;
	}

	bool has_active_tasks(sample_t at_time) {
		//io::mlog << "active? " << pq.top()->next_time() << " <= "<< at_time << " ? " << (pq.top()->next_time() <= at_time) << io::endl;
		return pq.top()->next_time() <= at_time;
	}

	void update(handle_type h) { pq.update(h); }
};

inline bool work_queue_t::cmp_func::operator() (const task_base* const& lhs, const task_base* const& rhs) const
{
	std::cerr << lhs->next_time() << " <-> " << rhs->next_time() << std::endl;
	if(lhs->next_time() == rhs->next_time())
	{
		std::cerr << " -> " << lhs->cmp(*rhs) << std::endl;
	}
	return (lhs->next_time() == rhs->next_time())
		? lhs->cmp(*rhs)
		: lhs->next_time() > rhs->next_time(); // should be <, but we start with small values
/*	bool left_end = lhs.itr != lhs.vals.end();
	bool right_end = rhs.itr != rhs.vals.end();

	return (right_end && !left_end) ||*/
}

}

#endif // WORK_QUEUE_H
