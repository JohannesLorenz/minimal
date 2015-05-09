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
#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>

#include "project.h"
#include "io.h"
#include "config.h"

#include "plugin.h"

namespace mini {

plugin_t::plugin_t(const char *path)
{
	no_rt::mlog << "Opening plugin: " << path << std::endl;

	// valgrind memory leak is not our fault:
	handle = dlopen(path, RTLD_LAZY);
	if(!handle)
	 throw dlerror();
}

plugin_t::~plugin_t() { if(handle) dlclose(handle); }

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

void cp_file(const char* dst_name, const char* src_name)
{
	std::ifstream src(src_name, std::ios::binary);
	std::ofstream dst(dst_name, std::ios::binary);
	if(!src.good())
	 throw "cp: Could not open source file";
	else if(!dst.good())
	 throw "cp: Could not open dest file";
	dst << src.rdbuf();
}

// TODO: check for rtld_private


#ifdef HAVE_RTLD_PRIVATE
#error "to be implemented"
#endif

void* multi_plugin_t::get_funcptr(const char* funcname)
{
	int dlopen_mode = RTLD_NOW | RTLD_LOCAL;
	const char* tmp_name;

	// use RTLD_PRIVATE or copy the library
#ifdef HAVE_RTLD_PRIVATE
	dlopen_mode |= RTLD_PRIVATE; // TODO: please someone test this...
	tmp_name = path.c_str();
#else
	const std::string tmp_file_name = "/tmp/minimal_"
		+ std::string(funcname) + "_"
		+ std::to_string(plugin_id) + ".so";
	cp_file(tmp_file_name.c_str(), path.c_str());
	tmp_name = tmp_file_name.c_str();
#endif

	no_rt::mlog << "Calling function " << funcname <<
		" from plugin: " << path << std::endl;
	// load the library
	void* handle = dlopen(tmp_name, dlopen_mode);
	if(!handle)
	 throw std::string(dlerror());
	// TODO: dlclose in ctor?

	// remove the temporary library
#ifndef HAVE_RTLD_PRIVATE
	remove(tmp_file_name.c_str());
#endif

	// retrieve function pointer as void*
	void* fptr = dlsym(handle, funcname);

	const char* error;
	if ((error = dlerror()))  {
		no_rt::mlog << "Error calling function from plugin: "
			  << error << std::endl;
		return nullptr; // TODO: throw?
	}
	else return fptr;
}

multi_plugin_t::multi_plugin_t(const char *path)
	: path(path)
{
}

}
