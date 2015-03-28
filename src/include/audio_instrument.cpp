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

#include <jack.h>
#include "audio_instrument.h"

constexpr std::size_t buffer_size = 1 << 14;
constexpr std::size_t sample_size = sizeof(jack_default_audio_sample_t);
constexpr std::size_t rb_size = buffer_size * sample_size;

namespace mini
{

audio_instrument_t::audio_instrument_t(const char *name) :
	instrument_t(name),
	audio_out((effect_t&)*this, rb_size) {

	// connect jack out to audio out


}

}
