//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   KrylovMG.hh
 * \author robertsj
 * \date   Apr 9, 2012
 * \brief  KrylovMG class definition.
 */
//---------------------------------------------------------------------------//

#ifndef KRYLOVMG_HH_
#define KRYLOVMG_HH_

#include "InnerIteration.hh"
#include "MultigroupSolver.hh"
#include "Sweeper.hh"
#include "PreconditionerMG.hh"
#include "petsc.h"

namespace detran
{

//---------------------------------------------------------------------------//
/*!
 *  \class KrylovMG
 *  \brief Solves the multigroup transport equation via Krylov methods.
 *
 * Traditionally, the Gauss-Seidel method has been used for multigroup
 * problems. For each group, the within-group equation is solved, and
 * the the fluxes are updated for use in the next group.  However, for
 * problems with significant upscatter, Gauss-Seidel can be quite
 * expensive, even when GMRES (or some better-than-source-iteration
 * scheme) is used for the within group solve.  As an
 * alternative, we can apply GMRES (or other Krylov solvers) to
 * the multigroup
 * problem directly.  The linear system is then
 * \f[
 *     \left( ( \mathbf{I} -
 *         \left(\begin{array}{ccc}
 *             T_1  & \cdots & 0     \\
 *             0    & \ddots & 0     \\
 *             0    & 0      & T_G
 *         \end{array}\right) \cdot
 *         \left(\begin{array}{ccc}
 *             M    & \cdots & 0     \\
 *             0    & \ddots & 0     \\
 *             0    & 0      & M
 *         \end{array}\right) \cdot
 *         \left(\begin{array}{ccc}
 *            \mathbf{S}_{11} & \cdots & \mathbf{S}_{1G} \\
 *            \vdots          & \ddots & \vdots          \\
 *            \mathbf{S}_{G1} & 0      & \mathbf{S}_{GG}
 *         \end{array}\right)
 *     \right ) \cdot
 *     \left[ \begin{array}{c}
 *         \phi_1 \\
 *         \vdots \\
 *         \phi_G
 *     \end{array} \right] =
 *     \left[ \begin{array}{c}
 *          \mathbf{T}_1 q_1 \\
 *          \vdots \\
 *          \mathbf{T}_G q_G
 *     \end{array} \right] \, .
 * \f]
 * Of course, this can be written succinctly in the same way as the within-
 * group equation:
 * \f[
 *     (\mathbf{I}-\mathbf{TMS})\phi = \mathbf{T}q \, ,
 * \f]
 * where \f$ \mathbf{T} = D\mathbf{L}^{-1} \f$ is the sweeping operator with
 * moment contributions added implicitly, and where the Krylov vectors are
 * energy-dependent.
 *
 * By default, only the energy block in which upscatter occurs is
 * solved via Krylov methods.  Because Gauss-Seidel is exact for
 * downscatter, it is used for the downscatter-only block.  The user can
 * switch this using "outer_upscatter_cutoff".
 *
 * Reference:
 *   Evans, T., Davidson, G. and Mosher, S. "Parallel Algorithms for
 *   Fixed-Source and Eigenvalue Problems", NSTD Seminar (ORNL), May 27, 2010.
 *
 * \todo Consider better ways to handle memory between Vec and vector.  May
 *       want to devise a moment container based on pointer that allows
 *       one to swap memory temporarily (as done with Vec)
 *
 */
//---------------------------------------------------------------------------//

template <class D>
class KrylovMG: public MultigroupSolver<D>
{

public:

  //-------------------------------------------------------------------------//
  // TYPEDEFS
  //-------------------------------------------------------------------------//

  typedef detran_utilities::SP<KrylovMG<D> >        SP_solver;
  typedef MultigroupSolver<D>                       Base;
  typedef typename Base::SP_solver                  SP_base;
  typedef typename Base::SP_inner                   SP_inner;
  typedef typename Base::SP_input                   SP_input;
  typedef typename Base::SP_state                   SP_state;
  typedef typename Base::SP_mesh                    SP_mesh;
  typedef typename Base::SP_material                SP_material;
  typedef typename Base::SP_quadrature              SP_quadrature;
  typedef typename Base::SP_boundary                SP_boundary;
  typedef typename Base::SP_externalsource          SP_externalsource;
  typedef typename Base::SP_fissionsource           SP_fissionsource;
  typedef typename Sweeper<D>::SP_sweeper           SP_sweeper;
  typedef typename SweepSource<D>::SP_sweepsource   SP_sweepsource;
  typedef PreconditionerMG::SP_pc                   SP_pc;
  typedef detran_utilities::vec_dbl                 vec_dbl;

  //-------------------------------------------------------------------------//
  // CONSTRUCTOR & DESTRUCTOR
  //-------------------------------------------------------------------------//

