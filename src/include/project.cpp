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

#include <stack>
#include "project.h"

namespace mini {

project_t::project_t()
{
}

project_t::~project_t()
{
	for(const effect_t* ef : effects())
	 delete ef;
}

void project_t::finalize()
{
	for(effect_t* e : effects()) // TODO: -> initializer list
	{
		e->instantiate();
		if(e->writers.empty())
		{
			_effect_root.readers.push_back(e);
			e->writers.push_back(&_effect_root);
		}
	}

	std::stack<effect_t*> ready_fx;
	ready_fx.push(&effect_root());

	std::size_t next_id = 0;

	// set an id to all effects in topological order
	do
	{
		effect_t* cur_effect = ready_fx.top();
		ready_fx.pop();

		if(cur_effect->id() != has_id::no_id())
		 throw "Id given twice";

		cur_effect->set_id(next_id++);

		//no_rt::mlog << "set id: " << next_id - 1 << " for " << cur_effect << std::endl;

		const auto cb = [&](const std::vector<effect_t*>& next_vector)
		{
			for(effect_t* next: next_vector)
			{
				bool parents_done = true;
				for(const effect_t* par: next->writers)
				 parents_done = parents_done && (par->id() != has_id::no_id());
				if(parents_done)
				 ready_fx.push(next);
			}
		};

		cb(cur_effect->readers);
		cb(cur_effect->deps);

	} while(ready_fx.size());
	finalized = true;
}

/*track_t& project_t::add_track(const instrument_t &ins) {
	_tracks.push_back(track_t(ins.id()));
	return _tracks.back();
}

track_t::track_t(const instrument_t::id_t& instr_id)
	: instr_id(instr_id)
{

}*/

}

