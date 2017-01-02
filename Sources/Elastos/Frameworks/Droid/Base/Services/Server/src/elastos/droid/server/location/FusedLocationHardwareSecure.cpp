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

#include "elastos/droid/server/location/FusedLocationHardwareSecure.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Hardware::Location::EIID_IIFusedLocationHardware;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

CAR_INTERFACE_IMPL_2(FusedLocationHardwareSecure, Object, IIFusedLocationHardware, IBinder)

FusedLocationHardwareSecure::FusedLocationHardwareSecure()
{
}

ECode FusedLocationHardwareSecure::constructor(
    /* [in] */ IIFusedLocationHardware* locationHardware,
    /* [in] */ IContext* context,
    /* [in] */ const String& permissionId)
{
    mLocationHardware = locationHardware;
    mContext = context;
    mPermissionId = permissionId;
    return NOERROR;
}

void FusedLocationHardwareSecure::CheckPermissions()
{
    StringBuilder message("Permission '");
    message += mPermissionId;
    message += "' not granted to access FusedLocationHardware";
    mContext->EnforceCallingPermission(
            mPermissionId,
            message.ToString());
}

ECode FusedLocationHardwareSecure::RegisterSink(
    /* [in] */ IIFusedLocationHardwareSink* eventSink)
{
    CheckPermissions();
    ECode ec = mLocationHardware->RegisterSink(eventSink);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode FusedLocationHardwareSecure::UnregisterSink(
    /* [in] */ IIFusedLocationHardwareSink* eventSink)
{
    CheckPermissions();
    ECode ec = mLocationHardware->UnregisterSink(eventSink);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode FusedLocationHardwareSecure::GetSupportedBatchSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    CheckPermissions();
    ECode ec = mLocationHardware->GetSupportedBatchSize(size);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode FusedLocationHardwareSecure::StartBatching(
    /* [in] */ Int32 id,
    /* [in] */ IFusedBatchOptions* batchOptions)
{
    CheckPermissions();
    ECode ec = mLocationHardware->StartBatching(id, batchOptions);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode FusedLocationHardwareSecure::StopBatching(
    /* [in] */ Int32 id)
{
    CheckPermissions();
    ECode ec = mLocationHardware->StopBatching(id);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode FusedLocationHardwareSecure::UpdateBatchingOptions(
    /* [in] */ Int32 id,
    /* [in] */ IFusedBatchOptions* batchOptions)
{
    CheckPermissions();
    ECode ec = mLocationHardware->UpdateBatchingOptions(id, batchOptions);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode FusedLocationHardwareSecure::RequestBatchOfLocations(
    /* [in] */ Int32 batchSizeRequested)
{
    CheckPermissions();
    ECode ec = mLocationHardware->RequestBatchOfLocations(batchSizeRequested);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode FusedLocationHardwareSecure::SupportsDiagnosticDataInjection(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    CheckPermissions();
    ECode ec = mLocationHardware->SupportsDiagnosticDataInjection(result);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode FusedLocationHardwareSecure::InjectDiagnosticData(
    /* [in] */ const String& data)
{
    CheckPermissions();
    ECode ec = mLocationHardware->InjectDiagnosticData(data);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode FusedLocationHardwareSecure::SupportsDeviceContextInjection(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    CheckPermissions();
    ECode ec = mLocationHardware->SupportsDeviceContextInjection(result);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode FusedLocationHardwareSecure::InjectDeviceContext(
    /* [in] */ Int32 deviceEnabledContext)
{
    CheckPermissions();
    ECode ec = mLocationHardware->InjectDeviceContext(deviceEnabledContext);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos