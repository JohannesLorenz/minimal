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

#include "threadpool/src/include/threadpool.h"
//#include "threadpool/src/include/thread.h" // TODO: get rid of this?

#include "loaded_project.h"

namespace mini {

class engine_t
{
	threadpool::threadpool_t tp;
	std::vector<threadpool::thread_t> threads;

	mini::loaded_project_t lpro;
public:
	engine_t();
	virtual ~engine_t();
	//! should start the engine, such that it will repeatedly call
	//! the processing callback
	virtual void run() = 0;

	void load_project(project_t &&pro);

	void play_until(float end);

/*	//! should return a description like "audio", "video" or "data"
	virtual const char* type_hint() = 0;*/
};

}

#endif // ENGINE_H
