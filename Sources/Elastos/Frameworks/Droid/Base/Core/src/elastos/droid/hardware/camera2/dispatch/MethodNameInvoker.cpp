
#include "elastos/droid/hardware/camera2/dispatch/MethodNameInvoker.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Core::ICharSequence;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::IMap;
using Elastos::Utility::Concurrent::IConcurrentHashMap;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

const String TAG("MethodNameInvoker");

CAR_INTERFACE_IMPL(MethodNameInvoker, Object, IMethodNameInvoker)

MethodNameInvoker::MethodNameInvoker()
{
    CConcurrentHashMap::New((IConcurrentHashMap**)&mMethods);
}

ECode MethodNameInvoker::constructor(
    /* [in] */ IDispatchable* target,
    /* [in] */ IInterfaceInfo* targetClass)
{
    assert(targetClass != NULL);
    mTargetClass = targetClass;
    mTarget = target;
    return NOERROR;
}

ECode MethodNameInvoker::GetMethodInfo(
    /* [in] */ const String& methodName,
    /* [in] */ const String& signature,
    /* [out] */ IMethodInfo** methodInfo)
{
    VALIDATE_NOT_NULL(methodInfo)
    *methodInfo = NULL;

    AutoPtr<ICharSequence> csq = CoreUtils::Convert(methodName);
    AutoPtr<IInterface> obj;
    mMethods->Get(csq.Get(), (IInterface**)&obj);
    AutoPtr<IMethodInfo> targetMethod = IMethodInfo::Probe(obj);
    if (targetMethod == NULL) {
        mTargetClass->GetMethodInfo(methodName, signature, (IMethodInfo**)&targetMethod);

        if (targetMethod == NULL) {
            String name, ns;
            mTargetClass->GetName(&name);
            mTargetClass->GetNamespace(&ns);
            Logger::E(TAG, "Method [%s, %s] does not exist on class %s.%s",
                methodName.string(), signature.string(), ns.string(), name.string());
            assert(0);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    *methodInfo = targetMethod;
    REFCOUNT_ADD(*methodInfo)
    return NOERROR;
}

ECode MethodNameInvoker::Invoke(
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* params)
{
    return mTarget->Dispatch(method, params);
}


} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
