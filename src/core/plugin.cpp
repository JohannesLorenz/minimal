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

#include <dlfcn.h>

#include "project.h"
#include "io.h"

#include "plugin.h"

namespace mini {

plugin_t::plugin_t(const char *path)
{
	// valgrind memory leak is not our fault:
	handle = dlopen(path, RTLD_LAZY);
	if(!handle)
	 throw dlerror();
}

plugin_t::~plugin_t() { dlclose(handle); }

bool plugin_t::load_project(project_t &pro)
{
	void (*init_project)(project_t&);
	const char *error;

	// for the cast syntax, consult man dlopen (3)
	*(void**) (&init_project) = dlsym(handle, "init");

	if ((error = dlerror()))  {
		no_rt::mlog << "Error calling init() from plugin: "
			  << error << std::endl;
		pro.invalidate();
		return false; // TODO: throw?
	}

	init_project(pro);
	return true;
}

}
