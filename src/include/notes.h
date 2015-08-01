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

#ifndef NOTES_H
#define NOTES_H

// TODO: forw decl?
#include "mports.h"
#include "note_line.h"
#include "daw.h"

namespace mini {

using notes_t = events_t<music_note_properties>;
using note_signal_t = event_signal_t<music_note_properties>;
using note_line_t = event_line_t<music_note_properties>;
using notes_in = events_in_t<music_note_properties>;
using notes_out = events_out_t<music_note_properties>;

}

#endif // NOTES_H
