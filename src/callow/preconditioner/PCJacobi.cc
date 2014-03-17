//----------------------------------*-C++-*-----------------------------------//
/**
 *  @file  PCJacobi.cc
 *  @brief PCJacobi member definitions
 *  @note  Copyright (C) 2012-2013 Jeremy Roberts
 */
//----------------------------------------------------------------------------//

#include "PCJacobi.hh"

namespace callow
{

//----------------------------------------------------------------------------//
PCJacobi::PCJacobi(SP_matrix A, SP_db db)
  : Preconditioner("PCJacobi", db)
{
  Require(A);
  Require(A->number_rows() == A->number_columns());
  Insist(dynamic_cast<Matrix*>(A.bp()),
    "Need an explicit matrix for use with PCILU0");
  Matrix::SP_matrix B(A);

  // create the diagonal vector
  int n = B->number_rows();
  d_P = new Vector(n, 0.0);

  // load values from A and avoid divide by zero
  for (int i = 0; i < n; ++i)
  {
    int d = B->diagonal(i);
    double aii = (*B)[d];
    if (aii == 0.0)
      (*d_P)[i] = 1.0; // or n?
    else
      (*d_P)[i] = 1.0 / aii;  // / aii;
  }

  d_size = n;
}

//----------------------------------------------------------------------------//
void PCJacobi::apply(const Vector &b, Vector &x)
{
  Require(x.size() == d_P->size());

  // apply x = inv(P)*b
  x.copy(b);
  x.multiply((*d_P));
}

} // end namespace detran

//----------------------------------------------------------------------------//
//              end of file PCJacobi.cc
//----------------------------------------------------------------------------//
