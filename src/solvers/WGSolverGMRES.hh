//----------------------------------*-C++-*----------------------------------//
/**
 *  @file   WGSolverGMRES.hh
 *  @author robertsj
 *  @date   Apr 4, 2012
 *  @brief  WGSolverGMRES class definition.
 */
//---------------------------------------------------------------------------//

#ifndef detran_WGSOLVERGMRES_HH_
#define detran_WGSOLVERGMRES_HH_

#include "WGSolver.hh"
#include "PreconditionerWG.hh"
#include "petsc.h"

namespace detran
{

//---------------------------------------------------------------------------//
/**
 *  @class WGSolverGMRES
 *  @brief Solve the within-group problem with GMRES.
 *
 *  From @ref WGSolver, we know the within-group problem can be
 *  written in operator notation as
 *  @f[
 *      (\mathbf{I} - \mathbf{D}\mathbf{L}^{-1}\mathbf{MS})\phi
 *      = \mathbf{D} \mathbf{L}^{-1} Q \, ,
 *  @f]
 *  or
 *  @f[
 *      \mathbf{A}x = b \, .
 *  @f]
 *
 *  This class couples with callow to make available its set of applicable
 *  solvers, the default being GMRES.  Other solvers are selected
 *  by command line flags, e.g. -ksp_type bcgs uses a BiCongugate Gradient
 *  Stabilized algorithm.  Past experience suggests GMRES works best.
 *
 *  For Krylov iterations to perform successfully, preconditioning
 *  is often required.  A good preconditioner $\f \mathbf{M} \f$
 *  is in some way "similar" to the operator $\f \mathbf{A} \f$, and
 *  applying its inverse $\f \mathbf{M}^{-1} $\f can be done cheaply.
 *  Currently, a diffusion preconditioner is available for within-group
 *  solvers; see \ref PreconditionerWG for a detailed description.
 *
 */
//---------------------------------------------------------------------------//

template <class D>
class WGSolverGMRES: public WGSolver<D>
{

public:

  //-------------------------------------------------------------------------//
  // TYPEDEFS
  //-------------------------------------------------------------------------//

  typedef WGSolver<D>                           Base;
  typedef typename Base::SP_solver              SP_solver;
  typedef typename Base::SP_input               SP_input;
  typedef typename Base::SP_state               SP_state;
  typedef typename Base::SP_mesh                SP_mesh;
  typedef typename Base::SP_material            SP_material;
  typedef typename Base::SP_quadrature          SP_quadrature;
  typedef typename Base::SP_boundary            SP_boundary;
  typedef typename Base::SP_MtoD                SP_MtoD;
  typedef typename Base::SP_externalsource      SP_externalsource;
  typedef typename Base::vec_externalsource     vec_externalsource;
  typedef typename Base::SP_fissionsource       SP_fissionsource;
  typedef typename Base::SP_sweeper             SP_sweeper;
  typedef typename Base::SP_sweepsource         SP_sweepsource;
  typedef typename Base::moments_type           moments_type;
  typedef typename Base::size_t                 size_t;
  typedef PreconditionerWG::SP_pc               SP_pc;
  typedef detran_utilities::vec_dbl             vec_dbl;

  //-------------------------------------------------------------------------//
  // CONSTRUCTOR & DESTRUCTOR
  //-------------------------------------------------------------------------//

  /**
   *  @brief Constructor
   *  @param state             State vectors, etc.
   *  @param mat               Material definitions.
   *  @param quadrature        Angular mesh.
   *  @param boundary          Boundary fluxes.
   *  @param external_source   User-defined external source.
   *  @param fission_source    Fission source.
   *  @param multiply          Flag for fixed source multiplying problem
   */
  WGSolverGMRES(SP_state                   state,
                SP_material                material,
                SP_quadrature              quadrature,
                SP_boundary                boundary,
                const vec_externalsource  &q_e,
                SP_fissionsource           q_f,
                bool                       multiply);

  // Destructor
  ~WGSolverGMRES()
  {
    KSPDestroy(&d_solver);
    MatDestroy(&d_operator);
    VecDestroy(&d_X);
    VecDestroy(&d_B);
  }

  //-------------------------------------------------------------------------//
  // ABSTRACT INTERFACE -- ALL WITHIN-GROUP SOLVERS MUST IMPLEMENT
  //-------------------------------------------------------------------------//

  /// Solve the within group equation.
  void solve(const size_t g);

  // Friend functions for applying dimension-specific operator.
  friend PetscErrorCode apply_WGTO_1D(Mat A, Vec x, Vec y);
  friend PetscErrorCode apply_WGTO_2D(Mat A, Vec x, Vec y);
  friend PetscErrorCode apply_WGTO_3D(Mat A, Vec x, Vec y);

private:

  //-------------------------------------------------------------------------//
  // DATA
  //-------------------------------------------------------------------------//

  // Make inherited data visible
  using Base::d_input;
  using Base::d_state;
  using Base::d_mesh;
  using Base::d_material;
  using Base::d_quadrature;
  using Base::d_boundary;
  using Base::d_sweeper;
  using Base::d_sweepsource;
  using Base::d_tolerance;
  using Base::d_maximum_iterations;
  using Base::d_print_level;
  using Base::d_print_interval;
  using Base::d_g;

  /// Main linear solver
  KSP d_solver;
  /// Operator "A" in "Ax = b"
  Mat d_operator;
  /// Solution vector
  Vec d_X;
  /// Right hand side
  Vec d_B;
  /// Size of the moments portion of d_X
  int d_moments_size;
  /// Size of the boundary portion of d_X
  int d_boundary_size;
  int d_reflective_solve_iterations;
  /// Preconditioner flag
  bool d_use_pc;
  /// Diffusion preconditioner
  SP_pc d_pc;

  //-------------------------------------------------------------------------//
  // IMPLEMENTATION
  //-------------------------------------------------------------------------//

  /// Set the templated operator function.
  PetscErrorCode set_operation();

  /// Build the right hand side.
  void build_rhs(State::moments_type &B);

  //---------------------------------------------------------------------------//
  /*!
   * \brief A matrix-vector shell for the within-group transport operator.
   *
   * Given a Krylov vector \f$ x \f$, this returns
   * \f[
   *    x' \leftarrow
   *       (\mathbf{I} - \mathbf{DL}^{-1}\mathbf{MS})x \, .
   * \f]
   *
   * This is called by thin wrappers, since PETSc needs the matrix-vector
   * operation as a function pointer, which precludes a member function.
   *
   * \note This is public, since I haven't figured out a way to use friends
   *       in this way---maybe there is no way.
   *
   * \param   A       PETSc shell matrix
   * \param   x       Incoming PETSc vector
   * \param   y       Outgoing PETSc vector
   */
public:
  PetscErrorCode apply_WGTO(Mat A, Vec x, Vec y);

};

} // namespace detran

//---------------------------------------------------------------------------//
// EXTERNAL WRAPPER FUNCTIONS
//---------------------------------------------------------------------------//

PetscErrorCode apply_WGTO_1D(Mat A, Vec x, Vec y);
PetscErrorCode apply_WGTO_2D(Mat A, Vec x, Vec y);
PetscErrorCode apply_WGTO_3D(Mat A, Vec x, Vec y);

//---------------------------------------------------------------------------//
// INLINE FUNCTIONS
//---------------------------------------------------------------------------//

#include "WGSolverGMRES.i.hh"

#endif /* detran_WGSOLVERGMRES_HH_ */

//---------------------------------------------------------------------------//
//              end of WGSolverGMRES.hh
//---------------------------------------------------------------------------//
