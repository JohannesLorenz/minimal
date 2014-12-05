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

#ifndef PROJECT_H
#define PROJECT_H

#include <vector>
#include <map>
#include <algorithm>

#include "instrument.h"
#include "utils.h"
#include "tuple_helpers.h"
#include "lfo.h"

namespace mini
{

using octave_t = std::size_t;
using key_note_t = octave_t; // TODO: class!

/*enum key_note_t // TODO: enum_class?
{ // TODO: operator++?
	c,
	d,
	e,
	f,
	g,
	a,
	h,
};*/

class key_t
{
	int key;
	static const unsigned octave_len = 13;
public:
	key_t(const octave_t& o, const key_note_t& k)
	: key(o*13 + k)
	{
	}
	operator int() const { return key; }
};

class timeline_t
{

};

class line_t
{
	std::vector<int> pos;
public:
	template<class ...PosClass>
	line_t(PosClass&& ...positions) :
		pos { positions... }
	{
	}
};

// concept
template<class ScaleType>
class segment_t
{
	using scale_t = ScaleType;
	scale_t _begin, _end;
	//segment_t parent;
	std::vector<segment_t> lines;
	bool constonly = false;
public:

};

namespace daw_visit {

	using namespace daw;

	inline std::pair<note_geom_t, note_t> visit(note_geom_t offset, const note_t& n)
	{
		std::cerr << "Adding note, offset: " << offset.offs << ", start: " << offset.start << std::endl;
		return std::make_pair(offset, n);
	}

	inline std::multimap<note_geom_t, note_t> visit(note_geom_t offset, const notes_t& ns)
	{
		// TODO: can we not use vectors? -> linear time
		note_geom_t abs_offs = ns.geom + offset;
		std::multimap<note_geom_t, note_t> res;
		for(const notes_t::pair_t<note_t>& p : ns.get<note_t>())
		{
		//	std::pair<note_geom_t, note_t> p = visit(n);
			res.insert(visit(abs_offs + p.first, *p.second));
		}
		for(const notes_t::pair_t<notes_t>& ns2 : ns.get<notes_t>())
		{
			std::multimap<note_geom_t, note_t> res2 = visit(abs_offs, *ns2.second);
			std::move(res2.begin(), res2.end(), std::inserter(res, res.begin()));
		}
		std::cerr << "Added " << res.size() << " notes." << std::endl;
		return res;
	}

	inline cmd_vectors visit(const track_t& t)
	{
		cmd_vectors result;
		std::cerr << "sz: " << t.get<notes_t>().size() << std::endl;

		for(const auto& pr : t.get<notes_t>())
		{
			std::multimap<note_geom_t, note_t> mm = visit(t.geom + pr.first, *pr.second);
			cmd_vectors note_commands =
				t.instrument()->make_note_commands(mm);
			for(auto& pr : note_commands) {
				pr.second.insert(std::numeric_limits<float>::max()); // sentinel
			}
			std::cerr << "Added " << note_commands.size() << " note commands to track." << std::endl;

			for(const auto& pr : note_commands)
			{
				auto itr = result.find(pr.first);
				if(itr == result.end())
				 result.emplace(pr.first, std::move(pr.second));
				else
				 std::move(pr.second.begin(), pr.second.end(),
					std::inserter(itr->second, itr->second.end()));
			}

			//std::move(note_commands.begin(), note_commands.end(),
			//	std::inserter(result, result.end()));
		}
		//for(t.get<auto_t>) // automation tracks...

		for(const auto& pr : t.get<command_base>())
		{
			result.emplace(pr.second, std::set<float>{});
		}

		std::cerr << "Added track with " << result.size() << " note commands." << std::endl;
		return result;
	}

	// rtosc port (via instrument), commands, times
	using global_map = std::map<const instrument_t*, cmd_vectors>;

