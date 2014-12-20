/*************************************************************************/
/* minimal - a minimal osc sequencer                                     */
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
#include <limits>
#include "effect.h"

namespace mini
{

/*template<class OutType = float>
struct lfo_t;

template<class OutType = float>
struct lfo_con : ef_con_t<lfo_t<OutType>>, public port_chain<freq_lfo_out<OutType>>
{
};*/

template<class OutType>
struct lfo_t : effect_t, public port_chain<lfo_out<OutType>>
{
	using base = port_chain<lfo_out<OutType>>;
	const float min, max, mm2, middle;
	const float start, end, times, outside, step;
	const float repeat;
	float next_time; // TODO?
	//float time =

	//using lfo_out = lfo_out<OutType>;

	float _proceed(float time)
	{
		if(time < start) {
			lfo_out<OutType>::port.set(outside);
			return start;
		}
		else if(time < end)
		{
			lfo_out<OutType>::port.set(middle + sinf(time-start) * mm2);
			// TODO: repeat etc.

			return time + step;
		}
		else
		{
			lfo_out<OutType>::port.set(outside);
			return std::numeric_limits<float>::max();
		}
	//	return 0.0f; // TODO
	}

	lfo_t(float min, float max, float start, float end, float times = 1.0f, float outside = 0.0f, float step = default_step) :
		base(*this),
		min(min),
		max(max),
		mm2((max - min)/2.0f),
		middle(min + mm2),
		start(start),
		end(end),
		times(times),
		outside(outside),
		step(step),
		repeat((end - start)/times){}
};

}

#endif // LFO_H
