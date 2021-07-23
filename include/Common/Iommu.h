// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file Iommu.h
/// \brief Definition of functions related to the IOMMU
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_FLPPROTOTYPE_COMMON_IOMMU_H_
#define ALICEO2_FLPPROTOTYPE_COMMON_IOMMU_H_

namespace AliceO2
{
namespace Common
{
namespace Iommu
{

/// Checks if the IOMMU is enabled
bool isEnabled();

} // namespace Iommu
} // namespace Common
} // namespace AliceO2

#endif // ALICEO2_FLPPROTOTYPE_COMMON_IOMMU_H_

