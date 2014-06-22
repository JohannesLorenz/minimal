/*************************************************************************/
/* mmms - minimal multimedia studio                                      */
/* Copyright (C) 2014-2014                                               */
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
#include <iostream>
#include "project.h"

#include "plugin.h"

plugin_t::plugin_t(const char *path)
{
	handle = dlopen(path, RTLD_LAZY);
	if(!handle)
	{
		std::cerr << "Error loading library " << path << ": "
			  << dlerror() << std::endl;
		/*return false;*/ // TODO: throw
	}
}

plugin_t::~plugin_t() { dlclose(handle); }

bool plugin_t::load_project(project &pro)
{
	void (*init_project)(project&); // TODO: prefer "using"
	char *error;

	// for the cast syntax, consult man dlopen (3)
	*(void**) (&init_project) = dlsym(handle, "init");

	if ((error = dlerror()))  {
		std::cerr << "Error calling init() from plugin: "
			  << error << std::endl;
		return false;
	}

	init_project(pro);
	return true;
}