  /*!
   *  \brief Constructor
   *
   *  \param input             Input database.
   *  \param state             State vectors, etc.
   *  \param mesh              Problem mesh.
   *  \param mat               Material definitions.
   *  \param quadrature        Angular mesh.
   *  \param boundary          Boundary fluxes.
   *  \param external_source   User-defined external source.
   *  \param fission_source    Fission source.
   */
  KrylovMG(SP_input           input,
           SP_state           state,
           SP_mesh            mesh,
           SP_material        material,
           SP_quadrature      quadrature,
           SP_boundary        boundary,
           SP_externalsource  q_e,
           SP_fissionsource   q_f);

  /// Destructor
  ~KrylovMG()
  {
    KSPDestroy(&d_solver);
    MatDestroy(&d_operator);
    VecDestroy(&d_X);
    VecDestroy(&d_B);
  }

  /*!
   *  \brief SP Constructor.
   */
  static SP_solver
  Create(SP_input           input,
         SP_state           state,
         SP_mesh            mesh,
         SP_material        material,
         SP_quadrature      quadrature,
         SP_boundary        boundary,
         SP_externalsource  q_e,
         SP_fissionsource   q_f)
  {
    SP_solver p(new KrylovMG(input, state, mesh, material,
                             quadrature, boundary, q_e, q_f));
    return p;
  }

  //-------------------------------------------------------------------------//
  // ABSTRACT INTERFACE -- ALL MULTIGROUP SOLVERS MUST IMPLEMENT
  //-------------------------------------------------------------------------//

  /// Solve the multigroup equations.
  void solve();

private:

  //-------------------------------------------------------------------------//
  // DATA
  //-------------------------------------------------------------------------//

  // Expose base members.
  using Base::d_input;
  using Base::d_state;
  using Base::d_mesh;
  using Base::d_material;
  using Base::d_quadrature;
  using Base::d_boundary;
  using Base::d_external_source;
  using Base::d_fissionsource;
  using Base::d_downscatter;
  using Base::d_number_groups;
  using Base::d_max_iters;
  using Base::d_tolerance;
  using Base::d_print_out;
  using Base::d_print_interval;
  using Base::d_inner_solver;

  /// Linear solver
  KSP d_solver;

  /// Operator "A" in "Ax = b"
  Mat d_operator;

  /// Solution vector
  Vec d_X;

  /// Right hand side
  Vec d_B;

  /// Size of the moments portion of d_X
  int d_moments_size;

  /// Size of the moments portion of d_X in a group
  int d_moments_size_group;

  /// Size of the boundary portion of d_X
  int d_boundary_size;

  /// Size of the boundary portion of d_X in a group
  int d_boundary_size_group;

  /*!
   *  \brief Only groups equal to or above this cutoff are
   *         subject to upscatter iterations.
   *
   *  While \ref Material has an upscatter cutoff that it computes
   *  internally based on the data, the user can set this cutoff
   *  to a different value for solving.  By default, the solver
   *  cutoff is equal to the \ref Material cutoff, and so those
   *  groups into which no upscatter occurs are solved by
   *  Gauss-Seidel, and the remaining groups are solved by the
   *  PETSc solver selected.  The user can set the cutoff to
   *  zero to use the PETSc solver for the entire energy spectrum,
   *  or any value in between zero through the actual cutoff.
   *  The user \e cannot set the cutoff any higher than the \ref
   *  Material cutoff, since that would be a different problem.
   *
   */
  int d_upscatter_cutoff;

  /// Upscatter block size (number of groups in Krylov solve)
  int d_upscatter_size;

  /// Count of reflective solve iterations
  int d_reflective_solve_iterations;

  /// Sweeper
  SP_sweeper d_sweeper;

  /// Sweep source
  SP_sweepsource d_sweepsource;

  /// Preconditioner
  SP_pc d_pc;

  /// Preconditioner flag
  bool d_use_pc;

  //-------------------------------------------------------------------------//
  // IMPLEMENTATION
  //-------------------------------------------------------------------------//

  /// Set the templated operator function.
  PetscErrorCode set_operation();

  /// Build the right hand side.
  void build_rhs(State::vec_moments_type &B);

  //---------------------------------------------------------------------------//
  /*!
   * \brief A matrix-vector shell for the within-group transport operator.
   *
   * This is called by thin wrappers, since PETSc needs the matrix-vector
   * operation as a function pointer, which precludes a member function.
   *
   * \param   A       PETSc shell matrix
   * \param   x       Incoming PETSc vector
   * \param   y       Outgoing PETSc vector
   */
public:
  PetscErrorCode apply_MGTO(Mat A, Vec x, Vec y);

};

} // namespace detran

//---------------------------------------------------------------------------//
// INLINE FUNCTIONS
//---------------------------------------------------------------------------//

#include "KrylovMG.i.hh"

//---------------------------------------------------------------------------//
// EXTERNAL WRAPPER FUNCTIONS
//---------------------------------------------------------------------------//

PetscErrorCode apply_MGTO_1D(Mat A, Vec x, Vec y);
PetscErrorCode apply_MGTO_2D(Mat A, Vec x, Vec y);
PetscErrorCode apply_MGTO_3D(Mat A, Vec x, Vec y);

#endif /* KRYLOVMG_HH_ */

//---------------------------------------------------------------------------//
//              end of KrylovMG.hh
//---------------------------------------------------------------------------//
