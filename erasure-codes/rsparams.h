#pragma once

#include <armadillo>


namespace erasure
{
	void a()
	{
		arma::umat44 mat;
		auto aa = arma::inv(mat);
	}
}
