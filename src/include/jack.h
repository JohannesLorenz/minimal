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

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <atomic>
#include <cstddef>
#include <iostream> // TODO

//#include <sys/types.h> // for size_t

/*// forward declaration of internal type
typedef struct {
	char *buf;
	volatile size_t write_ptr;
	volatile size_t read_ptr;
	size_t size;
	size_t size_mask;
	int mlocked;
} jack_ringbuffer_t ;
*/

#include <jack/jack.h>
//#include <jack/ringbuffer.h>

namespace mini {

/*class ringbuffer_old_t
{
	jack_ringbuffer_t *ring;
	std::size_t channels = 2;
public:
	std::size_t can_read_size() const;

	constexpr static std::size_t sample_size() { return sizeof(jack_default_audio_sample_t); }

	// TODO: audio_ringbuffer_t?
	std::size_t bytes_per_frame() const { return channels * sample_size(); }

	bool can_read() const { return can_read_size() <= bytes_per_frame(); }

	std::size_t read(char* framebuf, std::size_t size);

	std::size_t write(const char* framebuf, std::size_t size);

	void prepare();

	ringbuffer_t(std::size_t size);

	~ringbuffer_t();
};*/




class client_t
{
	jack_client_t *client;
	// TODO: pass function pointer as template?
	static int p_process(jack_nframes_t frames, void* _cl) {
		client_t* cl = reinterpret_cast<client_t*>(_cl);
		return cl->process(frames);
	}
	static void p_shutdown(void* _cl) {
		client_t* cl = reinterpret_cast<client_t*>(_cl);
		cl->shutdown();
	}
public:
	std::size_t sample_rate() const;

	client_t(const char *clientname);
	virtual ~client_t();

	virtual int process(jack_nframes_t) = 0;
	virtual void shutdown() = 0;
};

}

#endif // RINGBUFFER_H
