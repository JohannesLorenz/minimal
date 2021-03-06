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

#ifndef JACK_ENGINE_H
#define JACK_ENGINE_H

#include "jack.h"
#include "engine.h"

namespace mini {

class jack_engine_t : public engine_t, public jack::client_t
{
	virtual int process(jack::frames_t samples);
	virtual void shutdown() {
		stop();
		//throw "shutdown not implemented :P";
	}

	sample_no_t get_sample_rate() { return sample_rate(); }

	jack::port_t out[2];
	sample_no_t limit = 0, samples_until_now = 0;

	void vrun(bars_t _limit);
public:
	jack_engine_t();
	const char* type_hint() { return "audio"; }
};

}

#endif // JACK_ENGINE_H
