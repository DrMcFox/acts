// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///  Boost include(s)
#define BOOST_TEST_MODULE Navigator Tests

#include <boost/test/included/unit_test.hpp>
// leave blank line

#include <boost/test/data/test_case.hpp>
// leave blank line

#include <boost/test/output_test_stream.hpp>
// leave blank line

#include <memory>
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Extrapolator/Navigator.hpp"
#include "Acts/Propagator/detail/ConstrainedStep.hpp"
#include "Acts/Surfaces/CylinderSurface.hpp"
#include "Acts/Tests/CommonHelpers/CylindricalTrackingGeometry.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Intersection.hpp"
#include "Acts/Utilities/Units.hpp"

namespace bdata = boost::unit_test::data;
namespace tt    = boost::test_tools;

namespace Acts {

using VectorHelpers::perp;

namespace Test {

  /// This is a simple cache struct to mimic the
  /// Propagator cache
  struct PropagatorState
  {

    /// This is a simple cache struct to mimic the
    /// Stepper cache in the propagation
    struct StepperState
    {

      /// Access method to position
      const Vector3D&
      position() const
      {
        return pos;
      }

      /// Access method to momentum
      const Vector3D&
      momentum() const
      {
        return dir;
      }

      /// Access method to direction
      const Vector3D&
      direction() const
      {
        return dir;
      }

      /// Return a corrector
      VoidIntersectionCorrector
      corrector() const
      {
        return VoidIntersectionCorrector();
      }

      /// Position
      Vector3D pos = Vector3D(0., 0., 0.);

      /// and mumentum
      Vector3D dir = Vector3D(1., 0., 0.);

      /// the navigation direction
      NavigationDirection navDir = forward;

      // accummulated path length cache
      double pathAccumulated = 0.;

      // adaptive sep size of the runge-kutta integration
      Cstep stepSize = Cstep(100 * units::_cm);
    };

    /// emulate the options template
    struct Options
    {
      /// Debug output
      /// the string where debug messages are stored (optionally)
      bool        debug       = false;
      std::string debugString = "";
      /// buffer & formatting for consistent output
      size_t debugPfxWidth = 30;
      size_t debugMsgWidth = 50;
    };

    /// Navigation cache: the start surface
    const Surface* startSurface = nullptr;

    /// Navigation cache: the current surface
    const Surface* currentSurface = nullptr;

    /// Navigation cache: the target surface
    const Surface* targetSurface = nullptr;
    bool           targetReached = false;

    /// Give some options
    Options options;

    /// The Stepper state - internal statew of the Stepper
    StepperState stepping;

    /// Navigation state - internal state of the Navigator
    Navigator::state_type navigation;
  };

  template <typename stepper_state_t>
  void
  step(stepper_state_t& sstate)
  {
    // update the cache position
    sstate.pos = sstate.pos + sstate.stepSize * sstate.dir;
    // create navigation parameters
    return;
  }

  // the surface cache & the creation of the geometry

  CylindricalTrackingGeometry cGeometry;
  auto                        tGeometry = cGeometry();

  // the debug boolean
  bool debug = true;

