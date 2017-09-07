
static constexpr size_t k = 2;
static constexpr size_t n = 3;
static constexpr size_t data_size = 32;

#include "recover-template.hpp"

int main()
{
	erasure_encoder_flags flags[] = {
		ERASURE_FORCE_REF_IMPL,
		ERASURE_FORCE_ADV_IMPL,
		ERASURE_FORCE_SSSE3_IMPL,
		ERASURE_FORCE_AVX2_IMPL
	};

	for (auto flag : flags)
	{
		if (!run_test(flag))
			return 1;
	}

	return 0;
}
