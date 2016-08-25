/*************************************************************************/
/* test files for minimal                                                */
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

#include <algorithm>

#include "project.h"
#include "lfo.h"
#include "osc_string.h"
#include "command.h"
//#include "zynaddsubfx.h"
#include "io.h"

using namespace mini;

bool are_equal(const std::vector<char>& v, const char* ptr, std::size_t n)
{
	for(auto itr = v.begin(); n; --n, ++itr, ++ptr)
	 if(*itr != *ptr)
	  return false;
	return true;
}

int main()
{
/*	zynaddsubfx_t::note_on<> note_on(1, 2, 3);
	inspect_osc_string(note_on.buffer);
	note_on.complete_buffer();
	inspect_osc_string(note_on.buffer);*/

	try {
		command<osc_float, self_port_templ<int>> fl("/float", 42.0, self_port_templ<int>{});
		//inspect_osc_string(fl.buffer);

		fl.buffer().inspect(no_rt::mlog);

		fl.port_at<1>().set(123);
		fl.command::update();

		fl.complete_buffer().inspect(no_rt::mlog);
		
		assert(are_equal(fl.buffer().data(), "/float\0\0,fi\0\0", 12));

	//	no_rt::mlog << "pad size: " << mini::pad_size<vint>::value() << std::endl;


#if OLD_ZYN
		using m_note_on_t = zynaddsubfx_t::note_on<use_no_port, use_no_port, self_port_templ>;

		zynaddsubfx_t z("hello world!");
		m_note_on_t non(&z, 0, 1, self_port_templ<int, true>{});
		non.cmd_ptr->buffer().inspect(no_rt::mlog);


		non.cmd_ptr->port_at<2>().set(99);

		non.cmd_ptr->command::update();

	//	no_rt::mlog << "ARG NOW:" << std::get<2>(non.args) << std::endl;

		non.cmd_ptr->complete_buffer();
		non.cmd_ptr->buffer().inspect(no_rt::mlog);
#endif
		
		command<const char*> str_cmd("/string", "hello world!");
		str_cmd.buffer().inspect(no_rt::mlog);
		str_cmd.complete_buffer().inspect(no_rt::mlog);

	{
		command<osc_float, bool, bool, self_port_templ<bool>, osc_int> has_bool("/bool", 42.0, false, true, self_port_templ<bool>{}, 42);
		//inspect_osc_string(fl.buffer);

		has_bool.buffer().inspect(no_rt::mlog);

		has_bool.port_at<3>().set(true);
		has_bool.command::update();

		has_bool.complete_buffer().inspect(no_rt::mlog);
	}

	} catch (const char* s)
	{
		no_rt::mlog << s << std::endl;
		return 1;
	}

	no_rt::mlog << "SUCCESS" << std::endl;
	return 0;
}

