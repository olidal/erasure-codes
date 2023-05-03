#include <cstddef>
#include <cstring>

static constexpr size_t k = 205;
static constexpr size_t n = 255;
static constexpr size_t data_size = 256;

#include "recover-template.hpp"
//#include "lohi_table.h"

int main()
{
	return run_test(ERASURE_DEFAULT) ? 0 : 1;
}
