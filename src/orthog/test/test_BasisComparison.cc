//----------------------------------*-C++-*----------------------------------//
/**
 *  @file   test_BasisComparison.cc
 *  @author Jeremy Roberts
 *  @date   Apr 1, 2012
 *  @brief  Test of DLP class
 */
//---------------------------------------------------------------------------//

// LIST OF TEST FUNCTIONS
#define TEST_LIST                               \
        FUNC(test_BasisComparisonAnalytic  )    \
        FUNC(test_BasisComparisonDiscontinuous)

#include "utilities/TestDriver.hh"
#include "utilities/Definitions.hh"
#include "orthog/CLP.hh"
#include "orthog/DLP.hh"
#include "orthog/DCP.hh"
#include "orthog/DCT.hh"
#include "callow/utils/Initialization.hh"
#include <cmath>

using namespace detran_orthog;
using namespace detran_utilities;
using namespace detran_test;
using namespace std;

int main(int argc, char *argv[])
{
  callow_initialize(argc, argv);
  RUN(argc, argv);
  callow_finalize();
}

//---------------------------------------------------------------------------//
// TEST DEFINITIONS
//---------------------------------------------------------------------------//

/*
 *  This test compares the accuracy of all the basis sets on
 *  an analytic (trig) function defined at evenly-spaced points.
 *  Results are given based on L1, L2, and Linf norms.
 */
int test_BasisComparisonAnalytic(int argc, char *argv[])
{

  int N = 20; // Size of the vector
  int M = N-1; // Maximum order
  double width = 2.0 / N;

  // Abscissa for function and width for CLP
  vec_dbl x(N, 0);
  x[0] = -1.0 + width / 2.0;
  for (int i = 1; i < N; ++i) x[i] = x[i-1] + width;
  vec_dbl dx(N, width);

  // Errors in various norms
  vec_dbl err1_clp(N, 0.0);
  vec_dbl err2_clp(N, 0.0);
  vec_dbl errI_clp(N, 0.0);
  vec_dbl err1_dlp(N, 0.0);
  vec_dbl err2_dlp(N, 0.0);
  vec_dbl errI_dlp(N, 0.0);
  vec_dbl err1_dcp(N, 0.0);
  vec_dbl err2_dcp(N, 0.0);
  vec_dbl errI_dcp(N, 0.0);
  vec_dbl err1_dct(N, 0.0);
  vec_dbl err2_dct(N, 0.0);
  vec_dbl errI_dct(N, 0.0);

  // Function vector (cos(x) over -1 .. 1)
  callow::Vector f(N, 0.0);
  for (int i = 0; i < N; ++i) f[i] = std::cos(x[i]) + std::sin(x[i]);

  // Loop over all possible orders.  We compute the error
  // for each basis at each truncated order.
  for (int o = 0; o <= M; ++o)
  {
    // Basis sets
    CLP clp(o, x, dx);
    DLP dlp(o, N);
    DCP dcp(o, N);
    DCT dct(o, N);

    // Transformed vector
    callow::Vector ft(o + 1, 0.0);
    // Reconstructed vector
    callow::Vector f2(N, 0.0);

    // CLP
    clp.transform(f, ft);
    clp.inverse(ft, f2);
    err1_clp[o] = f2.norm_residual(f, callow::L1);
    err2_clp[o] = f2.norm_residual(f, callow::L2);
    errI_clp[o] = f2.norm_residual(f, callow::LINF);
    // DLP
    dlp.transform(f, ft);
    dlp.inverse(ft, f2);
    err1_dlp[o] = f2.norm_residual(f, callow::L1);
    err2_dlp[o] = f2.norm_residual(f, callow::L2);
    errI_dlp[o] = f2.norm_residual(f, callow::LINF);
    // DCP
    dcp.transform(f, ft);
    dcp.inverse(ft, f2);
    err1_dcp[o] = f2.norm_residual(f, callow::L1);
    err2_dcp[o] = f2.norm_residual(f, callow::L2);
    errI_dcp[o] = f2.norm_residual(f, callow::LINF);
    // DCT
    dct.transform(f, ft);
    dct.inverse(ft, f2);
    err1_dct[o] = f2.norm_residual(f, callow::L1);
    err2_dct[o] = f2.norm_residual(f, callow::L2);
    errI_dct[o] = f2.norm_residual(f, callow::LINF);

    printf("%4i | %12.6e %12.6e %12.6e %12.6e | %12.6e %12.6e %12.6e %12.6e | %12.6e %12.6e %12.6e %12.6e \n",
           o,
           err1_clp[o], err1_dlp[o], err1_dcp[o], err1_dct[o],
           err2_clp[o], err2_dlp[o], err2_dcp[o], err2_dct[o],
           errI_clp[o], errI_dlp[o], errI_dcp[o], errI_dct[o]);
  }

  return 0;
}

/*
 *  This test compares the accuracy of all the basis sets on
 *  an analytic (trig) function defined at evenly-spaced points.
 *  Results are given based on L1, L2, and Linf norms.
 */
int test_BasisComparisonDiscontinuous(int argc, char *argv[])
{
  return 0;
}

//---------------------------------------------------------------------------//
//              end of test_MomentIndexer.cc
//---------------------------------------------------------------------------//
