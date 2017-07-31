#pragma once

#include <cstdint>

namespace galois
{
	uint8_t mul(uint8_t a, uint8_t b);
	uint8_t div(uint8_t a, uint8_t b);
	uint8_t exp(uint8_t a, uint8_t b);
	uint8_t add(uint8_t a, uint8_t b);
	uint8_t sub(uint8_t a, uint8_t b);

	struct symbol
	{
		uint8_t value;

		symbol() = default;
		symbol(uint8_t v) :
			value(v)
		{

		}

		static symbol exp(symbol a, symbol b)
		{
			return galois::exp(a.value, b.value);
		}

		explicit operator uint8_t() const
		{
			return value;
		}

		template<typename T>
		bool operator ==(const T& v)
		{
			return value == v;
		}
		template<typename T>
		bool operator !=(const T& v)
		{
			return !(*this == v);
		}
	};

	inline symbol operator+(symbol a, symbol b)
	{
		return symbol{ add(a.value, b.value) };
	}
	inline symbol operator-(symbol a, symbol b)
	{
		return symbol{ sub(a.value, b.value) };
	}
	inline symbol operator*(symbol a, symbol b)
	{
		return symbol{ mul(a.value, b.value) };
	}
	inline symbol operator/(symbol a, symbol b)
	{
		return symbol{ div(a.value, b.value) };
	}

	inline symbol& operator+=(symbol& a, symbol b)
	{
		a = a + b;
		return a;
	}
	inline symbol& operator-=(symbol& a, symbol b)
	{
		a = a - b;
		return a;
	}
	inline symbol& operator*=(symbol& a, symbol b)
	{
		a = a * b;
		return a;
	}
	inline symbol& operator/=(symbol& a, symbol b)
	{
		a = a / b;
		return a;
	}
}
