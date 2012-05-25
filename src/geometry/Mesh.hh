//----------------------------------*-C++-*----------------------------------//
/*!
 *  \file   Mesh.hh
 *  \author Jeremy Roberts
 *  \brief  Mesh class definition.
 */
//---------------------------------------------------------------------------//

#ifndef MESH_HH_
#define MESH_HH_

// Other libtran headers
#include "Definitions.hh"
#include "DBC.hh"
#include "SP.hh"
#include "Warning.hh"

// System headers
#include  <map>

namespace detran
{




//---------------------------------------------------------------------------//
/*!
 *  \class Mesh
 *  \brief Abstract Cartesian mesh class.
 *
 *  Note, the constructors are protected to forbid direct instantiation of
 *  the Mesh class.  Rather, use the dimension-specific subclasses.  We
 *  could use a pure virtual destructor as an alternative.
 *
 */
//---------------------------------------------------------------------------//
class Mesh : public Object
{

public:

  /// Useful enumerations
  enum SIDES
  {
    LEFT,
    RIGHT,
    BOTTOM,
    TOP,
    SOUTH,
    NORTH,
    END_SIDES
  };

  enum FACE2D
  {
    VERT, HORZ
  };

  enum FACE3D
  {
    YZ, XZ, XY
  };

  typedef SP<Mesh>                          SP_mesh;
  typedef std::map<std::string, vec_int>    mesh_map_type;

  // Note, these constructors are protected to hide them from the
  // user.  These are to be called by inherited classes.  I keep
  // the constructors at the top for convention.
protected:

  /*!
   *  \brief Constructor.
   *
   *  \param    dim         Spatial dimension
   *  \param    xfm         Fine meshes per coarse mesh in x dimension.
   *  \param    yfm         Fine meshes per coarse mesh in y dimension.
   *  \param    zfm         Fine meshes per coarse mesh in z dimension.
   *  \param    xcme        Coarse mesh edges x dimension.
   *  \param    ycme        Coarse mesh edges y dimension.
   *  \param    zcme        Coarse mesh edges z dimension.
   */
  Mesh(int dim,
       vec_int xfm,  vec_int yfm,  vec_int zfm,
       vec_dbl xcme, vec_dbl ycme, vec_dbl zcme,
       vec_int mat_map);

  /*!
   *  \brief Constructor.
   *
   *  \param    dim         Spatial dimension
   *  \param    xfme        Fine mesh edges x dimension.
   *  \param    yfme        Fine mesh edges y dimension.
   *  \param    zfme        Fine mesh edges z dimension.
   */
  Mesh(int dim,
       vec_dbl xfme, vec_dbl yfme, vec_dbl zfme,
       vec_int mat_map);

  Mesh(int dim) : d_dimension(dim) {}

public:

  //------------------------------------------------------------------------//
  // Setters
  //------------------------------------------------------------------------//

  /*!
   * \brief  Add map of coarse mesh integer properties.
   *
   * This is an easy way to set mesh properties for meshes based on
   * simple coarse mesh regions.
   *
   * \param  map_key   String description of map.
   * \param  mesh_map  Logically multi-dimensional map as 1-d vector.
   */
  void add_coarse_mesh_map(std::string map_key, vec_int mesh_map);

  /*!
   * \brief  Add map of fine mesh integer properties.
   *
   * This adds properties for fine meshes directly, and so is meanty for
   * use with higher level mesh construction, e.g. pin cells, where
   * assignment is not possible by simple coarse mesh bounds.
   *
   * \note If the key exists, this function overwrites the map.
   *
   * \param  map_key   String description of map.
   * \param  mesh_map  Logically multi-dimensional map as 1-d vector.
   */
  void add_mesh_map(std::string map_key, vec_int mesh_map);

  //------------------------------------------------------------------------//
  // Getters
  //------------------------------------------------------------------------//

  /// Return total number of cells.
  int number_cells()
  {
    return d_number_cells;
  }

