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

#ifndef IO_H
#define IO_H

//#define ENABLE_IO

//#ifdef ENABLE_IO
#include <iostream>
//#endif

namespace mini {
namespace io {

#ifdef ENABLE_IO
using mlog_t = std::ostream;
mlog_t& mlog = std::clog;
mlog_t& endl(mlog_t& os) {
	return std::endl(os);
}
mlog_t& mlog_no_rt = std::clog;
#else

struct mlog_t
{
	//! this function does nothing
	template<class T>
	mlog_t& operator<<(const T&) {
		return *this;
	}
};

extern mlog_t mlog;

inline mlog_t& endl(mlog_t& os) {
	return os;
}

/*namespace std
{
template<typename _CharT, typename _Traits>
class basic_ostream;
typedef basic_ostream<char> 		ostream;
}*/

// TODO: fwd declare ostream

extern std::ostream& mlog_no_rt;
#endif

}
}

#endif // IO_H
