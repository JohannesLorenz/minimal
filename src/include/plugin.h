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

#ifndef PLUGIN_H
#define PLUGIN_H

#include <string>

namespace mini
{

class project_t;

/**
 * @brief A class to keep dynamically loaded libraries.
 *
 * @note The plugin must be opened as long as the plugins stuff is needed.
 */
class plugin_t
{
	void* handle = nullptr;
public:
	/**
	 * @brief plugin_t
	 * @param path full path to the .so file
	 */
	plugin_t(const char* path);
	~plugin_t();

	bool load_project(project_t& pro);
};

/**
 * @brief A class to keep multiply dynamically loaded libraries.
 *
 * @note The plugin must be opened as long as the plugins stuff is needed.
 */
class multi_plugin_t
{
	std::string path;
	void* get_funcptr(const char* funcname);

	int plugin_id = 0;
public:
	/**
	 * @brief plugin_t
	 * @param path full path to the .so file
	 */
	multi_plugin_t(const char* path);
	multi_plugin_t() = default;

	void set_path(const char* _path) { path = _path; }

	template<class Ret, class ...Args>
	Ret call(const char* function, Args&&... args)
	{
		Ret (*funcptr)(Args...);
		*(void**) (&funcptr) = get_funcptr(function);
		if(funcptr)
		 return funcptr(args...);
		else
		 throw "function not found in dynamic library";
	}

	template<class Ret, class ...Args>
	Ret call_voidptr(const char* function, Args&&... args)
	{
		void*(*funcptr)(Args...);
		*(void**) (&funcptr) = get_funcptr(function);
		if(funcptr)
		 return static_cast<Ret>(funcptr(args...));
		else
		 throw "function not found in dynamic library";
	}
};


}

#endif // PLUGIN_H
