// TODO: license

#ifndef AUDIO_PROTOCOL_H
#define AUDIO_PROTOCOL_H

#include "audio.h"
#include "protocol.h"

namespace mini {

template<>
class protocol_t<m_ringbuffer_t> : public protocol_tbase_t<m_ringbuffer_t> {
	using base = protocol_tbase_t<m_ringbuffer_t>;
public:
	using base::base;
/*	m_reader_t rd;
	// TODO: unused technique?
	protocol_t(bool on_change = true, bars_t each_seconds
		= numeric_limits<bars_t>::max()) :
		base(on_change, each_seconds)
	{
		rd.connect(&base::input.data);
	}

	bool _proceed() {
		std::clog << rd << std::endl;
		return true;
	}*/
};

}

#endif // AUDIO_PROTOCOL_H
