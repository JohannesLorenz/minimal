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
	zynaddsubfx_t sine_bass;
	sine_bass.add_param_fixed("/bla", 1, "zwei");
	p.add_instrument(&sine_bass);

	// tracks
	track_t track1(sine_bass);
	//t.add_timeline(~~)
	track1.add_line(1,1, line_t(1,2,3));
//	t.set_param_fixed("", 3); // TODO: instrument
}

}

