/*************************************************************************/
/* minimal - a minimal rtosc sequencer                                   */
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

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <tuple>
#include <string>
#include <vector>
#include <memory>
#include <iostream> // TODO
#include <limits>
#include <map>
#include <set>
// ...
// when is it going to end?

#include "types.h"
#include "utils.h"
#include "daw.h"
#include "rtosc_string.h"

namespace mini
{


#if 0

template<class Functor, class Tar, class ...Src>
class func
{
	using id_t = std::size_t;
	static id_t next_id;
	Functor ftor;
public:
	id_t id;
	func(Functor& ftor) : ftor(ftor), id(next_id++) {}
	Tar operator()(Src... input) { return ftor(input...); }
};


template<class F, class T, class ...S>
typename func<F, T, S...>::id_t func<F, T, S...>::next_id;

template<class T>
class con
{
	using id_t = std::size_t;
	static id_t no_id() { return std::numeric_limits<id_t>::max(); }
public:
	id_t id;
	T value;
	bool is_fixed() const { return id == no_id(); }
	con(const T& fixed_value) :
		id(no_id()),
		value(fixed_value)
	{
	}
	con(const typename T::value_type& fixed_value) :
		id(no_id()),
		value(fixed_value)
	{
	}

	template<class F, class ...S>
	con(const func<F, T, S...>& func) :
		id(func.id),
		value(0)
	{
	}


/*	con(const con<T>& other) = default;
	con(const con<typename T::base_type>& other) :
		id(other.id),
		value(other.value)
	{
	}*/
};
#endif


class command_base
{
protected:
	std::string _path; // TODO: std string everywhere
public:

	using call_functor = void (*)(const char*, const char*, ...);

	const std::string& path() const { return _path; }
	command_base(const char* _path) : _path(_path) {}
	virtual std::string type_str() const = 0;
	virtual const rtosc_string& complete_buffer() const = 0;

//	virtual void execute(functor_base<>& ftor) const = 0;

//	virtual command_base* clone() const = 0; // TODO: generic clone class?
	virtual const rtosc_string& buffer() const = 0;
	virtual ~command_base() = 0;

	virtual bool operator==(const command_base& other) const = 0;
	virtual bool operator<(const command_base& other) const = 0;
};


namespace detail
{

	template<class T>
	constexpr std::size_t length_of() {
//		return util::dont_instantiate_me_func<std::size_t>();
		return T::size();
	}
//	template<>
//	constexpr std::size_t length_of<int>() { return 4; }

	template<class... Others>
	constexpr std::size_t est_length_args();

	template<class Arg1, class ...More>
	constexpr std::size_t est_length_args_first()
	{
		return length_of<Arg1>() + est_length_args<More...>();
	}

	template<class... Others>
	constexpr std::size_t est_length_args()
	{
		return est_length_args_first<Others...>();
	}

	template<>
	constexpr std::size_t est_length_args<>() { return 0; }
}

template<std::size_t PadSize>
constexpr std::size_t pad(std::size_t pos) {
	return pos + ((PadSize - pos % PadSize) % PadSize);
}

/*std::string to_osc_string(float f) {
	std::string res = "0000";
	*(float*)res.data() = f;
	return res;
}

std::string to_osc_string(int32_t i) {
	std::string res = "0000";
	*(int*)res.data() = i;
	return res;
}*/

namespace command_detail
{
/*	template<class T>
	void _append_single(std::string& , const T& ) {
		// general case: can not append
	}

	template<class T, char Sign>
	void _append_single(std::string& s, const variable<no_port<T>, Sign>& v) {
		s += ' ' + to_osc_string(v.value());
	}*/


	template<bool Printable> // Printable = false
	struct _append_single
	{
		template<class T>
		static void exec(std::vector<char>& , const T& )
		{
			// general case: can not append
		}
	};

	template<>
	struct _append_single<true>
	{
		template<class T>
		static void exec(std::vector<char>& s, const T& elem)
		{
			std::cerr << "app single" << std::endl;
			std::vector<char> osc_str = elem.to_osc_string(); // TODO: this is too slow
			std::copy(osc_str.begin(), osc_str.end(), std::back_inserter(s)); // TODO: move?
		}
	};

