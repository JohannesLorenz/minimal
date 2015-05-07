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

#ifndef PROJECT_H
#define PROJECT_H

#include <sys/types.h>

#include <vector>
#include <map>
#include <algorithm>

#include "instrument.h"
#include "tuple_helpers.h"
#include "effect.h"

namespace mini
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

class effect_root_t : public effect_t
{
	void instantiate() {}
	void clean_up() {}
	bool _proceed(sample_t ) { return true; }
public:
	effect_root_t(effect_root_t&& ) = default;
	effect_root_t() : effect_t("effect root") {}
};

//! Consists of all data which is needed to serialize a project.
//! This class will never be instantiated in an so file
class project_t : public util::non_copyable_t
{
	// note: if you add any variable here, got to operator= !
	bool finalized = true;
	bool valid = true;
	float _tempo = 140.0;
	std::string _title;
//	std::vector<std::unique_ptr<instrument_t>> _instruments;
	std::vector<effect_t*> _effects;
//	std::vector<track_t> _tracks;
//	daw::global_t _global;

	effect_root_t _effect_root;

public:
	effect_root_t& effect_root() { return _effect_root; }

	project_t();
	~project_t();

	project_t(project_t&& ) = default;
	project_t& operator=(project_t&& p) noexcept
	{
		// this is a punishment...
		finalized = p.finalized;
		valid = p.valid;
		_tempo = p._tempo;
		_title = std::move(p._title);
		_effects = std::move(p._effects);
		return *this;
	}
	project_t& operator=(const project_t& p) = delete;

#if 0
	project_t(project_t&& other):
		_instruments(std::move(other._instruments))/*,
		_tracks(std::move(other._tracks))*/
	{
	}
#endif

//	daw::global_t& global() { return _global; }
	const std::vector<effect_t*>& effects() const { return _effects; }
	//! this should rarely be used!
	std::vector<effect_t*>& get_effects_noconst() { return _effects; }

//	const std::vector<std::unique_ptr<instrument_t>>& instruments() const {
//		return _instruments; }
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
		//_instruments.emplace_back(new T(args...));
		//return static_cast<T&>(*_instruments.back()); // TODO: correct cast?
		effect_t* ef = new T(args...);
//		ef->set_id(_effects.size());
		_effects.push_back(ef);
		return static_cast<T&>(*ef);
	}

	void finalize();

//	track_t& add_track(const instrument_t& ins);

	void invalidate() { valid = false; }
};

}

#endif // PROJECT_H
