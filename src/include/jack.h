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

#ifndef JACK_H
#define JACK_H

#include <atomic>
#include <cstddef>

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

#include <jack/jack.h> // TODO: types.h?
//#include <jack/ringbuffer.h>

namespace mini {

/*class ringbuffer_old_t
{
	jack_ringbuffer_t *ring;
	std::size_t channels = 2;
public:
	std::size_t can_read_size() const;

	constexpr static std::size_t sample_size() { return sizeof(jack_default_audio_sample_no_t); }

	// TODO: audio_ringbuffer_t?
	std::size_t bytes_per_frame() const { return channels * sample_size(); }

	bool can_read() const { return can_read_size() <= bytes_per_frame(); }

	std::size_t read(char* framebuf, std::size_t size);

	std::size_t write(const char* framebuf, std::size_t size);

	void prepare();

	ringbuffer_t(std::size_t size);

	~ringbuffer_t();
};*/

namespace jack
{

using frames_t = jack_nframes_t;

class port_t
{
	jack_port_t* port = nullptr;
	port_t(jack_port_t* port) : port(port) {}
	friend class client_t;
	void* _get_buffer(frames_t nframes);
	const void* _get_buffer(frames_t nframes) const;
public:
	port_t() {}
	port_t(const port_t& ) = default;
	bool operator!() const { return port; }
	const char* name() const;

	template<class T>
	T* get_buffer(frames_t nframes) {
		return static_cast<T*>(_get_buffer(nframes));
	}
	template<class T>
	const T* get_buffer(frames_t nframes) const {
		return static_cast<const T*>(_get_buffer(nframes));
	}
};

#if 0
// TODO: own header
namespace scales
{
	//constexpr sample_no_t samples_per_sec = 1024; sample_rate
	constexpr sample_no_t useconds_per_lfo_intv = 1024;

	constexpr sample_no_t samples_per_bar(sample_no_t samples_per_sec,
		)
	{
		return samples_per_sec << 1;
	}



	// constants depending on others
	constexpr sample_no_t samples_per_bar = samples_per_sec * 2;
	constexpr sample_no_t usecs_per_sample = 1000000 / samples_per_sec;

	constexpr sample_no_t samples_per_lfo_intv = ;
}
#endif

class client_t
{
public: // TODO
	jack_client_t *client = nullptr;
private:
	// TODO: pass function pointer as template?
	static int p_process(frames_t frames, void* _cl) {
		client_t* cl = reinterpret_cast<client_t*>(_cl);
		return cl->process(frames);
	}
	static void p_shutdown(void* _cl) {
		client_t* cl = reinterpret_cast<client_t*>(_cl);
		cl->shutdown();
	}

	virtual int process(frames_t) = 0;
	virtual void shutdown() = 0;
public:
	frames_t sample_rate() const;

	port_t register_port(const char *port_name,
		const char *port_type, unsigned long flags,
		unsigned long buffer_size);

	int connect(const char *source_port,
		const char *destination_port);

	client_t(const char *clientname);
	client_t() {}
	void init(const char* clientname);
	void activate();
	virtual ~client_t();
};

/*struct jack_port_data {};

struct jack_out : out_port_templ<jack_port_data>
{

};

struct jack_in : in_port_templ<jack_port_data>
{

};*/

}

}

#endif // JACK_H
