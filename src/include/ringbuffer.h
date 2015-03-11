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

class ringbuffer_t;

class ringbuffer_common_t
{
private:
	static std::size_t calc_size(std::size_t sz);
protected:
	const std::size_t size; // TODO: const
	const std::size_t size_mask;
public:
	ringbuffer_common_t(std::size_t sz);
	~ringbuffer_common_t();
};

class ringbuffer_t : protected ringbuffer_common_t
{
	std::atomic<std::size_t> w_left; //, w_right;
	std::atomic<std::size_t> readers_left;
	std::size_t num_readers; //!< to be const after initialisation

//	volatile std::size_t read_ptr;
//	std::atomic<std::size_t> iteration;
//	int mlocked = 0;
	char* const buf;

	friend class ringbuffer_reader_t;

public:
	ringbuffer_t(std::size_t sz);
	~ringbuffer_t();

	std::size_t write_space() const;
	std::size_t write(const char *src, size_t cnt);
};

class ringbuffer_reader_t : protected ringbuffer_common_t
{
	const char* const buf;
	ringbuffer_t* const ref;
	std::size_t read_ptr = 0;
//	std::size_t iteration = 0;

	std::size_t read_space() const;
	//std::size_t read(char *dest, std::size_t cnt);

//	std::size_t read_space() const;
	std::size_t read(char *dest, std::size_t cnt);

	void try_inc(std::size_t range)
	{
#if 0
		const std::size_t
			w = ref->w_left.load(std::memory_order_relaxed),
			r = read_ptr;//,
			//w_iteration = ref->iteration.load(std::memory_order_relaxed);
		// TODO: compare exchange?

		const std::size_t until_w =
			(w_iteration - iteration) * size
			+ w - r;
		std::cerr << w_iteration << ", " << iteration << std::endl;
		std::cerr << w << ", " << r << std::endl;
		std::cerr << until_w << ", " << range << std::endl;
		if(until_w < range)
		 throw "Error: Read data is not reliable anymore";
#endif
		const std::size_t old_read_ptr = read_ptr;

		read_ptr = (read_ptr + range) & size_mask;
		if((read_ptr ^ old_read_ptr) & (size >> 1)) // highest bit flipped
		{
			std::cerr << "decreasing readers left..." << std::endl;
			--ref->readers_left;
		}

	}

	struct read_sequence_t
	{
		const char* const ptr;
		const char* const buf;
	//	std::size_t size_1, size_2;
		ringbuffer_reader_t* reader_ref;
		std::size_t range;
		read_sequence_t(ringbuffer_reader_t& rb, std::size_t range) :
			ptr(rb.buf + rb.read_ptr),
			buf(rb.buf),
			reader_ref(&rb),
		//	size_1()
			range(range)
		{
		}

		~read_sequence_t()
		{
			// TODO: check!!!
			reader_ref->try_inc(range);
		}

		// single member access
		const char& operator[](std::size_t idx) {
			return *(buf + ((reader_ref->read_ptr + idx) & reader_ref->size_mask));
		}
	};
public:
	ringbuffer_reader_t(ringbuffer_t& ref) : ringbuffer_common_t(ref.size), buf(ref.buf), ref(&ref) {
		++ref.num_readers;
	}
	read_sequence_t read_sequence(std::size_t range) {
		return read_sequence_t(*this, range);
	}
};

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