	inline global_map visit(global_t& g)
	{
		global_map res;
		std::cerr << "sz0: " << g.get<track_t>().size() << std::endl;

		for(const auto& pr : g.get<track_t>())
		{
			const track_t& t = *pr.second;
			const instrument_t* ins = t.instrument(); // TODO: should t.instrument ret ref?

			//cmd_vectors v = std::make_pair(&t, visit(t));
			cmd_vectors _v = visit(t);

			using cmd_pair = std::pair<const command_base*, std::set<float>>;

			for(const cmd_pair pr : _v)
			{
				auto ins_itr = res.find(ins);
				if(ins_itr == res.end())
				{
					// simply insert it
					cmd_vectors new_map { std::make_pair(pr.first, /*std::move*/(pr.second)) };
					res.emplace_hint(ins_itr, ins, new_map);
				}
				else
				{
					const auto vt = pr;
					//for(const cmd_vectors::value_type vt : v)
					{
						const command_base& cmd = *vt.first;
						const std::set<float>& vals = vt.second;

						// if instrument *and* command are equal,
						// add the set to the known command

						auto ins_cmd = ins_itr->second.find(&cmd);
						if(ins_cmd == ins_itr->second.end())
						{
							ins_itr->second.emplace(&cmd, vals);
						}
						else
						{
							std::set<float>& vals_existing = ins_cmd->second;
							vals_existing.insert(vals.begin(), vals.end());
						}

					}
				}
			}

			// TODO: geometry of t is unused here?
		}
		/*for(const global_t& g : g.get<global_t>())
		{
			res.insert(std::make_pair(t.id(), visit(t)));
		}*/ // TODO


		for(const auto& pr : res)
		{
			std::cerr << "Summary: contents of instrument " << pr.first->name() << ": " << std::endl;
			for(const auto& pr2 : pr.second)
			{
				std::cerr << " - track: " << pr2.first->buffer() << std::endl;
				for(const float& f : pr2.second)
				 std::cerr << "  * note at: " << f << std::endl;
			}
		}

		return res;
	}

}


//template<class Self, class Child>
//Child cseg(Self& );




/*

class track_t
{
	const instrument_t::id_t instr_id;
	std::map<key_t, line_t> lines;
public:
	void add_line(octave_t octave, key_note_t key, line_t&& line)
	{
		add_line(key_t(octave, key), std::move(line));
	}
	void add_line(key_t key, line_t&& line)
	{
		lines[key] = line;
	}
	// TODO: disallow ctor for user
	track_t(const instrument_t::id_t& instr_id);
};*/

//! Consists of all data which is needed to serialize a project.
//! This class will never be instantiated in an so file
class project_t : non_copyable_t
{
	bool valid = true;
	float _tempo = 140.0;
	std::string _title;
	std::vector<std::unique_ptr<instrument_t>> _instruments;
	std::vector<effect*> _effects;
//	std::vector<track_t> _tracks;
	daw::global_t _global;
public:
	project_t();
	~project_t();
	project_t(project_t&& other):
		_instruments(std::move(other._instruments))/*,
		_tracks(std::move(other._tracks))*/
	{
	}

	daw::global_t& global() { return _global; }
	std::vector<effect*>& effects() { return _effects; }

	const std::vector<std::unique_ptr<instrument_t>>& instruments() const {
		return _instruments; }
//	const std::vector<track_t>& tracks() const { return _tracks; }

	void set_tempo(float tempo) { _tempo = tempo; }
	float tempo() const { return _tempo; }
	//void set_tempo(unsigned short tempo) { _tempo = tempo; }
	void set_title(const char* title) { _title = title; }
	void set_title(const std::string& title) { _title = title; }
	const std::string& title() const { return _title; }

//	void add_track(const track_t& track) { _tracks.push_back(track); }
	// todo: can we avoid add instrument, by using add track?
/*	template<class T>
	void add_instrument(const T&& ins) {
		_instruments.push_back(std::unique_ptr<T>(std::move(ins)));
	}*/
	template<class T, class ...Args>
	T& emplace(Args ...args) {
		_instruments.emplace_back(new T(args...));
		return static_cast<T&>(*_instruments.back()); // TODO: correct cast?
	}
//	track_t& add_track(const instrument_t& ins);

	void invalidate() { valid = false; }
};

}

#endif // PROJECT_H
