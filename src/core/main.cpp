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

#include <cstdlib>
#include <cassert>
#include <iostream>
#include "project.h"
#include "plugin.h"
#include "ports.h"

using namespace mmms;

void main_init()
{
	// TODO: needed?
	//For misc utf8 chars
	setlocale(LC_ALL, "");
}

project_t main_load_project(const char* lib_name)
{
	plugin_t plugin(lib_name);

	project_t pro;

	std::cout << "Attempting to load project: " << lib_name << std::endl;
	plugin.load_project(pro);

	std::cout << "Loaded project: " << pro.title() << std::endl;
	return pro;
}

int main(int argc, char** argv)
{
	main_init();
	lo_server_t server;

	assert(argc == 2);

	project_t pro = main_load_project(argv[1]);

//	usleep(100);

	return EXIT_SUCCESS;
}
