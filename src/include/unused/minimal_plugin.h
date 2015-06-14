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

#ifndef MINIMAL_PLUGIN_H
#define MINIMAL_PLUGIN_H

//#include <iostream> // TODO!!

class minimal_plugin
{
	virtual void _send_osc_cmd(const char*) = 0;
protected:
	using sample_rate_t = unsigned long;
	using sample_t = unsigned long;
public:
	void send_osc_cmd(const char* msg) {
		//std::cerr << "sending: " << msg << std::endl;
		_send_osc_cmd(msg); }
	virtual void prepare() = 0;
	virtual bool proceed(sample_t sample_count) = 0;
	virtual ~minimal_plugin() {} // dynamic lib dtors must not be pure
};

#endif // MINIMAL_PLUGIN_H
