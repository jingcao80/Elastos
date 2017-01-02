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

#include "elastos/droid/os/CBatteryManager.h"
#include "elastos/droid/os/CBatteryProperty.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::CBatteryProperty;
using Elastos::Droid::Os::IBatteryProperty;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CBatteryManager, Object, IBatteryManager)

CAR_OBJECT_IMPL(CBatteryManager)

CBatteryManager::CBatteryManager()
{}

ECode CBatteryManager::constructor()
{
    return NOERROR;
}

Int64 CBatteryManager::QueryProperty(
    /* [in] */ Int32 id)
{
    Int64 ret;

    using Elastos::Core::Math;

    if (mBatteryPropertiesRegistrar == NULL) {
        AutoPtr<IInterface> b = ServiceManager::GetService(String("batteryproperties"));
        mBatteryPropertiesRegistrar = IIBatteryPropertiesRegistrar::Probe(b);

        if (mBatteryPropertiesRegistrar == NULL)
            return Math::INT64_MIN_VALUE;
    }

    // try {
        AutoPtr<IBatteryProperty> prop;
        CBatteryProperty::New((IBatteryProperty**)&prop);

        Int32 data;
        ECode ec = mBatteryPropertiesRegistrar->GetProperty(id, prop, &data);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            ret = Math::INT64_MIN_VALUE;
        }
        else {
            if (data == 0) {
                prop->GetInt64(&ret);
            }
            else {
                ret = Math::INT64_MIN_VALUE;
            }
        }
    // } catch (RemoteException e) {
    //     ret = Math::INT64_MIN_VALUE;
    // }

    return ret;
}

ECode CBatteryManager::GetInt32Property(
    /* [in] */ Int32 id,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (Int32)QueryProperty(id);
    return NOERROR;
}

ECode CBatteryManager::GetInt64Property(
    /* [in] */ Int32 id,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = QueryProperty(id);
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
