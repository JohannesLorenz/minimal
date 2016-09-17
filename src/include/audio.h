/*************************************************************************/
/* minimal - a minimal osc sequencer                                     */
/* Copyright (C) 2014-2016                                               */
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

#ifndef AUDIO_H
#define AUDIO_H

#include <iosfwd>
#include <array>
#include <jack/types.h>
#include <ringbuffer/ringbuffer.h>

#include "mports.h"
#include "utils.h"

namespace mini
{

using sample_t = jack_default_audio_sample_t;

constexpr std::size_t buffer_size = 1 << 14;
constexpr std::size_t sample_size = sizeof(jack_default_audio_sample_t);
constexpr std::size_t rb_size = buffer_size * sample_size;

//template<class T, std::size_t N = 2>
//using multiplex = std::array<T, N>;

//! not my favorite solution, but new fails for float[2]
template<class T, std::size_t Channels = 2>
struct Stereo
{
	T ch[Channels];

	// Master.h suggests operator[], so we provide it...
	T& operator[](std::size_t idx) { return ch[idx]; }
	const T& operator[](std::size_t idx) const { return ch[idx]; }

	template<std::size_t I> const T& at() const { return ch[I]; }
	template<std::size_t I> T& at() { return ch[I]; }
};

#if 0 // Unused, Stereo is being used instead
template<class T, std::size_t N = 2>
class multiplex
{
	/*struct test {
		test(test&& ) = default;
		test(const test& ) = delete;
		test(std::size_t ) {}
	};
	using T = test;
*/
	T data[N];
public:
	template<class ...Args>
	multiplex(Args&&... args) : data{std::forward<Args>(args)...} {}

	/*template<class T2>
	multiplex(const multiplex<T2, N>& other) :
		data(other.data)
	{}*/

private:/*
	template<class T2, std::size_t... Id>
	void assign_single(const multiplex<T2, N>& other) {

	}

	template<class T2, std::size_t... Ids>
	multiplex<T, N>& assign(const multiplex<T2, N>& other, const util::seq<Ids...>& ) {
		assign_single<Ids>(other)...;
		return *this;
	}*/
public:

	template<class T2>
	multiplex<T, N>& operator=(const multiplex<T2, N>& other) {
		//return assign(other, util::gen_seq<N>());
		data = other.data;
	}
	
	// don't use this, implement std::get<I> instead
//	const T& operator[](std::size_t i) const { return data[i]; }
//	T& operator[](std::size_t i) { return data[i]; }
};

#endif

class m_ringbuffer_t : public ringbuffer_t<Stereo<float>>, util::non_movable_t
{
	using ringbuffer_t<Stereo<float>>::ringbuffer_t;
};

struct audio_out : out_port_t<m_ringbuffer_t>
{
	using base::out_port_t;
};

class m_reader_t : public ringbuffer_reader_t<Stereo<float>> {
public:
	m_reader_t() : ringbuffer_reader_t(rb_size) {}
	friend std::ostream& operator<< (std::ostream& stream,
		class m_reader_t& r);
	void operator=(m_ringbuffer_t& rb) { connect(rb); }
};

//! redefinition for the port, since there is nothing to assign
template<bool IsDep>
struct in_port_t<m_reader_t, IsDep> :
	public in_port_noassign_t<m_reader_t, IsDep>
{
public:
	using in_port_noassign_t<m_reader_t, IsDep>::in_port_noassign_t;
};

template<bool IsDep = true>
struct audio_in : public in_port_t<m_reader_t, IsDep>
{
	void on_read(sample_no_t ) {} // default behaviour
	using base = in_port_t<m_reader_t, IsDep>;
	using base::in_port_t;
};

template<>
struct input_type_t<m_ringbuffer_t*>
{
	using type = m_reader_t;
};

}

#endif // AUDIO_H
