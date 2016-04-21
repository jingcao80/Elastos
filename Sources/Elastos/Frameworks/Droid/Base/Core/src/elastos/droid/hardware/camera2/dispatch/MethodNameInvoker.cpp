
#include "elastos/droid/hardware/camera2/dispatch/MethodNameInvoker.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Core::ICharSequence;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::IMap;
using Elastos::Utility::Concurrent::IConcurrentHashMap;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

CAR_INTERFACE_IMPL(MethodNameInvoker, Object, IMethodNameInvoker)

MethodNameInvoker::MethodNameInvoker()
{
    CConcurrentHashMap::New((IConcurrentHashMap**)&mMethods);
}

ECode MethodNameInvoker::constructor()
{
    return NOERROR;
}

ECode MethodNameInvoker::constructor(
    /* [in] */ IDispatchable* target,
    /* [in] */ IClassInfo* targetClass)
{
    mTargetClass = targetClass;
    mTarget = target;
    return NOERROR;
}

ECode MethodNameInvoker::Invoke(
    /* [in] */ const String& methodName,
    /* [in] */ ArrayOf<IInterface*>* params)
{
    FAIL_RETURN(Preconditions::CheckNotNull(methodName))

    AutoPtr<ICharSequence> charObj = CoreUtils::Convert(methodName);
    AutoPtr<IInterface> obj;
    mMethods->Get(TO_IINTERFACE(charObj), (IInterface**)&obj);
    AutoPtr<IMethodInfo> targetMethod = IMethodInfo::Probe(obj);
    if (targetMethod == NULL) {
        AutoPtr<ArrayOf<IMethodInfo*> > methodInfos;
        mTargetClass->GetAllMethodInfos(methodInfos);
        for (Int32 i = 0; i < methodInfos->GetLength(); i++) {
            AutoPtr<IMethodInfo> method = (*methodInfos)[i];

            // TODO future: match types of params if possible
            String name;
            method->GetName(&name);
            AutoPtr<ArrayOf<IParamInfo*> > paramInfo;
            method->GetAllParamInfos(paramInfo);
            if (name.Equals(methodName) &&
                    (params->GetLength() == paramInfo->GetLength()) ) {
                targetMethod = method;
                mMethods->Put(TO_IINTERFACE(charObj), targetMethod);
                break;
            }
        }

        if (targetMethod == NULL) {
            // throw new IllegalArgumentException(
            //         "Method " + methodName + " does not exist on class " + mTargetClass);
            String name;
            mTargetClass->GetName(&name);
            Slogger::E("MethodNameInvoker", "Method %s does not exist on class %s",
                    methodName.string(), name.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    // try {
    return mTarget->Dispatch(targetMethod, params);
    // } catch (Throwable e) {
    //     UncheckedThrow.throwAnyException(e);
    //     // unreachable
    //     return null;
    // }
}

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
