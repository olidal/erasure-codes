#include "matrix.h"

#include <cstdlib>
#include <cstring>

namespace gfarith
{
	symbol* matrix::alloc_values() const
	{
		return (symbol*)std::malloc(datasize());
	}

	matrix::matrix() :
		rows(0),
		cols(0),
		values(nullptr)
	{

	}
	matrix::matrix(size_t rows, size_t cols) :
		rows(rows),
		cols(cols)
	{
		values = alloc_values();
	}
	matrix::matrix(size_t rows, size_t cols, symbol diag) :
		rows(rows),
		cols(cols)
	{
		values = alloc_values();

		if (values != nullptr)
		{
			std::memset(values, 0, datasize());

			size_t mindim = rows < cols ? rows : cols;
			for (size_t i = 0; i < mindim; ++i)
			{
				values[i] = diag;
			}
		}
	}

	matrix::matrix(const matrix& m) :
		rows(m.rows),
		cols(m.cols)
	{
		values = alloc_values();

		std::memcpy(values, m.values, datasize());
	}
	matrix::matrix(matrix&& m) :
		rows(m.rows),
		cols(m.cols)
	{
		values = m.values;

		m.values = nullptr;
		m.rows = m.cols = 0;
	}

	matrix::~matrix()
	{
		free(values);
	}

	matrix& matrix::operator=(const matrix& m)
	{
		if (m.values == values)
			return *this;

		if (!values)
		{
			values = alloc_values();
		}
		else
		{
			values = (symbol*)std::realloc(values, datasize());
		}

		if (!values)
		{
			// Memory allocation failed 
			rows = 0;
			cols = 0;
		}
		else
		{
			std::memcpy(values, m.values, datasize());
		}

		return *this;
	}
	matrix& matrix::operator=(matrix&& m)
	{
		if (values == m.values)
			return *this;

		if (values)
			free(values);

		rows = m.rows;
		cols = m.cols;
		values = m.values;

		m.rows = m.cols = 0;
		m.values = nullptr;

		return *this;
	}

	matrix matrix::submatrix(size_t r1, size_t r2, size_t c1, size_t c2) const
	{
		assert(r1 <= r2 && r2 <= rows);
		assert(c1 <= c2 && c2 <= cols);
		
		if (is_null())
			return matrix();

		matrix m{ r2 - r1, c2 - c1 };

		for (size_t r = 0; r < m.size1(); ++r)
		{
			std::memcpy(m[r].data(), (*this)[r1 + r].data() + c1, m.size2());
		}

		return m;
	}

	matrix operator*(const matrix& a, const matrix& b)
	{
		assert(a.size2() == b.size1());

		if (a.is_null() || b.is_null())
			return matrix();

		matrix result(a.size1(), b.size2());

		for (size_t i = 0; i < a.size1(); ++i)
		{
			for (size_t j = 0; j < b.size2(); ++j)
			{
				symbol& sum = result(i, j) = 0;

				for (size_t k = 0; k < a.size2(); ++k)
				{
					sum += a(i, k) * b(k, j);
				}
			}
		}

		return result;
	}

	void div_row(symbol* row, symbol divisor, size_t length)
	{
		for (size_t i = 0; i < length; ++i)
			row[i] /= divisor;
	}
	// Subtract row2 from row1 and store the result in row1
	void sub_rows(symbol* row1, const symbol* row2, size_t length)
	{
		for (size_t i = 0; i < length; ++i)
			row1[i] -= row2[i];
	}

	matrix matrix::inverse() const
	{
		assert(rows == cols);

		if (this->is_null())
			return matrix();

		matrix m{ rows, cols * 2 };

		std::memset(m.data(), 0, m.datasize());

		for (size_t i = 0; i < rows; ++i)
		{
			std::memcpy(m[i].data(), (*this)[i].data(), cols * sizeof(symbol));

			m(i, cols + i) = 1;
		}

		for (size_t r1 = 0; r1 < m.size1(); ++r1)
		{
			symbol div = m(r1, r1);

			assert(div.value != 0);
			if (div.value != 1)
			{
				for (size_t c = r1; c < m.size2(); ++c)
				{
					m(r1, c) /= div;
				}
			}

			for (size_t r2 = r1 + 1; r2 < m.size1(); ++r2)
			{
				for (size_t c = r1; c < m.size2(); ++c)
				{
					m(r2, c) -= m(r1, c);
				}
			}
		}

		return m;
	}
}
