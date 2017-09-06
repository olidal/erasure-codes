
static constexpr size_t k = 243;
static constexpr size_t n = 255;
static constexpr size_t data_size = 256;

#include "recover-template.hpp"

int main()
{
	// Expect failure for now
	return run_test(ERASURE_DEFAULT) ? 1 : 0;
}
