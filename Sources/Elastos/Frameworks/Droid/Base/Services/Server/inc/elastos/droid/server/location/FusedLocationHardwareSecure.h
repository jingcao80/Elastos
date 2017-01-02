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

#ifndef __ELASTOS_DROID_SERVER_LOCATION_FUSEDLOCATIONHARDWARESECURE_H__
#define __ELASTOS_DROID_SERVER_LOCATION_FUSEDLOCATIONHARDWARESECURE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Location::IIFusedLocationHardware;
using Elastos::Droid::Hardware::Location::IIFusedLocationHardwareSink;
using Elastos::Droid::Location::IFusedBatchOptions;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * FusedLocationHardware decorator that adds permission checking.
 * @hide
 */
class FusedLocationHardwareSecure
    : public Object
    , public IIFusedLocationHardware
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    FusedLocationHardwareSecure();

    CARAPI constructor(
        /* [in] */ IIFusedLocationHardware* locationHardware,
        /* [in] */ IContext* context,
        /* [in] */ const String& permissionId);

    // @Override
    CARAPI RegisterSink(
        /* [in] */ IIFusedLocationHardwareSink* eventSink);

    // @Override
    CARAPI UnregisterSink(
        /* [in] */ IIFusedLocationHardwareSink* eventSink);

    // @Override
    CARAPI GetSupportedBatchSize(
        /* [out] */ Int32* size);

    // @Override
    CARAPI StartBatching(
        /* [in] */ Int32 id,
        /* [in] */ IFusedBatchOptions* batchOptions);

    // @Override
    CARAPI StopBatching(
        /* [in] */ Int32 id);

    // @Override
    CARAPI UpdateBatchingOptions(
        /* [in] */ Int32 id,
        /* [in] */ IFusedBatchOptions* batchoOptions);

    // @Override
    CARAPI RequestBatchOfLocations(
        /* [in] */ Int32 batchSizeRequested);

    // @Override
    CARAPI SupportsDiagnosticDataInjection(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI InjectDiagnosticData(
        /* [in] */ const String& data);

    // @Override
    CARAPI SupportsDeviceContextInjection(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI InjectDeviceContext(
        /* [in] */ Int32 deviceEnabledContext);


    CARAPI ToString(
        /* [out] */ String* str)
    {
        return Object::ToString(str);
    }

private:
    CARAPI_(void) CheckPermissions();

private:
    AutoPtr<IIFusedLocationHardware> mLocationHardware;
    AutoPtr<IContext> mContext;
    String mPermissionId;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_FUSEDLOCATIONHARDWARESECURE_H__

