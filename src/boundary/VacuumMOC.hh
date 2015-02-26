//----------------------------------*-C++-*----------------------------------//
/**
 *  @file   VacuumMOC.hh
 *  @brief  VacuumMOC class definition.
 *  @author Jeremy Roberts
 *  @date   Jun 26, 2012
 */
//---------------------------------------------------------------------------//

#ifndef detran_VACUUMMOC_HH_
#define detran_VACUUMMOC_HH_

#include "BoundaryConditionMOC.hh"

namespace detran
{

//---------------------------------------------------------------------------//
/**
 *  @class VacuumMOC
 *  @brief Vacuum boundary condition for MOC
 */
//---------------------------------------------------------------------------//

template <class D>
class VacuumMOC : public BoundaryConditionMOC<D>
{

public:

  //-------------------------------------------------------------------------//
  // TYPEDEFS
  //-------------------------------------------------------------------------//

  typedef BoundaryConditionMOC<D>             Base;
  typedef typename Base::SP_bc                SP_bc;
  typedef typename Base::Boundary_T           Boundary_T;
  typedef typename Base::SP_input             SP_input;
  typedef typename Base::SP_geometry          SP_geometry;
  typedef typename Base::SP_trackdb           SP_trackdb;
  typedef typename Base::size_t               size_t;

  //-------------------------------------------------------------------------//
  // CONSTRUCTOR & DESTRUCTOR
  //-------------------------------------------------------------------------//

  VacuumMOC(BoundaryMOC<D>& boundary,
            const size_t side,
            SP_input input,
            SP_geometry geo,
            SP_trackdb tracks)
    : BoundaryConditionMOC<D>(boundary, side, input, geo, tracks)
  {
    /* ... */
  }

  //-------------------------------------------------------------------------//
  // ABSTRACT INTERFACE -- ALL MOC BOUNDARY CONDITIONS MUST IMPLEMENT THESE
  //-------------------------------------------------------------------------//

  /// Set initial and/or fixed boundary condition.  Vacuum does nothing.
  void set(const size_t g){}

  /// Update a boundary following a sweep.  Vacuum does nothing.
  void update(const size_t g){}

  /// Update a boundary for a given angle following a sweep. Vacuum does nothing.
  void update(const size_t g, const size_t o, const size_t a){}

private:

};


} // end namespace detran

#endif // detran_VACUUMMOC_HH_

//---------------------------------------------------------------------------//
//              end of file VacuumMOC.hh
//---------------------------------------------------------------------------//
