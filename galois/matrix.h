#pragma once

#include "ops.h"

#include <cstring>
#include <cassert>
#include <type_traits>

namespace galois
{
	static constexpr size_t rows = 16;
	static constexpr size_t cols = 16;

	int GALOIS_EXPORT invert_matrix(symbol* src, size_t k);

	template<size_t rows, size_t cols>
	struct matrix
	{
		static constexpr size_t n_rows = rows;
		static constexpr size_t n_cols = cols;

		symbol data[rows][cols];

		symbol& operator()(size_t r, size_t c)
		{
			return data[r][c];
		}
		const symbol& operator()(size_t r, size_t c) const
		{
			return data[r][c];
		}

		symbol* operator[](size_t r)
		{
			return data[r];
		}
		const symbol* operator[](size_t r) const
		{
			return data[r];
		}

		bool operator==(const matrix& m) const
		{
			return std::memcmp(data, m.data, sizeof(data)) == 0;
		}
		bool operator!=(const matrix& m) const
		{
			return !(*this == m);
		}

		matrix() = default;
		matrix(symbol diag)
		{
			std::memset(data, 0, sizeof(data));
			size_t lim = rows < cols ? rows : cols;

			for (size_t i = 0; i < lim; ++i)
			{
				data[i][i] = diag;
			}
		}
		matrix(const symbol* mem)
		{
			std::memcpy(data, mem, sizeof(data));
		}

		void swaprows(size_t r1, size_t r2)
		{
			symbol tmp[cols];

			std::memcpy(tmp, data[r1], sizeof(tmp));
			std::memcpy(data[r1], data[r2], sizeof(tmp));
			std::memcpy(data[r2], tmp, sizeof(tmp));
		}

		template<typename = typename std::enable_if<rows == cols>::type>
		matrix inverse() const
		{
			matrix ninv = *this;
			invert_matrix((symbol*)ninv.data, rows);
			return ninv;
		}
	
		template<size_t r1, size_t c1, size_t r2, size_t c2>
		typename std::enable_if<(r2 <= rows && c2 <= cols), matrix<r2 - r1, c2 - c1>>::type
			submatrix() const
		{
			matrix<r2 - r1, c2 - c1> m;

			for (size_t r = 0; r < r2 - r1; ++r)
			{
				for (size_t c = 0; c < c2 - c1; ++c)
				{
					m(r, c) = data[r + r1][c + c1];
				}
			}

			return m;
		}
		template<size_t r, size_t c>
		typename std::enable_if<(r <= rows && c <= cols), matrix<r, c>>::type
			submatrix() const
		{
			return submatrix<0, 0, r, c>();
		}
	};

	template<size_t A, size_t B, size_t C>
	matrix<A, C> operator*(
		const matrix<A, B>& a,
		const matrix<B, C>& b)
	{
		matrix<A, C> r;

		for (size_t i = 0; i < A; ++i)
		{
			for (size_t k = 0; k < C; ++k)
			{
				symbol sum = 0;
				for (size_t j = 0; j < B; ++j)
					sum += a(i, j) * b(j, k);
				r(i, k) = sum;
			}
		}
		
		return r;
	}

	template<size_t n, size_t k>
	static matrix<n, k> vandermonde()
	{
		matrix<n, k> result;
		for (uint8_t r = 0; r < matrix<n, k>::n_rows; ++r)
		{
			for (uint8_t c = 0; c < matrix<n, k>::n_cols; ++c)
			{
				result(r, c) = symbol::exp(r, c);
			}
		}
		return result;
	}
	template<size_t n, size_t k>
	static matrix<n, k> build_matrix()
	{
		auto vm = vandermonde<n, k>();
		auto inv = vm.submatrix<k, k>().inverse();
		return vm * inv;
	}
}
