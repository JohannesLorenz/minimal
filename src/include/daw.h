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

#ifndef DAW_H
#define DAW_H

#include <iosfwd>
#include <tuple>
#include <map>



#include "io.h" // TODO !!!


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
		note_geom_t& operator+=(const note_geom_t& rhs) {
			start += rhs.start; offs += rhs.offs;
			return *this;
		}
		note_geom_t& operator-=(const note_geom_t& rhs) {
			start -= rhs.start; offs -= rhs.offs;
			return *this;
		}
		note_geom_t operator+(const note_geom_t& rhs) const {
			return note_geom_t(start + rhs.start, offs + rhs.offs);
		}
		note_geom_t operator-(const note_geom_t& rhs) const {
			return note_geom_t(start - rhs.start, offs - rhs.offs);
		}

		friend std::ostream& operator<<(std::ostream& os, const note_geom_t& n);
	};
#if 0
	// http://stackoverflow.com/questions/1005476/
	// how-to-detect-whether-there-is-a-specific-member-variable-in-class
	template<typename T>
	struct has_geom
	{
		struct fallback { int geom; }; // introduce member name "x"
		struct derived : T, fallback { };

		template<typename C, C> struct ch_t;

		template<typename C> static char (&f(ch_t<int fallback::*, &C::geom>*))[1];
		template<typename C> static char (&f(...))[2];

		static bool const value = sizeof(f<derived>(0)) == 2;
	};

	template<bool>
	struct add_geom_of {
		template<class T, class G>
		static void exec(G& value, const T& segment) { value += segment.geom; }
	};
	template<>
	struct add_geom_of<false> {
		template<class T, class G>
		static void exec(G& , const T&) {}
	};
#endif

