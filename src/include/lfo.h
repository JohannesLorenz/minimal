/*************************************************************************/
/* minimal - a minimal rtosc sequencer                                   */
/* Copyright (C) 2014-2014                                               */
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

#ifndef LFO_H
#define LFO_H

#include <cmath>
#include <vector>
#include "types.h"

namespace mini
{

class effect;

template<class T, class RefT>
class base_port
{
protected:
	effect* ef_ref;
	RefT ref;
public:
	using type = T;
	RefT get() const { return ref; }
	base_port(effect* ef_ref, RefT ref) :
		ef_ref(ef_ref), ref(ref) {}
};


template<class T>
class out_port : public base_port<T, T&>
{
	using base = base_port<T, T&>;
public:
	const T& set(const T& new_val) const { return base::ref = new_val; }
	using base_port<T, T&>::base_port;
};


template<class T>
class in_port : public base_port<T, const T&>
{
	using base = base_port<T, const T&>;
public:
	void connect(const out_port<T>& op) {
		op.ef_ref->readers.push_back(base::ef_ref);
		base::ef_ref->writers.push_back(op.ef_ref);
	}
	using base_port<T, T&>::base_port;
};

class effect
{
public:
	std::vector<effect*> readers, writers;
	virtual void proceed(float time) = 0;
};

struct lfo_t : effect
{
	float val;
	out_port<float> out;
	void proceed(float time)
	{
		val = sinf(time);
	}
	lfo_t() : out(this, val) {}
};

}

#endif // LFO_H
