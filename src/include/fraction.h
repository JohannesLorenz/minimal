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

#ifndef FRACTION_H
#define FRACTION_H

#include <iosfwd>

namespace mini
{

template<class T1, class T2>
inline constexpr T1 gcd(T1 a, T2 b) {
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

template<class T1, class T2>
inline constexpr T1 lcm(T1 a, T2 b, T1 tmp = 0)
{
//	T1 temp = gcd(a, b);
//	return temp ? (a / temp * b) : 0;
	return (tmp = gcd(a, b)), tmp ? ( a/tmp * b ) : 0;
}

/**
	class for fast and simple fractions

	For restrictions for @a Nominator and @a Denominator, simply try out
	something and see whether you get compiler warnings :P
*/
template<class Numerator, class Denominator>
class fraction_t
{
	using num_t = Numerator;
	using denom_t = Denominator;

	num_t n;
	denom_t d;

public:
	//! TODO: should be private?
	//! creates a fraction from n*m/d, whereas gcd(n,d) must be 1
	//! however, gcd(m,d) = 1 is not required
	fraction_t(num_t _n, denom_t _d, num_t _m, num_t tmp = 0) :
		n(_n * _m/(tmp = gcd(_d, _m))), d(_d/tmp)
	{
		if(!d)
		 throw("d must not be zero in n/d");
	}

public:
	fraction_t(num_t _n, denom_t _d) :
		//n(_n/gcd(_d,_n)), d(_d*n/_n)
		fraction_t(1, _d, _n)
	{
	}

	fraction_t(const fraction_t& ) = default;

	const fraction_t operator+(const fraction_t& rhs) const {
		std::size_t l = lcm(d, rhs.d);
		return fraction_t(n * l / d + rhs.n * l / rhs.d, l);
	}

	bool operator==(const fraction_t& other) const {
		return other.n == n && other.d == d;
	}
	//! prints a fraction as "z+n/d"	
	template<class N, class D>
	friend std::ostream& operator<<(std::ostream& os, const fraction_t<N, D>& b);

	//! returns the floor of n/d
	num_t floor() const { return n/d; }
	//! returns the rest of dividing n/d
	fraction_t rest() const { return fraction_t(n%d, d); }
	//sample_t ceil() const { r } TODO: this is not just n/d + 1

	num_t numerator() const { return n; }
	denom_t denominator() const { return d; }

	bool operator<(const fraction_t& other) const {
		num_t l = lcm(d, other.d);
		// TODO: shortening for if d>o.d && n<o.n and the opposite?
		return n * (l/d) < other.n * (l/other.d);
	}

	// TODO: code duplication, see above
	bool operator>(const fraction_t& other) const {
		num_t l = lcm(d, other.d);
		// TODO: shortening for if d>o.d && n<o.n and the opposite?
		return n * (l/d) > other.n * (l/other.d);
	}

	fraction_t operator-() const { return fraction_t(-n, d); }
	template<class N, class D>
	friend fraction_t<N, D> operator+(
		typename fraction_t<N, D>::num_t val,
		const fraction_t<N, D>& bar);
	template<class N, class D>
	friend fraction_t<N, D> operator*(
		typename fraction_t<N, D>::num_t val,
		const fraction_t<N, D>& bar);

	const fraction_t operator-(const fraction_t& rhs) const {
		return operator+(-rhs);
	}
};

template<class N, class D>
inline fraction_t<N, D> operator+(
	typename fraction_t<N, D>::num_t val,
	const fraction_t<N, D>& bar)
{
	typename fraction_t<N, D>::denom_t d = bar.denominator();
	return fraction_t<N, D>(val * d + bar.numerator(), d);
}

template<class N, class D>
inline fraction_t<N, D> operator*(
	typename fraction_t<N, D>::num_t val,
	const fraction_t<N, D>& bar)
{
	return fraction_t<N, D>(bar.numerator(), bar.denominator(), val);
}

std::ostream & print_fraction(std::ostream& os, unsigned long long n, unsigned long long d);

template<class N, class D>
inline std::ostream& operator<<(std::ostream& os,
		const fraction_t<N, D>& f) {
	return print_fraction(os, f.numerator(), f.denominator());
}

}

#endif // FRACTION_H
