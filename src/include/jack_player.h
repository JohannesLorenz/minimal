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

#ifndef JACK_PLAYER_H
#define JACK_PLAYER_H

#include "audio.h"
#include "jack.h" // TODO: filename...
#include "effect.h"

namespace mini
{

class jack_player_t : public effect_t, public audio_in
{
	//jack::client_t client;
	//multiplex<jack::port_t> ports;
public:
	jack_player_t();
	void instantiate();
	void clean_up() {} // TODO??
	bool _proceed(sample_t );
};

}

#endif // JACK_PLAYER_H
