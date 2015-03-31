/*************************************************************************/
/* test.cpp - test files for minimal                                     */
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
		command<osc_float, self_port_templ<int>> fl("/float", 42.0, self_port_templ<int>{});
		//inspect_rtosc_string(fl.buffer);

		fl.buffer().inspect();

		fl.port_at<1>().set(123);
		fl.command::update();

		fl.complete_buffer().inspect();
		
		assert(!fl.buffer().data().compare("/float\0\0,fi\0\0", 12));

	//	std::cerr << "pad size: " << mini::pad_size<vint>::value() << std::endl;



		using m_note_on_t = zynaddsubfx_t::note_on<use_no_port, use_no_port, self_port_templ>;

		zynaddsubfx_t z("hello world!");
		m_note_on_t non(&z, 0, 1, self_port_templ<int, true>{});
		non.cmd_ptr->buffer().inspect();



		non.cmd_ptr->port_at<2>().set(99);

		non.cmd_ptr->command::update();

	//	std::cerr << "ARG NOW:" << std::get<2>(non.args) << std::endl;

		non.cmd_ptr->complete_buffer();
		non.cmd_ptr->buffer().inspect();
		
		
		command<const char*> str_cmd("/string", "hello world!");
		str_cmd.buffer().inspect();
		str_cmd.complete_buffer().inspect();

	{
		command<osc_float, bool, bool, self_port_templ<bool>, osc_int> has_bool("/bool", 42.0, false, true, self_port_templ<bool>{}, 42);
		//inspect_rtosc_string(fl.buffer);

		has_bool.buffer().inspect();

		has_bool.port_at<3>().set(true);
		has_bool.command::update();

		has_bool.complete_buffer().inspect();
	}

	} catch (const char* s)
	{
		std::cerr << s << std::endl;
		return 1;
	}

	std::cerr << "SUCCESS" << std::endl;
	return 0;
}

