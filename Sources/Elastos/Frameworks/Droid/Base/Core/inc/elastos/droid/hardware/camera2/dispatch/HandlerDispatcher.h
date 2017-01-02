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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_HANDLERDISPATCHER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_HANDLERDISPATCHER_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

class HandlerDispatcher
    : public Object
    , public IHandlerDispatcher
    , public IDispatchable
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ IDispatchable* target,
            /* [in] */ IMethodInfo* method,
            /* [in] */ IArgumentList* args);

        CARAPI Run();

    private:
        AutoPtr<IDispatchable> mDispatchTarget;
        AutoPtr<IMethodInfo> mMethod;
        AutoPtr<IArgumentList> mArgs;
    };

public:
    CAR_INTERFACE_DECL()

    HandlerDispatcher();

    virtual ~HandlerDispatcher() {}

    /**
     * Create a dispatcher that forwards it's dispatch calls by posting
     * them onto the {@code handler} as a {@code Runnable}.
     *
     * @param dispatchTarget the destination whose method calls will be redirected into the handler
     * @param handler all calls into {@code dispatchTarget} will be posted onto this handler
     * @param <T> the type of the element you want to wrap.
     * @return a dispatcher that will forward it's dispatch calls to a handler
     */
    CARAPI constructor(
        /* [in] */ IDispatchable* dispatchTarget,
        /* [in] */ IHandler* handler);

    //@Override
    CARAPI Dispatch(
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args);

private:
    static const String TAG;

    AutoPtr<IDispatchable> mDispatchTarget;
    AutoPtr<IHandler> mHandler;
};

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_HANDLERDISPATCHER_H__
