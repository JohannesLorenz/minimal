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

#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <boost/heap/fibonacci_heap.hpp>

namespace mini {

class work_queue_t
{
protected:
	class task_base
	{
//		const command_base* cmd;
		float _next_time;
	protected:
		void update_next_time(float new_value) {
			_next_time = new_value;
		}
	public:
		virtual void proceed(float time) = 0;
		float next_time() const { return _next_time; }
		task_base(float next_time) : _next_time(next_time) {}
//		virtual float next() = 0;
	};


	using pq_entry = task_base*; // TODO: redundancy

	struct cmp_func
	{
		bool operator() (const pq_entry& lhs, const pq_entry& rhs) const
		{
			return lhs->next_time() > rhs->next_time(); // should be <, but we start with small values

		/*	bool left_end = lhs.itr != lhs.vals.end();
			bool right_end = rhs.itr != rhs.vals.end();

			return (right_end && !left_end) ||*/
		}
	};

	typedef boost::heap::fibonacci_heap<pq_entry, boost::heap::compare<cmp_func>> pq_type;
	pq_type pq;

	float run_tasks(float pos)
	{
		while(pq.top()->next_time() <= pos)
		{

			pq_entry top = std::move(pq.top());
			pq.pop();


			/*const bool reinsert = top->proceed(pos);
			if(reinsert)
			 pq.push(top);*/
			top->proceed(pos); // will update the next-time event
			pq.push(top);

		}
		return pq.top()->next_time();
	}
};

}

#endif // WORK_QUEUE_H
