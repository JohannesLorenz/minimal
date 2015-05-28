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

#ifndef COMMAND_H
#define COMMAND_H

#include <utility>
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
	std::string _path;
public:

//	using call_functor = void (*)(const char*, const char*, ...);

	const std::string& path() const { return _path; }
	command_base(const char* _path) :
		_path(_path) {}
//	virtual std::string type_str() const = 0;
//	virtual bool update() = 0;
	virtual const osc_string& complete_buffer() const = 0;

//	virtual void execute(functor_base<>& ftor) const = 0;

//	virtual command_base* clone() const = 0; // TODO: generic clone class?
	virtual const osc_string& buffer() const = 0;
	virtual ~command_base() = 0;

	virtual bool operator==(const command_base& other) const = 0;
	virtual bool operator<(const command_base& other) const = 0;
};

template<std::size_t I, class ...Args2>
using type_at = typename std::tuple_element<I, std::tuple<Args2...>>::type;


namespace command_detail
{
	//template<class T, T>
	struct do_nothing
	{
		template<class ...Args>
		static void exec(const Args&...) {}
	};

	template<std::size_t I, class ...Args>
	struct est_length_args
	{
		constexpr static std::size_t exec(const std::tuple<Args...>& t) {
			return pad_size(std::get<I>(t)) + est_length_args<I-1, Args...>(); }
	};

	template<class ...Args>
	struct est_length_args<0, Args...> {
		constexpr static std::size_t exec(const std::tuple<Args...>& ) { return 0; }
	};

	//! pushes back a given element to the vector if Printable is true
	// TODO: bad name: no allocation... append()?
	template<bool > // false, i.e. not printable
	struct push_back_single : do_nothing {};

	template<>
	struct push_back_single<true>
	{
		template<class T>
		static void exec(std::vector<char>& s, const T& elem)
		{
		//	io::mlog << "app single" << io::endl;
			std::vector<char> osc_str = to_osc_string(elem); // TODO: this is too slow
			std::copy(osc_str.begin(), osc_str.end(), std::back_inserter(s)); // TODO: move?
		}
	};

	template<bool > // false, i.e. no fix size
	struct pad_single : do_nothing {}; // todo: rename: pad/resize

	template<>
	struct pad_single<true>
	{
		template<class T>
		static void exec(std::vector<char>& s, const T& elem)
		{
			//no_rt::mlog << "resizing: "<< s.size() << " -> "<< s.size() + pad_size<T>::value() <<std::endl;
			s.resize(s.size() + pad_size(elem));
			std::fill(s.end() - pad_size(elem), s.end(), 0); // debug only
		}
	};

	//! prefills a vector with a tuple as far as possible,
	//! using push back or pad instructions
	template<bool /*do sth*/, std::size_t N, std::size_t I, class ...Args2>
	struct prefill
	{
		static void exec(std::vector<char>& s, const std::tuple<Args2...>& tp)
		{
			using tp_at = type_at<I, Args2...>;
			constexpr bool _is_const = is_const<tp_at>::value;
			constexpr bool _size_fix = size_fix<tp_at>::value;

			// case 1: it's const -> fill it in
			push_back_single<_is_const>::exec(s, std::get<I>(tp));
			// case 2: not const, but fixed size -> buffer it
			constexpr bool case_2 = (!_is_const) && _size_fix;
			pad_single<case_2>::exec(s, std::get<I>(tp));
			// continue if const or fix
			prefill<_size_fix || _is_const, N, I+1, Args2...>::exec(s, tp);
		}
	};


	template<std::size_t N, std::size_t I, class ...Args2>
	struct prefill<false, N, I, Args2...> : do_nothing {}; // nothing to execute

	template<bool Ok, std::size_t N, class ...Args2>
	struct prefill<Ok, N, N, Args2...> : do_nothing {}; // end reached

	// TODO: generalize all this: template<class ...Args, bool Active> action { exec(); }... then inherit

	//! single version of complete for if resize is needed
	template<bool DoComplete, bool NeedResize> // DoComplete = true, NeedResize = true
	struct complete_single
	{
		template<class T>
		static void exec(std::vector<char>& v, std::vector<char>::iterator* itr, const T& elem)
		{
		/*	io::mlog << "app single" << io::endl;
			std::vector<char> osc_str = elem.to_osc_string(); // TODO: this is too slow
			std::copy(osc_str.begin(), osc_str.end(), std::back_inserter(s)); // TODO: move?*/

			push_back_single<true>::exec(v, elem);
			*itr = v.end(); // obviously...

		//	v.resize();
		}
	};

	//! single version of complete for if still in reserved area
	template<>
	struct complete_single<true, false> // DoComplete = true, NeedResize = false
	{
		template<class T>
		static void exec(std::vector<char>& , std::vector<char>::iterator* itr, const T& elem)
		{
			std::vector<char> osc_str = to_osc_string(elem); // TODO: this is too slow

		//	io::mlog << "complete_single of "<< get_value<T>::exec(elem) << ": " << osc_str.size() << " bytes: " << io::endl;
			// note the difference: no inserter here
		//	io::mlog << "test before: " << osc_string(v) << io::endl;
			std::copy(osc_str.begin(), osc_str.end(), *itr); // TODO: move?
		//	io::mlog << "test now: " << osc_string(v) << io::endl;

			*itr += osc_str.size();
		}
	};

