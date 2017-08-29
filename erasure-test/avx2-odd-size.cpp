
static constexpr size_t k = 15;
static constexpr size_t n = 20;
static constexpr size_t data_size = 145;

#include "recover-template.hpp"

int main()
{
	return run_test(ERASURE_FORCE_AVX2_IMPL) ? 0 : 1;
}
