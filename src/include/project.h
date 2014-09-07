/*************************************************************************/
/* mmms - minimal multimedia studio                                      */
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

#ifndef PROJECT_H
#define PROJECT_H

#include <vector>
#include <map>

#include "instrument.h"
#include "utils.h"
#include "tuple_helpers.h"

namespace mmms
{

using octave_t = std::size_t;
using key_note_t = octave_t; // TODO: class!

/*enum key_note_t // TODO: enum_class?
{ // TODO: operator++?
	c,
	d,
	e,
	f,
	g,
	a,
	h,
};*/

class key_t
{
	int key;
	static const unsigned octave_len = 13;
public:
	key_t(const octave_t& o, const key_note_t& k)
	: key(o*13 + k)
	{
	}
	operator int() const { return key; }
};

class timeline_t
{

};

class line_t
{
	std::vector<int> pos;
public:
	template<class ...PosClass>
	line_t(PosClass&& ...positions) :
		pos { positions... }
	{
	}
};

// concept
template<class ScaleType>
class segment_t
{
	using scale_t = ScaleType;
	scale_t _begin, _end;
	//segment_t parent;
	std::vector<segment_t> lines;
	bool constonly = false;
public:

};


namespace daw
{
	class geom_t
	{
	public:
		float start;
		geom_t(float start) : start(start) {}
		static geom_t zero() { return geom_t(0.0f); }
	};

	class note_geom_t
	{
	public:
		float start;
		int offs;
		note_geom_t(float start, int offs) : start(start), offs(offs) {}
	};

	template<class Geom, class ...Children>
	class seg_base // : non_copyable_t
	{
	public:
		// TODO: private and protected accessors?
		Geom geom;
		seg_base(Geom geom) : geom(geom) {}
	protected:
		std::tuple<std::vector<Children>...> children;
		template<class T, class ...Args>
		T& make(Args ...args) {
			auto& v = tuple_helpers::get<std::vector<T>>(children);
			v.emplace_back(args...);
			return v.back();
		}
		template<class T>
		void add(const T& t) {
			auto& v = tuple_helpers::get<std::vector<T>>(children);
			v.push_back(t); // TODO: push back pointer, id, ... ?
		}
	};


	/*template<class Self, class ...Children>
	class daw_base
	{
	};*/

	struct note_event_t {
		int inst_id; float pos;
	public:
		note_event_t(int inst_id, float pos) : inst_id(inst_id), pos(pos) {}
	};

	class note_t : seg_base<note_geom_t> {
		float propagate() const { return geom.start; } // TODO: also propagate end?
		using seg_base::seg_base;
	public:
	};

	//! just notes, not corresponding to any instrument
	class notes_t : seg_base<note_geom_t, notes_t, note_t>
	{
		float propagate(float /*note*/) const { return geom.start; /*TODO: note*/ }
		using seg_base::seg_base;
	public:
		note_t& note(note_geom_t geom) { return make<note_t>(geom); }
		notes_t& notes(note_geom_t geom) { return make<notes_t>(geom); }
	};

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
	class track_t : public seg_base<geom_t, track_t, notes_t>, note_event_propagator<track_t>
	{
		using child_type = note_t;
		using seg_base::seg_base;
		instrument_t::id_t id;
	public:
		void add_notes(const notes& n, geom_t geom) { add<notes_t>(geom); } // TODO: add with geom (maybe default arg?)
		notes_t& notes(note_geom_t geom) { return make<notes_t>(geom); }
		//track_t(instrument_t::id_t id) : id(id) {}
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
	class global_t : seg_base<geom_t, global_t, track_t>, note_event_propagator<track_t>
	{
		using child_type = track_t; // TODO: unused?
	public:
		void add_track(const track& t) { add<track_t>(geom); }
		track_t& track(geom_t geom) { return make<track_t>(geom); }
		global_t& global(geom_t geom) { return make<global_t>(geom); }
		global_t(geom_t g = geom_t{0}) : seg_base(g) {}
	};


}

//template<class Self, class Child>
//Child cseg(Self& );




/*

class track_t
{
	const instrument_t::id_t instr_id;
	std::map<key_t, line_t> lines;
public:
	void add_line(octave_t octave, key_note_t key, line_t&& line)
	{
		add_line(key_t(octave, key), std::move(line));
	}
	void add_line(key_t key, line_t&& line)
	{
		lines[key] = line;
	}
	// TODO: disallow ctor for user
	track_t(const instrument_t::id_t& instr_id);
};*/

//! Consists of all data which is needed to serialize a project.
//! This class will never be instantiated in an so file
class project_t : non_copyable_t
{
	bool valid = true;
	float _tempo = 140.0;
	std::string _title;
	std::vector<std::unique_ptr<instrument_t>> _instruments;
//	std::vector<track_t> _tracks;
	daw::global_t _global;
public:
	project_t();
	~project_t();
	project_t(project_t&& other):
		_instruments(std::move(other._instruments))/*,
		_tracks(std::move(other._tracks))*/
	{
	}

	daw::global_t& global() { return _global; }

	const std::vector<std::unique_ptr<instrument_t>>& instruments() const {
		return _instruments; }
//	const std::vector<track_t>& tracks() const { return _tracks; }

	void set_tempo(float tempo) { _tempo = tempo; }
	float tempo() const { return _tempo; }
	//void set_tempo(unsigned short tempo) { _tempo = tempo; }
	void set_title(const char* title) { _title = title; }
	void set_title(const std::string& title) { _title = title; }
	const std::string& title() const { return _title; }

//	void add_track(const track_t& track) { _tracks.push_back(track); }
	// todo: can we avoid add instrument, by using add track?
/*	template<class T>
	void add_instrument(const T&& ins) {
		_instruments.push_back(std::unique_ptr<T>(std::move(ins)));
	}*/
	template<class T, class ...Args>
	T& emplace(Args ...args) {
		_instruments.emplace_back(new T(args...));
		return static_cast<T&>(*_instruments.back()); // TODO: correct cast?
	}
//	track_t& add_track(const instrument_t& ins);

	void invalidate() { valid = false; }
};

}

#endif // PROJECT_H
