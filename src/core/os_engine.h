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

#ifndef OS_ENGINE_H
#define OS_ENGINE_H

#include <thread>
#include "engine.h"

namespace mini {

class os_engine_t : public engine_t
{
	std::thread run_thread;
	const sample_no_t sample_rate;

	bool process(sample_no_t samples);
/*	void shutdown() {
		stop();
		//throw "shutdown not implemented :P";
	}
*/
	sample_no_t get_sample_rate() { return sample_rate; }

	sample_no_t limit = 0, samples_until_now = 0;

	void vrun(bars_t _limit);

	//! permanent function for @a run_thread
	void run_loop();
	
	// FEATURE: can the need of static functions be avoided with C++11 threads?
	static void run_loop_static(os_engine_t* obj) { obj->run_loop(); }
public:
	const char* type_hint() { return "none"; }

	//! @param sample_rate desired sample rate for this engine
	os_engine_t(sample_no_t sample_rate = 1024)
		: sample_rate(sample_rate) {
	}
};

}

#endif // OS_ENGINE_H
