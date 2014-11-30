/*************************************************************************/
/* test.cpp - test files for minimal                                     */
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
#include "lfo.h"
#include "rtosc_string.h"

using namespace mini;

int main()
{
/*	zynaddsubfx_t::note_on<> note_on(1, 2, 3);
	inspect_rtosc_string(note_on.buffer);
	note_on.complete_buffer();
	inspect_rtosc_string(note_on.buffer);*/

	try {

		command<ofloat<>, oint<>> fl("/float", 42.0, 1);
		//inspect_rtosc_string(fl.buffer);
		fl.complete_buffer().inspect();

		lfo_t lfo;
		command<oint<>, ofloat<out_port<float>>, oint<>> c2("/test2", 16384, lfo.out, 0);
		c2._buffer.inspect();
		c2.complete_buffer().inspect();

	} catch (const char* s)
	{
		std::cerr << s << std::endl;
	}

	return 0;
}

