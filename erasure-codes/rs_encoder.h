
#include <cstdint>

namespace erasure
{
	struct encode_parameters
	{
		size_t n; // Total number of shards
		size_t k; // Number of data shards
		size_t data_size; // Size of each shard (in bytes)
	};
	
	void encode(
		const encode_parameters& params,
		const uint8_t* const* shards,
		uint8_t* const* parity);

	bool recover(
		const encode_parameters& params,
		uint8_t* const* shards,
		const bool* present);
}
