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

#ifndef IO_PORTS_H
#define IO_PORTS_H

#include <array>

#include "sample.h"
#include "types.h"
#include "utils.h"

#include "port_fwd.h"

namespace mini
{

class effect_t;
class in_port_base;
class out_port_base;

void add_out_port(effect_t& e, out_port_base* opb);
void add_in_port(effect_t& e, in_port_base* opb);

class port_base : public is_variable
{


	port_base(const port_base& ) = delete;
public:
	port_base() = default;
	port_base(port_base&& ) noexcept = default;
};

template<class T, class SourceT, bool IsDep>
class in_port_templ_base;
template<class T>
class out_port_templ_base;

class out_port_base : public port_base
{
	effect_t* e;
protected:
	bool start = true;
	std::vector<in_port_base*> _readers;

	void _notify_set(sample_no_t now)
	{
		change_stamp = now;
		start = false;
	}
public:
	sample_no_t change_stamp = -1.0f;
	out_port_base(effect_t& ef) :
		e(&ef)
	{
	}

	const std::vector<in_port_base*>& readers() const { return _readers; }
	std::vector<in_port_base*>& readers() { return _readers; }
	// FEATURE: std::forward
	const effect_t* effect() const { return e; }

//	sample_no_t next_time = std::numeric_limits<sample_no_t>::max();

	template<class T1, class T2Source, class T2, bool IsDep>
	friend void internal_connect(in_port_templ_base<T1, T2Source, IsDep>& ipt, out_port_templ_base<T2>& opt);
};

template<class T, class SourceT, bool IsDep>
class in_port_t;


namespace detail {
	template<class T>
	struct remove_pointer {
		using type = T;
	};
	template<class T>
	struct remove_pointer<T*> {
		using type = T;
	};
	template<class T>
	T& deref_if_ptr(T* x) {
		return *x;
	}
	template<class T>
	T& deref_if_ptr(T& x) {
		return x;
	}
}

template<class T>
class out_port_templ_base : public out_port_base
{
	//operator const T&() { return data; }
public:
	//void connect(const in_port_t& ip) { target = &ip; }

	using type = T;
	using data_type = T;

	out_port_templ_base(effect_t& e) : // FEATURE : forward?
		out_port_base(e)
	{
		add_out_port(e, this);
	}

//	const T& get() const { return data; }
#if 0
	virtual void set(const T& new_val, sample_no_t now);
	void set(const T& new_val, sample_no_t now)
	{ /*return base::ref*/
		if(start || data != new_val)
		{
			data = new_val;
			change_stamp = now;
			start = false;
		}
	//	changed = true;
	}
#endif
	virtual const T& value() const = 0;
	virtual T& value() = 0;
};

template<class T>
class out_port_t : public out_port_templ_base<T>
{
	T data;
protected:
	//! identifies us as a base for children
	using base = out_port_t<T>;

public:
	template <class ...Args>
	out_port_t(effect_t& e, Args... args) : // FEATURE : forward?
		out_port_templ_base<T>(e),
		data(args...)
	{
	}

	virtual const T& value() const { return data; }
	virtual T& value() { return data; }

	void set(const T& new_val, sample_no_t now)
	{ /*return base::ref*/
		if(out_port_templ_base<T>::start || data != new_val)
		{
			data = new_val;
			out_port_templ_base<T>::_notify_set(now);
		}
	//	changed = true;
	}
};

template<class T>
class out_port_ref_t : public out_port_templ_base<T>
{
	T* data;
protected:
	//! identifies us as a base for children
	using base = out_port_ref_t<T>;
public:
	out_port_ref_t(effect_t& e, T& ref) : // FEATURE : forward?
		out_port_templ_base<T>(e),
		data(&ref)
	{
	}

	out_port_ref_t(effect_t& e) : // FEATURE: forward?
		out_port_templ_base<T>(e),
		data(nullptr)
	{
	}

	virtual const T& value() const { return *data; }
	virtual T& value() { return *data; }

	const T*& ref() const { return data; }
	T*& ref() { return data; }

	/*void notify_set(sample_no_t now)
	{
		change_stamp = now;
		start = false;
	}*/
	void notify_set(sample_no_t now) {
		out_port_templ_base<T>::_notify_set(now);
	}
};

template<class TIn, class TOut>
class connection_base
{
	TIn& in_port;
	TOut& out_port;

	connection_base(TIn& in_port, TOut& out_port) :
		in_port(in_port),
		out_port(out_port)
	{}
};


// TODO: abstract port base
class in_port_base : public util::non_copyable_t, public port_base
{
	effect_t* e;
	bool _is_trigger = false;
//	bool _is_precomputable;
protected:
	sample_no_t change_stamp = -1;
	std::size_t id;