	template<bool SizeFix> // SizeFix = false
	struct _fill_single
	{
		template<class T>
		static void exec(std::vector<char>& )
		{
			// general case: can not fill
		}
	};

	template<>
	struct _fill_single<true>
	{
		template<class T>
		static void exec(std::vector<char>& s)
		{
			std::cerr << "fill single" << std::endl;
			s.resize(s.size() + T::size());
			std::fill(s.end() - T::size(), s.end(), 0); // debug only
		}
	};


	template<bool Ok, std::size_t N, std::size_t I, class ...Args2>
	struct _append
	{
		static void exec(std::vector<char>& s, const std::tuple<Args2...>& tp)
		{
			using tp_at = typename std::tuple_element<I, std::tuple<Args2...>>::type;
			// case 1: it's const -> fill it in
			_append_single<tp_at::is_const()>::exec(s, std::get<I>(tp));
			// case 2: not const, but fixed size -> buffer it
			constexpr bool case_2 = (!tp_at::is_const()) && tp_at::size_fix();
			_fill_single<case_2>::template exec<tp_at>(s);
			// continue if const or fix
			_append<tp_at::size_fix() || tp_at::is_const(), N, I+1, Args2...>::exec(s, tp);
		}
	};

	template<std::size_t N, std::size_t I, class ...Args2>
	struct _append<false, N, I, Args2...>
	{
		static void exec(std::vector<char>& , const std::tuple<Args2...>& )
		{
			// not ok
		}
	};

	template<bool Ok, std::size_t N, class ...Args2>
	struct _append<Ok, N, N, Args2...>
	{
		static void exec(std::vector<char>& , const std::tuple<Args2...>& )
		{
			// end reached
		}
	};



	template<bool DoComplete> // DoComplete = false
	struct _complete_single
	{
		template<class ...Args2>
		static void exec(std::vector<char>::iterator& , const std::tuple<Args2...>& )
		{
			// general case: can not fill
		}
	};

	template<>
	struct _complete_single<true>
	{
		template<class ...Args2>
		static void exec(std::vector<char>::iterator& , const std::tuple<Args2...>& )
		{
			// TODO next
		}
	};


	template<std::size_t N, std::size_t I, class ...Args2>
	struct _complete
	{
		static void exec(std::vector<char>::iterator& itr, const std::tuple<Args2...>& tp)
		{
			using tp_at = typename std::tuple_element<I, std::tuple<Args2...>>::type;

			// TODO: non fix size
			_complete_single<tp_at::size_fix() && !tp_at::is_const()>::exec(itr, std::get<I>(tp));

		/*	// case 1: it's const -> fill it in
			_append_single<tp_at::is_const()>::exec(s, std::get<I>(tp));
			// case 2: not const, but fixed size -> buffer it
			_fill_single<tp_at::size_fix()>::template exec<tp_at>(s);
			// continue if const or fix
			_append<tp_at::size_fix() || tp_at::is_const(), N, I+1, Args2...>::exec(s, tp);*/
		}
	};

	template<std::size_t N, class ...Args2>
	struct _complete<N, N, Args2...>
	{
		static void exec(std::vector<char>::iterator& , const std::tuple<Args2...>& )
		{
			// end reached
		}
	};

}

template<class ...Args>
class command : public command_base
{
	using self = command<Args...>;
	std::tuple<Args...> args;


public:
	mutable rtosc_string _buffer;
private:

	constexpr std::size_t est_length() const {
		return pad<4>(path().length() + 1) // path + \0
			+ pad<4>(sizeof...(Args) + 2)// ,<types>\0
			+ detail::est_length_args<Args...>();
	}

/*	template<class ...Args2>
	void _append(std::string& , Args2... )
	{
		// general case: can not be done
	}

	void _append(std::string& )
	{
		// end reached
	}

	template<class T, char sign, class ...Args2>
	void _append(std::string& prefix, variable<no_port<T>, sign> v, Args2... more_args)
	{

		_append(prefix, more_args...);
	}

	template<class T, char sign, class ...Args2>
	void _append(std::string& prefix, variable<no_port<T>, sign> v, Args2... more_args)
	{

		_append::exec(prefix, more_args...);
	}*/

