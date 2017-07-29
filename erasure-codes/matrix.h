#pragma once

#include "galois.h"

namespace erasure
{
	typedef galois_int<uint8_t> symbol_t;

	struct matrix
	{
		size_t n_rows;
		size_t n_cols;
		symbol_t* data;

	private:
		size_t alloc_size() const
		{
			return sizeof(symbol_t) * n_rows * n_cols;
		}

	public:
		symbol_t& operator()(size_t r, size_t c)
		{
			return data[n_cols * r + c];
		}
		symbol_t& operator()(size_t r, size_t c) const
		{
			return data[n_cols * r + c];
		}

		matrix() :
			n_rows(0),
			n_cols(0),
			data(nullptr)
		{

		}
		matrix(size_t rows, size_t cols) :
			n_rows(rows),
			n_cols(cols),
			data((symbol_t*)malloc(alloc_size()))
		{
			assert(data != nullptr);
		}
		matrix(size_t rows, size_t cols, symbol_t v) :
			n_rows(rows),
			n_cols(cols),
			data((symbol_t*)malloc(alloc_size()))
		{
			assert(data != nullptr);
			std::memset(data, 0, alloc_size());
			auto max = std::min(rows, cols);
			for (size_t i = 0; i < max; ++i)
			{
				(*this)(i, i) = v;
			}
		}

		matrix(const matrix& mat) :
			n_rows(mat.n_rows),
			n_cols(mat.n_cols),
			data((symbol_t*)std::malloc(alloc_size()))

		{
			std::memcpy(data, mat.data, alloc_size());
		}
		matrix(matrix&& mat) :
			n_rows(mat.n_rows),
			n_cols(mat.n_cols),
			data(mat.data)
		{
			mat.n_cols = mat.n_rows = 0;
			mat.data = nullptr;
		}

		matrix& operator=(const matrix& mat)
		{
			if (mat.data == data)
				return *this;

			n_rows = mat.n_rows;
			n_cols = mat.n_cols;

			data = (symbol_t*)realloc(data, alloc_size());
			std::memcpy(data, mat.data, alloc_size());

			return *this;
		}
		matrix& operator=(matrix&& mat)
		{
			if (mat.data == data)
				return *this;

			free(data);

			n_rows = mat.n_rows;
			n_cols = mat.n_cols;
			data = mat.data;

			mat.n_rows = 0;
			mat.n_cols = 0;
			mat.data = nullptr;

			return *this;
		}

		~matrix()
		{
			free(data);
		}

		matrix operator*(const matrix& m) const
		{
			assert(m.n_rows == n_cols);

			matrix r = matrix(n_rows, m.n_cols);

			for (size_t i = 0; i < n_rows; ++i)
			{
				for (size_t k = 0; k < m.n_cols; ++k)
				{
					symbol_t sum = 0;
					for (size_t j = 0; j < n_cols; ++j)
					{
						sum += (*this)(i, j) * m(j, k);
					}
					r(i, k) = sum;
				}
			}

			return r;
		}

		matrix submatrix(size_t rmax, size_t cmax) const
		{
			matrix m = matrix(rmax, cmax);

			for (size_t r = 0; r < rmax; ++r)
			{
				std::memcpy(&m(r, 0), &(*this)(r, 0), sizeof(symbol_t) * cmax);
			}

			return m;
		}
		matrix submatrix(
			size_t rmin, size_t cmin, 
			size_t rmax, size_t cmax) const
		{
			matrix m = matrix(rmax - rmin, cmax - cmin);

			for (size_t r = 0; r < rmax - rmin; ++r)
			{
				for (size_t c = 0; c < cmax - cmin; ++c)
				{
					m(r, c) = (*this)(rmin + r, cmin + c);
				}
			}

			return m;
		}

		symbol_t* row(size_t r)
		{
			return &data[n_cols * r];
		}
		const symbol_t* row(size_t r) const
		{
			return &data[n_cols * r];
		}
	};

	inline matrix inverse(matrix orig)
	{
		assert(orig.n_rows == orig.n_cols);
		matrix inv = matrix(orig.n_rows, orig.n_cols, 1);

		for (size_t i = 0; i < orig.n_rows; ++i)
		{
			symbol_t div = orig(i, i);

			for (size_t j = 0; j < orig.n_cols; ++j)
				orig(i, j) /= div;
			for (size_t j = 0; j < orig.n_cols; ++j)
				inv(i, j) /= div;

			for (size_t j = 0; j < orig.n_rows; ++j)
			{
				if (j == i)
					continue;

				symbol_t mult = orig(j, i);

				for (size_t k = i; k < orig.n_cols; ++k)
				{
					symbol_t fact = orig(i, k) * mult;
					orig(j, k) -= fact;
					inv(j, k) -= fact;
				}
			}
		}

		return inv;
	}
}
