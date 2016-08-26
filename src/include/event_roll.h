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

#ifndef EVENT_ROLL_H
#define EVENT_ROLL_H

//! @file event_roll.h abstraction of a piano roll

#include <iosfwd>
#include <map>

#include "effect.h"
#include "daw.h"
#include "mports.h"
#include "impl.h"
#include "simple.h"

namespace mini {

void log_note_event(bool on, const bars_t& start, int event_id);

//constexpr unsigned char MAX_NOTES_PRESSED = 32;

template<class T>
class event_roll_t;

class roll_impl_base
{
protected:
	using m_geom_t = daw::note_geom_t;

	std::size_t visit_depth = 0;

	void log_visit_events(m_geom_t cur_offset, m_geom_t next_offset) const;
	void log_visit_event(m_geom_t next_offset, int next_visit_id) const;
};

template<class NoteProperties>
class roll_impl : roll_impl_base, public is_impl_of_t<event_roll_t<NoteProperties>>
{
	friend class event_roll_t<NoteProperties>;
	using m_event_roll_t = event_roll_t<NoteProperties>;
	using impl_t = is_impl_of_t<m_event_roll_t>;

	sample_no_t last_time = -1.0f;
	//std::map<int, std::map<sample_no_t, event_t>> piano_rolls;
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
	using event_map_t = std::multimap<m_geom_t, m_note_event>;
	event_map_t note_events;
	typename event_map_t::const_iterator itr;

//	notes_impl_t root;

	int next_visit_id = 0;

	void visit(const daw::events_t<NoteProperties>& n, const m_geom_t offset,
		bars_t factor = bars_t(1, 1))
	{
		++visit_depth;
		m_geom_t cur_offs = offset + n.geom();
		for(const auto& n2 :
			n.template get<daw::events_t<NoteProperties>>()) {
#ifdef DEBUG_NOTE_LINE
			for(std::size_t x = visit_depth; x; --x)
			 io::mlog << "  ";
			io::mlog << "recursing: " << cur_offs + n2.first << io::endl;
#endif
			log_visit_events(cur_offs, n2.first);

			visit(*n2.second.child, cur_offs + n2.first, factor * n2.second.factor);
		}
		for(const auto& n2 :
			n.template get<daw::event_t<NoteProperties>>())
		{
			const daw::event_t<NoteProperties>& cur_note = *n2.second.child;
			const m_geom_t next_offs = cur_offs + n2.first;

			log_visit_event(next_offs, next_visit_id);

			std::cerr << "factors: " << factor << ", " << n2.second.factor << std::endl;
			note_events.emplace(next_offs,
				m_note_event(true, next_visit_id, cur_note.value()));
			note_events.emplace(next_offs + m_geom_t(cur_note.length() * factor * n2.second.factor, 0),
				m_note_event(false, next_visit_id++, cur_note.value()));
		}
		--visit_depth;
	}

public:
	roll_impl(m_event_roll_t *nl) : is_impl_of_t<m_event_roll_t>(nl)
	{
		// insert notes
		visit(impl_t::ref->notes, m_geom_t(bars_t(0, 1), 0));
		// insert sentinel
		note_events.emplace(m_geom_t(bars_t(100000, 1), 1), // TODO: this number...
			m_note_event(true, std::numeric_limits<int>::max(), 0));

		itr = note_events.begin();
		std::cerr << "LINEIMPL" << std::endl;
		std::cerr << *this << std::endl;
	}

	sample_no_t _proceed(sample_no_t pos)
	{
		event_signal_t<NoteProperties>& events_out = impl_t::ref->events_out_t<NoteProperties>::value();
		std::pair<int, int>* recently_changed_ptr = events_out.recently_changed.data();

		// itr points to note_events
		// => check all not yet played notes at/before the current position
		while(
			as_samples_floor(itr->first.start, info.samples_per_bar) <= pos)
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
				 throw "note on, but end of polyphony reached!";

				events_at->first = event.id;
				events_at->second = event;
			}
			else
			{
				for(; id < POLY_MAX && events_at->first != event.id; ++events_at, ++id) ;

				std::cerr << "note off: " << as_bars(pos, info.samples_per_bar) << ", "
					  << itr->first << std::endl;

				if(id >= POLY_MAX)
				 throw std::runtime_error("note off for a note that is already off");

				events_at->first = -1;
			}
			recently_changed_ptr->first = geom.offs;
			(recently_changed_ptr++)->second = id;

			// io logging
			log_note_event(event.on, itr->first.start, event.id);
	
			++itr;
		}

		recently_changed_ptr->first = -1;
		++events_out.changed_stamp;
		impl_t::ref->events_out_t<NoteProperties>::change_stamp = pos;

		last_time = pos;
		std::cerr << "start: " << itr->first.start << std::endl;
		return as_samples_floor(itr->first.start, info.samples_per_bar);
	}

	template<class N>
	friend std::ostream& operator<<(std::ostream& os, roll_impl<N> e);
};

template<class N>
std::ostream& operator<<(std::ostream& os, roll_impl<N> e) {
//	for(const auto& pr : e.note_events) ;
	(void) e;
	return os;
}

//! abstraction of a piano roll for events of type T
template<class T>
class event_roll_t : public effect_t, public events_out_t<T>, has_impl_t<roll_impl<T>, event_roll_t<T>> // TODO: which header?
{
	friend class roll_impl<T>;
	using impl_t = has_impl_t<roll_impl<T>, event_roll_t<T>>;

	//! @note: one might need to store the events_t blocks seperated for muting etc
	daw::events_t<T> notes;

public:
	event_roll_t(const char* name) :
//		port_chain<events_out_t>((effect_t&)*this),
		effect_t(name),
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

	std::ostream& dump(std::ostream& os = std::cerr) const
	{
		os << "event line: " << std::endl;
		return notes.dump(os);
	}

	template<class T2>
	friend std::ostream& operator<<(std::ostream& os, const event_roll_t<T2>& e);
};

template<class T>
inline std::ostream& operator<<(std::ostream& os, const event_roll_t<T>& e)
{
	return os << "event line: " << std::endl
		<< e.notes;
	//	<< *e.impl;
//	return os << e.id() << std::endl;
}

template<class T>
void nlf(std::ostream& os, const T& x)
{
	os << x << std::endl;
}

}

#endif // EVENT_ROLL_H