	rtosc_string prefill_buffer() const
	{
		std::vector<char> res(_path.begin(), _path.end());
		do {
		 res.push_back('\0');
		} while(res.size() % 4);
		std::string t_s = type_str();
		std::copy(t_s.begin(), t_s.end(), std::back_inserter(res));
		do {
		 res.push_back('\0');
		} while(res.size() % 4);
		command_detail::_append<true, sizeof...(Args), 0, Args...>::exec(res, args);
		return res;
	}


public:
	command(const char* _path, Args... args) :
		command_base(_path),
		args(args...),
		_buffer(prefill_buffer()) {

		std::cerr << "est. length: " << est_length() << std::endl;
	}

	const rtosc_string& complete_buffer() const
	{
//		command_detail::_complete<sizeof...(Args), 0, Args...>::exec(buffer, args);
		return _buffer;
	}
	const rtosc_string& buffer() const { return _buffer; }


	std::string type_str() const {
		std::string res { ',' , Args::sign()... };
		return res; // TODO: in one line!
	}

	/*std::string prepare_buffer() {

	}*/

	//
	virtual bool operator==(const command_base& other) const {
		//return other.path() == _path && other.type_str() == type_str();
		return _buffer.operator==(other.buffer());
	}
	virtual bool operator<(const command_base& other) const {
		return _buffer.operator<(other.buffer());
	/*	std::size_t pathdiff = other.path().compare(_path);
		if(pathdiff)
		 return (pathdiff > 0);
		else
		 return type_str() < other.type_str();*/
	}

/*	template<class ...Args2>
	bool operator==(const command<Args2...>* other) {
		return (other->_path == _path) && other->type_str() == type_str();
	}*/




	/*void execute(functor_base& ftor) const {
		std::cout << "EXE..." << std::endl;

	_execute(ftor, typename gens<sizeof...(Args)>::type()); }*/

/*	self& casted() { return *this; }

	template<std::size_t i>
	auto arg() -> decltype(std::get<i>(args)) const // TODO: not use decltype
	{ return std::get<i>(args); }*/




	//std::string arg(std::size_t i)

	/*std::string rtosc_msg() const
	{
		std::string res = path;
		std::string type_str { Args::sign()... };
		res += " " + type_str;
		//for(std::size_t i = 0; i < sizeof...(Args); ++i)
		// res += " " + std::get<i>().to_str();
		// ^^ TODO!!!
		return res;
	}*/
//	virtual command* clone() const { return new command(*this); }
	virtual ~command(); // TODO: = 0 ?
};

// TODO: cpp file?
template<class ...Args>
command<Args...>::~command() {}

#include "utils.h" // TODO

class named_t
{
	const std::string _name;
public:
	const std::string& name() const { return _name; }
	named_t(const char* _name) : _name(_name) {}
};


struct map_cmp
{
	bool operator()(const command_base* c1, const command_base* c2)
	{
		return *c1 < *c2;
	}
};

// height, command + times
using cmd_vectors = std::map<const command_base*, std::set<float>, map_cmp>; // TODO: prefer vector?

class instrument_t : public named_t, non_copyable_t
{
public:
	using id_t = std::size_t;
private:
	static std::size_t next_id;
	const std::size_t _id;
	std::vector<command_base*> commands; // TODO: unique?
	const std::vector<command_base*> _quit_commands;
public:
	const std::vector<command_base*>& quit_commands() const {
		return _quit_commands;
	}
	using port_t = int;
	instrument_t(const char* name,
		const std::vector<command_base*>&& _quit_commands) :
		named_t(name),
		_id(next_id++),
		_quit_commands(_quit_commands)
		{ std::cout << "instrument: constructed" << std::endl; }
	virtual ~instrument_t();
//	virtual instrument_t* clone() const = 0; // TODO: generic clone class?

	//instrument_t(const instrument_t& other);


	const id_t& id() const { return _id; }
	enum class type
	{
		zyn
	};
/*	template<class ...Args>
	void add_param_fixed(const char* param, Args ...args) {
		using command_t = command<Args...>;
		commands.push_back(std::unique_ptr<command_t>(new command_t(param, args...)));
	}*/


