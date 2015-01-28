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

#ifndef IMPL_H
#define IMPL_H

namespace mini
{

template<class Impl, class T>
class has_impl_t
{
protected:
	Impl* impl;
	T* ref;
public:
	//has_impl_t() : impl(new Impl) {}
	//template<class T>
	has_impl_t(T* ref) : impl(nullptr), ref(ref) {} //impl(new Impl(ref)) {}
	void instantiate() {
		impl = new Impl(ref);
	}
	~has_impl_t() { delete impl; }
	Impl* get_impl() { return impl; }
	const Impl* get_impl() const { return impl; }
};

template<class Ref>
class is_impl_of_t
{
protected:
	Ref* ref;
public:
	template<class T>
	is_impl_of_t(T* ref) : ref(ref) {}
	virtual ~is_impl_of_t() = 0;
};

template<class Ref>
is_impl_of_t<Ref>::~is_impl_of_t() {}

}

#endif // IMPL_H
