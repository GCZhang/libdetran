//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   Boundary.hh
 * \author Jeremy Roberts
 * \date   Mar 24, 2012
 * \brief  Boundary class definition.
 * \note   Copyright (C) 2012 Jeremy Roberts. 
 */
//---------------------------------------------------------------------------//

#ifndef BOUNDARY_HH_
#define BOUNDARY_HH_

// Detran
#include "BoundaryCondition.hh"
#include "Quadrature.hh"
#include "Mesh.hh"
#include "Traits.hh"

// Utilities
#include "DBC.hh"
#include "Definitions.hh"
#include "InputDB.hh"
#include "SP.hh"

// System
#include <vector>

namespace detran
{

/*!
 *  \brief Boundary traits for simplify type access.
 */
template <class D>
class BoundaryTraits
{
public:
  typedef vec2_dbl value_type;
};
template <>
class BoundaryTraits<_2D>
{
public:
  typedef vec_dbl value_type;
};
template <>
class BoundaryTraits<_1D>
{
public:
  typedef double value_type;
};

//---------------------------------------------------------------------------//
/*!
 * \class Boundary
 * \brief Boundary flux container.
 *
 * Since arbitrary boundary functions are integral to the response matrix
 * method, it helps to have a really easy interface for handling boundary
 * information.  The Boundary class contains all boundary fluxes (incident
 * and outgoing) for each surface.  The exact type of a boundary flux
 * is templated. E.g. a 1d boundary flux (for an angle and group) is
 * just a double, whereas for a 3d problem, it is a 2-d array.
 *
 * Keeping the fundamental type templated should allow this to be used in
 * all the SN stuff and potentially MOC applications. 
 *
 * All boundary fluxes are stored as follows:
 * for g = 0, ng
 *   for a = 0, na
 *     for i,j,k
 *
 * Note, the angles are order by the quadrature as follows:
 * for octant = 0, no
 *   for angle = 0, na in octant
 *
 * For most applications, the order of the mu/eta/ksi within an octant
 * doesn't matter.  However, for response generation, it helps a lot
 * to have a standard ordering.  Hence, we enforce
 * for all mu
 *  for all eta
 *    for all xi
 *
 */
//---------------------------------------------------------------------------//
template <class D>
class Boundary : public Object
{

public:

  enum inout
  {
      IN,
      OUT
  };

  typedef SP<Boundary>                              SP_boundary;
  typedef InputDB::SP_input                         SP_input;
  typedef Mesh::SP_mesh                             SP_mesh;
  typedef Quadrature::SP_quadrature                 SP_quadrature;
  typedef typename BoundaryTraits<D>::value_type    boundary_flux_type;
  typedef typename BoundaryCondition<D>::SP_bc      SP_bc;
  typedef std::vector<boundary_flux_type>           vec_boundary_flux;
  typedef std::vector<vec_boundary_flux>            vec2_boundary_flux;
  typedef std::vector<vec2_boundary_flux>           vec3_boundary_flux;

  /*!
   *  \brief Constructor.
   *
   *  \param    input       User input database.
   *  \param    mesh        Cartesian mesh.
   *  \param    quadrature  Angular quadrature.
   */
  Boundary(SP_input        input,
           SP_mesh         mesh,
           SP_quadrature   quadrature);

  /*!
   *  \brief SP Constructor.
   *
   *  \param    input       User input database.
   *  \param    mesh        Cartesian mesh.
   *  \param    quadrature  Angular quadrature.
   */
  static SP<detran::Boundary<D> >
  Create(SP<detran::InputDB>       input,
         SP<detran::Mesh>          mesh,
         SP<detran::Quadrature>    quadrature)
  {
    SP_boundary p;
    p = new Boundary(input, mesh, quadrature);
    return p;
  }

  /// \name Boundary Flux Access
  /// \{

  /*
   *  \brief Const access to a boundary flux using cardinal indices.
   *
   *  This (and the mutable version) interface is for use
   *  in sweeping, where octants and angles are cycled. 
   *
   *  \param    side  Side index.
   *  \param    o     Octant index.
   *  \param    a     Angle index (within octant).
   *  \param    g     Energy group.
   *  \return         Constant reference to boundary flux.
   */
  const boundary_flux_type& 
  operator()(int side, int o, int a, int g) const;

