// TODO: license
#include <iostream>
#include <iomanip>

#include "audio.h"

namespace mini {

std::ostream& operator<< (std::ostream& stream,
		m_reader_t& r)
{
	float avg[2] = { 0.0f, 0.0f };
	auto rs = r.read_max(r.read_space());
	//for(std::size_t side = 0; side <= 1; ++side)
	for(std::size_t i = 0; i < rs.size(); ++i)
	{
		avg[0] += rs[i].at<0>();
		avg[1] += rs[i].at<1>();
	}
	stream << "Average: " << std::setprecision(2) << (avg[0]/=rs.size()) << ", " << (avg[1]/=rs.size()) << std::endl;

	return stream;
}

}

