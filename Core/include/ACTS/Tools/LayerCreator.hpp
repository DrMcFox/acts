// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// LayerCreator.h, ACTS project
///////////////////////////////////////////////////////////////////

#ifndef ACTS_GEOMETRYTOOLS_LAYERCREATOR_H
#define ACTS_GEOMETRYTOOLS_LAYERCREATOR_H 1

#include "ACTS/Tools/SurfaceArrayCreator.hpp"
#include "ACTS/Utilities/ApproachDescriptor.hpp"
#include "ACTS/Utilities/BinUtility.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

#ifndef ACTS_LAYERCREATOR_TAKESMALLERBIGGER
#define ACTS_LAYERCREATOR_TAKESMALLERBIGGER
#define takeSmaller(current, test) current = current < test ? current : test
#define takeBigger(current, test) current  = current > test ? current : test
#define takeSmallerBigger(cSmallest, cBiggest, test)                           \
  takeSmaller(cSmallest, test);                                                \
  takeBigger(cBiggest, test)
#endif

namespace Acts {

namespace Test {
  struct LayerCreatorFixture;
}
class Surface;
class Layer;
typedef std::shared_ptr<Layer> MutableLayerPtr;

/// @class LayerCreator
///
/// The LayerCreator is able to build cylinde,r disc layers or plane layers from
/// detector elements
///
class LayerCreator
{
public:
  friend Acts::Test::LayerCreatorFixture;
  ///  @struct Config
  ///  Configuration for the LayerCreator
  ///  This is the nexted configuration struct for the LayerCreator class
  struct Config
  {
    /// surface array helper
    std::shared_ptr<const SurfaceArrayCreator> surfaceArrayCreator = nullptr;
    /// cylinder module z tolerance : it counts at same z, if ...
    double cylinderZtolerance;
    /// cylinder module phi tolerance : it counts at same phi, if ...
    double cylinderPhiTolerance;

    // standard constructor
    Config() : cylinderZtolerance(10.), cylinderPhiTolerance(0.1) {}
  };

  /// Constructor
  ///
  /// @param lcConfig is the configuration object
  /// @param logger logging instance
  LayerCreator(const Config&                 lcConfig,
               std::unique_ptr<const Logger> logger
               = getDefaultLogger("LayerCreator", Logging::INFO));

  /// Destructor
  ~LayerCreator() = default;

  /// returning a cylindrical layer
  ///
  /// @param surfaces is the vector of pointers to sensitive surfaces
  /// represented by this layer
  /// @pre the pointers to the sensitive surfaces in the surfaces vectors all
  /// need to be valid, since no check is performed
  /// @param envelopeR is the additional envelope applied in R
  /// @param envelopeZ is the additional envelope applied in z
  /// @param binsPhi is number of bins the sensitive surfaces are ordered in phi
  /// @param binsZ is number of bins the sensitive surfaces are ordered in Z
  /// @param transform is the (optional) transform of the layer
  /// @param ad possibility to hand over a specific ApproachDescriptor, which is
  /// needed for material mapping. Otherwise the default ApproachDescriptor will
  /// be taken used for this layer
  ///
  /// @return shared pointer to a newly created layer
  MutableLayerPtr
  cylinderLayer(const std::vector<const Surface*>&  surfaces,
                double                              envelopeR,
                double                              envelopeZ,
                size_t                              binsPhi,
                size_t                              binsZ,
                std::shared_ptr<const Transform3D>  transform = nullptr,
                std::unique_ptr<ApproachDescriptor> ad        = nullptr) const;

