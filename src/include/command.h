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

#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <string>
#include <algorithm>
#include <tuple>
#include "osc_string.h"
#include "types.h"

namespace mini
{

class command_base
{
protected:
	std::string _path; // TODO: std string everywhere
public:

	using call_functor = void (*)(const char*, const char*, ...);

	const std::string& path() const { return _path; }
	command_base(const char* _path) :
		_path(_path) {}
	virtual std::string type_str() const = 0;
	virtual bool update() = 0;
	virtual const osc_string& complete_buffer() const = 0;
	virtual float get_next_time() const = 0;

//	virtual void execute(functor_base<>& ftor) const = 0;

//	virtual command_base* clone() const = 0; // TODO: generic clone class?
	virtual const osc_string& buffer() const = 0;
	virtual ~command_base() = 0;

	virtual bool operator==(const command_base& other) const = 0;
	virtual bool operator<(const command_base& other) const = 0;
};


namespace detail
{

	template<class T>
	constexpr std::size_t length_of() {
//		return util::dont_instantiate_me_func<std::size_t>();
		return pad_size<T>::value();
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

namespace command_detail
{
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
		//	std::cerr << "app single" << std::endl;
			std::vector<char> osc_str = to_osc_string(elem); // TODO: this is too slow
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
			s.resize(s.size() + pad_size<T>::value());
			std::fill(s.end() - pad_size<T>::value(), s.end(), 0); // debug only
		}
	};


	template<bool Ok, std::size_t N, std::size_t I, class ...Args2>
	struct _append
	{
		static void exec(std::vector<char>& s, const std::tuple<Args2...>& tp)
		{
			using tp_at = typename std::tuple_element<I, std::tuple<Args2...>>::type;
			constexpr bool _is_const = is_const<tp_at>::value();
			constexpr bool _size_fix = size_fix<tp_at>::value();

			// case 1: it's const -> fill it in
			_append_single<_is_const>::exec(s, std::get<I>(tp));
			// case 2: not const, but fixed size -> buffer it
			constexpr bool case_2 = (!_is_const) && _size_fix;
			_fill_single<case_2>::template exec<tp_at>(s);
			// continue if const or fix
			_append<_size_fix || _is_const, N, I+1, Args2...>::exec(s, tp);
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


	// TODO: generalize all this: template<class ...Args, bool Active> action { exec(); }... then inherit


	template<bool DoComplete, bool EndReached> // DoComplete = true, EndReached = true
	struct _complete_single
	{
		template<class T>
		static void exec(std::vector<char>& v, std::vector<char>::iterator* itr, const T& elem)
		{
		/*	std::cerr << "app single" << std::endl;
			std::vector<char> osc_str = elem.to_osc_string(); // TODO: this is too slow
			std::copy(osc_str.begin(), osc_str.end(), std::back_inserter(s)); // TODO: move?*/

			_append_single<true>::exec(v, elem);
			*itr = v.end(); // obviously...


		//	v.resize();
		}
	};

	template<>
	struct _complete_single<true, false> // DoComplete = true, EndReached = false
	{
		template<class T>
		static void exec(std::vector<char>& v, std::vector<char>::iterator* itr, const T& elem)
		{
			std::cerr << "app single" << std::endl;
			std::vector<char> osc_str = to_osc_string(elem); // TODO: this is too slow
			// note the difference: no inserter here
			std::cerr << "test before: " << osc_string(v) << std::endl;
			std::copy(osc_str.begin(), osc_str.end(), *itr); // TODO: move?
			std::cerr << "test now: " << osc_string(v) << std::endl;

			*itr += osc_str.size();
		}
	};

	template<bool EndReached> // DoComplete = false => do nothing
	struct _complete_single<false, EndReached>
	{
		template<class T>
		static void exec(std::vector<char>& , std::vector<char>::iterator* , const T& )
		{
			// do not complete
		}
	};


	template<bool All, std::size_t N, std::size_t I, class ...Args2> // All = false
	struct _complete
	{
		static void exec(std::vector<char>& v, std::vector<char>::iterator* itr, const std::tuple<Args2...>& tp)
		{
			using tp_at = typename std::tuple_element<I, std::tuple<Args2...>>::type;

			// TODO: non fix size
			constexpr bool cond1 = !is_const<tp_at>::value() || All;
			_complete_single<cond1, All>::exec(v, itr, std::get<I>(tp));

			constexpr bool next_all = All || !size_fix<tp_at>::value(); // first non fix marks
			_complete<next_all, N, I+1, Args2...>::exec(v, itr, tp);

		/*	// case 1: it's const -> fill it in
			_append_single<tp_at::is_const()>::exec(s, std::get<I>(tp));
			// case 2: not const, but fixed size -> buffer it
			_fill_single<tp_at::size_fix()>::template exec<tp_at>(s);
			// continue if const or fix
			_append<tp_at::size_fix() || tp_at::is_const(), N, I+1, Args2...>::exec(s, tp);*/
		}
	};

	template<bool All, std::size_t N, class ...Args2> // All = false
	struct _complete<All, N, N, Args2...>
	{
		static void exec(std::vector<char>& , std::vector<char>::iterator* , const std::tuple<Args2...>& )
		{
			// end reached
		}
	};

	template<std::size_t N, std::size_t I>
	struct _update
	{
		template<class ...Args2>
		static bool exec(std::tuple<Args2...>& tpl)
		{
			bool this_up = variable_detail::update(std::get<I>(tpl));
			bool last_up = _update<N, I+1>::template exec<Args2...>(tpl);
			return this_up || last_up;
		}
	};

	template<std::size_t N>
	struct _update<N, N>
	{
		template<class ...Args2>
		static bool exec(const std::tuple<Args2...>& )
		{
			return false; // no value updated at start
			// end reached
		}
	};

	template<std::size_t N, std::size_t I>
	struct _next_time
	{
		template<class ...Args2>
		static float exec(const std::tuple<Args2...>& tpl)
		{
			return std::min(
					variable_detail::get_next_time(std::get<I>(tpl)),
					_next_time<N, I+1>::template exec<Args2...>(tpl)
				);
		}
	};

	template<std::size_t N>
	struct _next_time<N, N>
	{
		template<class ...Args2>
		static float exec(const std::tuple<Args2...>& )
		{
			return std::numeric_limits<float>::max();
			// end reached
		}
	};

}

// TODO: unused class
template<class ...Args>
class _command : public command_base
{
	using self = _command<Args...>;
protected:
	std::tuple<Args...> args;

