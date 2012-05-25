//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   detran_transport.i
 * \author Jeremy Roberts
 * \brief  Python interface for detran transport.
 */
//---------------------------------------------------------------------------//

// State
%include "State.hh"
// Boundary
%include "Boundary.hh"
// Sources
%include "ExternalSource.hh"
%include "ConstantSource.hh"
%include "FissionSource.hh"
// Acceleration
//%include "Acceleration.hh"
// Trait
%include "Traits.hh"
// Equations
%include "Equation.hh"
%include "Equation_DD_1D.hh"
%include "Equation_DD_2D.hh"
%include "Equation_DD_3D.hh"
%include "Equation_SC_2D.hh"
// SweepSource
%include "SweepSource.hh"

// Sweepers
%include "Sweeper.hh"
%include "Sweeper1D.hh"
%include "Sweeper2D.hh"
%include "Sweeper3D.hh"

// Helpers
%include "ReactionRates.hh"

// Templates 

%template(StateSP)          detran::SP<detran::State>;

%template(Boundary1D)       detran::Boundary<detran::_1D>;
%template(Boundary1DSP)     detran::SP<detran::Boundary<detran::_1D> >;
%template(Boundary2D)       detran::Boundary<detran::_2D>;
%template(Boundary2DSP)     detran::SP<detran::Boundary<detran::_2D> >;
%template(Boundary3D)       detran::Boundary<detran::_3D>;
%template(Boundary3DSP)     detran::SP<detran::Boundary<detran::_3D> >;

%template(ExternalSourceSP) detran::SP<detran::ExternalSource>;
%template(ConstantSourceSP) detran::SP<detran::ConstantSource>;
%template(FissionSourceSP)  detran::SP<detran::FissionSource>;

%template(Equation1D)       detran::Equation<detran::_1D>;
%template(Equation2D)       detran::Equation<detran::_2D>;
%template(Equation3D)       detran::Equation<detran::_3D>;

%template(SweepSource1D)    detran::SweepSource<detran::_1D>;
%template(SweepSource2D)    detran::SweepSource<detran::_2D>;
%template(SweepSource3D)    detran::SweepSource<detran::_3D>;

//%template(Acceleration1DSP) detran::Acceleration<detran::_1D>;
//%template(Acceleration2DSP) detran::Acceleration<detran::_2D>;
//%template(Acceleration3DSP) detran::Acceleration<detran::_3D>;

// SWEEPER
//%template(Sweeper1D_T)      detran::Sweeper<detran::_1D>;
%template(Sweeper1D_DD)     detran::Sweeper1D<detran::Equation_DD_1D>;
%template(Sweeper2D_DD)     detran::Sweeper2D<detran::Equation_DD_2D>;
%template(Sweeper3D_DD)     detran::Sweeper3D<detran::Equation_DD_3D>;
%template(Sweeper2D_SC)     detran::Sweeper2D<detran::Equation_SC_2D>;
%template(Sweeper1D_DDSP)   detran::SP<detran::Sweeper1D<detran::Equation_DD_1D> >;
%template(Sweeper2D_DDSP)   detran::SP<detran::Sweeper2D<detran::Equation_DD_2D> >;
%template(Sweeper3D_DDSP)   detran::SP<detran::Sweeper3D<detran::Equation_DD_3D> >;
%template(Sweeper2D_SCSP)   detran::SP<detran::Sweeper2D<detran::Equation_SC_2D> >;

// Helpers
%template(ReactionRatesSP)  detran::SP<detran::ReactionRates>;

//---------------------------------------------------------------------------//
//              end of detran_transport.i
//---------------------------------------------------------------------------//