  /// returning a cylindrical layer
  ///
  /// @param surfaces is the vector of pointers to sensitive surfaces
  /// represented by this layer
  /// @pre the pointers to the sensitive surfaces in the surfaces vectors all
  /// need to be valid, since no check is performed
  /// @param layerRmin is the inner radius of the layer
  /// @param layerRmax is the outer radius of the layer
  /// @param layerHalfZ is the half length in z of the layer
  /// @param bTypePhi binning type in phi (equidistant/arbitrary)
  /// @param bTypeZ binning type in z (equidistant/arbitrary)
  /// @param transform is the (optional) transform of the layer
  /// @param ad possibility to hand over a specific ApproachDescriptor, which is
  /// needed for material mapping. Otherwise the default ApproachDescriptor will
  /// be taken used for this layer
  ///
  /// @return shared pointer to a newly created layer
  MutableLayerPtr
  cylinderLayer(const std::vector<const Surface*>&  surfaces,
                double                              layerRmin,
                double                              layerRmax,
                double                              layerHalfZ,
                BinningType                         bTypePhi,
                BinningType                         bTypeZ,
                std::shared_ptr<const Transform3D>  transform = nullptr,
                std::unique_ptr<ApproachDescriptor> ad        = nullptr) const;

  /// returning a cylindrical layer
  ///
  /// @param surfaces is the vector of pointers to sensitive surfaces
  /// represented by this layer
  /// @pre the pointers to the sensitive surfaces in the surfaces vectors all
  /// need to be valid, since no check is performed
  /// @param surfaces is the vector of sensitive surfaces represented by this
  /// layer
  /// @param envelopeR is the additional envelope applied in R
  /// @param envelopeZ is the additional envelope applied in z
  /// @param bTypePhi binning type in phi (equidistant/arbitrary)
  /// @param bTypeZ binning type in z (equidistant/arbitrary)
  /// @param transform is the (optional) transform of the layer
  /// @param ad possibility to hand over a specific ApproachDescriptor, which is
  /// needed for material mapping. Otherwise the default ApproachDescriptor will
  /// be taken used for this layer
  ///
  /// @return shared pointer to a newly created layer
  MutableLayerPtr
  cylinderLayer(const std::vector<const Surface*>&  surfaces,
                double                              envelopeR,
                double                              envelopeZ,
                BinningType                         bTypePhi,
                BinningType                         bTypeZ,
                std::shared_ptr<const Transform3D>  transform = nullptr,
                std::unique_ptr<ApproachDescriptor> ad        = nullptr) const;

  /// returning a cylindrical layer
  ///
  /// @param surfaces is the vector of pointers to sensitive surfaces
  /// represented by this layer
  /// @pre the pointers to the sensitive surfaces in the surfaces vectors all
  /// need to be valid, since no check is performed
  /// @param envelopeMinR is the additional envelope applied in R at Rmin
  /// @param envelopeMaxR is the additional envelope applied in R in Rmax
  /// @param envelopeZ is the additional envelope applied in z
  /// @param binsR is number of bins the sensitive surfaces are ordered in R
  /// @param binsPhi is number of bins the sensitive surfaces are ordered in Phi
  /// @param transform is the (optional) transform of the layer
  /// @param ad possibility to hand over a specific ApproachDescriptor, which is
  /// needed for material mapping. Otherwise the default ApproachDescriptor will
  /// be taken used for this layer
  ///
  /// @return shared pointer to a newly created layer
  MutableLayerPtr
  discLayer(const std::vector<const Surface*>&  surfaces,
            double                              envelopeMinR,
            double                              envelopeMaxR,
            double                              envelopeZ,
            size_t                              binsR,
            size_t                              binsPhi,
            std::shared_ptr<const Transform3D>  transform = nullptr,
            std::unique_ptr<ApproachDescriptor> ad        = nullptr) const;

  /// returning a cylindrical layer
  ///
  /// @param surfaces is the vector of pointers to sensitive surfaces
  /// represented by this layer
  /// @pre the pointers to the sensitive surfaces in the surfaces vectors all
  /// need to be valid, since no check is performed
  /// @param layerRmin is the inner radius of the layer
  /// @param layerRmax is the outer radius of the layer
  /// @param layerZmin is the minimum in z of the layer
  /// @param layerZmax is the maximum in z of the layer
  /// @param bTypeR binning type in r (equidistant/arbitrary)
  /// @param bTypePhi binning type in phi (equidistant/arbitrary)
  /// @param transform is the (optional) transform of the layer
  /// @param ad possibility to hand over a specific ApproachDescriptor, which is
  /// needed for material mapping. Otherwise the default ApproachDescriptor will
  /// be taken used for this layer
  ///
  /// @return shared pointer to a newly created layer
  MutableLayerPtr
  discLayer(const std::vector<const Surface*>&  surfaces,
            double                              layerZmin,
            double                              layerZmax,
            double                              layerRmin,
            double                              layerRmax,
            BinningType                         bTypeR,
            BinningType                         bTypePhi,
            std::shared_ptr<const Transform3D>  transform = nullptr,
            std::unique_ptr<ApproachDescriptor> ad        = nullptr) const;

