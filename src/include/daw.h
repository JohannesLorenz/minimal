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
#include "command.h" // TODO: forward declare?
#include "bars.h"

namespace mini {

//template<class Impl>
class instrument_t;

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

	class note_geom_t
	{
	public:
		bars_t start;
		int offs;
		note_geom_t(bars_t start, int offs) : start(start), offs(offs) {}
		static note_geom_t zero() noexcept { return note_geom_t(bars_t(0, 1), 0); }
		bool operator<(const note_geom_t& rhs) const
		{
			return start == rhs.start
				? offs < rhs.offs
				: start < rhs.start;
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
	public:
		template<class ChildType>
		using pair_t = typename map_t<ChildType>::value_type;

		std::tuple<map_t<Children>...> _children;
	public:
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
		}
	};

#if 0
	struct note_event_t {
		int inst_id; sample_t pos;
	public:
		note_event_t(int inst_id, sample_t pos) : inst_id(inst_id), pos(pos) {}
	};
#endif

	class note_t
	{
		char _velocity = 64;
		bars_t _length = bars::_8;
	public:
		char velocity() const { return _velocity; }
		bars_t length() const { return _length; }
	};

	/*class note_t : public seg_base<note_geom_t> {
		sample_t propagate() const { return geom.start; } // TODO: also propagate end?
	public:
		note_data_t n;
		using seg_base::seg_base;
	};*/

	//! just notes, not corresponding to any instrument
	class notes_t : public seg_base<note_geom_t, notes_t, note_t>
	{
		bars_t propagate(bars_t /*note*/) const { return geom.start; /*TODO: note*/ }
	public:
		void add_notes(const notes_t& notes, geom_t other_geom)
		{
			for(const auto pr : notes.get<note_t>())
			{
				add_note(*pr.second, other_geom - geom + pr.first);
			}
		}

		void add_note(const note_t& n, geom_t geom = geom_t::zero()) { add<note_t>(n, geom); }
		/*notes_t(const note_geom_t& geom) : seg_base(geom) {
			add_note(note_t(), note_geom_t(std::numeric_limits<bars_t>::max(), 0));
		}*/
		using seg_base::seg_base;
//		note_t& note(note_geom_t geom) { return make<note_t>(geom); }
//		notes_t& notes(note_geom_t geom) { return make<notes_t>(geom); }
	};
#if 0
	template<class Child>
	class note_event_propagator
	{
	protected:
		note_event_t propagate(note_event_t ne, Child c) {
			return note_event(ne.inst_id, ne.pos + c.geom.start);
		}
	};

	//! notes, corresponding to single instruments
	//! if you want to use multiple tracks with the same instrument, make them and set them in the ctor
	//! if you want multiple instruments to play the same, make multiple tracks and use the same notes object

	class track_t : public seg_base<note_geom_t, notes_t, command_base>, note_event_propagator<track_t>
	{
		using child_type = note_t;
		//instrument_t::id_t ins_id;
		instrument_t* ins;
	public:
		const instrument_t* instrument() const { return ins; }
		using seg_base::seg_base;
		void add_notes(const notes_t& n, note_geom_t geom = geom_t::zero()) { add<notes_t>(n, geom); }

	//	template<class Command>
	//	void add_command(const Command& cb) { add<Command, command_base>(cb, note_geom_t(0, 0)); }

//		notes_t& notes(note_geom_t geom) { return make<notes_t>(geom); }
		track_t(instrument_t& ins) : /*ins_id(ins.id()),*/ ins(&ins) {}
	};

/*	class inst_list_t : seg_base<inst_list_t, inst_t>, note_event_propagator<inst_t>
	{
		using child_type = inst_t;
		note_event_t propagate(note_event_t ne, inst_t i) {
			return note_event_t(ne.inst_id, ne.pos + i.geom.start);
		}
		using seg_base::seg_base;
	public:
		inst_t& make_inst(geom_t geom) { return make<inst_t>(geom); }
	};

	class chunk_list_t : seg_base<chunk_list_t, inst_list_t>, note_event_propagator<inst_list_t>
	{
		using child_type = inst_list_t;
		using seg_base::seg_base;
	public:
		inst_list_t& make_inst_list(geom_t geom) { return make<inst_list_t>(geom); }
	};*/

	//! a pattern of instruments. e.g., different drum patterns are different globals
	class global_t : public seg_base<geom_t, global_t, track_t>, note_event_propagator<track_t>
	{
		using child_type = track_t; // TODO: unused?
	public:
		using seg_base::seg_base;
		void add_track(const track_t& t) { add<track_t>(t, geom); }
//		track_t& track(geom_t geom) { return make<track_t>(geom); }
//		global_t& global(geom_t geom) { return make<global_t>(geom); }
		global_t(geom_t  = geom_t{0}) /*: seg_base(g)*/ {}
	};
#endif
}

}

#endif // DAW_H