	constexpr std::size_t est_length() const {
		return pad<4>(path().length() + 1) // path + \0
			+ pad<4>(sizeof...(Args) + 2)// ,<types>\0
			+ detail::est_length_args<Args...>();
	}
public:
	_command(const char* _path, Args... args) :
		command_base(_path),
		args(args...) {

		// std::cerr << "est. length: " << est_length() << std::endl;
	}

	virtual ~_command();

	std::string type_str() const {
		std::string res { ',' , sign<Args>()... };
		return res; // TODO: in one line! constexpr?
	}
};

template<class ...Args>
_command<Args...>::~_command() {}

template<class T>
struct _vari
{
	using type = T;
};
/*
namespace pick_vari
{

	namespace detail
	{
		template<class ...Args> struct nothing {};
		template<class, class> struct lr {};

		template<class First1, class ...Args1, class ...Args2> struct lr<nothing<First1, Args1...>, nothing<Args2...>> {
			using tuple_t = typename lr<nothing<Args1...>, nothing<Args2...>>::tuple_t;
		};

		template<class T, class ...Args1, class ...Args2> struct lr<nothing<_vari<T>, Args1...>, nothing<Args2...>> {
			using tuple_t = typename lr<nothing<Args1...>, nothing<Args2..., T>>::tuple_t;
		};

		template<class ...Args2> struct lr<nothing<>, nothing<Args2...>> {
			using tuple_t = std::tuple<Args2...>;
		};
	}

	template<class ...Args> struct seq {
		using tuple_t = typename detail::lr<detail::nothing<Args...>, detail::nothing<>>::tuple_t;
	};

}*/

template<class T>
struct arg_if_vari
{
	using type = T;
};

template<class T>
struct arg_if_vari<_vari<T>>
{
	using type = T;
};


template<class ...Args>
class testcommand : public _command<typename arg_if_vari<Args>::type...>
{
	using base_t = _command<typename arg_if_vari<Args>::type...>;
public:
	mutable osc_string _buffer;

private:



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

	osc_string prefill_buffer() const
	{
		std::vector<char> res(base_t::_path.begin(), base_t::_path.end());
		do {
			res.push_back('\0');
		} while(res.size() % 4);
		std::string t_s = base_t::type_str();
		std::copy(t_s.begin(), t_s.end(), std::back_inserter(res));
		do {
			res.push_back('\0');
		} while(res.size() % 4);
		command_detail::_append<true, sizeof...(Args), 0, Args...>::exec(res, base_t::args);
		return res;
	}


public:
	testcommand(const char* _path, Args... args) :
		base_t(_path, args...),
		_buffer(prefill_buffer())
		{
		}

	bool update()
	{
		bool changes = command_detail::_update<sizeof...(Args), 0>::template exec<Args...>(base_t::args);
		if(changes)
		 complete_buffer();
		return changes;
	}

	float get_next_time() const {
		//float result = std::numeric_limits<float>::max();
		return command_detail::_next_time<sizeof...(Args), 0>::template exec<Args...>(base_t::args);
	}

	const osc_string& complete_buffer() const
	{
		auto itr = _buffer.get_itr_first_arg();
		command_detail::_complete<false, sizeof...(Args), 0, Args...>::exec(_buffer.data(), &itr, base_t::args);
		return buffer();
	}
	const osc_string& buffer() const { return _buffer; }

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
	virtual ~testcommand(); // TODO: = 0 ?
};

// TODO: cpp file?
template<class ...Args>
testcommand<Args...>::~testcommand() {}

template<typename T>
class is_port
{
	typedef char false_type[1];
	typedef char true_type[2];

	template<typename U>
	static true_type& tester(typename U::data_type*);

	template<typename>
	static false_type& tester(...);

public:
	static const bool value = sizeof(tester<T>(0)) == sizeof(true_type);
};

template<class T, bool Is>
struct _data_type_if_port
{
	using type = T;
};

template<class T>
struct _data_type_if_port<T, true>
{
	using type = typename T::data_type;
};

template<class T>
using data_type_if_port = _data_type_if_port<T, is_port<T>::value>;


struct empty_port {};

template<class P, bool>
struct _make_port {
	using type = empty_port;
};

template<class P>
struct _make_port<P, true> {
	using type = P;
};

template<class P>
using make_port = _make_port<P, is_port<P>::value>;

template<class ...Args>
class command : public testcommand<typename data_type_if_port<Args>::type...>
{
	using base = testcommand<typename data_type_if_port<Args>::type...>;
	using base::base;
public:
	std::tuple<make_port<Args>...> in_ports;

	bool update()
	{
		//fetch_ports(); // TODO!!
		return base::update();
	}

	~command();
};

template<class ...Args>
command<Args...>::~command() {}

} // namespace mini

#endif // COMMAND_H