  /// returning a cylindrical layer
  ///
  /// @param surfaces is the vector of pointers to sensitive surfaces
  /// represented by this layer
  /// @pre the pointers to the sensitive surfaces in the surfaces vectors all
  /// need to be valid, since no check is performed
  /// @param envelopeMinR is the additional envelope applied in R at Rmin
  /// @param envelopeMaxR is the additional envelope applied in R in Rmax
  /// @param envelopeZ is the additional envelope applied in z
  /// @param bTypeR binning type in r (equidistant/arbitrary)
  /// @param bTypePhi binning type in phi (equidistant/arbitrary)
  /// @param transform is the (optional) transform of the layer
  /// @param ad possibility to hand over a specific ApproachDescriptor, which is
  /// needed for material mapping. Otherwise the default ApproachDescriptor will
  /// be taken used for this layer
  ///
  /// @return shared pointer to a newly created layer
  MutableLayerPtr
  discLayer(const std::vector<const Surface*>&  surfaces,
            double                              envelopeMinR,
            double                              envelopeMaxR,
            double                              envelopeZ,
            BinningType                         bTypeR,
            BinningType                         bTypePhi,
            std::shared_ptr<const Transform3D>  transform = nullptr,
            std::unique_ptr<ApproachDescriptor> ad        = nullptr) const;

  /// returning a cylindrical layer
  ///
  /// @param surfaces is the vector of pointers to sensitive surfaces
  /// represented by this layer
  /// @pre the pointers to the sensitive surfaces in the surfaces vectors all
  /// need to be valid, since no check is performed
  /// @param envelopeXY is the additional envelope applied in XY
  /// @param envelopeZ is the additional envelope applied in Z
  /// @param binsX is number of bins the sensitive surfaces are ordered in X
  /// @param binsY is number of bins the sensitive surfaces are ordered in Y
  /// @param transform is the (optional) transform of the layer
  /// @param ad possibility to hand over a specific ApproachDescriptor, which is
  /// needed for material mapping. Otherwise the default ApproachDescriptor will
  /// be taken used for this layer
  ///
  /// @return shared pointer to a newly created layer
  MutableLayerPtr
  planeLayer(const std::vector<const Surface*>&  surfaces,
             double                              envelopeXY,
             double                              envelopeZ,
             size_t                              binsX,
             size_t                              binsY,
             std::shared_ptr<const Transform3D>  transform = nullptr,
             std::unique_ptr<ApproachDescriptor> ad        = nullptr) const;

  /// Set the configuration object
  /// @param lcConfig is the configuration struct
  void
  setConfiguration(const Config& lcConfig);

  /// Access th configuration object
  Config
  getConfiguration() const;

  /// set logging instance
  void
  setLogger(std::unique_ptr<const Logger> logger);

  // associate surfaces contained by this layer to this layer
  void
  associateSurfacesToLayer(Layer& layer) const;

private:
  /// Validates that all the sensitive surfaces are actually accessible through
  /// the binning
  ///
  /// @param surfGrid is the object grid from the surface array
  /// @para surfaces is the vector of sensitive surfaces
  bool
  checkBinning(const SurfaceArray& sArray) const;

  /// configuration object
  Config m_cfg;

  /// Private acces method to the logger
  const Logger&
  logger() const
  {
    return *m_logger;
  }

  /// logging instance
  std::unique_ptr<const Logger> m_logger;
};

inline LayerCreator::Config
LayerCreator::getConfiguration() const
{
  return m_cfg;
}

}  // namespace Acts

#endif  // ACTS_GEOMETRYTOOLS_LAYERCREATOR_H
