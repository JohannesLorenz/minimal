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

#ifndef NODE_H
#define NODE_H

#include <map>

#include "simple.h"

namespace mini {

class instrument_t;

class nnode : public named_t
{
private:
	void register_as_child(nnode* child);
public: // TODO?
	// TODO: call this the other way in the future?
	virtual instrument_t* get_ins() {
		if(parent)
		 return parent->get_ins();
		else
		 throw "root class did not implement get_ins()";
	}

protected:
	std::map<nnode*, bool> children;
	nnode* parent = nullptr;
public:
	struct ctor
	{
		class nnode* parent;
		const char* name;
		ctor(nnode* parent, const char* name) :
			parent(parent),
			name(name) {}
	};

	nnode(const char* ext, nnode* parent);

	void print_parent_chain() const;
	void print_tree(unsigned initial_depth = 0) const;

	std::string full_path() const {
		return parent
			? parent->full_path() + "/" + name()
			: /*"/" +*/ name(); // TODO: why does this work??
	}

	template<class PortT> PortT& add_if_new(const std::string& ext)
	{
		// TODO: is it good to pass this here?
		return *new PortT(this, get_ins(), ext);
		//return static_cast<PortT&>(
		//	*used_ch.emplace(ext, new NodeT(ins, name(), ext)).
		//	first->second);
	}

	template<class PortT>
	PortT& spawn(const std::string& ext) {
		return add_if_new<PortT>(ext);
	}

	virtual void on_preinit() {}

	void preinit();
};

}

#endif // NODE_H
