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

#include <cstdlib>
#include <cassert>
#include <unistd.h>
#include <termios.h>

#include "loaded_project.h"
#include "plugin.h"
#include "lo_port.h"
#include "io.h"

#include "threadpool/src/include/thread.h"

using namespace mini;

void main_init()
{
	// TODO: needed?
	//For misc utf8 chars
	setlocale(LC_ALL, "");

	struct termios ctrl;
	tcgetattr(STDIN_FILENO, &ctrl);
	// turn off canonical mode => input unbuffered
	ctrl.c_lflag &= ~ICANON;
	tcsetattr(STDIN_FILENO, TCSANOW, &ctrl);
}

/*project_t main_load_project(const char* lib_name)
{
	plugin_t plugin = plugin_t(lib_name);

	project_t pro;

	std::cout << "Attempting to load project: " << lib_name << std::endl;
	plugin.load_project(pro); // TODO: return pro?

	std::cout << "Loaded project: " << pro.title() << std::endl;

//	loaded_project lo_pro(project_t(pro));

	return pro;
}*/

int main(int argc, char** argv)
{
	try {
		main_init();

		assert(argc == 2);

		//std::cout << "main:" << main_load_project(argv[1]).title() << std::endl;

		plugin_t plugin(argv[1]);
		project_t pro;

		plugin.load_project(pro);
		
		engine eng;
		eng.load_project(pro);
		eng.play_until(4.0f);

		/*loaded_project_t lpro(std::move(pro));
		
		player_t<int> pl(lpro);
		pl.play_until(4.0f);*/

	//	sleep(5);
	} catch(const char* msg) {
		no_rt::mlog << "Aborting on error thrown: " << std::endl
			<< msg << std::endl;
	}

	return EXIT_SUCCESS;
}
