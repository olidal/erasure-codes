#pragma once

#include <cstdint>
#include <array>
#include <cassert>
#include <limits>

namespace erasure
{
	template<typename T, bool auto_init = true>
	struct galois_field
	{
		typedef T value_type;
		static constexpr size_t field_max = std::numeric_limits<value_type>::max();
		typedef typename std::array<value_type, field_max + 1> table_type;

		static_assert(std::is_unsigned<value_type>::value, "galois_field can only have an unsigned value type");

		static table_type log_table;
		static table_type exp_table;

	private:
		static bool gen_log_table(table_type& table, value_type polynomial)
		{
			for (auto& val : table)
				val = field_max;

			uint64_t b = 1;

			for (value_type log = 0; log < field_max; ++log)
			{
				if (table[b] != field_max)
					return false;

				table[b] = log;

				b <<= 1;
				if (field_max < b)
				{
					b = (b - (field_max + 1)) ^ polynomial;
				}
			}

			return true;
		}
		static void gen_exp_table(table_type& table, const table_type& log_table)
		{
			for (value_type i = 1; i < field_max; ++i)
			{
				auto log = log_table[i];
				table[log] = i;
			}

			{
				auto log = log_table[255];
				table[log] = 255;
			}
		}

#ifdef ERASURE_CODES_GALOIS_AUTO_INIT
		struct dummy
		{
			dummy()
			{
				if (auto_init)
					init();
			}
		};

		// Hack to make init get called
		static const dummy dummy_val = dummy();
#endif
	public:
		static void init()
		{
			value_type poly = 1;
			for (; poly <= field_max; ++poly)
			{
				if (gen_log_table(log_table, poly))
					break;
			}

			// This field size has no valid irreducible polynomials
			assert(poly != field_max + 1);

			gen_exp_table(exp_table, log_table);
		}

		static value_type add(value_type a, value_type b)
		{
			return a ^ b;
		}
		static value_type sub(value_type a, value_type b)
		{
			return a ^ b;
		}
		static value_type mul(value_type a, value_type b)
		{
			if (a == 0 || b == 0)
				return 0;

			value_type log_a = log_table[a];
			value_type log_b = log_table[b];
			value_type result = log_a + log_b;

			return exp_table[result];
		}
		static value_type div(value_type a, value_type b)
		{
			assert(b != 0);
			if (a == 0)
				return 0;

			auto log_a = log_table[a];
			auto log_b = log_table[b];
			auto result = log_a - log_b;

			return exp_table[result];
		}
		static value_type exp(value_type a, value_type n)
		{
			if (n == 0)
				return 1;
			if (a == 0)
				return 0;

			auto log_a = log_table[a];
			auto result = log_a * n;
			return exp_table[result];
		}
	};

	template<typename T, bool auto_init = true>
	struct galois_int
	{
		typedef galois_field<T, auto_init> field_type;
		typedef T value_type;

		value_type value;

		galois_int() = default;
		galois_int(value_type v) :
			value(v)
		{

		}

		galois_int operator+(galois_int b) const
		{
			return field_type::add(value, b.value);
		}
		galois_int operator-(galois_int b) const
		{
			return field_type::sub(value, b.value);
		}
		galois_int operator*(galois_int b) const
		{
			return field_type::mul(value, b.value);
		}
		galois_int operator/(galois_int b) const
		{
			return field_type::div(value, b.value);
		}

		galois_int& operator+=(galois_int b)
		{
			return *this = *this + b;
		}
		galois_int& operator-=(galois_int b)
		{
			return *this = *this - b;
		}
		galois_int& operator*=(galois_int b)
		{
			return *this = *this * b;
		}
		galois_int& operator/=(galois_int b)
		{
			return *this = *this / b;
		}

		bool operator ==(galois_int v) const
		{
			return value == v.value;
		}
		bool operator !=(galois_int v) const
		{
			return value != v.value;
		}

		explicit operator value_type() const
		{
			return value;
		}

		static galois_int exp(galois_int a, galois_int n)
		{
			return field_type::exp(a.value, n.value);
		}
	};

	template<typename T, bool auto_init>
	typename galois_field<T, auto_init>::table_type galois_field<T, auto_init>::log_table;
	template<typename T, bool auto_init>
	typename galois_field<T, auto_init>::table_type galois_field<T, auto_init>::exp_table;
}
