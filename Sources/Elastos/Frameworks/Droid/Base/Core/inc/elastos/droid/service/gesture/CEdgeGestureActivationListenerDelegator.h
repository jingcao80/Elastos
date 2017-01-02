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

#ifndef __ELASTOS_DROID_SERVICE_GESTURE_CEDGE_GESTURE_ACTIVATION_LISTENER_DELEGATOR_H__
#define __ELASTOS_DROID_SERVICE_GESTURE_CEDGE_GESTURE_ACTIVATION_LISTENER_DELEGATOR_H__

#include "_Elastos_Droid_Service_Gesture_CEdgeGestureActivationListenerDelegator.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Gesture {

 CarClass(CEdgeGestureActivationListenerDelegator)
    , public Object
    , public IIEdgeGestureActivationListener
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IEdgeGestureActivationListener* weakHost);

    CARAPI OnEdgeGestureActivation(
        /* [in] */ Int32 touchX,
        /* [in] */ Int32 touchY,
        /* [in] */ Int32 positionIndex,
        /* [in] */ Int32 flags);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<IWeakReference> mWeakHost;
};

} // namespace Gesture
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_GESTURE_CEDGE_GESTURE_ACTIVATION_LISTENER_DELEGATOR_H__
