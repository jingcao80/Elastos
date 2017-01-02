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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_BROADCASTDISPATCHER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_BROADCASTDISPATCHER_H__

#include "Elastos.Droid.Hardware.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Camera2::Dispatch::IDispatchable;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

class BroadcastDispatcher
    : public Object
    , public IBroadcastDispatcher
    , public IDispatchable
{
public:
    CAR_INTERFACE_DECL()

    BroadcastDispatcher();

    virtual ~BroadcastDispatcher() {}

    /**
     * Create a broadcast dispatcher from the supplied dispatch targets.
     *
     * @param dispatchTargets one or more targets to dispatch to
     */
    //@SafeVarargs
    CARAPI constructor(
        /* [in] */ ArrayOf<IDispatchable*>* dispatchTargets);

    //@Override
    CARAPI Dispatch(
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args);

private:
    AutoPtr< ArrayOf<IDispatchable*> > mDispatchTargets;
};

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_BROADCASTDISPATCHER_H__
