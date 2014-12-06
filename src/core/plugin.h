/*************************************************************************/
/* minimal - a minimal osc sequencer                                     */
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

#ifndef PLUGIN_H
#define PLUGIN_H

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
	void* handle;
public:
	/**
	 * @brief plugin_t
	 * @param path full path to the .so file
	 */
	plugin_t(const char* path);
	~plugin_t();

	bool load_project(project_t& pro);
	bool send_rtosc_msg(const char* path, const char* msg_args, ...);
};

}

#endif // PLUGIN_H
