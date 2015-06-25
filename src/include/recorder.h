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

#ifndef RECORDER_H
#define RECORDER_H

#include <sndfile.hh>
#include <ringbuffer/ringbuffer.h>

#include "audio.h"
#include "jack.h"
#include "effect.h"

namespace mini
{


/*class recorder_client_t : client_t
{
	jack_thread_info_t* info;
	void process(jack_nframes_t frames);
	void shutdown();
};*/

class recorder_t : public effect_t, public audio_in
{
	SndfileHandle fp;
	m_ringbuffer_t rb;
	float* framebuf;
public:
	recorder_t(const char *filename,
		int format = SF_FORMAT_WAV | SF_FORMAT_PCM_16);
	~recorder_t() { delete[] framebuf; }

	bool _proceed(sample_no_t time);
};

}

#endif // RECORDER_H
