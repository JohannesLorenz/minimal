#ifndef SCALES_H
#define SCALES_H

namespace scales {

//! the "full" scale, with all black and white keys
enum chromatic
{
	C,
	Cs,
	Db = Cs,
	D,
	Ds,
	Eb = Ds,
	E,
	F,
	Fs,
	Gb = Fs,
	G,
	Gs,
	Ab = Gs,
	A,
	As,
	Bb = As,
	B,
	octave_size
};

static_assert(octave_size == 12, "octave size computed wrong");

}

#endif // SCALES_H