/*	namespace detail
	{
		struct multiplier_t
		{
			bars_t repeat_end = bars_t(0, 1);
			bars_t value;

			template<class T>
			void operator()(T& child)
			{
				for(typename T::value_type& pr : child)
				 *pr.second *= value;

				repeat_end = std::max(repeat_end, child.rend()->second->length());
				// new_e.emplace(pr.first, new mevent_t((*pr.second) * b));
			}

			multiplier_t(const bars_t& value) : value(value) {}
		};
	}*/


	template<class Geom, class ...Children>
	class seg_base : public util::counted_t<Geom, Children...> // : non_copyable_t
	{
		using counted = util::counted_t<Geom, Children...>;
	public:
		using geom_t = Geom;
	protected:
		geom_t _geom = geom_t::zero();
	public:
		const geom_t& geom() const { return _geom; }

		seg_base(geom_t geom = geom_t::zero()) :
			_geom(geom) {}
	protected:

		template<class T>
		struct child_with_meta
		{
			//bars_t end = 0;
			bars_t factor;
			const T* child;
			child_with_meta(const T* child, const bars_t& factor = 1) :
				factor(factor), child(child) {}
			bars_t recalculate_end() const {
				return /*end =*/ child->recalculate_end(factor);
			}
		};

		// we use pointers here to share children between segments
		template<class ChildType>
		using map_t = std::multimap<geom_t, child_with_meta<ChildType>>;
	public:
		template<class ChildType>
		using pair_t = typename map_t<ChildType>::value_type;
	private:
		std::tuple<map_t<Children>...> _children;

		bars_t /*_end = bars_t(0, 1),*/ _repeat_end = bars_t(0, 1);

		bars_t factor = bars_t(1, 1);

		struct end_recalculator_t
		{
			bars_t factor, end = 0;
			end_recalculator_t(bars_t factor) : factor(factor) {}
			template<class T>
			void operator()(const T& child_map)
			{
				for(const auto& pr : child_map)
				{
					std::cerr << "end_recalc : " << end << ", " << pr.first.start + pr.second.recalculate_end()
						<< std::endl;
					end = std::max(end, pr.first.start + pr.second.recalculate_end());
				}
			}
		};

	public:
//		bars_t end() const { return _end; }
		bars_t repeat_end() const { return _repeat_end; }

		bars_t recalculate_end(const bars_t& factor) const
		{
			end_recalculator_t end_recalculator(factor);
			tuple_helpers::for_each(_children, end_recalculator);
			std::cerr << "recalc_end: " << end_recalculator.end << ", " << geom().start << std::endl;
			return end_recalculator.end + geom().start;
		}

		// this is mostly needed if the user glues notes together
		// with operator<<
		// once the real end should be needed, we should use
		// length() and repeat_length()
		bars_t length() const { return _repeat_end; }

		template<class T>
		map_t<T>& get() {
			return tuple_helpers::get<map_t<T>>(_children);
		}

		template<class T>
		const map_t<T>& get() const {
			return tuple_helpers::get<map_t<T>>(_children);
		}

		using self_t = seg_base<Geom, Children...>;
		self_t operator+=(scales::note n) { _geom += n; return *this; }

	protected:
	/*	template<class T, class ...Args>
		T& make(Args ...args) {
			map_t<T>& m = tuple_helpers::get<map_t<T>>(children);
			return m.emplace(std::piecewise_construct,
				std::forward_as_tuple(T),
				std::forward_as_tuple(Args...)).second;
		}*/


	public:
		template<class T, class StoreT = T>
		void add(const T& t, const geom_t& geom, const bars_t& _factor = bars_t(1, 1))
		{
			map_t<StoreT>& m = get<StoreT>();
#if 0
			add_geom_of<has_geom<T>::value>::exec(geom, t);
#endif
			m.emplace(geom, child_with_meta<T>(new T(t), _factor)); // TODO: do not make a copy
			// TODO: set to NULL if this pointer may not be deleted anymore!

			_repeat_end = std::max(_repeat_end, geom.start + t.length());

			io::mlog << "repeatend: " << _repeat_end << io::endl;
		}

#if 0
		self_t& operator*=(const bars_t& b)
		{
			detail::multiplier_t multiplier(b);

			tuple_helpers::for_each(_children, multiplier);
			_repeat_end = multiplier.repeat_end;

#if 0
			typename base::template map_t<mevent_t> new_e;
			for(auto pr : copy.template get<mevent_t>())
			 new_e.emplace(pr.first, new mevent_t((*pr.second) * b));
			get<mevent_t>() = new_e;

			typename base::template map_t<mevents_t> new_es;
			for(auto pr : copy.template get<mevents_t>())
			 new_es.emplace(pr.first, new mevents_t((*pr.second) * b));
			get<mevents_t>() = new_es;

			const std::pair<const note_geom_t, const mevent_t*>& last_event = *copy.template get<mevent_t>().rend();
			const std::pair<const note_geom_t, const mevents_t*>& last_events = *copy.template get<mevents_t>().rend();

			copy._repeat_end = std::max(last_event.first.start + last_event.second->length(),
						last_events.first.start + last_events.second->length());

			return copy;
#endif
		}
#endif
		template<class Geom2, class ...Children2>
		friend std::ostream& operator<<(std::ostream& os, const seg_base<Geom2, Children2...>& sb);
	};


	namespace detail
	{
		struct print_all
		{
			std::ostream* os;
			static std::size_t depth;
			template<class T>
			void operator()(const T& map)
			{
				++depth;
				for(const auto& pr : map)
				{
					for(std::size_t d2 = depth; d2; --d2)
					 *os << "  ";
					*os << pr.first << ": " << *pr.second.child << std::endl;
				}
				--depth;
			}
		};
	}

	template<class Geom, class ...Children>
	std::ostream& operator<<(std::ostream& os, const seg_base<Geom, Children...>& sb)
	{
		detail::print_all printer { &os };
		tuple_helpers::for_each(sb._children, printer);
		return os;
	}


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
		bars_t _length = bars_t(1, 1); // FEATURE: //(1 bars::_1);
	public:
		bars_t length() const { return _length; }
		bars_t recalculate_end(const bars_t& factor) const { return _length * factor; }
		event_t& operator*=(const bars_t& b) { _length *= b; return *this; }
		event_t operator*(const bars_t& b) const { event_t r = *this; return r*=b; }
	};

	/*class event_t : public seg_base<note_geom_t> {
		sample_no_t propagate() const { return geom.start; } ... also propagate end?
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

	struct scale
	{
		bars_t _factor;
	public:
		scale(const bars_t& factor) : _factor(factor) {}
		const bars_t& factor() const { return _factor; }
	};

	//! allows inserting notes *one after another*
	//! also recalls the current insertion position
	template<class T, class T2>
	class insert_seq
	{
		T* cur_e;
		bars_t cur_pos = bars_t(0, 1);
		bars_t factor = bars_t(1, 1);

		template<class T3>
		insert_seq& add_with_offset(const T3& new_e, note_offset_t no, bars_t length) {
			cur_e->template add<T3>(new_e, note_geom_t(cur_pos, no), factor);
			cur_pos += length;
			return *this;
		}
	public:
		insert_seq(T& e) : cur_e(&e) {}

		template<class T3>
		insert_seq& operator<<(const T3& new_e) {
			return add_with_offset<T3>(new_e, 0, new_e.recalculate_end(factor));
		}

		insert_seq& operator<<(const scale& sc) { factor = sc.factor(); return *this; }

		insert_seq& operator<<(const scales::note& n)
		{
			T2* ptr = new T2();
			add_with_offset<T2>(*ptr, n.value(), factor);
			if(ptr) delete ptr;
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
	public:
		using base = seg_base<note_geom_t, events_t<NoteProperties>,
			event_t<NoteProperties>>;
	private:
		using geom_t = typename base::geom_t;
//		bars_t propagate(bars_t /*note*/) const { return base::geom.start; ... or note ? }
	public:
		void add_notes(const mevents_t& notes, geom_t other_geom)
		{
			//io::mlog << "add_notes: " << notes.geom() << ", " << other_geom << io::endl;
			base::template add<mevents_t>(notes, other_geom);
		}

		void add_note(const mevent_t& n, geom_t geom = geom_t::zero()) { base::template add<mevent_t>(n, geom); }
		/*events_t(const note_geom_t& geom) : seg_base(geom) {
			add_note(event_t(), note_geom_t(std::numeric_limits<bars_t>::max(), 0));
		}*/
		using seg_base<note_geom_t, events_t<NoteProperties>,
			event_t<NoteProperties>>::seg_base;
