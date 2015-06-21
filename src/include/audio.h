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

#ifndef AUDIO_H
#define AUDIO_H

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
struct Stereo {
	T ch[Channels];
	T& operator[](std::size_t idx) { return ch[idx]; }
	const T& operator[](std::size_t idx) const { return ch[idx]; }
};

template<class T, std::size_t N = 2>
struct multiplex
{
	/*struct test {
		test(test&& ) = default;
		test(const test& ) = delete;
		test(std::size_t ) {}
	};
	using T = test;
*/
	T data[N];

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
	
	// TODO: should be like std::get<I> ?
	const T& operator[](std::size_t i) const { return data[i]; }
	T& operator[](std::size_t i) { return data[i]; }
};

class m_ringbuffer_t : public ringbuffer_t<Stereo<float>>, util::non_movable_t
{
	using ringbuffer_t<Stereo<float>>::ringbuffer_t;
};

struct audio_out : out_port_templ<m_ringbuffer_t>
{
	using base::out_port_templ;
};

class m_reader_t : public ringbuffer_reader_t<Stereo<float>> {
public:
	m_reader_t() : ringbuffer_reader_t(rb_size) {}
};

//! redefinition for the port, since there is nothing to assign
// TODO: general version for multiplex<T>
template<bool IsDep>
struct in_port_templ<m_reader_t, IsDep> : public in_port_templ_noassign<m_reader_t, IsDep>
{
public:
	using in_port_templ_noassign<m_reader_t, IsDep>::in_port_templ_noassign;
	// TODO: noassign is incorrect, use foreach...
};

//! redefinition of connection
template<bool IsDep>
void operator<<(in_port_templ<m_reader_t, IsDep>& ipt,
	out_port_templ<m_ringbuffer_t>& opt)
{
	internal_connect(ipt, opt);
	ipt.data.connect(opt.data);
}

struct audio_in : in_port_templ<m_reader_t, true /*TODO?*/>
{
	void on_read(sample_no_t ) {} // TODO??
	audio_in(effect_t& e) : in_port_templ(e) {}
	//using base::in_port_templ;
};

}

#endif // AUDIO_H
