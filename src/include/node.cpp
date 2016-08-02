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

#include "io.h"
#include "node.h"

namespace mini {

void nnode::register_as_child(nnode *child) {
	children.emplace(child, false);
}

nnode::nnode(const char *ext, nnode *parent) :
	named_t(ext), parent(parent) {
	if(parent)
	 parent->register_as_child(this);
}

nnode::nnode(const char *ext, nnode *parent, std::size_t idx) :
	nnode((ext + std::to_string(idx)).c_str(), parent) {}

void nnode::print_parent_chain() const {
	no_rt::mlog << name() << " -> " << std::endl;
	if(parent)
	 parent->print_parent_chain();
	else
	 no_rt::mlog << "(root)" << std::endl;
}

void nnode::print_tree(unsigned initial_depth) const
{
	unsigned next_depth = initial_depth + 1;
	const char* recent_child = "/"; // '/' is an invalid name
	unsigned recent_count = 0;

	for(const auto& pr : children)
	{
		if(pr.first->name() == recent_child)
		 ++recent_count;
		else
		 recent_count = 1;
		recent_child = pr.first->name().c_str();

		// first occurence?
		if(recent_count == 1)
		{
			for(unsigned i = 0; i < initial_depth; ++i)
			 no_rt::mlog << "  ";
			no_rt::mlog << "- " << pr.first->name() << std::endl;
		}

		// always append the tree
		pr.first->print_tree(next_depth);
	}
}

void nnode::preinit() {
	on_preinit();
	for(const auto& pr : children) {
		pr.first->on_preinit();
	}
}

}

