// TODO: license
#include <iostream>

#include "audio.h"

namespace mini {

std::ostream& operator<< (std::ostream& stream,
		m_reader_t& r)
{
	float avg = 0.0f;
	auto rs = r.read_max(r.read_space());
	for(std::size_t side = 0; side <= 1; ++side)
	for(std::size_t i = 0; i < rs.size(); ++i)
	{
		avg += rs[i][side];
	}
	stream << "Average: " << avg << std::endl;

	return stream;
}

}

