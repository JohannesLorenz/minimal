#ifndef SCALES_H
#define SCALES_H

namespace scales {

namespace note_detail {

//! the "full" scale, with all black and white keys
enum chromatic
{
	c,
	cs,
	db = cs,
	d,
	ds,
	eb = ds,
	e,
	f,
	fs,
	gb = fs,
	g,
	gs,
	ab = gs,
	a,
	as,
	bb = as,
	b,
	octave_size
};

static_assert(octave_size == 12, "octave size computed wrong");

}

//! variable describing the "height" of a note, 0 = C_0
using key_t = char;

class note
{
	key_t key;
public:
	constexpr note(key_t key) : key(key) {}

	constexpr note operator^(key_t i) const {
		return note(key +
			i * scales::note_detail::chromatic::octave_size);
	}

	explicit operator key_t() const { return key; }
};

// TODO:
/*static_assert((scales::chromatic::A^1) ==
	12 + (int)scales::chromatic::A,
	"chromatic shifting failed");*/

constexpr const note c = note_detail::c;
constexpr const note cs = note_detail::cs;
constexpr const note db = note_detail::db;
constexpr const note d = note_detail::d;
constexpr const note ds = note_detail::ds;
constexpr const note eb = note_detail::eb;
constexpr const note e = note_detail::e;
constexpr const note f = note_detail::f;
constexpr const note fs = note_detail::fs;
constexpr const note gb = note_detail::gb;
constexpr const note g = note_detail::g;
constexpr const note gs = note_detail::gs;
constexpr const note ab = note_detail::ab;
constexpr const note a = note_detail::a;
constexpr const note as = note_detail::as;
constexpr const note bb = note_detail::bb;
constexpr const note b = note_detail::b;

}

#endif // SCALES_H
