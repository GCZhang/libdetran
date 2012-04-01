//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   mesh_fixture.hh
 * \author Jeremy Roberts
 * \date   Apr 1, 2012
 * \brief  
 * \note   Copyright (C) 2012 Jeremy Roberts. 
 */
//---------------------------------------------------------------------------//

#ifndef MESH_FIXTURE_HH_
#define MESH_FIXTURE_HH_

// Detran includes
#include "Mesh2D.hh"
#include "Mesh3D.hh"

// Detran utilities
#include "DBC.hh"

// System includes

namespace detran_test
{

typedef detran::Mesh::SP_mesh SP_mesh;

/*!
 *  \brief Create a common 2D mesh for transport tests.
 *
 *  This is a square mesh with 2x2 coarse meshes of
 *  width 10 cm sides, each with 10x10 fine meshes.
 *  The bottom left coarse mesh has material 0, while
 *  the rest have material 1.
 */
//static SP_mesh mesh_1d_fixture()


/*!
 *  \brief Create a common 2D mesh for transport tests.
 *
 *  This is a square mesh with 2x2 coarse meshes of
 *  width 10 cm sides, each with 10x10 fine meshes.
 *  The bottom left coarse mesh has material 0, while
 *  the rest have material 1.
 */
static SP_mesh mesh_2d_fixture()
{
  // Define the mesh coarse and fine meshes, and the
  // coarse mesh material map.
  detran::Mesh::vec_dbl cm(3, 0.0);
  cm[1] = 10.0;
  cm[2] = 20.0;
  detran::Mesh::vec_int fm(2, 10);
  // All maps are stored in a single dimension.  The
  // memory order follows the transpose of the physical
  // order.  Here, the entries are (i=0,j=0), (i=0,j=1),
  // etc.  Hence, all entries are 1 but the first.
  detran::Mesh::vec_int mat_map(4, 1);
  mat_map[0] = 0;

  // Create the new database.
  SP_mesh mesh;
  mesh = new detran::Mesh2D(fm, fm, cm, cm, mat_map);

  // Ensure a valid mesh.
  Ensure(mesh->is_valid());

  // Return the fixture.
  return mesh;
}

/*!
 *  \brief Create a common 3D mesh for transport tests.
 *
 *  This is a cube mesh with 2x2x2 coarse meshes of
 *  width 4 cm sides, each with 4x4 fine meshes.
 *  The bottom left coarse mesh has material 0, while
 *  the rest have material 1.
 */
static SP_mesh mesh_3d_fixture()
{
  // Define the mesh coarse and fine meshes, and the
  // coarse mesh material map.
  detran::Mesh::vec_dbl cm(3, 0.0);
  cm[1] = 4.0;
  cm[2] = 4.0;
  detran::Mesh::vec_int fm(2, 4);
  detran::Mesh::vec_int mat_map(8, 1);
  mat_map[0] = 0;

  // Create the new database.
  SP_mesh mesh;
  mesh = new detran::Mesh3D(fm, fm, fm, cm, cm, cm, mat_map);

  // Ensure a valid mesh.
  Ensure(mesh->is_valid());

  // Return the fixture.
  return mesh;
}

} // end namespace detran_test

#endif /* MESH_FIXTURE_HH_ */

//---------------------------------------------------------------------------//
//              end of mesh_fixture.hh
//---------------------------------------------------------------------------//
