/*************************************************************************/
/* minimal - a minimal osc sequencer                                     */
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

#include "effect.h"

namespace mini
{

note_line_impl::note_line_impl(note_line_t *nl) : is_impl_of_t<note_line_t>(nl)
{
	for(const auto& pr : ref->notes.get<note_t>())
	{
		int note_height = ref->notes.geom.offs + pr.first.offs;
		int start = ref->notes.geom.start + pr.first.start;
		note_lines[note_height].emplace(start, *pr.second);
	}

	for(const auto& pr : note_lines)
	{
		add_task(new note_task_t(*this, pr.first, pr.second, pr.second.begin()->first));
	}
}

void note_line_impl::note_task_t::proceed(float time)
{
	note_signal_t& notes_out = nl_ref->ref->notes_out::data;

	if(time != nl_ref->last_time)
	{
		notes_out.last_changed_hint = notes_out.changed_hint;
	}

	//ins->con.send_osc_str(cmd->buffer());

	//*(last_key++) = note_height;

	//const float& cur_start = itr->first;
	const note_t& cur_note = itr->second;

	*(notes_out.last_changed_hint++) = note_height;

	if(is_on)
	{
		update_next_time((++itr)->first);
	}
	else
	{
		update_next_time(time + cur_note.length());
	}

	is_on = !is_on;
	notes_out.lines[note_height] = is_on;

	nl_ref->last_time = time;

}

}

