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

#include "elastos/droid/hardware/camera2/utils/CameraServiceBinderDecorator.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL_2(CameraServiceBinderDecorator::CameraServiceBinderDecoratorListener, Object,
        ICameraBinderDecoratorListener, IDecoratorListener)

const String CameraServiceBinderDecorator::TAG("CameraServiceBinderDecorator");

ECode CameraServiceBinderDecorator::CameraServiceBinderDecoratorListener::OnCatchException(
    /* [in] */ IMethodInfo* m,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [in] */ ECode t,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (t == (ECode)E_DEAD_OBJECT_EXCEPTION) {
        // Can sometimes happen (camera service died)
        // Pass on silently
    }
    else if (t == (ECode)E_REMOTE_EXCEPTION) {
        // Some other kind of remote exception - this is not normal, so let's at least
        // note it before moving on
        //Log.e(TAG, "Unexpected RemoteException from camera service call.", t);
        Logger::E("CameraServiceBinderDecorator", "Unexpected RemoteException from camera service call. %d", t);
    }

    // All other exceptions also get sent onward
    *result = FALSE;
    return NOERROR;
}

ECode CameraServiceBinderDecorator::NewInstance(
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    assert(0 && "TODO");
    // Decorator.<T> newInstance(obj, new CameraServiceBinderDecoratorListener());
    // REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
