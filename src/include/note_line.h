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
#include "mports.h"
#include "impl.h"
//#include "io.h"

namespace mini {

//constexpr unsigned char MAX_NOTES_PRESSED = 32;

using namespace daw; // TODO

template<class T>
class note_line_t;

class music_note_properties
{
	char volume = 64;
	char valocity() const { return volume; }
};

template<class NoteProperties>
class note_line_impl : public is_impl_of_t<note_line_t<NoteProperties>>//, public work_queue_t
{
	friend class note_line_t<NoteProperties>;
	using m_note_line_t = note_line_t<NoteProperties>;
	using impl_t = is_impl_of_t<m_note_line_t>;

	sample_no_t last_time = -1.0f;
	//std::map<int, std::map<sample_no_t, note_t>> note_lines;
	/*sample_no_t last_time = -1.0f;
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

	struct m_note_event : public NoteProperties
	{
		bool on;
		int id; // TODO: unused?
	};

	using event_map_t = std::map<note_geom_t, m_note_event>;
	event_map_t note_events;
	typename event_map_t::const_iterator itr;

	/*struct note_task_t : public task_base
	{
		//const loaded_instrument_t* ins;
		//const command_base* cmd;
		note_line_impl* nl_ref;
		//int* last_key;
		const int note_height;
		bool is_on = false;
		std::map<note_geom_t, m_note_event>::const_iterator itr;


		void proceed(sample_no_t time);

		note_task_t(note_line_impl& nl_ref,
			const int& note_height,
			const std::map<sample_no_t, note_t>& values,
			sample_no_t first_event = 0.0f) :
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

	void visit(const notes_t<NoteProperties>& n, const note_geom_t offset)
	{
		note_geom_t cur_offs = offset + n.geom;
		for(const std::pair<const note_geom_t,
			const notes_t<NoteProperties>*>& n2 :
			n.template get<notes_t<NoteProperties>>()) {
			visit(*n2.second, cur_offs + n2.first);
		}
		for(const std::pair<const note_geom_t,
			const note_t<NoteProperties>*>& n2 :
			n.template get<note_t<NoteProperties>>())
		{
			const note_t<NoteProperties>& cur_note = *n2.second;
			const note_geom_t next_offs = cur_offs + n2.first;
			std::cerr << "emplacing: " << next_visit_id << std::endl;
			note_events.emplace(next_offs,
				m_note_event{true, cur_note.velocity(), next_visit_id}); // TODO: 1
			note_events.emplace(next_offs + note_geom_t(cur_note.length(), 0),
				m_note_event{false, cur_note.velocity(), next_visit_id++});
		}
	}

public:
	note_line_impl(m_note_line_t *nl) : is_impl_of_t<m_note_line_t>(nl)
	{
		// insert notes
		visit(impl_t::ref->notes, note_geom_t(bars_t(0, 1), 0));
		// insert sentinel
		note_events.emplace(note_geom_t(bars_t(100000, 1), 1), // TODO: this number...
			m_note_event{true, 0, std::numeric_limits<int>::max()});

		itr = note_events.begin();
	}

	sample_no_t _proceed(sample_no_t amnt)
	{
		note_signal_t<NoteProperties>& notes_out = impl_t::ref->notes_out::data;
		std::pair<int, int>* recently_changed_ptr = notes_out.recently_changed.data();

		// itr points to note_events
		while(
			as_samples_floor(itr->first.start, info.samples_per_bar) <= amnt) // TODO! 0.1f 0.1f 0.1f
		{
			const note_geom_t& geom = itr->first;
			const m_note_event& event = itr->second;
			std::pair<int, int>* notes_at = notes_out.lines[geom.offs];
			std::size_t id = 0;
			if(event.on)
			{
				// skip used slots
				for(; id < POLY_MAX && notes_at->first > 0; ++notes_at, ++id) ;

				if(id >= POLY_MAX)
				 throw "end of polyphony reached!";

				notes_at->first = event.id;
				notes_at->second = event.volume;
				io::mlog << "note on: " << event.id << io::endl;
			}
			else
			{
				for(; id < POLY_MAX && notes_at->first != event.id; ++notes_at, ++id) ;

				if(id >= POLY_MAX)
				 throw "end of polyphony reached!";

				notes_at->first = -1;

				io::mlog << "note off: " << event.id << io::endl;
			}

			recently_changed_ptr->first = geom.offs;
			(recently_changed_ptr++)->second = id;

			io::mlog << "played one note: " << itr->first.start << io::endl;
	
			++itr;
		}

		recently_changed_ptr->first = -1;
		++notes_out.changed_stamp;
		impl_t::ref->notes_out::change_stamp = amnt;

		last_time = amnt;
		return as_samples_floor(itr->first.start, info.samples_per_bar); // TODO! 0.1f 0.1f 0.1f
	}
};

template<class T>
class note_line_t : public effect_t, public notes_out<T>, has_impl_t<note_line_impl<T>, note_line_t<T>> // TODO: which header?
{
	friend class note_line_impl<T>;
	using impl_t = has_impl_t<note_line_impl<T>, note_line_t<T>>;

	//! @note: one might need to store the notes_t blocks seperated for muting etc
	notes_t<T> notes;

public:
	note_line_t() :
//		port_chain<notes_out>((effect_t&)*this),
		notes_out<T>((effect_t&)*this),
		impl_t(this)
	{
	}

	void instantiate() {
		impl_t::instantiate();
		set_next_time(
			as_samples_floor(impl_t::impl->note_events.begin()->first.start,
				info.samples_per_bar)); // TODO! 0.1f 0.1f 0.1f
	}

	void clean_up() {}

	void add_notes(const notes_t<T>& n, const note_geom_t& ng) {
		//note_events.emplace(ng, n);
		notes.add_notes(n, ng);
	}


	bool _proceed()
	{
		//io::mlog << "proceeding with note line... " << io::endl;
		set_next_time(impl_t::impl->_proceed(time()));
		return true;
	}
};

}

#endif // NOTE_LINE_H

