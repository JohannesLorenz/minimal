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

#ifndef SAMPLE_H
#define SAMPLE_H

#include <cstdint>

namespace mini {

//! signed int of at least 64 bit
using sample_no_t = int_least64_t;
using sample_rate_t = sample_no_t;

extern sample_no_t global_samplerate;

inline sample_no_t operator"" _smps(unsigned long long int n) { return n; }

// enough samples for one day
static_assert(sizeof(sample_no_t) >= 8,
	"need 64 bit ints");

// TODO: not here?
// 1 bar = 2 seconds
// 1000 samples per second

// fixed constants
constexpr sample_no_t samples_per_sec = 1024;
constexpr sample_no_t useconds_per_lfo_intv = 1024;

// constants depending on others
constexpr sample_no_t samples_per_bar = samples_per_sec * 2;
constexpr sample_no_t usecs_per_sample = 1000000 / samples_per_sec;

//constexpr sample_no_t samples_per_lfo_intv = ;

}

#endif // SAMPLE_H