  BOOST_AUTO_TEST_CASE(Navigator_methods)
  {

    // create a navigator
    Navigator navigator;
    navigator.trackingGeometry = tGeometry;
    navigator.resolveSensitive = true;
    navigator.resolveMaterial  = true;
    navigator.resolvePassive   = false;

    // position and direction vector
    Vector3D position(0., 0., 0);
    Vector3D momentum(1., 1., 0);

    // the propagator cache
    PropagatorState state;
    state.options.debug = debug;

    // the stepper cache
    state.stepping.pos = position;
    state.stepping.dir = momentum.normalized();

    // foward navigation ----------------------------------------------
    if (debug) {
      std::cout << "<<<<<<<<<<<<<<<<<<<<< FORWARD NAVIGATION >>>>>>>>>>>>>>>>>>"
                << std::endl;
    }

    // (1) Initialization navigation from start point
    // - this will call resolveLayers() as well
    // - and thus should call a return to the stepper
    navigator.status(state);
    // Check that the currentVolume is set
    BOOST_TEST((state.navigation.currentVolume != nullptr));
    // Check that the currentVolume is the startVolume
    BOOST_TEST(
        (state.navigation.currentVolume == state.navigation.startVolume));
    // Check that the currentSurface is reset to:
    BOOST_TEST((state.navigation.currentSurface == nullptr));
    // No layer has been found
    BOOST_TEST((state.navigation.navLayers.size() == 0));
    // ACTORS-ABORTERS-TARGET
    navigator.target(state);
    // A layer has been found
    BOOST_TEST((state.navigation.navLayers.size() == 1));
    // The iterator should points to the begin
    BOOST_TEST(
        (state.navigation.navLayerIter == state.navigation.navLayers.begin()));
    // Cache the beam pipe radius
    double beamPipeRadius
        = perp(state.navigation.navLayerIter->intersection.position);
    // step size has been updated
    BOOST_CHECK_CLOSE_FRACTION(static_cast<double>(state.stepping.stepSize),
                               beamPipeRadius,
                               s_onSurfaceTolerance);
    if (debug) {
      std::cout << "<<< Test 1a >>> initialize at "
                << toString(state.stepping.position()) << std::endl;
      std::cout << state.options.debugString << std::endl;
      // Clear the debug string for the next test
      state.options.debugString = "";
    }

    // Do the step towards the beam pipe
    step(state.stepping);

    // (2) re-entering navigator:
    // STATUS
    navigator.status(state);
    // Check that the currentVolume is the still startVolume
    BOOST_TEST(
        (state.navigation.currentVolume == state.navigation.startVolume));
    // The layer number has not changed
    BOOST_TEST((state.navigation.navLayers.size() == 1));
    // The iterator still points to the begin
    BOOST_TEST(
        (state.navigation.navLayerIter == state.navigation.navLayers.begin()));
    // ACTORS-ABORTERS-TARGET
    navigator.target(state);

    if (debug) {
      std::cout << "<<< Test 1b >>> step to the BeamPipe at  "
                << toString(state.stepping.position()) << std::endl;
      std::cout << state.options.debugString << std::endl;
      state.options.debugString = "";
    }

    // Do the step towards the boundary
    step(state.stepping);

    // (3) re-entering navigator:
    // STATUS
    navigator.status(state);
    // ACTORS-ABORTERS-TARGET
    navigator.target(state);

    if (debug) {
      std::cout << "<<< Test 1c >>> step to the Boundary at  "
                << toString(state.stepping.position()) << std::endl;
      std::cout << state.options.debugString << std::endl;
      state.options.debugString = "";
    }

    // positive return: do the step
    step(state.stepping);
    // (4) re-entering navigator:
    // STATUS
    navigator.status(state);
    // ACTORS-ABORTERS-TARGET
    navigator.target(state);

    if (debug) {
      std::cout << "<<< Test 1d >>> step to 1st layer at  "
                << toString(state.stepping.position()) << std::endl;
      std::cout << state.options.debugString << std::endl;
      state.options.debugString = "";
    }

    // Step through the surfaces on first layer
    for (size_t isf = 0; isf < 5; ++isf) {

      step(state.stepping);
      // (5-9) re-entering navigator:
      // STATUS
      navigator.status(state);
      // ACTORS-ABORTERS-TARGET
      navigator.target(state);

      if (debug) {
        std::cout << "<<< Test 1e-1i >>> step within 1st layer at  "
                  << toString(state.stepping.position()) << std::endl;
        std::cout << state.options.debugString << std::endl;
        state.options.debugString = "";
      }
    }

    // positive return: do the step
    step(state.stepping);
    // (10) re-entering navigator:
    // STATUS
    navigator.status(state);
    // ACTORS-ABORTERS-TARGET
    navigator.target(state);

    if (debug) {
      std::cout << "<<< Test 1j >>> step to 2nd layer at  "
                << toString(state.stepping.position()) << std::endl;
      std::cout << state.options.debugString << std::endl;
      state.options.debugString = "";
    }

    // Step through the surfaces on second layer
    for (size_t isf = 0; isf < 5; ++isf) {

      step(state.stepping);
      // (11-15) re-entering navigator:
      // STATUS
      navigator.status(state);
      // ACTORS-ABORTERS-TARGET
      navigator.target(state);

      if (debug) {
        std::cout << "<<< Test 1k-1o >>> step within 2nd layer at  "
                  << toString(state.stepping.position()) << std::endl;
        std::cout << state.options.debugString << std::endl;
        state.options.debugString = "";
      }
    }

    // positive return: do the step
    step(state.stepping);
    // (16) re-entering navigator:
    // STATUS
    navigator.status(state);
    // ACTORS-ABORTERS-TARGET
    navigator.target(state);

    if (debug) {
      std::cout << "<<< Test 1p >>> step to 3rd layer at  "
                << toString(state.stepping.position()) << std::endl;
      std::cout << state.options.debugString << std::endl;
      state.options.debugString = "";
    }

    // Step through the surfaces on third layer
    for (size_t isf = 0; isf < 3; ++isf) {

      step(state.stepping);
      // (17-19) re-entering navigator:
      // STATUS
      navigator.status(state);
      // ACTORS-ABORTERS-TARGET
      navigator.target(state);

      if (debug) {
        std::cout << "<<< Test 1q-1s >>> step within 3rd layer at  "
                  << toString(state.stepping.position()) << std::endl;
        std::cout << state.options.debugString << std::endl;
        state.options.debugString = "";
      }
    }

    // positive return: do the step
    step(state.stepping);
    // (20) re-entering navigator:
    // STATUS
    navigator.status(state);
    // ACTORS-ABORTERS-TARGET
    navigator.target(state);

    if (debug) {
      std::cout << "<<< Test 1t >>> step to 4th layer at  "
                << toString(state.stepping.position()) << std::endl;
      std::cout << state.options.debugString << std::endl;
      state.options.debugString = "";
    }

    // Step through the surfaces on second layer
    for (size_t isf = 0; isf < 3; ++isf) {

      step(state.stepping);
      // (21-23) re-entering navigator:
      // STATUS
      navigator.status(state);
      // ACTORS-ABORTERS-TARGET
      navigator.target(state);

      if (debug) {
        std::cout << "<<< Test 1t-1v >>> step within 4th layer at  "
                  << toString(state.stepping.position()) << std::endl;
        std::cout << state.options.debugString << std::endl;
        state.options.debugString = "";
      }
    }

    // positive return: do the step
    step(state.stepping);
    // (24) re-entering navigator:
    // STATUS
    navigator.status(state);
    // ACTORS-ABORTERS-TARGET
    navigator.target(state);

    if (debug) {
      std::cout << "<<< Test 1w >>> step to boundary at  "
                << toString(state.stepping.position()) << std::endl;
      std::cout << state.options.debugString << std::endl;
      state.options.debugString = "";
    }
  }

}  // namespace Test
}  // namespace Acts