	template<bool NeedResize> // DoComplete = false => do nothing
	struct complete_single<false, NeedResize>
	{
		template<class T>
		static void exec(std::vector<char>& , std::vector<char>::iterator* itr, const T& elem)
		{
			*itr += pad_size(elem);
			// do not complete
		}
	};

	//! completes a pre-filled vector with a tuple
	template<bool All, std::size_t N, std::size_t I, class ...Args2> // All = false
	struct complete
	{
		static void exec(std::vector<char>& v, std::vector<char>::iterator* itr, const std::tuple<Args2...>& tp)
		{
			using tp_at = type_at<I, Args2...>;
			// TODO: non fix size
			constexpr bool cond1 = !is_const<tp_at>::value || All;
			complete_single<cond1, All>::exec(v, itr, std::get<I>(tp));

			constexpr bool next_all = All || !size_fix<tp_at>::value; // first non fix marks
			complete<next_all, N, I+1, Args2...>::exec(v, itr, tp);
		}
	};

	template<bool All, std::size_t N, class ...Args2> // All = false
	struct complete<All, N, N, Args2...> : do_nothing {}; // end reached

	template<bool FixedSign> // = true
	struct fill_enhanced_type_str_single
	{
		template<std::size_t I, class ...Args2>
		static void exec(std::string& s, const std::tuple<Args2...>& )
		{
			s.push_back(sign<typename std::tuple_element<I, std::tuple<Args2...>>::type>::value);
		}
	};

	template<>
	struct fill_enhanced_type_str_single<false>
	{
		template<std::size_t I, class ...Args2>
		static void exec(std::string& s, const std::tuple<Args2...>& tp)
		{
			s.push_back(sign_of(std::get<I>(tp)));
		}
	};


	template<std::size_t N, std::size_t I = 0>
	struct fill_enhanced_type_str
	{
		template<class ...Args2>
		static void exec(std::string& s, const std::tuple<Args2...>& tp)
		{
			using tp_at = type_at<I, Args2...>;
			constexpr bool fixed_sign = has_fixed_sign<tp_at>();
			fill_enhanced_type_str_single<fixed_sign>::template exec<I>(s, tp);
			fill_enhanced_type_str<N, I+1>::exec(s, tp);
		}
	};

	template<std::size_t N> // All = false
	struct fill_enhanced_type_str<N, N> : do_nothing {}; // end reached

	template<std::size_t I, bool FixedSign> // FixedSign = true
	struct update_enhanced_type_str_single : do_nothing {};
	// => already filled in

	template<std::size_t I>
	struct update_enhanced_type_str_single<I, false>
	{
		template<class ...Args2>
		static void exec(std::vector<char>::iterator& itr, const std::tuple<Args2...>& tp)
		{
			// TODO: is going backward cache efficient?
			*(itr + I) = sign_of(std::get<I>(tp));
		}
	};


	template<std::size_t N, std::size_t I = 0>
	struct update_enhanced_type_str
	{
		template<class ...Args2>
		static void exec(std::vector<char>::iterator& itr, const std::tuple<Args2...>& tp) {
			using tp_at = type_at<I, Args2...>;
			update_enhanced_type_str_single<I, has_fixed_sign<tp_at>()>::exec(itr, tp);
			update_enhanced_type_str<N, I+1>::exec(itr, tp);
		}
	};

	template<std::size_t N>
	struct update_enhanced_type_str<N, N> : do_nothing {};
}

// TODO: unused class
template<class ...Args>
class _command : public command_base
{
	using self = _command<Args...>;
protected:
public:// TODO: protected or document this
	std::tuple<Args...> args;
protected:
	constexpr std::size_t est_length() const {
		return pad<4>(path().length() + 1) // path + \0
			+ pad<4>(sizeof...(Args) + 2)// ,<types>\0
			+ command_detail::est_length_args<sizeof...(Args), Args...>::exec(args);
	}
public:
	template<class ...Args2>
	_command(const char* _path, Args2&&... args) :
		command_base(_path),
		args(std::forward<Args2>(args)...) {

		// no_rt::mlog << "est. length: " << est_length() << std::endl;
	}

	virtual ~_command();

/*	std::string type_str() const { // TODO: static variant?
		std::string res { ',' , sign<Args>::value... };
		return res; // TODO: in one line! constexpr?
	}*/
};

template<class ...Args>
_command<Args...>::~_command() {}

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

template<class ...Args>
class testcommand : public _command<Args...>
{
	using base_t = _command<Args...>;
public: // TODO?
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
		std::string t_s(","); //= base_t::type_str();
		command_detail::fill_enhanced_type_str<sizeof...(Args)>::exec(t_s, base_t::args);
		std::copy(t_s.begin(), t_s.end(), std::back_inserter(res));
		do {
			res.push_back('\0');
		} while(res.size() % 4);
		command_detail::prefill<true, sizeof...(Args), 0, Args...>::exec(res, base_t::args); // TODO: default args
		return res;
	}


