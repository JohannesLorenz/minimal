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

#ifndef ENGINE_H
#define ENGINE_H

#include "atomic.h"
//#include <threadpool/src/include/threadpool.h>
//#include <threadpool/src/include/thread.h>

#include "player.h"
#include "io.h"

namespace mini {

/*class main_tp : public threadpool::threadpool_t
{
};

class other_tp : public threadpool::threadpool_t
{
	std::vector<threadpool::thread_t> threads;
public:
	other_tp();
};

class multi_tp : public main_tp, other_tp
{
	virtual bool callback() {
		return true;
	}
};*/

//! There must be a class containing player and threadpool, and this one
//! volunteered for it.
class engine_t
{
	atomic_def<bool, false> _is_running;

	//multi_tp tp;

	//mini::loaded_project_t lpro;
	//mini::project_t lpro;

	//! should start the engine, such that it will repeatedly call
	//! the processing callback
	virtual void vrun(bars_t limit) = 0;

	virtual sample_no_t get_sample_rate() = 0;

protected:
	//! processes all fx of the player
	void proceed(sample_no_t samples);

	void stop() { _is_running.store(false); }
	player_t player;

public:
	virtual ~engine_t();

	void load_project(project_t &pro);

	//void play_until(bars_t end); // deprecated?

	bool is_running() const { return _is_running; }

	void run_until(bars_t limit)
	{
		if(_is_running) {
			throw "engine already running";
		} else {
			_is_running.store(true);
			vrun(limit);

			// update info class now
			// FEATURE: return sample rate?
			info.global_samplerate = get_sample_rate();
			info.recompute();

			//_is_running.store(false);
			//io::mlog << "Engine finished..." << io::endl;
		}
	}



/*	//! should return a description like "audio", "video" or "data"
	virtual const char* type_hint() = 0;*/
};

}

#endif // ENGINE_H
