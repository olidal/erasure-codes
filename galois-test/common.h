
#include <cstring>

struct assert_faliure {};

void require_impl(const char* message, bool result)
{
	throw assert_faliure;
}

#define REQUIRE(cond) require_impl(#cond, cond)
