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

#ifndef AUDIO_SINK_H
#define AUDIO_SINK_H

#include "effect.h"
#include "audio.h"

namespace mini {

class audio_sink_t : public effect_t, public audio_in
{
	void instantiate() {}
	void clean_up() {}

	bool _proceed() {
		// nothing to do, the engine will read from us
		set_next_time(std::numeric_limits<sample_no_t>::max());
		return true;
	}
public:
	audio_sink_t() : // TODO: lfo base class?
		effect_t("audio sink"),
		audio_in((effect_t&)*this)
	{
		set_next_time(std::numeric_limits<sample_no_t>::max());
	}
};

}

#endif // AUDIO_SINK_H
