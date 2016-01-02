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

	// TODO: assure that n will always carry the minus?
	num_t n;
	denom_t d;

//	constexpr fraction_t(num_t _n, denom_t _d) : n(_n), d(_d) {}
public:
private:
	//! creates a fraction from n*m/d, where _gcd = gcd(n,d) != 1
	//! however, gcd(m,d) = 1 is not required
	constexpr fraction_t(num_t _n, denom_t _d, num_t _m, num_t _gcd) :
		n(_n * _m/_gcd), d(_d/_gcd)
	{
		//if(!d)
		// throw("d must not be zero in n/d");
	}

	//! creates a fraction from n*m/d, whereas gcd(n,d) must be 1
	//! however, gcd(m,d) = 1 is not required
	constexpr fraction_t(num_t _n, denom_t _d, num_t _m) :
		//n(_n * _m/(tmp = gcd(_d, _m))), d(_d/tmp)
		fraction_t(_n, _d, _m, gcd(_d, _m))
	{
		//if(!d)
		// throw("d must not be zero in n/d");
	}


public:
	constexpr fraction_t(num_t _n, denom_t _d) :
		//n(_n/gcd(_d,_n)), d(_d*n/_n)
		fraction_t(1, _d, _n)
	{
	}

	fraction_t(const fraction_t& ) = default;

	fraction_t operator+(const fraction_t& rhs) const {
		num_t l = lcm(d, rhs.d);
		return fraction_t(n * l / d + rhs.n * l / rhs.d, l);
	}

	fraction_t& operator+=(const fraction_t& rhs) {
		return (*this = *this + rhs);
	}

	constexpr bool operator==(const fraction_t& other) const {
		return other.n == n && other.d == d;
	}

	constexpr bool operator!=(const fraction_t& other) const {
		return ! operator==(other);
	}

	//! prints a fraction as "z+n/d"	
	template<class N, class D>
	friend std::ostream& operator<<(std::ostream& os, const fraction_t<N, D>& b);

	//! returns the floor of n/d
	num_t floor() const { return n/d; }
	//! returns the rest of dividing n/d
	fraction_t rest() const { return fraction_t(n%d, d); }
	//sample_no_t ceil() const { r } TODO: this is not just n/d + 1

	num_t numerator() const { return n; }
	denom_t denominator() const { return d; }

	bool operator<(const fraction_t& other) const {
		denom_t l = lcm(d, other.d);
		// TODO: shortening for if d>o.d && n<o.n and the opposite?
		return n * (l/d) < other.n * (l/other.d);
	}

	// TODO: code duplication, see above
	bool operator>(const fraction_t& other) const {
		num_t l = lcm(d, other.d);
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

	fraction_t operator-(const fraction_t& rhs) const {
		return operator+(-rhs);
	}
	
	fraction_t operator*(const fraction_t& other) const
	{
		// only need to calc gcd twice
		fraction_t f1(n, other.d), f2(other.n, d);
		return {f1.n * f2.n, f1.d * f2.d};
	}

	fraction_t& operator*=(const fraction_t& rhs) {
		return (*this = *this * rhs);
	}
	
	//! allows multiplication without multiplication sign
	//! if you don't like this, inherit and overwrite
	template<class T>
	auto operator()(const T& t) -> decltype(*this * t) {
		return *this * t;
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

std::ostream & print_fraction(std::ostream& os,
	long long z,
	signed long long n, signed long long d);

template<class N, class D>
inline std::ostream& operator<<(std::ostream& os,
		const fraction_t<N, D>& f) {
	typename fraction_t<N, D>::num_t floor = f.floor();
	typename fraction_t<N, D>::num_t rest = f.numerator() - f.floor() * f.denominator();
	return print_fraction(os, floor, rest, f.denominator());
}

}

#endif // FRACTION_H

