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

#include "ringbuffer.h"


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

ringbuffer_t::ringbuffer_t(std::size_t sz) :
	ringbuffer_common_t(sz),
	buf(new char[ringbuffer_common_t::size])
{
	w_left.store(0, std::memory_order_relaxed);
	readers_left.store(0, std::memory_order_relaxed);
//	w_right.store(0, std::memory_order_relaxed);
//	iteration.store(0, std::memory_order_relaxed);
/*	std::size_t power_of_two;
	for (power_of_two = 1; 1 << power_of_two < sz; power_of_two++);

	size = 1 << power_of_two;
	size_mask = size - 1;
//	write_ptr = 0;
//	read_ptr = 0;*/
	//buf = new char[size];
//	mlocked = 0;
}

ringbuffer_t::~ringbuffer_t()
{
/*	if (rb->mlocked) {
		munlock (rb->buf, rb->size);
	}*/
	delete[] buf;
}

std::size_t ringbuffer_reader_t::read_space() const
{
	const std::size_t
		w = ref->w_left.load(std::memory_order_relaxed),
		r = read_ptr;

	if (w > r) {
		return w - r;
	} else {
		return (w - r + ref->size) & ref->size_mask;
	}
}

std::size_t ringbuffer_t::write_space() const
{
	// case 1: at least one r is still in the other half
	//  => write_space is end of this half

	// case 2: all r are already in the half of w
	//  => write_space is end of this half + other half

	std::cerr << "size: " << size << ", wl: " << w_left.load(std::memory_order_relaxed) << ", size_mask: " << size_mask << std::endl;
	std::cerr << "rl: " << readers_left.load(std::memory_order_relaxed) << std::endl;
	std::cerr << "WS: " <<
		((size >> 1) - (w_left.load(std::memory_order_relaxed) & (size_mask >> 1)))
		<< "+" << ((readers_left.load(std::memory_order_relaxed) == false) * (size >> 1)) << std::endl;

	return ((size >> 1) - (w_left.load(std::memory_order_relaxed) & (size_mask >> 1))) // = way to next half
		+ ((readers_left.load(std::memory_order_relaxed) == false) * (size >> 1))
		;
}

/*size_t ringbuffer_reader_t::read(char *dest, size_t cnt)
{
	std::size_t to_read;
	std::size_t cnt2;
	std::size_t n1, n2;

	std::size_t free_cnt = read_space();
	switch(free_cnt)
	{
		case std::numeric_limits<std::size_t>::max():
			throw "read pointer too slow - invalid";
		case 0: return 0; // nothing to read
		default: ;
	}

	to_read = cnt > free_cnt ? free_cnt : cnt;

	cnt2 = read_ptr + to_read;

	if (cnt2 > ref->size) {
		n1 = ref->size - read_ptr;
		n2 = cnt2 & ref->size_mask;
	} else {
		n1 = to_read;
		n2 = 0;
	}

	memcpy (dest, &(ref->buf[read_ptr]), n1);
	read_ptr += n1;
	read_ptr &= ref->size_mask;

	if (n2) {
		memcpy (dest + n1, &(ref->buf[read_ptr]), n2);
		read_ptr += n2;
		read_ptr &= ref->size_mask;
	}

	return to_read;
}*/

std::size_t ringbuffer_t::write (const char *src, size_t cnt)
{

	std::size_t free_cnt;
	std::size_t cnt2;
	std::size_t to_write;
	std::size_t n1, n2;

	if ((free_cnt = write_space()) == 0) {
		return 0;
	}

	to_write = cnt > free_cnt ? free_cnt : cnt;
	// the readers are no obstacle, but you can not write more
	// than the buffer ;)
//	to_write = cnt > size ? size : cnt;

	cnt2 = w_left.load(std::memory_order_relaxed) + to_write;

	if (cnt2 > size) {
		n1 = size - w_left.load(std::memory_order_relaxed);
		n2 = cnt2 & size_mask;
	} else {
		n1 = to_write;
		n2 = 0;
	}

	// here starts the writing

	//std::size_t tmp_write_ptr = write_ptr;
/*	w_right.store(((w_right.load(std::memory_order_relaxed)
		+ n1)
		+ n2) & size_mask, std::memory_order_relaxed);*/

	const std::size_t old_w_left = w_left;

	memcpy (&(buf[w_left]), src, n1);
	w_left.store(w_left + n1 & size_mask, std::memory_order_relaxed);

	if (n2) {
		memcpy (&(buf[w_left]), src + n1, n2);
		w_left.store(w_left + n2 & size_mask, std::memory_order_relaxed);
	}

	std::cerr << "wl, old wl: " << w_left << old_w_left << std::endl;

	// TODO: inefficient or:
	if((w_left ^ old_w_left) & (size >> 1) || to_write == size) // highest bit flipped
	{
		if(readers_left)
		 throw "impossible";
		std::cerr << "resetting readers left..." << std::endl;
		readers_left = num_readers;
	}

//	if(w_right != w_left)
//	 throw "impossible";

	return to_write;
}

std::size_t ringbuffer_common_t::calc_size(std::size_t sz)
{
	std::size_t power_of_two;
	for (power_of_two = 1; 1 << power_of_two < sz; power_of_two++);
	return 1 << power_of_two;
}

ringbuffer_common_t::ringbuffer_common_t(std::size_t sz) :
	size(calc_size(sz)),
	size_mask(size - 1)/*,
	buf(new char[size])*/
{}

ringbuffer_common_t::~ringbuffer_common_t() { /*delete[] buf;*/ }

}

