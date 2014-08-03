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

class track_t
{
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
};

class project_t
{
	bool valid = true;
	float _tempo = 140.0;
	const char* _title;
	std::vector<track_t> tracks;
public:
	project_t();
	void set_tempo(float tempo) { _tempo = tempo; }
	float tempo() const { return _tempo; }
	//void set_tempo(unsigned short tempo) { _tempo = tempo; }
	void set_title(const char* title) { _title = title; }
	const char* title() const { return _title; }

	void add_track(track_t&& track) { tracks.emplace_back(track); }
	void invalidate() { valid = false; }
};

}

#endif // PROJECT_H
