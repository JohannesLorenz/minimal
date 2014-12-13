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

#ifndef EFFECT_H
#define EFFECT_H

#include <set>
#include <vector>
#include "types.h"
#include "work_queue.h"

const float default_step = 0.1f; //0.001seconds; // suggested by fundamental

namespace mini
{

class effect_t;

template<class T, class RefT>
class base_port
{
protected:
	effect_t* ef_ref;
	RefT ref;
	//RefT last_value;
	bool _changed;
	void reset_value(const T& new_value) {
		_changed = (ref != new_value);
		ref = new_value;
	}

public:
	const effect_t* get_ef_ref() const { return ef_ref; }
	effect_t* get_ef_ref() { return ef_ref; }
	bool changed() { return changed; }
	using type = T;
	const RefT& get() const { return ref; }
	base_port(effect_t& ef_ref) :
		ef_ref(&ef_ref) {}
};


template<class T>
class out_port : public base_port<T, T>
{
	using base = base_port<T, T>;
public:
	const T& set(const T& new_val) { return base::ref = new_val; }
	using base_port<T, T>::base_port;
};


template<class T>
class in_port : public base_port<T, T>
{
	using base = base_port<T, T>;
	const out_port<T>* out_p;
public:
	void connect(out_port<T>& op) {
		out_p = &op;
		op.get_ef_ref()->readers.push_back(base::ef_ref);
		base::ef_ref->writers.push_back(op.get_ef_ref());
	}

	void update() { reset_value(out_p->get()); }

	using base_port<T, T>::base_port;
};

class effect_t
{
public:
	std::vector<effect_t*> readers, writers;
	// returns the next time when the effect must be started
	virtual float proceed(float time) = 0;
};

constexpr unsigned char MAX_NOTES_PRESSED = 32;

class note_line_t : public effect_t, public work_queue_t
{
	int notes_pressed[MAX_NOTES_PRESSED];
	using notes_pressed_ref = int*;
	out_port<notes_pressd_ref> notes_pressed;

	struct note_task_t : public task_base
	{
		const loaded_instrument_t* ins;
		const command_base* cmd;
		std::set<float>::const_iterator itr;

		void proceed(float time) {
			ins->con.send_osc_str(cmd->buffer());
			update_next_time(*++itr);
		}
	};



	float proceed(float time) {

	}
};

}

#endif // EFFECT_H