  /*
   *  \brief Mutable access to boundary flux using cardinal indices.
   */
  boundary_flux_type&
  operator()(int side, int o, int a, int g);

  /*
   *  \brief Const access to ordered incident flux.
   */
  const boundary_flux_type&
  incident(int side, int angle, int g) const;

  /*
   *  \brief Mutable access to incident boundary flux.
   */
  boundary_flux_type&
  incident(int side, int angle, int g);

  /*
   *  \brief Const access to ordered outgoing flux.
   */
  const boundary_flux_type&
  outgoing(int side, int angle, int g) const;

  /*
   *  \brief Mutable access to outgoing boundary flux.
   */
  boundary_flux_type&
  outgoing(int side, int angle, int g);

  /// \}

  /*!
   *  \brief Set the boundaries for a within-group solve.
   *
   *  This sets any boundaries that must be fixed for
   *  a solve, such as any fixed boundary source.
   *
   *  \param  g   Group of current solve
   */
  void set(int g);

  /*!
   *  \brief Update the boundaries for each sweep.
   *
   *  This updates all incident boundary fluxes
   *  using the current outgoing boundary fluxes
   *  in a group.  What happens in the update is
   *  a function of the underlying boundary
   *  condition.
   *
   *  \param  g   Group of current solve
   */
  void update(int g);

  /*!
   *  \brief Update the boundaries for a single angle.
   *
   *  This is an alternative update that only updates
   *  the incident boundary flux for a particular
   *  angle.  When called within a sweep, this allows
   *  the most recent boundary fluxes to be used,
   *  in effect producing a Gauss-Seidel iteration.
   *
   *  \note This cannot be used for Krylov solvers.
   *
   *  \param  g   Group of current solve
   *  \param  o   Octant being swept
   *  \param  a   Angle within octant being swept
   */
  void update(int g, int o, int a);

  /*!
   *  \brief Clear the boundary container for a group.
   *
   *  In some cases, a client might require homogeneous
   *  boundaries, perhaps after a fixed boundary has
   *  been used to construct a right hand side for a
   *  Krylov solve.
   *
   *  \param  g   Group of current solve
   */
  void clear(int g);

  /*!
   *  \brief  Map the local index to cardinal index.
   *
   *  In some cases, we need the boundary in its local order,
   *  e.g. left to right in space and angle.
   *
   *  \param    angle
   *
   */
  virtual int ordered_angle(int side, int angle, int inout) const;

  /// \name Getters
  /// \{

  /// Return the input.
  SP_input get_input() const
  {
    return d_input;
  }

  /// Return the mesh.
  SP_mesh get_mesh() const
  {
    return d_mesh;
  }

  /// Return the quadratur.
  SP_quadrature get_quadrature() const
  {
    return d_quadrature;
  }

  /// Number of boundary flux values in a group on a side
  int boundary_flux_size(int side) const
  {
    return d_boundary_flux_size[side];
  }

  /// \}

  void set_incident(int g, double *v);

  void get_incident(int g, double *v);

  /// Does the boundary have any reflective conditions?
  bool has_reflective() const
  {
    return d_has_reflective;
  }

  /// Is a side reflective?
  bool is_reflective(int side) const
  {
    return d_is_reflective[side];
  }

  bool is_valid() const
  {
    return true;
  }

private:

  /// \name Data
  /// \{

  /// Input database
  SP_input d_input;

  /// Mesh
  SP_mesh d_mesh;

  /// Angular quadrature
  SP_quadrature d_quadrature;

  /// Number of energy groups
  int d_number_groups;

  /// Boundary flux (side, energy, angle, space)
  vec3_boundary_flux d_boundary_flux;

  /// Vector of boundary conditions.
  std::vector<SP_bc> d_bc;

  /// Do I have any reflective conditions?
  bool d_has_reflective;

  std::vector<bool> d_is_reflective;

  /// Size of the boundary flux on a side in one group.
  vec_int d_boundary_flux_size;

  /// \}

  /// \name Implementation
  /// \{

  /// Sizes the boundary flux containers.
  void initialize();

  /// \}

};

} // end namespace detran

//---------------------------------------------------------------------------//
// INLINE FUNCTIONS
//---------------------------------------------------------------------------//

#include "Boundary.i.hh"

#endif /* BOUNDARY_HH_ */

//---------------------------------------------------------------------------//
//              end of Boundary.hh
//---------------------------------------------------------------------------//
