#pragma once

#include "symbol.h"

#include <cstring>
#include <cstdlib>
#include <cassert>

namespace erasure
{
	class matrix_row
	{
	private:
		size_t length;
		symbol* values;

	public:
		matrix_row() :
			length(0),
			values(nullptr)
		{

		}
		matrix_row(size_t length, symbol* values) :
			length(length),
			values(values)
		{

		}
		matrix_row(const matrix_row& m) :
			length(m.length),
			values(m.values)
		{

		}

		matrix_row& operator=(const matrix_row& m)
		{
			if (length == m.length 
				&& !!values 
				&& !!m.values 
				&& values != m.values)
			{
				std::memcpy(values, m.values, sizeof(symbol) * size());
			}
			else
			{
				values = m.values;
				length = m.length;
			}

			return *this;
		}

		symbol& operator[](size_t i)
		{
			assert(i < length);

			return values[i];
		}
		const symbol& operator[](size_t i) const
		{
			assert(i < length);
			return values[i];
		}

		symbol* data()
		{
			return values;
		}
		const symbol* data() const
		{
			return values;
		}

		size_t size1() const
		{
			return 1;
		}
		size_t size2() const
		{
			return length;
		}
		size_t size() const
		{
			return length;
		}
	};

	class matrix
	{
	private:
		size_t rows;
		size_t cols;
		symbol* values;

		size_t datasize() const
		{
			return rows * cols * sizeof(symbol);
		}

	public:
		matrix() :
			rows(0),
			cols(0),
			values(nullptr)
		{

		}
		matrix(size_t rows, size_t cols) :
			rows(rows),
			cols(cols),
			values(nullptr)
		{
			if (datasize() != 0)
				values = (symbol*)std::malloc(datasize());
		}

		~matrix()
		{
			free(values);
		}

		matrix(const matrix& m) :
			rows(m.rows),
			cols(m.cols),
			values((symbol*)std::malloc(datasize()))
		{
			if (!!values)
				std::memcpy(values, m.values, datasize());
		}
		matrix(matrix&& m) :
			rows(m.rows),
			cols(m.cols),
			values(m.values)
		{
			m.values = nullptr;
			m.rows = m.cols = 0;
		}

		matrix& operator=(const matrix& m)
		{
			if (values == m.values)
				return *this;

			rows = m.rows;
			cols = m.cols;

			free(values);
			values = (symbol*)std::malloc(datasize());

			if (!!values)
				std::memcpy(values, m.values, datasize());

			return *this;
		}
		matrix& operator=(matrix&& m)
		{
			if (this == &m)
				return *this;

			free(values);

			rows = m.rows;
			cols = m.cols;
			values = m.values;

			m.rows = m.cols = 0;
			m.values = nullptr;

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

		bool is_null() const
		{
			return values == nullptr;
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
			assert(r < rows && c < cols);

			return values[r * cols + c];
		}
		const symbol& operator()(size_t r, size_t c) const
		{
			assert(r < rows && c < cols);

			return values[r * cols + c];
		}

		matrix_row operator[](size_t r)
		{
			return row(r);
		}

		matrix_row row(size_t r) const
		{
			assert(r < rows);
			assert(!is_null());
			return matrix_row(cols, values + (r * cols));
		}
		matrix submatrix(size_t r1, size_t r2, size_t c1, size_t c2) const
		{
			assert(r1 <= r2 && r2 <= rows);
			assert(c1 <= c2 && c2 <= cols);

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
		if (lhs.is_null() || rhs.is_null())
			return matrix();

		matrix result(lhs.size1(), rhs.size2());

		for (size_t i = 0; i < lhs.size1(); ++i)
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
