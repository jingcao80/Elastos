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

#include "elastos/droid/server/location/GpsStatusListenerHelper.h"

using Elastos::Droid::Location::IIGpsStatusListener;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//====================================
//GpsStatusListenerHelper::Operation1
//====================================

CAR_INTERFACE_IMPL(GpsStatusListenerHelper::Operation1, Object, IListenerOperation)

GpsStatusListenerHelper::Operation1::Operation1(
    /* [in] */ Int32 timeToFirstFix)
    : mTimeToFirstFix(timeToFirstFix)
{}

ECode GpsStatusListenerHelper::Operation1::Execute(
    /* [in] */ IInterface* listener)
{
    AutoPtr<IIGpsStatusListener> gsListener = IIGpsStatusListener::Probe(listener);
    gsListener->OnFirstFix(mTimeToFirstFix);
    return NOERROR;
}

//====================================
//GpsStatusListenerHelper::Operation2
//====================================
CAR_INTERFACE_IMPL(GpsStatusListenerHelper::Operation2, Object, IListenerOperation)

GpsStatusListenerHelper::Operation2::Operation2(
    /* [in] */ Boolean isNavigating)
    : mIsNavigating(isNavigating)
{}

ECode GpsStatusListenerHelper::Operation2::Execute(
    /* [in] */ IInterface* listener)
{
    AutoPtr<IIGpsStatusListener> gsListener = IIGpsStatusListener::Probe(listener);
    if (mIsNavigating) {
        gsListener->OnGpsStarted();
    } else {
        gsListener->OnGpsStopped();
    }
    return NOERROR;
}

//====================================
//GpsStatusListenerHelper::Operation3
//====================================
CAR_INTERFACE_IMPL(GpsStatusListenerHelper::Operation3, Object, IListenerOperation)

GpsStatusListenerHelper::Operation3::Operation3(
/* [in] */ Int32 svCount,
/* [in] */ ArrayOf<Int32>* prns,
/* [in] */ ArrayOf<Float>* snrs,
/* [in] */ ArrayOf<Float>* elevations,
/* [in] */ ArrayOf<Float>* azimuths,
/* [in] */ Int32 ephemerisMask,
/* [in] */ Int32 almanacMask,
/* [in] */ Int32 usedInFixMask)
    : mSvCount(svCount)
    , mPrns(prns)
    , mSnrs(snrs)
    , mElevations(elevations)
    , mAzimuths(azimuths)
    , mEphemerisMask(ephemerisMask)
    , mAlmanacMask(almanacMask)
    , mUsedInFixMask(usedInFixMask)
{}

ECode GpsStatusListenerHelper::Operation3::Execute(
    /* [in] */ IInterface* listener)
{
    AutoPtr<IIGpsStatusListener> gsListener = IIGpsStatusListener::Probe(listener);
    gsListener->OnSvStatusChanged(
        mSvCount,
        mPrns,
        mSnrs,
        mElevations,
        mAzimuths,
        mEphemerisMask,
        mAlmanacMask,
        mUsedInFixMask);
    return NOERROR;
}

//====================================
//GpsStatusListenerHelper::Operation4
//====================================
CAR_INTERFACE_IMPL(GpsStatusListenerHelper::Operation4, Object, IListenerOperation)

GpsStatusListenerHelper::Operation4::Operation4(
    /* [in] */ Int64 timestamp,
    /* [in] */ const String& nmea)
    : mTimestamp(timestamp)
    , mNmea(nmea)
{}

ECode GpsStatusListenerHelper::Operation4::Execute(
    /* [in] */ IInterface* listener)
{
    AutoPtr<IIGpsStatusListener> gsListener = IIGpsStatusListener::Probe(listener);
    gsListener->OnNmeaReceived(mTimestamp, mNmea);
    return NOERROR;
}

//====================================
//GpsStatusListenerHelper
//====================================

GpsStatusListenerHelper::GpsStatusListenerHelper()
    : RemoteListenerHelper(String("GpsStatusListenerHelper"))
{}

ECode GpsStatusListenerHelper::OnFirstFix(
    /* [in] */ Int32 timeToFirstFix)
{
    AutoPtr<Operation1> operation = new Operation1(timeToFirstFix);
    RemoteListenerHelper::Foreach(operation.Get());
    return NOERROR;
}

ECode GpsStatusListenerHelper::OnStatusChanged(
    /* [in] */ Boolean isNavigating)
{
    AutoPtr<Operation2> operation = new Operation2(isNavigating);
    RemoteListenerHelper::Foreach(operation.Get());
    return NOERROR;
}

ECode GpsStatusListenerHelper::OnSvStatusChanged(
    /* [in] */ Int32 svCount,
    /* [in] */ ArrayOf<Int32>* prns,
    /* [in] */ ArrayOf<Float>* snrs,
    /* [in] */ ArrayOf<Float>* elevations,
    /* [in] */ ArrayOf<Float>* azimuths,
    /* [in] */ Int32 ephemerisMask,
    /* [in] */ Int32 almanacMask,
    /* [in] */ Int32 usedInFixMask)
{
    AutoPtr<Operation3> operation = new Operation3(
        svCount, prns, snrs, elevations, azimuths, ephemerisMask, almanacMask, usedInFixMask);
    RemoteListenerHelper::Foreach(operation.Get());
    return NOERROR;
}

ECode GpsStatusListenerHelper::OnNmeaReceived(
    /* [in] */ Int64 timestamp,
    /* [in] */ const String& nmea)
{
    AutoPtr<Operation4> operation = new Operation4(timestamp, nmea);
    RemoteListenerHelper::Foreach(operation.Get());
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos