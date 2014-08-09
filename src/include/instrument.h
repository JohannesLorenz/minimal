/*************************************************************************/
/* mmms - minimal multimedia studio                                      */
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

namespace mmms
{

//! note: if binary gets too large, we might need to not use templates...

class command_base
{
protected:
	std::string _path; // TODO: std string everywhere
public:
	const std::string& path() const { return _path; }
	command_base(const char* _path) : _path(_path) {}
//	virtual command_base* clone() const = 0; // TODO: generic clone class?
	virtual ~command_base() { std::cout << "destroying command_base" << std::endl; }
};

template<class Path, class ...Args>
class command : public command_base
{
	std::tuple<Args...> args;
public:
	command(Args... args) :
		command_base("???"),
		args(args...) {}
//	virtual command* clone() const { return new command(*this); }
	virtual ~command() {}
};

#include "utils.h" // TODO

class named_t
{
	const std::string _name;
public:
	const std::string& name() const { return _name; }
	named_t(const char* _name) : _name(_name) {}
};

class instrument_t : named_t, non_copyable_t
{
public:
	using id_t = std::size_t;
private:
	static std::size_t next_id;
	const std::size_t _id;
	std::vector<command_base*> commands; // TODO: unique?
public:
	using port_t = int;
	instrument_t(const char* name) :
		named_t(name),
		_id(next_id++)
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
};

namespace paths
{
	static constexpr const char noteOn[] = "noteOn";
}

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
	std::string make_start_command() const;
	port_t get_port(pid_t pid, int ) const;
	zynaddsubfx_t(const char* name) : instrument_t(name) {}
	virtual ~zynaddsubfx_t() {} //!< in case someone derives this class

	class p_char {
		char c;
	public:
		p_char(char c) : c(c) {}
		static char sign() { return 'c'; }
	};

	// TODO: string as template param?
	/*class note_on : public command<p_char, p_char, p_char>
	{
		static const char* path() { return "/noteOn"; }
		template<class ...Args>
		note_on(const char* _path, Args ...args) : command(_path, ...args) {}
	};
	class note_off : public command<p_char, p_char> { static const char* path() { return "/noteOff"; } };*/
	class note_on : public command<fixed_str<'n', 'o', 't', 'e', 'O', 'n'>, p_char, p_char, p_char> { using command::command;
	};
};

/*template<class Cmd, class ...Args>
make_cmd()*/


}

#endif // INSTRUMENT_H
