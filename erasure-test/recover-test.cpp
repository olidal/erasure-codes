
static constexpr size_t k = 2;
static constexpr size_t n = 3;
static constexpr size_t data_size = 32;

#include "recover-template.hpp"

int main()
{
	encoder_flags flags[] = {
		USE_REF_IMPL,
		USE_ADV_IMPL,
		USE_SSSE3_IMPL,
		USE_AVX2_IMPL
	};

	for (auto flag : flags)
	{
		if (!run_test(flag))
			return 1;
	}

	return 0;
}