  /// Return number of cells in specified dimension.
  int number_cells(int dim)
  {
    Require(dim >= 0);
    Require(dim < d_dimension);
    if (dim == 0)
      return d_number_cells_x;
    else if (dim == 1)
      return d_number_cells_y;
    else
      return d_number_cells_z;
  }

  int number_cells_x()
  {
    return d_number_cells_x;
  }

  int number_cells_y()
  {
    return d_number_cells_y;
  }

  int number_cells_z()
  {
    return d_number_cells_z;
  }

  double width(int dim, int ijk)
  {
    Require(dim >= 0);
    Require(dim < d_dimension);
    if (dim == 0)
      return dx(ijk);
    else if (dim == 1)
      return dy(ijk);
    else
      return dz(ijk);
  }

  double dx(int i)
  {
    Require (i >= 0);
    Require (i < d_number_cells_x);
    return d_dx[i];
  }

  double dy(int j)
  {
    Require (j >= 0);
    Require (j < d_number_cells_y);
    return d_dy[j];
  }

  double dz(int k)
  {
    Require (k >= 0);
    Require (k < d_number_cells_z);
    return d_dz[k];
  }

  int dimension()
  {
    return d_dimension;
  }

  /*!
   *
   * \brief   Returns the cardinal index for i, j, and k
   *
   * \param   i  Index along x axis.
   * \param   j  Index along y axis.
   * \param   k  Index along z axis.
   * \return     Cardinal index.
   */
  int index(int i, int j = 0, int k = 0)
  {
    Require(i >= 0);
    Require(i < d_number_cells_x);
    Require(j >= 0);
    Require(j < d_number_cells_y);
    Require(k >= 0);
    Require(k < d_number_cells_z);
    return i + j * d_number_cells_x + k * d_number_cells_x * d_number_cells_y;
  }

  /// Check if fine mesh map exists.
  bool mesh_map_exists(std::string map_key);

  /*!
   * \brief  Get map of fine mesh integer properties.
   *
   * This adds properties for fine meshes directly, and so is meant for
   * use with higher level mesh construction, e.g. pin cells, where
   * assignment is not possible by simple coarse mesh bounds.
   *
   * \param   m  Logically multi-dimensional map as 1-d vector.
   */
  const vec_int& mesh_map(std::string map_key);

  /// Display some key features
  void display() const;

  /// Unimplemented DBC function.
  bool is_valid() const
  {
    return true;
  }

protected:

  /// x fine meshes in each x coarse mesh
  vec_int d_xfm;

  /// y fine meshes in each y coarse mesh
  vec_int d_yfm;

  /// z fine meshes in each y coarse mesh
  vec_int d_zfm;

  /// x coarse mesh edges
  vec_dbl d_xcme;

  /// y coarse mesh edges
  vec_dbl d_ycme;

  /// z coarse mesh edges
  vec_dbl d_zcme;

  /// x widths
  vec_dbl d_dx;

  /// y widths
  vec_dbl d_dy;

  /// z widths
  vec_dbl d_dz;

  /// Total number of cells
  int d_number_cells;

  /// Number of cells in x direction
  int d_number_cells_x;

  /// Number of cells in y direction
  int d_number_cells_y;

  /// Number of cells in y direction
  int d_number_cells_z;

  /*!
   *  Map container containing a key describing a mesh property and a fine
   *  mesh map defining the property in each cell.  These properties
   *  include materials, coarse mesh regions (pins, assembly, fuel,
   *  moderator, etc.), and anything else the user wants to edit.
   */
  mesh_map_type d_mesh_map;

  int d_dimension;

private:

  /*!
   *  \brief   Common construction tasks.
   */
  void setup();

};

} // end namespace detran

//---------------------------------------------------------------------------//
// INLINE FUNCTIONS
//---------------------------------------------------------------------------//

//#include "Mesh.i.hh"

#endif /* MESH_HH_ */

//---------------------------------------------------------------------------//
//              end of Mesh.hh
//---------------------------------------------------------------------------//
