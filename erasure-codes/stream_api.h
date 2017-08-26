#include <cstdint>

namespace erasure
{
	struct rs_encoder;
	struct recover_stream;
	struct encode_stream;

	enum error_code
	{
		// The operation completed successfully
		SUCCESS = 0,
		RECOVER_FAILED,
		INVALID_ARGUMENTS,
		INTERNAL_ERROR
	};

	recover_stream* create_recover_stream(
		rs_encoder* encoder,
		const bool* present);
	encode_stream* create_encode_stream(
		rs_encoder* encoder,
		const bool* should_encode);
	void destroy_stream(recover_stream* stream);
	void destroy_stream(encode_stream* stream);

	error_code stream_encode(
		encode_stream* stream,
		const uint8_t* const* shards,
		uint8_t* const* parity);
	error_code stream_recover_data(
		recover_stream* stream,
		uint8_t* const* shards);
}