#if 0
		events_t operator*(const bars_t& b) const
		{
			mevents_t copy = *this;
			copy *= b;
#if 0
			typename base::template map_t<mevent_t> new_e;
			for(auto pr : copy.template get<mevent_t>())
			 new_e.emplace(pr.first, new mevent_t((*pr.second) * b));
			copy.template get<mevent_t>() = new_e;
			
			typename base::template map_t<mevents_t> new_es;
			for(auto pr : copy.template get<mevents_t>())
			 new_es.emplace(pr.first, new mevents_t((*pr.second) * b));
			copy.template get<mevents_t>() = new_es;

/*			const std::pair<const note_geom_t, const mevent_t*>& last_event = *copy.template get<mevent_t>().rend();
			const std::pair<const note_geom_t, const mevents_t*>& last_events = *copy.template get<mevents_t>().rend();

			copy._repeat_end = std::max(last_event.first.start + last_event.second->length(),
						last_events.first.start + last_events.second->length());
*/
#endif
			return copy;
		}
#endif
//		event_t& note(note_geom_t geom) { return make<event_t>(geom); }
//		events_t& notes(note_geom_t geom) { return make<events_t>(geom); }

		/*
		TODO: instead, allow cast to insertion sequence?
		*/
		insert_seq<mevents_t, mevent_t> operator<<(const mevents_t& n) { // TODO: std::forward?
			return insert_seq<mevents_t, mevent_t>(*this) << n;
		}

		insert_seq<mevents_t, mevent_t> operator<<(const scale& sc) { // TODO: std::forward?
			return insert_seq<mevents_t, mevent_t>(*this) << sc;
		}

		std::ostream& dump(std::ostream& os = std::cerr) const
		{
			//return os << "events +" << base::geom << ":" << std::endl;
			return os << *this;
		}

	//	friend std::ostream& operator<<(std::ostream& os, const events_t<NoteProperties>& e);
	};

	template<class N>
	std::ostream& operator<<(std::ostream& os, const events_t<N>& e)
	{
		os << "events +" << e.geom() << ":" << std::endl;
		os << (const typename events_t<N>::base&)e;
		return os;
	}
	
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
