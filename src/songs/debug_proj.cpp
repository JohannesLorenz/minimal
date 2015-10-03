/*************************************************************************/
/* debug_proj.cpp - a small demo project for debugging minimal           */
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

#include "debug.h"
/*#include "jack_player.h"*/
#include "project.h"
#include "bars.h"
//#include "lfo.h"
//#include "note_line.h"

using namespace mini;
using namespace bars;

extern "C"
{

void init(project_t& p)
{
	// general
	p.set_tempo(140);
	p.set_title("debug project");

	// effects
	start_t& start = p.emplace<start_t>();
	pipe_t& p1 = p.emplace<pipe_t>(3, 15);
	pipe_t& p2 = p.emplace<pipe_t>(7, 40);
	in2_t& sink = p.emplace<in2_t>();

	// effect connections
	p1 << start;
	p2 << start;
	(int_in_1&)sink << p1; // -2 is global
	(int_in_2&)sink << p2;

	p.sink << sink;
//	player << sine_bass;

	// PEnable
	// "part0/kit0/adpars/voice0/AmpEnvelope/Penvsustain:i"

/*	const auto ftor = [](int c) { return c % 100 + 50; };
	func<decltype(ftor), p_char, int> f(ftor);
	zynaddsubfx_t::note_on cmd(f, 2, 3);*/
}

}

