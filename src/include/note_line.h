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
#include "simple.h"
//#include "io.h"

namespace mini {

//constexpr unsigned char MAX_NOTES_PRESSED = 32;

template<class T>
class event_line_t;

template<class NoteProperties>
class line_impl : public is_impl_of_t<event_line_t<NoteProperties>>
{
	friend class event_line_t<NoteProperties>;
	using m_event_line_t = event_line_t<NoteProperties>;
	using impl_t = is_impl_of_t<m_event_line_t>;
	using m_geom_t = daw::note_geom_t;

	sample_no_t last_time = -1.0f;
	//std::map<int, std::map<sample_no_t, event_t>> note_lines;
	/*sample_no_t last_time = -1.0f;
	struct notes_impl_t
	{
		std::map<note_geom_t, event_t>::const_iterator itr;
		//std::vector<event_t_impl> children;
		typedef boost::heap::fibonacci_heap<notes_impl_t> pq_type;
		std::pair<note_geom_t, event_t> next_elem; // todo: ptr?


		pq_type pq;


		std::pair<note_geom_t, event_t> next_note() {
			next_elem =
			return std::min(*itr, pq.top());
		}
	};*/

	struct m_note_event : public NoteProperties
	{
		bool on;
		int id;
		template<class ...Args>
		m_note_event(bool on, int id, Args... args) :
			NoteProperties(args...), on(on), id(id)
		{
		}
	};

	//  TODO: rename m_note_event -> m_event, allow subclasses of it via NoteProperties?
	using event_map_t = std::map<m_geom_t, m_note_event>;
	event_map_t note_events;
	typename event_map_t::const_iterator itr;

//	notes_impl_t root;

	int next_visit_id = 0;

	void visit(const daw::events_t<NoteProperties>& n, const m_geom_t offset)
	{
		m_geom_t cur_offs = offset + n.geom;
		for(const std::pair<const daw::note_geom_t,
			const daw::events_t<NoteProperties>*>& n2 :
			n.template get<daw::events_t<NoteProperties>>()) {
			visit(*n2.second, cur_offs + n2.first);
		}
		for(const std::pair<const m_geom_t,
			const daw::event_t<NoteProperties>*>& n2 :
			n.template get<daw::event_t<NoteProperties>>())
		{
			const daw::event_t<NoteProperties>& cur_note = *n2.second;
			const m_geom_t next_offs = cur_offs + n2.first;
			std::cerr << "emplacing: " << next_visit_id << std::endl;
			// TODO: this does not work for all note properties
			note_events.emplace(next_offs,
				m_note_event(true, next_visit_id, cur_note.value())); // TODO: 1
			note_events.emplace(next_offs + m_geom_t(cur_note.length(), 0),
				m_note_event(false, next_visit_id++, cur_note.value()));
		}
	}

public:
	line_impl(m_event_line_t *nl) : is_impl_of_t<m_event_line_t>(nl)
	{
		// insert notes
		visit(impl_t::ref->notes, m_geom_t(bars_t(0, 1), 0));
		// insert sentinel
		note_events.emplace(m_geom_t(bars_t(100000, 1), 1), // TODO: this number...
			m_note_event(true, std::numeric_limits<int>::max(), 0));

		itr = note_events.begin();
	}

	sample_no_t _proceed(sample_no_t amnt)
	{
		event_signal_t<NoteProperties>& events_out = impl_t::ref->events_out_t<NoteProperties>::data;
		std::pair<int, int>* recently_changed_ptr = events_out.recently_changed.data();

		// itr points to note_events
		while(
			as_samples_floor(itr->first.start, info.samples_per_bar) <= amnt)
		{
			const m_geom_t& geom = itr->first;
			const m_note_event& event = itr->second;
			std::pair<int, NoteProperties>* events_at = events_out.lines[geom.offs];
			std::size_t id = 0;
			if(event.on)
			{
				// skip used slots
				for(; id < POLY_MAX && events_at->first > 0; ++events_at, ++id) ;

				if(id >= POLY_MAX)
				 throw "end of polyphony reached!";

				events_at->first = event.id;
				events_at->second = event;
				io::mlog << "note on: " << event.id << io::endl;
			}
			else
			{
				for(; id < POLY_MAX && events_at->first != event.id; ++events_at, ++id) ;

				if(id >= POLY_MAX)
				 throw "end of polyphony reached!";

				events_at->first = -1;

				io::mlog << "note off: " << event.id << io::endl;
			}

			recently_changed_ptr->first = geom.offs;
			(recently_changed_ptr++)->second = id;

			io::mlog << "played one note: " << itr->first.start << io::endl;
	
			++itr;
		}

		recently_changed_ptr->first = -1;
		++events_out.changed_stamp;
		impl_t::ref->events_out_t<NoteProperties>::change_stamp = amnt;

		last_time = amnt;
		return as_samples_floor(itr->first.start, info.samples_per_bar);
	}
};

template<class T>
class event_line_t : public effect_t, public events_out_t<T>, has_impl_t<line_impl<T>, event_line_t<T>> // TODO: which header?
{
	friend class line_impl<T>;
	using impl_t = has_impl_t<line_impl<T>, event_line_t<T>>;

	//! @note: one might need to store the events_t blocks seperated for muting etc
	daw::events_t<T> notes;

public:
	event_line_t() :
//		port_chain<events_out_t>((effect_t&)*this),
		events_out_t<T>((effect_t&)*this),
		impl_t(this)
	{
	}

	void instantiate() {
		impl_t::instantiate();
		set_next_time(
			as_samples_floor(impl_t::impl->note_events.begin()->first.start,
				info.samples_per_bar));
	}

	void clean_up() {}

	void add_notes(const daw::events_t<T>& n, const daw::note_geom_t& ng) {
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

