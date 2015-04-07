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

#include "ringbuffer/src/lib/ringbuffer.h"
#include "ports.h"
#include "utils.h"

#include <jack/types.h>

namespace mini
{

constexpr std::size_t buffer_size = 1 << 14;
constexpr std::size_t sample_size = sizeof(jack_default_audio_sample_t);
constexpr std::size_t rb_size = buffer_size * sample_size;

//template<class T, std::size_t N = 2>
//using multiplex = std::array<T, N>;

template<class T, std::size_t N = 2>
struct multiplex
{
	T data[N];

	template<class ...Args>
	multiplex(Args&&... args) : data{std::forward<Args>(args)...} {} // TODO: std forward

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

	const T& operator[](std::size_t i) const { return data[i]; }
	T& operator[](std::size_t i) { return data[i]; }
};

struct audio_out : out_port_templ<multiplex<ringbuffer_t>>
{
	using base::out_port_templ;
};

//! redefinition for the port, since there is nothing to assign
template<bool IsDep>
struct in_port_templ<multiplex<ringbuffer_reader_t>, IsDep> : public in_port_templ_noassign<multiplex<ringbuffer_reader_t>, IsDep>
{
public:
	using in_port_templ_noassign<multiplex<ringbuffer_reader_t>, IsDep>::in_port_templ_noassign;
};

//! redefinition of connection
template<bool IsDep>
void operator<<(in_port_templ<multiplex<ringbuffer_reader_t>, IsDep>& ipt,
	out_port_templ<multiplex<ringbuffer_t>>& opt)
{
	internal_connect(ipt, opt);
	ipt.data[0].connect(opt.data[0]);
	ipt.data[1].connect(opt.data[1]);
}


struct audio_in : in_port_templ<multiplex<ringbuffer_reader_t>, true /*TODO?*/>
{
	void on_recv(sample_t ) {} // TODO??
	using base::in_port_templ;
};

}

#endif // AUDIO_H
