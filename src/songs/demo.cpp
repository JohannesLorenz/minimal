/*************************************************************************/
/* demo.cpp - a small demo song for minimal                              */
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
	zyn_tree_t& sine_bass = p.emplace<zynaddsubfx_t>("sine bass");
	//sine_bass.add_param_fixed("/bla", 1, std::string("zwei"));

//	sine_bass.add_command_fixed<zynaddsubfx_t::note_on<>>(0, 42, 10);


/*	// tracks
	track_t& track1 = p.add_track(sine_bass);
	//t.add_timeline(~~)
	track1.add_line(1,1, line_t(1,2,3));
//	p.add_track(track1);
	//t.set_param_fixed("", 3); // TODO: instrument*/

	using namespace daw;

	notes_t maj(note_geom_t(0, 0));
	maj.add_note(note_t(), note_geom_t(0, 0));
	maj.add_note(note_t(), note_geom_t(0.33, 1));
	maj.add_note(note_t(), note_geom_t(0.67, 2));

	// 8 major chords
	note_line_t& nl = p.emplace<note_line_t>();
	nl.add_notes(maj, note_geom_t(0, 62));
	nl.add_notes(maj, note_geom_t(1, 63));
	nl.add_notes(maj, note_geom_t(2, 64));
	nl.add_notes(maj, note_geom_t(3, 65));
	nl.add_notes(maj, note_geom_t(4, 66));
	nl.add_notes(maj, note_geom_t(5, 67));
	nl.add_notes(maj, note_geom_t(6, 68));
	nl.add_notes(maj, note_geom_t(7, 69));

	lfo_t<int>& m_lfo = p.emplace<lfo_t<int>>(0.0, 64.0, 0.0, 8.);

	lfo_t<int>& constant_0 = p.emplace<lfo_t<int>>(0.0, 0.0, std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), 1.0f, 0.0f);
	lfo_t<int>& constant_1 = p.emplace<lfo_t<int>>(0.0, 0.0, std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), 1.0f, 1.0f);
	lfo_t<int>& constant_m2 = p.emplace<lfo_t<int>>(0.0, 0.0, std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), 1.0f, -2.0f);

//	in_port<int> ip(sine_bass);
//	ip.connect(m_lfo->out);

	//zyn::p_envsustain<in_port_templ<int>>* envsustain = sine_bass.add0().global().amp_env().envsustain<in_port_templ<int>>(); // todo: need discretizer
	auto* volume = sine_bass.volume<in_port_templ<int>>();
	auto* panning = sine_bass.part0().Ppanning<in_port_templ<int>>();
	auto* ins_fx_i = sine_bass.insefx<>().efftype<in_port_templ<int>>();
		//sine_bass.part<0>().partefx<0>().efftype<in_port_templ<int>>();
	auto* ins_fx_part = sine_bass.part<0>().partefx<0>().eff0_part_id<in_port_templ<int>>();

	// effect connections
	volume->cmd_ptr->port_at<0>() << m_lfo;
	panning->cmd_ptr->port_at<0>() << constant_0;
	ins_fx_part->cmd_ptr->port_at<0>() << constant_m2; // -2 is global
	ins_fx_i->cmd_ptr->port_at<0>() << constant_1;

	sine_bass.note_input() << nl;

	// PEnable
	// "part0/kit0/adpars/voice0/AmpEnvelope/Penvsustain:i"

/*	const auto ftor = [](int c) { return c % 100 + 50; };
	func<decltype(ftor), p_char, int> f(ftor);
	zynaddsubfx_t::note_on cmd(f, 2, 3);*/
}

}

