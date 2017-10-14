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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CAMERASERVICEBINDERDECORATOR_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CAMERASERVICEBINDERDECORATOR_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class CameraServiceBinderDecorator
{
private:
    class CameraServiceBinderDecoratorListener
        : public Object
        , public ICameraBinderDecoratorListener
        , public IDecoratorListener
    {
    public:
        CAR_INTERFACE_DECL()

        //@Override
        CARAPI OnCatchException(
            /* [in] */ IMethodInfo* m,
            /* [in] */ ArrayOf<IInterface*>* args,
            /* [in] */ ECode t,
            /* [out] */ Boolean* result);
    };

public:
    /**
     * <p>
     * Wraps the type T with a proxy that will check 'status_t' return codes
     * from the native side of the camera service, and throw Java exceptions
     * automatically based on the code.
     * </p>
     *
     * @param obj object that will serve as the target for all method calls
     * @param <T> the type of the element you want to wrap. This must be an interface.
     * @return a proxy that will intercept all invocations to obj
     */
    static CARAPI NewInstance(
        /* [in] */ IInterface* obj,
        /* [out] */ IInterface** result);

private:
    static const String TAG;
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CAMERASERVICEBINDERDECORATOR_H__
