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

#ifndef NOTES_H
#define NOTES_H

// TODO: forw decl?
#include "mports.h"
#include "event_roll.h"
#include "daw.h"

namespace mini {

namespace daw { // TODO: namespace daw correct here?

//! value() = velocity
class music_note_properties : public value_t<char, 64>
{
public:
	using value_t<char, 64>::value_t;
	// TODO: operator<<, print char with '+'
};

class note_t : public event_t<music_note_properties>
{
};

using volume_t = unsigned char;

inline note_t operator*(note_t&& n, volume_t& v)
{
	n.set(v);
	return n;
}

/*class notes_t : public events_t<music_note_properties>
{
public:
	using events_t<music_note_properties>::events_t;
};*/
using notes_t = events_t<music_note_properties>;

notes_t operator+(const notes_t& no, const scales::note& n) {
	notes_t copy = no;
	/*for(auto& pr : get<note_t>())
	 pr.first += n;*/
	copy += n;
	return copy;
}

// swap multiplicands
template<class T> // TODO: std::forward?
T operator*(const T& other, const note_t& n) {
	return n * other;
}

}
using note_signal_t = event_signal_t<daw::music_note_properties>;
using piano_roll_t = event_roll_t<daw::music_note_properties>;
using notes_in = events_in_t<daw::music_note_properties>;
using notes_out = events_out_t<daw::music_note_properties>;

}

#endif // NOTES_H

