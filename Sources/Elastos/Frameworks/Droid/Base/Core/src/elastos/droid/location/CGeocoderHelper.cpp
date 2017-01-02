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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/location/CGeocoderHelper.h"
#include "elastos/droid/os/CServiceManager.h"

using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL(CGeocoderHelper, Singleton, IGeocoderHelper)

CAR_SINGLETON_IMPL(CGeocoderHelper)

ECode CGeocoderHelper::IsPresent(
    /* [out] */ Boolean* isPresent)
{
    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    AutoPtr<IInterface> obj;
    serviceManager->GetService(IContext::LOCATION_SERVICE, (IInterface**)&obj);
    IILocationManager* lm = IILocationManager::Probe(obj);
    return lm->GeocoderIsPresent(isPresent);
}

} // namespace Location
} // namespace Droid
} // namespace Elastos