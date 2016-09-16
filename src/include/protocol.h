#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <sstream>
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
class protocol_tbase_t : public protocol_base_t, public out_port_ref_t<T>
{
	using input_type = T*;//typename input_type_t<T>::type*;
	using reader_type = typename input_type_t<T*>::type;

/*	class log_port_t : public in_port_t<reader_type, T> {
		using base = in_port_t<reader_type, T>;
	public:
		using base::base;
		void on_read(sample_no_t ) override {}
		// WARNING: this will not work if base::data is a pointer!
		void instantiate_port() override {
			base::data = base::source->value(); }
	};

	log_port_t log_port; */

	reader_type reader;

	void instantiate() {
		// assign pointers for redirection
		out_port_ref_t<T>::ref() = input.get();
		std::cerr << "redirect: " << out_port_ref_t<T>::ref();

		m_assign(reader, input.value());
		//log_port << static_cast<out_port_ref_t<T>&>(*input.get_source());
	}
public:

	class m_proto_in : public in_port_t<input_type, T, true>
	{
		using base = in_port_t<input_type, T, true>;
	public:
		using in_port_t<input_type, T, true>::in_port_t;
		void on_read(sample_no_t ) override {} // TODO??
		// TODO: add dummy to mports.h?
		void instantiate_port() override { base::get() = &base::source->value(); }
		//m_proto_in(effect_t& e) : in_port_t<T*>(e) {}
	};

	m_proto_in input;
	std::string last_value;
	bool _proceed()
	{
	/*	std::clog << time() << " <-> " << last_call << ", " << interval << std::endl;
		if(time() - last_call >= interval)
		{
			last_call = time();
			std::clog << log_port.data << std::endl;
		}
		return true;*/
//		std::clog << /*log_port.data*/ reader << std::endl;
//		std::clog << time() << " <-> " << interval << std::endl;
		std::stringstream ss;

//		ss << reader;

		ss << detail::deref_if_ptr(reader);

		if(input.needs_update())
		{
			//if(ss.str() != last_value)
			{
				last_value = ss.str();
				std::cout << as_bars(time(), info.samples_per_bar) << ": " << last_value << std::endl;
			}

			// these two *must* be ccalled, even if we don't read/write any data
			input.update();
			out_port_ref_t<T>::notify_set(time());
		}

		std::cerr << "next_time: " << as_bars(time() + interval, info.samples_per_bar) << std::endl;

		set_next_time(time() + interval);

		return true;
	}

	protocol_tbase_t(bool on_change = true, bars_t each_seconds
		= numeric_limits<bars_t>::max()) :
		protocol_base_t(on_change, each_seconds),
		out_port_ref_t<T>((effect_t&)*this),
		//log_port(*this),
		input(*this)
		{
	}

};

template<class T>
class protocol_t : public protocol_tbase_t<T> {
	using protocol_tbase_t<T>::protocol_tbase_t;
};

}

#endif // PROTOCOL_H
