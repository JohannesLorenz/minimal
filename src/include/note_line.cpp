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

#include "io.h"
#include "note_line.h"

namespace mini
{

note_line_impl::note_line_impl(note_line_t *nl) : is_impl_of_t<note_line_t>(nl)
{
	// insert notes
	visit(ref->notes, note_geom_t(bars_t(0, 1), 0));
	// insert sentinel
	note_events.emplace(note_geom_t(bars_t(std::numeric_limits<sample_no_t>::max(), 1), 1),
		m_note_event{true, 0, std::numeric_limits<int>::max()});

	itr = note_events.begin();
}

sample_no_t note_line_impl::_proceed(sample_no_t time)
{
	note_signal_t& notes_out = ref->notes_out::data;
	std::pair<int, int>* recently_changed_ptr = notes_out.recently_changed.data();

	while(as_samples_floor(itr->first.start, samples_per_bar) <= time) // TODO! 0.1f 0.1f 0.1f
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
			io::mlog << "note on: " << event.id << io::endl;
		}
		else
		{
			for(; id < POLY_MAX && notes_at->first != event.id; ++notes_at, ++id) ;

			if(id >= POLY_MAX)
			 throw "end of polyphony reached!";

			notes_at->first = -1;

			io::mlog << "note off: " << event.id << io::endl;
		}

		recently_changed_ptr->first = geom.offs;
		(recently_changed_ptr++)->second = id;

		io::mlog << "played one note: " << itr->first.start << io::endl;

		++itr;
	}

	recently_changed_ptr->first = -1;
	++notes_out.changed_stamp;
	ref->notes_out::change_stamp = time;

	last_time = time;
	return as_samples_floor(itr->first.start, samples_per_bar); // TODO! 0.1f 0.1f 0.1f
}

}