public:
	//! constructs the command and prepares internal OSC string buffer as
	//! far as this is possible at construction
	template<class ...Args2>
	testcommand(const char* _path, Args2&&... args) :
		base_t(_path, std::forward<Args2>(args)...),
		_buffer(prefill_buffer())
		{
		}

	const osc_string& complete_buffer() const
	{
		auto itr = _buffer.get_itr_type_str();
		command_detail::update_enhanced_type_str<sizeof...(Args)>::exec(itr, base_t::args);

		itr = _buffer.get_itr_first_arg(); // TODO: reuse old itr
		command_detail::complete<false, sizeof...(Args), 0, Args...>::exec(_buffer.data(), &itr, base_t::args);
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

#if 0
namespace fetch_detail
{

template<class T1, class T2>
struct fetch_single
{
	// I owe you a cookie if you can make this without a struct
	// (and without overcomplication...)
	static void exec(T1& port, T2& storage) {
		storage.set(port.get());
//		io::mlog << "FETCHED: " << storage << " <- " << port.get() << io::endl;
	}
};

template<class T2>
struct fetch_single<empty_port, T2>
{
	// I owe you a cookie if you can make this without a struct
	// (and without overcomplication...)
	static void exec(const empty_port& , T2& )
	{
	}
};

template<int N, int I, class ...Args>
struct fetch_ports
{
	static void exec(std::tuple<make_port<Args>...>& ports, std::tuple<data_type_if_port<Args>...>& args)
	{
		using T1 = typename std::tuple_element<I, std::tuple<make_port<Args>...>>::type;
		using T2 = typename std::tuple_element<I, std::tuple<data_type_if_port<Args>...>>::type;
		fetch_single<T1, T2>::exec(std::get<I>(ports), std::get<I>(args));
		fetch_ports<N, I+1, Args...>::exec(ports, args);
	}
};


template<int N, class ...Args>
struct fetch_ports<N, N, Args...>
{
	static void exec(const std::tuple<make_port<Args>...>& ,
		const std::tuple<data_type_if_port<Args>...>& )
	{
	}
};

}
#endif

//! this is somehow std::forward...
template<class T, bool>
struct _rval_if_port
{
	using type = T&&;
};

template<class T>
struct _rval_if_port<T, false>
{
	using type = const T&;
};

template<class T>
using rval_if_port = typename _rval_if_port<T, is_port<T>::value>::type;

/*template<class T>
struct rval_if_possible<T&>
{
	using type = T&;
};*/

template<bool IsVar> // true
struct _for_all_variables_single
{
	template<class Var, class Ftor>
	static void exec(Var& v, Ftor& f) { f(v); }
};

template<>
struct _for_all_variables_single<false> : command_detail::do_nothing{};

// TODO: count *down* ??
template<std::size_t N, std::size_t I = 0>
class _for_all_variables {
	template<class Tpl>
	using type_at = typename std::tuple_element<I, Tpl>::type;
public:
	template<class Ftor, class Tpl>
	static void exec(Ftor& f, Tpl& tpl)
	{
		_for_all_variables_single<_is_variable<type_at<Tpl>>()>::exec(std::get<I>(tpl), f);
		_for_all_variables<N, I+1>::exec(f, tpl);
	}
};

template<std::size_t N>
class _for_all_variables<N, N> : public command_detail::do_nothing {};

template<class ...Args>
class command : /*public port_tuple<make_port<Args>...>,*/ public testcommand<Args...>
{
	using base = testcommand<Args...>;
//	using port_tuple_t = port_tuple<make_port<Args>...>;
//	using base::base;

	/*void fetch_ports() {
		fetch_detail::fetch_ports<sizeof...(Args), 0, Args...>::exec(port_tuple_t::in_ports, base::args);
	}*/

	template<std::size_t Idx>
	using type_at = typename std::tuple_element<Idx, std::tuple<Args...>>::type;

public:
	template<class ...Args2>
	command(const char* _path, Args2&&... args) :
	//	port_tuple_t(std::move(args)...),
		base(_path, std::forward<Args2>(args)...)
		//base(_path, get_from_port<Args, is_port<Args>::value>::exec(args)...) // triple expansion!
		//base(_path, from_port_or_val<Args, is_port<Args>::value>::exec(args)...)
	{
	}

	bool update()
	{
//		fetch_ports();
		return true;
	}

	template<std::size_t Idx>
	type_at<Idx>& port_at() {
		return std::get<Idx>(base::args);
	}

	~command();

	template<class Ftor>
	void for_all_variables(Ftor& f) {
		_for_all_variables<sizeof...(Args)>::exec(f, base::args);
	}
};

template<class ...Args>
command<Args...>::~command() {}

} // namespace mini

#endif // COMMAND_H
