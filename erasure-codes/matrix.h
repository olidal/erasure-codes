#pragma once

#include "symbol.h"

#include <cstring>
#include <cstdlib>
#include <cassert>

namespace erasure
{
	class matrix
	{
	private:
		size_t rows;
		size_t cols;
		symbol* values;
		bool can_delete;

		size_t datasize() const
		{
			return rows * cols * sizeof(symbol);
		}

	public:
		matrix() :
			rows(0),
			cols(0),
			values(nullptr),
			can_delete(true)
		{

		}
		matrix(size_t rows, size_t cols) :
			rows(rows),
			cols(cols),
			values((symbol*)std::malloc(datasize())),
			can_delete(true)
		{

		}
		matrix(size_t rows, size_t cols, symbol* values) :
			rows(rows),
			cols(cols),
			values(values),
			can_delete(false)
		{

		}

		matrix(const matrix& m) :
			rows(m.rows),
			cols(m.cols),
			values((symbol*)std::malloc(datasize())),
			can_delete(true)
		{
			std::memcpy(values, m.values, datasize());
		}
		matrix(matrix&& m) :
			rows(m.rows),
			cols(m.cols),
			values(m.values),
			can_delete(m.can_delete)
		{
			m.values = nullptr;
			m.rows = m.cols = 0;
			m.can_delete = true;
		}

		matrix& operator=(const matrix& m)
		{
			if (values == m.values)
				return *this;

			rows = m.rows;
			cols = m.cols;
			can_delete = true;

			if (!values)
				values = (symbol*)std::malloc(datasize());
			else
				values = (symbol*)std::realloc(values, datasize());

			std::memcpy(values, m.values, datasize());

			return *this;
		}
		matrix& operator=(matrix&& m)
		{
			if (this == &m)
				return *this;

			if (can_delete)
			{
				free(values);

				rows = m.rows;
				cols = m.cols;
				values = m.values;
				can_delete = m.can_delete;

				m.rows = m.cols = 0;
				m.values = nullptr;
				m.can_delete = true;
			}
			else
			{
				*this = reinterpret_cast<const matrix&>(m);
			}

			return *this;
		}

		size_t size1() const
		{
			return rows;
		}
		size_t size2() const
		{
			return cols;
		}
		size_t size() const
		{
			return size1() * size2();
		}

		symbol* data()
		{
			return values;
		}
		const symbol* data() const
		{
			return values;
		}

		symbol& operator()(size_t r, size_t c)
		{
			return values[r * cols + c];
		}
		const symbol& operator()(size_t r, size_t c) const
		{
			return values[r * cols + c];
		}

		matrix operator[](size_t r)
		{
			return row(r);
		}

		matrix row(size_t r) const
		{
			assert(r < rows);
			return matrix(1, cols, values + (r * cols));
		}
		matrix submatrix(size_t r1, size_t r2, size_t c1, size_t c2) const
		{
			assert(r1 < r2 && r2 < rows);
			assert(c1 < c2 && c2 < rows);

			matrix m(r2 - r1, c2 - c1);

			for (size_t r = 0; r < m.size1(); ++r)
			{
				for (size_t c = 0; c < m.size2(); ++c)
				{
					m(r, c) = (*this)(r + r1, c + c1);
				}
			}

			return m;
		}
	};

	inline matrix operator*(const matrix& lhs, const matrix& rhs)
	{
		assert(lhs.size2() == rhs.size1());
		matrix result(lhs.size1(), rhs.size2());

		for (size_t i = 0; i < lhs.size2(); ++i)
		{
			for (size_t j = 0; j < rhs.size2(); ++j)
			{
				symbol& sum = result(i, j) = 0;

				for (size_t k = 0; k < lhs.size2(); ++k)
				{
					sum += lhs(i, k) * rhs(k, j);
				}
			}
		}

		return result;
	}

	bool inverse(matrix& mat);

	matrix vandermonde(size_t n, size_t k);
	matrix build_matrix(size_t n, size_t k);
}
