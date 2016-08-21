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

#include <climits>
#include <cmath>
#include "lfo.h"
#include "io.h"

namespace mini
{

float sine_base::calc(float time) const
{
	return middle +
		sinf((time-start) * premult) * mm2;
}

void lfo_base::lfo_proceed_message()
{
	io::mlog << "proceeding with lfo... " << io::endl;
}

void sine_base::print_value(float val)
{
	io::mlog << "lfo value: " << val << io::endl;
}

sample_no_t lfo_base::never()
{
	return std::numeric_limits<sample_no_t>::max();
}

sine_base::sine_base(float min, float max, bars_t _start, bars_t _end, float times, float outside, sample_no_t step) :
	min(min),
	max(max),
	mm2((max - min)/2.0f),
	middle(min + mm2),
	start(as_samples_floor(_start, info.samples_per_bar)),
	end(as_samples_floor(_end, info.samples_per_bar)),
	times(times),
	outside(outside),
	step(step),
	repeat((end - start)/times),
	premult(times * M_PI/(end - start))
{
}



}
