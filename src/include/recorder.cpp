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

#include "recorder.h"

namespace mini {

recorder_t::recorder_t(const char* filename, int format) :
	audio_in((effect_t&)*this, rb_size, rb_size),
	fp(SndfileHandle(filename, SFM_WRITE, format
		, 2 // channels
		, 48000 // srate
	)),
	rb(/*ringbuffer_t::sample_size() **/ 16384), // TODO: size...
	framebuf(new float[/*rb.bytes_per_frame() /*/ sizeof(float)])
{
}

bool recorder_t::_proceed(sample_t time)
{ // TODO: separate IO thread?
	// TODO: read multiple at a time
#if 0
	while(rb.can_read()) // TODO: & snd file can capture
	{
		rb.read(reinterpret_cast<char*>(framebuf),
			rb.bytes_per_frame());
		if(fp.writef(framebuf, 1) != 1)
		{
			throw "soundfile write error";
		}
	}
#endif
	set_next_time(time + 0.1f); // TODO!!
	return true;
}

}
