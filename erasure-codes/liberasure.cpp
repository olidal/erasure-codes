#include "liberasure.h"

#include "rs_encoder.h"

extern "C" erasure_encoder erasure_create_encoder(
	const erasure_encoder_parameters* params,
	enum erasure_encoder_flags flags)
{
	try
	{
		if (params->data_size == 0
			|| params->n == 0
			|| params->k == 0)
			return nullptr;

		if (params->k >= params->n)
			return nullptr;

		return erasure::create_encoder(
				*params,
				(erasure::encoder_flags)flags);
	} 
	catch (...)
	{
		return nullptr;
	}
}
extern "C" void erasure_destroy_encoder(erasure_encoder encoder)
{
	try
	{
		erasure::destroy_encoder(encoder);
	}
	catch (...)
	{
		
	}
}

extern "C" enum erasure_error_code erasure_encode(
	erasure_encoder encoder,
	const uint8_t* const* shards,
	uint8_t* const* parity)
{
	if (!encoder || !shards || !parity)
		return ERASURE_INVALID_ARGUMENTS;

	try
	{
		return (erasure_error_code)erasure::encode(
			encoder,
			shards,
			parity);
	}
	catch (...)
	{
		return ERASURE_INTERNAL_ERROR;
	}
}

extern "C" enum erasure_error_code erasure_encode_partial(
	erasure_encoder encoder,
	const uint8_t* const* shards,
	uint8_t* const* parity,
	const erasure_bool* present)
{
	if (!encoder || !shards || !parity || !present)
		return ERASURE_INVALID_ARGUMENTS;

	try
	{
		return (erasure_error_code)erasure::encode_partial(
			encoder,
			shards,
			parity,
			(const bool*)present);
	}
	catch (...)
	{
		return ERASURE_INTERNAL_ERROR;
	}
}

extern "C" enum erasure_error_code erasure_recover_data(
	erasure_encoder encoder,
	uint8_t* const* shards,
	const erasure_bool* present)
{
	if (!encoder || !shards || !present)
		return ERASURE_INVALID_ARGUMENTS;

	try
	{
		return (erasure_error_code)erasure::recover_data(
			encoder,
			shards,
			(const bool*)present);
	}
	catch (...)
	{
		return ERASURE_INTERNAL_ERROR;
	}
}

extern "C" enum erasure_error_code erasure_recover(
	erasure_encoder encoder,
	uint8_t* const* shards,
	const erasure_bool* present)
{
	if (!encoder || !shards || !present)
		return ERASURE_INVALID_ARGUMENTS;

	try
	{
		return (erasure_error_code)erasure::recover(
			encoder,
			shards,
			(const bool*)present);
	}
	catch (...)
	{
		return ERASURE_INTERNAL_ERROR;
	}
}
