#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <iostream>
#include <limits>
#include "effect.h"
#include "bars.h"
#include "mports.h" // TODO: split in in and out ports?

namespace mini {

enum class timing_behaviour
{
	timing_on_change,
	timing_never
};

class protocol_base_t : public effect_t
{
public:
	bool on_change = false; // TODO: private
	sample_no_t interval; // TODO: private
	protocol_base_t(bool on_change = true, bars_t each_seconds
		= numeric_limits<bars_t>::max());

	void instantiate() {}
	void clean_up() {}
};

template<class T>
class protocol_t : public protocol_base_t, public out_port_templ<T*>
{

public:

	struct m_proto_in : public in_port_templ<T*>
	{
		using in_port_templ<T*>::in_port_templ;
		void on_read(sample_no_t ) {} // TODO??
		//m_proto_in(effect_t& e) : in_port_templ<T*>(e) {}
	};

	m_proto_in input;

	bool _proceed() {

		std::clog << input.data << std::endl;
		return true;
	}

	protocol_t(bool on_change = true, bars_t each_seconds
		= numeric_limits<bars_t>::max()) :
		protocol_base_t(on_change, each_seconds),
		out_port_templ<T*>(*this),
		input(*this, out_port_templ<T*>::data)
		{
	}

};

}

#endif // PROTOCOL_H
