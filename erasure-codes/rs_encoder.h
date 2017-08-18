
#include <cstdint>

namespace erasure
{
	struct encode_parameters
	{
		uint8_t n; // Total number of shards
		uint8_t k; // Number of data shards
		size_t data_size; // Size of each shard (in bytes)
	};
	struct rs_encoder;

	enum encoder_flags
	{
		DEFAULT = 0
	};
	enum error_code
	{
		// The operation completed successfully
		SUCCESS = 0,
		RECOVER_FAILED,
		INVALID_ARGUMENTS,
		INTERNAL_ERROR
	};

	rs_encoder* create_encoder(
		const encode_parameters& params,
		encoder_flags flags = DEFAULT);
	void destroy_encoder(rs_encoder* encoder);

	error_code encode(
		rs_encoder* encoder,
		const uint8_t* const* shards,
		uint8_t* const* parity);
	/* Encodes a subset of the parity shards given
	   the data shards. Unused parity shard pointers
	   are not accessed and may be null.
	   
	   PARAMETERS:
	   encoder: 
	      An encoder instance.
	   shards:  
	      The data shards from which the parity
	      shards are calculated.
	   parity:
	      The parity shards.
	   should_encode:
	      A boolean array specifying which parity
	      shards are to be calculated.
	*/
	error_code encode_partial(
		rs_encoder* encoder,
		const uint8_t* const* shards,
		uint8_t* const* parity,
		const bool* should_encode);

	error_code recover_data(
		rs_encoder* encoder,
		uint8_t* const* shards,
		const bool* present);
	error_code recover(
		rs_encoder* encoder,
		uint8_t* const* shards,
		const bool* present);
}
