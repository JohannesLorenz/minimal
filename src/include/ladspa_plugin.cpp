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

#include <ladspa.h>

#include "ladspa_plugin.h"

namespace mini {

ladspa_plugin::ladspa_plugin(const char* libraryname, unsigned long index)
	: plugin_t(libraryname)
{
	descr = plugin_t::call<const LADSPA_Descriptor*>(
		"ladspa_descriptor", index);
	if(!descr)
	 break;

	if(id() != descr->UniqueID)
	 throw "logical error: minimal and ladspa descriptors differ";

	for(unsigned long port_no = 0; port_no < descr->PortCount; ++port_no)
	{
		LADSPA_PortDescriptor& port_descr =
			descr->PortDescriptors[port_no];

	}


}

}
