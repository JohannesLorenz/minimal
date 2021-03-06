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

#ifndef AUDIO_INSTRUMENT_H
#define AUDIO_INSTRUMENT_H

#include <jack/types.h>

#include "jack.h"
#include "instrument.h"
#include "audio.h"

namespace mini
{

class audio_instrument_t : public instrument_t, public audio_out
{
public:
	audio_instrument_t(const char* name);
private: // TODO
	void init(/*jack_client_t& client*/);
public:
	int process(jack_nframes_t nframes);
	void shutdown() {}

	void init_2();
};

}

#endif // AUDIO_INSTRUMENT_H
