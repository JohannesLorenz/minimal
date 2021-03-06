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

#ifndef DAW_H
#define DAW_H

#include <tuple>
#include <map>
#include "utils.h"
#include "tuple_helpers.h"
#include "bars.h"
#include "scales.h" // TODO: only chromatic.h

namespace mini {

namespace daw
{
	class geom_t
	{
	public:
		bars_t start;
		geom_t(bars_t start) noexcept : start(start) {}
		static geom_t zero() noexcept { return geom_t(bars_t(0, 1)); }
		bool operator<(const geom_t& rhs) const
		{
			return start < rhs.start;
		}
	};

	using note_offset_t = unsigned char;

	class note_geom_t
	{
	public:
		bars_t start;
		note_offset_t offs;
		note_geom_t(bars_t start, note_offset_t offs) : start(start), offs(offs) {}
		note_geom_t(bars_t start, scales::note offs) : start(start), offs((scales::key_t)offs) {}
		static note_geom_t zero() noexcept { return note_geom_t(bars_t(0, 1), 0); }
		bool operator<(const note_geom_t& rhs) const
		{
			return start == rhs.start
				? offs < rhs.offs
				: start < rhs.start;
		}
		note_geom_t operator+=(const scales::note& n)
		{
			offs += n.value();
			return *this;
		}
		note_geom_t operator+(const note_geom_t& rhs) const {
			return note_geom_t(start + rhs.start, offs + rhs.offs);
		}
		note_geom_t operator-(const note_geom_t& rhs) const {
			return note_geom_t(start - rhs.start, offs - rhs.offs);
		}
	};

	template<class Geom, class ...Children>
	class seg_base : public util::counted_t<Geom, Children...> // : non_copyable_t
	{
		using counted = util::counted_t<Geom, Children...>;
	public:
		// TODO: private and protected accessors?
		using geom_t = Geom;
		geom_t geom = geom_t::zero();

		seg_base(geom_t geom = geom_t::zero()) : geom(geom) {}

	protected:
		template<class ChildType>
		using map_t = std::multimap<geom_t, const ChildType*>;
		// TODO: can the * be omitted?
	public:
		template<class ChildType>
		using pair_t = typename map_t<ChildType>::value_type;

		std::tuple<map_t<Children>...> _children;
	private:
		bars_t _end = bars_t(0, 1), _repeat_end = bars_t(0, 1);
	public:
		bars_t end() const { return _end; }
		bars_t repeat_end() const { return _repeat_end; }

		template<class T>
		map_t<T>& get() {
			return tuple_helpers::get<map_t<T>>(_children);
		}

		template<class T>
		const map_t<T>& get() const {
			return tuple_helpers::get<map_t<T>>(_children);
		}
	protected:
	/*	template<class T, class ...Args>
		T& make(Args ...args) {
			map_t<T>& m = tuple_helpers::get<map_t<T>>(children);
			return m.emplace(std::piecewise_construct,
				std::forward_as_tuple(T),
				std::forward_as_tuple(Args...)).second;
		}*/
		template<class T, class StoreT = T>
		void add(const T& t, const geom_t& geom) {
			map_t<StoreT>& m = get<StoreT>();
			m.emplace(geom, new T(t)); // TODO: push back pointer, id, ... ?
			_repeat_end = _end = std::max(_end, geom.start + t.length());
		}
	};

#if 0
	struct note_event_t {
		int inst_id; sample_no_t pos;
	public:
		note_event_t(int inst_id, sample_no_t pos) : inst_id(inst_id), pos(pos) {}
	};
#endif

	template<class NoteProperties>
	class event_t : public NoteProperties
	{
		bars_t _length = bars_t(1, 1); // TODO: //(1 bars::_1);
	public:
		bars_t length() const { return _length; }
		event_t& operator*=(const bars_t& b) { _length *= b; return *this; }
		event_t operator*(const bars_t& b) const { event_t r = *this; return r*=b; }
	};

	/*class event_t : public seg_base<note_geom_t> {
		sample_no_t propagate() const { return geom.start; } // TODO: also propagate end?
	public:
		note_data_t n;
		using seg_base::seg_base;
	};*/
	
/*	class single_events_t
	{
		bars_t& length;
		event_t& info;
		single_events_t(const bars_t& length, const event_t& info) :
			length(length), info(info)
		{

		}
	};*/

	template<class T>
	class insert_seq
	{
		T cur_e;
		bars_t cur_pos = bars_t(0, 1);
	public:
		insert_seq& operator<<(const T& new_e) {
			cur_e.add_notes(new_e, note_geom_t(cur_pos, 0));
			cur_pos += new_e.repeat_end();
			return *this;
		}
	};

	//! just notes, not corresponding to any instrument
	template<class NoteProperties>
	class events_t : public seg_base<note_geom_t, events_t<NoteProperties>,
		event_t<NoteProperties>>
	{
		using mevents_t = events_t<NoteProperties>;
		using mevent_t = event_t<NoteProperties>;
		using base = seg_base<note_geom_t, events_t<NoteProperties>,
			event_t<NoteProperties>>;
		using geom_t = typename base::geom_t;
		bars_t propagate(bars_t /*note*/) const { return base::geom.start; /*TODO: note*/ }
	public:
		void add_notes(const mevents_t& notes, geom_t other_geom)
		{
			for(const auto pr : notes.template get<mevent_t>())
			{
				add_note(*pr.second, other_geom - base::geom + pr.first);
			}
		}

		void add_note(const mevent_t& n, geom_t geom = geom_t::zero()) { base::template add<mevent_t>(n, geom); }
		/*events_t(const note_geom_t& geom) : seg_base(geom) {
			add_note(event_t(), note_geom_t(std::numeric_limits<bars_t>::max(), 0));
		}*/
		using seg_base<note_geom_t, events_t<NoteProperties>,
			event_t<NoteProperties>>::seg_base;
		events_t operator*(const bars_t& b) const
		{
			events_t copy = *this;
			
			typename base::template map_t<mevent_t> new_e;
			for(auto pr : copy.template get<mevent_t>())
			 new_e.emplace(pr.first, new mevent_t((*pr.second) * b));
			copy.template get<mevent_t>() = new_e;
			
			typename base::template map_t<mevents_t> new_es;
			for(auto pr : copy.template get<mevents_t>())
			 new_es.emplace(pr.first, new mevents_t((*pr.second) * b));
			copy.template get<mevents_t>() = new_es;

			return copy;
		}

//		event_t& note(note_geom_t geom) { return make<event_t>(geom); }
//		events_t& notes(note_geom_t geom) { return make<events_t>(geom); }
		insert_seq<mevents_t> operator<<(const mevents_t& n) { // TODO: std::forward?
			return insert_seq<mevents_t>() << *this << n;
#if 0		
			add_notes(std::move(n), geom_t(bars_t(0, 1), 0));
			// TODO: recall last position
			return *this;
#endif
		}
	};
	
	// TODO: move?
	template<class T>
	events_t<T> operator*(const bars_t& b, const events_t<T>& e) {
		return e * b;
	}
/*
	operator*(const bars_t& b, const scales::note& n)
	{
		return single_events_t(b, n);
	}
*/

}


}

#endif // DAW_H