	//bool unread_changes = false; initally send values ... is this good? 
protected:
/*
	friends
*/
	template<class T1, class T2Source, class T2, bool IsDep>
	friend void internal_connect(in_port_templ_base<T1, T2Source, IsDep>& ipt, out_port_templ_base<T2>& opt);

//	template<class T1, class T2, bool IsDep>
//	friend void operator<<(in_port_t<const T1*, T2, IsDep>& ipt, out_port_templ_base<T2>& opt);
public:


	std::size_t get_id() const { return id; }
	void init_id(std::size_t new_id) { id = new_id; }
	const effect_t* get_effect() const { return e; }

	in_port_base(in_port_base&& ) noexcept = default;

	in_port_base(effect_t& ef) :
		e(&ef)
	{
		add_in_port(ef, this);
		//no_rt::mlog << "NEW IN PORT AT: " << this << std::endl;
	}
/*
	in_port_base(effect_t& ef, const out_port_base& source) :
		e(&ef),
		source(&source)
	{
		add_in_port(ef, this);
		no_rt::mlog << "NEW IN PORT AT: " << this << std::endl;
	}*/

	template<class P>
	in_port_base(port_ctor<P> pc) : in_port_base(*pc.effect()) {}

/*	sample_no_t get_outs_next_time() const {
		return source->next_time;
	}*/

	//! the function to fetch data from an outport
	virtual bool update() = 0;

	bool is_trigger() const { return _is_trigger; }
	void set_trigger(bool is_trigger = true) { _is_trigger = is_trigger; } // TODO: ctor?

/*	bool is_precomputable() const { return _is_precomputable; }
	void set_precomputable(bool is_precomputable = true) { _is_precomputable = is_precomputable; }
*/
	virtual void on_read(sample_no_t time) = 0;

	virtual void instantiate_port() = 0;
	
	virtual ~in_port_base() {}
};

template<class T, class SourceT, bool IsDep = true>
class in_port_templ_base : public in_port_base
{
protected:
	out_port_templ_base<SourceT>* source = nullptr;

	T data;
	using data_type = T;
public:
	using in_port_base::in_port_base;
	template <class ...Args>
	in_port_templ_base(effect_t& e, Args... args) : // FEATURE: forward?
		in_port_base(e),
		data(args...)
	{
	}

	bool is_dependency() const { return IsDep; }

	out_port_templ_base<SourceT>*& get_source() { return source; }
	const out_port_templ_base<SourceT>*& get_source() const { return source; }

	const T& get() const { return data; }
	T& get() { return data; }

	typename detail::remove_pointer<T>::type& value() { return detail::deref_if_ptr(data); }
	const typename detail::remove_pointer<T>::type& value() const { return detail::deref_if_ptr(data); }
	
	using type = T;

	bool up_to_date() const { return source->change_stamp <= change_stamp; }
	bool needs_update() const { return source->change_stamp > change_stamp; }

protected:
	void update_stamp() {
		//if(unread_changes)
		// throw "omitting a value now!";
		//unread_changes = true;
		// TODO: check for unread changes via stamp?
		//       e.g. by storing the two last stamps...
		change_stamp = source->change_stamp;
	}

};

template<class T1, class T2>
void m_assign(T1*& ptr, T2& ref)
{
	ptr = &ref;
}

template<class T1, class T2>
void m_assign(T1& ref1, T2& ref2)
{
	ref1 = ref2;
}

template<class T, class SourceT, bool IsDep>
class in_port_noassign_t : public in_port_templ_base<T, SourceT, IsDep>
{
protected:
	//! identifies us as a base for children
	using base = in_port_t<T, SourceT, IsDep>;
	using templ_base = in_port_templ_base<T, SourceT, IsDep>;
public:
	using in_port_templ_base<T, SourceT, IsDep>::in_port_templ_base;
protected:
	bool set()
	{
		templ_base::update_stamp();
		return true;
	}

public:
	void instantiate_port() override { // TODO: private virtual funcs
		m_assign(templ_base::data, templ_base::source->value());
	}

	bool update() override {
		bool outdated = templ_base::change_stamp < templ_base::source->change_stamp;
//		if(outdated) std::cerr << templ_base::change_stamp << " yyy " <<  templ_base::source->change_stamp << std::endl;
		return (outdated) && set();
	}
};

template<class T, class SourceT = T, bool IsDep = true>
class in_port_t : public in_port_templ_base<T, SourceT, IsDep>
{
protected:
	//! identifies us as a base for children
	using base = in_port_t<T, SourceT, IsDep>;
	using templ_base = in_port_templ_base<T, SourceT, IsDep>;

public:
	using in_port_templ_base<T, SourceT, IsDep>::in_port_templ_base;

protected:
	bool set(const SourceT& new_value)
	{
		templ_base::update_stamp();
		templ_base::data = new_value; // conversion from SourceT to T
		return true;
	}

public:
	void instantiate_port() override {
		templ_base::data = templ_base::source->value();
	}

