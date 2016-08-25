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

#include <unistd.h>
#include <fstream>

#include "pid.h"

//!< maximum lenght a pid has on any POSIX system
//!< this is an estimation, but more than 12 looks insane
constexpr std::size_t max_pid_len = 12;

//!< safe pid lenght guess, posix conform
std::size_t os_guess_pid_length()
{
    const char* pid_max_file = "/proc/sys/kernel/pid_max";
    if(-1 == access(pid_max_file, R_OK)) {
	return max_pid_len;
    }
    else {
	std::ifstream is(pid_max_file);
	if(!is.good())
	    return max_pid_len;
	else {
	    std::string s;
	    is >> s;
	    for(const auto& c : s)
		if(c < '0' || c > '9')
		    return max_pid_len;
	    return std::min(s.length(), max_pid_len);
	}
    }
}

//!< returns pid padded, posix conform
std::string os_pid_as_padded_string(pid_t pid)
{
    char result_str[max_pid_len << 1];
    std::fill_n(result_str, max_pid_len, '0');
    std::size_t written = snprintf(result_str + max_pid_len, max_pid_len,
	"%d", (int)pid);
    // the below pointer should never cause segfaults:
    return result_str + max_pid_len + written - os_guess_pid_length();
}

std::string os_pid_as_padded_string()
{
    return os_pid_as_padded_string(getpid());
}

