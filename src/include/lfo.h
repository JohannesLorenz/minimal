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

namespace mini
{

/*template<class OutType = float>
struct lfo_t;

template<class OutType = float>
struct lfo_con : ef_con_t<lfo_t<OutType>>, public port_chain<freq_lfo_out<OutType>>
{
};*/

template<class OutType>
struct lfo_t : effect_t, freq_lfo_out<OutType>
{
	//using base = port_chain<lfo_out<OutType>>;
	const float min, max, mm2, middle;
	const float start, end, times, outside, step;
	const float repeat;
	const float premult;
	//float time =

	//using lfo_out = lfo_out<OutType>;

	void instantiate() {}
	void clean_up() {}

	float _proceed(float time)
	{
		std::cerr << "proceeding with lfo... " << std::endl;
		if(time < start) {
			freq_lfo_out<OutType>::set(outside, time);
			return start;
		}
		else if(time < end)
		{
			freq_lfo_out<OutType>::set(middle + cosf((time-start) * premult) * mm2, time);
			// TODO: repeat etc.
			std::cerr << "lfo value: " << middle + cosf((time-start) * premult) * mm2 << std::endl;

			return time + step;
		}
		else
		{
			freq_lfo_out<OutType>::set(outside, time);
			return std::numeric_limits<float>::max();
		}
	//	return 0.0f; // TODO
	}

	lfo_t(float min, float max, float start, float end, float times = 1.0f, float outside = 0.0f, float step = default_step) :
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
		set_next_time(start);
	}
};

}

#endif // LFO_H
