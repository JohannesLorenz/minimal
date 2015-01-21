/*************************************************************************/
/* demo.cpp - a small demo song for minimal                              */
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

#include "zynaddsubfx.h"
#include "project.h"
#include "lfo.h"
#include "note_line.h"

using namespace mini;

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

//	sine_bass.add_command_fixed<zynaddsubfx_t::note_on<>>(0, 42, 10);



/*	// tracks
	track_t& track1 = p.add_track(sine_bass);
	//t.add_timeline(~~)
	track1.add_line(1,1, line_t(1,2,3));
//	p.add_track(track1);
	//t.set_param_fixed("", 3); // TODO: instrument*/

	using namespace daw;

/*	notes_t maj(note_geom_t(0, 0)); // TODO: no arg
	maj.note(note_geom_t(0, 0));
	maj.note(note_geom_t(4, 1));
	maj.note(note_geom_t(7, 2));

	global_t& global = p.global();
	track_t& t1 = global.track(geom_t(0)); // TODO: tempo, channel, instr
	notes_t& notes = t1.notes(note_geom_t(0, 42)); // start from note "42"

	// 4 major chords
	notes.notes(note_geom_t(0, 0)) = maj;
	notes.notes(note_geom_t(2, 1)) = maj;
	notes.notes(note_geom_t(4, 2)) = maj;
	notes.notes(note_geom_t(5, 3)) = maj;*/

	notes_t maj(note_geom_t(0, 0));
	maj.add_note(note_t(), note_geom_t(0, 0));
	maj.add_note(note_t(), note_geom_t(0.33, 1));
	maj.add_note(note_t(), note_geom_t(0.67, 2));

	note_line_t* nl = new note_line_t();
	nl->add_notes(maj, note_geom_t(0, 62));
	nl->add_notes(maj, note_geom_t(1, 63));
	nl->add_notes(maj, note_geom_t(2, 64));
	nl->add_notes(maj, note_geom_t(3, 65));

#if 0
	track_t t1/*(sine_bass)*/;
	// 4 major chords
	t1.add_notes(maj, note_geom_t(0, 62));
	t1.add_notes(maj, note_geom_t(1, 63));
	t1.add_notes(maj, note_geom_t(2, 64));
	t1.add_notes(maj, note_geom_t(3, 65));
#endif

/*	lfo_t* m_lfo = new lfo_t;
	p.effects().push_back(m_lfo);

	command<oint<out_port<float>>> cmd("/part0/kit0/adpars/global/AmpEnvelope/Penvsustain", m_lfo->out);
//	t1.add_command(cmd);
*/
	lfo_t<int>* m_lfo = new lfo_t<int>(0.0, 127.0, 0.0, 4.0);
	p.effects().push_back(m_lfo);
	p.effects().push_back(nl);
//	in_port<int> ip(sine_bass);
//	ip.connect(m_lfo->out);
	auto envsustain = sine_bass.add0().global().amp_env().envsustain<in_port_templ<int>>(); // todo: need discretizer

	// effect connections
	*envsustain << *m_lfo;
	sine_bass.note_input() << (const out_port_templ<note_signal_t>&)*nl;

//	t1.add_command(cmd);



//	global_t& global = p.global();
//	global.add_track(t1);


	// PEnable
	// "part0/kit0/adpars/voice0/AmpEnvelope/Penvsustain:i"

/*	const auto ftor = [](int c) { return c % 100 + 50; };
	func<decltype(ftor), p_char, int> f(ftor);
	zynaddsubfx_t::note_on cmd(f, 2, 3);*/
}

}

