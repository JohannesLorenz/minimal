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
#include "osc_string.h"
#include "zynaddsubfx.h"

using namespace mini;

int main()
{
/*	zynaddsubfx_t::note_on<> note_on(1, 2, 3);
	inspect_rtosc_string(note_on.buffer);
	note_on.complete_buffer();
	inspect_rtosc_string(note_on.buffer);*/

	try {
		self_port_templ<int>* port = new self_port_templ<int>();
		port->set(123);

		command<osc_float, vint<self_port_templ<int>>> fl("/float", 42.0, port);
		//inspect_rtosc_string(fl.buffer);

		fl.buffer().inspect();

		fl.complete_buffer().inspect();

		std::cerr << "pad size: " << mini::pad_size<vint<in_port_templ<int>>>::value() << std::endl;



		using m_note_on_t = zynaddsubfx_t::note_on<use_no_port, use_no_port, self_port_templ>;

		m_note_on_t non(0, 1, 0);
		non.buffer().inspect();



		std::get<2>(non.in_ports).set(99);

		non.command::update();

		std::cerr << "ARG NOW:" << std::get<2>(non.args) << std::endl;

		non.complete_buffer();
		non.buffer().inspect();
#if 0 // TODO
		lfo_t<> lfo(-42.0, +42.0, 0.0f, 4.0f);
		command<oint<>, ofloat<in_port<float>>, oint<>> c2("/test2", 16384, lfo.out, 0);
		c2._buffer.inspect();
		c2.complete_buffer().inspect();
#endif
	} catch (const char* s)
	{
		std::cerr << s << std::endl;
	}

	return 0;
}

