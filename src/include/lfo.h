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

#ifndef LFO_H
#define LFO_H

#include <cmath>
#include <limits>
#include "effect.h"
#include "ports.h"
#include "io.h"

namespace mini
{

/*template<class OutType = float>
struct lfo_t;

template<class OutType = float>
struct lfo_con : ef_con_t<lfo_t<OutType>>, public port_chain<freq_lfo_out<OutType>>
{
};*/

constexpr sample_t default_lfo_step = 441;

enum class lfo_type
{
	cos,
	constant
};

template<class OutType, lfo_type Lfo = lfo_type::cos>
struct lfo_t : effect_t, freq_lfo_out<OutType>
{
	//using base = port_chain<lfo_out<OutType>>;
	const float min, max, mm2, middle;
	const sample_t start, end;
	const float times;
	const float outside;
	const sample_t step;
	const float repeat; // unused
	const float premult;
	//float time =

	//using lfo_out = lfo_out<OutType>;

	void instantiate() {}
	void clean_up() {}

	bool _proceed(sample_t time)
	{
		io::mlog << "proceeding with lfo... " << io::endl;
		if(time < start) {
			freq_lfo_out<OutType>::set(outside, time);
			set_next_time(start);
		}
		else if(time < end)
		{
			freq_lfo_out<OutType>::set(middle + cosf((time-start) * premult) * mm2, time);
			// TODO: repeat etc.
			io::mlog << "lfo value: " << middle + cosf((time-start) * premult) * mm2 << io::endl;

			set_next_time(time + step);
		}
		else
		{
			freq_lfo_out<OutType>::set(outside, time);
			set_next_time(std::numeric_limits<sample_t>::max());
		}
		return true; // LFO is always single threaded
	//	return 0.0f; // TODO
	}

	lfo_t(float min, float max, sample_t start, sample_t end, float times = 1.0f, float outside = 0.0f, sample_t step = default_lfo_step) :
		effect_t(std::tuple<freq_lfo_out<OutType>&>{*this}),
		freq_lfo_out<OutType>((effect_t&)*this),
		min(min),
		max(max),
		mm2((max - min)/2.0f),
		middle(min + mm2),
		start(start),
		end(end),
		times(times),
		outside(outside),
		step(step),
		repeat((end - start)/times),
		premult(2 * M_PI/(end - start))
	{
		set_next_time(0.0f); // must be set initially, even if 0.0f < start
	}
};

template<class OutType, OutType Value>
struct constant : effect_t, freq_lfo_out<OutType>
{
	constant() : // TODO: lfo base class?
		freq_lfo_out<OutType>((effect_t&)*this)
	{
		freq_lfo_out<OutType>::set(Value, 0.0f);
		set_next_time(0.0f);
	}

	void instantiate() {}
	void clean_up() {}

	// this will be only called on startup
	sample_t _proceed(sample_t ) {
		//freq_lfo_out<OutType>::set(Value, time);
		return std::numeric_limits<sample_t>::max();
	}
};

}

#endif // LFO_H