	bool update() override {
		bool outdated = templ_base::change_stamp < templ_base::source->change_stamp;
	//	if(outdated) std::cerr << templ_base::change_stamp << " <--outdated-- "
	//		<<  templ_base::source->change_stamp << std::endl;
		return (outdated) && set(templ_base::source->value());
	}

};

template<class T, class SourceT, bool IsDep>
struct in_port_t<T*, SourceT, IsDep> : public in_port_noassign_t<T*, SourceT, IsDep>
{
	using templ_base = in_port_noassign_t<T*, SourceT, IsDep>;
public:
	using templ_base::templ_base;
};

/*class m_reader_t;

template<bool IsDep>
struct in_port_t<m_reader_t, IsDep> : public in_port_noassign_t<m_reader_t, IsDep>
{
public:
	using in_port_noassign_t<m_reader_t, IsDep>::in_port_noassign_t;
};*/


template<class T1, class T2Source, class T2, bool IsDep>
void internal_connect(in_port_templ_base<T1, T2Source, IsDep>& ipt, out_port_templ_base<T2>& opt)
{
	if(ipt.get_source() != nullptr)
	 throw "double connect to in port";
	ipt.get_source() = &opt;
	opt._readers.push_back(&ipt);
	if(ipt.is_dependency()) // TODO: via template matching
	 opt.e->deps.push_back(ipt.e);
	else
	 opt.e->readers.push_back(ipt.e);
	ipt.e->writers.push_back(opt.e);
}


//! copy-value based connection
template<class T1, class T2, bool IsDep>
void operator<<(in_port_t<T1, T2, IsDep>& ipt, out_port_templ_base<T2>& opt)
{
	internal_connect(ipt, opt);
}

//! pointer based connection
template<class T, class T2, bool IsDep>
void operator<<(in_port_t<const T*, T2, IsDep>& ipt, out_port_templ_base<T2>& opt)
{
	internal_connect(ipt, opt);
	ipt.data = &opt.value();
}

#if 0
//! pointer based connection
template<class T, class T2, bool IsDep>
void operator<<(in_port_t<const T*, T2, IsDep>& ipt, out_port_templ_base<T2>& opt)
{
	internal_connect(ipt, opt);
	ipt.data = &opt.data;
}

//! pointer based connection
template<class T, class T2, bool IsDep>
void operator<<(in_port_t<T, T2, IsDep>& ipt, out_port_templ_base<T2*>& opt)
{
	internal_connect(ipt, opt);
	ipt.data = *opt.data;
}
#endif


//! connection that does no assignments once running
template<class T, class T2, bool IsDep>
void operator<<(in_port_noassign_t<T, T2, IsDep>& ipt, out_port_templ_base<T2>& opt)
{
	internal_connect(ipt, opt);
	ipt.get() = opt.value();
}


template<class T, T Value>
struct _constant
{
	constexpr static T value = Value;
};

//! constant value connection // TODO: also for pointer?
template<class T, T V, bool IsDep>
void operator<<(in_port_t<T, T, IsDep>& ipt, const _constant<T, V>&) // FEATURE: forward
{
	ipt.data = V;
}

class self_port_base : public port_base
{
public:
	bool unread_changes = false; // initally send values - TODO
/*	sample_no_t get_outs_next_time() const {
		// who knows :-)) (bad validation of protocol...)
		return std::numeric_limits<sample_no_t>::max();
	}*/
};

template<class T, bool = false>
class self_port_templ : public self_port_base
{
protected:
	T data;
	sample_no_t next_time;

protected:
/*	bool set(const T& new_value) {
		if(!unread_changes)
		 throw "omitted a vlue";
		unread_changes = (data == new_value);
		if(unread_changes)
		{
			data = new_value;
		}
		return unread_changes;
	}*/

public:
	using data_type = T;

	const T& get() const { return data; }

	void set(const T& _data) { data = _data; }

	//! this is actually an invalidation of the protocol...
	constexpr bool update() const { return true; }

	//! this has no effect
	void set_trigger(bool = true) const { }

	using type = T;
};

//! specialize this
template<class T>
struct input_type_t
{
	using type = T;
};

}

#endif // IO_PORTS_H
