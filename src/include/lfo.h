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

#include "effect.h"
#include "mports.h"
#include "bars.h"

namespace mini
{

/*template<class OutType = float>
struct lfo_t;

template<class OutType = float>
struct lfo_con : ef_con_t<lfo_t<OutType>>, public port_chain<freq_lfo_out<OutType>>
{
};*/

constexpr sample_no_t default_lfo_step = 441; // 441 ?

enum class lfo_type
{
	cos,
	constant
};

struct lfo_base
{
	static sample_no_t never();
	static void lfo_proceed_message();
};

struct sine_base : lfo_base
{
	//using base = port_chain<lfo_out<OutType>>;
	const float min, max, mm2 /* max + min / 2*/, middle;
	const sample_no_t start, end;
	const float times;
	const float outside;
	const sample_no_t step;
	const float repeat; // unused
	//! factor to multiply by 2PI/(end - start)
	//! and the times variable
	const float premult;
	//float time =

	float calc(float time) const;

	static void print_value(float val);

	sine_base(float min, float max, bars_t _start, bars_t _end, float times = 1.0f, float outside = 0.0f, sample_no_t step = default_lfo_step);
};

template<class OutType, lfo_type Lfo = lfo_type::cos>
struct lfo_t : sine_base, effect_t, freq_lfo_out<OutType>
{
	//using lfo_out = lfo_out<OutType>;

	bool _proceed()
	{
		lfo_proceed_message();

		if(time() < start) {
			freq_lfo_out<OutType>::set(outside, time());
			set_next_time(start);
		}
		else if(time() < end)
		{
			float new_value = calc(time());
			print_value(new_value);

			freq_lfo_out<OutType>::set(new_value, time());
			// TODO: repeat etc.

			set_next_time(time() + step);
		}
		else
		{
			freq_lfo_out<OutType>::set(outside, time());
			set_next_time(never());
		}
		return true; // LFO is always single threaded
	//	return 0.0f; // TODO
	}

	void instantiate() {}
	void clean_up() {}

	lfo_t(float min, float max, bars_t _start, bars_t _end, float times = 1.0f, float outside = 0.0f, sample_no_t step = default_lfo_step) :
		sine_base(min, max, _start, _end, times, outside, step),
		effect_t(std::tuple<freq_lfo_out<OutType>&>{*this}),
		freq_lfo_out<OutType>((effect_t&)*this) {
		init_next_time(0); // must be set initially, even if 0.0f < start
	}
};

template<class OutType, OutType Value>
struct constant : lfo_base, effect_t, freq_lfo_out<OutType>
{
	constant() : // TODO: lfo base class?
		freq_lfo_out<OutType>((effect_t&)*this)
	{
		freq_lfo_out<OutType>::set(Value, 0.0f);
		set_next_time(0);
	}

	void instantiate() {}
	void clean_up() {}

	// this will be only called on startup
	bool _proceed(sample_no_t ) {
		//freq_lfo_out<OutType>::set(Value, time);
		set_next_time(never());
		return true;
	}
};

}

#endif // LFO_H
