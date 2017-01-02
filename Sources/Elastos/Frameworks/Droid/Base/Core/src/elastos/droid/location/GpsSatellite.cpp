//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/location/GpsSatellite.h"

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL(GpsSatellite, Object, IGpsSatellite)

GpsSatellite::GpsSatellite()
    : mValid(FALSE)
    , mHasEphemeris(FALSE)
    , mHasAlmanac(FALSE)
    , mUsedInFix(FALSE)
    , mPrn(0)
    , mSnr(0.0f)
    , mElevation(0.0f)
    , mAzimuth(0.0f)
{
}

ECode GpsSatellite::constructor(
    /* [in] */ Int32 prn)
{
    mPrn = prn;
    return NOERROR;
}

ECode GpsSatellite::SetStatus(
    /* [in] */ IGpsSatellite* satellite)
{
    VALIDATE_NOT_NULL(satellite);
    satellite->IsValid(&mValid);
    satellite->HasEphemeris(&mHasEphemeris);
    satellite->HasAlmanac(&mHasAlmanac);
    satellite->UsedInFix(&mUsedInFix);
    satellite->GetSnr(&mSnr);
    satellite->GetElevation(&mElevation);
    satellite->GetAzimuth(&mAzimuth);
    return NOERROR;
}

ECode GpsSatellite::GetPrn(
    /* [out] */ Int32* prn)
{
    VALIDATE_NOT_NULL(prn);
    *prn = mPrn;
    return NOERROR;
}

ECode GpsSatellite::SetPrn(
    /* [in] */ Int32 prn)
{
    mPrn = prn;
    return NOERROR;
}

ECode GpsSatellite::GetSnr(
    /* [out] */ Float* snr)
{
    VALIDATE_NOT_NULL(snr);
    *snr = mSnr;
    return NOERROR;
}

ECode GpsSatellite::SetSnr(
    /* [in] */ Float snr)
{
    mSnr = snr;
    return NOERROR;
}

ECode GpsSatellite::GetElevation(
    /* [out] */ Float* elevation)
{
    VALIDATE_NOT_NULL(elevation);
    *elevation = mElevation;
    return NOERROR;
}

ECode GpsSatellite::SetElevation(
    /* [in] */ Float elevation)
{
    mElevation = elevation;
    return NOERROR;
}

ECode GpsSatellite::GetAzimuth(
    /* [out] */ Float* azimuth)
{
    VALIDATE_NOT_NULL(azimuth);
    *azimuth = mAzimuth;
    return NOERROR;
}

ECode GpsSatellite::SetAzimuth(
    /* [in] */ Float azimuth)
{
    mAzimuth = azimuth;
    return NOERROR;
}

ECode GpsSatellite::HasEphemeris(
    /* [out] */ Boolean* hasEphemeris)
{
    VALIDATE_NOT_NULL(hasEphemeris);
    *hasEphemeris = mHasEphemeris;
    return NOERROR;
}

ECode GpsSatellite::SetHasEphemeris(
    /* [in] */ Boolean hasEphemeris)
{
    mHasEphemeris = hasEphemeris;
    return NOERROR;
}

ECode GpsSatellite::HasAlmanac(
    /* [out] */ Boolean* hasAlmanac)
{
    VALIDATE_NOT_NULL(hasAlmanac);
    *hasAlmanac = mHasAlmanac;
    return NOERROR;
}


ECode GpsSatellite::SetHasAlmanac(
    /* [in] */ Boolean hasAlmanac)
{
    mHasAlmanac = hasAlmanac;
    return NOERROR;
}

ECode GpsSatellite::UsedInFix(
    /* [out] */ Boolean* usedInFix)
{
    VALIDATE_NOT_NULL(usedInFix);
    *usedInFix = mUsedInFix;
    return NOERROR;
}


ECode GpsSatellite::SetUsedInFix(
    /* [in] */ Boolean usedInFix)
{
    mUsedInFix = usedInFix;
    return NOERROR;
}

ECode GpsSatellite::IsValid(
    /* [out] */ Boolean* valid)
{
    VALIDATE_NOT_NULL(valid);
    *valid = mValid;
    return NOERROR;
}

ECode GpsSatellite::SetValid(
    /* [in] */ Boolean valid)
{
    mValid = valid;
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
