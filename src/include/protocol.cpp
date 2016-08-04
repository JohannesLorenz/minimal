#include "protocol.h"

namespace mini {

protocol_base_t::protocol_base_t(bool on_change, bars_t each_seconds) :
	on_change(on_change),
	interval(as_samples_floor(each_seconds, info.samples_per_bar)),
	last_call(0)
{
	set_next_time(interval);
}

}

