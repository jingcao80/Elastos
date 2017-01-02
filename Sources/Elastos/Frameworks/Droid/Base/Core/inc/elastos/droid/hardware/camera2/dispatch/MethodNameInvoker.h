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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_METHODNAMEINVOKER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_METHODNAMEINVOKER_H__

#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::Concurrent::IConcurrentHashMap;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

class MethodNameInvoker
    : public Object
    , public IMethodNameInvoker
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Create a dispatcher that does nothing when dispatched to.
     */
    MethodNameInvoker();

    virtual ~MethodNameInvoker() {}

    /**
     * Create a new method name invoker.
     *
     * @param target destination dispatch type, invokes will be redirected to this dispatcher
     * @param targetClass destination dispatch class, the invoked methods will be from this class
     */
    CARAPI constructor(
        /* [in] */ IDispatchable* target,
        /* [in] */ IInterfaceInfo* targetClass);

    CARAPI GetMethodInfo(
        /* [in] */ const String& name,
        /* [in] */ const String& signature,
        /* [out] */ IMethodInfo** methodInfo);

    /**
     * Invoke a method by its name.
     *
     * <p>If more than one method exists in {@code targetClass}, the first method with the right
     * number of arguments will be used, and later calls will all use that method.</p>
     *
     * @param methodName
     *          The name of the method, which will be matched 1:1 to the destination method
     * @param params
     *          Variadic parameter list.
     * @return
     *          The same kind of value that would normally be returned by calling {@code methodName}
     *          statically.
     *
     * @throws IllegalArgumentException if {@code methodName} does not exist on the target class
     * @throws Throwable will rethrow anything that the target method would normally throw
     */
    //@SuppressWarnings("unchecked")
    CARAPI Invoke(
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* params);

private:
    AutoPtr<IDispatchable> mTarget;
    AutoPtr<IInterfaceInfo> mTargetClass;
    AutoPtr<IConcurrentHashMap> mMethods;
};

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_METHODNAMEINVOKER_H__
