/*************************************************************************/
/* minimal - a minimal osc sequencer                                     */
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

#include "note_line.h"

namespace mini
{

note_line_impl::note_line_impl(note_line_t *nl) : is_impl_of_t<note_line_t>(nl)
{
	// insert notes
	visit(ref->notes, note_geom_t(0, 0));
	// insert sentinel
	note_events.emplace(note_geom_t(std::numeric_limits<float>::max(), 1),
		m_note_event{true, 0, std::numeric_limits<int>::max()});

	itr = note_events.begin();
}

float note_line_impl::_proceed(float time)
{
	note_signal_t& notes_out = ref->notes_out::data;
	std::pair<int, int>* recently_changed_ptr = notes_out.recently_changed.data();

	while(itr->first.start <= time)
	{
		const note_geom_t& geom = itr->first;
		const m_note_event& event = itr->second;
		std::pair<int, int>* notes_at = notes_out.lines[geom.offs];
		std::size_t id = 0;
		if(event.on)
		{
			// skip used slots
			for(; id < POLY_MAX && notes_at->first > 0; ++notes_at, ++id) ;

			if(id >= POLY_MAX)
			 throw "end of polyphony reached!";

			notes_at->first = event.id;
			notes_at->second = event.volume;
			std::cerr << "note on: " << event.id <<  std::endl;
		}
		else
		{
			for(; id < POLY_MAX && notes_at->first != event.id; ++notes_at, ++id) ;

			if(id >= POLY_MAX)
			 throw "end of polyphony reached!";

			notes_at->first = -1;

			std::cerr << "note off: " << event.id <<  std::endl;
		}

		recently_changed_ptr->first = geom.offs;
		(recently_changed_ptr++)->second = id;

		std::cerr << "played one note: " << itr->first.start << std::endl;

		++itr;
	}

	recently_changed_ptr->first = -1;
	++notes_out.changed_stamp;
	ref->notes_out::change_stamp = time;

	last_time = time;
	return itr->first.start;
}

}
