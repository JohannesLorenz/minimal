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

// mostly a copy of jack's ringbuffer, however, with multiple readers

#include <algorithm>
//#include <jack/jack.h>
//#include <jack/ringbuffer.h>

#include <stdlib.h>
#include <string.h>
#include <limits>

#include "jack.h"


namespace mini {

#if 0
std::size_t ringbuffer_t::can_read_size() const {
	return jack_ringbuffer_read_space(ring);
}

/*constexpr std::size_t ringbuffer_t::sample_size()
{
	return sizeof(jack_default_audio_sample_t);
}*/

std::size_t ringbuffer_t::read(char *framebuf, std::size_t size) {
	return jack_ringbuffer_read(ring, framebuf, size);
}

std::size_t ringbuffer_t::write(const char *framebuf, std::size_t size) {
	return jack_ringbuffer_write(ring, framebuf, size);
}

void ringbuffer_t::prepare() { std::fill_n(ring->buf, ring->size, 0); }

ringbuffer_t::ringbuffer_t(std::size_t size)
	: ring(jack_ringbuffer_create(size))
{
}

ringbuffer_t::~ringbuffer_t() { jack_ringbuffer_free(ring); }
#endif

std::size_t client_t::sample_rate() const
{
	return jack_get_sample_rate(client);
}

client_t::client_t(const char* clientname)
{
	client = jack_client_open (clientname, JackNullOption, nullptr);
	jack_set_process_callback(client, p_process, this);
	jack_on_shutdown(client, p_shutdown, this);
}

client_t::~client_t()
{
	jack_client_close(client);
}

}

