/*************************************************************************/
/* demo.cpp - a small demo song for mmms                                 */
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

#include "project.h"

using namespace mmms;

extern "C"
{

void init(project_t& p)
{
	// general
	p.set_tempo(140);
	p.set_title("demo-song");

	// instruments
	// TODO: disallow "zynaddsubfx sine_base ...."
	zynaddsubfx_t& sine_bass = p.emplace<zynaddsubfx_t>("sine bass");
	//sine_bass.add_param_fixed("/bla", 1, std::string("zwei"));
//	sine_bass.add_command_fixed<command>();
	sine_bass.add_command_fixed<zynaddsubfx_t::note_on>(0, 42, 10);

/*	// tracks
	track_t& track1 = p.add_track(sine_bass);
	//t.add_timeline(~~)
	track1.add_line(1,1, line_t(1,2,3));
//	p.add_track(track1);
	//t.set_param_fixed("", 3); // TODO: instrument*/

	using namespace daw;

	notes_t maj(note_geom_t(0, 0)); // TODO: no arg
	maj.note(note_geom_t(0, 0));
	maj.note(note_geom_t(4, 1));
	maj.note(note_geom_t(7, 2));

	global_t global;
	track_t& t1 = global.track(geom_t(0)); // TODO: tempo, channel, instr
	notes_t& notes = t1.notes(note_geom_t(0, 42)); // start from note "42"

	// 4 major chords
	notes.notes(note_geom_t(0, 0)) = maj;
	notes.notes(note_geom_t(2, 1)) = maj;
	notes.notes(note_geom_t(4, 2)) = maj;
	notes.notes(note_geom_t(5, 3)) = maj;

	zynaddsubfx_t::note_on cmd();
}

}

