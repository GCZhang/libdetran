//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   Reflective.i.hh
 * \author robertsj
 * \date   Apr 10, 2012
 * \brief  Reflective inline member definitions.
 */
//---------------------------------------------------------------------------//

#ifndef REFLECTIVE_I_HH_
#define REFLECTIVE_I_HH_

// System
#include <iostream>

namespace detran
{

template <class D>
void Reflective<D>::update(const size_t g)
{
  for (int o = 0; o < d_quadrature->number_octants()/2; o++)
  {
    for (int a = 0; a < d_quadrature->number_angles_octant(); a++)
    {
      // Reroute reflecting fluxes.
      d_boundary(d_side, d_octants[o][Boundary_T::IN], a, g) =
        d_boundary(d_side, d_octants[o][Boundary_T::OUT], a, g);
    }
  }
}

// \todo This is not the most elegant solution
template <class D>
void Reflective<D>::update(const size_t g, const size_t o, const size_t a)
{
  int o_out = -1;
  for (int i = 0; i < d_octants.size(); i++)
  {
    if (d_octants[i][Boundary_T::IN] == o)
    {
      o_out = d_octants[i][Boundary_T::OUT];
    }
  }
  // Only reroute fluxes if I am an outgoing side.
  if (o_out >= 0)
  {
    // Reroute reflecting fluxes.
    d_boundary(d_side, o, a, g) =
      d_boundary(d_side, o_out, a, g);
  }
}

//---------------------------------------------------------------------------//
// PRIVATE IMPLEMENTATION
//---------------------------------------------------------------------------//

// All octants are arranged to follow the right hand rule defined so
// that (+x)->(+y)->(+z)
//      (-x)->(-y)->(+z)
//
// These are the combinations for all six sides:
// +x  -->  -/+ y --> -/+ z  : 7, 4, 3, 0
// -x  -->  +/- y --> -/+ z  : 5, 6, 1, 2
// +y  -->  -/+ z --> -/+ x  : 5, 1, 4, 0
// -y  -->  +/- z --> -/+ x  : 6, 7, 2, 3
// +z  -->  -/+ x --> -/+ y  : 2, 3, 1, 0
// -z  -->  +/- x --> -/+ y  : 7, 6, 4, 5

//(x->y->z, y->z->x, z->x->y) and the normal
// right hand rule.  For example, if I'm
// incident on the left face, I am looking along the +x, my left is
// oriented along +y, and above me is +z.  If I'm incident on the south
// face, I'm looking along +z, my left is -x
template <class D>
void Reflective<D>::setup_octant()
{
  if (d_side == Mesh::LEFT)
  {
    // incident octants: 7, 4, 3, 0
    d_octants[0][0] = 7;
    d_octants[1][0] = 4;
    d_octants[2][0] = 3;
    d_octants[3][0] = 0;
    // outgoing octants: 6, 5, 2, 1
    d_octants[0][1] = 6;
    d_octants[1][1] = 5;
    d_octants[2][1] = 2;
    d_octants[3][1] = 1;
  }
  else if (d_side == Mesh::RIGHT)
  {
    // incident octants: 5, 6, 1, 2
    d_octants[0][0] = 5;
    d_octants[1][0] = 6;
    d_octants[2][0] = 1;
    d_octants[3][0] = 2;
    // outgoing octants: 4, 7, 0, 3
    d_octants[0][1] = 4;
    d_octants[1][1] = 7;
    d_octants[2][1] = 0;
    d_octants[3][1] = 3;
  }
  else if (d_side == Mesh::BOTTOM)
  {
    // incident octants: 5, 1, 4, 0
    d_octants[0][0] = 5;
    d_octants[1][0] = 1;
    d_octants[2][0] = 4;
    d_octants[3][0] = 0;
    // outgoing octants: 6, 2, 7, 3
    d_octants[0][1] = 6;
    d_octants[1][1] = 2;
    d_octants[2][1] = 7;
    d_octants[3][1] = 3;
  }
  else if (d_side == Mesh::TOP)
  {
    // incident octants: 6, 7, 2, 3
    d_octants[0][0] = 6;
    d_octants[1][0] = 7;
    d_octants[2][0] = 2;
    d_octants[3][0] = 3;
    // outgoing octants: 5, 4, 1, 0
    d_octants[0][1] = 5;
    d_octants[1][1] = 4;
    d_octants[2][1] = 1;
    d_octants[3][1] = 0;
  }
  else if (d_side == Mesh::SOUTH)
  {
    // incident octants: 2, 3, 1, 0
    d_octants[0][0] = 2;
    d_octants[1][0] = 3;
    d_octants[2][0] = 1;
    d_octants[3][0] = 0;
    // outgoing octants: 6, 7, 5, 4
    d_octants[0][1] = 6;
    d_octants[1][1] = 7;
    d_octants[2][1] = 5;
    d_octants[3][1] = 4;
  }
  else
  {
    // incident octants: 7, 6, 4, 5
    d_octants[0][0] = 7;
    d_octants[1][0] = 6;
    d_octants[2][0] = 4;
    d_octants[3][0] = 5;
    // outgoing octants: 3, 2, 0, 1
    d_octants[0][1] = 3;
    d_octants[1][1] = 2;
    d_octants[2][1] = 0;
    d_octants[3][1] = 1;
  }
}

template <>
void Reflective<_2D>::setup_octant()
{
  Require(d_side == Mesh::LEFT   || d_side == Mesh::RIGHT ||
          d_side == Mesh::BOTTOM || d_side == Mesh::TOP   );
  if (d_side == Mesh::LEFT)
  {
    d_octants[0][0] = 0; // [left/right oct][in/out]
    d_octants[1][0] = 3;
    d_octants[0][1] = 1;
    d_octants[1][1] = 2;
  }
  else if (d_side == Mesh::RIGHT)
  {
    d_octants[0][0] = 2;
    d_octants[1][0] = 1;
    d_octants[0][1] = 3;
    d_octants[1][1] = 0;
  }
  else if (d_side == Mesh::BOTTOM)
  {
    d_octants[0][0] = 1;
    d_octants[1][0] = 0;
    d_octants[0][1] = 2;
    d_octants[1][1] = 3;
  }
  else // TOP
  {
    d_octants[0][0] = 3;
    d_octants[1][0] = 2;
    d_octants[0][1] = 0;
    d_octants[1][1] = 1;
  }
}


template <>
void Reflective<_1D>::setup_octant()
{
  Require(d_side == Mesh::LEFT || d_side == Mesh::RIGHT);
  if (d_side == Mesh::LEFT)
  {
    // incident octant: 0
    d_octants[0][0] = 0;
    // outgoing octant: 1
    d_octants[0][1] = 1;
  }
  else
  {
    // incident octant: 1
    d_octants[0][0] = 1;
    // outgoing octant: 0
    d_octants[0][1] = 0;
  }
}

} // end namespace detran

#endif /* REFLECTIVE_I_HH_ */
