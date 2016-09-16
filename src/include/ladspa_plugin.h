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

#ifndef LADSPA_PLUGIN_H
#define LADSPA_PLUGIN_H

#include "plugin.h"

#include "effect.h"
#include "mports.h"
#include "audio_instrument.h"

namespace mini {

class ladspa_plugin : public audio_instrument_t, plugin_t
{
	//! the descriptor: an instance returning all info,
	//! also returning the handle which has the implementation
	const struct LADSPA_Descriptor* descr;

	//! the implementation of the plugin
	struct LADSPA_Handle handle;

	bool _proceed();
protected:
	using id_t = unsigned long;
	using in_port_data = audio;
	using in_port_control = in_port_t<float>;
public:
	ladspa_plugin(const char* libraryname, unsigned long index);
	bool loading_was_successfull() const { return descr; }

	virtual void instantiate() = 0;
	virtual void clean_up() = 0;
	virtual void pass_changed_ports(const std::vector<bool>& ) {}

};

class ladspa_glame_lp2
{
	id_t id() const { return 1903; }
	in_port_t<float> cutoff_frequency;
};

}

#endif // LADSPA_PLUGIN_H
