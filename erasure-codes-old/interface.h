#pragma once

#include <cstdint>

namespace erasure
{
	enum reconstruct_code
	{
		RECONSTRUCT_OK,
		RECONSTRUCT_ERR_TOO_FEW_SHARDS
	};

	static constexpr size_t n = 6;
	static constexpr size_t k = 4;
	static constexpr size_t num_bytes = 2;

	void encode(uint8_t** shards, uint8_t** parity);
	reconstruct_code reconstruct(uint8_t** shards, bool* present);
}
