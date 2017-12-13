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

//! generic, non-template base for all lfos
struct lfo_base
{
	static sample_no_t never();
	static void lfo_proceed_message();
};

enum class scale_type
{
	linear,
	logarithmic
};

template<class T>
struct freq_lfo_params
{
	const T min, max;
	const T range; //!< = max - min
	const scale_type scale;
	freq_lfo_params(T min, T max, scale_type scale) :
		min(min), max(max), range(max - min),
		scale(scale)
		{}
};

template<class T>
struct freq_lfo_out : public freq_lfo_params<T>, out_port_t<T>
{
	using out_port_t<T>::out_port_t;
	template <class ...Args>
	freq_lfo_out(T min, T max, scale_type scale, effect_t& e, Args... args) : // FEATURE : forward?
		freq_lfo_params<T>(min, max, scale),
		out_port_t(e, args...)
	{
	}
};

template<class T>
struct freq_lfo_in : public freq_lfo_params<T>, in_port_t<T>
{
	using in_port_t<T>::in_port_t;
	template <class ...Args>
	freq_lfo_in(T min, T max, scale_type scale, effect_t& e, Args... args) : // FEATURE: forward?
		freq_lfo_params<T>(min, max, scale),
		in_port_t(e, args...)
	{
	}
};

template<class T1, class T2>
struct more_precise_type
{
	using type = typename more_precise_type<T2, T1>::type;
};

template<>
struct more_precise_type<int, float> { using type = float; };

template<>
struct more_precise_type<int, bool> { using type = int; };

template<class TIn, class TOut>
class connection<freq_lfo_in<TIn>, freq_lfo_out<TOut>> : public connection<TIn, TOut>
{
	using base = connection<TIn, TOut>;
	void vinstantiate() { vtransmit(); }
	void vtransmit() {
		const freq_lfo_in<TIn>& in = base::in_port;
		const freq_lfo_out<TOut>& out = base::out_port;
		if(in.scale == out.scale)
		{
			typename more_precise_type<TIn, TOut>::type val = out.value();
			in.data = ((val - out.min) / out.range) * in.range + in.min;
		}
		// the input interprets the (from the port coming) data logarithmically
		// i.e. we have to calculate the exp function
		else if(in.scale == scale_type::logarithmic)
		{
			// since expf and logf work with float, there is no reason
			// to not use floats

			// in.min/max have the values the user sees, i.e. non-exponential
			float limi = logf(in.min), lima = logf(in.max);
			// TODO: precompute all this!

			float val = out.value();

			float scaled_0_1 = ((val - out.min) / out.range);

			in.data = expf(scaled_0_1 * (lima - limi) + limi);


		}
		else if(out.scale == scale_type::logarithmic)
		{
			// this needs to calculate exactly the reverse from the
			// recent if-block

			float val = out.value();

			float scaled_0_1 = logf( val / out.min ) / logf( ((float)out.max) / out.min );

			in.data = scaled_0_1 * in.range + in.min;
		}
	}

public:
	using base::base;
};

//! template base for all lfos
template<class OutType>
struct lfo_templ_base : effect_t, freq_lfo_out<OutType>
{
	lfo_templ_base() :
		effect_t(std::tuple<freq_lfo_out<OutType>&>{*this}),
		freq_lfo_out<OutType>((effect_t&)*this)
	{
		init_next_time(0); // must be set initially, even if 0.0f < start
	}
};

//! non-template-base for sine
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
struct lfo_t : sine_base, lfo_templ_base<OutType>
{
	//using lfo_out = lfo_out<OutType>;
	using effect = lfo_templ_base<OutType>;

	bool _proceed()
	{
		sample_no_t time = effect::time();

		lfo_proceed_message();

		if(time < start) {
			effect::set(outside, time);
			effect::set_next_time(start);
		}
		else if(time < end)
		{
			float new_value = calc(time);
			print_value(new_value);

			effect::set(new_value, time);
			// TODO: repeat etc.

			effect::set_next_time(time + step);
		}
		else
		{
			effect::set(outside, time);
			effect::set_next_time(never());
		}
		return true; // LFO is always single threaded
	//	return 0.0f; // TODO
	}

	void instantiate() {}
	void clean_up() {}

	lfo_t(float min, float max, bars_t _start, bars_t _end, float times = 1.0f, float outside = 0.0f, sample_no_t step = default_lfo_step) :
		sine_base(min, max, _start, _end, times, outside, step) {
	}
};

template<class OutType, OutType Value>
struct constant : lfo_base, lfo_templ_base<OutType>
{
	constant()
	{
		freq_lfo_out<OutType>::set(Value, 0.0f);
	}

	void instantiate() {}
	void clean_up() {}

	// this will be only called on startup
	bool _proceed(sample_no_t ) {
		lfo_templ_base<OutType>::set_next_time(never());
		return true;
	}
};

}

#endif // LFO_H
