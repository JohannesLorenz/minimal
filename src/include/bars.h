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

#ifndef BARS_H
#define BARS_H

#include "sample.h"
#include <iosfwd>

namespace mini
{

/*class m_time_t
{
	constexpr static double time_per_tick = 1000000 / 1024.0; // in useconds
public:
	tick_t pos; // represents 1/1024 seconds // suggested by fundamental
	void tick() { usleep(time_per_tick); }
	void tick(tick_t n_ticks) { usleep(time_per_tick * n_ticks); }
};*/

constexpr std::size_t gcd(std::size_t a, std::size_t b) {
	return b == 0 ? a : gcd(b, a % b);
}

/*std::size_t gcd(std::size_t a, std::size_t b)
{
	for (;;)
	{
		if (a == 0) return b;
		b %= a;
		if (b == 0) return a;
		a %= b;
	}
}*/

std::size_t lcm(int a, int b)
{
	std::size_t temp = gcd(a, b);

	return temp ? (a / temp * b) : 0;
}

class bars_t
{
	sample_t n, c;

	static sample_t samples_per_bar;
public:
	bars_t(sample_t _n, sample_t _c) :
		n(_n/gcd(_c,_n)), c(_c*n/_n)
	{
		if(!c)
		 throw("c can not be zero in n/c");	
	}
	const bars_t operator+(const bars_t& rhs) const {
		std::size_t _lcm = lcm(c, rhs.c);
		return bars_t(n * _lcm / c + rhs.n * _lcm / rhs.c, _lcm);
	}

	bool operator==(const bars_t& other) const {
		return other.n == n && other.c == c;
	}
	
	friend std::ostream& operator<<(std::ostream& os, const bars_t& b);

	static void set_samples_per_bar(sample_t n) {
		samples_per_bar = n;
	}
	
	sample_t floor() const { return n/c; }
	bars_t rest() const { return bars_t(n%c, c); }
	//sample_t ceil() const { r } TODO: this is not just n/c + 1

	sample_t as_samples_floor() const {
		return bars_t(n * samples_per_bar, c).floor();
	}
};

std::ostream& operator<<(std::ostream& os,
		const bars_t& b);

namespace bars
{

const bars_t _1(1, 1),
	_2(1, 2),
	_3(1, 3),
	_4(1, 4),
	_6(1, 6),
	_8(1, 8),
	_12(1, 12),
	_16(1, 16),
	_24(1, 24),
	_32(1, 32),
	_48(1, 18),
	_64(1, 64);

}

}

#endif // BARS_H
