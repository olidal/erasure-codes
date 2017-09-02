#include "matrix.h"
#include "encode.h"

#include <intrin.h>
#include <cstring>
#include <cstdio>

#include <algorithm>

namespace {
#define SWAP(a,b,e) std::swap(a,b)
	typedef uint8_t gf;

	void addmul(gf* dst, gf* src, gf c, int sz)
	{
		if (c != 0)
			gfarith::adv::mul_add_row(c, src, dst, sz);
	}

	//TODO: Update this to something
	//      that was written myself.
	//      Preferably something easier
	//      to understand and maintain.
	static int invert_mat(gf *src, int k)
	{
		gf c, *p;
		int irow, icol, row, col, i, ix;

		int error = 1;
		int* indxc = (int*)alloca(sizeof(int) * k);
		int* indxr = (int*)alloca(sizeof(int) * k);
		int* ipiv = (int*)alloca(sizeof(int) * k);
		gf* id_row = (gf*)alloca(sizeof(gf) * k);

		memset(id_row, 0, k * sizeof(gf));
		/*
		* ipiv marks elements already used as pivots.
		*/
		for (i = 0; i < k; i++)
			ipiv[i] = 0;

		for (col = 0; col < k; col++) {
			gf *pivot_row;
			/*
			* Zeroing column 'col', look for a non-zero element.
			* First try on the diagonal, if it fails, look elsewhere.
			*/
			irow = icol = -1;
			if (ipiv[col] != 1 && src[col*k + col] != 0) {
				irow = col;
				icol = col;
				goto found_piv;
			}
			for (row = 0; row < k; row++) {
				if (ipiv[row] != 1) {
					for (ix = 0; ix < k; ix++) {
						if (ipiv[ix] == 0) {
							if (src[row*k + ix] != 0) {
								irow = row;
								icol = ix;
								goto found_piv;
							}
						}
						else if (ipiv[ix] > 1) {
							fprintf(stderr, "singular matrix\n");
							goto fail;
						}
					}
				}
			}
			if (icol == -1) {
				fprintf(stderr, "XXX pivot not found!\n");
				goto fail;
			}
		found_piv:
			++(ipiv[icol]);
			/*
			* swap rows irow and icol, so afterwards the diagonal
			* element will be correct. Rarely done, not worth
			* optimizing.
			*/
			if (irow != icol) {
				for (ix = 0; ix < k; ix++) {
					SWAP(src[irow*k + ix], src[icol*k + ix], gf);
				}
			}
			indxr[col] = irow;
			indxc[col] = icol;
			pivot_row = &src[icol*k];
			c = pivot_row[icol];
			if (c == 0) {
				fprintf(stderr, "singular matrix 2\n");
				goto fail;
			}
			if (c != 1) { /* otherwhise this is a NOP */
						  /*
						  * this is done often , but optimizing is not so
						  * fruitful, at least in the obvious ways (unrolling)
						  */
				c = gfarith::div(1, c);
				pivot_row[icol] = 1;
				for (ix = 0; ix < k; ix++)
					pivot_row[ix] = gfarith::mul(c, pivot_row[ix]);
			}
			/*
			* from all rows, remove multiples of the selected row
			* to zero the relevant entry (in fact, the entry is not zero
			* because we know it must be zero).
			* (Here, if we know that the pivot_row is the identity,
			* we can optimize the addmul).
			*/
			id_row[icol] = 1;
			if (memcmp(pivot_row, id_row, k * sizeof(gf)) != 0) {
				for (p = src, ix = 0; ix < k; ix++, p += k) {
					if (ix != icol) {
						c = p[icol];
						p[icol] = 0;
						addmul(p, pivot_row, c, k);
					}
				}
			}
			id_row[icol] = 0;
		} /* done all columns */
		for (col = k - 1; col >= 0; col--) {
			if (indxr[col] < 0 || indxr[col] >= k)
				fprintf(stderr, "AARGH, indxr[col] %d\n", indxr[col]);
			else if (indxc[col] < 0 || indxc[col] >= k)
				fprintf(stderr, "AARGH, indxc[col] %d\n", indxc[col]);
			else
				if (indxr[col] != indxc[col]) {
					for (row = 0; row < k; row++) {
						SWAP(src[row*k + indxr[col]], src[row*k + indxc[col]], gf);
					}
				}
		}
		error = 0;
	fail:
		return error;
	}
}

namespace gfarith
{
	matrix matrix::inverse() const
	{
		assert(size1() == size2());
		assert(size1() <= INT_MAX);

		if (this->is_null())
			return matrix();

		matrix m = *this;
		gf* vals = (gf*)m.data();

		if (invert_mat(vals, (int)size1()) != 0)
			return matrix();

		return m;
	}
}