	template<class C, class ...Args>
	void add_command_fixed(Args ...args) {
		commands.push_back(new C(args...));
	}

	void set_param_fixed(const char* param, ...);
	virtual std::string make_start_command() const = 0;
	//! shall return the lo port (UDP) after the program was started
	virtual port_t get_port(pid_t pid, int fd) const = 0;
	//instrument_t(instrument_t&& other) = default;

	virtual cmd_vectors make_note_commands(const std::multimap<daw::note_geom_t, daw::note_t>& ) const = 0;
};

template <char ...Letters> class fixed_str {
	static std::string make_str() { return std::string(Letters...); }
};


class zynaddsubfx_t : public instrument_t
{
	// TODO: read from options file
/*	const char* binary
		= "/tmp/cprogs/fl_abs/gcc/src/zynaddsubfx";
	const char* default_args = "--no-gui -O alsa";*/
public:

	/*template<template<class> C1, template<class> C2>
	class note_on : public command<int_f, int_f, int_f> { //using command::command;
	public:
		note_on(con<p_char> x, con<p_char> y, con<p_char> z) : command("/noteOn", x, y, z) {} // TODO: a bit much work?
	};*/




	std::string make_start_command() const;
	cmd_vectors make_note_commands(const std::multimap<daw::note_geom_t, daw::note_t>& mm) const
	{
		// channel, note, velocity

		// note offset <-> command
		std::map<int, command_base*> cmd_of;

		cmd_vectors res;
		for(const std::pair<daw::note_geom_t, daw::note_t>& pr : mm)
		{
		//	res.emplace_back(new command<int_f, int_f, int_v>("/noteOn", 0, pr.first.offs, pr.second.velocity()), pr.first.start); // TODO: valgrind!

			auto itr1 = cmd_of.find(pr.first.offs);
			if(itr1 == cmd_of.end())
			{
				// TODO: valgrind
				command_base* cmd = new command<oint<>, oint<>, oint<>>("/noteOn", 0, pr.first.offs, pr.second.velocity());
				cmd_of.emplace_hint(itr1, pr.first.offs, cmd);

				// TODO: note_off

				res.emplace(cmd, std::set<float>{pr.first.start});
				std::cerr << "New note command: " << cmd << std::endl;

				cmd = new command<oint<>, oint<>>("/noteOff", 0, pr.first.offs);
				res.emplace(cmd, std::set<float>{pr.first.start + pr.second.length()});

				std::cerr << "Map content now: " << std::endl;
				for(const auto& p : res)
				{
					std::cerr << p.first->buffer() << std::endl;
				}
			}
			else
			{
				res.find(itr1->second)->second.insert(pr.first.start);
				std::cerr << "Found note command." << std::endl;
			}



		}
		std::cerr << "Added " << res.size() << " note commands to track." << std::endl;
		return res;
	}

	port_t get_port(pid_t pid, int ) const;
	zynaddsubfx_t(const char* name);
	virtual ~zynaddsubfx_t() {} //!< in case someone derives this class

	// TODO: string as template param?
/*	class note_on : public command<p_char, p_char, p_char>
	{
		static const char* path() { return "/noteOn"; }
		template<class ...Args>
		note_on(const char* _path, Args ...args) : command(_path, ...args) {}
	};*/

	//class note_off : public command<p_char, p_char> { static const char* path() { return "/noteOff"; } };
	template<class Port1 = no_port<int>, class Port2 = no_port<int>, class Port3 = no_port<int>>
	class note_on : public command<oint<Port1>, oint<Port2>, oint<Port3>>
	{
		using base = command<oint<Port1>, oint<Port2>, oint<Port3>>;
	public:
		static const char* path() { return "/noteOn"; } // TODO: noteOn string is code duplicate
		note_on(oint<Port1> chan, oint<Port2> note, oint<Port3> velocity)
			: base("/noteOn", chan, note, velocity)
		{
		}
	};
};


/*template<class Cmd, class ...Args>
make_cmd()*/


}

#endif // INSTRUMENT_H
