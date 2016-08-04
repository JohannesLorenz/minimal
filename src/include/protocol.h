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
	sample_no_t interval, last_call; // TODO: private
	protocol_base_t(bool on_change = true, bars_t each_seconds
		= numeric_limits<bars_t>::max());

	void clean_up() {}
};

template<class T>
class protocol_tbase_t : public protocol_base_t, public out_port_templ<T*>
{
	using input_type = T*;//typename input_type_t<T>::type*;
	using reader_type = typename input_type_t<T*>::type;

	struct log_port_t : public in_port_templ<reader_type> {
		using in_port_templ<reader_type>::in_port_templ;
		void on_read(sample_no_t ) {}
	};

	log_port_t log_port;

	void instantiate() {
		// assign pointers for redirection
		out_port_templ<T*>::data = input.data;
		log_port << static_cast<out_port_templ<T>&>(*input.get_source());
	}
public:

	struct m_proto_in : public in_port_templ<input_type>
	{
		using in_port_templ<input_type>::in_port_templ;
		void on_read(sample_no_t ) {} // TODO??
		//m_proto_in(effect_t& e) : in_port_templ<T*>(e) {}
	};

	m_proto_in input;

	bool _proceed()
	{
	/*	std::clog << time() << " <-> " << last_call << ", " << interval << std::endl;
		if(time() - last_call >= interval)
		{
			last_call = time();
			std::clog << log_port.data << std::endl;
		}
		return true;*/
		std::clog << log_port.data << std::endl;
		set_next_time(time() + interval);
		return true;
	}

	protocol_tbase_t(bool on_change = true, bars_t each_seconds
		= numeric_limits<bars_t>::max()) :
		protocol_base_t(on_change, each_seconds),
		out_port_templ<T*>((effect_t&)*this),
		log_port(*this),
		input(*this)
		{
	}

};

template<class T>
class protocol_t : protocol_tbase_t<T> {
	using protocol_tbase_t<T>::protocol_tbase_t;
};

}

#endif // PROTOCOL_H
