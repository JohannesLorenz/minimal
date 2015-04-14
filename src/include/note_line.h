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

#ifndef NOTE_LINE_H
#define NOTE_LINE_H

#include "effect.h"
#include "daw.h"
#include "ports.h"
#include "impl.h"
#include "io.h"

namespace mini {

//constexpr unsigned char MAX_NOTES_PRESSED = 32;

using namespace daw; // TODO

class note_line_t;

class note_line_impl : public is_impl_of_t<note_line_t>//, public work_queue_t
{
	friend class note_line_t;

	sample_t last_time = -1.0f;
	//std::map<int, std::map<sample_t, note_t>> note_lines;
	/*sample_t last_time = -1.0f;
	struct notes_impl_t
	{
		std::map<note_geom_t, note_t>::const_iterator itr;
		//std::vector<note_t_impl> children;
		typedef boost::heap::fibonacci_heap<notes_impl_t> pq_type;
		std::pair<note_geom_t, note_t> next_elem; // todo: ptr?


		pq_type pq;


		std::pair<note_geom_t, note_t> next_note() {
			next_elem =
			return std::min(*itr, pq.top());
		}
	};*/

	struct m_note_event
	{
		bool on;
		int volume;
		int id; // TODO: unused?
	};

	std::map<note_geom_t, m_note_event> note_events;
	std::map<note_geom_t, m_note_event>::const_iterator itr;

	/*struct note_task_t : public task_base
	{
		//const loaded_instrument_t* ins;
		//const command_base* cmd;
		note_line_impl* nl_ref;
		//int* last_key;
		const int note_height;
		bool is_on = false;
		std::map<note_geom_t, m_note_event>::const_iterator itr;


		void proceed(sample_t time);

		note_task_t(note_line_impl& nl_ref,
			const int& note_height,
			const std::map<sample_t, note_t>& values,
			sample_t first_event = 0.0f) :
			task_base(first_event),
			nl_ref(&nl_ref),
			//last_key(nl_ref.notes_pressed.get()),
			note_height(note_height),
			itr(nl_ref.note_events.begin())
		{
			(void)values; // TODO
			if(note_height < 0 || note_height >= (int)NOTES_MAX)
			 throw "invalid note height";
		}
	};*/


//	notes_impl_t root;

	int next_visit_id = 0;

	void visit(const notes_t& n, const note_geom_t offset)
	{
		note_geom_t cur_offs = offset + n.geom;
		for(const std::pair<const note_geom_t, const notes_t*>& n2 : n.get<notes_t>()) {
			visit(*n2.second, cur_offs + n2.first);
		}
		for(const std::pair<const note_geom_t, const note_t*>& n2 : n.get<note_t>())
		{
			const note_t& cur_note = *n2.second;
			const note_geom_t next_offs = cur_offs + n2.first;
			note_events.emplace(next_offs,
				m_note_event{true, cur_note.velocity(), next_visit_id}); // TODO: 1
			note_events.emplace(next_offs + note_geom_t(cur_note.length(), 0),
				m_note_event{false, cur_note.velocity(), next_visit_id++});
		}
	}

public:

	note_line_impl(note_line_t *nl);

	sample_t _proceed(sample_t time); /* {
		return run_tasks(time);
	}*/
};

class note_line_t : public effect_t, public notes_out, has_impl_t<note_line_impl, note_line_t> // TODO: which header?
{
	friend class note_line_impl;
	using impl_t = has_impl_t<note_line_impl, note_line_t>;

	//! @note: one might need to store the notes_t blocks seperated for muting etc
	notes_t notes;

public:
	note_line_t() :
//		port_chain<notes_out>((effect_t&)*this),
		notes_out((effect_t&)*this),
		impl_t(this)
	{
	}

	void instantiate() {
		impl_t::instantiate();
		set_next_time(impl->note_events.begin()->first.start.floor()); // TODO! 0.1f 0.1f 0.1f
	}

	void clean_up() {}

	void add_notes(const notes_t& n, const note_geom_t& ng) {
		//note_events.emplace(ng, n);
		notes.add_notes(n, ng);
	}


	sample_t _proceed(sample_t time)
	{
		io::mlog << "proceeding with note line... " << io::endl;
		sample_t next = impl->_proceed(time);
		return next;
	}
};

}

#endif // NOTE_LINE_H
