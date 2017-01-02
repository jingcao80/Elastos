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

#ifndef __ELASTOS_DROID_SERVER_LOCATION_GPSSTATUSLISTENERHELPER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GPSSTATUSLISTENERHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Location.h"
#include "elastos/droid/server/location/RemoteListenerHelper.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Server::Location::IListenerOperation;
using Elastos::Droid::Server::Location::RemoteListenerHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * Implementation of a handler for {@link IGpsStatusListener}.
 */
class GpsStatusListenerHelper
    : public RemoteListenerHelper
{
private:
    class Operation1
        : public Object
        , public IListenerOperation
    {
    public:
        CAR_INTERFACE_DECL()

        Operation1(
            /* [in] */ Int32 timeToFirstFix);

        //@Overide
        CARAPI Execute(
            /* [in] */ IInterface* listener);

    private:
        const Int32 mTimeToFirstFix;
    };

    class Operation2
        : public Object
        , public IListenerOperation
    {
    public:
        CAR_INTERFACE_DECL()

        Operation2(
            /* [in] */ Boolean isNavigating);

        //@Overide
        CARAPI Execute(
            /* [in] */ IInterface* listener);

    private:
        const Boolean mIsNavigating;
    };

    class Operation3
        : public Object
        , public IListenerOperation
    {
    public:
        CAR_INTERFACE_DECL()

        Operation3(
        /* [in] */ Int32 svCount,
        /* [in] */ ArrayOf<Int32>* prns,
        /* [in] */ ArrayOf<Float>* snrs,
        /* [in] */ ArrayOf<Float>* elevations,
        /* [in] */ ArrayOf<Float>* azimuths,
        /* [in] */ Int32 ephemerisMask,
        /* [in] */ Int32 almanacMask,
        /* [in] */ Int32 usedInFixMask);

        //@Overide
        CARAPI Execute(
            /* [in] */ IInterface* listener);

    private:
        const Int32 mSvCount;
        const AutoPtr<ArrayOf<Int32> > mPrns;
        const AutoPtr<ArrayOf<Float> > mSnrs;
        const AutoPtr<ArrayOf<Float> > mElevations;
        const AutoPtr<ArrayOf<Float> > mAzimuths;
        const Int32 mEphemerisMask;
        const Int32 mAlmanacMask;
        const Int32 mUsedInFixMask;
    };

    class Operation4
        : public Object
        , public IListenerOperation
    {
    public:
        CAR_INTERFACE_DECL()

        Operation4(
            /* [in] */ Int64 timestamp,
            /* [in] */ const String& nmea);

        //@Overide
        CARAPI Execute(
            /* [in] */ IInterface* listener);

    private:
        Int64 mTimestamp;
        const String mNmea;
    };

public:
    GpsStatusListenerHelper();

    CARAPI OnFirstFix(
        /* [in] */ Int32 timeToFirstFix);

    CARAPI OnStatusChanged(
        /* [in] */ Boolean isNavigating);

    CARAPI OnSvStatusChanged(
        /* [in] */ Int32 svCount,
        /* [in] */ ArrayOf<Int32>* prns,
        /* [in] */ ArrayOf<Float>* snrs,
        /* [in] */ ArrayOf<Float>* elevations,
        /* [in] */ ArrayOf<Float>* azimuths,
        /* [in] */ Int32 ephemerisMask,
        /* [in] */ Int32 almanacMask,
        /* [in] */ Int32 usedInFixMask);

    CARAPI OnNmeaReceived(
        /* [in] */ Int64 timestamp,
        /* [in] */ const String& nmea);
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GPSSTATUSLISTENERHELPER_H__