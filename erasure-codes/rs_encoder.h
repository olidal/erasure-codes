
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
		uint8_t** shards,
		uint8_t** parity);

	bool recover(
		const encode_parameters& params,
		uint8_t** shards,
		bool* present);
}
