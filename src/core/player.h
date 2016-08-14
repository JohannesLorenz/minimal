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

#ifndef LOADED_PROJECT_H
#define LOADED_PROJECT_H

#include "project.h"
#include "work_queue.h"
#include "sample.h"
#include "bars.h"
#include "spinlock.h"
#include "audio.h"
#include "io.h"

namespace mini {

/*
//! rt-safe stack, once reserved
template<class T>
class array_stack
{
	std::vector<T> data;
public:
	void reserve(const std::size_t& size) { data.reserve(size); }
	void push(const T& new_value) { data.push_back(new_value); }
	T&& pop() {
		T&& top = std::move(*data.rbegin());
		data.pop_back();
		return std::move(top);
	}
	std::size_t size() const { return data.size(); }
};*/

using loaded_project_t = project_t;

class _player_t : public work_queue_t // TODO: own header
{
	//array_stack<effect_t*> ready_fx;

	sample_no_t pos = 0; //!< number of samples played until now
	loaded_project_t* project; // TODO! must be const

	spinlock_t work_queue_lock;

//	std::set<sample_no_t> end_set = { std::numeric_limits<sample_no_t>::max() };

	class task_effect : public task_base_with_handle
	{
	//	const instrument_t* ins;
	public:
		effect_t* const effect;
		std::vector<task_effect*> in_efcs, out_efcs;
		//const command_base* cmd;
	public:
		task_effect(effect_t* effect) :
			// TODO: 0 is wrong if we don't start playback at 0
			task_base_with_handle(effect->get_next_time()),
			effect(effect)
		{
		}

		void proceed()
		{
			io::mlog << "next effect: " << effect->name() << " (id: " << effect->id() << ')'
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

		bool cmp(const task_base& other) const {
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

/*		handle_type& get_handle() final {

		}*/
	};

	std::vector<std::vector<bool>> changed_ports;

	void process(sample_no_t work);

	void init();
public:
	class audio_sink_t* sink() { return project->sink; }

	_player_t() = default;
	void set_project(loaded_project_t& _project);
	_player_t(loaded_project_t& _project);
	//void play_until(sample_no_t dest);

	//! will be asking for the next nframes frames (i.e. samples for us)
	//! and announce the following callback at @a pos + @a nframes samples
	void callback(std::size_t nframes) {
		if(next_task_time() <= pos + (sample_no_t)nframes)
		 process(nframes);
		pos += nframes;
	}
};

using player_t = _player_t;
/*
//! template type of @a _player_t
template<class SinkType>
class player_t : public _player_t
{
	SinkType sink;
public:
	using _player_t::_player_t;
};*/

// TODO: class audio_stereo_project will set sink to mult<ringbuffer_t>

}

#endif // LOADED_PROJECT_H
